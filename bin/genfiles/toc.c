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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 86
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
void*Cyc_Toc_typ_to_c(void*t);
# 41
struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 44
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
return(struct _dyneither_ptr)buf;};};};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 541
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 544
struct _tuple20 _tmp165=*env;struct _tuple20 _tmp166=_tmp165;struct Cyc_Dict_Dict*_tmp179;struct _tuple14*_tmp178;_LL64: _tmp179=(_tmp166.f1)->qvar_tags;_tmp178=_tmp166.f2;_LL65:;{
struct _tuple14 _tmp167=*_tmp178;struct _tuple14 _tmp168=_tmp167;struct _tuple1*_tmp177;struct _tuple1*_tmp176;_LL67: _tmp177=_tmp168.f1;_tmp176=_tmp168.f2;_LL68:;{
struct _handler_cons _tmp169;_push_handler(& _tmp169);{int _tmp16B=0;if(setjmp(_tmp169.handler))_tmp16B=1;if(!_tmp16B){{struct _tuple1*_tmp16C=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp179,Cyc_Toc_qvar_tag_cmp,_tmp178);_npop_handler(0);return _tmp16C;};_pop_handler();}else{void*_tmp16A=(void*)_exn_thrown;void*_tmp16D=_tmp16A;void*_tmp175;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp16D)->tag == Cyc_Dict_Absent){_LL6A: _LL6B: {
# 548
struct _tuple14*_tmp16E=({struct _tuple14*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->f1=_tmp177;_tmp174->f2=_tmp176;_tmp174;});
struct _tuple1*_tmp16F=_tmp177;union Cyc_Absyn_Nmspace _tmp173;struct _dyneither_ptr _tmp172;_LL6F: _tmp173=_tmp16F->f1;_tmp172=*_tmp16F->f2;_LL70:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp172,_tmp176);
struct _tuple1*res=({struct _tuple1*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->f1=_tmp173;_tmp170->f2=({struct _dyneither_ptr*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171[0]=newvar;_tmp171;});_tmp170;});
*_tmp179=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp179,_tmp16E,res);
return res;};}}else{_LL6C: _tmp175=_tmp16D;_LL6D:(int)_rethrow(_tmp175);}_LL69:;}};};};}
# 557
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp17A;_tmp17A.f1=fieldname;_tmp17A.f2=dtname;_tmp17A;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 562
void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 566
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 569
struct _tuple21 _tmp17B=*env;struct _tuple21 _tmp17C=_tmp17B;struct Cyc_List_List**_tmp194;struct Cyc_List_List*_tmp193;_LL72: _tmp194=(_tmp17C.f1)->tuple_types;_tmp193=_tmp17C.f2;_LL73:;
# 571
{struct Cyc_List_List*_tmp17D=*_tmp194;for(0;_tmp17D != 0;_tmp17D=_tmp17D->tl){
struct _tuple22*_tmp17E=(struct _tuple22*)_tmp17D->hd;struct _tuple22*_tmp17F=_tmp17E;void*_tmp182;struct Cyc_List_List*_tmp181;_LL75: _tmp182=_tmp17F->f1;_tmp181=_tmp17F->f2;_LL76:;{
struct Cyc_List_List*_tmp180=_tmp193;
for(0;_tmp180 != 0  && _tmp181 != 0;(_tmp180=_tmp180->tl,_tmp181=_tmp181->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp180->hd)).f2,(void*)_tmp181->hd))
break;}
if(_tmp180 == 0  && _tmp181 == 0)
return _tmp182;};}}{
# 581
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp192;_tmp192.tag=1;_tmp192.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp190[1]={& _tmp192};Cyc_aprintf(({const char*_tmp191="_tuple%d";_tag_dyneither(_tmp191,sizeof(char),9);}),_tag_dyneither(_tmp190,sizeof(void*),1));});});_tmp18F;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp193);
struct Cyc_List_List*_tmp183=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmp183=({struct Cyc_List_List*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->hd=({struct Cyc_Absyn_Aggrfield*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->name=Cyc_Absyn_fieldname(i);_tmp185->tq=Cyc_Toc_mt_tq;_tmp185->type=(void*)ts2->hd;_tmp185->width=0;_tmp185->attributes=0;_tmp185->requires_clause=0;_tmp185;});_tmp184->tl=_tmp183;_tmp184;});}}
_tmp183=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp183);{
struct Cyc_Absyn_Aggrdecl*_tmp186=({struct Cyc_Absyn_Aggrdecl*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->kind=Cyc_Absyn_StructA;_tmp18C->sc=Cyc_Absyn_Public;_tmp18C->name=({struct _tuple1*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->f1=
Cyc_Absyn_Rel_n(0);_tmp18E->f2=xname;_tmp18E;});_tmp18C->tvs=0;_tmp18C->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->exist_vars=0;_tmp18D->rgn_po=0;_tmp18D->fields=_tmp183;_tmp18D->tagged=0;_tmp18D;});_tmp18C->attributes=0;_tmp18C->expected_mem_kind=0;_tmp18C;});
# 596
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp189;_tmp189.tag=5;_tmp189.f1=_tmp186;_tmp189;});_tmp188;}),0);_tmp187->tl=Cyc_Toc_result_decls;_tmp187;});
*_tmp194=({struct Cyc_List_List*_tmp18A=_region_malloc(d,sizeof(*_tmp18A));_tmp18A->hd=({struct _tuple22*_tmp18B=_region_malloc(d,sizeof(*_tmp18B));_tmp18B->f1=x;_tmp18B->f2=ts;_tmp18B;});_tmp18A->tl=*_tmp194;_tmp18A;});
return x;};};}
# 601
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 610
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 618
struct _tuple24 _tmp195=*env;struct _tuple24 _tmp196=_tmp195;struct Cyc_List_List**_tmp1C0;struct _tuple1*_tmp1BF;struct Cyc_List_List*_tmp1BE;struct Cyc_List_List*_tmp1BD;struct Cyc_List_List*_tmp1BC;_LL78: _tmp1C0=(_tmp196.f1)->abs_struct_types;_tmp1BF=(_tmp196.f2)->f1;_tmp1BE=(_tmp196.f2)->f2;_tmp1BD=(_tmp196.f2)->f3;_tmp1BC=(_tmp196.f2)->f4;_LL79:;
# 623
{struct Cyc_List_List*_tmp197=*_tmp1C0;for(0;_tmp197 != 0;_tmp197=_tmp197->tl){
struct _tuple25*_tmp198=(struct _tuple25*)_tmp197->hd;struct _tuple25*_tmp199=_tmp198;struct _tuple1*_tmp1A1;struct Cyc_List_List*_tmp1A0;void*_tmp19F;_LL7B: _tmp1A1=_tmp199->f1;_tmp1A0=_tmp199->f2;_tmp19F=_tmp199->f3;_LL7C:;
# 626
if(Cyc_Absyn_qvar_cmp(_tmp1A1,_tmp1BF)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1BD)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1A0)){
int okay=1;
{struct Cyc_List_List*_tmp19A=_tmp1BD;for(0;_tmp19A != 0;(_tmp19A=_tmp19A->tl,_tmp1A0=_tmp1A0->tl)){
void*_tmp19B=(void*)_tmp19A->hd;
void*_tmp19C=(void*)((struct Cyc_List_List*)_check_null(_tmp1A0))->hd;
# 633
{struct Cyc_Absyn_Kind*_tmp19D=Cyc_Tcutil_typ_kind(_tmp19B);struct Cyc_Absyn_Kind*_tmp19E=_tmp19D;switch(((struct Cyc_Absyn_Kind*)_tmp19E)->kind){case Cyc_Absyn_EffKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_RgnKind: _LL80: _LL81:
# 638
 continue;default: _LL82: _LL83:
# 641
 if(Cyc_Tcutil_unify(_tmp19B,_tmp19C) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp19B),Cyc_Toc_typ_to_c(_tmp19C)))
# 643
continue;
# 646
okay=0;
goto _LL7D;}_LL7D:;}
# 649
break;}}
# 651
if(okay)
# 653
return _tmp19F;}}}{
# 661
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1BB;_tmp1BB.tag=1;_tmp1BB.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp1B9[1]={& _tmp1BB};Cyc_aprintf(({const char*_tmp1BA="_tuple%d";_tag_dyneither(_tmp1BA,sizeof(char),9);}),_tag_dyneither(_tmp1B9,sizeof(void*),1));});});_tmp1B8;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp1A2=0;
# 665
*_tmp1C0=({struct Cyc_List_List*_tmp1A3=_region_malloc(d,sizeof(*_tmp1A3));_tmp1A3->hd=({struct _tuple25*_tmp1A4=_region_malloc(d,sizeof(*_tmp1A4));_tmp1A4->f1=_tmp1BF;_tmp1A4->f2=_tmp1BD;_tmp1A4->f3=x;_tmp1A4;});_tmp1A3->tl=*_tmp1C0;_tmp1A3;});{
# 668
struct _RegionHandle _tmp1A5=_new_region("r");struct _RegionHandle*r=& _tmp1A5;_push_region(r);
{struct Cyc_List_List*_tmp1A6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1BE,_tmp1BD);
for(0;_tmp1BC != 0;_tmp1BC=_tmp1BC->tl){
struct Cyc_Absyn_Aggrfield*_tmp1A7=(struct Cyc_Absyn_Aggrfield*)_tmp1BC->hd;
void*t=_tmp1A7->type;
struct Cyc_List_List*atts=_tmp1A7->attributes;
# 676
if(_tmp1BC->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t)))
atts=({struct Cyc_List_List*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp1AA;_tmp1AA.tag=6;_tmp1AA.f1=0;_tmp1AA;});_tmp1A9;});_tmp1A8->tl=atts;_tmp1A8;});
# 679
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp1A6,t));
# 682
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp1AC;_tmp1AC.tag=8;_tmp1AC.f1=({struct Cyc_Absyn_ArrayInfo _tmp1AD;_tmp1AD.elt_type=Cyc_Absyn_void_star_typ();_tmp1AD.tq=Cyc_Absyn_empty_tqual(0);_tmp1AD.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp1AD.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp1AD.zt_loc=0;_tmp1AD;});_tmp1AC;});_tmp1AB;});
# 686
_tmp1A2=({struct Cyc_List_List*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->hd=({struct Cyc_Absyn_Aggrfield*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->name=_tmp1A7->name;_tmp1AF->tq=Cyc_Toc_mt_tq;_tmp1AF->type=t;_tmp1AF->width=_tmp1A7->width;_tmp1AF->attributes=atts;_tmp1AF->requires_clause=0;_tmp1AF;});_tmp1AE->tl=_tmp1A2;_tmp1AE;});}
# 688
_tmp1A2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1A2);{
struct Cyc_Absyn_Aggrdecl*_tmp1B0=({struct Cyc_Absyn_Aggrdecl*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5->kind=Cyc_Absyn_StructA;_tmp1B5->sc=Cyc_Absyn_Public;_tmp1B5->name=({struct _tuple1*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->f1=
Cyc_Absyn_Rel_n(0);_tmp1B7->f2=xname;_tmp1B7;});_tmp1B5->tvs=0;_tmp1B5->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6->exist_vars=0;_tmp1B6->rgn_po=0;_tmp1B6->fields=_tmp1A2;_tmp1B6->tagged=0;_tmp1B6;});_tmp1B5->attributes=0;_tmp1B5->expected_mem_kind=0;_tmp1B5;});
# 696
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1B3;_tmp1B3.tag=5;_tmp1B3.f1=_tmp1B0;_tmp1B3;});_tmp1B2;}),0);_tmp1B1->tl=Cyc_Toc_result_decls;_tmp1B1;});{
void*_tmp1B4=x;_npop_handler(0);return _tmp1B4;};};}
# 669
;_pop_region(r);};};}
# 700
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 704
struct _tuple23 env=({struct _tuple23 _tmp1C1;_tmp1C1.f1=struct_name;_tmp1C1.f2=type_vars;_tmp1C1.f3=type_args;_tmp1C1.f4=fields;_tmp1C1;});
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 711
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp1C2=Cyc_Toc_temp_var_counter ++;
struct _tuple1*res=({struct _tuple1*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->f1=Cyc_Absyn_Loc_n;_tmp1C3->f2=({struct _dyneither_ptr*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1C7;_tmp1C7.tag=1;_tmp1C7.f1=(unsigned int)_tmp1C2;({void*_tmp1C5[1]={& _tmp1C7};Cyc_aprintf(({const char*_tmp1C6="_tmp%X";_tag_dyneither(_tmp1C6,sizeof(char),7);}),_tag_dyneither(_tmp1C5,sizeof(void*),1));});});_tmp1C4;});_tmp1C3;});
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 719
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp1C8=*env;struct _tuple26 _tmp1C9=_tmp1C8;struct Cyc_Xarray_Xarray*_tmp1D1;_LL85: _tmp1D1=(_tmp1C9.f1)->temp_labels;_LL86:;{
int _tmp1CA=Cyc_Toc_fresh_label_counter ++;
if(_tmp1CA < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp1D1))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp1D1,_tmp1CA);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1D0;_tmp1D0.tag=1;_tmp1D0.f1=(unsigned int)_tmp1CA;({void*_tmp1CE[1]={& _tmp1D0};Cyc_aprintf(({const char*_tmp1CF="_LL%X";_tag_dyneither(_tmp1CF,sizeof(char),6);}),_tag_dyneither(_tmp1CE,sizeof(void*),1));});});_tmp1CD;});
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp1D1,res)!= _tmp1CA)
({void*_tmp1CB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1CC="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp1CC,sizeof(char),43);}),_tag_dyneither(_tmp1CB,sizeof(void*),0));});
return res;};};}
# 730
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 737
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp1D2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp1D2))->hd)->name)!= 0){
++ ans;
_tmp1D2=_tmp1D2->tl;}
# 744
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 750
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 755
struct _tuple9 _tmp1D3=*a;struct _tuple9 _tmp1D4=_tmp1D3;struct _dyneither_ptr*_tmp1D8;struct Cyc_Absyn_Tqual _tmp1D7;void*_tmp1D6;_LL88: _tmp1D8=_tmp1D4.f1;_tmp1D7=_tmp1D4.f2;_tmp1D6=_tmp1D4.f3;_LL89:;
return({struct _tuple9*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->f1=_tmp1D8;_tmp1D5->f2=_tmp1D7;_tmp1D5->f3=Cyc_Toc_typ_to_c(_tmp1D6);_tmp1D5;});}
# 758
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1D9=*x;struct _tuple11 _tmp1DA=_tmp1D9;struct Cyc_Absyn_Tqual _tmp1DD;void*_tmp1DC;_LL8B: _tmp1DD=_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;_LL8C:;
return({struct _tuple11*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->f1=_tmp1DD;_tmp1DB->f2=Cyc_Toc_typ_to_c(_tmp1DC);_tmp1DB;});}
# 777 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1DE=Cyc_Tcutil_compress(t);void*_tmp1DF=_tmp1DE;void*_tmp1E5;void*_tmp1E4;struct Cyc_Absyn_Tqual _tmp1E3;struct Cyc_Absyn_Exp*_tmp1E2;union Cyc_Absyn_Constraint*_tmp1E1;unsigned int _tmp1E0;switch(*((int*)_tmp1DF)){case 8: _LL8E: _tmp1E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DF)->f1).elt_type;_tmp1E3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DF)->f1).tq;_tmp1E2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DF)->f1).num_elts;_tmp1E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DF)->f1).zero_term;_tmp1E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DF)->f1).zt_loc;_LL8F:
# 780
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp1E4),_tmp1E3);case 1: _LL90: _tmp1E5=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DF)->f2;if(_tmp1E5 != 0){_LL91:
 return Cyc_Toc_typ_to_c_array(_tmp1E5);}else{goto _LL92;}default: _LL92: _LL93:
 return Cyc_Toc_typ_to_c(t);}_LL8D:;}
# 786
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 788
return({struct Cyc_Absyn_Aggrfield*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->name=f->name;_tmp1E6->tq=Cyc_Toc_mt_tq;_tmp1E6->type=
# 790
Cyc_Toc_typ_to_c(f->type);_tmp1E6->width=f->width;_tmp1E6->attributes=f->attributes;_tmp1E6->requires_clause=0;_tmp1E6;});}
# 795
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 797
return;}
# 800
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
cs=({void**_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp1E7;});
# 805
return*cs;}
# 807
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);_tmp1E8;});
# 812
return*r;}
# 814
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_tmp1E9;});
# 819
return*r;}
# 821
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1EA=Cyc_Tcutil_compress(t);void*_tmp1EB=_tmp1EA;struct Cyc_Absyn_Tvar*_tmp1EC;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EB)->tag == 2){_LL95: _tmp1EC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EB)->f1;_LL96:
# 824
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL97: _LL98:
 return 0;}_LL94:;}
# 828
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1ED=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1EE=_tmp1ED;if(((struct Cyc_Absyn_Kind*)_tmp1EE)->kind == Cyc_Absyn_AnyKind){_LL9A: _LL9B:
 return 1;}else{_LL9C: _LL9D:
 return 0;}_LL99:;}
# 835
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1EF=t;struct Cyc_Absyn_Datatypedecl*_tmp23A;void**_tmp239;struct Cyc_Absyn_Enumdecl*_tmp238;struct Cyc_Absyn_Aggrdecl*_tmp237;struct Cyc_Absyn_Exp*_tmp236;struct Cyc_Absyn_Exp*_tmp235;void*_tmp234;struct _tuple1*_tmp233;struct Cyc_List_List*_tmp232;struct Cyc_Absyn_Typedefdecl*_tmp231;void*_tmp230;struct Cyc_List_List*_tmp22F;struct _tuple1*_tmp22E;union Cyc_Absyn_AggrInfoU _tmp22D;struct Cyc_List_List*_tmp22C;enum Cyc_Absyn_AggrKind _tmp22B;struct Cyc_List_List*_tmp22A;struct Cyc_List_List*_tmp229;struct Cyc_Absyn_Tqual _tmp228;void*_tmp227;struct Cyc_List_List*_tmp226;int _tmp225;struct Cyc_Absyn_VarargInfo*_tmp224;struct Cyc_List_List*_tmp223;void*_tmp222;struct Cyc_Absyn_Tqual _tmp221;struct Cyc_Absyn_Exp*_tmp220;unsigned int _tmp21F;void*_tmp21E;struct Cyc_Absyn_Tqual _tmp21D;union Cyc_Absyn_Constraint*_tmp21C;struct Cyc_Absyn_Datatypedecl*_tmp21B;struct Cyc_Absyn_Datatypefield*_tmp21A;struct Cyc_Absyn_Tvar*_tmp219;void**_tmp218;switch(*((int*)_tmp1EF)){case 0: _LL9F: _LLA0:
 return t;case 1: _LLA1: _tmp218=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EF)->f2;_LLA2:
# 839
 if(*_tmp218 == 0){
*_tmp218=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 843
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp218));case 2: _LLA3: _tmp219=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EF)->f1;_LLA4:
# 845
 if((Cyc_Tcutil_tvar_kind(_tmp219,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 848
return(void*)& Cyc_Absyn_VoidType_val;else{
# 850
return Cyc_Absyn_void_star_typ();}case 3: _LLA5: _LLA6:
# 852
 return(void*)& Cyc_Absyn_VoidType_val;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1EF)->f1).field_info).KnownDatatypefield).tag == 2){_LLA7: _tmp21B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1EF)->f1).field_info).KnownDatatypefield).val).f1;_tmp21A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1EF)->f1).field_info).KnownDatatypefield).val).f2;_LLA8:
# 854
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp21A->name,_tmp21B->name));}else{_LLA9: _LLAA:
({void*_tmp1F0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1F1="unresolved DatatypeFieldType";_tag_dyneither(_tmp1F1,sizeof(char),29);}),_tag_dyneither(_tmp1F0,sizeof(void*),0));});}case 5: _LLAB: _tmp21E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EF)->f1).elt_typ;_tmp21D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EF)->f1).elt_tq;_tmp21C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EF)->f1).ptr_atts).bounds;_LLAC:
# 859
 _tmp21E=Cyc_Toc_typ_to_c(_tmp21E);{
void*_tmp1F2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp21C);void*_tmp1F3=_tmp1F2;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1F3)->tag == 0){_LLE0: _LLE1:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LLE2: _LLE3:
 return Cyc_Absyn_star_typ(_tmp21E,(void*)& Cyc_Absyn_HeapRgn_val,_tmp21D,Cyc_Absyn_false_conref);}_LLDF:;};case 6: _LLAD: _LLAE:
# 864
 goto _LLB0;case 7: _LLAF: _LLB0:
 return t;case 8: _LLB1: _tmp222=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EF)->f1).elt_type;_tmp221=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EF)->f1).tq;_tmp220=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EF)->f1).num_elts;_tmp21F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EF)->f1).zt_loc;_LLB2:
# 867
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp222),_tmp221,_tmp220,Cyc_Absyn_false_conref,_tmp21F);case 9: _LLB3: _tmp228=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).ret_tqual;_tmp227=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).ret_typ;_tmp226=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).args;_tmp225=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).c_varargs;_tmp224=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).cyc_varargs;_tmp223=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).attributes;_LLB4: {
# 873
struct Cyc_List_List*_tmp1F4=0;
for(0;_tmp223 != 0;_tmp223=_tmp223->tl){
void*_tmp1F5=(void*)_tmp223->hd;void*_tmp1F6=_tmp1F5;switch(*((int*)_tmp1F6)){case 4: _LLE5: _LLE6:
 goto _LLE8;case 5: _LLE7: _LLE8:
 goto _LLEA;case 19: _LLE9: _LLEA:
 continue;case 22: _LLEB: _LLEC:
 continue;case 21: _LLED: _LLEE:
 continue;case 20: _LLEF: _LLF0:
 continue;default: _LLF1: _LLF2:
 _tmp1F4=({struct Cyc_List_List*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7->hd=(void*)_tmp223->hd;_tmp1F7->tl=_tmp1F4;_tmp1F7;});goto _LLE4;}_LLE4:;}{
# 884
struct Cyc_List_List*_tmp1F8=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp226);
if(_tmp224 != 0){
# 887
void*_tmp1F9=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp224->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp1FA=({struct _tuple9*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->f1=_tmp224->name;_tmp1FC->f2=_tmp224->tq;_tmp1FC->f3=_tmp1F9;_tmp1FC;});
_tmp1F8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1F8,({struct Cyc_List_List*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB->hd=_tmp1FA;_tmp1FB->tl=0;_tmp1FB;}));}
# 891
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1FE;_tmp1FE.tag=9;_tmp1FE.f1=({struct Cyc_Absyn_FnInfo _tmp1FF;_tmp1FF.tvars=0;_tmp1FF.effect=0;_tmp1FF.ret_tqual=_tmp228;_tmp1FF.ret_typ=Cyc_Toc_typ_to_c(_tmp227);_tmp1FF.args=_tmp1F8;_tmp1FF.c_varargs=_tmp225;_tmp1FF.cyc_varargs=0;_tmp1FF.rgn_po=0;_tmp1FF.attributes=_tmp1F4;_tmp1FF.requires_clause=0;_tmp1FF.requires_relns=0;_tmp1FF.ensures_clause=0;_tmp1FF.ensures_relns=0;_tmp1FF;});_tmp1FE;});_tmp1FD;});};}case 10: _LLB5: _tmp229=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1EF)->f1;_LLB6:
# 896
 _tmp229=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp229);
return Cyc_Toc_add_tuple_type(_tmp229);case 12: _LLB7: _tmp22B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1EF)->f1;_tmp22A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1EF)->f2;_LLB8:
# 901
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp201;_tmp201.tag=12;_tmp201.f1=_tmp22B;_tmp201.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp22A);_tmp201;});_tmp200;});case 11: _LLB9: _tmp22D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1EF)->f1).aggr_info;_tmp22C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1EF)->f1).targs;_LLBA:
# 905
{union Cyc_Absyn_AggrInfoU _tmp202=_tmp22D;if((_tmp202.UnknownAggr).tag == 1){_LLF4: _LLF5:
 return t;}else{_LLF6: _LLF7:
 goto _LLF3;}_LLF3:;}{
# 909
struct Cyc_Absyn_Aggrdecl*_tmp203=Cyc_Absyn_get_known_aggrdecl(_tmp22D);
if(_tmp203->expected_mem_kind){
# 912
if(_tmp203->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp207;_tmp207.tag=0;_tmp207.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 915
Cyc_Absynpp_qvar2string(_tmp203->name));({struct Cyc_String_pa_PrintArg_struct _tmp206;_tmp206.tag=0;_tmp206.f1=(struct _dyneither_ptr)(
# 914
_tmp203->kind == Cyc_Absyn_UnionA?({const char*_tmp208="union";_tag_dyneither(_tmp208,sizeof(char),6);}):({const char*_tmp209="struct";_tag_dyneither(_tmp209,sizeof(char),7);}));({void*_tmp204[2]={& _tmp206,& _tmp207};Cyc_Tcutil_warn(0,({const char*_tmp205="%s %s was never defined.";_tag_dyneither(_tmp205,sizeof(char),25);}),_tag_dyneither(_tmp204,sizeof(void*),2));});});});}
# 920
if(_tmp203->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp203->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp20A=_tmp203->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp203->impl))->fields;
if(_tmp20A == 0)return Cyc_Toc_aggrdecl_type(_tmp203->name,Cyc_Absyn_strctq);
for(0;_tmp20A->tl != 0;_tmp20A=_tmp20A->tl){;}{
void*_tmp20B=((struct Cyc_Absyn_Aggrfield*)_tmp20A->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp20B))){
if(_tmp203->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp20E;_tmp20E.tag=0;_tmp20E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp203->name));({void*_tmp20C[1]={& _tmp20E};Cyc_Tcutil_warn(0,({const char*_tmp20D="struct %s ended up being abstract.";_tag_dyneither(_tmp20D,sizeof(char),35);}),_tag_dyneither(_tmp20C,sizeof(void*),1));});});{
# 934
struct _RegionHandle _tmp20F=_new_region("r");struct _RegionHandle*r=& _tmp20F;_push_region(r);
{struct Cyc_List_List*_tmp210=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp203->tvs,_tmp22C);
void*_tmp211=Cyc_Tcutil_rsubstitute(r,_tmp210,_tmp20B);
if(Cyc_Toc_is_abstract_type(_tmp211)){void*_tmp212=Cyc_Toc_aggrdecl_type(_tmp203->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp212;}{
void*_tmp213=Cyc_Toc_add_struct_type(_tmp203->name,_tmp203->tvs,_tmp22C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp203->impl))->fields);_npop_handler(0);return _tmp213;};}
# 935
;_pop_region(r);};}
# 940
return Cyc_Toc_aggrdecl_type(_tmp203->name,Cyc_Absyn_strctq);};};};case 13: _LLBB: _tmp22E=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1EF)->f1;_LLBC:
 return t;case 14: _LLBD: _tmp22F=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1EF)->f1;_LLBE:
 Cyc_Toc_enumfields_to_c(_tmp22F);return t;case 17: _LLBF: _tmp233=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EF)->f1;_tmp232=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EF)->f2;_tmp231=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EF)->f3;_tmp230=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EF)->f4;_LLC0:
# 944
 if(_tmp230 == 0  || Cyc_noexpand_r){
if(_tmp232 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp215;_tmp215.tag=17;_tmp215.f1=_tmp233;_tmp215.f2=0;_tmp215.f3=_tmp231;_tmp215.f4=0;_tmp215;});_tmp214;});else{
return t;}}else{
# 949
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp217;_tmp217.tag=17;_tmp217.f1=_tmp233;_tmp217.f2=0;_tmp217.f3=_tmp231;_tmp217.f4=Cyc_Toc_typ_to_c(_tmp230);_tmp217;});_tmp216;});}case 19: _LLC1: _LLC2:
 return Cyc_Absyn_uint_typ;case 15: _LLC3: _tmp234=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1EF)->f1;_LLC4:
 return Cyc_Toc_rgn_typ();case 16: _LLC5: _LLC6:
 return Cyc_Toc_dyn_rgn_typ();case 20: _LLC7: _LLC8:
# 955
 goto _LLCA;case 21: _LLC9: _LLCA:
 goto _LLCC;case 22: _LLCB: _LLCC:
 goto _LLCE;case 23: _LLCD: _LLCE:
 goto _LLD0;case 24: _LLCF: _LLD0:
 goto _LLD2;case 25: _LLD1: _LLD2:
 return Cyc_Absyn_void_star_typ();case 18: _LLD3: _tmp235=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1EF)->f1;_LLD4:
# 965
 return t;case 27: _LLD5: _tmp236=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1EF)->f1;_LLD6:
# 967
 return t;case 28: _LLD7: _LLD8:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EF)->f1)->r)){case 0: _LLD9: _tmp237=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EF)->f1)->r)->f1;_LLDA:
# 970
 Cyc_Toc_aggrdecl_to_c(_tmp237,1);
if(_tmp237->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp237->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp237->name,Cyc_Absyn_strctq);}case 1: _LLDB: _tmp238=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EF)->f1)->r)->f1;_LLDC:
# 975
 Cyc_Toc_enumdecl_to_c(_tmp238);
return t;default: _LLDD: _tmp23A=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EF)->f1)->r)->f1;_tmp239=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EF)->f2;_LLDE:
# 978
 Cyc_Toc_datatypedecl_to_c(_tmp23A);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp239)));}}_LL9E:;}
# 983
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp23B=t;void*_tmp23D;struct Cyc_Absyn_Tqual _tmp23C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23B)->tag == 8){_LLF9: _tmp23D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23B)->f1).elt_type;_tmp23C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23B)->f1).tq;_LLFA:
# 986
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp23D,(void*)& Cyc_Absyn_HeapRgn_val,_tmp23C,Cyc_Absyn_false_conref),e);}else{_LLFB: _LLFC:
 return Cyc_Toc_cast_it(t,e);}_LLF8:;}
# 993
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp23E=Cyc_Tcutil_compress(t);void*_tmp23F=_tmp23E;struct Cyc_List_List*_tmp24B;struct Cyc_Absyn_Datatypedecl*_tmp24A;struct Cyc_Absyn_Datatypefield*_tmp249;struct Cyc_List_List*_tmp248;union Cyc_Absyn_AggrInfoU _tmp247;void*_tmp246;switch(*((int*)_tmp23F)){case 0: _LLFE: _LLFF:
 return 1;case 2: _LL100: _LL101:
 return 0;case 6: _LL102: _LL103:
 goto _LL105;case 13: _LL104: _LL105:
 goto _LL107;case 14: _LL106: _LL107:
 goto _LL109;case 7: _LL108: _LL109:
 goto _LL10B;case 9: _LL10A: _LL10B:
 goto _LL10D;case 19: _LL10C: _LL10D:
 return 1;case 8: _LL10E: _tmp246=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23F)->f1).elt_type;_LL10F:
 return Cyc_Toc_atomic_typ(_tmp246);case 11: _LL110: _tmp247=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp23F)->f1).aggr_info;_LL111:
# 1010
{union Cyc_Absyn_AggrInfoU _tmp240=_tmp247;if((_tmp240.UnknownAggr).tag == 1){_LL123: _LL124:
 return 0;}else{_LL125: _LL126:
 goto _LL122;}_LL122:;}{
# 1014
struct Cyc_Absyn_Aggrdecl*_tmp241=Cyc_Absyn_get_known_aggrdecl(_tmp247);
if(_tmp241->impl == 0)
return 0;
{struct Cyc_List_List*_tmp242=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp241->impl))->fields;for(0;_tmp242 != 0;_tmp242=_tmp242->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp242->hd)->type))return 0;}}
return 1;};case 12: _LL112: _tmp248=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23F)->f2;_LL113:
# 1021
 for(0;_tmp248 != 0;_tmp248=_tmp248->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp248->hd)->type))return 0;}
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23F)->f1).field_info).KnownDatatypefield).tag == 2){_LL114: _tmp24A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23F)->f1).field_info).KnownDatatypefield).val).f1;_tmp249=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23F)->f1).field_info).KnownDatatypefield).val).f2;_LL115:
# 1025
 _tmp24B=_tmp249->typs;goto _LL117;}else{goto _LL120;}case 10: _LL116: _tmp24B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp23F)->f1;_LL117:
# 1027
 for(0;_tmp24B != 0;_tmp24B=_tmp24B->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp24B->hd)).f2))return 0;}
return 1;case 3: _LL118: _LL119:
# 1032
 goto _LL11B;case 5: _LL11A: _LL11B:
 goto _LL11D;case 16: _LL11C: _LL11D:
 goto _LL11F;case 15: _LL11E: _LL11F:
 return 0;default: _LL120: _LL121:
({struct Cyc_String_pa_PrintArg_struct _tmp245;_tmp245.tag=0;_tmp245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp243[1]={& _tmp245};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp244="atomic_typ:  bad type %s";_tag_dyneither(_tmp244,sizeof(char),25);}),_tag_dyneither(_tmp243,sizeof(void*),1));});});}_LLFD:;}
# 1040
static int Cyc_Toc_is_void_star(void*t){
void*_tmp24C=Cyc_Tcutil_compress(t);void*_tmp24D=_tmp24C;void*_tmp250;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24D)->tag == 5){_LL128: _tmp250=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24D)->f1).elt_typ;_LL129: {
# 1043
void*_tmp24E=Cyc_Tcutil_compress(_tmp250);void*_tmp24F=_tmp24E;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp24F)->tag == 0){_LL12D: _LL12E:
 return 1;}else{_LL12F: _LL130:
 return 0;}_LL12C:;}}else{_LL12A: _LL12B:
# 1047
 return 0;}_LL127:;}
# 1051
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1055
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1060
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp251=Cyc_Tcutil_compress(t);void*_tmp252=_tmp251;struct Cyc_List_List*_tmp25E;union Cyc_Absyn_AggrInfoU _tmp25D;switch(*((int*)_tmp252)){case 11: _LL132: _tmp25D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp252)->f1).aggr_info;_LL133: {
# 1063
struct Cyc_Absyn_Aggrdecl*_tmp253=Cyc_Absyn_get_known_aggrdecl(_tmp25D);
if(_tmp253->impl == 0)
({void*_tmp254=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp255="is_poly_field: type missing fields";_tag_dyneither(_tmp255,sizeof(char),35);}),_tag_dyneither(_tmp254,sizeof(void*),0));});
_tmp25E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp253->impl))->fields;goto _LL135;}case 12: _LL134: _tmp25E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp252)->f2;_LL135: {
# 1068
struct Cyc_Absyn_Aggrfield*_tmp256=Cyc_Absyn_lookup_field(_tmp25E,f);
if(_tmp256 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp259;_tmp259.tag=0;_tmp259.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp257[1]={& _tmp259};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp258="is_poly_field: bad field %s";_tag_dyneither(_tmp258,sizeof(char),28);}),_tag_dyneither(_tmp257,sizeof(void*),1));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp256->type);}default: _LL136: _LL137:
({struct Cyc_String_pa_PrintArg_struct _tmp25C;_tmp25C.tag=0;_tmp25C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp25A[1]={& _tmp25C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp25B="is_poly_field: bad type %s";_tag_dyneither(_tmp25B,sizeof(char),27);}),_tag_dyneither(_tmp25A,sizeof(void*),1));});});}_LL131:;}
# 1079
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp25F=e->r;void*_tmp260=_tmp25F;struct Cyc_Absyn_Exp*_tmp26A;struct _dyneither_ptr*_tmp269;struct Cyc_Absyn_Exp*_tmp268;struct _dyneither_ptr*_tmp267;switch(*((int*)_tmp260)){case 20: _LL139: _tmp268=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp260)->f1;_tmp267=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp260)->f2;_LL13A:
# 1082
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp268->topt),_tmp267) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 21: _LL13B: _tmp26A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp260)->f1;_tmp269=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp260)->f2;_LL13C: {
# 1085
void*_tmp261=Cyc_Tcutil_compress((void*)_check_null(_tmp26A->topt));void*_tmp262=_tmp261;void*_tmp266;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp262)->tag == 5){_LL140: _tmp266=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp262)->f1).elt_typ;_LL141:
# 1087
 return Cyc_Toc_is_poly_field(_tmp266,_tmp269) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LL142: _LL143:
({struct Cyc_String_pa_PrintArg_struct _tmp265;_tmp265.tag=0;_tmp265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp26A->topt)));({void*_tmp263[1]={& _tmp265};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp264="is_poly_project: bad type %s";_tag_dyneither(_tmp264,sizeof(char),29);}),_tag_dyneither(_tmp263,sizeof(void*),1));});});}_LL13F:;}default: _LL13D: _LL13E:
# 1090
 return 0;}_LL138:;}
# 1095
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->hd=s;_tmp26B->tl=0;_tmp26B;}),0);}
# 1099
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->hd=s;_tmp26C->tl=0;_tmp26C;}),0);}
# 1103
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1109
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp26D[2];_tmp26D[1]=s;_tmp26D[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26D,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1113
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__fast_region_malloc_e,({struct Cyc_Absyn_Exp*_tmp26E[2];_tmp26E[1]=s;_tmp26E[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26E,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1117
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp26F[2];_tmp26F[1]=n;_tmp26F[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26F,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{
# 1121
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp270[2];_tmp270[1]=n;_tmp270[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp270,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}
# 1124
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp271[3];_tmp271[2]=n;_tmp271[1]=s;_tmp271[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp271,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
# 1129
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272->hd=e;_tmp272->tl=0;_tmp272;}),0);}
# 1132
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,({struct Cyc_List_List*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->hd=e;_tmp273->tl=0;_tmp273;}),0);}
# 1136
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,({struct Cyc_Absyn_Exp*_tmp274[0];((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp274,sizeof(struct Cyc_Absyn_Exp*),0));}),0),0);}
# 1142
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1150
int is_string=0;
{void*_tmp275=e->r;void*_tmp276=_tmp275;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp276)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp276)->f1).Wstring_c).tag){case 8: _LL145: _LL146:
 is_string=1;goto _LL144;case 9: _LL147: _LL148:
 is_string=1;goto _LL144;default: goto _LL149;}else{_LL149: _LL14A:
 goto _LL144;}_LL144:;}{
# 1156
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp279;_tmp279.tag=0;_tmp279.f1=vd;_tmp279;});_tmp278;}),0);_tmp277->tl=Cyc_Toc_result_decls;_tmp277;});
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1166
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1168
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1170
struct Cyc_Absyn_Exp*urm_exp;
urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp27A[3];_tmp27A[2]=({struct _tuple19*_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D->f1=0;_tmp27D->f2=xplussz;_tmp27D;});_tmp27A[1]=({struct _tuple19*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->f1=0;_tmp27C->f2=xexp;_tmp27C;});_tmp27A[0]=({struct _tuple19*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B->f1=0;_tmp27B->f2=xexp;_tmp27B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp27A,sizeof(struct _tuple19*),3));}),0);
# 1174
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1213 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1215
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp27E=nv;int _tmp27F;_LL14C: _tmp27F=_tmp27E->toplevel;_LL14D:;
return _tmp27F;}
# 1219
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp280=nv;int*_tmp281;_LL14F: _tmp281=_tmp280->in_lhs;_LL150:;
return*_tmp281;}
# 1225
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp282=_region_malloc(r,sizeof(*_tmp282));_tmp282->break_lab=(struct _dyneither_ptr**)0;_tmp282->continue_lab=(struct _dyneither_ptr**)0;_tmp282->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp282->toplevel=(int)1;_tmp282->in_lhs=(int*)({int*_tmp285=_region_malloc(r,sizeof(*_tmp285));_tmp285[0]=0;_tmp285;});_tmp282->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp282->struct_info).varsizeexp=({struct Cyc_Absyn_Exp**_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284[0]=0;_tmp284;});(_tmp282->struct_info).lhs_exp=0;_tmp282->struct_info;});_tmp282->in_sizeof=(int*)({int*_tmp283=_region_malloc(r,sizeof(*_tmp283));_tmp283[0]=0;_tmp283;});_tmp282->rgn=(struct _RegionHandle*)r;_tmp282;});}
# 1236
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp286=e;struct _dyneither_ptr**_tmp28E;struct _dyneither_ptr**_tmp28D;struct Cyc_Toc_FallthruInfo*_tmp28C;int _tmp28B;int*_tmp28A;struct Cyc_Toc_StructInfo _tmp289;int*_tmp288;_LL152: _tmp28E=_tmp286->break_lab;_tmp28D=_tmp286->continue_lab;_tmp28C=_tmp286->fallthru_info;_tmp28B=_tmp286->toplevel;_tmp28A=_tmp286->in_lhs;_tmp289=_tmp286->struct_info;_tmp288=_tmp286->in_sizeof;_LL153:;
return({struct Cyc_Toc_Env*_tmp287=_region_malloc(r,sizeof(*_tmp287));_tmp287->break_lab=(struct _dyneither_ptr**)_tmp28E;_tmp287->continue_lab=(struct _dyneither_ptr**)_tmp28D;_tmp287->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp28C;_tmp287->toplevel=(int)_tmp28B;_tmp287->in_lhs=(int*)_tmp28A;_tmp287->struct_info=(struct Cyc_Toc_StructInfo)_tmp289;_tmp287->in_sizeof=(int*)_tmp288;_tmp287->rgn=(struct _RegionHandle*)r;_tmp287;});}
# 1241
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp28F=e;struct _dyneither_ptr**_tmp297;struct _dyneither_ptr**_tmp296;struct Cyc_Toc_FallthruInfo*_tmp295;int _tmp294;int*_tmp293;struct Cyc_Toc_StructInfo _tmp292;int*_tmp291;_LL155: _tmp297=_tmp28F->break_lab;_tmp296=_tmp28F->continue_lab;_tmp295=_tmp28F->fallthru_info;_tmp294=_tmp28F->toplevel;_tmp293=_tmp28F->in_lhs;_tmp292=_tmp28F->struct_info;_tmp291=_tmp28F->in_sizeof;_LL156:;
return({struct Cyc_Toc_Env*_tmp290=_region_malloc(r,sizeof(*_tmp290));_tmp290->break_lab=(struct _dyneither_ptr**)_tmp297;_tmp290->continue_lab=(struct _dyneither_ptr**)_tmp296;_tmp290->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp295;_tmp290->toplevel=(int)0;_tmp290->in_lhs=(int*)_tmp293;_tmp290->struct_info=(struct Cyc_Toc_StructInfo)_tmp292;_tmp290->in_sizeof=(int*)_tmp291;_tmp290->rgn=(struct _RegionHandle*)r;_tmp290;});}
# 1245
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp298=e;struct _dyneither_ptr**_tmp2A0;struct _dyneither_ptr**_tmp29F;struct Cyc_Toc_FallthruInfo*_tmp29E;int _tmp29D;int*_tmp29C;struct Cyc_Toc_StructInfo _tmp29B;int*_tmp29A;_LL158: _tmp2A0=_tmp298->break_lab;_tmp29F=_tmp298->continue_lab;_tmp29E=_tmp298->fallthru_info;_tmp29D=_tmp298->toplevel;_tmp29C=_tmp298->in_lhs;_tmp29B=_tmp298->struct_info;_tmp29A=_tmp298->in_sizeof;_LL159:;
return({struct Cyc_Toc_Env*_tmp299=_region_malloc(r,sizeof(*_tmp299));_tmp299->break_lab=(struct _dyneither_ptr**)_tmp2A0;_tmp299->continue_lab=(struct _dyneither_ptr**)_tmp29F;_tmp299->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp29E;_tmp299->toplevel=(int)1;_tmp299->in_lhs=(int*)_tmp29C;_tmp299->struct_info=(struct Cyc_Toc_StructInfo)_tmp29B;_tmp299->in_sizeof=(int*)_tmp29A;_tmp299->rgn=(struct _RegionHandle*)r;_tmp299;});}
# 1249
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp2A1=e;int*_tmp2A2;_LL15B: _tmp2A2=_tmp2A1->in_lhs;_LL15C:;
*_tmp2A2=b;}
# 1253
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp2A3=e;int*_tmp2A5;_LL15E: _tmp2A5=_tmp2A3->in_sizeof;_LL15F:;{
int _tmp2A4=*_tmp2A5;
*_tmp2A5=b;
return _tmp2A4;};}
# 1259
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp2A6=e;int*_tmp2A7;_LL161: _tmp2A7=_tmp2A6->in_sizeof;_LL162:;
return*_tmp2A7;}
# 1264
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1266
struct Cyc_Toc_Env*_tmp2A8=e;struct _dyneither_ptr**_tmp2B0;struct _dyneither_ptr**_tmp2AF;struct Cyc_Toc_FallthruInfo*_tmp2AE;int _tmp2AD;int*_tmp2AC;struct Cyc_Absyn_Exp**_tmp2AB;int*_tmp2AA;_LL164: _tmp2B0=_tmp2A8->break_lab;_tmp2AF=_tmp2A8->continue_lab;_tmp2AE=_tmp2A8->fallthru_info;_tmp2AD=_tmp2A8->toplevel;_tmp2AC=_tmp2A8->in_lhs;_tmp2AB=(_tmp2A8->struct_info).varsizeexp;_tmp2AA=_tmp2A8->in_sizeof;_LL165:;
return({struct Cyc_Toc_Env*_tmp2A9=_region_malloc(r,sizeof(*_tmp2A9));_tmp2A9->break_lab=(struct _dyneither_ptr**)_tmp2B0;_tmp2A9->continue_lab=(struct _dyneither_ptr**)_tmp2AF;_tmp2A9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2AE;_tmp2A9->toplevel=(int)_tmp2AD;_tmp2A9->in_lhs=(int*)_tmp2AC;_tmp2A9->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp2A9->struct_info).varsizeexp=_tmp2AB;(_tmp2A9->struct_info).lhs_exp=exp;_tmp2A9->struct_info;});_tmp2A9->in_sizeof=(int*)_tmp2AA;_tmp2A9->rgn=(struct _RegionHandle*)r;_tmp2A9;});}
# 1272
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp2B1=e;struct _dyneither_ptr**_tmp2B9;struct _dyneither_ptr**_tmp2B8;struct Cyc_Toc_FallthruInfo*_tmp2B7;int _tmp2B6;int*_tmp2B5;struct Cyc_Toc_StructInfo _tmp2B4;int*_tmp2B3;_LL167: _tmp2B9=_tmp2B1->break_lab;_tmp2B8=_tmp2B1->continue_lab;_tmp2B7=_tmp2B1->fallthru_info;_tmp2B6=_tmp2B1->toplevel;_tmp2B5=_tmp2B1->in_lhs;_tmp2B4=_tmp2B1->struct_info;_tmp2B3=_tmp2B1->in_sizeof;_LL168:;
return({struct Cyc_Toc_Env*_tmp2B2=_region_malloc(r,sizeof(*_tmp2B2));_tmp2B2->break_lab=(struct _dyneither_ptr**)0;_tmp2B2->continue_lab=(struct _dyneither_ptr**)0;_tmp2B2->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2B7;_tmp2B2->toplevel=(int)_tmp2B6;_tmp2B2->in_lhs=(int*)_tmp2B5;_tmp2B2->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B4;_tmp2B2->in_sizeof=(int*)_tmp2B3;_tmp2B2->rgn=(struct _RegionHandle*)r;_tmp2B2;});}
# 1278
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1282
struct Cyc_Toc_Env*_tmp2BA=e;struct _dyneither_ptr**_tmp2C4;struct _dyneither_ptr**_tmp2C3;struct Cyc_Toc_FallthruInfo*_tmp2C2;int _tmp2C1;int*_tmp2C0;struct Cyc_Toc_StructInfo _tmp2BF;int*_tmp2BE;_LL16A: _tmp2C4=_tmp2BA->break_lab;_tmp2C3=_tmp2BA->continue_lab;_tmp2C2=_tmp2BA->fallthru_info;_tmp2C1=_tmp2BA->toplevel;_tmp2C0=_tmp2BA->in_lhs;_tmp2BF=_tmp2BA->struct_info;_tmp2BE=_tmp2BA->in_sizeof;_LL16B:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp2BD=_region_malloc(r,sizeof(*_tmp2BD));_tmp2BD->label=fallthru_l;_tmp2BD->binders=fallthru_binders;_tmp2BD;});
# 1285
return({struct Cyc_Toc_Env*_tmp2BB=_region_malloc(r,sizeof(*_tmp2BB));_tmp2BB->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2BC=_region_malloc(r,sizeof(*_tmp2BC));_tmp2BC[0]=break_l;_tmp2BC;});_tmp2BB->continue_lab=(struct _dyneither_ptr**)_tmp2C3;_tmp2BB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi;_tmp2BB->toplevel=(int)_tmp2C1;_tmp2BB->in_lhs=(int*)_tmp2C0;_tmp2BB->struct_info=(struct Cyc_Toc_StructInfo)_tmp2BF;_tmp2BB->in_sizeof=(int*)_tmp2BE;_tmp2BB->rgn=(struct _RegionHandle*)r;_tmp2BB;});};}
# 1288
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1290
struct Cyc_Toc_Env*_tmp2C5=e;struct _dyneither_ptr**_tmp2CE;struct _dyneither_ptr**_tmp2CD;struct Cyc_Toc_FallthruInfo*_tmp2CC;int _tmp2CB;int*_tmp2CA;struct Cyc_Toc_StructInfo _tmp2C9;int*_tmp2C8;_LL16D: _tmp2CE=_tmp2C5->break_lab;_tmp2CD=_tmp2C5->continue_lab;_tmp2CC=_tmp2C5->fallthru_info;_tmp2CB=_tmp2C5->toplevel;_tmp2CA=_tmp2C5->in_lhs;_tmp2C9=_tmp2C5->struct_info;_tmp2C8=_tmp2C5->in_sizeof;_LL16E:;
return({struct Cyc_Toc_Env*_tmp2C6=_region_malloc(r,sizeof(*_tmp2C6));_tmp2C6->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2C7=_region_malloc(r,sizeof(*_tmp2C7));_tmp2C7[0]=break_l;_tmp2C7;});_tmp2C6->continue_lab=(struct _dyneither_ptr**)_tmp2CD;_tmp2C6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp2C6->toplevel=(int)_tmp2CB;_tmp2C6->in_lhs=(int*)_tmp2CA;_tmp2C6->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C9;_tmp2C6->in_sizeof=(int*)_tmp2C8;_tmp2C6->rgn=(struct _RegionHandle*)r;_tmp2C6;});}
# 1297
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1300
struct Cyc_Toc_Env*_tmp2CF=e;struct _dyneither_ptr**_tmp2D8;struct _dyneither_ptr**_tmp2D7;struct Cyc_Toc_FallthruInfo*_tmp2D6;int _tmp2D5;int*_tmp2D4;struct Cyc_Toc_StructInfo _tmp2D3;int*_tmp2D2;_LL170: _tmp2D8=_tmp2CF->break_lab;_tmp2D7=_tmp2CF->continue_lab;_tmp2D6=_tmp2CF->fallthru_info;_tmp2D5=_tmp2CF->toplevel;_tmp2D4=_tmp2CF->in_lhs;_tmp2D3=_tmp2CF->struct_info;_tmp2D2=_tmp2CF->in_sizeof;_LL171:;
return({struct Cyc_Toc_Env*_tmp2D0=_region_malloc(r,sizeof(*_tmp2D0));_tmp2D0->break_lab=(struct _dyneither_ptr**)0;_tmp2D0->continue_lab=(struct _dyneither_ptr**)_tmp2D7;_tmp2D0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2D1=_region_malloc(r,sizeof(*_tmp2D1));_tmp2D1->label=next_l;_tmp2D1->binders=0;_tmp2D1;});_tmp2D0->toplevel=(int)_tmp2D5;_tmp2D0->in_lhs=(int*)_tmp2D4;_tmp2D0->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D3;_tmp2D0->in_sizeof=(int*)_tmp2D2;_tmp2D0->rgn=(struct _RegionHandle*)r;_tmp2D0;});}
# 1313 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1316
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1318
void*_tmp2D9=e->annot;void*_tmp2DA=_tmp2D9;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2DA)->tag == Cyc_CfFlowInfo_UnknownZ){_LL173: _LL174:
# 1321
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2DA)->tag == Cyc_CfFlowInfo_NotZero){_LL175: _LL176:
# 1324
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2DA)->tag == Cyc_CfFlowInfo_IsZero){_LL177: _LL178:
# 1326
({void*_tmp2DB=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2DC="dereference of NULL pointer";_tag_dyneither(_tmp2DC,sizeof(char),28);}),_tag_dyneither(_tmp2DB,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2DA)->tag == Cyc_Absyn_EmptyAnnot){_LL179: _LL17A:
# 1332
 return 1;}else{_LL17B: _LL17C:
({void*_tmp2DD=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2DE="need_null_check";_tag_dyneither(_tmp2DE,sizeof(char),16);}),_tag_dyneither(_tmp2DD,sizeof(void*),0));});}}}}_LL172:;}
# 1337
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2DF=e->annot;void*_tmp2E0=_tmp2DF;struct Cyc_List_List*_tmp2E6;struct Cyc_List_List*_tmp2E5;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2E0)->tag == Cyc_CfFlowInfo_UnknownZ){_LL17E: _tmp2E5=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2E0)->f1;_LL17F:
 return _tmp2E5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2E0)->tag == Cyc_CfFlowInfo_NotZero){_LL180: _tmp2E6=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2E0)->f1;_LL181:
 return _tmp2E6;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2E0)->tag == Cyc_CfFlowInfo_IsZero){_LL182: _LL183:
# 1342
({void*_tmp2E1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2E2="dereference of NULL pointer";_tag_dyneither(_tmp2E2,sizeof(char),28);}),_tag_dyneither(_tmp2E1,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2E0)->tag == Cyc_Absyn_EmptyAnnot){_LL184: _LL185:
 return 0;}else{_LL186: _LL187:
({void*_tmp2E3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E4="get_relns";_tag_dyneither(_tmp2E4,sizeof(char),10);}),_tag_dyneither(_tmp2E3,sizeof(void*),0));});}}}}_LL17D:;}static char _tmp2EA[8]="*bogus*";
# 1352
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1362 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1365
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2E7=a->r;void*_tmp2E8=_tmp2E7;void*_tmp2EB;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E8)->tag == 1){_LL189: _tmp2EB=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1;_LL18A: {
# 1368
struct Cyc_Absyn_Vardecl*_tmp2E9=Cyc_Tcutil_nonesc_vardecl(_tmp2EB);
if(_tmp2E9 == 0)goto _LL18C;
x=_tmp2E9;
goto _LL188;}}else{_LL18B: _LL18C: {
# 1374
static struct _dyneither_ptr bogus_string={_tmp2EA,_tmp2EA,_tmp2EA + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1377
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1383
x=& bogus_vardecl;
x->type=a_typ;}}_LL188:;}{
# 1386
void*_tmp2EC=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1391
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2EC);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1400
if(valid_rop_i){
struct Cyc_List_List*_tmp2ED=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1408
if(!Cyc_Relations_consistent_relations(_tmp2ED))return 1;}
# 1411
inner_loop: {
void*_tmp2EE=i->r;void*_tmp2EF=_tmp2EE;struct Cyc_Absyn_Exp*_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F3;void*_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F1;switch(*((int*)_tmp2EF)){case 13: _LL18E: _tmp2F2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EF)->f1;_tmp2F1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EF)->f2;_LL18F:
 i=_tmp2F1;goto inner_loop;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EF)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EF)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EF)->f2)->tl != 0){_LL190: _tmp2F4=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EF)->f2)->hd;_tmp2F3=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EF)->f2)->tl)->hd;_LL191: {
# 1417
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp2F3,& rop_z)){
# 1420
struct Cyc_List_List*_tmp2F0=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1422
return !Cyc_Relations_consistent_relations(_tmp2F0);}
# 1424
goto _LL18D;}}else{goto _LL192;}}else{goto _LL192;}}else{goto _LL192;}default: _LL192: _LL193:
 goto _LL18D;}_LL18D:;}
# 1427
return 0;};}
# 1430
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2F5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2F6="Missing type in primop ";_tag_dyneither(_tmp2F6,sizeof(char),24);}),_tag_dyneither(_tmp2F5,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1434
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2F7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2F8="Missing type in primop ";_tag_dyneither(_tmp2F8,sizeof(char),24);}),_tag_dyneither(_tmp2F7,sizeof(void*),0));});
return(void*)_check_null(e->topt);}
# 1438
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple11*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->f1=Cyc_Toc_mt_tq;_tmp2F9->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp2F9;});}
# 1441
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);
return({struct _tuple19*_tmp2FA=_cycalloc(sizeof(*_tmp2FA));_tmp2FA->f1=0;_tmp2FA->f2=e;_tmp2FA;});}
# 1448
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1451
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2FB=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp2FB=Cyc_Absyn_add_exp(_tmp2FB,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1459
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp2FB): Cyc_Toc_malloc_ptr(_tmp2FB);else{
# 1463
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp2FB);}}else{
# 1468
t=struct_typ;
eo=0;}
# 1471
if(do_declare){
void*_tmp2FC=x->r;void*_tmp2FD=_tmp2FC;void*_tmp300;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2FD)->tag == 1){_LL195: _tmp300=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_LL196:
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(Cyc_Absyn_binding2qvar(_tmp300),t,eo,s,0),0);}else{_LL197: _LL198:
({void*_tmp2FE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2FF="bogus x to make_struct";_tag_dyneither(_tmp2FF,sizeof(char),23);}),_tag_dyneither(_tmp2FE,sizeof(void*),0));});}_LL194:;}else{
# 1478
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1481
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1485
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp301=Cyc_Tcutil_compress(typ);void*_tmp302=_tmp301;struct Cyc_List_List*_tmp305;union Cyc_Absyn_AggrInfoU _tmp304;struct Cyc_List_List*_tmp303;switch(*((int*)_tmp302)){case 11: _LL19A: _tmp304=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp302)->f1).aggr_info;_tmp303=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp302)->f1).targs;_LL19B: {
# 1488
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp304);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp305=aggrfields;goto _LL19D;};}case 12: _LL19C: _tmp305=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp302)->f2;_LL19D:
# 1493
 if(_tmp305 == 0)return 0;
return 1;default: _LL19E: _LL19F:
# 1499
 return 0;}_LL199:;}
# 1503
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp306=Cyc_Tcutil_compress(t);void*_tmp307=_tmp306;void*_tmp30B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp307)->tag == 8){_LL1A1: _tmp30B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp307)->f1).elt_type;_LL1A2:
 return _tmp30B;}else{_LL1A3: _LL1A4:
({struct Cyc_String_pa_PrintArg_struct _tmp30A;_tmp30A.tag=0;_tmp30A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp308[1]={& _tmp30A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp309="array_elt_type called on %s";_tag_dyneither(_tmp309,sizeof(char),28);}),_tag_dyneither(_tmp308,sizeof(void*),1));});});}_LL1A0:;}
# 1510
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1516
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1521
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1526
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp30C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp30C != 0;_tmp30C=_tmp30C->tl){
# 1530
struct _tuple19*_tmp30D=(struct _tuple19*)_tmp30C->hd;struct _tuple19*_tmp30E=_tmp30D;struct Cyc_List_List*_tmp320;struct Cyc_Absyn_Exp*_tmp31F;_LL1A6: _tmp320=_tmp30E->f1;_tmp31F=_tmp30E->f2;_LL1A7:;{
# 1534
struct Cyc_Absyn_Exp*e_index;
if(_tmp320 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1538
if(_tmp320->tl != 0)({void*_tmp30F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp310="multiple designators in array";_tag_dyneither(_tmp310,sizeof(char),30);}),_tag_dyneither(_tmp30F,sizeof(void*),0));});{
void*_tmp311=(void*)_tmp320->hd;
void*_tmp312=_tmp311;struct Cyc_Absyn_Exp*_tmp315;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp312)->tag == 0){_LL1A9: _tmp315=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp312)->f1;_LL1AA:
# 1542
 Cyc_Toc_exp_to_c(nv,_tmp315);
e_index=_tmp315;
goto _LL1A8;}else{_LL1AB: _LL1AC:
({void*_tmp313=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp314="field name designators in array";_tag_dyneither(_tmp314,sizeof(char),32);}),_tag_dyneither(_tmp313,sizeof(void*),0));});}_LL1A8:;};}{
# 1548
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp316=_tmp31F->r;void*_tmp317=_tmp316;void*_tmp31E;struct Cyc_List_List*_tmp31D;struct Cyc_Absyn_Vardecl*_tmp31C;struct Cyc_Absyn_Exp*_tmp31B;struct Cyc_Absyn_Exp*_tmp31A;int _tmp319;struct Cyc_List_List*_tmp318;switch(*((int*)_tmp317)){case 25: _LL1AE: _tmp318=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp317)->f1;_LL1AF:
# 1551
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp318,s);
goto _LL1AD;case 26: _LL1B0: _tmp31C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp317)->f1;_tmp31B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp317)->f2;_tmp31A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp317)->f3;_tmp319=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp317)->f4;_LL1B1:
# 1554
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp31C,_tmp31B,_tmp31A,_tmp319,s,0);
# 1556
goto _LL1AD;case 29: _LL1B2: _tmp31E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp317)->f1;_tmp31D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp317)->f2;_LL1B3:
# 1558
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp31E,_tmp31D,s);
goto _LL1AD;case 27: _LL1B4: _LL1B5:
# 1561
 goto _LL1AD;default: _LL1B6: _LL1B7:
# 1563
 Cyc_Toc_exp_to_c(nv,_tmp31F);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp31F),0),s,0);
goto _LL1AD;}_LL1AD:;};};}}
# 1569
return s;}
# 1574
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1579
struct _tuple1*_tmp321=vd->name;
void*expected_elt_type;
{void*_tmp322=Cyc_Tcutil_compress(comprehension_type);void*_tmp323=_tmp322;void*_tmp327;void*_tmp326;switch(*((int*)_tmp323)){case 8: _LL1B9: _tmp326=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp323)->f1).elt_type;_LL1BA:
# 1583
 expected_elt_type=_tmp326;goto _LL1B8;case 5: _LL1BB: _tmp327=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp323)->f1).elt_typ;_LL1BC:
# 1585
 expected_elt_type=_tmp327;goto _LL1B8;default: _LL1BD: _LL1BE:
({void*_tmp324=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp325="init_comprehension passed a bad type!";_tag_dyneither(_tmp325,sizeof(char),38);}),_tag_dyneither(_tmp324,sizeof(void*),0));});}_LL1B8:;}{
# 1588
void*_tmp328=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1592
{void*_tmp329=e2->r;void*_tmp32A=_tmp329;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp32A)->tag == 27){_LL1C0: _LL1C1:
 return Cyc_Absyn_skip_stmt(0);}else{_LL1C2: _LL1C3:
 goto _LL1BF;}_LL1BF:;}{
# 1596
struct _RegionHandle _tmp32B=_new_region("r2");struct _RegionHandle*r2=& _tmp32B;_push_region(r2);
{struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp321,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp321,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp321,0),0);
# 1603
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp321,0),0);
struct Cyc_Absyn_Stmt*body;
# 1607
{void*_tmp32C=e2->r;void*_tmp32D=_tmp32C;void*_tmp334;struct Cyc_List_List*_tmp333;struct Cyc_Absyn_Vardecl*_tmp332;struct Cyc_Absyn_Exp*_tmp331;struct Cyc_Absyn_Exp*_tmp330;int _tmp32F;struct Cyc_List_List*_tmp32E;switch(*((int*)_tmp32D)){case 25: _LL1C5: _tmp32E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp32D)->f1;_LL1C6:
# 1609
 body=Cyc_Toc_init_array(nv,expected_elt_type,lval,_tmp32E,Cyc_Toc_skip_stmt_dl());
goto _LL1C4;case 26: _LL1C7: _tmp332=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32D)->f1;_tmp331=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32D)->f2;_tmp330=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32D)->f3;_tmp32F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32D)->f4;_LL1C8:
# 1612
 body=Cyc_Toc_init_comprehension(nv,expected_elt_type,lval,_tmp332,_tmp331,_tmp330,_tmp32F,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1C4;case 29: _LL1C9: _tmp334=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp32D)->f1;_tmp333=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp32D)->f2;_LL1CA:
# 1615
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp334,_tmp333,Cyc_Toc_skip_stmt_dl());
goto _LL1C4;default: _LL1CB: _LL1CC:
# 1618
 Cyc_Toc_exp_to_c(nv,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL1C4;}_LL1C4:;}{
# 1622
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1624
if(zero_term){
# 1629
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp328,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1634
struct Cyc_Absyn_Stmt*_tmp335=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp321,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp335;};};}
# 1597
;_pop_region(r2);};};}
# 1640
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1643
{struct Cyc_List_List*_tmp336=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp336 != 0;_tmp336=_tmp336->tl){
struct _tuple19*_tmp337=(struct _tuple19*)_tmp336->hd;struct _tuple19*_tmp338=_tmp337;struct Cyc_List_List*_tmp34C;struct Cyc_Absyn_Exp*_tmp34B;_LL1CE: _tmp34C=_tmp338->f1;_tmp34B=_tmp338->f2;_LL1CF:;
if(_tmp34C == 0)
({void*_tmp339=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp33A="empty designator list";_tag_dyneither(_tmp33A,sizeof(char),22);}),_tag_dyneither(_tmp339,sizeof(void*),0));});
if(_tmp34C->tl != 0)
({void*_tmp33B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp33C="too many designators in anonymous struct";_tag_dyneither(_tmp33C,sizeof(char),41);}),_tag_dyneither(_tmp33B,sizeof(void*),0));});{
void*_tmp33D=(void*)_tmp34C->hd;void*_tmp33E=_tmp33D;struct _dyneither_ptr*_tmp34A;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33E)->tag == 1){_LL1D1: _tmp34A=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33E)->f1;_LL1D2: {
# 1651
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp34A,0);
{void*_tmp33F=_tmp34B->r;void*_tmp340=_tmp33F;void*_tmp347;struct Cyc_List_List*_tmp346;struct Cyc_Absyn_Vardecl*_tmp345;struct Cyc_Absyn_Exp*_tmp344;struct Cyc_Absyn_Exp*_tmp343;int _tmp342;struct Cyc_List_List*_tmp341;switch(*((int*)_tmp340)){case 25: _LL1D6: _tmp341=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp340)->f1;_LL1D7:
# 1654
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp34B->topt),lval,_tmp341,s);goto _LL1D5;case 26: _LL1D8: _tmp345=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp340)->f1;_tmp344=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp340)->f2;_tmp343=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp340)->f3;_tmp342=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp340)->f4;_LL1D9:
# 1656
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp34B->topt),lval,_tmp345,_tmp344,_tmp343,_tmp342,s,0);
goto _LL1D5;case 29: _LL1DA: _tmp347=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp340)->f1;_tmp346=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp340)->f2;_LL1DB:
# 1659
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp347,_tmp346,s);goto _LL1D5;case 27: _LL1DC: _LL1DD:
# 1661
 goto _LL1D5;default: _LL1DE: _LL1DF:
# 1663
 Cyc_Toc_exp_to_c(nv,_tmp34B);
# 1665
if(Cyc_Toc_is_poly_field(struct_type,_tmp34A))
_tmp34B=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp34B);
# 1668
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp34B,0),0),s,0);
goto _LL1D5;}_LL1D5:;}
# 1671
goto _LL1D0;}}else{_LL1D3: _LL1D4:
({void*_tmp348=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp349="array designator in struct";_tag_dyneither(_tmp349,sizeof(char),27);}),_tag_dyneither(_tmp348,sizeof(void*),0));});}_LL1D0:;};}}
# 1675
return s;}
# 1680
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1684
struct _RegionHandle _tmp34D=_new_region("r");struct _RegionHandle*r=& _tmp34D;_push_region(r);
{struct Cyc_List_List*_tmp34E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp34F=Cyc_Toc_add_tuple_type(_tmp34E);
# 1688
struct _tuple1*_tmp350=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp351=Cyc_Absyn_var_exp(_tmp350,0);
struct Cyc_Absyn_Stmt*_tmp352=Cyc_Absyn_exp_stmt(_tmp351,0);
# 1692
struct Cyc_Absyn_Exp*(*_tmp353)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1694
int is_atomic=1;
struct Cyc_List_List*_tmp354=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp354);for(0;_tmp354 != 0;(_tmp354=_tmp354->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp354->hd;
struct Cyc_Absyn_Exp*lval=_tmp353(_tmp351,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp355=e->r;void*_tmp356=_tmp355;struct Cyc_Absyn_Vardecl*_tmp35B;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*_tmp359;int _tmp358;struct Cyc_List_List*_tmp357;switch(*((int*)_tmp356)){case 25: _LL1E1: _tmp357=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp356)->f1;_LL1E2:
# 1702
 _tmp352=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp357,_tmp352);
goto _LL1E0;case 26: _LL1E3: _tmp35B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp356)->f1;_tmp35A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp356)->f2;_tmp359=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp356)->f3;_tmp358=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp356)->f4;_LL1E4:
# 1705
 _tmp352=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp35B,_tmp35A,_tmp359,_tmp358,_tmp352,0);
# 1707
goto _LL1E0;case 27: _LL1E5: _LL1E6:
# 1709
 goto _LL1E0;default: _LL1E7: _LL1E8:
# 1712
 Cyc_Toc_exp_to_c(nv,e);
_tmp352=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp353(_tmp351,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp352,0);
# 1715
goto _LL1E0;}_LL1E0:;};}}{
# 1718
struct Cyc_Absyn_Exp*_tmp35C=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp350,0),_tmp34F,_tmp352,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp35C;};}
# 1685
;_pop_region(r);}
# 1722
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp35D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp35D != 0;_tmp35D=_tmp35D->tl){
struct Cyc_Absyn_Aggrfield*_tmp35E=(struct Cyc_Absyn_Aggrfield*)_tmp35D->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp35E->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1729
({void*_tmp35F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp362;_tmp362.tag=0;_tmp362.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp360[1]={& _tmp362};Cyc_aprintf(({const char*_tmp361="get_member_offset %s failed";_tag_dyneither(_tmp361,sizeof(char),28);}),_tag_dyneither(_tmp360,sizeof(void*),1));});}),_tag_dyneither(_tmp35F,sizeof(void*),0));});}
# 1732
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp363=Cyc_Tcutil_compress(*typ);void*_tmp364=_tmp363;void*_tmp36C;struct Cyc_Absyn_Tqual _tmp36B;struct Cyc_Absyn_Exp*_tmp36A;union Cyc_Absyn_Constraint*_tmp369;unsigned int _tmp368;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp364)->tag == 8){_LL1EA: _tmp36C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp364)->f1).elt_type;_tmp36B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp364)->f1).tq;_tmp36A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp364)->f1).num_elts;_tmp369=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp364)->f1).zero_term;_tmp368=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp364)->f1).zt_loc;_LL1EB:
# 1737
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp36A))){
*typ=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp365=_cycalloc(sizeof(*_tmp365));_tmp365[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp366;_tmp366.tag=8;_tmp366.f1=({struct Cyc_Absyn_ArrayInfo _tmp367;_tmp367.elt_type=_tmp36C;_tmp367.tq=_tmp36B;_tmp367.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp367.zero_term=_tmp369;_tmp367.zt_loc=_tmp368;_tmp367;});_tmp366;});_tmp365;});
return 1;}
# 1741
goto _LL1E9;}else{_LL1EC: _LL1ED:
 goto _LL1E9;}_LL1E9:;}
# 1744
return 0;}
# 1747
static int Cyc_Toc_is_array_type(void*t){
void*_tmp36D=Cyc_Tcutil_compress(t);void*_tmp36E=_tmp36D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36E)->tag == 8){_LL1EF: _LL1F0:
 return 1;}else{_LL1F1: _LL1F2:
 return 0;}_LL1EE:;}
# 1754
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1756
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1760
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp36F=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp370=_tmp36F->attributes;
void*_tmp371=_tmp36F->type;
if((!Cyc_Toc_is_array_type(_tmp371),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp371))))
_tmp370=({struct Cyc_List_List*_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp374;_tmp374.tag=6;_tmp374.f1=0;_tmp374;});_tmp373;});_tmp372->tl=_tmp370;_tmp372;});{
struct Cyc_Absyn_Aggrfield*_tmp375=({struct Cyc_Absyn_Aggrfield*_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377->name=_tmp36F->name;_tmp377->tq=Cyc_Toc_mt_tq;_tmp377->type=new_field_type;_tmp377->width=_tmp36F->width;_tmp377->attributes=_tmp370;_tmp377->requires_clause=0;_tmp377;});
# 1772
new_fields=({struct Cyc_List_List*_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376->hd=_tmp375;_tmp376->tl=new_fields;_tmp376;});};}else{
# 1774
new_fields=({struct Cyc_List_List*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp378->tl=new_fields;_tmp378;});}}{
# 1776
struct Cyc_List_List*_tmp379=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp381;_tmp381.tag=1;_tmp381.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp37F[1]={& _tmp381};Cyc_aprintf(({const char*_tmp380="_genStruct%d";_tag_dyneither(_tmp380,sizeof(char),13);}),_tag_dyneither(_tmp37F,sizeof(void*),1));});});_tmp37E;});
# 1779
struct _tuple1*qv=({struct _tuple1*_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D->f1=Cyc_Absyn_Abs_n(0,1);_tmp37D->f2=name;_tmp37D;});
struct Cyc_Absyn_AggrdeclImpl*_tmp37A=({struct Cyc_Absyn_AggrdeclImpl*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;_tmp37C->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;_tmp37C->fields=_tmp379;_tmp37C->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;_tmp37C;});
# 1784
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B->kind=ad->kind;_tmp37B->sc=ad->sc;_tmp37B->name=qv;_tmp37B->tvs=ad->tvs;_tmp37B->impl=_tmp37A;_tmp37B->expected_mem_kind=0;_tmp37B->attributes=ad->attributes;_tmp37B;});
# 1791
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1795
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1801
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp382=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp382,0);
do_declare=1;}else{
# 1809
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1812
struct Cyc_Absyn_Stmt*_tmp383=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1816
struct Cyc_Absyn_Exp**varexp;
{void*_tmp384=_tmp383->r;void*_tmp385=_tmp384;struct Cyc_Absyn_Exp**_tmp388;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp385)->tag == 1){_LL1F4: _tmp388=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp385)->f1;_LL1F5:
 varexp=_tmp388;goto _LL1F3;}else{_LL1F6: _LL1F7:
({void*_tmp386=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp387="stmt not an expression!";_tag_dyneither(_tmp387,sizeof(char),24);}),_tag_dyneither(_tmp386,sizeof(void*),0));});}_LL1F3:;}{
# 1822
struct Cyc_Absyn_Exp*(*_tmp389)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1826
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp38A=Cyc_Tcutil_compress(struct_type);void*_tmp38B=_tmp38A;union Cyc_Absyn_AggrInfoU _tmp38F;struct Cyc_List_List*_tmp38E;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B)->tag == 11){_LL1F9: _tmp38F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B)->f1).aggr_info;_tmp38E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B)->f1).targs;_LL1FA:
# 1833
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp38F);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp38E;
goto _LL1F8;}else{_LL1FB: _LL1FC:
({void*_tmp38C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp38D="init_struct: bad struct type";_tag_dyneither(_tmp38D,sizeof(char),29);}),_tag_dyneither(_tmp38C,sizeof(void*),0));});}_LL1F8:;}
# 1840
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp390=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp390->tl != 0;_tmp390=_tmp390->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp391=(struct Cyc_Absyn_Aggrfield*)_tmp390->hd;
struct _RegionHandle _tmp392=_new_region("temp");struct _RegionHandle*temp=& _tmp392;_push_region(temp);
{struct Cyc_List_List*_tmp393=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1849
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp393,_tmp391->type))){
struct Cyc_List_List*_tmp394=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp395=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp393,_tmp394);
# 1853
struct Cyc_List_List*new_fields=0;
for(_tmp390=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp390 != 0;_tmp390=_tmp390->tl){
struct Cyc_Absyn_Aggrfield*_tmp396=(struct Cyc_Absyn_Aggrfield*)_tmp390->hd;
struct Cyc_Absyn_Aggrfield*_tmp397=({struct Cyc_Absyn_Aggrfield*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->name=_tmp396->name;_tmp39A->tq=Cyc_Toc_mt_tq;_tmp39A->type=
# 1858
Cyc_Tcutil_rsubstitute(temp,_tmp395,_tmp396->type);_tmp39A->width=_tmp396->width;_tmp39A->attributes=_tmp396->attributes;_tmp39A->requires_clause=0;_tmp39A;});
# 1866
if(_tmp390->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp397->type))
index=({struct _dyneither_ptr**_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398[0]=_tmp397->name;_tmp398;});}
# 1873
new_fields=({struct Cyc_List_List*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399->hd=_tmp397;_tmp399->tl=new_fields;_tmp399;});}
# 1875
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3A6;_tmp3A6.tag=1;_tmp3A6.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp3A4[1]={& _tmp3A6};Cyc_aprintf(({const char*_tmp3A5="_genStruct%d";_tag_dyneither(_tmp3A5,sizeof(char),13);}),_tag_dyneither(_tmp3A4,sizeof(void*),1));});});_tmp3A3;});
# 1879
struct _tuple1*qv=({struct _tuple1*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2->f1=Cyc_Absyn_Abs_n(0,1);_tmp3A2->f2=name;_tmp3A2;});
struct Cyc_Absyn_AggrdeclImpl*_tmp39B=({struct Cyc_Absyn_AggrdeclImpl*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->exist_vars=0;_tmp3A1->rgn_po=0;_tmp3A1->fields=aggrfields;_tmp3A1->tagged=0;_tmp3A1;});
# 1884
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->kind=Cyc_Absyn_StructA;_tmp3A0->sc=Cyc_Absyn_Public;_tmp3A0->name=qv;_tmp3A0->tvs=0;_tmp3A0->expected_mem_kind=0;_tmp3A0->impl=_tmp39B;_tmp3A0->attributes=0;_tmp3A0;});
# 1891
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp39D;_tmp39D.tag=11;_tmp39D.f1=({struct Cyc_Absyn_AggrInfo _tmp39E;_tmp39E.aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F[0]=new_ad;_tmp39F;}));_tmp39E.targs=0;_tmp39E;});_tmp39D;});_tmp39C;});
# 1895
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1845
;_pop_region(temp);};}{
# 1899
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3A7=_new_region("r");struct _RegionHandle*r=& _tmp3A7;_push_region(r);
{struct Cyc_List_List*_tmp3A8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3A8 != 0;_tmp3A8=_tmp3A8->tl){
struct _tuple19*_tmp3A9=(struct _tuple19*)_tmp3A8->hd;struct _tuple19*_tmp3AA=_tmp3A9;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3CE;_LL1FE: _tmp3CF=_tmp3AA->f1;_tmp3CE=_tmp3AA->f2;_LL1FF:;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3CE->topt));
if(_tmp3CF == 0)
({void*_tmp3AB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3AC="empty designator list";_tag_dyneither(_tmp3AC,sizeof(char),22);}),_tag_dyneither(_tmp3AB,sizeof(void*),0));});
if(_tmp3CF->tl != 0){
# 1909
struct _tuple1*_tmp3AD=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3AE=Cyc_Absyn_var_exp(_tmp3AD,0);
for(0;_tmp3CF != 0;_tmp3CF=_tmp3CF->tl){
void*_tmp3AF=(void*)_tmp3CF->hd;void*_tmp3B0=_tmp3AF;struct _dyneither_ptr*_tmp3B3;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B0)->tag == 1){_LL201: _tmp3B3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B0)->f1;_LL202:
# 1915
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3B3))
_tmp3AE=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3AE);
_tmp383=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp389(xexp,_tmp3B3,0),_tmp3AE,0),0),_tmp383,0);
# 1919
goto _LL200;}else{_LL203: _LL204:
({void*_tmp3B1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3B2="array designator in struct";_tag_dyneither(_tmp3B2,sizeof(char),27);}),_tag_dyneither(_tmp3B1,sizeof(void*),0));});}_LL200:;}
# 1923
Cyc_Toc_exp_to_c(nv,_tmp3CE);
_tmp383=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3AE,_tmp3CE,0),0),_tmp383,0);}else{
# 1926
void*_tmp3B4=(void*)_tmp3CF->hd;void*_tmp3B5=_tmp3B4;struct _dyneither_ptr*_tmp3CD;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B5)->tag == 1){_LL206: _tmp3CD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B5)->f1;_LL207: {
# 1928
struct Cyc_Absyn_Exp*lval=_tmp389(xexp,_tmp3CD,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3CD);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp383=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp383,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1937
int e1_translated=0;
{void*_tmp3B6=_tmp3CE->r;void*_tmp3B7=_tmp3B6;void*_tmp3CA;struct Cyc_List_List*_tmp3C9;struct Cyc_Absyn_Exp*_tmp3C8;void*_tmp3C7;struct Cyc_Absyn_Vardecl*_tmp3C6;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C4;int _tmp3C3;struct Cyc_List_List*_tmp3C2;switch(*((int*)_tmp3B7)){case 25: _LL20B: _tmp3C2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3B7)->f1;_LL20C:
# 1940
 _tmp383=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3CD)))->type,lval,_tmp3C2,_tmp383);
# 1942
goto _LL20A;case 26: _LL20D: _tmp3C6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B7)->f1;_tmp3C5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B7)->f2;_tmp3C4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B7)->f3;_tmp3C3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B7)->f4;_LL20E:
# 1944
 _tmp383=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3CD)))->type,lval,_tmp3C6,_tmp3C5,_tmp3C4,_tmp3C3,_tmp383,0);
# 1948
e1_translated=1;
_tmp3C8=_tmp3C5;_tmp3C7=(void*)_check_null(_tmp3C4->topt);goto _LL210;case 27: _LL20F: _tmp3C8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3B7)->f1;_tmp3C7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3B7)->f2;_LL210:
# 1953
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3CD)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3C8);{
void*_tmp3B8=Cyc_Toc_typ_to_c(_tmp3C7);
*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3B9[2];_tmp3B9[1]=
Cyc_Absyn_sizeoftyp_exp(_tmp3B8,0);_tmp3B9[0]=_tmp3C8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3B9,sizeof(struct Cyc_Absyn_Exp*),2));}),0),
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3BA;(_tmp3BA.Int_c).val=({struct _tuple6 _tmp3BB;_tmp3BB.f1=Cyc_Absyn_Unsigned;_tmp3BB.f2=(int)sizeof(double);_tmp3BB;});(_tmp3BA.Int_c).tag=5;_tmp3BA;}),0),0);};}
# 1961
goto _LL20A;case 29: _LL211: _tmp3CA=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3B7)->f1;_tmp3C9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3B7)->f2;_LL212:
# 1963
 _tmp383=Cyc_Toc_init_anon_struct(nv,lval,_tmp3CA,_tmp3C9,_tmp383);goto _LL20A;default: _LL213: _LL214: {
# 1965
void*old_e_typ=(void*)_check_null(_tmp3CE->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3CE);
{void*_tmp3BC=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3BC)->tag == 11){_LL216: _LL217:
# 1972
 if(old_e_typ != _tmp3CE->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3CE->topt))){
# 1976
ad=Cyc_Toc_update_aggr_type(ad,_tmp3CD,(void*)_check_null(_tmp3CE->topt));
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3BE;_tmp3BE.tag=11;_tmp3BE.f1=({struct Cyc_Absyn_AggrInfo _tmp3BF;_tmp3BF.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0[0]=ad;_tmp3C0;}));_tmp3BF.targs=0;_tmp3BF;});_tmp3BE;});_tmp3BD;});
# 1980
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1982
goto _LL215;}else{_LL218: _LL219:
 goto _LL215;}_LL215:;}{
# 1985
struct Cyc_Absyn_Aggrfield*_tmp3C1=Cyc_Absyn_lookup_field(aggrfields,_tmp3CD);
# 1987
if(Cyc_Toc_is_poly_field(struct_type,_tmp3CD) && !was_ptr_type)
_tmp3CE=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3CE);
# 1990
if(exist_types != 0)
_tmp3CE=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3C1))->type),_tmp3CE);
# 1993
_tmp383=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3CE,0),0),_tmp383,0);
goto _LL20A;};}}_LL20A:;}
# 1996
goto _LL205;};}}else{_LL208: _LL209:
({void*_tmp3CB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3CC="array designator in struct";_tag_dyneither(_tmp3CC,sizeof(char),27);}),_tag_dyneither(_tmp3CB,sizeof(void*),0));});}_LL205:;}}}
# 2003
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2008
struct Cyc_Absyn_Exp*_tmp3D0=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp383,pointer,rgnopt,is_atomic,do_declare);
# 2010
_tmp3D0->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3D1=_tmp3D0;_npop_handler(0);return _tmp3D1;};};
# 1901
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2016
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2019
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2022
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2[0]=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3D3;_tmp3D3.tag=4;_tmp3D3.f1=e1;_tmp3D3.f2=incr;_tmp3D3;});_tmp3D2;}),0);}
# 2026
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2035
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2037
void*_tmp3D4=e1->r;void*_tmp3D5=_tmp3D4;struct Cyc_Absyn_Exp*_tmp3DD;struct _dyneither_ptr*_tmp3DC;int _tmp3DB;int _tmp3DA;void*_tmp3D9;struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Stmt*_tmp3D7;switch(*((int*)_tmp3D5)){case 36: _LL21B: _tmp3D7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3D5)->f1;_LL21C:
 Cyc_Toc_lvalue_assign_stmt(_tmp3D7,fs,f,f_env);goto _LL21A;case 13: _LL21D: _tmp3D9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D5)->f1;_tmp3D8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D5)->f2;_LL21E:
 Cyc_Toc_lvalue_assign(_tmp3D8,fs,f,f_env);goto _LL21A;case 20: _LL21F: _tmp3DD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D5)->f1;_tmp3DC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D5)->f2;_tmp3DB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D5)->f3;_tmp3DA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D5)->f4;_LL220:
# 2042
 e1->r=_tmp3DD->r;
Cyc_Toc_lvalue_assign(e1,({struct Cyc_List_List*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->hd=_tmp3DC;_tmp3D6->tl=fs;_tmp3D6;}),f,f_env);
goto _LL21A;default: _LL221: _LL222: {
# 2050
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2052
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL21A;}}_LL21A:;}
# 2058
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2060
void*_tmp3DE=s->r;void*_tmp3DF=_tmp3DE;struct Cyc_Absyn_Stmt*_tmp3E6;struct Cyc_Absyn_Decl*_tmp3E5;struct Cyc_Absyn_Stmt*_tmp3E4;struct Cyc_Absyn_Exp*_tmp3E3;switch(*((int*)_tmp3DF)){case 1: _LL224: _tmp3E3=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3DF)->f1;_LL225:
 Cyc_Toc_lvalue_assign(_tmp3E3,fs,f,f_env);goto _LL223;case 12: _LL226: _tmp3E5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3DF)->f1;_tmp3E4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3DF)->f2;_LL227:
# 2063
 Cyc_Toc_lvalue_assign_stmt(_tmp3E4,fs,f,f_env);goto _LL223;case 2: _LL228: _tmp3E6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3DF)->f2;_LL229:
 Cyc_Toc_lvalue_assign_stmt(_tmp3E6,fs,f,f_env);goto _LL223;default: _LL22A: _LL22B:
({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0;_tmp3E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3E0[1]={& _tmp3E2};Cyc_Toc_toc_impos(({const char*_tmp3E1="lvalue_assign_stmt: %s";_tag_dyneither(_tmp3E1,sizeof(char),23);}),_tag_dyneither(_tmp3E0,sizeof(void*),1));});});}_LL223:;}
# 2069
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2073
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3E7=e->r;void*_tmp3E8=_tmp3E7;struct Cyc_Absyn_Stmt*_tmp3EF;struct Cyc_Absyn_Exp*_tmp3EE;void**_tmp3ED;struct Cyc_Absyn_Exp**_tmp3EC;switch(*((int*)_tmp3E8)){case 13: _LL22D: _tmp3ED=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E8)->f1;_tmp3EC=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E8)->f2;_LL22E:
# 2076
*_tmp3EC=Cyc_Toc_push_address_exp(*_tmp3EC);
*_tmp3ED=Cyc_Absyn_cstar_typ(*_tmp3ED,Cyc_Toc_mt_tq);
return e;case 19: _LL22F: _tmp3EE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3E8)->f1;_LL230:
# 2080
 return _tmp3EE;case 36: _LL231: _tmp3EF=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3E8)->f1;_LL232:
# 2084
 Cyc_Toc_push_address_stmt(_tmp3EF);
return e;default: _LL233: _LL234:
# 2087
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);
({struct Cyc_String_pa_PrintArg_struct _tmp3EB;_tmp3EB.tag=0;_tmp3EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp3E9[1]={& _tmp3EB};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3EA="can't take & of exp %s";_tag_dyneither(_tmp3EA,sizeof(char),23);}),_tag_dyneither(_tmp3E9,sizeof(void*),1));});});}_LL22C:;}
# 2092
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3F0=s->r;void*_tmp3F1=_tmp3F0;struct Cyc_Absyn_Exp**_tmp3F7;struct Cyc_Absyn_Stmt*_tmp3F6;struct Cyc_Absyn_Stmt*_tmp3F5;switch(*((int*)_tmp3F1)){case 2: _LL236: _tmp3F5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3F1)->f2;_LL237:
 _tmp3F6=_tmp3F5;goto _LL239;case 12: _LL238: _tmp3F6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F1)->f2;_LL239:
 Cyc_Toc_push_address_stmt(_tmp3F6);goto _LL235;case 1: _LL23A: _tmp3F7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3F1)->f1;_LL23B:
*_tmp3F7=Cyc_Toc_push_address_exp(*_tmp3F7);goto _LL235;default: _LL23C: _LL23D:
# 2098
({struct Cyc_String_pa_PrintArg_struct _tmp3F4;_tmp3F4.tag=0;_tmp3F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3F2[1]={& _tmp3F4};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3F3="can't take & of stmt %s";_tag_dyneither(_tmp3F3,sizeof(char),24);}),_tag_dyneither(_tmp3F2,sizeof(void*),1));});});}_LL235:;}
# 2103
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2105
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2107
if(x == 0)return 0;
result=({struct Cyc_List_List*_tmp3F8=_region_malloc(r2,sizeof(*_tmp3F8));_tmp3F8->hd=f(x->hd,env);_tmp3F8->tl=0;_tmp3F8;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmp3F9=_region_malloc(r2,sizeof(*_tmp3F9));_tmp3F9->hd=f(x->hd,env);_tmp3F9->tl=0;_tmp3F9;});
prev=prev->tl;}
# 2114
return result;}
# 2116
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2120
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA->f1=0;_tmp3FA->f2=e;_tmp3FA;});}
# 2124
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3FB=Cyc_Tcutil_compress(t);void*_tmp3FC=_tmp3FB;struct Cyc_Absyn_PtrInfo _tmp3FF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FC)->tag == 5){_LL23F: _tmp3FF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FC)->f1;_LL240:
 return _tmp3FF;}else{_LL241: _LL242:
({void*_tmp3FD=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3FE="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3FE,sizeof(char),28);}),_tag_dyneither(_tmp3FD,sizeof(void*),0));});}_LL23E:;}
# 2134
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp400=Cyc_Tcutil_compress(t);void*_tmp401=_tmp400;int _tmp40D;enum Cyc_Absyn_Sign _tmp40C;enum Cyc_Absyn_Sign _tmp40B;enum Cyc_Absyn_Sign _tmp40A;enum Cyc_Absyn_Sign _tmp409;enum Cyc_Absyn_Sign _tmp408;switch(*((int*)_tmp401)){case 5: _LL244: _LL245:
 res=Cyc_Absyn_null_exp(0);goto _LL243;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp401)->f2){case Cyc_Absyn_Char_sz: _LL246: _tmp408=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp401)->f1;_LL247:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp408,'\000'),0);goto _LL243;case Cyc_Absyn_Short_sz: _LL248: _tmp409=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp401)->f1;_LL249:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp409,0),0);goto _LL243;case Cyc_Absyn_Int_sz: _LL24E: _tmp40A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp401)->f1;_LL24F:
# 2142
 _tmp40B=_tmp40A;goto _LL251;case Cyc_Absyn_Long_sz: _LL250: _tmp40B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp401)->f1;_LL251:
# 2144
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp40B,0),0);goto _LL243;default: _LL252: _tmp40C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp401)->f1;_LL253:
# 2146
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp40C,(long long)0),0);goto _LL243;}case 13: _LL24A: _LL24B:
# 2140
 goto _LL24D;case 14: _LL24C: _LL24D:
 _tmp40A=Cyc_Absyn_Unsigned;goto _LL24F;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp401)->f1){case 0: _LL254: _LL255:
# 2147
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp402="0.0F";_tag_dyneither(_tmp402,sizeof(char),5);}),0),0);goto _LL243;case 1: _LL256: _LL257:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp403="0.0";_tag_dyneither(_tmp403,sizeof(char),4);}),1),0);goto _LL243;default: _LL258: _tmp40D=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp401)->f1;_LL259:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp404="0.0L";_tag_dyneither(_tmp404,sizeof(char),5);}),_tmp40D),0);goto _LL243;}default: _LL25A: _LL25B:
# 2151
({struct Cyc_String_pa_PrintArg_struct _tmp407;_tmp407.tag=0;_tmp407.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp405[1]={& _tmp407};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp406="found non-zero type %s in generate_zero";_tag_dyneither(_tmp406,sizeof(char),40);}),_tag_dyneither(_tmp405,sizeof(void*),1));});});}_LL243:;}
# 2153
res->topt=t;
return res;}
# 2160
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2173 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp40E=Cyc_Toc_typ_to_c(elt_type);
void*_tmp40F=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp410=Cyc_Absyn_cstar_typ(_tmp40E,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp411=({struct Cyc_Core_Opt*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->v=_tmp410;_tmp448;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp412=e1->r;void*_tmp413=_tmp412;struct Cyc_Absyn_Exp*_tmp419;struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Exp*_tmp417;switch(*((int*)_tmp413)){case 19: _LL25D: _tmp417=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_LL25E:
# 2181
 if(!is_dyneither){
_tmp417=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp417,0,Cyc_Absyn_Other_coercion,0);
_tmp417->topt=fat_ptr_type;}
# 2185
Cyc_Toc_exp_to_c(nv,_tmp417);xinit=_tmp417;goto _LL25C;case 22: _LL25F: _tmp419=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_tmp418=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp413)->f2;_LL260:
# 2187
 if(!is_dyneither){
_tmp419=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp419,0,Cyc_Absyn_Other_coercion,0);
_tmp419->topt=fat_ptr_type;}
# 2191
Cyc_Toc_exp_to_c(nv,_tmp419);Cyc_Toc_exp_to_c(nv,_tmp418);
xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp414[3];_tmp414[2]=_tmp418;_tmp414[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp414[0]=_tmp419;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp414,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
goto _LL25C;default: _LL261: _LL262:
({void*_tmp415=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp416="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp416,sizeof(char),53);}),_tag_dyneither(_tmp415,sizeof(void*),0));});}_LL25C:;}{
# 2197
struct _tuple1*_tmp41A=Cyc_Toc_temp_var();
struct _RegionHandle _tmp41B=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp41B;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp41C=({struct Cyc_Absyn_Vardecl*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->sc=Cyc_Absyn_Public;_tmp447->name=_tmp41A;_tmp447->tq=Cyc_Toc_mt_tq;_tmp447->type=_tmp40F;_tmp447->initializer=xinit;_tmp447->rgn=0;_tmp447->attributes=0;_tmp447->escapes=0;_tmp447;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp41D=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp446;_tmp446.tag=4;_tmp446.f1=_tmp41C;_tmp446;});_tmp445;});
struct Cyc_Absyn_Exp*_tmp41E=Cyc_Absyn_varb_exp((void*)_tmp41D,0);
_tmp41E->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_deref_exp(_tmp41E,0);
_tmp41F->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp41F);{
struct _tuple1*_tmp420=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp421=({struct Cyc_Absyn_Vardecl*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->sc=Cyc_Absyn_Public;_tmp444->name=_tmp420;_tmp444->tq=Cyc_Toc_mt_tq;_tmp444->type=_tmp40E;_tmp444->initializer=_tmp41F;_tmp444->rgn=0;_tmp444->attributes=0;_tmp444->escapes=0;_tmp444;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp422=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp443;_tmp443.tag=4;_tmp443.f1=_tmp421;_tmp443;});_tmp442;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp423=Cyc_Absyn_varb_exp((void*)_tmp422,0);
_tmp423->topt=_tmp41F->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp423,e2,0);
z_init->topt=_tmp423->topt;}
# 2216
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp424=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp425=({struct Cyc_Absyn_Vardecl*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->sc=Cyc_Absyn_Public;_tmp441->name=_tmp424;_tmp441->tq=Cyc_Toc_mt_tq;_tmp441->type=_tmp40E;_tmp441->initializer=z_init;_tmp441->rgn=0;_tmp441->attributes=0;_tmp441->escapes=0;_tmp441;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp426=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp440;_tmp440.tag=4;_tmp440.f1=_tmp425;_tmp440;});_tmp43F;});
# 2222
struct Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_varb_exp((void*)_tmp422,0);_tmp427->topt=_tmp41F->topt;{
struct Cyc_Absyn_Exp*_tmp428=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp427,_tmp428,0);
_tmp429->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp429);{
# 2228
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Absyn_varb_exp((void*)_tmp426,0);_tmp42A->topt=_tmp41F->topt;{
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp42A,_tmp42B,0);
_tmp42C->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp42C);{
# 2234
struct Cyc_List_List*_tmp42D=({struct Cyc_Absyn_Exp*_tmp43E[2];_tmp43E[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp43E[0]=
# 2234
Cyc_Absyn_varb_exp((void*)_tmp41D,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp43E,sizeof(struct Cyc_Absyn_Exp*),2));});
# 2236
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp42D,0),_tmp42E,0);{
# 2241
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp429,_tmp42C,0),0);
struct Cyc_Absyn_Stmt*_tmp430=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp431=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp((void*)_tmp41D,0),Cyc_Toc_curr_sp,0);
_tmp431=Cyc_Toc_cast_it(_tmp410,_tmp431);{
struct Cyc_Absyn_Exp*_tmp432=Cyc_Absyn_deref_exp(_tmp431,0);
struct Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_assign_exp(_tmp432,Cyc_Absyn_var_exp(_tmp424,0),0);
struct Cyc_Absyn_Stmt*_tmp434=Cyc_Absyn_exp_stmt(_tmp433,0);
_tmp434=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp42F,_tmp430,Cyc_Absyn_skip_stmt(0),0),_tmp434,0);
_tmp434=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp437;_tmp437.tag=0;_tmp437.f1=_tmp425;_tmp437;});_tmp436;});_tmp435->loc=0;_tmp435;}),_tmp434,0);
_tmp434=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp43A;_tmp43A.tag=0;_tmp43A.f1=_tmp421;_tmp43A;});_tmp439;});_tmp438->loc=0;_tmp438;}),_tmp434,0);
_tmp434=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp43D;_tmp43D.tag=0;_tmp43D.f1=_tmp41C;_tmp43D;});_tmp43C;});_tmp43B->loc=0;_tmp43B;}),_tmp434,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp434);};};};};};};};};};}
# 2199
;_pop_region(rgn2);};}
# 2267 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2271
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp449=Cyc_Tcutil_compress(aggrtype);void*_tmp44A=_tmp449;union Cyc_Absyn_AggrInfoU _tmp44F;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44A)->tag == 11){_LL264: _tmp44F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44A)->f1).aggr_info;_LL265:
# 2274
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp44F);goto _LL263;}else{_LL266: _LL267:
({void*_tmp44B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp44E;_tmp44E.tag=0;_tmp44E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));({void*_tmp44C[1]={& _tmp44E};Cyc_aprintf(({const char*_tmp44D="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp44D,sizeof(char),51);}),_tag_dyneither(_tmp44C,sizeof(void*),1));});}),_tag_dyneither(_tmp44B,sizeof(void*),0));});}_LL263:;}{
# 2278
struct _tuple1*_tmp450=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_var_exp(_tmp450,0);
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp453=Cyc_Absyn_aggrarrow_exp(_tmp451,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp454=Cyc_Absyn_neq_exp(_tmp453,_tmp452,0);
struct Cyc_Absyn_Exp*_tmp455=Cyc_Absyn_aggrarrow_exp(_tmp451,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp456=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp455,0),0);
struct Cyc_Absyn_Stmt*_tmp457=Cyc_Absyn_ifthenelse_stmt(_tmp454,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp458=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp459=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp45A=Cyc_Absyn_declare_stmt(_tmp450,_tmp458,_tmp459,Cyc_Absyn_seq_stmt(_tmp457,_tmp456,0),0);
return Cyc_Toc_stmt_exp_r(_tmp45A);}else{
# 2292
struct Cyc_Absyn_Exp*_tmp45B=Cyc_Toc_member_exp(aggrproj(_tmp451,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp45C=Cyc_Absyn_neq_exp(_tmp45B,_tmp452,0);
struct Cyc_Absyn_Exp*_tmp45D=Cyc_Toc_member_exp(aggrproj(_tmp451,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp45E=Cyc_Absyn_exp_stmt(_tmp45D,0);
struct Cyc_Absyn_Stmt*_tmp45F=Cyc_Absyn_ifthenelse_stmt(_tmp45C,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp460=Cyc_Absyn_declare_stmt(_tmp450,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp45F,_tmp45E,0),0);
return Cyc_Toc_stmt_exp_r(_tmp460);}};}
# 2302
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2305
void*_tmp461=Cyc_Tcutil_compress(t);void*_tmp462=_tmp461;union Cyc_Absyn_AggrInfoU _tmp469;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp462)->tag == 11){_LL269: _tmp469=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp462)->f1).aggr_info;_LL26A: {
# 2307
struct Cyc_Absyn_Aggrdecl*_tmp463=Cyc_Absyn_get_known_aggrdecl(_tmp469);
*f_tag=Cyc_Toc_get_member_offset(_tmp463,f);{
# 2310
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp468;_tmp468.tag=0;_tmp468.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp467;_tmp467.tag=0;_tmp467.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp463->name).f2);({void*_tmp465[2]={& _tmp467,& _tmp468};Cyc_aprintf(({const char*_tmp466="_union_%s_%s";_tag_dyneither(_tmp466,sizeof(char),13);}),_tag_dyneither(_tmp465,sizeof(void*),2));});});});
*tagged_member_type=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464[0]=str;_tmp464;}));
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp463->impl))->tagged;};}}else{_LL26B: _LL26C:
 return 0;}_LL268:;}
# 2321
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2324
void*_tmp46A=e->r;void*_tmp46B=_tmp46A;struct Cyc_Absyn_Exp*_tmp477;struct _dyneither_ptr*_tmp476;int*_tmp475;struct Cyc_Absyn_Exp*_tmp474;struct _dyneither_ptr*_tmp473;int*_tmp472;struct Cyc_Absyn_Exp*_tmp471;switch(*((int*)_tmp46B)){case 13: _LL26E: _tmp471=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL26F:
({void*_tmp46C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp46D="cast on lhs!";_tag_dyneither(_tmp46D,sizeof(char),13);}),_tag_dyneither(_tmp46C,sizeof(void*),0));});case 20: _LL270: _tmp474=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp473=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp472=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46B)->f4;_LL271:
# 2327
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp474->topt),_tmp473,f_tag,tagged_member_type,clear_read,_tmp472);case 21: _LL272: _tmp477=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp476=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp475=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46B)->f4;_LL273: {
# 2330
void*_tmp46E=Cyc_Tcutil_compress((void*)_check_null(_tmp477->topt));void*_tmp46F=_tmp46E;void*_tmp470;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->tag == 5){_LL277: _tmp470=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->f1).elt_typ;_LL278:
# 2332
 return Cyc_Toc_is_tagged_union_project_impl(_tmp470,_tmp476,f_tag,tagged_member_type,clear_read,_tmp475);}else{_LL279: _LL27A:
# 2334
 return 0;}_LL276:;}default: _LL274: _LL275:
# 2336
 return 0;}_LL26D:;}
# 2349 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2353
struct _tuple1*_tmp478=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp478,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2363
struct Cyc_Absyn_Exp*_tmp479=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp479,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2366
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp478,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2372
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2387 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp47A=Cyc_Absyn_var_exp(max,0);
# 2395
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp47A=Cyc_Absyn_add_exp(_tmp47A,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2406
s=Cyc_Absyn_skip_stmt(0);}}{
# 2408
struct _RegionHandle _tmp47B=_new_region("r");struct _RegionHandle*r=& _tmp47B;_push_region(r);
{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp48F=_region_malloc(r,sizeof(*_tmp48F));_tmp48F->hd=({struct _tuple28*_tmp490=_region_malloc(r,sizeof(*_tmp490));_tmp490->f1=max;_tmp490->f2=Cyc_Absyn_uint_typ;_tmp490->f3=e1;_tmp490;});_tmp48F->tl=0;_tmp48F;});
# 2411
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp47C[2];_tmp47C[1]=_tmp47A;_tmp47C[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp47C,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{
# 2418
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp47D[2];_tmp47D[1]=_tmp47A;_tmp47D[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp47D,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{
# 2424
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
decls=({struct Cyc_List_List*_tmp47E=_region_malloc(r,sizeof(*_tmp47E));_tmp47E->hd=({struct _tuple28*_tmp47F=_region_malloc(r,sizeof(*_tmp47F));_tmp47F->f1=a;_tmp47F->f2=ptr_typ;_tmp47F->f3=ainit;_tmp47F;});_tmp47E->tl=decls;_tmp47E;});
if(!Cyc_Tcutil_is_pointer_type(old_typ))
({void*_tmp480=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp481="new_comprehension_to_c: comprehension is not an array type";_tag_dyneither(_tmp481,sizeof(char),59);}),_tag_dyneither(_tmp480,sizeof(void*),0));});
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp482=Cyc_Toc_temp_var();
void*_tmp483=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp484=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp485=
Cyc_Absyn_fncall_exp(_tmp484,({struct Cyc_Absyn_Exp*_tmp488[3];_tmp488[2]=_tmp47A;_tmp488[1]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp488[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp488,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 2436
decls=({struct Cyc_List_List*_tmp486=_region_malloc(r,sizeof(*_tmp486));_tmp486->hd=({struct _tuple28*_tmp487=_region_malloc(r,sizeof(*_tmp487));_tmp487->f1=_tmp482;_tmp487->f2=_tmp483;_tmp487->f3=_tmp485;_tmp487;});_tmp486->tl=decls;_tmp486;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp482,0),0),0);}else{
# 2439
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp489=decls;for(0;_tmp489 != 0;_tmp489=_tmp489->tl){
struct _tuple28 _tmp48A=*((struct _tuple28*)_tmp489->hd);struct _tuple28 _tmp48B=_tmp48A;struct _tuple1*_tmp48E;void*_tmp48D;struct Cyc_Absyn_Exp*_tmp48C;_LL27C: _tmp48E=_tmp48B.f1;_tmp48D=_tmp48B.f2;_tmp48C=_tmp48B.f3;_LL27D:;
s=Cyc_Absyn_declare_stmt(_tmp48E,_tmp48D,_tmp48C,s,0);}}
# 2444
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2409
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2448
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp491=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp494;_tmp494.tag=0;_tmp494.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp492[1]={& _tmp494};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp493="exp_to_c: no type for %s";_tag_dyneither(_tmp493,sizeof(char),25);}),_tag_dyneither(_tmp492,sizeof(void*),1));});});
# 2454
if((nv->struct_info).lhs_exp != 0){
void*_tmp495=_tmp491;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp495)->tag == 28){_LL27F: _LL280:
 goto _LL27E;}else{_LL281: _LL282:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}_LL27E:;}{
# 2462
void*old_typ=(void*)_check_null(e->topt);
void*_tmp496=_tmp491;struct Cyc_Absyn_Stmt*_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B0;struct _dyneither_ptr*_tmp6AF;struct Cyc_Absyn_Exp*_tmp6AE;struct Cyc_Absyn_Exp*_tmp6AD;int _tmp6AC;struct Cyc_Absyn_Exp*_tmp6AB;void**_tmp6AA;struct Cyc_Absyn_Exp*_tmp6A9;int _tmp6A8;int _tmp6A7;struct Cyc_List_List*_tmp6A6;struct Cyc_Absyn_Datatypedecl*_tmp6A5;struct Cyc_Absyn_Datatypefield*_tmp6A4;void*_tmp6A3;struct Cyc_List_List*_tmp6A2;struct _tuple1*_tmp6A1;struct Cyc_List_List*_tmp6A0;struct Cyc_List_List*_tmp69F;struct Cyc_Absyn_Aggrdecl*_tmp69E;struct Cyc_Absyn_Exp*_tmp69D;void*_tmp69C;int _tmp69B;struct Cyc_Absyn_Vardecl*_tmp69A;struct Cyc_Absyn_Exp*_tmp699;struct Cyc_Absyn_Exp*_tmp698;int _tmp697;struct Cyc_List_List*_tmp696;struct Cyc_List_List*_tmp695;struct Cyc_Absyn_Exp*_tmp694;struct Cyc_Absyn_Exp*_tmp693;struct Cyc_Absyn_Exp*_tmp692;struct _dyneither_ptr*_tmp691;int _tmp690;int _tmp68F;struct Cyc_Absyn_Exp*_tmp68E;struct _dyneither_ptr*_tmp68D;int _tmp68C;int _tmp68B;struct Cyc_Absyn_Exp*_tmp68A;void*_tmp689;struct Cyc_List_List*_tmp688;void*_tmp687;struct Cyc_Absyn_Exp*_tmp686;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_Absyn_Exp*_tmp684;struct Cyc_Absyn_Exp*_tmp683;void**_tmp682;struct Cyc_Absyn_Exp*_tmp681;int _tmp680;enum Cyc_Absyn_Coercion _tmp67F;struct Cyc_Absyn_Exp*_tmp67E;struct Cyc_List_List*_tmp67D;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67B;int _tmp67A;struct Cyc_Absyn_Exp*_tmp679;struct Cyc_List_List*_tmp678;int _tmp677;struct Cyc_List_List*_tmp676;struct Cyc_Absyn_VarargInfo*_tmp675;struct Cyc_Absyn_Exp*_tmp674;struct Cyc_List_List*_tmp673;struct Cyc_Absyn_Exp*_tmp672;struct Cyc_Absyn_Exp*_tmp671;struct Cyc_Absyn_Exp*_tmp670;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_Absyn_Exp*_tmp66D;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp*_tmp66B;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_Absyn_Exp*_tmp669;struct Cyc_Core_Opt*_tmp668;struct Cyc_Absyn_Exp*_tmp667;struct Cyc_Absyn_Exp*_tmp666;enum Cyc_Absyn_Incrementor _tmp665;enum Cyc_Absyn_Primop _tmp664;struct Cyc_List_List*_tmp663;switch(*((int*)_tmp496)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp496)->f1).Null_c).tag == 1){_LL284: _LL285: {
# 2469
struct Cyc_Absyn_Exp*_tmp497=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp497,_tmp497))->r;else{
# 2474
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp498[3];_tmp498[2]=_tmp497;_tmp498[1]=_tmp497;_tmp498[0]=_tmp497;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp498,sizeof(struct Cyc_Absyn_Exp*),3));}));}}else{
# 2476
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2478
goto _LL283;}}else{_LL286: _LL287:
 goto _LL283;}case 1: _LL288: _LL289:
 goto _LL283;case 2: _LL28A: _tmp664=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp663=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL28B: {
# 2483
struct Cyc_List_List*_tmp499=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp663);
# 2485
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp663);
{enum Cyc_Absyn_Primop _tmp49A=_tmp664;switch(_tmp49A){case Cyc_Absyn_Numelts: _LL2D9: _LL2DA: {
# 2488
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp663))->hd;
{void*_tmp49B=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp49C=_tmp49B;void*_tmp4AE;union Cyc_Absyn_Constraint*_tmp4AD;union Cyc_Absyn_Constraint*_tmp4AC;union Cyc_Absyn_Constraint*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;switch(*((int*)_tmp49C)){case 8: _LL2EE: _tmp4AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp49C)->f1).num_elts;_LL2EF:
# 2492
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp4AA)))
({void*_tmp49D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp49E="can't calculate numelts";_tag_dyneither(_tmp49E,sizeof(char),24);}),_tag_dyneither(_tmp49D,sizeof(void*),0));});
e->r=_tmp4AA->r;goto _LL2ED;case 5: _LL2F0: _tmp4AE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49C)->f1).elt_typ;_tmp4AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49C)->f1).ptr_atts).nullable;_tmp4AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49C)->f1).ptr_atts).bounds;_tmp4AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49C)->f1).ptr_atts).zero_term;_LL2F1:
# 2496
{void*_tmp49F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4AC);void*_tmp4A0=_tmp49F;struct Cyc_Absyn_Exp*_tmp4A5;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4A0)->tag == 0){_LL2F5: _LL2F6:
# 2498
 e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*_tmp4A1[2];_tmp4A1[1]=
# 2500
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4AE),0);_tmp4A1[0]=(struct Cyc_Absyn_Exp*)_tmp663->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A1,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL2F4;}else{_LL2F7: _tmp4A5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4A0)->f1;_LL2F8:
# 2504
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4AB)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp663->hd);
# 2508
e->r=Cyc_Toc_fncall_exp_r(function_e,({struct Cyc_Absyn_Exp*_tmp4A2[2];_tmp4A2[1]=_tmp4A5;_tmp4A2[0]=(struct Cyc_Absyn_Exp*)_tmp663->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A2,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4AD)){
if(!Cyc_Evexp_c_can_eval(_tmp4A5))
({void*_tmp4A3=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4A4="can't calculate numelts";_tag_dyneither(_tmp4A4,sizeof(char),24);}),_tag_dyneither(_tmp4A3,sizeof(void*),0));});
# 2513
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp4A5,Cyc_Absyn_uint_exp(0,0));}else{
# 2515
e->r=_tmp4A5->r;goto _LL2F4;}}
# 2517
goto _LL2F4;}_LL2F4:;}
# 2519
goto _LL2ED;default: _LL2F2: _LL2F3:
# 2521
({struct Cyc_String_pa_PrintArg_struct _tmp4A9;_tmp4A9.tag=0;_tmp4A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp4A8;_tmp4A8.tag=0;_tmp4A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({void*_tmp4A6[2]={& _tmp4A8,& _tmp4A9};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4A7="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp4A7,sizeof(char),41);}),_tag_dyneither(_tmp4A6,sizeof(void*),2));});});});}_LL2ED:;}
# 2524
goto _LL2D8;}case Cyc_Absyn_Plus: _LL2DB: _LL2DC:
# 2529
{void*_tmp4AF=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp499))->hd);void*_tmp4B0=_tmp4AF;void*_tmp4B8;union Cyc_Absyn_Constraint*_tmp4B7;union Cyc_Absyn_Constraint*_tmp4B6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B0)->tag == 5){_LL2FA: _tmp4B8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B0)->f1).elt_typ;_tmp4B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B0)->f1).ptr_atts).bounds;_tmp4B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B0)->f1).ptr_atts).zero_term;_LL2FB:
# 2531
{void*_tmp4B1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4B7);void*_tmp4B2=_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B5;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4B2)->tag == 0){_LL2FF: _LL300: {
# 2533
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp663))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp663->tl))->hd;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4B3[3];_tmp4B3[2]=e2;_tmp4B3[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4B8),0);_tmp4B3[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B3,sizeof(struct Cyc_Absyn_Exp*),3));}));
goto _LL2FE;}}else{_LL301: _tmp4B5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4B2)->f1;_LL302:
# 2539
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4B6)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp663))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp663->tl))->hd;
e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*_tmp4B4[3];_tmp4B4[2]=e2;_tmp4B4[1]=_tmp4B5;_tmp4B4[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B4,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r;}
# 2544
goto _LL2FE;}_LL2FE:;}
# 2546
goto _LL2F9;}else{_LL2FC: _LL2FD:
# 2548
 goto _LL2F9;}_LL2F9:;}
# 2550
goto _LL2D8;case Cyc_Absyn_Minus: _LL2DD: _LL2DE: {
# 2555
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp499))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp663))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp663->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp499->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2565
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4B9[3];_tmp4B9[2]=
# 2567
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0);_tmp4B9[1]=
# 2566
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4B9[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B9,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 2570
goto _LL2D8;}case Cyc_Absyn_Eq: _LL2DF: _LL2E0:
 goto _LL2E2;case Cyc_Absyn_Neq: _LL2E1: _LL2E2:
 goto _LL2E4;case Cyc_Absyn_Gt: _LL2E3: _LL2E4:
 goto _LL2E6;case Cyc_Absyn_Gte: _LL2E5: _LL2E6:
 goto _LL2E8;case Cyc_Absyn_Lt: _LL2E7: _LL2E8:
 goto _LL2EA;case Cyc_Absyn_Lte: _LL2E9: _LL2EA: {
# 2578
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp663))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp663->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp499))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp499->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
goto _LL2D8;}default: _LL2EB: _LL2EC:
 goto _LL2D8;}_LL2D8:;}
# 2590
goto _LL283;}case 4: _LL28C: _tmp666=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp665=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL28D: {
# 2592
void*e2_cyc_typ=(void*)_check_null(_tmp666->topt);
# 2601 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp4D0="increment";_tag_dyneither(_tmp4D0,sizeof(char),10);});
if(_tmp665 == Cyc_Absyn_PreDec  || _tmp665 == Cyc_Absyn_PostDec)incr_str=({const char*_tmp4BA="decrement";_tag_dyneither(_tmp4BA,sizeof(char),10);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp666,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp4BD;_tmp4BD.tag=0;_tmp4BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4BB[1]={& _tmp4BD};Cyc_Tcutil_terr(e->loc,({const char*_tmp4BC="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp4BC,sizeof(char),74);}),_tag_dyneither(_tmp4BB,sizeof(void*),1));});});
({void*_tmp4BE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4BF="in-place inc/dec on zero-term";_tag_dyneither(_tmp4BF,sizeof(char),30);}),_tag_dyneither(_tmp4BE,sizeof(void*),0));});}{
# 2610
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp666,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4C0=Cyc_Absyn_signed_int_exp(1,0);
_tmp4C0->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4C1=_tmp665;switch(_tmp4C1){case Cyc_Absyn_PreInc: _LL304: _LL305:
# 2617
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));_tmp4C2[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4C3;_tmp4C3.tag=3;_tmp4C3.f1=_tmp666;_tmp4C3.f2=({struct Cyc_Core_Opt*_tmp4C4=_cycalloc_atomic(sizeof(*_tmp4C4));_tmp4C4->v=(void*)Cyc_Absyn_Plus;_tmp4C4;});_tmp4C3.f3=_tmp4C0;_tmp4C3;});_tmp4C2;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL306: _LL307:
# 2621
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4C6;_tmp4C6.tag=3;_tmp4C6.f1=_tmp666;_tmp4C6.f2=({struct Cyc_Core_Opt*_tmp4C7=_cycalloc_atomic(sizeof(*_tmp4C7));_tmp4C7->v=(void*)Cyc_Absyn_Minus;_tmp4C7;});_tmp4C6.f3=_tmp4C0;_tmp4C6;});_tmp4C5;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL308: _LL309:
# 2625
({struct Cyc_String_pa_PrintArg_struct _tmp4CA;_tmp4CA.tag=0;_tmp4CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4C8[1]={& _tmp4CA};Cyc_Tcutil_terr(e->loc,({const char*_tmp4C9="in-place post-%s is not supported on @tagged union members";_tag_dyneither(_tmp4C9,sizeof(char),59);}),_tag_dyneither(_tmp4C8,sizeof(void*),1));});});
# 2627
({void*_tmp4CB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4CC="in-place inc/dec on @tagged union";_tag_dyneither(_tmp4CC,sizeof(char),34);}),_tag_dyneither(_tmp4CB,sizeof(void*),0));});}_LL303:;};}
# 2630
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp666);
Cyc_Toc_set_lhs(nv,0);{
# 2635
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp665 == Cyc_Absyn_PostInc  || _tmp665 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2641
if(_tmp665 == Cyc_Absyn_PreDec  || _tmp665 == Cyc_Absyn_PostDec)
change=-1;
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4CD[3];_tmp4CD[2]=
# 2645
Cyc_Absyn_signed_int_exp(change,0);_tmp4CD[1]=
# 2644
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4CD[0]=
# 2643
Cyc_Toc_push_address_exp(_tmp666);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CD,sizeof(struct Cyc_Absyn_Exp*),3));}));}else{
# 2646
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2650
struct Cyc_Toc_functionSet*_tmp4CE=_tmp665 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2652
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp4CE,_tmp666);
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4CF[2];_tmp4CF[1]=
Cyc_Absyn_signed_int_exp(1,0);_tmp4CF[0]=
# 2653
Cyc_Toc_push_address_exp(_tmp666);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CF,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
# 2655
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp666)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp666,0,Cyc_Toc_incr_lvalue,_tmp665);
e->r=_tmp666->r;}}}
# 2659
goto _LL283;};};}case 3: _LL28E: _tmp669=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp668=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_tmp667=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp496)->f3;_LL28F: {
# 2678 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp669->topt);
void*e2_old_typ=(void*)_check_null(_tmp667->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp669,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp669);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp667);
e->r=Cyc_Toc_tagged_union_assignop(_tmp669,e1_old_typ,_tmp668,_tmp667,e2_old_typ,f_tag,tagged_member_struct_type);
# 2689
return;}{
# 2691
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp669,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp669,_tmp668,_tmp667,ptr_type,is_dyneither,elt_type);
# 2697
return;}{
# 2701
int e1_poly=Cyc_Toc_is_poly_project(_tmp669);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp669);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp667);{
# 2707
int done=0;
if(_tmp668 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp4D1=(enum Cyc_Absyn_Primop)_tmp668->v;enum Cyc_Absyn_Primop _tmp4D2=_tmp4D1;switch(_tmp4D2){case Cyc_Absyn_Plus: _LL30B: _LL30C:
 change=_tmp667;goto _LL30A;case Cyc_Absyn_Minus: _LL30D: _LL30E:
# 2715
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp667,0);goto _LL30A;default: _LL30F: _LL310:
({void*_tmp4D3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4D4="bad t ? pointer arithmetic";_tag_dyneither(_tmp4D4,sizeof(char),27);}),_tag_dyneither(_tmp4D3,sizeof(void*),0));});}_LL30A:;}
# 2718
done=1;{
# 2720
struct Cyc_Absyn_Exp*_tmp4D5=Cyc_Toc__dyneither_ptr_inplace_plus_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp4D5,({struct Cyc_Absyn_Exp*_tmp4D6[3];_tmp4D6[2]=change;_tmp4D6[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4D6[0]=
# 2721
Cyc_Toc_push_address_exp(_tmp669);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D6,sizeof(struct Cyc_Absyn_Exp*),3));}));};}else{
# 2724
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2727
enum Cyc_Absyn_Primop _tmp4D7=(enum Cyc_Absyn_Primop)_tmp668->v;enum Cyc_Absyn_Primop _tmp4D8=_tmp4D7;if(_tmp4D8 == Cyc_Absyn_Plus){_LL312: _LL313:
# 2729
 done=1;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp669),({struct Cyc_Absyn_Exp*_tmp4D9[2];_tmp4D9[1]=_tmp667;_tmp4D9[0]=_tmp669;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D9,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL311;}else{_LL314: _LL315:
({void*_tmp4DA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4DB="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp4DB,sizeof(char),39);}),_tag_dyneither(_tmp4DA,sizeof(void*),0));});}_LL311:;}}}
# 2736
if(!done){
# 2738
if(e1_poly)
_tmp667->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp667->r,0));
# 2744
if(!Cyc_Absyn_is_lvalue(_tmp669)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp669,0,Cyc_Toc_assignop_lvalue,({struct _tuple27*_tmp4DC=_cycalloc(sizeof(struct _tuple27)* 1);_tmp4DC[0]=(struct _tuple27)({struct _tuple27 _tmp4DD;_tmp4DD.f1=_tmp668;_tmp4DD.f2=_tmp667;_tmp4DD;});_tmp4DC;}));
e->r=_tmp669->r;}}
# 2749
goto _LL283;};};};}case 5: _LL290: _tmp66C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp66B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_tmp66A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp496)->f3;_LL291:
# 2751
 Cyc_Toc_exp_to_c(nv,_tmp66C);
Cyc_Toc_exp_to_c(nv,_tmp66B);
Cyc_Toc_exp_to_c(nv,_tmp66A);
goto _LL283;case 6: _LL292: _tmp66E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp66D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL293:
# 2756
 Cyc_Toc_exp_to_c(nv,_tmp66E);
Cyc_Toc_exp_to_c(nv,_tmp66D);
goto _LL283;case 7: _LL294: _tmp670=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp66F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL295:
# 2760
 Cyc_Toc_exp_to_c(nv,_tmp670);
Cyc_Toc_exp_to_c(nv,_tmp66F);
goto _LL283;case 8: _LL296: _tmp672=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp671=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL297:
# 2764
 Cyc_Toc_exp_to_c(nv,_tmp672);
Cyc_Toc_exp_to_c(nv,_tmp671);
goto _LL283;case 9: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496)->f3 == 0){_LL298: _tmp674=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp673=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL299:
# 2768
 Cyc_Toc_exp_to_c(nv,_tmp674);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp673);
goto _LL283;}else{_LL29A: _tmp679=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp678=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_tmp677=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496)->f3)->num_varargs;_tmp676=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496)->f3)->injectors;_tmp675=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496)->f3)->vai;_LL29B: {
# 2780 "toc.cyc"
struct _RegionHandle _tmp4DE=_new_region("r");struct _RegionHandle*r=& _tmp4DE;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp677,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp675->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2788
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp678);
int num_normargs=num_args - _tmp677;
# 2792
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp678=_tmp678->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp678))->hd);
new_args=({struct Cyc_List_List*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->hd=(struct Cyc_Absyn_Exp*)_tmp678->hd;_tmp4DF->tl=new_args;_tmp4DF;});}}
# 2797
new_args=({struct Cyc_List_List*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp4E1[3];_tmp4E1[2]=num_varargs_exp;_tmp4E1[1]=
# 2799
Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp4E1[0]=argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E1,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp4E0->tl=new_args;_tmp4E0;});
# 2802
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2804
Cyc_Toc_exp_to_c(nv,_tmp679);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp679,new_args,0),0);
# 2808
if(_tmp675->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp4E2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp675->type));void*_tmp4E3=_tmp4E2;struct Cyc_Absyn_Datatypedecl*_tmp4E6;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E3)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E3)->f1).datatype_info).KnownDatatype).tag == 2){_LL317: _tmp4E6=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E3)->f1).datatype_info).KnownDatatype).val;_LL318:
 tud=_tmp4E6;goto _LL316;}else{goto _LL319;}}else{_LL319: _LL31A:
({void*_tmp4E4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4E5="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp4E5,sizeof(char),44);}),_tag_dyneither(_tmp4E4,sizeof(void*),0));});}_LL316:;}{
# 2814
struct _dyneither_ptr vs=({unsigned int _tmp4EF=(unsigned int)_tmp677;struct _tuple1**_tmp4F0=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmp4EF));struct _dyneither_ptr _tmp4F2=_tag_dyneither(_tmp4F0,sizeof(struct _tuple1*),_tmp4EF);{unsigned int _tmp4F1=_tmp4EF;unsigned int i;for(i=0;i < _tmp4F1;i ++){_tmp4F0[i]=(struct _tuple1*)Cyc_Toc_temp_var();}}_tmp4F2;});
# 2816
if(_tmp677 != 0){
# 2818
struct Cyc_List_List*_tmp4E7=0;
{int i=_tmp677 - 1;for(0;i >= 0;-- i){
_tmp4E7=({struct Cyc_List_List*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp4E8->tl=_tmp4E7;_tmp4E8;});}}
# 2822
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp4E7,0),s,0);{
# 2825
int i=0;for(0;_tmp678 != 0;(((_tmp678=_tmp678->tl,_tmp676=_tmp676->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp678->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp4E9=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp676))->hd;struct Cyc_Absyn_Datatypefield*_tmp4EA=_tmp4E9;struct _tuple1*_tmp4EC;struct Cyc_List_List*_tmp4EB;_LL31C: _tmp4EC=_tmp4EA->name;_tmp4EB=_tmp4EA->typs;_LL31D:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp4EB))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2840
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2843
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp4EC),0),s,0);
# 2846
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4EC,tud->name)),0,s,0);};}};}else{
# 2853
struct Cyc_List_List*_tmp4ED=({struct _tuple19*_tmp4EE[3];_tmp4EE[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4EE[1]=
# 2853
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4EE[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4EE,sizeof(struct _tuple19*),3));});
# 2855
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2865
{int i=0;for(0;_tmp678 != 0;(_tmp678=_tmp678->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp678->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp678->hd,0),s,0);}}
# 2871
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2884 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2886
_npop_handler(0);goto _LL283;
# 2780 "toc.cyc"
;_pop_region(r);}}case 10: _LL29C: _tmp67B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp67A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL29D:
# 2889 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp67B);{
struct Cyc_Absyn_Exp*_tmp4F3=_tmp67A?Cyc_Toc_newrethrow_exp(_tmp67B): Cyc_Toc_newthrow_exp(_tmp67B);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp4F3,0))->r;
goto _LL283;};case 11: _LL29E: _tmp67C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_LL29F:
 Cyc_Toc_exp_to_c(nv,_tmp67C);goto _LL283;case 12: _LL2A0: _tmp67E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp67D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL2A1:
# 2895
 Cyc_Toc_exp_to_c(nv,_tmp67E);
# 2904 "toc.cyc"
for(0;_tmp67D != 0;_tmp67D=_tmp67D->tl){
enum Cyc_Absyn_KindQual _tmp4F4=(Cyc_Tcutil_typ_kind((void*)_tmp67D->hd))->kind;
if(_tmp4F4 != Cyc_Absyn_EffKind  && _tmp4F4 != Cyc_Absyn_RgnKind){
{void*_tmp4F5=Cyc_Tcutil_compress((void*)_tmp67D->hd);void*_tmp4F6=_tmp4F5;switch(*((int*)_tmp4F6)){case 2: _LL31F: _LL320:
 goto _LL322;case 3: _LL321: _LL322:
 continue;default: _LL323: _LL324:
# 2911
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp67E,0))->r;
goto _LL31E;}_LL31E:;}
# 2914
break;}}
# 2917
goto _LL283;case 13: _LL2A2: _tmp682=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp681=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_tmp680=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp496)->f3;_tmp67F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp496)->f4;_LL2A3: {
# 2919
void*old_t2=(void*)_check_null(_tmp681->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp682;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp682=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp681);
# 2926
{struct _tuple29 _tmp4F7=({struct _tuple29 _tmp532;_tmp532.f1=Cyc_Tcutil_compress(old_t2);_tmp532.f2=Cyc_Tcutil_compress(new_typ);_tmp532;});struct _tuple29 _tmp4F8=_tmp4F7;struct Cyc_Absyn_PtrInfo _tmp531;struct Cyc_Absyn_PtrInfo _tmp530;struct Cyc_Absyn_PtrInfo _tmp52F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F8.f1)->tag == 5)switch(*((int*)_tmp4F8.f2)){case 5: _LL326: _tmp530=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F8.f1)->f1;_tmp52F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F8.f2)->f1;_LL327: {
# 2928
int _tmp4F9=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp530.ptr_atts).nullable);
int _tmp4FA=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52F.ptr_atts).nullable);
void*_tmp4FB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp530.ptr_atts).bounds);
void*_tmp4FC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52F.ptr_atts).bounds);
int _tmp4FD=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp530.ptr_atts).zero_term);
int _tmp4FE=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52F.ptr_atts).zero_term);
{struct _tuple29 _tmp4FF=({struct _tuple29 _tmp52C;_tmp52C.f1=_tmp4FB;_tmp52C.f2=_tmp4FC;_tmp52C;});struct _tuple29 _tmp500=_tmp4FF;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp528;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp500.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp500.f2)->tag == 1){_LL32D: _tmp529=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp500.f1)->f1;_tmp528=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp500.f2)->f1;_LL32E:
# 2936
 if((!Cyc_Evexp_c_can_eval(_tmp529) || !Cyc_Evexp_c_can_eval(_tmp528)) && !
Cyc_Evexp_same_const_exp(_tmp529,_tmp528))
({void*_tmp501=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp502="can't validate cast due to potential size differences";_tag_dyneither(_tmp502,sizeof(char),54);}),_tag_dyneither(_tmp501,sizeof(void*),0));});
if(_tmp4F9  && !_tmp4FA){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp503=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp504="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp504,sizeof(char),44);}),_tag_dyneither(_tmp503,sizeof(void*),0));});
# 2945
if(_tmp67F != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp507;_tmp507.tag=0;_tmp507.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp505[1]={& _tmp507};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp506="null-check conversion mis-classified: %s";_tag_dyneither(_tmp506,sizeof(char),41);}),_tag_dyneither(_tmp505,sizeof(void*),1));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp681);
if(do_null_check){
if(!_tmp680)
({void*_tmp508=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp509="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp509,sizeof(char),58);}),_tag_dyneither(_tmp508,sizeof(void*),0));});
# 2952
e->r=Cyc_Toc_cast_it_r(*_tmp682,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A->hd=_tmp681;_tmp50A->tl=0;_tmp50A;}),0));}else{
# 2956
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp681->r;}};}else{
# 2961
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp681->r;}
# 2968
goto _LL32C;}else{_LL32F: _tmp52A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp500.f1)->f1;_LL330:
# 2970
 if(!Cyc_Evexp_c_can_eval(_tmp52A))
({void*_tmp50B=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp50C="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp50C,sizeof(char),71);}),_tag_dyneither(_tmp50B,sizeof(void*),0));});
# 2973
if(_tmp67F == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp50F;_tmp50F.tag=0;_tmp50F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp50D[1]={& _tmp50F};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp50E="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp50E,sizeof(char),44);}),_tag_dyneither(_tmp50D,sizeof(void*),1));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2977
if((_tmp4FD  && !(_tmp52F.elt_tq).real_const) && !_tmp4FE)
# 2980
_tmp52A=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp52A,Cyc_Absyn_uint_exp(1,0),0);
# 2982
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp52A,_tmp681))->r;}else{
# 2984
struct Cyc_Absyn_Exp*_tmp510=Cyc_Toc__tag_dyneither_e;
# 2986
if(_tmp4FD){
# 2991
struct _tuple1*_tmp511=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp512=Cyc_Absyn_var_exp(_tmp511,0);
struct Cyc_Absyn_Exp*arg3;
# 2996
{void*_tmp513=_tmp681->r;void*_tmp514=_tmp513;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp514)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp514)->f1).Wstring_c).tag){case 8: _LL336: _LL337:
# 2998
 arg3=_tmp52A;goto _LL335;case 9: _LL338: _LL339:
# 3000
 arg3=_tmp52A;goto _LL335;default: goto _LL33A;}else{_LL33A: _LL33B:
# 3002
 arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp681),({struct Cyc_Absyn_Exp*_tmp515[2];_tmp515[1]=_tmp52A;_tmp515[0]=
# 3004
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp512);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp515,sizeof(struct Cyc_Absyn_Exp*),2));}),0);
goto _LL335;}_LL335:;}
# 3007
if(!_tmp4FE  && !(_tmp52F.elt_tq).real_const)
# 3010
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3012
struct Cyc_Absyn_Exp*_tmp516=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52F.elt_typ),0);
struct Cyc_Absyn_Exp*_tmp517=Cyc_Absyn_fncall_exp(_tmp510,({struct Cyc_Absyn_Exp*_tmp519[3];_tmp519[2]=arg3;_tmp519[1]=_tmp516;_tmp519[0]=_tmp512;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp519,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
struct Cyc_Absyn_Stmt*_tmp518=Cyc_Absyn_exp_stmt(_tmp517,0);
_tmp518=Cyc_Absyn_declare_stmt(_tmp511,Cyc_Toc_typ_to_c(old_t2),_tmp681,_tmp518,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp518);};}else{
# 3019
e->r=Cyc_Toc_fncall_exp_r(_tmp510,({struct Cyc_Absyn_Exp*_tmp51A[3];_tmp51A[2]=_tmp52A;_tmp51A[1]=
# 3021
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52F.elt_typ),0);_tmp51A[0]=_tmp681;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp51A,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 3025
goto _LL32C;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp500.f2)->tag == 1){_LL331: _tmp52B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp500.f2)->f1;_LL332:
# 3027
 if(!Cyc_Evexp_c_can_eval(_tmp52B))
({void*_tmp51B=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp51C="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp51C,sizeof(char),71);}),_tag_dyneither(_tmp51B,sizeof(void*),0));});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp51D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp51E="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp51E,sizeof(char),45);}),_tag_dyneither(_tmp51D,sizeof(void*),0));});{
# 3039 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp51F=_tmp52B;
if(_tmp4FD  && !_tmp4FE)
_tmp51F=Cyc_Absyn_add_exp(_tmp52B,Cyc_Absyn_uint_exp(1,0),0);{
# 3046
struct Cyc_Absyn_Exp*_tmp520=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp521=Cyc_Absyn_fncall_exp(_tmp520,({struct Cyc_Absyn_Exp*_tmp523[3];_tmp523[2]=_tmp51F;_tmp523[1]=
# 3049
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp530.elt_typ),0);_tmp523[0]=_tmp681;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp523,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 3051
if(!_tmp4FA)
_tmp521->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522->hd=
Cyc_Absyn_copy_exp(_tmp521);_tmp522->tl=0;_tmp522;}));
e->r=Cyc_Toc_cast_it_r(*_tmp682,_tmp521);
goto _LL32C;};};}else{_LL333: _LL334:
# 3059
 DynCast:
 if((_tmp4FD  && !_tmp4FE) && !(_tmp52F.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp524=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp525="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp525,sizeof(char),70);}),_tag_dyneither(_tmp524,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp526=Cyc_Toc__dyneither_ptr_decrease_size_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp526,({struct Cyc_Absyn_Exp*_tmp527[3];_tmp527[2]=
# 3066
Cyc_Absyn_uint_exp(1,0);_tmp527[1]=
# 3065
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp530.elt_typ),0);_tmp527[0]=_tmp681;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp527,sizeof(struct Cyc_Absyn_Exp*),3));}));};}
# 3068
goto _LL32C;}}_LL32C:;}
# 3070
goto _LL325;}case 6: _LL328: _tmp531=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F8.f1)->f1;_LL329:
# 3072
{void*_tmp52D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp531.ptr_atts).bounds);void*_tmp52E=_tmp52D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp52E)->tag == 0){_LL33D: _LL33E:
# 3074
 _tmp681->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp681->r,_tmp681->loc),Cyc_Toc_curr_sp);goto _LL33C;}else{_LL33F: _LL340:
 goto _LL33C;}_LL33C:;}
# 3077
goto _LL325;default: goto _LL32A;}else{_LL32A: _LL32B:
# 3079
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp681->r;
goto _LL325;}_LL325:;}
# 3083
goto _LL283;}case 14: _LL2A4: _tmp683=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_LL2A5:
# 3087
{void*_tmp533=_tmp683->r;void*_tmp534=_tmp533;struct Cyc_List_List*_tmp53F;struct _tuple1*_tmp53E;struct Cyc_List_List*_tmp53D;struct Cyc_List_List*_tmp53C;switch(*((int*)_tmp534)){case 28: _LL342: _tmp53E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp534)->f1;_tmp53D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp534)->f2;_tmp53C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp534)->f3;_LL343:
# 3089
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp537;_tmp537.tag=0;_tmp537.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp683->loc));({void*_tmp535[1]={& _tmp537};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp536="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp536,sizeof(char),42);}),_tag_dyneither(_tmp535,sizeof(void*),1));});});{
struct Cyc_Absyn_Exp*_tmp538=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp683->topt),_tmp53D,1,0,_tmp53C,_tmp53E);
e->r=_tmp538->r;
e->topt=_tmp538->topt;
goto _LL341;};case 23: _LL344: _tmp53F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp534)->f1;_LL345:
# 3097
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp53B;_tmp53B.tag=0;_tmp53B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp683->loc));({void*_tmp539[1]={& _tmp53B};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp53A="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp53A,sizeof(char),42);}),_tag_dyneither(_tmp539,sizeof(void*),1));});});
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp53F))->r;
goto _LL341;default: _LL346: _LL347:
# 3103
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp683);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp683)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp683,0,Cyc_Toc_address_lvalue,1);
# 3109
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp683);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp683->topt))))
# 3113
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp683);}
# 3115
goto _LL341;}_LL341:;}
# 3117
goto _LL283;case 15: _LL2A6: _tmp685=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp684=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL2A7:
# 3120
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0;_tmp542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp684->loc));({void*_tmp540[1]={& _tmp542};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp541="%s: new at top-level";_tag_dyneither(_tmp541,sizeof(char),21);}),_tag_dyneither(_tmp540,sizeof(void*),1));});});{
void*new_e_type=(void*)_check_null(_tmp684->topt);
{void*_tmp543=_tmp684->r;void*_tmp544=_tmp543;struct Cyc_List_List*_tmp573;struct _tuple1*_tmp572;struct Cyc_List_List*_tmp571;struct Cyc_List_List*_tmp570;struct Cyc_Absyn_Aggrdecl*_tmp56F;struct Cyc_Absyn_Exp*_tmp56E;void*_tmp56D;int _tmp56C;struct Cyc_Absyn_Vardecl*_tmp56B;struct Cyc_Absyn_Exp*_tmp56A;struct Cyc_Absyn_Exp*_tmp569;int _tmp568;struct Cyc_List_List*_tmp567;switch(*((int*)_tmp544)){case 25: _LL349: _tmp567=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL34A: {
# 3128
struct _tuple1*_tmp545=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp546=Cyc_Absyn_var_exp(_tmp545,0);
struct Cyc_Absyn_Stmt*_tmp547=Cyc_Toc_init_array(nv,new_e_type,_tmp546,_tmp567,Cyc_Absyn_exp_stmt(_tmp546,0));
void*old_elt_typ;
{void*_tmp548=Cyc_Tcutil_compress(old_typ);void*_tmp549=_tmp548;void*_tmp54E;struct Cyc_Absyn_Tqual _tmp54D;union Cyc_Absyn_Constraint*_tmp54C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp549)->tag == 5){_LL356: _tmp54E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp549)->f1).elt_typ;_tmp54D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp549)->f1).elt_tq;_tmp54C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp549)->f1).ptr_atts).zero_term;_LL357:
# 3134
 old_elt_typ=_tmp54E;goto _LL355;}else{_LL358: _LL359:
# 3136
 old_elt_typ=({void*_tmp54A=0;((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp54B="exp_to_c:new array expression doesn't have ptr type";_tag_dyneither(_tmp54B,sizeof(char),52);}),_tag_dyneither(_tmp54A,sizeof(void*),0));});}_LL355:;}{
# 3139
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp54F=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp550=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp567),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp685 == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp550);else{
# 3147
struct Cyc_Absyn_Exp*r=_tmp685;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp550);}
# 3152
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp545,_tmp54F,e1,_tmp547,0));
goto _LL348;};}case 26: _LL34B: _tmp56B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_tmp56A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_tmp569=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp544)->f3;_tmp568=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp544)->f4;_LL34C:
# 3156
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp685,old_typ,_tmp56A,(void*)_check_null(_tmp569->topt),_tmp568,_tmp569,_tmp56B);
goto _LL348;case 27: _LL34D: _tmp56E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_tmp56D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_tmp56C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp544)->f3;_LL34E:
# 3160
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp685,old_typ,_tmp56E,_tmp56D,_tmp56C,0,0);
goto _LL348;case 28: _LL34F: _tmp572=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_tmp571=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_tmp570=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp544)->f3;_tmp56F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp544)->f4;_LL350: {
# 3165
struct Cyc_Absyn_Exp*_tmp551=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp684->topt),_tmp571,1,_tmp685,_tmp570,_tmp572);
e->r=_tmp551->r;
e->topt=_tmp551->topt;
goto _LL348;}case 23: _LL351: _tmp573=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL352:
# 3171
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp685,_tmp573))->r;
goto _LL348;default: _LL353: _LL354: {
# 3178
void*old_elt_typ=(void*)_check_null(_tmp684->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3181
struct _tuple1*_tmp552=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp553=Cyc_Absyn_var_exp(_tmp552,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp553,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp685 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3188
struct Cyc_Absyn_Exp*r=_tmp685;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3196
int done=0;
{void*_tmp554=_tmp684->r;void*_tmp555=_tmp554;void*_tmp563;struct Cyc_Absyn_Exp*_tmp562;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp555)->tag == 13){_LL35B: _tmp563=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_tmp562=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp555)->f2;_LL35C:
# 3199
{struct _tuple29 _tmp556=({struct _tuple29 _tmp561;_tmp561.f1=Cyc_Tcutil_compress(_tmp563);_tmp561.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp562->topt));_tmp561;});struct _tuple29 _tmp557=_tmp556;void*_tmp560;union Cyc_Absyn_Constraint*_tmp55F;union Cyc_Absyn_Constraint*_tmp55E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp557.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp557.f2)->tag == 5){_LL360: _tmp560=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp557.f1)->f1).elt_typ;_tmp55F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp557.f1)->f1).ptr_atts).bounds;_tmp55E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp557.f2)->f1).ptr_atts).bounds;_LL361:
# 3202
{struct _tuple29 _tmp558=({struct _tuple29 _tmp55D;_tmp55D.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp55F);_tmp55D.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp55E);_tmp55D;});struct _tuple29 _tmp559=_tmp558;struct Cyc_Absyn_Exp*_tmp55C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp559.f1)->tag == 0){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp559.f2)->tag == 1){_LL365: _tmp55C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp559.f2)->f1;_LL366:
# 3204
 Cyc_Toc_exp_to_c(nv,_tmp562);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp55A=Cyc_Toc__init_dyneither_ptr_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp55A,({struct Cyc_Absyn_Exp*_tmp55B[4];_tmp55B[3]=_tmp55C;_tmp55B[2]=
# 3210
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp560),0);_tmp55B[1]=_tmp562;_tmp55B[0]=mexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp55B,sizeof(struct Cyc_Absyn_Exp*),4));}));
# 3212
goto _LL364;};}else{goto _LL367;}}else{_LL367: _LL368:
 goto _LL364;}_LL364:;}
# 3215
goto _LL35F;}else{goto _LL362;}}else{_LL362: _LL363:
 goto _LL35F;}_LL35F:;}
# 3218
goto _LL35A;}else{_LL35D: _LL35E:
 goto _LL35A;}_LL35A:;}
# 3221
if(!done){
struct Cyc_Absyn_Stmt*_tmp564=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp553),0);
struct Cyc_Absyn_Exp*_tmp565=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp684);
_tmp564=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp553,_tmp565,0),_tmp684,0),_tmp564,0);{
# 3227
void*_tmp566=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp552,_tmp566,mexp,_tmp564,0));};}
# 3230
goto _LL348;};}}_LL348:;}
# 3232
goto _LL283;};case 17: _LL2A8: _tmp686=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_LL2A9: {
# 3235
int _tmp574=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp686);
Cyc_Toc_set_in_sizeof(nv,_tmp574);
goto _LL283;}case 16: _LL2AA: _tmp687=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_LL2AB:
 e->r=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp576;_tmp576.tag=16;_tmp576.f1=Cyc_Toc_typ_to_c(_tmp687);_tmp576;});_tmp575;});goto _LL283;case 18: _LL2AC: _tmp689=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp688=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL2AD: {
# 3241
void*_tmp577=_tmp689;
struct Cyc_List_List*_tmp578=_tmp688;
for(0;_tmp578 != 0;_tmp578=_tmp578->tl){
void*_tmp579=(void*)_tmp578->hd;void*_tmp57A=_tmp579;unsigned int _tmp590;struct _dyneither_ptr*_tmp58F;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp57A)->tag == 0){_LL36A: _tmp58F=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp57A)->f1;_LL36B:
 goto _LL369;}else{_LL36C: _tmp590=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp57A)->f1;_LL36D:
# 3247
{void*_tmp57B=Cyc_Tcutil_compress(_tmp577);void*_tmp57C=_tmp57B;struct Cyc_Absyn_Datatypefield*_tmp58E;struct Cyc_List_List*_tmp58D;struct Cyc_List_List*_tmp58C;union Cyc_Absyn_AggrInfoU _tmp58B;switch(*((int*)_tmp57C)){case 11: _LL36F: _tmp58B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp57C)->f1).aggr_info;_LL370: {
# 3249
struct Cyc_Absyn_Aggrdecl*_tmp57D=Cyc_Absyn_get_known_aggrdecl(_tmp58B);
if(_tmp57D->impl == 0)
({void*_tmp57E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp57F="struct fields must be known";_tag_dyneither(_tmp57F,sizeof(char),28);}),_tag_dyneither(_tmp57E,sizeof(void*),0));});
_tmp58C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57D->impl))->fields;goto _LL372;}case 12: _LL371: _tmp58C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57C)->f2;_LL372: {
# 3254
struct Cyc_Absyn_Aggrfield*_tmp580=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp58C,(int)_tmp590);
_tmp578->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp582;_tmp582.tag=0;_tmp582.f1=_tmp580->name;_tmp582;});_tmp581;}));
_tmp577=_tmp580->type;
goto _LL36E;}case 10: _LL373: _tmp58D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp57C)->f1;_LL374:
# 3259
 _tmp578->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp584;_tmp584.tag=0;_tmp584.f1=Cyc_Absyn_fieldname((int)(_tmp590 + 1));_tmp584;});_tmp583;}));
_tmp577=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp58D,(int)_tmp590)).f2;
goto _LL36E;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57C)->f1).field_info).KnownDatatypefield).tag == 2){_LL375: _tmp58E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57C)->f1).field_info).KnownDatatypefield).val).f2;_LL376:
# 3263
 if(_tmp590 == 0)
_tmp578->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp586;_tmp586.tag=0;_tmp586.f1=Cyc_Toc_tag_sp;_tmp586;});_tmp585;}));else{
# 3266
_tmp577=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp58E->typs,(int)(_tmp590 - 1))).f2;
_tmp578->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp588;_tmp588.tag=0;_tmp588.f1=Cyc_Absyn_fieldname((int)_tmp590);_tmp588;});_tmp587;}));}
# 3269
goto _LL36E;}else{goto _LL377;}default: _LL377: _LL378:
({void*_tmp589=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp58A="impossible type for offsetof tuple index";_tag_dyneither(_tmp58A,sizeof(char),41);}),_tag_dyneither(_tmp589,sizeof(void*),0));});
goto _LL36E;}_LL36E:;}
# 3273
goto _LL369;}_LL369:;}
# 3276
e->r=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp592;_tmp592.tag=18;_tmp592.f1=Cyc_Toc_typ_to_c(_tmp689);_tmp592.f2=_tmp688;_tmp592;});_tmp591;});
goto _LL283;}case 19: _LL2AE: _tmp68A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_LL2AF: {
# 3279
int _tmp593=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp594=Cyc_Tcutil_compress((void*)_check_null(_tmp68A->topt));
{void*_tmp595=_tmp594;void*_tmp5AA;struct Cyc_Absyn_Tqual _tmp5A9;void*_tmp5A8;union Cyc_Absyn_Constraint*_tmp5A7;union Cyc_Absyn_Constraint*_tmp5A6;union Cyc_Absyn_Constraint*_tmp5A5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->tag == 5){_LL37A: _tmp5AA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).elt_typ;_tmp5A9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).elt_tq;_tmp5A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).ptr_atts).rgn;_tmp5A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).ptr_atts).nullable;_tmp5A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).ptr_atts).bounds;_tmp5A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).ptr_atts).zero_term;_LL37B:
# 3284
{void*_tmp596=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5A6);void*_tmp597=_tmp596;struct Cyc_Absyn_Exp*_tmp5A2;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp597)->tag == 1){_LL37F: _tmp5A2=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp597)->f1;_LL380: {
# 3286
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp68A);
Cyc_Toc_exp_to_c(nv,_tmp68A);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp598=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp599="inserted null check due to dereference";_tag_dyneither(_tmp599,sizeof(char),39);}),_tag_dyneither(_tmp598,sizeof(void*),0));});
# 3292
_tmp68A->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp594),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->hd=
Cyc_Absyn_copy_exp(_tmp68A);_tmp59A->tl=0;_tmp59A;}),0));}
# 3300
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A5)){
struct _tuple12 _tmp59B=Cyc_Evexp_eval_const_uint_exp(_tmp5A2);struct _tuple12 _tmp59C=_tmp59B;unsigned int _tmp5A0;int _tmp59F;_LL384: _tmp5A0=_tmp59C.f1;_tmp59F=_tmp59C.f2;_LL385:;
# 3306
if(!_tmp59F  || _tmp5A0 <= 0)
({void*_tmp59D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp59E="cannot determine dereference is in bounds";_tag_dyneither(_tmp59E,sizeof(char),42);}),_tag_dyneither(_tmp59D,sizeof(void*),0));});}
# 3309
goto _LL37E;}}else{_LL381: _LL382: {
# 3312
struct Cyc_Absyn_Exp*_tmp5A1=Cyc_Absyn_uint_exp(0,0);
_tmp5A1->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp68A,_tmp5A1);
Cyc_Toc_exp_to_c(nv,e);
goto _LL37E;}}_LL37E:;}
# 3318
goto _LL379;}else{_LL37C: _LL37D:
({void*_tmp5A3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5A4="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp5A4,sizeof(char),29);}),_tag_dyneither(_tmp5A3,sizeof(void*),0));});}_LL379:;}
# 3321
Cyc_Toc_set_lhs(nv,_tmp593);
goto _LL283;};}case 20: _LL2B0: _tmp68E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp68D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_tmp68C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp496)->f3;_tmp68B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp496)->f4;_LL2B1: {
# 3324
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp68E->topt);
Cyc_Toc_exp_to_c(nv,_tmp68E);
if(_tmp68C  && _tmp68B)
e->r=Cyc_Toc_check_tagged_union(_tmp68E,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp68D,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3332
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL283;}case 21: _LL2B2: _tmp692=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp691=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_tmp690=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp496)->f3;_tmp68F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp496)->f4;_LL2B3: {
# 3336
int _tmp5AB=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp692->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp692);
Cyc_Toc_exp_to_c(nv,_tmp692);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp5AC=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp5AD=_tmp5AC;void*_tmp5C3;struct Cyc_Absyn_Tqual _tmp5C2;void*_tmp5C1;union Cyc_Absyn_Constraint*_tmp5C0;union Cyc_Absyn_Constraint*_tmp5BF;union Cyc_Absyn_Constraint*_tmp5BE;_LL387: _tmp5C3=_tmp5AD.elt_typ;_tmp5C2=_tmp5AD.elt_tq;_tmp5C1=(_tmp5AD.ptr_atts).rgn;_tmp5C0=(_tmp5AD.ptr_atts).nullable;_tmp5BF=(_tmp5AD.ptr_atts).bounds;_tmp5BE=(_tmp5AD.ptr_atts).zero_term;_LL388:;
{void*_tmp5AE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5BF);void*_tmp5AF=_tmp5AE;struct Cyc_Absyn_Exp*_tmp5BD;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AF)->tag == 1){_LL38A: _tmp5BD=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AF)->f1;_LL38B: {
# 3347
struct _tuple12 _tmp5B0=Cyc_Evexp_eval_const_uint_exp(_tmp5BD);struct _tuple12 _tmp5B1=_tmp5B0;unsigned int _tmp5BB;int _tmp5BA;_LL38F: _tmp5BB=_tmp5B1.f1;_tmp5BA=_tmp5B1.f2;_LL390:;
if(_tmp5BA){
if(_tmp5BB < 1)
({void*_tmp5B2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5B3="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp5B3,sizeof(char),44);}),_tag_dyneither(_tmp5B2,sizeof(void*),0));});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5B4=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5B5="inserted null check due to dereference";_tag_dyneither(_tmp5B5,sizeof(char),39);}),_tag_dyneither(_tmp5B4,sizeof(void*),0));});
# 3355
_tmp692->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5B6[1];_tmp5B6[0]=
Cyc_Absyn_new_exp(_tmp692->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5B6,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}}else{
# 3360
if(!Cyc_Evexp_c_can_eval(_tmp5BD))
({void*_tmp5B7=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5B8="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp5B8,sizeof(char),47);}),_tag_dyneither(_tmp5B7,sizeof(void*),0));});
# 3364
_tmp692->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5B9[4];_tmp5B9[3]=
# 3368
Cyc_Absyn_uint_exp(0,0);_tmp5B9[2]=
# 3367
Cyc_Absyn_sizeoftyp_exp(_tmp5C3,0);_tmp5B9[1]=_tmp5BD;_tmp5B9[0]=
# 3366
Cyc_Absyn_new_exp(_tmp692->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5B9,sizeof(struct Cyc_Absyn_Exp*),4));}),0));}
# 3370
goto _LL389;}}else{_LL38C: _LL38D: {
# 3373
void*ta1=Cyc_Toc_typ_to_c(_tmp5C3);
_tmp692->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp5C2),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,({struct Cyc_Absyn_Exp*_tmp5BC[3];_tmp5BC[2]=
# 3378
Cyc_Absyn_uint_exp(0,0);_tmp5BC[1]=
# 3377
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5BC[0]=
# 3376
Cyc_Absyn_new_exp(_tmp692->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5BC,sizeof(struct Cyc_Absyn_Exp*),3));}),0));
# 3381
goto _LL389;}}_LL389:;}
# 3383
if(_tmp690  && _tmp68F)
e->r=Cyc_Toc_check_tagged_union(_tmp692,Cyc_Toc_typ_to_c(e1typ),_tmp5C3,_tmp691,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3386
if(is_poly  && _tmp68F)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp5AB);
goto _LL283;};};}case 22: _LL2B4: _tmp694=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp693=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL2B5: {
# 3391
int _tmp5C4=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5C5=Cyc_Tcutil_compress((void*)_check_null(_tmp694->topt));
# 3395
{void*_tmp5C6=_tmp5C5;void*_tmp5F0;struct Cyc_Absyn_Tqual _tmp5EF;void*_tmp5EE;union Cyc_Absyn_Constraint*_tmp5ED;union Cyc_Absyn_Constraint*_tmp5EC;union Cyc_Absyn_Constraint*_tmp5EB;struct Cyc_List_List*_tmp5EA;switch(*((int*)_tmp5C6)){case 10: _LL392: _tmp5EA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5C6)->f1;_LL393:
# 3398
 Cyc_Toc_exp_to_c(nv,_tmp694);
Cyc_Toc_exp_to_c(nv,_tmp693);{
struct _tuple12 _tmp5C7=Cyc_Evexp_eval_const_uint_exp(_tmp693);struct _tuple12 _tmp5C8=_tmp5C7;unsigned int _tmp5CC;int _tmp5CB;_LL399: _tmp5CC=_tmp5C8.f1;_tmp5CB=_tmp5C8.f2;_LL39A:;
if(!_tmp5CB)
({void*_tmp5C9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5CA="unknown tuple subscript in translation to C";_tag_dyneither(_tmp5CA,sizeof(char),44);}),_tag_dyneither(_tmp5C9,sizeof(void*),0));});
e->r=Cyc_Toc_aggrmember_exp_r(_tmp694,Cyc_Absyn_fieldname((int)(_tmp5CC + 1)));
goto _LL391;};case 5: _LL394: _tmp5F0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).elt_typ;_tmp5EF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).elt_tq;_tmp5EE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).rgn;_tmp5ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).nullable;_tmp5EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).bounds;_tmp5EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).zero_term;_LL395: {
# 3406
struct Cyc_List_List*_tmp5CD=Cyc_Toc_get_relns(_tmp694);
int _tmp5CE=Cyc_Toc_need_null_check(_tmp694);
int _tmp5CF=Cyc_Toc_in_sizeof(nv);
# 3414
int in_bnds=_tmp5CF;
{void*_tmp5D0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5EC);void*_tmp5D1=_tmp5D0;_LL39C: _LL39D:
# 3417
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp5C5,_tmp5CD,_tmp694,_tmp693);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp5D4;_tmp5D4.tag=0;_tmp5D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp5D2[1]={& _tmp5D4};Cyc_Tcutil_warn(e->loc,({const char*_tmp5D3="bounds check necessary for %s";_tag_dyneither(_tmp5D3,sizeof(char),30);}),_tag_dyneither(_tmp5D2,sizeof(void*),1));});});_LL39B:;}
# 3424
Cyc_Toc_exp_to_c(nv,_tmp694);
Cyc_Toc_exp_to_c(nv,_tmp693);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp5D5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5EC);void*_tmp5D6=_tmp5D5;struct Cyc_Absyn_Exp*_tmp5E7;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5D6)->tag == 1){_LL39F: _tmp5E7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5D6)->f1;_LL3A0: {
# 3429
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5ED) && _tmp5CE;
void*ta1=Cyc_Toc_typ_to_c(_tmp5F0);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5EF);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5D7=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5D8="inserted null check due to dereference";_tag_dyneither(_tmp5D8,sizeof(char),39);}),_tag_dyneither(_tmp5D7,sizeof(void*),0));});
_tmp694->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5D9[1];_tmp5D9[0]=
Cyc_Absyn_copy_exp(_tmp694);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D9,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}else{
# 3441
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5EB)){
# 3443
if(!Cyc_Evexp_c_can_eval(_tmp5E7))
({void*_tmp5DA=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5DB="cannot determine subscript is in bounds";_tag_dyneither(_tmp5DB,sizeof(char),40);}),_tag_dyneither(_tmp5DA,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp694);
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,({struct Cyc_Absyn_Exp*_tmp5DC[3];_tmp5DC[2]=_tmp693;_tmp5DC[1]=_tmp5E7;_tmp5DC[0]=_tmp694;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5DC,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));};}else{
# 3449
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp5E7))
({void*_tmp5DD=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5DE="cannot determine subscript is in bounds";_tag_dyneither(_tmp5DE,sizeof(char),40);}),_tag_dyneither(_tmp5DD,sizeof(void*),0));});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5DF=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5E0="inserted null check due to dereference";_tag_dyneither(_tmp5E0,sizeof(char),39);}),_tag_dyneither(_tmp5DF,sizeof(void*),0));});
# 3455
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5E1[4];_tmp5E1[3]=_tmp693;_tmp5E1[2]=
# 3458
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5E1[1]=_tmp5E7;_tmp5E1[0]=_tmp694;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E1,sizeof(struct Cyc_Absyn_Exp*),4));}),0)));}else{
# 3461
if(!Cyc_Evexp_c_can_eval(_tmp5E7))
({void*_tmp5E2=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5E3="cannot determine subscript is in bounds";_tag_dyneither(_tmp5E3,sizeof(char),40);}),_tag_dyneither(_tmp5E2,sizeof(void*),0));});
# 3464
_tmp693->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({struct Cyc_Absyn_Exp*_tmp5E4[2];_tmp5E4[1]=
Cyc_Absyn_copy_exp(_tmp693);_tmp5E4[0]=_tmp5E7;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E4,sizeof(struct Cyc_Absyn_Exp*),2));}));}}}}
# 3467
goto _LL39E;}}else{_LL3A1: _LL3A2: {
# 3469
void*ta1=Cyc_Toc_typ_to_c(_tmp5F0);
if(in_bnds){
# 3473
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5EF),
Cyc_Toc_member_exp(_tmp694,Cyc_Toc_curr_sp,0)),_tmp693);}else{
# 3478
struct Cyc_Absyn_Exp*_tmp5E5=Cyc_Toc__check_dyneither_subscript_e;
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5EF),
Cyc_Absyn_fncall_exp(_tmp5E5,({struct Cyc_Absyn_Exp*_tmp5E6[3];_tmp5E6[2]=_tmp693;_tmp5E6[1]=
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5E6[0]=_tmp694;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E6,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}
# 3484
goto _LL39E;}}_LL39E:;}
# 3486
goto _LL391;}default: _LL396: _LL397:
({void*_tmp5E8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5E9="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp5E9,sizeof(char),49);}),_tag_dyneither(_tmp5E8,sizeof(void*),0));});}_LL391:;}
# 3489
Cyc_Toc_set_lhs(nv,_tmp5C4);
goto _LL283;};}case 23: _LL2B6: _tmp695=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_LL2B7:
# 3492
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp695))->r;else{
# 3497
struct Cyc_List_List*_tmp5F1=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp695);
void*_tmp5F2=Cyc_Toc_add_tuple_type(_tmp5F1);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp695 != 0;(_tmp695=_tmp695->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp695->hd);
dles=({struct Cyc_List_List*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->hd=({struct _tuple19*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->f1=0;_tmp5F4->f2=(struct Cyc_Absyn_Exp*)_tmp695->hd;_tmp5F4;});_tmp5F3->tl=dles;_tmp5F3;});}}
# 3504
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3507
goto _LL283;case 25: _LL2B8: _tmp696=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_LL2B9:
# 3511
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp696);
{struct Cyc_List_List*_tmp5F5=_tmp696;for(0;_tmp5F5 != 0;_tmp5F5=_tmp5F5->tl){
struct _tuple19*_tmp5F6=(struct _tuple19*)_tmp5F5->hd;struct _tuple19*_tmp5F7=_tmp5F6;struct Cyc_Absyn_Exp*_tmp5F8;_LL3A4: _tmp5F8=_tmp5F7->f2;_LL3A5:;
Cyc_Toc_exp_to_c(nv,_tmp5F8);}}
# 3516
goto _LL283;case 26: _LL2BA: _tmp69A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp699=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_tmp698=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp496)->f3;_tmp697=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp496)->f4;_LL2BB: {
# 3520
struct _tuple12 _tmp5F9=Cyc_Evexp_eval_const_uint_exp(_tmp699);struct _tuple12 _tmp5FA=_tmp5F9;unsigned int _tmp604;int _tmp603;_LL3A7: _tmp604=_tmp5FA.f1;_tmp603=_tmp5FA.f2;_LL3A8:;{
void*_tmp5FB=Cyc_Toc_typ_to_c((void*)_check_null(_tmp698->topt));
Cyc_Toc_exp_to_c(nv,_tmp698);{
struct Cyc_List_List*es=0;
# 3525
if(!Cyc_Toc_is_zero(_tmp698)){
if(!_tmp603)
({void*_tmp5FC=0;Cyc_Tcutil_terr(_tmp699->loc,({const char*_tmp5FD="cannot determine value of constant";_tag_dyneither(_tmp5FD,sizeof(char),35);}),_tag_dyneither(_tmp5FC,sizeof(void*),0));});
{unsigned int i=0;for(0;i < _tmp604;++ i){
es=({struct Cyc_List_List*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->hd=({struct _tuple19*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->f1=0;_tmp5FF->f2=_tmp698;_tmp5FF;});_tmp5FE->tl=es;_tmp5FE;});}}
# 3531
if(_tmp697){
struct Cyc_Absyn_Exp*_tmp600=Cyc_Toc_cast_it(_tmp5FB,Cyc_Absyn_uint_exp(0,0));
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->hd=({struct _tuple19*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->f1=0;_tmp602->f2=_tmp600;_tmp602;});_tmp601->tl=0;_tmp601;}));}}
# 3536
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL283;};};}case 27: _LL2BC: _tmp69D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp69C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_tmp69B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp496)->f3;_LL2BD:
# 3541
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL283;case 28: _LL2BE: _tmp6A1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp6A0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_tmp69F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp496)->f3;_tmp69E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp496)->f4;_LL2BF:
# 3546
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp605=Cyc_Toc_init_struct(nv,old_typ,_tmp6A0,0,0,_tmp69F,_tmp6A1);
e->r=_tmp605->r;
e->topt=_tmp605->topt;}else{
# 3557
if(_tmp69E == 0)
({void*_tmp606=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp607="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp607,sizeof(char),38);}),_tag_dyneither(_tmp606,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp69E;
# 3561
struct _RegionHandle _tmp608=_new_region("rgn");struct _RegionHandle*rgn=& _tmp608;_push_region(rgn);
{struct Cyc_List_List*_tmp609=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp69F,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3565
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69E->impl))->tagged){
# 3567
struct _tuple30*_tmp60A=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp609))->hd;struct _tuple30*_tmp60B=_tmp60A;struct Cyc_Absyn_Aggrfield*_tmp618;struct Cyc_Absyn_Exp*_tmp617;_LL3AA: _tmp618=_tmp60B->f1;_tmp617=_tmp60B->f2;_LL3AB:;{
void*_tmp60C=(void*)_check_null(_tmp617->topt);
void*_tmp60D=_tmp618->type;
Cyc_Toc_exp_to_c(nv,_tmp617);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp60D) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp60C))
_tmp617->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp617->r,0));{
# 3576
int i=Cyc_Toc_get_member_offset(_tmp69E,_tmp618->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_List_List*_tmp60E=({struct _tuple19*_tmp614[2];_tmp614[1]=({struct _tuple19*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->f1=0;_tmp616->f2=_tmp617;_tmp616;});_tmp614[0]=({struct _tuple19*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->f1=0;_tmp615->f2=field_tag_exp;_tmp615;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp614,sizeof(struct _tuple19*),2));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp60E,0);
struct Cyc_List_List*ds=({void*_tmp611[1];_tmp611[0]=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp613;_tmp613.tag=1;_tmp613.f1=_tmp618->name;_tmp613;});_tmp612;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp611,sizeof(void*),1));});
struct Cyc_List_List*dles=({struct _tuple19*_tmp60F[1];_tmp60F[0]=({struct _tuple19*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->f1=ds;_tmp610->f2=umem;_tmp610;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60F,sizeof(struct _tuple19*),1));});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3585
struct Cyc_List_List*_tmp619=0;
struct Cyc_List_List*_tmp61A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp61A != 0;_tmp61A=_tmp61A->tl){
struct Cyc_List_List*_tmp61B=_tmp609;for(0;_tmp61B != 0;_tmp61B=_tmp61B->tl){
if((*((struct _tuple30*)_tmp61B->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp61A->hd){
struct _tuple30*_tmp61C=(struct _tuple30*)_tmp61B->hd;struct _tuple30*_tmp61D=_tmp61C;struct Cyc_Absyn_Aggrfield*_tmp623;struct Cyc_Absyn_Exp*_tmp622;_LL3AD: _tmp623=_tmp61D->f1;_tmp622=_tmp61D->f2;_LL3AE:;{
void*_tmp61E=Cyc_Toc_typ_to_c(_tmp623->type);
void*_tmp61F=Cyc_Toc_typ_to_c((void*)_check_null(_tmp622->topt));
Cyc_Toc_exp_to_c(nv,_tmp622);
# 3595
if(!Cyc_Tcutil_unify(_tmp61E,_tmp61F)){
# 3597
if(!Cyc_Tcutil_is_arithmetic_type(_tmp61E) || !
Cyc_Tcutil_is_arithmetic_type(_tmp61F))
_tmp622=Cyc_Toc_cast_it(_tmp61E,Cyc_Absyn_copy_exp(_tmp622));}
_tmp619=({struct Cyc_List_List*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->hd=({struct _tuple19*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->f1=0;_tmp621->f2=_tmp622;_tmp621;});_tmp620->tl=_tmp619;_tmp620;});
break;};}}}
# 3604
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp619));}}
# 3562
;_pop_region(rgn);};}
# 3608
goto _LL283;case 29: _LL2C0: _tmp6A3=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp6A2=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL2C1: {
# 3610
struct Cyc_List_List*fs;
{void*_tmp624=Cyc_Tcutil_compress(_tmp6A3);void*_tmp625=_tmp624;struct Cyc_List_List*_tmp629;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp625)->tag == 12){_LL3B0: _tmp629=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp625)->f2;_LL3B1:
 fs=_tmp629;goto _LL3AF;}else{_LL3B2: _LL3B3:
({struct Cyc_String_pa_PrintArg_struct _tmp628;_tmp628.tag=0;_tmp628.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp6A3));({void*_tmp626[1]={& _tmp628};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp627="anon struct has type %s";_tag_dyneither(_tmp627,sizeof(char),24);}),_tag_dyneither(_tmp626,sizeof(void*),1));});});}_LL3AF:;}{
# 3615
struct _RegionHandle _tmp62A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp62A;_push_region(rgn);{
struct Cyc_List_List*_tmp62B=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp6A2,Cyc_Absyn_StructA,fs);
for(0;_tmp62B != 0;_tmp62B=_tmp62B->tl){
struct _tuple30*_tmp62C=(struct _tuple30*)_tmp62B->hd;struct _tuple30*_tmp62D=_tmp62C;struct Cyc_Absyn_Aggrfield*_tmp631;struct Cyc_Absyn_Exp*_tmp630;_LL3B5: _tmp631=_tmp62D->f1;_tmp630=_tmp62D->f2;_LL3B6:;{
void*_tmp62E=(void*)_check_null(_tmp630->topt);
void*_tmp62F=_tmp631->type;
Cyc_Toc_exp_to_c(nv,_tmp630);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp62F) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp62E))
_tmp630->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp630->r,0));};}
# 3629
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp6A2);}
# 3631
_npop_handler(0);goto _LL283;
# 3615
;_pop_region(rgn);};}case 30: _LL2C2: _tmp6A6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp6A5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_tmp6A4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp496)->f3;_LL2C3: {
# 3634
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp632=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp633=Cyc_Absyn_var_exp(_tmp632,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6A4->name,_tmp6A5->name));
tag_exp=_tmp6A5->is_extensible?Cyc_Absyn_var_exp(_tmp6A4->name,0):
 Cyc_Toc_datatype_tag(_tmp6A5,_tmp6A4->name);
mem_exp=_tmp633;{
struct Cyc_List_List*_tmp634=_tmp6A4->typs;
# 3645
if(Cyc_Toc_is_toplevel(nv)){
# 3647
struct Cyc_List_List*dles=0;
for(0;_tmp6A6 != 0;(_tmp6A6=_tmp6A6->tl,_tmp634=_tmp634->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp6A6->hd;
void*_tmp635=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp634))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp635))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);
dles=({struct Cyc_List_List*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->hd=({struct _tuple19*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->f1=0;_tmp637->f2=cur_e;_tmp637;});_tmp636->tl=dles;_tmp636;});}
# 3658
dles=({struct Cyc_List_List*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->hd=({struct _tuple19*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->f1=0;_tmp639->f2=tag_exp;_tmp639;});_tmp638->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp638;});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3664
struct Cyc_List_List*_tmp63A=({struct Cyc_List_List*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->hd=
Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp641->tl=0;_tmp641;});
# 3667
{int i=1;for(0;_tmp6A6 != 0;(((_tmp6A6=_tmp6A6->tl,i ++)),_tmp634=_tmp634->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp6A6->hd;
void*_tmp63B=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp634))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp63B))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp63C=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3677
_tmp63A=({struct Cyc_List_List*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->hd=_tmp63C;_tmp63D->tl=_tmp63A;_tmp63D;});};}}{
# 3679
struct Cyc_Absyn_Stmt*_tmp63E=Cyc_Absyn_exp_stmt(_tmp633,0);
struct Cyc_Absyn_Stmt*_tmp63F=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->hd=_tmp63E;_tmp640->tl=_tmp63A;_tmp640;})),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp632,datatype_ctype,0,_tmp63F,0));};}
# 3683
goto _LL283;};}case 31: _LL2C4: _LL2C5:
# 3685
 goto _LL2C7;case 32: _LL2C6: _LL2C7:
 goto _LL283;case 33: _LL2C8: _tmp6AC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp496)->f1).is_calloc;_tmp6AB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp496)->f1).rgn;_tmp6AA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp496)->f1).elt_type;_tmp6A9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp496)->f1).num_elts;_tmp6A8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp496)->f1).fat_result;_tmp6A7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp496)->f1).inline_call;_LL2C9: {
# 3689
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp6AA)));
Cyc_Toc_exp_to_c(nv,_tmp6A9);
# 3693
if(_tmp6A8){
struct _tuple1*_tmp642=Cyc_Toc_temp_var();
struct _tuple1*_tmp643=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp6AC){
xexp=_tmp6A9;
if(_tmp6AB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6AB;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp642,0));}else{
# 3704
pexp=Cyc_Toc_calloc_exp(*_tmp6AA,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp642,0));}
# 3706
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp644[3];_tmp644[2]=
# 3708
Cyc_Absyn_var_exp(_tmp642,0);_tmp644[1]=
# 3707
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp644[0]=Cyc_Absyn_var_exp(_tmp643,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp644,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{
# 3710
if(_tmp6AB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6AB;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp6A7)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,Cyc_Absyn_var_exp(_tmp642,0));else{
# 3716
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp642,0));}}else{
# 3718
pexp=Cyc_Toc_malloc_exp(*_tmp6AA,Cyc_Absyn_var_exp(_tmp642,0));}
# 3720
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp645[3];_tmp645[2]=
Cyc_Absyn_var_exp(_tmp642,0);_tmp645[1]=
# 3720
Cyc_Absyn_uint_exp(1,0);_tmp645[0]=Cyc_Absyn_var_exp(_tmp643,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp645,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}{
# 3723
struct Cyc_Absyn_Stmt*_tmp646=Cyc_Absyn_declare_stmt(_tmp642,Cyc_Absyn_uint_typ,_tmp6A9,
Cyc_Absyn_declare_stmt(_tmp643,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp646);};}else{
# 3728
if(_tmp6AC){
if(_tmp6AB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6AB;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp6A9))->r;}else{
# 3734
e->r=(Cyc_Toc_calloc_exp(*_tmp6AA,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp6A9))->r;}}else{
# 3737
if(_tmp6AB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6AB;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp6A7)
e->r=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp6A9))->r;else{
# 3743
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp6A9))->r;}}else{
# 3745
e->r=(Cyc_Toc_malloc_exp(*_tmp6AA,_tmp6A9))->r;}}}
# 3749
goto _LL283;}case 34: _LL2CA: _tmp6AE=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp6AD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL2CB: {
# 3758
void*e1_old_typ=(void*)_check_null(_tmp6AE->topt);
void*e2_old_typ=(void*)_check_null(_tmp6AD->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp647=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp648="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp648,sizeof(char),57);}),_tag_dyneither(_tmp647,sizeof(void*),0));});{
# 3766
unsigned int _tmp649=e->loc;
struct _tuple1*_tmp64A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp64B=Cyc_Absyn_var_exp(_tmp64A,_tmp649);_tmp64B->topt=e1_old_typ;{
struct _tuple1*_tmp64C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp64D=Cyc_Absyn_var_exp(_tmp64C,_tmp649);_tmp64D->topt=e2_old_typ;{
# 3772
struct Cyc_Absyn_Exp*_tmp64E=Cyc_Absyn_assign_exp(Cyc_Tcutil_deep_copy_exp(1,_tmp6AE),_tmp64D,_tmp649);_tmp64E->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp64F=Cyc_Absyn_exp_stmt(_tmp64E,_tmp649);
struct Cyc_Absyn_Exp*_tmp650=Cyc_Absyn_assign_exp(Cyc_Tcutil_deep_copy_exp(1,_tmp6AD),_tmp64B,_tmp649);_tmp650->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp651=Cyc_Absyn_exp_stmt(_tmp650,_tmp649);
# 3777
struct Cyc_Absyn_Stmt*_tmp652=Cyc_Absyn_declare_stmt(_tmp64A,e1_old_typ,_tmp6AE,
Cyc_Absyn_declare_stmt(_tmp64C,e2_old_typ,_tmp6AD,
Cyc_Absyn_seq_stmt(_tmp64F,_tmp651,_tmp649),_tmp649),_tmp649);
Cyc_Toc_stmt_to_c(nv,_tmp652);
e->r=Cyc_Toc_stmt_exp_r(_tmp652);
goto _LL283;};};};};};}case 37: _LL2CC: _tmp6B0=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_tmp6AF=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL2CD: {
# 3785
void*_tmp653=Cyc_Tcutil_compress((void*)_check_null(_tmp6B0->topt));
Cyc_Toc_exp_to_c(nv,_tmp6B0);
{void*_tmp654=_tmp653;struct Cyc_Absyn_Aggrdecl*_tmp65A;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp654)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp654)->f1).aggr_info).KnownAggr).tag == 2){_LL3B8: _tmp65A=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp654)->f1).aggr_info).KnownAggr).val;_LL3B9: {
# 3789
struct Cyc_Absyn_Exp*_tmp655=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp65A,_tmp6AF),0);
struct Cyc_Absyn_Exp*_tmp656=Cyc_Toc_member_exp(_tmp6B0,_tmp6AF,0);
struct Cyc_Absyn_Exp*_tmp657=Cyc_Toc_member_exp(_tmp656,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp657,_tmp655,0))->r;
goto _LL3B7;}}else{goto _LL3BA;}}else{_LL3BA: _LL3BB:
({void*_tmp658=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp659="non-aggregate type in tagcheck";_tag_dyneither(_tmp659,sizeof(char),31);}),_tag_dyneither(_tmp658,sizeof(void*),0));});}_LL3B7:;}
# 3796
goto _LL283;}case 36: _LL2CE: _tmp6B1=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp496)->f1;_LL2CF:
 Cyc_Toc_stmt_to_c(nv,_tmp6B1);goto _LL283;case 35: _LL2D0: _LL2D1:
({void*_tmp65B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp65C="UnresolvedMem";_tag_dyneither(_tmp65C,sizeof(char),14);}),_tag_dyneither(_tmp65B,sizeof(void*),0));});case 24: _LL2D2: _LL2D3:
({void*_tmp65D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp65E="compoundlit";_tag_dyneither(_tmp65E,sizeof(char),12);}),_tag_dyneither(_tmp65D,sizeof(void*),0));});case 38: _LL2D4: _LL2D5:
({void*_tmp65F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp660="valueof(-)";_tag_dyneither(_tmp660,sizeof(char),11);}),_tag_dyneither(_tmp65F,sizeof(void*),0));});default: _LL2D6: _LL2D7:
({void*_tmp661=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp662="__asm__";_tag_dyneither(_tmp662,sizeof(char),8);}),_tag_dyneither(_tmp661,sizeof(void*),0));});}_LL283:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3829 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3831
return({struct _tuple31*_tmp6B2=_region_malloc(r,sizeof(*_tmp6B2));_tmp6B2->f1=0;_tmp6B2->f2=Cyc_Toc_fresh_label();_tmp6B2->f3=Cyc_Toc_fresh_label();_tmp6B2->f4=sc;_tmp6B2;});}
# 3836
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp6B3=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3842
if((int)(((_tmp6B3->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp6B6=_tmp6B3->orig_pat;if((_tmp6B6.pattern).tag != 1)_throw_match();(_tmp6B6.pattern).val;})->topt);
void*_tmp6B4=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp6B5=_tmp6B4;switch(*((int*)_tmp6B5)){case 0: _LL3BD: _LL3BE:
# 3847
 goto _LL3C0;case 11: _LL3BF: _LL3C0:
 goto _LL3C2;case 12: _LL3C1: _LL3C2:
 goto _LL3BC;default: _LL3C3: _LL3C4:
 v=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(t),v);goto _LL3BC;}_LL3BC:;}{
# 3853
void*_tmp6B7=_tmp6B3->access;void*_tmp6B8=_tmp6B7;struct Cyc_Absyn_Datatypedecl*_tmp6C3;struct Cyc_Absyn_Datatypefield*_tmp6C2;unsigned int _tmp6C1;int _tmp6C0;struct _dyneither_ptr*_tmp6BF;unsigned int _tmp6BE;switch(*((int*)_tmp6B8)){case 0: _LL3C6: _LL3C7:
# 3858
 goto _LL3C5;case 1: _LL3C8: _LL3C9:
# 3863
 if(ps->tl != 0){
void*_tmp6B9=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp6BA=_tmp6B9;struct Cyc_Absyn_Datatypedecl*_tmp6BD;struct Cyc_Absyn_Datatypefield*_tmp6BC;unsigned int _tmp6BB;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6BA)->tag == 3){_LL3D1: _tmp6BD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6BA)->f1;_tmp6BC=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6BA)->f2;_tmp6BB=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6BA)->f3;_LL3D2:
# 3866
 ps=ps->tl;
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6BC->name,_tmp6BD->name)),Cyc_Toc_mt_tq),v);
v=Cyc_Absyn_aggrarrow_exp(v,Cyc_Absyn_fieldname((int)(_tmp6BB + 1)),0);
continue;}else{_LL3D3: _LL3D4:
# 3871
 goto _LL3D0;}_LL3D0:;}
# 3874
v=Cyc_Absyn_deref_exp(v,0);
goto _LL3C5;case 2: _LL3CA: _tmp6BE=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp6B8)->f1;_LL3CB:
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6BE + 1)),0);goto _LL3C5;case 4: _LL3CC: _tmp6C0=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6B8)->f1;_tmp6BF=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6B8)->f2;_LL3CD:
# 3878
 v=Cyc_Toc_member_exp(v,_tmp6BF,0);
if(_tmp6C0)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0);
goto _LL3C5;default: _LL3CE: _tmp6C3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B8)->f1;_tmp6C2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B8)->f2;_tmp6C1=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B8)->f3;_LL3CF:
# 3883
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6C1 + 1)),0);
goto _LL3C5;}_LL3C5:;};}
# 3887
return v;}
# 3892
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp6C4=t;struct Cyc_Absyn_Datatypedecl*_tmp6DF;struct Cyc_Absyn_Datatypefield*_tmp6DE;struct _dyneither_ptr*_tmp6DD;int _tmp6DC;int _tmp6DB;struct Cyc_Absyn_Datatypedecl*_tmp6DA;struct Cyc_Absyn_Datatypefield*_tmp6D9;unsigned int _tmp6D8;struct _dyneither_ptr _tmp6D7;int _tmp6D6;void*_tmp6D5;struct Cyc_Absyn_Enumfield*_tmp6D4;struct Cyc_Absyn_Enumdecl*_tmp6D3;struct Cyc_Absyn_Enumfield*_tmp6D2;struct Cyc_Absyn_Exp*_tmp6D1;switch(*((int*)_tmp6C4)){case 0: _LL3D6: _tmp6D1=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6C4)->f1;_LL3D7:
# 3896
 if(_tmp6D1 == 0)return v;else{return _tmp6D1;}case 1: _LL3D8: _LL3D9:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 2: _LL3DA: _LL3DB:
 return Cyc_Absyn_neq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 3: _LL3DC: _tmp6D3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6C4)->f1;_tmp6D2=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6C4)->f2;_LL3DD:
# 3900
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6C6;_tmp6C6.tag=31;_tmp6C6.f1=_tmp6D3;_tmp6C6.f2=_tmp6D2;_tmp6C6;});_tmp6C5;}),0),0);case 4: _LL3DE: _tmp6D5=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6C4)->f1;_tmp6D4=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6C4)->f2;_LL3DF:
# 3902
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6C8;_tmp6C8.tag=32;_tmp6C8.f1=_tmp6D5;_tmp6C8.f2=_tmp6D4;_tmp6C8;});_tmp6C7;}),0),0);case 5: _LL3E0: _tmp6D7=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6C4)->f1;_tmp6D6=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6C4)->f2;_LL3E1:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_float_exp(_tmp6D7,_tmp6D6,0),0);case 6: _LL3E2: _tmp6D8=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6C4)->f1;_LL3E3:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp(_tmp6D8,0),0);case 7: _LL3E4: _tmp6DB=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6C4)->f1;_tmp6DA=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6C4)->f2;_tmp6D9=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6C4)->f3;_LL3E5:
# 3908
 LOOP1: {
void*_tmp6C9=v->r;void*_tmp6CA=_tmp6C9;struct Cyc_Absyn_Exp*_tmp6CC;struct Cyc_Absyn_Exp*_tmp6CB;switch(*((int*)_tmp6CA)){case 13: _LL3EB: _tmp6CB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6CA)->f2;_LL3EC:
 v=_tmp6CB;goto LOOP1;case 19: _LL3ED: _tmp6CC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6CA)->f1;_LL3EE:
 v=_tmp6CC;goto _LL3EA;default: _LL3EF: _LL3F0:
 goto _LL3EA;}_LL3EA:;}
# 3915
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6D9->name,_tmp6DA->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_uint_exp((unsigned int)_tmp6DB,0),0);case 8: _LL3E6: _tmp6DD=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6C4)->f1;_tmp6DC=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6C4)->f2;_LL3E7:
# 3918
 v=Cyc_Toc_member_exp(Cyc_Toc_member_exp(v,_tmp6DD,0),Cyc_Toc_tag_sp,0);
return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp((unsigned int)_tmp6DC,0),0);default: _LL3E8: _tmp6DF=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6C4)->f1;_tmp6DE=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6C4)->f2;_LL3E9:
# 3922
 LOOP2: {
void*_tmp6CD=v->r;void*_tmp6CE=_tmp6CD;struct Cyc_Absyn_Exp*_tmp6D0;struct Cyc_Absyn_Exp*_tmp6CF;switch(*((int*)_tmp6CE)){case 13: _LL3F2: _tmp6CF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6CE)->f2;_LL3F3:
 v=_tmp6CF;goto LOOP2;case 19: _LL3F4: _tmp6D0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6CE)->f1;_LL3F5:
 v=_tmp6D0;goto _LL3F1;default: _LL3F6: _LL3F7:
 goto _LL3F1;}_LL3F1:;}
# 3929
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6DE->name,_tmp6DF->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp6DE->name,0),0);}_LL3D5:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3941
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple32{int f1;void*f2;};
# 3949
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3951
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp6E2=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp6E3=_tmp6E2;void*_tmp6ED;_LL3F9: _tmp6ED=_tmp6E3.f1;_LL3FA:;{
void*_tmp6E4=_tmp6ED;struct Cyc_Absyn_Exp*_tmp6EC;struct _dyneither_ptr*_tmp6EB;switch(*((int*)_tmp6E4)){case 3: _LL3FC: _LL3FD:
# 3957
 goto _LL3FF;case 4: _LL3FE: _LL3FF:
 goto _LL401;case 6: _LL400: _LL401:
# 3960
 continue;case 8: _LL402: _tmp6EB=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6E4)->f1;_LL403:
# 3962
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5[0]=({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmp6E6;_tmp6E6.tag=3;_tmp6E6.f1=_tmp6EB;_tmp6E6;});_tmp6E5;});
continue;case 7: _LL404: _LL405:
# 3966
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0: _LL406: _tmp6EC=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6E4)->f1;if(_tmp6EC != 0){_LL407:
# 3970
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7[0]=({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmp6E8;_tmp6E8.tag=2;_tmp6E8.f1=_tmp6EC;_tmp6E8;});_tmp6E7;});
return({struct _tuple32 _tmp6E9;_tmp6E9.f1=0;_tmp6E9.f2=k;_tmp6E9;});}else{_LL408: _LL409:
 goto _LL40B;}case 1: _LL40A: _LL40B:
 goto _LL40D;case 2: _LL40C: _LL40D:
 goto _LL40F;case 5: _LL40E: _LL40F:
 goto _LL411;default: _LL410: _LL411:
 return({struct _tuple32 _tmp6EA;_tmp6EA.f1=0;_tmp6EA.f2=k;_tmp6EA;});}_LL3FB:;};}
# 3979
return({struct _tuple32 _tmp6EE;_tmp6EE.f1=c;_tmp6EE.f2=k;_tmp6EE;});}
# 3984
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp6EF=p;int _tmp6FC;int _tmp6FB;unsigned int _tmp6FA;void*_tmp6F9;struct Cyc_Absyn_Enumfield*_tmp6F8;struct Cyc_Absyn_Enumdecl*_tmp6F7;struct Cyc_Absyn_Enumfield*_tmp6F6;switch(*((int*)_tmp6EF)){case 3: _LL413: _tmp6F7=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6EF)->f1;_tmp6F6=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6EF)->f2;_LL414:
# 3988
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6F1;_tmp6F1.tag=31;_tmp6F1.f1=_tmp6F7;_tmp6F1.f2=_tmp6F6;_tmp6F1;});_tmp6F0;}),0);goto _LL412;case 4: _LL415: _tmp6F9=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6EF)->f1;_tmp6F8=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6EF)->f2;_LL416:
# 3990
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6F3;_tmp6F3.tag=32;_tmp6F3.f1=_tmp6F9;_tmp6F3.f2=_tmp6F8;_tmp6F3;});_tmp6F2;}),0);goto _LL412;case 6: _LL417: _tmp6FA=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6EF)->f1;_LL418:
 _tmp6FB=(int)_tmp6FA;goto _LL41A;case 7: _LL419: _tmp6FB=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6EF)->f1;_LL41A:
 _tmp6FC=_tmp6FB;goto _LL41C;case 8: _LL41B: _tmp6FC=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6EF)->f2;_LL41C:
# 3994
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp6FC,0);
goto _LL412;default: _LL41D: _LL41E:
({void*_tmp6F4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6F5="compile_pat_test_as_case!";_tag_dyneither(_tmp6F5,sizeof(char),26);}),_tag_dyneither(_tmp6F4,sizeof(void*),0));});}_LL412:;}
# 3998
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD[0]=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp6FE;_tmp6FE.tag=17;_tmp6FE.f1=e;_tmp6FE;});_tmp6FD;}),0);}
# 4002
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0);}struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 4011
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 4014
void*t=(void*)_check_null(p->topt);
void*_tmp6FF=p->r;void*_tmp700=_tmp6FF;union Cyc_Absyn_AggrInfoU _tmp738;struct Cyc_List_List*_tmp737;struct Cyc_List_List*_tmp736;struct Cyc_List_List*_tmp735;struct Cyc_Absyn_Pat*_tmp734;struct Cyc_Absyn_Datatypedecl*_tmp733;struct Cyc_Absyn_Datatypefield*_tmp732;struct Cyc_List_List*_tmp731;struct Cyc_Absyn_Vardecl*_tmp730;struct Cyc_Absyn_Pat*_tmp72F;struct Cyc_Absyn_Vardecl*_tmp72E;struct Cyc_Absyn_Vardecl*_tmp72D;struct Cyc_Absyn_Pat*_tmp72C;struct Cyc_Absyn_Vardecl*_tmp72B;switch(*((int*)_tmp700)){case 2: _LL420: _tmp72B=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp700)->f2;_LL421: {
# 4017
struct Cyc_Absyn_Pat*_tmp701=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp701->topt=p->topt;
_tmp72D=_tmp72B;_tmp72C=_tmp701;goto _LL423;}case 1: _LL422: _tmp72D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp700)->f1;_tmp72C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp700)->f2;_LL423:
# 4022
*decls=({struct Cyc_List_List*_tmp702=_region_malloc(rgn,sizeof(*_tmp702));_tmp702->hd=_tmp72D;_tmp702->tl=*decls;_tmp702;});{
struct Cyc_Absyn_Stmt*_tmp703=Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp704=_cycalloc(sizeof(*_tmp704));_tmp704[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp705;_tmp705.tag=4;_tmp705.f1=_tmp72D;_tmp705;});_tmp704;}),0),Cyc_Absyn_copy_exp(path),0);
return Cyc_Toc_seq_stmt_opt(_tmp703,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp72C));};case 4: _LL424: _tmp72E=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp700)->f2;_LL425:
# 4027
*decls=({struct Cyc_List_List*_tmp706=_region_malloc(rgn,sizeof(*_tmp706));_tmp706->hd=_tmp72E;_tmp706->tl=*decls;_tmp706;});
return Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp708;_tmp708.tag=4;_tmp708.f1=_tmp72E;_tmp708;});_tmp707;}),0),Cyc_Absyn_copy_exp(path),0);case 0: _LL426: _LL427:
 return 0;case 3: _LL428: _tmp730=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp700)->f1;_tmp72F=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp700)->f2;_LL429:
# 4032
*decls=({struct Cyc_List_List*_tmp709=_region_malloc(rgn,sizeof(*_tmp709));_tmp709->hd=_tmp730;_tmp709->tl=*decls;_tmp709;});
_tmp730->type=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);{
# 4035
struct Cyc_Absyn_Stmt*_tmp70A=Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp70C;_tmp70C.tag=4;_tmp70C.f1=_tmp730;_tmp70C;});_tmp70B;}),0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path))),0);
return Cyc_Toc_seq_stmt_opt(_tmp70A,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp72F));};case 9: _LL42A: _LL42B:
# 4040
 goto _LL42D;case 10: _LL42C: _LL42D:
 goto _LL42F;case 11: _LL42E: _LL42F:
 goto _LL431;case 12: _LL430: _LL431:
 goto _LL433;case 13: _LL432: _LL433:
 goto _LL435;case 14: _LL434: _LL435:
 return 0;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp700)->f1)->r)->tag == 8){_LL436: _tmp733=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp700)->f1)->r)->f1;_tmp732=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp700)->f1)->r)->f2;_tmp731=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp700)->f1)->r)->f3;_LL437:
# 4049
 if(_tmp731 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp732->name,_tmp733->name);
void*_tmp70D=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
path=Cyc_Toc_cast_it(_tmp70D,path);{
int cnt=1;
struct Cyc_List_List*_tmp70E=_tmp732->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp731 != 0;(((_tmp731=_tmp731->tl,_tmp70E=((struct Cyc_List_List*)_check_null(_tmp70E))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp70F=(struct Cyc_Absyn_Pat*)_tmp731->hd;
if(_tmp70F->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp710=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp70E))->hd)).f2;
void*_tmp711=(void*)_check_null(_tmp70F->topt);
void*_tmp712=Cyc_Toc_typ_to_c_array(_tmp711);
struct Cyc_Absyn_Exp*_tmp713=Cyc_Absyn_aggrarrow_exp(path,Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp710)))
_tmp713=Cyc_Toc_cast_it(_tmp712,_tmp713);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp713,_tmp70F));};}
# 4067
return s;};};}else{_LL440: _tmp734=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp700)->f1;_LL441:
# 4116
 return Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Absyn_deref_exp(path,0),_tmp734);}case 8: _LL438: _tmp735=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp700)->f3;_LL439:
# 4069
 _tmp736=_tmp735;goto _LL43B;case 5: _LL43A: _tmp736=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp700)->f1;_LL43B: {
# 4071
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp736 != 0;(_tmp736=_tmp736->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp714=(struct Cyc_Absyn_Pat*)_tmp736->hd;
if(_tmp714->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp715=(void*)_check_null(_tmp714->topt);
struct _dyneither_ptr*_tmp716=Cyc_Absyn_fieldname(cnt);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Toc_member_exp(path,_tmp716,0),_tmp714));};}
# 4081
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp700)->f1 == 0){_LL43C: _LL43D:
({void*_tmp717=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp718="unresolved aggregate pattern!";_tag_dyneither(_tmp718,sizeof(char),30);}),_tag_dyneither(_tmp717,sizeof(void*),0));});}else{_LL43E: _tmp738=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp700)->f1)->aggr_info;_tmp737=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp700)->f3;_LL43F: {
# 4085
struct Cyc_Absyn_Aggrdecl*_tmp719=Cyc_Absyn_get_known_aggrdecl(_tmp738);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp737 != 0;_tmp737=_tmp737->tl){
struct _tuple33*_tmp71A=(struct _tuple33*)_tmp737->hd;
struct Cyc_Absyn_Pat*_tmp71B=(*_tmp71A).f2;
if(_tmp71B->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp71C=(void*)((struct Cyc_List_List*)_check_null((*_tmp71A).f1))->hd;void*_tmp71D=_tmp71C;struct _dyneither_ptr*_tmp720;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp71D)->tag == 1){_LL449: _tmp720=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp71D)->f1;_LL44A:
 f=_tmp720;goto _LL448;}else{_LL44B: _LL44C:
(int)_throw((void*)({struct Cyc_Toc_Match_error_exn_struct*_tmp71E=_cycalloc_atomic(sizeof(*_tmp71E));_tmp71E[0]=({struct Cyc_Toc_Match_error_exn_struct _tmp71F;_tmp71F.tag=Cyc_Toc_Match_error;_tmp71F;});_tmp71E;}));}_LL448:;}{
# 4097
void*_tmp721=(void*)_check_null(_tmp71B->topt);
void*_tmp722=Cyc_Toc_typ_to_c_array(_tmp721);
struct Cyc_Absyn_Exp*_tmp723=Cyc_Toc_member_exp(path,f,0);
# 4101
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp719->impl))->tagged)_tmp723=Cyc_Toc_member_exp(_tmp723,Cyc_Toc_val_sp,0);{
void*_tmp724=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp719->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp724))
_tmp723=Cyc_Toc_cast_it(_tmp722,_tmp723);else{
if(!Cyc_Toc_is_array_type(_tmp724) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp724)))
# 4108
_tmp723=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp722,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp723,0)),0);}
# 4111
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp723,_tmp71B));};};};}
# 4113
return s;}}case 15: _LL442: _LL443:
# 4118
({void*_tmp725=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp726="unknownid pat";_tag_dyneither(_tmp726,sizeof(char),14);}),_tag_dyneither(_tmp725,sizeof(void*),0));});case 16: _LL444: _LL445:
({void*_tmp727=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp728="unknowncall pat";_tag_dyneither(_tmp728,sizeof(char),16);}),_tag_dyneither(_tmp727,sizeof(void*),0));});default: _LL446: _LL447:
({void*_tmp729=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp72A="exp pat";_tag_dyneither(_tmp72A,sizeof(char),8);}),_tag_dyneither(_tmp729,sizeof(void*),0));});}_LL41F:;}struct _tuple34{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4129
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4137
void*_tmp739=dopt;struct Cyc_List_List*_tmp778;struct Cyc_List_List*_tmp777;void*_tmp776;struct Cyc_Tcpat_Rhs*_tmp775;if(_tmp739 == 0){_LL44E: _LL44F:
# 4139
 return Cyc_Absyn_skip_stmt(0);}else{switch(*((int*)_tmp739)){case 0: _LL450: _LL451:
 return Cyc_Toc_throw_match_stmt();case 1: _LL452: _tmp775=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp739)->f1;_LL453:
# 4144
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp73A=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp73B=_tmp73A;int*_tmp742;struct _dyneither_ptr*_tmp741;struct _dyneither_ptr*_tmp740;struct Cyc_Absyn_Switch_clause*_tmp73F;_LL457: _tmp742=(int*)& _tmp73B->f1;_tmp741=_tmp73B->f2;_tmp740=_tmp73B->f3;_tmp73F=_tmp73B->f4;_LL458:;{
struct Cyc_Absyn_Stmt*_tmp73C=_tmp73F->body;
if(_tmp73C == _tmp775->rhs){
# 4149
if(*_tmp742)
return Cyc_Absyn_goto_stmt(_tmp741,0);
*_tmp742=1;{
# 4153
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp73F->pattern);
# 4156
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp740,_tmp73F->body,0);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0);
res=Cyc_Absyn_label_stmt(_tmp741,res,0);
*bodies=({struct Cyc_List_List*_tmp73D=_region_malloc(rgn,sizeof(*_tmp73D));_tmp73D->hd=({struct _tuple34*_tmp73E=_region_malloc(rgn,sizeof(*_tmp73E));_tmp73E->f1=nv;_tmp73E->f2=_tmp740;_tmp73E->f3=_tmp73C;_tmp73E;});_tmp73D->tl=*bodies;_tmp73D;});
return res;};}};}
# 4164
({void*_tmp743=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp744="couldn't find rhs!";_tag_dyneither(_tmp744,sizeof(char),19);}),_tag_dyneither(_tmp743,sizeof(void*),0));});default: _LL454: _tmp778=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp739)->f1;_tmp777=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp739)->f2;_tmp776=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp739)->f3;_LL455: {
# 4167
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp776,lscs,v);
# 4169
struct Cyc_Absyn_Exp*_tmp745=Cyc_Toc_compile_path(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp778),Cyc_Absyn_var_exp(v,0));
struct Cyc_List_List*_tmp746=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp777);
# 4172
struct _tuple32 _tmp747=Cyc_Toc_admits_switch(_tmp746);struct _tuple32 _tmp748=_tmp747;int _tmp774;void*_tmp773;_LL45A: _tmp774=_tmp748.f1;_tmp773=_tmp748.f2;_LL45B:;
if(_tmp774 > 1){
# 4176
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759->hd=({struct Cyc_Absyn_Switch_clause*_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp75A->pat_vars=0;_tmp75A->where_clause=0;_tmp75A->body=res;_tmp75A->loc=0;_tmp75A;});_tmp759->tl=0;_tmp759;});
# 4179
for(0;_tmp746 != 0;_tmp746=_tmp746->tl){
struct _tuple29 _tmp749=*((struct _tuple29*)_tmp746->hd);struct _tuple29 _tmp74A=_tmp749;void*_tmp750;void*_tmp74F;_LL45D: _tmp750=_tmp74A.f1;_tmp74F=_tmp74A.f2;_LL45E:;{
# 4182
struct Cyc_Absyn_Pat*_tmp74B=Cyc_Toc_compile_pat_test_as_case(_tmp750);
# 4184
struct Cyc_Absyn_Stmt*_tmp74C=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp74F,lscs,v);
# 4186
new_lscs=({struct Cyc_List_List*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D->hd=({struct Cyc_Absyn_Switch_clause*_tmp74E=_cycalloc(sizeof(*_tmp74E));_tmp74E->pattern=_tmp74B;_tmp74E->pat_vars=0;_tmp74E->where_clause=0;_tmp74E->body=_tmp74C;_tmp74E->loc=0;_tmp74E;});_tmp74D->tl=new_lscs;_tmp74D;});};}
# 4188
{void*_tmp751=_tmp773;struct _dyneither_ptr*_tmp756;switch(*((int*)_tmp751)){case 1: _LL460: _LL461:
# 4191
 LOOP1: {
void*_tmp752=_tmp745->r;void*_tmp753=_tmp752;struct Cyc_Absyn_Exp*_tmp755;struct Cyc_Absyn_Exp*_tmp754;switch(*((int*)_tmp753)){case 13: _LL469: _tmp754=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp753)->f2;_LL46A:
 _tmp745=_tmp754;goto LOOP1;case 19: _LL46B: _tmp755=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp753)->f1;_LL46C:
 _tmp745=_tmp755;goto _LL468;default: _LL46D: _LL46E:
 goto _LL468;}_LL468:;}
# 4197
_tmp745=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),_tmp745),0);goto _LL45F;case 3: _LL462: _tmp756=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp751)->f1;_LL463:
# 4200
 _tmp745=Cyc_Toc_member_exp(Cyc_Toc_member_exp(_tmp745,_tmp756,0),Cyc_Toc_tag_sp,0);goto _LL45F;case 2: _LL464: _LL465:
# 4202
 goto _LL45F;default: _LL466: _LL467:
 goto _LL45F;}_LL45F:;}
# 4205
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp758;_tmp758.tag=10;_tmp758.f1=_tmp745;_tmp758.f2=new_lscs;_tmp758.f3=0;_tmp758;});_tmp757;}),0);}else{
# 4209
void*_tmp75B=_tmp773;struct Cyc_Absyn_Exp*_tmp772;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp75B)->tag == 2){_LL470: _tmp772=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp75B)->f1;_LL471: {
# 4211
struct Cyc_List_List*_tmp75C=_tmp746;void*_tmp76B;struct Cyc_Tcpat_Rhs*_tmp76A;if(_tmp75C != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp75C)->hd)->f2)->tag == 1){if(((struct Cyc_List_List*)_tmp75C)->tl == 0){_LL475: _tmp76B=((struct _tuple29*)_tmp75C->hd)->f1;_tmp76A=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp75C->hd)->f2)->f1;_LL476:
# 4217
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp75D=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp75E=_tmp75D;int*_tmp765;struct _dyneither_ptr*_tmp764;struct _dyneither_ptr*_tmp763;struct Cyc_Absyn_Switch_clause*_tmp762;_LL47A: _tmp765=(int*)& _tmp75E->f1;_tmp764=_tmp75E->f2;_tmp763=_tmp75E->f3;_tmp762=_tmp75E->f4;_LL47B:;{
struct Cyc_Absyn_Stmt*_tmp75F=_tmp762->body;
if(_tmp75F == _tmp76A->rhs){
# 4222
if(*_tmp765)
return Cyc_Absyn_goto_stmt(_tmp764,0);
*_tmp765=1;{
# 4226
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp762->pattern);
# 4229
Cyc_Toc_exp_to_c(nv,_tmp772);{
# 4232
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp763,_tmp762->body,0);
r=Cyc_Absyn_ifthenelse_stmt(_tmp772,r,res,0);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0);
r=Cyc_Absyn_label_stmt(_tmp764,r,0);
*bodies=({struct Cyc_List_List*_tmp760=_region_malloc(rgn,sizeof(*_tmp760));_tmp760->hd=({struct _tuple34*_tmp761=_region_malloc(rgn,sizeof(*_tmp761));_tmp761->f1=nv;_tmp761->f2=_tmp763;_tmp761->f3=_tmp75F;_tmp761;});_tmp760->tl=*bodies;_tmp760;});
return r;};};}};}
# 4241
({void*_tmp766=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp767="couldn't find rhs!";_tag_dyneither(_tmp767,sizeof(char),19);}),_tag_dyneither(_tmp766,sizeof(void*),0));});}else{goto _LL477;}}else{goto _LL477;}}else{_LL477: _LL478:
({void*_tmp768=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp769="bad where clause in match compiler";_tag_dyneither(_tmp769,sizeof(char),35);}),_tag_dyneither(_tmp768,sizeof(void*),0));});}_LL474:;}}else{_LL472: _LL473:
# 4246
 for(0;_tmp746 != 0;_tmp746=_tmp746->tl){
struct _tuple29 _tmp76C=*((struct _tuple29*)_tmp746->hd);struct _tuple29 _tmp76D=_tmp76C;void*_tmp771;void*_tmp770;_LL47D: _tmp771=_tmp76D.f1;_tmp770=_tmp76D.f2;_LL47E:;{
struct Cyc_Absyn_Exp*_tmp76E=Cyc_Toc_compile_pat_test(_tmp745,_tmp771);
struct Cyc_Absyn_Stmt*_tmp76F=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp770,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp76E,_tmp76F,res,0);};}}_LL46F:;}
# 4254
return res;}}}_LL44D:;}
# 4264
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4266
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple34*_tmp779=(struct _tuple34*)bodies->hd;struct _tuple34*_tmp77A=_tmp779;struct Cyc_Toc_Env**_tmp77C;struct Cyc_Absyn_Stmt*_tmp77B;_LL480: _tmp77C=(struct Cyc_Toc_Env**)& _tmp77A->f1;_tmp77B=_tmp77A->f3;_LL481:;
if(_tmp77B == s)return _tmp77C;}
# 4272
return 0;}
# 4276
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4279
void*_tmp77D=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4282
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp77E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp77E;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp77F=Cyc_Toc_share_env(rgn,nv);
# 4289
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4294
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp77F,& mydecls,& mybodies,dopt,lscs,v);
# 4302
{struct Cyc_List_List*_tmp780=lscs;for(0;_tmp780 != 0;_tmp780=_tmp780->tl){
struct _tuple31*_tmp781=(struct _tuple31*)_tmp780->hd;struct _tuple31*_tmp782=_tmp781;struct _dyneither_ptr*_tmp78A;struct Cyc_Absyn_Switch_clause*_tmp789;_LL483: _tmp78A=_tmp782->f3;_tmp789=_tmp782->f4;_LL484:;{
struct Cyc_Absyn_Stmt*s=_tmp789->body;
# 4306
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4309
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp783=*envp;
# 4312
if(_tmp780->tl != 0){
struct _tuple31*_tmp784=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp780->tl))->hd;struct _tuple31*_tmp785=_tmp784;struct _dyneither_ptr*_tmp788;struct Cyc_Absyn_Switch_clause*_tmp787;_LL486: _tmp788=_tmp785->f3;_tmp787=_tmp785->f4;_LL487:;{
# 4316
struct Cyc_Toc_Env**_tmp786=Cyc_Toc_find_case_env(mybodies,_tmp787->body);
# 4321
if(_tmp786 == 0)
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp783,end_l),s);else{
# 4325
struct Cyc_List_List*vs=0;
if(_tmp787->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp787->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 4330
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn,_tmp783,end_l,_tmp788,vs),s);}};}else{
# 4334
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp783,end_l),s);}};};}}{
# 4338
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4340
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp78B=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
_tmp78B->name=Cyc_Toc_temp_var();
_tmp78B->type=Cyc_Toc_typ_to_c_array(_tmp78B->type);
res=Cyc_Absyn_decl_stmt(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp78C=_cycalloc(sizeof(*_tmp78C));_tmp78C[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp78D;_tmp78D.tag=0;_tmp78D.f1=_tmp78B;_tmp78D;});_tmp78C;}),0),res,0);}
# 4347
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;
_npop_handler(0);return;};}
# 4286
;_pop_region(rgn);};}
# 4353
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 4355
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4360
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4362
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp78E=_cycalloc(sizeof(*_tmp78E));_tmp78E->hd=
Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp78E->tl=0;_tmp78E;}),0),0);}
# 4366
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4371
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4373
while(1){
void*_tmp78F=s->r;void*_tmp790=_tmp78F;struct Cyc_Absyn_Exp*_tmp7EE;struct Cyc_Absyn_Stmt*_tmp7ED;struct Cyc_List_List*_tmp7EC;void*_tmp7EB;struct Cyc_Absyn_Stmt*_tmp7EA;struct Cyc_Absyn_Exp*_tmp7E9;struct _dyneither_ptr*_tmp7E8;struct Cyc_Absyn_Stmt*_tmp7E7;struct Cyc_Absyn_Decl*_tmp7E6;struct Cyc_Absyn_Stmt*_tmp7E5;struct Cyc_List_List*_tmp7E4;struct Cyc_Absyn_Switch_clause**_tmp7E3;struct Cyc_Absyn_Exp*_tmp7E2;struct Cyc_List_List*_tmp7E1;void*_tmp7E0;struct Cyc_Absyn_Exp*_tmp7DF;struct Cyc_Absyn_Exp*_tmp7DE;struct Cyc_Absyn_Exp*_tmp7DD;struct Cyc_Absyn_Stmt*_tmp7DC;struct Cyc_Absyn_Exp*_tmp7DB;struct Cyc_Absyn_Stmt*_tmp7DA;struct Cyc_Absyn_Exp*_tmp7D9;struct Cyc_Absyn_Stmt*_tmp7D8;struct Cyc_Absyn_Stmt*_tmp7D7;struct Cyc_Absyn_Exp*_tmp7D6;struct Cyc_Absyn_Stmt*_tmp7D5;struct Cyc_Absyn_Stmt*_tmp7D4;struct Cyc_Absyn_Exp*_tmp7D3;switch(*((int*)_tmp790)){case 0: _LL489: _LL48A:
# 4376
 return;case 1: _LL48B: _tmp7D3=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_LL48C:
# 4378
 Cyc_Toc_exp_to_c(nv,_tmp7D3);
return;case 2: _LL48D: _tmp7D5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_tmp7D4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp790)->f2;_LL48E:
# 4381
 Cyc_Toc_stmt_to_c(nv,_tmp7D5);
s=_tmp7D4;
continue;case 3: _LL48F: _tmp7D6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_LL490: {
# 4385
void*topt=0;
if(_tmp7D6 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7D6->topt));
Cyc_Toc_exp_to_c(nv,_tmp7D6);}{
# 4391
int _tmp791=Cyc_Toc_get_npop(s);
if(_tmp791 > 0){
if(topt != 0){
struct _tuple1*_tmp792=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp793=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp792,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp792,topt,_tmp7D6,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(_tmp791),_tmp793,0),0))->r;}else{
# 4400
Cyc_Toc_do_npop_before(_tmp791,s);}}
# 4402
return;};}case 4: _LL491: _tmp7D9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_tmp7D8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp790)->f2;_tmp7D7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp790)->f3;_LL492:
# 4404
 Cyc_Toc_exp_to_c(nv,_tmp7D9);
Cyc_Toc_stmt_to_c(nv,_tmp7D8);
s=_tmp7D7;
continue;case 5: _LL493: _tmp7DB=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp790)->f1).f1;_tmp7DA=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp790)->f2;_LL494:
# 4409
 Cyc_Toc_exp_to_c(nv,_tmp7DB);{
struct _RegionHandle _tmp794=_new_region("temp");struct _RegionHandle*temp=& _tmp794;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7DA);
# 4413
_npop_handler(0);return;
# 4410
;_pop_region(temp);};case 6: _LL495: _LL496: {
# 4415
struct Cyc_Toc_Env*_tmp795=nv;struct _dyneither_ptr**_tmp796;_LL4AC: _tmp796=_tmp795->break_lab;_LL4AD:;
if(_tmp796 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp796);
# 4419
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;}case 7: _LL497: _LL498: {
# 4422
struct Cyc_Toc_Env*_tmp797=nv;struct _dyneither_ptr**_tmp798;_LL4AF: _tmp798=_tmp797->continue_lab;_LL4B0:;
if(_tmp798 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp798);
goto _LL49A;}case 8: _LL499: _LL49A:
# 4428
 Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;case 9: _LL49B: _tmp7DF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_tmp7DE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp790)->f2).f1;_tmp7DD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp790)->f3).f1;_tmp7DC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp790)->f4;_LL49C:
# 4432
 Cyc_Toc_exp_to_c(nv,_tmp7DF);Cyc_Toc_exp_to_c(nv,_tmp7DE);Cyc_Toc_exp_to_c(nv,_tmp7DD);{
struct _RegionHandle _tmp799=_new_region("temp");struct _RegionHandle*temp=& _tmp799;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7DC);
# 4436
_npop_handler(0);return;
# 4433
;_pop_region(temp);};case 10: _LL49D: _tmp7E2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_tmp7E1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp790)->f2;_tmp7E0=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp790)->f3;_LL49E:
# 4438
 Cyc_Toc_xlate_switch(nv,s,_tmp7E2,_tmp7E1,_tmp7E0);
return;case 11: _LL49F: _tmp7E4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_tmp7E3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp790)->f2;_LL4A0: {
# 4441
struct Cyc_Toc_Env*_tmp79A=nv;struct Cyc_Toc_FallthruInfo*_tmp7A5;_LL4B2: _tmp7A5=_tmp79A->fallthru_info;_LL4B3:;
if(_tmp7A5 == 0)
({void*_tmp79B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp79C="fallthru in unexpected place";_tag_dyneither(_tmp79C,sizeof(char),29);}),_tag_dyneither(_tmp79B,sizeof(void*),0));});{
struct Cyc_Toc_FallthruInfo _tmp79D=*_tmp7A5;struct Cyc_Toc_FallthruInfo _tmp79E=_tmp79D;struct _dyneither_ptr*_tmp7A4;struct Cyc_List_List*_tmp7A3;_LL4B5: _tmp7A4=_tmp79E.label;_tmp7A3=_tmp79E.binders;_LL4B6:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7A4,0);
# 4447
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s2);{
struct Cyc_List_List*_tmp79F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7A3);
struct Cyc_List_List*_tmp7A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7E4);
for(0;_tmp79F != 0;(_tmp79F=_tmp79F->tl,_tmp7A0=_tmp7A0->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A0))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp7A1=_cycalloc(sizeof(*_tmp7A1));_tmp7A1[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp7A2;_tmp7A2.tag=5;_tmp7A2.f1=(struct Cyc_Absyn_Vardecl*)_tmp79F->hd;_tmp7A2;});_tmp7A1;}),0),(struct Cyc_Absyn_Exp*)_tmp7A0->hd,0),s2,0);}
# 4455
s->r=s2->r;
return;};};};}case 12: _LL4A1: _tmp7E6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_tmp7E5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp790)->f2;_LL4A2:
# 4461
{void*_tmp7A6=_tmp7E6->r;void*_tmp7A7=_tmp7A6;struct Cyc_Absyn_Tvar*_tmp7C5;struct Cyc_Absyn_Vardecl*_tmp7C4;int _tmp7C3;struct Cyc_Absyn_Exp*_tmp7C2;struct Cyc_Absyn_Fndecl*_tmp7C1;struct Cyc_List_List*_tmp7C0;struct Cyc_Absyn_Pat*_tmp7BF;struct Cyc_Absyn_Exp*_tmp7BE;void*_tmp7BD;struct Cyc_Absyn_Vardecl*_tmp7BC;switch(*((int*)_tmp7A7)){case 0: _LL4B8: _tmp7BC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7A7)->f1;_LL4B9:
 Cyc_Toc_local_decl_to_c(nv,_tmp7BC,_tmp7E5);goto _LL4B7;case 2: _LL4BA: _tmp7BF=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7A7)->f1;_tmp7BE=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7A7)->f3;_tmp7BD=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7A7)->f4;_LL4BB:
# 4469
{void*_tmp7A8=_tmp7BF->r;void*_tmp7A9=_tmp7A8;struct Cyc_Absyn_Vardecl*_tmp7AC;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7A9)->tag == 1){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7A9)->f2)->r)->tag == 0){_LL4C5: _tmp7AC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7A9)->f1;_LL4C6:
# 4471
 _tmp7AC->name=Cyc_Toc_temp_var();
_tmp7AC->initializer=_tmp7BE;
_tmp7E6->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));_tmp7AA[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7AB;_tmp7AB.tag=0;_tmp7AB.f1=_tmp7AC;_tmp7AB;});_tmp7AA;});
Cyc_Toc_local_decl_to_c(nv,_tmp7AC,_tmp7E5);
goto _LL4C4;}else{goto _LL4C7;}}else{_LL4C7: _LL4C8:
# 4480
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7BF,_tmp7BD,(void*)_check_null(_tmp7BE->topt),_tmp7BE,_tmp7E5))->r;
goto _LL4C4;}_LL4C4:;}
# 4483
goto _LL4B7;case 3: _LL4BC: _tmp7C0=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7A7)->f1;_LL4BD: {
# 4487
struct Cyc_List_List*_tmp7AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7C0);
if(_tmp7AD == 0)
({void*_tmp7AE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7AF="empty Letv_d";_tag_dyneither(_tmp7AF,sizeof(char),13);}),_tag_dyneither(_tmp7AE,sizeof(void*),0));});
_tmp7E6->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));_tmp7B0[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7B1;_tmp7B1.tag=0;_tmp7B1.f1=(struct Cyc_Absyn_Vardecl*)_tmp7AD->hd;_tmp7B1;});_tmp7B0;});
_tmp7AD=_tmp7AD->tl;
for(0;_tmp7AD != 0;_tmp7AD=_tmp7AD->tl){
struct Cyc_Absyn_Decl*_tmp7B2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7B3=_cycalloc(sizeof(*_tmp7B3));_tmp7B3[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7B4;_tmp7B4.tag=0;_tmp7B4.f1=(struct Cyc_Absyn_Vardecl*)_tmp7AD->hd;_tmp7B4;});_tmp7B3;}),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7B2,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4496
Cyc_Toc_stmt_to_c(nv,s);
goto _LL4B7;}case 1: _LL4BE: _tmp7C1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7A7)->f1;_LL4BF:
# 4499
 Cyc_Toc_fndecl_to_c(nv,_tmp7C1,0);
Cyc_Toc_stmt_to_c(nv,_tmp7E5);
goto _LL4B7;case 4: _LL4C0: _tmp7C5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A7)->f1;_tmp7C4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A7)->f2;_tmp7C3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A7)->f3;_tmp7C2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A7)->f4;_LL4C1: {
# 4503
struct Cyc_Absyn_Stmt*_tmp7B5=_tmp7E5;
# 4511
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7C4->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4518
Cyc_Toc_stmt_to_c(nv,_tmp7B5);
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp7B5,0))->r;else{
if((unsigned int)_tmp7C2){
Cyc_Toc_exp_to_c(nv,_tmp7C2);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp7C2,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp7B5,0))->r;};}else{
# 4527
s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7->hd=
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0);_tmp7B7->tl=0;_tmp7B7;}),0),
# 4532
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*_tmp7B8[1];_tmp7B8[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7B8,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),
Cyc_Absyn_seq_stmt(_tmp7B5,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp7B9[1];_tmp7B9[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7B9,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r;}}
# 4538
return;}default: _LL4C2: _LL4C3:
({void*_tmp7BA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7BB="bad nested declaration within function";_tag_dyneither(_tmp7BB,sizeof(char),39);}),_tag_dyneither(_tmp7BA,sizeof(void*),0));});}_LL4B7:;}
# 4541
return;case 13: _LL4A3: _tmp7E8=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_tmp7E7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp790)->f2;_LL4A4:
# 4543
 s=_tmp7E7;continue;case 14: _LL4A5: _tmp7EA=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_tmp7E9=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp790)->f2).f1;_LL4A6: {
# 4545
struct _RegionHandle _tmp7C6=_new_region("temp");struct _RegionHandle*temp=& _tmp7C6;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7EA);
Cyc_Toc_exp_to_c(nv,_tmp7E9);
# 4549
_npop_handler(0);return;
# 4545
;_pop_region(temp);}case 15: _LL4A7: _tmp7ED=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_tmp7EC=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp790)->f2;_tmp7EB=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp790)->f3;_LL4A8: {
# 4563 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4573
e_exp->topt=e_typ;{
struct _RegionHandle _tmp7C7=_new_region("temp");struct _RegionHandle*temp=& _tmp7C7;_push_region(temp);
# 4576
Cyc_Toc_stmt_to_c(nv,_tmp7ED);{
struct Cyc_Absyn_Stmt*_tmp7C8=Cyc_Absyn_seq_stmt(_tmp7ED,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4581
struct Cyc_Absyn_Stmt*_tmp7C9=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7D0=_cycalloc(sizeof(*_tmp7D0));_tmp7D0[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp7D1;_tmp7D1.tag=10;_tmp7D1.f1=e_exp;_tmp7D1.f2=_tmp7EC;_tmp7D1.f3=_tmp7EB;_tmp7D1;});_tmp7D0;}),0);
# 4583
Cyc_Toc_stmt_to_c(nv,_tmp7C9);{
# 4586
struct Cyc_Absyn_Exp*_tmp7CA=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));_tmp7CF->hd=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp7CF->tl=0;_tmp7CF;}),0);
# 4590
struct Cyc_Absyn_Stmt*_tmp7CB=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp7CE=_cycalloc(sizeof(*_tmp7CE));_tmp7CE->hd=
Cyc_Absyn_address_exp(h_exp,0);_tmp7CE->tl=0;_tmp7CE;}),0),0);
# 4594
struct Cyc_Absyn_Exp*_tmp7CC=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp7CD=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp7CB,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp7CC,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp7CA,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp7CD,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp7C8,
# 4605
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp7C9,0),0),0),0),0),0))->r;};};
# 4609
_npop_handler(0);return;
# 4574
;_pop_region(temp);};}default: _LL4A9: _tmp7EE=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp790)->f1;_LL4AA:
# 4611
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4614
Cyc_Toc_exp_to_c(nv,_tmp7EE);
s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2->hd=_tmp7EE;_tmp7D2->tl=0;_tmp7D2;}),0));}
# 4617
return;}_LL488:;}}
# 4626
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp7EF=_new_region("frgn");struct _RegionHandle*frgn=& _tmp7EF;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp7F0=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp7F1=f->args;for(0;_tmp7F1 != 0;_tmp7F1=_tmp7F1->tl){
struct _tuple1*_tmp7F2=({struct _tuple1*_tmp7F3=_cycalloc(sizeof(*_tmp7F3));_tmp7F3->f1=Cyc_Absyn_Loc_n;_tmp7F3->f2=(*((struct _tuple9*)_tmp7F1->hd)).f1;_tmp7F3;});
(*((struct _tuple9*)_tmp7F1->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp7F1->hd)).f3);}}
# 4642
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4646
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4[0]=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);_tmp7F4;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp7F5=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp7F6=_tmp7F5;struct _dyneither_ptr*_tmp7FF;struct Cyc_Absyn_Tqual _tmp7FE;void*_tmp7FD;int _tmp7FC;_LL4CA: _tmp7FF=_tmp7F6.name;_tmp7FE=_tmp7F6.tq;_tmp7FD=_tmp7F6.type;_tmp7FC=_tmp7F6.inject;_LL4CB:;{
void*_tmp7F7=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp7FD,(void*)& Cyc_Absyn_HeapRgn_val,_tmp7FE,Cyc_Absyn_false_conref));
struct _tuple1*_tmp7F8=({struct _tuple1*_tmp7FB=_cycalloc(sizeof(*_tmp7FB));_tmp7FB->f1=Cyc_Absyn_Loc_n;_tmp7FB->f2=(struct _dyneither_ptr*)_check_null(_tmp7FF);_tmp7FB;});
f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9->hd=({struct _tuple9*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA->f1=_tmp7FF;_tmp7FA->f2=_tmp7FE;_tmp7FA->f3=_tmp7F7;_tmp7FA;});_tmp7F9->tl=0;_tmp7F9;}));
f->cyc_varargs=0;};}
# 4655
{struct Cyc_List_List*_tmp800=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp800 != 0;_tmp800=_tmp800->tl){
((struct Cyc_Absyn_Vardecl*)_tmp800->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp800->hd)->type);}}
# 4658
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp7F0),f->body);}
# 4635
;_pop_region(frgn);};}
# 4663
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp801=s;switch(_tmp801){case Cyc_Absyn_Abstract: _LL4CD: _LL4CE:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL4CF: _LL4D0:
 return Cyc_Absyn_Extern;default: _LL4D1: _LL4D2:
 return s;}_LL4CC:;}struct _tuple35{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple36{struct Cyc_Toc_TocState*f1;struct _tuple35*f2;};
# 4680 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple36*env){
# 4683
struct _tuple36 _tmp802=*env;struct _tuple36 _tmp803=_tmp802;struct Cyc_Toc_TocState*_tmp842;struct Cyc_Absyn_Aggrdecl*_tmp841;int _tmp840;_LL4D4: _tmp842=_tmp803.f1;_tmp841=(_tmp803.f2)->f1;_tmp840=(_tmp803.f2)->f2;_LL4D5:;{
struct _tuple1*_tmp804=_tmp841->name;
struct Cyc_Toc_TocState _tmp805=*_tmp842;struct Cyc_Toc_TocState _tmp806=_tmp805;struct Cyc_Dict_Dict*_tmp83F;_LL4D7: _tmp83F=_tmp806.aggrs_so_far;_LL4D8:;{
int seen_defn_before;
struct _tuple18**_tmp807=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp83F,_tmp804);
if(_tmp807 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp841->kind == Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp808=_region_malloc(d,sizeof(*_tmp808));_tmp808->f1=_tmp841;_tmp808->f2=Cyc_Absyn_strctq(_tmp804);_tmp808;});else{
# 4694
v=({struct _tuple18*_tmp809=_region_malloc(d,sizeof(*_tmp809));_tmp809->f1=_tmp841;_tmp809->f2=Cyc_Absyn_unionq_typ(_tmp804);_tmp809;});}
*_tmp83F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp83F,_tmp804,v);};}else{
# 4697
struct _tuple18*_tmp80A=*_tmp807;struct _tuple18*_tmp80B=_tmp80A;struct Cyc_Absyn_Aggrdecl*_tmp80E;void*_tmp80D;_LL4DA: _tmp80E=_tmp80B->f1;_tmp80D=_tmp80B->f2;_LL4DB:;
if(_tmp80E->impl == 0){
*_tmp83F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp83F,_tmp804,({struct _tuple18*_tmp80C=_region_malloc(d,sizeof(*_tmp80C));_tmp80C->f1=_tmp841;_tmp80C->f2=_tmp80D;_tmp80C;}));
seen_defn_before=0;}else{
# 4702
seen_defn_before=1;}}{
# 4704
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E->kind=_tmp841->kind;_tmp83E->sc=Cyc_Absyn_Public;_tmp83E->name=_tmp841->name;_tmp83E->tvs=0;_tmp83E->impl=0;_tmp83E->expected_mem_kind=0;_tmp83E->attributes=_tmp841->attributes;_tmp83E;});
# 4711
if(_tmp841->impl != 0  && !seen_defn_before){
new_ad->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp80F=_cycalloc(sizeof(*_tmp80F));_tmp80F->exist_vars=0;_tmp80F->rgn_po=0;_tmp80F->fields=0;_tmp80F->tagged=0;_tmp80F;});{
# 4716
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp810=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp841->impl))->fields;for(0;_tmp810 != 0;_tmp810=_tmp810->tl){
# 4720
struct Cyc_Absyn_Aggrfield*_tmp811=(struct Cyc_Absyn_Aggrfield*)_tmp810->hd;
void*_tmp812=_tmp811->type;
struct Cyc_List_List*_tmp813=_tmp811->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp812)) && (
_tmp841->kind == Cyc_Absyn_StructA  && _tmp810->tl == 0  || _tmp841->kind == Cyc_Absyn_UnionA)){
# 4734 "toc.cyc"
void*_tmp814=Cyc_Tcutil_compress(_tmp812);void*_tmp815=_tmp814;void*_tmp822;struct Cyc_Absyn_Tqual _tmp821;union Cyc_Absyn_Constraint*_tmp820;unsigned int _tmp81F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp815)->tag == 8){_LL4DD: _tmp822=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp815)->f1).elt_type;_tmp821=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp815)->f1).tq;_tmp820=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp815)->f1).zero_term;_tmp81F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp815)->f1).zt_loc;_LL4DE:
# 4737
 _tmp812=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp817;_tmp817.tag=8;_tmp817.f1=({struct Cyc_Absyn_ArrayInfo _tmp818;_tmp818.elt_type=_tmp822;_tmp818.tq=_tmp821;_tmp818.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp818.zero_term=_tmp820;_tmp818.zt_loc=_tmp81F;_tmp818;});_tmp817;});_tmp816;});
goto _LL4DC;}else{_LL4DF: _LL4E0:
# 4740
 _tmp813=({struct Cyc_List_List*_tmp819=_cycalloc(sizeof(*_tmp819));_tmp819->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp81B;_tmp81B.tag=6;_tmp81B.f1=0;_tmp81B;});_tmp81A;});_tmp819->tl=_tmp813;_tmp819;});
_tmp812=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp81D;_tmp81D.tag=8;_tmp81D.f1=({struct Cyc_Absyn_ArrayInfo _tmp81E;_tmp81E.elt_type=Cyc_Absyn_void_star_typ();_tmp81E.tq=
Cyc_Absyn_empty_tqual(0);_tmp81E.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp81E.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp81E.zt_loc=0;_tmp81E;});_tmp81D;});_tmp81C;});}_LL4DC:;}{
# 4747
struct Cyc_Absyn_Aggrfield*_tmp823=({struct Cyc_Absyn_Aggrfield*_tmp839=_cycalloc(sizeof(*_tmp839));_tmp839->name=_tmp811->name;_tmp839->tq=Cyc_Toc_mt_tq;_tmp839->type=
# 4749
Cyc_Toc_typ_to_c(_tmp812);_tmp839->width=_tmp811->width;_tmp839->attributes=_tmp813;_tmp839->requires_clause=0;_tmp839;});
# 4757
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp841->impl))->tagged){
void*_tmp824=_tmp823->type;
struct _dyneither_ptr*_tmp825=_tmp823->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp837;_tmp837.tag=0;_tmp837.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp825);({struct Cyc_String_pa_PrintArg_struct _tmp836;_tmp836.tag=0;_tmp836.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp841->name).f2);({void*_tmp834[2]={& _tmp836,& _tmp837};Cyc_aprintf(({const char*_tmp835="_union_%s_%s";_tag_dyneither(_tmp835,sizeof(char),13);}),_tag_dyneither(_tmp834,sizeof(void*),2));});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp833=_cycalloc(sizeof(*_tmp833));_tmp833[0]=s;_tmp833;});
struct Cyc_Absyn_Aggrfield*_tmp826=({struct Cyc_Absyn_Aggrfield*_tmp832=_cycalloc(sizeof(*_tmp832));_tmp832->name=Cyc_Toc_val_sp;_tmp832->tq=Cyc_Toc_mt_tq;_tmp832->type=_tmp824;_tmp832->width=0;_tmp832->attributes=0;_tmp832->requires_clause=0;_tmp832;});
struct Cyc_Absyn_Aggrfield*_tmp827=({struct Cyc_Absyn_Aggrfield*_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831->name=Cyc_Toc_tag_sp;_tmp831->tq=Cyc_Toc_mt_tq;_tmp831->type=Cyc_Absyn_sint_typ;_tmp831->width=0;_tmp831->attributes=0;_tmp831->requires_clause=0;_tmp831;});
struct Cyc_List_List*_tmp828=({struct Cyc_Absyn_Aggrfield*_tmp830[2];_tmp830[1]=_tmp826;_tmp830[0]=_tmp827;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp830,sizeof(struct Cyc_Absyn_Aggrfield*),2));});
struct Cyc_Absyn_Aggrdecl*_tmp829=({struct Cyc_Absyn_Aggrdecl*_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D->kind=Cyc_Absyn_StructA;_tmp82D->sc=Cyc_Absyn_Public;_tmp82D->name=({struct _tuple1*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F->f1=Cyc_Absyn_Loc_n;_tmp82F->f2=str;_tmp82F;});_tmp82D->tvs=0;_tmp82D->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E->exist_vars=0;_tmp82E->rgn_po=0;_tmp82E->fields=_tmp828;_tmp82E->tagged=0;_tmp82E;});_tmp82D->expected_mem_kind=0;_tmp82D->attributes=0;_tmp82D;});
# 4770
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp82A=_cycalloc(sizeof(*_tmp82A));_tmp82A->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp82C;_tmp82C.tag=5;_tmp82C.f1=_tmp829;_tmp82C;});_tmp82B;}),0);_tmp82A->tl=Cyc_Toc_result_decls;_tmp82A;});
_tmp823->type=Cyc_Absyn_strct(str);}
# 4773
new_fields=({struct Cyc_List_List*_tmp838=_cycalloc(sizeof(*_tmp838));_tmp838->hd=_tmp823;_tmp838->tl=new_fields;_tmp838;});};}}
# 4775
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4777
if(_tmp840)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A->hd=({struct Cyc_Absyn_Decl*_tmp83B=_cycalloc(sizeof(*_tmp83B));_tmp83B->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp83D;_tmp83D.tag=5;_tmp83D.f1=new_ad;_tmp83D;});_tmp83C;});_tmp83B->loc=0;_tmp83B;});_tmp83A->tl=Cyc_Toc_result_decls;_tmp83A;});
return 0;};};};}
# 4782
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple35 p=({struct _tuple35 _tmp843;_tmp843.f1=ad;_tmp843.f2=add_to_decls;_tmp843;});
((int(*)(struct _tuple35*arg,int(*f)(struct _RegionHandle*,struct _tuple36*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple37{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4811 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4814
struct _tuple37 _tmp844=*env;struct _tuple37 _tmp845=_tmp844;struct Cyc_Set_Set**_tmp857;struct Cyc_Absyn_Datatypedecl*_tmp856;_LL4E2: _tmp857=(_tmp845.f1)->datatypes_so_far;_tmp856=_tmp845.f2;_LL4E3:;{
struct _tuple1*_tmp846=_tmp856->name;
if(_tmp856->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp857,_tmp846))
return 0;
*_tmp857=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp857,_tmp846);
{struct Cyc_List_List*_tmp847=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp856->fields))->v;for(0;_tmp847 != 0;_tmp847=_tmp847->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp847->hd;
# 4822
struct Cyc_List_List*_tmp848=0;
int i=1;
{struct Cyc_List_List*_tmp849=f->typs;for(0;_tmp849 != 0;(_tmp849=_tmp849->tl,i ++)){
struct _dyneither_ptr*_tmp84A=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp84B=({struct Cyc_Absyn_Aggrfield*_tmp84D=_cycalloc(sizeof(*_tmp84D));_tmp84D->name=_tmp84A;_tmp84D->tq=(*((struct _tuple11*)_tmp849->hd)).f1;_tmp84D->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp849->hd)).f2);_tmp84D->width=0;_tmp84D->attributes=0;_tmp84D->requires_clause=0;_tmp84D;});
_tmp848=({struct Cyc_List_List*_tmp84C=_cycalloc(sizeof(*_tmp84C));_tmp84C->hd=_tmp84B;_tmp84C->tl=_tmp848;_tmp84C;});}}
# 4830
_tmp848=({struct Cyc_List_List*_tmp84E=_cycalloc(sizeof(*_tmp84E));_tmp84E->hd=({struct Cyc_Absyn_Aggrfield*_tmp84F=_cycalloc(sizeof(*_tmp84F));_tmp84F->name=Cyc_Toc_tag_sp;_tmp84F->tq=Cyc_Toc_mt_tq;_tmp84F->type=Cyc_Absyn_sint_typ;_tmp84F->width=0;_tmp84F->attributes=0;_tmp84F->requires_clause=0;_tmp84F;});_tmp84E->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp848);_tmp84E;});{
struct Cyc_Absyn_Aggrdecl*_tmp850=({struct Cyc_Absyn_Aggrdecl*_tmp854=_cycalloc(sizeof(*_tmp854));_tmp854->kind=Cyc_Absyn_StructA;_tmp854->sc=Cyc_Absyn_Public;_tmp854->name=
# 4834
Cyc_Toc_collapse_qvars(f->name,_tmp856->name);_tmp854->tvs=0;_tmp854->expected_mem_kind=0;_tmp854->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp855=_cycalloc(sizeof(*_tmp855));_tmp855->exist_vars=0;_tmp855->rgn_po=0;_tmp855->fields=_tmp848;_tmp855->tagged=0;_tmp855;});_tmp854->attributes=0;_tmp854;});
# 4838
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp851=_cycalloc(sizeof(*_tmp851));_tmp851->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp852=_cycalloc(sizeof(*_tmp852));_tmp852[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp853;_tmp853.tag=5;_tmp853.f1=_tmp850;_tmp853;});_tmp852;}),0);_tmp851->tl=Cyc_Toc_result_decls;_tmp851;});};}}
# 4840
return 0;};}
# 4843
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4862 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4865
struct _tuple37 _tmp858=*env;struct _tuple37 _tmp859=_tmp858;struct Cyc_Toc_TocState*_tmp87E;struct Cyc_Absyn_Datatypedecl*_tmp87D;_LL4E5: _tmp87E=_tmp859.f1;_tmp87D=_tmp859.f2;_LL4E6:;
if(_tmp87D->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp85A=*_tmp87E;struct Cyc_Toc_TocState _tmp85B=_tmp85A;struct Cyc_Dict_Dict*_tmp87C;_LL4E8: _tmp87C=_tmp85B.xdatatypes_so_far;_LL4E9:;{
struct _tuple1*_tmp85C=_tmp87D->name;
{struct Cyc_List_List*_tmp85D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp87D->fields))->v;for(0;_tmp85D != 0;_tmp85D=_tmp85D->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp85D->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp85E=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp85F=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp85E,Cyc_Absyn_false_conref,0);
# 4876
int*_tmp860=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp87C,f->name);int*_tmp861=_tmp860;if(_tmp861 == 0){_LL4EB: _LL4EC: {
# 4878
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4882
struct Cyc_Absyn_Vardecl*_tmp862=Cyc_Absyn_new_vardecl(f->name,_tmp85F,initopt);
_tmp862->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp863=_cycalloc(sizeof(*_tmp863));_tmp863->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp864=_cycalloc(sizeof(*_tmp864));_tmp864[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp865;_tmp865.tag=0;_tmp865.f1=_tmp862;_tmp865;});_tmp864;}),0);_tmp863->tl=Cyc_Toc_result_decls;_tmp863;});
*_tmp87C=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp87C,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp866=f->typs;for(0;_tmp866 != 0;(_tmp866=_tmp866->tl,i ++)){
struct _dyneither_ptr*_tmp867=({struct _dyneither_ptr*_tmp86B=_cycalloc(sizeof(*_tmp86B));_tmp86B[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp86E;_tmp86E.tag=1;_tmp86E.f1=(unsigned long)i;({void*_tmp86C[1]={& _tmp86E};Cyc_aprintf(({const char*_tmp86D="f%d";_tag_dyneither(_tmp86D,sizeof(char),4);}),_tag_dyneither(_tmp86C,sizeof(void*),1));});});_tmp86B;});
struct Cyc_Absyn_Aggrfield*_tmp868=({struct Cyc_Absyn_Aggrfield*_tmp86A=_cycalloc(sizeof(*_tmp86A));_tmp86A->name=_tmp867;_tmp86A->tq=(*((struct _tuple11*)_tmp866->hd)).f1;_tmp86A->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp866->hd)).f2);_tmp86A->width=0;_tmp86A->attributes=0;_tmp86A->requires_clause=0;_tmp86A;});
fields=({struct Cyc_List_List*_tmp869=_cycalloc(sizeof(*_tmp869));_tmp869->hd=_tmp868;_tmp869->tl=fields;_tmp869;});}}
# 4895
fields=({struct Cyc_List_List*_tmp86F=_cycalloc(sizeof(*_tmp86F));_tmp86F->hd=({struct Cyc_Absyn_Aggrfield*_tmp870=_cycalloc(sizeof(*_tmp870));_tmp870->name=Cyc_Toc_tag_sp;_tmp870->tq=Cyc_Toc_mt_tq;_tmp870->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp870->width=0;_tmp870->attributes=0;_tmp870->requires_clause=0;_tmp870;});_tmp86F->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp86F;});{
struct Cyc_Absyn_Aggrdecl*_tmp871=({struct Cyc_Absyn_Aggrdecl*_tmp875=_cycalloc(sizeof(*_tmp875));_tmp875->kind=Cyc_Absyn_StructA;_tmp875->sc=Cyc_Absyn_Public;_tmp875->name=
# 4900
Cyc_Toc_collapse_qvars(f->name,_tmp87D->name);_tmp875->tvs=0;_tmp875->expected_mem_kind=0;_tmp875->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp876=_cycalloc(sizeof(*_tmp876));_tmp876->exist_vars=0;_tmp876->rgn_po=0;_tmp876->fields=fields;_tmp876->tagged=0;_tmp876;});_tmp875->attributes=0;_tmp875;});
# 4904
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp872=_cycalloc(sizeof(*_tmp872));_tmp872->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp873=_cycalloc(sizeof(*_tmp873));_tmp873[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp874;_tmp874.tag=5;_tmp874.f1=_tmp871;_tmp874;});_tmp873;}),0);_tmp872->tl=Cyc_Toc_result_decls;_tmp872;});
# 4906
goto _LL4EA;};};};}}else{if(*((int*)_tmp861)== 0){_LL4ED: _LL4EE:
# 4908
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp877=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp878=Cyc_Absyn_new_vardecl(f->name,_tmp85F,_tmp877);
_tmp878->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp879=_cycalloc(sizeof(*_tmp879));_tmp879->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp87A=_cycalloc(sizeof(*_tmp87A));_tmp87A[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp87B;_tmp87B.tag=0;_tmp87B.f1=_tmp878;_tmp87B;});_tmp87A;}),0);_tmp879->tl=Cyc_Toc_result_decls;_tmp879;});
*_tmp87C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp87C,f->name,1);}
# 4915
goto _LL4EA;}else{_LL4EF: _LL4F0:
 goto _LL4EA;}}_LL4EA:;}}
# 4919
return 0;};};}
# 4922
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4926
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4932
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4936
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp87F=init->r;void*_tmp880=_tmp87F;struct Cyc_Absyn_Vardecl*_tmp886;struct Cyc_Absyn_Exp*_tmp885;struct Cyc_Absyn_Exp*_tmp884;int _tmp883;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp880)->tag == 26){_LL4F2: _tmp886=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp880)->f1;_tmp885=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp880)->f2;_tmp884=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp880)->f3;_tmp883=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp880)->f4;_LL4F3:
# 4943
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp886,_tmp885,_tmp884,_tmp883,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL4F1;}else{_LL4F4: _LL4F5:
# 4948
 if(vd->sc == Cyc_Absyn_Static){
# 4952
struct _RegionHandle _tmp881=_new_region("temp");struct _RegionHandle*temp=& _tmp881;_push_region(temp);
{struct Cyc_Toc_Env*_tmp882=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp882,init);}
# 4953
;_pop_region(temp);}else{
# 4957
Cyc_Toc_exp_to_c(nv,init);}
goto _LL4F1;}_LL4F1:;}else{
# 4962
void*_tmp887=Cyc_Tcutil_compress(old_typ);void*_tmp888=_tmp887;void*_tmp88F;struct Cyc_Absyn_Exp*_tmp88E;union Cyc_Absyn_Constraint*_tmp88D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp888)->tag == 8){_LL4F7: _tmp88F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp888)->f1).elt_type;_tmp88E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp888)->f1).num_elts;_tmp88D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp888)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp88D)){_LL4F8:
 if(_tmp88E == 0)
({void*_tmp889=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp88A="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp88A,sizeof(char),55);}),_tag_dyneither(_tmp889,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*num_elts=_tmp88E;
struct Cyc_Absyn_Exp*_tmp88B=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4970
struct Cyc_Absyn_Exp*_tmp88C=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp88B,_tmp88C,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL4F6;};}else{goto _LL4F9;}}else{_LL4F9: _LL4FA:
 goto _LL4F6;}_LL4F6:;}}
# 4981
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp890=d;struct Cyc_List_List*_tmp897;struct Cyc_List_List*_tmp896;void**_tmp895;struct Cyc_Tcpat_Rhs*_tmp894;switch(*((int*)_tmp890)){case 0: _LL4FC: _LL4FD:
 return d;case 1: _LL4FE: _tmp894=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp890)->f1;_LL4FF:
 _tmp894->rhs=success;return d;default: _LL500: _tmp897=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp890)->f1;_tmp896=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp890)->f2;_tmp895=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp890)->f3;_LL501:
# 4986
*_tmp895=Cyc_Toc_rewrite_decision(*_tmp895,success);
for(0;_tmp896 != 0;_tmp896=_tmp896->tl){
struct _tuple29*_tmp891=(struct _tuple29*)_tmp896->hd;struct _tuple29*_tmp892=_tmp891;void**_tmp893;_LL503: _tmp893=(void**)& _tmp892->f2;_LL504:;
*_tmp893=Cyc_Toc_rewrite_decision(*_tmp893,success);}
# 4991
return d;}_LL4FB:;}
# 5002 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 5004
struct _RegionHandle _tmp898=_new_region("rgn");struct _RegionHandle*rgn=& _tmp898;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp899=Cyc_Toc_share_env(rgn,nv);
void*_tmp89A=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp899,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 5014
struct Cyc_Absyn_Stmt*_tmp89B=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 5018
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp89B);{
# 5020
struct Cyc_Absyn_Switch_clause*_tmp89C=({struct Cyc_Absyn_Switch_clause*_tmp8A9=_cycalloc(sizeof(*_tmp8A9));_tmp8A9->pattern=p;_tmp8A9->pat_vars=0;_tmp8A9->where_clause=0;_tmp8A9->body=_tmp89B;_tmp8A9->loc=0;_tmp8A9;});
struct Cyc_List_List*_tmp89D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,({struct Cyc_Absyn_Switch_clause*_tmp8A8[1];_tmp8A8[0]=_tmp89C;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(rgn,_tag_dyneither(_tmp8A8,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));
# 5023
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 5027
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp899,& mydecls,& mybodies,dopt,_tmp89D,v);
# 5030
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple34*_tmp89E=(struct _tuple34*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple34*_tmp89F=_tmp89E;struct Cyc_Toc_Env*_tmp8A1;struct Cyc_Absyn_Stmt*_tmp8A0;_LL506: _tmp8A1=_tmp89F->f1;_tmp8A0=_tmp89F->f3;_LL507:;
if(_tmp8A0 == _tmp89B){senv=_tmp8A1;goto FOUND_ENV;}}
# 5035
({void*_tmp8A2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp8A3="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp8A3,sizeof(char),33);}),_tag_dyneither(_tmp8A2,sizeof(void*),0));});
FOUND_ENV:
# 5039
 Cyc_Toc_stmt_to_c(senv,s);{
# 5041
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0);
# 5043
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp8A4=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
_tmp8A4->name=Cyc_Toc_temp_var();
_tmp8A4->type=Cyc_Toc_typ_to_c_array(_tmp8A4->type);
res=Cyc_Absyn_decl_stmt(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8A5=_cycalloc(sizeof(*_tmp8A5));_tmp8A5[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp8A6;_tmp8A6.tag=0;_tmp8A6.f1=_tmp8A4;_tmp8A6;});_tmp8A5;}),0),res,0);}
# 5050
res=Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c(_tmp89A),e,res,0);{
struct Cyc_Absyn_Stmt*_tmp8A7=res;_npop_handler(0);return _tmp8A7;};};};};}
# 5005
;_pop_region(rgn);}
# 5058
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8AA=e->r;void*_tmp8AB=_tmp8AA;struct Cyc_Absyn_MallocInfo*_tmp8D5;struct Cyc_Absyn_Stmt*_tmp8D4;void**_tmp8D3;void**_tmp8D2;struct Cyc_List_List*_tmp8D1;struct Cyc_List_List*_tmp8D0;void**_tmp8CF;struct Cyc_List_List*_tmp8CE;void**_tmp8CD;struct Cyc_Absyn_Exp*_tmp8CC;struct Cyc_Absyn_Exp*_tmp8CB;struct Cyc_List_List*_tmp8CA;struct Cyc_Absyn_Exp*_tmp8C9;struct Cyc_Absyn_Exp*_tmp8C8;struct Cyc_Absyn_Exp*_tmp8C7;struct Cyc_Absyn_Exp*_tmp8C6;struct Cyc_Absyn_Exp*_tmp8C5;struct Cyc_Absyn_Exp*_tmp8C4;struct Cyc_Absyn_Exp*_tmp8C3;struct Cyc_Absyn_Exp*_tmp8C2;struct Cyc_Absyn_Exp*_tmp8C1;struct Cyc_Absyn_Exp*_tmp8C0;struct Cyc_Absyn_Exp*_tmp8BF;struct Cyc_Absyn_Exp*_tmp8BE;struct Cyc_Absyn_Exp*_tmp8BD;struct Cyc_Absyn_Exp*_tmp8BC;struct Cyc_Absyn_Exp*_tmp8BB;struct Cyc_List_List*_tmp8BA;struct Cyc_Absyn_Exp*_tmp8B9;struct Cyc_Absyn_Exp*_tmp8B8;struct Cyc_Absyn_Exp*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_Absyn_Exp*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B4;struct Cyc_Absyn_Exp*_tmp8B3;struct Cyc_Absyn_Exp*_tmp8B2;switch(*((int*)_tmp8AB)){case 19: _LL509: _tmp8B2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL50A:
 _tmp8B3=_tmp8B2;goto _LL50C;case 20: _LL50B: _tmp8B3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL50C:
 _tmp8B4=_tmp8B3;goto _LL50E;case 21: _LL50D: _tmp8B4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL50E:
 _tmp8B5=_tmp8B4;goto _LL510;case 14: _LL50F: _tmp8B5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL510:
 _tmp8B6=_tmp8B5;goto _LL512;case 10: _LL511: _tmp8B6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL512:
 _tmp8B7=_tmp8B6;goto _LL514;case 11: _LL513: _tmp8B7=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL514:
 _tmp8B8=_tmp8B7;goto _LL516;case 17: _LL515: _tmp8B8=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL516:
 _tmp8B9=_tmp8B8;goto _LL518;case 4: _LL517: _tmp8B9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL518:
 Cyc_Toc_exptypes_to_c(_tmp8B9);goto _LL508;case 2: _LL519: _tmp8BA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_LL51A:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8BA);goto _LL508;case 6: _LL51B: _tmp8BC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_tmp8BB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_LL51C:
 _tmp8BE=_tmp8BC;_tmp8BD=_tmp8BB;goto _LL51E;case 7: _LL51D: _tmp8BE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_tmp8BD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_LL51E:
 _tmp8C0=_tmp8BE;_tmp8BF=_tmp8BD;goto _LL520;case 8: _LL51F: _tmp8C0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_tmp8BF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_LL520:
 _tmp8C2=_tmp8C0;_tmp8C1=_tmp8BF;goto _LL522;case 22: _LL521: _tmp8C2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_tmp8C1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_LL522:
 _tmp8C4=_tmp8C2;_tmp8C3=_tmp8C1;goto _LL524;case 34: _LL523: _tmp8C4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_tmp8C3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_LL524:
 _tmp8C6=_tmp8C4;_tmp8C5=_tmp8C3;goto _LL526;case 3: _LL525: _tmp8C6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_tmp8C5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8AB)->f3;_LL526:
 Cyc_Toc_exptypes_to_c(_tmp8C6);Cyc_Toc_exptypes_to_c(_tmp8C5);goto _LL508;case 5: _LL527: _tmp8C9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_tmp8C8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_tmp8C7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8AB)->f3;_LL528:
# 5076
 Cyc_Toc_exptypes_to_c(_tmp8C9);Cyc_Toc_exptypes_to_c(_tmp8C8);Cyc_Toc_exptypes_to_c(_tmp8C7);goto _LL508;case 9: _LL529: _tmp8CB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_tmp8CA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_LL52A:
# 5078
 Cyc_Toc_exptypes_to_c(_tmp8CB);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8CA);goto _LL508;case 13: _LL52B: _tmp8CD=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_tmp8CC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_LL52C:
*_tmp8CD=Cyc_Toc_typ_to_c(*_tmp8CD);Cyc_Toc_exptypes_to_c(_tmp8CC);goto _LL508;case 24: _LL52D: _tmp8CF=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1)->f3;_tmp8CE=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_LL52E:
# 5081
*_tmp8CF=Cyc_Toc_typ_to_c(*_tmp8CF);
_tmp8D0=_tmp8CE;goto _LL530;case 35: _LL52F: _tmp8D0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8AB)->f2;_LL530:
 _tmp8D1=_tmp8D0;goto _LL532;case 25: _LL531: _tmp8D1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL532:
# 5085
 for(0;_tmp8D1 != 0;_tmp8D1=_tmp8D1->tl){
struct _tuple19 _tmp8AC=*((struct _tuple19*)_tmp8D1->hd);struct _tuple19 _tmp8AD=_tmp8AC;struct Cyc_Absyn_Exp*_tmp8AE;_LL55A: _tmp8AE=_tmp8AD.f2;_LL55B:;
Cyc_Toc_exptypes_to_c(_tmp8AE);}
# 5089
goto _LL508;case 18: _LL533: _tmp8D2=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL534:
 _tmp8D3=_tmp8D2;goto _LL536;case 16: _LL535: _tmp8D3=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL536:
*_tmp8D3=Cyc_Toc_typ_to_c(*_tmp8D3);goto _LL508;case 36: _LL537: _tmp8D4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL538:
 Cyc_Toc_stmttypes_to_c(_tmp8D4);goto _LL508;case 33: _LL539: _tmp8D5=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8AB)->f1;_LL53A:
# 5094
 if(_tmp8D5->elt_type != 0)
_tmp8D5->elt_type=({void**_tmp8AF=_cycalloc(sizeof(*_tmp8AF));_tmp8AF[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8D5->elt_type)));_tmp8AF;});
Cyc_Toc_exptypes_to_c(_tmp8D5->num_elts);
goto _LL508;case 0: _LL53B: _LL53C:
 goto _LL53E;case 1: _LL53D: _LL53E:
 goto _LL540;case 31: _LL53F: _LL540:
 goto _LL542;case 39: _LL541: _LL542:
 goto _LL544;case 32: _LL543: _LL544:
 goto _LL508;case 29: _LL545: _LL546:
# 5104
 goto _LL548;case 30: _LL547: _LL548:
 goto _LL54A;case 28: _LL549: _LL54A:
 goto _LL54C;case 26: _LL54B: _LL54C:
 goto _LL54E;case 27: _LL54D: _LL54E:
 goto _LL550;case 23: _LL54F: _LL550:
 goto _LL552;case 12: _LL551: _LL552:
 goto _LL554;case 15: _LL553: _LL554:
 goto _LL556;case 38: _LL555: _LL556:
 goto _LL558;default: _LL557: _LL558:
# 5114
({void*_tmp8B0=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp8B1="Cyclone expression within C code";_tag_dyneither(_tmp8B1,sizeof(char),33);}),_tag_dyneither(_tmp8B0,sizeof(void*),0));});
goto _LL508;}_LL508:;}
# 5119
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp8D6=d->r;void*_tmp8D7=_tmp8D6;struct Cyc_Absyn_Typedefdecl*_tmp8E1;struct Cyc_Absyn_Enumdecl*_tmp8E0;struct Cyc_Absyn_Aggrdecl*_tmp8DF;struct Cyc_Absyn_Fndecl*_tmp8DE;struct Cyc_Absyn_Vardecl*_tmp8DD;switch(*((int*)_tmp8D7)){case 0: _LL55D: _tmp8DD=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8D7)->f1;_LL55E:
# 5122
 _tmp8DD->type=Cyc_Toc_typ_to_c(_tmp8DD->type);
if(_tmp8DD->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8DD->initializer));
goto _LL55C;case 1: _LL55F: _tmp8DE=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8D7)->f1;_LL560:
# 5126
 _tmp8DE->ret_type=Cyc_Toc_typ_to_c(_tmp8DE->ret_type);
{struct Cyc_List_List*_tmp8D8=_tmp8DE->args;for(0;_tmp8D8 != 0;_tmp8D8=_tmp8D8->tl){
(*((struct _tuple9*)_tmp8D8->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8D8->hd)).f3);}}
# 5130
goto _LL55C;case 5: _LL561: _tmp8DF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8D7)->f1;_LL562:
 Cyc_Toc_aggrdecl_to_c(_tmp8DF,1);goto _LL55C;case 7: _LL563: _tmp8E0=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8D7)->f1;_LL564:
# 5133
 if(_tmp8E0->fields != 0){
struct Cyc_List_List*_tmp8D9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8E0->fields))->v;for(0;_tmp8D9 != 0;_tmp8D9=_tmp8D9->tl){
struct Cyc_Absyn_Enumfield*_tmp8DA=(struct Cyc_Absyn_Enumfield*)_tmp8D9->hd;
if(_tmp8DA->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8DA->tag));}}
# 5138
goto _LL55C;case 8: _LL565: _tmp8E1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8D7)->f1;_LL566:
 _tmp8E1->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8E1->defn));goto _LL55C;case 2: _LL567: _LL568:
 goto _LL56A;case 3: _LL569: _LL56A:
 goto _LL56C;case 6: _LL56B: _LL56C:
 goto _LL56E;case 9: _LL56D: _LL56E:
 goto _LL570;case 10: _LL56F: _LL570:
 goto _LL572;case 11: _LL571: _LL572:
 goto _LL574;case 12: _LL573: _LL574:
 goto _LL576;case 4: _LL575: _LL576:
# 5149
({void*_tmp8DB=0;Cyc_Tcutil_terr(d->loc,({const char*_tmp8DC="Cyclone declaration within C code";_tag_dyneither(_tmp8DC,sizeof(char),34);}),_tag_dyneither(_tmp8DB,sizeof(void*),0));});
goto _LL55C;case 13: _LL577: _LL578:
 goto _LL57A;default: _LL579: _LL57A:
# 5153
 goto _LL55C;}_LL55C:;}
# 5157
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp8E2=s->r;void*_tmp8E3=_tmp8E2;struct Cyc_Absyn_Stmt*_tmp8FE;struct Cyc_Absyn_Stmt*_tmp8FD;struct Cyc_Absyn_Exp*_tmp8FC;struct Cyc_Absyn_Decl*_tmp8FB;struct Cyc_Absyn_Stmt*_tmp8FA;struct Cyc_Absyn_Exp*_tmp8F9;struct Cyc_List_List*_tmp8F8;void*_tmp8F7;struct Cyc_Absyn_Exp*_tmp8F6;struct Cyc_Absyn_Exp*_tmp8F5;struct Cyc_Absyn_Exp*_tmp8F4;struct Cyc_Absyn_Stmt*_tmp8F3;struct Cyc_Absyn_Exp*_tmp8F2;struct Cyc_Absyn_Stmt*_tmp8F1;struct Cyc_Absyn_Exp*_tmp8F0;struct Cyc_Absyn_Stmt*_tmp8EF;struct Cyc_Absyn_Stmt*_tmp8EE;struct Cyc_Absyn_Exp*_tmp8ED;struct Cyc_Absyn_Stmt*_tmp8EC;struct Cyc_Absyn_Stmt*_tmp8EB;struct Cyc_Absyn_Exp*_tmp8EA;switch(*((int*)_tmp8E3)){case 1: _LL57C: _tmp8EA=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f1;_LL57D:
 Cyc_Toc_exptypes_to_c(_tmp8EA);goto _LL57B;case 2: _LL57E: _tmp8EC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f1;_tmp8EB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f2;_LL57F:
 Cyc_Toc_stmttypes_to_c(_tmp8EC);Cyc_Toc_stmttypes_to_c(_tmp8EB);goto _LL57B;case 3: _LL580: _tmp8ED=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f1;_LL581:
 if(_tmp8ED != 0)Cyc_Toc_exptypes_to_c(_tmp8ED);goto _LL57B;case 4: _LL582: _tmp8F0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f1;_tmp8EF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f2;_tmp8EE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f3;_LL583:
# 5163
 Cyc_Toc_exptypes_to_c(_tmp8F0);Cyc_Toc_stmttypes_to_c(_tmp8EF);Cyc_Toc_stmttypes_to_c(_tmp8EE);goto _LL57B;case 5: _LL584: _tmp8F2=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f1).f1;_tmp8F1=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f2;_LL585:
# 5165
 Cyc_Toc_exptypes_to_c(_tmp8F2);Cyc_Toc_stmttypes_to_c(_tmp8F1);goto _LL57B;case 9: _LL586: _tmp8F6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f1;_tmp8F5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f2).f1;_tmp8F4=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f3).f1;_tmp8F3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f4;_LL587:
# 5167
 Cyc_Toc_exptypes_to_c(_tmp8F6);Cyc_Toc_exptypes_to_c(_tmp8F5);Cyc_Toc_exptypes_to_c(_tmp8F4);
Cyc_Toc_stmttypes_to_c(_tmp8F3);goto _LL57B;case 10: _LL588: _tmp8F9=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f1;_tmp8F8=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f2;_tmp8F7=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f3;_LL589:
# 5170
 Cyc_Toc_exptypes_to_c(_tmp8F9);
for(0;_tmp8F8 != 0;_tmp8F8=_tmp8F8->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp8F8->hd)->body);}
goto _LL57B;case 12: _LL58A: _tmp8FB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f1;_tmp8FA=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f2;_LL58B:
 Cyc_Toc_decltypes_to_c(_tmp8FB);Cyc_Toc_stmttypes_to_c(_tmp8FA);goto _LL57B;case 14: _LL58C: _tmp8FD=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f1;_tmp8FC=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f2).f1;_LL58D:
 Cyc_Toc_stmttypes_to_c(_tmp8FD);Cyc_Toc_exptypes_to_c(_tmp8FC);goto _LL57B;case 13: _LL58E: _tmp8FE=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8E3)->f2;_LL58F:
 Cyc_Toc_stmttypes_to_c(_tmp8FE);goto _LL57B;case 0: _LL590: _LL591:
 goto _LL593;case 6: _LL592: _LL593:
 goto _LL595;case 7: _LL594: _LL595:
 goto _LL597;case 8: _LL596: _LL597:
# 5180
 goto _LL57B;case 11: _LL598: _LL599:
# 5183
 s->r=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp8E4=_cycalloc_atomic(sizeof(*_tmp8E4));_tmp8E4[0]=({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmp8E5;_tmp8E5.tag=0;_tmp8E5;});_tmp8E4;});
goto _LL57B;case 15: _LL59A: _LL59B:
 goto _LL59D;default: _LL59C: _LL59D:
# 5187
({void*_tmp8E6=0;Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8E9;_tmp8E9.tag=0;_tmp8E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp8E7[1]={& _tmp8E9};Cyc_aprintf(({const char*_tmp8E8="Cyclone statement in C code: %s";_tag_dyneither(_tmp8E8,sizeof(char),32);}),_tag_dyneither(_tmp8E7,sizeof(void*),1));});}),_tag_dyneither(_tmp8E6,sizeof(void*),0));});
goto _LL57B;}_LL57B:;}
# 5195
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp8FF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp900="decls_to_c: not at toplevel!";_tag_dyneither(_tmp900,sizeof(char),29);}),_tag_dyneither(_tmp8FF,sizeof(void*),0));});{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp901=d->r;void*_tmp902=_tmp901;struct Cyc_List_List*_tmp91A;struct Cyc_List_List*_tmp919;struct Cyc_List_List*_tmp918;struct Cyc_List_List*_tmp917;struct Cyc_Absyn_Typedefdecl*_tmp916;struct Cyc_Absyn_Enumdecl*_tmp915;struct Cyc_Absyn_Datatypedecl*_tmp914;struct Cyc_Absyn_Aggrdecl*_tmp913;struct Cyc_Absyn_Fndecl*_tmp912;struct Cyc_Absyn_Vardecl*_tmp911;switch(*((int*)_tmp902)){case 0: _LL59F: _tmp911=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp902)->f1;_LL5A0: {
# 5202
struct _tuple1*_tmp903=_tmp911->name;
# 5204
if(_tmp911->sc == Cyc_Absyn_ExternC)
_tmp903=({struct _tuple1*_tmp904=_cycalloc(sizeof(*_tmp904));_tmp904->f1=Cyc_Absyn_Abs_n(0,1);_tmp904->f2=(*_tmp903).f2;_tmp904;});
if(_tmp911->initializer != 0){
if(_tmp911->sc == Cyc_Absyn_ExternC)_tmp911->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp911->initializer));else{
# 5211
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp911->initializer));}}
# 5213
_tmp911->name=_tmp903;
_tmp911->sc=Cyc_Toc_scope_to_c(_tmp911->sc);
_tmp911->type=Cyc_Toc_typ_to_c(_tmp911->type);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp905=_cycalloc(sizeof(*_tmp905));_tmp905->hd=d;_tmp905->tl=Cyc_Toc_result_decls;_tmp905;});
goto _LL59E;}case 1: _LL5A1: _tmp912=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp902)->f1;_LL5A2: {
# 5219
struct _tuple1*_tmp906=_tmp912->name;
# 5221
if(_tmp912->sc == Cyc_Absyn_ExternC){
_tmp906=({struct _tuple1*_tmp907=_cycalloc(sizeof(*_tmp907));_tmp907->f1=Cyc_Absyn_Abs_n(0,1);_tmp907->f2=(*_tmp906).f2;_tmp907;});
_tmp912->sc=Cyc_Absyn_Public;}
# 5225
_tmp912->name=_tmp906;
Cyc_Toc_fndecl_to_c(nv,_tmp912,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp908=_cycalloc(sizeof(*_tmp908));_tmp908->hd=d;_tmp908->tl=Cyc_Toc_result_decls;_tmp908;});
goto _LL59E;}case 2: _LL5A3: _LL5A4:
 goto _LL5A6;case 3: _LL5A5: _LL5A6:
({void*_tmp909=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp90A="letdecl at toplevel";_tag_dyneither(_tmp90A,sizeof(char),20);}),_tag_dyneither(_tmp909,sizeof(void*),0));});case 4: _LL5A7: _LL5A8:
({void*_tmp90B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp90C="region decl at toplevel";_tag_dyneither(_tmp90C,sizeof(char),24);}),_tag_dyneither(_tmp90B,sizeof(void*),0));});case 5: _LL5A9: _tmp913=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp902)->f1;_LL5AA:
# 5233
 Cyc_Toc_aggrdecl_to_c(_tmp913,1);
goto _LL59E;case 6: _LL5AB: _tmp914=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp902)->f1;_LL5AC:
# 5236
 if(_tmp914->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp914);else{
# 5239
Cyc_Toc_datatypedecl_to_c(_tmp914);}
goto _LL59E;case 7: _LL5AD: _tmp915=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp902)->f1;_LL5AE:
# 5242
 Cyc_Toc_enumdecl_to_c(_tmp915);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp90D=_cycalloc(sizeof(*_tmp90D));_tmp90D->hd=d;_tmp90D->tl=Cyc_Toc_result_decls;_tmp90D;});
goto _LL59E;case 8: _LL5AF: _tmp916=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp902)->f1;_LL5B0:
# 5246
 _tmp916->tvs=0;
if(_tmp916->defn != 0)
_tmp916->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp916->defn));else{
# 5250
enum Cyc_Absyn_KindQual _tmp90E=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp916->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp90F=_tmp90E;if(_tmp90F == Cyc_Absyn_BoxKind){_LL5BE: _LL5BF:
 _tmp916->defn=Cyc_Absyn_void_star_typ();goto _LL5BD;}else{_LL5C0: _LL5C1:
 _tmp916->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL5BD;}_LL5BD:;}
# 5255
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp910=_cycalloc(sizeof(*_tmp910));_tmp910->hd=d;_tmp910->tl=Cyc_Toc_result_decls;_tmp910;});
goto _LL59E;case 13: _LL5B1: _LL5B2:
 goto _LL5B4;case 14: _LL5B3: _LL5B4:
# 5259
 goto _LL59E;case 9: _LL5B5: _tmp917=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp902)->f2;_LL5B6:
 _tmp918=_tmp917;goto _LL5B8;case 10: _LL5B7: _tmp918=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp902)->f2;_LL5B8:
 _tmp919=_tmp918;goto _LL5BA;case 11: _LL5B9: _tmp919=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp902)->f1;_LL5BA:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp919,top,cinclude);goto _LL59E;default: _LL5BB: _tmp91A=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp902)->f1;_LL5BC:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp91A,top,1);goto _LL59E;}_LL59E:;};}
# 5266
return nv;}
# 5270
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp91B=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp91C=_tmp91B;struct Cyc_Core_DynamicRegion*_tmp920;_LL5C3: _tmp920=_tmp91C.key;_LL5C4:;{
struct Cyc_Toc_TocState*_tmp91D=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp920,0,Cyc_Toc_empty_toc_state);
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp91E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp91E));_tmp91E->dyn=(struct Cyc_Core_DynamicRegion*)_tmp920;_tmp91E->state=(struct Cyc_Toc_TocState*)_tmp91D;_tmp91E;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;
Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmp91F=_cycalloc(sizeof(struct _dyneither_ptr*)* 42);_tmp91F[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str;_tmp91F[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str;_tmp91F[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str;_tmp91F[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str;_tmp91F[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str;_tmp91F[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str;_tmp91F[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str;_tmp91F[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str;_tmp91F[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str;_tmp91F[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str;_tmp91F[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str;_tmp91F[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str;_tmp91F[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str;_tmp91F[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str;_tmp91F[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str;_tmp91F[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str;_tmp91F[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str;_tmp91F[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str;_tmp91F[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp91F[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str;_tmp91F[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp91F[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp91F[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str;_tmp91F[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str;_tmp91F[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str;_tmp91F[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str;_tmp91F[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str;_tmp91F[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str;_tmp91F[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str;_tmp91F[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str;_tmp91F[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str;_tmp91F[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str;_tmp91F[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str;_tmp91F[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str;_tmp91F[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str;_tmp91F[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str;_tmp91F[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str;_tmp91F[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp91F[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str;_tmp91F[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str;_tmp91F[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str;_tmp91F[41]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str;_tmp91F;}),sizeof(struct _dyneither_ptr*),42);};}
# 5327
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp921=*ts;struct Cyc_Toc_TocState _tmp922=_tmp921;struct Cyc_Xarray_Xarray*_tmp923;_LL5C6: _tmp923=_tmp922.temp_labels;_LL5C7:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp923);
return 0;}
# 5335
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 5337
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp924=_cycalloc(sizeof(*_tmp924));_tmp924[0]=pop_tables;_tmp924;});
Cyc_Toc_init();
{struct _RegionHandle _tmp925=_new_region("start");struct _RegionHandle*start=& _tmp925;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);;_pop_region(start);}{
# 5347
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp926=ts;struct Cyc_Toc_TocStateWrap*_tmp927=Cyc_Toc_toc_state;ts=_tmp927;Cyc_Toc_toc_state=_tmp926;});{
struct Cyc_Toc_TocStateWrap _tmp928=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp929=_tmp928;struct Cyc_Core_DynamicRegion*_tmp92B;struct Cyc_Toc_TocState*_tmp92A;_LL5C9: _tmp92B=_tmp929.dyn;_tmp92A=_tmp929.state;_LL5CA:;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp92B,_tmp92A,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp92B);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 5354
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);};};}
