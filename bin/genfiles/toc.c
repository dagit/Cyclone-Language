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
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;
# 177
void Cyc_Core_ufree(void*ptr);
# 190
struct _dyneither_ptr Cyc_Core_alias_refptr(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 237
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_rckey();
# 245
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);
# 251
void*Cyc_Core_open_region(struct Cyc_Core_DynamicRegion*key,void*arg,void*(*body)(struct _RegionHandle*h,void*arg));
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
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 40
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_PP_Ppstate;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 692 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 910
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 917
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 927
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 930
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 937
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 940
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 955
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 957
extern void*Cyc_Absyn_sint_typ;
# 969
void*Cyc_Absyn_exn_typ();
# 980
extern void*Cyc_Absyn_bounds_one;
# 988
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 994
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 996
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 999
void*Cyc_Absyn_void_star_typ();
# 1001
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1007
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1076
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1095
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1099
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1101
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1106
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1110
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1115
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1158
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1161
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1171
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1175
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1185
struct _tuple1*Cyc_Absyn_binding2qvar(void*);
# 1190
extern int Cyc_Absyn_no_regions;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 36 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 47
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 63
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 67
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 78
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 112 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
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
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 95
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 145
int Cyc_Tcutil_unify(void*,void*);
# 150
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 237 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 243
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 245
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 247
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 257
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 315
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);
# 344
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 348
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 351
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 40
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 75
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 86
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
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
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 119
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 126
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 142 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 161
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 233 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 31 "toc.h"
typedef struct Cyc_Hashtable_Table*Cyc_Toc_table_t;
# 34
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);
# 38
struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 41
extern int Cyc_Toc_warn_bounds_checks;
extern int Cyc_Toc_warn_all_null_deref;struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 49 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};
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
# 59 "toc.cyc"
extern int Cyc_noexpand_r;
# 70 "toc.cyc"
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 76
struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}
# 86
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 106
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp0=*x;struct _tuple14 _tmp1=_tmp0;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL1: _tmp7=_tmp1.f1;_tmp6=_tmp1.f2;_LL2:;{
struct _tuple14 _tmp2=*y;struct _tuple14 _tmp3=_tmp2;struct _tuple1*_tmp5;struct _tuple1*_tmp4;_LL4: _tmp5=_tmp3.f1;_tmp4=_tmp3.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp7,_tmp5);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp6,_tmp4);};};}
# 115
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d,int unused){
return({struct Cyc_Toc_TocState*_tmp8=_region_malloc(d,sizeof(*_tmp8));_tmp8->tuple_types=({struct Cyc_List_List**_tmpE=_region_malloc(d,sizeof(*_tmpE));_tmpE[0]=0;_tmpE;});_tmp8->aggrs_so_far=({struct Cyc_Dict_Dict*_tmpD=_region_malloc(d,sizeof(*_tmpD));_tmpD[0]=
# 118
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpD;});_tmp8->abs_struct_types=({struct Cyc_List_List**_tmpC=_region_malloc(d,sizeof(*_tmpC));_tmpC[0]=0;_tmpC;});_tmp8->datatypes_so_far=({struct Cyc_Set_Set**_tmpB=_region_malloc(d,sizeof(*_tmpB));_tmpB[0]=
# 120
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpB;});_tmp8->xdatatypes_so_far=({struct Cyc_Dict_Dict*_tmpA=_region_malloc(d,sizeof(*_tmpA));_tmpA[0]=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpA;});_tmp8->qvar_tags=({struct Cyc_Dict_Dict*_tmp9=_region_malloc(d,sizeof(*_tmp9));_tmp9[0]=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmp9;});_tmp8->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp8;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 132
typedef struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state_t;
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 139
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 142
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmpF=ts;struct Cyc_Toc_TocStateWrap*_tmp10=Cyc_Toc_toc_state;ts=_tmp10;Cyc_Toc_toc_state=_tmpF;});{
struct Cyc_Toc_TocStateWrap _tmp11=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp12=_tmp11;struct Cyc_Core_DynamicRegion*_tmp1B;struct Cyc_Toc_TocState*_tmp1A;_LL7: _tmp1B=_tmp12.dyn;_tmp1A=_tmp12.state;_LL8:;{
struct _dyneither_ptr _tmp13=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1B,sizeof(struct Cyc_Core_DynamicRegion),1));
*ts=({struct Cyc_Toc_TocStateWrap _tmp14;_tmp14.dyn=(struct Cyc_Core_DynamicRegion*)_tmp1B;_tmp14.state=(struct Cyc_Toc_TocState*)_tmp1A;_tmp14;});
({struct Cyc_Toc_TocStateWrap*_tmp15=ts;struct Cyc_Toc_TocStateWrap*_tmp16=Cyc_Toc_toc_state;ts=_tmp16;Cyc_Toc_toc_state=_tmp15;});{
struct _tuple15 _tmp17=({struct _tuple15 _tmp19;_tmp19.f1=_tmp1A;_tmp19.f2=arg;_tmp19;});
void*_tmp18=((void*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple15*arg,void*(*body)(struct _RegionHandle*h,struct _tuple15*arg)))Cyc_Core_open_region)((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1)),& _tmp17,f);
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1)));
return _tmp18;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 154
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 157
struct _tuple17 _tmp1C=*env;struct _tuple17 _tmp1D=_tmp1C;struct Cyc_Toc_TocState*_tmp23;struct _tuple1*_tmp22;void*(*_tmp21)(struct _tuple1*);_LLA: _tmp23=_tmp1D.f1;_tmp22=(_tmp1D.f2)->f1;_tmp21=(_tmp1D.f2)->f2;_LLB:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp23->aggrs_so_far,_tmp22);
if(v == 0)
return _tmp21(_tmp22);else{
# 162
struct _tuple18*_tmp1E=*v;struct _tuple18*_tmp1F=_tmp1E;void*_tmp20;_LLD: _tmp20=_tmp1F->f2;_LLE:;
return _tmp20;}};}
# 167
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp24;_tmp24.f1=q;_tmp24.f2=type_maker;_tmp24;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 172
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 179
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 182
Cyc_vfprintf(Cyc_stderr,fmt,ap);
({void*_tmp25=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp26="\n";_tag_dyneither(_tmp26,sizeof(char),2);}),_tag_dyneither(_tmp25,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp27=_cycalloc_atomic(sizeof(*_tmp27));_tmp27[0]=({struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp28;_tmp28.tag=Cyc_Toc_Toc_Unimplemented;_tmp28;});_tmp27;}));}
# 187
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 190
Cyc_vfprintf(Cyc_stderr,fmt,ap);
({void*_tmp29=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A="\n";_tag_dyneither(_tmp2A,sizeof(char),2);}),_tag_dyneither(_tmp29,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp2B=_cycalloc_atomic(sizeof(*_tmp2B));_tmp2B[0]=({struct Cyc_Toc_Toc_Impossible_exn_struct _tmp2C;_tmp2C.tag=Cyc_Toc_Toc_Impossible;_tmp2C;});_tmp2B;}));}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp2D[5]="curr";
# 205 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp2D,_tmp2D,_tmp2D + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp2E[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp2E,_tmp2E,_tmp2E + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp2F[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp2F,_tmp2F,_tmp2F + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp30[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp30,_tmp30,_tmp30 + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp31[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp31,_tmp31,_tmp31 + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp32[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp32,_tmp32,_tmp32 + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp33[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp33,_tmp33,_tmp33 + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp34[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp34,_tmp34,_tmp34 + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 218
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp35[7]="_throw";
# 227 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp35,_tmp35,_tmp35 + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp38[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp38,_tmp38,_tmp38 + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp3B[14]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp3B,_tmp3B,_tmp3B + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp3E[13]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp3E,_tmp3E,_tmp3E + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp41[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp41,_tmp41,_tmp41 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp44[14]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp44,_tmp44,_tmp44 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp47[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp47,_tmp47,_tmp47 + 12};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp4A[28]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp4A,_tmp4A,_tmp4A + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp4D[31]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp4D,_tmp4D,_tmp4D + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp50[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp50,_tmp50,_tmp50 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp53[15]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp53,_tmp53,_tmp53 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp56[15]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp56,_tmp56,_tmp56 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp59[20]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp59,_tmp59,_tmp59 + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__init_dyneither_ptr_bnd={0,& Cyc_Toc__init_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,(void*)& Cyc_Toc__init_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp5C[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp5C,_tmp5C,_tmp5C + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp5F[20]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp5F,_tmp5F,_tmp5F + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp62[19]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp62,_tmp62,_tmp62 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp65[24]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp65,_tmp65,_tmp65 + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp68[25]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp68,_tmp68,_tmp68 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp6B[23]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp6B,_tmp6B,_tmp6B + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp6E[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp6E,_tmp6E,_tmp6E + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp71[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp71,_tmp71,_tmp71 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp74[30]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp74,_tmp74,_tmp74 + 30};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp77[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp77,_tmp77,_tmp77 + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp7A[20]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp7A,_tmp7A,_tmp7A + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp7D[15]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp7D,_tmp7D,_tmp7D + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp80[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp80,_tmp80,_tmp80 + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp83[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp83,_tmp83,_tmp83 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp86[19]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp86,_tmp86,_tmp86 + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp89[21]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp89,_tmp89,_tmp89 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp8C[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp8C,_tmp8C,_tmp8C + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp8F[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp8F,_tmp8F,_tmp8F + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp92[24]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp92,_tmp92,_tmp92 + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp95[28]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp95,_tmp95,_tmp95 + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp98[23]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp98,_tmp98,_tmp98 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp9B[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp9B,_tmp9B,_tmp9B + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp9E[29]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp9E,_tmp9E,_tmp9E + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmpA1[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmpA1,_tmpA1,_tmpA1 + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmpA4[29]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmpA4,_tmpA4,_tmpA4 + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmpA7[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmpA7,_tmpA7,_tmpA7 + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmpAA[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmpAA,_tmpAA,_tmpAA + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmpAD[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmpAD,_tmpAD,_tmpAD + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmpB0[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmpB0,_tmpB0,_tmpB0 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmpB3[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmpB3,_tmpB3,_tmpB3 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmpB6[33]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmpB6,_tmpB6,_tmpB6 + 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmpB9[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmpB9,_tmpB9,_tmpB9 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmpBC[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmpBC,_tmpBC,_tmpBC + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmpBF[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmpBF,_tmpBF,_tmpBF + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmpC2[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmpC2,_tmpC2,_tmpC2 + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmpC5[39]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmpC5,_tmpC5,_tmpC5 + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmpC8[37]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmpC8,_tmpC8,_tmpC8 + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmpCB[10]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmpCB,_tmpCB,_tmpCB + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmpCE[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmpCE,_tmpCE,_tmpCE + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmpD1[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmpD1,_tmpD1,_tmpD1 + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmpD4[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmpD4,_tmpD4,_tmpD4 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmpD7[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmpD7,_tmpD7,_tmpD7 + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmpDA[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmpDA,_tmpDA,_tmpDA + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmpDD[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmpDD,_tmpDD,_tmpDD + 13};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmpE0[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmpE0,_tmpE0,_tmpE0 + 12};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmpE3[13]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmpE3,_tmpE3,_tmpE3 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmpE6[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmpE6,_tmpE6,_tmpE6 + 12};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmpE9[16]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmpE9,_tmpE9,_tmpE9 + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__open_dynregion_bnd={0,& Cyc_Toc__open_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,(void*)& Cyc_Toc__open_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmpEC[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmpEC,_tmpEC,_tmpEC + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_dynregion_bnd={0,& Cyc_Toc__push_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,(void*)& Cyc_Toc__push_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmpEF[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmpEF,_tmpEF,_tmpEF + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_dynregion_bnd={0,& Cyc_Toc__pop_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,(void*)& Cyc_Toc__pop_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmpF2[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmpF2,_tmpF2,_tmpF2 + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__reset_region_bnd={0,& Cyc_Toc__reset_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,(void*)& Cyc_Toc__reset_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmpF5[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpF5,_tmpF5,_tmpF5 + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpF8[29]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpF8,_tmpF8,_tmpF8 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpFB[11]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpFB,_tmpFB,_tmpFB + 11};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__swap_word_bnd={0,& Cyc_Toc__swap_word_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,(void*)& Cyc_Toc__swap_word_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpFE[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpFE,_tmpFE,_tmpFE + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__swap_dyneither_bnd={0,& Cyc_Toc__swap_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,(void*)& Cyc_Toc__swap_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmp101[13]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmp101,_tmp101,_tmp101 + 13};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp104[9]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmp104,_tmp104,_tmp104 + 9};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp107[20]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmp107,_tmp107,_tmp107 + 20};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 302
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 305
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 307
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 309
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0);}
# 315
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp10B=e->r;void*_tmp10C=_tmp10B;struct Cyc_Absyn_Exp*_tmp10D;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10C)->tag == 13){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10C)->f4 == Cyc_Absyn_No_coercion){_LL10: _tmp10D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10C)->f2;_LL11:
 return Cyc_Toc_cast_it(t,_tmp10D);}else{goto _LL12;}}else{_LL12: _LL13:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}_LLF:;}
# 321
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp10F;_tmp10F.tag=13;_tmp10F.f1=t;_tmp10F.f2=e;_tmp10F.f3=0;_tmp10F.f4=Cyc_Absyn_No_coercion;_tmp10F;});_tmp10E;});}
# 324
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110[0]=({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp111;_tmp111.tag=19;_tmp111.f1=e;_tmp111;});_tmp110;});}
# 327
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp113;_tmp113.tag=22;_tmp113.f1=e1;_tmp113.f2=e2;_tmp113;});_tmp112;});}
# 330
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp115;_tmp115.tag=36;_tmp115.f1=s;_tmp115;});_tmp114;});}
# 333
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp117;_tmp117.tag=16;_tmp117.f1=t;_tmp117;});_tmp116;});}
# 336
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp119;_tmp119.tag=9;_tmp119.f1=e;_tmp119.f2=es;_tmp119.f3=0;_tmp119.f4=1;_tmp119;});_tmp118;});}
# 339
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp11B;_tmp11B.tag=1;_tmp11B.f1=e;_tmp11B;});_tmp11A;});}
# 342
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp11D;_tmp11D.tag=2;_tmp11D.f1=s1;_tmp11D.f2=s2;_tmp11D;});_tmp11C;});}
# 345
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp11F;_tmp11F.tag=5;_tmp11F.f1=e1;_tmp11F.f2=e2;_tmp11F.f3=e3;_tmp11F;});_tmp11E;});}
# 348
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp121;_tmp121.tag=20;_tmp121.f1=e;_tmp121.f2=n;_tmp121.f3=0;_tmp121.f4=0;_tmp121;});_tmp120;});}
# 351
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp123;_tmp123.tag=21;_tmp123.f1=e;_tmp123.f2=n;_tmp123.f3=0;_tmp123.f4=0;_tmp123;});_tmp122;});}
# 354
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 357
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp125;_tmp125.tag=35;_tmp125.f1=tdopt;_tmp125.f2=ds;_tmp125;});_tmp124;});}
# 359
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp127;_tmp127.tag=8;_tmp127.f1=v;_tmp127;});_tmp126;});}
# 362
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 366
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp129=e->r;void*_tmp12A=_tmp129;struct Cyc_Absyn_Exp*_tmp12B;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12A)->tag == 19){_LL15: _tmp12B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12A)->f1;_LL16:
 return Cyc_Absyn_aggrarrow_exp(_tmp12B,f,loc);}else{_LL17: _LL18:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL14:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 383
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 392
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 401
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 410
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 421
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp12C=Cyc_Tcutil_compress(t);void*_tmp12D=_tmp12C;enum Cyc_Absyn_Size_of _tmp137;switch(*((int*)_tmp12D)){case 6: _LL1A: _tmp137=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp12D)->f2;_LL1B:
# 425
{enum Cyc_Absyn_Size_of _tmp12E=_tmp137;switch(_tmp12E){case Cyc_Absyn_Char_sz: _LL27: _LL28:
 function=fS->fchar;goto _LL26;case Cyc_Absyn_Short_sz: _LL29: _LL2A:
 function=fS->fshort;goto _LL26;case Cyc_Absyn_Int_sz: _LL2B: _LL2C:
 function=fS->fint;goto _LL26;default: _LL2D: _LL2E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F[0]=({struct Cyc_Core_Impossible_exn_struct _tmp130;_tmp130.tag=Cyc_Core_Impossible;_tmp130.f1=({const char*_tmp131="impossible IntType (not char, short or int)";_tag_dyneither(_tmp131,sizeof(char),44);});_tmp130;});_tmp12F;}));}_LL26:;}
# 431
goto _LL19;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp12D)->f1){case 0: _LL1C: _LL1D:
# 433
 function=fS->ffloat;
goto _LL19;case 1: _LL1E: _LL1F:
# 436
 function=fS->fdouble;
goto _LL19;default: _LL20: _LL21:
# 439
 function=fS->flongdouble;
goto _LL19;}case 5: _LL22: _LL23:
# 442
 function=fS->fvoidstar;
goto _LL19;default: _LL24: _LL25:
# 445
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132[0]=({struct Cyc_Core_Impossible_exn_struct _tmp133;_tmp133.tag=Cyc_Core_Impossible;_tmp133.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp136;_tmp136.tag=0;_tmp136.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp134[1]={& _tmp136};Cyc_aprintf(({const char*_tmp135="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp135,sizeof(char),67);}),_tag_dyneither(_tmp134,sizeof(void*),1));});});_tmp133;});_tmp132;}));}_LL19:;}
# 447
return function;}
# 449
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 452
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp138=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp139=_tmp138;void*_tmp13D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->tag == 5){_LL30: _tmp13D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->f1).elt_typ;_LL31:
# 455
 return Cyc_Toc_getFunctionType(fS,_tmp13D);}else{_LL32: _LL33:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=({struct Cyc_Core_Impossible_exn_struct _tmp13B;_tmp13B.tag=Cyc_Core_Impossible;_tmp13B.f1=({const char*_tmp13C="impossible type (not pointer)";_tag_dyneither(_tmp13C,sizeof(char),30);});_tmp13B;});_tmp13A;}));}_LL2F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 463
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp13E=e->r;void*_tmp13F=_tmp13E;struct Cyc_List_List*_tmp14B;struct Cyc_List_List*_tmp14A;struct Cyc_List_List*_tmp149;struct Cyc_List_List*_tmp148;struct Cyc_List_List*_tmp147;struct Cyc_Absyn_Exp*_tmp146;long long _tmp145;int _tmp144;short _tmp143;struct _dyneither_ptr _tmp142;char _tmp141;switch(*((int*)_tmp13F)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13F)->f1).Null_c).tag){case 2: _LL35: _tmp141=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13F)->f1).Char_c).val).f2;_LL36:
 return _tmp141 == '\000';case 3: _LL37: _tmp142=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13F)->f1).Wchar_c).val;_LL38: {
# 467
unsigned long _tmp140=Cyc_strlen((struct _dyneither_ptr)_tmp142);
int i=0;
if(_tmp140 >= 2  && *((const char*)_check_dyneither_subscript(_tmp142,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp142,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp142,sizeof(char),1))== 'x'  && _tmp140 >= 3) && *((const char*)_check_dyneither_subscript(_tmp142,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp140;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp142,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 477
return 0;}}case 4: _LL39: _tmp143=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13F)->f1).Short_c).val).f2;_LL3A:
 return _tmp143 == 0;case 5: _LL3B: _tmp144=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13F)->f1).Int_c).val).f2;_LL3C:
 return _tmp144 == 0;case 6: _LL3D: _tmp145=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13F)->f1).LongLong_c).val).f2;_LL3E:
 return _tmp145 == 0;case 1: _LL3F: _LL40:
 return 1;default: goto _LL4D;}case 13: _LL41: _tmp146=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp13F)->f2;_LL42:
 return Cyc_Toc_is_zero(_tmp146);case 23: _LL43: _tmp147=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp13F)->f1;_LL44:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp147);case 25: _LL45: _tmp148=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp13F)->f1;_LL46:
 _tmp149=_tmp148;goto _LL48;case 28: _LL47: _tmp149=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp13F)->f3;_LL48:
 _tmp14A=_tmp149;goto _LL4A;case 24: _LL49: _tmp14A=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp13F)->f2;_LL4A:
 _tmp14B=_tmp14A;goto _LL4C;case 35: _LL4B: _tmp14B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp13F)->f2;_LL4C:
# 488
 for(0;_tmp14B != 0;_tmp14B=_tmp14B->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp14B->hd)).f2))return 0;}
return 1;default: _LL4D: _LL4E:
 return 0;}_LL34:;}
# 496
static int Cyc_Toc_is_nullable(void*t){
void*_tmp14C=Cyc_Tcutil_compress(t);void*_tmp14D=_tmp14C;union Cyc_Absyn_Constraint*_tmp150;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->tag == 5){_LL50: _tmp150=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->f1).ptr_atts).nullable;_LL51:
# 499
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp150);}else{_LL52: _LL53:
({void*_tmp14E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp14F="is_nullable";_tag_dyneither(_tmp14F,sizeof(char),12);}),_tag_dyneither(_tmp14E,sizeof(void*),0));});}_LL4F:;}
# 505
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
struct _tuple1*_tmp151=x;union Cyc_Absyn_Nmspace _tmp164;struct _dyneither_ptr _tmp163;_LL55: _tmp164=_tmp151->f1;_tmp163=*_tmp151->f2;_LL56:;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp163);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmp152=_tmp164;struct Cyc_List_List*_tmp155;struct Cyc_List_List*_tmp154;struct Cyc_List_List*_tmp153;switch((_tmp152.Abs_n).tag){case 4: _LL58: _LL59:
 nms=0;goto _LL57;case 1: _LL5A: _tmp153=(_tmp152.Rel_n).val;_LL5B:
 nms=_tmp153;goto _LL57;case 2: _LL5C: _tmp154=(_tmp152.Abs_n).val;_LL5D:
 nms=_tmp154;goto _LL57;default: _LL5E: _tmp155=(_tmp152.C_n).val;_LL5F:
 nms=_tmp155;goto _LL57;}_LL57:;}
# 518
{struct Cyc_List_List*_tmp156=nms;for(0;_tmp156 != 0;_tmp156=_tmp156->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp156->hd));}}{
struct _dyneither_ptr buf=({unsigned int _tmp161=len;char*_tmp162=_cyccalloc_atomic(sizeof(char),_tmp161);_tag_dyneither(_tmp162,sizeof(char),_tmp161);});
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp157=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr*_tmp158=_tmp157;struct _dyneither_ptr _tmp15C;_LL61: _tmp15C=*_tmp158;_LL62:;
({struct _dyneither_ptr _tmp159=p;char _tmp15A=*((char*)_check_dyneither_subscript(_tmp159,sizeof(char),0));char _tmp15B='_';if(_get_dyneither_size(_tmp159,sizeof(char))== 1  && (_tmp15A == '\000'  && _tmp15B != '\000'))_throw_arraybounds();*((char*)_tmp159.curr)=_tmp15B;});_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp15C);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp15C));}
# 529
({struct _dyneither_ptr _tmp15D=p;char _tmp15E=*((char*)_check_dyneither_subscript(_tmp15D,sizeof(char),0));char _tmp15F='_';if(_get_dyneither_size(_tmp15D,sizeof(char))== 1  && (_tmp15E == '\000'  && _tmp15F != '\000'))_throw_arraybounds();*((char*)_tmp15D.curr)=_tmp15F;});_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp163);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
Cyc_strcpy(p,({const char*_tmp160="_struct";_tag_dyneither(_tmp160,sizeof(char),8);}));
return(struct _dyneither_ptr)buf;};};};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};static char _tmp17A[8]="*bogus*";
# 541
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 544
static struct _dyneither_ptr bogus_string={_tmp17A,_tmp17A,_tmp17A + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 548
struct _tuple20 _tmp165=*env;struct _tuple20 _tmp166=_tmp165;struct Cyc_Dict_Dict*_tmp179;struct _tuple14*_tmp178;_LL64: _tmp179=(_tmp166.f1)->qvar_tags;_tmp178=_tmp166.f2;_LL65:;{
struct _tuple14 _tmp167=*_tmp178;struct _tuple14 _tmp168=_tmp167;struct _tuple1*_tmp177;struct _tuple1*_tmp176;_LL67: _tmp177=_tmp168.f1;_tmp176=_tmp168.f2;_LL68:;{
struct _handler_cons _tmp169;_push_handler(& _tmp169);{int _tmp16B=0;if(setjmp(_tmp169.handler))_tmp16B=1;if(!_tmp16B){
{struct _tuple1*_tmp16C=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp179,Cyc_Toc_qvar_tag_cmp,_tmp178);_npop_handler(0);return _tmp16C;};_pop_handler();}else{void*_tmp16A=(void*)_exn_thrown;void*_tmp16D=_tmp16A;void*_tmp175;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp16D)->tag == Cyc_Dict_Absent){_LL6A: _LL6B: {
# 554
struct _tuple14*_tmp16E=({struct _tuple14*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->f1=_tmp177;_tmp174->f2=_tmp176;_tmp174;});
struct _tuple1*_tmp16F=_tmp177;union Cyc_Absyn_Nmspace _tmp173;struct _dyneither_ptr _tmp172;_LL6F: _tmp173=_tmp16F->f1;_tmp172=*_tmp16F->f2;_LL70:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp172,_tmp176);
struct _tuple1*res=({struct _tuple1*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->f1=_tmp173;_tmp170->f2=({struct _dyneither_ptr*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171[0]=newvar;_tmp171;});_tmp170;});
*_tmp179=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp179,_tmp16E,res);
return res;};}}else{_LL6C: _tmp175=_tmp16D;_LL6D:(int)_rethrow(_tmp175);}_LL69:;}};};};}
# 563
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp17B;_tmp17B.f1=fieldname;_tmp17B.f2=dtname;_tmp17B;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 568
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 572
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 575
struct _tuple21 _tmp17C=*env;struct _tuple21 _tmp17D=_tmp17C;struct Cyc_List_List**_tmp195;struct Cyc_List_List*_tmp194;_LL72: _tmp195=(_tmp17D.f1)->tuple_types;_tmp194=_tmp17D.f2;_LL73:;
# 577
{struct Cyc_List_List*_tmp17E=*_tmp195;for(0;_tmp17E != 0;_tmp17E=_tmp17E->tl){
struct _tuple22*_tmp17F=(struct _tuple22*)_tmp17E->hd;struct _tuple22*_tmp180=_tmp17F;void*_tmp183;struct Cyc_List_List*_tmp182;_LL75: _tmp183=_tmp180->f1;_tmp182=_tmp180->f2;_LL76:;{
struct Cyc_List_List*_tmp181=_tmp194;
for(0;_tmp181 != 0  && _tmp182 != 0;(_tmp181=_tmp181->tl,_tmp182=_tmp182->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp181->hd)).f2,(void*)_tmp182->hd))
break;}
if(_tmp181 == 0  && _tmp182 == 0)
return _tmp183;};}}{
# 587
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp193;_tmp193.tag=1;_tmp193.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp191[1]={& _tmp193};Cyc_aprintf(({const char*_tmp192="_tuple%d";_tag_dyneither(_tmp192,sizeof(char),9);}),_tag_dyneither(_tmp191,sizeof(void*),1));});});_tmp190;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp194);
struct Cyc_List_List*_tmp184=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmp184=({struct Cyc_List_List*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->hd=({struct Cyc_Absyn_Aggrfield*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->name=Cyc_Absyn_fieldname(i);_tmp186->tq=Cyc_Toc_mt_tq;_tmp186->type=(void*)ts2->hd;_tmp186->width=0;_tmp186->attributes=0;_tmp186->requires_clause=0;_tmp186;});_tmp185->tl=_tmp184;_tmp185;});}}
_tmp184=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp184);{
struct Cyc_Absyn_Aggrdecl*_tmp187=({struct Cyc_Absyn_Aggrdecl*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->kind=Cyc_Absyn_StructA;_tmp18D->sc=Cyc_Absyn_Public;_tmp18D->name=({struct _tuple1*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->f1=
Cyc_Absyn_Rel_n(0);_tmp18F->f2=xname;_tmp18F;});_tmp18D->tvs=0;_tmp18D->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->exist_vars=0;_tmp18E->rgn_po=0;_tmp18E->fields=_tmp184;_tmp18E->tagged=0;_tmp18E;});_tmp18D->attributes=0;_tmp18D->expected_mem_kind=0;_tmp18D;});
# 602
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp18A;_tmp18A.tag=5;_tmp18A.f1=_tmp187;_tmp18A;});_tmp189;}),0);_tmp188->tl=Cyc_Toc_result_decls;_tmp188;});
*_tmp195=({struct Cyc_List_List*_tmp18B=_region_malloc(d,sizeof(*_tmp18B));_tmp18B->hd=({struct _tuple22*_tmp18C=_region_malloc(d,sizeof(*_tmp18C));_tmp18C->f1=x;_tmp18C->f2=ts;_tmp18C;});_tmp18B->tl=*_tmp195;_tmp18B;});
return x;};};}
# 607
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 616
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 624
struct _tuple24 _tmp196=*env;struct _tuple24 _tmp197=_tmp196;struct Cyc_List_List**_tmp1C1;struct _tuple1*_tmp1C0;struct Cyc_List_List*_tmp1BF;struct Cyc_List_List*_tmp1BE;struct Cyc_List_List*_tmp1BD;_LL78: _tmp1C1=(_tmp197.f1)->abs_struct_types;_tmp1C0=(_tmp197.f2)->f1;_tmp1BF=(_tmp197.f2)->f2;_tmp1BE=(_tmp197.f2)->f3;_tmp1BD=(_tmp197.f2)->f4;_LL79:;
# 629
{struct Cyc_List_List*_tmp198=*_tmp1C1;for(0;_tmp198 != 0;_tmp198=_tmp198->tl){
struct _tuple25*_tmp199=(struct _tuple25*)_tmp198->hd;struct _tuple25*_tmp19A=_tmp199;struct _tuple1*_tmp1A2;struct Cyc_List_List*_tmp1A1;void*_tmp1A0;_LL7B: _tmp1A2=_tmp19A->f1;_tmp1A1=_tmp19A->f2;_tmp1A0=_tmp19A->f3;_LL7C:;
# 632
if(Cyc_Absyn_qvar_cmp(_tmp1A2,_tmp1C0)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1BE)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1A1)){
int okay=1;
{struct Cyc_List_List*_tmp19B=_tmp1BE;for(0;_tmp19B != 0;(_tmp19B=_tmp19B->tl,_tmp1A1=_tmp1A1->tl)){
void*_tmp19C=(void*)_tmp19B->hd;
void*_tmp19D=(void*)((struct Cyc_List_List*)_check_null(_tmp1A1))->hd;
# 639
{struct Cyc_Absyn_Kind*_tmp19E=Cyc_Tcutil_typ_kind(_tmp19C);struct Cyc_Absyn_Kind*_tmp19F=_tmp19E;switch(((struct Cyc_Absyn_Kind*)_tmp19F)->kind){case Cyc_Absyn_EffKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_RgnKind: _LL80: _LL81:
# 644
 continue;default: _LL82: _LL83:
# 647
 if(Cyc_Tcutil_unify(_tmp19C,_tmp19D) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp19C),Cyc_Toc_typ_to_c(_tmp19D)))
# 649
continue;
# 652
okay=0;
goto _LL7D;}_LL7D:;}
# 655
break;}}
# 657
if(okay)
# 659
return _tmp1A0;}}}{
# 667
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1BC;_tmp1BC.tag=1;_tmp1BC.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp1BA[1]={& _tmp1BC};Cyc_aprintf(({const char*_tmp1BB="_tuple%d";_tag_dyneither(_tmp1BB,sizeof(char),9);}),_tag_dyneither(_tmp1BA,sizeof(void*),1));});});_tmp1B9;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp1A3=0;
# 671
*_tmp1C1=({struct Cyc_List_List*_tmp1A4=_region_malloc(d,sizeof(*_tmp1A4));_tmp1A4->hd=({struct _tuple25*_tmp1A5=_region_malloc(d,sizeof(*_tmp1A5));_tmp1A5->f1=_tmp1C0;_tmp1A5->f2=_tmp1BE;_tmp1A5->f3=x;_tmp1A5;});_tmp1A4->tl=*_tmp1C1;_tmp1A4;});{
# 674
struct _RegionHandle _tmp1A6=_new_region("r");struct _RegionHandle*r=& _tmp1A6;_push_region(r);
{struct Cyc_List_List*_tmp1A7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1BF,_tmp1BE);
for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){
struct Cyc_Absyn_Aggrfield*_tmp1A8=(struct Cyc_Absyn_Aggrfield*)_tmp1BD->hd;
void*t=_tmp1A8->type;
struct Cyc_List_List*atts=_tmp1A8->attributes;
# 682
if(_tmp1BD->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t)))
atts=({struct Cyc_List_List*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp1AB;_tmp1AB.tag=6;_tmp1AB.f1=0;_tmp1AB;});_tmp1AA;});_tmp1A9->tl=atts;_tmp1A9;});
# 685
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp1A7,t));
# 688
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp1AD;_tmp1AD.tag=8;_tmp1AD.f1=({struct Cyc_Absyn_ArrayInfo _tmp1AE;_tmp1AE.elt_type=Cyc_Absyn_void_star_typ();_tmp1AE.tq=Cyc_Absyn_empty_tqual(0);_tmp1AE.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp1AE.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp1AE.zt_loc=0;_tmp1AE;});_tmp1AD;});_tmp1AC;});
# 692
_tmp1A3=({struct Cyc_List_List*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->hd=({struct Cyc_Absyn_Aggrfield*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->name=_tmp1A8->name;_tmp1B0->tq=Cyc_Toc_mt_tq;_tmp1B0->type=t;_tmp1B0->width=_tmp1A8->width;_tmp1B0->attributes=atts;_tmp1B0->requires_clause=0;_tmp1B0;});_tmp1AF->tl=_tmp1A3;_tmp1AF;});}
# 694
_tmp1A3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1A3);{
struct Cyc_Absyn_Aggrdecl*_tmp1B1=({struct Cyc_Absyn_Aggrdecl*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6->kind=Cyc_Absyn_StructA;_tmp1B6->sc=Cyc_Absyn_Public;_tmp1B6->name=({struct _tuple1*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->f1=
Cyc_Absyn_Rel_n(0);_tmp1B8->f2=xname;_tmp1B8;});_tmp1B6->tvs=0;_tmp1B6->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->exist_vars=0;_tmp1B7->rgn_po=0;_tmp1B7->fields=_tmp1A3;_tmp1B7->tagged=0;_tmp1B7;});_tmp1B6->attributes=0;_tmp1B6->expected_mem_kind=0;_tmp1B6;});
# 702
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1B4;_tmp1B4.tag=5;_tmp1B4.f1=_tmp1B1;_tmp1B4;});_tmp1B3;}),0);_tmp1B2->tl=Cyc_Toc_result_decls;_tmp1B2;});{
void*_tmp1B5=x;_npop_handler(0);return _tmp1B5;};};}
# 675
;_pop_region(r);};};}
# 706
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 710
struct _tuple23 env=({struct _tuple23 _tmp1C2;_tmp1C2.f1=struct_name;_tmp1C2.f2=type_vars;_tmp1C2.f3=type_args;_tmp1C2.f4=fields;_tmp1C2;});
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 717
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp1C3=Cyc_Toc_temp_var_counter ++;
struct _tuple1*res=({struct _tuple1*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->f1=Cyc_Absyn_Loc_n;_tmp1C4->f2=({struct _dyneither_ptr*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1C8;_tmp1C8.tag=1;_tmp1C8.f1=(unsigned int)_tmp1C3;({void*_tmp1C6[1]={& _tmp1C8};Cyc_aprintf(({const char*_tmp1C7="_tmp%X";_tag_dyneither(_tmp1C7,sizeof(char),7);}),_tag_dyneither(_tmp1C6,sizeof(void*),1));});});_tmp1C5;});_tmp1C4;});
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 725
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp1C9=*env;struct _tuple26 _tmp1CA=_tmp1C9;struct Cyc_Xarray_Xarray*_tmp1D2;_LL85: _tmp1D2=(_tmp1CA.f1)->temp_labels;_LL86:;{
int _tmp1CB=Cyc_Toc_fresh_label_counter ++;
if(_tmp1CB < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp1D2))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp1D2,_tmp1CB);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1D1;_tmp1D1.tag=1;_tmp1D1.f1=(unsigned int)_tmp1CB;({void*_tmp1CF[1]={& _tmp1D1};Cyc_aprintf(({const char*_tmp1D0="_LL%X";_tag_dyneither(_tmp1D0,sizeof(char),6);}),_tag_dyneither(_tmp1CF,sizeof(void*),1));});});_tmp1CE;});
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp1D2,res)!= _tmp1CB)
({void*_tmp1CC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1CD="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp1CD,sizeof(char),43);}),_tag_dyneither(_tmp1CC,sizeof(void*),0));});
return res;};};}
# 736
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 743
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp1D3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp1D3))->hd)->name)!= 0){
++ ans;
_tmp1D3=_tmp1D3->tl;}
# 750
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 756
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 761
struct _tuple9 _tmp1D4=*a;struct _tuple9 _tmp1D5=_tmp1D4;struct _dyneither_ptr*_tmp1D9;struct Cyc_Absyn_Tqual _tmp1D8;void*_tmp1D7;_LL88: _tmp1D9=_tmp1D5.f1;_tmp1D8=_tmp1D5.f2;_tmp1D7=_tmp1D5.f3;_LL89:;
return({struct _tuple9*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->f1=_tmp1D9;_tmp1D6->f2=_tmp1D8;_tmp1D6->f3=Cyc_Toc_typ_to_c(_tmp1D7);_tmp1D6;});}
# 764
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1DA=*x;struct _tuple11 _tmp1DB=_tmp1DA;struct Cyc_Absyn_Tqual _tmp1DE;void*_tmp1DD;_LL8B: _tmp1DE=_tmp1DB.f1;_tmp1DD=_tmp1DB.f2;_LL8C:;
return({struct _tuple11*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->f1=_tmp1DE;_tmp1DC->f2=Cyc_Toc_typ_to_c(_tmp1DD);_tmp1DC;});}
# 783 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1DF=Cyc_Tcutil_compress(t);void*_tmp1E0=_tmp1DF;void*_tmp1E6;void*_tmp1E5;struct Cyc_Absyn_Tqual _tmp1E4;struct Cyc_Absyn_Exp*_tmp1E3;union Cyc_Absyn_Constraint*_tmp1E2;unsigned int _tmp1E1;switch(*((int*)_tmp1E0)){case 8: _LL8E: _tmp1E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E0)->f1).elt_type;_tmp1E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E0)->f1).tq;_tmp1E3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E0)->f1).num_elts;_tmp1E2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E0)->f1).zero_term;_tmp1E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E0)->f1).zt_loc;_LL8F:
# 786
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp1E5),_tmp1E4);case 1: _LL90: _tmp1E6=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E0)->f2;if(_tmp1E6 != 0){_LL91:
 return Cyc_Toc_typ_to_c_array(_tmp1E6);}else{goto _LL92;}default: _LL92: _LL93:
 return Cyc_Toc_typ_to_c(t);}_LL8D:;}
# 792
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 794
return({struct Cyc_Absyn_Aggrfield*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->name=f->name;_tmp1E7->tq=Cyc_Toc_mt_tq;_tmp1E7->type=
# 796
Cyc_Toc_typ_to_c(f->type);_tmp1E7->width=f->width;_tmp1E7->attributes=f->attributes;_tmp1E7->requires_clause=0;_tmp1E7;});}
# 801
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 803
return;}
# 806
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
cs=({void**_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp1E8;});
# 811
return*cs;}
# 813
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);_tmp1E9;});
# 818
return*r;}
# 820
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_tmp1EA;});
# 825
return*r;}
# 827
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1EB=Cyc_Tcutil_compress(t);void*_tmp1EC=_tmp1EB;struct Cyc_Absyn_Tvar*_tmp1ED;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EC)->tag == 2){_LL95: _tmp1ED=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EC)->f1;_LL96:
# 830
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL97: _LL98:
 return 0;}_LL94:;}
# 834
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1EE=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1EF=_tmp1EE;if(((struct Cyc_Absyn_Kind*)_tmp1EF)->kind == Cyc_Absyn_AnyKind){_LL9A: _LL9B:
 return 1;}else{_LL9C: _LL9D:
 return 0;}_LL99:;}
# 841
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1F0=t;struct Cyc_Absyn_Datatypedecl*_tmp23B;void**_tmp23A;struct Cyc_Absyn_Enumdecl*_tmp239;struct Cyc_Absyn_Aggrdecl*_tmp238;struct Cyc_Absyn_Exp*_tmp237;struct Cyc_Absyn_Exp*_tmp236;void*_tmp235;struct _tuple1*_tmp234;struct Cyc_List_List*_tmp233;struct Cyc_Absyn_Typedefdecl*_tmp232;void*_tmp231;struct Cyc_List_List*_tmp230;struct _tuple1*_tmp22F;union Cyc_Absyn_AggrInfoU _tmp22E;struct Cyc_List_List*_tmp22D;enum Cyc_Absyn_AggrKind _tmp22C;struct Cyc_List_List*_tmp22B;struct Cyc_List_List*_tmp22A;struct Cyc_Absyn_Tqual _tmp229;void*_tmp228;struct Cyc_List_List*_tmp227;int _tmp226;struct Cyc_Absyn_VarargInfo*_tmp225;struct Cyc_List_List*_tmp224;void*_tmp223;struct Cyc_Absyn_Tqual _tmp222;struct Cyc_Absyn_Exp*_tmp221;unsigned int _tmp220;void*_tmp21F;struct Cyc_Absyn_Tqual _tmp21E;union Cyc_Absyn_Constraint*_tmp21D;struct Cyc_Absyn_Datatypedecl*_tmp21C;struct Cyc_Absyn_Datatypefield*_tmp21B;struct Cyc_Absyn_Tvar*_tmp21A;void**_tmp219;switch(*((int*)_tmp1F0)){case 0: _LL9F: _LLA0:
 return t;case 1: _LLA1: _tmp219=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F0)->f2;_LLA2:
# 845
 if(*_tmp219 == 0){
*_tmp219=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 849
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp219));case 2: _LLA3: _tmp21A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F0)->f1;_LLA4:
# 851
 if((Cyc_Tcutil_tvar_kind(_tmp21A,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 854
return(void*)& Cyc_Absyn_VoidType_val;else{
# 856
return Cyc_Absyn_void_star_typ();}case 3: _LLA5: _LLA6:
# 858
 return(void*)& Cyc_Absyn_VoidType_val;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F0)->f1).field_info).KnownDatatypefield).tag == 2){_LLA7: _tmp21C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F0)->f1).field_info).KnownDatatypefield).val).f1;_tmp21B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F0)->f1).field_info).KnownDatatypefield).val).f2;_LLA8:
# 860
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp21B->name,_tmp21C->name));}else{_LLA9: _LLAA:
({void*_tmp1F1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1F2="unresolved DatatypeFieldType";_tag_dyneither(_tmp1F2,sizeof(char),29);}),_tag_dyneither(_tmp1F1,sizeof(void*),0));});}case 5: _LLAB: _tmp21F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F0)->f1).elt_typ;_tmp21E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F0)->f1).elt_tq;_tmp21D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F0)->f1).ptr_atts).bounds;_LLAC:
# 865
 _tmp21F=Cyc_Toc_typ_to_c(_tmp21F);{
void*_tmp1F3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp21D);void*_tmp1F4=_tmp1F3;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1F4)->tag == 0){_LLE0: _LLE1:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LLE2: _LLE3:
 return Cyc_Absyn_star_typ(_tmp21F,(void*)& Cyc_Absyn_HeapRgn_val,_tmp21E,Cyc_Absyn_false_conref);}_LLDF:;};case 6: _LLAD: _LLAE:
# 870
 goto _LLB0;case 7: _LLAF: _LLB0:
 return t;case 8: _LLB1: _tmp223=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F0)->f1).elt_type;_tmp222=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F0)->f1).tq;_tmp221=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F0)->f1).num_elts;_tmp220=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F0)->f1).zt_loc;_LLB2:
# 873
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp223),_tmp222,_tmp221,Cyc_Absyn_false_conref,_tmp220);case 9: _LLB3: _tmp229=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).ret_tqual;_tmp228=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).ret_typ;_tmp227=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).args;_tmp226=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).c_varargs;_tmp225=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).cyc_varargs;_tmp224=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F0)->f1).attributes;_LLB4: {
# 879
struct Cyc_List_List*_tmp1F5=0;
for(0;_tmp224 != 0;_tmp224=_tmp224->tl){
void*_tmp1F6=(void*)_tmp224->hd;void*_tmp1F7=_tmp1F6;switch(*((int*)_tmp1F7)){case 4: _LLE5: _LLE6:
 goto _LLE8;case 5: _LLE7: _LLE8:
 goto _LLEA;case 19: _LLE9: _LLEA:
 continue;case 22: _LLEB: _LLEC:
 continue;case 21: _LLED: _LLEE:
 continue;case 20: _LLEF: _LLF0:
 continue;default: _LLF1: _LLF2:
 _tmp1F5=({struct Cyc_List_List*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8->hd=(void*)_tmp224->hd;_tmp1F8->tl=_tmp1F5;_tmp1F8;});goto _LLE4;}_LLE4:;}{
# 890
struct Cyc_List_List*_tmp1F9=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp227);
if(_tmp225 != 0){
# 893
void*_tmp1FA=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp225->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp1FB=({struct _tuple9*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->f1=_tmp225->name;_tmp1FD->f2=_tmp225->tq;_tmp1FD->f3=_tmp1FA;_tmp1FD;});
_tmp1F9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1F9,({struct Cyc_List_List*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->hd=_tmp1FB;_tmp1FC->tl=0;_tmp1FC;}));}
# 897
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1FF;_tmp1FF.tag=9;_tmp1FF.f1=({struct Cyc_Absyn_FnInfo _tmp200;_tmp200.tvars=0;_tmp200.effect=0;_tmp200.ret_tqual=_tmp229;_tmp200.ret_typ=Cyc_Toc_typ_to_c(_tmp228);_tmp200.args=_tmp1F9;_tmp200.c_varargs=_tmp226;_tmp200.cyc_varargs=0;_tmp200.rgn_po=0;_tmp200.attributes=_tmp1F5;_tmp200.requires_clause=0;_tmp200.requires_relns=0;_tmp200.ensures_clause=0;_tmp200.ensures_relns=0;_tmp200;});_tmp1FF;});_tmp1FE;});};}case 10: _LLB5: _tmp22A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F0)->f1;_LLB6:
# 902
 _tmp22A=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp22A);
return Cyc_Toc_add_tuple_type(_tmp22A);case 12: _LLB7: _tmp22C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F0)->f1;_tmp22B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F0)->f2;_LLB8:
# 907
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp202;_tmp202.tag=12;_tmp202.f1=_tmp22C;_tmp202.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp22B);_tmp202;});_tmp201;});case 11: _LLB9: _tmp22E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F0)->f1).aggr_info;_tmp22D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F0)->f1).targs;_LLBA:
# 911
{union Cyc_Absyn_AggrInfoU _tmp203=_tmp22E;if((_tmp203.UnknownAggr).tag == 1){_LLF4: _LLF5:
 return t;}else{_LLF6: _LLF7:
 goto _LLF3;}_LLF3:;}{
# 915
struct Cyc_Absyn_Aggrdecl*_tmp204=Cyc_Absyn_get_known_aggrdecl(_tmp22E);
if(_tmp204->expected_mem_kind){
# 918
if(_tmp204->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp208;_tmp208.tag=0;_tmp208.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 921
Cyc_Absynpp_qvar2string(_tmp204->name));({struct Cyc_String_pa_PrintArg_struct _tmp207;_tmp207.tag=0;_tmp207.f1=(struct _dyneither_ptr)(
# 920
_tmp204->kind == Cyc_Absyn_UnionA?({const char*_tmp209="union";_tag_dyneither(_tmp209,sizeof(char),6);}):({const char*_tmp20A="struct";_tag_dyneither(_tmp20A,sizeof(char),7);}));({void*_tmp205[2]={& _tmp207,& _tmp208};Cyc_Tcutil_warn(0,({const char*_tmp206="%s %s was never defined.";_tag_dyneither(_tmp206,sizeof(char),25);}),_tag_dyneither(_tmp205,sizeof(void*),2));});});});}
# 926
if(_tmp204->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp204->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp20B=_tmp204->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp204->impl))->fields;
if(_tmp20B == 0)return Cyc_Toc_aggrdecl_type(_tmp204->name,Cyc_Absyn_strctq);
for(0;_tmp20B->tl != 0;_tmp20B=_tmp20B->tl){;}{
void*_tmp20C=((struct Cyc_Absyn_Aggrfield*)_tmp20B->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp20C))){
if(_tmp204->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0;_tmp20F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp204->name));({void*_tmp20D[1]={& _tmp20F};Cyc_Tcutil_warn(0,({const char*_tmp20E="struct %s ended up being abstract.";_tag_dyneither(_tmp20E,sizeof(char),35);}),_tag_dyneither(_tmp20D,sizeof(void*),1));});});{
# 940
struct _RegionHandle _tmp210=_new_region("r");struct _RegionHandle*r=& _tmp210;_push_region(r);
{struct Cyc_List_List*_tmp211=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp204->tvs,_tmp22D);
void*_tmp212=Cyc_Tcutil_rsubstitute(r,_tmp211,_tmp20C);
if(Cyc_Toc_is_abstract_type(_tmp212)){void*_tmp213=Cyc_Toc_aggrdecl_type(_tmp204->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp213;}{
void*_tmp214=Cyc_Toc_add_struct_type(_tmp204->name,_tmp204->tvs,_tmp22D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp204->impl))->fields);_npop_handler(0);return _tmp214;};}
# 941
;_pop_region(r);};}
# 946
return Cyc_Toc_aggrdecl_type(_tmp204->name,Cyc_Absyn_strctq);};};};case 13: _LLBB: _tmp22F=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1F0)->f1;_LLBC:
 return t;case 14: _LLBD: _tmp230=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1F0)->f1;_LLBE:
 Cyc_Toc_enumfields_to_c(_tmp230);return t;case 17: _LLBF: _tmp234=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1F0)->f1;_tmp233=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1F0)->f2;_tmp232=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1F0)->f3;_tmp231=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1F0)->f4;_LLC0:
# 950
 if(_tmp231 == 0  || Cyc_noexpand_r){
if(_tmp233 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp216;_tmp216.tag=17;_tmp216.f1=_tmp234;_tmp216.f2=0;_tmp216.f3=_tmp232;_tmp216.f4=0;_tmp216;});_tmp215;});else{
return t;}}else{
# 955
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp218;_tmp218.tag=17;_tmp218.f1=_tmp234;_tmp218.f2=0;_tmp218.f3=_tmp232;_tmp218.f4=Cyc_Toc_typ_to_c(_tmp231);_tmp218;});_tmp217;});}case 19: _LLC1: _LLC2:
 return Cyc_Absyn_uint_typ;case 15: _LLC3: _tmp235=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F0)->f1;_LLC4:
 return Cyc_Toc_rgn_typ();case 16: _LLC5: _LLC6:
 return Cyc_Toc_dyn_rgn_typ();case 20: _LLC7: _LLC8:
# 961
 goto _LLCA;case 21: _LLC9: _LLCA:
 goto _LLCC;case 22: _LLCB: _LLCC:
 goto _LLCE;case 23: _LLCD: _LLCE:
 goto _LLD0;case 24: _LLCF: _LLD0:
 goto _LLD2;case 25: _LLD1: _LLD2:
 return Cyc_Absyn_void_star_typ();case 18: _LLD3: _tmp236=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1F0)->f1;_LLD4:
# 971
 return t;case 27: _LLD5: _tmp237=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1F0)->f1;_LLD6:
# 973
 return t;case 28: _LLD7: _LLD8:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F0)->f1)->r)){case 0: _LLD9: _tmp238=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F0)->f1)->r)->f1;_LLDA:
# 976
 Cyc_Toc_aggrdecl_to_c(_tmp238,1);
if(_tmp238->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp238->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp238->name,Cyc_Absyn_strctq);}case 1: _LLDB: _tmp239=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F0)->f1)->r)->f1;_LLDC:
# 981
 Cyc_Toc_enumdecl_to_c(_tmp239);
return t;default: _LLDD: _tmp23B=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F0)->f1)->r)->f1;_tmp23A=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F0)->f2;_LLDE:
# 984
 Cyc_Toc_datatypedecl_to_c(_tmp23B);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp23A)));}}_LL9E:;}
# 989
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp23C=t;void*_tmp23E;struct Cyc_Absyn_Tqual _tmp23D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23C)->tag == 8){_LLF9: _tmp23E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23C)->f1).elt_type;_tmp23D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23C)->f1).tq;_LLFA:
# 992
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp23E,(void*)& Cyc_Absyn_HeapRgn_val,_tmp23D,Cyc_Absyn_false_conref),e);}else{_LLFB: _LLFC:
 return Cyc_Toc_cast_it(t,e);}_LLF8:;}
# 999
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp23F=Cyc_Tcutil_compress(t);void*_tmp240=_tmp23F;struct Cyc_List_List*_tmp24C;struct Cyc_Absyn_Datatypedecl*_tmp24B;struct Cyc_Absyn_Datatypefield*_tmp24A;struct Cyc_List_List*_tmp249;union Cyc_Absyn_AggrInfoU _tmp248;void*_tmp247;switch(*((int*)_tmp240)){case 0: _LLFE: _LLFF:
 return 1;case 2: _LL100: _LL101:
 return 0;case 6: _LL102: _LL103:
 goto _LL105;case 13: _LL104: _LL105:
 goto _LL107;case 14: _LL106: _LL107:
 goto _LL109;case 7: _LL108: _LL109:
 goto _LL10B;case 9: _LL10A: _LL10B:
 goto _LL10D;case 19: _LL10C: _LL10D:
 return 1;case 8: _LL10E: _tmp247=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp240)->f1).elt_type;_LL10F:
 return Cyc_Toc_atomic_typ(_tmp247);case 11: _LL110: _tmp248=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp240)->f1).aggr_info;_LL111:
# 1016
{union Cyc_Absyn_AggrInfoU _tmp241=_tmp248;if((_tmp241.UnknownAggr).tag == 1){_LL123: _LL124:
 return 0;}else{_LL125: _LL126:
 goto _LL122;}_LL122:;}{
# 1020
struct Cyc_Absyn_Aggrdecl*_tmp242=Cyc_Absyn_get_known_aggrdecl(_tmp248);
if(_tmp242->impl == 0)
return 0;
{struct Cyc_List_List*_tmp243=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp242->impl))->fields;for(0;_tmp243 != 0;_tmp243=_tmp243->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp243->hd)->type))return 0;}}
return 1;};case 12: _LL112: _tmp249=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp240)->f2;_LL113:
# 1027
 for(0;_tmp249 != 0;_tmp249=_tmp249->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp249->hd)->type))return 0;}
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp240)->f1).field_info).KnownDatatypefield).tag == 2){_LL114: _tmp24B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp240)->f1).field_info).KnownDatatypefield).val).f1;_tmp24A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp240)->f1).field_info).KnownDatatypefield).val).f2;_LL115:
# 1031
 _tmp24C=_tmp24A->typs;goto _LL117;}else{goto _LL120;}case 10: _LL116: _tmp24C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp240)->f1;_LL117:
# 1033
 for(0;_tmp24C != 0;_tmp24C=_tmp24C->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp24C->hd)).f2))return 0;}
return 1;case 3: _LL118: _LL119:
# 1038
 goto _LL11B;case 5: _LL11A: _LL11B:
 goto _LL11D;case 16: _LL11C: _LL11D:
 goto _LL11F;case 15: _LL11E: _LL11F:
 return 0;default: _LL120: _LL121:
({struct Cyc_String_pa_PrintArg_struct _tmp246;_tmp246.tag=0;_tmp246.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp244[1]={& _tmp246};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp245="atomic_typ:  bad type %s";_tag_dyneither(_tmp245,sizeof(char),25);}),_tag_dyneither(_tmp244,sizeof(void*),1));});});}_LLFD:;}
# 1046
static int Cyc_Toc_is_void_star(void*t){
void*_tmp24D=Cyc_Tcutil_compress(t);void*_tmp24E=_tmp24D;void*_tmp251;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E)->tag == 5){_LL128: _tmp251=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E)->f1).elt_typ;_LL129: {
# 1049
void*_tmp24F=Cyc_Tcutil_compress(_tmp251);void*_tmp250=_tmp24F;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp250)->tag == 0){_LL12D: _LL12E:
 return 1;}else{_LL12F: _LL130:
 return 0;}_LL12C:;}}else{_LL12A: _LL12B:
# 1053
 return 0;}_LL127:;}
# 1057
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1061
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1066
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp252=Cyc_Tcutil_compress(t);void*_tmp253=_tmp252;struct Cyc_List_List*_tmp25F;union Cyc_Absyn_AggrInfoU _tmp25E;switch(*((int*)_tmp253)){case 11: _LL132: _tmp25E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp253)->f1).aggr_info;_LL133: {
# 1069
struct Cyc_Absyn_Aggrdecl*_tmp254=Cyc_Absyn_get_known_aggrdecl(_tmp25E);
if(_tmp254->impl == 0)
({void*_tmp255=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp256="is_poly_field: type missing fields";_tag_dyneither(_tmp256,sizeof(char),35);}),_tag_dyneither(_tmp255,sizeof(void*),0));});
_tmp25F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp254->impl))->fields;goto _LL135;}case 12: _LL134: _tmp25F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp253)->f2;_LL135: {
# 1074
struct Cyc_Absyn_Aggrfield*_tmp257=Cyc_Absyn_lookup_field(_tmp25F,f);
if(_tmp257 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp25A;_tmp25A.tag=0;_tmp25A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp258[1]={& _tmp25A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp259="is_poly_field: bad field %s";_tag_dyneither(_tmp259,sizeof(char),28);}),_tag_dyneither(_tmp258,sizeof(void*),1));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp257->type);}default: _LL136: _LL137:
({struct Cyc_String_pa_PrintArg_struct _tmp25D;_tmp25D.tag=0;_tmp25D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp25B[1]={& _tmp25D};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp25C="is_poly_field: bad type %s";_tag_dyneither(_tmp25C,sizeof(char),27);}),_tag_dyneither(_tmp25B,sizeof(void*),1));});});}_LL131:;}
# 1085
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp260=e->r;void*_tmp261=_tmp260;struct Cyc_Absyn_Exp*_tmp26B;struct _dyneither_ptr*_tmp26A;struct Cyc_Absyn_Exp*_tmp269;struct _dyneither_ptr*_tmp268;switch(*((int*)_tmp261)){case 20: _LL139: _tmp269=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp261)->f1;_tmp268=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp261)->f2;_LL13A:
# 1088
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp269->topt),_tmp268) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 21: _LL13B: _tmp26B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp261)->f1;_tmp26A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp261)->f2;_LL13C: {
# 1091
void*_tmp262=Cyc_Tcutil_compress((void*)_check_null(_tmp26B->topt));void*_tmp263=_tmp262;void*_tmp267;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp263)->tag == 5){_LL140: _tmp267=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp263)->f1).elt_typ;_LL141:
# 1093
 return Cyc_Toc_is_poly_field(_tmp267,_tmp26A) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LL142: _LL143:
({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0;_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp26B->topt)));({void*_tmp264[1]={& _tmp266};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp265="is_poly_project: bad type %s";_tag_dyneither(_tmp265,sizeof(char),29);}),_tag_dyneither(_tmp264,sizeof(void*),1));});});}_LL13F:;}default: _LL13D: _LL13E:
# 1096
 return 0;}_LL138:;}
# 1101
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->hd=s;_tmp26C->tl=0;_tmp26C;}),0);}
# 1105
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->hd=s;_tmp26D->tl=0;_tmp26D;}),0);}
# 1109
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1115
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp26E[2];_tmp26E[1]=s;_tmp26E[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26E,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1119
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__fast_region_malloc_e,({struct Cyc_Absyn_Exp*_tmp26F[2];_tmp26F[1]=s;_tmp26F[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26F,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1123
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp270[2];_tmp270[1]=n;_tmp270[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp270,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{
# 1127
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp271[2];_tmp271[1]=n;_tmp271[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp271,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}
# 1130
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp272[3];_tmp272[2]=n;_tmp272[1]=s;_tmp272[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp272,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
# 1135
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->hd=e;_tmp273->tl=0;_tmp273;}),0);}
# 1138
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,({struct Cyc_List_List*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->hd=e;_tmp274->tl=0;_tmp274;}),0);}
# 1142
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,({struct Cyc_Absyn_Exp*_tmp275[0];((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp275,sizeof(struct Cyc_Absyn_Exp*),0));}),0),0);}
# 1148
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1156
int is_string=0;
{void*_tmp276=e->r;void*_tmp277=_tmp276;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp277)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp277)->f1).Wstring_c).tag){case 8: _LL145: _LL146:
 is_string=1;goto _LL144;case 9: _LL147: _LL148:
 is_string=1;goto _LL144;default: goto _LL149;}else{_LL149: _LL14A:
 goto _LL144;}_LL144:;}{
# 1162
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp27A;_tmp27A.tag=0;_tmp27A.f1=vd;_tmp27A;});_tmp279;}),0);_tmp278->tl=Cyc_Toc_result_decls;_tmp278;});
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1172
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1174
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1176
struct Cyc_Absyn_Exp*urm_exp;
urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp27B[3];_tmp27B[2]=({struct _tuple19*_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->f1=0;_tmp27E->f2=xplussz;_tmp27E;});_tmp27B[1]=({struct _tuple19*_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D->f1=0;_tmp27D->f2=xexp;_tmp27D;});_tmp27B[0]=({struct _tuple19*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->f1=0;_tmp27C->f2=xexp;_tmp27C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp27B,sizeof(struct _tuple19*),3));}),0);
# 1180
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1219 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1221
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp27F=nv;int _tmp280;_LL14C: _tmp280=_tmp27F->toplevel;_LL14D:;
return _tmp280;}
# 1225
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp281=nv;int*_tmp282;_LL14F: _tmp282=_tmp281->in_lhs;_LL150:;
return*_tmp282;}
# 1231
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp283=_region_malloc(r,sizeof(*_tmp283));_tmp283->break_lab=(struct _dyneither_ptr**)0;_tmp283->continue_lab=(struct _dyneither_ptr**)0;_tmp283->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp283->toplevel=(int)1;_tmp283->in_lhs=(int*)({int*_tmp286=_region_malloc(r,sizeof(*_tmp286));_tmp286[0]=0;_tmp286;});_tmp283->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp283->struct_info).varsizeexp=({struct Cyc_Absyn_Exp**_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285[0]=0;_tmp285;});(_tmp283->struct_info).lhs_exp=0;_tmp283->struct_info;});_tmp283->in_sizeof=(int*)({int*_tmp284=_region_malloc(r,sizeof(*_tmp284));_tmp284[0]=0;_tmp284;});_tmp283->rgn=(struct _RegionHandle*)r;_tmp283;});}
# 1242
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp287=e;struct _dyneither_ptr**_tmp28F;struct _dyneither_ptr**_tmp28E;struct Cyc_Toc_FallthruInfo*_tmp28D;int _tmp28C;int*_tmp28B;struct Cyc_Toc_StructInfo _tmp28A;int*_tmp289;_LL152: _tmp28F=_tmp287->break_lab;_tmp28E=_tmp287->continue_lab;_tmp28D=_tmp287->fallthru_info;_tmp28C=_tmp287->toplevel;_tmp28B=_tmp287->in_lhs;_tmp28A=_tmp287->struct_info;_tmp289=_tmp287->in_sizeof;_LL153:;
return({struct Cyc_Toc_Env*_tmp288=_region_malloc(r,sizeof(*_tmp288));_tmp288->break_lab=(struct _dyneither_ptr**)_tmp28F;_tmp288->continue_lab=(struct _dyneither_ptr**)_tmp28E;_tmp288->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp28D;_tmp288->toplevel=(int)_tmp28C;_tmp288->in_lhs=(int*)_tmp28B;_tmp288->struct_info=(struct Cyc_Toc_StructInfo)_tmp28A;_tmp288->in_sizeof=(int*)_tmp289;_tmp288->rgn=(struct _RegionHandle*)r;_tmp288;});}
# 1247
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp290=e;struct _dyneither_ptr**_tmp298;struct _dyneither_ptr**_tmp297;struct Cyc_Toc_FallthruInfo*_tmp296;int _tmp295;int*_tmp294;struct Cyc_Toc_StructInfo _tmp293;int*_tmp292;_LL155: _tmp298=_tmp290->break_lab;_tmp297=_tmp290->continue_lab;_tmp296=_tmp290->fallthru_info;_tmp295=_tmp290->toplevel;_tmp294=_tmp290->in_lhs;_tmp293=_tmp290->struct_info;_tmp292=_tmp290->in_sizeof;_LL156:;
return({struct Cyc_Toc_Env*_tmp291=_region_malloc(r,sizeof(*_tmp291));_tmp291->break_lab=(struct _dyneither_ptr**)_tmp298;_tmp291->continue_lab=(struct _dyneither_ptr**)_tmp297;_tmp291->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp296;_tmp291->toplevel=(int)0;_tmp291->in_lhs=(int*)_tmp294;_tmp291->struct_info=(struct Cyc_Toc_StructInfo)_tmp293;_tmp291->in_sizeof=(int*)_tmp292;_tmp291->rgn=(struct _RegionHandle*)r;_tmp291;});}
# 1251
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp299=e;struct _dyneither_ptr**_tmp2A1;struct _dyneither_ptr**_tmp2A0;struct Cyc_Toc_FallthruInfo*_tmp29F;int _tmp29E;int*_tmp29D;struct Cyc_Toc_StructInfo _tmp29C;int*_tmp29B;_LL158: _tmp2A1=_tmp299->break_lab;_tmp2A0=_tmp299->continue_lab;_tmp29F=_tmp299->fallthru_info;_tmp29E=_tmp299->toplevel;_tmp29D=_tmp299->in_lhs;_tmp29C=_tmp299->struct_info;_tmp29B=_tmp299->in_sizeof;_LL159:;
return({struct Cyc_Toc_Env*_tmp29A=_region_malloc(r,sizeof(*_tmp29A));_tmp29A->break_lab=(struct _dyneither_ptr**)_tmp2A1;_tmp29A->continue_lab=(struct _dyneither_ptr**)_tmp2A0;_tmp29A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp29F;_tmp29A->toplevel=(int)1;_tmp29A->in_lhs=(int*)_tmp29D;_tmp29A->struct_info=(struct Cyc_Toc_StructInfo)_tmp29C;_tmp29A->in_sizeof=(int*)_tmp29B;_tmp29A->rgn=(struct _RegionHandle*)r;_tmp29A;});}
# 1255
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp2A2=e;int*_tmp2A3;_LL15B: _tmp2A3=_tmp2A2->in_lhs;_LL15C:;
*_tmp2A3=b;}
# 1259
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp2A4=e;int*_tmp2A6;_LL15E: _tmp2A6=_tmp2A4->in_sizeof;_LL15F:;{
int _tmp2A5=*_tmp2A6;
*_tmp2A6=b;
return _tmp2A5;};}
# 1265
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp2A7=e;int*_tmp2A8;_LL161: _tmp2A8=_tmp2A7->in_sizeof;_LL162:;
return*_tmp2A8;}
# 1270
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1272
struct Cyc_Toc_Env*_tmp2A9=e;struct _dyneither_ptr**_tmp2B1;struct _dyneither_ptr**_tmp2B0;struct Cyc_Toc_FallthruInfo*_tmp2AF;int _tmp2AE;int*_tmp2AD;struct Cyc_Absyn_Exp**_tmp2AC;int*_tmp2AB;_LL164: _tmp2B1=_tmp2A9->break_lab;_tmp2B0=_tmp2A9->continue_lab;_tmp2AF=_tmp2A9->fallthru_info;_tmp2AE=_tmp2A9->toplevel;_tmp2AD=_tmp2A9->in_lhs;_tmp2AC=(_tmp2A9->struct_info).varsizeexp;_tmp2AB=_tmp2A9->in_sizeof;_LL165:;
return({struct Cyc_Toc_Env*_tmp2AA=_region_malloc(r,sizeof(*_tmp2AA));_tmp2AA->break_lab=(struct _dyneither_ptr**)_tmp2B1;_tmp2AA->continue_lab=(struct _dyneither_ptr**)_tmp2B0;_tmp2AA->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2AF;_tmp2AA->toplevel=(int)_tmp2AE;_tmp2AA->in_lhs=(int*)_tmp2AD;_tmp2AA->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp2AA->struct_info).varsizeexp=_tmp2AC;(_tmp2AA->struct_info).lhs_exp=exp;_tmp2AA->struct_info;});_tmp2AA->in_sizeof=(int*)_tmp2AB;_tmp2AA->rgn=(struct _RegionHandle*)r;_tmp2AA;});}
# 1278
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp2B2=e;struct _dyneither_ptr**_tmp2BA;struct _dyneither_ptr**_tmp2B9;struct Cyc_Toc_FallthruInfo*_tmp2B8;int _tmp2B7;int*_tmp2B6;struct Cyc_Toc_StructInfo _tmp2B5;int*_tmp2B4;_LL167: _tmp2BA=_tmp2B2->break_lab;_tmp2B9=_tmp2B2->continue_lab;_tmp2B8=_tmp2B2->fallthru_info;_tmp2B7=_tmp2B2->toplevel;_tmp2B6=_tmp2B2->in_lhs;_tmp2B5=_tmp2B2->struct_info;_tmp2B4=_tmp2B2->in_sizeof;_LL168:;
return({struct Cyc_Toc_Env*_tmp2B3=_region_malloc(r,sizeof(*_tmp2B3));_tmp2B3->break_lab=(struct _dyneither_ptr**)0;_tmp2B3->continue_lab=(struct _dyneither_ptr**)0;_tmp2B3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2B8;_tmp2B3->toplevel=(int)_tmp2B7;_tmp2B3->in_lhs=(int*)_tmp2B6;_tmp2B3->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B5;_tmp2B3->in_sizeof=(int*)_tmp2B4;_tmp2B3->rgn=(struct _RegionHandle*)r;_tmp2B3;});}
# 1284
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1288
struct Cyc_Toc_Env*_tmp2BB=e;struct _dyneither_ptr**_tmp2C5;struct _dyneither_ptr**_tmp2C4;struct Cyc_Toc_FallthruInfo*_tmp2C3;int _tmp2C2;int*_tmp2C1;struct Cyc_Toc_StructInfo _tmp2C0;int*_tmp2BF;_LL16A: _tmp2C5=_tmp2BB->break_lab;_tmp2C4=_tmp2BB->continue_lab;_tmp2C3=_tmp2BB->fallthru_info;_tmp2C2=_tmp2BB->toplevel;_tmp2C1=_tmp2BB->in_lhs;_tmp2C0=_tmp2BB->struct_info;_tmp2BF=_tmp2BB->in_sizeof;_LL16B:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp2BE=_region_malloc(r,sizeof(*_tmp2BE));_tmp2BE->label=fallthru_l;_tmp2BE->binders=fallthru_binders;_tmp2BE;});
# 1291
return({struct Cyc_Toc_Env*_tmp2BC=_region_malloc(r,sizeof(*_tmp2BC));_tmp2BC->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2BD=_region_malloc(r,sizeof(*_tmp2BD));_tmp2BD[0]=break_l;_tmp2BD;});_tmp2BC->continue_lab=(struct _dyneither_ptr**)_tmp2C4;_tmp2BC->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi;_tmp2BC->toplevel=(int)_tmp2C2;_tmp2BC->in_lhs=(int*)_tmp2C1;_tmp2BC->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C0;_tmp2BC->in_sizeof=(int*)_tmp2BF;_tmp2BC->rgn=(struct _RegionHandle*)r;_tmp2BC;});};}
# 1294
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1296
struct Cyc_Toc_Env*_tmp2C6=e;struct _dyneither_ptr**_tmp2CF;struct _dyneither_ptr**_tmp2CE;struct Cyc_Toc_FallthruInfo*_tmp2CD;int _tmp2CC;int*_tmp2CB;struct Cyc_Toc_StructInfo _tmp2CA;int*_tmp2C9;_LL16D: _tmp2CF=_tmp2C6->break_lab;_tmp2CE=_tmp2C6->continue_lab;_tmp2CD=_tmp2C6->fallthru_info;_tmp2CC=_tmp2C6->toplevel;_tmp2CB=_tmp2C6->in_lhs;_tmp2CA=_tmp2C6->struct_info;_tmp2C9=_tmp2C6->in_sizeof;_LL16E:;
return({struct Cyc_Toc_Env*_tmp2C7=_region_malloc(r,sizeof(*_tmp2C7));_tmp2C7->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2C8=_region_malloc(r,sizeof(*_tmp2C8));_tmp2C8[0]=break_l;_tmp2C8;});_tmp2C7->continue_lab=(struct _dyneither_ptr**)_tmp2CE;_tmp2C7->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp2C7->toplevel=(int)_tmp2CC;_tmp2C7->in_lhs=(int*)_tmp2CB;_tmp2C7->struct_info=(struct Cyc_Toc_StructInfo)_tmp2CA;_tmp2C7->in_sizeof=(int*)_tmp2C9;_tmp2C7->rgn=(struct _RegionHandle*)r;_tmp2C7;});}
# 1303
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1306
struct Cyc_Toc_Env*_tmp2D0=e;struct _dyneither_ptr**_tmp2D9;struct _dyneither_ptr**_tmp2D8;struct Cyc_Toc_FallthruInfo*_tmp2D7;int _tmp2D6;int*_tmp2D5;struct Cyc_Toc_StructInfo _tmp2D4;int*_tmp2D3;_LL170: _tmp2D9=_tmp2D0->break_lab;_tmp2D8=_tmp2D0->continue_lab;_tmp2D7=_tmp2D0->fallthru_info;_tmp2D6=_tmp2D0->toplevel;_tmp2D5=_tmp2D0->in_lhs;_tmp2D4=_tmp2D0->struct_info;_tmp2D3=_tmp2D0->in_sizeof;_LL171:;
return({struct Cyc_Toc_Env*_tmp2D1=_region_malloc(r,sizeof(*_tmp2D1));_tmp2D1->break_lab=(struct _dyneither_ptr**)0;_tmp2D1->continue_lab=(struct _dyneither_ptr**)_tmp2D8;_tmp2D1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2D2=_region_malloc(r,sizeof(*_tmp2D2));_tmp2D2->label=next_l;_tmp2D2->binders=0;_tmp2D2;});_tmp2D1->toplevel=(int)_tmp2D6;_tmp2D1->in_lhs=(int*)_tmp2D5;_tmp2D1->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D4;_tmp2D1->in_sizeof=(int*)_tmp2D3;_tmp2D1->rgn=(struct _RegionHandle*)r;_tmp2D1;});}
# 1319 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1322
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1324
void*_tmp2DA=e->annot;void*_tmp2DB=_tmp2DA;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2DB)->tag == Cyc_CfFlowInfo_UnknownZ){_LL173: _LL174:
# 1327
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2DB)->tag == Cyc_CfFlowInfo_NotZero){_LL175: _LL176:
# 1330
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2DB)->tag == Cyc_CfFlowInfo_IsZero){_LL177: _LL178:
# 1332
({void*_tmp2DC=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2DD="dereference of NULL pointer";_tag_dyneither(_tmp2DD,sizeof(char),28);}),_tag_dyneither(_tmp2DC,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2DB)->tag == Cyc_Absyn_EmptyAnnot){_LL179: _LL17A:
# 1338
 return 1;}else{_LL17B: _LL17C:
({void*_tmp2DE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2DF="need_null_check";_tag_dyneither(_tmp2DF,sizeof(char),16);}),_tag_dyneither(_tmp2DE,sizeof(void*),0));});}}}}_LL172:;}
# 1343
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2E0=e->annot;void*_tmp2E1=_tmp2E0;struct Cyc_List_List*_tmp2E7;struct Cyc_List_List*_tmp2E6;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2E1)->tag == Cyc_CfFlowInfo_UnknownZ){_LL17E: _tmp2E6=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2E1)->f1;_LL17F:
 return _tmp2E6;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2E1)->tag == Cyc_CfFlowInfo_NotZero){_LL180: _tmp2E7=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2E1)->f1;_LL181:
 return _tmp2E7;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2E1)->tag == Cyc_CfFlowInfo_IsZero){_LL182: _LL183:
# 1348
({void*_tmp2E2=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2E3="dereference of NULL pointer";_tag_dyneither(_tmp2E3,sizeof(char),28);}),_tag_dyneither(_tmp2E2,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2E1)->tag == Cyc_Absyn_EmptyAnnot){_LL184: _LL185:
 return 0;}else{_LL186: _LL187:
({void*_tmp2E4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E5="get_relns";_tag_dyneither(_tmp2E5,sizeof(char),10);}),_tag_dyneither(_tmp2E4,sizeof(void*),0));});}}}}_LL17D:;}static char _tmp2EB[8]="*bogus*";
# 1358
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1368 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1371
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2E8=a->r;void*_tmp2E9=_tmp2E8;void*_tmp2EC;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E9)->tag == 1){_LL189: _tmp2EC=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E9)->f1;_LL18A: {
# 1374
struct Cyc_Absyn_Vardecl*_tmp2EA=Cyc_Tcutil_nonesc_vardecl(_tmp2EC);
if(_tmp2EA == 0)goto _LL18C;
x=_tmp2EA;
goto _LL188;}}else{_LL18B: _LL18C: {
# 1380
static struct _dyneither_ptr bogus_string={_tmp2EB,_tmp2EB,_tmp2EB + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1383
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1389
x=& bogus_vardecl;
x->type=a_typ;}}_LL188:;}{
# 1392
void*_tmp2ED=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1397
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2ED);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1406
if(valid_rop_i){
struct Cyc_List_List*_tmp2EE=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1414
if(!Cyc_Relations_consistent_relations(_tmp2EE))return 1;}
# 1417
inner_loop: {
void*_tmp2EF=i->r;void*_tmp2F0=_tmp2EF;struct Cyc_Absyn_Exp*_tmp2F5;struct Cyc_Absyn_Exp*_tmp2F4;void*_tmp2F3;struct Cyc_Absyn_Exp*_tmp2F2;switch(*((int*)_tmp2F0)){case 13: _LL18E: _tmp2F3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F0)->f1;_tmp2F2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F0)->f2;_LL18F:
 i=_tmp2F2;goto inner_loop;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F0)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F0)->f2)->tl != 0){_LL190: _tmp2F5=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F0)->f2)->hd;_tmp2F4=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F0)->f2)->tl)->hd;_LL191: {
# 1423
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp2F4,& rop_z)){
# 1426
struct Cyc_List_List*_tmp2F1=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1428
return !Cyc_Relations_consistent_relations(_tmp2F1);}
# 1430
goto _LL18D;}}else{goto _LL192;}}else{goto _LL192;}}else{goto _LL192;}default: _LL192: _LL193:
 goto _LL18D;}_LL18D:;}
# 1433
return 0;};}
# 1436
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2F6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2F7="Missing type in primop ";_tag_dyneither(_tmp2F7,sizeof(char),24);}),_tag_dyneither(_tmp2F6,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1440
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2F8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2F9="Missing type in primop ";_tag_dyneither(_tmp2F9,sizeof(char),24);}),_tag_dyneither(_tmp2F8,sizeof(void*),0));});
return(void*)_check_null(e->topt);}
# 1444
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple11*_tmp2FA=_cycalloc(sizeof(*_tmp2FA));_tmp2FA->f1=Cyc_Toc_mt_tq;_tmp2FA->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp2FA;});}
# 1447
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);
return({struct _tuple19*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->f1=0;_tmp2FB->f2=e;_tmp2FB;});}
# 1454
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1457
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2FC=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp2FC=Cyc_Absyn_add_exp(_tmp2FC,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1465
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp2FC): Cyc_Toc_malloc_ptr(_tmp2FC);else{
# 1469
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp2FC);}}else{
# 1474
t=struct_typ;
eo=0;}
# 1477
if(do_declare){
void*_tmp2FD=x->r;void*_tmp2FE=_tmp2FD;void*_tmp301;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2FE)->tag == 1){_LL195: _tmp301=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2FE)->f1;_LL196:
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(Cyc_Absyn_binding2qvar(_tmp301),t,eo,s,0),0);}else{_LL197: _LL198:
({void*_tmp2FF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp300="bogus x to make_struct";_tag_dyneither(_tmp300,sizeof(char),23);}),_tag_dyneither(_tmp2FF,sizeof(void*),0));});}_LL194:;}else{
# 1484
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1487
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1491
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp302=Cyc_Tcutil_compress(typ);void*_tmp303=_tmp302;struct Cyc_List_List*_tmp306;union Cyc_Absyn_AggrInfoU _tmp305;struct Cyc_List_List*_tmp304;switch(*((int*)_tmp303)){case 11: _LL19A: _tmp305=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp303)->f1).aggr_info;_tmp304=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp303)->f1).targs;_LL19B: {
# 1494
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp305);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp306=aggrfields;goto _LL19D;};}case 12: _LL19C: _tmp306=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp303)->f2;_LL19D:
# 1499
 if(_tmp306 == 0)return 0;
return 1;default: _LL19E: _LL19F:
# 1505
 return 0;}_LL199:;}
# 1509
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp307=Cyc_Tcutil_compress(t);void*_tmp308=_tmp307;void*_tmp30C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp308)->tag == 8){_LL1A1: _tmp30C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp308)->f1).elt_type;_LL1A2:
 return _tmp30C;}else{_LL1A3: _LL1A4:
({struct Cyc_String_pa_PrintArg_struct _tmp30B;_tmp30B.tag=0;_tmp30B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp309[1]={& _tmp30B};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp30A="array_elt_type called on %s";_tag_dyneither(_tmp30A,sizeof(char),28);}),_tag_dyneither(_tmp309,sizeof(void*),1));});});}_LL1A0:;}
# 1516
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1522
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1527
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1532
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp30D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp30D != 0;_tmp30D=_tmp30D->tl){
# 1536
struct _tuple19*_tmp30E=(struct _tuple19*)_tmp30D->hd;struct _tuple19*_tmp30F=_tmp30E;struct Cyc_List_List*_tmp321;struct Cyc_Absyn_Exp*_tmp320;_LL1A6: _tmp321=_tmp30F->f1;_tmp320=_tmp30F->f2;_LL1A7:;{
# 1540
struct Cyc_Absyn_Exp*e_index;
if(_tmp321 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1544
if(_tmp321->tl != 0)({void*_tmp310=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp311="multiple designators in array";_tag_dyneither(_tmp311,sizeof(char),30);}),_tag_dyneither(_tmp310,sizeof(void*),0));});{
void*_tmp312=(void*)_tmp321->hd;
void*_tmp313=_tmp312;struct Cyc_Absyn_Exp*_tmp316;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp313)->tag == 0){_LL1A9: _tmp316=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp313)->f1;_LL1AA:
# 1548
 Cyc_Toc_exp_to_c(nv,_tmp316);
e_index=_tmp316;
goto _LL1A8;}else{_LL1AB: _LL1AC:
({void*_tmp314=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp315="field name designators in array";_tag_dyneither(_tmp315,sizeof(char),32);}),_tag_dyneither(_tmp314,sizeof(void*),0));});}_LL1A8:;};}{
# 1554
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp317=_tmp320->r;void*_tmp318=_tmp317;void*_tmp31F;struct Cyc_List_List*_tmp31E;struct Cyc_Absyn_Vardecl*_tmp31D;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_Absyn_Exp*_tmp31B;int _tmp31A;struct Cyc_List_List*_tmp319;switch(*((int*)_tmp318)){case 25: _LL1AE: _tmp319=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_LL1AF:
# 1557
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp319,s);
goto _LL1AD;case 26: _LL1B0: _tmp31D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_tmp31C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f2;_tmp31B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f3;_tmp31A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f4;_LL1B1:
# 1560
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp31D,_tmp31C,_tmp31B,_tmp31A,s,0);
# 1562
goto _LL1AD;case 29: _LL1B2: _tmp31F=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_tmp31E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp318)->f2;_LL1B3:
# 1564
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp31F,_tmp31E,s);
goto _LL1AD;case 27: _LL1B4: _LL1B5:
# 1567
 goto _LL1AD;default: _LL1B6: _LL1B7:
# 1569
 Cyc_Toc_exp_to_c(nv,_tmp320);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp320),0),s,0);
goto _LL1AD;}_LL1AD:;};};}}
# 1575
return s;}
# 1580
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1585
struct _tuple1*_tmp322=vd->name;
void*expected_elt_type;
{void*_tmp323=Cyc_Tcutil_compress(comprehension_type);void*_tmp324=_tmp323;void*_tmp328;void*_tmp327;switch(*((int*)_tmp324)){case 8: _LL1B9: _tmp327=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp324)->f1).elt_type;_LL1BA:
# 1589
 expected_elt_type=_tmp327;goto _LL1B8;case 5: _LL1BB: _tmp328=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->f1).elt_typ;_LL1BC:
# 1591
 expected_elt_type=_tmp328;goto _LL1B8;default: _LL1BD: _LL1BE:
({void*_tmp325=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp326="init_comprehension passed a bad type!";_tag_dyneither(_tmp326,sizeof(char),38);}),_tag_dyneither(_tmp325,sizeof(void*),0));});}_LL1B8:;}{
# 1594
void*_tmp329=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1598
{void*_tmp32A=e2->r;void*_tmp32B=_tmp32A;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp32B)->tag == 27){_LL1C0: _LL1C1:
 return Cyc_Absyn_skip_stmt(0);}else{_LL1C2: _LL1C3:
 goto _LL1BF;}_LL1BF:;}{
# 1602
struct _RegionHandle _tmp32C=_new_region("r2");struct _RegionHandle*r2=& _tmp32C;_push_region(r2);
{struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp322,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp322,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp322,0),0);
# 1609
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp322,0),0);
struct Cyc_Absyn_Stmt*body;
# 1613
{void*_tmp32D=e2->r;void*_tmp32E=_tmp32D;void*_tmp335;struct Cyc_List_List*_tmp334;struct Cyc_Absyn_Vardecl*_tmp333;struct Cyc_Absyn_Exp*_tmp332;struct Cyc_Absyn_Exp*_tmp331;int _tmp330;struct Cyc_List_List*_tmp32F;switch(*((int*)_tmp32E)){case 25: _LL1C5: _tmp32F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp32E)->f1;_LL1C6:
# 1615
 body=Cyc_Toc_init_array(nv,expected_elt_type,lval,_tmp32F,Cyc_Toc_skip_stmt_dl());
goto _LL1C4;case 26: _LL1C7: _tmp333=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32E)->f1;_tmp332=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32E)->f2;_tmp331=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32E)->f3;_tmp330=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32E)->f4;_LL1C8:
# 1618
 body=Cyc_Toc_init_comprehension(nv,expected_elt_type,lval,_tmp333,_tmp332,_tmp331,_tmp330,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1C4;case 29: _LL1C9: _tmp335=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp32E)->f1;_tmp334=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp32E)->f2;_LL1CA:
# 1621
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp335,_tmp334,Cyc_Toc_skip_stmt_dl());
goto _LL1C4;default: _LL1CB: _LL1CC:
# 1624
 Cyc_Toc_exp_to_c(nv,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL1C4;}_LL1C4:;}{
# 1628
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1630
if(zero_term){
# 1635
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp329,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1640
struct Cyc_Absyn_Stmt*_tmp336=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp322,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp336;};};}
# 1603
;_pop_region(r2);};};}
# 1646
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1649
{struct Cyc_List_List*_tmp337=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp337 != 0;_tmp337=_tmp337->tl){
struct _tuple19*_tmp338=(struct _tuple19*)_tmp337->hd;struct _tuple19*_tmp339=_tmp338;struct Cyc_List_List*_tmp34D;struct Cyc_Absyn_Exp*_tmp34C;_LL1CE: _tmp34D=_tmp339->f1;_tmp34C=_tmp339->f2;_LL1CF:;
if(_tmp34D == 0)
({void*_tmp33A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp33B="empty designator list";_tag_dyneither(_tmp33B,sizeof(char),22);}),_tag_dyneither(_tmp33A,sizeof(void*),0));});
if(_tmp34D->tl != 0)
({void*_tmp33C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp33D="too many designators in anonymous struct";_tag_dyneither(_tmp33D,sizeof(char),41);}),_tag_dyneither(_tmp33C,sizeof(void*),0));});{
void*_tmp33E=(void*)_tmp34D->hd;void*_tmp33F=_tmp33E;struct _dyneither_ptr*_tmp34B;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33F)->tag == 1){_LL1D1: _tmp34B=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33F)->f1;_LL1D2: {
# 1657
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp34B,0);
{void*_tmp340=_tmp34C->r;void*_tmp341=_tmp340;void*_tmp348;struct Cyc_List_List*_tmp347;struct Cyc_Absyn_Vardecl*_tmp346;struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Absyn_Exp*_tmp344;int _tmp343;struct Cyc_List_List*_tmp342;switch(*((int*)_tmp341)){case 25: _LL1D6: _tmp342=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp341)->f1;_LL1D7:
# 1660
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp34C->topt),lval,_tmp342,s);goto _LL1D5;case 26: _LL1D8: _tmp346=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp341)->f1;_tmp345=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp341)->f2;_tmp344=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp341)->f3;_tmp343=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp341)->f4;_LL1D9:
# 1662
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp34C->topt),lval,_tmp346,_tmp345,_tmp344,_tmp343,s,0);
goto _LL1D5;case 29: _LL1DA: _tmp348=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp341)->f1;_tmp347=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp341)->f2;_LL1DB:
# 1665
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp348,_tmp347,s);goto _LL1D5;case 27: _LL1DC: _LL1DD:
# 1667
 goto _LL1D5;default: _LL1DE: _LL1DF:
# 1669
 Cyc_Toc_exp_to_c(nv,_tmp34C);
# 1671
if(Cyc_Toc_is_poly_field(struct_type,_tmp34B))
_tmp34C=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp34C);
# 1674
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp34C,0),0),s,0);
goto _LL1D5;}_LL1D5:;}
# 1677
goto _LL1D0;}}else{_LL1D3: _LL1D4:
({void*_tmp349=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp34A="array designator in struct";_tag_dyneither(_tmp34A,sizeof(char),27);}),_tag_dyneither(_tmp349,sizeof(void*),0));});}_LL1D0:;};}}
# 1681
return s;}
# 1686
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1690
struct _RegionHandle _tmp34E=_new_region("r");struct _RegionHandle*r=& _tmp34E;_push_region(r);
{struct Cyc_List_List*_tmp34F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp350=Cyc_Toc_add_tuple_type(_tmp34F);
# 1694
struct _tuple1*_tmp351=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp352=Cyc_Absyn_var_exp(_tmp351,0);
struct Cyc_Absyn_Stmt*_tmp353=Cyc_Absyn_exp_stmt(_tmp352,0);
# 1698
struct Cyc_Absyn_Exp*(*_tmp354)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1700
int is_atomic=1;
struct Cyc_List_List*_tmp355=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp355);for(0;_tmp355 != 0;(_tmp355=_tmp355->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp355->hd;
struct Cyc_Absyn_Exp*lval=_tmp354(_tmp352,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp356=e->r;void*_tmp357=_tmp356;struct Cyc_Absyn_Vardecl*_tmp35C;struct Cyc_Absyn_Exp*_tmp35B;struct Cyc_Absyn_Exp*_tmp35A;int _tmp359;struct Cyc_List_List*_tmp358;switch(*((int*)_tmp357)){case 25: _LL1E1: _tmp358=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp357)->f1;_LL1E2:
# 1708
 _tmp353=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp358,_tmp353);
goto _LL1E0;case 26: _LL1E3: _tmp35C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp357)->f1;_tmp35B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp357)->f2;_tmp35A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp357)->f3;_tmp359=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp357)->f4;_LL1E4:
# 1711
 _tmp353=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp35C,_tmp35B,_tmp35A,_tmp359,_tmp353,0);
# 1713
goto _LL1E0;case 27: _LL1E5: _LL1E6:
# 1715
 goto _LL1E0;default: _LL1E7: _LL1E8:
# 1718
 Cyc_Toc_exp_to_c(nv,e);
_tmp353=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp354(_tmp352,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp353,0);
# 1721
goto _LL1E0;}_LL1E0:;};}}{
# 1724
struct Cyc_Absyn_Exp*_tmp35D=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp351,0),_tmp350,_tmp353,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp35D;};}
# 1691
;_pop_region(r);}
# 1728
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp35E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp35E != 0;_tmp35E=_tmp35E->tl){
struct Cyc_Absyn_Aggrfield*_tmp35F=(struct Cyc_Absyn_Aggrfield*)_tmp35E->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp35F->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1735
({void*_tmp360=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp363;_tmp363.tag=0;_tmp363.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp361[1]={& _tmp363};Cyc_aprintf(({const char*_tmp362="get_member_offset %s failed";_tag_dyneither(_tmp362,sizeof(char),28);}),_tag_dyneither(_tmp361,sizeof(void*),1));});}),_tag_dyneither(_tmp360,sizeof(void*),0));});}
# 1738
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp364=Cyc_Tcutil_compress(*typ);void*_tmp365=_tmp364;void*_tmp36D;struct Cyc_Absyn_Tqual _tmp36C;struct Cyc_Absyn_Exp*_tmp36B;union Cyc_Absyn_Constraint*_tmp36A;unsigned int _tmp369;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp365)->tag == 8){_LL1EA: _tmp36D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp365)->f1).elt_type;_tmp36C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp365)->f1).tq;_tmp36B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp365)->f1).num_elts;_tmp36A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp365)->f1).zero_term;_tmp369=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp365)->f1).zt_loc;_LL1EB:
# 1743
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp36B))){
*typ=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp367;_tmp367.tag=8;_tmp367.f1=({struct Cyc_Absyn_ArrayInfo _tmp368;_tmp368.elt_type=_tmp36D;_tmp368.tq=_tmp36C;_tmp368.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp368.zero_term=_tmp36A;_tmp368.zt_loc=_tmp369;_tmp368;});_tmp367;});_tmp366;});
return 1;}
# 1747
goto _LL1E9;}else{_LL1EC: _LL1ED:
 goto _LL1E9;}_LL1E9:;}
# 1750
return 0;}
# 1753
static int Cyc_Toc_is_array_type(void*t){
void*_tmp36E=Cyc_Tcutil_compress(t);void*_tmp36F=_tmp36E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36F)->tag == 8){_LL1EF: _LL1F0:
 return 1;}else{_LL1F1: _LL1F2:
 return 0;}_LL1EE:;}
# 1760
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1762
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1766
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp370=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp371=_tmp370->attributes;
void*_tmp372=_tmp370->type;
if((!Cyc_Toc_is_array_type(_tmp372),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp372))))
_tmp371=({struct Cyc_List_List*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp375;_tmp375.tag=6;_tmp375.f1=0;_tmp375;});_tmp374;});_tmp373->tl=_tmp371;_tmp373;});{
struct Cyc_Absyn_Aggrfield*_tmp376=({struct Cyc_Absyn_Aggrfield*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378->name=_tmp370->name;_tmp378->tq=Cyc_Toc_mt_tq;_tmp378->type=new_field_type;_tmp378->width=_tmp370->width;_tmp378->attributes=_tmp371;_tmp378->requires_clause=0;_tmp378;});
# 1778
new_fields=({struct Cyc_List_List*_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377->hd=_tmp376;_tmp377->tl=new_fields;_tmp377;});};}else{
# 1780
new_fields=({struct Cyc_List_List*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp379->tl=new_fields;_tmp379;});}}{
# 1782
struct Cyc_List_List*_tmp37A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp382;_tmp382.tag=1;_tmp382.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp380[1]={& _tmp382};Cyc_aprintf(({const char*_tmp381="_genStruct%d";_tag_dyneither(_tmp381,sizeof(char),13);}),_tag_dyneither(_tmp380,sizeof(void*),1));});});_tmp37F;});
# 1785
struct _tuple1*qv=({struct _tuple1*_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E->f1=Cyc_Absyn_Abs_n(0,1);_tmp37E->f2=name;_tmp37E;});
struct Cyc_Absyn_AggrdeclImpl*_tmp37B=({struct Cyc_Absyn_AggrdeclImpl*_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;_tmp37D->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;_tmp37D->fields=_tmp37A;_tmp37D->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;_tmp37D;});
# 1790
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C->kind=ad->kind;_tmp37C->sc=ad->sc;_tmp37C->name=qv;_tmp37C->tvs=ad->tvs;_tmp37C->impl=_tmp37B;_tmp37C->expected_mem_kind=0;_tmp37C->attributes=ad->attributes;_tmp37C;});
# 1797
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1801
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1807
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp383=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp383,0);
do_declare=1;}else{
# 1815
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1818
struct Cyc_Absyn_Stmt*_tmp384=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1822
struct Cyc_Absyn_Exp**varexp;
{void*_tmp385=_tmp384->r;void*_tmp386=_tmp385;struct Cyc_Absyn_Exp**_tmp389;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp386)->tag == 1){_LL1F4: _tmp389=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp386)->f1;_LL1F5:
 varexp=_tmp389;goto _LL1F3;}else{_LL1F6: _LL1F7:
({void*_tmp387=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp388="stmt not an expression!";_tag_dyneither(_tmp388,sizeof(char),24);}),_tag_dyneither(_tmp387,sizeof(void*),0));});}_LL1F3:;}{
# 1828
struct Cyc_Absyn_Exp*(*_tmp38A)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1832
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp38B=Cyc_Tcutil_compress(struct_type);void*_tmp38C=_tmp38B;union Cyc_Absyn_AggrInfoU _tmp390;struct Cyc_List_List*_tmp38F;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38C)->tag == 11){_LL1F9: _tmp390=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38C)->f1).aggr_info;_tmp38F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38C)->f1).targs;_LL1FA:
# 1839
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp390);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp38F;
goto _LL1F8;}else{_LL1FB: _LL1FC:
({void*_tmp38D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp38E="init_struct: bad struct type";_tag_dyneither(_tmp38E,sizeof(char),29);}),_tag_dyneither(_tmp38D,sizeof(void*),0));});}_LL1F8:;}
# 1846
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp391=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp391->tl != 0;_tmp391=_tmp391->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp392=(struct Cyc_Absyn_Aggrfield*)_tmp391->hd;
struct _RegionHandle _tmp393=_new_region("temp");struct _RegionHandle*temp=& _tmp393;_push_region(temp);
{struct Cyc_List_List*_tmp394=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1855
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp394,_tmp392->type))){
struct Cyc_List_List*_tmp395=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp396=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp394,_tmp395);
# 1859
struct Cyc_List_List*new_fields=0;
for(_tmp391=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp391 != 0;_tmp391=_tmp391->tl){
struct Cyc_Absyn_Aggrfield*_tmp397=(struct Cyc_Absyn_Aggrfield*)_tmp391->hd;
struct Cyc_Absyn_Aggrfield*_tmp398=({struct Cyc_Absyn_Aggrfield*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B->name=_tmp397->name;_tmp39B->tq=Cyc_Toc_mt_tq;_tmp39B->type=
# 1864
Cyc_Tcutil_rsubstitute(temp,_tmp396,_tmp397->type);_tmp39B->width=_tmp397->width;_tmp39B->attributes=_tmp397->attributes;_tmp39B->requires_clause=0;_tmp39B;});
# 1872
if(_tmp391->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp398->type))
index=({struct _dyneither_ptr**_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399[0]=_tmp398->name;_tmp399;});}
# 1879
new_fields=({struct Cyc_List_List*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->hd=_tmp398;_tmp39A->tl=new_fields;_tmp39A;});}
# 1881
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=1;_tmp3A7.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp3A5[1]={& _tmp3A7};Cyc_aprintf(({const char*_tmp3A6="_genStruct%d";_tag_dyneither(_tmp3A6,sizeof(char),13);}),_tag_dyneither(_tmp3A5,sizeof(void*),1));});});_tmp3A4;});
# 1885
struct _tuple1*qv=({struct _tuple1*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3->f1=Cyc_Absyn_Abs_n(0,1);_tmp3A3->f2=name;_tmp3A3;});
struct Cyc_Absyn_AggrdeclImpl*_tmp39C=({struct Cyc_Absyn_AggrdeclImpl*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2->exist_vars=0;_tmp3A2->rgn_po=0;_tmp3A2->fields=aggrfields;_tmp3A2->tagged=0;_tmp3A2;});
# 1890
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->kind=Cyc_Absyn_StructA;_tmp3A1->sc=Cyc_Absyn_Public;_tmp3A1->name=qv;_tmp3A1->tvs=0;_tmp3A1->expected_mem_kind=0;_tmp3A1->impl=_tmp39C;_tmp3A1->attributes=0;_tmp3A1;});
# 1897
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp39E;_tmp39E.tag=11;_tmp39E.f1=({struct Cyc_Absyn_AggrInfo _tmp39F;_tmp39F.aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0[0]=new_ad;_tmp3A0;}));_tmp39F.targs=0;_tmp39F;});_tmp39E;});_tmp39D;});
# 1901
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1851
;_pop_region(temp);};}{
# 1905
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3A8=_new_region("r");struct _RegionHandle*r=& _tmp3A8;_push_region(r);
{struct Cyc_List_List*_tmp3A9=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3A9 != 0;_tmp3A9=_tmp3A9->tl){
struct _tuple19*_tmp3AA=(struct _tuple19*)_tmp3A9->hd;struct _tuple19*_tmp3AB=_tmp3AA;struct Cyc_List_List*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3CF;_LL1FE: _tmp3D0=_tmp3AB->f1;_tmp3CF=_tmp3AB->f2;_LL1FF:;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3CF->topt));
if(_tmp3D0 == 0)
({void*_tmp3AC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3AD="empty designator list";_tag_dyneither(_tmp3AD,sizeof(char),22);}),_tag_dyneither(_tmp3AC,sizeof(void*),0));});
if(_tmp3D0->tl != 0){
# 1915
struct _tuple1*_tmp3AE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3AF=Cyc_Absyn_var_exp(_tmp3AE,0);
for(0;_tmp3D0 != 0;_tmp3D0=_tmp3D0->tl){
void*_tmp3B0=(void*)_tmp3D0->hd;void*_tmp3B1=_tmp3B0;struct _dyneither_ptr*_tmp3B4;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B1)->tag == 1){_LL201: _tmp3B4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B1)->f1;_LL202:
# 1921
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3B4))
_tmp3AF=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3AF);
_tmp384=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38A(xexp,_tmp3B4,0),_tmp3AF,0),0),_tmp384,0);
# 1925
goto _LL200;}else{_LL203: _LL204:
({void*_tmp3B2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3B3="array designator in struct";_tag_dyneither(_tmp3B3,sizeof(char),27);}),_tag_dyneither(_tmp3B2,sizeof(void*),0));});}_LL200:;}
# 1929
Cyc_Toc_exp_to_c(nv,_tmp3CF);
_tmp384=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3AF,_tmp3CF,0),0),_tmp384,0);}else{
# 1932
void*_tmp3B5=(void*)_tmp3D0->hd;void*_tmp3B6=_tmp3B5;struct _dyneither_ptr*_tmp3CE;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B6)->tag == 1){_LL206: _tmp3CE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B6)->f1;_LL207: {
# 1934
struct Cyc_Absyn_Exp*lval=_tmp38A(xexp,_tmp3CE,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3CE);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp384=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp384,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1943
int e1_translated=0;
{void*_tmp3B7=_tmp3CF->r;void*_tmp3B8=_tmp3B7;void*_tmp3CB;struct Cyc_List_List*_tmp3CA;struct Cyc_Absyn_Exp*_tmp3C9;void*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3C7;struct Cyc_Absyn_Exp*_tmp3C6;struct Cyc_Absyn_Exp*_tmp3C5;int _tmp3C4;struct Cyc_List_List*_tmp3C3;switch(*((int*)_tmp3B8)){case 25: _LL20B: _tmp3C3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3B8)->f1;_LL20C:
# 1946
 _tmp384=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3CE)))->type,lval,_tmp3C3,_tmp384);
# 1948
goto _LL20A;case 26: _LL20D: _tmp3C7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B8)->f1;_tmp3C6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B8)->f2;_tmp3C5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B8)->f3;_tmp3C4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B8)->f4;_LL20E:
# 1950
 _tmp384=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3CE)))->type,lval,_tmp3C7,_tmp3C6,_tmp3C5,_tmp3C4,_tmp384,0);
# 1954
e1_translated=1;
_tmp3C9=_tmp3C6;_tmp3C8=(void*)_check_null(_tmp3C5->topt);goto _LL210;case 27: _LL20F: _tmp3C9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3B8)->f1;_tmp3C8=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3B8)->f2;_LL210:
# 1959
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3CE)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3C9);{
void*_tmp3B9=Cyc_Toc_typ_to_c(_tmp3C8);
*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3BA[2];_tmp3BA[1]=
Cyc_Absyn_sizeoftyp_exp(_tmp3B9,0);_tmp3BA[0]=_tmp3C9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3BA,sizeof(struct Cyc_Absyn_Exp*),2));}),0),
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3BB;(_tmp3BB.Int_c).val=({struct _tuple6 _tmp3BC;_tmp3BC.f1=Cyc_Absyn_Unsigned;_tmp3BC.f2=(int)sizeof(double);_tmp3BC;});(_tmp3BB.Int_c).tag=5;_tmp3BB;}),0),0);};}
# 1967
goto _LL20A;case 29: _LL211: _tmp3CB=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3B8)->f1;_tmp3CA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3B8)->f2;_LL212:
# 1969
 _tmp384=Cyc_Toc_init_anon_struct(nv,lval,_tmp3CB,_tmp3CA,_tmp384);goto _LL20A;default: _LL213: _LL214: {
# 1971
void*old_e_typ=(void*)_check_null(_tmp3CF->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3CF);
{void*_tmp3BD=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3BD)->tag == 11){_LL216: _LL217:
# 1978
 if(old_e_typ != _tmp3CF->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3CF->topt))){
# 1982
ad=Cyc_Toc_update_aggr_type(ad,_tmp3CE,(void*)_check_null(_tmp3CF->topt));
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3BF;_tmp3BF.tag=11;_tmp3BF.f1=({struct Cyc_Absyn_AggrInfo _tmp3C0;_tmp3C0.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1[0]=ad;_tmp3C1;}));_tmp3C0.targs=0;_tmp3C0;});_tmp3BF;});_tmp3BE;});
# 1986
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1988
goto _LL215;}else{_LL218: _LL219:
 goto _LL215;}_LL215:;}{
# 1991
struct Cyc_Absyn_Aggrfield*_tmp3C2=Cyc_Absyn_lookup_field(aggrfields,_tmp3CE);
# 1993
if(Cyc_Toc_is_poly_field(struct_type,_tmp3CE) && !was_ptr_type)
_tmp3CF=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3CF);
# 1996
if(exist_types != 0)
_tmp3CF=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3C2))->type),_tmp3CF);
# 1999
_tmp384=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3CF,0),0),_tmp384,0);
goto _LL20A;};}}_LL20A:;}
# 2002
goto _LL205;};}}else{_LL208: _LL209:
({void*_tmp3CC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3CD="array designator in struct";_tag_dyneither(_tmp3CD,sizeof(char),27);}),_tag_dyneither(_tmp3CC,sizeof(void*),0));});}_LL205:;}}}
# 2009
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2014
struct Cyc_Absyn_Exp*_tmp3D1=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp384,pointer,rgnopt,is_atomic,do_declare);
# 2016
_tmp3D1->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3D2=_tmp3D1;_npop_handler(0);return _tmp3D2;};};
# 1907
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2022
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2025
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2028
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3[0]=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3D4;_tmp3D4.tag=4;_tmp3D4.f1=e1;_tmp3D4.f2=incr;_tmp3D4;});_tmp3D3;}),0);}
# 2032
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2041
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2043
void*_tmp3D5=e1->r;void*_tmp3D6=_tmp3D5;struct Cyc_Absyn_Exp*_tmp3DE;struct _dyneither_ptr*_tmp3DD;int _tmp3DC;int _tmp3DB;void*_tmp3DA;struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3D8;switch(*((int*)_tmp3D6)){case 36: _LL21B: _tmp3D8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_LL21C:
 Cyc_Toc_lvalue_assign_stmt(_tmp3D8,fs,f,f_env);goto _LL21A;case 13: _LL21D: _tmp3DA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3D9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_LL21E:
 Cyc_Toc_lvalue_assign(_tmp3D9,fs,f,f_env);goto _LL21A;case 20: _LL21F: _tmp3DE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3DD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_tmp3DC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D6)->f3;_tmp3DB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D6)->f4;_LL220:
# 2048
 e1->r=_tmp3DE->r;
Cyc_Toc_lvalue_assign(e1,({struct Cyc_List_List*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->hd=_tmp3DD;_tmp3D7->tl=fs;_tmp3D7;}),f,f_env);
goto _LL21A;default: _LL221: _LL222: {
# 2056
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2058
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL21A;}}_LL21A:;}
# 2064
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2066
void*_tmp3DF=s->r;void*_tmp3E0=_tmp3DF;struct Cyc_Absyn_Stmt*_tmp3E7;struct Cyc_Absyn_Decl*_tmp3E6;struct Cyc_Absyn_Stmt*_tmp3E5;struct Cyc_Absyn_Exp*_tmp3E4;switch(*((int*)_tmp3E0)){case 1: _LL224: _tmp3E4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3E0)->f1;_LL225:
 Cyc_Toc_lvalue_assign(_tmp3E4,fs,f,f_env);goto _LL223;case 12: _LL226: _tmp3E6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3E0)->f1;_tmp3E5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3E0)->f2;_LL227:
# 2069
 Cyc_Toc_lvalue_assign_stmt(_tmp3E5,fs,f,f_env);goto _LL223;case 2: _LL228: _tmp3E7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3E0)->f2;_LL229:
 Cyc_Toc_lvalue_assign_stmt(_tmp3E7,fs,f,f_env);goto _LL223;default: _LL22A: _LL22B:
({struct Cyc_String_pa_PrintArg_struct _tmp3E3;_tmp3E3.tag=0;_tmp3E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3E1[1]={& _tmp3E3};Cyc_Toc_toc_impos(({const char*_tmp3E2="lvalue_assign_stmt: %s";_tag_dyneither(_tmp3E2,sizeof(char),23);}),_tag_dyneither(_tmp3E1,sizeof(void*),1));});});}_LL223:;}
# 2075
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2079
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3E8=e->r;void*_tmp3E9=_tmp3E8;struct Cyc_Absyn_Stmt*_tmp3F0;struct Cyc_Absyn_Exp*_tmp3EF;void**_tmp3EE;struct Cyc_Absyn_Exp**_tmp3ED;switch(*((int*)_tmp3E9)){case 13: _LL22D: _tmp3EE=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_tmp3ED=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E9)->f2;_LL22E:
# 2082
*_tmp3ED=Cyc_Toc_push_address_exp(*_tmp3ED);
*_tmp3EE=Cyc_Absyn_cstar_typ(*_tmp3EE,Cyc_Toc_mt_tq);
return e;case 19: _LL22F: _tmp3EF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_LL230:
# 2086
 return _tmp3EF;case 36: _LL231: _tmp3F0=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_LL232:
# 2090
 Cyc_Toc_push_address_stmt(_tmp3F0);
return e;default: _LL233: _LL234:
# 2093
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);
({struct Cyc_String_pa_PrintArg_struct _tmp3EC;_tmp3EC.tag=0;_tmp3EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp3EA[1]={& _tmp3EC};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3EB="can't take & of exp %s";_tag_dyneither(_tmp3EB,sizeof(char),23);}),_tag_dyneither(_tmp3EA,sizeof(void*),1));});});}_LL22C:;}
# 2098
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3F1=s->r;void*_tmp3F2=_tmp3F1;struct Cyc_Absyn_Exp**_tmp3F8;struct Cyc_Absyn_Stmt*_tmp3F7;struct Cyc_Absyn_Stmt*_tmp3F6;switch(*((int*)_tmp3F2)){case 2: _LL236: _tmp3F6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3F2)->f2;_LL237:
 _tmp3F7=_tmp3F6;goto _LL239;case 12: _LL238: _tmp3F7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F2)->f2;_LL239:
 Cyc_Toc_push_address_stmt(_tmp3F7);goto _LL235;case 1: _LL23A: _tmp3F8=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3F2)->f1;_LL23B:
*_tmp3F8=Cyc_Toc_push_address_exp(*_tmp3F8);goto _LL235;default: _LL23C: _LL23D:
# 2104
({struct Cyc_String_pa_PrintArg_struct _tmp3F5;_tmp3F5.tag=0;_tmp3F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3F3[1]={& _tmp3F5};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3F4="can't take & of stmt %s";_tag_dyneither(_tmp3F4,sizeof(char),24);}),_tag_dyneither(_tmp3F3,sizeof(void*),1));});});}_LL235:;}
# 2109
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2111
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2113
if(x == 0)return 0;
result=({struct Cyc_List_List*_tmp3F9=_region_malloc(r2,sizeof(*_tmp3F9));_tmp3F9->hd=f(x->hd,env);_tmp3F9->tl=0;_tmp3F9;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmp3FA=_region_malloc(r2,sizeof(*_tmp3FA));_tmp3FA->hd=f(x->hd,env);_tmp3FA->tl=0;_tmp3FA;});
prev=prev->tl;}
# 2120
return result;}
# 2122
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2126
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->f1=0;_tmp3FB->f2=e;_tmp3FB;});}
# 2130
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3FC=Cyc_Tcutil_compress(t);void*_tmp3FD=_tmp3FC;struct Cyc_Absyn_PtrInfo _tmp400;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FD)->tag == 5){_LL23F: _tmp400=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FD)->f1;_LL240:
 return _tmp400;}else{_LL241: _LL242:
({void*_tmp3FE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3FF="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3FF,sizeof(char),28);}),_tag_dyneither(_tmp3FE,sizeof(void*),0));});}_LL23E:;}
# 2140
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp401=Cyc_Tcutil_compress(t);void*_tmp402=_tmp401;int _tmp40E;enum Cyc_Absyn_Sign _tmp40D;enum Cyc_Absyn_Sign _tmp40C;enum Cyc_Absyn_Sign _tmp40B;enum Cyc_Absyn_Sign _tmp40A;enum Cyc_Absyn_Sign _tmp409;switch(*((int*)_tmp402)){case 5: _LL244: _LL245:
 res=Cyc_Absyn_null_exp(0);goto _LL243;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp402)->f2){case Cyc_Absyn_Char_sz: _LL246: _tmp409=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp402)->f1;_LL247:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp409,'\000'),0);goto _LL243;case Cyc_Absyn_Short_sz: _LL248: _tmp40A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp402)->f1;_LL249:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp40A,0),0);goto _LL243;case Cyc_Absyn_Int_sz: _LL24E: _tmp40B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp402)->f1;_LL24F:
# 2148
 _tmp40C=_tmp40B;goto _LL251;case Cyc_Absyn_Long_sz: _LL250: _tmp40C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp402)->f1;_LL251:
# 2150
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp40C,0),0);goto _LL243;default: _LL252: _tmp40D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp402)->f1;_LL253:
# 2152
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp40D,(long long)0),0);goto _LL243;}case 13: _LL24A: _LL24B:
# 2146
 goto _LL24D;case 14: _LL24C: _LL24D:
 _tmp40B=Cyc_Absyn_Unsigned;goto _LL24F;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp402)->f1){case 0: _LL254: _LL255:
# 2153
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp403="0.0F";_tag_dyneither(_tmp403,sizeof(char),5);}),0),0);goto _LL243;case 1: _LL256: _LL257:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp404="0.0";_tag_dyneither(_tmp404,sizeof(char),4);}),1),0);goto _LL243;default: _LL258: _tmp40E=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp402)->f1;_LL259:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp405="0.0L";_tag_dyneither(_tmp405,sizeof(char),5);}),_tmp40E),0);goto _LL243;}default: _LL25A: _LL25B:
# 2157
({struct Cyc_String_pa_PrintArg_struct _tmp408;_tmp408.tag=0;_tmp408.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp406[1]={& _tmp408};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp407="found non-zero type %s in generate_zero";_tag_dyneither(_tmp407,sizeof(char),40);}),_tag_dyneither(_tmp406,sizeof(void*),1));});});}_LL243:;}
# 2159
res->topt=t;
return res;}
# 2166
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2179 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp40F=Cyc_Toc_typ_to_c(elt_type);
void*_tmp410=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp411=Cyc_Absyn_cstar_typ(_tmp40F,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp412=({struct Cyc_Core_Opt*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->v=_tmp411;_tmp449;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp413=e1->r;void*_tmp414=_tmp413;struct Cyc_Absyn_Exp*_tmp41A;struct Cyc_Absyn_Exp*_tmp419;struct Cyc_Absyn_Exp*_tmp418;switch(*((int*)_tmp414)){case 19: _LL25D: _tmp418=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp414)->f1;_LL25E:
# 2187
 if(!is_dyneither){
_tmp418=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp418,0,Cyc_Absyn_Other_coercion,0);
_tmp418->topt=fat_ptr_type;}
# 2191
Cyc_Toc_exp_to_c(nv,_tmp418);xinit=_tmp418;goto _LL25C;case 22: _LL25F: _tmp41A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp414)->f1;_tmp419=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp414)->f2;_LL260:
# 2193
 if(!is_dyneither){
_tmp41A=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp41A,0,Cyc_Absyn_Other_coercion,0);
_tmp41A->topt=fat_ptr_type;}
# 2197
Cyc_Toc_exp_to_c(nv,_tmp41A);Cyc_Toc_exp_to_c(nv,_tmp419);
xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp415[3];_tmp415[2]=_tmp419;_tmp415[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp415[0]=_tmp41A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp415,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
goto _LL25C;default: _LL261: _LL262:
({void*_tmp416=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp417="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp417,sizeof(char),53);}),_tag_dyneither(_tmp416,sizeof(void*),0));});}_LL25C:;}{
# 2203
struct _tuple1*_tmp41B=Cyc_Toc_temp_var();
struct _RegionHandle _tmp41C=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp41C;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp41D=({struct Cyc_Absyn_Vardecl*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->sc=Cyc_Absyn_Public;_tmp448->name=_tmp41B;_tmp448->tq=Cyc_Toc_mt_tq;_tmp448->type=_tmp410;_tmp448->initializer=xinit;_tmp448->rgn=0;_tmp448->attributes=0;_tmp448->escapes=0;_tmp448;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp41E=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp447;_tmp447.tag=4;_tmp447.f1=_tmp41D;_tmp447;});_tmp446;});
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_varb_exp((void*)_tmp41E,0);
_tmp41F->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp420=Cyc_Absyn_deref_exp(_tmp41F,0);
_tmp420->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp420);{
struct _tuple1*_tmp421=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp422=({struct Cyc_Absyn_Vardecl*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->sc=Cyc_Absyn_Public;_tmp445->name=_tmp421;_tmp445->tq=Cyc_Toc_mt_tq;_tmp445->type=_tmp40F;_tmp445->initializer=_tmp420;_tmp445->rgn=0;_tmp445->attributes=0;_tmp445->escapes=0;_tmp445;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp423=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp444;_tmp444.tag=4;_tmp444.f1=_tmp422;_tmp444;});_tmp443;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp424=Cyc_Absyn_varb_exp((void*)_tmp423,0);
_tmp424->topt=_tmp420->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp424,e2,0);
z_init->topt=_tmp424->topt;}
# 2222
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp425=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp426=({struct Cyc_Absyn_Vardecl*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->sc=Cyc_Absyn_Public;_tmp442->name=_tmp425;_tmp442->tq=Cyc_Toc_mt_tq;_tmp442->type=_tmp40F;_tmp442->initializer=z_init;_tmp442->rgn=0;_tmp442->attributes=0;_tmp442->escapes=0;_tmp442;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp427=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp441;_tmp441.tag=4;_tmp441.f1=_tmp426;_tmp441;});_tmp440;});
# 2228
struct Cyc_Absyn_Exp*_tmp428=Cyc_Absyn_varb_exp((void*)_tmp423,0);_tmp428->topt=_tmp420->topt;{
struct Cyc_Absyn_Exp*_tmp429=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp428,_tmp429,0);
_tmp42A->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp42A);{
# 2234
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_varb_exp((void*)_tmp427,0);_tmp42B->topt=_tmp420->topt;{
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp42D=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp42B,_tmp42C,0);
_tmp42D->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp42D);{
# 2240
struct Cyc_List_List*_tmp42E=({struct Cyc_Absyn_Exp*_tmp43F[2];_tmp43F[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp43F[0]=
# 2240
Cyc_Absyn_varb_exp((void*)_tmp41E,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp43F,sizeof(struct Cyc_Absyn_Exp*),2));});
# 2242
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp42E,0),_tmp42F,0);{
# 2247
struct Cyc_Absyn_Exp*_tmp430=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp42A,_tmp42D,0),0);
struct Cyc_Absyn_Stmt*_tmp431=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp432=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp((void*)_tmp41E,0),Cyc_Toc_curr_sp,0);
_tmp432=Cyc_Toc_cast_it(_tmp411,_tmp432);{
struct Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_deref_exp(_tmp432,0);
struct Cyc_Absyn_Exp*_tmp434=Cyc_Absyn_assign_exp(_tmp433,Cyc_Absyn_var_exp(_tmp425,0),0);
struct Cyc_Absyn_Stmt*_tmp435=Cyc_Absyn_exp_stmt(_tmp434,0);
_tmp435=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp430,_tmp431,Cyc_Absyn_skip_stmt(0),0),_tmp435,0);
_tmp435=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp438;_tmp438.tag=0;_tmp438.f1=_tmp426;_tmp438;});_tmp437;});_tmp436->loc=0;_tmp436;}),_tmp435,0);
_tmp435=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp43B;_tmp43B.tag=0;_tmp43B.f1=_tmp422;_tmp43B;});_tmp43A;});_tmp439->loc=0;_tmp439;}),_tmp435,0);
_tmp435=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp43E;_tmp43E.tag=0;_tmp43E.f1=_tmp41D;_tmp43E;});_tmp43D;});_tmp43C->loc=0;_tmp43C;}),_tmp435,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp435);};};};};};};};};};}
# 2205
;_pop_region(rgn2);};}
# 2273 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2277
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp44A=Cyc_Tcutil_compress(aggrtype);void*_tmp44B=_tmp44A;union Cyc_Absyn_AggrInfoU _tmp450;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44B)->tag == 11){_LL264: _tmp450=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44B)->f1).aggr_info;_LL265:
# 2280
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp450);goto _LL263;}else{_LL266: _LL267:
({void*_tmp44C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp44F;_tmp44F.tag=0;_tmp44F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));({void*_tmp44D[1]={& _tmp44F};Cyc_aprintf(({const char*_tmp44E="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp44E,sizeof(char),51);}),_tag_dyneither(_tmp44D,sizeof(void*),1));});}),_tag_dyneither(_tmp44C,sizeof(void*),0));});}_LL263:;}{
# 2284
struct _tuple1*_tmp451=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_var_exp(_tmp451,0);
struct Cyc_Absyn_Exp*_tmp453=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp454=Cyc_Absyn_aggrarrow_exp(_tmp452,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp455=Cyc_Absyn_neq_exp(_tmp454,_tmp453,0);
struct Cyc_Absyn_Exp*_tmp456=Cyc_Absyn_aggrarrow_exp(_tmp452,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp457=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp456,0),0);
struct Cyc_Absyn_Stmt*_tmp458=Cyc_Absyn_ifthenelse_stmt(_tmp455,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp459=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp45A=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp45B=Cyc_Absyn_declare_stmt(_tmp451,_tmp459,_tmp45A,Cyc_Absyn_seq_stmt(_tmp458,_tmp457,0),0);
return Cyc_Toc_stmt_exp_r(_tmp45B);}else{
# 2298
struct Cyc_Absyn_Exp*_tmp45C=Cyc_Toc_member_exp(aggrproj(_tmp452,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp45D=Cyc_Absyn_neq_exp(_tmp45C,_tmp453,0);
struct Cyc_Absyn_Exp*_tmp45E=Cyc_Toc_member_exp(aggrproj(_tmp452,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp45F=Cyc_Absyn_exp_stmt(_tmp45E,0);
struct Cyc_Absyn_Stmt*_tmp460=Cyc_Absyn_ifthenelse_stmt(_tmp45D,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp461=Cyc_Absyn_declare_stmt(_tmp451,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp460,_tmp45F,0),0);
return Cyc_Toc_stmt_exp_r(_tmp461);}};}
# 2308
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2311
void*_tmp462=Cyc_Tcutil_compress(t);void*_tmp463=_tmp462;union Cyc_Absyn_AggrInfoU _tmp46A;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp463)->tag == 11){_LL269: _tmp46A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp463)->f1).aggr_info;_LL26A: {
# 2313
struct Cyc_Absyn_Aggrdecl*_tmp464=Cyc_Absyn_get_known_aggrdecl(_tmp46A);
*f_tag=Cyc_Toc_get_member_offset(_tmp464,f);{
# 2316
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp469;_tmp469.tag=0;_tmp469.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp468;_tmp468.tag=0;_tmp468.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp464->name).f2);({void*_tmp466[2]={& _tmp468,& _tmp469};Cyc_aprintf(({const char*_tmp467="_union_%s_%s";_tag_dyneither(_tmp467,sizeof(char),13);}),_tag_dyneither(_tmp466,sizeof(void*),2));});});});
*tagged_member_type=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465[0]=str;_tmp465;}));
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp464->impl))->tagged;};}}else{_LL26B: _LL26C:
 return 0;}_LL268:;}
# 2327
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2330
void*_tmp46B=e->r;void*_tmp46C=_tmp46B;struct Cyc_Absyn_Exp*_tmp478;struct _dyneither_ptr*_tmp477;int*_tmp476;struct Cyc_Absyn_Exp*_tmp475;struct _dyneither_ptr*_tmp474;int*_tmp473;struct Cyc_Absyn_Exp*_tmp472;switch(*((int*)_tmp46C)){case 13: _LL26E: _tmp472=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL26F:
({void*_tmp46D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp46E="cast on lhs!";_tag_dyneither(_tmp46E,sizeof(char),13);}),_tag_dyneither(_tmp46D,sizeof(void*),0));});case 20: _LL270: _tmp475=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp474=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp473=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL271:
# 2333
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp475->topt),_tmp474,f_tag,tagged_member_type,clear_read,_tmp473);case 21: _LL272: _tmp478=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp477=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp476=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL273: {
# 2336
void*_tmp46F=Cyc_Tcutil_compress((void*)_check_null(_tmp478->topt));void*_tmp470=_tmp46F;void*_tmp471;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp470)->tag == 5){_LL277: _tmp471=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp470)->f1).elt_typ;_LL278:
# 2338
 return Cyc_Toc_is_tagged_union_project_impl(_tmp471,_tmp477,f_tag,tagged_member_type,clear_read,_tmp476);}else{_LL279: _LL27A:
# 2340
 return 0;}_LL276:;}default: _LL274: _LL275:
# 2342
 return 0;}_LL26D:;}
# 2355 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2359
struct _tuple1*_tmp479=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp479,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2369
struct Cyc_Absyn_Exp*_tmp47A=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp47A,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2372
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp479,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2378
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2393 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp47B=Cyc_Absyn_var_exp(max,0);
# 2401
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp47B=Cyc_Absyn_add_exp(_tmp47B,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2412
s=Cyc_Absyn_skip_stmt(0);}}{
# 2414
struct _RegionHandle _tmp47C=_new_region("r");struct _RegionHandle*r=& _tmp47C;_push_region(r);
{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp490=_region_malloc(r,sizeof(*_tmp490));_tmp490->hd=({struct _tuple28*_tmp491=_region_malloc(r,sizeof(*_tmp491));_tmp491->f1=max;_tmp491->f2=Cyc_Absyn_uint_typ;_tmp491->f3=e1;_tmp491;});_tmp490->tl=0;_tmp490;});
# 2417
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp47D[2];_tmp47D[1]=_tmp47B;_tmp47D[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp47D,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{
# 2424
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp47E[2];_tmp47E[1]=_tmp47B;_tmp47E[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp47E,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{
# 2430
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
decls=({struct Cyc_List_List*_tmp47F=_region_malloc(r,sizeof(*_tmp47F));_tmp47F->hd=({struct _tuple28*_tmp480=_region_malloc(r,sizeof(*_tmp480));_tmp480->f1=a;_tmp480->f2=ptr_typ;_tmp480->f3=ainit;_tmp480;});_tmp47F->tl=decls;_tmp47F;});
if(!Cyc_Tcutil_is_pointer_type(old_typ))
({void*_tmp481=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp482="new_comprehension_to_c: comprehension is not an array type";_tag_dyneither(_tmp482,sizeof(char),59);}),_tag_dyneither(_tmp481,sizeof(void*),0));});
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp483=Cyc_Toc_temp_var();
void*_tmp484=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp485=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp486=
Cyc_Absyn_fncall_exp(_tmp485,({struct Cyc_Absyn_Exp*_tmp489[3];_tmp489[2]=_tmp47B;_tmp489[1]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp489[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp489,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 2442
decls=({struct Cyc_List_List*_tmp487=_region_malloc(r,sizeof(*_tmp487));_tmp487->hd=({struct _tuple28*_tmp488=_region_malloc(r,sizeof(*_tmp488));_tmp488->f1=_tmp483;_tmp488->f2=_tmp484;_tmp488->f3=_tmp486;_tmp488;});_tmp487->tl=decls;_tmp487;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp483,0),0),0);}else{
# 2445
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp48A=decls;for(0;_tmp48A != 0;_tmp48A=_tmp48A->tl){
struct _tuple28 _tmp48B=*((struct _tuple28*)_tmp48A->hd);struct _tuple28 _tmp48C=_tmp48B;struct _tuple1*_tmp48F;void*_tmp48E;struct Cyc_Absyn_Exp*_tmp48D;_LL27C: _tmp48F=_tmp48C.f1;_tmp48E=_tmp48C.f2;_tmp48D=_tmp48C.f3;_LL27D:;
s=Cyc_Absyn_declare_stmt(_tmp48F,_tmp48E,_tmp48D,s,0);}}
# 2450
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2415
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2454
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp492=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp495;_tmp495.tag=0;_tmp495.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp493[1]={& _tmp495};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp494="exp_to_c: no type for %s";_tag_dyneither(_tmp494,sizeof(char),25);}),_tag_dyneither(_tmp493,sizeof(void*),1));});});
# 2460
if((nv->struct_info).lhs_exp != 0){
void*_tmp496=_tmp492;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp496)->tag == 28){_LL27F: _LL280:
 goto _LL27E;}else{_LL281: _LL282:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}_LL27E:;}{
# 2468
void*old_typ=(void*)_check_null(e->topt);
void*_tmp497=_tmp492;struct Cyc_Absyn_Stmt*_tmp6B2;struct Cyc_Absyn_Exp*_tmp6B1;struct _dyneither_ptr*_tmp6B0;struct Cyc_Absyn_Exp*_tmp6AF;struct Cyc_Absyn_Exp*_tmp6AE;int _tmp6AD;struct Cyc_Absyn_Exp*_tmp6AC;void**_tmp6AB;struct Cyc_Absyn_Exp*_tmp6AA;int _tmp6A9;int _tmp6A8;struct Cyc_List_List*_tmp6A7;struct Cyc_Absyn_Datatypedecl*_tmp6A6;struct Cyc_Absyn_Datatypefield*_tmp6A5;void*_tmp6A4;struct Cyc_List_List*_tmp6A3;struct _tuple1*_tmp6A2;struct Cyc_List_List*_tmp6A1;struct Cyc_List_List*_tmp6A0;struct Cyc_Absyn_Aggrdecl*_tmp69F;struct Cyc_Absyn_Exp*_tmp69E;void*_tmp69D;int _tmp69C;struct Cyc_Absyn_Vardecl*_tmp69B;struct Cyc_Absyn_Exp*_tmp69A;struct Cyc_Absyn_Exp*_tmp699;int _tmp698;struct Cyc_List_List*_tmp697;struct Cyc_List_List*_tmp696;struct Cyc_Absyn_Exp*_tmp695;struct Cyc_Absyn_Exp*_tmp694;struct Cyc_Absyn_Exp*_tmp693;struct _dyneither_ptr*_tmp692;int _tmp691;int _tmp690;struct Cyc_Absyn_Exp*_tmp68F;struct _dyneither_ptr*_tmp68E;int _tmp68D;int _tmp68C;struct Cyc_Absyn_Exp*_tmp68B;void*_tmp68A;struct Cyc_List_List*_tmp689;void*_tmp688;struct Cyc_Absyn_Exp*_tmp687;struct Cyc_Absyn_Exp*_tmp686;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_Absyn_Exp*_tmp684;void**_tmp683;struct Cyc_Absyn_Exp*_tmp682;int _tmp681;enum Cyc_Absyn_Coercion _tmp680;struct Cyc_Absyn_Exp*_tmp67F;struct Cyc_List_List*_tmp67E;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_Absyn_Exp*_tmp67C;int _tmp67B;struct Cyc_Absyn_Exp*_tmp67A;struct Cyc_List_List*_tmp679;int _tmp678;struct Cyc_List_List*_tmp677;struct Cyc_Absyn_VarargInfo*_tmp676;struct Cyc_Absyn_Exp*_tmp675;struct Cyc_List_List*_tmp674;struct Cyc_Absyn_Exp*_tmp673;struct Cyc_Absyn_Exp*_tmp672;struct Cyc_Absyn_Exp*_tmp671;struct Cyc_Absyn_Exp*_tmp670;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_Absyn_Exp*_tmp66D;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp*_tmp66B;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_Core_Opt*_tmp669;struct Cyc_Absyn_Exp*_tmp668;struct Cyc_Absyn_Exp*_tmp667;enum Cyc_Absyn_Incrementor _tmp666;enum Cyc_Absyn_Primop _tmp665;struct Cyc_List_List*_tmp664;switch(*((int*)_tmp497)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp497)->f1).Null_c).tag == 1){_LL284: _LL285: {
# 2475
struct Cyc_Absyn_Exp*_tmp498=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp498,_tmp498))->r;else{
# 2480
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp499[3];_tmp499[2]=_tmp498;_tmp499[1]=_tmp498;_tmp499[0]=_tmp498;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp499,sizeof(struct Cyc_Absyn_Exp*),3));}));}}else{
# 2482
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2484
goto _LL283;}}else{_LL286: _LL287:
 goto _LL283;}case 1: _LL288: _LL289:
 goto _LL283;case 2: _LL28A: _tmp665=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp664=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL28B: {
# 2489
struct Cyc_List_List*_tmp49A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp664);
# 2491
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp664);
{enum Cyc_Absyn_Primop _tmp49B=_tmp665;switch(_tmp49B){case Cyc_Absyn_Numelts: _LL2D9: _LL2DA: {
# 2494
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp664))->hd;
{void*_tmp49C=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp49D=_tmp49C;void*_tmp4AF;union Cyc_Absyn_Constraint*_tmp4AE;union Cyc_Absyn_Constraint*_tmp4AD;union Cyc_Absyn_Constraint*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;switch(*((int*)_tmp49D)){case 8: _LL2EE: _tmp4AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp49D)->f1).num_elts;_LL2EF:
# 2498
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp4AB)))
({void*_tmp49E=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp49F="can't calculate numelts";_tag_dyneither(_tmp49F,sizeof(char),24);}),_tag_dyneither(_tmp49E,sizeof(void*),0));});
e->r=_tmp4AB->r;goto _LL2ED;case 5: _LL2F0: _tmp4AF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49D)->f1).elt_typ;_tmp4AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49D)->f1).ptr_atts).nullable;_tmp4AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49D)->f1).ptr_atts).bounds;_tmp4AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49D)->f1).ptr_atts).zero_term;_LL2F1:
# 2502
{void*_tmp4A0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4AD);void*_tmp4A1=_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A6;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4A1)->tag == 0){_LL2F5: _LL2F6:
# 2504
 e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*_tmp4A2[2];_tmp4A2[1]=
# 2506
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4AF),0);_tmp4A2[0]=(struct Cyc_Absyn_Exp*)_tmp664->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A2,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL2F4;}else{_LL2F7: _tmp4A6=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4A1)->f1;_LL2F8:
# 2510
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4AC)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp664->hd);
# 2514
e->r=Cyc_Toc_fncall_exp_r(function_e,({struct Cyc_Absyn_Exp*_tmp4A3[2];_tmp4A3[1]=_tmp4A6;_tmp4A3[0]=(struct Cyc_Absyn_Exp*)_tmp664->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A3,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4AE)){
if(!Cyc_Evexp_c_can_eval(_tmp4A6))
({void*_tmp4A4=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4A5="can't calculate numelts";_tag_dyneither(_tmp4A5,sizeof(char),24);}),_tag_dyneither(_tmp4A4,sizeof(void*),0));});
# 2519
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp4A6,Cyc_Absyn_uint_exp(0,0));}else{
# 2521
e->r=_tmp4A6->r;goto _LL2F4;}}
# 2523
goto _LL2F4;}_LL2F4:;}
# 2525
goto _LL2ED;default: _LL2F2: _LL2F3:
# 2527
({struct Cyc_String_pa_PrintArg_struct _tmp4AA;_tmp4AA.tag=0;_tmp4AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp4A9;_tmp4A9.tag=0;_tmp4A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({void*_tmp4A7[2]={& _tmp4A9,& _tmp4AA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4A8="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp4A8,sizeof(char),41);}),_tag_dyneither(_tmp4A7,sizeof(void*),2));});});});}_LL2ED:;}
# 2530
goto _LL2D8;}case Cyc_Absyn_Plus: _LL2DB: _LL2DC:
# 2535
{void*_tmp4B0=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp49A))->hd);void*_tmp4B1=_tmp4B0;void*_tmp4B9;union Cyc_Absyn_Constraint*_tmp4B8;union Cyc_Absyn_Constraint*_tmp4B7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B1)->tag == 5){_LL2FA: _tmp4B9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B1)->f1).elt_typ;_tmp4B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B1)->f1).ptr_atts).bounds;_tmp4B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B1)->f1).ptr_atts).zero_term;_LL2FB:
# 2537
{void*_tmp4B2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4B8);void*_tmp4B3=_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B6;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4B3)->tag == 0){_LL2FF: _LL300: {
# 2539
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp664))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp664->tl))->hd;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4B4[3];_tmp4B4[2]=e2;_tmp4B4[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4B9),0);_tmp4B4[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B4,sizeof(struct Cyc_Absyn_Exp*),3));}));
goto _LL2FE;}}else{_LL301: _tmp4B6=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4B3)->f1;_LL302:
# 2545
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4B7)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp664))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp664->tl))->hd;
e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*_tmp4B5[3];_tmp4B5[2]=e2;_tmp4B5[1]=_tmp4B6;_tmp4B5[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B5,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r;}
# 2550
goto _LL2FE;}_LL2FE:;}
# 2552
goto _LL2F9;}else{_LL2FC: _LL2FD:
# 2554
 goto _LL2F9;}_LL2F9:;}
# 2556
goto _LL2D8;case Cyc_Absyn_Minus: _LL2DD: _LL2DE: {
# 2561
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp49A))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp664))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp664->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp49A->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2571
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4BA[3];_tmp4BA[2]=
# 2573
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0);_tmp4BA[1]=
# 2572
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4BA[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BA,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 2576
goto _LL2D8;}case Cyc_Absyn_Eq: _LL2DF: _LL2E0:
 goto _LL2E2;case Cyc_Absyn_Neq: _LL2E1: _LL2E2:
 goto _LL2E4;case Cyc_Absyn_Gt: _LL2E3: _LL2E4:
 goto _LL2E6;case Cyc_Absyn_Gte: _LL2E5: _LL2E6:
 goto _LL2E8;case Cyc_Absyn_Lt: _LL2E7: _LL2E8:
 goto _LL2EA;case Cyc_Absyn_Lte: _LL2E9: _LL2EA: {
# 2584
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp664))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp664->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp49A))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp49A->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
goto _LL2D8;}default: _LL2EB: _LL2EC:
 goto _LL2D8;}_LL2D8:;}
# 2596
goto _LL283;}case 4: _LL28C: _tmp667=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp666=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL28D: {
# 2598
void*e2_cyc_typ=(void*)_check_null(_tmp667->topt);
# 2607 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp4D1="increment";_tag_dyneither(_tmp4D1,sizeof(char),10);});
if(_tmp666 == Cyc_Absyn_PreDec  || _tmp666 == Cyc_Absyn_PostDec)incr_str=({const char*_tmp4BB="decrement";_tag_dyneither(_tmp4BB,sizeof(char),10);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp667,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp4BE;_tmp4BE.tag=0;_tmp4BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4BC[1]={& _tmp4BE};Cyc_Tcutil_terr(e->loc,({const char*_tmp4BD="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp4BD,sizeof(char),74);}),_tag_dyneither(_tmp4BC,sizeof(void*),1));});});
({void*_tmp4BF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4C0="in-place inc/dec on zero-term";_tag_dyneither(_tmp4C0,sizeof(char),30);}),_tag_dyneither(_tmp4BF,sizeof(void*),0));});}{
# 2616
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp667,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4C1=Cyc_Absyn_signed_int_exp(1,0);
_tmp4C1->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4C2=_tmp666;switch(_tmp4C2){case Cyc_Absyn_PreInc: _LL304: _LL305:
# 2623
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4C4;_tmp4C4.tag=3;_tmp4C4.f1=_tmp667;_tmp4C4.f2=({struct Cyc_Core_Opt*_tmp4C5=_cycalloc_atomic(sizeof(*_tmp4C5));_tmp4C5->v=(void*)Cyc_Absyn_Plus;_tmp4C5;});_tmp4C4.f3=_tmp4C1;_tmp4C4;});_tmp4C3;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL306: _LL307:
# 2627
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4C7;_tmp4C7.tag=3;_tmp4C7.f1=_tmp667;_tmp4C7.f2=({struct Cyc_Core_Opt*_tmp4C8=_cycalloc_atomic(sizeof(*_tmp4C8));_tmp4C8->v=(void*)Cyc_Absyn_Minus;_tmp4C8;});_tmp4C7.f3=_tmp4C1;_tmp4C7;});_tmp4C6;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL308: _LL309:
# 2631
({struct Cyc_String_pa_PrintArg_struct _tmp4CB;_tmp4CB.tag=0;_tmp4CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4C9[1]={& _tmp4CB};Cyc_Tcutil_terr(e->loc,({const char*_tmp4CA="in-place post-%s is not supported on @tagged union members";_tag_dyneither(_tmp4CA,sizeof(char),59);}),_tag_dyneither(_tmp4C9,sizeof(void*),1));});});
# 2633
({void*_tmp4CC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4CD="in-place inc/dec on @tagged union";_tag_dyneither(_tmp4CD,sizeof(char),34);}),_tag_dyneither(_tmp4CC,sizeof(void*),0));});}_LL303:;};}
# 2636
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp667);
Cyc_Toc_set_lhs(nv,0);{
# 2641
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp666 == Cyc_Absyn_PostInc  || _tmp666 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2647
if(_tmp666 == Cyc_Absyn_PreDec  || _tmp666 == Cyc_Absyn_PostDec)
change=-1;
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4CE[3];_tmp4CE[2]=
# 2651
Cyc_Absyn_signed_int_exp(change,0);_tmp4CE[1]=
# 2650
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4CE[0]=
# 2649
Cyc_Toc_push_address_exp(_tmp667);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CE,sizeof(struct Cyc_Absyn_Exp*),3));}));}else{
# 2652
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2656
struct Cyc_Toc_functionSet*_tmp4CF=_tmp666 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2658
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp4CF,_tmp667);
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4D0[2];_tmp4D0[1]=
Cyc_Absyn_signed_int_exp(1,0);_tmp4D0[0]=
# 2659
Cyc_Toc_push_address_exp(_tmp667);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D0,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
# 2661
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp667)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp667,0,Cyc_Toc_incr_lvalue,_tmp666);
e->r=_tmp667->r;}}}
# 2665
goto _LL283;};};}case 3: _LL28E: _tmp66A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp669=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_tmp668=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp497)->f3;_LL28F: {
# 2684 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp66A->topt);
void*e2_old_typ=(void*)_check_null(_tmp668->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp66A,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp66A);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp668);
e->r=Cyc_Toc_tagged_union_assignop(_tmp66A,e1_old_typ,_tmp669,_tmp668,e2_old_typ,f_tag,tagged_member_struct_type);
# 2695
return;}{
# 2697
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp66A,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp66A,_tmp669,_tmp668,ptr_type,is_dyneither,elt_type);
# 2703
return;}{
# 2707
int e1_poly=Cyc_Toc_is_poly_project(_tmp66A);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp66A);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp668);{
# 2713
int done=0;
if(_tmp669 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp4D2=(enum Cyc_Absyn_Primop)_tmp669->v;enum Cyc_Absyn_Primop _tmp4D3=_tmp4D2;switch(_tmp4D3){case Cyc_Absyn_Plus: _LL30B: _LL30C:
 change=_tmp668;goto _LL30A;case Cyc_Absyn_Minus: _LL30D: _LL30E:
# 2721
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp668,0);goto _LL30A;default: _LL30F: _LL310:
({void*_tmp4D4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4D5="bad t ? pointer arithmetic";_tag_dyneither(_tmp4D5,sizeof(char),27);}),_tag_dyneither(_tmp4D4,sizeof(void*),0));});}_LL30A:;}
# 2724
done=1;{
# 2726
struct Cyc_Absyn_Exp*_tmp4D6=Cyc_Toc__dyneither_ptr_inplace_plus_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp4D6,({struct Cyc_Absyn_Exp*_tmp4D7[3];_tmp4D7[2]=change;_tmp4D7[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4D7[0]=
# 2727
Cyc_Toc_push_address_exp(_tmp66A);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D7,sizeof(struct Cyc_Absyn_Exp*),3));}));};}else{
# 2730
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2733
enum Cyc_Absyn_Primop _tmp4D8=(enum Cyc_Absyn_Primop)_tmp669->v;enum Cyc_Absyn_Primop _tmp4D9=_tmp4D8;if(_tmp4D9 == Cyc_Absyn_Plus){_LL312: _LL313:
# 2735
 done=1;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp66A),({struct Cyc_Absyn_Exp*_tmp4DA[2];_tmp4DA[1]=_tmp668;_tmp4DA[0]=_tmp66A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4DA,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL311;}else{_LL314: _LL315:
({void*_tmp4DB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4DC="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp4DC,sizeof(char),39);}),_tag_dyneither(_tmp4DB,sizeof(void*),0));});}_LL311:;}}}
# 2742
if(!done){
# 2744
if(e1_poly)
_tmp668->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp668->r,0));
# 2750
if(!Cyc_Absyn_is_lvalue(_tmp66A)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp66A,0,Cyc_Toc_assignop_lvalue,({struct _tuple27*_tmp4DD=_cycalloc(sizeof(struct _tuple27)* 1);_tmp4DD[0]=(struct _tuple27)({struct _tuple27 _tmp4DE;_tmp4DE.f1=_tmp669;_tmp4DE.f2=_tmp668;_tmp4DE;});_tmp4DD;}));
e->r=_tmp66A->r;}}
# 2755
goto _LL283;};};};}case 5: _LL290: _tmp66D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp66C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_tmp66B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp497)->f3;_LL291:
# 2757
 Cyc_Toc_exp_to_c(nv,_tmp66D);
Cyc_Toc_exp_to_c(nv,_tmp66C);
Cyc_Toc_exp_to_c(nv,_tmp66B);
goto _LL283;case 6: _LL292: _tmp66F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp66E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL293:
# 2762
 Cyc_Toc_exp_to_c(nv,_tmp66F);
Cyc_Toc_exp_to_c(nv,_tmp66E);
goto _LL283;case 7: _LL294: _tmp671=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp670=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL295:
# 2766
 Cyc_Toc_exp_to_c(nv,_tmp671);
Cyc_Toc_exp_to_c(nv,_tmp670);
goto _LL283;case 8: _LL296: _tmp673=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp672=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL297:
# 2770
 Cyc_Toc_exp_to_c(nv,_tmp673);
Cyc_Toc_exp_to_c(nv,_tmp672);
goto _LL283;case 9: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497)->f3 == 0){_LL298: _tmp675=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp674=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL299:
# 2774
 Cyc_Toc_exp_to_c(nv,_tmp675);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp674);
goto _LL283;}else{_LL29A: _tmp67A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp679=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_tmp678=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497)->f3)->num_varargs;_tmp677=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497)->f3)->injectors;_tmp676=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497)->f3)->vai;_LL29B: {
# 2786 "toc.cyc"
struct _RegionHandle _tmp4DF=_new_region("r");struct _RegionHandle*r=& _tmp4DF;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp678,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp676->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2794
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp679);
int num_normargs=num_args - _tmp678;
# 2798
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp679=_tmp679->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp679))->hd);
new_args=({struct Cyc_List_List*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->hd=(struct Cyc_Absyn_Exp*)_tmp679->hd;_tmp4E0->tl=new_args;_tmp4E0;});}}
# 2803
new_args=({struct Cyc_List_List*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp4E2[3];_tmp4E2[2]=num_varargs_exp;_tmp4E2[1]=
# 2805
Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp4E2[0]=argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E2,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp4E1->tl=new_args;_tmp4E1;});
# 2808
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2810
Cyc_Toc_exp_to_c(nv,_tmp67A);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp67A,new_args,0),0);
# 2814
if(_tmp676->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp4E3=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp676->type));void*_tmp4E4=_tmp4E3;struct Cyc_Absyn_Datatypedecl*_tmp4E7;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E4)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E4)->f1).datatype_info).KnownDatatype).tag == 2){_LL317: _tmp4E7=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E4)->f1).datatype_info).KnownDatatype).val;_LL318:
 tud=_tmp4E7;goto _LL316;}else{goto _LL319;}}else{_LL319: _LL31A:
({void*_tmp4E5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4E6="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp4E6,sizeof(char),44);}),_tag_dyneither(_tmp4E5,sizeof(void*),0));});}_LL316:;}{
# 2820
struct _dyneither_ptr vs=({unsigned int _tmp4F0=(unsigned int)_tmp678;struct _tuple1**_tmp4F1=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmp4F0));struct _dyneither_ptr _tmp4F3=_tag_dyneither(_tmp4F1,sizeof(struct _tuple1*),_tmp4F0);{unsigned int _tmp4F2=_tmp4F0;unsigned int i;for(i=0;i < _tmp4F2;i ++){_tmp4F1[i]=(struct _tuple1*)Cyc_Toc_temp_var();}}_tmp4F3;});
# 2822
if(_tmp678 != 0){
# 2824
struct Cyc_List_List*_tmp4E8=0;
{int i=_tmp678 - 1;for(0;i >= 0;-- i){
_tmp4E8=({struct Cyc_List_List*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp4E9->tl=_tmp4E8;_tmp4E9;});}}
# 2828
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp4E8,0),s,0);{
# 2831
int i=0;for(0;_tmp679 != 0;(((_tmp679=_tmp679->tl,_tmp677=_tmp677->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp679->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp4EA=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp677))->hd;struct Cyc_Absyn_Datatypefield*_tmp4EB=_tmp4EA;struct _tuple1*_tmp4ED;struct Cyc_List_List*_tmp4EC;_LL31C: _tmp4ED=_tmp4EB->name;_tmp4EC=_tmp4EB->typs;_LL31D:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp4EC))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2846
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2849
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp4ED),0),s,0);
# 2852
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4ED,tud->name)),0,s,0);};}};}else{
# 2859
struct Cyc_List_List*_tmp4EE=({struct _tuple19*_tmp4EF[3];_tmp4EF[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4EF[1]=
# 2859
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4EF[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4EF,sizeof(struct _tuple19*),3));});
# 2861
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2871
{int i=0;for(0;_tmp679 != 0;(_tmp679=_tmp679->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp679->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp679->hd,0),s,0);}}
# 2877
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2890 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2892
_npop_handler(0);goto _LL283;
# 2786 "toc.cyc"
;_pop_region(r);}}case 10: _LL29C: _tmp67C=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp67B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL29D:
# 2895 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp67C);{
struct Cyc_Absyn_Exp*_tmp4F4=_tmp67B?Cyc_Toc_newrethrow_exp(_tmp67C): Cyc_Toc_newthrow_exp(_tmp67C);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp4F4,0))->r;
goto _LL283;};case 11: _LL29E: _tmp67D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_LL29F:
 Cyc_Toc_exp_to_c(nv,_tmp67D);goto _LL283;case 12: _LL2A0: _tmp67F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp67E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL2A1:
# 2901
 Cyc_Toc_exp_to_c(nv,_tmp67F);
# 2910 "toc.cyc"
for(0;_tmp67E != 0;_tmp67E=_tmp67E->tl){
enum Cyc_Absyn_KindQual _tmp4F5=(Cyc_Tcutil_typ_kind((void*)_tmp67E->hd))->kind;
if(_tmp4F5 != Cyc_Absyn_EffKind  && _tmp4F5 != Cyc_Absyn_RgnKind){
{void*_tmp4F6=Cyc_Tcutil_compress((void*)_tmp67E->hd);void*_tmp4F7=_tmp4F6;switch(*((int*)_tmp4F7)){case 2: _LL31F: _LL320:
 goto _LL322;case 3: _LL321: _LL322:
 continue;default: _LL323: _LL324:
# 2917
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp67F,0))->r;
goto _LL31E;}_LL31E:;}
# 2920
break;}}
# 2923
goto _LL283;case 13: _LL2A2: _tmp683=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp682=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_tmp681=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp497)->f3;_tmp680=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp497)->f4;_LL2A3: {
# 2925
void*old_t2=(void*)_check_null(_tmp682->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp683;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp683=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp682);
# 2932
{struct _tuple29 _tmp4F8=({struct _tuple29 _tmp533;_tmp533.f1=Cyc_Tcutil_compress(old_t2);_tmp533.f2=Cyc_Tcutil_compress(new_typ);_tmp533;});struct _tuple29 _tmp4F9=_tmp4F8;struct Cyc_Absyn_PtrInfo _tmp532;struct Cyc_Absyn_PtrInfo _tmp531;struct Cyc_Absyn_PtrInfo _tmp530;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F9.f1)->tag == 5)switch(*((int*)_tmp4F9.f2)){case 5: _LL326: _tmp531=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F9.f1)->f1;_tmp530=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F9.f2)->f1;_LL327: {
# 2934
int _tmp4FA=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp531.ptr_atts).nullable);
int _tmp4FB=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp530.ptr_atts).nullable);
void*_tmp4FC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp531.ptr_atts).bounds);
void*_tmp4FD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp530.ptr_atts).bounds);
int _tmp4FE=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp531.ptr_atts).zero_term);
int _tmp4FF=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp530.ptr_atts).zero_term);
{struct _tuple29 _tmp500=({struct _tuple29 _tmp52D;_tmp52D.f1=_tmp4FC;_tmp52D.f2=_tmp4FD;_tmp52D;});struct _tuple29 _tmp501=_tmp500;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp501.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp501.f2)->tag == 1){_LL32D: _tmp52A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp501.f1)->f1;_tmp529=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp501.f2)->f1;_LL32E:
# 2942
 if((!Cyc_Evexp_c_can_eval(_tmp52A) || !Cyc_Evexp_c_can_eval(_tmp529)) && !
Cyc_Evexp_same_const_exp(_tmp52A,_tmp529))
({void*_tmp502=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp503="can't validate cast due to potential size differences";_tag_dyneither(_tmp503,sizeof(char),54);}),_tag_dyneither(_tmp502,sizeof(void*),0));});
if(_tmp4FA  && !_tmp4FB){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp504=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp505="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp505,sizeof(char),44);}),_tag_dyneither(_tmp504,sizeof(void*),0));});
# 2951
if(_tmp680 != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0;_tmp508.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp506[1]={& _tmp508};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp507="null-check conversion mis-classified: %s";_tag_dyneither(_tmp507,sizeof(char),41);}),_tag_dyneither(_tmp506,sizeof(void*),1));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp682);
if(do_null_check){
if(!_tmp681)
({void*_tmp509=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp50A="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp50A,sizeof(char),58);}),_tag_dyneither(_tmp509,sizeof(void*),0));});
# 2958
e->r=Cyc_Toc_cast_it_r(*_tmp683,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->hd=_tmp682;_tmp50B->tl=0;_tmp50B;}),0));}else{
# 2962
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp682->r;}};}else{
# 2967
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp682->r;}
# 2974
goto _LL32C;}else{_LL32F: _tmp52B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp501.f1)->f1;_LL330:
# 2976
 if(!Cyc_Evexp_c_can_eval(_tmp52B))
({void*_tmp50C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp50D="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp50D,sizeof(char),71);}),_tag_dyneither(_tmp50C,sizeof(void*),0));});
# 2979
if(_tmp680 == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0;_tmp510.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp50E[1]={& _tmp510};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp50F="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp50F,sizeof(char),44);}),_tag_dyneither(_tmp50E,sizeof(void*),1));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2983
if((_tmp4FE  && !(_tmp530.elt_tq).real_const) && !_tmp4FF)
# 2986
_tmp52B=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp52B,Cyc_Absyn_uint_exp(1,0),0);
# 2988
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp52B,_tmp682))->r;}else{
# 2990
struct Cyc_Absyn_Exp*_tmp511=Cyc_Toc__tag_dyneither_e;
# 2992
if(_tmp4FE){
# 2997
struct _tuple1*_tmp512=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp513=Cyc_Absyn_var_exp(_tmp512,0);
struct Cyc_Absyn_Exp*arg3;
# 3002
{void*_tmp514=_tmp682->r;void*_tmp515=_tmp514;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp515)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp515)->f1).Wstring_c).tag){case 8: _LL336: _LL337:
# 3004
 arg3=_tmp52B;goto _LL335;case 9: _LL338: _LL339:
# 3006
 arg3=_tmp52B;goto _LL335;default: goto _LL33A;}else{_LL33A: _LL33B:
# 3008
 arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp682),({struct Cyc_Absyn_Exp*_tmp516[2];_tmp516[1]=_tmp52B;_tmp516[0]=
# 3010
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp513);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp516,sizeof(struct Cyc_Absyn_Exp*),2));}),0);
goto _LL335;}_LL335:;}
# 3013
if(!_tmp4FF  && !(_tmp530.elt_tq).real_const)
# 3016
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3018
struct Cyc_Absyn_Exp*_tmp517=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp530.elt_typ),0);
struct Cyc_Absyn_Exp*_tmp518=Cyc_Absyn_fncall_exp(_tmp511,({struct Cyc_Absyn_Exp*_tmp51A[3];_tmp51A[2]=arg3;_tmp51A[1]=_tmp517;_tmp51A[0]=_tmp513;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp51A,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
struct Cyc_Absyn_Stmt*_tmp519=Cyc_Absyn_exp_stmt(_tmp518,0);
_tmp519=Cyc_Absyn_declare_stmt(_tmp512,Cyc_Toc_typ_to_c(old_t2),_tmp682,_tmp519,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp519);};}else{
# 3025
e->r=Cyc_Toc_fncall_exp_r(_tmp511,({struct Cyc_Absyn_Exp*_tmp51B[3];_tmp51B[2]=_tmp52B;_tmp51B[1]=
# 3027
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp530.elt_typ),0);_tmp51B[0]=_tmp682;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp51B,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 3031
goto _LL32C;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp501.f2)->tag == 1){_LL331: _tmp52C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp501.f2)->f1;_LL332:
# 3033
 if(!Cyc_Evexp_c_can_eval(_tmp52C))
({void*_tmp51C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp51D="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp51D,sizeof(char),71);}),_tag_dyneither(_tmp51C,sizeof(void*),0));});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp51E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp51F="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp51F,sizeof(char),45);}),_tag_dyneither(_tmp51E,sizeof(void*),0));});{
# 3045 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp520=_tmp52C;
if(_tmp4FE  && !_tmp4FF)
_tmp520=Cyc_Absyn_add_exp(_tmp52C,Cyc_Absyn_uint_exp(1,0),0);{
# 3052
struct Cyc_Absyn_Exp*_tmp521=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp522=Cyc_Absyn_fncall_exp(_tmp521,({struct Cyc_Absyn_Exp*_tmp524[3];_tmp524[2]=_tmp520;_tmp524[1]=
# 3055
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp531.elt_typ),0);_tmp524[0]=_tmp682;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp524,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 3057
if(!_tmp4FB)
_tmp522->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523->hd=
Cyc_Absyn_copy_exp(_tmp522);_tmp523->tl=0;_tmp523;}));
e->r=Cyc_Toc_cast_it_r(*_tmp683,_tmp522);
goto _LL32C;};};}else{_LL333: _LL334:
# 3065
 DynCast:
 if((_tmp4FE  && !_tmp4FF) && !(_tmp530.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp525=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp526="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp526,sizeof(char),70);}),_tag_dyneither(_tmp525,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp527=Cyc_Toc__dyneither_ptr_decrease_size_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp527,({struct Cyc_Absyn_Exp*_tmp528[3];_tmp528[2]=
# 3072
Cyc_Absyn_uint_exp(1,0);_tmp528[1]=
# 3071
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp531.elt_typ),0);_tmp528[0]=_tmp682;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp528,sizeof(struct Cyc_Absyn_Exp*),3));}));};}
# 3074
goto _LL32C;}}_LL32C:;}
# 3076
goto _LL325;}case 6: _LL328: _tmp532=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F9.f1)->f1;_LL329:
# 3078
{void*_tmp52E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp532.ptr_atts).bounds);void*_tmp52F=_tmp52E;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp52F)->tag == 0){_LL33D: _LL33E:
# 3080
 _tmp682->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp682->r,_tmp682->loc),Cyc_Toc_curr_sp);goto _LL33C;}else{_LL33F: _LL340:
 goto _LL33C;}_LL33C:;}
# 3083
goto _LL325;default: goto _LL32A;}else{_LL32A: _LL32B:
# 3085
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp682->r;
goto _LL325;}_LL325:;}
# 3089
goto _LL283;}case 14: _LL2A4: _tmp684=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_LL2A5:
# 3093
{void*_tmp534=_tmp684->r;void*_tmp535=_tmp534;struct Cyc_List_List*_tmp540;struct _tuple1*_tmp53F;struct Cyc_List_List*_tmp53E;struct Cyc_List_List*_tmp53D;switch(*((int*)_tmp535)){case 28: _LL342: _tmp53F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp535)->f1;_tmp53E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp535)->f2;_tmp53D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp535)->f3;_LL343:
# 3095
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp538;_tmp538.tag=0;_tmp538.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp684->loc));({void*_tmp536[1]={& _tmp538};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp537="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp537,sizeof(char),42);}),_tag_dyneither(_tmp536,sizeof(void*),1));});});{
struct Cyc_Absyn_Exp*_tmp539=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp684->topt),_tmp53E,1,0,_tmp53D,_tmp53F);
e->r=_tmp539->r;
e->topt=_tmp539->topt;
goto _LL341;};case 23: _LL344: _tmp540=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp535)->f1;_LL345:
# 3103
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp53C;_tmp53C.tag=0;_tmp53C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp684->loc));({void*_tmp53A[1]={& _tmp53C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp53B="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp53B,sizeof(char),42);}),_tag_dyneither(_tmp53A,sizeof(void*),1));});});
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp540))->r;
goto _LL341;default: _LL346: _LL347:
# 3109
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp684);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp684)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp684,0,Cyc_Toc_address_lvalue,1);
# 3115
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp684);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp684->topt))))
# 3119
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp684);}
# 3121
goto _LL341;}_LL341:;}
# 3123
goto _LL283;case 15: _LL2A6: _tmp686=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp685=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL2A7:
# 3126
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp543;_tmp543.tag=0;_tmp543.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp685->loc));({void*_tmp541[1]={& _tmp543};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp542="%s: new at top-level";_tag_dyneither(_tmp542,sizeof(char),21);}),_tag_dyneither(_tmp541,sizeof(void*),1));});});{
void*new_e_type=(void*)_check_null(_tmp685->topt);
{void*_tmp544=_tmp685->r;void*_tmp545=_tmp544;struct Cyc_List_List*_tmp574;struct _tuple1*_tmp573;struct Cyc_List_List*_tmp572;struct Cyc_List_List*_tmp571;struct Cyc_Absyn_Aggrdecl*_tmp570;struct Cyc_Absyn_Exp*_tmp56F;void*_tmp56E;int _tmp56D;struct Cyc_Absyn_Vardecl*_tmp56C;struct Cyc_Absyn_Exp*_tmp56B;struct Cyc_Absyn_Exp*_tmp56A;int _tmp569;struct Cyc_List_List*_tmp568;switch(*((int*)_tmp545)){case 25: _LL349: _tmp568=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp545)->f1;_LL34A: {
# 3134
struct _tuple1*_tmp546=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp547=Cyc_Absyn_var_exp(_tmp546,0);
struct Cyc_Absyn_Stmt*_tmp548=Cyc_Toc_init_array(nv,new_e_type,_tmp547,_tmp568,Cyc_Absyn_exp_stmt(_tmp547,0));
void*old_elt_typ;
{void*_tmp549=Cyc_Tcutil_compress(old_typ);void*_tmp54A=_tmp549;void*_tmp54F;struct Cyc_Absyn_Tqual _tmp54E;union Cyc_Absyn_Constraint*_tmp54D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A)->tag == 5){_LL356: _tmp54F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A)->f1).elt_typ;_tmp54E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A)->f1).elt_tq;_tmp54D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A)->f1).ptr_atts).zero_term;_LL357:
# 3140
 old_elt_typ=_tmp54F;goto _LL355;}else{_LL358: _LL359:
# 3142
 old_elt_typ=({void*_tmp54B=0;((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp54C="exp_to_c:new array expression doesn't have ptr type";_tag_dyneither(_tmp54C,sizeof(char),52);}),_tag_dyneither(_tmp54B,sizeof(void*),0));});}_LL355:;}{
# 3145
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp550=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp551=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp568),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp686 == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp551);else{
# 3153
struct Cyc_Absyn_Exp*r=_tmp686;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp551);}
# 3158
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp546,_tmp550,e1,_tmp548,0));
goto _LL348;};}case 26: _LL34B: _tmp56C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp545)->f1;_tmp56B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp545)->f2;_tmp56A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp545)->f3;_tmp569=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp545)->f4;_LL34C:
# 3162
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp686,old_typ,_tmp56B,(void*)_check_null(_tmp56A->topt),_tmp569,_tmp56A,_tmp56C);
goto _LL348;case 27: _LL34D: _tmp56F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp545)->f1;_tmp56E=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp545)->f2;_tmp56D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp545)->f3;_LL34E:
# 3166
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp686,old_typ,_tmp56F,_tmp56E,_tmp56D,0,0);
goto _LL348;case 28: _LL34F: _tmp573=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp545)->f1;_tmp572=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp545)->f2;_tmp571=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp545)->f3;_tmp570=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp545)->f4;_LL350: {
# 3171
struct Cyc_Absyn_Exp*_tmp552=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp685->topt),_tmp572,1,_tmp686,_tmp571,_tmp573);
e->r=_tmp552->r;
e->topt=_tmp552->topt;
goto _LL348;}case 23: _LL351: _tmp574=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp545)->f1;_LL352:
# 3177
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp686,_tmp574))->r;
goto _LL348;default: _LL353: _LL354: {
# 3184
void*old_elt_typ=(void*)_check_null(_tmp685->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3187
struct _tuple1*_tmp553=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp554=Cyc_Absyn_var_exp(_tmp553,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp554,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp686 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3194
struct Cyc_Absyn_Exp*r=_tmp686;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3202
int done=0;
{void*_tmp555=_tmp685->r;void*_tmp556=_tmp555;void*_tmp564;struct Cyc_Absyn_Exp*_tmp563;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp556)->tag == 13){_LL35B: _tmp564=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp556)->f1;_tmp563=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp556)->f2;_LL35C:
# 3205
{struct _tuple29 _tmp557=({struct _tuple29 _tmp562;_tmp562.f1=Cyc_Tcutil_compress(_tmp564);_tmp562.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp563->topt));_tmp562;});struct _tuple29 _tmp558=_tmp557;void*_tmp561;union Cyc_Absyn_Constraint*_tmp560;union Cyc_Absyn_Constraint*_tmp55F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp558.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp558.f2)->tag == 5){_LL360: _tmp561=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp558.f1)->f1).elt_typ;_tmp560=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp558.f1)->f1).ptr_atts).bounds;_tmp55F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp558.f2)->f1).ptr_atts).bounds;_LL361:
# 3208
{struct _tuple29 _tmp559=({struct _tuple29 _tmp55E;_tmp55E.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp560);_tmp55E.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp55F);_tmp55E;});struct _tuple29 _tmp55A=_tmp559;struct Cyc_Absyn_Exp*_tmp55D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp55A.f1)->tag == 0){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp55A.f2)->tag == 1){_LL365: _tmp55D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp55A.f2)->f1;_LL366:
# 3210
 Cyc_Toc_exp_to_c(nv,_tmp563);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp55B=Cyc_Toc__init_dyneither_ptr_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp55B,({struct Cyc_Absyn_Exp*_tmp55C[4];_tmp55C[3]=_tmp55D;_tmp55C[2]=
# 3216
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp561),0);_tmp55C[1]=_tmp563;_tmp55C[0]=mexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp55C,sizeof(struct Cyc_Absyn_Exp*),4));}));
# 3218
goto _LL364;};}else{goto _LL367;}}else{_LL367: _LL368:
 goto _LL364;}_LL364:;}
# 3221
goto _LL35F;}else{goto _LL362;}}else{_LL362: _LL363:
 goto _LL35F;}_LL35F:;}
# 3224
goto _LL35A;}else{_LL35D: _LL35E:
 goto _LL35A;}_LL35A:;}
# 3227
if(!done){
struct Cyc_Absyn_Stmt*_tmp565=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp554),0);
struct Cyc_Absyn_Exp*_tmp566=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp685);
_tmp565=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp554,_tmp566,0),_tmp685,0),_tmp565,0);{
# 3233
void*_tmp567=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp553,_tmp567,mexp,_tmp565,0));};}
# 3236
goto _LL348;};}}_LL348:;}
# 3238
goto _LL283;};case 17: _LL2A8: _tmp687=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_LL2A9: {
# 3241
int _tmp575=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp687);
Cyc_Toc_set_in_sizeof(nv,_tmp575);
goto _LL283;}case 16: _LL2AA: _tmp688=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_LL2AB:
 e->r=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp577;_tmp577.tag=16;_tmp577.f1=Cyc_Toc_typ_to_c(_tmp688);_tmp577;});_tmp576;});goto _LL283;case 18: _LL2AC: _tmp68A=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp689=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL2AD: {
# 3247
void*_tmp578=_tmp68A;
struct Cyc_List_List*_tmp579=_tmp689;
for(0;_tmp579 != 0;_tmp579=_tmp579->tl){
void*_tmp57A=(void*)_tmp579->hd;void*_tmp57B=_tmp57A;unsigned int _tmp591;struct _dyneither_ptr*_tmp590;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp57B)->tag == 0){_LL36A: _tmp590=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp57B)->f1;_LL36B:
 goto _LL369;}else{_LL36C: _tmp591=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp57B)->f1;_LL36D:
# 3253
{void*_tmp57C=Cyc_Tcutil_compress(_tmp578);void*_tmp57D=_tmp57C;struct Cyc_Absyn_Datatypefield*_tmp58F;struct Cyc_List_List*_tmp58E;struct Cyc_List_List*_tmp58D;union Cyc_Absyn_AggrInfoU _tmp58C;switch(*((int*)_tmp57D)){case 11: _LL36F: _tmp58C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp57D)->f1).aggr_info;_LL370: {
# 3255
struct Cyc_Absyn_Aggrdecl*_tmp57E=Cyc_Absyn_get_known_aggrdecl(_tmp58C);
if(_tmp57E->impl == 0)
({void*_tmp57F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp580="struct fields must be known";_tag_dyneither(_tmp580,sizeof(char),28);}),_tag_dyneither(_tmp57F,sizeof(void*),0));});
_tmp58D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57E->impl))->fields;goto _LL372;}case 12: _LL371: _tmp58D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57D)->f2;_LL372: {
# 3260
struct Cyc_Absyn_Aggrfield*_tmp581=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp58D,(int)_tmp591);
_tmp579->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp583;_tmp583.tag=0;_tmp583.f1=_tmp581->name;_tmp583;});_tmp582;}));
_tmp578=_tmp581->type;
goto _LL36E;}case 10: _LL373: _tmp58E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp57D)->f1;_LL374:
# 3265
 _tmp579->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp585;_tmp585.tag=0;_tmp585.f1=Cyc_Absyn_fieldname((int)(_tmp591 + 1));_tmp585;});_tmp584;}));
_tmp578=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp58E,(int)_tmp591)).f2;
goto _LL36E;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57D)->f1).field_info).KnownDatatypefield).tag == 2){_LL375: _tmp58F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57D)->f1).field_info).KnownDatatypefield).val).f2;_LL376:
# 3269
 if(_tmp591 == 0)
_tmp579->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp587;_tmp587.tag=0;_tmp587.f1=Cyc_Toc_tag_sp;_tmp587;});_tmp586;}));else{
# 3272
_tmp578=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp58F->typs,(int)(_tmp591 - 1))).f2;
_tmp579->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp589;_tmp589.tag=0;_tmp589.f1=Cyc_Absyn_fieldname((int)_tmp591);_tmp589;});_tmp588;}));}
# 3275
goto _LL36E;}else{goto _LL377;}default: _LL377: _LL378:
({void*_tmp58A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp58B="impossible type for offsetof tuple index";_tag_dyneither(_tmp58B,sizeof(char),41);}),_tag_dyneither(_tmp58A,sizeof(void*),0));});
goto _LL36E;}_LL36E:;}
# 3279
goto _LL369;}_LL369:;}
# 3282
e->r=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp593;_tmp593.tag=18;_tmp593.f1=Cyc_Toc_typ_to_c(_tmp68A);_tmp593.f2=_tmp689;_tmp593;});_tmp592;});
goto _LL283;}case 19: _LL2AE: _tmp68B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_LL2AF: {
# 3285
int _tmp594=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp595=Cyc_Tcutil_compress((void*)_check_null(_tmp68B->topt));
{void*_tmp596=_tmp595;void*_tmp5AB;struct Cyc_Absyn_Tqual _tmp5AA;void*_tmp5A9;union Cyc_Absyn_Constraint*_tmp5A8;union Cyc_Absyn_Constraint*_tmp5A7;union Cyc_Absyn_Constraint*_tmp5A6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->tag == 5){_LL37A: _tmp5AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).elt_typ;_tmp5AA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).elt_tq;_tmp5A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).ptr_atts).rgn;_tmp5A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).ptr_atts).nullable;_tmp5A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).ptr_atts).bounds;_tmp5A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).ptr_atts).zero_term;_LL37B:
# 3290
{void*_tmp597=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5A7);void*_tmp598=_tmp597;struct Cyc_Absyn_Exp*_tmp5A3;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp598)->tag == 1){_LL37F: _tmp5A3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp598)->f1;_LL380: {
# 3292
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp68B);
Cyc_Toc_exp_to_c(nv,_tmp68B);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp599=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp59A="inserted null check due to dereference";_tag_dyneither(_tmp59A,sizeof(char),39);}),_tag_dyneither(_tmp599,sizeof(void*),0));});
# 3298
_tmp68B->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp595),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->hd=
Cyc_Absyn_copy_exp(_tmp68B);_tmp59B->tl=0;_tmp59B;}),0));}
# 3306
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A6)){
struct _tuple12 _tmp59C=Cyc_Evexp_eval_const_uint_exp(_tmp5A3);struct _tuple12 _tmp59D=_tmp59C;unsigned int _tmp5A1;int _tmp5A0;_LL384: _tmp5A1=_tmp59D.f1;_tmp5A0=_tmp59D.f2;_LL385:;
# 3312
if(!_tmp5A0  || _tmp5A1 <= 0)
({void*_tmp59E=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp59F="cannot determine dereference is in bounds";_tag_dyneither(_tmp59F,sizeof(char),42);}),_tag_dyneither(_tmp59E,sizeof(void*),0));});}
# 3315
goto _LL37E;}}else{_LL381: _LL382: {
# 3318
struct Cyc_Absyn_Exp*_tmp5A2=Cyc_Absyn_uint_exp(0,0);
_tmp5A2->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp68B,_tmp5A2);
Cyc_Toc_exp_to_c(nv,e);
goto _LL37E;}}_LL37E:;}
# 3324
goto _LL379;}else{_LL37C: _LL37D:
({void*_tmp5A4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5A5="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp5A5,sizeof(char),29);}),_tag_dyneither(_tmp5A4,sizeof(void*),0));});}_LL379:;}
# 3327
Cyc_Toc_set_lhs(nv,_tmp594);
goto _LL283;};}case 20: _LL2B0: _tmp68F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp68E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_tmp68D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp497)->f3;_tmp68C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp497)->f4;_LL2B1: {
# 3330
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp68F->topt);
Cyc_Toc_exp_to_c(nv,_tmp68F);
if(_tmp68D  && _tmp68C)
e->r=Cyc_Toc_check_tagged_union(_tmp68F,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp68E,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3338
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL283;}case 21: _LL2B2: _tmp693=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp692=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_tmp691=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp497)->f3;_tmp690=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp497)->f4;_LL2B3: {
# 3342
int _tmp5AC=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp693->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp693);
Cyc_Toc_exp_to_c(nv,_tmp693);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp5AD=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp5AE=_tmp5AD;void*_tmp5C4;struct Cyc_Absyn_Tqual _tmp5C3;void*_tmp5C2;union Cyc_Absyn_Constraint*_tmp5C1;union Cyc_Absyn_Constraint*_tmp5C0;union Cyc_Absyn_Constraint*_tmp5BF;_LL387: _tmp5C4=_tmp5AE.elt_typ;_tmp5C3=_tmp5AE.elt_tq;_tmp5C2=(_tmp5AE.ptr_atts).rgn;_tmp5C1=(_tmp5AE.ptr_atts).nullable;_tmp5C0=(_tmp5AE.ptr_atts).bounds;_tmp5BF=(_tmp5AE.ptr_atts).zero_term;_LL388:;
{void*_tmp5AF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5C0);void*_tmp5B0=_tmp5AF;struct Cyc_Absyn_Exp*_tmp5BE;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5B0)->tag == 1){_LL38A: _tmp5BE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5B0)->f1;_LL38B: {
# 3353
struct _tuple12 _tmp5B1=Cyc_Evexp_eval_const_uint_exp(_tmp5BE);struct _tuple12 _tmp5B2=_tmp5B1;unsigned int _tmp5BC;int _tmp5BB;_LL38F: _tmp5BC=_tmp5B2.f1;_tmp5BB=_tmp5B2.f2;_LL390:;
if(_tmp5BB){
if(_tmp5BC < 1)
({void*_tmp5B3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5B4="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp5B4,sizeof(char),44);}),_tag_dyneither(_tmp5B3,sizeof(void*),0));});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5B5=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5B6="inserted null check due to dereference";_tag_dyneither(_tmp5B6,sizeof(char),39);}),_tag_dyneither(_tmp5B5,sizeof(void*),0));});
# 3361
_tmp693->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5B7[1];_tmp5B7[0]=
Cyc_Absyn_new_exp(_tmp693->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5B7,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}}else{
# 3366
if(!Cyc_Evexp_c_can_eval(_tmp5BE))
({void*_tmp5B8=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5B9="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp5B9,sizeof(char),47);}),_tag_dyneither(_tmp5B8,sizeof(void*),0));});
# 3370
_tmp693->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5BA[4];_tmp5BA[3]=
# 3374
Cyc_Absyn_uint_exp(0,0);_tmp5BA[2]=
# 3373
Cyc_Absyn_sizeoftyp_exp(_tmp5C4,0);_tmp5BA[1]=_tmp5BE;_tmp5BA[0]=
# 3372
Cyc_Absyn_new_exp(_tmp693->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5BA,sizeof(struct Cyc_Absyn_Exp*),4));}),0));}
# 3376
goto _LL389;}}else{_LL38C: _LL38D: {
# 3379
void*ta1=Cyc_Toc_typ_to_c(_tmp5C4);
_tmp693->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp5C3),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,({struct Cyc_Absyn_Exp*_tmp5BD[3];_tmp5BD[2]=
# 3384
Cyc_Absyn_uint_exp(0,0);_tmp5BD[1]=
# 3383
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5BD[0]=
# 3382
Cyc_Absyn_new_exp(_tmp693->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5BD,sizeof(struct Cyc_Absyn_Exp*),3));}),0));
# 3387
goto _LL389;}}_LL389:;}
# 3389
if(_tmp691  && _tmp690)
e->r=Cyc_Toc_check_tagged_union(_tmp693,Cyc_Toc_typ_to_c(e1typ),_tmp5C4,_tmp692,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3392
if(is_poly  && _tmp690)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp5AC);
goto _LL283;};};}case 22: _LL2B4: _tmp695=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp694=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL2B5: {
# 3397
int _tmp5C5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5C6=Cyc_Tcutil_compress((void*)_check_null(_tmp695->topt));
# 3401
{void*_tmp5C7=_tmp5C6;void*_tmp5F1;struct Cyc_Absyn_Tqual _tmp5F0;void*_tmp5EF;union Cyc_Absyn_Constraint*_tmp5EE;union Cyc_Absyn_Constraint*_tmp5ED;union Cyc_Absyn_Constraint*_tmp5EC;struct Cyc_List_List*_tmp5EB;switch(*((int*)_tmp5C7)){case 10: _LL392: _tmp5EB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5C7)->f1;_LL393:
# 3404
 Cyc_Toc_exp_to_c(nv,_tmp695);
Cyc_Toc_exp_to_c(nv,_tmp694);{
struct _tuple12 _tmp5C8=Cyc_Evexp_eval_const_uint_exp(_tmp694);struct _tuple12 _tmp5C9=_tmp5C8;unsigned int _tmp5CD;int _tmp5CC;_LL399: _tmp5CD=_tmp5C9.f1;_tmp5CC=_tmp5C9.f2;_LL39A:;
if(!_tmp5CC)
({void*_tmp5CA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5CB="unknown tuple subscript in translation to C";_tag_dyneither(_tmp5CB,sizeof(char),44);}),_tag_dyneither(_tmp5CA,sizeof(void*),0));});
e->r=Cyc_Toc_aggrmember_exp_r(_tmp695,Cyc_Absyn_fieldname((int)(_tmp5CD + 1)));
goto _LL391;};case 5: _LL394: _tmp5F1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).elt_typ;_tmp5F0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).elt_tq;_tmp5EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).ptr_atts).rgn;_tmp5EE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).ptr_atts).nullable;_tmp5ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).ptr_atts).bounds;_tmp5EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).ptr_atts).zero_term;_LL395: {
# 3412
struct Cyc_List_List*_tmp5CE=Cyc_Toc_get_relns(_tmp695);
int _tmp5CF=Cyc_Toc_need_null_check(_tmp695);
int _tmp5D0=Cyc_Toc_in_sizeof(nv);
# 3420
int in_bnds=_tmp5D0;
{void*_tmp5D1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5ED);void*_tmp5D2=_tmp5D1;_LL39C: _LL39D:
# 3423
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp5C6,_tmp5CE,_tmp695,_tmp694);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp5D5;_tmp5D5.tag=0;_tmp5D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp5D3[1]={& _tmp5D5};Cyc_Tcutil_warn(e->loc,({const char*_tmp5D4="bounds check necessary for %s";_tag_dyneither(_tmp5D4,sizeof(char),30);}),_tag_dyneither(_tmp5D3,sizeof(void*),1));});});_LL39B:;}
# 3430
Cyc_Toc_exp_to_c(nv,_tmp695);
Cyc_Toc_exp_to_c(nv,_tmp694);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp5D6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5ED);void*_tmp5D7=_tmp5D6;struct Cyc_Absyn_Exp*_tmp5E8;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5D7)->tag == 1){_LL39F: _tmp5E8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5D7)->f1;_LL3A0: {
# 3435
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5EE) && _tmp5CF;
void*ta1=Cyc_Toc_typ_to_c(_tmp5F1);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5F0);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5D8=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5D9="inserted null check due to dereference";_tag_dyneither(_tmp5D9,sizeof(char),39);}),_tag_dyneither(_tmp5D8,sizeof(void*),0));});
_tmp695->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5DA[1];_tmp5DA[0]=
Cyc_Absyn_copy_exp(_tmp695);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5DA,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}else{
# 3447
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5EC)){
# 3449
if(!Cyc_Evexp_c_can_eval(_tmp5E8))
({void*_tmp5DB=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5DC="cannot determine subscript is in bounds";_tag_dyneither(_tmp5DC,sizeof(char),40);}),_tag_dyneither(_tmp5DB,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp695);
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,({struct Cyc_Absyn_Exp*_tmp5DD[3];_tmp5DD[2]=_tmp694;_tmp5DD[1]=_tmp5E8;_tmp5DD[0]=_tmp695;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5DD,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));};}else{
# 3455
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp5E8))
({void*_tmp5DE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5DF="cannot determine subscript is in bounds";_tag_dyneither(_tmp5DF,sizeof(char),40);}),_tag_dyneither(_tmp5DE,sizeof(void*),0));});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5E0=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5E1="inserted null check due to dereference";_tag_dyneither(_tmp5E1,sizeof(char),39);}),_tag_dyneither(_tmp5E0,sizeof(void*),0));});
# 3461
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5E2[4];_tmp5E2[3]=_tmp694;_tmp5E2[2]=
# 3464
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5E2[1]=_tmp5E8;_tmp5E2[0]=_tmp695;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E2,sizeof(struct Cyc_Absyn_Exp*),4));}),0)));}else{
# 3467
if(!Cyc_Evexp_c_can_eval(_tmp5E8))
({void*_tmp5E3=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5E4="cannot determine subscript is in bounds";_tag_dyneither(_tmp5E4,sizeof(char),40);}),_tag_dyneither(_tmp5E3,sizeof(void*),0));});
# 3470
_tmp694->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({struct Cyc_Absyn_Exp*_tmp5E5[2];_tmp5E5[1]=
Cyc_Absyn_copy_exp(_tmp694);_tmp5E5[0]=_tmp5E8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E5,sizeof(struct Cyc_Absyn_Exp*),2));}));}}}}
# 3473
goto _LL39E;}}else{_LL3A1: _LL3A2: {
# 3475
void*ta1=Cyc_Toc_typ_to_c(_tmp5F1);
if(in_bnds){
# 3479
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5F0),
Cyc_Toc_member_exp(_tmp695,Cyc_Toc_curr_sp,0)),_tmp694);}else{
# 3484
struct Cyc_Absyn_Exp*_tmp5E6=Cyc_Toc__check_dyneither_subscript_e;
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5F0),
Cyc_Absyn_fncall_exp(_tmp5E6,({struct Cyc_Absyn_Exp*_tmp5E7[3];_tmp5E7[2]=_tmp694;_tmp5E7[1]=
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5E7[0]=_tmp695;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E7,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}
# 3490
goto _LL39E;}}_LL39E:;}
# 3492
goto _LL391;}default: _LL396: _LL397:
({void*_tmp5E9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5EA="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp5EA,sizeof(char),49);}),_tag_dyneither(_tmp5E9,sizeof(void*),0));});}_LL391:;}
# 3495
Cyc_Toc_set_lhs(nv,_tmp5C5);
goto _LL283;};}case 23: _LL2B6: _tmp696=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_LL2B7:
# 3498
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp696))->r;else{
# 3503
struct Cyc_List_List*_tmp5F2=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp696);
void*_tmp5F3=Cyc_Toc_add_tuple_type(_tmp5F2);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp696 != 0;(_tmp696=_tmp696->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp696->hd);
dles=({struct Cyc_List_List*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->hd=({struct _tuple19*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->f1=0;_tmp5F5->f2=(struct Cyc_Absyn_Exp*)_tmp696->hd;_tmp5F5;});_tmp5F4->tl=dles;_tmp5F4;});}}
# 3510
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3513
goto _LL283;case 25: _LL2B8: _tmp697=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_LL2B9:
# 3517
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp697);
{struct Cyc_List_List*_tmp5F6=_tmp697;for(0;_tmp5F6 != 0;_tmp5F6=_tmp5F6->tl){
struct _tuple19*_tmp5F7=(struct _tuple19*)_tmp5F6->hd;struct _tuple19*_tmp5F8=_tmp5F7;struct Cyc_Absyn_Exp*_tmp5F9;_LL3A4: _tmp5F9=_tmp5F8->f2;_LL3A5:;
Cyc_Toc_exp_to_c(nv,_tmp5F9);}}
# 3522
goto _LL283;case 26: _LL2BA: _tmp69B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp69A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_tmp699=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp497)->f3;_tmp698=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp497)->f4;_LL2BB: {
# 3526
struct _tuple12 _tmp5FA=Cyc_Evexp_eval_const_uint_exp(_tmp69A);struct _tuple12 _tmp5FB=_tmp5FA;unsigned int _tmp605;int _tmp604;_LL3A7: _tmp605=_tmp5FB.f1;_tmp604=_tmp5FB.f2;_LL3A8:;{
void*_tmp5FC=Cyc_Toc_typ_to_c((void*)_check_null(_tmp699->topt));
Cyc_Toc_exp_to_c(nv,_tmp699);{
struct Cyc_List_List*es=0;
# 3531
if(!Cyc_Toc_is_zero(_tmp699)){
if(!_tmp604)
({void*_tmp5FD=0;Cyc_Tcutil_terr(_tmp69A->loc,({const char*_tmp5FE="cannot determine value of constant";_tag_dyneither(_tmp5FE,sizeof(char),35);}),_tag_dyneither(_tmp5FD,sizeof(void*),0));});
{unsigned int i=0;for(0;i < _tmp605;++ i){
es=({struct Cyc_List_List*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->hd=({struct _tuple19*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->f1=0;_tmp600->f2=_tmp699;_tmp600;});_tmp5FF->tl=es;_tmp5FF;});}}
# 3537
if(_tmp698){
struct Cyc_Absyn_Exp*_tmp601=Cyc_Toc_cast_it(_tmp5FC,Cyc_Absyn_uint_exp(0,0));
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->hd=({struct _tuple19*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->f1=0;_tmp603->f2=_tmp601;_tmp603;});_tmp602->tl=0;_tmp602;}));}}
# 3542
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL283;};};}case 27: _LL2BC: _tmp69E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp69D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_tmp69C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp497)->f3;_LL2BD:
# 3547
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL283;case 28: _LL2BE: _tmp6A2=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp6A1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_tmp6A0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp497)->f3;_tmp69F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp497)->f4;_LL2BF:
# 3552
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp606=Cyc_Toc_init_struct(nv,old_typ,_tmp6A1,0,0,_tmp6A0,_tmp6A2);
e->r=_tmp606->r;
e->topt=_tmp606->topt;}else{
# 3563
if(_tmp69F == 0)
({void*_tmp607=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp608="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp608,sizeof(char),38);}),_tag_dyneither(_tmp607,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp69F;
# 3567
struct _RegionHandle _tmp609=_new_region("rgn");struct _RegionHandle*rgn=& _tmp609;_push_region(rgn);
{struct Cyc_List_List*_tmp60A=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp6A0,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3571
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69F->impl))->tagged){
# 3573
struct _tuple30*_tmp60B=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp60A))->hd;struct _tuple30*_tmp60C=_tmp60B;struct Cyc_Absyn_Aggrfield*_tmp619;struct Cyc_Absyn_Exp*_tmp618;_LL3AA: _tmp619=_tmp60C->f1;_tmp618=_tmp60C->f2;_LL3AB:;{
void*_tmp60D=(void*)_check_null(_tmp618->topt);
void*_tmp60E=_tmp619->type;
Cyc_Toc_exp_to_c(nv,_tmp618);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp60E) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp60D))
_tmp618->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp618->r,0));{
# 3582
int i=Cyc_Toc_get_member_offset(_tmp69F,_tmp619->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_List_List*_tmp60F=({struct _tuple19*_tmp615[2];_tmp615[1]=({struct _tuple19*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->f1=0;_tmp617->f2=_tmp618;_tmp617;});_tmp615[0]=({struct _tuple19*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->f1=0;_tmp616->f2=field_tag_exp;_tmp616;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp615,sizeof(struct _tuple19*),2));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp60F,0);
struct Cyc_List_List*ds=({void*_tmp612[1];_tmp612[0]=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp614;_tmp614.tag=1;_tmp614.f1=_tmp619->name;_tmp614;});_tmp613;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp612,sizeof(void*),1));});
struct Cyc_List_List*dles=({struct _tuple19*_tmp610[1];_tmp610[0]=({struct _tuple19*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->f1=ds;_tmp611->f2=umem;_tmp611;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp610,sizeof(struct _tuple19*),1));});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3591
struct Cyc_List_List*_tmp61A=0;
struct Cyc_List_List*_tmp61B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp61B != 0;_tmp61B=_tmp61B->tl){
struct Cyc_List_List*_tmp61C=_tmp60A;for(0;_tmp61C != 0;_tmp61C=_tmp61C->tl){
if((*((struct _tuple30*)_tmp61C->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp61B->hd){
struct _tuple30*_tmp61D=(struct _tuple30*)_tmp61C->hd;struct _tuple30*_tmp61E=_tmp61D;struct Cyc_Absyn_Aggrfield*_tmp624;struct Cyc_Absyn_Exp*_tmp623;_LL3AD: _tmp624=_tmp61E->f1;_tmp623=_tmp61E->f2;_LL3AE:;{
void*_tmp61F=Cyc_Toc_typ_to_c(_tmp624->type);
void*_tmp620=Cyc_Toc_typ_to_c((void*)_check_null(_tmp623->topt));
Cyc_Toc_exp_to_c(nv,_tmp623);
# 3601
if(!Cyc_Tcutil_unify(_tmp61F,_tmp620)){
# 3603
if(!Cyc_Tcutil_is_arithmetic_type(_tmp61F) || !
Cyc_Tcutil_is_arithmetic_type(_tmp620))
_tmp623=Cyc_Toc_cast_it(_tmp61F,Cyc_Absyn_copy_exp(_tmp623));}
_tmp61A=({struct Cyc_List_List*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->hd=({struct _tuple19*_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->f1=0;_tmp622->f2=_tmp623;_tmp622;});_tmp621->tl=_tmp61A;_tmp621;});
break;};}}}
# 3610
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp61A));}}
# 3568
;_pop_region(rgn);};}
# 3614
goto _LL283;case 29: _LL2C0: _tmp6A4=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp6A3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL2C1: {
# 3616
struct Cyc_List_List*fs;
{void*_tmp625=Cyc_Tcutil_compress(_tmp6A4);void*_tmp626=_tmp625;struct Cyc_List_List*_tmp62A;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp626)->tag == 12){_LL3B0: _tmp62A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp626)->f2;_LL3B1:
 fs=_tmp62A;goto _LL3AF;}else{_LL3B2: _LL3B3:
({struct Cyc_String_pa_PrintArg_struct _tmp629;_tmp629.tag=0;_tmp629.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp6A4));({void*_tmp627[1]={& _tmp629};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp628="anon struct has type %s";_tag_dyneither(_tmp628,sizeof(char),24);}),_tag_dyneither(_tmp627,sizeof(void*),1));});});}_LL3AF:;}{
# 3621
struct _RegionHandle _tmp62B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp62B;_push_region(rgn);{
struct Cyc_List_List*_tmp62C=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp6A3,Cyc_Absyn_StructA,fs);
for(0;_tmp62C != 0;_tmp62C=_tmp62C->tl){
struct _tuple30*_tmp62D=(struct _tuple30*)_tmp62C->hd;struct _tuple30*_tmp62E=_tmp62D;struct Cyc_Absyn_Aggrfield*_tmp632;struct Cyc_Absyn_Exp*_tmp631;_LL3B5: _tmp632=_tmp62E->f1;_tmp631=_tmp62E->f2;_LL3B6:;{
void*_tmp62F=(void*)_check_null(_tmp631->topt);
void*_tmp630=_tmp632->type;
Cyc_Toc_exp_to_c(nv,_tmp631);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp630) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp62F))
_tmp631->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp631->r,0));};}
# 3635
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp6A3);}
# 3637
_npop_handler(0);goto _LL283;
# 3621
;_pop_region(rgn);};}case 30: _LL2C2: _tmp6A7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp6A6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_tmp6A5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp497)->f3;_LL2C3: {
# 3640
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp633=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp634=Cyc_Absyn_var_exp(_tmp633,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6A5->name,_tmp6A6->name));
tag_exp=_tmp6A6->is_extensible?Cyc_Absyn_var_exp(_tmp6A5->name,0):
 Cyc_Toc_datatype_tag(_tmp6A6,_tmp6A5->name);
mem_exp=_tmp634;{
struct Cyc_List_List*_tmp635=_tmp6A5->typs;
# 3651
if(Cyc_Toc_is_toplevel(nv)){
# 3653
struct Cyc_List_List*dles=0;
for(0;_tmp6A7 != 0;(_tmp6A7=_tmp6A7->tl,_tmp635=_tmp635->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp6A7->hd;
void*_tmp636=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp635))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp636))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);
dles=({struct Cyc_List_List*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->hd=({struct _tuple19*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->f1=0;_tmp638->f2=cur_e;_tmp638;});_tmp637->tl=dles;_tmp637;});}
# 3664
dles=({struct Cyc_List_List*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->hd=({struct _tuple19*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->f1=0;_tmp63A->f2=tag_exp;_tmp63A;});_tmp639->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp639;});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3670
struct Cyc_List_List*_tmp63B=({struct Cyc_List_List*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->hd=
Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp642->tl=0;_tmp642;});
# 3673
{int i=1;for(0;_tmp6A7 != 0;(((_tmp6A7=_tmp6A7->tl,i ++)),_tmp635=_tmp635->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp6A7->hd;
void*_tmp63C=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp635))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp63C))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp63D=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3683
_tmp63B=({struct Cyc_List_List*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->hd=_tmp63D;_tmp63E->tl=_tmp63B;_tmp63E;});};}}{
# 3685
struct Cyc_Absyn_Stmt*_tmp63F=Cyc_Absyn_exp_stmt(_tmp634,0);
struct Cyc_Absyn_Stmt*_tmp640=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->hd=_tmp63F;_tmp641->tl=_tmp63B;_tmp641;})),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp633,datatype_ctype,0,_tmp640,0));};}
# 3689
goto _LL283;};}case 31: _LL2C4: _LL2C5:
# 3691
 goto _LL2C7;case 32: _LL2C6: _LL2C7:
 goto _LL283;case 33: _LL2C8: _tmp6AD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp497)->f1).is_calloc;_tmp6AC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp497)->f1).rgn;_tmp6AB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp497)->f1).elt_type;_tmp6AA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp497)->f1).num_elts;_tmp6A9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp497)->f1).fat_result;_tmp6A8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp497)->f1).inline_call;_LL2C9: {
# 3695
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp6AB)));
Cyc_Toc_exp_to_c(nv,_tmp6AA);
# 3699
if(_tmp6A9){
struct _tuple1*_tmp643=Cyc_Toc_temp_var();
struct _tuple1*_tmp644=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp6AD){
xexp=_tmp6AA;
if(_tmp6AC != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6AC;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp643,0));}else{
# 3710
pexp=Cyc_Toc_calloc_exp(*_tmp6AB,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp643,0));}
# 3712
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp645[3];_tmp645[2]=
# 3714
Cyc_Absyn_var_exp(_tmp643,0);_tmp645[1]=
# 3713
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp645[0]=Cyc_Absyn_var_exp(_tmp644,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp645,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{
# 3716
if(_tmp6AC != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6AC;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp6A8)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,Cyc_Absyn_var_exp(_tmp643,0));else{
# 3722
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp643,0));}}else{
# 3724
pexp=Cyc_Toc_malloc_exp(*_tmp6AB,Cyc_Absyn_var_exp(_tmp643,0));}
# 3726
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp646[3];_tmp646[2]=
Cyc_Absyn_var_exp(_tmp643,0);_tmp646[1]=
# 3726
Cyc_Absyn_uint_exp(1,0);_tmp646[0]=Cyc_Absyn_var_exp(_tmp644,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp646,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}{
# 3729
struct Cyc_Absyn_Stmt*_tmp647=Cyc_Absyn_declare_stmt(_tmp643,Cyc_Absyn_uint_typ,_tmp6AA,
Cyc_Absyn_declare_stmt(_tmp644,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp647);};}else{
# 3734
if(_tmp6AD){
if(_tmp6AC != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6AC;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp6AA))->r;}else{
# 3740
e->r=(Cyc_Toc_calloc_exp(*_tmp6AB,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp6AA))->r;}}else{
# 3743
if(_tmp6AC != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6AC;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp6A8)
e->r=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp6AA))->r;else{
# 3749
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp6AA))->r;}}else{
# 3751
e->r=(Cyc_Toc_malloc_exp(*_tmp6AB,_tmp6AA))->r;}}}
# 3755
goto _LL283;}case 34: _LL2CA: _tmp6AF=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp6AE=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL2CB: {
# 3764
void*e1_old_typ=(void*)_check_null(_tmp6AF->topt);
void*e2_old_typ=(void*)_check_null(_tmp6AE->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp648=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp649="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp649,sizeof(char),57);}),_tag_dyneither(_tmp648,sizeof(void*),0));});{
# 3772
unsigned int _tmp64A=e->loc;
struct _tuple1*_tmp64B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp64C=Cyc_Absyn_var_exp(_tmp64B,_tmp64A);_tmp64C->topt=e1_old_typ;{
struct _tuple1*_tmp64D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp64E=Cyc_Absyn_var_exp(_tmp64D,_tmp64A);_tmp64E->topt=e2_old_typ;{
# 3778
struct Cyc_Absyn_Exp*_tmp64F=Cyc_Absyn_assign_exp(Cyc_Tcutil_deep_copy_exp(1,_tmp6AF),_tmp64E,_tmp64A);_tmp64F->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp650=Cyc_Absyn_exp_stmt(_tmp64F,_tmp64A);
struct Cyc_Absyn_Exp*_tmp651=Cyc_Absyn_assign_exp(Cyc_Tcutil_deep_copy_exp(1,_tmp6AE),_tmp64C,_tmp64A);_tmp651->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp652=Cyc_Absyn_exp_stmt(_tmp651,_tmp64A);
# 3783
struct Cyc_Absyn_Stmt*_tmp653=Cyc_Absyn_declare_stmt(_tmp64B,e1_old_typ,_tmp6AF,
Cyc_Absyn_declare_stmt(_tmp64D,e2_old_typ,_tmp6AE,
Cyc_Absyn_seq_stmt(_tmp650,_tmp652,_tmp64A),_tmp64A),_tmp64A);
Cyc_Toc_stmt_to_c(nv,_tmp653);
e->r=Cyc_Toc_stmt_exp_r(_tmp653);
goto _LL283;};};};};};}case 37: _LL2CC: _tmp6B1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_tmp6B0=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL2CD: {
# 3791
void*_tmp654=Cyc_Tcutil_compress((void*)_check_null(_tmp6B1->topt));
Cyc_Toc_exp_to_c(nv,_tmp6B1);
{void*_tmp655=_tmp654;struct Cyc_Absyn_Aggrdecl*_tmp65B;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp655)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp655)->f1).aggr_info).KnownAggr).tag == 2){_LL3B8: _tmp65B=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp655)->f1).aggr_info).KnownAggr).val;_LL3B9: {
# 3795
struct Cyc_Absyn_Exp*_tmp656=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp65B,_tmp6B0),0);
struct Cyc_Absyn_Exp*_tmp657=Cyc_Toc_member_exp(_tmp6B1,_tmp6B0,0);
struct Cyc_Absyn_Exp*_tmp658=Cyc_Toc_member_exp(_tmp657,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp658,_tmp656,0))->r;
goto _LL3B7;}}else{goto _LL3BA;}}else{_LL3BA: _LL3BB:
({void*_tmp659=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp65A="non-aggregate type in tagcheck";_tag_dyneither(_tmp65A,sizeof(char),31);}),_tag_dyneither(_tmp659,sizeof(void*),0));});}_LL3B7:;}
# 3802
goto _LL283;}case 36: _LL2CE: _tmp6B2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp497)->f1;_LL2CF:
 Cyc_Toc_stmt_to_c(nv,_tmp6B2);goto _LL283;case 35: _LL2D0: _LL2D1:
({void*_tmp65C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp65D="UnresolvedMem";_tag_dyneither(_tmp65D,sizeof(char),14);}),_tag_dyneither(_tmp65C,sizeof(void*),0));});case 24: _LL2D2: _LL2D3:
({void*_tmp65E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp65F="compoundlit";_tag_dyneither(_tmp65F,sizeof(char),12);}),_tag_dyneither(_tmp65E,sizeof(void*),0));});case 38: _LL2D4: _LL2D5:
({void*_tmp660=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp661="valueof(-)";_tag_dyneither(_tmp661,sizeof(char),11);}),_tag_dyneither(_tmp660,sizeof(void*),0));});default: _LL2D6: _LL2D7:
({void*_tmp662=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp663="__asm__";_tag_dyneither(_tmp663,sizeof(char),8);}),_tag_dyneither(_tmp662,sizeof(void*),0));});}_LL283:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3835 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3837
return({struct _tuple31*_tmp6B3=_region_malloc(r,sizeof(*_tmp6B3));_tmp6B3->f1=0;_tmp6B3->f2=Cyc_Toc_fresh_label();_tmp6B3->f3=Cyc_Toc_fresh_label();_tmp6B3->f4=sc;_tmp6B3;});}
# 3842
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp6B4=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3848
if((int)(((_tmp6B4->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp6B7=_tmp6B4->orig_pat;if((_tmp6B7.pattern).tag != 1)_throw_match();(_tmp6B7.pattern).val;})->topt);
void*_tmp6B5=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp6B6=_tmp6B5;switch(*((int*)_tmp6B6)){case 0: _LL3BD: _LL3BE:
# 3853
 goto _LL3C0;case 11: _LL3BF: _LL3C0:
 goto _LL3C2;case 12: _LL3C1: _LL3C2:
 goto _LL3BC;default: _LL3C3: _LL3C4:
 v=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(t),v);goto _LL3BC;}_LL3BC:;}{
# 3859
void*_tmp6B8=_tmp6B4->access;void*_tmp6B9=_tmp6B8;struct Cyc_Absyn_Datatypedecl*_tmp6C4;struct Cyc_Absyn_Datatypefield*_tmp6C3;unsigned int _tmp6C2;int _tmp6C1;struct _dyneither_ptr*_tmp6C0;unsigned int _tmp6BF;switch(*((int*)_tmp6B9)){case 0: _LL3C6: _LL3C7:
# 3864
 goto _LL3C5;case 1: _LL3C8: _LL3C9:
# 3869
 if(ps->tl != 0){
void*_tmp6BA=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp6BB=_tmp6BA;struct Cyc_Absyn_Datatypedecl*_tmp6BE;struct Cyc_Absyn_Datatypefield*_tmp6BD;unsigned int _tmp6BC;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6BB)->tag == 3){_LL3D1: _tmp6BE=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6BB)->f1;_tmp6BD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6BB)->f2;_tmp6BC=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6BB)->f3;_LL3D2:
# 3872
 ps=ps->tl;
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6BD->name,_tmp6BE->name)),Cyc_Toc_mt_tq),v);
v=Cyc_Absyn_aggrarrow_exp(v,Cyc_Absyn_fieldname((int)(_tmp6BC + 1)),0);
continue;}else{_LL3D3: _LL3D4:
# 3877
 goto _LL3D0;}_LL3D0:;}
# 3880
v=Cyc_Absyn_deref_exp(v,0);
goto _LL3C5;case 2: _LL3CA: _tmp6BF=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp6B9)->f1;_LL3CB:
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6BF + 1)),0);goto _LL3C5;case 4: _LL3CC: _tmp6C1=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6B9)->f1;_tmp6C0=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6B9)->f2;_LL3CD:
# 3884
 v=Cyc_Toc_member_exp(v,_tmp6C0,0);
if(_tmp6C1)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0);
goto _LL3C5;default: _LL3CE: _tmp6C4=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B9)->f1;_tmp6C3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B9)->f2;_tmp6C2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B9)->f3;_LL3CF:
# 3889
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6C2 + 1)),0);
goto _LL3C5;}_LL3C5:;};}
# 3893
return v;}
# 3898
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp6C5=t;struct Cyc_Absyn_Datatypedecl*_tmp6E0;struct Cyc_Absyn_Datatypefield*_tmp6DF;struct _dyneither_ptr*_tmp6DE;int _tmp6DD;int _tmp6DC;struct Cyc_Absyn_Datatypedecl*_tmp6DB;struct Cyc_Absyn_Datatypefield*_tmp6DA;unsigned int _tmp6D9;struct _dyneither_ptr _tmp6D8;int _tmp6D7;void*_tmp6D6;struct Cyc_Absyn_Enumfield*_tmp6D5;struct Cyc_Absyn_Enumdecl*_tmp6D4;struct Cyc_Absyn_Enumfield*_tmp6D3;struct Cyc_Absyn_Exp*_tmp6D2;switch(*((int*)_tmp6C5)){case 0: _LL3D6: _tmp6D2=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6C5)->f1;_LL3D7:
# 3902
 if(_tmp6D2 == 0)return v;else{return _tmp6D2;}case 1: _LL3D8: _LL3D9:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 2: _LL3DA: _LL3DB:
 return Cyc_Absyn_neq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 3: _LL3DC: _tmp6D4=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6C5)->f1;_tmp6D3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6C5)->f2;_LL3DD:
# 3906
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6C7;_tmp6C7.tag=31;_tmp6C7.f1=_tmp6D4;_tmp6C7.f2=_tmp6D3;_tmp6C7;});_tmp6C6;}),0),0);case 4: _LL3DE: _tmp6D6=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6C5)->f1;_tmp6D5=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6C5)->f2;_LL3DF:
# 3908
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6C9;_tmp6C9.tag=32;_tmp6C9.f1=_tmp6D6;_tmp6C9.f2=_tmp6D5;_tmp6C9;});_tmp6C8;}),0),0);case 5: _LL3E0: _tmp6D8=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6C5)->f1;_tmp6D7=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6C5)->f2;_LL3E1:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_float_exp(_tmp6D8,_tmp6D7,0),0);case 6: _LL3E2: _tmp6D9=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6C5)->f1;_LL3E3:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp(_tmp6D9,0),0);case 7: _LL3E4: _tmp6DC=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6C5)->f1;_tmp6DB=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6C5)->f2;_tmp6DA=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6C5)->f3;_LL3E5:
# 3914
 LOOP1: {
void*_tmp6CA=v->r;void*_tmp6CB=_tmp6CA;struct Cyc_Absyn_Exp*_tmp6CD;struct Cyc_Absyn_Exp*_tmp6CC;switch(*((int*)_tmp6CB)){case 13: _LL3EB: _tmp6CC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6CB)->f2;_LL3EC:
 v=_tmp6CC;goto LOOP1;case 19: _LL3ED: _tmp6CD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6CB)->f1;_LL3EE:
 v=_tmp6CD;goto _LL3EA;default: _LL3EF: _LL3F0:
 goto _LL3EA;}_LL3EA:;}
# 3921
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6DA->name,_tmp6DB->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_uint_exp((unsigned int)_tmp6DC,0),0);case 8: _LL3E6: _tmp6DE=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6C5)->f1;_tmp6DD=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6C5)->f2;_LL3E7:
# 3924
 v=Cyc_Toc_member_exp(Cyc_Toc_member_exp(v,_tmp6DE,0),Cyc_Toc_tag_sp,0);
return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp((unsigned int)_tmp6DD,0),0);default: _LL3E8: _tmp6E0=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6C5)->f1;_tmp6DF=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6C5)->f2;_LL3E9:
# 3928
 LOOP2: {
void*_tmp6CE=v->r;void*_tmp6CF=_tmp6CE;struct Cyc_Absyn_Exp*_tmp6D1;struct Cyc_Absyn_Exp*_tmp6D0;switch(*((int*)_tmp6CF)){case 13: _LL3F2: _tmp6D0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6CF)->f2;_LL3F3:
 v=_tmp6D0;goto LOOP2;case 19: _LL3F4: _tmp6D1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6CF)->f1;_LL3F5:
 v=_tmp6D1;goto _LL3F1;default: _LL3F6: _LL3F7:
 goto _LL3F1;}_LL3F1:;}
# 3935
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6DF->name,_tmp6E0->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp6DF->name,0),0);}_LL3D5:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3947
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple32{int f1;void*f2;};
# 3955
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3957
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp6E3=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp6E4=_tmp6E3;void*_tmp6EE;_LL3F9: _tmp6EE=_tmp6E4.f1;_LL3FA:;{
void*_tmp6E5=_tmp6EE;struct Cyc_Absyn_Exp*_tmp6ED;struct _dyneither_ptr*_tmp6EC;switch(*((int*)_tmp6E5)){case 3: _LL3FC: _LL3FD:
# 3963
 goto _LL3FF;case 4: _LL3FE: _LL3FF:
 goto _LL401;case 6: _LL400: _LL401:
# 3966
 continue;case 8: _LL402: _tmp6EC=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6E5)->f1;_LL403:
# 3968
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmp6E7;_tmp6E7.tag=3;_tmp6E7.f1=_tmp6EC;_tmp6E7;});_tmp6E6;});
continue;case 7: _LL404: _LL405:
# 3972
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0: _LL406: _tmp6ED=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6E5)->f1;if(_tmp6ED != 0){_LL407:
# 3976
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8[0]=({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmp6E9;_tmp6E9.tag=2;_tmp6E9.f1=_tmp6ED;_tmp6E9;});_tmp6E8;});
return({struct _tuple32 _tmp6EA;_tmp6EA.f1=0;_tmp6EA.f2=k;_tmp6EA;});}else{_LL408: _LL409:
 goto _LL40B;}case 1: _LL40A: _LL40B:
 goto _LL40D;case 2: _LL40C: _LL40D:
 goto _LL40F;case 5: _LL40E: _LL40F:
 goto _LL411;default: _LL410: _LL411:
 return({struct _tuple32 _tmp6EB;_tmp6EB.f1=0;_tmp6EB.f2=k;_tmp6EB;});}_LL3FB:;};}
# 3985
return({struct _tuple32 _tmp6EF;_tmp6EF.f1=c;_tmp6EF.f2=k;_tmp6EF;});}
# 3990
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp6F0=p;int _tmp6FD;int _tmp6FC;unsigned int _tmp6FB;void*_tmp6FA;struct Cyc_Absyn_Enumfield*_tmp6F9;struct Cyc_Absyn_Enumdecl*_tmp6F8;struct Cyc_Absyn_Enumfield*_tmp6F7;switch(*((int*)_tmp6F0)){case 3: _LL413: _tmp6F8=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6F0)->f1;_tmp6F7=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6F0)->f2;_LL414:
# 3994
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6F1=_cycalloc(sizeof(*_tmp6F1));_tmp6F1[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6F2;_tmp6F2.tag=31;_tmp6F2.f1=_tmp6F8;_tmp6F2.f2=_tmp6F7;_tmp6F2;});_tmp6F1;}),0);goto _LL412;case 4: _LL415: _tmp6FA=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6F0)->f1;_tmp6F9=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6F0)->f2;_LL416:
# 3996
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6F4;_tmp6F4.tag=32;_tmp6F4.f1=_tmp6FA;_tmp6F4.f2=_tmp6F9;_tmp6F4;});_tmp6F3;}),0);goto _LL412;case 6: _LL417: _tmp6FB=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6F0)->f1;_LL418:
 _tmp6FC=(int)_tmp6FB;goto _LL41A;case 7: _LL419: _tmp6FC=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6F0)->f1;_LL41A:
 _tmp6FD=_tmp6FC;goto _LL41C;case 8: _LL41B: _tmp6FD=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6F0)->f2;_LL41C:
# 4000
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp6FD,0);
goto _LL412;default: _LL41D: _LL41E:
({void*_tmp6F5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6F6="compile_pat_test_as_case!";_tag_dyneither(_tmp6F6,sizeof(char),26);}),_tag_dyneither(_tmp6F5,sizeof(void*),0));});}_LL412:;}
# 4004
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp6FF;_tmp6FF.tag=17;_tmp6FF.f1=e;_tmp6FF;});_tmp6FE;}),0);}
# 4008
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0);}struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 4017
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 4020
void*t=(void*)_check_null(p->topt);
void*_tmp700=p->r;void*_tmp701=_tmp700;union Cyc_Absyn_AggrInfoU _tmp739;struct Cyc_List_List*_tmp738;struct Cyc_List_List*_tmp737;struct Cyc_List_List*_tmp736;struct Cyc_Absyn_Pat*_tmp735;struct Cyc_Absyn_Datatypedecl*_tmp734;struct Cyc_Absyn_Datatypefield*_tmp733;struct Cyc_List_List*_tmp732;struct Cyc_Absyn_Vardecl*_tmp731;struct Cyc_Absyn_Pat*_tmp730;struct Cyc_Absyn_Vardecl*_tmp72F;struct Cyc_Absyn_Vardecl*_tmp72E;struct Cyc_Absyn_Pat*_tmp72D;struct Cyc_Absyn_Vardecl*_tmp72C;switch(*((int*)_tmp701)){case 2: _LL420: _tmp72C=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp701)->f2;_LL421: {
# 4023
struct Cyc_Absyn_Pat*_tmp702=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp702->topt=p->topt;
_tmp72E=_tmp72C;_tmp72D=_tmp702;goto _LL423;}case 1: _LL422: _tmp72E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp701)->f1;_tmp72D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp701)->f2;_LL423:
# 4028
*decls=({struct Cyc_List_List*_tmp703=_region_malloc(rgn,sizeof(*_tmp703));_tmp703->hd=_tmp72E;_tmp703->tl=*decls;_tmp703;});{
struct Cyc_Absyn_Stmt*_tmp704=Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp706;_tmp706.tag=4;_tmp706.f1=_tmp72E;_tmp706;});_tmp705;}),0),Cyc_Absyn_copy_exp(path),0);
return Cyc_Toc_seq_stmt_opt(_tmp704,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp72D));};case 4: _LL424: _tmp72F=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp701)->f2;_LL425:
# 4033
*decls=({struct Cyc_List_List*_tmp707=_region_malloc(rgn,sizeof(*_tmp707));_tmp707->hd=_tmp72F;_tmp707->tl=*decls;_tmp707;});
return Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp709;_tmp709.tag=4;_tmp709.f1=_tmp72F;_tmp709;});_tmp708;}),0),Cyc_Absyn_copy_exp(path),0);case 0: _LL426: _LL427:
 return 0;case 3: _LL428: _tmp731=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp701)->f1;_tmp730=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp701)->f2;_LL429:
# 4038
*decls=({struct Cyc_List_List*_tmp70A=_region_malloc(rgn,sizeof(*_tmp70A));_tmp70A->hd=_tmp731;_tmp70A->tl=*decls;_tmp70A;});
_tmp731->type=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);{
# 4041
struct Cyc_Absyn_Stmt*_tmp70B=Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp70D;_tmp70D.tag=4;_tmp70D.f1=_tmp731;_tmp70D;});_tmp70C;}),0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path))),0);
return Cyc_Toc_seq_stmt_opt(_tmp70B,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp730));};case 9: _LL42A: _LL42B:
# 4046
 goto _LL42D;case 10: _LL42C: _LL42D:
 goto _LL42F;case 11: _LL42E: _LL42F:
 goto _LL431;case 12: _LL430: _LL431:
 goto _LL433;case 13: _LL432: _LL433:
 goto _LL435;case 14: _LL434: _LL435:
 return 0;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp701)->f1)->r)->tag == 8){_LL436: _tmp734=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp701)->f1)->r)->f1;_tmp733=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp701)->f1)->r)->f2;_tmp732=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp701)->f1)->r)->f3;_LL437:
# 4055
 if(_tmp732 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp733->name,_tmp734->name);
void*_tmp70E=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
path=Cyc_Toc_cast_it(_tmp70E,path);{
int cnt=1;
struct Cyc_List_List*_tmp70F=_tmp733->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp732 != 0;(((_tmp732=_tmp732->tl,_tmp70F=((struct Cyc_List_List*)_check_null(_tmp70F))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp710=(struct Cyc_Absyn_Pat*)_tmp732->hd;
if(_tmp710->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp711=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp70F))->hd)).f2;
void*_tmp712=(void*)_check_null(_tmp710->topt);
void*_tmp713=Cyc_Toc_typ_to_c_array(_tmp712);
struct Cyc_Absyn_Exp*_tmp714=Cyc_Absyn_aggrarrow_exp(path,Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp711)))
_tmp714=Cyc_Toc_cast_it(_tmp713,_tmp714);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp714,_tmp710));};}
# 4073
return s;};};}else{_LL440: _tmp735=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp701)->f1;_LL441:
# 4122
 return Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Absyn_deref_exp(path,0),_tmp735);}case 8: _LL438: _tmp736=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp701)->f3;_LL439:
# 4075
 _tmp737=_tmp736;goto _LL43B;case 5: _LL43A: _tmp737=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp701)->f1;_LL43B: {
# 4077
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp737 != 0;(_tmp737=_tmp737->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp715=(struct Cyc_Absyn_Pat*)_tmp737->hd;
if(_tmp715->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp716=(void*)_check_null(_tmp715->topt);
struct _dyneither_ptr*_tmp717=Cyc_Absyn_fieldname(cnt);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Toc_member_exp(path,_tmp717,0),_tmp715));};}
# 4087
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp701)->f1 == 0){_LL43C: _LL43D:
({void*_tmp718=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp719="unresolved aggregate pattern!";_tag_dyneither(_tmp719,sizeof(char),30);}),_tag_dyneither(_tmp718,sizeof(void*),0));});}else{_LL43E: _tmp739=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp701)->f1)->aggr_info;_tmp738=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp701)->f3;_LL43F: {
# 4091
struct Cyc_Absyn_Aggrdecl*_tmp71A=Cyc_Absyn_get_known_aggrdecl(_tmp739);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp738 != 0;_tmp738=_tmp738->tl){
struct _tuple33*_tmp71B=(struct _tuple33*)_tmp738->hd;
struct Cyc_Absyn_Pat*_tmp71C=(*_tmp71B).f2;
if(_tmp71C->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp71D=(void*)((struct Cyc_List_List*)_check_null((*_tmp71B).f1))->hd;void*_tmp71E=_tmp71D;struct _dyneither_ptr*_tmp721;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp71E)->tag == 1){_LL449: _tmp721=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp71E)->f1;_LL44A:
 f=_tmp721;goto _LL448;}else{_LL44B: _LL44C:
(int)_throw((void*)({struct Cyc_Toc_Match_error_exn_struct*_tmp71F=_cycalloc_atomic(sizeof(*_tmp71F));_tmp71F[0]=({struct Cyc_Toc_Match_error_exn_struct _tmp720;_tmp720.tag=Cyc_Toc_Match_error;_tmp720;});_tmp71F;}));}_LL448:;}{
# 4103
void*_tmp722=(void*)_check_null(_tmp71C->topt);
void*_tmp723=Cyc_Toc_typ_to_c_array(_tmp722);
struct Cyc_Absyn_Exp*_tmp724=Cyc_Toc_member_exp(path,f,0);
# 4107
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp71A->impl))->tagged)_tmp724=Cyc_Toc_member_exp(_tmp724,Cyc_Toc_val_sp,0);{
void*_tmp725=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp71A->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp725))
_tmp724=Cyc_Toc_cast_it(_tmp723,_tmp724);else{
if(!Cyc_Toc_is_array_type(_tmp725) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp725)))
# 4114
_tmp724=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp723,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp724,0)),0);}
# 4117
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp724,_tmp71C));};};};}
# 4119
return s;}}case 15: _LL442: _LL443:
# 4124
({void*_tmp726=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp727="unknownid pat";_tag_dyneither(_tmp727,sizeof(char),14);}),_tag_dyneither(_tmp726,sizeof(void*),0));});case 16: _LL444: _LL445:
({void*_tmp728=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp729="unknowncall pat";_tag_dyneither(_tmp729,sizeof(char),16);}),_tag_dyneither(_tmp728,sizeof(void*),0));});default: _LL446: _LL447:
({void*_tmp72A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp72B="exp pat";_tag_dyneither(_tmp72B,sizeof(char),8);}),_tag_dyneither(_tmp72A,sizeof(void*),0));});}_LL41F:;}struct _tuple34{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4135
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4143
void*_tmp73A=dopt;struct Cyc_List_List*_tmp779;struct Cyc_List_List*_tmp778;void*_tmp777;struct Cyc_Tcpat_Rhs*_tmp776;if(_tmp73A == 0){_LL44E: _LL44F:
# 4145
 return Cyc_Absyn_skip_stmt(0);}else{switch(*((int*)_tmp73A)){case 0: _LL450: _LL451:
 return Cyc_Toc_throw_match_stmt();case 1: _LL452: _tmp776=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp73A)->f1;_LL453:
# 4150
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp73B=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp73C=_tmp73B;int*_tmp743;struct _dyneither_ptr*_tmp742;struct _dyneither_ptr*_tmp741;struct Cyc_Absyn_Switch_clause*_tmp740;_LL457: _tmp743=(int*)& _tmp73C->f1;_tmp742=_tmp73C->f2;_tmp741=_tmp73C->f3;_tmp740=_tmp73C->f4;_LL458:;{
struct Cyc_Absyn_Stmt*_tmp73D=_tmp740->body;
if(_tmp73D == _tmp776->rhs){
# 4155
if(*_tmp743)
return Cyc_Absyn_goto_stmt(_tmp742,0);
*_tmp743=1;{
# 4159
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp740->pattern);
# 4162
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp741,_tmp740->body,0);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0);
res=Cyc_Absyn_label_stmt(_tmp742,res,0);
*bodies=({struct Cyc_List_List*_tmp73E=_region_malloc(rgn,sizeof(*_tmp73E));_tmp73E->hd=({struct _tuple34*_tmp73F=_region_malloc(rgn,sizeof(*_tmp73F));_tmp73F->f1=nv;_tmp73F->f2=_tmp741;_tmp73F->f3=_tmp73D;_tmp73F;});_tmp73E->tl=*bodies;_tmp73E;});
return res;};}};}
# 4170
({void*_tmp744=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp745="couldn't find rhs!";_tag_dyneither(_tmp745,sizeof(char),19);}),_tag_dyneither(_tmp744,sizeof(void*),0));});default: _LL454: _tmp779=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp73A)->f1;_tmp778=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp73A)->f2;_tmp777=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp73A)->f3;_LL455: {
# 4173
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp777,lscs,v);
# 4175
struct Cyc_Absyn_Exp*_tmp746=Cyc_Toc_compile_path(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp779),Cyc_Absyn_var_exp(v,0));
struct Cyc_List_List*_tmp747=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp778);
# 4178
struct _tuple32 _tmp748=Cyc_Toc_admits_switch(_tmp747);struct _tuple32 _tmp749=_tmp748;int _tmp775;void*_tmp774;_LL45A: _tmp775=_tmp749.f1;_tmp774=_tmp749.f2;_LL45B:;
if(_tmp775 > 1){
# 4182
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A->hd=({struct Cyc_Absyn_Switch_clause*_tmp75B=_cycalloc(sizeof(*_tmp75B));_tmp75B->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp75B->pat_vars=0;_tmp75B->where_clause=0;_tmp75B->body=res;_tmp75B->loc=0;_tmp75B;});_tmp75A->tl=0;_tmp75A;});
# 4185
for(0;_tmp747 != 0;_tmp747=_tmp747->tl){
struct _tuple29 _tmp74A=*((struct _tuple29*)_tmp747->hd);struct _tuple29 _tmp74B=_tmp74A;void*_tmp751;void*_tmp750;_LL45D: _tmp751=_tmp74B.f1;_tmp750=_tmp74B.f2;_LL45E:;{
# 4188
struct Cyc_Absyn_Pat*_tmp74C=Cyc_Toc_compile_pat_test_as_case(_tmp751);
# 4190
struct Cyc_Absyn_Stmt*_tmp74D=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp750,lscs,v);
# 4192
new_lscs=({struct Cyc_List_List*_tmp74E=_cycalloc(sizeof(*_tmp74E));_tmp74E->hd=({struct Cyc_Absyn_Switch_clause*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->pattern=_tmp74C;_tmp74F->pat_vars=0;_tmp74F->where_clause=0;_tmp74F->body=_tmp74D;_tmp74F->loc=0;_tmp74F;});_tmp74E->tl=new_lscs;_tmp74E;});};}
# 4194
{void*_tmp752=_tmp774;struct _dyneither_ptr*_tmp757;switch(*((int*)_tmp752)){case 1: _LL460: _LL461:
# 4197
 LOOP1: {
void*_tmp753=_tmp746->r;void*_tmp754=_tmp753;struct Cyc_Absyn_Exp*_tmp756;struct Cyc_Absyn_Exp*_tmp755;switch(*((int*)_tmp754)){case 13: _LL469: _tmp755=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp754)->f2;_LL46A:
 _tmp746=_tmp755;goto LOOP1;case 19: _LL46B: _tmp756=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp754)->f1;_LL46C:
 _tmp746=_tmp756;goto _LL468;default: _LL46D: _LL46E:
 goto _LL468;}_LL468:;}
# 4203
_tmp746=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),_tmp746),0);goto _LL45F;case 3: _LL462: _tmp757=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp752)->f1;_LL463:
# 4206
 _tmp746=Cyc_Toc_member_exp(Cyc_Toc_member_exp(_tmp746,_tmp757,0),Cyc_Toc_tag_sp,0);goto _LL45F;case 2: _LL464: _LL465:
# 4208
 goto _LL45F;default: _LL466: _LL467:
 goto _LL45F;}_LL45F:;}
# 4211
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp759;_tmp759.tag=10;_tmp759.f1=_tmp746;_tmp759.f2=new_lscs;_tmp759.f3=0;_tmp759;});_tmp758;}),0);}else{
# 4215
void*_tmp75C=_tmp774;struct Cyc_Absyn_Exp*_tmp773;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp75C)->tag == 2){_LL470: _tmp773=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp75C)->f1;_LL471: {
# 4217
struct Cyc_List_List*_tmp75D=_tmp747;void*_tmp76C;struct Cyc_Tcpat_Rhs*_tmp76B;if(_tmp75D != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp75D)->hd)->f2)->tag == 1){if(((struct Cyc_List_List*)_tmp75D)->tl == 0){_LL475: _tmp76C=((struct _tuple29*)_tmp75D->hd)->f1;_tmp76B=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp75D->hd)->f2)->f1;_LL476:
# 4223
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp75E=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp75F=_tmp75E;int*_tmp766;struct _dyneither_ptr*_tmp765;struct _dyneither_ptr*_tmp764;struct Cyc_Absyn_Switch_clause*_tmp763;_LL47A: _tmp766=(int*)& _tmp75F->f1;_tmp765=_tmp75F->f2;_tmp764=_tmp75F->f3;_tmp763=_tmp75F->f4;_LL47B:;{
struct Cyc_Absyn_Stmt*_tmp760=_tmp763->body;
if(_tmp760 == _tmp76B->rhs){
# 4228
if(*_tmp766)
return Cyc_Absyn_goto_stmt(_tmp765,0);
*_tmp766=1;{
# 4232
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp763->pattern);
# 4235
Cyc_Toc_exp_to_c(nv,_tmp773);{
# 4238
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp764,_tmp763->body,0);
r=Cyc_Absyn_ifthenelse_stmt(_tmp773,r,res,0);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0);
r=Cyc_Absyn_label_stmt(_tmp765,r,0);
*bodies=({struct Cyc_List_List*_tmp761=_region_malloc(rgn,sizeof(*_tmp761));_tmp761->hd=({struct _tuple34*_tmp762=_region_malloc(rgn,sizeof(*_tmp762));_tmp762->f1=nv;_tmp762->f2=_tmp764;_tmp762->f3=_tmp760;_tmp762;});_tmp761->tl=*bodies;_tmp761;});
return r;};};}};}
# 4247
({void*_tmp767=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp768="couldn't find rhs!";_tag_dyneither(_tmp768,sizeof(char),19);}),_tag_dyneither(_tmp767,sizeof(void*),0));});}else{goto _LL477;}}else{goto _LL477;}}else{_LL477: _LL478:
({void*_tmp769=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp76A="bad where clause in match compiler";_tag_dyneither(_tmp76A,sizeof(char),35);}),_tag_dyneither(_tmp769,sizeof(void*),0));});}_LL474:;}}else{_LL472: _LL473:
# 4252
 for(0;_tmp747 != 0;_tmp747=_tmp747->tl){
struct _tuple29 _tmp76D=*((struct _tuple29*)_tmp747->hd);struct _tuple29 _tmp76E=_tmp76D;void*_tmp772;void*_tmp771;_LL47D: _tmp772=_tmp76E.f1;_tmp771=_tmp76E.f2;_LL47E:;{
struct Cyc_Absyn_Exp*_tmp76F=Cyc_Toc_compile_pat_test(_tmp746,_tmp772);
struct Cyc_Absyn_Stmt*_tmp770=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp771,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp76F,_tmp770,res,0);};}}_LL46F:;}
# 4260
return res;}}}_LL44D:;}
# 4270
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4272
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple34*_tmp77A=(struct _tuple34*)bodies->hd;struct _tuple34*_tmp77B=_tmp77A;struct Cyc_Toc_Env**_tmp77D;struct Cyc_Absyn_Stmt*_tmp77C;_LL480: _tmp77D=(struct Cyc_Toc_Env**)& _tmp77B->f1;_tmp77C=_tmp77B->f3;_LL481:;
if(_tmp77C == s)return _tmp77D;}
# 4278
return 0;}
# 4282
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4285
void*_tmp77E=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4288
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp77F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp77F;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp780=Cyc_Toc_share_env(rgn,nv);
# 4295
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4300
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp780,& mydecls,& mybodies,dopt,lscs,v);
# 4308
{struct Cyc_List_List*_tmp781=lscs;for(0;_tmp781 != 0;_tmp781=_tmp781->tl){
struct _tuple31*_tmp782=(struct _tuple31*)_tmp781->hd;struct _tuple31*_tmp783=_tmp782;struct _dyneither_ptr*_tmp78B;struct Cyc_Absyn_Switch_clause*_tmp78A;_LL483: _tmp78B=_tmp783->f3;_tmp78A=_tmp783->f4;_LL484:;{
struct Cyc_Absyn_Stmt*s=_tmp78A->body;
# 4312
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4315
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp784=*envp;
# 4318
if(_tmp781->tl != 0){
struct _tuple31*_tmp785=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp781->tl))->hd;struct _tuple31*_tmp786=_tmp785;struct _dyneither_ptr*_tmp789;struct Cyc_Absyn_Switch_clause*_tmp788;_LL486: _tmp789=_tmp786->f3;_tmp788=_tmp786->f4;_LL487:;{
# 4322
struct Cyc_Toc_Env**_tmp787=Cyc_Toc_find_case_env(mybodies,_tmp788->body);
# 4327
if(_tmp787 == 0)
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp784,end_l),s);else{
# 4331
struct Cyc_List_List*vs=0;
if(_tmp788->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp788->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 4336
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn,_tmp784,end_l,_tmp789,vs),s);}};}else{
# 4340
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp784,end_l),s);}};};}}{
# 4344
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4346
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp78C=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
_tmp78C->name=Cyc_Toc_temp_var();
_tmp78C->type=Cyc_Toc_typ_to_c_array(_tmp78C->type);
res=Cyc_Absyn_decl_stmt(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp78D=_cycalloc(sizeof(*_tmp78D));_tmp78D[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp78E;_tmp78E.tag=0;_tmp78E.f1=_tmp78C;_tmp78E;});_tmp78D;}),0),res,0);}
# 4353
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;
_npop_handler(0);return;};}
# 4292
;_pop_region(rgn);};}
# 4359
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 4361
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4366
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4368
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp78F=_cycalloc(sizeof(*_tmp78F));_tmp78F->hd=
Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp78F->tl=0;_tmp78F;}),0),0);}
# 4372
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4377
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4379
while(1){
void*_tmp790=s->r;void*_tmp791=_tmp790;struct Cyc_Absyn_Exp*_tmp7EF;struct Cyc_Absyn_Stmt*_tmp7EE;struct Cyc_List_List*_tmp7ED;void*_tmp7EC;struct Cyc_Absyn_Stmt*_tmp7EB;struct Cyc_Absyn_Exp*_tmp7EA;struct _dyneither_ptr*_tmp7E9;struct Cyc_Absyn_Stmt*_tmp7E8;struct Cyc_Absyn_Decl*_tmp7E7;struct Cyc_Absyn_Stmt*_tmp7E6;struct Cyc_List_List*_tmp7E5;struct Cyc_Absyn_Switch_clause**_tmp7E4;struct Cyc_Absyn_Exp*_tmp7E3;struct Cyc_List_List*_tmp7E2;void*_tmp7E1;struct Cyc_Absyn_Exp*_tmp7E0;struct Cyc_Absyn_Exp*_tmp7DF;struct Cyc_Absyn_Exp*_tmp7DE;struct Cyc_Absyn_Stmt*_tmp7DD;struct Cyc_Absyn_Exp*_tmp7DC;struct Cyc_Absyn_Stmt*_tmp7DB;struct Cyc_Absyn_Exp*_tmp7DA;struct Cyc_Absyn_Stmt*_tmp7D9;struct Cyc_Absyn_Stmt*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7D7;struct Cyc_Absyn_Stmt*_tmp7D6;struct Cyc_Absyn_Stmt*_tmp7D5;struct Cyc_Absyn_Exp*_tmp7D4;switch(*((int*)_tmp791)){case 0: _LL489: _LL48A:
# 4382
 return;case 1: _LL48B: _tmp7D4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_LL48C:
# 4384
 Cyc_Toc_exp_to_c(nv,_tmp7D4);
return;case 2: _LL48D: _tmp7D6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_tmp7D5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp791)->f2;_LL48E:
# 4387
 Cyc_Toc_stmt_to_c(nv,_tmp7D6);
s=_tmp7D5;
continue;case 3: _LL48F: _tmp7D7=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_LL490: {
# 4391
void*topt=0;
if(_tmp7D7 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7D7->topt));
Cyc_Toc_exp_to_c(nv,_tmp7D7);}{
# 4397
int _tmp792=Cyc_Toc_get_npop(s);
if(_tmp792 > 0){
if(topt != 0){
struct _tuple1*_tmp793=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp794=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp793,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp793,topt,_tmp7D7,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(_tmp792),_tmp794,0),0))->r;}else{
# 4406
Cyc_Toc_do_npop_before(_tmp792,s);}}
# 4408
return;};}case 4: _LL491: _tmp7DA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_tmp7D9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp791)->f2;_tmp7D8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp791)->f3;_LL492:
# 4410
 Cyc_Toc_exp_to_c(nv,_tmp7DA);
Cyc_Toc_stmt_to_c(nv,_tmp7D9);
s=_tmp7D8;
continue;case 5: _LL493: _tmp7DC=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp791)->f1).f1;_tmp7DB=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp791)->f2;_LL494:
# 4415
 Cyc_Toc_exp_to_c(nv,_tmp7DC);{
struct _RegionHandle _tmp795=_new_region("temp");struct _RegionHandle*temp=& _tmp795;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7DB);
# 4419
_npop_handler(0);return;
# 4416
;_pop_region(temp);};case 6: _LL495: _LL496: {
# 4421
struct Cyc_Toc_Env*_tmp796=nv;struct _dyneither_ptr**_tmp797;_LL4AC: _tmp797=_tmp796->break_lab;_LL4AD:;
if(_tmp797 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp797);
# 4425
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;}case 7: _LL497: _LL498: {
# 4428
struct Cyc_Toc_Env*_tmp798=nv;struct _dyneither_ptr**_tmp799;_LL4AF: _tmp799=_tmp798->continue_lab;_LL4B0:;
if(_tmp799 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp799);
goto _LL49A;}case 8: _LL499: _LL49A:
# 4434
 Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;case 9: _LL49B: _tmp7E0=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_tmp7DF=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp791)->f2).f1;_tmp7DE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp791)->f3).f1;_tmp7DD=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp791)->f4;_LL49C:
# 4438
 Cyc_Toc_exp_to_c(nv,_tmp7E0);Cyc_Toc_exp_to_c(nv,_tmp7DF);Cyc_Toc_exp_to_c(nv,_tmp7DE);{
struct _RegionHandle _tmp79A=_new_region("temp");struct _RegionHandle*temp=& _tmp79A;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7DD);
# 4442
_npop_handler(0);return;
# 4439
;_pop_region(temp);};case 10: _LL49D: _tmp7E3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_tmp7E2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp791)->f2;_tmp7E1=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp791)->f3;_LL49E:
# 4444
 Cyc_Toc_xlate_switch(nv,s,_tmp7E3,_tmp7E2,_tmp7E1);
return;case 11: _LL49F: _tmp7E5=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_tmp7E4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp791)->f2;_LL4A0: {
# 4447
struct Cyc_Toc_Env*_tmp79B=nv;struct Cyc_Toc_FallthruInfo*_tmp7A6;_LL4B2: _tmp7A6=_tmp79B->fallthru_info;_LL4B3:;
if(_tmp7A6 == 0)
({void*_tmp79C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp79D="fallthru in unexpected place";_tag_dyneither(_tmp79D,sizeof(char),29);}),_tag_dyneither(_tmp79C,sizeof(void*),0));});{
struct Cyc_Toc_FallthruInfo _tmp79E=*_tmp7A6;struct Cyc_Toc_FallthruInfo _tmp79F=_tmp79E;struct _dyneither_ptr*_tmp7A5;struct Cyc_List_List*_tmp7A4;_LL4B5: _tmp7A5=_tmp79F.label;_tmp7A4=_tmp79F.binders;_LL4B6:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7A5,0);
# 4453
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s2);{
struct Cyc_List_List*_tmp7A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7A4);
struct Cyc_List_List*_tmp7A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7E5);
for(0;_tmp7A0 != 0;(_tmp7A0=_tmp7A0->tl,_tmp7A1=_tmp7A1->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A1))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp7A3;_tmp7A3.tag=5;_tmp7A3.f1=(struct Cyc_Absyn_Vardecl*)_tmp7A0->hd;_tmp7A3;});_tmp7A2;}),0),(struct Cyc_Absyn_Exp*)_tmp7A1->hd,0),s2,0);}
# 4461
s->r=s2->r;
return;};};};}case 12: _LL4A1: _tmp7E7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_tmp7E6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp791)->f2;_LL4A2:
# 4467
{void*_tmp7A7=_tmp7E7->r;void*_tmp7A8=_tmp7A7;struct Cyc_Absyn_Tvar*_tmp7C6;struct Cyc_Absyn_Vardecl*_tmp7C5;int _tmp7C4;struct Cyc_Absyn_Exp*_tmp7C3;struct Cyc_Absyn_Fndecl*_tmp7C2;struct Cyc_List_List*_tmp7C1;struct Cyc_Absyn_Pat*_tmp7C0;struct Cyc_Absyn_Exp*_tmp7BF;void*_tmp7BE;struct Cyc_Absyn_Vardecl*_tmp7BD;switch(*((int*)_tmp7A8)){case 0: _LL4B8: _tmp7BD=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7A8)->f1;_LL4B9:
 Cyc_Toc_local_decl_to_c(nv,_tmp7BD,_tmp7E6);goto _LL4B7;case 2: _LL4BA: _tmp7C0=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7A8)->f1;_tmp7BF=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7A8)->f3;_tmp7BE=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7A8)->f4;_LL4BB:
# 4475
{void*_tmp7A9=_tmp7C0->r;void*_tmp7AA=_tmp7A9;struct Cyc_Absyn_Vardecl*_tmp7AD;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AA)->tag == 1){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AA)->f2)->r)->tag == 0){_LL4C5: _tmp7AD=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AA)->f1;_LL4C6:
# 4477
 _tmp7AD->name=Cyc_Toc_temp_var();
_tmp7AD->initializer=_tmp7BF;
_tmp7E7->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7AB=_cycalloc(sizeof(*_tmp7AB));_tmp7AB[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7AC;_tmp7AC.tag=0;_tmp7AC.f1=_tmp7AD;_tmp7AC;});_tmp7AB;});
Cyc_Toc_local_decl_to_c(nv,_tmp7AD,_tmp7E6);
goto _LL4C4;}else{goto _LL4C7;}}else{_LL4C7: _LL4C8:
# 4486
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7C0,_tmp7BE,(void*)_check_null(_tmp7BF->topt),_tmp7BF,_tmp7E6))->r;
goto _LL4C4;}_LL4C4:;}
# 4489
goto _LL4B7;case 3: _LL4BC: _tmp7C1=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7A8)->f1;_LL4BD: {
# 4493
struct Cyc_List_List*_tmp7AE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7C1);
if(_tmp7AE == 0)
({void*_tmp7AF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7B0="empty Letv_d";_tag_dyneither(_tmp7B0,sizeof(char),13);}),_tag_dyneither(_tmp7AF,sizeof(void*),0));});
_tmp7E7->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));_tmp7B1[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7B2;_tmp7B2.tag=0;_tmp7B2.f1=(struct Cyc_Absyn_Vardecl*)_tmp7AE->hd;_tmp7B2;});_tmp7B1;});
_tmp7AE=_tmp7AE->tl;
for(0;_tmp7AE != 0;_tmp7AE=_tmp7AE->tl){
struct Cyc_Absyn_Decl*_tmp7B3=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7B5;_tmp7B5.tag=0;_tmp7B5.f1=(struct Cyc_Absyn_Vardecl*)_tmp7AE->hd;_tmp7B5;});_tmp7B4;}),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7B3,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4502
Cyc_Toc_stmt_to_c(nv,s);
goto _LL4B7;}case 1: _LL4BE: _tmp7C2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7A8)->f1;_LL4BF:
# 4505
 Cyc_Toc_fndecl_to_c(nv,_tmp7C2,0);
Cyc_Toc_stmt_to_c(nv,_tmp7E6);
goto _LL4B7;case 4: _LL4C0: _tmp7C6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A8)->f1;_tmp7C5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A8)->f2;_tmp7C4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A8)->f3;_tmp7C3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A8)->f4;_LL4C1: {
# 4509
struct Cyc_Absyn_Stmt*_tmp7B6=_tmp7E6;
# 4517
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7C5->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4524
Cyc_Toc_stmt_to_c(nv,_tmp7B6);
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp7B6,0))->r;else{
if((unsigned int)_tmp7C3){
Cyc_Toc_exp_to_c(nv,_tmp7C3);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp7C3,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp7B6,0))->r;};}else{
# 4533
s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8->hd=
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0);_tmp7B8->tl=0;_tmp7B8;}),0),
# 4538
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*_tmp7B9[1];_tmp7B9[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7B9,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),
Cyc_Absyn_seq_stmt(_tmp7B6,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp7BA[1];_tmp7BA[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7BA,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r;}}
# 4544
return;}default: _LL4C2: _LL4C3:
({void*_tmp7BB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7BC="bad nested declaration within function";_tag_dyneither(_tmp7BC,sizeof(char),39);}),_tag_dyneither(_tmp7BB,sizeof(void*),0));});}_LL4B7:;}
# 4547
return;case 13: _LL4A3: _tmp7E9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_tmp7E8=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp791)->f2;_LL4A4:
# 4549
 s=_tmp7E8;continue;case 14: _LL4A5: _tmp7EB=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_tmp7EA=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp791)->f2).f1;_LL4A6: {
# 4551
struct _RegionHandle _tmp7C7=_new_region("temp");struct _RegionHandle*temp=& _tmp7C7;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7EB);
Cyc_Toc_exp_to_c(nv,_tmp7EA);
# 4555
_npop_handler(0);return;
# 4551
;_pop_region(temp);}case 15: _LL4A7: _tmp7EE=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_tmp7ED=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp791)->f2;_tmp7EC=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp791)->f3;_LL4A8: {
# 4569 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4579
e_exp->topt=e_typ;{
struct _RegionHandle _tmp7C8=_new_region("temp");struct _RegionHandle*temp=& _tmp7C8;_push_region(temp);
# 4582
Cyc_Toc_stmt_to_c(nv,_tmp7EE);{
struct Cyc_Absyn_Stmt*_tmp7C9=Cyc_Absyn_seq_stmt(_tmp7EE,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4587
struct Cyc_Absyn_Stmt*_tmp7CA=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7D1=_cycalloc(sizeof(*_tmp7D1));_tmp7D1[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp7D2;_tmp7D2.tag=10;_tmp7D2.f1=e_exp;_tmp7D2.f2=_tmp7ED;_tmp7D2.f3=_tmp7EC;_tmp7D2;});_tmp7D1;}),0);
# 4589
Cyc_Toc_stmt_to_c(nv,_tmp7CA);{
# 4592
struct Cyc_Absyn_Exp*_tmp7CB=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp7D0=_cycalloc(sizeof(*_tmp7D0));_tmp7D0->hd=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp7D0->tl=0;_tmp7D0;}),0);
# 4596
struct Cyc_Absyn_Stmt*_tmp7CC=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));_tmp7CF->hd=
Cyc_Absyn_address_exp(h_exp,0);_tmp7CF->tl=0;_tmp7CF;}),0),0);
# 4600
struct Cyc_Absyn_Exp*_tmp7CD=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp7CE=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp7CC,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp7CD,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp7CB,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp7CE,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp7C9,
# 4611
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp7CA,0),0),0),0),0),0))->r;};};
# 4615
_npop_handler(0);return;
# 4580
;_pop_region(temp);};}default: _LL4A9: _tmp7EF=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp791)->f1;_LL4AA:
# 4617
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4620
Cyc_Toc_exp_to_c(nv,_tmp7EF);
s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp7D3=_cycalloc(sizeof(*_tmp7D3));_tmp7D3->hd=_tmp7EF;_tmp7D3->tl=0;_tmp7D3;}),0));}
# 4623
return;}_LL488:;}}
# 4632
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp7F0=_new_region("frgn");struct _RegionHandle*frgn=& _tmp7F0;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp7F1=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp7F2=f->args;for(0;_tmp7F2 != 0;_tmp7F2=_tmp7F2->tl){
struct _tuple1*_tmp7F3=({struct _tuple1*_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4->f1=Cyc_Absyn_Loc_n;_tmp7F4->f2=(*((struct _tuple9*)_tmp7F2->hd)).f1;_tmp7F4;});
(*((struct _tuple9*)_tmp7F2->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp7F2->hd)).f3);}}
# 4648
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4652
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp7F5=_cycalloc(sizeof(*_tmp7F5));_tmp7F5[0]=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);_tmp7F5;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp7F6=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp7F7=_tmp7F6;struct _dyneither_ptr*_tmp800;struct Cyc_Absyn_Tqual _tmp7FF;void*_tmp7FE;int _tmp7FD;_LL4CA: _tmp800=_tmp7F7.name;_tmp7FF=_tmp7F7.tq;_tmp7FE=_tmp7F7.type;_tmp7FD=_tmp7F7.inject;_LL4CB:;{
void*_tmp7F8=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp7FE,(void*)& Cyc_Absyn_HeapRgn_val,_tmp7FF,Cyc_Absyn_false_conref));
struct _tuple1*_tmp7F9=({struct _tuple1*_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC->f1=Cyc_Absyn_Loc_n;_tmp7FC->f2=(struct _dyneither_ptr*)_check_null(_tmp800);_tmp7FC;});
f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA->hd=({struct _tuple9*_tmp7FB=_cycalloc(sizeof(*_tmp7FB));_tmp7FB->f1=_tmp800;_tmp7FB->f2=_tmp7FF;_tmp7FB->f3=_tmp7F8;_tmp7FB;});_tmp7FA->tl=0;_tmp7FA;}));
f->cyc_varargs=0;};}
# 4661
{struct Cyc_List_List*_tmp801=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp801 != 0;_tmp801=_tmp801->tl){
((struct Cyc_Absyn_Vardecl*)_tmp801->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp801->hd)->type);}}
# 4664
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp7F1),f->body);}
# 4641
;_pop_region(frgn);};}
# 4669
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp802=s;switch(_tmp802){case Cyc_Absyn_Abstract: _LL4CD: _LL4CE:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL4CF: _LL4D0:
 return Cyc_Absyn_Extern;default: _LL4D1: _LL4D2:
 return s;}_LL4CC:;}struct _tuple35{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple36{struct Cyc_Toc_TocState*f1;struct _tuple35*f2;};
# 4686 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple36*env){
# 4689
struct _tuple36 _tmp803=*env;struct _tuple36 _tmp804=_tmp803;struct Cyc_Toc_TocState*_tmp843;struct Cyc_Absyn_Aggrdecl*_tmp842;int _tmp841;_LL4D4: _tmp843=_tmp804.f1;_tmp842=(_tmp804.f2)->f1;_tmp841=(_tmp804.f2)->f2;_LL4D5:;{
struct _tuple1*_tmp805=_tmp842->name;
struct Cyc_Toc_TocState _tmp806=*_tmp843;struct Cyc_Toc_TocState _tmp807=_tmp806;struct Cyc_Dict_Dict*_tmp840;_LL4D7: _tmp840=_tmp807.aggrs_so_far;_LL4D8:;{
int seen_defn_before;
struct _tuple18**_tmp808=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp840,_tmp805);
if(_tmp808 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp842->kind == Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp809=_region_malloc(d,sizeof(*_tmp809));_tmp809->f1=_tmp842;_tmp809->f2=Cyc_Absyn_strctq(_tmp805);_tmp809;});else{
# 4700
v=({struct _tuple18*_tmp80A=_region_malloc(d,sizeof(*_tmp80A));_tmp80A->f1=_tmp842;_tmp80A->f2=Cyc_Absyn_unionq_typ(_tmp805);_tmp80A;});}
*_tmp840=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp840,_tmp805,v);};}else{
# 4703
struct _tuple18*_tmp80B=*_tmp808;struct _tuple18*_tmp80C=_tmp80B;struct Cyc_Absyn_Aggrdecl*_tmp80F;void*_tmp80E;_LL4DA: _tmp80F=_tmp80C->f1;_tmp80E=_tmp80C->f2;_LL4DB:;
if(_tmp80F->impl == 0){
*_tmp840=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp840,_tmp805,({struct _tuple18*_tmp80D=_region_malloc(d,sizeof(*_tmp80D));_tmp80D->f1=_tmp842;_tmp80D->f2=_tmp80E;_tmp80D;}));
seen_defn_before=0;}else{
# 4708
seen_defn_before=1;}}{
# 4710
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp83F=_cycalloc(sizeof(*_tmp83F));_tmp83F->kind=_tmp842->kind;_tmp83F->sc=Cyc_Absyn_Public;_tmp83F->name=_tmp842->name;_tmp83F->tvs=0;_tmp83F->impl=0;_tmp83F->expected_mem_kind=0;_tmp83F->attributes=_tmp842->attributes;_tmp83F;});
# 4717
if(_tmp842->impl != 0  && !seen_defn_before){
new_ad->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp810=_cycalloc(sizeof(*_tmp810));_tmp810->exist_vars=0;_tmp810->rgn_po=0;_tmp810->fields=0;_tmp810->tagged=0;_tmp810;});{
# 4722
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp811=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp842->impl))->fields;for(0;_tmp811 != 0;_tmp811=_tmp811->tl){
# 4726
struct Cyc_Absyn_Aggrfield*_tmp812=(struct Cyc_Absyn_Aggrfield*)_tmp811->hd;
void*_tmp813=_tmp812->type;
struct Cyc_List_List*_tmp814=_tmp812->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp813)) && (
_tmp842->kind == Cyc_Absyn_StructA  && _tmp811->tl == 0  || _tmp842->kind == Cyc_Absyn_UnionA)){
# 4740 "toc.cyc"
void*_tmp815=Cyc_Tcutil_compress(_tmp813);void*_tmp816=_tmp815;void*_tmp823;struct Cyc_Absyn_Tqual _tmp822;union Cyc_Absyn_Constraint*_tmp821;unsigned int _tmp820;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp816)->tag == 8){_LL4DD: _tmp823=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp816)->f1).elt_type;_tmp822=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp816)->f1).tq;_tmp821=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp816)->f1).zero_term;_tmp820=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp816)->f1).zt_loc;_LL4DE:
# 4743
 _tmp813=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp817=_cycalloc(sizeof(*_tmp817));_tmp817[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp818;_tmp818.tag=8;_tmp818.f1=({struct Cyc_Absyn_ArrayInfo _tmp819;_tmp819.elt_type=_tmp823;_tmp819.tq=_tmp822;_tmp819.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp819.zero_term=_tmp821;_tmp819.zt_loc=_tmp820;_tmp819;});_tmp818;});_tmp817;});
goto _LL4DC;}else{_LL4DF: _LL4E0:
# 4746
 _tmp814=({struct Cyc_List_List*_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp81C;_tmp81C.tag=6;_tmp81C.f1=0;_tmp81C;});_tmp81B;});_tmp81A->tl=_tmp814;_tmp81A;});
_tmp813=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp81D=_cycalloc(sizeof(*_tmp81D));_tmp81D[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp81E;_tmp81E.tag=8;_tmp81E.f1=({struct Cyc_Absyn_ArrayInfo _tmp81F;_tmp81F.elt_type=Cyc_Absyn_void_star_typ();_tmp81F.tq=
Cyc_Absyn_empty_tqual(0);_tmp81F.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp81F.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp81F.zt_loc=0;_tmp81F;});_tmp81E;});_tmp81D;});}_LL4DC:;}{
# 4753
struct Cyc_Absyn_Aggrfield*_tmp824=({struct Cyc_Absyn_Aggrfield*_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A->name=_tmp812->name;_tmp83A->tq=Cyc_Toc_mt_tq;_tmp83A->type=
# 4755
Cyc_Toc_typ_to_c(_tmp813);_tmp83A->width=_tmp812->width;_tmp83A->attributes=_tmp814;_tmp83A->requires_clause=0;_tmp83A;});
# 4763
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp842->impl))->tagged){
void*_tmp825=_tmp824->type;
struct _dyneither_ptr*_tmp826=_tmp824->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp838;_tmp838.tag=0;_tmp838.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp826);({struct Cyc_String_pa_PrintArg_struct _tmp837;_tmp837.tag=0;_tmp837.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp842->name).f2);({void*_tmp835[2]={& _tmp837,& _tmp838};Cyc_aprintf(({const char*_tmp836="_union_%s_%s";_tag_dyneither(_tmp836,sizeof(char),13);}),_tag_dyneither(_tmp835,sizeof(void*),2));});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp834=_cycalloc(sizeof(*_tmp834));_tmp834[0]=s;_tmp834;});
struct Cyc_Absyn_Aggrfield*_tmp827=({struct Cyc_Absyn_Aggrfield*_tmp833=_cycalloc(sizeof(*_tmp833));_tmp833->name=Cyc_Toc_val_sp;_tmp833->tq=Cyc_Toc_mt_tq;_tmp833->type=_tmp825;_tmp833->width=0;_tmp833->attributes=0;_tmp833->requires_clause=0;_tmp833;});
struct Cyc_Absyn_Aggrfield*_tmp828=({struct Cyc_Absyn_Aggrfield*_tmp832=_cycalloc(sizeof(*_tmp832));_tmp832->name=Cyc_Toc_tag_sp;_tmp832->tq=Cyc_Toc_mt_tq;_tmp832->type=Cyc_Absyn_sint_typ;_tmp832->width=0;_tmp832->attributes=0;_tmp832->requires_clause=0;_tmp832;});
struct Cyc_List_List*_tmp829=({struct Cyc_Absyn_Aggrfield*_tmp831[2];_tmp831[1]=_tmp827;_tmp831[0]=_tmp828;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp831,sizeof(struct Cyc_Absyn_Aggrfield*),2));});
struct Cyc_Absyn_Aggrdecl*_tmp82A=({struct Cyc_Absyn_Aggrdecl*_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E->kind=Cyc_Absyn_StructA;_tmp82E->sc=Cyc_Absyn_Public;_tmp82E->name=({struct _tuple1*_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830->f1=Cyc_Absyn_Loc_n;_tmp830->f2=str;_tmp830;});_tmp82E->tvs=0;_tmp82E->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F->exist_vars=0;_tmp82F->rgn_po=0;_tmp82F->fields=_tmp829;_tmp82F->tagged=0;_tmp82F;});_tmp82E->expected_mem_kind=0;_tmp82E->attributes=0;_tmp82E;});
# 4776
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp82D;_tmp82D.tag=5;_tmp82D.f1=_tmp82A;_tmp82D;});_tmp82C;}),0);_tmp82B->tl=Cyc_Toc_result_decls;_tmp82B;});
_tmp824->type=Cyc_Absyn_strct(str);}
# 4779
new_fields=({struct Cyc_List_List*_tmp839=_cycalloc(sizeof(*_tmp839));_tmp839->hd=_tmp824;_tmp839->tl=new_fields;_tmp839;});};}}
# 4781
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4783
if(_tmp841)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp83B=_cycalloc(sizeof(*_tmp83B));_tmp83B->hd=({struct Cyc_Absyn_Decl*_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp83D=_cycalloc(sizeof(*_tmp83D));_tmp83D[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp83E;_tmp83E.tag=5;_tmp83E.f1=new_ad;_tmp83E;});_tmp83D;});_tmp83C->loc=0;_tmp83C;});_tmp83B->tl=Cyc_Toc_result_decls;_tmp83B;});
return 0;};};};}
# 4788
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple35 p=({struct _tuple35 _tmp844;_tmp844.f1=ad;_tmp844.f2=add_to_decls;_tmp844;});
((int(*)(struct _tuple35*arg,int(*f)(struct _RegionHandle*,struct _tuple36*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple37{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4817 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4820
struct _tuple37 _tmp845=*env;struct _tuple37 _tmp846=_tmp845;struct Cyc_Set_Set**_tmp858;struct Cyc_Absyn_Datatypedecl*_tmp857;_LL4E2: _tmp858=(_tmp846.f1)->datatypes_so_far;_tmp857=_tmp846.f2;_LL4E3:;{
struct _tuple1*_tmp847=_tmp857->name;
if(_tmp857->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp858,_tmp847))
return 0;
*_tmp858=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp858,_tmp847);
{struct Cyc_List_List*_tmp848=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp857->fields))->v;for(0;_tmp848 != 0;_tmp848=_tmp848->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp848->hd;
# 4828
struct Cyc_List_List*_tmp849=0;
int i=1;
{struct Cyc_List_List*_tmp84A=f->typs;for(0;_tmp84A != 0;(_tmp84A=_tmp84A->tl,i ++)){
struct _dyneither_ptr*_tmp84B=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp84C=({struct Cyc_Absyn_Aggrfield*_tmp84E=_cycalloc(sizeof(*_tmp84E));_tmp84E->name=_tmp84B;_tmp84E->tq=(*((struct _tuple11*)_tmp84A->hd)).f1;_tmp84E->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp84A->hd)).f2);_tmp84E->width=0;_tmp84E->attributes=0;_tmp84E->requires_clause=0;_tmp84E;});
_tmp849=({struct Cyc_List_List*_tmp84D=_cycalloc(sizeof(*_tmp84D));_tmp84D->hd=_tmp84C;_tmp84D->tl=_tmp849;_tmp84D;});}}
# 4836
_tmp849=({struct Cyc_List_List*_tmp84F=_cycalloc(sizeof(*_tmp84F));_tmp84F->hd=({struct Cyc_Absyn_Aggrfield*_tmp850=_cycalloc(sizeof(*_tmp850));_tmp850->name=Cyc_Toc_tag_sp;_tmp850->tq=Cyc_Toc_mt_tq;_tmp850->type=Cyc_Absyn_sint_typ;_tmp850->width=0;_tmp850->attributes=0;_tmp850->requires_clause=0;_tmp850;});_tmp84F->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp849);_tmp84F;});{
struct Cyc_Absyn_Aggrdecl*_tmp851=({struct Cyc_Absyn_Aggrdecl*_tmp855=_cycalloc(sizeof(*_tmp855));_tmp855->kind=Cyc_Absyn_StructA;_tmp855->sc=Cyc_Absyn_Public;_tmp855->name=
# 4840
Cyc_Toc_collapse_qvars(f->name,_tmp857->name);_tmp855->tvs=0;_tmp855->expected_mem_kind=0;_tmp855->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp856=_cycalloc(sizeof(*_tmp856));_tmp856->exist_vars=0;_tmp856->rgn_po=0;_tmp856->fields=_tmp849;_tmp856->tagged=0;_tmp856;});_tmp855->attributes=0;_tmp855;});
# 4844
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp852=_cycalloc(sizeof(*_tmp852));_tmp852->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp853=_cycalloc(sizeof(*_tmp853));_tmp853[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp854;_tmp854.tag=5;_tmp854.f1=_tmp851;_tmp854;});_tmp853;}),0);_tmp852->tl=Cyc_Toc_result_decls;_tmp852;});};}}
# 4846
return 0;};}
# 4849
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4868 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4871
struct _tuple37 _tmp859=*env;struct _tuple37 _tmp85A=_tmp859;struct Cyc_Toc_TocState*_tmp87F;struct Cyc_Absyn_Datatypedecl*_tmp87E;_LL4E5: _tmp87F=_tmp85A.f1;_tmp87E=_tmp85A.f2;_LL4E6:;
if(_tmp87E->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp85B=*_tmp87F;struct Cyc_Toc_TocState _tmp85C=_tmp85B;struct Cyc_Dict_Dict*_tmp87D;_LL4E8: _tmp87D=_tmp85C.xdatatypes_so_far;_LL4E9:;{
struct _tuple1*_tmp85D=_tmp87E->name;
{struct Cyc_List_List*_tmp85E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp87E->fields))->v;for(0;_tmp85E != 0;_tmp85E=_tmp85E->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp85E->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp85F=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp860=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp85F,Cyc_Absyn_false_conref,0);
# 4882
int*_tmp861=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp87D,f->name);int*_tmp862=_tmp861;if(_tmp862 == 0){_LL4EB: _LL4EC: {
# 4884
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4888
struct Cyc_Absyn_Vardecl*_tmp863=Cyc_Absyn_new_vardecl(f->name,_tmp860,initopt);
_tmp863->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp864=_cycalloc(sizeof(*_tmp864));_tmp864->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp865=_cycalloc(sizeof(*_tmp865));_tmp865[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp866;_tmp866.tag=0;_tmp866.f1=_tmp863;_tmp866;});_tmp865;}),0);_tmp864->tl=Cyc_Toc_result_decls;_tmp864;});
*_tmp87D=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp87D,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp867=f->typs;for(0;_tmp867 != 0;(_tmp867=_tmp867->tl,i ++)){
struct _dyneither_ptr*_tmp868=({struct _dyneither_ptr*_tmp86C=_cycalloc(sizeof(*_tmp86C));_tmp86C[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp86F;_tmp86F.tag=1;_tmp86F.f1=(unsigned long)i;({void*_tmp86D[1]={& _tmp86F};Cyc_aprintf(({const char*_tmp86E="f%d";_tag_dyneither(_tmp86E,sizeof(char),4);}),_tag_dyneither(_tmp86D,sizeof(void*),1));});});_tmp86C;});
struct Cyc_Absyn_Aggrfield*_tmp869=({struct Cyc_Absyn_Aggrfield*_tmp86B=_cycalloc(sizeof(*_tmp86B));_tmp86B->name=_tmp868;_tmp86B->tq=(*((struct _tuple11*)_tmp867->hd)).f1;_tmp86B->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp867->hd)).f2);_tmp86B->width=0;_tmp86B->attributes=0;_tmp86B->requires_clause=0;_tmp86B;});
fields=({struct Cyc_List_List*_tmp86A=_cycalloc(sizeof(*_tmp86A));_tmp86A->hd=_tmp869;_tmp86A->tl=fields;_tmp86A;});}}
# 4901
fields=({struct Cyc_List_List*_tmp870=_cycalloc(sizeof(*_tmp870));_tmp870->hd=({struct Cyc_Absyn_Aggrfield*_tmp871=_cycalloc(sizeof(*_tmp871));_tmp871->name=Cyc_Toc_tag_sp;_tmp871->tq=Cyc_Toc_mt_tq;_tmp871->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp871->width=0;_tmp871->attributes=0;_tmp871->requires_clause=0;_tmp871;});_tmp870->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp870;});{
struct Cyc_Absyn_Aggrdecl*_tmp872=({struct Cyc_Absyn_Aggrdecl*_tmp876=_cycalloc(sizeof(*_tmp876));_tmp876->kind=Cyc_Absyn_StructA;_tmp876->sc=Cyc_Absyn_Public;_tmp876->name=
# 4906
Cyc_Toc_collapse_qvars(f->name,_tmp87E->name);_tmp876->tvs=0;_tmp876->expected_mem_kind=0;_tmp876->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp877=_cycalloc(sizeof(*_tmp877));_tmp877->exist_vars=0;_tmp877->rgn_po=0;_tmp877->fields=fields;_tmp877->tagged=0;_tmp877;});_tmp876->attributes=0;_tmp876;});
# 4910
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp873=_cycalloc(sizeof(*_tmp873));_tmp873->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp874=_cycalloc(sizeof(*_tmp874));_tmp874[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp875;_tmp875.tag=5;_tmp875.f1=_tmp872;_tmp875;});_tmp874;}),0);_tmp873->tl=Cyc_Toc_result_decls;_tmp873;});
# 4912
goto _LL4EA;};};};}}else{if(*((int*)_tmp862)== 0){_LL4ED: _LL4EE:
# 4914
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp878=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp879=Cyc_Absyn_new_vardecl(f->name,_tmp860,_tmp878);
_tmp879->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp87A=_cycalloc(sizeof(*_tmp87A));_tmp87A->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp87B=_cycalloc(sizeof(*_tmp87B));_tmp87B[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp87C;_tmp87C.tag=0;_tmp87C.f1=_tmp879;_tmp87C;});_tmp87B;}),0);_tmp87A->tl=Cyc_Toc_result_decls;_tmp87A;});
*_tmp87D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp87D,f->name,1);}
# 4921
goto _LL4EA;}else{_LL4EF: _LL4F0:
 goto _LL4EA;}}_LL4EA:;}}
# 4925
return 0;};};}
# 4928
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4932
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4938
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4942
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp880=init->r;void*_tmp881=_tmp880;struct Cyc_Absyn_Vardecl*_tmp887;struct Cyc_Absyn_Exp*_tmp886;struct Cyc_Absyn_Exp*_tmp885;int _tmp884;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp881)->tag == 26){_LL4F2: _tmp887=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp881)->f1;_tmp886=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp881)->f2;_tmp885=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp881)->f3;_tmp884=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp881)->f4;_LL4F3:
# 4949
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp887,_tmp886,_tmp885,_tmp884,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL4F1;}else{_LL4F4: _LL4F5:
# 4954
 if(vd->sc == Cyc_Absyn_Static){
# 4958
struct _RegionHandle _tmp882=_new_region("temp");struct _RegionHandle*temp=& _tmp882;_push_region(temp);
{struct Cyc_Toc_Env*_tmp883=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp883,init);}
# 4959
;_pop_region(temp);}else{
# 4963
Cyc_Toc_exp_to_c(nv,init);}
goto _LL4F1;}_LL4F1:;}else{
# 4968
void*_tmp888=Cyc_Tcutil_compress(old_typ);void*_tmp889=_tmp888;void*_tmp890;struct Cyc_Absyn_Exp*_tmp88F;union Cyc_Absyn_Constraint*_tmp88E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp889)->tag == 8){_LL4F7: _tmp890=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp889)->f1).elt_type;_tmp88F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp889)->f1).num_elts;_tmp88E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp889)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp88E)){_LL4F8:
 if(_tmp88F == 0)
({void*_tmp88A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp88B="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp88B,sizeof(char),55);}),_tag_dyneither(_tmp88A,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*num_elts=_tmp88F;
struct Cyc_Absyn_Exp*_tmp88C=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4976
struct Cyc_Absyn_Exp*_tmp88D=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp88C,_tmp88D,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL4F6;};}else{goto _LL4F9;}}else{_LL4F9: _LL4FA:
 goto _LL4F6;}_LL4F6:;}}
# 4987
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp891=d;struct Cyc_List_List*_tmp898;struct Cyc_List_List*_tmp897;void**_tmp896;struct Cyc_Tcpat_Rhs*_tmp895;switch(*((int*)_tmp891)){case 0: _LL4FC: _LL4FD:
 return d;case 1: _LL4FE: _tmp895=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp891)->f1;_LL4FF:
 _tmp895->rhs=success;return d;default: _LL500: _tmp898=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp891)->f1;_tmp897=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp891)->f2;_tmp896=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp891)->f3;_LL501:
# 4992
*_tmp896=Cyc_Toc_rewrite_decision(*_tmp896,success);
for(0;_tmp897 != 0;_tmp897=_tmp897->tl){
struct _tuple29*_tmp892=(struct _tuple29*)_tmp897->hd;struct _tuple29*_tmp893=_tmp892;void**_tmp894;_LL503: _tmp894=(void**)& _tmp893->f2;_LL504:;
*_tmp894=Cyc_Toc_rewrite_decision(*_tmp894,success);}
# 4997
return d;}_LL4FB:;}
# 5008 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 5010
struct _RegionHandle _tmp899=_new_region("rgn");struct _RegionHandle*rgn=& _tmp899;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp89A=Cyc_Toc_share_env(rgn,nv);
void*_tmp89B=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp89A,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 5020
struct Cyc_Absyn_Stmt*_tmp89C=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 5024
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp89C);{
# 5026
struct Cyc_Absyn_Switch_clause*_tmp89D=({struct Cyc_Absyn_Switch_clause*_tmp8AA=_cycalloc(sizeof(*_tmp8AA));_tmp8AA->pattern=p;_tmp8AA->pat_vars=0;_tmp8AA->where_clause=0;_tmp8AA->body=_tmp89C;_tmp8AA->loc=0;_tmp8AA;});
struct Cyc_List_List*_tmp89E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,({struct Cyc_Absyn_Switch_clause*_tmp8A9[1];_tmp8A9[0]=_tmp89D;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(rgn,_tag_dyneither(_tmp8A9,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));
# 5029
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 5033
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp89A,& mydecls,& mybodies,dopt,_tmp89E,v);
# 5036
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple34*_tmp89F=(struct _tuple34*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple34*_tmp8A0=_tmp89F;struct Cyc_Toc_Env*_tmp8A2;struct Cyc_Absyn_Stmt*_tmp8A1;_LL506: _tmp8A2=_tmp8A0->f1;_tmp8A1=_tmp8A0->f3;_LL507:;
if(_tmp8A1 == _tmp89C){senv=_tmp8A2;goto FOUND_ENV;}}
# 5041
({void*_tmp8A3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp8A4="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp8A4,sizeof(char),33);}),_tag_dyneither(_tmp8A3,sizeof(void*),0));});
FOUND_ENV:
# 5045
 Cyc_Toc_stmt_to_c(senv,s);{
# 5047
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0);
# 5049
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp8A5=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
_tmp8A5->name=Cyc_Toc_temp_var();
_tmp8A5->type=Cyc_Toc_typ_to_c_array(_tmp8A5->type);
res=Cyc_Absyn_decl_stmt(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8A6=_cycalloc(sizeof(*_tmp8A6));_tmp8A6[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp8A7;_tmp8A7.tag=0;_tmp8A7.f1=_tmp8A5;_tmp8A7;});_tmp8A6;}),0),res,0);}
# 5056
res=Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c(_tmp89B),e,res,0);{
struct Cyc_Absyn_Stmt*_tmp8A8=res;_npop_handler(0);return _tmp8A8;};};};};}
# 5011
;_pop_region(rgn);}
# 5064
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8AB=e->r;void*_tmp8AC=_tmp8AB;struct Cyc_Absyn_MallocInfo*_tmp8D6;struct Cyc_Absyn_Stmt*_tmp8D5;void**_tmp8D4;void**_tmp8D3;struct Cyc_List_List*_tmp8D2;struct Cyc_List_List*_tmp8D1;void**_tmp8D0;struct Cyc_List_List*_tmp8CF;void**_tmp8CE;struct Cyc_Absyn_Exp*_tmp8CD;struct Cyc_Absyn_Exp*_tmp8CC;struct Cyc_List_List*_tmp8CB;struct Cyc_Absyn_Exp*_tmp8CA;struct Cyc_Absyn_Exp*_tmp8C9;struct Cyc_Absyn_Exp*_tmp8C8;struct Cyc_Absyn_Exp*_tmp8C7;struct Cyc_Absyn_Exp*_tmp8C6;struct Cyc_Absyn_Exp*_tmp8C5;struct Cyc_Absyn_Exp*_tmp8C4;struct Cyc_Absyn_Exp*_tmp8C3;struct Cyc_Absyn_Exp*_tmp8C2;struct Cyc_Absyn_Exp*_tmp8C1;struct Cyc_Absyn_Exp*_tmp8C0;struct Cyc_Absyn_Exp*_tmp8BF;struct Cyc_Absyn_Exp*_tmp8BE;struct Cyc_Absyn_Exp*_tmp8BD;struct Cyc_Absyn_Exp*_tmp8BC;struct Cyc_List_List*_tmp8BB;struct Cyc_Absyn_Exp*_tmp8BA;struct Cyc_Absyn_Exp*_tmp8B9;struct Cyc_Absyn_Exp*_tmp8B8;struct Cyc_Absyn_Exp*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_Absyn_Exp*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B4;struct Cyc_Absyn_Exp*_tmp8B3;switch(*((int*)_tmp8AC)){case 19: _LL509: _tmp8B3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL50A:
 _tmp8B4=_tmp8B3;goto _LL50C;case 20: _LL50B: _tmp8B4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL50C:
 _tmp8B5=_tmp8B4;goto _LL50E;case 21: _LL50D: _tmp8B5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL50E:
 _tmp8B6=_tmp8B5;goto _LL510;case 14: _LL50F: _tmp8B6=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL510:
 _tmp8B7=_tmp8B6;goto _LL512;case 10: _LL511: _tmp8B7=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL512:
 _tmp8B8=_tmp8B7;goto _LL514;case 11: _LL513: _tmp8B8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL514:
 _tmp8B9=_tmp8B8;goto _LL516;case 17: _LL515: _tmp8B9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL516:
 _tmp8BA=_tmp8B9;goto _LL518;case 4: _LL517: _tmp8BA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL518:
 Cyc_Toc_exptypes_to_c(_tmp8BA);goto _LL508;case 2: _LL519: _tmp8BB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_LL51A:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8BB);goto _LL508;case 6: _LL51B: _tmp8BD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_tmp8BC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_LL51C:
 _tmp8BF=_tmp8BD;_tmp8BE=_tmp8BC;goto _LL51E;case 7: _LL51D: _tmp8BF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_tmp8BE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_LL51E:
 _tmp8C1=_tmp8BF;_tmp8C0=_tmp8BE;goto _LL520;case 8: _LL51F: _tmp8C1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_tmp8C0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_LL520:
 _tmp8C3=_tmp8C1;_tmp8C2=_tmp8C0;goto _LL522;case 22: _LL521: _tmp8C3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_tmp8C2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_LL522:
 _tmp8C5=_tmp8C3;_tmp8C4=_tmp8C2;goto _LL524;case 34: _LL523: _tmp8C5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_tmp8C4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_LL524:
 _tmp8C7=_tmp8C5;_tmp8C6=_tmp8C4;goto _LL526;case 3: _LL525: _tmp8C7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_tmp8C6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8AC)->f3;_LL526:
 Cyc_Toc_exptypes_to_c(_tmp8C7);Cyc_Toc_exptypes_to_c(_tmp8C6);goto _LL508;case 5: _LL527: _tmp8CA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_tmp8C9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_tmp8C8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8AC)->f3;_LL528:
# 5082
 Cyc_Toc_exptypes_to_c(_tmp8CA);Cyc_Toc_exptypes_to_c(_tmp8C9);Cyc_Toc_exptypes_to_c(_tmp8C8);goto _LL508;case 9: _LL529: _tmp8CC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_tmp8CB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_LL52A:
# 5084
 Cyc_Toc_exptypes_to_c(_tmp8CC);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8CB);goto _LL508;case 13: _LL52B: _tmp8CE=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_tmp8CD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_LL52C:
*_tmp8CE=Cyc_Toc_typ_to_c(*_tmp8CE);Cyc_Toc_exptypes_to_c(_tmp8CD);goto _LL508;case 24: _LL52D: _tmp8D0=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1)->f3;_tmp8CF=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_LL52E:
# 5087
*_tmp8D0=Cyc_Toc_typ_to_c(*_tmp8D0);
_tmp8D1=_tmp8CF;goto _LL530;case 35: _LL52F: _tmp8D1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8AC)->f2;_LL530:
 _tmp8D2=_tmp8D1;goto _LL532;case 25: _LL531: _tmp8D2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL532:
# 5091
 for(0;_tmp8D2 != 0;_tmp8D2=_tmp8D2->tl){
struct _tuple19 _tmp8AD=*((struct _tuple19*)_tmp8D2->hd);struct _tuple19 _tmp8AE=_tmp8AD;struct Cyc_Absyn_Exp*_tmp8AF;_LL55A: _tmp8AF=_tmp8AE.f2;_LL55B:;
Cyc_Toc_exptypes_to_c(_tmp8AF);}
# 5095
goto _LL508;case 18: _LL533: _tmp8D3=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL534:
 _tmp8D4=_tmp8D3;goto _LL536;case 16: _LL535: _tmp8D4=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL536:
*_tmp8D4=Cyc_Toc_typ_to_c(*_tmp8D4);goto _LL508;case 36: _LL537: _tmp8D5=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL538:
 Cyc_Toc_stmttypes_to_c(_tmp8D5);goto _LL508;case 33: _LL539: _tmp8D6=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8AC)->f1;_LL53A:
# 5100
 if(_tmp8D6->elt_type != 0)
_tmp8D6->elt_type=({void**_tmp8B0=_cycalloc(sizeof(*_tmp8B0));_tmp8B0[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8D6->elt_type)));_tmp8B0;});
Cyc_Toc_exptypes_to_c(_tmp8D6->num_elts);
goto _LL508;case 0: _LL53B: _LL53C:
 goto _LL53E;case 1: _LL53D: _LL53E:
 goto _LL540;case 31: _LL53F: _LL540:
 goto _LL542;case 39: _LL541: _LL542:
 goto _LL544;case 32: _LL543: _LL544:
 goto _LL508;case 29: _LL545: _LL546:
# 5110
 goto _LL548;case 30: _LL547: _LL548:
 goto _LL54A;case 28: _LL549: _LL54A:
 goto _LL54C;case 26: _LL54B: _LL54C:
 goto _LL54E;case 27: _LL54D: _LL54E:
 goto _LL550;case 23: _LL54F: _LL550:
 goto _LL552;case 12: _LL551: _LL552:
 goto _LL554;case 15: _LL553: _LL554:
 goto _LL556;case 38: _LL555: _LL556:
 goto _LL558;default: _LL557: _LL558:
# 5120
({void*_tmp8B1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp8B2="Cyclone expression within C code";_tag_dyneither(_tmp8B2,sizeof(char),33);}),_tag_dyneither(_tmp8B1,sizeof(void*),0));});
goto _LL508;}_LL508:;}
# 5125
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp8D7=d->r;void*_tmp8D8=_tmp8D7;struct Cyc_Absyn_Typedefdecl*_tmp8E2;struct Cyc_Absyn_Enumdecl*_tmp8E1;struct Cyc_Absyn_Aggrdecl*_tmp8E0;struct Cyc_Absyn_Fndecl*_tmp8DF;struct Cyc_Absyn_Vardecl*_tmp8DE;switch(*((int*)_tmp8D8)){case 0: _LL55D: _tmp8DE=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8D8)->f1;_LL55E:
# 5128
 _tmp8DE->type=Cyc_Toc_typ_to_c(_tmp8DE->type);
if(_tmp8DE->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8DE->initializer));
goto _LL55C;case 1: _LL55F: _tmp8DF=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8D8)->f1;_LL560:
# 5132
 _tmp8DF->ret_type=Cyc_Toc_typ_to_c(_tmp8DF->ret_type);
{struct Cyc_List_List*_tmp8D9=_tmp8DF->args;for(0;_tmp8D9 != 0;_tmp8D9=_tmp8D9->tl){
(*((struct _tuple9*)_tmp8D9->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8D9->hd)).f3);}}
# 5136
goto _LL55C;case 5: _LL561: _tmp8E0=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8D8)->f1;_LL562:
 Cyc_Toc_aggrdecl_to_c(_tmp8E0,1);goto _LL55C;case 7: _LL563: _tmp8E1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8D8)->f1;_LL564:
# 5139
 if(_tmp8E1->fields != 0){
struct Cyc_List_List*_tmp8DA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8E1->fields))->v;for(0;_tmp8DA != 0;_tmp8DA=_tmp8DA->tl){
struct Cyc_Absyn_Enumfield*_tmp8DB=(struct Cyc_Absyn_Enumfield*)_tmp8DA->hd;
if(_tmp8DB->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8DB->tag));}}
# 5144
goto _LL55C;case 8: _LL565: _tmp8E2=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8D8)->f1;_LL566:
 _tmp8E2->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8E2->defn));goto _LL55C;case 2: _LL567: _LL568:
 goto _LL56A;case 3: _LL569: _LL56A:
 goto _LL56C;case 6: _LL56B: _LL56C:
 goto _LL56E;case 9: _LL56D: _LL56E:
 goto _LL570;case 10: _LL56F: _LL570:
 goto _LL572;case 11: _LL571: _LL572:
 goto _LL574;case 12: _LL573: _LL574:
 goto _LL576;case 4: _LL575: _LL576:
# 5155
({void*_tmp8DC=0;Cyc_Tcutil_terr(d->loc,({const char*_tmp8DD="Cyclone declaration within C code";_tag_dyneither(_tmp8DD,sizeof(char),34);}),_tag_dyneither(_tmp8DC,sizeof(void*),0));});
goto _LL55C;case 13: _LL577: _LL578:
 goto _LL57A;default: _LL579: _LL57A:
# 5159
 goto _LL55C;}_LL55C:;}
# 5163
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp8E3=s->r;void*_tmp8E4=_tmp8E3;struct Cyc_Absyn_Stmt*_tmp8FF;struct Cyc_Absyn_Stmt*_tmp8FE;struct Cyc_Absyn_Exp*_tmp8FD;struct Cyc_Absyn_Decl*_tmp8FC;struct Cyc_Absyn_Stmt*_tmp8FB;struct Cyc_Absyn_Exp*_tmp8FA;struct Cyc_List_List*_tmp8F9;void*_tmp8F8;struct Cyc_Absyn_Exp*_tmp8F7;struct Cyc_Absyn_Exp*_tmp8F6;struct Cyc_Absyn_Exp*_tmp8F5;struct Cyc_Absyn_Stmt*_tmp8F4;struct Cyc_Absyn_Exp*_tmp8F3;struct Cyc_Absyn_Stmt*_tmp8F2;struct Cyc_Absyn_Exp*_tmp8F1;struct Cyc_Absyn_Stmt*_tmp8F0;struct Cyc_Absyn_Stmt*_tmp8EF;struct Cyc_Absyn_Exp*_tmp8EE;struct Cyc_Absyn_Stmt*_tmp8ED;struct Cyc_Absyn_Stmt*_tmp8EC;struct Cyc_Absyn_Exp*_tmp8EB;switch(*((int*)_tmp8E4)){case 1: _LL57C: _tmp8EB=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f1;_LL57D:
 Cyc_Toc_exptypes_to_c(_tmp8EB);goto _LL57B;case 2: _LL57E: _tmp8ED=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f1;_tmp8EC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f2;_LL57F:
 Cyc_Toc_stmttypes_to_c(_tmp8ED);Cyc_Toc_stmttypes_to_c(_tmp8EC);goto _LL57B;case 3: _LL580: _tmp8EE=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f1;_LL581:
 if(_tmp8EE != 0)Cyc_Toc_exptypes_to_c(_tmp8EE);goto _LL57B;case 4: _LL582: _tmp8F1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f1;_tmp8F0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f2;_tmp8EF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f3;_LL583:
# 5169
 Cyc_Toc_exptypes_to_c(_tmp8F1);Cyc_Toc_stmttypes_to_c(_tmp8F0);Cyc_Toc_stmttypes_to_c(_tmp8EF);goto _LL57B;case 5: _LL584: _tmp8F3=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f1).f1;_tmp8F2=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f2;_LL585:
# 5171
 Cyc_Toc_exptypes_to_c(_tmp8F3);Cyc_Toc_stmttypes_to_c(_tmp8F2);goto _LL57B;case 9: _LL586: _tmp8F7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f1;_tmp8F6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f2).f1;_tmp8F5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f3).f1;_tmp8F4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f4;_LL587:
# 5173
 Cyc_Toc_exptypes_to_c(_tmp8F7);Cyc_Toc_exptypes_to_c(_tmp8F6);Cyc_Toc_exptypes_to_c(_tmp8F5);
Cyc_Toc_stmttypes_to_c(_tmp8F4);goto _LL57B;case 10: _LL588: _tmp8FA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f1;_tmp8F9=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f2;_tmp8F8=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f3;_LL589:
# 5176
 Cyc_Toc_exptypes_to_c(_tmp8FA);
for(0;_tmp8F9 != 0;_tmp8F9=_tmp8F9->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp8F9->hd)->body);}
goto _LL57B;case 12: _LL58A: _tmp8FC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f1;_tmp8FB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f2;_LL58B:
 Cyc_Toc_decltypes_to_c(_tmp8FC);Cyc_Toc_stmttypes_to_c(_tmp8FB);goto _LL57B;case 14: _LL58C: _tmp8FE=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f1;_tmp8FD=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f2).f1;_LL58D:
 Cyc_Toc_stmttypes_to_c(_tmp8FE);Cyc_Toc_exptypes_to_c(_tmp8FD);goto _LL57B;case 13: _LL58E: _tmp8FF=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8E4)->f2;_LL58F:
 Cyc_Toc_stmttypes_to_c(_tmp8FF);goto _LL57B;case 0: _LL590: _LL591:
 goto _LL593;case 6: _LL592: _LL593:
 goto _LL595;case 7: _LL594: _LL595:
 goto _LL597;case 8: _LL596: _LL597:
# 5186
 goto _LL57B;case 11: _LL598: _LL599:
# 5189
 s->r=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp8E5=_cycalloc_atomic(sizeof(*_tmp8E5));_tmp8E5[0]=({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmp8E6;_tmp8E6.tag=0;_tmp8E6;});_tmp8E5;});
goto _LL57B;case 15: _LL59A: _LL59B:
 goto _LL59D;default: _LL59C: _LL59D:
# 5193
({void*_tmp8E7=0;Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8EA;_tmp8EA.tag=0;_tmp8EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp8E8[1]={& _tmp8EA};Cyc_aprintf(({const char*_tmp8E9="Cyclone statement in C code: %s";_tag_dyneither(_tmp8E9,sizeof(char),32);}),_tag_dyneither(_tmp8E8,sizeof(void*),1));});}),_tag_dyneither(_tmp8E7,sizeof(void*),0));});
goto _LL57B;}_LL57B:;}
# 5201
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp900=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp901="decls_to_c: not at toplevel!";_tag_dyneither(_tmp901,sizeof(char),29);}),_tag_dyneither(_tmp900,sizeof(void*),0));});{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp902=d->r;void*_tmp903=_tmp902;struct Cyc_List_List*_tmp91B;struct Cyc_List_List*_tmp91A;struct Cyc_List_List*_tmp919;struct Cyc_List_List*_tmp918;struct Cyc_Absyn_Typedefdecl*_tmp917;struct Cyc_Absyn_Enumdecl*_tmp916;struct Cyc_Absyn_Datatypedecl*_tmp915;struct Cyc_Absyn_Aggrdecl*_tmp914;struct Cyc_Absyn_Fndecl*_tmp913;struct Cyc_Absyn_Vardecl*_tmp912;switch(*((int*)_tmp903)){case 0: _LL59F: _tmp912=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp903)->f1;_LL5A0: {
# 5208
struct _tuple1*_tmp904=_tmp912->name;
# 5210
if(_tmp912->sc == Cyc_Absyn_ExternC)
_tmp904=({struct _tuple1*_tmp905=_cycalloc(sizeof(*_tmp905));_tmp905->f1=Cyc_Absyn_Abs_n(0,1);_tmp905->f2=(*_tmp904).f2;_tmp905;});
if(_tmp912->initializer != 0){
if(_tmp912->sc == Cyc_Absyn_ExternC)_tmp912->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp912->initializer));else{
# 5217
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp912->initializer));}}
# 5219
_tmp912->name=_tmp904;
_tmp912->sc=Cyc_Toc_scope_to_c(_tmp912->sc);
_tmp912->type=Cyc_Toc_typ_to_c(_tmp912->type);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp906=_cycalloc(sizeof(*_tmp906));_tmp906->hd=d;_tmp906->tl=Cyc_Toc_result_decls;_tmp906;});
goto _LL59E;}case 1: _LL5A1: _tmp913=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp903)->f1;_LL5A2: {
# 5225
struct _tuple1*_tmp907=_tmp913->name;
# 5227
if(_tmp913->sc == Cyc_Absyn_ExternC){
_tmp907=({struct _tuple1*_tmp908=_cycalloc(sizeof(*_tmp908));_tmp908->f1=Cyc_Absyn_Abs_n(0,1);_tmp908->f2=(*_tmp907).f2;_tmp908;});
_tmp913->sc=Cyc_Absyn_Public;}
# 5231
_tmp913->name=_tmp907;
Cyc_Toc_fndecl_to_c(nv,_tmp913,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp909=_cycalloc(sizeof(*_tmp909));_tmp909->hd=d;_tmp909->tl=Cyc_Toc_result_decls;_tmp909;});
goto _LL59E;}case 2: _LL5A3: _LL5A4:
 goto _LL5A6;case 3: _LL5A5: _LL5A6:
({void*_tmp90A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp90B="letdecl at toplevel";_tag_dyneither(_tmp90B,sizeof(char),20);}),_tag_dyneither(_tmp90A,sizeof(void*),0));});case 4: _LL5A7: _LL5A8:
({void*_tmp90C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp90D="region decl at toplevel";_tag_dyneither(_tmp90D,sizeof(char),24);}),_tag_dyneither(_tmp90C,sizeof(void*),0));});case 5: _LL5A9: _tmp914=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp903)->f1;_LL5AA:
# 5239
 Cyc_Toc_aggrdecl_to_c(_tmp914,1);
goto _LL59E;case 6: _LL5AB: _tmp915=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp903)->f1;_LL5AC:
# 5242
 if(_tmp915->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp915);else{
# 5245
Cyc_Toc_datatypedecl_to_c(_tmp915);}
goto _LL59E;case 7: _LL5AD: _tmp916=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp903)->f1;_LL5AE:
# 5248
 Cyc_Toc_enumdecl_to_c(_tmp916);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp90E=_cycalloc(sizeof(*_tmp90E));_tmp90E->hd=d;_tmp90E->tl=Cyc_Toc_result_decls;_tmp90E;});
goto _LL59E;case 8: _LL5AF: _tmp917=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp903)->f1;_LL5B0:
# 5252
 _tmp917->tvs=0;
if(_tmp917->defn != 0)
_tmp917->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp917->defn));else{
# 5256
enum Cyc_Absyn_KindQual _tmp90F=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp917->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp910=_tmp90F;if(_tmp910 == Cyc_Absyn_BoxKind){_LL5BE: _LL5BF:
 _tmp917->defn=Cyc_Absyn_void_star_typ();goto _LL5BD;}else{_LL5C0: _LL5C1:
 _tmp917->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL5BD;}_LL5BD:;}
# 5261
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp911=_cycalloc(sizeof(*_tmp911));_tmp911->hd=d;_tmp911->tl=Cyc_Toc_result_decls;_tmp911;});
goto _LL59E;case 13: _LL5B1: _LL5B2:
 goto _LL5B4;case 14: _LL5B3: _LL5B4:
# 5265
 goto _LL59E;case 9: _LL5B5: _tmp918=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp903)->f2;_LL5B6:
 _tmp919=_tmp918;goto _LL5B8;case 10: _LL5B7: _tmp919=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp903)->f2;_LL5B8:
 _tmp91A=_tmp919;goto _LL5BA;case 11: _LL5B9: _tmp91A=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp903)->f1;_LL5BA:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp91A,top,cinclude);goto _LL59E;default: _LL5BB: _tmp91B=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp903)->f1;_LL5BC:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp91B,top,1);goto _LL59E;}_LL59E:;};}
# 5272
return nv;}
# 5276
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp91C=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp91D=_tmp91C;struct Cyc_Core_DynamicRegion*_tmp921;_LL5C3: _tmp921=_tmp91D.key;_LL5C4:;{
struct Cyc_Toc_TocState*_tmp91E=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp921,0,Cyc_Toc_empty_toc_state);
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp91F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp91F));_tmp91F->dyn=(struct Cyc_Core_DynamicRegion*)_tmp921;_tmp91F->state=(struct Cyc_Toc_TocState*)_tmp91E;_tmp91F;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;
Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmp920=_cycalloc(sizeof(struct _dyneither_ptr*)* 42);_tmp920[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str;_tmp920[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str;_tmp920[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str;_tmp920[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str;_tmp920[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str;_tmp920[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str;_tmp920[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str;_tmp920[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str;_tmp920[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str;_tmp920[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str;_tmp920[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str;_tmp920[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str;_tmp920[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str;_tmp920[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str;_tmp920[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str;_tmp920[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str;_tmp920[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str;_tmp920[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str;_tmp920[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp920[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str;_tmp920[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp920[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp920[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str;_tmp920[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str;_tmp920[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str;_tmp920[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str;_tmp920[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str;_tmp920[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str;_tmp920[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str;_tmp920[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str;_tmp920[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str;_tmp920[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str;_tmp920[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str;_tmp920[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str;_tmp920[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str;_tmp920[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str;_tmp920[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str;_tmp920[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp920[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str;_tmp920[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str;_tmp920[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str;_tmp920[41]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str;_tmp920;}),sizeof(struct _dyneither_ptr*),42);};}
# 5333
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp922=*ts;struct Cyc_Toc_TocState _tmp923=_tmp922;struct Cyc_Xarray_Xarray*_tmp924;_LL5C6: _tmp924=_tmp923.temp_labels;_LL5C7:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp924);
return 0;}
# 5341
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 5343
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp925=_cycalloc(sizeof(*_tmp925));_tmp925[0]=pop_tables;_tmp925;});
Cyc_Toc_init();
{struct _RegionHandle _tmp926=_new_region("start");struct _RegionHandle*start=& _tmp926;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);;_pop_region(start);}{
# 5353
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp927=ts;struct Cyc_Toc_TocStateWrap*_tmp928=Cyc_Toc_toc_state;ts=_tmp928;Cyc_Toc_toc_state=_tmp927;});{
struct Cyc_Toc_TocStateWrap _tmp929=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp92A=_tmp929;struct Cyc_Core_DynamicRegion*_tmp92C;struct Cyc_Toc_TocState*_tmp92B;_LL5C9: _tmp92C=_tmp92A.dyn;_tmp92B=_tmp92A.state;_LL5CA:;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp92C,_tmp92B,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp92C);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 5360
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);};};}
