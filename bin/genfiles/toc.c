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
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);
# 251
void*Cyc_Core_open_region(struct Cyc_Core_DynamicRegion*key,void*arg,void*(*body)(struct _RegionHandle*h,void*arg));struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
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
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);
# 37 "position.h"
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
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
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
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
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
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
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1175
struct _tuple1*Cyc_Absyn_binding2qvar(void*);
# 1180
extern int Cyc_Absyn_no_regions;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 76
int Cyc_Absynpp_is_anon_aggrtype(void*t);struct Cyc_Set_Set;
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
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 34 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);
# 38
void*Cyc_Toc_typ_to_c(void*t);
void Cyc_Toc_finish();
# 42
struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 45
extern int Cyc_Toc_warn_bounds_checks;
extern int Cyc_Toc_warn_all_null_deref;struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 61 "toc.cyc"
extern int Cyc_noexpand_r;
# 71
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 73
({struct _dyneither_ptr _tmp91F=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp91E=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp91E,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp91F,ap);});}
# 75
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 77
({struct _dyneither_ptr _tmp921=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp920=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp920,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp921,ap);});}
# 81
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0U;
unsigned int Cyc_Toc_bounds_checks_eliminated=0U;
# 86
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 89
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 99
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 101
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 124
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp2=*x;struct _tuple14 _tmp3=_tmp2;struct _tuple1*_tmp9;struct _tuple1*_tmp8;_LL1: _tmp9=_tmp3.f1;_tmp8=_tmp3.f2;_LL2:;{
struct _tuple14 _tmp4=*y;struct _tuple14 _tmp5=_tmp4;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL4: _tmp7=_tmp5.f1;_tmp6=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp9,_tmp7);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp8,_tmp6);};};}
# 133
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d,int unused){
return({struct Cyc_Toc_TocState*_tmpA=_region_malloc(d,sizeof(*_tmpA));({struct Cyc_List_List**_tmp922=({struct Cyc_List_List**_tmp10=_region_malloc(d,sizeof(*_tmp10));_tmp10[0]=0;_tmp10;});_tmpA->tuple_types=_tmp922;});({struct Cyc_Dict_Dict*_tmp924=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp923=
# 136
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpF[0]=_tmp923;});_tmpF;});_tmpA->aggrs_so_far=_tmp924;});({struct Cyc_List_List**_tmp925=({struct Cyc_List_List**_tmpE=_region_malloc(d,sizeof(*_tmpE));_tmpE[0]=0;_tmpE;});_tmpA->abs_struct_types=_tmp925;});({struct Cyc_Set_Set**_tmp927=({struct Cyc_Set_Set**_tmpD=_region_malloc(d,sizeof(*_tmpD));({struct Cyc_Set_Set*_tmp926=
# 138
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpD[0]=_tmp926;});_tmpD;});_tmpA->datatypes_so_far=_tmp927;});({struct Cyc_Dict_Dict*_tmp929=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp928=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpC[0]=_tmp928;});_tmpC;});_tmpA->xdatatypes_so_far=_tmp929;});({struct Cyc_Dict_Dict*_tmp92B=({struct Cyc_Dict_Dict*_tmpB=_region_malloc(d,sizeof(*_tmpB));({struct Cyc_Dict_Dict _tmp92A=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmpB[0]=_tmp92A;});_tmpB;});_tmpA->qvar_tags=_tmp92B;});({struct Cyc_Xarray_Xarray*_tmp92C=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmpA->temp_labels=_tmp92C;});_tmpA;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 151
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 157
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 160
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp11=ts;struct Cyc_Toc_TocStateWrap*_tmp12=Cyc_Toc_toc_state;ts=_tmp12;Cyc_Toc_toc_state=_tmp11;});{
struct Cyc_Toc_TocStateWrap _tmp13=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp14=_tmp13;struct Cyc_Core_DynamicRegion*_tmp1D;struct Cyc_Toc_TocState*_tmp1C;_LL1: _tmp1D=_tmp14.dyn;_tmp1C=_tmp14.state;_LL2:;{
struct _dyneither_ptr _tmp15=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1D,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp92D=({struct Cyc_Toc_TocStateWrap _tmp16;_tmp16.dyn=(struct Cyc_Core_DynamicRegion*)_tmp1D;_tmp16.state=(struct Cyc_Toc_TocState*)_tmp1C;_tmp16;});*ts=_tmp92D;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
struct _tuple15 _tmp19=({struct _tuple15 _tmp1B;_tmp1B.f1=_tmp1C;_tmp1B.f2=arg;_tmp1B;});
void*_tmp1A=((void*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple15*arg,void*(*body)(struct _RegionHandle*h,struct _tuple15*arg)))Cyc_Core_open_region)((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp15,sizeof(struct Cyc_Core_DynamicRegion),1U)),& _tmp19,f);
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp15,sizeof(struct Cyc_Core_DynamicRegion),1U)));
return _tmp1A;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 172
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 175
struct _tuple17 _tmp1E=*env;struct _tuple17 _tmp1F=_tmp1E;struct Cyc_Toc_TocState*_tmp25;struct _tuple1*_tmp24;void*(*_tmp23)(struct _tuple1*);_LL1: _tmp25=_tmp1F.f1;_tmp24=(_tmp1F.f2)->f1;_tmp23=(_tmp1F.f2)->f2;_LL2:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp25->aggrs_so_far,_tmp24);
if(v == 0)
return _tmp23(_tmp24);else{
# 180
struct _tuple18*_tmp20=*v;struct _tuple18*_tmp21=_tmp20;void*_tmp22;_LL4: _tmp22=_tmp21->f2;_LL5:;
return _tmp22;}};}
# 185
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp26;_tmp26.f1=q;_tmp26.f2=type_maker;_tmp26;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp27[5U]="curr";
# 198 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp27,_tmp27,_tmp27 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp28[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp28,_tmp28,_tmp28 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp29[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp29,_tmp29,_tmp29 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp2A[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp2B[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp2B,_tmp2B,_tmp2B + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2C[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2C,_tmp2C,_tmp2C + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2D[17U]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2D,_tmp2D,_tmp2D + 17U};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp2E[16U]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp2E,_tmp2E,_tmp2E + 16U};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;static char _tmp2F[7U]="_throw";
# 219 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp2F,_tmp2F,_tmp2F + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp32[7U]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp32,_tmp32,_tmp32 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp35[14U]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp35,_tmp35,_tmp35 + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp38[13U]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp38,_tmp38,_tmp38 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp3B[12U]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp3B,_tmp3B,_tmp3B + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp3E[14U]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp3E,_tmp3E,_tmp3E + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp41[12U]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp41,_tmp41,_tmp41 + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp44[28U]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp44,_tmp44,_tmp44 + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp47[31U]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp47,_tmp47,_tmp47 + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp4A[27U]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp4A,_tmp4A,_tmp4A + 27U};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0U,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1U,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp4D[15U]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp4D,_tmp4D,_tmp4D + 15U};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0U,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1U,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp50[15U]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp50,_tmp50,_tmp50 + 15U};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0U,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1U,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp53[20U]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp53,_tmp53,_tmp53 + 20U};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__init_dyneither_ptr_bnd={0U,& Cyc_Toc__init_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1U,(void*)& Cyc_Toc__init_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp56[21U]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp56,_tmp56,_tmp56 + 21U};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0U,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1U,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp59[20U]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp59,_tmp59,_tmp59 + 20U};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0U,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1U,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp5C[19U]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp5C,_tmp5C,_tmp5C + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp5F[24U]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp5F,_tmp5F,_tmp5F + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp62[25U]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp62,_tmp62,_tmp62 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp65[23U]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp65,_tmp65,_tmp65 + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp68[25U]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp68,_tmp68,_tmp68 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp6B[26U]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp6B,_tmp6B,_tmp6B + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp6E[30U]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp6E,_tmp6E,_tmp6E + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp71[28U]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp71,_tmp71,_tmp71 + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp74[20U]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp74,_tmp74,_tmp74 + 20U};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp77[15U]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp77,_tmp77,_tmp77 + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp7A[20U]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp7A,_tmp7A,_tmp7A + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp7D[21U]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp7D,_tmp7D,_tmp7D + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp80[19U]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp80,_tmp80,_tmp80 + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp83[21U]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp83,_tmp83,_tmp83 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp86[22U]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp86,_tmp86,_tmp86 + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp89[26U]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp89,_tmp89,_tmp89 + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp8C[24U]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp8C,_tmp8C,_tmp8C + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp8F[28U]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp8F,_tmp8F,_tmp8F + 28U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp92[23U]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp92,_tmp92,_tmp92 + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp95[28U]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp95,_tmp95,_tmp95 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp98[29U]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp98,_tmp98,_tmp98 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp9B[27U]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp9B,_tmp9B,_tmp9B + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp9E[29U]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp9E,_tmp9E,_tmp9E + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmpA1[30U]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmpA1,_tmpA1,_tmpA1 + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmpA4[34U]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmpA4,_tmpA4,_tmpA4 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmpA7[32U]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmpA7,_tmpA7,_tmpA7 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmpAA[33U]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmpAA,_tmpAA,_tmpAA + 33U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmpAD[28U]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmpAD,_tmpAD,_tmpAD + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmpB0[33U]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmpB0,_tmpB0,_tmpB0 + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmpB3[34U]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmpB3,_tmpB3,_tmpB3 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmpB6[32U]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmpB6,_tmpB6,_tmpB6 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmpB9[34U]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmpB9,_tmpB9,_tmpB9 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmpBC[35U]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmpBC,_tmpBC,_tmpBC + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmpBF[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmpBF,_tmpBF,_tmpBF + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmpC2[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmpC2,_tmpC2,_tmpC2 + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmpC5[10U]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmpC5,_tmpC5,_tmpC5 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmpC8[11U]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmpC8,_tmpC8,_tmpC8 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmpCB[17U]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmpCB,_tmpCB,_tmpCB + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmpCE[18U]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmpCE,_tmpCE,_tmpCE + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmpD1[15U]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmpD1,_tmpD1,_tmpD1 + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmpD4[15U]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmpD4,_tmpD4,_tmpD4 + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmpD7[13U]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmpD7,_tmpD7,_tmpD7 + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmpDA[12U]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmpDA,_tmpDA,_tmpDA + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmpDD[13U]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmpDD,_tmpDD,_tmpDD + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmpE0[12U]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmpE0,_tmpE0,_tmpE0 + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmpE3[16U]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmpE3,_tmpE3,_tmpE3 + 16U};static struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__open_dynregion_bnd={0U,& Cyc_Toc__open_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1U,(void*)& Cyc_Toc__open_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmpE6[16U]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmpE6,_tmpE6,_tmpE6 + 16U};static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_dynregion_bnd={0U,& Cyc_Toc__push_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1U,(void*)& Cyc_Toc__push_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmpE9[15U]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmpE9,_tmpE9,_tmpE9 + 15U};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_dynregion_bnd={0U,& Cyc_Toc__pop_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1U,(void*)& Cyc_Toc__pop_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmpEC[19U]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpEC,_tmpEC,_tmpEC + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpEF[29U]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpEF,_tmpEF,_tmpEF + 29U};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0U,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpF2[11U]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpF2,_tmpF2,_tmpF2 + 11U};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__swap_word_bnd={0U,& Cyc_Toc__swap_word_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1U,(void*)& Cyc_Toc__swap_word_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpF5[16U]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpF5,_tmpF5,_tmpF5 + 16U};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__swap_dyneither_bnd={0U,& Cyc_Toc__swap_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1U,(void*)& Cyc_Toc__swap_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpF8[13U]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpF8,_tmpF8,_tmpF8 + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmpFB[9U]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmpFB,_tmpFB,_tmpFB + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmpFE[20U]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmpFE,_tmpFE,_tmpFE + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 293
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11U,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 296
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 298
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 300
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}
# 306
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp102=e->r;void*_tmp103=_tmp102;struct Cyc_Absyn_Exp*_tmp104;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp103)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp103)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp104=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp104);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 312
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp105=_cycalloc(sizeof(*_tmp105));({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp92E=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp106;_tmp106.tag=14U;_tmp106.f1=t;_tmp106.f2=e;_tmp106.f3=0;_tmp106.f4=Cyc_Absyn_No_coercion;_tmp106;});_tmp105[0]=_tmp92E;});_tmp105;});}
# 315
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp92F=({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp108;_tmp108.tag=20U;_tmp108.f1=e;_tmp108;});_tmp107[0]=_tmp92F;});_tmp107;});}
# 318
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp109=_cycalloc(sizeof(*_tmp109));({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp930=({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp10A;_tmp10A.tag=23U;_tmp10A.f1=e1;_tmp10A.f2=e2;_tmp10A;});_tmp109[0]=_tmp930;});_tmp109;});}
# 321
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp931=({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp10C;_tmp10C.tag=37U;_tmp10C.f1=s;_tmp10C;});_tmp10B[0]=_tmp931;});_tmp10B;});}
# 324
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp932=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp10E;_tmp10E.tag=17U;_tmp10E.f1=t;_tmp10E;});_tmp10D[0]=_tmp932;});_tmp10D;});}
# 327
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp933=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp110;_tmp110.tag=10U;_tmp110.f1=e;_tmp110.f2=es;_tmp110.f3=0;_tmp110.f4=1;_tmp110;});_tmp10F[0]=_tmp933;});_tmp10F;});}
# 330
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp111=_cycalloc(sizeof(*_tmp111));({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp934=({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp112;_tmp112.tag=1U;_tmp112.f1=e;_tmp112;});_tmp111[0]=_tmp934;});_tmp111;});}
# 333
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp935=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp114;_tmp114.tag=2U;_tmp114.f1=s1;_tmp114.f2=s2;_tmp114;});_tmp113[0]=_tmp935;});_tmp113;});}
# 336
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp115=_cycalloc(sizeof(*_tmp115));({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp936=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp116;_tmp116.tag=6U;_tmp116.f1=e1;_tmp116.f2=e2;_tmp116.f3=e3;_tmp116;});_tmp115[0]=_tmp936;});_tmp115;});}
# 339
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp117=_cycalloc(sizeof(*_tmp117));({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp937=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp118;_tmp118.tag=21U;_tmp118.f1=e;_tmp118.f2=n;_tmp118.f3=0;_tmp118.f4=0;_tmp118;});_tmp117[0]=_tmp937;});_tmp117;});}
# 342
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp119=_cycalloc(sizeof(*_tmp119));({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp938=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp11A;_tmp11A.tag=22U;_tmp11A.f1=e;_tmp11A.f2=n;_tmp11A.f3=0;_tmp11A.f4=0;_tmp11A;});_tmp119[0]=_tmp938;});_tmp119;});}
# 345
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 348
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp939=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp11C;_tmp11C.tag=36U;_tmp11C.f1=tdopt;_tmp11C.f2=ds;_tmp11C;});_tmp11B[0]=_tmp939;});_tmp11B;});}
# 350
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp93A=({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp11E;_tmp11E.tag=8U;_tmp11E.f1=v;_tmp11E;});_tmp11D[0]=_tmp93A;});_tmp11D;});}
# 353
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 357
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp120=e->r;void*_tmp121=_tmp120;struct Cyc_Absyn_Exp*_tmp122;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp121)->tag == 20U){_LL1: _tmp122=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp121)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp122,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 376
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 385
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 394
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 403
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 414
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp123=Cyc_Tcutil_compress(t);void*_tmp124=_tmp123;enum Cyc_Absyn_Size_of _tmp12B;switch(*((int*)_tmp124)){case 6U: _LL1: _tmp12B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp124)->f2;_LL2:
# 418
{enum Cyc_Absyn_Size_of _tmp125=_tmp12B;switch(_tmp125){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp126=0U;({struct _dyneither_ptr _tmp93B=({const char*_tmp127="impossible IntType (not char, short or int)";_tag_dyneither(_tmp127,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp93B,_tag_dyneither(_tmp126,sizeof(void*),0U));});});}_LLD:;}
# 424
goto _LL0;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp124)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}case 5U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp12A;_tmp12A.tag=0U;({struct _dyneither_ptr _tmp93C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp12A.f1=_tmp93C;});({void*_tmp128[1U]={& _tmp12A};({struct _dyneither_ptr _tmp93D=({const char*_tmp129="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp129,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp93D,_tag_dyneither(_tmp128,sizeof(void*),1U));});});});}_LL0:;}
# 431
return function;}
# 433
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp12C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp12D=_tmp12C;void*_tmp130;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12D)->tag == 5U){_LL1: _tmp130=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12D)->f1).elt_typ;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp130);}else{_LL3: _LL4:
({void*_tmp12E=0U;({struct _dyneither_ptr _tmp93E=({const char*_tmp12F="impossible type (not pointer)";_tag_dyneither(_tmp12F,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp93E,_tag_dyneither(_tmp12E,sizeof(void*),0U));});});}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 443
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp131=e->r;void*_tmp132=_tmp131;struct Cyc_List_List*_tmp13E;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;long long _tmp138;int _tmp137;short _tmp136;struct _dyneither_ptr _tmp135;char _tmp134;switch(*((int*)_tmp132)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).Null_c).tag){case 2U: _LL1: _tmp134=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).Char_c).val).f2;_LL2:
 return _tmp134 == '\000';case 3U: _LL3: _tmp135=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).Wchar_c).val;_LL4: {
# 447
unsigned long _tmp133=Cyc_strlen((struct _dyneither_ptr)_tmp135);
int i=0;
if(_tmp133 >= 2  && *((const char*)_check_dyneither_subscript(_tmp135,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp135,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp135.curr)[1]== 'x'  && _tmp133 >= 3) && *((const char*)_check_dyneither_subscript(_tmp135,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp133;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp135,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 457
return 0;}}case 4U: _LL5: _tmp136=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).Short_c).val).f2;_LL6:
 return _tmp136 == 0;case 5U: _LL7: _tmp137=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).Int_c).val).f2;_LL8:
 return _tmp137 == 0;case 6U: _LL9: _tmp138=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).LongLong_c).val).f2;_LLA:
 return _tmp138 == 0;case 1U: _LLD: _LLE:
# 462
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 461
 goto _LLE;case 14U: _LLF: _tmp139=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp132)->f2;_LL10:
# 463
 return Cyc_Toc_is_zero(_tmp139);case 24U: _LL11: _tmp13A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp132)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp13A);case 26U: _LL13: _tmp13B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp132)->f1;_LL14:
 _tmp13C=_tmp13B;goto _LL16;case 29U: _LL15: _tmp13C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp132)->f3;_LL16:
 _tmp13D=_tmp13C;goto _LL18;case 25U: _LL17: _tmp13D=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp132)->f2;_LL18:
 _tmp13E=_tmp13D;goto _LL1A;case 36U: _LL19: _tmp13E=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp132)->f2;_LL1A:
# 469
 for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp13E->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 478
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmp13F=x;union Cyc_Absyn_Nmspace _tmp151;struct _dyneither_ptr*_tmp150;_LL1: _tmp151=_tmp13F->f1;_tmp150=_tmp13F->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp140=_tmp151;struct Cyc_List_List*_tmp14F;struct Cyc_List_List*_tmp14E;struct Cyc_List_List*_tmp14D;switch((_tmp140.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmp14D=0;goto _LL7;case 1U: _LL6: _tmp14D=(_tmp140.Rel_n).val;_LL7:
 _tmp14E=_tmp14D;goto _LL9;case 2U: _LL8: _tmp14E=(_tmp140.Abs_n).val;_LL9:
 _tmp14F=_tmp14E;goto _LLB;default: _LLA: _tmp14F=(_tmp140.C_n).val;_LLB:
# 487
 if(_tmp14F == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp144;_tmp144.tag=0U;_tmp144.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp150);({struct Cyc_String_pa_PrintArg_struct _tmp143;_tmp143.tag=0U;_tmp143.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);({void*_tmp141[2U]={& _tmp143,& _tmp144};({struct _dyneither_ptr _tmp93F=({const char*_tmp142="%s_%s_struct";_tag_dyneither(_tmp142,sizeof(char),13U);});Cyc_aprintf(_tmp93F,_tag_dyneither(_tmp141,sizeof(void*),2U));});});});});{
struct _RegionHandle _tmp145=_new_region("r");struct _RegionHandle*r=& _tmp145;_push_region(r);
{struct _dyneither_ptr _tmp14C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0U;_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp150);({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0U;({struct _dyneither_ptr _tmp942=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp941=r;struct Cyc_List_List*_tmp940=_tmp14F;Cyc_rstr_sepstr(_tmp941,_tmp940,({const char*_tmp14B="_";_tag_dyneither(_tmp14B,sizeof(char),2U);}));}));_tmp149.f1=_tmp942;});({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0U;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);({void*_tmp146[3U]={& _tmp148,& _tmp149,& _tmp14A};({struct _dyneither_ptr _tmp943=({const char*_tmp147="%s_%s_%s_struct";_tag_dyneither(_tmp147,sizeof(char),16U);});Cyc_aprintf(_tmp943,_tag_dyneither(_tmp146,sizeof(void*),3U));});});});});});_npop_handler(0U);return _tmp14C;};_pop_region(r);};}_LL3:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 500
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 503
struct _tuple20 _tmp152=*env;struct _tuple20 _tmp153=_tmp152;struct Cyc_Dict_Dict*_tmp166;struct _tuple14*_tmp165;_LL1: _tmp166=(_tmp153.f1)->qvar_tags;_tmp165=_tmp153.f2;_LL2:;{
struct _tuple14 _tmp154=*_tmp165;struct _tuple14 _tmp155=_tmp154;struct _tuple1*_tmp164;struct _tuple1*_tmp163;_LL4: _tmp164=_tmp155.f1;_tmp163=_tmp155.f2;_LL5:;{
struct _handler_cons _tmp156;_push_handler(& _tmp156);{int _tmp158=0;if(setjmp(_tmp156.handler))_tmp158=1;if(!_tmp158){{struct _tuple1*_tmp159=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp166,Cyc_Toc_qvar_tag_cmp,_tmp165);_npop_handler(0U);return _tmp159;};_pop_handler();}else{void*_tmp157=(void*)_exn_thrown;void*_tmp15A=_tmp157;void*_tmp162;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15A)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 507
struct _tuple14*_tmp15B=({struct _tuple14*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->f1=_tmp164;_tmp161->f2=_tmp163;_tmp161;});
struct _tuple1*_tmp15C=_tmp164;union Cyc_Absyn_Nmspace _tmp160;struct _dyneither_ptr*_tmp15F;_LLC: _tmp160=_tmp15C->f1;_tmp15F=_tmp15C->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp15F,_tmp163);
struct _tuple1*res=({struct _tuple1*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->f1=_tmp160;({struct _dyneither_ptr*_tmp944=({struct _dyneither_ptr*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E[0]=newvar;_tmp15E;});_tmp15D->f2=_tmp944;});_tmp15D;});
({struct Cyc_Dict_Dict _tmp945=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp166,_tmp15B,res);*_tmp166=_tmp945;});
return res;};}}else{_LL9: _tmp162=_tmp15A;_LLA:(int)_rethrow(_tmp162);}_LL6:;}};};};}
# 516
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp167;_tmp167.f1=fieldname;_tmp167.f2=dtname;_tmp167;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 521
void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 525
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 528
struct _tuple21 _tmp168=*env;struct _tuple21 _tmp169=_tmp168;struct Cyc_List_List**_tmp181;struct Cyc_List_List*_tmp180;_LL1: _tmp181=(_tmp169.f1)->tuple_types;_tmp180=_tmp169.f2;_LL2:;
# 530
{struct Cyc_List_List*_tmp16A=*_tmp181;for(0;_tmp16A != 0;_tmp16A=_tmp16A->tl){
struct _tuple22*_tmp16B=(struct _tuple22*)_tmp16A->hd;struct _tuple22*_tmp16C=_tmp16B;void*_tmp16F;struct Cyc_List_List*_tmp16E;_LL4: _tmp16F=_tmp16C->f1;_tmp16E=_tmp16C->f2;_LL5:;{
struct Cyc_List_List*_tmp16D=_tmp180;
for(0;_tmp16D != 0  && _tmp16E != 0;(_tmp16D=_tmp16D->tl,_tmp16E=_tmp16E->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp16D->hd)).f2,(void*)_tmp16E->hd))
break;}
if(_tmp16D == 0  && _tmp16E == 0)
return _tmp16F;};}}{
# 540
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp17C=_cycalloc(sizeof(*_tmp17C));({struct _dyneither_ptr _tmp947=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp17F;_tmp17F.tag=1U;_tmp17F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp17D[1U]={& _tmp17F};({struct _dyneither_ptr _tmp946=({const char*_tmp17E="_tuple%d";_tag_dyneither(_tmp17E,sizeof(char),9U);});Cyc_aprintf(_tmp946,_tag_dyneither(_tmp17D,sizeof(void*),1U));});});});_tmp17C[0]=_tmp947;});_tmp17C;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp180);
struct Cyc_List_List*_tmp170=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp94A=({struct Cyc_List_List*_tmp171=_cycalloc(sizeof(*_tmp171));({struct Cyc_Absyn_Aggrfield*_tmp949=({struct Cyc_Absyn_Aggrfield*_tmp172=_cycalloc(sizeof(*_tmp172));({struct _dyneither_ptr*_tmp948=Cyc_Absyn_fieldname(i);_tmp172->name=_tmp948;});_tmp172->tq=Cyc_Toc_mt_tq;_tmp172->type=(void*)ts2->hd;_tmp172->width=0;_tmp172->attributes=0;_tmp172->requires_clause=0;_tmp172;});_tmp171->hd=_tmp949;});_tmp171->tl=_tmp170;_tmp171;});_tmp170=_tmp94A;});}}
({struct Cyc_List_List*_tmp94B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp170);_tmp170=_tmp94B;});{
struct Cyc_Absyn_Aggrdecl*_tmp173=({struct Cyc_Absyn_Aggrdecl*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->kind=Cyc_Absyn_StructA;_tmp179->sc=Cyc_Absyn_Public;({struct _tuple1*_tmp94D=({struct _tuple1*_tmp17B=_cycalloc(sizeof(*_tmp17B));({union Cyc_Absyn_Nmspace _tmp94C=
Cyc_Absyn_Rel_n(0);_tmp17B->f1=_tmp94C;});_tmp17B->f2=xname;_tmp17B;});_tmp179->name=_tmp94D;});_tmp179->tvs=0;({struct Cyc_Absyn_AggrdeclImpl*_tmp94E=({struct Cyc_Absyn_AggrdeclImpl*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->exist_vars=0;_tmp17A->rgn_po=0;_tmp17A->fields=_tmp170;_tmp17A->tagged=0;_tmp17A;});_tmp179->impl=_tmp94E;});_tmp179->attributes=0;_tmp179->expected_mem_kind=0;_tmp179;});
# 555
({struct Cyc_List_List*_tmp951=({struct Cyc_List_List*_tmp174=_cycalloc(sizeof(*_tmp174));({struct Cyc_Absyn_Decl*_tmp950=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp175=_cycalloc(sizeof(*_tmp175));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp94F=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp176;_tmp176.tag=5U;_tmp176.f1=_tmp173;_tmp176;});_tmp175[0]=_tmp94F;});_tmp175;}),0U);_tmp174->hd=_tmp950;});_tmp174->tl=Cyc_Toc_result_decls;_tmp174;});Cyc_Toc_result_decls=_tmp951;});
({struct Cyc_List_List*_tmp953=({struct Cyc_List_List*_tmp177=_region_malloc(d,sizeof(*_tmp177));({struct _tuple22*_tmp952=({struct _tuple22*_tmp178=_region_malloc(d,sizeof(*_tmp178));_tmp178->f1=x;_tmp178->f2=ts;_tmp178;});_tmp177->hd=_tmp952;});_tmp177->tl=*_tmp181;_tmp177;});*_tmp181=_tmp953;});
return x;};};}
# 560
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 569
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 577
struct _tuple24 _tmp182=*env;struct _tuple24 _tmp183=_tmp182;struct Cyc_List_List**_tmp1AD;struct _tuple1*_tmp1AC;struct Cyc_List_List*_tmp1AB;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*_tmp1A9;_LL1: _tmp1AD=(_tmp183.f1)->abs_struct_types;_tmp1AC=(_tmp183.f2)->f1;_tmp1AB=(_tmp183.f2)->f2;_tmp1AA=(_tmp183.f2)->f3;_tmp1A9=(_tmp183.f2)->f4;_LL2:;
# 582
{struct Cyc_List_List*_tmp184=*_tmp1AD;for(0;_tmp184 != 0;_tmp184=_tmp184->tl){
struct _tuple25*_tmp185=(struct _tuple25*)_tmp184->hd;struct _tuple25*_tmp186=_tmp185;struct _tuple1*_tmp18E;struct Cyc_List_List*_tmp18D;void*_tmp18C;_LL4: _tmp18E=_tmp186->f1;_tmp18D=_tmp186->f2;_tmp18C=_tmp186->f3;_LL5:;
# 585
if(Cyc_Absyn_qvar_cmp(_tmp18E,_tmp1AC)== 0  && ({
int _tmp954=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1AA);_tmp954 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp18D);})){
int okay=1;
{struct Cyc_List_List*_tmp187=_tmp1AA;for(0;_tmp187 != 0;(_tmp187=_tmp187->tl,_tmp18D=_tmp18D->tl)){
void*_tmp188=(void*)_tmp187->hd;
void*_tmp189=(void*)((struct Cyc_List_List*)_check_null(_tmp18D))->hd;
# 592
{struct Cyc_Absyn_Kind*_tmp18A=Cyc_Tcutil_typ_kind(_tmp188);struct Cyc_Absyn_Kind*_tmp18B=_tmp18A;switch(((struct Cyc_Absyn_Kind*)_tmp18B)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 597
 continue;default: _LLB: _LLC:
# 600
 if(Cyc_Tcutil_unify(_tmp188,_tmp189) || ({void*_tmp955=Cyc_Toc_typ_to_c(_tmp188);Cyc_Tcutil_unify(_tmp955,Cyc_Toc_typ_to_c(_tmp189));}))
# 602
continue;
# 605
okay=0;
goto _LL6;}_LL6:;}
# 608
break;}}
# 610
if(okay)
# 612
return _tmp18C;}}}{
# 619
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));({struct _dyneither_ptr _tmp957=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1A8;_tmp1A8.tag=1U;_tmp1A8.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp1A6[1U]={& _tmp1A8};({struct _dyneither_ptr _tmp956=({const char*_tmp1A7="_tuple%d";_tag_dyneither(_tmp1A7,sizeof(char),9U);});Cyc_aprintf(_tmp956,_tag_dyneither(_tmp1A6,sizeof(void*),1U));});});});_tmp1A5[0]=_tmp957;});_tmp1A5;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp18F=0;
# 623
({struct Cyc_List_List*_tmp959=({struct Cyc_List_List*_tmp190=_region_malloc(d,sizeof(*_tmp190));({struct _tuple25*_tmp958=({struct _tuple25*_tmp191=_region_malloc(d,sizeof(*_tmp191));_tmp191->f1=_tmp1AC;_tmp191->f2=_tmp1AA;_tmp191->f3=x;_tmp191;});_tmp190->hd=_tmp958;});_tmp190->tl=*_tmp1AD;_tmp190;});*_tmp1AD=_tmp959;});{
# 626
struct _RegionHandle _tmp192=_new_region("r");struct _RegionHandle*r=& _tmp192;_push_region(r);
{struct Cyc_List_List*_tmp193=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1AB,_tmp1AA);
for(0;_tmp1A9 != 0;_tmp1A9=_tmp1A9->tl){
struct Cyc_Absyn_Aggrfield*_tmp194=(struct Cyc_Absyn_Aggrfield*)_tmp1A9->hd;
void*t=_tmp194->type;
struct Cyc_List_List*atts=_tmp194->attributes;
# 634
if(_tmp1A9->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t)))
({struct Cyc_List_List*_tmp95C=({struct Cyc_List_List*_tmp195=_cycalloc(sizeof(*_tmp195));({void*_tmp95B=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp196=_cycalloc(sizeof(*_tmp196));({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp95A=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp197;_tmp197.tag=6U;_tmp197.f1=0;_tmp197;});_tmp196[0]=_tmp95A;});_tmp196;});_tmp195->hd=_tmp95B;});_tmp195->tl=atts;_tmp195;});atts=_tmp95C;});
# 637
({void*_tmp95D=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp193,t));t=_tmp95D;});
# 640
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
({void*_tmp964=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp198=_cycalloc(sizeof(*_tmp198));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp963=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp199;_tmp199.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmp962=({struct Cyc_Absyn_ArrayInfo _tmp19A;({void*_tmp95E=Cyc_Absyn_void_star_typ();_tmp19A.elt_type=_tmp95E;});({struct Cyc_Absyn_Tqual _tmp95F=Cyc_Absyn_empty_tqual(0U);_tmp19A.tq=_tmp95F;});({struct Cyc_Absyn_Exp*_tmp960=
Cyc_Absyn_uint_exp(0U,0U);_tmp19A.num_elts=_tmp960;});({union Cyc_Absyn_Constraint*_tmp961=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp19A.zero_term=_tmp961;});_tmp19A.zt_loc=0U;_tmp19A;});_tmp199.f1=_tmp962;});_tmp199;});_tmp198[0]=_tmp963;});_tmp198;});
# 641
t=_tmp964;});
# 644
({struct Cyc_List_List*_tmp966=({struct Cyc_List_List*_tmp19B=_cycalloc(sizeof(*_tmp19B));({struct Cyc_Absyn_Aggrfield*_tmp965=({struct Cyc_Absyn_Aggrfield*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->name=_tmp194->name;_tmp19C->tq=Cyc_Toc_mt_tq;_tmp19C->type=t;_tmp19C->width=_tmp194->width;_tmp19C->attributes=atts;_tmp19C->requires_clause=0;_tmp19C;});_tmp19B->hd=_tmp965;});_tmp19B->tl=_tmp18F;_tmp19B;});_tmp18F=_tmp966;});}
# 646
({struct Cyc_List_List*_tmp967=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp18F);_tmp18F=_tmp967;});{
struct Cyc_Absyn_Aggrdecl*_tmp19D=({struct Cyc_Absyn_Aggrdecl*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->kind=Cyc_Absyn_StructA;_tmp1A2->sc=Cyc_Absyn_Public;({struct _tuple1*_tmp969=({struct _tuple1*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));({union Cyc_Absyn_Nmspace _tmp968=
Cyc_Absyn_Rel_n(0);_tmp1A4->f1=_tmp968;});_tmp1A4->f2=xname;_tmp1A4;});_tmp1A2->name=_tmp969;});_tmp1A2->tvs=0;({struct Cyc_Absyn_AggrdeclImpl*_tmp96A=({struct Cyc_Absyn_AggrdeclImpl*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->exist_vars=0;_tmp1A3->rgn_po=0;_tmp1A3->fields=_tmp18F;_tmp1A3->tagged=0;_tmp1A3;});_tmp1A2->impl=_tmp96A;});_tmp1A2->attributes=0;_tmp1A2->expected_mem_kind=0;_tmp1A2;});
# 654
({struct Cyc_List_List*_tmp96D=({struct Cyc_List_List*_tmp19E=_cycalloc(sizeof(*_tmp19E));({struct Cyc_Absyn_Decl*_tmp96C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp19F=_cycalloc(sizeof(*_tmp19F));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp96B=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1A0;_tmp1A0.tag=5U;_tmp1A0.f1=_tmp19D;_tmp1A0;});_tmp19F[0]=_tmp96B;});_tmp19F;}),0U);_tmp19E->hd=_tmp96C;});_tmp19E->tl=Cyc_Toc_result_decls;_tmp19E;});Cyc_Toc_result_decls=_tmp96D;});{
void*_tmp1A1=x;_npop_handler(0U);return _tmp1A1;};};}
# 627
;_pop_region(r);};};}
# 658
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 662
struct _tuple23 env=({struct _tuple23 _tmp1AE;_tmp1AE.f1=struct_name;_tmp1AE.f2=type_vars;_tmp1AE.f3=type_args;_tmp1AE.f4=fields;_tmp1AE;});
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 669
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp970=({struct _dyneither_ptr*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));({struct _dyneither_ptr _tmp96F=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=1U;_tmp1B3.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;({void*_tmp1B1[1U]={& _tmp1B3};({struct _dyneither_ptr _tmp96E=({const char*_tmp1B2="_tmp%X";_tag_dyneither(_tmp1B2,sizeof(char),7U);});Cyc_aprintf(_tmp96E,_tag_dyneither(_tmp1B1,sizeof(void*),1U));});});});_tmp1B0[0]=_tmp96F;});_tmp1B0;});_tmp1AF->f2=_tmp970;});_tmp1AF;});}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 675
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp1B4=*env;struct _tuple26 _tmp1B5=_tmp1B4;struct Cyc_Xarray_Xarray*_tmp1BD;_LL1: _tmp1BD=(_tmp1B5.f1)->temp_labels;_LL2:;{
int _tmp1B6=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp971=_tmp1B6;_tmp971 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp1BD);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp1BD,_tmp1B6);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));({struct _dyneither_ptr _tmp973=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1BC;_tmp1BC.tag=1U;_tmp1BC.f1=(unsigned int)_tmp1B6;({void*_tmp1BA[1U]={& _tmp1BC};({struct _dyneither_ptr _tmp972=({const char*_tmp1BB="_LL%X";_tag_dyneither(_tmp1BB,sizeof(char),6U);});Cyc_aprintf(_tmp972,_tag_dyneither(_tmp1BA,sizeof(void*),1U));});});});_tmp1B9[0]=_tmp973;});_tmp1B9;});
if(({int _tmp974=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp1BD,res);_tmp974 != _tmp1B6;}))
({void*_tmp1B7=0U;({struct _dyneither_ptr _tmp975=({const char*_tmp1B8="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp1B8,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp975,_tag_dyneither(_tmp1B7,sizeof(void*),0U));});});
return res;};};}
# 686
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 692
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp1BE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp1BE))->hd)->name)!= 0){
++ ans;
_tmp1BE=_tmp1BE->tl;}
# 699
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 705
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp1BF=*a;struct _tuple9 _tmp1C0=_tmp1BF;struct _dyneither_ptr*_tmp1C4;struct Cyc_Absyn_Tqual _tmp1C3;void*_tmp1C2;_LL1: _tmp1C4=_tmp1C0.f1;_tmp1C3=_tmp1C0.f2;_tmp1C2=_tmp1C0.f3;_LL2:;
return({struct _tuple9*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->f1=_tmp1C4;_tmp1C1->f2=_tmp1C3;({void*_tmp976=Cyc_Toc_typ_to_c(_tmp1C2);_tmp1C1->f3=_tmp976;});_tmp1C1;});}
# 712
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1C5=*x;struct _tuple11 _tmp1C6=_tmp1C5;struct Cyc_Absyn_Tqual _tmp1C9;void*_tmp1C8;_LL1: _tmp1C9=_tmp1C6.f1;_tmp1C8=_tmp1C6.f2;_LL2:;
return({struct _tuple11*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->f1=_tmp1C9;({void*_tmp977=Cyc_Toc_typ_to_c(_tmp1C8);_tmp1C7->f2=_tmp977;});_tmp1C7;});}
# 731 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1CA=Cyc_Tcutil_compress(t);void*_tmp1CB=_tmp1CA;void*_tmp1D1;void*_tmp1D0;struct Cyc_Absyn_Tqual _tmp1CF;struct Cyc_Absyn_Exp*_tmp1CE;union Cyc_Absyn_Constraint*_tmp1CD;unsigned int _tmp1CC;switch(*((int*)_tmp1CB)){case 8U: _LL1: _tmp1D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).elt_type;_tmp1CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).tq;_tmp1CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).num_elts;_tmp1CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).zero_term;_tmp1CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).zt_loc;_LL2:
# 734
 return({void*_tmp978=Cyc_Toc_typ_to_c_array(_tmp1D0);Cyc_Absyn_cstar_typ(_tmp978,_tmp1CF);});case 1U: _LL3: _tmp1D1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CB)->f2;if(_tmp1D1 != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp1D1);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 740
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 742
return({struct Cyc_Absyn_Aggrfield*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->name=f->name;_tmp1D2->tq=Cyc_Toc_mt_tq;({void*_tmp979=
# 744
Cyc_Toc_typ_to_c(f->type);_tmp1D2->type=_tmp979;});_tmp1D2->width=f->width;_tmp1D2->attributes=f->attributes;_tmp1D2->requires_clause=0;_tmp1D2;});}
# 749
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 751
return;}
# 754
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
({void**_tmp97B=({void**_tmp1D3=_cycalloc(sizeof(*_tmp1D3));({void*_tmp97A=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp1D3[0]=_tmp97A;});_tmp1D3;});cs=_tmp97B;});
return*cs;}
# 760
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp97E=({void**_tmp1D4=_cycalloc(sizeof(*_tmp1D4));({void*_tmp97D=({void*_tmp97C=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp97C,Cyc_Toc_mt_tq);});_tmp1D4[0]=_tmp97D;});_tmp1D4;});r=_tmp97E;});
return*r;}
# 766
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp981=({void**_tmp1D5=_cycalloc(sizeof(*_tmp1D5));({void*_tmp980=({void*_tmp97F=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp97F,Cyc_Toc_mt_tq);});_tmp1D5[0]=_tmp980;});_tmp1D5;});r=_tmp981;});
return*r;}
# 772
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1D6=Cyc_Tcutil_compress(t);void*_tmp1D7=_tmp1D6;struct Cyc_Absyn_Tvar*_tmp1D8;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D7)->tag == 2U){_LL1: _tmp1D8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D7)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 778
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1D9=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1DA=_tmp1D9;if(((struct Cyc_Absyn_Kind*)_tmp1DA)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 785
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1DB=t;struct Cyc_Absyn_Datatypedecl*_tmp225;void**_tmp224;struct Cyc_Absyn_Enumdecl*_tmp223;struct Cyc_Absyn_Aggrdecl*_tmp222;struct Cyc_Absyn_Exp*_tmp221;struct Cyc_Absyn_Exp*_tmp220;struct _tuple1*_tmp21F;struct Cyc_List_List*_tmp21E;struct Cyc_Absyn_Typedefdecl*_tmp21D;void*_tmp21C;struct Cyc_List_List*_tmp21B;struct _tuple1*_tmp21A;union Cyc_Absyn_AggrInfoU _tmp219;struct Cyc_List_List*_tmp218;enum Cyc_Absyn_AggrKind _tmp217;struct Cyc_List_List*_tmp216;struct Cyc_List_List*_tmp215;struct Cyc_Absyn_Tqual _tmp214;void*_tmp213;struct Cyc_List_List*_tmp212;int _tmp211;struct Cyc_Absyn_VarargInfo*_tmp210;struct Cyc_List_List*_tmp20F;void*_tmp20E;struct Cyc_Absyn_Tqual _tmp20D;struct Cyc_Absyn_Exp*_tmp20C;unsigned int _tmp20B;void*_tmp20A;struct Cyc_Absyn_Tqual _tmp209;union Cyc_Absyn_Constraint*_tmp208;struct Cyc_Absyn_Datatypedecl*_tmp207;struct Cyc_Absyn_Datatypefield*_tmp206;struct Cyc_Absyn_Tvar*_tmp205;void**_tmp204;switch(*((int*)_tmp1DB)){case 0U: _LL1: _LL2:
 return t;case 1U: _LL3: _tmp204=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DB)->f2;_LL4:
# 789
 if(*_tmp204 == 0){
*_tmp204=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 793
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp204));case 2U: _LL5: _tmp205=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DB)->f1;_LL6:
# 795
 if((Cyc_Tcutil_tvar_kind(_tmp205,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 797
return(void*)& Cyc_Absyn_VoidType_val;else{
# 799
return Cyc_Absyn_void_star_typ();}case 3U: _LL7: _LL8:
# 801
 return(void*)& Cyc_Absyn_VoidType_val;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1DB)->f1).field_info).KnownDatatypefield).tag == 2){_LL9: _tmp207=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1DB)->f1).field_info).KnownDatatypefield).val).f1;_tmp206=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1DB)->f1).field_info).KnownDatatypefield).val).f2;_LLA:
# 803
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp206->name,_tmp207->name));}else{_LLB: _LLC:
({void*_tmp1DC=0U;({struct _dyneither_ptr _tmp982=({const char*_tmp1DD="unresolved DatatypeFieldType";_tag_dyneither(_tmp1DD,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp982,_tag_dyneither(_tmp1DC,sizeof(void*),0U));});});}case 5U: _LLD: _tmp20A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DB)->f1).elt_typ;_tmp209=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DB)->f1).elt_tq;_tmp208=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DB)->f1).ptr_atts).bounds;_LLE:
# 808
({void*_tmp983=Cyc_Toc_typ_to_c(_tmp20A);_tmp20A=_tmp983;});{
void*_tmp1DE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp208);void*_tmp1DF=_tmp1DE;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1DF)->tag == 0U){_LL42: _LL43:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LL44: _LL45:
 return Cyc_Absyn_star_typ(_tmp20A,(void*)& Cyc_Absyn_HeapRgn_val,_tmp209,Cyc_Absyn_false_conref);}_LL41:;};case 6U: _LLF: _LL10:
# 813
 goto _LL12;case 7U: _LL11: _LL12:
 return t;case 8U: _LL13: _tmp20E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DB)->f1).elt_type;_tmp20D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DB)->f1).tq;_tmp20C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DB)->f1).num_elts;_tmp20B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DB)->f1).zt_loc;_LL14:
# 816
 return({void*_tmp987=Cyc_Toc_typ_to_c(_tmp20E);struct Cyc_Absyn_Tqual _tmp986=_tmp20D;struct Cyc_Absyn_Exp*_tmp985=_tmp20C;union Cyc_Absyn_Constraint*_tmp984=Cyc_Absyn_false_conref;Cyc_Absyn_array_typ(_tmp987,_tmp986,_tmp985,_tmp984,_tmp20B);});case 9U: _LL15: _tmp214=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).ret_tqual;_tmp213=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).ret_typ;_tmp212=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).args;_tmp211=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).c_varargs;_tmp210=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).cyc_varargs;_tmp20F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).attributes;_LL16: {
# 822
struct Cyc_List_List*_tmp1E0=0;
for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){
void*_tmp1E1=(void*)_tmp20F->hd;void*_tmp1E2=_tmp1E1;switch(*((int*)_tmp1E2)){case 4U: _LL47: _LL48:
 goto _LL4A;case 5U: _LL49: _LL4A:
 goto _LL4C;case 19U: _LL4B: _LL4C:
 continue;case 22U: _LL4D: _LL4E:
 continue;case 21U: _LL4F: _LL50:
 continue;case 20U: _LL51: _LL52:
 continue;default: _LL53: _LL54:
({struct Cyc_List_List*_tmp988=({struct Cyc_List_List*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->hd=(void*)_tmp20F->hd;_tmp1E3->tl=_tmp1E0;_tmp1E3;});_tmp1E0=_tmp988;});goto _LL46;}_LL46:;}{
# 833
struct Cyc_List_List*_tmp1E4=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp212);
if(_tmp210 != 0){
# 836
void*_tmp1E5=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp210->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp1E6=({struct _tuple9*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->f1=_tmp210->name;_tmp1E8->f2=_tmp210->tq;_tmp1E8->f3=_tmp1E5;_tmp1E8;});
({struct Cyc_List_List*_tmp98A=({struct Cyc_List_List*_tmp989=_tmp1E4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp989,({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=_tmp1E6;_tmp1E7->tl=0;_tmp1E7;}));});_tmp1E4=_tmp98A;});}
# 840
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp98D=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1EA;_tmp1EA.tag=9U;({struct Cyc_Absyn_FnInfo _tmp98C=({struct Cyc_Absyn_FnInfo _tmp1EB;_tmp1EB.tvars=0;_tmp1EB.effect=0;_tmp1EB.ret_tqual=_tmp214;({void*_tmp98B=Cyc_Toc_typ_to_c(_tmp213);_tmp1EB.ret_typ=_tmp98B;});_tmp1EB.args=_tmp1E4;_tmp1EB.c_varargs=_tmp211;_tmp1EB.cyc_varargs=0;_tmp1EB.rgn_po=0;_tmp1EB.attributes=_tmp1E0;_tmp1EB.requires_clause=0;_tmp1EB.requires_relns=0;_tmp1EB.ensures_clause=0;_tmp1EB.ensures_relns=0;_tmp1EB;});_tmp1EA.f1=_tmp98C;});_tmp1EA;});_tmp1E9[0]=_tmp98D;});_tmp1E9;});};}case 10U: _LL17: _tmp215=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1DB)->f1;_LL18:
# 845
({struct Cyc_List_List*_tmp98E=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp215);_tmp215=_tmp98E;});
return Cyc_Toc_add_tuple_type(_tmp215);case 12U: _LL19: _tmp217=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1DB)->f1;_tmp216=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1DB)->f2;_LL1A:
# 850
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp990=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1ED;_tmp1ED.tag=12U;_tmp1ED.f1=_tmp217;({struct Cyc_List_List*_tmp98F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp216);_tmp1ED.f2=_tmp98F;});_tmp1ED;});_tmp1EC[0]=_tmp990;});_tmp1EC;});case 11U: _LL1B: _tmp219=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1DB)->f1).aggr_info;_tmp218=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1DB)->f1).targs;_LL1C:
# 854
{union Cyc_Absyn_AggrInfoU _tmp1EE=_tmp219;if((_tmp1EE.UnknownAggr).tag == 1){_LL56: _LL57:
 return t;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}{
# 858
struct Cyc_Absyn_Aggrdecl*_tmp1EF=Cyc_Absyn_get_known_aggrdecl(_tmp219);
if(_tmp1EF->expected_mem_kind){
# 861
if(_tmp1EF->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1F3;_tmp1F3.tag=0U;({struct _dyneither_ptr _tmp991=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 864
Cyc_Absynpp_qvar2string(_tmp1EF->name));_tmp1F3.f1=_tmp991;});({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0U;({struct _dyneither_ptr _tmp992=(struct _dyneither_ptr)(
# 863
_tmp1EF->kind == Cyc_Absyn_UnionA?({const char*_tmp1F4="union";_tag_dyneither(_tmp1F4,sizeof(char),6U);}):({const char*_tmp1F5="struct";_tag_dyneither(_tmp1F5,sizeof(char),7U);}));_tmp1F2.f1=_tmp992;});({void*_tmp1F0[2U]={& _tmp1F2,& _tmp1F3};({struct _dyneither_ptr _tmp993=({const char*_tmp1F1="%s %s was never defined.";_tag_dyneither(_tmp1F1,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp993,_tag_dyneither(_tmp1F0,sizeof(void*),2U));});});});});}
# 869
if(_tmp1EF->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1EF->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1F6=_tmp1EF->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1EF->impl))->fields;
if(_tmp1F6 == 0)return Cyc_Toc_aggrdecl_type(_tmp1EF->name,Cyc_Absyn_strctq);
for(0;_tmp1F6->tl != 0;_tmp1F6=_tmp1F6->tl){;}{
void*_tmp1F7=((struct Cyc_Absyn_Aggrfield*)_tmp1F6->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1F7))){
if(_tmp1EF->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0U;({struct _dyneither_ptr _tmp994=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp1EF->name));_tmp1FA.f1=_tmp994;});({void*_tmp1F8[1U]={& _tmp1FA};({struct _dyneither_ptr _tmp995=({const char*_tmp1F9="struct %s ended up being abstract.";_tag_dyneither(_tmp1F9,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp995,_tag_dyneither(_tmp1F8,sizeof(void*),1U));});});});{
# 882
struct _RegionHandle _tmp1FB=_new_region("r");struct _RegionHandle*r=& _tmp1FB;_push_region(r);
{struct Cyc_List_List*_tmp1FC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1EF->tvs,_tmp218);
void*_tmp1FD=Cyc_Tcutil_rsubstitute(r,_tmp1FC,_tmp1F7);
if(Cyc_Toc_is_abstract_type(_tmp1FD)){void*_tmp1FE=Cyc_Toc_aggrdecl_type(_tmp1EF->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp1FE;}{
void*_tmp1FF=Cyc_Toc_add_struct_type(_tmp1EF->name,_tmp1EF->tvs,_tmp218,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1EF->impl))->fields);_npop_handler(0U);return _tmp1FF;};}
# 883
;_pop_region(r);};}
# 888
return Cyc_Toc_aggrdecl_type(_tmp1EF->name,Cyc_Absyn_strctq);};};};case 13U: _LL1D: _tmp21A=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1DB)->f1;_LL1E:
 return t;case 14U: _LL1F: _tmp21B=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1DB)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp21B);return t;case 17U: _LL21: _tmp21F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1DB)->f1;_tmp21E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1DB)->f2;_tmp21D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1DB)->f3;_tmp21C=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1DB)->f4;_LL22:
# 894
 if(_tmp21C == 0){
if(_tmp21E != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp200=_cycalloc(sizeof(*_tmp200));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp996=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp201;_tmp201.tag=17U;_tmp201.f1=_tmp21F;_tmp201.f2=0;_tmp201.f3=_tmp21D;_tmp201.f4=0;_tmp201;});_tmp200[0]=_tmp996;});_tmp200;});else{
return t;}}else{
# 899
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp202=_cycalloc(sizeof(*_tmp202));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp998=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp203;_tmp203.tag=17U;_tmp203.f1=_tmp21F;_tmp203.f2=0;_tmp203.f3=_tmp21D;({void*_tmp997=Cyc_Toc_typ_to_c(_tmp21C);_tmp203.f4=_tmp997;});_tmp203;});_tmp202[0]=_tmp998;});_tmp202;});}case 19U: _LL23: _LL24:
 return Cyc_Absyn_uint_typ;case 15U: _LL25: _LL26:
 return Cyc_Toc_rgn_typ();case 16U: _LL27: _LL28:
 return Cyc_Toc_dyn_rgn_typ();case 20U: _LL29: _LL2A:
# 905
 goto _LL2C;case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 23U: _LL2F: _LL30:
 goto _LL32;case 24U: _LL31: _LL32:
 goto _LL34;case 25U: _LL33: _LL34:
 return Cyc_Absyn_void_star_typ();case 18U: _LL35: _tmp220=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1DB)->f1;_LL36:
# 915
 return t;case 27U: _LL37: _tmp221=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1DB)->f1;_LL38:
# 917
 return t;case 28U: _LL39: _LL3A:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1DB)->f1)->r)){case 0U: _LL3B: _tmp222=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1DB)->f1)->r)->f1;_LL3C:
# 920
 Cyc_Toc_aggrdecl_to_c(_tmp222,1);
if(_tmp222->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp222->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp222->name,Cyc_Absyn_strctq);}case 1U: _LL3D: _tmp223=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1DB)->f1)->r)->f1;_LL3E:
# 925
 Cyc_Toc_enumdecl_to_c(_tmp223);
return t;default: _LL3F: _tmp225=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1DB)->f1)->r)->f1;_tmp224=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1DB)->f2;_LL40:
# 928
 Cyc_Toc_datatypedecl_to_c(_tmp225);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp224)));}}_LL0:;}
# 933
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp226=t;void*_tmp228;struct Cyc_Absyn_Tqual _tmp227;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp226)->tag == 8U){_LL1: _tmp228=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp226)->f1).elt_type;_tmp227=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp226)->f1).tq;_LL2:
# 936
 return({void*_tmp999=Cyc_Absyn_star_typ(_tmp228,(void*)& Cyc_Absyn_HeapRgn_val,_tmp227,Cyc_Absyn_false_conref);Cyc_Toc_cast_it(_tmp999,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 943
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp229=Cyc_Tcutil_compress(t);void*_tmp22A=_tmp229;struct Cyc_List_List*_tmp236;struct Cyc_Absyn_Datatypedecl*_tmp235;struct Cyc_Absyn_Datatypefield*_tmp234;struct Cyc_List_List*_tmp233;union Cyc_Absyn_AggrInfoU _tmp232;void*_tmp231;switch(*((int*)_tmp22A)){case 0U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 0;case 6U: _LL5: _LL6:
 goto _LL8;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;case 9U: _LLD: _LLE:
 goto _LL10;case 19U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmp231=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22A)->f1).elt_type;_LL12:
 return Cyc_Toc_atomic_typ(_tmp231);case 11U: _LL13: _tmp232=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp22A)->f1).aggr_info;_LL14:
# 960
{union Cyc_Absyn_AggrInfoU _tmp22B=_tmp232;if((_tmp22B.UnknownAggr).tag == 1){_LL26: _LL27:
 return 0;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}{
# 964
struct Cyc_Absyn_Aggrdecl*_tmp22C=Cyc_Absyn_get_known_aggrdecl(_tmp232);
if(_tmp22C->impl == 0)
return 0;
{struct Cyc_List_List*_tmp22D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22C->impl))->fields;for(0;_tmp22D != 0;_tmp22D=_tmp22D->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp22D->hd)->type))return 0;}}
return 1;};case 12U: _LL15: _tmp233=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp22A)->f2;_LL16:
# 971
 for(0;_tmp233 != 0;_tmp233=_tmp233->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp233->hd)->type))return 0;}
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp22A)->f1).field_info).KnownDatatypefield).tag == 2){_LL17: _tmp235=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp22A)->f1).field_info).KnownDatatypefield).val).f1;_tmp234=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp22A)->f1).field_info).KnownDatatypefield).val).f2;_LL18:
# 975
 _tmp236=_tmp234->typs;goto _LL1A;}else{goto _LL23;}case 10U: _LL19: _tmp236=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp22A)->f1;_LL1A:
# 977
 for(0;_tmp236 != 0;_tmp236=_tmp236->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp236->hd)).f2))return 0;}
return 1;case 3U: _LL1B: _LL1C:
# 982
 goto _LL1E;case 5U: _LL1D: _LL1E:
 goto _LL20;case 16U: _LL1F: _LL20:
 goto _LL22;case 15U: _LL21: _LL22:
 return 0;default: _LL23: _LL24:
({struct Cyc_String_pa_PrintArg_struct _tmp230;_tmp230.tag=0U;({struct _dyneither_ptr _tmp99A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp230.f1=_tmp99A;});({void*_tmp22E[1U]={& _tmp230};({struct _dyneither_ptr _tmp99B=({const char*_tmp22F="atomic_typ:  bad type %s";_tag_dyneither(_tmp22F,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp99B,_tag_dyneither(_tmp22E,sizeof(void*),1U));});});});}_LL0:;}
# 990
static int Cyc_Toc_is_void_star(void*t){
void*_tmp237=Cyc_Tcutil_compress(t);void*_tmp238=_tmp237;void*_tmp23B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp238)->tag == 5U){_LL1: _tmp23B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp238)->f1).elt_typ;_LL2: {
# 993
void*_tmp239=Cyc_Tcutil_compress(_tmp23B);void*_tmp23A=_tmp239;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp23A)->tag == 0U){_LL6: _LL7:
 return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 997
 return 0;}_LL0:;}
# 1001
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1005
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1010
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp23C=Cyc_Tcutil_compress(t);void*_tmp23D=_tmp23C;struct Cyc_List_List*_tmp249;union Cyc_Absyn_AggrInfoU _tmp248;switch(*((int*)_tmp23D)){case 11U: _LL1: _tmp248=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp23D)->f1).aggr_info;_LL2: {
# 1013
struct Cyc_Absyn_Aggrdecl*_tmp23E=Cyc_Absyn_get_known_aggrdecl(_tmp248);
if(_tmp23E->impl == 0)
({void*_tmp23F=0U;({struct _dyneither_ptr _tmp99C=({const char*_tmp240="is_poly_field: type missing fields";_tag_dyneither(_tmp240,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp99C,_tag_dyneither(_tmp23F,sizeof(void*),0U));});});
_tmp249=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->fields;goto _LL4;}case 12U: _LL3: _tmp249=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23D)->f2;_LL4: {
# 1018
struct Cyc_Absyn_Aggrfield*_tmp241=Cyc_Absyn_lookup_field(_tmp249,f);
if(_tmp241 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp244;_tmp244.tag=0U;_tmp244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp242[1U]={& _tmp244};({struct _dyneither_ptr _tmp99D=({const char*_tmp243="is_poly_field: bad field %s";_tag_dyneither(_tmp243,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp99D,_tag_dyneither(_tmp242,sizeof(void*),1U));});});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp241->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp247;_tmp247.tag=0U;({struct _dyneither_ptr _tmp99E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp247.f1=_tmp99E;});({void*_tmp245[1U]={& _tmp247};({struct _dyneither_ptr _tmp99F=({const char*_tmp246="is_poly_field: bad type %s";_tag_dyneither(_tmp246,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp99F,_tag_dyneither(_tmp245,sizeof(void*),1U));});});});}_LL0:;}
# 1029
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp24A=e->r;void*_tmp24B=_tmp24A;struct Cyc_Absyn_Exp*_tmp255;struct _dyneither_ptr*_tmp254;struct Cyc_Absyn_Exp*_tmp253;struct _dyneither_ptr*_tmp252;switch(*((int*)_tmp24B)){case 21U: _LL1: _tmp253=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp24B)->f1;_tmp252=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp24B)->f2;_LL2:
# 1032
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp253->topt),_tmp252) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp255=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp24B)->f1;_tmp254=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp24B)->f2;_LL4: {
# 1035
void*_tmp24C=Cyc_Tcutil_compress((void*)_check_null(_tmp255->topt));void*_tmp24D=_tmp24C;void*_tmp251;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24D)->tag == 5U){_LL8: _tmp251=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24D)->f1).elt_typ;_LL9:
# 1037
 return Cyc_Toc_is_poly_field(_tmp251,_tmp254) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp250;_tmp250.tag=0U;({struct _dyneither_ptr _tmp9A0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp255->topt)));_tmp250.f1=_tmp9A0;});({void*_tmp24E[1U]={& _tmp250};({struct _dyneither_ptr _tmp9A1=({const char*_tmp24F="is_poly_project: bad type %s";_tag_dyneither(_tmp24F,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9A1,_tag_dyneither(_tmp24E,sizeof(void*),1U));});});});}_LL7:;}default: _LL5: _LL6:
# 1040
 return 0;}_LL0:;}
# 1045
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp9A2=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp9A2,({struct Cyc_List_List*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256->hd=s;_tmp256->tl=0;_tmp256;}),0U);});}
# 1049
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp9A3=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp9A3,({struct Cyc_List_List*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257->hd=s;_tmp257->tl=0;_tmp257;}),0U);});}
# 1053
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1059
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp9A4=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp9A4,({struct Cyc_Absyn_Exp*_tmp258[2U];_tmp258[1U]=s;_tmp258[0U]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp258,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1063
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp9A5=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp9A5,({struct Cyc_Absyn_Exp*_tmp259[2U];_tmp259[1U]=s;_tmp259[0U]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp259,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1067
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return({struct Cyc_Absyn_Exp*_tmp9A6=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp9A6,({struct Cyc_Absyn_Exp*_tmp25A[2U];_tmp25A[1U]=n;_tmp25A[0U]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25A,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1071
return({struct Cyc_Absyn_Exp*_tmp9A7=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp9A7,({struct Cyc_Absyn_Exp*_tmp25B[2U];_tmp25B[1U]=n;_tmp25B[0U]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25B,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1074
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp9A8=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp9A8,({struct Cyc_Absyn_Exp*_tmp25C[3U];_tmp25C[2U]=n;_tmp25C[1U]=s;_tmp25C[0U]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25C,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1079
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp9A9=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp9A9,({struct Cyc_List_List*_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D->hd=e;_tmp25D->tl=0;_tmp25D;}),0U);});}
# 1082
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp9AA=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp9AA,({struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->hd=e;_tmp25E->tl=0;_tmp25E;}),0U);});}
# 1086
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp9AB=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp9AB,({struct Cyc_Absyn_Exp*_tmp25F[0U];((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25F,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1092
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1100
int is_string=0;
{void*_tmp260=e->r;void*_tmp261=_tmp260;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp261)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp261)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4:
 is_string=1;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 1106
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp9AE=({struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));({struct Cyc_Absyn_Decl*_tmp9AD=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp263=_cycalloc(sizeof(*_tmp263));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp9AC=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp264;_tmp264.tag=0U;_tmp264.f1=vd;_tmp264;});_tmp263[0]=_tmp9AC;});_tmp263;}),0U);_tmp262->hd=_tmp9AD;});_tmp262->tl=Cyc_Toc_result_decls;_tmp262;});Cyc_Toc_result_decls=_tmp9AE;});
({struct Cyc_Absyn_Exp*_tmp9AF=Cyc_Absyn_var_exp(x,0U);xexp=_tmp9AF;});
({struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp9B0;});}else{
# 1116
({struct Cyc_Absyn_Exp*_tmp9B2=({void*_tmp9B1=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp9B1,e);});xexp=_tmp9B2;});
# 1118
({struct Cyc_Absyn_Exp*_tmp9B4=({void*_tmp9B3=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp9B3,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp9B4;});}
# 1120
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp265[3U];({struct _tuple19*_tmp9B5=({struct _tuple19*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->f1=0;_tmp268->f2=xplussz;_tmp268;});_tmp265[2U]=_tmp9B5;});({struct _tuple19*_tmp9B6=({struct _tuple19*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->f1=0;_tmp267->f2=xexp;_tmp267;});_tmp265[1U]=_tmp9B6;});({struct _tuple19*_tmp9B7=({struct _tuple19*_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266->f1=0;_tmp266->f2=xexp;_tmp266;});_tmp265[0U]=_tmp9B7;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp265,sizeof(struct _tuple19*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1163 "toc.cyc"
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp269=nv;int _tmp26A;_LL1: _tmp26A=_tmp269->toplevel;_LL2:;
return _tmp26A;}
# 1167
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp26B=nv;int*_tmp26C;_LL1: _tmp26C=_tmp26B->in_lhs;_LL2:;
return*_tmp26C;}
# 1173
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp26D=_region_malloc(r,sizeof(*_tmp26D));_tmp26D->break_lab=(struct _dyneither_ptr**)0;_tmp26D->continue_lab=(struct _dyneither_ptr**)0;_tmp26D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp26D->toplevel=(int)1;({typeof((int*)({int*_tmp270=_region_malloc(r,sizeof(*_tmp270));_tmp270[0]=0;_tmp270;}))_tmp9B8=(int*)({int*_tmp270=_region_malloc(r,sizeof(*_tmp270));_tmp270[0]=0;_tmp270;});_tmp26D->in_lhs=_tmp9B8;});({typeof((struct Cyc_Toc_StructInfo)({({struct Cyc_Absyn_Exp**_tmp9B9=({struct Cyc_Absyn_Exp**_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=0;_tmp26F;});(_tmp26D->struct_info).varsizeexp=_tmp9B9;});(_tmp26D->struct_info).lhs_exp=0;_tmp26D->struct_info;}))_tmp9BA=(struct Cyc_Toc_StructInfo)({({struct Cyc_Absyn_Exp**_tmp9B9=({struct Cyc_Absyn_Exp**_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=0;_tmp26F;});(_tmp26D->struct_info).varsizeexp=_tmp9B9;});(_tmp26D->struct_info).lhs_exp=0;_tmp26D->struct_info;});_tmp26D->struct_info=_tmp9BA;});({typeof((int*)({int*_tmp26E=_region_malloc(r,sizeof(*_tmp26E));_tmp26E[0]=0;_tmp26E;}))_tmp9BB=(int*)({int*_tmp26E=_region_malloc(r,sizeof(*_tmp26E));_tmp26E[0]=0;_tmp26E;});_tmp26D->in_sizeof=_tmp9BB;});_tmp26D->rgn=(struct _RegionHandle*)r;_tmp26D;});}
# 1184
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp271=e;struct _dyneither_ptr**_tmp279;struct _dyneither_ptr**_tmp278;struct Cyc_Toc_FallthruInfo*_tmp277;int _tmp276;int*_tmp275;struct Cyc_Toc_StructInfo _tmp274;int*_tmp273;_LL1: _tmp279=_tmp271->break_lab;_tmp278=_tmp271->continue_lab;_tmp277=_tmp271->fallthru_info;_tmp276=_tmp271->toplevel;_tmp275=_tmp271->in_lhs;_tmp274=_tmp271->struct_info;_tmp273=_tmp271->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp272=_region_malloc(r,sizeof(*_tmp272));_tmp272->break_lab=(struct _dyneither_ptr**)_tmp279;_tmp272->continue_lab=(struct _dyneither_ptr**)_tmp278;_tmp272->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp277;_tmp272->toplevel=(int)_tmp276;_tmp272->in_lhs=(int*)_tmp275;_tmp272->struct_info=(struct Cyc_Toc_StructInfo)_tmp274;_tmp272->in_sizeof=(int*)_tmp273;_tmp272->rgn=(struct _RegionHandle*)r;_tmp272;});}
# 1189
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp27A=e;struct _dyneither_ptr**_tmp282;struct _dyneither_ptr**_tmp281;struct Cyc_Toc_FallthruInfo*_tmp280;int _tmp27F;int*_tmp27E;struct Cyc_Toc_StructInfo _tmp27D;int*_tmp27C;_LL1: _tmp282=_tmp27A->break_lab;_tmp281=_tmp27A->continue_lab;_tmp280=_tmp27A->fallthru_info;_tmp27F=_tmp27A->toplevel;_tmp27E=_tmp27A->in_lhs;_tmp27D=_tmp27A->struct_info;_tmp27C=_tmp27A->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp27B=_region_malloc(r,sizeof(*_tmp27B));_tmp27B->break_lab=(struct _dyneither_ptr**)_tmp282;_tmp27B->continue_lab=(struct _dyneither_ptr**)_tmp281;_tmp27B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp280;_tmp27B->toplevel=(int)0;_tmp27B->in_lhs=(int*)_tmp27E;_tmp27B->struct_info=(struct Cyc_Toc_StructInfo)_tmp27D;_tmp27B->in_sizeof=(int*)_tmp27C;_tmp27B->rgn=(struct _RegionHandle*)r;_tmp27B;});}
# 1193
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp283=e;struct _dyneither_ptr**_tmp28B;struct _dyneither_ptr**_tmp28A;struct Cyc_Toc_FallthruInfo*_tmp289;int _tmp288;int*_tmp287;struct Cyc_Toc_StructInfo _tmp286;int*_tmp285;_LL1: _tmp28B=_tmp283->break_lab;_tmp28A=_tmp283->continue_lab;_tmp289=_tmp283->fallthru_info;_tmp288=_tmp283->toplevel;_tmp287=_tmp283->in_lhs;_tmp286=_tmp283->struct_info;_tmp285=_tmp283->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp284=_region_malloc(r,sizeof(*_tmp284));_tmp284->break_lab=(struct _dyneither_ptr**)_tmp28B;_tmp284->continue_lab=(struct _dyneither_ptr**)_tmp28A;_tmp284->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp289;_tmp284->toplevel=(int)1;_tmp284->in_lhs=(int*)_tmp287;_tmp284->struct_info=(struct Cyc_Toc_StructInfo)_tmp286;_tmp284->in_sizeof=(int*)_tmp285;_tmp284->rgn=(struct _RegionHandle*)r;_tmp284;});}
# 1197
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp28C=e;int*_tmp28D;_LL1: _tmp28D=_tmp28C->in_lhs;_LL2:;
*_tmp28D=b;}
# 1201
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp28E=e;int*_tmp290;_LL1: _tmp290=_tmp28E->in_sizeof;_LL2:;{
int _tmp28F=*_tmp290;
*_tmp290=b;
return _tmp28F;};}
# 1207
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp291=e;int*_tmp292;_LL1: _tmp292=_tmp291->in_sizeof;_LL2:;
return*_tmp292;}
# 1212
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1214
struct Cyc_Toc_Env*_tmp293=e;struct _dyneither_ptr**_tmp29B;struct _dyneither_ptr**_tmp29A;struct Cyc_Toc_FallthruInfo*_tmp299;int _tmp298;int*_tmp297;struct Cyc_Absyn_Exp**_tmp296;int*_tmp295;_LL1: _tmp29B=_tmp293->break_lab;_tmp29A=_tmp293->continue_lab;_tmp299=_tmp293->fallthru_info;_tmp298=_tmp293->toplevel;_tmp297=_tmp293->in_lhs;_tmp296=(_tmp293->struct_info).varsizeexp;_tmp295=_tmp293->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp294=_region_malloc(r,sizeof(*_tmp294));_tmp294->break_lab=(struct _dyneither_ptr**)_tmp29B;_tmp294->continue_lab=(struct _dyneither_ptr**)_tmp29A;_tmp294->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp299;_tmp294->toplevel=(int)_tmp298;_tmp294->in_lhs=(int*)_tmp297;({typeof((struct Cyc_Toc_StructInfo)({(_tmp294->struct_info).varsizeexp=_tmp296;(_tmp294->struct_info).lhs_exp=exp;_tmp294->struct_info;}))_tmp9BC=(struct Cyc_Toc_StructInfo)({(_tmp294->struct_info).varsizeexp=_tmp296;(_tmp294->struct_info).lhs_exp=exp;_tmp294->struct_info;});_tmp294->struct_info=_tmp9BC;});_tmp294->in_sizeof=(int*)_tmp295;_tmp294->rgn=(struct _RegionHandle*)r;_tmp294;});}
# 1220
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp29C=e;struct _dyneither_ptr**_tmp2A4;struct _dyneither_ptr**_tmp2A3;struct Cyc_Toc_FallthruInfo*_tmp2A2;int _tmp2A1;int*_tmp2A0;struct Cyc_Toc_StructInfo _tmp29F;int*_tmp29E;_LL1: _tmp2A4=_tmp29C->break_lab;_tmp2A3=_tmp29C->continue_lab;_tmp2A2=_tmp29C->fallthru_info;_tmp2A1=_tmp29C->toplevel;_tmp2A0=_tmp29C->in_lhs;_tmp29F=_tmp29C->struct_info;_tmp29E=_tmp29C->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp29D=_region_malloc(r,sizeof(*_tmp29D));_tmp29D->break_lab=(struct _dyneither_ptr**)0;_tmp29D->continue_lab=(struct _dyneither_ptr**)0;_tmp29D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2A2;_tmp29D->toplevel=(int)_tmp2A1;_tmp29D->in_lhs=(int*)_tmp2A0;_tmp29D->struct_info=(struct Cyc_Toc_StructInfo)_tmp29F;_tmp29D->in_sizeof=(int*)_tmp29E;_tmp29D->rgn=(struct _RegionHandle*)r;_tmp29D;});}
# 1226
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1230
struct Cyc_Toc_Env*_tmp2A5=e;struct _dyneither_ptr**_tmp2AF;struct _dyneither_ptr**_tmp2AE;struct Cyc_Toc_FallthruInfo*_tmp2AD;int _tmp2AC;int*_tmp2AB;struct Cyc_Toc_StructInfo _tmp2AA;int*_tmp2A9;_LL1: _tmp2AF=_tmp2A5->break_lab;_tmp2AE=_tmp2A5->continue_lab;_tmp2AD=_tmp2A5->fallthru_info;_tmp2AC=_tmp2A5->toplevel;_tmp2AB=_tmp2A5->in_lhs;_tmp2AA=_tmp2A5->struct_info;_tmp2A9=_tmp2A5->in_sizeof;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp2A8=_region_malloc(r,sizeof(*_tmp2A8));_tmp2A8->label=fallthru_l;_tmp2A8->binders=fallthru_binders;_tmp2A8;});
# 1233
return({struct Cyc_Toc_Env*_tmp2A6=_region_malloc(r,sizeof(*_tmp2A6));({typeof((struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2A7=_region_malloc(r,sizeof(*_tmp2A7));_tmp2A7[0]=break_l;_tmp2A7;}))_tmp9BD=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2A7=_region_malloc(r,sizeof(*_tmp2A7));_tmp2A7[0]=break_l;_tmp2A7;});_tmp2A6->break_lab=_tmp9BD;});_tmp2A6->continue_lab=(struct _dyneither_ptr**)_tmp2AE;_tmp2A6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi;_tmp2A6->toplevel=(int)_tmp2AC;_tmp2A6->in_lhs=(int*)_tmp2AB;_tmp2A6->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AA;_tmp2A6->in_sizeof=(int*)_tmp2A9;_tmp2A6->rgn=(struct _RegionHandle*)r;_tmp2A6;});};}
# 1236
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1238
struct Cyc_Toc_Env*_tmp2B0=e;struct _dyneither_ptr**_tmp2B9;struct _dyneither_ptr**_tmp2B8;struct Cyc_Toc_FallthruInfo*_tmp2B7;int _tmp2B6;int*_tmp2B5;struct Cyc_Toc_StructInfo _tmp2B4;int*_tmp2B3;_LL1: _tmp2B9=_tmp2B0->break_lab;_tmp2B8=_tmp2B0->continue_lab;_tmp2B7=_tmp2B0->fallthru_info;_tmp2B6=_tmp2B0->toplevel;_tmp2B5=_tmp2B0->in_lhs;_tmp2B4=_tmp2B0->struct_info;_tmp2B3=_tmp2B0->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp2B1=_region_malloc(r,sizeof(*_tmp2B1));({typeof((struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2B2=_region_malloc(r,sizeof(*_tmp2B2));_tmp2B2[0]=break_l;_tmp2B2;}))_tmp9BE=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2B2=_region_malloc(r,sizeof(*_tmp2B2));_tmp2B2[0]=break_l;_tmp2B2;});_tmp2B1->break_lab=_tmp9BE;});_tmp2B1->continue_lab=(struct _dyneither_ptr**)_tmp2B8;_tmp2B1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp2B1->toplevel=(int)_tmp2B6;_tmp2B1->in_lhs=(int*)_tmp2B5;_tmp2B1->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B4;_tmp2B1->in_sizeof=(int*)_tmp2B3;_tmp2B1->rgn=(struct _RegionHandle*)r;_tmp2B1;});}
# 1245
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1248
struct Cyc_Toc_Env*_tmp2BA=e;struct _dyneither_ptr**_tmp2C3;struct _dyneither_ptr**_tmp2C2;struct Cyc_Toc_FallthruInfo*_tmp2C1;int _tmp2C0;int*_tmp2BF;struct Cyc_Toc_StructInfo _tmp2BE;int*_tmp2BD;_LL1: _tmp2C3=_tmp2BA->break_lab;_tmp2C2=_tmp2BA->continue_lab;_tmp2C1=_tmp2BA->fallthru_info;_tmp2C0=_tmp2BA->toplevel;_tmp2BF=_tmp2BA->in_lhs;_tmp2BE=_tmp2BA->struct_info;_tmp2BD=_tmp2BA->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp2BB=_region_malloc(r,sizeof(*_tmp2BB));_tmp2BB->break_lab=(struct _dyneither_ptr**)0;_tmp2BB->continue_lab=(struct _dyneither_ptr**)_tmp2C2;({typeof((struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2BC=_region_malloc(r,sizeof(*_tmp2BC));_tmp2BC->label=next_l;_tmp2BC->binders=0;_tmp2BC;}))_tmp9BF=(struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2BC=_region_malloc(r,sizeof(*_tmp2BC));_tmp2BC->label=next_l;_tmp2BC->binders=0;_tmp2BC;});_tmp2BB->fallthru_info=_tmp9BF;});_tmp2BB->toplevel=(int)_tmp2C0;_tmp2BB->in_lhs=(int*)_tmp2BF;_tmp2BB->struct_info=(struct Cyc_Toc_StructInfo)_tmp2BE;_tmp2BB->in_sizeof=(int*)_tmp2BD;_tmp2BB->rgn=(struct _RegionHandle*)r;_tmp2BB;});}
# 1261 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1264
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1266
void*_tmp2C4=e->annot;void*_tmp2C5=_tmp2C4;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2C5)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _LL2: {
# 1269
void*_tmp2C6=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2C7=_tmp2C6;union Cyc_Absyn_Constraint*_tmp2CA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7)->tag == 5U){_LLC: _tmp2CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7)->f1).ptr_atts).nullable;_LLD:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2CA);}else{_LLE: _LLF:
({void*_tmp2C8=0U;({struct _dyneither_ptr _tmp9C0=({const char*_tmp2C9="Toc::is_nullable";_tag_dyneither(_tmp2C9,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9C0,_tag_dyneither(_tmp2C8,sizeof(void*),0U));});});}_LLB:;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2C5)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _LL4:
# 1275
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2C5)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1277
({void*_tmp2CB=0U;({unsigned int _tmp9C2=e->loc;struct _dyneither_ptr _tmp9C1=({const char*_tmp2CC="NULL pointer check will definitely fail";_tag_dyneither(_tmp2CC,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9C2,_tmp9C1,_tag_dyneither(_tmp2CB,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2C5)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
# 1283
 return 1;}else{_LL9: _LLA:
({void*_tmp2CD=0U;({struct _dyneither_ptr _tmp9C3=({const char*_tmp2CE="need_null_check";_tag_dyneither(_tmp2CE,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9C3,_tag_dyneither(_tmp2CD,sizeof(void*),0U));});});}}}}_LL0:;}
# 1288
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2CF=e->annot;void*_tmp2D0=_tmp2CF;struct Cyc_List_List*_tmp2D6;struct Cyc_List_List*_tmp2D5;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp2D5=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2D0)->f1;_LL2:
 return _tmp2D5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp2D6=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2D0)->f1;_LL4:
 return _tmp2D6;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1293
({void*_tmp2D1=0U;({unsigned int _tmp9C5=e->loc;struct _dyneither_ptr _tmp9C4=({const char*_tmp2D2="dereference of NULL pointer";_tag_dyneither(_tmp2D2,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp9C5,_tmp9C4,_tag_dyneither(_tmp2D1,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
({void*_tmp2D3=0U;({struct _dyneither_ptr _tmp9C6=({const char*_tmp2D4="get_relns";_tag_dyneither(_tmp2D4,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9C6,_tag_dyneither(_tmp2D3,sizeof(void*),0U));});});}}}}_LL0:;}static char _tmp2DA[8U]="*bogus*";
# 1303
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1313 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1316
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2D7=a->r;void*_tmp2D8=_tmp2D7;void*_tmp2DB;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D8)->tag == 1U){_LL1: _tmp2DB=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL2: {
# 1319
struct Cyc_Absyn_Vardecl*_tmp2D9=Cyc_Tcutil_nonesc_vardecl(_tmp2DB);
if(_tmp2D9 == 0)goto _LL4;
x=_tmp2D9;
goto _LL0;}}else{_LL3: _LL4: {
# 1325
static struct _dyneither_ptr bogus_string={_tmp2DA,_tmp2DA,_tmp2DA + 8U};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1328
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1334
x=& bogus_vardecl;
x->type=a_typ;}}_LL0:;}{
# 1337
void*_tmp2DC=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1342
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2DC);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
({struct Cyc_List_List*_tmp9C7=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);relns=_tmp9C7;});}
# 1351
if(valid_rop_i){
struct Cyc_List_List*_tmp2DD=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1359
if(!Cyc_Relations_consistent_relations(_tmp2DD))return 1;}
# 1362
inner_loop: {
void*_tmp2DE=i->r;void*_tmp2DF=_tmp2DE;struct Cyc_Absyn_Exp*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E3;void*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E1;switch(*((int*)_tmp2DF)){case 14U: _LL6: _tmp2E2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp2E1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL7:
 i=_tmp2E1;goto inner_loop;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2)->tl != 0){_LL8: _tmp2E4=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2)->hd;_tmp2E3=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2)->tl)->hd;_LL9: {
# 1368
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp2E3,& rop_z)){
# 1371
struct Cyc_List_List*_tmp2E0=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1373
return !Cyc_Relations_consistent_relations(_tmp2E0);}
# 1375
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1378
return 0;};}
# 1381
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2E5=0U;({struct _dyneither_ptr _tmp9C8=({const char*_tmp2E6="Missing type in primop ";_tag_dyneither(_tmp2E6,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9C8,_tag_dyneither(_tmp2E5,sizeof(void*),0U));});});
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1385
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2E7=0U;({struct _dyneither_ptr _tmp9C9=({const char*_tmp2E8="Missing type in primop ";_tag_dyneither(_tmp2E8,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9C9,_tag_dyneither(_tmp2E7,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1389
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple11*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9->f1=Cyc_Toc_mt_tq;({void*_tmp9CA=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp2E9->f2=_tmp9CA;});_tmp2E9;});}
# 1392
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);
return({struct _tuple19*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA->f1=0;_tmp2EA->f2=e;_tmp2EA;});}
# 1399
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1402
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2EB=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0U),0U);
if(*(nv->struct_info).varsizeexp != 0){
({struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Absyn_add_exp(_tmp2EB,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0U);_tmp2EB=_tmp9CB;});
*(nv->struct_info).varsizeexp=0;}
# 1410
({void*_tmp9CC=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);t=_tmp9CC;});
if(rgnopt == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp9CD=is_atomic?Cyc_Toc_malloc_atomic(_tmp2EB): Cyc_Toc_malloc_ptr(_tmp2EB);eo=_tmp9CD;});else{
# 1414
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp9CE=Cyc_Toc_rmalloc_exp(r,_tmp2EB);eo=_tmp9CE;});}}else{
# 1419
t=struct_typ;
eo=0;}{
# 1422
struct Cyc_Absyn_Exp*res;
if(do_declare){
void*_tmp2EC=x->r;void*_tmp2ED=_tmp2EC;void*_tmp2F0;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2ED)->tag == 1U){_LL1: _tmp2F0=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1;_LL2:
# 1426
({struct Cyc_Absyn_Exp*_tmp9D2=Cyc_Absyn_stmt_exp(({struct _tuple1*_tmp9D1=Cyc_Absyn_binding2qvar(_tmp2F0);void*_tmp9D0=t;struct Cyc_Absyn_Exp*_tmp9CF=eo;Cyc_Absyn_declare_stmt(_tmp9D1,_tmp9D0,_tmp9CF,s,0U);}),0U);res=_tmp9D2;});
res->topt=t;
return res;}else{_LL3: _LL4:
({void*_tmp2EE=0U;({struct _dyneither_ptr _tmp9D3=({const char*_tmp2EF="bogus x to make_struct";_tag_dyneither(_tmp2EF,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D3,_tag_dyneither(_tmp2EE,sizeof(void*),0U));});});}_LL0:;}else{
# 1433
if(eo != 0)
({struct Cyc_Absyn_Exp*_tmp9D5=Cyc_Absyn_stmt_exp(({struct Cyc_Absyn_Stmt*_tmp9D4=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0U),0U);Cyc_Absyn_seq_stmt(_tmp9D4,s,0U);}),0U);res=_tmp9D5;});else{
# 1436
({struct Cyc_Absyn_Exp*_tmp9D6=Cyc_Absyn_stmt_exp(s,0U);res=_tmp9D6;});}
res->topt=t;
return res;}};}
# 1442
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp2F1=Cyc_Tcutil_compress(typ);void*_tmp2F2=_tmp2F1;struct Cyc_List_List*_tmp2F5;union Cyc_Absyn_AggrInfoU _tmp2F4;struct Cyc_List_List*_tmp2F3;switch(*((int*)_tmp2F2)){case 11U: _LL1: _tmp2F4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F2)->f1).aggr_info;_tmp2F3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F2)->f1).targs;_LL2: {
# 1445
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp2F4);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp2F5=aggrfields;goto _LL4;};}case 12U: _LL3: _tmp2F5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F2)->f2;_LL4:
# 1450
 if(_tmp2F5 == 0)return 0;
return 1;default: _LL5: _LL6:
# 1456
 return 0;}_LL0:;}
# 1460
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp2F6=Cyc_Tcutil_compress(t);void*_tmp2F7=_tmp2F6;void*_tmp2FB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F7)->tag == 8U){_LL1: _tmp2FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F7)->f1).elt_type;_LL2:
 return _tmp2FB;}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp2FA;_tmp2FA.tag=0U;({struct _dyneither_ptr _tmp9D7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp2FA.f1=_tmp9D7;});({void*_tmp2F8[1U]={& _tmp2FA};({struct _dyneither_ptr _tmp9D8=({const char*_tmp2F9="array_elt_type called on %s";_tag_dyneither(_tmp2F9,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D8,_tag_dyneither(_tmp2F8,sizeof(void*),1U));});});});}_LL0:;}
# 1467
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1473
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1478
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1483
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp2FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp2FC != 0;_tmp2FC=_tmp2FC->tl){
# 1487
struct _tuple19*_tmp2FD=(struct _tuple19*)_tmp2FC->hd;struct _tuple19*_tmp2FE=_tmp2FD;struct Cyc_List_List*_tmp310;struct Cyc_Absyn_Exp*_tmp30F;_LL1: _tmp310=_tmp2FE->f1;_tmp30F=_tmp2FE->f2;_LL2:;{
# 1491
struct Cyc_Absyn_Exp*e_index;
if(_tmp310 == 0)
({struct Cyc_Absyn_Exp*_tmp9D9=Cyc_Absyn_signed_int_exp(count --,0U);e_index=_tmp9D9;});else{
# 1495
if(_tmp310->tl != 0)({void*_tmp2FF=0U;({struct _dyneither_ptr _tmp9DA=({const char*_tmp300="multiple designators in array";_tag_dyneither(_tmp300,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp9DA,_tag_dyneither(_tmp2FF,sizeof(void*),0U));});});{
void*_tmp301=(void*)_tmp310->hd;
void*_tmp302=_tmp301;struct Cyc_Absyn_Exp*_tmp305;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp302)->tag == 0U){_LL4: _tmp305=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp302)->f1;_LL5:
# 1499
 Cyc_Toc_exp_to_c(nv,_tmp305);
e_index=_tmp305;
goto _LL3;}else{_LL6: _LL7:
({void*_tmp303=0U;({struct _dyneither_ptr _tmp9DB=({const char*_tmp304="field name designators in array";_tag_dyneither(_tmp304,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp9DB,_tag_dyneither(_tmp303,sizeof(void*),0U));});});}_LL3:;};}{
# 1505
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0U);
void*_tmp306=_tmp30F->r;void*_tmp307=_tmp306;void*_tmp30E;struct Cyc_List_List*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30C;struct Cyc_Absyn_Exp*_tmp30B;struct Cyc_Absyn_Exp*_tmp30A;int _tmp309;struct Cyc_List_List*_tmp308;switch(*((int*)_tmp307)){case 26U: _LL9: _tmp308=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp307)->f1;_LLA:
# 1508
({struct Cyc_Absyn_Stmt*_tmp9DC=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp308,s);s=_tmp9DC;});
goto _LL8;case 27U: _LLB: _tmp30C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f1;_tmp30B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f2;_tmp30A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f3;_tmp309=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f4;_LLC:
# 1511
({struct Cyc_Absyn_Stmt*_tmp9DD=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp30C,_tmp30B,_tmp30A,_tmp309,s,0);s=_tmp9DD;});
# 1513
goto _LL8;case 30U: _LLD: _tmp30E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp307)->f1;_tmp30D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp307)->f2;_LLE:
# 1515
({struct Cyc_Absyn_Stmt*_tmp9DE=Cyc_Toc_init_anon_struct(nv,lval,_tmp30E,_tmp30D,s);s=_tmp9DE;});
goto _LL8;case 28U: _LLF: _LL10:
# 1518
 goto _LL8;default: _LL11: _LL12:
# 1520
 Cyc_Toc_exp_to_c(nv,_tmp30F);
({struct Cyc_Absyn_Stmt*_tmp9E2=({struct Cyc_Absyn_Stmt*_tmp9E1=({struct Cyc_Absyn_Exp*_tmp9E0=Cyc_Absyn_subscript_exp(lhs,e_index,0U);Cyc_Absyn_assign_stmt(_tmp9E0,({
void*_tmp9DF=Cyc_Toc_typ_to_c(orig_elt_type);Cyc_Toc_cast_it(_tmp9DF,_tmp30F);}),0U);});
# 1521
Cyc_Absyn_seq_stmt(_tmp9E1,s,0U);});s=_tmp9E2;});
# 1523
goto _LL8;}_LL8:;};};}}
# 1526
return s;}
# 1531
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1536
struct _tuple1*_tmp311=vd->name;
void*expected_elt_type;
{void*_tmp312=Cyc_Tcutil_compress(comprehension_type);void*_tmp313=_tmp312;void*_tmp317;void*_tmp316;switch(*((int*)_tmp313)){case 8U: _LL1: _tmp316=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313)->f1).elt_type;_LL2:
# 1540
 expected_elt_type=_tmp316;goto _LL0;case 5U: _LL3: _tmp317=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313)->f1).elt_typ;_LL4:
# 1542
 expected_elt_type=_tmp317;goto _LL0;default: _LL5: _LL6:
({void*_tmp314=0U;({struct _dyneither_ptr _tmp9E3=({const char*_tmp315="init_comprehension passed a bad type!";_tag_dyneither(_tmp315,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E3,_tag_dyneither(_tmp314,sizeof(void*),0U));});});}_LL0:;}{
# 1545
void*_tmp318=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1549
{void*_tmp319=e2->r;void*_tmp31A=_tmp319;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp31A)->tag == 28U){_LL8: _LL9:
 return Cyc_Absyn_skip_stmt(0U);}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}{
# 1553
struct _RegionHandle _tmp31B=_new_region("r2");struct _RegionHandle*r2=& _tmp31B;_push_region(r2);
{struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=({struct Cyc_Absyn_Exp*_tmp9E4=Cyc_Absyn_var_exp(_tmp311,0U);Cyc_Absyn_assign_exp(_tmp9E4,Cyc_Absyn_signed_int_exp(0,0U),0U);});
struct Cyc_Absyn_Exp*eb=({struct Cyc_Absyn_Exp*_tmp9E5=Cyc_Absyn_var_exp(_tmp311,0U);Cyc_Absyn_lt_exp(_tmp9E5,Cyc_Absyn_var_exp(max,0U),0U);});
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp311,0U),0U);
# 1560
struct Cyc_Absyn_Exp*lval=({struct Cyc_Absyn_Exp*_tmp9E6=lhs;Cyc_Absyn_subscript_exp(_tmp9E6,Cyc_Absyn_var_exp(_tmp311,0U),0U);});
struct Cyc_Absyn_Stmt*body;
# 1564
{void*_tmp31C=e2->r;void*_tmp31D=_tmp31C;void*_tmp324;struct Cyc_List_List*_tmp323;struct Cyc_Absyn_Vardecl*_tmp322;struct Cyc_Absyn_Exp*_tmp321;struct Cyc_Absyn_Exp*_tmp320;int _tmp31F;struct Cyc_List_List*_tmp31E;switch(*((int*)_tmp31D)){case 26U: _LLD: _tmp31E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp31D)->f1;_LLE:
# 1566
({struct Cyc_Absyn_Stmt*_tmp9EB=({struct Cyc_Toc_Env*_tmp9EA=nv;void*_tmp9E9=expected_elt_type;struct Cyc_Absyn_Exp*_tmp9E8=lval;struct Cyc_List_List*_tmp9E7=_tmp31E;Cyc_Toc_init_array(_tmp9EA,_tmp9E9,_tmp9E8,_tmp9E7,Cyc_Toc_skip_stmt_dl());});body=_tmp9EB;});
goto _LLC;case 27U: _LLF: _tmp322=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f1;_tmp321=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f2;_tmp320=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f3;_tmp31F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f4;_LL10:
# 1569
({struct Cyc_Absyn_Stmt*_tmp9F3=({struct Cyc_Toc_Env*_tmp9F2=nv;void*_tmp9F1=expected_elt_type;struct Cyc_Absyn_Exp*_tmp9F0=lval;struct Cyc_Absyn_Vardecl*_tmp9EF=_tmp322;struct Cyc_Absyn_Exp*_tmp9EE=_tmp321;struct Cyc_Absyn_Exp*_tmp9ED=_tmp320;int _tmp9EC=_tmp31F;Cyc_Toc_init_comprehension(_tmp9F2,_tmp9F1,_tmp9F0,_tmp9EF,_tmp9EE,_tmp9ED,_tmp9EC,Cyc_Toc_skip_stmt_dl(),0);});body=_tmp9F3;});
goto _LLC;case 30U: _LL11: _tmp324=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp31D)->f1;_tmp323=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp31D)->f2;_LL12:
# 1572
({struct Cyc_Absyn_Stmt*_tmp9F8=({struct Cyc_Toc_Env*_tmp9F7=nv;struct Cyc_Absyn_Exp*_tmp9F6=lval;void*_tmp9F5=_tmp324;struct Cyc_List_List*_tmp9F4=_tmp323;Cyc_Toc_init_anon_struct(_tmp9F7,_tmp9F6,_tmp9F5,_tmp9F4,Cyc_Toc_skip_stmt_dl());});body=_tmp9F8;});
goto _LLC;default: _LL13: _LL14:
# 1575
 Cyc_Toc_exp_to_c(nv,e2);
({struct Cyc_Absyn_Stmt*_tmp9FB=({struct Cyc_Absyn_Exp*_tmp9FA=lval;Cyc_Absyn_assign_stmt(_tmp9FA,({void*_tmp9F9=Cyc_Toc_typ_to_c(expected_elt_type);Cyc_Toc_cast_it(_tmp9F9,e2);}),0U);});body=_tmp9FB;});
goto _LLC;}_LLC:;}{
# 1579
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0U);
# 1581
if(zero_term){
# 1586
struct Cyc_Absyn_Exp*ex=({struct Cyc_Absyn_Exp*_tmp9FE=({struct Cyc_Absyn_Exp*_tmp9FC=Cyc_Absyn_new_exp(lhs->r,0U);Cyc_Absyn_subscript_exp(_tmp9FC,Cyc_Absyn_var_exp(max,0U),0U);});Cyc_Absyn_assign_exp(_tmp9FE,({
void*_tmp9FD=_tmp318;Cyc_Toc_cast_it(_tmp9FD,Cyc_Absyn_uint_exp(0U,0U));}),0U);});
({struct Cyc_Absyn_Stmt*_tmpA00=({struct Cyc_Absyn_Stmt*_tmp9FF=s2;Cyc_Absyn_seq_stmt(_tmp9FF,Cyc_Absyn_exp_stmt(ex,0U),0U);});s2=_tmpA00;});}{
# 1591
struct Cyc_Absyn_Stmt*_tmp325=({struct Cyc_Absyn_Stmt*_tmpA04=({struct _tuple1*_tmpA03=max;void*_tmpA02=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpA01=e1;Cyc_Absyn_declare_stmt(_tmpA03,_tmpA02,_tmpA01,
Cyc_Absyn_declare_stmt(_tmp311,Cyc_Absyn_uint_typ,0,s2,0U),0U);});
# 1591
Cyc_Absyn_seq_stmt(_tmpA04,s,0U);});_npop_handler(0U);return _tmp325;};};}
# 1554
;_pop_region(r2);};};}
# 1597
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1600
{struct Cyc_List_List*_tmp326=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp326 != 0;_tmp326=_tmp326->tl){
struct _tuple19*_tmp327=(struct _tuple19*)_tmp326->hd;struct _tuple19*_tmp328=_tmp327;struct Cyc_List_List*_tmp33C;struct Cyc_Absyn_Exp*_tmp33B;_LL1: _tmp33C=_tmp328->f1;_tmp33B=_tmp328->f2;_LL2:;
if(_tmp33C == 0)
({void*_tmp329=0U;({struct _dyneither_ptr _tmpA05=({const char*_tmp32A="empty designator list";_tag_dyneither(_tmp32A,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA05,_tag_dyneither(_tmp329,sizeof(void*),0U));});});
if(_tmp33C->tl != 0)
({void*_tmp32B=0U;({struct _dyneither_ptr _tmpA06=({const char*_tmp32C="too many designators in anonymous struct";_tag_dyneither(_tmp32C,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA06,_tag_dyneither(_tmp32B,sizeof(void*),0U));});});{
void*_tmp32D=(void*)_tmp33C->hd;void*_tmp32E=_tmp32D;struct _dyneither_ptr*_tmp33A;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp32E)->tag == 1U){_LL4: _tmp33A=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp32E)->f1;_LL5: {
# 1608
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp33A,0U);
{void*_tmp32F=_tmp33B->r;void*_tmp330=_tmp32F;void*_tmp337;struct Cyc_List_List*_tmp336;struct Cyc_Absyn_Vardecl*_tmp335;struct Cyc_Absyn_Exp*_tmp334;struct Cyc_Absyn_Exp*_tmp333;int _tmp332;struct Cyc_List_List*_tmp331;switch(*((int*)_tmp330)){case 26U: _LL9: _tmp331=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp330)->f1;_LLA:
# 1611
({struct Cyc_Absyn_Stmt*_tmpA07=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp33B->topt),lval,_tmp331,s);s=_tmpA07;});goto _LL8;case 27U: _LLB: _tmp335=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f1;_tmp334=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f2;_tmp333=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f3;_tmp332=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f4;_LLC:
# 1613
({struct Cyc_Absyn_Stmt*_tmpA08=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp33B->topt),lval,_tmp335,_tmp334,_tmp333,_tmp332,s,0);s=_tmpA08;});
goto _LL8;case 30U: _LLD: _tmp337=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp330)->f1;_tmp336=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp330)->f2;_LLE:
# 1616
({struct Cyc_Absyn_Stmt*_tmpA09=Cyc_Toc_init_anon_struct(nv,lval,_tmp337,_tmp336,s);s=_tmpA09;});goto _LL8;case 28U: _LLF: _LL10:
# 1618
 goto _LL8;default: _LL11: _LL12:
# 1620
 Cyc_Toc_exp_to_c(nv,_tmp33B);
# 1622
if(Cyc_Toc_is_poly_field(struct_type,_tmp33A))
({struct Cyc_Absyn_Exp*_tmpA0B=({void*_tmpA0A=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmpA0A,_tmp33B);});_tmp33B=_tmpA0B;});
# 1625
({struct Cyc_Absyn_Stmt*_tmpA0D=({struct Cyc_Absyn_Stmt*_tmpA0C=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp33B,0U),0U);Cyc_Absyn_seq_stmt(_tmpA0C,s,0U);});s=_tmpA0D;});
goto _LL8;}_LL8:;}
# 1628
goto _LL3;}}else{_LL6: _LL7:
({void*_tmp338=0U;({struct _dyneither_ptr _tmpA0E=({const char*_tmp339="array designator in struct";_tag_dyneither(_tmp339,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA0E,_tag_dyneither(_tmp338,sizeof(void*),0U));});});}_LL3:;};}}
# 1632
return s;}
# 1637
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1641
struct _RegionHandle _tmp33D=_new_region("r");struct _RegionHandle*r=& _tmp33D;_push_region(r);
{struct Cyc_List_List*_tmp33E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp33F=Cyc_Toc_add_tuple_type(_tmp33E);
# 1645
struct _tuple1*_tmp340=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp341=Cyc_Absyn_var_exp(_tmp340,0U);
struct Cyc_Absyn_Stmt*_tmp342=Cyc_Absyn_exp_stmt(_tmp341,0U);
# 1649
struct Cyc_Absyn_Exp*(*_tmp343)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1651
int is_atomic=1;
struct Cyc_List_List*_tmp344=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp344);for(0;_tmp344 != 0;(_tmp344=_tmp344->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp344->hd;
struct Cyc_Absyn_Exp*lval=({struct Cyc_Absyn_Exp*(*_tmpA10)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=_tmp343;struct Cyc_Absyn_Exp*_tmpA0F=_tmp341;_tmpA10(_tmpA0F,Cyc_Absyn_fieldname(i),0U);});
({int _tmpA11=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));is_atomic=_tmpA11;});{
void*_tmp345=e->r;void*_tmp346=_tmp345;struct Cyc_Absyn_Vardecl*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;int _tmp348;struct Cyc_List_List*_tmp347;switch(*((int*)_tmp346)){case 26U: _LL1: _tmp347=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp346)->f1;_LL2:
# 1659
({struct Cyc_Absyn_Stmt*_tmpA12=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp347,_tmp342);_tmp342=_tmpA12;});
goto _LL0;case 27U: _LL3: _tmp34B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f1;_tmp34A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f2;_tmp349=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f3;_tmp348=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f4;_LL4:
# 1662
({struct Cyc_Absyn_Stmt*_tmpA13=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp34B,_tmp34A,_tmp349,_tmp348,_tmp342,0);_tmp342=_tmpA13;});
# 1664
goto _LL0;case 28U: _LL5: _LL6:
# 1666
 goto _LL0;default: _LL7: _LL8:
# 1669
 Cyc_Toc_exp_to_c(nv,e);
({struct Cyc_Absyn_Stmt*_tmpA18=({struct Cyc_Absyn_Stmt*_tmpA17=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA16=({struct Cyc_Absyn_Exp*(*_tmpA15)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=_tmp343;struct Cyc_Absyn_Exp*_tmpA14=_tmp341;_tmpA15(_tmpA14,Cyc_Absyn_fieldname(i),0U);});Cyc_Absyn_assign_exp(_tmpA16,e,0U);}),0U);Cyc_Absyn_seq_stmt(_tmpA17,_tmp342,0U);});_tmp342=_tmpA18;});
# 1672
goto _LL0;}_LL0:;};}}{
# 1675
struct Cyc_Absyn_Exp*_tmp34C=({struct Cyc_Toc_Env*_tmpA1E=nv;struct Cyc_Absyn_Exp*_tmpA1D=Cyc_Absyn_var_exp(_tmp340,0U);void*_tmpA1C=_tmp33F;struct Cyc_Absyn_Stmt*_tmpA1B=_tmp342;int _tmpA1A=pointer;struct Cyc_Absyn_Exp*_tmpA19=rgnopt;Cyc_Toc_make_struct(_tmpA1E,_tmpA1D,_tmpA1C,_tmpA1B,_tmpA1A,_tmpA19,is_atomic,1);});_npop_handler(0U);return _tmp34C;};}
# 1642
;_pop_region(r);}
# 1679
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp34D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp34D != 0;_tmp34D=_tmp34D->tl){
struct Cyc_Absyn_Aggrfield*_tmp34E=(struct Cyc_Absyn_Aggrfield*)_tmp34D->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp34E->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1686
({void*_tmp34F=0U;({struct _dyneither_ptr _tmpA20=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp352;_tmp352.tag=0U;_tmp352.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp350[1U]={& _tmp352};({struct _dyneither_ptr _tmpA1F=({const char*_tmp351="get_member_offset %s failed";_tag_dyneither(_tmp351,sizeof(char),28U);});Cyc_aprintf(_tmpA1F,_tag_dyneither(_tmp350,sizeof(void*),1U));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA20,_tag_dyneither(_tmp34F,sizeof(void*),0U));});});}
# 1689
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp353=Cyc_Tcutil_compress(*typ);void*_tmp354=_tmp353;void*_tmp35C;struct Cyc_Absyn_Tqual _tmp35B;struct Cyc_Absyn_Exp*_tmp35A;union Cyc_Absyn_Constraint*_tmp359;unsigned int _tmp358;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->tag == 8U){_LL1: _tmp35C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).elt_type;_tmp35B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).tq;_tmp35A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).num_elts;_tmp359=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).zero_term;_tmp358=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).zt_loc;_LL2:
# 1694
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp35A))){
({void*_tmpA24=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp355=_cycalloc(sizeof(*_tmp355));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpA23=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp356;_tmp356.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmpA22=({struct Cyc_Absyn_ArrayInfo _tmp357;_tmp357.elt_type=_tmp35C;_tmp357.tq=_tmp35B;({struct Cyc_Absyn_Exp*_tmpA21=Cyc_Absyn_uint_exp(0U,0U);_tmp357.num_elts=_tmpA21;});_tmp357.zero_term=_tmp359;_tmp357.zt_loc=_tmp358;_tmp357;});_tmp356.f1=_tmpA22;});_tmp356;});_tmp355[0]=_tmpA23;});_tmp355;});*typ=_tmpA24;});
return 1;}
# 1698
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1701
return 0;}
# 1704
static int Cyc_Toc_is_array_type(void*t){
void*_tmp35D=Cyc_Tcutil_compress(t);void*_tmp35E=_tmp35D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35E)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1711
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1713
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1717
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp35F=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp360=_tmp35F->attributes;
void*_tmp361=_tmp35F->type;
if((!Cyc_Toc_is_array_type(_tmp361),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp361))))
({struct Cyc_List_List*_tmpA27=({struct Cyc_List_List*_tmp362=_cycalloc(sizeof(*_tmp362));({void*_tmpA26=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp363=_cycalloc(sizeof(*_tmp363));({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA25=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp364;_tmp364.tag=6U;_tmp364.f1=0;_tmp364;});_tmp363[0]=_tmpA25;});_tmp363;});_tmp362->hd=_tmpA26;});_tmp362->tl=_tmp360;_tmp362;});_tmp360=_tmpA27;});{
struct Cyc_Absyn_Aggrfield*_tmp365=({struct Cyc_Absyn_Aggrfield*_tmp367=_cycalloc(sizeof(*_tmp367));_tmp367->name=_tmp35F->name;_tmp367->tq=Cyc_Toc_mt_tq;_tmp367->type=new_field_type;_tmp367->width=_tmp35F->width;_tmp367->attributes=_tmp360;_tmp367->requires_clause=0;_tmp367;});
# 1729
({struct Cyc_List_List*_tmpA28=({struct Cyc_List_List*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->hd=_tmp365;_tmp366->tl=new_fields;_tmp366;});new_fields=_tmpA28;});};}else{
# 1731
({struct Cyc_List_List*_tmpA29=({struct Cyc_List_List*_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp368->tl=new_fields;_tmp368;});new_fields=_tmpA29;});}}{
# 1733
struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp36E=_cycalloc(sizeof(*_tmp36E));({struct _dyneither_ptr _tmpA2B=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp371;_tmp371.tag=1U;_tmp371.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp36F[1U]={& _tmp371};({struct _dyneither_ptr _tmpA2A=({const char*_tmp370="_genStruct%d";_tag_dyneither(_tmp370,sizeof(char),13U);});Cyc_aprintf(_tmpA2A,_tag_dyneither(_tmp36F,sizeof(void*),1U));});});});_tmp36E[0]=_tmpA2B;});_tmp36E;});
# 1736
struct _tuple1*qv=({struct _tuple1*_tmp36D=_cycalloc(sizeof(*_tmp36D));({union Cyc_Absyn_Nmspace _tmpA2C=Cyc_Absyn_Abs_n(0,1);_tmp36D->f1=_tmpA2C;});_tmp36D->f2=name;_tmp36D;});
struct Cyc_Absyn_AggrdeclImpl*_tmp36A=({struct Cyc_Absyn_AggrdeclImpl*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;_tmp36C->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;_tmp36C->fields=_tmp369;_tmp36C->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;_tmp36C;});
# 1741
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->kind=ad->kind;_tmp36B->sc=ad->sc;_tmp36B->name=qv;_tmp36B->tvs=ad->tvs;_tmp36B->impl=_tmp36A;_tmp36B->expected_mem_kind=0;_tmp36B->attributes=ad->attributes;_tmp36B;});
# 1748
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1752
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1758
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp372=Cyc_Toc_temp_var();
({struct Cyc_Absyn_Exp*_tmpA2D=Cyc_Absyn_var_exp(_tmp372,0U);xexp=_tmpA2D;});
do_declare=1;}else{
# 1766
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
({struct Cyc_Toc_Env*_tmpA2E=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);nv=_tmpA2E;});}{
# 1769
struct Cyc_Absyn_Stmt*_tmp373=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0U);
# 1773
struct Cyc_Absyn_Exp**varexp;
{void*_tmp374=_tmp373->r;void*_tmp375=_tmp374;struct Cyc_Absyn_Exp**_tmp378;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp375)->tag == 1U){_LL1: _tmp378=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp375)->f1;_LL2:
 varexp=_tmp378;goto _LL0;}else{_LL3: _LL4:
({void*_tmp376=0U;({struct _dyneither_ptr _tmpA2F=({const char*_tmp377="stmt not an expression!";_tag_dyneither(_tmp377,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA2F,_tag_dyneither(_tmp376,sizeof(void*),0U));});});}_LL0:;}{
# 1779
struct Cyc_Absyn_Exp*(*_tmp379)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1783
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp37A=Cyc_Tcutil_compress(struct_type);void*_tmp37B=_tmp37A;union Cyc_Absyn_AggrInfoU _tmp37F;struct Cyc_List_List*_tmp37E;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37B)->tag == 11U){_LL6: _tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37B)->f1).aggr_info;_tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37B)->f1).targs;_LL7:
# 1790
({struct Cyc_Absyn_Aggrdecl*_tmpA30=Cyc_Absyn_get_known_aggrdecl(_tmp37F);ad=_tmpA30;});
({struct Cyc_List_List*_tmpA31=aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;orig_aggrfields=_tmpA31;});
forall_types=_tmp37E;
goto _LL5;}else{_LL8: _LL9:
({void*_tmp37C=0U;({struct _dyneither_ptr _tmpA32=({const char*_tmp37D="init_struct: bad struct type";_tag_dyneither(_tmp37D,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA32,_tag_dyneither(_tmp37C,sizeof(void*),0U));});});}_LL5:;}
# 1797
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp380=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp380->tl != 0;_tmp380=_tmp380->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp381=(struct Cyc_Absyn_Aggrfield*)_tmp380->hd;
struct _RegionHandle _tmp382=_new_region("temp");struct _RegionHandle*temp=& _tmp382;_push_region(temp);
{struct Cyc_List_List*_tmp383=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1806
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp383,_tmp381->type))){
struct Cyc_List_List*_tmp384=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp385=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp383,_tmp384);
# 1810
struct Cyc_List_List*new_fields=0;
for(_tmp380=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp380 != 0;_tmp380=_tmp380->tl){
struct Cyc_Absyn_Aggrfield*_tmp386=(struct Cyc_Absyn_Aggrfield*)_tmp380->hd;
struct Cyc_Absyn_Aggrfield*_tmp387=({struct Cyc_Absyn_Aggrfield*_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A->name=_tmp386->name;_tmp38A->tq=Cyc_Toc_mt_tq;({void*_tmpA33=
# 1815
Cyc_Tcutil_rsubstitute(temp,_tmp385,_tmp386->type);_tmp38A->type=_tmpA33;});_tmp38A->width=_tmp386->width;_tmp38A->attributes=_tmp386->attributes;_tmp38A->requires_clause=0;_tmp38A;});
# 1823
if(_tmp380->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp387->type))
({struct _dyneither_ptr**_tmpA34=({struct _dyneither_ptr**_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388[0]=_tmp387->name;_tmp388;});index=_tmpA34;});}
# 1830
({struct Cyc_List_List*_tmpA35=({struct Cyc_List_List*_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389->hd=_tmp387;_tmp389->tl=new_fields;_tmp389;});new_fields=_tmpA35;});}
# 1832
exist_types=0;
({struct Cyc_List_List*_tmpA36=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);aggrfields=_tmpA36;});{
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp393=_cycalloc(sizeof(*_tmp393));({struct _dyneither_ptr _tmpA38=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp396;_tmp396.tag=1U;_tmp396.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp394[1U]={& _tmp396};({struct _dyneither_ptr _tmpA37=({const char*_tmp395="_genStruct%d";_tag_dyneither(_tmp395,sizeof(char),13U);});Cyc_aprintf(_tmpA37,_tag_dyneither(_tmp394,sizeof(void*),1U));});});});_tmp393[0]=_tmpA38;});_tmp393;});
# 1836
struct _tuple1*qv=({struct _tuple1*_tmp392=_cycalloc(sizeof(*_tmp392));({union Cyc_Absyn_Nmspace _tmpA39=Cyc_Absyn_Abs_n(0,1);_tmp392->f1=_tmpA39;});_tmp392->f2=name;_tmp392;});
struct Cyc_Absyn_AggrdeclImpl*_tmp38B=({struct Cyc_Absyn_AggrdeclImpl*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->exist_vars=0;_tmp391->rgn_po=0;_tmp391->fields=aggrfields;_tmp391->tagged=0;_tmp391;});
# 1841
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390->kind=Cyc_Absyn_StructA;_tmp390->sc=Cyc_Absyn_Public;_tmp390->name=qv;_tmp390->tvs=0;_tmp390->expected_mem_kind=0;_tmp390->impl=_tmp38B;_tmp390->attributes=0;_tmp390;});
# 1848
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
({void*_tmpA3D=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp38C=_cycalloc(sizeof(*_tmp38C));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpA3C=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp38D;_tmp38D.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpA3B=({struct Cyc_Absyn_AggrInfo _tmp38E;({union Cyc_Absyn_AggrInfoU _tmpA3A=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=new_ad;_tmp38F;}));_tmp38E.aggr_info=_tmpA3A;});_tmp38E.targs=0;_tmp38E;});_tmp38D.f1=_tmpA3B;});_tmp38D;});_tmp38C[0]=_tmpA3C;});_tmp38C;});struct_type=_tmpA3D;});
# 1852
({void*_tmpA3E=Cyc_Toc_typ_to_c(struct_type);aggr_typ=_tmpA3E;});};}}
# 1802
;_pop_region(temp);};}{
# 1856
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp397=_new_region("r");struct _RegionHandle*r=& _tmp397;_push_region(r);
{struct Cyc_List_List*_tmp398=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp398 != 0;_tmp398=_tmp398->tl){
struct _tuple19*_tmp399=(struct _tuple19*)_tmp398->hd;struct _tuple19*_tmp39A=_tmp399;struct Cyc_List_List*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3BE;_LLB: _tmp3BF=_tmp39A->f1;_tmp3BE=_tmp39A->f2;_LLC:;
({int _tmpA3F=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3BE->topt));is_atomic=_tmpA3F;});
if(_tmp3BF == 0)
({void*_tmp39B=0U;({struct _dyneither_ptr _tmpA40=({const char*_tmp39C="empty designator list";_tag_dyneither(_tmp39C,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA40,_tag_dyneither(_tmp39B,sizeof(void*),0U));});});
if(_tmp3BF->tl != 0){
# 1866
struct _tuple1*_tmp39D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39E=Cyc_Absyn_var_exp(_tmp39D,0U);
for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
void*_tmp39F=(void*)_tmp3BF->hd;void*_tmp3A0=_tmp39F;struct _dyneither_ptr*_tmp3A3;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A0)->tag == 1U){_LLE: _tmp3A3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A0)->f1;_LLF:
# 1872
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3A3))
({struct Cyc_Absyn_Exp*_tmpA42=({void*_tmpA41=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmpA41,_tmp39E);});_tmp39E=_tmpA42;});
({struct Cyc_Absyn_Stmt*_tmpA45=({struct Cyc_Absyn_Stmt*_tmpA44=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA43=_tmp379(xexp,_tmp3A3,0U);Cyc_Absyn_assign_exp(_tmpA43,_tmp39E,0U);}),0U);Cyc_Absyn_seq_stmt(_tmpA44,_tmp373,0U);});_tmp373=_tmpA45;});
# 1876
goto _LLD;}else{_LL10: _LL11:
({void*_tmp3A1=0U;({struct _dyneither_ptr _tmpA46=({const char*_tmp3A2="array designator in struct";_tag_dyneither(_tmp3A2,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA46,_tag_dyneither(_tmp3A1,sizeof(void*),0U));});});}_LLD:;}
# 1880
Cyc_Toc_exp_to_c(nv,_tmp3BE);
({struct Cyc_Absyn_Stmt*_tmpA48=({struct Cyc_Absyn_Stmt*_tmpA47=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp39E,_tmp3BE,0U),0U);Cyc_Absyn_seq_stmt(_tmpA47,_tmp373,0U);});_tmp373=_tmpA48;});}else{
# 1883
void*_tmp3A4=(void*)_tmp3BF->hd;void*_tmp3A5=_tmp3A4;struct _dyneither_ptr*_tmp3BD;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A5)->tag == 1U){_LL13: _tmp3BD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A5)->f1;_LL14: {
# 1885
struct Cyc_Absyn_Exp*lval=_tmp379(xexp,_tmp3BD,0U);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3BD);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0U);
({struct Cyc_Absyn_Stmt*_tmpA4A=({struct Cyc_Absyn_Stmt*_tmpA49=Cyc_Absyn_exp_stmt(assn_exp,0U);Cyc_Absyn_seq_stmt(_tmpA49,_tmp373,0U);});_tmp373=_tmpA4A;});
({struct Cyc_Absyn_Exp*_tmpA4B=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0U);lval=_tmpA4B;});}{
# 1894
int e1_translated=0;
{void*_tmp3A6=_tmp3BE->r;void*_tmp3A7=_tmp3A6;void*_tmp3BA;struct Cyc_List_List*_tmp3B9;struct Cyc_Absyn_Exp*_tmp3B8;void*_tmp3B7;struct Cyc_Absyn_Vardecl*_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B4;int _tmp3B3;struct Cyc_List_List*_tmp3B2;switch(*((int*)_tmp3A7)){case 26U: _LL18: _tmp3B2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_LL19:
# 1897
({struct Cyc_Absyn_Stmt*_tmpA4C=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3BD)))->type,lval,_tmp3B2,_tmp373);_tmp373=_tmpA4C;});
# 1899
goto _LL17;case 27U: _LL1A: _tmp3B6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_tmp3B5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f2;_tmp3B4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f3;_tmp3B3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f4;_LL1B:
# 1901
({struct Cyc_Absyn_Stmt*_tmpA4D=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3BD)))->type,lval,_tmp3B6,_tmp3B5,_tmp3B4,_tmp3B3,_tmp373,0);_tmp373=_tmpA4D;});
# 1905
e1_translated=1;
_tmp3B8=_tmp3B5;_tmp3B7=(void*)_check_null(_tmp3B4->topt);goto _LL1D;case 28U: _LL1C: _tmp3B8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_tmp3B7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3A7)->f2;_LL1D:
# 1910
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3BD)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3B8);{
void*_tmp3A8=Cyc_Toc_typ_to_c(_tmp3B7);
({struct Cyc_Absyn_Exp*_tmpA52=({
struct Cyc_Absyn_Exp*_tmpA51=({struct Cyc_Absyn_Exp*_tmpA4F=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmpA4F,({struct Cyc_Absyn_Exp*_tmp3A9[2U];({struct Cyc_Absyn_Exp*_tmpA4E=
Cyc_Absyn_sizeoftyp_exp(_tmp3A8,0U);_tmp3A9[1U]=_tmpA4E;});_tmp3A9[0U]=_tmp3B8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A9,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1914
Cyc_Absyn_add_exp(_tmpA51,
# 1916
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3AA;({struct _tuple6 _tmpA50=({struct _tuple6 _tmp3AB;_tmp3AB.f1=Cyc_Absyn_Unsigned;_tmp3AB.f2=(int)sizeof(double);_tmp3AB;});(_tmp3AA.Int_c).val=_tmpA50;});(_tmp3AA.Int_c).tag=5;_tmp3AA;}),0U),0U);});
# 1913
*(nv->struct_info).varsizeexp=_tmpA52;});};}
# 1918
goto _LL17;case 30U: _LL1E: _tmp3BA=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_tmp3B9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3A7)->f2;_LL1F:
# 1920
({struct Cyc_Absyn_Stmt*_tmpA53=Cyc_Toc_init_anon_struct(nv,lval,_tmp3BA,_tmp3B9,_tmp373);_tmp373=_tmpA53;});goto _LL17;default: _LL20: _LL21: {
# 1922
void*old_e_typ=(void*)_check_null(_tmp3BE->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
({struct Cyc_Toc_Env*_tmpA54=Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv);Cyc_Toc_exp_to_c(_tmpA54,_tmp3BE);});
{void*_tmp3AC=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3AC)->tag == 11U){_LL23: _LL24:
# 1929
 if(old_e_typ != _tmp3BE->topt  && !({void*_tmpA55=Cyc_Toc_typ_to_c(old_e_typ);Cyc_Tcutil_unify(_tmpA55,(void*)_check_null(_tmp3BE->topt));})){
# 1933
({struct Cyc_Absyn_Aggrdecl*_tmpA56=Cyc_Toc_update_aggr_type(ad,_tmp3BD,(void*)_check_null(_tmp3BE->topt));ad=_tmpA56;});
({void*_tmpA5A=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpA59=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3AE;_tmp3AE.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpA58=({struct Cyc_Absyn_AggrInfo _tmp3AF;({union Cyc_Absyn_AggrInfoU _tmpA57=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0[0]=ad;_tmp3B0;}));_tmp3AF.aggr_info=_tmpA57;});_tmp3AF.targs=0;_tmp3AF;});_tmp3AE.f1=_tmpA58;});_tmp3AE;});_tmp3AD[0]=_tmpA59;});_tmp3AD;});
# 1934
struct_type=_tmpA5A;});
# 1937
({void*_tmpA5B=Cyc_Toc_typ_to_c(struct_type);aggr_typ=_tmpA5B;});}
# 1939
goto _LL22;}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 1942
struct Cyc_Absyn_Aggrfield*_tmp3B1=Cyc_Absyn_lookup_field(aggrfields,_tmp3BD);
# 1944
if(Cyc_Toc_is_poly_field(struct_type,_tmp3BD) && !was_ptr_type)
({struct Cyc_Absyn_Exp*_tmpA5D=({void*_tmpA5C=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmpA5C,_tmp3BE);});_tmp3BE=_tmpA5D;});
# 1947
if(exist_types != 0)
({struct Cyc_Absyn_Exp*_tmpA5F=({void*_tmpA5E=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3B1))->type);Cyc_Toc_cast_it(_tmpA5E,_tmp3BE);});_tmp3BE=_tmpA5F;});
# 1950
({struct Cyc_Absyn_Stmt*_tmpA61=({struct Cyc_Absyn_Stmt*_tmpA60=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3BE,0U),0U);Cyc_Absyn_seq_stmt(_tmpA60,_tmp373,0U);});_tmp373=_tmpA61;});
goto _LL17;};}}_LL17:;}
# 1953
goto _LL12;};}}else{_LL15: _LL16:
({void*_tmp3BB=0U;({struct _dyneither_ptr _tmpA62=({const char*_tmp3BC="array designator in struct";_tag_dyneither(_tmp3BC,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA62,_tag_dyneither(_tmp3BB,sizeof(void*),0U));});});}_LL12:;}}}
# 1960
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
({struct Cyc_Absyn_Exp*_tmpA63=Cyc_Toc_cast_it(cast_type,*varexp);*varexp=_tmpA63;});}{
# 1965
struct Cyc_Absyn_Exp*_tmp3C0=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp373,pointer,rgnopt,is_atomic,do_declare);
# 1967
({void*_tmpA64=pointer?Cyc_Absyn_cstar_typ(struct_type,Cyc_Toc_mt_tq): struct_type;_tmp3C0->topt=_tmpA64;});{
struct Cyc_Absyn_Exp*_tmp3C1=_tmp3C0;_npop_handler(0U);return _tmp3C1;};};
# 1858
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1973
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1976
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1979
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpA65=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3C3;_tmp3C3.tag=5U;_tmp3C3.f1=e1;_tmp3C3.f2=incr;_tmp3C3;});_tmp3C2[0]=_tmpA65;});_tmp3C2;}),0U);}
# 1983
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1992
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1994
void*_tmp3C4=e1->r;void*_tmp3C5=_tmp3C4;struct Cyc_Absyn_Exp*_tmp3CD;struct _dyneither_ptr*_tmp3CC;int _tmp3CB;int _tmp3CA;void*_tmp3C9;struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C7;switch(*((int*)_tmp3C5)){case 37U: _LL1: _tmp3C7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3C5)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp3C7,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp3C9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C5)->f1;_tmp3C8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C5)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp3C8,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp3CD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f1;_tmp3CC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f2;_tmp3CB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f3;_tmp3CA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f4;_LL6:
# 1999
 e1->r=_tmp3CD->r;
({struct Cyc_Absyn_Exp*_tmpA68=e1;struct Cyc_List_List*_tmpA67=({struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->hd=_tmp3CC;_tmp3C6->tl=fs;_tmp3C6;});struct Cyc_Absyn_Exp*(*_tmpA66)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmpA68,_tmpA67,_tmpA66,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 2007
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2009
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmpA69=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmpA69;});}
({void*_tmpA6A=(f(e1_copy,f_env))->r;e1->r=_tmpA6A;});
goto _LL0;}}_LL0:;}
# 2015
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2017
void*_tmp3CE=s->r;void*_tmp3CF=_tmp3CE;struct Cyc_Absyn_Stmt*_tmp3D6;struct Cyc_Absyn_Decl*_tmp3D5;struct Cyc_Absyn_Stmt*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D3;switch(*((int*)_tmp3CF)){case 1U: _LL1: _tmp3D3=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp3D3,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp3D5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f1;_tmp3D4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f2;_LL4:
# 2020
 Cyc_Toc_lvalue_assign_stmt(_tmp3D4,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp3D6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp3D6,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0U;({struct _dyneither_ptr _tmpA6B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp3D2.f1=_tmpA6B;});({void*_tmp3D0[1U]={& _tmp3D2};({struct _dyneither_ptr _tmpA6C=({const char*_tmp3D1="lvalue_assign_stmt: %s";_tag_dyneither(_tmp3D1,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmpA6C,_tag_dyneither(_tmp3D0,sizeof(void*),1U));});});});}_LL0:;}
# 2026
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2030
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3D7=e->r;void*_tmp3D8=_tmp3D7;struct Cyc_Absyn_Stmt*_tmp3DF;struct Cyc_Absyn_Exp*_tmp3DE;void**_tmp3DD;struct Cyc_Absyn_Exp**_tmp3DC;switch(*((int*)_tmp3D8)){case 14U: _LL1: _tmp3DD=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D8)->f1;_tmp3DC=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D8)->f2;_LL2:
# 2033
({struct Cyc_Absyn_Exp*_tmpA6D=Cyc_Toc_push_address_exp(*_tmp3DC);*_tmp3DC=_tmpA6D;});
({void*_tmpA6E=Cyc_Absyn_cstar_typ(*_tmp3DD,Cyc_Toc_mt_tq);*_tmp3DD=_tmpA6E;});
return e;case 20U: _LL3: _tmp3DE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3D8)->f1;_LL4:
# 2037
 return _tmp3DE;case 37U: _LL5: _tmp3DF=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3D8)->f1;_LL6:
# 2041
 Cyc_Toc_push_address_stmt(_tmp3DF);
return e;default: _LL7: _LL8:
# 2044
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0U;({struct _dyneither_ptr _tmpA6F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp3DB.f1=_tmpA6F;});({void*_tmp3D9[1U]={& _tmp3DB};({struct _dyneither_ptr _tmpA70=({const char*_tmp3DA="can't take & of exp %s";_tag_dyneither(_tmp3DA,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA70,_tag_dyneither(_tmp3D9,sizeof(void*),1U));});});});}_LL0:;}
# 2049
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3E0=s->r;void*_tmp3E1=_tmp3E0;struct Cyc_Absyn_Exp**_tmp3E7;struct Cyc_Absyn_Stmt*_tmp3E6;struct Cyc_Absyn_Stmt*_tmp3E5;switch(*((int*)_tmp3E1)){case 2U: _LL1: _tmp3E5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3E1)->f2;_LL2:
 _tmp3E6=_tmp3E5;goto _LL4;case 12U: _LL3: _tmp3E6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3E1)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp3E6);goto _LL0;case 1U: _LL5: _tmp3E7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3E1)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpA71=Cyc_Toc_push_address_exp(*_tmp3E7);*_tmp3E7=_tmpA71;});goto _LL0;default: _LL7: _LL8:
# 2055
({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0U;({struct _dyneither_ptr _tmpA72=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp3E4.f1=_tmpA72;});({void*_tmp3E2[1U]={& _tmp3E4};({struct _dyneither_ptr _tmpA73=({const char*_tmp3E3="can't take & of stmt %s";_tag_dyneither(_tmp3E3,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA73,_tag_dyneither(_tmp3E2,sizeof(void*),1U));});});});}_LL0:;}
# 2060
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2062
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2064
if(x == 0)return 0;
({struct Cyc_List_List*_tmpA75=({struct Cyc_List_List*_tmp3E8=_region_malloc(r2,sizeof(*_tmp3E8));({void*_tmpA74=f(x->hd,env);_tmp3E8->hd=_tmpA74;});_tmp3E8->tl=0;_tmp3E8;});result=_tmpA75;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
({struct Cyc_List_List*_tmpA77=({struct Cyc_List_List*_tmp3E9=_region_malloc(r2,sizeof(*_tmp3E9));({void*_tmpA76=f(x->hd,env);_tmp3E9->hd=_tmpA76;});_tmp3E9->tl=0;_tmp3E9;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmpA77;});
prev=prev->tl;}
# 2071
return result;}
# 2073
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2077
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->f1=0;_tmp3EA->f2=e;_tmp3EA;});}
# 2081
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3EB=Cyc_Tcutil_compress(t);void*_tmp3EC=_tmp3EB;struct Cyc_Absyn_PtrInfo _tmp3EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC)->tag == 5U){_LL1: _tmp3EF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC)->f1;_LL2:
 return _tmp3EF;}else{_LL3: _LL4:
({void*_tmp3ED=0U;({struct _dyneither_ptr _tmpA78=({const char*_tmp3EE="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3EE,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA78,_tag_dyneither(_tmp3ED,sizeof(void*),0U));});});}_LL0:;}
# 2091
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3F0=Cyc_Tcutil_compress(t);void*_tmp3F1=_tmp3F0;int _tmp3FD;enum Cyc_Absyn_Sign _tmp3FC;enum Cyc_Absyn_Sign _tmp3FB;enum Cyc_Absyn_Sign _tmp3FA;enum Cyc_Absyn_Sign _tmp3F9;enum Cyc_Absyn_Sign _tmp3F8;switch(*((int*)_tmp3F1)){case 5U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmpA79=Cyc_Absyn_null_exp(0U);res=_tmpA79;});goto _LL0;case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f2){case Cyc_Absyn_Char_sz: _LL3: _tmp3F8=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmpA7A=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3F8,'\000'),0U);res=_tmpA7A;});goto _LL0;case Cyc_Absyn_Short_sz: _LL5: _tmp3F9=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpA7B=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3F9,0),0U);res=_tmpA7B;});goto _LL0;case Cyc_Absyn_Int_sz: _LLB: _tmp3FA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LLC:
# 2099
 _tmp3FB=_tmp3FA;goto _LLE;case Cyc_Absyn_Long_sz: _LLD: _tmp3FB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LLE:
# 2101
({struct Cyc_Absyn_Exp*_tmpA7C=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3FB,0),0U);res=_tmpA7C;});goto _LL0;default: _LLF: _tmp3FC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL10:
# 2103
({struct Cyc_Absyn_Exp*_tmpA7D=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3FC,(long long)0),0U);res=_tmpA7D;});goto _LL0;}case 13U: _LL7: _LL8:
# 2097
 goto _LLA;case 14U: _LL9: _LLA:
 _tmp3FA=Cyc_Absyn_Unsigned;goto _LLC;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1)->f1){case 0U: _LL11: _LL12:
# 2104
({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F2="0.0F";_tag_dyneither(_tmp3F2,sizeof(char),5U);}),0),0U);res=_tmpA7E;});goto _LL0;case 1U: _LL13: _LL14:
({struct Cyc_Absyn_Exp*_tmpA7F=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F3="0.0";_tag_dyneither(_tmp3F3,sizeof(char),4U);}),1),0U);res=_tmpA7F;});goto _LL0;default: _LL15: _tmp3FD=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1)->f1;_LL16:
({struct Cyc_Absyn_Exp*_tmpA81=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmpA80=({const char*_tmp3F4="0.0L";_tag_dyneither(_tmp3F4,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmpA80,_tmp3FD);}),0U);res=_tmpA81;});goto _LL0;}default: _LL17: _LL18:
# 2108
({struct Cyc_String_pa_PrintArg_struct _tmp3F7;_tmp3F7.tag=0U;({struct _dyneither_ptr _tmpA82=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp3F7.f1=_tmpA82;});({void*_tmp3F5[1U]={& _tmp3F7};({struct _dyneither_ptr _tmpA83=({const char*_tmp3F6="found non-zero type %s in generate_zero";_tag_dyneither(_tmp3F6,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA83,_tag_dyneither(_tmp3F5,sizeof(void*),1U));});});});}_LL0:;}
# 2110
res->topt=t;
return res;}
# 2117
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2130 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp3FE=Cyc_Toc_typ_to_c(elt_type);
void*_tmp3FF=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp400=Cyc_Absyn_cstar_typ(_tmp3FE,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp401=({struct Cyc_Core_Opt*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->v=_tmp400;_tmp438;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp402=e1->r;void*_tmp403=_tmp402;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp407;switch(*((int*)_tmp403)){case 20U: _LL1: _tmp407=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_LL2:
# 2138
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmpA84=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp407,0,Cyc_Absyn_Other_coercion,0U);_tmp407=_tmpA84;});
_tmp407->topt=fat_ptr_type;}
# 2142
Cyc_Toc_exp_to_c(nv,_tmp407);xinit=_tmp407;goto _LL0;case 23U: _LL3: _tmp409=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_tmp408=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp403)->f2;_LL4:
# 2144
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmpA85=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp409,0,Cyc_Absyn_Other_coercion,0U);_tmp409=_tmpA85;});
_tmp409->topt=fat_ptr_type;}
# 2148
Cyc_Toc_exp_to_c(nv,_tmp409);Cyc_Toc_exp_to_c(nv,_tmp408);
({struct Cyc_Absyn_Exp*_tmpA88=({struct Cyc_Absyn_Exp*_tmpA87=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmpA87,({struct Cyc_Absyn_Exp*_tmp404[3U];_tmp404[2U]=_tmp408;({struct Cyc_Absyn_Exp*_tmpA86=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp404[1U]=_tmpA86;});_tmp404[0U]=_tmp409;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp404,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2149
xinit=_tmpA88;});
# 2151
goto _LL0;default: _LL5: _LL6:
({void*_tmp405=0U;({struct _dyneither_ptr _tmpA89=({const char*_tmp406="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp406,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA89,_tag_dyneither(_tmp405,sizeof(void*),0U));});});}_LL0:;}{
# 2154
struct _tuple1*_tmp40A=Cyc_Toc_temp_var();
struct _RegionHandle _tmp40B=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp40B;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp40C=({struct Cyc_Absyn_Vardecl*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->sc=Cyc_Absyn_Public;_tmp437->name=_tmp40A;_tmp437->varloc=0U;_tmp437->tq=Cyc_Toc_mt_tq;_tmp437->type=_tmp3FF;_tmp437->initializer=xinit;_tmp437->rgn=0;_tmp437->attributes=0;_tmp437->escapes=0;_tmp437;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp40D=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp435=_cycalloc(sizeof(*_tmp435));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpA8A=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp436;_tmp436.tag=4U;_tmp436.f1=_tmp40C;_tmp436;});_tmp435[0]=_tmpA8A;});_tmp435;});
struct Cyc_Absyn_Exp*_tmp40E=Cyc_Absyn_varb_exp((void*)_tmp40D,0U);
_tmp40E->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp40F=Cyc_Absyn_deref_exp(_tmp40E,0U);
_tmp40F->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp40F);{
struct _tuple1*_tmp410=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp411=({struct Cyc_Absyn_Vardecl*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->sc=Cyc_Absyn_Public;_tmp434->name=_tmp410;_tmp434->varloc=0U;_tmp434->tq=Cyc_Toc_mt_tq;_tmp434->type=_tmp3FE;_tmp434->initializer=_tmp40F;_tmp434->rgn=0;_tmp434->attributes=0;_tmp434->escapes=0;_tmp434;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp412=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp432=_cycalloc(sizeof(*_tmp432));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpA8B=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp433;_tmp433.tag=4U;_tmp433.f1=_tmp411;_tmp433;});_tmp432[0]=_tmpA8B;});_tmp432;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp413=Cyc_Absyn_varb_exp((void*)_tmp412,0U);
_tmp413->topt=_tmp40F->topt;
({struct Cyc_Absyn_Exp*_tmpA8C=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp413,e2,0U);z_init=_tmpA8C;});
z_init->topt=_tmp413->topt;}
# 2173
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp414=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp415=({struct Cyc_Absyn_Vardecl*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->sc=Cyc_Absyn_Public;_tmp431->name=_tmp414;_tmp431->varloc=0U;_tmp431->tq=Cyc_Toc_mt_tq;_tmp431->type=_tmp3FE;_tmp431->initializer=z_init;_tmp431->rgn=0;_tmp431->attributes=0;_tmp431->escapes=0;_tmp431;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp416=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42F=_cycalloc(sizeof(*_tmp42F));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpA8D=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp430;_tmp430.tag=4U;_tmp430.f1=_tmp415;_tmp430;});_tmp42F[0]=_tmpA8D;});_tmp42F;});
# 2179
struct Cyc_Absyn_Exp*_tmp417=Cyc_Absyn_varb_exp((void*)_tmp412,0U);_tmp417->topt=_tmp40F->topt;{
struct Cyc_Absyn_Exp*_tmp418=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp419=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp417,_tmp418,0U);
_tmp419->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp419);{
# 2185
struct Cyc_Absyn_Exp*_tmp41A=Cyc_Absyn_varb_exp((void*)_tmp416,0U);_tmp41A->topt=_tmp40F->topt;{
struct Cyc_Absyn_Exp*_tmp41B=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp41A,_tmp41B,0U);
_tmp41C->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp41C);{
# 2191
struct Cyc_List_List*_tmp41D=({struct Cyc_Absyn_Exp*_tmp42E[2U];({struct Cyc_Absyn_Exp*_tmpA8E=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp42E[1U]=_tmpA8E;});({struct Cyc_Absyn_Exp*_tmpA8F=
# 2191
Cyc_Absyn_varb_exp((void*)_tmp40D,0U);_tmp42E[0U]=_tmpA8F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42E,sizeof(struct Cyc_Absyn_Exp*),2U));});
# 2193
struct Cyc_Absyn_Exp*_tmp41E=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmpA91=({struct Cyc_Absyn_Exp*_tmpA90=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp41D,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmpA90,_tmp41E,0U);});xsize=_tmpA91;});{
# 2198
struct Cyc_Absyn_Exp*_tmp41F=({struct Cyc_Absyn_Exp*_tmpA92=xsize;Cyc_Absyn_and_exp(_tmpA92,Cyc_Absyn_and_exp(_tmp419,_tmp41C,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp420=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp421=({struct Cyc_Absyn_Exp*_tmpA93=Cyc_Absyn_varb_exp((void*)_tmp40D,0U);Cyc_Toc_member_exp(_tmpA93,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmpA94=Cyc_Toc_cast_it(_tmp400,_tmp421);_tmp421=_tmpA94;});{
struct Cyc_Absyn_Exp*_tmp422=Cyc_Absyn_deref_exp(_tmp421,0U);
struct Cyc_Absyn_Exp*_tmp423=({struct Cyc_Absyn_Exp*_tmpA95=_tmp422;Cyc_Absyn_assign_exp(_tmpA95,Cyc_Absyn_var_exp(_tmp414,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp424=Cyc_Absyn_exp_stmt(_tmp423,0U);
({struct Cyc_Absyn_Stmt*_tmpA99=({struct Cyc_Absyn_Stmt*_tmpA98=({struct Cyc_Absyn_Exp*_tmpA97=_tmp41F;struct Cyc_Absyn_Stmt*_tmpA96=_tmp420;Cyc_Absyn_ifthenelse_stmt(_tmpA97,_tmpA96,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmpA98,_tmp424,0U);});_tmp424=_tmpA99;});
({struct Cyc_Absyn_Stmt*_tmpA9D=({struct Cyc_Absyn_Decl*_tmpA9C=({struct Cyc_Absyn_Decl*_tmp425=_cycalloc(sizeof(*_tmp425));({void*_tmpA9B=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp426=_cycalloc(sizeof(*_tmp426));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpA9A=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp427;_tmp427.tag=0U;_tmp427.f1=_tmp415;_tmp427;});_tmp426[0]=_tmpA9A;});_tmp426;});_tmp425->r=_tmpA9B;});_tmp425->loc=0U;_tmp425;});Cyc_Absyn_decl_stmt(_tmpA9C,_tmp424,0U);});_tmp424=_tmpA9D;});
({struct Cyc_Absyn_Stmt*_tmpAA1=({struct Cyc_Absyn_Decl*_tmpAA0=({struct Cyc_Absyn_Decl*_tmp428=_cycalloc(sizeof(*_tmp428));({void*_tmpA9F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp429=_cycalloc(sizeof(*_tmp429));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpA9E=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp42A;_tmp42A.tag=0U;_tmp42A.f1=_tmp411;_tmp42A;});_tmp429[0]=_tmpA9E;});_tmp429;});_tmp428->r=_tmpA9F;});_tmp428->loc=0U;_tmp428;});Cyc_Absyn_decl_stmt(_tmpAA0,_tmp424,0U);});_tmp424=_tmpAA1;});
({struct Cyc_Absyn_Stmt*_tmpAA5=({struct Cyc_Absyn_Decl*_tmpAA4=({struct Cyc_Absyn_Decl*_tmp42B=_cycalloc(sizeof(*_tmp42B));({void*_tmpAA3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAA2=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp42D;_tmp42D.tag=0U;_tmp42D.f1=_tmp40C;_tmp42D;});_tmp42C[0]=_tmpAA2;});_tmp42C;});_tmp42B->r=_tmpAA3;});_tmp42B->loc=0U;_tmp42B;});Cyc_Absyn_decl_stmt(_tmpAA4,_tmp424,0U);});_tmp424=_tmpAA5;});
({void*_tmpAA6=Cyc_Toc_stmt_exp_r(_tmp424);e->r=_tmpAA6;});};};};};};};};};};}
# 2156
;_pop_region(rgn2);};}
# 2224 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2228
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp439=Cyc_Tcutil_compress(aggrtype);void*_tmp43A=_tmp439;union Cyc_Absyn_AggrInfoU _tmp43F;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43A)->tag == 11U){_LL1: _tmp43F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43A)->f1).aggr_info;_LL2:
# 2231
({struct Cyc_Absyn_Aggrdecl*_tmpAA7=Cyc_Absyn_get_known_aggrdecl(_tmp43F);ad=_tmpAA7;});goto _LL0;}else{_LL3: _LL4:
({void*_tmp43B=0U;({struct _dyneither_ptr _tmpAAA=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp43E;_tmp43E.tag=0U;({struct _dyneither_ptr _tmpAA8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(aggrtype));_tmp43E.f1=_tmpAA8;});({void*_tmp43C[1U]={& _tmp43E};({struct _dyneither_ptr _tmpAA9=({const char*_tmp43D="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp43D,sizeof(char),51U);});Cyc_aprintf(_tmpAA9,_tag_dyneither(_tmp43C,sizeof(void*),1U));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAAA,_tag_dyneither(_tmp43B,sizeof(void*),0U));});});}_LL0:;}{
# 2235
struct _tuple1*_tmp440=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp441=Cyc_Absyn_var_exp(_tmp440,0U);
struct Cyc_Absyn_Exp*_tmp442=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp443=Cyc_Absyn_aggrarrow_exp(_tmp441,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp444=Cyc_Absyn_neq_exp(_tmp443,_tmp442,0U);
struct Cyc_Absyn_Exp*_tmp445=Cyc_Absyn_aggrarrow_exp(_tmp441,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp446=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp445,0U),0U);
struct Cyc_Absyn_Stmt*_tmp447=({struct Cyc_Absyn_Exp*_tmpAAC=_tmp444;struct Cyc_Absyn_Stmt*_tmpAAB=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmpAAC,_tmpAAB,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp448=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp449=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp44A=({struct _tuple1*_tmpAAF=_tmp440;void*_tmpAAE=_tmp448;struct Cyc_Absyn_Exp*_tmpAAD=_tmp449;Cyc_Absyn_declare_stmt(_tmpAAF,_tmpAAE,_tmpAAD,Cyc_Absyn_seq_stmt(_tmp447,_tmp446,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp44A);}else{
# 2249
struct Cyc_Absyn_Exp*_tmp44B=({struct Cyc_Absyn_Exp*_tmpAB0=aggrproj(_tmp441,f,0U);Cyc_Toc_member_exp(_tmpAB0,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_neq_exp(_tmp44B,_tmp442,0U);
struct Cyc_Absyn_Exp*_tmp44D=({struct Cyc_Absyn_Exp*_tmpAB1=aggrproj(_tmp441,f,0U);Cyc_Toc_member_exp(_tmpAB1,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp44E=Cyc_Absyn_exp_stmt(_tmp44D,0U);
struct Cyc_Absyn_Stmt*_tmp44F=({struct Cyc_Absyn_Exp*_tmpAB3=_tmp44C;struct Cyc_Absyn_Stmt*_tmpAB2=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmpAB3,_tmpAB2,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp450=({struct _tuple1*_tmpAB6=_tmp440;void*_tmpAB5=e1_c_type;struct Cyc_Absyn_Exp*_tmpAB4=e1;Cyc_Absyn_declare_stmt(_tmpAB6,_tmpAB5,_tmpAB4,Cyc_Absyn_seq_stmt(_tmp44F,_tmp44E,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp450);}};}
# 2259
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2262
void*_tmp451=Cyc_Tcutil_compress(t);void*_tmp452=_tmp451;union Cyc_Absyn_AggrInfoU _tmp459;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp452)->tag == 11U){_LL1: _tmp459=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp452)->f1).aggr_info;_LL2: {
# 2264
struct Cyc_Absyn_Aggrdecl*_tmp453=Cyc_Absyn_get_known_aggrdecl(_tmp459);
({int _tmpAB7=Cyc_Toc_get_member_offset(_tmp453,f);*f_tag=_tmpAB7;});{
# 2267
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp458;_tmp458.tag=0U;_tmp458.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp457;_tmp457.tag=0U;_tmp457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp453->name).f2);({void*_tmp455[2U]={& _tmp457,& _tmp458};({struct _dyneither_ptr _tmpAB8=({const char*_tmp456="_union_%s_%s";_tag_dyneither(_tmp456,sizeof(char),13U);});Cyc_aprintf(_tmpAB8,_tag_dyneither(_tmp455,sizeof(void*),2U));});});});});
({void*_tmpAB9=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454[0]=str;_tmp454;}));*tagged_member_type=_tmpAB9;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp453->impl))->tagged;};}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2278
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2281
void*_tmp45A=e->r;void*_tmp45B=_tmp45A;struct Cyc_Absyn_Exp*_tmp467;struct _dyneither_ptr*_tmp466;int*_tmp465;struct Cyc_Absyn_Exp*_tmp464;struct _dyneither_ptr*_tmp463;int*_tmp462;struct Cyc_Absyn_Exp*_tmp461;switch(*((int*)_tmp45B)){case 14U: _LL1: _tmp461=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_LL2:
({void*_tmp45C=0U;({struct _dyneither_ptr _tmpABA=({const char*_tmp45D="cast on lhs!";_tag_dyneither(_tmp45D,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpABA,_tag_dyneither(_tmp45C,sizeof(void*),0U));});});case 21U: _LL3: _tmp464=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp463=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_tmp462=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45B)->f4;_LL4:
# 2284
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp464->topt),_tmp463,f_tag,tagged_member_type,clear_read,_tmp462);case 22U: _LL5: _tmp467=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp466=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_tmp465=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45B)->f4;_LL6: {
# 2287
void*_tmp45E=Cyc_Tcutil_compress((void*)_check_null(_tmp467->topt));void*_tmp45F=_tmp45E;void*_tmp460;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->tag == 5U){_LLA: _tmp460=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->f1).elt_typ;_LLB:
# 2289
 return Cyc_Toc_is_tagged_union_project_impl(_tmp460,_tmp466,f_tag,tagged_member_type,clear_read,_tmp465);}else{_LLC: _LLD:
# 2291
 return 0;}_LL9:;}default: _LL7: _LL8:
# 2293
 return 0;}_LL0:;}
# 2306 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2310
struct _tuple1*_tmp468=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp468,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmpABB=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmpABB;});else{
# 2320
struct Cyc_Absyn_Exp*_tmp469=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmpABE=({struct Cyc_Absyn_Exp*_tmpABD=_tmp469;struct Cyc_Absyn_Stmt*_tmpABC=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmpABD,_tmpABC,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmpABE;});}{
# 2323
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmpAC1=_tmp468;void*_tmpAC0=Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpABF=
Cyc_Toc_push_address_exp(e1);
# 2323
Cyc_Absyn_declare_stmt(_tmpAC1,_tmpAC0,_tmpABF,
# 2325
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2329
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2344 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp46A=Cyc_Absyn_var_exp(max,0U);
# 2352
struct Cyc_Absyn_Stmt*s;
if(zero_term)
({struct Cyc_Absyn_Exp*_tmpAC3=({struct Cyc_Absyn_Exp*_tmpAC2=_tmp46A;Cyc_Absyn_add_exp(_tmpAC2,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp46A=_tmpAC3;});
if(init_exp != 0)
({struct Cyc_Absyn_Stmt*_tmpACB=({struct Cyc_Toc_Env*_tmpACA=nv;void*_tmpAC9=new_e_type;struct Cyc_Absyn_Exp*_tmpAC8=
Cyc_Absyn_var_exp(a,0U);
# 2356
struct Cyc_Absyn_Vardecl*_tmpAC7=(struct Cyc_Absyn_Vardecl*)_check_null(vd);struct Cyc_Absyn_Exp*_tmpAC6=
Cyc_Absyn_var_exp(max,0U);
# 2356
struct Cyc_Absyn_Exp*_tmpAC5=init_exp;int _tmpAC4=zero_term;Cyc_Toc_init_comprehension(_tmpACA,_tmpAC9,_tmpAC8,_tmpAC7,_tmpAC6,_tmpAC5,_tmpAC4,
# 2358
Cyc_Toc_skip_stmt_dl(),1);});
# 2356
s=_tmpACB;});else{
# 2359
if(zero_term)
({struct Cyc_Absyn_Stmt*_tmpACF=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpACE=({struct Cyc_Absyn_Exp*_tmpACC=Cyc_Absyn_var_exp(a,0U);Cyc_Absyn_subscript_exp(_tmpACC,Cyc_Absyn_var_exp(max,0U),0U);});Cyc_Absyn_assign_exp(_tmpACE,({
void*_tmpACD=elt_typ;Cyc_Toc_cast_it(_tmpACD,Cyc_Absyn_uint_exp(0U,0U));}),0U);}),0U);
# 2360
s=_tmpACF;});else{
# 2363
({struct Cyc_Absyn_Stmt*_tmpAD0=Cyc_Absyn_skip_stmt(0U);s=_tmpAD0;});}}{
# 2365
struct _RegionHandle _tmp46B=_new_region("r");struct _RegionHandle*r=& _tmp46B;_push_region(r);
{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp47F=_region_malloc(r,sizeof(*_tmp47F));({struct _tuple28*_tmpAD1=({struct _tuple28*_tmp480=_region_malloc(r,sizeof(*_tmp480));_tmp480->f1=max;_tmp480->f2=Cyc_Absyn_uint_typ;_tmp480->f3=e1;_tmp480;});_tmp47F->hd=_tmpAD1;});_tmp47F->tl=0;_tmp47F;});
# 2368
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmpAD5=({void*_tmpAD4=old_elt_typ;Cyc_Toc_malloc_exp(_tmpAD4,({
struct Cyc_Absyn_Exp*_tmpAD3=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmpAD3,({struct Cyc_Absyn_Exp*_tmp46C[2U];_tmp46C[1U]=_tmp46A;({struct Cyc_Absyn_Exp*_tmpAD2=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp46C[0U]=_tmpAD2;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46C,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);}));});
# 2370
ai=_tmpAD5;});else{
# 2375
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmpAD9=({struct Cyc_Absyn_Exp*_tmpAD8=r;Cyc_Toc_rmalloc_exp(_tmpAD8,({struct Cyc_Absyn_Exp*_tmpAD7=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmpAD7,({struct Cyc_Absyn_Exp*_tmp46D[2U];_tmp46D[1U]=_tmp46A;({struct Cyc_Absyn_Exp*_tmpAD6=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp46D[0U]=_tmpAD6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46D,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);}));});
# 2377
ai=_tmpAD9;});}{
# 2381
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
({struct Cyc_List_List*_tmpADB=({struct Cyc_List_List*_tmp46E=_region_malloc(r,sizeof(*_tmp46E));({struct _tuple28*_tmpADA=({struct _tuple28*_tmp46F=_region_malloc(r,sizeof(*_tmp46F));_tmp46F->f1=a;_tmp46F->f2=ptr_typ;_tmp46F->f3=ainit;_tmp46F;});_tmp46E->hd=_tmpADA;});_tmp46E->tl=decls;_tmp46E;});decls=_tmpADB;});
if(!Cyc_Tcutil_is_pointer_type(old_typ))
({void*_tmp470=0U;({struct _dyneither_ptr _tmpADC=({const char*_tmp471="new_comprehension_to_c: comprehension is not an array type";_tag_dyneither(_tmp471,sizeof(char),59U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpADC,_tag_dyneither(_tmp470,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp472=Cyc_Toc_temp_var();
void*_tmp473=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp474=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp475=({
struct Cyc_Absyn_Exp*_tmpADF=_tmp474;Cyc_Absyn_fncall_exp(_tmpADF,({struct Cyc_Absyn_Exp*_tmp478[3U];_tmp478[2U]=_tmp46A;({struct Cyc_Absyn_Exp*_tmpADD=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp478[1U]=_tmpADD;});({struct Cyc_Absyn_Exp*_tmpADE=Cyc_Absyn_var_exp(a,0U);_tmp478[0U]=_tmpADE;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp478,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2393
({struct Cyc_List_List*_tmpAE1=({struct Cyc_List_List*_tmp476=_region_malloc(r,sizeof(*_tmp476));({struct _tuple28*_tmpAE0=({struct _tuple28*_tmp477=_region_malloc(r,sizeof(*_tmp477));_tmp477->f1=_tmp472;_tmp477->f2=_tmp473;_tmp477->f3=_tmp475;_tmp477;});_tmp476->hd=_tmpAE0;});_tmp476->tl=decls;_tmp476;});decls=_tmpAE1;});
({struct Cyc_Absyn_Stmt*_tmpAE3=({struct Cyc_Absyn_Stmt*_tmpAE2=s;Cyc_Absyn_seq_stmt(_tmpAE2,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp472,0U),0U),0U);});s=_tmpAE3;});}else{
# 2396
({struct Cyc_Absyn_Stmt*_tmpAE5=({struct Cyc_Absyn_Stmt*_tmpAE4=s;Cyc_Absyn_seq_stmt(_tmpAE4,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0U),0U),0U);});s=_tmpAE5;});}
{struct Cyc_List_List*_tmp479=decls;for(0;_tmp479 != 0;_tmp479=_tmp479->tl){
struct _tuple28 _tmp47A=*((struct _tuple28*)_tmp479->hd);struct _tuple28 _tmp47B=_tmp47A;struct _tuple1*_tmp47E;void*_tmp47D;struct Cyc_Absyn_Exp*_tmp47C;_LL1: _tmp47E=_tmp47B.f1;_tmp47D=_tmp47B.f2;_tmp47C=_tmp47B.f3;_LL2:;
({struct Cyc_Absyn_Stmt*_tmpAE6=Cyc_Absyn_declare_stmt(_tmp47E,_tmp47D,_tmp47C,s,0U);s=_tmpAE6;});}}
# 2401
({void*_tmpAE7=Cyc_Toc_stmt_exp_r(s);e->r=_tmpAE7;});};}
# 2366
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2405
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp481=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp484;_tmp484.tag=0U;({struct _dyneither_ptr _tmpAE8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp484.f1=_tmpAE8;});({void*_tmp482[1U]={& _tmp484};({struct _dyneither_ptr _tmpAE9=({const char*_tmp483="exp_to_c: no type for %s";_tag_dyneither(_tmp483,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAE9,_tag_dyneither(_tmp482,sizeof(void*),1U));});});});
# 2411
if((nv->struct_info).lhs_exp != 0){
void*_tmp485=_tmp481;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp485)->tag == 29U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
({struct Cyc_Toc_Env*_tmpAEA=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);nv=_tmpAEA;});}_LL0:;}{
# 2419
void*old_typ=(void*)_check_null(e->topt);
void*_tmp486=_tmp481;struct Cyc_Absyn_Stmt*_tmp6A6;struct Cyc_Absyn_Exp*_tmp6A5;struct _dyneither_ptr*_tmp6A4;struct Cyc_Absyn_Exp*_tmp6A3;struct Cyc_Absyn_Exp*_tmp6A2;int _tmp6A1;struct Cyc_Absyn_Exp*_tmp6A0;void**_tmp69F;struct Cyc_Absyn_Exp*_tmp69E;int _tmp69D;int _tmp69C;struct Cyc_List_List*_tmp69B;struct Cyc_Absyn_Datatypedecl*_tmp69A;struct Cyc_Absyn_Datatypefield*_tmp699;void*_tmp698;struct Cyc_List_List*_tmp697;struct _tuple1*_tmp696;struct Cyc_List_List*_tmp695;struct Cyc_List_List*_tmp694;struct Cyc_Absyn_Aggrdecl*_tmp693;struct Cyc_Absyn_Exp*_tmp692;void*_tmp691;int _tmp690;struct Cyc_Absyn_Vardecl*_tmp68F;struct Cyc_Absyn_Exp*_tmp68E;struct Cyc_Absyn_Exp*_tmp68D;int _tmp68C;struct Cyc_List_List*_tmp68B;struct Cyc_List_List*_tmp68A;struct Cyc_Absyn_Exp*_tmp689;struct Cyc_Absyn_Exp*_tmp688;struct Cyc_Absyn_Exp*_tmp687;struct _dyneither_ptr*_tmp686;int _tmp685;int _tmp684;struct Cyc_Absyn_Exp*_tmp683;struct _dyneither_ptr*_tmp682;int _tmp681;int _tmp680;struct Cyc_Absyn_Exp*_tmp67F;void*_tmp67E;struct Cyc_List_List*_tmp67D;void*_tmp67C;struct Cyc_Absyn_Exp*_tmp67B;struct Cyc_Absyn_Exp*_tmp67A;struct Cyc_Absyn_Exp*_tmp679;struct Cyc_Absyn_Exp*_tmp678;void**_tmp677;struct Cyc_Absyn_Exp*_tmp676;int _tmp675;enum Cyc_Absyn_Coercion _tmp674;struct Cyc_Absyn_Exp*_tmp673;struct Cyc_List_List*_tmp672;struct Cyc_Absyn_Exp*_tmp671;struct Cyc_Absyn_Exp*_tmp670;int _tmp66F;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_List_List*_tmp66D;int _tmp66C;struct Cyc_List_List*_tmp66B;struct Cyc_Absyn_VarargInfo*_tmp66A;struct Cyc_Absyn_Exp*_tmp669;struct Cyc_List_List*_tmp668;struct Cyc_Absyn_Exp*_tmp667;struct Cyc_Absyn_Exp*_tmp666;struct Cyc_Absyn_Exp*_tmp665;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_Absyn_Exp*_tmp663;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_Absyn_Exp*_tmp661;struct Cyc_Absyn_Exp*_tmp660;struct Cyc_Absyn_Exp*_tmp65F;struct Cyc_Absyn_Exp*_tmp65E;struct Cyc_Core_Opt*_tmp65D;struct Cyc_Absyn_Exp*_tmp65C;struct Cyc_Absyn_Exp*_tmp65B;enum Cyc_Absyn_Incrementor _tmp65A;enum Cyc_Absyn_Primop _tmp659;struct Cyc_List_List*_tmp658;switch(*((int*)_tmp486)){case 2U: _LL6: _LL7:
# 2422
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL5;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp486)->f1).Null_c).tag == 1){_LL8: _LL9: {
# 2429
struct Cyc_Absyn_Exp*_tmp487=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmpAEB=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp487,_tmp487))->r;e->r=_tmpAEB;});else{
# 2434
({void*_tmpAED=({struct Cyc_Absyn_Exp*_tmpAEC=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmpAEC,({struct Cyc_Absyn_Exp*_tmp488[3U];_tmp488[2U]=_tmp487;_tmp488[1U]=_tmp487;_tmp488[0U]=_tmp487;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp488,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmpAED;});}}else{
# 2436
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2438
goto _LL5;}}else{_LLA: _LLB:
 goto _LL5;}case 1U: _LLC: _LLD:
 goto _LL5;case 3U: _LLE: _tmp659=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp658=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LLF: {
# 2443
struct Cyc_List_List*_tmp489=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp658);
# 2445
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp658);
{enum Cyc_Absyn_Primop _tmp48A=_tmp659;switch(_tmp48A){case Cyc_Absyn_Numelts: _LL5D: _LL5E: {
# 2448
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp658))->hd;
{void*_tmp48B=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp48C=_tmp48B;void*_tmp49E;union Cyc_Absyn_Constraint*_tmp49D;union Cyc_Absyn_Constraint*_tmp49C;union Cyc_Absyn_Constraint*_tmp49B;struct Cyc_Absyn_Exp*_tmp49A;switch(*((int*)_tmp48C)){case 8U: _LL72: _tmp49A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48C)->f1).num_elts;_LL73:
# 2452
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp49A)))
({void*_tmp48D=0U;({unsigned int _tmpAEF=e->loc;struct _dyneither_ptr _tmpAEE=({const char*_tmp48E="can't calculate numelts";_tag_dyneither(_tmp48E,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpAEF,_tmpAEE,_tag_dyneither(_tmp48D,sizeof(void*),0U));});});
e->r=_tmp49A->r;goto _LL71;case 5U: _LL74: _tmp49E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).elt_typ;_tmp49D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).ptr_atts).nullable;_tmp49C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).ptr_atts).bounds;_tmp49B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).ptr_atts).zero_term;_LL75:
# 2456
{void*_tmp48F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp49C);void*_tmp490=_tmp48F;struct Cyc_Absyn_Exp*_tmp495;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp490)->tag == 0U){_LL79: _LL7A:
# 2458
({void*_tmpAF2=({struct Cyc_Absyn_Exp*_tmpAF1=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmpAF1,({struct Cyc_Absyn_Exp*_tmp491[2U];({struct Cyc_Absyn_Exp*_tmpAF0=
# 2460
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp49E),0U);_tmp491[1U]=_tmpAF0;});_tmp491[0U]=(struct Cyc_Absyn_Exp*)_tmp658->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp491,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2458
e->r=_tmpAF2;});
# 2461
goto _LL78;}else{_LL7B: _tmp495=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp490)->f1;_LL7C:
# 2464
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp49B)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp658->hd);
# 2468
({void*_tmpAF4=({struct Cyc_Absyn_Exp*_tmpAF3=function_e;Cyc_Toc_fncall_exp_r(_tmpAF3,({struct Cyc_Absyn_Exp*_tmp492[2U];_tmp492[1U]=_tmp495;_tmp492[0U]=(struct Cyc_Absyn_Exp*)_tmp658->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp492,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmpAF4;});}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp49D)){
if(!Cyc_Evexp_c_can_eval(_tmp495))
({void*_tmp493=0U;({unsigned int _tmpAF6=e->loc;struct _dyneither_ptr _tmpAF5=({const char*_tmp494="can't calculate numelts";_tag_dyneither(_tmp494,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpAF6,_tmpAF5,_tag_dyneither(_tmp493,sizeof(void*),0U));});});
# 2473
({void*_tmpAF9=({struct Cyc_Absyn_Exp*_tmpAF8=arg;struct Cyc_Absyn_Exp*_tmpAF7=_tmp495;Cyc_Toc_conditional_exp_r(_tmpAF8,_tmpAF7,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmpAF9;});}else{
# 2475
e->r=_tmp495->r;goto _LL78;}}
# 2477
goto _LL78;}_LL78:;}
# 2479
goto _LL71;default: _LL76: _LL77:
# 2481
({struct Cyc_String_pa_PrintArg_struct _tmp499;_tmp499.tag=0U;({struct _dyneither_ptr _tmpAFA=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp499.f1=_tmpAFA;});({struct Cyc_String_pa_PrintArg_struct _tmp498;_tmp498.tag=0U;({struct _dyneither_ptr _tmpAFB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp498.f1=_tmpAFB;});({void*_tmp496[2U]={& _tmp498,& _tmp499};({struct _dyneither_ptr _tmpAFC=({const char*_tmp497="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp497,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAFC,_tag_dyneither(_tmp496,sizeof(void*),2U));});});});});}_LL71:;}
# 2484
goto _LL5C;}case Cyc_Absyn_Plus: _LL5F: _LL60:
# 2489
{void*_tmp49F=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp489))->hd);void*_tmp4A0=_tmp49F;void*_tmp4A8;union Cyc_Absyn_Constraint*_tmp4A7;union Cyc_Absyn_Constraint*_tmp4A6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->tag == 5U){_LL7E: _tmp4A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).elt_typ;_tmp4A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).ptr_atts).bounds;_tmp4A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).ptr_atts).zero_term;_LL7F:
# 2491
{void*_tmp4A1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4A7);void*_tmp4A2=_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A5;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4A2)->tag == 0U){_LL83: _LL84: {
# 2493
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp658))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp658->tl))->hd;
({void*_tmpAFF=({struct Cyc_Absyn_Exp*_tmpAFE=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmpAFE,({struct Cyc_Absyn_Exp*_tmp4A3[3U];_tmp4A3[2U]=e2;({struct Cyc_Absyn_Exp*_tmpAFD=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4A8),0U);_tmp4A3[1U]=_tmpAFD;});_tmp4A3[0U]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A3,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2495
e->r=_tmpAFF;});
# 2497
goto _LL82;}}else{_LL85: _tmp4A5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4A2)->f1;_LL86:
# 2499
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4A6)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp658))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp658->tl))->hd;
({void*_tmpB01=({struct Cyc_Absyn_Exp*_tmpB00=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmpB00,({struct Cyc_Absyn_Exp*_tmp4A4[3U];_tmp4A4[2U]=e2;_tmp4A4[1U]=_tmp4A5;_tmp4A4[0U]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A4,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmpB01;});}
# 2504
goto _LL82;}_LL82:;}
# 2506
goto _LL7D;}else{_LL80: _LL81:
# 2508
 goto _LL7D;}_LL7D:;}
# 2510
goto _LL5C;case Cyc_Absyn_Minus: _LL61: _LL62: {
# 2515
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp489))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp658))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp658->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp489->tl))->hd)){
({void*_tmpB03=({struct Cyc_Absyn_Exp*_tmpB02=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmpB02,Cyc_Toc_curr_sp);});e1->r=_tmpB03;});
({void*_tmpB05=({struct Cyc_Absyn_Exp*_tmpB04=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmpB04,Cyc_Toc_curr_sp);});e2->r=_tmpB05;});
({void*_tmpB07=({void*_tmpB06=Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_typ,Cyc_Toc_mt_tq);e2->topt=_tmpB06;});e1->topt=_tmpB07;});
({void*_tmpB09=({struct Cyc_Absyn_Exp*_tmpB08=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmpB08,
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U),0U);})->r;
# 2523
e->r=_tmpB09;});}else{
# 2526
({void*_tmpB0D=({struct Cyc_Absyn_Exp*_tmpB0C=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmpB0C,({struct Cyc_Absyn_Exp*_tmp4A9[3U];({struct Cyc_Absyn_Exp*_tmpB0A=
# 2528
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp4A9[2U]=_tmpB0A;});({struct Cyc_Absyn_Exp*_tmpB0B=
# 2527
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp4A9[1U]=_tmpB0B;});_tmp4A9[0U]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A9,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2526
e->r=_tmpB0D;});}}
# 2531
goto _LL5C;}case Cyc_Absyn_Eq: _LL63: _LL64:
 goto _LL66;case Cyc_Absyn_Neq: _LL65: _LL66:
 goto _LL68;case Cyc_Absyn_Gt: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Gte: _LL69: _LL6A:
 goto _LL6C;case Cyc_Absyn_Lt: _LL6B: _LL6C:
 goto _LL6E;case Cyc_Absyn_Lte: _LL6D: _LL6E: {
# 2539
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp658))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp658->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp489))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp489->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ)){
void*_tmp4AA=({void*_tmpB0E=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmpB0E,Cyc_Toc_mt_tq);});
({void*_tmpB11=({void*_tmpB10=_tmp4AA;Cyc_Toc_cast_it_r(_tmpB10,({struct Cyc_Absyn_Exp*_tmpB0F=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmpB0F,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmpB11;});
e1->topt=_tmp4AA;}
# 2549
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
void*_tmp4AB=({void*_tmpB12=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmpB12,Cyc_Toc_mt_tq);});
({void*_tmpB15=({void*_tmpB14=_tmp4AB;Cyc_Toc_cast_it_r(_tmpB14,({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmpB13,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmpB15;});
e2->topt=_tmp4AB;}
# 2554
goto _LL5C;}default: _LL6F: _LL70:
 goto _LL5C;}_LL5C:;}
# 2557
goto _LL5;}case 5U: _LL10: _tmp65B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp65A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL11: {
# 2559
void*e2_cyc_typ=(void*)_check_null(_tmp65B->topt);
# 2568 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp4C2="increment";_tag_dyneither(_tmp4C2,sizeof(char),10U);});
if(_tmp65A == Cyc_Absyn_PreDec  || _tmp65A == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmpB16=({const char*_tmp4AC="decrement";_tag_dyneither(_tmp4AC,sizeof(char),10U);});incr_str=_tmpB16;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp65B,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp4AF;_tmp4AF.tag=0U;_tmp4AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4AD[1U]={& _tmp4AF};({unsigned int _tmpB18=e->loc;struct _dyneither_ptr _tmpB17=({const char*_tmp4AE="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp4AE,sizeof(char),74U);});Cyc_Tcutil_terr(_tmpB18,_tmpB17,_tag_dyneither(_tmp4AD,sizeof(void*),1U));});});});
({void*_tmp4B0=0U;({struct _dyneither_ptr _tmpB19=({const char*_tmp4B1="in-place inc/dec on zero-term";_tag_dyneither(_tmp4B1,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB19,_tag_dyneither(_tmp4B0,sizeof(void*),0U));});});}{
# 2577
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp65B,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_signed_int_exp(1,0U);
_tmp4B2->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4B3=_tmp65A;switch(_tmp4B3){case Cyc_Absyn_PreInc: _LL88: _LL89:
# 2584
({void*_tmpB1C=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB1B=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4B5;_tmp4B5.tag=4U;_tmp4B5.f1=_tmp65B;({struct Cyc_Core_Opt*_tmpB1A=({struct Cyc_Core_Opt*_tmp4B6=_cycalloc_atomic(sizeof(*_tmp4B6));_tmp4B6->v=(void*)Cyc_Absyn_Plus;_tmp4B6;});_tmp4B5.f2=_tmpB1A;});_tmp4B5.f3=_tmp4B2;_tmp4B5;});_tmp4B4[0]=_tmpB1B;});_tmp4B4;});e->r=_tmpB1C;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL8A: _LL8B:
# 2588
({void*_tmpB1F=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB1E=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4B8;_tmp4B8.tag=4U;_tmp4B8.f1=_tmp65B;({struct Cyc_Core_Opt*_tmpB1D=({struct Cyc_Core_Opt*_tmp4B9=_cycalloc_atomic(sizeof(*_tmp4B9));_tmp4B9->v=(void*)Cyc_Absyn_Minus;_tmp4B9;});_tmp4B8.f2=_tmpB1D;});_tmp4B8.f3=_tmp4B2;_tmp4B8;});_tmp4B7[0]=_tmpB1E;});_tmp4B7;});e->r=_tmpB1F;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL8C: _LL8D:
# 2592
({struct Cyc_String_pa_PrintArg_struct _tmp4BC;_tmp4BC.tag=0U;_tmp4BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4BA[1U]={& _tmp4BC};({unsigned int _tmpB21=e->loc;struct _dyneither_ptr _tmpB20=({const char*_tmp4BB="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp4BB,sizeof(char),58U);});Cyc_Tcutil_terr(_tmpB21,_tmpB20,_tag_dyneither(_tmp4BA,sizeof(void*),1U));});});});
# 2594
({void*_tmp4BD=0U;({struct _dyneither_ptr _tmpB22=({const char*_tmp4BE="in-place inc/dec on @tagged union";_tag_dyneither(_tmp4BE,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB22,_tag_dyneither(_tmp4BD,sizeof(void*),0U));});});}_LL87:;};}
# 2597
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp65B);
Cyc_Toc_set_lhs(nv,0);{
# 2602
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp65A == Cyc_Absyn_PostInc  || _tmp65A == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2608
if(_tmp65A == Cyc_Absyn_PreDec  || _tmp65A == Cyc_Absyn_PostDec)
change=-1;
({void*_tmpB27=({struct Cyc_Absyn_Exp*_tmpB26=fn_e;Cyc_Toc_fncall_exp_r(_tmpB26,({struct Cyc_Absyn_Exp*_tmp4BF[3U];({struct Cyc_Absyn_Exp*_tmpB23=
# 2612
Cyc_Absyn_signed_int_exp(change,0U);_tmp4BF[2U]=_tmpB23;});({struct Cyc_Absyn_Exp*_tmpB24=
# 2611
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp4BF[1U]=_tmpB24;});({struct Cyc_Absyn_Exp*_tmpB25=
# 2610
Cyc_Toc_push_address_exp(_tmp65B);_tmp4BF[0U]=_tmpB25;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BF,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmpB27;});}else{
# 2613
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2617
struct Cyc_Toc_functionSet*_tmp4C0=_tmp65A != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2619
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp4C0,_tmp65B);
({void*_tmpB2B=({struct Cyc_Absyn_Exp*_tmpB2A=fn_e;Cyc_Toc_fncall_exp_r(_tmpB2A,({struct Cyc_Absyn_Exp*_tmp4C1[2U];({struct Cyc_Absyn_Exp*_tmpB28=
Cyc_Absyn_signed_int_exp(1,0U);_tmp4C1[1U]=_tmpB28;});({struct Cyc_Absyn_Exp*_tmpB29=
# 2620
Cyc_Toc_push_address_exp(_tmp65B);_tmp4C1[0U]=_tmpB29;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C1,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmpB2B;});}else{
# 2622
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp65B)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp65B,0,Cyc_Toc_incr_lvalue,_tmp65A);
e->r=_tmp65B->r;}}}
# 2626
goto _LL5;};};}case 4U: _LL12: _tmp65E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp65D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp65C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL13: {
# 2645 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp65E->topt);
void*e2_old_typ=(void*)_check_null(_tmp65C->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp65E,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp65E);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp65C);
({void*_tmpB2C=Cyc_Toc_tagged_union_assignop(_tmp65E,e1_old_typ,_tmp65D,_tmp65C,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmpB2C;});
# 2656
return;}{
# 2658
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp65E,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp65E,_tmp65D,_tmp65C,ptr_type,is_dyneither,elt_type);
# 2664
return;}{
# 2668
int e1_poly=Cyc_Toc_is_poly_project(_tmp65E);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp65E);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp65C);{
# 2674
int done=0;
if(_tmp65D != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp4C3=(enum Cyc_Absyn_Primop)_tmp65D->v;enum Cyc_Absyn_Primop _tmp4C4=_tmp4C3;switch(_tmp4C4){case Cyc_Absyn_Plus: _LL8F: _LL90:
 change=_tmp65C;goto _LL8E;case Cyc_Absyn_Minus: _LL91: _LL92:
# 2682
({struct Cyc_Absyn_Exp*_tmpB2D=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp65C,0U);change=_tmpB2D;});goto _LL8E;default: _LL93: _LL94:
({void*_tmp4C5=0U;({struct _dyneither_ptr _tmpB2E=({const char*_tmp4C6="bad t ? pointer arithmetic";_tag_dyneither(_tmp4C6,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB2E,_tag_dyneither(_tmp4C5,sizeof(void*),0U));});});}_LL8E:;}
# 2685
done=1;{
# 2687
struct Cyc_Absyn_Exp*_tmp4C7=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmpB32=({struct Cyc_Absyn_Exp*_tmpB31=_tmp4C7;Cyc_Toc_fncall_exp_r(_tmpB31,({struct Cyc_Absyn_Exp*_tmp4C8[3U];_tmp4C8[2U]=change;({struct Cyc_Absyn_Exp*_tmpB2F=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp4C8[1U]=_tmpB2F;});({struct Cyc_Absyn_Exp*_tmpB30=
# 2688
Cyc_Toc_push_address_exp(_tmp65E);_tmp4C8[0U]=_tmpB30;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C8,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmpB32;});};}else{
# 2691
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2694
enum Cyc_Absyn_Primop _tmp4C9=(enum Cyc_Absyn_Primop)_tmp65D->v;enum Cyc_Absyn_Primop _tmp4CA=_tmp4C9;if(_tmp4CA == Cyc_Absyn_Plus){_LL96: _LL97:
# 2696
 done=1;
({void*_tmpB34=({struct Cyc_Absyn_Exp*_tmpB33=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp65E);Cyc_Toc_fncall_exp_r(_tmpB33,({struct Cyc_Absyn_Exp*_tmp4CB[2U];_tmp4CB[1U]=_tmp65C;_tmp4CB[0U]=_tmp65E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CB,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmpB34;});
goto _LL95;}else{_LL98: _LL99:
({void*_tmp4CC=0U;({struct _dyneither_ptr _tmpB35=({const char*_tmp4CD="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp4CD,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB35,_tag_dyneither(_tmp4CC,sizeof(void*),0U));});});}_LL95:;}}}
# 2703
if(!done){
# 2705
if(e1_poly)
({void*_tmpB37=({void*_tmpB36=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpB36,Cyc_Absyn_new_exp(_tmp65C->r,0U));});_tmp65C->r=_tmpB37;});
# 2711
if(!Cyc_Absyn_is_lvalue(_tmp65E)){
({struct Cyc_Absyn_Exp*_tmpB39=_tmp65E;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmpB39,0,Cyc_Toc_assignop_lvalue,({struct _tuple27*_tmp4CE=_cycalloc(sizeof(struct _tuple27)* 1);({typeof((struct _tuple27)({struct _tuple27 _tmp4CF;_tmp4CF.f1=_tmp65D;_tmp4CF.f2=_tmp65C;_tmp4CF;}))_tmpB38=(struct _tuple27)({struct _tuple27 _tmp4CF;_tmp4CF.f1=_tmp65D;_tmp4CF.f2=_tmp65C;_tmp4CF;});_tmp4CE[0]=_tmpB38;});_tmp4CE;}));});
e->r=_tmp65E->r;}}
# 2716
goto _LL5;};};};}case 6U: _LL14: _tmp661=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp660=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp65F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL15:
# 2718
 Cyc_Toc_exp_to_c(nv,_tmp661);
Cyc_Toc_exp_to_c(nv,_tmp660);
Cyc_Toc_exp_to_c(nv,_tmp65F);
goto _LL5;case 7U: _LL16: _tmp663=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp662=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL17:
# 2723
 Cyc_Toc_exp_to_c(nv,_tmp663);
Cyc_Toc_exp_to_c(nv,_tmp662);
goto _LL5;case 8U: _LL18: _tmp665=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp664=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL19:
# 2727
 Cyc_Toc_exp_to_c(nv,_tmp665);
Cyc_Toc_exp_to_c(nv,_tmp664);
goto _LL5;case 9U: _LL1A: _tmp667=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp666=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL1B:
# 2731
 Cyc_Toc_exp_to_c(nv,_tmp667);
Cyc_Toc_exp_to_c(nv,_tmp666);
goto _LL5;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3 == 0){_LL1C: _tmp669=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp668=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL1D:
# 2735
 Cyc_Toc_exp_to_c(nv,_tmp669);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp668);
goto _LL5;}else{_LL1E: _tmp66E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp66D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp66C=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3)->num_varargs;_tmp66B=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3)->injectors;_tmp66A=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3)->vai;_LL1F: {
# 2747 "toc.cyc"
struct _RegionHandle _tmp4D0=_new_region("r");struct _RegionHandle*r=& _tmp4D0;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp66C,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp66A->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0U);
# 2755
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp66D);
int num_normargs=num_args - _tmp66C;
# 2759
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp66D=_tmp66D->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp66D))->hd);
({struct Cyc_List_List*_tmpB3A=({struct Cyc_List_List*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1->hd=(struct Cyc_Absyn_Exp*)_tmp66D->hd;_tmp4D1->tl=new_args;_tmp4D1;});new_args=_tmpB3A;});}}
# 2764
({struct Cyc_List_List*_tmpB3E=({struct Cyc_List_List*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));({struct Cyc_Absyn_Exp*_tmpB3D=({struct Cyc_Absyn_Exp*_tmpB3C=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpB3C,({struct Cyc_Absyn_Exp*_tmp4D3[3U];_tmp4D3[2U]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmpB3B=
# 2766
Cyc_Absyn_sizeoftyp_exp(cva_type,0U);_tmp4D3[1U]=_tmpB3B;});_tmp4D3[0U]=argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D3,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});_tmp4D2->hd=_tmpB3D;});_tmp4D2->tl=new_args;_tmp4D2;});
# 2764
new_args=_tmpB3E;});
# 2769
({struct Cyc_List_List*_tmpB3F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmpB3F;});
# 2771
Cyc_Toc_exp_to_c(nv,_tmp66E);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp66E,new_args,0U),0U);
# 2775
if(_tmp66A->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp4D4=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp66A->type));void*_tmp4D5=_tmp4D4;struct Cyc_Absyn_Datatypedecl*_tmp4D8;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D5)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D5)->f1).datatype_info).KnownDatatype).tag == 2){_LL9B: _tmp4D8=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D5)->f1).datatype_info).KnownDatatype).val;_LL9C:
 tud=_tmp4D8;goto _LL9A;}else{goto _LL9D;}}else{_LL9D: _LL9E:
({void*_tmp4D6=0U;({struct _dyneither_ptr _tmpB40=({const char*_tmp4D7="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp4D7,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB40,_tag_dyneither(_tmp4D6,sizeof(void*),0U));});});}_LL9A:;}{
# 2781
struct _dyneither_ptr vs=({unsigned int _tmp4E1=(unsigned int)_tmp66C;struct _tuple1**_tmp4E2=(struct _tuple1**)({struct _RegionHandle*_tmpB41=r;_region_malloc(_tmpB41,_check_times(sizeof(struct _tuple1*),_tmp4E1));});struct _dyneither_ptr _tmp4E4=_tag_dyneither(_tmp4E2,sizeof(struct _tuple1*),_tmp4E1);{unsigned int _tmp4E3=_tmp4E1;unsigned int i;for(i=0;i < _tmp4E3;i ++){({typeof((struct _tuple1*)Cyc_Toc_temp_var())_tmpB42=(struct _tuple1*)Cyc_Toc_temp_var();_tmp4E2[i]=_tmpB42;});}}_tmp4E4;});
# 2783
if(_tmp66C != 0){
# 2785
struct Cyc_List_List*_tmp4D9=0;
{int i=_tmp66C - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmpB44=({struct Cyc_List_List*_tmp4DA=_cycalloc(sizeof(*_tmp4DA));({struct _tuple19*_tmpB43=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U));_tmp4DA->hd=_tmpB43;});_tmp4DA->tl=_tmp4D9;_tmp4DA;});_tmp4D9=_tmpB44;});}}
# 2789
({struct Cyc_Absyn_Stmt*_tmpB48=({struct _tuple1*_tmpB47=argv;void*_tmpB46=arr_type;struct Cyc_Absyn_Exp*_tmpB45=Cyc_Absyn_unresolvedmem_exp(0,_tmp4D9,0U);Cyc_Absyn_declare_stmt(_tmpB47,_tmpB46,_tmpB45,s,0U);});s=_tmpB48;});{
# 2792
int i=0;for(0;_tmp66D != 0;(((_tmp66D=_tmp66D->tl,_tmp66B=_tmp66B->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp66D->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp4DB=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp66B))->hd;struct Cyc_Absyn_Datatypefield*_tmp4DC=_tmp4DB;struct _tuple1*_tmp4DE;struct Cyc_List_List*_tmp4DD;_LLA0: _tmp4DE=_tmp4DC->name;_tmp4DD=_tmp4DC->typs;_LLA1:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp4DD))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmpB49=Cyc_Toc_cast_it(field_typ,arg);arg=_tmpB49;});
# 2807
({struct Cyc_Absyn_Stmt*_tmpB4D=({struct Cyc_Absyn_Stmt*_tmpB4C=({struct Cyc_Absyn_Exp*_tmpB4B=({struct Cyc_Absyn_Exp*_tmpB4A=varexp;Cyc_Toc_member_exp(_tmpB4A,Cyc_Absyn_fieldname(1),0U);});Cyc_Absyn_assign_stmt(_tmpB4B,arg,0U);});Cyc_Absyn_seq_stmt(_tmpB4C,s,0U);});s=_tmpB4D;});
# 2810
({struct Cyc_Absyn_Stmt*_tmpB50=({struct Cyc_Absyn_Stmt*_tmpB4F=({struct Cyc_Absyn_Exp*_tmpB4E=Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0U);Cyc_Absyn_assign_stmt(_tmpB4E,
Cyc_Toc_datatype_tag(tud,_tmp4DE),0U);});
# 2810
Cyc_Absyn_seq_stmt(_tmpB4F,s,0U);});s=_tmpB50;});
# 2813
({struct Cyc_Absyn_Stmt*_tmpB53=({struct _tuple1*_tmpB52=var;void*_tmpB51=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4DE,tud->name));Cyc_Absyn_declare_stmt(_tmpB52,_tmpB51,0,s,0U);});s=_tmpB53;});};}};}else{
# 2820
struct Cyc_List_List*_tmp4DF=({struct _tuple19*_tmp4E0[3U];({struct _tuple19*_tmpB54=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0U,0U));_tmp4E0[2U]=_tmpB54;});({struct _tuple19*_tmpB55=
# 2820
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0U,0U));_tmp4E0[1U]=_tmpB55;});({struct _tuple19*_tmpB56=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0U,0U));_tmp4E0[0U]=_tmpB56;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E0,sizeof(struct _tuple19*),3U));});
# 2822
({struct Cyc_Absyn_Stmt*_tmpB5A=({struct _tuple1*_tmpB59=argv;void*_tmpB58=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmpB57=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmpB59,_tmpB58,_tmpB57,s,0U);});s=_tmpB5A;});}};}else{
# 2832
{int i=0;for(0;_tmp66D != 0;(_tmp66D=_tmp66D->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp66D->hd);
({struct Cyc_Absyn_Stmt*_tmpB5E=({struct Cyc_Absyn_Stmt*_tmpB5D=({struct Cyc_Absyn_Exp*_tmpB5C=({struct Cyc_Absyn_Exp*_tmpB5B=argvexp;Cyc_Absyn_subscript_exp(_tmpB5B,Cyc_Absyn_uint_exp((unsigned int)i,0U),0U);});Cyc_Absyn_assign_stmt(_tmpB5C,(struct Cyc_Absyn_Exp*)_tmp66D->hd,0U);});Cyc_Absyn_seq_stmt(_tmpB5D,s,0U);});s=_tmpB5E;});}}
# 2838
({struct Cyc_Absyn_Stmt*_tmpB5F=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0U);s=_tmpB5F;});}
# 2851 "toc.cyc"
({void*_tmpB60=Cyc_Toc_stmt_exp_r(s);e->r=_tmpB60;});};}
# 2853
_npop_handler(0U);goto _LL5;
# 2747 "toc.cyc"
;_pop_region(r);}}case 11U: _LL20: _tmp670=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp66F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL21:
# 2856 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp670);{
struct Cyc_Absyn_Exp*_tmp4E5=_tmp66F?Cyc_Toc_newrethrow_exp(_tmp670): Cyc_Toc_newthrow_exp(_tmp670);
({void*_tmpB62=({void*_tmpB61=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmpB61,_tmp4E5,0U);})->r;e->r=_tmpB62;});
goto _LL5;};case 12U: _LL22: _tmp671=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL23:
 Cyc_Toc_exp_to_c(nv,_tmp671);goto _LL5;case 13U: _LL24: _tmp673=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp672=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL25:
# 2862
 Cyc_Toc_exp_to_c(nv,_tmp673);
# 2871 "toc.cyc"
for(0;_tmp672 != 0;_tmp672=_tmp672->tl){
enum Cyc_Absyn_KindQual _tmp4E6=(Cyc_Tcutil_typ_kind((void*)_tmp672->hd))->kind;
if(_tmp4E6 != Cyc_Absyn_EffKind  && _tmp4E6 != Cyc_Absyn_RgnKind){
{void*_tmp4E7=Cyc_Tcutil_compress((void*)_tmp672->hd);void*_tmp4E8=_tmp4E7;switch(*((int*)_tmp4E8)){case 2U: _LLA3: _LLA4:
 goto _LLA6;case 3U: _LLA5: _LLA6:
 continue;default: _LLA7: _LLA8:
# 2878
({void*_tmpB64=({void*_tmpB63=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmpB63,_tmp673,0U);})->r;e->r=_tmpB64;});
goto _LLA2;}_LLA2:;}
# 2881
break;}}
# 2884
goto _LL5;case 14U: _LL26: _tmp677=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp676=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp675=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp674=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL27: {
# 2886
void*old_t2=(void*)_check_null(_tmp676->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp677;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp677=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp676);
# 2893
{struct _tuple29 _tmp4E9=({struct _tuple29 _tmp524;({void*_tmpB65=Cyc_Tcutil_compress(old_t2);_tmp524.f1=_tmpB65;});({void*_tmpB66=Cyc_Tcutil_compress(new_typ);_tmp524.f2=_tmpB66;});_tmp524;});struct _tuple29 _tmp4EA=_tmp4E9;struct Cyc_Absyn_PtrInfo _tmp523;struct Cyc_Absyn_PtrInfo _tmp522;struct Cyc_Absyn_PtrInfo _tmp521;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EA.f1)->tag == 5U)switch(*((int*)_tmp4EA.f2)){case 5U: _LLAA: _tmp522=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EA.f1)->f1;_tmp521=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EA.f2)->f1;_LLAB: {
# 2895
int _tmp4EB=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp522.ptr_atts).nullable);
int _tmp4EC=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp521.ptr_atts).nullable);
void*_tmp4ED=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp522.ptr_atts).bounds);
void*_tmp4EE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp521.ptr_atts).bounds);
int _tmp4EF=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp522.ptr_atts).zero_term);
int _tmp4F0=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp521.ptr_atts).zero_term);
{struct _tuple29 _tmp4F1=({struct _tuple29 _tmp51E;_tmp51E.f1=_tmp4ED;_tmp51E.f2=_tmp4EE;_tmp51E;});struct _tuple29 _tmp4F2=_tmp4F1;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F2.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F2.f2)->tag == 1U){_LLB1: _tmp51B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F2.f1)->f1;_tmp51A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F2.f2)->f1;_LLB2:
# 2903
 if((!Cyc_Evexp_c_can_eval(_tmp51B) || !Cyc_Evexp_c_can_eval(_tmp51A)) && !
Cyc_Evexp_same_const_exp(_tmp51B,_tmp51A))
({void*_tmp4F3=0U;({unsigned int _tmpB68=e->loc;struct _dyneither_ptr _tmpB67=({const char*_tmp4F4="can't validate cast due to potential size differences";_tag_dyneither(_tmp4F4,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpB68,_tmpB67,_tag_dyneither(_tmp4F3,sizeof(void*),0U));});});
if(_tmp4EB  && !_tmp4EC){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp4F5=0U;({struct _dyneither_ptr _tmpB69=({const char*_tmp4F6="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp4F6,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpB69,_tag_dyneither(_tmp4F5,sizeof(void*),0U));});});
# 2912
if(_tmp674 != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0U;({struct _dyneither_ptr _tmpB6A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp4F9.f1=_tmpB6A;});({void*_tmp4F7[1U]={& _tmp4F9};({struct _dyneither_ptr _tmpB6B=({const char*_tmp4F8="null-check conversion mis-classified: %s";_tag_dyneither(_tmp4F8,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB6B,_tag_dyneither(_tmp4F7,sizeof(void*),1U));});});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp676);
if(do_null_check){
if(!_tmp675)
({void*_tmp4FA=0U;({unsigned int _tmpB6D=e->loc;struct _dyneither_ptr _tmpB6C=({const char*_tmp4FB="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp4FB,sizeof(char),58U);});Cyc_Tcutil_warn(_tmpB6D,_tmpB6C,_tag_dyneither(_tmp4FA,sizeof(void*),0U));});});
# 2919
({void*_tmpB70=({void*_tmpB6F=*_tmp677;Cyc_Toc_cast_it_r(_tmpB6F,({struct Cyc_Absyn_Exp*_tmpB6E=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmpB6E,({struct Cyc_List_List*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC->hd=_tmp676;_tmp4FC->tl=0;_tmp4FC;}),0U);}));});e->r=_tmpB70;});}else{
# 2923
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp676->r;}};}else{
# 2928
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp676->r;}
# 2935
goto _LLB0;}else{_LLB3: _tmp51C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F2.f1)->f1;_LLB4:
# 2937
 if(!Cyc_Evexp_c_can_eval(_tmp51C))
({void*_tmp4FD=0U;({unsigned int _tmpB72=e->loc;struct _dyneither_ptr _tmpB71=({const char*_tmp4FE="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp4FE,sizeof(char),71U);});Cyc_Tcutil_terr(_tmpB72,_tmpB71,_tag_dyneither(_tmp4FD,sizeof(void*),0U));});});
# 2940
if(_tmp674 == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0U;({struct _dyneither_ptr _tmpB73=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp501.f1=_tmpB73;});({void*_tmp4FF[1U]={& _tmp501};({struct _dyneither_ptr _tmpB74=({const char*_tmp500="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp500,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB74,_tag_dyneither(_tmp4FF,sizeof(void*),1U));});});});
if(Cyc_Toc_is_toplevel(nv)){
# 2944
if((_tmp4EF  && !(_tmp521.elt_tq).real_const) && !_tmp4F0)
# 2947
({struct Cyc_Absyn_Exp*_tmpB76=({struct Cyc_Absyn_Exp*_tmpB75=_tmp51C;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB75,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp51C=_tmpB76;});
# 2949
({void*_tmpB77=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp51C,_tmp676))->r;e->r=_tmpB77;});}else{
# 2951
struct Cyc_Absyn_Exp*_tmp502=Cyc_Toc__tag_dyneither_e;
# 2953
if(_tmp4EF){
# 2958
struct _tuple1*_tmp503=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp504=Cyc_Absyn_var_exp(_tmp503,0U);
struct Cyc_Absyn_Exp*arg3;
# 2963
{void*_tmp505=_tmp676->r;void*_tmp506=_tmp505;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp506)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp506)->f1).Wstring_c).tag){case 8U: _LLBA: _LLBB:
# 2965
 arg3=_tmp51C;goto _LLB9;case 9U: _LLBC: _LLBD:
# 2967
 arg3=_tmp51C;goto _LLB9;default: goto _LLBE;}else{_LLBE: _LLBF:
# 2969
({struct Cyc_Absyn_Exp*_tmpB7B=({struct Cyc_Absyn_Exp*_tmpB7A=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp676);Cyc_Absyn_fncall_exp(_tmpB7A,({struct Cyc_Absyn_Exp*_tmp507[2U];_tmp507[1U]=_tmp51C;({struct Cyc_Absyn_Exp*_tmpB79=({
# 2971
void*_tmpB78=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmpB78,_tmp504);});_tmp507[0U]=_tmpB79;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp507,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2969
arg3=_tmpB7B;});
# 2972
goto _LLB9;}_LLB9:;}
# 2974
if(!_tmp4F0  && !(_tmp521.elt_tq).real_const)
# 2977
({struct Cyc_Absyn_Exp*_tmpB7D=({struct Cyc_Absyn_Exp*_tmpB7C=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB7C,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmpB7D;});{
# 2979
struct Cyc_Absyn_Exp*_tmp508=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp521.elt_typ),0U);
struct Cyc_Absyn_Exp*_tmp509=({struct Cyc_Absyn_Exp*_tmpB7E=_tmp502;Cyc_Absyn_fncall_exp(_tmpB7E,({struct Cyc_Absyn_Exp*_tmp50B[3U];_tmp50B[2U]=arg3;_tmp50B[1U]=_tmp508;_tmp50B[0U]=_tmp504;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp50B,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp50A=Cyc_Absyn_exp_stmt(_tmp509,0U);
({struct Cyc_Absyn_Stmt*_tmpB82=({struct _tuple1*_tmpB81=_tmp503;void*_tmpB80=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmpB7F=_tmp676;Cyc_Absyn_declare_stmt(_tmpB81,_tmpB80,_tmpB7F,_tmp50A,0U);});_tmp50A=_tmpB82;});
({void*_tmpB83=Cyc_Toc_stmt_exp_r(_tmp50A);e->r=_tmpB83;});};}else{
# 2986
({void*_tmpB86=({struct Cyc_Absyn_Exp*_tmpB85=_tmp502;Cyc_Toc_fncall_exp_r(_tmpB85,({struct Cyc_Absyn_Exp*_tmp50C[3U];_tmp50C[2U]=_tmp51C;({struct Cyc_Absyn_Exp*_tmpB84=
# 2988
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp521.elt_typ),0U);_tmp50C[1U]=_tmpB84;});_tmp50C[0U]=_tmp676;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp50C,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2986
e->r=_tmpB86;});}}
# 2992
goto _LLB0;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F2.f2)->tag == 1U){_LLB5: _tmp51D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F2.f2)->f1;_LLB6:
# 2994
 if(!Cyc_Evexp_c_can_eval(_tmp51D))
({void*_tmp50D=0U;({unsigned int _tmpB88=e->loc;struct _dyneither_ptr _tmpB87=({const char*_tmp50E="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp50E,sizeof(char),71U);});Cyc_Tcutil_terr(_tmpB88,_tmpB87,_tag_dyneither(_tmp50D,sizeof(void*),0U));});});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp50F=0U;({struct _dyneither_ptr _tmpB89=({const char*_tmp510="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp510,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpB89,_tag_dyneither(_tmp50F,sizeof(void*),0U));});});{
# 3006 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp511=_tmp51D;
if(_tmp4EF  && !_tmp4F0)
({struct Cyc_Absyn_Exp*_tmpB8B=({struct Cyc_Absyn_Exp*_tmpB8A=_tmp51D;Cyc_Absyn_add_exp(_tmpB8A,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp511=_tmpB8B;});{
# 3013
struct Cyc_Absyn_Exp*_tmp512=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp513=({struct Cyc_Absyn_Exp*_tmpB8D=_tmp512;Cyc_Absyn_fncall_exp(_tmpB8D,({struct Cyc_Absyn_Exp*_tmp515[3U];_tmp515[2U]=_tmp511;({struct Cyc_Absyn_Exp*_tmpB8C=
# 3016
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp522.elt_typ),0U);_tmp515[1U]=_tmpB8C;});_tmp515[0U]=_tmp676;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp515,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3018
if(!_tmp4EC)
({void*_tmpB90=({struct Cyc_Absyn_Exp*_tmpB8F=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmpB8F,({struct Cyc_List_List*_tmp514=_cycalloc(sizeof(*_tmp514));({struct Cyc_Absyn_Exp*_tmpB8E=
Cyc_Absyn_copy_exp(_tmp513);_tmp514->hd=_tmpB8E;});_tmp514->tl=0;_tmp514;}));});
# 3019
_tmp513->r=_tmpB90;});
# 3021
({void*_tmpB91=Cyc_Toc_cast_it_r(*_tmp677,_tmp513);e->r=_tmpB91;});
goto _LLB0;};};}else{_LLB7: _LLB8:
# 3026
 DynCast:
 if((_tmp4EF  && !_tmp4F0) && !(_tmp521.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp516=0U;({struct _dyneither_ptr _tmpB92=({const char*_tmp517="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp517,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpB92,_tag_dyneither(_tmp516,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp518=Cyc_Toc__dyneither_ptr_decrease_size_e;
({void*_tmpB96=({struct Cyc_Absyn_Exp*_tmpB95=_tmp518;Cyc_Toc_fncall_exp_r(_tmpB95,({struct Cyc_Absyn_Exp*_tmp519[3U];({struct Cyc_Absyn_Exp*_tmpB93=
# 3033
Cyc_Absyn_uint_exp(1U,0U);_tmp519[2U]=_tmpB93;});({struct Cyc_Absyn_Exp*_tmpB94=
# 3032
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp522.elt_typ),0U);_tmp519[1U]=_tmpB94;});_tmp519[0U]=_tmp676;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp519,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 3031
e->r=_tmpB96;});};}
# 3035
goto _LLB0;}}_LLB0:;}
# 3037
goto _LLA9;}case 6U: _LLAC: _tmp523=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EA.f1)->f1;_LLAD:
# 3039
{void*_tmp51F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp523.ptr_atts).bounds);void*_tmp520=_tmp51F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp520)->tag == 0U){_LLC1: _LLC2:
# 3041
({void*_tmpB98=({struct Cyc_Absyn_Exp*_tmpB97=Cyc_Absyn_new_exp(_tmp676->r,_tmp676->loc);Cyc_Toc_aggrmember_exp_r(_tmpB97,Cyc_Toc_curr_sp);});_tmp676->r=_tmpB98;});
_tmp676->topt=new_typ_c;
goto _LLC0;}else{_LLC3: _LLC4:
 goto _LLC0;}_LLC0:;}
# 3046
goto _LLA9;default: goto _LLAE;}else{_LLAE: _LLAF:
# 3048
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp676->r;
goto _LLA9;}_LLA9:;}
# 3052
goto _LL5;}case 15U: _LL28: _tmp678=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL29:
# 3056
{void*_tmp525=_tmp678->r;void*_tmp526=_tmp525;struct Cyc_List_List*_tmp532;struct _tuple1*_tmp531;struct Cyc_List_List*_tmp530;struct Cyc_List_List*_tmp52F;switch(*((int*)_tmp526)){case 29U: _LLC6: _tmp531=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp526)->f1;_tmp530=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp526)->f2;_tmp52F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp526)->f3;_LLC7:
# 3058
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0U;({struct _dyneither_ptr _tmpB99=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp678->loc));_tmp529.f1=_tmpB99;});({void*_tmp527[1U]={& _tmp529};({struct _dyneither_ptr _tmpB9A=({const char*_tmp528="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp528,sizeof(char),42U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpB9A,_tag_dyneither(_tmp527,sizeof(void*),1U));});});});{
struct Cyc_Absyn_Exp*_tmp52A=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp678->topt),_tmp530,1,0,_tmp52F,_tmp531);
e->r=_tmp52A->r;
e->topt=_tmp52A->topt;
goto _LLC5;};case 24U: _LLC8: _tmp532=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp526)->f1;_LLC9:
# 3066
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp52D;_tmp52D.tag=0U;({struct _dyneither_ptr _tmpB9B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp678->loc));_tmp52D.f1=_tmpB9B;});({void*_tmp52B[1U]={& _tmp52D};({struct _dyneither_ptr _tmpB9C=({const char*_tmp52C="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp52C,sizeof(char),42U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpB9C,_tag_dyneither(_tmp52B,sizeof(void*),1U));});});});{
struct Cyc_Absyn_Exp*_tmp52E=Cyc_Toc_init_tuple(nv,1,0,_tmp532);
e->r=_tmp52E->r;
e->topt=_tmp52E->topt;
goto _LLC5;};default: _LLCA: _LLCB:
# 3074
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp678);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp678)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp678,0,Cyc_Toc_address_lvalue,1);
# 3080
({void*_tmpB9E=({void*_tmpB9D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmpB9D,_tmp678);});e->r=_tmpB9E;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp678->topt))))
# 3084
({void*_tmpBA0=({void*_tmpB9F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmpB9F,_tmp678);});e->r=_tmpBA0;});}
# 3086
goto _LLC5;}_LLC5:;}
# 3088
goto _LL5;case 16U: _LL2A: _tmp67A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp679=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL2B:
# 3091
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0U;({struct _dyneither_ptr _tmpBA1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp679->loc));_tmp535.f1=_tmpBA1;});({void*_tmp533[1U]={& _tmp535};({struct _dyneither_ptr _tmpBA2=({const char*_tmp534="%s: new at top-level";_tag_dyneither(_tmp534,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpBA2,_tag_dyneither(_tmp533,sizeof(void*),1U));});});});{
void*new_e_type=(void*)_check_null(_tmp679->topt);
{void*_tmp536=_tmp679->r;void*_tmp537=_tmp536;struct Cyc_List_List*_tmp567;struct _tuple1*_tmp566;struct Cyc_List_List*_tmp565;struct Cyc_List_List*_tmp564;struct Cyc_Absyn_Aggrdecl*_tmp563;struct Cyc_Absyn_Exp*_tmp562;void*_tmp561;int _tmp560;struct Cyc_Absyn_Vardecl*_tmp55F;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp55D;int _tmp55C;struct Cyc_List_List*_tmp55B;switch(*((int*)_tmp537)){case 26U: _LLCD: _tmp55B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp537)->f1;_LLCE: {
# 3099
struct _tuple1*_tmp538=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp539=Cyc_Absyn_var_exp(_tmp538,0U);
struct Cyc_Absyn_Stmt*_tmp53A=({struct Cyc_Toc_Env*_tmpBA6=nv;void*_tmpBA5=new_e_type;struct Cyc_Absyn_Exp*_tmpBA4=_tmp539;struct Cyc_List_List*_tmpBA3=_tmp55B;Cyc_Toc_init_array(_tmpBA6,_tmpBA5,_tmpBA4,_tmpBA3,Cyc_Absyn_exp_stmt(_tmp539,0U));});
void*old_elt_typ;
{void*_tmp53B=Cyc_Tcutil_compress(old_typ);void*_tmp53C=_tmp53B;void*_tmp541;struct Cyc_Absyn_Tqual _tmp540;union Cyc_Absyn_Constraint*_tmp53F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53C)->tag == 5U){_LLDA: _tmp541=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53C)->f1).elt_typ;_tmp540=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53C)->f1).elt_tq;_tmp53F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53C)->f1).ptr_atts).zero_term;_LLDB:
# 3105
 old_elt_typ=_tmp541;goto _LLD9;}else{_LLDC: _LLDD:
# 3107
({void*_tmpBA8=({void*_tmp53D=0U;({struct _dyneither_ptr _tmpBA7=({const char*_tmp53E="exp_to_c:new array expression doesn't have ptr type";_tag_dyneither(_tmp53E,sizeof(char),52U);});((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpBA7,_tag_dyneither(_tmp53D,sizeof(void*),0U));});});old_elt_typ=_tmpBA8;});}_LLD9:;}{
# 3110
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp542=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp543=({struct Cyc_Absyn_Exp*_tmpBA9=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);Cyc_Absyn_times_exp(_tmpBA9,
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp55B),0U),0U);});
struct Cyc_Absyn_Exp*e1;
if(_tmp67A == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_Toc_malloc_exp(old_elt_typ,_tmp543);e1=_tmpBAA;});else{
# 3118
struct Cyc_Absyn_Exp*r=_tmp67A;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmpBAB=Cyc_Toc_rmalloc_exp(r,_tmp543);e1=_tmpBAB;});}
# 3123
({void*_tmpBAC=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp538,_tmp542,e1,_tmp53A,0U));e->r=_tmpBAC;});
goto _LLCC;};}case 27U: _LLCF: _tmp55F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp537)->f1;_tmp55E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp537)->f2;_tmp55D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp537)->f3;_tmp55C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp537)->f4;_LLD0:
# 3127
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp67A,old_typ,_tmp55E,(void*)_check_null(_tmp55D->topt),_tmp55C,_tmp55D,_tmp55F);
goto _LLCC;case 28U: _LLD1: _tmp562=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp537)->f1;_tmp561=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp537)->f2;_tmp560=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp537)->f3;_LLD2:
# 3131
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp67A,old_typ,_tmp562,_tmp561,_tmp560,0,0);
goto _LLCC;case 29U: _LLD3: _tmp566=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp537)->f1;_tmp565=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp537)->f2;_tmp564=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp537)->f3;_tmp563=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp537)->f4;_LLD4: {
# 3136
struct Cyc_Absyn_Exp*_tmp544=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp679->topt),_tmp565,1,_tmp67A,_tmp564,_tmp566);
e->r=_tmp544->r;
e->topt=_tmp544->topt;
goto _LLCC;}case 24U: _LLD5: _tmp567=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp537)->f1;_LLD6: {
# 3142
struct Cyc_Absyn_Exp*_tmp545=Cyc_Toc_init_tuple(nv,1,_tmp67A,_tmp567);
e->r=_tmp545->r;
e->topt=_tmp545->topt;
goto _LLCC;}default: _LLD7: _LLD8: {
# 3151
void*old_elt_typ=(void*)_check_null(_tmp679->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3154
struct _tuple1*_tmp546=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp547=Cyc_Absyn_var_exp(_tmp546,0U);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp547,0U),0U);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp67A == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmpBAD=Cyc_Toc_malloc_exp(old_elt_typ,mexp);mexp=_tmpBAD;});else{
# 3161
struct Cyc_Absyn_Exp*r=_tmp67A;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmpBAE=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmpBAE;});}{
# 3169
int done=0;
{void*_tmp548=_tmp679->r;void*_tmp549=_tmp548;void*_tmp557;struct Cyc_Absyn_Exp*_tmp556;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp549)->tag == 14U){_LLDF: _tmp557=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp549)->f1;_tmp556=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp549)->f2;_LLE0:
# 3172
{struct _tuple29 _tmp54A=({struct _tuple29 _tmp555;({void*_tmpBAF=Cyc_Tcutil_compress(_tmp557);_tmp555.f1=_tmpBAF;});({void*_tmpBB0=Cyc_Tcutil_compress((void*)_check_null(_tmp556->topt));_tmp555.f2=_tmpBB0;});_tmp555;});struct _tuple29 _tmp54B=_tmp54A;void*_tmp554;union Cyc_Absyn_Constraint*_tmp553;union Cyc_Absyn_Constraint*_tmp552;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54B.f1)->tag == 5U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54B.f2)->tag == 5U){_LLE4: _tmp554=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54B.f1)->f1).elt_typ;_tmp553=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54B.f1)->f1).ptr_atts).bounds;_tmp552=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54B.f2)->f1).ptr_atts).bounds;_LLE5:
# 3175
{struct _tuple29 _tmp54C=({struct _tuple29 _tmp551;({void*_tmpBB1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp553);_tmp551.f1=_tmpBB1;});({void*_tmpBB2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp552);_tmp551.f2=_tmpBB2;});_tmp551;});struct _tuple29 _tmp54D=_tmp54C;struct Cyc_Absyn_Exp*_tmp550;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp54D.f1)->tag == 0U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp54D.f2)->tag == 1U){_LLE9: _tmp550=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp54D.f2)->f1;_LLEA:
# 3177
 Cyc_Toc_exp_to_c(nv,_tmp556);
({void*_tmpBB3=Cyc_Toc_sizeoftyp_exp_r(elt_typ);inner_mexp->r=_tmpBB3;});
done=1;{
struct Cyc_Absyn_Exp*_tmp54E=Cyc_Toc__init_dyneither_ptr_e;
({void*_tmpBB6=({struct Cyc_Absyn_Exp*_tmpBB5=_tmp54E;Cyc_Toc_fncall_exp_r(_tmpBB5,({struct Cyc_Absyn_Exp*_tmp54F[4U];_tmp54F[3U]=_tmp550;({struct Cyc_Absyn_Exp*_tmpBB4=
# 3183
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp554),0U);_tmp54F[2U]=_tmpBB4;});_tmp54F[1U]=_tmp556;_tmp54F[0U]=mexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp54F,sizeof(struct Cyc_Absyn_Exp*),4U));}));});
# 3181
e->r=_tmpBB6;});
# 3185
goto _LLE8;};}else{goto _LLEB;}}else{_LLEB: _LLEC:
 goto _LLE8;}_LLE8:;}
# 3188
goto _LLE3;}else{goto _LLE6;}}else{_LLE6: _LLE7:
 goto _LLE3;}_LLE3:;}
# 3191
goto _LLDE;}else{_LLE1: _LLE2:
 goto _LLDE;}_LLDE:;}
# 3194
if(!done){
struct Cyc_Absyn_Stmt*_tmp558=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp547),0U);
struct Cyc_Absyn_Exp*_tmp559=Cyc_Absyn_signed_int_exp(0,0U);
Cyc_Toc_exp_to_c(nv,_tmp679);
({struct Cyc_Absyn_Stmt*_tmpBB9=({struct Cyc_Absyn_Stmt*_tmpBB8=({struct Cyc_Absyn_Exp*_tmpBB7=Cyc_Absyn_subscript_exp(_tmp547,_tmp559,0U);Cyc_Absyn_assign_stmt(_tmpBB7,_tmp679,0U);});Cyc_Absyn_seq_stmt(_tmpBB8,_tmp558,0U);});_tmp558=_tmpBB9;});{
# 3200
void*_tmp55A=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
({void*_tmpBBA=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp546,_tmp55A,mexp,_tmp558,0U));e->r=_tmpBBA;});};}
# 3203
goto _LLCC;};}}_LLCC:;}
# 3205
goto _LL5;};case 18U: _LL2C: _tmp67B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2D: {
# 3208
int _tmp568=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp67B);
Cyc_Toc_set_in_sizeof(nv,_tmp568);
goto _LL5;}case 17U: _LL2E: _tmp67C=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2F:
({void*_tmpBBD=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp569=_cycalloc(sizeof(*_tmp569));({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpBBC=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp56A;_tmp56A.tag=17U;({void*_tmpBBB=Cyc_Toc_typ_to_c(_tmp67C);_tmp56A.f1=_tmpBBB;});_tmp56A;});_tmp569[0]=_tmpBBC;});_tmp569;});e->r=_tmpBBD;});goto _LL5;case 19U: _LL30: _tmp67E=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp67D=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL31: {
# 3214
void*_tmp56B=_tmp67E;
struct Cyc_List_List*_tmp56C=_tmp67D;
for(0;_tmp56C != 0;_tmp56C=_tmp56C->tl){
void*_tmp56D=(void*)_tmp56C->hd;void*_tmp56E=_tmp56D;unsigned int _tmp584;struct _dyneither_ptr*_tmp583;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp56E)->tag == 0U){_LLEE: _tmp583=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp56E)->f1;_LLEF:
 goto _LLED;}else{_LLF0: _tmp584=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp56E)->f1;_LLF1:
# 3220
{void*_tmp56F=Cyc_Tcutil_compress(_tmp56B);void*_tmp570=_tmp56F;struct Cyc_Absyn_Datatypefield*_tmp582;struct Cyc_List_List*_tmp581;struct Cyc_List_List*_tmp580;union Cyc_Absyn_AggrInfoU _tmp57F;switch(*((int*)_tmp570)){case 11U: _LLF3: _tmp57F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp570)->f1).aggr_info;_LLF4: {
# 3222
struct Cyc_Absyn_Aggrdecl*_tmp571=Cyc_Absyn_get_known_aggrdecl(_tmp57F);
if(_tmp571->impl == 0)
({void*_tmp572=0U;({struct _dyneither_ptr _tmpBBE=({const char*_tmp573="struct fields must be known";_tag_dyneither(_tmp573,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpBBE,_tag_dyneither(_tmp572,sizeof(void*),0U));});});
_tmp580=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp571->impl))->fields;goto _LLF6;}case 12U: _LLF5: _tmp580=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp570)->f2;_LLF6: {
# 3227
struct Cyc_Absyn_Aggrfield*_tmp574=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp580,(int)_tmp584);
({void*_tmpBC0=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp575=_cycalloc(sizeof(*_tmp575));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBBF=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp576;_tmp576.tag=0U;_tmp576.f1=_tmp574->name;_tmp576;});_tmp575[0]=_tmpBBF;});_tmp575;}));_tmp56C->hd=_tmpBC0;});
_tmp56B=_tmp574->type;
goto _LLF2;}case 10U: _LLF7: _tmp581=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp570)->f1;_LLF8:
# 3232
({void*_tmpBC3=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp577=_cycalloc(sizeof(*_tmp577));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBC2=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp578;_tmp578.tag=0U;({struct _dyneither_ptr*_tmpBC1=Cyc_Absyn_fieldname((int)(_tmp584 + 1));_tmp578.f1=_tmpBC1;});_tmp578;});_tmp577[0]=_tmpBC2;});_tmp577;}));_tmp56C->hd=_tmpBC3;});
({void*_tmpBC4=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp581,(int)_tmp584)).f2;_tmp56B=_tmpBC4;});
goto _LLF2;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp570)->f1).field_info).KnownDatatypefield).tag == 2){_LLF9: _tmp582=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp570)->f1).field_info).KnownDatatypefield).val).f2;_LLFA:
# 3236
 if(_tmp584 == 0)
({void*_tmpBC6=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp579=_cycalloc(sizeof(*_tmp579));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBC5=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp57A;_tmp57A.tag=0U;_tmp57A.f1=Cyc_Toc_tag_sp;_tmp57A;});_tmp579[0]=_tmpBC5;});_tmp579;}));_tmp56C->hd=_tmpBC6;});else{
# 3239
({void*_tmpBC7=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp582->typs,(int)(_tmp584 - 1))).f2;_tmp56B=_tmpBC7;});
({void*_tmpBCA=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBC9=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp57C;_tmp57C.tag=0U;({struct _dyneither_ptr*_tmpBC8=Cyc_Absyn_fieldname((int)_tmp584);_tmp57C.f1=_tmpBC8;});_tmp57C;});_tmp57B[0]=_tmpBC9;});_tmp57B;}));_tmp56C->hd=_tmpBCA;});}
# 3242
goto _LLF2;}else{goto _LLFB;}default: _LLFB: _LLFC:
({void*_tmp57D=0U;({struct _dyneither_ptr _tmpBCB=({const char*_tmp57E="impossible type for offsetof tuple index";_tag_dyneither(_tmp57E,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpBCB,_tag_dyneither(_tmp57D,sizeof(void*),0U));});});
goto _LLF2;}_LLF2:;}
# 3246
goto _LLED;}_LLED:;}
# 3249
({void*_tmpBCE=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp585=_cycalloc(sizeof(*_tmp585));({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBCD=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp586;_tmp586.tag=19U;({void*_tmpBCC=Cyc_Toc_typ_to_c(_tmp67E);_tmp586.f1=_tmpBCC;});_tmp586.f2=_tmp67D;_tmp586;});_tmp585[0]=_tmpBCD;});_tmp585;});e->r=_tmpBCE;});
goto _LL5;}case 20U: _LL32: _tmp67F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL33: {
# 3252
int _tmp587=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp588=Cyc_Tcutil_compress((void*)_check_null(_tmp67F->topt));
{void*_tmp589=_tmp588;void*_tmp59E;struct Cyc_Absyn_Tqual _tmp59D;void*_tmp59C;union Cyc_Absyn_Constraint*_tmp59B;union Cyc_Absyn_Constraint*_tmp59A;union Cyc_Absyn_Constraint*_tmp599;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->tag == 5U){_LLFE: _tmp59E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).elt_typ;_tmp59D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).elt_tq;_tmp59C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).ptr_atts).rgn;_tmp59B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).ptr_atts).nullable;_tmp59A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).ptr_atts).bounds;_tmp599=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).ptr_atts).zero_term;_LLFF:
# 3257
{void*_tmp58A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp59A);void*_tmp58B=_tmp58A;struct Cyc_Absyn_Exp*_tmp596;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58B)->tag == 1U){_LL103: _tmp596=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58B)->f1;_LL104: {
# 3259
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp67F);
Cyc_Toc_exp_to_c(nv,_tmp67F);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp58C=0U;({unsigned int _tmpBD0=e->loc;struct _dyneither_ptr _tmpBCF=({const char*_tmp58D="inserted null check due to dereference";_tag_dyneither(_tmp58D,sizeof(char),39U);});Cyc_Tcutil_warn(_tmpBD0,_tmpBCF,_tag_dyneither(_tmp58C,sizeof(void*),0U));});});
# 3265
({void*_tmpBD4=({void*_tmpBD3=Cyc_Toc_typ_to_c(_tmp588);Cyc_Toc_cast_it_r(_tmpBD3,({
struct Cyc_Absyn_Exp*_tmpBD2=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmpBD2,({struct Cyc_List_List*_tmp58E=_cycalloc(sizeof(*_tmp58E));({struct Cyc_Absyn_Exp*_tmpBD1=
Cyc_Absyn_copy_exp(_tmp67F);_tmp58E->hd=_tmpBD1;});_tmp58E->tl=0;_tmp58E;}),0U);}));});
# 3265
_tmp67F->r=_tmpBD4;});}
# 3273
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp599)){
struct _tuple12 _tmp58F=Cyc_Evexp_eval_const_uint_exp(_tmp596);struct _tuple12 _tmp590=_tmp58F;unsigned int _tmp594;int _tmp593;_LL108: _tmp594=_tmp590.f1;_tmp593=_tmp590.f2;_LL109:;
# 3279
if(!_tmp593  || _tmp594 <= 0)
({void*_tmp591=0U;({unsigned int _tmpBD6=e->loc;struct _dyneither_ptr _tmpBD5=({const char*_tmp592="cannot determine dereference is in bounds";_tag_dyneither(_tmp592,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpBD6,_tmpBD5,_tag_dyneither(_tmp591,sizeof(void*),0U));});});}
# 3282
goto _LL102;}}else{_LL105: _LL106: {
# 3285
struct Cyc_Absyn_Exp*_tmp595=Cyc_Absyn_uint_exp(0U,0U);
_tmp595->topt=Cyc_Absyn_uint_typ;
({void*_tmpBD7=Cyc_Toc_subscript_exp_r(_tmp67F,_tmp595);e->r=_tmpBD7;});
Cyc_Toc_exp_to_c(nv,e);
goto _LL102;}}_LL102:;}
# 3291
goto _LLFD;}else{_LL100: _LL101:
({void*_tmp597=0U;({struct _dyneither_ptr _tmpBD8=({const char*_tmp598="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp598,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpBD8,_tag_dyneither(_tmp597,sizeof(void*),0U));});});}_LLFD:;}
# 3294
Cyc_Toc_set_lhs(nv,_tmp587);
goto _LL5;};}case 21U: _LL34: _tmp683=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp682=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp681=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp680=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL35: {
# 3297
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp683->topt);
Cyc_Toc_exp_to_c(nv,_tmp683);
if(_tmp681  && _tmp680)
({void*_tmpBDD=({struct Cyc_Absyn_Exp*_tmpBDC=_tmp683;void*_tmpBDB=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmpBDA=e1_cyc_type;struct _dyneither_ptr*_tmpBD9=_tmp682;Cyc_Toc_check_tagged_union(_tmpBDC,_tmpBDB,_tmpBDA,_tmpBD9,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 3301
e->r=_tmpBDD;});
# 3305
if(is_poly)
({void*_tmpBDF=({void*_tmpBDE=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmpBDE,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmpBDF;});
goto _LL5;}case 22U: _LL36: _tmp687=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp686=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp685=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp684=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL37: {
# 3309
int _tmp59F=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp687->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp687);
Cyc_Toc_exp_to_c(nv,_tmp687);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp5A0=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp5A1=_tmp5A0;void*_tmp5B7;struct Cyc_Absyn_Tqual _tmp5B6;void*_tmp5B5;union Cyc_Absyn_Constraint*_tmp5B4;union Cyc_Absyn_Constraint*_tmp5B3;union Cyc_Absyn_Constraint*_tmp5B2;_LL10B: _tmp5B7=_tmp5A1.elt_typ;_tmp5B6=_tmp5A1.elt_tq;_tmp5B5=(_tmp5A1.ptr_atts).rgn;_tmp5B4=(_tmp5A1.ptr_atts).nullable;_tmp5B3=(_tmp5A1.ptr_atts).bounds;_tmp5B2=(_tmp5A1.ptr_atts).zero_term;_LL10C:;
{void*_tmp5A2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5B3);void*_tmp5A3=_tmp5A2;struct Cyc_Absyn_Exp*_tmp5B1;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5A3)->tag == 1U){_LL10E: _tmp5B1=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5A3)->f1;_LL10F: {
# 3320
struct _tuple12 _tmp5A4=Cyc_Evexp_eval_const_uint_exp(_tmp5B1);struct _tuple12 _tmp5A5=_tmp5A4;unsigned int _tmp5AF;int _tmp5AE;_LL113: _tmp5AF=_tmp5A5.f1;_tmp5AE=_tmp5A5.f2;_LL114:;
if(_tmp5AE){
if(_tmp5AF < 1)
({void*_tmp5A6=0U;({struct _dyneither_ptr _tmpBE0=({const char*_tmp5A7="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp5A7,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBE0,_tag_dyneither(_tmp5A6,sizeof(void*),0U));});});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5A8=0U;({unsigned int _tmpBE2=e->loc;struct _dyneither_ptr _tmpBE1=({const char*_tmp5A9="inserted null check due to dereference";_tag_dyneither(_tmp5A9,sizeof(char),39U);});Cyc_Tcutil_warn(_tmpBE2,_tmpBE1,_tag_dyneither(_tmp5A8,sizeof(void*),0U));});});
# 3328
({void*_tmpBE6=({void*_tmpBE5=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmpBE5,({
struct Cyc_Absyn_Exp*_tmpBE4=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmpBE4,({struct Cyc_Absyn_Exp*_tmp5AA[1U];({struct Cyc_Absyn_Exp*_tmpBE3=
Cyc_Absyn_new_exp(_tmp687->r,0U);_tmp5AA[0U]=_tmpBE3;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5AA,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 3328
_tmp687->r=_tmpBE6;});}}else{
# 3333
if(!Cyc_Evexp_c_can_eval(_tmp5B1))
({void*_tmp5AB=0U;({unsigned int _tmpBE8=e->loc;struct _dyneither_ptr _tmpBE7=({const char*_tmp5AC="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp5AC,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpBE8,_tmpBE7,_tag_dyneither(_tmp5AB,sizeof(void*),0U));});});
# 3337
({void*_tmpBEE=({void*_tmpBED=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmpBED,({
struct Cyc_Absyn_Exp*_tmpBEC=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmpBEC,({struct Cyc_Absyn_Exp*_tmp5AD[4U];({struct Cyc_Absyn_Exp*_tmpBE9=
# 3341
Cyc_Absyn_uint_exp(0U,0U);_tmp5AD[3U]=_tmpBE9;});({struct Cyc_Absyn_Exp*_tmpBEA=
# 3340
Cyc_Absyn_sizeoftyp_exp(_tmp5B7,0U);_tmp5AD[2U]=_tmpBEA;});_tmp5AD[1U]=_tmp5B1;({struct Cyc_Absyn_Exp*_tmpBEB=
# 3339
Cyc_Absyn_new_exp(_tmp687->r,0U);_tmp5AD[0U]=_tmpBEB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5AD,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 3337
_tmp687->r=_tmpBEE;});}
# 3343
goto _LL10D;}}else{_LL110: _LL111: {
# 3346
void*ta1=Cyc_Toc_typ_to_c(_tmp5B7);
({void*_tmpBF4=({void*_tmpBF3=Cyc_Absyn_cstar_typ(ta1,_tmp5B6);Cyc_Toc_cast_it_r(_tmpBF3,({
struct Cyc_Absyn_Exp*_tmpBF2=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmpBF2,({struct Cyc_Absyn_Exp*_tmp5B0[3U];({struct Cyc_Absyn_Exp*_tmpBEF=
# 3351
Cyc_Absyn_uint_exp(0U,0U);_tmp5B0[2U]=_tmpBEF;});({struct Cyc_Absyn_Exp*_tmpBF0=
# 3350
Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp5B0[1U]=_tmpBF0;});({struct Cyc_Absyn_Exp*_tmpBF1=
# 3349
Cyc_Absyn_new_exp(_tmp687->r,0U);_tmp5B0[0U]=_tmpBF1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5B0,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 3347
_tmp687->r=_tmpBF4;});
# 3354
goto _LL10D;}}_LL10D:;}
# 3356
if(_tmp685  && _tmp684)
({void*_tmpBF9=({struct Cyc_Absyn_Exp*_tmpBF8=_tmp687;void*_tmpBF7=Cyc_Toc_typ_to_c(e1typ);void*_tmpBF6=_tmp5B7;struct _dyneither_ptr*_tmpBF5=_tmp686;Cyc_Toc_check_tagged_union(_tmpBF8,_tmpBF7,_tmpBF6,_tmpBF5,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmpBF9;});
# 3359
if(is_poly  && _tmp684)
({void*_tmpBFB=({void*_tmpBFA=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmpBFA,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmpBFB;});
Cyc_Toc_set_lhs(nv,_tmp59F);
goto _LL5;};};}case 23U: _LL38: _tmp689=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp688=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL39: {
# 3364
int _tmp5B8=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5B9=Cyc_Tcutil_compress((void*)_check_null(_tmp689->topt));
# 3368
{void*_tmp5BA=_tmp5B9;void*_tmp5E4;struct Cyc_Absyn_Tqual _tmp5E3;void*_tmp5E2;union Cyc_Absyn_Constraint*_tmp5E1;union Cyc_Absyn_Constraint*_tmp5E0;union Cyc_Absyn_Constraint*_tmp5DF;struct Cyc_List_List*_tmp5DE;switch(*((int*)_tmp5BA)){case 10U: _LL116: _tmp5DE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5BA)->f1;_LL117:
# 3371
 Cyc_Toc_exp_to_c(nv,_tmp689);
Cyc_Toc_exp_to_c(nv,_tmp688);{
struct _tuple12 _tmp5BB=Cyc_Evexp_eval_const_uint_exp(_tmp688);struct _tuple12 _tmp5BC=_tmp5BB;unsigned int _tmp5C0;int _tmp5BF;_LL11D: _tmp5C0=_tmp5BC.f1;_tmp5BF=_tmp5BC.f2;_LL11E:;
if(!_tmp5BF)
({void*_tmp5BD=0U;({struct _dyneither_ptr _tmpBFC=({const char*_tmp5BE="unknown tuple subscript in translation to C";_tag_dyneither(_tmp5BE,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBFC,_tag_dyneither(_tmp5BD,sizeof(void*),0U));});});
({void*_tmpBFE=({struct Cyc_Absyn_Exp*_tmpBFD=_tmp689;Cyc_Toc_aggrmember_exp_r(_tmpBFD,Cyc_Absyn_fieldname((int)(_tmp5C0 + 1)));});e->r=_tmpBFE;});
goto _LL115;};case 5U: _LL118: _tmp5E4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BA)->f1).elt_typ;_tmp5E3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BA)->f1).elt_tq;_tmp5E2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BA)->f1).ptr_atts).rgn;_tmp5E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BA)->f1).ptr_atts).nullable;_tmp5E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BA)->f1).ptr_atts).bounds;_tmp5DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BA)->f1).ptr_atts).zero_term;_LL119: {
# 3379
struct Cyc_List_List*_tmp5C1=Cyc_Toc_get_relns(_tmp689);
int _tmp5C2=Cyc_Toc_need_null_check(_tmp689);
int _tmp5C3=Cyc_Toc_in_sizeof(nv);
# 3387
int in_bnds=_tmp5C3;
{void*_tmp5C4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E0);void*_tmp5C5=_tmp5C4;_LL120: _LL121:
# 3390
({int _tmpBFF=in_bnds  || Cyc_Toc_check_bounds(_tmp5B9,_tmp5C1,_tmp689,_tmp688);in_bnds=_tmpBFF;});
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp5C8;_tmp5C8.tag=0U;({struct _dyneither_ptr _tmpC00=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5C8.f1=_tmpC00;});({void*_tmp5C6[1U]={& _tmp5C8};({unsigned int _tmpC02=e->loc;struct _dyneither_ptr _tmpC01=({const char*_tmp5C7="bounds check necessary for %s";_tag_dyneither(_tmp5C7,sizeof(char),30U);});Cyc_Tcutil_warn(_tmpC02,_tmpC01,_tag_dyneither(_tmp5C6,sizeof(void*),1U));});});});_LL11F:;}
# 3397
Cyc_Toc_exp_to_c(nv,_tmp689);
Cyc_Toc_exp_to_c(nv,_tmp688);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp5C9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E0);void*_tmp5CA=_tmp5C9;struct Cyc_Absyn_Exp*_tmp5DB;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CA)->tag == 1U){_LL123: _tmp5DB=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CA)->f1;_LL124: {
# 3402
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5E1) && _tmp5C2;
void*ta1=Cyc_Toc_typ_to_c(_tmp5E4);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5E3);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5CB=0U;({unsigned int _tmpC04=e->loc;struct _dyneither_ptr _tmpC03=({const char*_tmp5CC="inserted null check due to dereference";_tag_dyneither(_tmp5CC,sizeof(char),39U);});Cyc_Tcutil_warn(_tmpC04,_tmpC03,_tag_dyneither(_tmp5CB,sizeof(void*),0U));});});
({void*_tmpC08=({void*_tmpC07=ta2;Cyc_Toc_cast_it_r(_tmpC07,({struct Cyc_Absyn_Exp*_tmpC06=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmpC06,({struct Cyc_Absyn_Exp*_tmp5CD[1U];({struct Cyc_Absyn_Exp*_tmpC05=
Cyc_Absyn_copy_exp(_tmp689);_tmp5CD[0U]=_tmpC05;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5CD,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 3411
_tmp689->r=_tmpC08;});}else{
# 3414
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5DF)){
# 3416
if(!Cyc_Evexp_c_can_eval(_tmp5DB))
({void*_tmp5CE=0U;({unsigned int _tmpC0A=e->loc;struct _dyneither_ptr _tmpC09=({const char*_tmp5CF="cannot determine subscript is in bounds";_tag_dyneither(_tmp5CF,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC0A,_tmpC09,_tag_dyneither(_tmp5CE,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp689);
({void*_tmpC0D=Cyc_Toc_deref_exp_r(({void*_tmpC0C=ta2;Cyc_Toc_cast_it(_tmpC0C,({
struct Cyc_Absyn_Exp*_tmpC0B=function_e;Cyc_Absyn_fncall_exp(_tmpC0B,({struct Cyc_Absyn_Exp*_tmp5D0[3U];_tmp5D0[2U]=_tmp688;_tmp5D0[1U]=_tmp5DB;_tmp5D0[0U]=_tmp689;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D0,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 3419
e->r=_tmpC0D;});};}else{
# 3422
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp5DB))
({void*_tmp5D1=0U;({unsigned int _tmpC0F=e->loc;struct _dyneither_ptr _tmpC0E=({const char*_tmp5D2="cannot determine subscript is in bounds";_tag_dyneither(_tmp5D2,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC0F,_tmpC0E,_tag_dyneither(_tmp5D1,sizeof(void*),0U));});});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5D3=0U;({unsigned int _tmpC11=e->loc;struct _dyneither_ptr _tmpC10=({const char*_tmp5D4="inserted null check due to dereference";_tag_dyneither(_tmp5D4,sizeof(char),39U);});Cyc_Tcutil_warn(_tmpC11,_tmpC10,_tag_dyneither(_tmp5D3,sizeof(void*),0U));});});
# 3428
({void*_tmpC15=Cyc_Toc_deref_exp_r(({void*_tmpC14=ta2;Cyc_Toc_cast_it(_tmpC14,({
struct Cyc_Absyn_Exp*_tmpC13=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmpC13,({struct Cyc_Absyn_Exp*_tmp5D5[4U];_tmp5D5[3U]=_tmp688;({struct Cyc_Absyn_Exp*_tmpC12=
# 3431
Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp5D5[2U]=_tmpC12;});_tmp5D5[1U]=_tmp5DB;_tmp5D5[0U]=_tmp689;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D5,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 3428
e->r=_tmpC15;});}else{
# 3434
if(!Cyc_Evexp_c_can_eval(_tmp5DB))
({void*_tmp5D6=0U;({unsigned int _tmpC17=e->loc;struct _dyneither_ptr _tmpC16=({const char*_tmp5D7="cannot determine subscript is in bounds";_tag_dyneither(_tmp5D7,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC17,_tmpC16,_tag_dyneither(_tmp5D6,sizeof(void*),0U));});});
# 3437
({void*_tmpC1A=({struct Cyc_Absyn_Exp*_tmpC19=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmpC19,({struct Cyc_Absyn_Exp*_tmp5D8[2U];({struct Cyc_Absyn_Exp*_tmpC18=
Cyc_Absyn_copy_exp(_tmp688);_tmp5D8[1U]=_tmpC18;});_tmp5D8[0U]=_tmp5DB;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D8,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 3437
_tmp688->r=_tmpC1A;});}}}}
# 3440
goto _LL122;}}else{_LL125: _LL126: {
# 3442
void*ta1=Cyc_Toc_typ_to_c(_tmp5E4);
if(in_bnds){
# 3446
++ Cyc_Toc_bounds_checks_eliminated;
({void*_tmpC1D=({struct Cyc_Absyn_Exp*_tmpC1C=({void*_tmpC1B=Cyc_Absyn_cstar_typ(ta1,_tmp5E3);Cyc_Toc_cast_it(_tmpC1B,
Cyc_Toc_member_exp(_tmp689,Cyc_Toc_curr_sp,0U));});
# 3447
Cyc_Toc_subscript_exp_r(_tmpC1C,_tmp688);});e->r=_tmpC1D;});}else{
# 3451
struct Cyc_Absyn_Exp*_tmp5D9=Cyc_Toc__check_dyneither_subscript_e;
({void*_tmpC21=Cyc_Toc_deref_exp_r(({void*_tmpC20=Cyc_Absyn_cstar_typ(ta1,_tmp5E3);Cyc_Toc_cast_it(_tmpC20,({
struct Cyc_Absyn_Exp*_tmpC1F=_tmp5D9;Cyc_Absyn_fncall_exp(_tmpC1F,({struct Cyc_Absyn_Exp*_tmp5DA[3U];_tmp5DA[2U]=_tmp688;({struct Cyc_Absyn_Exp*_tmpC1E=
Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp5DA[1U]=_tmpC1E;});_tmp5DA[0U]=_tmp689;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5DA,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 3452
e->r=_tmpC21;});}
# 3457
goto _LL122;}}_LL122:;}
# 3459
goto _LL115;}default: _LL11A: _LL11B:
({void*_tmp5DC=0U;({struct _dyneither_ptr _tmpC22=({const char*_tmp5DD="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp5DD,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC22,_tag_dyneither(_tmp5DC,sizeof(void*),0U));});});}_LL115:;}
# 3462
Cyc_Toc_set_lhs(nv,_tmp5B8);
goto _LL5;};}case 24U: _LL3A: _tmp68A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL3B:
# 3465
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp5E5=Cyc_Toc_init_tuple(nv,0,0,_tmp68A);
e->r=_tmp5E5->r;
e->topt=_tmp5E5->topt;}else{
# 3473
struct Cyc_List_List*_tmp5E6=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp68A);
void*_tmp5E7=Cyc_Toc_add_tuple_type(_tmp5E6);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp68A != 0;(_tmp68A=_tmp68A->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp68A->hd);
({struct Cyc_List_List*_tmpC24=({struct Cyc_List_List*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));({struct _tuple19*_tmpC23=({struct _tuple19*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->f1=0;_tmp5E9->f2=(struct Cyc_Absyn_Exp*)_tmp68A->hd;_tmp5E9;});_tmp5E8->hd=_tmpC23;});_tmp5E8->tl=dles;_tmp5E8;});dles=_tmpC24;});}}
# 3480
({struct Cyc_List_List*_tmpC25=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);dles=_tmpC25;});
({void*_tmpC26=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmpC26;});}
# 3483
goto _LL5;case 26U: _LL3C: _tmp68B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL3D:
# 3487
({void*_tmpC27=Cyc_Toc_unresolvedmem_exp_r(0,_tmp68B);e->r=_tmpC27;});
{struct Cyc_List_List*_tmp5EA=_tmp68B;for(0;_tmp5EA != 0;_tmp5EA=_tmp5EA->tl){
struct _tuple19*_tmp5EB=(struct _tuple19*)_tmp5EA->hd;struct _tuple19*_tmp5EC=_tmp5EB;struct Cyc_Absyn_Exp*_tmp5ED;_LL128: _tmp5ED=_tmp5EC->f2;_LL129:;
Cyc_Toc_exp_to_c(nv,_tmp5ED);}}
# 3492
goto _LL5;case 27U: _LL3E: _tmp68F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp68E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp68D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp68C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL3F: {
# 3496
struct _tuple12 _tmp5EE=Cyc_Evexp_eval_const_uint_exp(_tmp68E);struct _tuple12 _tmp5EF=_tmp5EE;unsigned int _tmp5F9;int _tmp5F8;_LL12B: _tmp5F9=_tmp5EF.f1;_tmp5F8=_tmp5EF.f2;_LL12C:;{
void*_tmp5F0=Cyc_Toc_typ_to_c((void*)_check_null(_tmp68D->topt));
Cyc_Toc_exp_to_c(nv,_tmp68D);{
struct Cyc_List_List*es=0;
# 3501
if(!Cyc_Toc_is_zero(_tmp68D)){
if(!_tmp5F8)
({void*_tmp5F1=0U;({unsigned int _tmpC29=_tmp68E->loc;struct _dyneither_ptr _tmpC28=({const char*_tmp5F2="cannot determine value of constant";_tag_dyneither(_tmp5F2,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpC29,_tmpC28,_tag_dyneither(_tmp5F1,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp5F9;++ i){
({struct Cyc_List_List*_tmpC2B=({struct Cyc_List_List*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));({struct _tuple19*_tmpC2A=({struct _tuple19*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->f1=0;_tmp5F4->f2=_tmp68D;_tmp5F4;});_tmp5F3->hd=_tmpC2A;});_tmp5F3->tl=es;_tmp5F3;});es=_tmpC2B;});}}
# 3507
if(_tmp68C){
struct Cyc_Absyn_Exp*_tmp5F5=({void*_tmpC2C=_tmp5F0;Cyc_Toc_cast_it(_tmpC2C,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmpC2F=({struct Cyc_List_List*_tmpC2E=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC2E,({struct Cyc_List_List*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));({struct _tuple19*_tmpC2D=({struct _tuple19*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->f1=0;_tmp5F7->f2=_tmp5F5;_tmp5F7;});_tmp5F6->hd=_tmpC2D;});_tmp5F6->tl=0;_tmp5F6;}));});es=_tmpC2F;});}}
# 3512
({void*_tmpC30=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmpC30;});
goto _LL5;};};}case 28U: _LL40: _tmp692=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp691=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp690=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL41:
# 3516
({void*_tmpC31=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmpC31;});
goto _LL5;case 29U: _LL42: _tmp696=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp695=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp694=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp693=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL43:
# 3521
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp5FA=Cyc_Toc_init_struct(nv,old_typ,_tmp695,0,0,_tmp694,_tmp696);
e->r=_tmp5FA->r;
e->topt=_tmp5FA->topt;}else{
# 3532
if(_tmp693 == 0)
({void*_tmp5FB=0U;({struct _dyneither_ptr _tmpC32=({const char*_tmp5FC="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp5FC,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC32,_tag_dyneither(_tmp5FB,sizeof(void*),0U));});});{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp693;
# 3536
struct _RegionHandle _tmp5FD=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5FD;_push_region(rgn);
{struct Cyc_List_List*_tmp5FE=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp694,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3540
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp693->impl))->tagged){
# 3542
struct _tuple30*_tmp5FF=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp5FE))->hd;struct _tuple30*_tmp600=_tmp5FF;struct Cyc_Absyn_Aggrfield*_tmp60D;struct Cyc_Absyn_Exp*_tmp60C;_LL12E: _tmp60D=_tmp600->f1;_tmp60C=_tmp600->f2;_LL12F:;{
void*_tmp601=(void*)_check_null(_tmp60C->topt);
void*_tmp602=_tmp60D->type;
Cyc_Toc_exp_to_c(nv,_tmp60C);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp602) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp601))
({void*_tmpC34=({
void*_tmpC33=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpC33,Cyc_Absyn_new_exp(_tmp60C->r,0U));});
# 3548
_tmp60C->r=_tmpC34;});{
# 3551
int i=Cyc_Toc_get_member_offset(_tmp693,_tmp60D->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp603=({struct _tuple19*_tmp609[2U];({struct _tuple19*_tmpC35=({struct _tuple19*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->f1=0;_tmp60B->f2=_tmp60C;_tmp60B;});_tmp609[1U]=_tmpC35;});({struct _tuple19*_tmpC36=({struct _tuple19*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->f1=0;_tmp60A->f2=field_tag_exp;_tmp60A;});_tmp609[0U]=_tmpC36;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp609,sizeof(struct _tuple19*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp603,0U);
struct Cyc_List_List*ds=({void*_tmp606[1U];({void*_tmpC38=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp607=_cycalloc(sizeof(*_tmp607));({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC37=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp608;_tmp608.tag=1U;_tmp608.f1=_tmp60D->name;_tmp608;});_tmp607[0]=_tmpC37;});_tmp607;});_tmp606[0U]=_tmpC38;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp606,sizeof(void*),1U));});
struct Cyc_List_List*dles=({struct _tuple19*_tmp604[1U];({struct _tuple19*_tmpC39=({struct _tuple19*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->f1=ds;_tmp605->f2=umem;_tmp605;});_tmp604[0U]=_tmpC39;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp604,sizeof(struct _tuple19*),1U));});
({void*_tmpC3A=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmpC3A;});};};}else{
# 3560
struct Cyc_List_List*_tmp60E=0;
struct Cyc_List_List*_tmp60F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp60F != 0;_tmp60F=_tmp60F->tl){
struct Cyc_List_List*_tmp610=_tmp5FE;for(0;_tmp610 != 0;_tmp610=_tmp610->tl){
if((*((struct _tuple30*)_tmp610->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp60F->hd){
struct _tuple30*_tmp611=(struct _tuple30*)_tmp610->hd;struct _tuple30*_tmp612=_tmp611;struct Cyc_Absyn_Aggrfield*_tmp618;struct Cyc_Absyn_Exp*_tmp617;_LL131: _tmp618=_tmp612->f1;_tmp617=_tmp612->f2;_LL132:;{
void*_tmp613=Cyc_Toc_typ_to_c(_tmp618->type);
void*_tmp614=Cyc_Toc_typ_to_c((void*)_check_null(_tmp617->topt));
Cyc_Toc_exp_to_c(nv,_tmp617);
# 3570
if(!Cyc_Tcutil_unify(_tmp613,_tmp614)){
# 3572
if(!Cyc_Tcutil_is_arithmetic_type(_tmp613) || !
Cyc_Tcutil_is_arithmetic_type(_tmp614))
({struct Cyc_Absyn_Exp*_tmpC3C=({void*_tmpC3B=_tmp613;Cyc_Toc_cast_it(_tmpC3B,Cyc_Absyn_copy_exp(_tmp617));});_tmp617=_tmpC3C;});}
({struct Cyc_List_List*_tmpC3E=({struct Cyc_List_List*_tmp615=_cycalloc(sizeof(*_tmp615));({struct _tuple19*_tmpC3D=({struct _tuple19*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->f1=0;_tmp616->f2=_tmp617;_tmp616;});_tmp615->hd=_tmpC3D;});_tmp615->tl=_tmp60E;_tmp615;});_tmp60E=_tmpC3E;});
break;};}}}
# 3579
({void*_tmpC3F=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp60E));e->r=_tmpC3F;});}}
# 3537
;_pop_region(rgn);};}
# 3583
goto _LL5;case 30U: _LL44: _tmp698=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp697=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL45: {
# 3585
struct Cyc_List_List*fs;
{void*_tmp619=Cyc_Tcutil_compress(_tmp698);void*_tmp61A=_tmp619;struct Cyc_List_List*_tmp61E;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp61A)->tag == 12U){_LL134: _tmp61E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp61A)->f2;_LL135:
 fs=_tmp61E;goto _LL133;}else{_LL136: _LL137:
({struct Cyc_String_pa_PrintArg_struct _tmp61D;_tmp61D.tag=0U;({struct _dyneither_ptr _tmpC40=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp698));_tmp61D.f1=_tmpC40;});({void*_tmp61B[1U]={& _tmp61D};({struct _dyneither_ptr _tmpC41=({const char*_tmp61C="anon struct has type %s";_tag_dyneither(_tmp61C,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC41,_tag_dyneither(_tmp61B,sizeof(void*),1U));});});});}_LL133:;}{
# 3590
struct _RegionHandle _tmp61F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp61F;_push_region(rgn);{
struct Cyc_List_List*_tmp620=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp697,Cyc_Absyn_StructA,fs);
for(0;_tmp620 != 0;_tmp620=_tmp620->tl){
struct _tuple30*_tmp621=(struct _tuple30*)_tmp620->hd;struct _tuple30*_tmp622=_tmp621;struct Cyc_Absyn_Aggrfield*_tmp626;struct Cyc_Absyn_Exp*_tmp625;_LL139: _tmp626=_tmp622->f1;_tmp625=_tmp622->f2;_LL13A:;{
void*_tmp623=(void*)_check_null(_tmp625->topt);
void*_tmp624=_tmp626->type;
Cyc_Toc_exp_to_c(nv,_tmp625);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp624) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp623))
({void*_tmpC43=({void*_tmpC42=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpC42,Cyc_Absyn_new_exp(_tmp625->r,0U));});_tmp625->r=_tmpC43;});};}
# 3604
({void*_tmpC44=Cyc_Toc_unresolvedmem_exp_r(0,_tmp697);e->r=_tmpC44;});}
# 3606
_npop_handler(0U);goto _LL5;
# 3590
;_pop_region(rgn);};}case 31U: _LL46: _tmp69B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp69A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp699=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL47: {
# 3609
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp627=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp628=Cyc_Absyn_var_exp(_tmp627,0U);
struct Cyc_Absyn_Exp*mem_exp;
({void*_tmpC45=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp699->name,_tmp69A->name));datatype_ctype=_tmpC45;});
({struct Cyc_Absyn_Exp*_tmpC46=_tmp69A->is_extensible?Cyc_Absyn_var_exp(_tmp699->name,0U):
 Cyc_Toc_datatype_tag(_tmp69A,_tmp699->name);
# 3615
tag_exp=_tmpC46;});
# 3617
mem_exp=_tmp628;{
struct Cyc_List_List*_tmp629=_tmp699->typs;
# 3620
if(Cyc_Toc_is_toplevel(nv)){
# 3622
struct Cyc_List_List*dles=0;
for(0;_tmp69B != 0;(_tmp69B=_tmp69B->tl,_tmp629=_tmp629->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp69B->hd;
void*_tmp62A=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp629))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp62A))
({struct Cyc_Absyn_Exp*_tmpC47=Cyc_Toc_cast_it(field_typ,cur_e);cur_e=_tmpC47;});
({struct Cyc_List_List*_tmpC49=({struct Cyc_List_List*_tmp62B=_cycalloc(sizeof(*_tmp62B));({struct _tuple19*_tmpC48=({struct _tuple19*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->f1=0;_tmp62C->f2=cur_e;_tmp62C;});_tmp62B->hd=_tmpC48;});_tmp62B->tl=dles;_tmp62B;});dles=_tmpC49;});}
# 3633
({struct Cyc_List_List*_tmpC4C=({struct Cyc_List_List*_tmp62D=_cycalloc(sizeof(*_tmp62D));({struct _tuple19*_tmpC4A=({struct _tuple19*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->f1=0;_tmp62E->f2=tag_exp;_tmp62E;});_tmp62D->hd=_tmpC4A;});({struct Cyc_List_List*_tmpC4B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp62D->tl=_tmpC4B;});_tmp62D;});dles=_tmpC4C;});
({void*_tmpC4D=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmpC4D;});}else{
# 3639
struct Cyc_List_List*_tmp62F=({struct Cyc_List_List*_tmp636=_cycalloc(sizeof(*_tmp636));({struct Cyc_Absyn_Stmt*_tmpC4F=({
struct Cyc_Absyn_Exp*_tmpC4E=Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0U);Cyc_Absyn_assign_stmt(_tmpC4E,tag_exp,0U);});_tmp636->hd=_tmpC4F;});_tmp636->tl=0;_tmp636;});
# 3642
{int i=1;for(0;_tmp69B != 0;(((_tmp69B=_tmp69B->tl,i ++)),_tmp629=_tmp629->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp69B->hd;
void*_tmp630=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp629))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp630))
({struct Cyc_Absyn_Exp*_tmpC50=Cyc_Toc_cast_it(field_typ,cur_e);cur_e=_tmpC50;});{
struct Cyc_Absyn_Stmt*_tmp631=({struct Cyc_Absyn_Exp*_tmpC52=({struct Cyc_Absyn_Exp*_tmpC51=mem_exp;Cyc_Toc_member_exp(_tmpC51,Cyc_Absyn_fieldname(i),0U);});Cyc_Absyn_assign_stmt(_tmpC52,cur_e,0U);});
# 3652
({struct Cyc_List_List*_tmpC53=({struct Cyc_List_List*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->hd=_tmp631;_tmp632->tl=_tmp62F;_tmp632;});_tmp62F=_tmpC53;});};}}{
# 3654
struct Cyc_Absyn_Stmt*_tmp633=Cyc_Absyn_exp_stmt(_tmp628,0U);
struct Cyc_Absyn_Stmt*_tmp634=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->hd=_tmp633;_tmp635->tl=_tmp62F;_tmp635;})),0U);
({void*_tmpC54=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp627,datatype_ctype,0,_tmp634,0U));e->r=_tmpC54;});};}
# 3658
goto _LL5;};}case 32U: _LL48: _LL49:
# 3660
 goto _LL4B;case 33U: _LL4A: _LL4B:
 goto _LL5;case 34U: _LL4C: _tmp6A1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).is_calloc;_tmp6A0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).rgn;_tmp69F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).elt_type;_tmp69E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).num_elts;_tmp69D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).fat_result;_tmp69C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).inline_call;_LL4D: {
# 3664
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp69F)));
Cyc_Toc_exp_to_c(nv,_tmp69E);
# 3668
if(_tmp69D){
struct _tuple1*_tmp637=Cyc_Toc_temp_var();
struct _tuple1*_tmp638=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp6A1){
xexp=_tmp69E;
if(_tmp6A0 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A0;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmpC57=({struct Cyc_Absyn_Exp*_tmpC56=rgn;struct Cyc_Absyn_Exp*_tmpC55=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpC56,_tmpC55,Cyc_Absyn_var_exp(_tmp637,0U));});pexp=_tmpC57;});}else{
# 3679
({struct Cyc_Absyn_Exp*_tmpC5A=({void*_tmpC59=*_tmp69F;struct Cyc_Absyn_Exp*_tmpC58=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpC59,_tmpC58,Cyc_Absyn_var_exp(_tmp637,0U));});pexp=_tmpC5A;});}
# 3681
({struct Cyc_Absyn_Exp*_tmpC5F=({struct Cyc_Absyn_Exp*_tmpC5E=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpC5E,({struct Cyc_Absyn_Exp*_tmp639[3U];({struct Cyc_Absyn_Exp*_tmpC5B=
# 3683
Cyc_Absyn_var_exp(_tmp637,0U);_tmp639[2U]=_tmpC5B;});({struct Cyc_Absyn_Exp*_tmpC5C=
# 3682
Cyc_Absyn_sizeoftyp_exp(t_c,0U);_tmp639[1U]=_tmpC5C;});({struct Cyc_Absyn_Exp*_tmpC5D=Cyc_Absyn_var_exp(_tmp638,0U);_tmp639[0U]=_tmpC5D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp639,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3681
rexp=_tmpC5F;});}else{
# 3685
if(_tmp6A0 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A0;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp69C)
({struct Cyc_Absyn_Exp*_tmpC61=({struct Cyc_Absyn_Exp*_tmpC60=rgn;Cyc_Toc_rmalloc_inline_exp(_tmpC60,Cyc_Absyn_var_exp(_tmp637,0U));});pexp=_tmpC61;});else{
# 3691
({struct Cyc_Absyn_Exp*_tmpC63=({struct Cyc_Absyn_Exp*_tmpC62=rgn;Cyc_Toc_rmalloc_exp(_tmpC62,Cyc_Absyn_var_exp(_tmp637,0U));});pexp=_tmpC63;});}}else{
# 3693
({struct Cyc_Absyn_Exp*_tmpC65=({void*_tmpC64=*_tmp69F;Cyc_Toc_malloc_exp(_tmpC64,Cyc_Absyn_var_exp(_tmp637,0U));});pexp=_tmpC65;});}
# 3695
({struct Cyc_Absyn_Exp*_tmpC6A=({struct Cyc_Absyn_Exp*_tmpC69=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpC69,({struct Cyc_Absyn_Exp*_tmp63A[3U];({struct Cyc_Absyn_Exp*_tmpC66=
Cyc_Absyn_var_exp(_tmp637,0U);_tmp63A[2U]=_tmpC66;});({struct Cyc_Absyn_Exp*_tmpC67=
# 3695
Cyc_Absyn_uint_exp(1U,0U);_tmp63A[1U]=_tmpC67;});({struct Cyc_Absyn_Exp*_tmpC68=Cyc_Absyn_var_exp(_tmp638,0U);_tmp63A[0U]=_tmpC68;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp63A,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});rexp=_tmpC6A;});}{
# 3698
struct Cyc_Absyn_Stmt*_tmp63B=({struct _tuple1*_tmpC70=_tmp637;void*_tmpC6F=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpC6E=_tmp69E;Cyc_Absyn_declare_stmt(_tmpC70,_tmpC6F,_tmpC6E,({
struct _tuple1*_tmpC6D=_tmp638;void*_tmpC6C=Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpC6B=pexp;Cyc_Absyn_declare_stmt(_tmpC6D,_tmpC6C,_tmpC6B,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmpC71=Cyc_Toc_stmt_exp_r(_tmp63B);e->r=_tmpC71;});};}else{
# 3703
if(_tmp6A1){
if(_tmp6A0 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A0;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmpC74=({struct Cyc_Absyn_Exp*_tmpC73=rgn;struct Cyc_Absyn_Exp*_tmpC72=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpC73,_tmpC72,_tmp69E);})->r;e->r=_tmpC74;});}else{
# 3709
({void*_tmpC77=({void*_tmpC76=*_tmp69F;struct Cyc_Absyn_Exp*_tmpC75=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpC76,_tmpC75,_tmp69E);})->r;e->r=_tmpC77;});}}else{
# 3712
if(_tmp6A0 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A0;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp69C)
({void*_tmpC78=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp69E))->r;e->r=_tmpC78;});else{
# 3718
({void*_tmpC79=(Cyc_Toc_rmalloc_exp(rgn,_tmp69E))->r;e->r=_tmpC79;});}}else{
# 3720
({void*_tmpC7A=(Cyc_Toc_malloc_exp(*_tmp69F,_tmp69E))->r;e->r=_tmpC7A;});}}}
# 3724
goto _LL5;}case 35U: _LL4E: _tmp6A3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp6A2=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL4F: {
# 3733
void*e1_old_typ=(void*)_check_null(_tmp6A3->topt);
void*e2_old_typ=(void*)_check_null(_tmp6A2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp63C=0U;({struct _dyneither_ptr _tmpC7B=({const char*_tmp63D="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp63D,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC7B,_tag_dyneither(_tmp63C,sizeof(void*),0U));});});{
# 3741
unsigned int _tmp63E=e->loc;
struct _tuple1*_tmp63F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp640=Cyc_Absyn_var_exp(_tmp63F,_tmp63E);_tmp640->topt=e1_old_typ;{
struct _tuple1*_tmp641=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp642=Cyc_Absyn_var_exp(_tmp641,_tmp63E);_tmp642->topt=e2_old_typ;{
# 3747
struct Cyc_Absyn_Exp*_tmp643=({struct Cyc_Absyn_Exp*_tmpC7D=Cyc_Tcutil_deep_copy_exp(1,_tmp6A3);struct Cyc_Absyn_Exp*_tmpC7C=_tmp642;Cyc_Absyn_assign_exp(_tmpC7D,_tmpC7C,_tmp63E);});_tmp643->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp644=Cyc_Absyn_exp_stmt(_tmp643,_tmp63E);
struct Cyc_Absyn_Exp*_tmp645=({struct Cyc_Absyn_Exp*_tmpC7F=Cyc_Tcutil_deep_copy_exp(1,_tmp6A2);struct Cyc_Absyn_Exp*_tmpC7E=_tmp640;Cyc_Absyn_assign_exp(_tmpC7F,_tmpC7E,_tmp63E);});_tmp645->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp646=Cyc_Absyn_exp_stmt(_tmp645,_tmp63E);
# 3752
struct Cyc_Absyn_Stmt*_tmp647=({struct _tuple1*_tmpC87=_tmp63F;void*_tmpC86=e1_old_typ;struct Cyc_Absyn_Exp*_tmpC85=_tmp6A3;struct Cyc_Absyn_Stmt*_tmpC84=({
struct _tuple1*_tmpC83=_tmp641;void*_tmpC82=e2_old_typ;struct Cyc_Absyn_Exp*_tmpC81=_tmp6A2;struct Cyc_Absyn_Stmt*_tmpC80=
Cyc_Absyn_seq_stmt(_tmp644,_tmp646,_tmp63E);
# 3753
Cyc_Absyn_declare_stmt(_tmpC83,_tmpC82,_tmpC81,_tmpC80,_tmp63E);});
# 3752
Cyc_Absyn_declare_stmt(_tmpC87,_tmpC86,_tmpC85,_tmpC84,_tmp63E);});
# 3755
Cyc_Toc_stmt_to_c(nv,_tmp647);
({void*_tmpC88=Cyc_Toc_stmt_exp_r(_tmp647);e->r=_tmpC88;});
goto _LL5;};};};};};}case 38U: _LL50: _tmp6A5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp6A4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL51: {
# 3760
void*_tmp648=Cyc_Tcutil_compress((void*)_check_null(_tmp6A5->topt));
Cyc_Toc_exp_to_c(nv,_tmp6A5);
{void*_tmp649=_tmp648;struct Cyc_Absyn_Aggrdecl*_tmp64F;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp649)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp649)->f1).aggr_info).KnownAggr).tag == 2){_LL13C: _tmp64F=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp649)->f1).aggr_info).KnownAggr).val;_LL13D: {
# 3764
struct Cyc_Absyn_Exp*_tmp64A=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp64F,_tmp6A4),0U);
struct Cyc_Absyn_Exp*_tmp64B=Cyc_Toc_member_exp(_tmp6A5,_tmp6A4,0U);
struct Cyc_Absyn_Exp*_tmp64C=Cyc_Toc_member_exp(_tmp64B,Cyc_Toc_tag_sp,0U);
({void*_tmpC89=(Cyc_Absyn_eq_exp(_tmp64C,_tmp64A,0U))->r;e->r=_tmpC89;});
goto _LL13B;}}else{goto _LL13E;}}else{_LL13E: _LL13F:
({void*_tmp64D=0U;({struct _dyneither_ptr _tmpC8A=({const char*_tmp64E="non-aggregate type in tagcheck";_tag_dyneither(_tmp64E,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC8A,_tag_dyneither(_tmp64D,sizeof(void*),0U));});});}_LL13B:;}
# 3771
goto _LL5;}case 37U: _LL52: _tmp6A6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL53:
 Cyc_Toc_stmt_to_c(nv,_tmp6A6);goto _LL5;case 36U: _LL54: _LL55:
({void*_tmp650=0U;({struct _dyneither_ptr _tmpC8B=({const char*_tmp651="UnresolvedMem";_tag_dyneither(_tmp651,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC8B,_tag_dyneither(_tmp650,sizeof(void*),0U));});});case 25U: _LL56: _LL57:
({void*_tmp652=0U;({struct _dyneither_ptr _tmpC8C=({const char*_tmp653="compoundlit";_tag_dyneither(_tmp653,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpC8C,_tag_dyneither(_tmp652,sizeof(void*),0U));});});case 39U: _LL58: _LL59:
({void*_tmp654=0U;({struct _dyneither_ptr _tmpC8D=({const char*_tmp655="valueof(-)";_tag_dyneither(_tmp655,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC8D,_tag_dyneither(_tmp654,sizeof(void*),0U));});});default: _LL5A: _LL5B:
({void*_tmp656=0U;({struct _dyneither_ptr _tmpC8E=({const char*_tmp657="__asm__";_tag_dyneither(_tmp657,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC8E,_tag_dyneither(_tmp656,sizeof(void*),0U));});});}_LL5:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3804 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3806
return({struct _tuple31*_tmp6A7=_region_malloc(r,sizeof(*_tmp6A7));_tmp6A7->f1=0;({struct _dyneither_ptr*_tmpC8F=Cyc_Toc_fresh_label();_tmp6A7->f2=_tmpC8F;});({struct _dyneither_ptr*_tmpC90=Cyc_Toc_fresh_label();_tmp6A7->f3=_tmpC90;});_tmp6A7->f4=sc;_tmp6A7;});}
# 3811
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp6A8=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3817
if((int)(((_tmp6A8->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp6AB=_tmp6A8->orig_pat;if((_tmp6AB.pattern).tag != 1)_throw_match();(_tmp6AB.pattern).val;})->topt);
void*_tmp6A9=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp6AA=_tmp6A9;switch(*((int*)_tmp6AA)){case 0U: _LL1: _LL2:
# 3822
 goto _LL4;case 11U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpC92=({void*_tmpC91=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpC91,v);});v=_tmpC92;});goto _LL0;}_LL0:;}{
# 3828
void*_tmp6AC=_tmp6A8->access;void*_tmp6AD=_tmp6AC;struct Cyc_Absyn_Datatypedecl*_tmp6B8;struct Cyc_Absyn_Datatypefield*_tmp6B7;unsigned int _tmp6B6;int _tmp6B5;struct _dyneither_ptr*_tmp6B4;unsigned int _tmp6B3;switch(*((int*)_tmp6AD)){case 0U: _LLA: _LLB:
# 3833
 goto _LL9;case 1U: _LLC: _LLD:
# 3838
 if(ps->tl != 0){
void*_tmp6AE=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp6AF=_tmp6AE;struct Cyc_Absyn_Datatypedecl*_tmp6B2;struct Cyc_Absyn_Datatypefield*_tmp6B1;unsigned int _tmp6B0;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AF)->tag == 3U){_LL15: _tmp6B2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AF)->f1;_tmp6B1=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AF)->f2;_tmp6B0=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AF)->f3;_LL16:
# 3841
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpC95=({void*_tmpC94=({void*_tmpC93=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6B1->name,_tmp6B2->name));Cyc_Absyn_cstar_typ(_tmpC93,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpC94,v);});v=_tmpC95;});
({struct Cyc_Absyn_Exp*_tmpC97=({struct Cyc_Absyn_Exp*_tmpC96=v;Cyc_Absyn_aggrarrow_exp(_tmpC96,Cyc_Absyn_fieldname((int)(_tmp6B0 + 1)),0U);});v=_tmpC97;});
continue;}else{_LL17: _LL18:
# 3846
 goto _LL14;}_LL14:;}
# 3849
({struct Cyc_Absyn_Exp*_tmpC98=Cyc_Absyn_deref_exp(v,0U);v=_tmpC98;});
goto _LL9;case 2U: _LLE: _tmp6B3=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp6AD)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpC9A=({struct Cyc_Absyn_Exp*_tmpC99=v;Cyc_Toc_member_exp(_tmpC99,Cyc_Absyn_fieldname((int)(_tmp6B3 + 1)),0U);});v=_tmpC9A;});goto _LL9;case 4U: _LL10: _tmp6B5=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6AD)->f1;_tmp6B4=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6AD)->f2;_LL11:
# 3853
({struct Cyc_Absyn_Exp*_tmpC9B=Cyc_Toc_member_exp(v,_tmp6B4,0U);v=_tmpC9B;});
if(_tmp6B5)
({struct Cyc_Absyn_Exp*_tmpC9C=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpC9C;});
goto _LL9;default: _LL12: _tmp6B8=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AD)->f1;_tmp6B7=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AD)->f2;_tmp6B6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AD)->f3;_LL13:
# 3858
({struct Cyc_Absyn_Exp*_tmpC9E=({struct Cyc_Absyn_Exp*_tmpC9D=v;Cyc_Toc_member_exp(_tmpC9D,Cyc_Absyn_fieldname((int)(_tmp6B6 + 1)),0U);});v=_tmpC9E;});
goto _LL9;}_LL9:;};}
# 3862
return v;}
# 3867
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp6B9=t;struct Cyc_Absyn_Datatypedecl*_tmp6D4;struct Cyc_Absyn_Datatypefield*_tmp6D3;struct _dyneither_ptr*_tmp6D2;int _tmp6D1;int _tmp6D0;struct Cyc_Absyn_Datatypedecl*_tmp6CF;struct Cyc_Absyn_Datatypefield*_tmp6CE;unsigned int _tmp6CD;struct _dyneither_ptr _tmp6CC;int _tmp6CB;void*_tmp6CA;struct Cyc_Absyn_Enumfield*_tmp6C9;struct Cyc_Absyn_Enumdecl*_tmp6C8;struct Cyc_Absyn_Enumfield*_tmp6C7;struct Cyc_Absyn_Exp*_tmp6C6;switch(*((int*)_tmp6B9)){case 0U: _LL1: _tmp6C6=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6B9)->f1;_LL2:
# 3871
 if(_tmp6C6 == 0)return v;else{return _tmp6C6;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpC9F=v;Cyc_Absyn_eq_exp(_tmpC9F,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpCA0=v;Cyc_Absyn_neq_exp(_tmpCA0,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp6C8=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6B9)->f1;_tmp6C7=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6B9)->f2;_LL8:
# 3875
 return({struct Cyc_Absyn_Exp*_tmpCA2=v;Cyc_Absyn_eq_exp(_tmpCA2,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCA1=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6BB;_tmp6BB.tag=32U;_tmp6BB.f1=_tmp6C8;_tmp6BB.f2=_tmp6C7;_tmp6BB;});_tmp6BA[0]=_tmpCA1;});_tmp6BA;}),0U),0U);});case 4U: _LL9: _tmp6CA=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6B9)->f1;_tmp6C9=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6B9)->f2;_LLA:
# 3877
 return({struct Cyc_Absyn_Exp*_tmpCA4=v;Cyc_Absyn_eq_exp(_tmpCA4,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCA3=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6BD;_tmp6BD.tag=33U;_tmp6BD.f1=_tmp6CA;_tmp6BD.f2=_tmp6C9;_tmp6BD;});_tmp6BC[0]=_tmpCA3;});_tmp6BC;}),0U),0U);});case 5U: _LLB: _tmp6CC=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6B9)->f1;_tmp6CB=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6B9)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpCA5=v;Cyc_Absyn_eq_exp(_tmpCA5,Cyc_Absyn_float_exp(_tmp6CC,_tmp6CB,0U),0U);});case 6U: _LLD: _tmp6CD=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6B9)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpCA6=v;Cyc_Absyn_eq_exp(_tmpCA6,Cyc_Absyn_signed_int_exp((int)_tmp6CD,0U),0U);});case 7U: _LLF: _tmp6D0=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6B9)->f1;_tmp6CF=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6B9)->f2;_tmp6CE=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6B9)->f3;_LL10:
# 3883
 LOOP1: {
void*_tmp6BE=v->r;void*_tmp6BF=_tmp6BE;struct Cyc_Absyn_Exp*_tmp6C1;struct Cyc_Absyn_Exp*_tmp6C0;switch(*((int*)_tmp6BF)){case 14U: _LL16: _tmp6C0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6BF)->f2;_LL17:
 v=_tmp6C0;goto LOOP1;case 20U: _LL18: _tmp6C1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_LL19:
 v=_tmp6C1;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3890
({struct Cyc_Absyn_Exp*_tmpCA9=({void*_tmpCA8=({void*_tmpCA7=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6CE->name,_tmp6CF->name));Cyc_Absyn_cstar_typ(_tmpCA7,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpCA8,v);});v=_tmpCA9;});
return({struct Cyc_Absyn_Exp*_tmpCAA=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpCAA,Cyc_Absyn_uint_exp((unsigned int)_tmp6D0,0U),0U);});case 8U: _LL11: _tmp6D2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6B9)->f1;_tmp6D1=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6B9)->f2;_LL12:
# 3895
({struct Cyc_Absyn_Exp*_tmpCAC=({struct Cyc_Absyn_Exp*_tmpCAB=Cyc_Toc_member_exp(v,_tmp6D2,0U);Cyc_Toc_member_exp(_tmpCAB,Cyc_Toc_tag_sp,0U);});v=_tmpCAC;});
return({struct Cyc_Absyn_Exp*_tmpCAD=v;Cyc_Absyn_eq_exp(_tmpCAD,Cyc_Absyn_signed_int_exp(_tmp6D1,0U),0U);});default: _LL13: _tmp6D4=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6B9)->f1;_tmp6D3=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6B9)->f2;_LL14:
# 3899
 LOOP2: {
void*_tmp6C2=v->r;void*_tmp6C3=_tmp6C2;struct Cyc_Absyn_Exp*_tmp6C5;struct Cyc_Absyn_Exp*_tmp6C4;switch(*((int*)_tmp6C3)){case 14U: _LL1D: _tmp6C4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6C3)->f2;_LL1E:
 v=_tmp6C4;goto LOOP2;case 20U: _LL1F: _tmp6C5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6C3)->f1;_LL20:
 v=_tmp6C5;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3906
({struct Cyc_Absyn_Exp*_tmpCB0=({void*_tmpCAF=({void*_tmpCAE=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6D3->name,_tmp6D4->name));Cyc_Absyn_cstar_typ(_tmpCAE,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpCAF,v);});v=_tmpCB0;});
return({struct Cyc_Absyn_Exp*_tmpCB1=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpCB1,Cyc_Absyn_var_exp(_tmp6D3->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3920
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple32{int f1;void*f2;};
# 3928
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3930
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp6D7=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp6D8=_tmp6D7;void*_tmp6E2;_LL1: _tmp6E2=_tmp6D8.f1;_LL2:;{
void*_tmp6D9=_tmp6E2;struct Cyc_Absyn_Exp*_tmp6E1;struct _dyneither_ptr*_tmp6E0;switch(*((int*)_tmp6D9)){case 3U: _LL4: _LL5:
# 3936
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3939
 continue;case 8U: _LLA: _tmp6E0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6D9)->f1;_LLB:
# 3941
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpCB3=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmpCB2=({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmp6DB;_tmp6DB.tag=3U;_tmp6DB.f1=_tmp6E0;_tmp6DB;});_tmp6DA[0]=_tmpCB2;});_tmp6DA;});k=_tmpCB3;});
continue;case 7U: _LLC: _LLD:
# 3945
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp6E1=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6D9)->f1;if(_tmp6E1 != 0){_LLF:
# 3949
({void*_tmpCB5=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmpCB4=({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmp6DD;_tmp6DD.tag=2U;_tmp6DD.f1=_tmp6E1;_tmp6DD;});_tmp6DC[0]=_tmpCB4;});_tmp6DC;});k=_tmpCB5;});
return({struct _tuple32 _tmp6DE;_tmp6DE.f1=0;_tmp6DE.f2=k;_tmp6DE;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple32 _tmp6DF;_tmp6DF.f1=0;_tmp6DF.f2=k;_tmp6DF;});}_LL3:;};}
# 3958
return({struct _tuple32 _tmp6E3;_tmp6E3.f1=c;_tmp6E3.f2=k;_tmp6E3;});}
# 3963
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp6E4=p;int _tmp6F1;int _tmp6F0;unsigned int _tmp6EF;void*_tmp6EE;struct Cyc_Absyn_Enumfield*_tmp6ED;struct Cyc_Absyn_Enumdecl*_tmp6EC;struct Cyc_Absyn_Enumfield*_tmp6EB;switch(*((int*)_tmp6E4)){case 3U: _LL1: _tmp6EC=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6E4)->f1;_tmp6EB=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6E4)->f2;_LL2:
# 3967
({struct Cyc_Absyn_Exp*_tmpCB7=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCB6=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6E6;_tmp6E6.tag=32U;_tmp6E6.f1=_tmp6EC;_tmp6E6.f2=_tmp6EB;_tmp6E6;});_tmp6E5[0]=_tmpCB6;});_tmp6E5;}),0U);e=_tmpCB7;});goto _LL0;case 4U: _LL3: _tmp6EE=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6E4)->f1;_tmp6ED=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6E4)->f2;_LL4:
# 3969
({struct Cyc_Absyn_Exp*_tmpCB9=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCB8=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6E8;_tmp6E8.tag=33U;_tmp6E8.f1=_tmp6EE;_tmp6E8.f2=_tmp6ED;_tmp6E8;});_tmp6E7[0]=_tmpCB8;});_tmp6E7;}),0U);e=_tmpCB9;});goto _LL0;case 6U: _LL5: _tmp6EF=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6E4)->f1;_LL6:
 _tmp6F0=(int)_tmp6EF;goto _LL8;case 7U: _LL7: _tmp6F0=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6E4)->f1;_LL8:
 _tmp6F1=_tmp6F0;goto _LLA;case 8U: _LL9: _tmp6F1=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6E4)->f2;_LLA:
# 3973
({struct Cyc_Absyn_Exp*_tmpCBA=Cyc_Absyn_uint_exp((unsigned int)_tmp6F1,0U);e=_tmpCBA;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp6E9=0U;({struct _dyneither_ptr _tmpCBB=({const char*_tmp6EA="compile_pat_test_as_case!";_tag_dyneither(_tmp6EA,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCBB,_tag_dyneither(_tmp6E9,sizeof(void*),0U));});});}_LL0:;}
# 3977
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmpCBC=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp6F3;_tmp6F3.tag=17U;_tmp6F3.f1=e;_tmp6F3;});_tmp6F2[0]=_tmpCBC;});_tmp6F2;}),0U);}
# 3981
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3990
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3993
void*t=(void*)_check_null(p->topt);
void*_tmp6F4=p->r;void*_tmp6F5=_tmp6F4;union Cyc_Absyn_AggrInfoU _tmp72D;struct Cyc_List_List*_tmp72C;struct Cyc_List_List*_tmp72B;struct Cyc_List_List*_tmp72A;struct Cyc_Absyn_Pat*_tmp729;struct Cyc_Absyn_Datatypedecl*_tmp728;struct Cyc_Absyn_Datatypefield*_tmp727;struct Cyc_List_List*_tmp726;struct Cyc_Absyn_Vardecl*_tmp725;struct Cyc_Absyn_Pat*_tmp724;struct Cyc_Absyn_Vardecl*_tmp723;struct Cyc_Absyn_Vardecl*_tmp722;struct Cyc_Absyn_Pat*_tmp721;struct Cyc_Absyn_Vardecl*_tmp720;switch(*((int*)_tmp6F5)){case 2U: _LL1: _tmp720=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp6F5)->f2;_LL2: {
# 3996
struct Cyc_Absyn_Pat*_tmp6F6=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp6F6->topt=p->topt;
_tmp722=_tmp720;_tmp721=_tmp6F6;goto _LL4;}case 1U: _LL3: _tmp722=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6F5)->f1;_tmp721=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6F5)->f2;_LL4:
# 4001
({struct Cyc_List_List*_tmpCBD=({struct Cyc_List_List*_tmp6F7=_region_malloc(rgn,sizeof(*_tmp6F7));_tmp6F7->hd=_tmp722;_tmp6F7->tl=*decls;_tmp6F7;});*decls=_tmpCBD;});{
struct Cyc_Absyn_Stmt*_tmp6F8=({struct Cyc_Absyn_Exp*_tmpCBF=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCBE=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6FA;_tmp6FA.tag=4U;_tmp6FA.f1=_tmp722;_tmp6FA;});_tmp6F9[0]=_tmpCBE;});_tmp6F9;}),0U);Cyc_Absyn_assign_stmt(_tmpCBF,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpCC0=_tmp6F8;Cyc_Toc_seq_stmt_opt(_tmpCC0,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp721));});};case 4U: _LL5: _tmp723=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp6F5)->f2;_LL6:
# 4006
({struct Cyc_List_List*_tmpCC1=({struct Cyc_List_List*_tmp6FB=_region_malloc(rgn,sizeof(*_tmp6FB));_tmp6FB->hd=_tmp723;_tmp6FB->tl=*decls;_tmp6FB;});*decls=_tmpCC1;});
return({struct Cyc_Absyn_Exp*_tmpCC3=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCC2=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6FD;_tmp6FD.tag=4U;_tmp6FD.f1=_tmp723;_tmp6FD;});_tmp6FC[0]=_tmpCC2;});_tmp6FC;}),0U);Cyc_Absyn_assign_stmt(_tmpCC3,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp725=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6F5)->f1;_tmp724=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6F5)->f2;_LLA:
# 4011
({struct Cyc_List_List*_tmpCC4=({struct Cyc_List_List*_tmp6FE=_region_malloc(rgn,sizeof(*_tmp6FE));_tmp6FE->hd=_tmp725;_tmp6FE->tl=*decls;_tmp6FE;});*decls=_tmpCC4;});
({void*_tmpCC5=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);_tmp725->type=_tmpCC5;});{
# 4014
struct Cyc_Absyn_Stmt*_tmp6FF=({struct Cyc_Absyn_Exp*_tmpCC9=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp700=_cycalloc(sizeof(*_tmp700));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCC6=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp701;_tmp701.tag=4U;_tmp701.f1=_tmp725;_tmp701;});_tmp700[0]=_tmpCC6;});_tmp700;}),0U);Cyc_Absyn_assign_stmt(_tmpCC9,({
void*_tmpCC8=({void*_tmpCC7=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_typ(_tmpCC7,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpCC8,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpCCA=_tmp6FF;Cyc_Toc_seq_stmt_opt(_tmpCCA,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp724));});};case 9U: _LLB: _LLC:
# 4019
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F5)->f1)->r)->tag == 8U){_LL17: _tmp728=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F5)->f1)->r)->f1;_tmp727=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F5)->f1)->r)->f2;_tmp726=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F5)->f1)->r)->f3;_LL18:
# 4028
 if(_tmp726 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp727->name,_tmp728->name);
void*_tmp702=({void*_tmpCCB=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_typ(_tmpCCB,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpCCC=Cyc_Toc_cast_it(_tmp702,path);path=_tmpCCC;});{
int cnt=1;
struct Cyc_List_List*_tmp703=_tmp727->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp726 != 0;(((_tmp726=_tmp726->tl,_tmp703=((struct Cyc_List_List*)_check_null(_tmp703))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp704=(struct Cyc_Absyn_Pat*)_tmp726->hd;
if(_tmp704->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp705=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp703))->hd)).f2;
void*_tmp706=(void*)_check_null(_tmp704->topt);
void*_tmp707=Cyc_Toc_typ_to_c_array(_tmp706);
struct Cyc_Absyn_Exp*_tmp708=({struct Cyc_Absyn_Exp*_tmpCCD=path;Cyc_Absyn_aggrarrow_exp(_tmpCCD,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp705)))
({struct Cyc_Absyn_Exp*_tmpCCE=Cyc_Toc_cast_it(_tmp707,_tmp708);_tmp708=_tmpCCE;});
({struct Cyc_Absyn_Stmt*_tmpCD0=({struct Cyc_Absyn_Stmt*_tmpCCF=s;Cyc_Toc_seq_stmt_opt(_tmpCCF,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp708,_tmp704));});s=_tmpCD0;});};}
# 4046
return s;};};}else{_LL21: _tmp729=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F5)->f1;_LL22:
# 4095
 return({struct _RegionHandle*_tmpCD4=rgn;struct Cyc_Toc_Env**_tmpCD3=nv;struct Cyc_List_List**_tmpCD2=decls;struct Cyc_Absyn_Exp*_tmpCD1=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpCD4,_tmpCD3,_tmpCD2,_tmpCD1,_tmp729);});}case 8U: _LL19: _tmp72A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp6F5)->f3;_LL1A:
# 4048
 _tmp72B=_tmp72A;goto _LL1C;case 5U: _LL1B: _tmp72B=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp6F5)->f1;_LL1C: {
# 4050
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp72B != 0;(_tmp72B=_tmp72B->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp709=(struct Cyc_Absyn_Pat*)_tmp72B->hd;
if(_tmp709->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp70A=(void*)_check_null(_tmp709->topt);
struct _dyneither_ptr*_tmp70B=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpCDA=({struct Cyc_Absyn_Stmt*_tmpCD9=s;Cyc_Toc_seq_stmt_opt(_tmpCD9,({struct _RegionHandle*_tmpCD8=rgn;struct Cyc_Toc_Env**_tmpCD7=nv;struct Cyc_List_List**_tmpCD6=decls;struct Cyc_Absyn_Exp*_tmpCD5=Cyc_Toc_member_exp(path,_tmp70B,0U);Cyc_Toc_extract_pattern_vars(_tmpCD8,_tmpCD7,_tmpCD6,_tmpCD5,_tmp709);}));});s=_tmpCDA;});};}
# 4060
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F5)->f1 == 0){_LL1D: _LL1E:
({void*_tmp70C=0U;({struct _dyneither_ptr _tmpCDB=({const char*_tmp70D="unresolved aggregate pattern!";_tag_dyneither(_tmp70D,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCDB,_tag_dyneither(_tmp70C,sizeof(void*),0U));});});}else{_LL1F: _tmp72D=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F5)->f1)->aggr_info;_tmp72C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F5)->f3;_LL20: {
# 4064
struct Cyc_Absyn_Aggrdecl*_tmp70E=Cyc_Absyn_get_known_aggrdecl(_tmp72D);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp72C != 0;_tmp72C=_tmp72C->tl){
struct _tuple33*_tmp70F=(struct _tuple33*)_tmp72C->hd;
struct Cyc_Absyn_Pat*_tmp710=(*_tmp70F).f2;
if(_tmp710->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp711=(void*)((struct Cyc_List_List*)_check_null((*_tmp70F).f1))->hd;void*_tmp712=_tmp711;struct _dyneither_ptr*_tmp715;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp712)->tag == 1U){_LL2A: _tmp715=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp712)->f1;_LL2B:
 f=_tmp715;goto _LL29;}else{_LL2C: _LL2D:
({void*_tmp713=0U;({struct _dyneither_ptr _tmpCDC=({const char*_tmp714="multiple designators in pattern!";_tag_dyneither(_tmp714,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCDC,_tag_dyneither(_tmp713,sizeof(void*),0U));});});}_LL29:;}{
# 4076
void*_tmp716=(void*)_check_null(_tmp710->topt);
void*_tmp717=Cyc_Toc_typ_to_c_array(_tmp716);
struct Cyc_Absyn_Exp*_tmp718=Cyc_Toc_member_exp(path,f,0U);
# 4080
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp70E->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpCDD=Cyc_Toc_member_exp(_tmp718,Cyc_Toc_val_sp,0U);_tmp718=_tmpCDD;});{
void*_tmp719=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp70E->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp719))
({struct Cyc_Absyn_Exp*_tmpCDE=Cyc_Toc_cast_it(_tmp717,_tmp718);_tmp718=_tmpCDE;});else{
if(!Cyc_Toc_is_array_type(_tmp719) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp719)))
# 4087
({struct Cyc_Absyn_Exp*_tmpCE0=Cyc_Absyn_deref_exp(({void*_tmpCDF=Cyc_Absyn_cstar_typ(_tmp717,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpCDF,
Cyc_Absyn_address_exp(_tmp718,0U));}),0U);
# 4087
_tmp718=_tmpCE0;});}
# 4090
({struct Cyc_Absyn_Stmt*_tmpCE2=({struct Cyc_Absyn_Stmt*_tmpCE1=s;Cyc_Toc_seq_stmt_opt(_tmpCE1,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp718,_tmp710));});s=_tmpCE2;});};};};}
# 4092
return s;}}case 15U: _LL23: _LL24:
# 4097
({void*_tmp71A=0U;({struct _dyneither_ptr _tmpCE3=({const char*_tmp71B="unknownid pat";_tag_dyneither(_tmp71B,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCE3,_tag_dyneither(_tmp71A,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp71C=0U;({struct _dyneither_ptr _tmpCE4=({const char*_tmp71D="unknowncall pat";_tag_dyneither(_tmp71D,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCE4,_tag_dyneither(_tmp71C,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp71E=0U;({struct _dyneither_ptr _tmpCE5=({const char*_tmp71F="exp pat";_tag_dyneither(_tmp71F,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCE5,_tag_dyneither(_tmp71E,sizeof(void*),0U));});});}_LL0:;}struct _tuple34{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4108
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4116
void*_tmp72E=dopt;struct Cyc_List_List*_tmp76D;struct Cyc_List_List*_tmp76C;void*_tmp76B;struct Cyc_Tcpat_Rhs*_tmp76A;if(_tmp72E == 0){_LL1: _LL2:
# 4118
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp72E)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp76A=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp72E)->f1;_LL6:
# 4123
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp72F=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp730=_tmp72F;int*_tmp737;struct _dyneither_ptr*_tmp736;struct _dyneither_ptr*_tmp735;struct Cyc_Absyn_Switch_clause*_tmp734;_LLA: _tmp737=(int*)& _tmp730->f1;_tmp736=_tmp730->f2;_tmp735=_tmp730->f3;_tmp734=_tmp730->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp731=_tmp734->body;
if(_tmp731 == _tmp76A->rhs){
# 4128
if(*_tmp737)
return Cyc_Absyn_goto_stmt(_tmp736,0U);
*_tmp737=1;{
# 4132
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpCE8=rgn;struct Cyc_List_List**_tmpCE7=decls;struct Cyc_Absyn_Exp*_tmpCE6=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpCE8,& nv,_tmpCE7,_tmpCE6,_tmp734->pattern);});
# 4135
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp735,_tmp734->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpCE9=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpCE9;});
({struct Cyc_Absyn_Stmt*_tmpCEA=Cyc_Absyn_label_stmt(_tmp736,res,0U);res=_tmpCEA;});
({struct Cyc_List_List*_tmpCEC=({struct Cyc_List_List*_tmp732=_region_malloc(rgn,sizeof(*_tmp732));({struct _tuple34*_tmpCEB=({struct _tuple34*_tmp733=_region_malloc(rgn,sizeof(*_tmp733));_tmp733->f1=nv;_tmp733->f2=_tmp735;_tmp733->f3=_tmp731;_tmp733;});_tmp732->hd=_tmpCEB;});_tmp732->tl=*bodies;_tmp732;});*bodies=_tmpCEC;});
return res;};}};}
# 4143
({void*_tmp738=0U;({struct _dyneither_ptr _tmpCED=({const char*_tmp739="couldn't find rhs!";_tag_dyneither(_tmp739,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCED,_tag_dyneither(_tmp738,sizeof(void*),0U));});});default: _LL7: _tmp76D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp72E)->f1;_tmp76C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp72E)->f2;_tmp76B=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp72E)->f3;_LL8: {
# 4146
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp76B,lscs,v);
# 4148
struct Cyc_Absyn_Exp*_tmp73A=({struct Cyc_List_List*_tmpCEE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp76D);Cyc_Toc_compile_path(_tmpCEE,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp73B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp76C);
# 4151
struct _tuple32 _tmp73C=Cyc_Toc_admits_switch(_tmp73B);struct _tuple32 _tmp73D=_tmp73C;int _tmp769;void*_tmp768;_LLD: _tmp769=_tmp73D.f1;_tmp768=_tmp73D.f2;_LLE:;
if(_tmp769 > 1){
# 4155
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp74E=_cycalloc(sizeof(*_tmp74E));({struct Cyc_Absyn_Switch_clause*_tmpCF0=({struct Cyc_Absyn_Switch_clause*_tmp74F=_cycalloc(sizeof(*_tmp74F));({struct Cyc_Absyn_Pat*_tmpCEF=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp74F->pattern=_tmpCEF;});_tmp74F->pat_vars=0;_tmp74F->where_clause=0;_tmp74F->body=res;_tmp74F->loc=0U;_tmp74F;});_tmp74E->hd=_tmpCF0;});_tmp74E->tl=0;_tmp74E;});
# 4158
for(0;_tmp73B != 0;_tmp73B=_tmp73B->tl){
struct _tuple29 _tmp73E=*((struct _tuple29*)_tmp73B->hd);struct _tuple29 _tmp73F=_tmp73E;void*_tmp745;void*_tmp744;_LL10: _tmp745=_tmp73F.f1;_tmp744=_tmp73F.f2;_LL11:;{
# 4161
struct Cyc_Absyn_Pat*_tmp740=Cyc_Toc_compile_pat_test_as_case(_tmp745);
# 4163
struct Cyc_Absyn_Stmt*_tmp741=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp744,lscs,v);
# 4165
({struct Cyc_List_List*_tmpCF2=({struct Cyc_List_List*_tmp742=_cycalloc(sizeof(*_tmp742));({struct Cyc_Absyn_Switch_clause*_tmpCF1=({struct Cyc_Absyn_Switch_clause*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743->pattern=_tmp740;_tmp743->pat_vars=0;_tmp743->where_clause=0;_tmp743->body=_tmp741;_tmp743->loc=0U;_tmp743;});_tmp742->hd=_tmpCF1;});_tmp742->tl=new_lscs;_tmp742;});new_lscs=_tmpCF2;});};}
# 4167
{void*_tmp746=_tmp768;struct _dyneither_ptr*_tmp74B;switch(*((int*)_tmp746)){case 1U: _LL13: _LL14:
# 4170
 LOOP1: {
void*_tmp747=_tmp73A->r;void*_tmp748=_tmp747;struct Cyc_Absyn_Exp*_tmp74A;struct Cyc_Absyn_Exp*_tmp749;switch(*((int*)_tmp748)){case 14U: _LL1C: _tmp749=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp748)->f2;_LL1D:
 _tmp73A=_tmp749;goto LOOP1;case 20U: _LL1E: _tmp74A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp748)->f1;_LL1F:
 _tmp73A=_tmp74A;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 4176
({struct Cyc_Absyn_Exp*_tmpCF4=Cyc_Absyn_deref_exp(({void*_tmpCF3=Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpCF3,_tmp73A);}),0U);_tmp73A=_tmpCF4;});goto _LL12;case 3U: _LL15: _tmp74B=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp746)->f1;_LL16:
# 4179
({struct Cyc_Absyn_Exp*_tmpCF6=({struct Cyc_Absyn_Exp*_tmpCF5=Cyc_Toc_member_exp(_tmp73A,_tmp74B,0U);Cyc_Toc_member_exp(_tmpCF5,Cyc_Toc_tag_sp,0U);});_tmp73A=_tmpCF6;});goto _LL12;case 2U: _LL17: _LL18:
# 4181
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 4184
({struct Cyc_Absyn_Stmt*_tmpCF8=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp74C=_cycalloc(sizeof(*_tmp74C));({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpCF7=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp74D;_tmp74D.tag=10U;_tmp74D.f1=_tmp73A;_tmp74D.f2=new_lscs;_tmp74D.f3=0;_tmp74D;});_tmp74C[0]=_tmpCF7;});_tmp74C;}),0U);res=_tmpCF8;});}else{
# 4188
void*_tmp750=_tmp768;struct Cyc_Absyn_Exp*_tmp767;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp750)->tag == 2U){_LL23: _tmp767=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp750)->f1;_LL24: {
# 4190
struct Cyc_List_List*_tmp751=_tmp73B;void*_tmp760;struct Cyc_Tcpat_Rhs*_tmp75F;if(_tmp751 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp751)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp751)->tl == 0){_LL28: _tmp760=((struct _tuple29*)_tmp751->hd)->f1;_tmp75F=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp751->hd)->f2)->f1;_LL29:
# 4196
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp752=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp753=_tmp752;int*_tmp75A;struct _dyneither_ptr*_tmp759;struct _dyneither_ptr*_tmp758;struct Cyc_Absyn_Switch_clause*_tmp757;_LL2D: _tmp75A=(int*)& _tmp753->f1;_tmp759=_tmp753->f2;_tmp758=_tmp753->f3;_tmp757=_tmp753->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp754=_tmp757->body;
if(_tmp754 == _tmp75F->rhs){
# 4201
if(*_tmp75A)
return Cyc_Absyn_goto_stmt(_tmp759,0U);
*_tmp75A=1;{
# 4205
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpCFB=rgn;struct Cyc_List_List**_tmpCFA=decls;struct Cyc_Absyn_Exp*_tmpCF9=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpCFB,& nv,_tmpCFA,_tmpCF9,_tmp757->pattern);});
# 4208
Cyc_Toc_exp_to_c(nv,_tmp767);{
# 4211
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp758,_tmp757->body,0U);
({struct Cyc_Absyn_Stmt*_tmpCFC=Cyc_Absyn_ifthenelse_stmt(_tmp767,r,res,0U);r=_tmpCFC;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpCFD=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpCFD;});
({struct Cyc_Absyn_Stmt*_tmpCFE=Cyc_Absyn_label_stmt(_tmp759,r,0U);r=_tmpCFE;});
({struct Cyc_List_List*_tmpD00=({struct Cyc_List_List*_tmp755=_region_malloc(rgn,sizeof(*_tmp755));({struct _tuple34*_tmpCFF=({struct _tuple34*_tmp756=_region_malloc(rgn,sizeof(*_tmp756));_tmp756->f1=nv;_tmp756->f2=_tmp758;_tmp756->f3=_tmp754;_tmp756;});_tmp755->hd=_tmpCFF;});_tmp755->tl=*bodies;_tmp755;});*bodies=_tmpD00;});
return r;};};}};}
# 4220
({void*_tmp75B=0U;({struct _dyneither_ptr _tmpD01=({const char*_tmp75C="couldn't find rhs!";_tag_dyneither(_tmp75C,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpD01,_tag_dyneither(_tmp75B,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp75D=0U;({struct _dyneither_ptr _tmpD02=({const char*_tmp75E="bad where clause in match compiler";_tag_dyneither(_tmp75E,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpD02,_tag_dyneither(_tmp75D,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 4225
 for(0;_tmp73B != 0;_tmp73B=_tmp73B->tl){
struct _tuple29 _tmp761=*((struct _tuple29*)_tmp73B->hd);struct _tuple29 _tmp762=_tmp761;void*_tmp766;void*_tmp765;_LL30: _tmp766=_tmp762.f1;_tmp765=_tmp762.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp763=Cyc_Toc_compile_pat_test(_tmp73A,_tmp766);
struct Cyc_Absyn_Stmt*_tmp764=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp765,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpD03=Cyc_Absyn_ifthenelse_stmt(_tmp763,_tmp764,res,0U);res=_tmpD03;});};}}_LL22:;}
# 4233
return res;}}}_LL0:;}
# 4243
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4245
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple34*_tmp76E=(struct _tuple34*)bodies->hd;struct _tuple34*_tmp76F=_tmp76E;struct Cyc_Toc_Env**_tmp771;struct Cyc_Absyn_Stmt*_tmp770;_LL1: _tmp771=(struct Cyc_Toc_Env**)& _tmp76F->f1;_tmp770=_tmp76F->f3;_LL2:;
if(_tmp770 == s)return _tmp771;}
# 4251
return 0;}
# 4255
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4258
void*_tmp772=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4261
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp773=_new_region("rgn");struct _RegionHandle*rgn=& _tmp773;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp774=Cyc_Toc_share_env(rgn,nv);
# 4268
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4273
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp774,& mydecls,& mybodies,dopt,lscs,v);
# 4281
{struct Cyc_List_List*_tmp775=lscs;for(0;_tmp775 != 0;_tmp775=_tmp775->tl){
struct _tuple31*_tmp776=(struct _tuple31*)_tmp775->hd;struct _tuple31*_tmp777=_tmp776;struct _dyneither_ptr*_tmp77F;struct Cyc_Absyn_Switch_clause*_tmp77E;_LL1: _tmp77F=_tmp777->f3;_tmp77E=_tmp777->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp77E->body;
# 4285
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4288
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp778=*envp;
# 4291
if(_tmp775->tl != 0){
struct _tuple31*_tmp779=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp775->tl))->hd;struct _tuple31*_tmp77A=_tmp779;struct _dyneither_ptr*_tmp77D;struct Cyc_Absyn_Switch_clause*_tmp77C;_LL4: _tmp77D=_tmp77A->f3;_tmp77C=_tmp77A->f4;_LL5:;{
# 4295
struct Cyc_Toc_Env**_tmp77B=Cyc_Toc_find_case_env(mybodies,_tmp77C->body);
# 4300
if(_tmp77B == 0)
({struct Cyc_Toc_Env*_tmpD04=Cyc_Toc_last_switchclause_env(rgn,_tmp778,end_l);Cyc_Toc_stmt_to_c(_tmpD04,s);});else{
# 4304
struct Cyc_List_List*vs=0;
if(_tmp77C->pat_vars != 0){
({struct Cyc_List_List*_tmpD05=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp77C->pat_vars))->v)).f1);vs=_tmpD05;});
({struct Cyc_List_List*_tmpD06=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpD06;});}
# 4309
({struct Cyc_Toc_Env*_tmpD07=Cyc_Toc_non_last_switchclause_env(rgn,_tmp778,end_l,_tmp77D,vs);Cyc_Toc_stmt_to_c(_tmpD07,s);});}};}else{
# 4313
({struct Cyc_Toc_Env*_tmpD08=Cyc_Toc_last_switchclause_env(rgn,_tmp778,end_l);Cyc_Toc_stmt_to_c(_tmpD08,s);});}};};}}{
# 4317
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpD0A=test_tree;Cyc_Absyn_seq_stmt(_tmpD0A,({struct _dyneither_ptr*_tmpD09=end_l;Cyc_Absyn_label_stmt(_tmpD09,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 4319
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp780=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpD0B=Cyc_Toc_temp_var();_tmp780->name=_tmpD0B;});
({void*_tmpD0C=Cyc_Toc_typ_to_c_array(_tmp780->type);_tmp780->type=_tmpD0C;});
({struct Cyc_Absyn_Stmt*_tmpD0F=({struct Cyc_Absyn_Decl*_tmpD0E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp781=_cycalloc(sizeof(*_tmp781));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD0D=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp782;_tmp782.tag=0U;_tmp782.f1=_tmp780;_tmp782;});_tmp781[0]=_tmpD0D;});_tmp781;}),0U);Cyc_Absyn_decl_stmt(_tmpD0E,res,0U);});res=_tmpD0F;});}
# 4326
({void*_tmpD13=({struct _tuple1*_tmpD12=v;void*_tmpD11=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpD10=e;Cyc_Absyn_declare_stmt(_tmpD12,_tmpD11,_tmpD10,res,0U);})->r;whole_s->r=_tmpD13;});
_npop_handler(0U);return;};}
# 4265
;_pop_region(rgn);};}
# 4332
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 4334
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4339
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4341
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpD15=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpD15,({struct Cyc_List_List*_tmp783=_cycalloc(sizeof(*_tmp783));({struct Cyc_Absyn_Exp*_tmpD14=
Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp783->hd=_tmpD14;});_tmp783->tl=0;_tmp783;}),0U);}),0U);}
# 4345
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpD17=({struct Cyc_Absyn_Stmt*_tmpD16=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpD16,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpD17;});}
# 4350
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4352
while(1){
void*_tmp784=s->r;void*_tmp785=_tmp784;struct Cyc_Absyn_Stmt*_tmp7E0;struct Cyc_List_List*_tmp7DF;void*_tmp7DE;struct Cyc_Absyn_Stmt*_tmp7DD;struct Cyc_Absyn_Exp*_tmp7DC;struct _dyneither_ptr*_tmp7DB;struct Cyc_Absyn_Stmt*_tmp7DA;struct Cyc_Absyn_Decl*_tmp7D9;struct Cyc_Absyn_Stmt*_tmp7D8;struct Cyc_List_List*_tmp7D7;struct Cyc_Absyn_Switch_clause**_tmp7D6;struct Cyc_Absyn_Exp*_tmp7D5;struct Cyc_List_List*_tmp7D4;void*_tmp7D3;struct Cyc_Absyn_Exp*_tmp7D2;struct Cyc_Absyn_Exp*_tmp7D1;struct Cyc_Absyn_Exp*_tmp7D0;struct Cyc_Absyn_Stmt*_tmp7CF;struct Cyc_Absyn_Exp*_tmp7CE;struct Cyc_Absyn_Stmt*_tmp7CD;struct Cyc_Absyn_Exp*_tmp7CC;struct Cyc_Absyn_Stmt*_tmp7CB;struct Cyc_Absyn_Stmt*_tmp7CA;struct Cyc_Absyn_Exp*_tmp7C9;struct Cyc_Absyn_Stmt*_tmp7C8;struct Cyc_Absyn_Stmt*_tmp7C7;struct Cyc_Absyn_Exp*_tmp7C6;switch(*((int*)_tmp785)){case 0U: _LL1: _LL2:
# 4355
 return;case 1U: _LL3: _tmp7C6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_LL4:
# 4357
 Cyc_Toc_exp_to_c(nv,_tmp7C6);
return;case 2U: _LL5: _tmp7C8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_tmp7C7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp785)->f2;_LL6:
# 4360
 Cyc_Toc_stmt_to_c(nv,_tmp7C8);
s=_tmp7C7;
continue;case 3U: _LL7: _tmp7C9=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_LL8: {
# 4364
void*topt=0;
if(_tmp7C9 != 0){
({void*_tmpD18=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7C9->topt));topt=_tmpD18;});
Cyc_Toc_exp_to_c(nv,_tmp7C9);}{
# 4370
int _tmp786=Cyc_Toc_get_npop(s);
if(_tmp786 > 0){
if(topt != 0){
struct _tuple1*_tmp787=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp788=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp787,0U),0U);
({void*_tmpD1D=({struct _tuple1*_tmpD1C=_tmp787;void*_tmpD1B=topt;struct Cyc_Absyn_Exp*_tmpD1A=_tmp7C9;Cyc_Absyn_declare_stmt(_tmpD1C,_tmpD1B,_tmpD1A,({
struct Cyc_Absyn_Stmt*_tmpD19=Cyc_Toc_make_npop_handler(_tmp786);Cyc_Absyn_seq_stmt(_tmpD19,_tmp788,0U);}),0U);})->r;
# 4375
s->r=_tmpD1D;});}else{
# 4379
Cyc_Toc_do_npop_before(_tmp786,s);}}
# 4381
return;};}case 4U: _LL9: _tmp7CC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_tmp7CB=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp785)->f2;_tmp7CA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp785)->f3;_LLA:
# 4383
 Cyc_Toc_exp_to_c(nv,_tmp7CC);
Cyc_Toc_stmt_to_c(nv,_tmp7CB);
s=_tmp7CA;
continue;case 5U: _LLB: _tmp7CE=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp785)->f1).f1;_tmp7CD=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp785)->f2;_LLC:
# 4388
 Cyc_Toc_exp_to_c(nv,_tmp7CE);{
struct _RegionHandle _tmp789=_new_region("temp");struct _RegionHandle*temp=& _tmp789;_push_region(temp);
({struct Cyc_Toc_Env*_tmpD1E=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpD1E,_tmp7CD);});
# 4392
_npop_handler(0U);return;
# 4389
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 4394
struct Cyc_Toc_Env*_tmp78A=nv;struct _dyneither_ptr**_tmp78B;_LL22: _tmp78B=_tmp78A->break_lab;_LL23:;
if(_tmp78B != 0)
({void*_tmpD1F=Cyc_Toc_goto_stmt_r(*_tmp78B);s->r=_tmpD1F;});
# 4398
({int _tmpD20=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpD20,s);});
return;}case 7U: _LLF: _LL10: {
# 4401
struct Cyc_Toc_Env*_tmp78C=nv;struct _dyneither_ptr**_tmp78D;_LL25: _tmp78D=_tmp78C->continue_lab;_LL26:;
if(_tmp78D != 0)
({void*_tmpD21=Cyc_Toc_goto_stmt_r(*_tmp78D);s->r=_tmpD21;});
goto _LL12;}case 8U: _LL11: _LL12:
# 4407
({int _tmpD22=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpD22,s);});
return;case 9U: _LL13: _tmp7D2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_tmp7D1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp785)->f2).f1;_tmp7D0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp785)->f3).f1;_tmp7CF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp785)->f4;_LL14:
# 4411
 Cyc_Toc_exp_to_c(nv,_tmp7D2);Cyc_Toc_exp_to_c(nv,_tmp7D1);Cyc_Toc_exp_to_c(nv,_tmp7D0);{
struct _RegionHandle _tmp78E=_new_region("temp");struct _RegionHandle*temp=& _tmp78E;_push_region(temp);
({struct Cyc_Toc_Env*_tmpD23=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpD23,_tmp7CF);});
# 4415
_npop_handler(0U);return;
# 4412
;_pop_region(temp);};case 10U: _LL15: _tmp7D5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_tmp7D4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp785)->f2;_tmp7D3=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp785)->f3;_LL16:
# 4417
 Cyc_Toc_xlate_switch(nv,s,_tmp7D5,_tmp7D4,_tmp7D3);
return;case 11U: _LL17: _tmp7D7=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_tmp7D6=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp785)->f2;_LL18: {
# 4420
struct Cyc_Toc_Env*_tmp78F=nv;struct Cyc_Toc_FallthruInfo*_tmp79A;_LL28: _tmp79A=_tmp78F->fallthru_info;_LL29:;
if(_tmp79A == 0)
({void*_tmp790=0U;({struct _dyneither_ptr _tmpD24=({const char*_tmp791="fallthru in unexpected place";_tag_dyneither(_tmp791,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpD24,_tag_dyneither(_tmp790,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp792=*_tmp79A;struct Cyc_Toc_FallthruInfo _tmp793=_tmp792;struct _dyneither_ptr*_tmp799;struct Cyc_List_List*_tmp798;_LL2B: _tmp799=_tmp793.label;_tmp798=_tmp793.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp799,0U);
# 4426
({int _tmpD25=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpD25,s2);});{
struct Cyc_List_List*_tmp794=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp798);
struct Cyc_List_List*_tmp795=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7D7);
for(0;_tmp794 != 0;(_tmp794=_tmp794->tl,_tmp795=_tmp795->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp795))->hd);
({struct Cyc_Absyn_Stmt*_tmpD29=({struct Cyc_Absyn_Stmt*_tmpD28=({struct Cyc_Absyn_Exp*_tmpD27=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp796=_cycalloc(sizeof(*_tmp796));({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmpD26=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp797;_tmp797.tag=5U;_tmp797.f1=(struct Cyc_Absyn_Vardecl*)_tmp794->hd;_tmp797;});_tmp796[0]=_tmpD26;});_tmp796;}),0U);Cyc_Absyn_assign_stmt(_tmpD27,(struct Cyc_Absyn_Exp*)_tmp795->hd,0U);});Cyc_Absyn_seq_stmt(_tmpD28,s2,0U);});s2=_tmpD29;});}
# 4434
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp7D9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_tmp7D8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp785)->f2;_LL1A:
# 4440
{void*_tmp79B=_tmp7D9->r;void*_tmp79C=_tmp79B;struct Cyc_Absyn_Tvar*_tmp7B9;struct Cyc_Absyn_Vardecl*_tmp7B8;struct Cyc_Absyn_Exp*_tmp7B7;struct Cyc_Absyn_Fndecl*_tmp7B6;struct Cyc_List_List*_tmp7B5;struct Cyc_Absyn_Pat*_tmp7B4;struct Cyc_Absyn_Exp*_tmp7B3;void*_tmp7B2;struct Cyc_Absyn_Vardecl*_tmp7B1;switch(*((int*)_tmp79C)){case 0U: _LL2E: _tmp7B1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp79C)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp7B1,_tmp7D8);goto _LL2D;case 2U: _LL30: _tmp7B4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79C)->f1;_tmp7B3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79C)->f3;_tmp7B2=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79C)->f4;_LL31:
# 4448
{void*_tmp79D=_tmp7B4->r;void*_tmp79E=_tmp79D;struct Cyc_Absyn_Vardecl*_tmp7A1;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp79E)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp79E)->f2)->r)->tag == 0U){_LL3B: _tmp7A1=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp79E)->f1;_LL3C:
# 4450
({struct _tuple1*_tmpD2A=Cyc_Toc_temp_var();_tmp7A1->name=_tmpD2A;});
_tmp7A1->initializer=_tmp7B3;
({void*_tmpD2C=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp79F=_cycalloc(sizeof(*_tmp79F));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD2B=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7A0;_tmp7A0.tag=0U;_tmp7A0.f1=_tmp7A1;_tmp7A0;});_tmp79F[0]=_tmpD2B;});_tmp79F;});_tmp7D9->r=_tmpD2C;});
Cyc_Toc_local_decl_to_c(nv,_tmp7A1,_tmp7D8);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 4459
({void*_tmpD2D=(Cyc_Toc_letdecl_to_c(nv,_tmp7B4,_tmp7B2,(void*)_check_null(_tmp7B3->topt),_tmp7B3,_tmp7D8))->r;s->r=_tmpD2D;});
goto _LL3A;}_LL3A:;}
# 4462
goto _LL2D;case 3U: _LL32: _tmp7B5=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp79C)->f1;_LL33: {
# 4466
struct Cyc_List_List*_tmp7A2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7B5);
if(_tmp7A2 == 0)
({void*_tmp7A3=0U;({struct _dyneither_ptr _tmpD2E=({const char*_tmp7A4="empty Letv_d";_tag_dyneither(_tmp7A4,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD2E,_tag_dyneither(_tmp7A3,sizeof(void*),0U));});});
({void*_tmpD30=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7A5=_cycalloc(sizeof(*_tmp7A5));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD2F=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7A6;_tmp7A6.tag=0U;_tmp7A6.f1=(struct Cyc_Absyn_Vardecl*)_tmp7A2->hd;_tmp7A6;});_tmp7A5[0]=_tmpD2F;});_tmp7A5;});_tmp7D9->r=_tmpD30;});
_tmp7A2=_tmp7A2->tl;
for(0;_tmp7A2 != 0;_tmp7A2=_tmp7A2->tl){
struct Cyc_Absyn_Decl*_tmp7A7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7A8=_cycalloc(sizeof(*_tmp7A8));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD31=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7A9;_tmp7A9.tag=0U;_tmp7A9.f1=(struct Cyc_Absyn_Vardecl*)_tmp7A2->hd;_tmp7A9;});_tmp7A8[0]=_tmpD31;});_tmp7A8;}),0U);
({void*_tmpD33=({struct Cyc_Absyn_Decl*_tmpD32=_tmp7A7;Cyc_Absyn_decl_stmt(_tmpD32,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpD33;});}
# 4475
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp7B6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp79C)->f1;_LL35:
# 4478
 Cyc_Toc_fndecl_to_c(nv,_tmp7B6,0);
Cyc_Toc_stmt_to_c(nv,_tmp7D8);
goto _LL2D;case 4U: _LL36: _tmp7B9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79C)->f1;_tmp7B8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79C)->f2;_tmp7B7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79C)->f3;_LL37: {
# 4482
struct Cyc_Absyn_Stmt*_tmp7AA=_tmp7D8;
# 4490
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7B8->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 4497
Cyc_Toc_stmt_to_c(nv,_tmp7AA);
if(Cyc_Absyn_no_regions)
({void*_tmpD37=({struct _tuple1*_tmpD36=x_var;void*_tmpD35=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpD34=
Cyc_Absyn_uint_exp(0U,0U);
# 4499
Cyc_Absyn_declare_stmt(_tmpD36,_tmpD35,_tmpD34,_tmp7AA,0U);})->r;s->r=_tmpD37;});else{
# 4501
if((unsigned int)_tmp7B7){
Cyc_Toc_exp_to_c(nv,_tmp7B7);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpD38=_tmp7B7;Cyc_Absyn_aggrarrow_exp(_tmpD38,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2U),0U);}),0U);
({void*_tmpD39=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp7AA,0U))->r;s->r=_tmpD39;});};}else{
# 4506
({void*_tmpD46=({
struct _tuple1*_tmpD45=rh_var;void*_tmpD44=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpD43=({
struct Cyc_Absyn_Exp*_tmpD3B=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpD3B,({struct Cyc_List_List*_tmp7AC=_cycalloc(sizeof(*_tmp7AC));({struct Cyc_Absyn_Exp*_tmpD3A=
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp7AC->hd=_tmpD3A;});_tmp7AC->tl=0;_tmp7AC;}),0U);});
# 4507
Cyc_Absyn_declare_stmt(_tmpD45,_tmpD44,_tmpD43,({
# 4511
struct _tuple1*_tmpD42=x_var;void*_tmpD41=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpD40=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpD42,_tmpD41,_tmpD40,({
struct Cyc_Absyn_Stmt*_tmpD3F=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpD3C=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpD3C,({struct Cyc_Absyn_Exp*_tmp7AD[1U];_tmp7AD[0U]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7AD,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpD3F,({
struct Cyc_Absyn_Stmt*_tmpD3E=_tmp7AA;Cyc_Absyn_seq_stmt(_tmpD3E,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpD3D=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpD3D,({struct Cyc_Absyn_Exp*_tmp7AE[1U];_tmp7AE[0U]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7AE,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 4506
s->r=_tmpD46;});}}
# 4517
return;}default: _LL38: _LL39:
({void*_tmp7AF=0U;({struct _dyneither_ptr _tmpD47=({const char*_tmp7B0="bad nested declaration within function";_tag_dyneither(_tmp7B0,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD47,_tag_dyneither(_tmp7AF,sizeof(void*),0U));});});}_LL2D:;}
# 4520
return;case 13U: _LL1B: _tmp7DB=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_tmp7DA=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp785)->f2;_LL1C:
# 4522
 s=_tmp7DA;continue;case 14U: _LL1D: _tmp7DD=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_tmp7DC=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp785)->f2).f1;_LL1E: {
# 4524
struct _RegionHandle _tmp7BA=_new_region("temp");struct _RegionHandle*temp=& _tmp7BA;_push_region(temp);
({struct Cyc_Toc_Env*_tmpD48=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpD48,_tmp7DD);});
Cyc_Toc_exp_to_c(nv,_tmp7DC);
# 4528
_npop_handler(0U);return;
# 4524
;_pop_region(temp);}default: _LL1F: _tmp7E0=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp785)->f1;_tmp7DF=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp785)->f2;_tmp7DE=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp785)->f3;_LL20: {
# 4542 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4552
e_exp->topt=e_typ;{
struct _RegionHandle _tmp7BB=_new_region("temp");struct _RegionHandle*temp=& _tmp7BB;_push_region(temp);
# 4555
Cyc_Toc_stmt_to_c(nv,_tmp7E0);{
struct Cyc_Absyn_Stmt*_tmp7BC=({struct Cyc_Absyn_Stmt*_tmpD49=_tmp7E0;Cyc_Absyn_seq_stmt(_tmpD49,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 4560
struct Cyc_Absyn_Stmt*_tmp7BD=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7C4=_cycalloc(sizeof(*_tmp7C4));({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpD4A=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp7C5;_tmp7C5.tag=10U;_tmp7C5.f1=e_exp;_tmp7C5.f2=_tmp7DF;_tmp7C5.f3=_tmp7DE;_tmp7C5;});_tmp7C4[0]=_tmpD4A;});_tmp7C4;}),0U);
# 4562
Cyc_Toc_stmt_to_c(nv,_tmp7BD);{
# 4565
struct Cyc_Absyn_Exp*_tmp7BE=({
struct Cyc_Absyn_Exp*_tmpD4C=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpD4C,({struct Cyc_List_List*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));({struct Cyc_Absyn_Exp*_tmpD4B=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp7C3->hd=_tmpD4B;});_tmp7C3->tl=0;_tmp7C3;}),0U);});
# 4569
struct Cyc_Absyn_Stmt*_tmp7BF=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpD4E=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpD4E,({struct Cyc_List_List*_tmp7C2=_cycalloc(sizeof(*_tmp7C2));({struct Cyc_Absyn_Exp*_tmpD4D=
Cyc_Absyn_address_exp(h_exp,0U);_tmp7C2->hd=_tmpD4D;});_tmp7C2->tl=0;_tmp7C2;}),0U);}),0U);
# 4573
struct Cyc_Absyn_Exp*_tmp7C0=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp7C1=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpD5D=({
struct _tuple1*_tmpD5C=h_var;void*_tmpD5B=h_typ;Cyc_Absyn_declare_stmt(_tmpD5C,_tmpD5B,0,({
struct Cyc_Absyn_Stmt*_tmpD5A=_tmp7BF;Cyc_Absyn_seq_stmt(_tmpD5A,({
struct _tuple1*_tmpD59=was_thrown_var;void*_tmpD58=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpD57=_tmp7C0;Cyc_Absyn_declare_stmt(_tmpD59,_tmpD58,_tmpD57,({
struct Cyc_Absyn_Stmt*_tmpD56=({struct Cyc_Absyn_Exp*_tmpD50=_tmp7BE;struct Cyc_Absyn_Stmt*_tmpD4F=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp7C1,0U);
# 4579
Cyc_Absyn_ifthenelse_stmt(_tmpD50,_tmpD4F,
# 4581
Cyc_Toc_skip_stmt_dl(),0U);});
# 4579
Cyc_Absyn_seq_stmt(_tmpD56,({
# 4582
struct Cyc_Absyn_Exp*_tmpD55=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpD54=_tmp7BC;Cyc_Absyn_ifthenelse_stmt(_tmpD55,_tmpD54,({
# 4584
struct _tuple1*_tmpD53=e_var;void*_tmpD52=e_typ;struct Cyc_Absyn_Exp*_tmpD51=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 4584
Cyc_Absyn_declare_stmt(_tmpD53,_tmpD52,_tmpD51,_tmp7BD,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 4575
s->r=_tmpD5D;});};};
# 4588
_npop_handler(0U);return;
# 4553
;_pop_region(temp);};}}_LL0:;}}
# 4597
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpD5E=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpD5E;});{
struct _RegionHandle _tmp7E1=_new_region("frgn");struct _RegionHandle*frgn=& _tmp7E1;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp7E2=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp7E3=f->args;for(0;_tmp7E3 != 0;_tmp7E3=_tmp7E3->tl){
struct _tuple1*_tmp7E4=({struct _tuple1*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5->f1=Cyc_Absyn_Loc_n;_tmp7E5->f2=(*((struct _tuple9*)_tmp7E3->hd)).f1;_tmp7E5;});
({void*_tmpD5F=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp7E3->hd)).f3);(*((struct _tuple9*)_tmp7E3->hd)).f3=_tmpD5F;});}}
# 4613
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 4617
({struct Cyc_Hashtable_Table**_tmpD61=({struct Cyc_Hashtable_Table**_tmp7E6=_cycalloc(sizeof(*_tmp7E6));({struct Cyc_Hashtable_Table*_tmpD60=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);_tmp7E6[0]=_tmpD60;});_tmp7E6;});Cyc_Toc_fn_pop_table=_tmpD61;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp7E7=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp7E8=_tmp7E7;struct _dyneither_ptr*_tmp7F1;struct Cyc_Absyn_Tqual _tmp7F0;void*_tmp7EF;int _tmp7EE;_LL1: _tmp7F1=_tmp7E8.name;_tmp7F0=_tmp7E8.tq;_tmp7EF=_tmp7E8.type;_tmp7EE=_tmp7E8.inject;_LL2:;{
void*_tmp7E9=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp7EF,(void*)& Cyc_Absyn_HeapRgn_val,_tmp7F0,Cyc_Absyn_false_conref));
struct _tuple1*_tmp7EA=({struct _tuple1*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED->f1=Cyc_Absyn_Loc_n;_tmp7ED->f2=(struct _dyneither_ptr*)_check_null(_tmp7F1);_tmp7ED;});
({struct Cyc_List_List*_tmpD64=({struct Cyc_List_List*_tmpD63=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpD63,({struct Cyc_List_List*_tmp7EB=_cycalloc(sizeof(*_tmp7EB));({struct _tuple9*_tmpD62=({struct _tuple9*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC->f1=_tmp7F1;_tmp7EC->f2=_tmp7F0;_tmp7EC->f3=_tmp7E9;_tmp7EC;});_tmp7EB->hd=_tmpD62;});_tmp7EB->tl=0;_tmp7EB;}));});f->args=_tmpD64;});
f->cyc_varargs=0;};}
# 4626
{struct Cyc_List_List*_tmp7F2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp7F2 != 0;_tmp7F2=_tmp7F2->tl){
({void*_tmpD65=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp7F2->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp7F2->hd)->type=_tmpD65;});}}
# 4629
({struct Cyc_Toc_Env*_tmpD66=Cyc_Toc_clear_toplevel(frgn,_tmp7E2);Cyc_Toc_stmt_to_c(_tmpD66,f->body);});}
# 4606
;_pop_region(frgn);};}
# 4633
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp7F3=s;switch(_tmp7F3){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple35{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple36{struct Cyc_Toc_TocState*f1;struct _tuple35*f2;};
# 4650 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple36*env){
# 4653
struct _tuple36 _tmp7F4=*env;struct _tuple36 _tmp7F5=_tmp7F4;struct Cyc_Toc_TocState*_tmp834;struct Cyc_Absyn_Aggrdecl*_tmp833;int _tmp832;_LL1: _tmp834=_tmp7F5.f1;_tmp833=(_tmp7F5.f2)->f1;_tmp832=(_tmp7F5.f2)->f2;_LL2:;{
struct _tuple1*_tmp7F6=_tmp833->name;
struct Cyc_Toc_TocState _tmp7F7=*_tmp834;struct Cyc_Toc_TocState _tmp7F8=_tmp7F7;struct Cyc_Dict_Dict*_tmp831;_LL4: _tmp831=_tmp7F8.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple18**_tmp7F9=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp831,_tmp7F6);
if(_tmp7F9 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp833->kind == Cyc_Absyn_StructA)
({struct _tuple18*_tmpD68=({struct _tuple18*_tmp7FA=_region_malloc(d,sizeof(*_tmp7FA));_tmp7FA->f1=_tmp833;({void*_tmpD67=Cyc_Absyn_strctq(_tmp7F6);_tmp7FA->f2=_tmpD67;});_tmp7FA;});v=_tmpD68;});else{
# 4664
({struct _tuple18*_tmpD6A=({struct _tuple18*_tmp7FB=_region_malloc(d,sizeof(*_tmp7FB));_tmp7FB->f1=_tmp833;({void*_tmpD69=Cyc_Absyn_unionq_typ(_tmp7F6);_tmp7FB->f2=_tmpD69;});_tmp7FB;});v=_tmpD6A;});}
({struct Cyc_Dict_Dict _tmpD6B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp831,_tmp7F6,v);*_tmp831=_tmpD6B;});};}else{
# 4667
struct _tuple18*_tmp7FC=*_tmp7F9;struct _tuple18*_tmp7FD=_tmp7FC;struct Cyc_Absyn_Aggrdecl*_tmp800;void*_tmp7FF;_LL7: _tmp800=_tmp7FD->f1;_tmp7FF=_tmp7FD->f2;_LL8:;
if(_tmp800->impl == 0){
({struct Cyc_Dict_Dict _tmpD6E=({struct Cyc_Dict_Dict _tmpD6D=*_tmp831;struct _tuple1*_tmpD6C=_tmp7F6;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmpD6D,_tmpD6C,({struct _tuple18*_tmp7FE=_region_malloc(d,sizeof(*_tmp7FE));_tmp7FE->f1=_tmp833;_tmp7FE->f2=_tmp7FF;_tmp7FE;}));});*_tmp831=_tmpD6E;});
seen_defn_before=0;}else{
# 4672
seen_defn_before=1;}}{
# 4674
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830->kind=_tmp833->kind;_tmp830->sc=Cyc_Absyn_Public;_tmp830->name=_tmp833->name;_tmp830->tvs=0;_tmp830->impl=0;_tmp830->expected_mem_kind=0;_tmp830->attributes=_tmp833->attributes;_tmp830;});
# 4681
if(_tmp833->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpD6F=({struct Cyc_Absyn_AggrdeclImpl*_tmp801=_cycalloc(sizeof(*_tmp801));_tmp801->exist_vars=0;_tmp801->rgn_po=0;_tmp801->fields=0;_tmp801->tagged=0;_tmp801;});new_ad->impl=_tmpD6F;});{
# 4686
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp802=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp833->impl))->fields;for(0;_tmp802 != 0;_tmp802=_tmp802->tl){
# 4690
struct Cyc_Absyn_Aggrfield*_tmp803=(struct Cyc_Absyn_Aggrfield*)_tmp802->hd;
void*_tmp804=_tmp803->type;
struct Cyc_List_List*_tmp805=_tmp803->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp804)) && (
_tmp833->kind == Cyc_Absyn_StructA  && _tmp802->tl == 0  || _tmp833->kind == Cyc_Absyn_UnionA)){
# 4704 "toc.cyc"
void*_tmp806=Cyc_Tcutil_compress(_tmp804);void*_tmp807=_tmp806;void*_tmp814;struct Cyc_Absyn_Tqual _tmp813;union Cyc_Absyn_Constraint*_tmp812;unsigned int _tmp811;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp807)->tag == 8U){_LLA: _tmp814=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp807)->f1).elt_type;_tmp813=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp807)->f1).tq;_tmp812=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp807)->f1).zero_term;_tmp811=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp807)->f1).zt_loc;_LLB:
# 4707
({void*_tmpD73=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp808=_cycalloc(sizeof(*_tmp808));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD72=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp809;_tmp809.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmpD71=({struct Cyc_Absyn_ArrayInfo _tmp80A;_tmp80A.elt_type=_tmp814;_tmp80A.tq=_tmp813;({struct Cyc_Absyn_Exp*_tmpD70=Cyc_Absyn_uint_exp(0U,0U);_tmp80A.num_elts=_tmpD70;});_tmp80A.zero_term=_tmp812;_tmp80A.zt_loc=_tmp811;_tmp80A;});_tmp809.f1=_tmpD71;});_tmp809;});_tmp808[0]=_tmpD72;});_tmp808;});_tmp804=_tmpD73;});
goto _LL9;}else{_LLC: _LLD:
# 4710
({struct Cyc_List_List*_tmpD76=({struct Cyc_List_List*_tmp80B=_cycalloc(sizeof(*_tmp80B));({void*_tmpD75=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp80C=_cycalloc(sizeof(*_tmp80C));({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpD74=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp80D;_tmp80D.tag=6U;_tmp80D.f1=0;_tmp80D;});_tmp80C[0]=_tmpD74;});_tmp80C;});_tmp80B->hd=_tmpD75;});_tmp80B->tl=_tmp805;_tmp80B;});_tmp805=_tmpD76;});
({void*_tmpD7D=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp80E=_cycalloc(sizeof(*_tmp80E));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD7C=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp80F;_tmp80F.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmpD7B=({struct Cyc_Absyn_ArrayInfo _tmp810;({void*_tmpD77=Cyc_Absyn_void_star_typ();_tmp810.elt_type=_tmpD77;});({struct Cyc_Absyn_Tqual _tmpD78=
Cyc_Absyn_empty_tqual(0U);_tmp810.tq=_tmpD78;});({struct Cyc_Absyn_Exp*_tmpD79=
Cyc_Absyn_uint_exp(0U,0U);_tmp810.num_elts=_tmpD79;});({union Cyc_Absyn_Constraint*_tmpD7A=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp810.zero_term=_tmpD7A;});_tmp810.zt_loc=0U;_tmp810;});_tmp80F.f1=_tmpD7B;});_tmp80F;});_tmp80E[0]=_tmpD7C;});_tmp80E;});
# 4711
_tmp804=_tmpD7D;});}_LL9:;}{
# 4717
struct Cyc_Absyn_Aggrfield*_tmp815=({struct Cyc_Absyn_Aggrfield*_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B->name=_tmp803->name;_tmp82B->tq=Cyc_Toc_mt_tq;({void*_tmpD7E=
# 4719
Cyc_Toc_typ_to_c(_tmp804);_tmp82B->type=_tmpD7E;});_tmp82B->width=_tmp803->width;_tmp82B->attributes=_tmp805;_tmp82B->requires_clause=0;_tmp82B;});
# 4727
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp833->impl))->tagged){
void*_tmp816=_tmp815->type;
struct _dyneither_ptr*_tmp817=_tmp815->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp829;_tmp829.tag=0U;_tmp829.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp817);({struct Cyc_String_pa_PrintArg_struct _tmp828;_tmp828.tag=0U;_tmp828.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp833->name).f2);({void*_tmp826[2U]={& _tmp828,& _tmp829};({struct _dyneither_ptr _tmpD7F=({const char*_tmp827="_union_%s_%s";_tag_dyneither(_tmp827,sizeof(char),13U);});Cyc_aprintf(_tmpD7F,_tag_dyneither(_tmp826,sizeof(void*),2U));});});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825[0]=s;_tmp825;});
struct Cyc_Absyn_Aggrfield*_tmp818=({struct Cyc_Absyn_Aggrfield*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824->name=Cyc_Toc_val_sp;_tmp824->tq=Cyc_Toc_mt_tq;_tmp824->type=_tmp816;_tmp824->width=0;_tmp824->attributes=0;_tmp824->requires_clause=0;_tmp824;});
struct Cyc_Absyn_Aggrfield*_tmp819=({struct Cyc_Absyn_Aggrfield*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823->name=Cyc_Toc_tag_sp;_tmp823->tq=Cyc_Toc_mt_tq;_tmp823->type=Cyc_Absyn_sint_typ;_tmp823->width=0;_tmp823->attributes=0;_tmp823->requires_clause=0;_tmp823;});
struct Cyc_List_List*_tmp81A=({struct Cyc_Absyn_Aggrfield*_tmp822[2U];_tmp822[1U]=_tmp818;_tmp822[0U]=_tmp819;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp822,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp81B=({struct Cyc_Absyn_Aggrdecl*_tmp81F=_cycalloc(sizeof(*_tmp81F));_tmp81F->kind=Cyc_Absyn_StructA;_tmp81F->sc=Cyc_Absyn_Public;({struct _tuple1*_tmpD80=({struct _tuple1*_tmp821=_cycalloc(sizeof(*_tmp821));_tmp821->f1=Cyc_Absyn_Loc_n;_tmp821->f2=str;_tmp821;});_tmp81F->name=_tmpD80;});_tmp81F->tvs=0;({struct Cyc_Absyn_AggrdeclImpl*_tmpD81=({struct Cyc_Absyn_AggrdeclImpl*_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820->exist_vars=0;_tmp820->rgn_po=0;_tmp820->fields=_tmp81A;_tmp820->tagged=0;_tmp820;});_tmp81F->impl=_tmpD81;});_tmp81F->expected_mem_kind=0;_tmp81F->attributes=0;_tmp81F;});
# 4740
({struct Cyc_List_List*_tmpD84=({struct Cyc_List_List*_tmp81C=_cycalloc(sizeof(*_tmp81C));({struct Cyc_Absyn_Decl*_tmpD83=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp81D=_cycalloc(sizeof(*_tmp81D));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD82=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp81E;_tmp81E.tag=5U;_tmp81E.f1=_tmp81B;_tmp81E;});_tmp81D[0]=_tmpD82;});_tmp81D;}),0U);_tmp81C->hd=_tmpD83;});_tmp81C->tl=Cyc_Toc_result_decls;_tmp81C;});Cyc_Toc_result_decls=_tmpD84;});
({void*_tmpD85=Cyc_Absyn_strct(str);_tmp815->type=_tmpD85;});}
# 4743
({struct Cyc_List_List*_tmpD86=({struct Cyc_List_List*_tmp82A=_cycalloc(sizeof(*_tmp82A));_tmp82A->hd=_tmp815;_tmp82A->tl=new_fields;_tmp82A;});new_fields=_tmpD86;});};}}
# 4745
({struct Cyc_List_List*_tmpD87=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpD87;});};}
# 4747
if(_tmp832)
({struct Cyc_List_List*_tmpD8B=({struct Cyc_List_List*_tmp82C=_cycalloc(sizeof(*_tmp82C));({struct Cyc_Absyn_Decl*_tmpD8A=({struct Cyc_Absyn_Decl*_tmp82D=_cycalloc(sizeof(*_tmp82D));({void*_tmpD89=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp82E=_cycalloc(sizeof(*_tmp82E));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD88=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp82F;_tmp82F.tag=5U;_tmp82F.f1=new_ad;_tmp82F;});_tmp82E[0]=_tmpD88;});_tmp82E;});_tmp82D->r=_tmpD89;});_tmp82D->loc=0U;_tmp82D;});_tmp82C->hd=_tmpD8A;});_tmp82C->tl=Cyc_Toc_result_decls;_tmp82C;});Cyc_Toc_result_decls=_tmpD8B;});
return 0;};};};}
# 4752
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple35 p=({struct _tuple35 _tmp835;_tmp835.f1=ad;_tmp835.f2=add_to_decls;_tmp835;});
((int(*)(struct _tuple35*arg,int(*f)(struct _RegionHandle*,struct _tuple36*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple37{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4781 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4784
struct _tuple37 _tmp836=*env;struct _tuple37 _tmp837=_tmp836;struct Cyc_Set_Set**_tmp849;struct Cyc_Absyn_Datatypedecl*_tmp848;_LL1: _tmp849=(_tmp837.f1)->datatypes_so_far;_tmp848=_tmp837.f2;_LL2:;{
struct _tuple1*_tmp838=_tmp848->name;
if(_tmp848->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp849,_tmp838))
return 0;
({struct Cyc_Set_Set*_tmpD8C=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp849,_tmp838);*_tmp849=_tmpD8C;});
{struct Cyc_List_List*_tmp839=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp848->fields))->v;for(0;_tmp839 != 0;_tmp839=_tmp839->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp839->hd;
# 4792
struct Cyc_List_List*_tmp83A=0;
int i=1;
{struct Cyc_List_List*_tmp83B=f->typs;for(0;_tmp83B != 0;(_tmp83B=_tmp83B->tl,i ++)){
struct _dyneither_ptr*_tmp83C=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp83D=({struct Cyc_Absyn_Aggrfield*_tmp83F=_cycalloc(sizeof(*_tmp83F));_tmp83F->name=_tmp83C;_tmp83F->tq=(*((struct _tuple11*)_tmp83B->hd)).f1;({void*_tmpD8D=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp83B->hd)).f2);_tmp83F->type=_tmpD8D;});_tmp83F->width=0;_tmp83F->attributes=0;_tmp83F->requires_clause=0;_tmp83F;});
({struct Cyc_List_List*_tmpD8E=({struct Cyc_List_List*_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E->hd=_tmp83D;_tmp83E->tl=_tmp83A;_tmp83E;});_tmp83A=_tmpD8E;});}}
# 4800
({struct Cyc_List_List*_tmpD91=({struct Cyc_List_List*_tmp840=_cycalloc(sizeof(*_tmp840));({struct Cyc_Absyn_Aggrfield*_tmpD8F=({struct Cyc_Absyn_Aggrfield*_tmp841=_cycalloc(sizeof(*_tmp841));_tmp841->name=Cyc_Toc_tag_sp;_tmp841->tq=Cyc_Toc_mt_tq;_tmp841->type=Cyc_Absyn_sint_typ;_tmp841->width=0;_tmp841->attributes=0;_tmp841->requires_clause=0;_tmp841;});_tmp840->hd=_tmpD8F;});({struct Cyc_List_List*_tmpD90=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp83A);_tmp840->tl=_tmpD90;});_tmp840;});
# 4800
_tmp83A=_tmpD91;});{
# 4802
struct Cyc_Absyn_Aggrdecl*_tmp842=({struct Cyc_Absyn_Aggrdecl*_tmp846=_cycalloc(sizeof(*_tmp846));_tmp846->kind=Cyc_Absyn_StructA;_tmp846->sc=Cyc_Absyn_Public;({struct _tuple1*_tmpD92=
# 4804
Cyc_Toc_collapse_qvars(f->name,_tmp848->name);_tmp846->name=_tmpD92;});_tmp846->tvs=0;_tmp846->expected_mem_kind=0;({struct Cyc_Absyn_AggrdeclImpl*_tmpD93=({struct Cyc_Absyn_AggrdeclImpl*_tmp847=_cycalloc(sizeof(*_tmp847));_tmp847->exist_vars=0;_tmp847->rgn_po=0;_tmp847->fields=_tmp83A;_tmp847->tagged=0;_tmp847;});_tmp846->impl=_tmpD93;});_tmp846->attributes=0;_tmp846;});
# 4808
({struct Cyc_List_List*_tmpD96=({struct Cyc_List_List*_tmp843=_cycalloc(sizeof(*_tmp843));({struct Cyc_Absyn_Decl*_tmpD95=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp844=_cycalloc(sizeof(*_tmp844));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD94=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp845;_tmp845.tag=5U;_tmp845.f1=_tmp842;_tmp845;});_tmp844[0]=_tmpD94;});_tmp844;}),0U);_tmp843->hd=_tmpD95;});_tmp843->tl=Cyc_Toc_result_decls;_tmp843;});Cyc_Toc_result_decls=_tmpD96;});};}}
# 4810
return 0;};}
# 4813
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4832 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4835
struct _tuple37 _tmp84A=*env;struct _tuple37 _tmp84B=_tmp84A;struct Cyc_Toc_TocState*_tmp870;struct Cyc_Absyn_Datatypedecl*_tmp86F;_LL1: _tmp870=_tmp84B.f1;_tmp86F=_tmp84B.f2;_LL2:;
if(_tmp86F->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp84C=*_tmp870;struct Cyc_Toc_TocState _tmp84D=_tmp84C;struct Cyc_Dict_Dict*_tmp86E;_LL4: _tmp86E=_tmp84D.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp84E=_tmp86F->name;
{struct Cyc_List_List*_tmp84F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp86F->fields))->v;for(0;_tmp84F != 0;_tmp84F=_tmp84F->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp84F->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp850=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp851=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp850,Cyc_Absyn_false_conref,0U);
# 4846
int*_tmp852=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp86E,f->name);int*_tmp853=_tmp852;if(_tmp853 == 0){_LL7: _LL8: {
# 4848
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpD97=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpD97;});{
# 4852
struct Cyc_Absyn_Vardecl*_tmp854=Cyc_Absyn_new_vardecl(0U,f->name,_tmp851,initopt);
_tmp854->sc=f->sc;
({struct Cyc_List_List*_tmpD9A=({struct Cyc_List_List*_tmp855=_cycalloc(sizeof(*_tmp855));({struct Cyc_Absyn_Decl*_tmpD99=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp856=_cycalloc(sizeof(*_tmp856));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD98=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp857;_tmp857.tag=0U;_tmp857.f1=_tmp854;_tmp857;});_tmp856[0]=_tmpD98;});_tmp856;}),0U);_tmp855->hd=_tmpD99;});_tmp855->tl=Cyc_Toc_result_decls;_tmp855;});Cyc_Toc_result_decls=_tmpD9A;});
({struct Cyc_Dict_Dict _tmpD9B=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp86E,f->name,f->sc != Cyc_Absyn_Extern);
# 4855
*_tmp86E=_tmpD9B;});{
# 4857
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp858=f->typs;for(0;_tmp858 != 0;(_tmp858=_tmp858->tl,i ++)){
struct _dyneither_ptr*_tmp859=({struct _dyneither_ptr*_tmp85D=_cycalloc(sizeof(*_tmp85D));({struct _dyneither_ptr _tmpD9D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp860;_tmp860.tag=1U;_tmp860.f1=(unsigned long)i;({void*_tmp85E[1U]={& _tmp860};({struct _dyneither_ptr _tmpD9C=({const char*_tmp85F="f%d";_tag_dyneither(_tmp85F,sizeof(char),4U);});Cyc_aprintf(_tmpD9C,_tag_dyneither(_tmp85E,sizeof(void*),1U));});});});_tmp85D[0]=_tmpD9D;});_tmp85D;});
struct Cyc_Absyn_Aggrfield*_tmp85A=({struct Cyc_Absyn_Aggrfield*_tmp85C=_cycalloc(sizeof(*_tmp85C));_tmp85C->name=_tmp859;_tmp85C->tq=(*((struct _tuple11*)_tmp858->hd)).f1;({void*_tmpD9E=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp858->hd)).f2);_tmp85C->type=_tmpD9E;});_tmp85C->width=0;_tmp85C->attributes=0;_tmp85C->requires_clause=0;_tmp85C;});
({struct Cyc_List_List*_tmpD9F=({struct Cyc_List_List*_tmp85B=_cycalloc(sizeof(*_tmp85B));_tmp85B->hd=_tmp85A;_tmp85B->tl=fields;_tmp85B;});fields=_tmpD9F;});}}
# 4865
({struct Cyc_List_List*_tmpDA3=({struct Cyc_List_List*_tmp861=_cycalloc(sizeof(*_tmp861));({struct Cyc_Absyn_Aggrfield*_tmpDA1=({struct Cyc_Absyn_Aggrfield*_tmp862=_cycalloc(sizeof(*_tmp862));_tmp862->name=Cyc_Toc_tag_sp;_tmp862->tq=Cyc_Toc_mt_tq;({void*_tmpDA0=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp862->type=_tmpDA0;});_tmp862->width=0;_tmp862->attributes=0;_tmp862->requires_clause=0;_tmp862;});_tmp861->hd=_tmpDA1;});({struct Cyc_List_List*_tmpDA2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp861->tl=_tmpDA2;});_tmp861;});
# 4865
fields=_tmpDA3;});{
# 4868
struct Cyc_Absyn_Aggrdecl*_tmp863=({struct Cyc_Absyn_Aggrdecl*_tmp867=_cycalloc(sizeof(*_tmp867));_tmp867->kind=Cyc_Absyn_StructA;_tmp867->sc=Cyc_Absyn_Public;({struct _tuple1*_tmpDA4=
# 4870
Cyc_Toc_collapse_qvars(f->name,_tmp86F->name);_tmp867->name=_tmpDA4;});_tmp867->tvs=0;_tmp867->expected_mem_kind=0;({struct Cyc_Absyn_AggrdeclImpl*_tmpDA5=({struct Cyc_Absyn_AggrdeclImpl*_tmp868=_cycalloc(sizeof(*_tmp868));_tmp868->exist_vars=0;_tmp868->rgn_po=0;_tmp868->fields=fields;_tmp868->tagged=0;_tmp868;});_tmp867->impl=_tmpDA5;});_tmp867->attributes=0;_tmp867;});
# 4874
({struct Cyc_List_List*_tmpDA8=({struct Cyc_List_List*_tmp864=_cycalloc(sizeof(*_tmp864));({struct Cyc_Absyn_Decl*_tmpDA7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp865=_cycalloc(sizeof(*_tmp865));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDA6=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp866;_tmp866.tag=5U;_tmp866.f1=_tmp863;_tmp866;});_tmp865[0]=_tmpDA6;});_tmp865;}),0U);_tmp864->hd=_tmpDA7;});_tmp864->tl=Cyc_Toc_result_decls;_tmp864;});Cyc_Toc_result_decls=_tmpDA8;});
# 4876
goto _LL6;};};};}}else{if(*((int*)_tmp853)== 0){_LL9: _LLA:
# 4878
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp869=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp86A=Cyc_Absyn_new_vardecl(0U,f->name,_tmp851,_tmp869);
_tmp86A->sc=f->sc;
({struct Cyc_List_List*_tmpDAB=({struct Cyc_List_List*_tmp86B=_cycalloc(sizeof(*_tmp86B));({struct Cyc_Absyn_Decl*_tmpDAA=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp86C=_cycalloc(sizeof(*_tmp86C));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDA9=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp86D;_tmp86D.tag=0U;_tmp86D.f1=_tmp86A;_tmp86D;});_tmp86C[0]=_tmpDA9;});_tmp86C;}),0U);_tmp86B->hd=_tmpDAA;});_tmp86B->tl=Cyc_Toc_result_decls;_tmp86B;});Cyc_Toc_result_decls=_tmpDAB;});
({struct Cyc_Dict_Dict _tmpDAC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp86E,f->name,1);*_tmp86E=_tmpDAC;});}
# 4885
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4889
return 0;};};}
# 4892
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4896
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4902
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpDAD=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpDAD;});
# 4906
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp871=init->r;void*_tmp872=_tmp871;struct Cyc_Absyn_Vardecl*_tmp878;struct Cyc_Absyn_Exp*_tmp877;struct Cyc_Absyn_Exp*_tmp876;int _tmp875;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp872)->tag == 27U){_LL1: _tmp878=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp872)->f1;_tmp877=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp872)->f2;_tmp876=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp872)->f3;_tmp875=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp872)->f4;_LL2:
# 4913
 vd->initializer=0;
({void*_tmpDB5=({struct Cyc_Toc_Env*_tmpDB4=nv;void*_tmpDB3=old_typ;struct Cyc_Absyn_Exp*_tmpDB2=Cyc_Absyn_var_exp(vd->name,0U);struct Cyc_Absyn_Vardecl*_tmpDB1=_tmp878;struct Cyc_Absyn_Exp*_tmpDB0=_tmp877;struct Cyc_Absyn_Exp*_tmpDAF=_tmp876;int _tmpDAE=_tmp875;Cyc_Toc_init_comprehension(_tmpDB4,_tmpDB3,_tmpDB2,_tmpDB1,_tmpDB0,_tmpDAF,_tmpDAE,
Cyc_Absyn_new_stmt(s->r,0U),0);})->r;
# 4914
s->r=_tmpDB5;});
# 4916
goto _LL0;}else{_LL3: _LL4:
# 4918
 if(vd->sc == Cyc_Absyn_Static){
# 4922
struct _RegionHandle _tmp873=_new_region("temp");struct _RegionHandle*temp=& _tmp873;_push_region(temp);
{struct Cyc_Toc_Env*_tmp874=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp874,init);}
# 4923
;_pop_region(temp);}else{
# 4927
Cyc_Toc_exp_to_c(nv,init);}
goto _LL0;}_LL0:;}else{
# 4932
void*_tmp879=Cyc_Tcutil_compress(old_typ);void*_tmp87A=_tmp879;void*_tmp881;struct Cyc_Absyn_Exp*_tmp880;union Cyc_Absyn_Constraint*_tmp87F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87A)->tag == 8U){_LL6: _tmp881=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87A)->f1).elt_type;_tmp880=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87A)->f1).num_elts;_tmp87F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87A)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp87F)){_LL7:
 if(_tmp880 == 0)
({void*_tmp87B=0U;({struct _dyneither_ptr _tmpDB6=({const char*_tmp87C="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp87C,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpDB6,_tag_dyneither(_tmp87B,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp880;
struct Cyc_Absyn_Exp*_tmp87D=({struct Cyc_Absyn_Exp*_tmpDB8=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpDB8,({
struct Cyc_Absyn_Exp*_tmpDB7=num_elts;Cyc_Absyn_add_exp(_tmpDB7,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4940
struct Cyc_Absyn_Exp*_tmp87E=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpDBA=({struct Cyc_Absyn_Stmt*_tmpDB9=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp87D,_tmp87E,0U),0U);Cyc_Toc_seq_stmt_r(_tmpDB9,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4941
s->r=_tmpDBA;});
# 4943
goto _LL5;};}else{goto _LL8;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}}
# 4951
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp882=d;struct Cyc_List_List*_tmp889;struct Cyc_List_List*_tmp888;void**_tmp887;struct Cyc_Tcpat_Rhs*_tmp886;switch(*((int*)_tmp882)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp886=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp882)->f1;_LL4:
 _tmp886->rhs=success;return d;default: _LL5: _tmp889=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp882)->f1;_tmp888=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp882)->f2;_tmp887=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp882)->f3;_LL6:
# 4956
({void*_tmpDBB=Cyc_Toc_rewrite_decision(*_tmp887,success);*_tmp887=_tmpDBB;});
for(0;_tmp888 != 0;_tmp888=_tmp888->tl){
struct _tuple29*_tmp883=(struct _tuple29*)_tmp888->hd;struct _tuple29*_tmp884=_tmp883;void**_tmp885;_LL8: _tmp885=(void**)& _tmp884->f2;_LL9:;
({void*_tmpDBC=Cyc_Toc_rewrite_decision(*_tmp885,success);*_tmp885=_tmpDBC;});}
# 4961
return d;}_LL0:;}
# 4972 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4974
struct _RegionHandle _tmp88A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp88A;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp88B=Cyc_Toc_share_env(rgn,nv);
void*_tmp88C=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp88B,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4984
struct Cyc_Absyn_Stmt*_tmp88D=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4988
({void*_tmpDBD=Cyc_Toc_rewrite_decision(dopt,_tmp88D);dopt=_tmpDBD;});{
# 4990
struct Cyc_Absyn_Switch_clause*_tmp88E=({struct Cyc_Absyn_Switch_clause*_tmp89B=_cycalloc(sizeof(*_tmp89B));_tmp89B->pattern=p;_tmp89B->pat_vars=0;_tmp89B->where_clause=0;_tmp89B->body=_tmp88D;_tmp89B->loc=0U;_tmp89B;});
struct Cyc_List_List*_tmp88F=({struct _RegionHandle*_tmpDC0=rgn;struct _RegionHandle*_tmpDBF=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpDC0,Cyc_Toc_gen_labels,_tmpDBF,({struct Cyc_Absyn_Switch_clause*_tmp89A[1U];_tmp89A[0U]=_tmp88E;({struct _RegionHandle*_tmpDBE=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpDBE,_tag_dyneither(_tmp89A,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4993
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4997
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp88B,& mydecls,& mybodies,dopt,_tmp88F,v);
# 5000
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple34*_tmp890=(struct _tuple34*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple34*_tmp891=_tmp890;struct Cyc_Toc_Env*_tmp893;struct Cyc_Absyn_Stmt*_tmp892;_LL1: _tmp893=_tmp891->f1;_tmp892=_tmp891->f3;_LL2:;
if(_tmp892 == _tmp88D){senv=_tmp893;goto FOUND_ENV;}}
# 5005
({void*_tmp894=0U;({struct _dyneither_ptr _tmpDC1=({const char*_tmp895="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp895,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpDC1,_tag_dyneither(_tmp894,sizeof(void*),0U));});});
FOUND_ENV:
# 5009
 Cyc_Toc_stmt_to_c(senv,s);{
# 5011
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 5013
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp896=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpDC2=Cyc_Toc_temp_var();_tmp896->name=_tmpDC2;});
({void*_tmpDC3=Cyc_Toc_typ_to_c_array(_tmp896->type);_tmp896->type=_tmpDC3;});
({struct Cyc_Absyn_Stmt*_tmpDC6=({struct Cyc_Absyn_Decl*_tmpDC5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp897=_cycalloc(sizeof(*_tmp897));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDC4=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp898;_tmp898.tag=0U;_tmp898.f1=_tmp896;_tmp898;});_tmp897[0]=_tmpDC4;});_tmp897;}),0U);Cyc_Absyn_decl_stmt(_tmpDC5,res,0U);});res=_tmpDC6;});}
# 5020
({struct Cyc_Absyn_Stmt*_tmpDCA=({struct _tuple1*_tmpDC9=v;void*_tmpDC8=Cyc_Toc_typ_to_c(_tmp88C);struct Cyc_Absyn_Exp*_tmpDC7=e;Cyc_Absyn_declare_stmt(_tmpDC9,_tmpDC8,_tmpDC7,res,0U);});res=_tmpDCA;});{
struct Cyc_Absyn_Stmt*_tmp899=res;_npop_handler(0U);return _tmp899;};};};};}
# 4975
;_pop_region(rgn);}
# 5028
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp89C=e->r;void*_tmp89D=_tmp89C;struct Cyc_Absyn_MallocInfo*_tmp8C7;struct Cyc_Absyn_Stmt*_tmp8C6;void**_tmp8C5;void**_tmp8C4;struct Cyc_List_List*_tmp8C3;struct Cyc_List_List*_tmp8C2;void**_tmp8C1;struct Cyc_List_List*_tmp8C0;void**_tmp8BF;struct Cyc_Absyn_Exp*_tmp8BE;struct Cyc_Absyn_Exp*_tmp8BD;struct Cyc_List_List*_tmp8BC;struct Cyc_Absyn_Exp*_tmp8BB;struct Cyc_Absyn_Exp*_tmp8BA;struct Cyc_Absyn_Exp*_tmp8B9;struct Cyc_Absyn_Exp*_tmp8B8;struct Cyc_Absyn_Exp*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_Absyn_Exp*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B4;struct Cyc_Absyn_Exp*_tmp8B3;struct Cyc_Absyn_Exp*_tmp8B2;struct Cyc_Absyn_Exp*_tmp8B1;struct Cyc_Absyn_Exp*_tmp8B0;struct Cyc_Absyn_Exp*_tmp8AF;struct Cyc_Absyn_Exp*_tmp8AE;struct Cyc_Absyn_Exp*_tmp8AD;struct Cyc_List_List*_tmp8AC;struct Cyc_Absyn_Exp*_tmp8AB;struct Cyc_Absyn_Exp*_tmp8AA;struct Cyc_Absyn_Exp*_tmp8A9;struct Cyc_Absyn_Exp*_tmp8A8;struct Cyc_Absyn_Exp*_tmp8A7;struct Cyc_Absyn_Exp*_tmp8A6;struct Cyc_Absyn_Exp*_tmp8A5;struct Cyc_Absyn_Exp*_tmp8A4;switch(*((int*)_tmp89D)){case 20U: _LL1: _tmp8A4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LL2:
 _tmp8A5=_tmp8A4;goto _LL4;case 21U: _LL3: _tmp8A5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LL4:
 _tmp8A6=_tmp8A5;goto _LL6;case 22U: _LL5: _tmp8A6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LL6:
 _tmp8A7=_tmp8A6;goto _LL8;case 15U: _LL7: _tmp8A7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LL8:
 _tmp8A8=_tmp8A7;goto _LLA;case 11U: _LL9: _tmp8A8=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LLA:
 _tmp8A9=_tmp8A8;goto _LLC;case 12U: _LLB: _tmp8A9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LLC:
 _tmp8AA=_tmp8A9;goto _LLE;case 18U: _LLD: _tmp8AA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LLE:
 _tmp8AB=_tmp8AA;goto _LL10;case 5U: _LLF: _tmp8AB=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LL10:
 Cyc_Toc_exptypes_to_c(_tmp8AB);goto _LL0;case 3U: _LL11: _tmp8AC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_LL12:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8AC);goto _LL0;case 7U: _LL13: _tmp8AE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_tmp8AD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_LL14:
 _tmp8B0=_tmp8AE;_tmp8AF=_tmp8AD;goto _LL16;case 8U: _LL15: _tmp8B0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_tmp8AF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_LL16:
 _tmp8B2=_tmp8B0;_tmp8B1=_tmp8AF;goto _LL18;case 9U: _LL17: _tmp8B2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_tmp8B1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_LL18:
 _tmp8B4=_tmp8B2;_tmp8B3=_tmp8B1;goto _LL1A;case 23U: _LL19: _tmp8B4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_tmp8B3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_LL1A:
 _tmp8B6=_tmp8B4;_tmp8B5=_tmp8B3;goto _LL1C;case 35U: _LL1B: _tmp8B6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_tmp8B5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_LL1C:
 _tmp8B8=_tmp8B6;_tmp8B7=_tmp8B5;goto _LL1E;case 4U: _LL1D: _tmp8B8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_tmp8B7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp89D)->f3;_LL1E:
 Cyc_Toc_exptypes_to_c(_tmp8B8);Cyc_Toc_exptypes_to_c(_tmp8B7);goto _LL0;case 6U: _LL1F: _tmp8BB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_tmp8BA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_tmp8B9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89D)->f3;_LL20:
# 5046
 Cyc_Toc_exptypes_to_c(_tmp8BB);Cyc_Toc_exptypes_to_c(_tmp8BA);Cyc_Toc_exptypes_to_c(_tmp8B9);goto _LL0;case 10U: _LL21: _tmp8BD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_tmp8BC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_LL22:
# 5048
 Cyc_Toc_exptypes_to_c(_tmp8BD);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8BC);goto _LL0;case 14U: _LL23: _tmp8BF=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_tmp8BE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_LL24:
({void*_tmpDCB=Cyc_Toc_typ_to_c(*_tmp8BF);*_tmp8BF=_tmpDCB;});Cyc_Toc_exptypes_to_c(_tmp8BE);goto _LL0;case 25U: _LL25: _tmp8C1=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp89D)->f1)->f3;_tmp8C0=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_LL26:
# 5051
({void*_tmpDCC=Cyc_Toc_typ_to_c(*_tmp8C1);*_tmp8C1=_tmpDCC;});
_tmp8C2=_tmp8C0;goto _LL28;case 36U: _LL27: _tmp8C2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp89D)->f2;_LL28:
 _tmp8C3=_tmp8C2;goto _LL2A;case 26U: _LL29: _tmp8C3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LL2A:
# 5055
 for(0;_tmp8C3 != 0;_tmp8C3=_tmp8C3->tl){
struct _tuple19 _tmp89E=*((struct _tuple19*)_tmp8C3->hd);struct _tuple19 _tmp89F=_tmp89E;struct Cyc_Absyn_Exp*_tmp8A0;_LL54: _tmp8A0=_tmp89F.f2;_LL55:;
Cyc_Toc_exptypes_to_c(_tmp8A0);}
# 5059
goto _LL0;case 19U: _LL2B: _tmp8C4=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LL2C:
 _tmp8C5=_tmp8C4;goto _LL2E;case 17U: _LL2D: _tmp8C5=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LL2E:
({void*_tmpDCD=Cyc_Toc_typ_to_c(*_tmp8C5);*_tmp8C5=_tmpDCD;});goto _LL0;case 37U: _LL2F: _tmp8C6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LL30:
 Cyc_Toc_stmttypes_to_c(_tmp8C6);goto _LL0;case 34U: _LL31: _tmp8C7=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp89D)->f1;_LL32:
# 5064
 if(_tmp8C7->elt_type != 0)
({void**_tmpDCF=({void**_tmp8A1=_cycalloc(sizeof(*_tmp8A1));({void*_tmpDCE=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8C7->elt_type)));_tmp8A1[0]=_tmpDCE;});_tmp8A1;});_tmp8C7->elt_type=_tmpDCF;});
Cyc_Toc_exptypes_to_c(_tmp8C7->num_elts);
goto _LL0;case 0U: _LL33: _LL34:
 goto _LL36;case 1U: _LL35: _LL36:
 goto _LL38;case 32U: _LL37: _LL38:
 goto _LL3A;case 40U: _LL39: _LL3A:
 goto _LL3C;case 33U: _LL3B: _LL3C:
 goto _LL0;case 2U: _LL3D: _LL3E:
# 5074
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
({void*_tmp8A2=0U;({unsigned int _tmpDD1=e->loc;struct _dyneither_ptr _tmpDD0=({const char*_tmp8A3="Cyclone expression within C code";_tag_dyneither(_tmp8A3,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpDD1,_tmpDD0,_tag_dyneither(_tmp8A2,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 5088
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp8C8=d->r;void*_tmp8C9=_tmp8C8;struct Cyc_Absyn_Typedefdecl*_tmp8D3;struct Cyc_Absyn_Enumdecl*_tmp8D2;struct Cyc_Absyn_Aggrdecl*_tmp8D1;struct Cyc_Absyn_Fndecl*_tmp8D0;struct Cyc_Absyn_Vardecl*_tmp8CF;switch(*((int*)_tmp8C9)){case 0U: _LL1: _tmp8CF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8C9)->f1;_LL2:
# 5091
({void*_tmpDD2=Cyc_Toc_typ_to_c(_tmp8CF->type);_tmp8CF->type=_tmpDD2;});
if(_tmp8CF->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8CF->initializer));
goto _LL0;case 1U: _LL3: _tmp8D0=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8C9)->f1;_LL4:
# 5095
({void*_tmpDD3=Cyc_Toc_typ_to_c(_tmp8D0->ret_type);_tmp8D0->ret_type=_tmpDD3;});
{struct Cyc_List_List*_tmp8CA=_tmp8D0->args;for(0;_tmp8CA != 0;_tmp8CA=_tmp8CA->tl){
({void*_tmpDD4=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8CA->hd)).f3);(*((struct _tuple9*)_tmp8CA->hd)).f3=_tmpDD4;});}}
# 5099
goto _LL0;case 5U: _LL5: _tmp8D1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8C9)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp8D1,1);goto _LL0;case 7U: _LL7: _tmp8D2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8C9)->f1;_LL8:
# 5102
 if(_tmp8D2->fields != 0){
struct Cyc_List_List*_tmp8CB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8D2->fields))->v;for(0;_tmp8CB != 0;_tmp8CB=_tmp8CB->tl){
struct Cyc_Absyn_Enumfield*_tmp8CC=(struct Cyc_Absyn_Enumfield*)_tmp8CB->hd;
if(_tmp8CC->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8CC->tag));}}
# 5107
goto _LL0;case 8U: _LL9: _tmp8D3=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8C9)->f1;_LLA:
({void*_tmpDD5=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8D3->defn));_tmp8D3->defn=_tmpDD5;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 5117
({void*_tmp8CD=0U;({unsigned int _tmpDD7=d->loc;struct _dyneither_ptr _tmpDD6=({const char*_tmp8CE="Cyclone declaration within C code";_tag_dyneither(_tmp8CE,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpDD7,_tmpDD6,_tag_dyneither(_tmp8CD,sizeof(void*),0U));});});
goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 5121
 goto _LL0;}_LL0:;}
# 5125
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp8D4=s->r;void*_tmp8D5=_tmp8D4;struct Cyc_Absyn_Stmt*_tmp8F0;struct Cyc_Absyn_Stmt*_tmp8EF;struct Cyc_Absyn_Exp*_tmp8EE;struct Cyc_Absyn_Decl*_tmp8ED;struct Cyc_Absyn_Stmt*_tmp8EC;struct Cyc_Absyn_Exp*_tmp8EB;struct Cyc_List_List*_tmp8EA;void*_tmp8E9;struct Cyc_Absyn_Exp*_tmp8E8;struct Cyc_Absyn_Exp*_tmp8E7;struct Cyc_Absyn_Exp*_tmp8E6;struct Cyc_Absyn_Stmt*_tmp8E5;struct Cyc_Absyn_Exp*_tmp8E4;struct Cyc_Absyn_Stmt*_tmp8E3;struct Cyc_Absyn_Exp*_tmp8E2;struct Cyc_Absyn_Stmt*_tmp8E1;struct Cyc_Absyn_Stmt*_tmp8E0;struct Cyc_Absyn_Exp*_tmp8DF;struct Cyc_Absyn_Stmt*_tmp8DE;struct Cyc_Absyn_Stmt*_tmp8DD;struct Cyc_Absyn_Exp*_tmp8DC;switch(*((int*)_tmp8D5)){case 1U: _LL1: _tmp8DC=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp8DC);goto _LL0;case 2U: _LL3: _tmp8DE=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f1;_tmp8DD=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp8DE);Cyc_Toc_stmttypes_to_c(_tmp8DD);goto _LL0;case 3U: _LL5: _tmp8DF=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f1;_LL6:
 if(_tmp8DF != 0)Cyc_Toc_exptypes_to_c(_tmp8DF);goto _LL0;case 4U: _LL7: _tmp8E2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f1;_tmp8E1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f2;_tmp8E0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f3;_LL8:
# 5131
 Cyc_Toc_exptypes_to_c(_tmp8E2);Cyc_Toc_stmttypes_to_c(_tmp8E1);Cyc_Toc_stmttypes_to_c(_tmp8E0);goto _LL0;case 5U: _LL9: _tmp8E4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f1).f1;_tmp8E3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f2;_LLA:
# 5133
 Cyc_Toc_exptypes_to_c(_tmp8E4);Cyc_Toc_stmttypes_to_c(_tmp8E3);goto _LL0;case 9U: _LLB: _tmp8E8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f1;_tmp8E7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f2).f1;_tmp8E6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f3).f1;_tmp8E5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f4;_LLC:
# 5135
 Cyc_Toc_exptypes_to_c(_tmp8E8);Cyc_Toc_exptypes_to_c(_tmp8E7);Cyc_Toc_exptypes_to_c(_tmp8E6);
Cyc_Toc_stmttypes_to_c(_tmp8E5);goto _LL0;case 10U: _LLD: _tmp8EB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f1;_tmp8EA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f2;_tmp8E9=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f3;_LLE:
# 5138
 Cyc_Toc_exptypes_to_c(_tmp8EB);
for(0;_tmp8EA != 0;_tmp8EA=_tmp8EA->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp8EA->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp8ED=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f1;_tmp8EC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp8ED);Cyc_Toc_stmttypes_to_c(_tmp8EC);goto _LL0;case 14U: _LL11: _tmp8EF=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f1;_tmp8EE=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp8EF);Cyc_Toc_exptypes_to_c(_tmp8EE);goto _LL0;case 13U: _LL13: _tmp8F0=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8D5)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp8F0);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 5148
 goto _LL0;case 11U: _LL1D: _LL1E:
# 5151
({void*_tmpDD9=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp8D6=_cycalloc_atomic(sizeof(*_tmp8D6));({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmpDD8=({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmp8D7;_tmp8D7.tag=0U;_tmp8D7;});_tmp8D6[0]=_tmpDD8;});_tmp8D6;});s->r=_tmpDD9;});
goto _LL0;default: _LL1F: _LL20:
# 5154
({void*_tmp8D8=0U;({unsigned int _tmpDDD=s->loc;struct _dyneither_ptr _tmpDDC=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8DB;_tmp8DB.tag=0U;({struct _dyneither_ptr _tmpDDA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp8DB.f1=_tmpDDA;});({void*_tmp8D9[1U]={& _tmp8DB};({struct _dyneither_ptr _tmpDDB=({const char*_tmp8DA="Cyclone statement in C code: %s";_tag_dyneither(_tmp8DA,sizeof(char),32U);});Cyc_aprintf(_tmpDDB,_tag_dyneither(_tmp8D9,sizeof(void*),1U));});});});Cyc_Tcutil_terr(_tmpDDD,_tmpDDC,_tag_dyneither(_tmp8D8,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 5162
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp8F1=0U;({struct _dyneither_ptr _tmpDDE=({const char*_tmp8F2="decls_to_c: not at toplevel!";_tag_dyneither(_tmp8F2,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDDE,_tag_dyneither(_tmp8F1,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp8F3=d->r;void*_tmp8F4=_tmp8F3;struct Cyc_List_List*_tmp90C;struct Cyc_List_List*_tmp90B;struct Cyc_List_List*_tmp90A;struct Cyc_List_List*_tmp909;struct Cyc_Absyn_Typedefdecl*_tmp908;struct Cyc_Absyn_Enumdecl*_tmp907;struct Cyc_Absyn_Datatypedecl*_tmp906;struct Cyc_Absyn_Aggrdecl*_tmp905;struct Cyc_Absyn_Fndecl*_tmp904;struct Cyc_Absyn_Vardecl*_tmp903;switch(*((int*)_tmp8F4)){case 0U: _LL1: _tmp903=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8F4)->f1;_LL2: {
# 5170
struct _tuple1*_tmp8F5=_tmp903->name;
# 5172
if(_tmp903->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpDE0=({struct _tuple1*_tmp8F6=_cycalloc(sizeof(*_tmp8F6));({union Cyc_Absyn_Nmspace _tmpDDF=Cyc_Absyn_Abs_n(0,1);_tmp8F6->f1=_tmpDDF;});_tmp8F6->f2=(*_tmp8F5).f2;_tmp8F6;});_tmp8F5=_tmpDE0;});
if(_tmp903->initializer != 0){
if(_tmp903->sc == Cyc_Absyn_ExternC)_tmp903->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp903->initializer));else{
# 5179
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp903->initializer));}}
# 5181
_tmp903->name=_tmp8F5;
({enum Cyc_Absyn_Scope _tmpDE1=Cyc_Toc_scope_to_c(_tmp903->sc);_tmp903->sc=_tmpDE1;});
({void*_tmpDE2=Cyc_Toc_typ_to_c(_tmp903->type);_tmp903->type=_tmpDE2;});
({struct Cyc_List_List*_tmpDE3=({struct Cyc_List_List*_tmp8F7=_cycalloc(sizeof(*_tmp8F7));_tmp8F7->hd=d;_tmp8F7->tl=Cyc_Toc_result_decls;_tmp8F7;});Cyc_Toc_result_decls=_tmpDE3;});
goto _LL0;}case 1U: _LL3: _tmp904=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8F4)->f1;_LL4: {
# 5187
struct _tuple1*_tmp8F8=_tmp904->name;
# 5189
if(_tmp904->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpDE5=({struct _tuple1*_tmp8F9=_cycalloc(sizeof(*_tmp8F9));({union Cyc_Absyn_Nmspace _tmpDE4=Cyc_Absyn_Abs_n(0,1);_tmp8F9->f1=_tmpDE4;});_tmp8F9->f2=(*_tmp8F8).f2;_tmp8F9;});_tmp8F8=_tmpDE5;});
_tmp904->sc=Cyc_Absyn_Public;}
# 5193
_tmp904->name=_tmp8F8;
Cyc_Toc_fndecl_to_c(nv,_tmp904,cinclude);
({struct Cyc_List_List*_tmpDE6=({struct Cyc_List_List*_tmp8FA=_cycalloc(sizeof(*_tmp8FA));_tmp8FA->hd=d;_tmp8FA->tl=Cyc_Toc_result_decls;_tmp8FA;});Cyc_Toc_result_decls=_tmpDE6;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp8FB=0U;({struct _dyneither_ptr _tmpDE7=({const char*_tmp8FC="letdecl at toplevel";_tag_dyneither(_tmp8FC,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpDE7,_tag_dyneither(_tmp8FB,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp8FD=0U;({struct _dyneither_ptr _tmpDE8=({const char*_tmp8FE="region decl at toplevel";_tag_dyneither(_tmp8FE,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpDE8,_tag_dyneither(_tmp8FD,sizeof(void*),0U));});});case 5U: _LLB: _tmp905=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8F4)->f1;_LLC:
# 5201
 Cyc_Toc_aggrdecl_to_c(_tmp905,1);
goto _LL0;case 6U: _LLD: _tmp906=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp8F4)->f1;_LLE:
# 5204
 if(_tmp906->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp906);else{
# 5207
Cyc_Toc_datatypedecl_to_c(_tmp906);}
goto _LL0;case 7U: _LLF: _tmp907=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8F4)->f1;_LL10:
# 5210
 Cyc_Toc_enumdecl_to_c(_tmp907);
({struct Cyc_List_List*_tmpDE9=({struct Cyc_List_List*_tmp8FF=_cycalloc(sizeof(*_tmp8FF));_tmp8FF->hd=d;_tmp8FF->tl=Cyc_Toc_result_decls;_tmp8FF;});Cyc_Toc_result_decls=_tmpDE9;});
goto _LL0;case 8U: _LL11: _tmp908=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8F4)->f1;_LL12:
# 5214
 _tmp908->tvs=0;
if(_tmp908->defn != 0)
({void*_tmpDEA=Cyc_Toc_typ_to_c((void*)_check_null(_tmp908->defn));_tmp908->defn=_tmpDEA;});else{
# 5218
enum Cyc_Absyn_KindQual _tmp900=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp908->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp901=_tmp900;if(_tmp901 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpDEB=Cyc_Absyn_void_star_typ();_tmp908->defn=_tmpDEB;});goto _LL1F;}else{_LL22: _LL23:
 _tmp908->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL1F;}_LL1F:;}
# 5225
if(Cyc_noexpand_r  || Cyc_Absynpp_is_anon_aggrtype((void*)_check_null(_tmp908->defn)))
({struct Cyc_List_List*_tmpDEC=({struct Cyc_List_List*_tmp902=_cycalloc(sizeof(*_tmp902));_tmp902->hd=d;_tmp902->tl=Cyc_Toc_result_decls;_tmp902;});Cyc_Toc_result_decls=_tmpDEC;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
# 5230
 goto _LL0;case 9U: _LL17: _tmp909=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp8F4)->f2;_LL18:
 _tmp90A=_tmp909;goto _LL1A;case 10U: _LL19: _tmp90A=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp8F4)->f2;_LL1A:
 _tmp90B=_tmp90A;goto _LL1C;case 11U: _LL1B: _tmp90B=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp8F4)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpDED=Cyc_Toc_decls_to_c(r,nv,_tmp90B,top,cinclude);nv=_tmpDED;});goto _LL0;default: _LL1D: _tmp90C=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp8F4)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpDEE=Cyc_Toc_decls_to_c(r,nv,_tmp90C,top,1);nv=_tmpDEE;});goto _LL0;}_LL0:;};}
# 5237
return nv;}
# 5241
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp90D=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp90E=_tmp90D;struct Cyc_Core_DynamicRegion*_tmp912;_LL1: _tmp912=_tmp90E.key;_LL2:;{
struct Cyc_Toc_TocState*_tmp90F=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp912,0,Cyc_Toc_empty_toc_state);
({struct Cyc_Toc_TocStateWrap*_tmpDEF=({struct Cyc_Toc_TocStateWrap*_tmp910=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp910));_tmp910->dyn=(struct Cyc_Core_DynamicRegion*)_tmp912;_tmp910->state=(struct Cyc_Toc_TocState*)_tmp90F;_tmp910;});Cyc_Toc_toc_state=_tmpDEF;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0U;
Cyc_Toc_bounds_checks_eliminated=0U;
({struct _dyneither_ptr _tmpDF0=_tag_dyneither(({struct _dyneither_ptr**_tmp911=_cycalloc(sizeof(struct _dyneither_ptr*)* 41);_tmp911[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str;_tmp911[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str;_tmp911[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str;_tmp911[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str;_tmp911[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str;_tmp911[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str;_tmp911[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str;_tmp911[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str;_tmp911[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str;_tmp911[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str;_tmp911[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str;_tmp911[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str;_tmp911[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str;_tmp911[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str;_tmp911[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str;_tmp911[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str;_tmp911[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str;_tmp911[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str;_tmp911[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp911[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str;_tmp911[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp911[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp911[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str;_tmp911[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str;_tmp911[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str;_tmp911[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str;_tmp911[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str;_tmp911[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str;_tmp911[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str;_tmp911[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str;_tmp911[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str;_tmp911[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str;_tmp911[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str;_tmp911[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str;_tmp911[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str;_tmp911[35]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str;_tmp911[36]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp911[37]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str;_tmp911[38]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str;_tmp911[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str;_tmp911[40]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str;_tmp911;}),sizeof(struct _dyneither_ptr*),41U);Cyc_Toc_globals=_tmpDF0;});};}
# 5297
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp913=*ts;struct Cyc_Toc_TocState _tmp914=_tmp913;struct Cyc_Xarray_Xarray*_tmp915;_LL1: _tmp915=_tmp914.temp_labels;_LL2:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp915);
return 0;}
# 5303
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp916=ts;struct Cyc_Toc_TocStateWrap*_tmp917=Cyc_Toc_toc_state;ts=_tmp917;Cyc_Toc_toc_state=_tmp916;});{
struct Cyc_Toc_TocStateWrap _tmp918=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp919=_tmp918;struct Cyc_Core_DynamicRegion*_tmp91B;struct Cyc_Toc_TocState*_tmp91A;_LL1: _tmp91B=_tmp919.dyn;_tmp91A=_tmp919.state;_LL2:;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp91B,_tmp91A,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp91B);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 5311
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 5317
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 5319
({struct Cyc_Hashtable_Table**_tmpDF1=({struct Cyc_Hashtable_Table**_tmp91C=_cycalloc(sizeof(*_tmp91C));_tmp91C[0]=pop_tables;_tmp91C;});Cyc_Toc_gpop_tables=_tmpDF1;});
Cyc_Toc_init();
{struct _RegionHandle _tmp91D=_new_region("start");struct _RegionHandle*start=& _tmp91D;_push_region(start);
({struct _RegionHandle*_tmpDF3=start;struct Cyc_Toc_Env*_tmpDF2=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpDF3,_tmpDF2,ds,1,0);});;_pop_region(start);}
# 5328
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
