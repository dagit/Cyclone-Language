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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 67
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 37
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Relations_Reln;
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
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 911
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 918
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 931
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 938
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 941
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 956
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 958
extern void*Cyc_Absyn_sint_typ;
# 970
void*Cyc_Absyn_exn_typ();
# 981
extern void*Cyc_Absyn_bounds_one;
# 989
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 995
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 997
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1000
void*Cyc_Absyn_void_star_typ();
# 1002
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1008
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1071
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1077
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1093
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1096
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1100
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1102
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1107
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1111
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1116
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1159
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1162
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1172
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1176
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1186
struct _tuple1*Cyc_Absyn_binding2qvar(void*);
# 1191
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
# 60 "toc.cyc"
extern int Cyc_noexpand_r;
# 71 "toc.cyc"
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 77
struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}
# 87
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 107
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp0=*x;struct _tuple14 _tmp1=_tmp0;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL1: _tmp7=_tmp1.f1;_tmp6=_tmp1.f2;_LL2:;{
struct _tuple14 _tmp2=*y;struct _tuple14 _tmp3=_tmp2;struct _tuple1*_tmp5;struct _tuple1*_tmp4;_LL4: _tmp5=_tmp3.f1;_tmp4=_tmp3.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp7,_tmp5);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp6,_tmp4);};};}
# 116
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d,int unused){
return({struct Cyc_Toc_TocState*_tmp8=_region_malloc(d,sizeof(*_tmp8));_tmp8->tuple_types=({struct Cyc_List_List**_tmpE=_region_malloc(d,sizeof(*_tmpE));_tmpE[0]=0;_tmpE;});_tmp8->aggrs_so_far=({struct Cyc_Dict_Dict*_tmpD=_region_malloc(d,sizeof(*_tmpD));_tmpD[0]=
# 119
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpD;});_tmp8->abs_struct_types=({struct Cyc_List_List**_tmpC=_region_malloc(d,sizeof(*_tmpC));_tmpC[0]=0;_tmpC;});_tmp8->datatypes_so_far=({struct Cyc_Set_Set**_tmpB=_region_malloc(d,sizeof(*_tmpB));_tmpB[0]=
# 121
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpB;});_tmp8->xdatatypes_so_far=({struct Cyc_Dict_Dict*_tmpA=_region_malloc(d,sizeof(*_tmpA));_tmpA[0]=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpA;});_tmp8->qvar_tags=({struct Cyc_Dict_Dict*_tmp9=_region_malloc(d,sizeof(*_tmp9));_tmp9[0]=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmp9;});_tmp8->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp8;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 133
typedef struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state_t;
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 140
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 143
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
# 155
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 158
struct _tuple17 _tmp1C=*env;struct _tuple17 _tmp1D=_tmp1C;struct Cyc_Toc_TocState*_tmp23;struct _tuple1*_tmp22;void*(*_tmp21)(struct _tuple1*);_LLA: _tmp23=_tmp1D.f1;_tmp22=(_tmp1D.f2)->f1;_tmp21=(_tmp1D.f2)->f2;_LLB:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp23->aggrs_so_far,_tmp22);
if(v == 0)
return _tmp21(_tmp22);else{
# 163
struct _tuple18*_tmp1E=*v;struct _tuple18*_tmp1F=_tmp1E;void*_tmp20;_LLD: _tmp20=_tmp1F->f2;_LLE:;
return _tmp20;}};}
# 168
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp24;_tmp24.f1=q;_tmp24.f2=type_maker;_tmp24;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 173
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 177
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 179
Cyc_Warn_vimpos((struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp25="Toc (unimplemented): ";_tag_dyneither(_tmp25,sizeof(char),22);}),(struct _dyneither_ptr)fmt),ap);}
# 181
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 183
Cyc_Warn_vimpos((struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp26="Toc: ";_tag_dyneither(_tmp26,sizeof(char),6);}),(struct _dyneither_ptr)fmt),ap);}static char _tmp27[5]="curr";
# 194 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp27,_tmp27,_tmp27 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp28[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp28,_tmp28,_tmp28 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp29[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp29,_tmp29,_tmp29 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp2A[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp2A,_tmp2A,_tmp2A + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp2B[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp2B,_tmp2B,_tmp2B + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2C[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2C,_tmp2C,_tmp2C + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2D[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2D,_tmp2D,_tmp2D + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp2E[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp2E,_tmp2E,_tmp2E + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 207
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp2F[7]="_throw";
# 216 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp2F,_tmp2F,_tmp2F + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp32[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp32,_tmp32,_tmp32 + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp35[14]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp35,_tmp35,_tmp35 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp38[13]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp38,_tmp38,_tmp38 + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp3B[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp3B,_tmp3B,_tmp3B + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp3E[14]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp3E,_tmp3E,_tmp3E + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp41[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp41,_tmp41,_tmp41 + 12};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp44[28]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp44,_tmp44,_tmp44 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp47[31]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp47,_tmp47,_tmp47 + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp4A[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp4A,_tmp4A,_tmp4A + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp4D[15]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp4D,_tmp4D,_tmp4D + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp50[15]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp50,_tmp50,_tmp50 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp53[20]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp53,_tmp53,_tmp53 + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__init_dyneither_ptr_bnd={0,& Cyc_Toc__init_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,(void*)& Cyc_Toc__init_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp56[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp56,_tmp56,_tmp56 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp59[20]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp59,_tmp59,_tmp59 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp5C[19]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp5C,_tmp5C,_tmp5C + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp5F[24]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp5F,_tmp5F,_tmp5F + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp62[25]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp62,_tmp62,_tmp62 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp65[23]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp65,_tmp65,_tmp65 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp68[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp68,_tmp68,_tmp68 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp6B[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp6B,_tmp6B,_tmp6B + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp6E[30]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp6E,_tmp6E,_tmp6E + 30};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp71[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp71,_tmp71,_tmp71 + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp74[20]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp74,_tmp74,_tmp74 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp77[15]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp77,_tmp77,_tmp77 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp7A[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp7A,_tmp7A,_tmp7A + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp7D[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp7D,_tmp7D,_tmp7D + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp80[19]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp80,_tmp80,_tmp80 + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp83[21]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp83,_tmp83,_tmp83 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp86[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp86,_tmp86,_tmp86 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp89[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp89,_tmp89,_tmp89 + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp8C[24]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp8C,_tmp8C,_tmp8C + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp8F[28]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp8F,_tmp8F,_tmp8F + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp92[23]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp92,_tmp92,_tmp92 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp95[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp95,_tmp95,_tmp95 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp98[29]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp98,_tmp98,_tmp98 + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp9B[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp9B,_tmp9B,_tmp9B + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp9E[29]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp9E,_tmp9E,_tmp9E + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmpA1[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmpA1,_tmpA1,_tmpA1 + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmpA4[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmpA4,_tmpA4,_tmpA4 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmpA7[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmpA7,_tmpA7,_tmpA7 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmpAA[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmpAA,_tmpAA,_tmpAA + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmpAD[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmpAD,_tmpAD,_tmpAD + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmpB0[33]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmpB0,_tmpB0,_tmpB0 + 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmpB3[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmpB3,_tmpB3,_tmpB3 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmpB6[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmpB6,_tmpB6,_tmpB6 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmpB9[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmpB9,_tmpB9,_tmpB9 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmpBC[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmpBC,_tmpBC,_tmpBC + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmpBF[39]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmpBF,_tmpBF,_tmpBF + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmpC2[37]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmpC2,_tmpC2,_tmpC2 + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmpC5[10]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmpC5,_tmpC5,_tmpC5 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmpC8[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmpC8,_tmpC8,_tmpC8 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmpCB[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmpCB,_tmpCB,_tmpCB + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmpCE[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmpCE,_tmpCE,_tmpCE + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmpD1[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmpD1,_tmpD1,_tmpD1 + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmpD4[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmpD4,_tmpD4,_tmpD4 + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmpD7[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmpD7,_tmpD7,_tmpD7 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmpDA[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmpDA,_tmpDA,_tmpDA + 12};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmpDD[13]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmpDD,_tmpDD,_tmpDD + 13};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmpE0[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmpE0,_tmpE0,_tmpE0 + 12};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmpE3[16]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmpE3,_tmpE3,_tmpE3 + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__open_dynregion_bnd={0,& Cyc_Toc__open_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,(void*)& Cyc_Toc__open_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmpE6[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmpE6,_tmpE6,_tmpE6 + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_dynregion_bnd={0,& Cyc_Toc__push_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,(void*)& Cyc_Toc__push_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmpE9[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmpE9,_tmpE9,_tmpE9 + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_dynregion_bnd={0,& Cyc_Toc__pop_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,(void*)& Cyc_Toc__pop_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmpEC[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmpEC,_tmpEC,_tmpEC + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__reset_region_bnd={0,& Cyc_Toc__reset_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,(void*)& Cyc_Toc__reset_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmpEF[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpEF,_tmpEF,_tmpEF + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpF2[29]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpF2,_tmpF2,_tmpF2 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpF5[11]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpF5,_tmpF5,_tmpF5 + 11};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__swap_word_bnd={0,& Cyc_Toc__swap_word_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,(void*)& Cyc_Toc__swap_word_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpF8[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpF8,_tmpF8,_tmpF8 + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__swap_dyneither_bnd={0,& Cyc_Toc__swap_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,(void*)& Cyc_Toc__swap_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpFB[13]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpFB,_tmpFB,_tmpFB + 13};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmpFE[9]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmpFE,_tmpFE,_tmpFE + 9};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp101[20]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmp101,_tmp101,_tmp101 + 20};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 291
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 294
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 296
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 298
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0);}
# 304
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp105=e->r;void*_tmp106=_tmp105;struct Cyc_Absyn_Exp*_tmp107;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp106)->tag == 13){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp106)->f4 == Cyc_Absyn_No_coercion){_LL10: _tmp107=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp106)->f2;_LL11:
 return Cyc_Toc_cast_it(t,_tmp107);}else{goto _LL12;}}else{_LL12: _LL13:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}_LLF:;}
# 310
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp109;_tmp109.tag=13;_tmp109.f1=t;_tmp109.f2=e;_tmp109.f3=0;_tmp109.f4=Cyc_Absyn_No_coercion;_tmp109;});_tmp108;});}
# 313
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp10B;_tmp10B.tag=19;_tmp10B.f1=e;_tmp10B;});_tmp10A;});}
# 316
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp10D;_tmp10D.tag=22;_tmp10D.f1=e1;_tmp10D.f2=e2;_tmp10D;});_tmp10C;});}
# 319
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp10F;_tmp10F.tag=36;_tmp10F.f1=s;_tmp10F;});_tmp10E;});}
# 322
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp111;_tmp111.tag=16;_tmp111.f1=t;_tmp111;});_tmp110;});}
# 325
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp113;_tmp113.tag=9;_tmp113.f1=e;_tmp113.f2=es;_tmp113.f3=0;_tmp113.f4=1;_tmp113;});_tmp112;});}
# 328
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp115;_tmp115.tag=1;_tmp115.f1=e;_tmp115;});_tmp114;});}
# 331
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116[0]=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp117;_tmp117.tag=2;_tmp117.f1=s1;_tmp117.f2=s2;_tmp117;});_tmp116;});}
# 334
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp119;_tmp119.tag=5;_tmp119.f1=e1;_tmp119.f2=e2;_tmp119.f3=e3;_tmp119;});_tmp118;});}
# 337
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp11B;_tmp11B.tag=20;_tmp11B.f1=e;_tmp11B.f2=n;_tmp11B.f3=0;_tmp11B.f4=0;_tmp11B;});_tmp11A;});}
# 340
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp11D;_tmp11D.tag=21;_tmp11D.f1=e;_tmp11D.f2=n;_tmp11D.f3=0;_tmp11D.f4=0;_tmp11D;});_tmp11C;});}
# 343
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 346
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp11F;_tmp11F.tag=35;_tmp11F.f1=tdopt;_tmp11F.f2=ds;_tmp11F;});_tmp11E;});}
# 348
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120[0]=({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp121;_tmp121.tag=8;_tmp121.f1=v;_tmp121;});_tmp120;});}
# 351
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 355
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp123=e->r;void*_tmp124=_tmp123;struct Cyc_Absyn_Exp*_tmp125;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp124)->tag == 19){_LL15: _tmp125=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp124)->f1;_LL16:
 return Cyc_Absyn_aggrarrow_exp(_tmp125,f,loc);}else{_LL17: _LL18:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL14:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 374
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 383
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 392
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 401
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 412
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp126=Cyc_Tcutil_compress(t);void*_tmp127=_tmp126;enum Cyc_Absyn_Size_of _tmp12E;switch(*((int*)_tmp127)){case 6: _LL1A: _tmp12E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp127)->f2;_LL1B:
# 416
{enum Cyc_Absyn_Size_of _tmp128=_tmp12E;switch(_tmp128){case Cyc_Absyn_Char_sz: _LL27: _LL28:
 function=fS->fchar;goto _LL26;case Cyc_Absyn_Short_sz: _LL29: _LL2A:
 function=fS->fshort;goto _LL26;case Cyc_Absyn_Int_sz: _LL2B: _LL2C:
 function=fS->fint;goto _LL26;default: _LL2D: _LL2E:
({void*_tmp129=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp12A="impossible IntType (not char, short or int)";_tag_dyneither(_tmp12A,sizeof(char),44);}),_tag_dyneither(_tmp129,sizeof(void*),0));});}_LL26:;}
# 422
goto _LL19;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp127)->f1){case 0: _LL1C: _LL1D:
 function=fS->ffloat;goto _LL19;case 1: _LL1E: _LL1F:
 function=fS->fdouble;goto _LL19;default: _LL20: _LL21:
 function=fS->flongdouble;goto _LL19;}case 5: _LL22: _LL23:
 function=fS->fvoidstar;goto _LL19;default: _LL24: _LL25:
({struct Cyc_String_pa_PrintArg_struct _tmp12D;_tmp12D.tag=0;_tmp12D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp12B[1]={& _tmp12D};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp12C="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp12C,sizeof(char),67);}),_tag_dyneither(_tmp12B,sizeof(void*),1));});});}_LL19:;}
# 429
return function;}
# 431
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp12F=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp130=_tmp12F;void*_tmp133;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp130)->tag == 5){_LL30: _tmp133=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp130)->f1).elt_typ;_LL31:
 return Cyc_Toc_getFunctionType(fS,_tmp133);}else{_LL32: _LL33:
({void*_tmp131=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp132="impossible type (not pointer)";_tag_dyneither(_tmp132,sizeof(char),30);}),_tag_dyneither(_tmp131,sizeof(void*),0));});}_LL2F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 441
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp134=e->r;void*_tmp135=_tmp134;struct Cyc_List_List*_tmp141;struct Cyc_List_List*_tmp140;struct Cyc_List_List*_tmp13F;struct Cyc_List_List*_tmp13E;struct Cyc_List_List*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;long long _tmp13B;int _tmp13A;short _tmp139;struct _dyneither_ptr _tmp138;char _tmp137;switch(*((int*)_tmp135)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135)->f1).Null_c).tag){case 2: _LL35: _tmp137=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135)->f1).Char_c).val).f2;_LL36:
 return _tmp137 == '\000';case 3: _LL37: _tmp138=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135)->f1).Wchar_c).val;_LL38: {
# 445
unsigned long _tmp136=Cyc_strlen((struct _dyneither_ptr)_tmp138);
int i=0;
if(_tmp136 >= 2  && *((const char*)_check_dyneither_subscript(_tmp138,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp138,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp138,sizeof(char),1))== 'x'  && _tmp136 >= 3) && *((const char*)_check_dyneither_subscript(_tmp138,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp136;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp138,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 455
return 0;}}case 4: _LL39: _tmp139=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135)->f1).Short_c).val).f2;_LL3A:
 return _tmp139 == 0;case 5: _LL3B: _tmp13A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135)->f1).Int_c).val).f2;_LL3C:
 return _tmp13A == 0;case 6: _LL3D: _tmp13B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp135)->f1).LongLong_c).val).f2;_LL3E:
 return _tmp13B == 0;case 1: _LL3F: _LL40:
 return 1;default: goto _LL4D;}case 13: _LL41: _tmp13C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp135)->f2;_LL42:
 return Cyc_Toc_is_zero(_tmp13C);case 23: _LL43: _tmp13D=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp135)->f1;_LL44:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp13D);case 25: _LL45: _tmp13E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp135)->f1;_LL46:
 _tmp13F=_tmp13E;goto _LL48;case 28: _LL47: _tmp13F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp135)->f3;_LL48:
 _tmp140=_tmp13F;goto _LL4A;case 24: _LL49: _tmp140=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp135)->f2;_LL4A:
 _tmp141=_tmp140;goto _LL4C;case 35: _LL4B: _tmp141=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp135)->f2;_LL4C:
# 466
 for(0;_tmp141 != 0;_tmp141=_tmp141->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp141->hd)).f2))return 0;}
return 1;default: _LL4D: _LL4E:
 return 0;}_LL34:;}
# 474
static int Cyc_Toc_is_nullable(void*t){
void*_tmp142=Cyc_Tcutil_compress(t);void*_tmp143=_tmp142;union Cyc_Absyn_Constraint*_tmp146;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp143)->tag == 5){_LL50: _tmp146=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp143)->f1).ptr_atts).nullable;_LL51:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp146);}else{_LL52: _LL53:
({void*_tmp144=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp145="Toc::is_nullable";_tag_dyneither(_tmp145,sizeof(char),17);}),_tag_dyneither(_tmp144,sizeof(void*),0));});}_LL4F:;}
# 483
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmp147=x;union Cyc_Absyn_Nmspace _tmp159;struct _dyneither_ptr*_tmp158;_LL55: _tmp159=_tmp147->f1;_tmp158=_tmp147->f2;_LL56:;{
union Cyc_Absyn_Nmspace _tmp148=_tmp159;struct Cyc_List_List*_tmp157;struct Cyc_List_List*_tmp156;struct Cyc_List_List*_tmp155;switch((_tmp148.Abs_n).tag){case 4: _LL58: _LL59:
 _tmp155=0;goto _LL5B;case 1: _LL5A: _tmp155=(_tmp148.Rel_n).val;_LL5B:
 _tmp156=_tmp155;goto _LL5D;case 2: _LL5C: _tmp156=(_tmp148.Abs_n).val;_LL5D:
 _tmp157=_tmp156;goto _LL5F;default: _LL5E: _tmp157=(_tmp148.C_n).val;_LL5F:
# 492
 if(_tmp157 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp158);({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0;_tmp14B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);({void*_tmp149[2]={& _tmp14B,& _tmp14C};Cyc_aprintf(({const char*_tmp14A="%s_%s_struct";_tag_dyneither(_tmp14A,sizeof(char),13);}),_tag_dyneither(_tmp149,sizeof(void*),2));});});});{
struct _RegionHandle _tmp14D=_new_region("r");struct _RegionHandle*r=& _tmp14D;_push_region(r);
{struct _dyneither_ptr _tmp154=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp152;_tmp152.tag=0;_tmp152.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp158);({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0;_tmp151.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_rstr_sepstr(r,_tmp157,({const char*_tmp153="_";_tag_dyneither(_tmp153,sizeof(char),2);})));({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0;_tmp150.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);({void*_tmp14E[3]={& _tmp150,& _tmp151,& _tmp152};Cyc_aprintf(({const char*_tmp14F="%s_%s_%s_struct";_tag_dyneither(_tmp14F,sizeof(char),16);}),_tag_dyneither(_tmp14E,sizeof(void*),3));});});});});_npop_handler(0);return _tmp154;};_pop_region(r);};}_LL57:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 505
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 508
struct _tuple20 _tmp15A=*env;struct _tuple20 _tmp15B=_tmp15A;struct Cyc_Dict_Dict*_tmp16E;struct _tuple14*_tmp16D;_LL61: _tmp16E=(_tmp15B.f1)->qvar_tags;_tmp16D=_tmp15B.f2;_LL62:;{
struct _tuple14 _tmp15C=*_tmp16D;struct _tuple14 _tmp15D=_tmp15C;struct _tuple1*_tmp16C;struct _tuple1*_tmp16B;_LL64: _tmp16C=_tmp15D.f1;_tmp16B=_tmp15D.f2;_LL65:;{
struct _handler_cons _tmp15E;_push_handler(& _tmp15E);{int _tmp160=0;if(setjmp(_tmp15E.handler))_tmp160=1;if(!_tmp160){{struct _tuple1*_tmp161=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp16E,Cyc_Toc_qvar_tag_cmp,_tmp16D);_npop_handler(0);return _tmp161;};_pop_handler();}else{void*_tmp15F=(void*)_exn_thrown;void*_tmp162=_tmp15F;void*_tmp16A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp162)->tag == Cyc_Dict_Absent){_LL67: _LL68: {
# 512
struct _tuple14*_tmp163=({struct _tuple14*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->f1=_tmp16C;_tmp169->f2=_tmp16B;_tmp169;});
struct _tuple1*_tmp164=_tmp16C;union Cyc_Absyn_Nmspace _tmp168;struct _dyneither_ptr*_tmp167;_LL6C: _tmp168=_tmp164->f1;_tmp167=_tmp164->f2;_LL6D:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp167,_tmp16B);
struct _tuple1*res=({struct _tuple1*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->f1=_tmp168;_tmp165->f2=({struct _dyneither_ptr*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166[0]=newvar;_tmp166;});_tmp165;});
*_tmp16E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp16E,_tmp163,res);
return res;};}}else{_LL69: _tmp16A=_tmp162;_LL6A:(int)_rethrow(_tmp16A);}_LL66:;}};};};}
# 521
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp16F;_tmp16F.f1=fieldname;_tmp16F.f2=dtname;_tmp16F;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 526
void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 530
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 533
struct _tuple21 _tmp170=*env;struct _tuple21 _tmp171=_tmp170;struct Cyc_List_List**_tmp189;struct Cyc_List_List*_tmp188;_LL6F: _tmp189=(_tmp171.f1)->tuple_types;_tmp188=_tmp171.f2;_LL70:;
# 535
{struct Cyc_List_List*_tmp172=*_tmp189;for(0;_tmp172 != 0;_tmp172=_tmp172->tl){
struct _tuple22*_tmp173=(struct _tuple22*)_tmp172->hd;struct _tuple22*_tmp174=_tmp173;void*_tmp177;struct Cyc_List_List*_tmp176;_LL72: _tmp177=_tmp174->f1;_tmp176=_tmp174->f2;_LL73:;{
struct Cyc_List_List*_tmp175=_tmp188;
for(0;_tmp175 != 0  && _tmp176 != 0;(_tmp175=_tmp175->tl,_tmp176=_tmp176->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp175->hd)).f2,(void*)_tmp176->hd))
break;}
if(_tmp175 == 0  && _tmp176 == 0)
return _tmp177;};}}{
# 545
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp187;_tmp187.tag=1;_tmp187.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp185[1]={& _tmp187};Cyc_aprintf(({const char*_tmp186="_tuple%d";_tag_dyneither(_tmp186,sizeof(char),9);}),_tag_dyneither(_tmp185,sizeof(void*),1));});});_tmp184;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp188);
struct Cyc_List_List*_tmp178=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmp178=({struct Cyc_List_List*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->hd=({struct Cyc_Absyn_Aggrfield*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->name=Cyc_Absyn_fieldname(i);_tmp17A->tq=Cyc_Toc_mt_tq;_tmp17A->type=(void*)ts2->hd;_tmp17A->width=0;_tmp17A->attributes=0;_tmp17A->requires_clause=0;_tmp17A;});_tmp179->tl=_tmp178;_tmp179;});}}
_tmp178=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp178);{
struct Cyc_Absyn_Aggrdecl*_tmp17B=({struct Cyc_Absyn_Aggrdecl*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->kind=Cyc_Absyn_StructA;_tmp181->sc=Cyc_Absyn_Public;_tmp181->name=({struct _tuple1*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->f1=
Cyc_Absyn_Rel_n(0);_tmp183->f2=xname;_tmp183;});_tmp181->tvs=0;_tmp181->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->exist_vars=0;_tmp182->rgn_po=0;_tmp182->fields=_tmp178;_tmp182->tagged=0;_tmp182;});_tmp181->attributes=0;_tmp181->expected_mem_kind=0;_tmp181;});
# 560
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp17E;_tmp17E.tag=5;_tmp17E.f1=_tmp17B;_tmp17E;});_tmp17D;}),0);_tmp17C->tl=Cyc_Toc_result_decls;_tmp17C;});
*_tmp189=({struct Cyc_List_List*_tmp17F=_region_malloc(d,sizeof(*_tmp17F));_tmp17F->hd=({struct _tuple22*_tmp180=_region_malloc(d,sizeof(*_tmp180));_tmp180->f1=x;_tmp180->f2=ts;_tmp180;});_tmp17F->tl=*_tmp189;_tmp17F;});
return x;};};}
# 565
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 574
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 582
struct _tuple24 _tmp18A=*env;struct _tuple24 _tmp18B=_tmp18A;struct Cyc_List_List**_tmp1B5;struct _tuple1*_tmp1B4;struct Cyc_List_List*_tmp1B3;struct Cyc_List_List*_tmp1B2;struct Cyc_List_List*_tmp1B1;_LL75: _tmp1B5=(_tmp18B.f1)->abs_struct_types;_tmp1B4=(_tmp18B.f2)->f1;_tmp1B3=(_tmp18B.f2)->f2;_tmp1B2=(_tmp18B.f2)->f3;_tmp1B1=(_tmp18B.f2)->f4;_LL76:;
# 587
{struct Cyc_List_List*_tmp18C=*_tmp1B5;for(0;_tmp18C != 0;_tmp18C=_tmp18C->tl){
struct _tuple25*_tmp18D=(struct _tuple25*)_tmp18C->hd;struct _tuple25*_tmp18E=_tmp18D;struct _tuple1*_tmp196;struct Cyc_List_List*_tmp195;void*_tmp194;_LL78: _tmp196=_tmp18E->f1;_tmp195=_tmp18E->f2;_tmp194=_tmp18E->f3;_LL79:;
# 590
if(Cyc_Absyn_qvar_cmp(_tmp196,_tmp1B4)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1B2)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp195)){
int okay=1;
{struct Cyc_List_List*_tmp18F=_tmp1B2;for(0;_tmp18F != 0;(_tmp18F=_tmp18F->tl,_tmp195=_tmp195->tl)){
void*_tmp190=(void*)_tmp18F->hd;
void*_tmp191=(void*)((struct Cyc_List_List*)_check_null(_tmp195))->hd;
# 597
{struct Cyc_Absyn_Kind*_tmp192=Cyc_Tcutil_typ_kind(_tmp190);struct Cyc_Absyn_Kind*_tmp193=_tmp192;switch(((struct Cyc_Absyn_Kind*)_tmp193)->kind){case Cyc_Absyn_EffKind: _LL7B: _LL7C:
 goto _LL7E;case Cyc_Absyn_RgnKind: _LL7D: _LL7E:
# 602
 continue;default: _LL7F: _LL80:
# 605
 if(Cyc_Tcutil_unify(_tmp190,_tmp191) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp190),Cyc_Toc_typ_to_c(_tmp191)))
# 607
continue;
# 610
okay=0;
goto _LL7A;}_LL7A:;}
# 613
break;}}
# 615
if(okay)
# 617
return _tmp194;}}}{
# 624
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1B0;_tmp1B0.tag=1;_tmp1B0.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp1AE[1]={& _tmp1B0};Cyc_aprintf(({const char*_tmp1AF="_tuple%d";_tag_dyneither(_tmp1AF,sizeof(char),9);}),_tag_dyneither(_tmp1AE,sizeof(void*),1));});});_tmp1AD;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp197=0;
# 628
*_tmp1B5=({struct Cyc_List_List*_tmp198=_region_malloc(d,sizeof(*_tmp198));_tmp198->hd=({struct _tuple25*_tmp199=_region_malloc(d,sizeof(*_tmp199));_tmp199->f1=_tmp1B4;_tmp199->f2=_tmp1B2;_tmp199->f3=x;_tmp199;});_tmp198->tl=*_tmp1B5;_tmp198;});{
# 631
struct _RegionHandle _tmp19A=_new_region("r");struct _RegionHandle*r=& _tmp19A;_push_region(r);
{struct Cyc_List_List*_tmp19B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1B3,_tmp1B2);
for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){
struct Cyc_Absyn_Aggrfield*_tmp19C=(struct Cyc_Absyn_Aggrfield*)_tmp1B1->hd;
void*t=_tmp19C->type;
struct Cyc_List_List*atts=_tmp19C->attributes;
# 639
if(_tmp1B1->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t)))
atts=({struct Cyc_List_List*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp19F;_tmp19F.tag=6;_tmp19F.f1=0;_tmp19F;});_tmp19E;});_tmp19D->tl=atts;_tmp19D;});
# 642
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp19B,t));
# 645
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp1A1;_tmp1A1.tag=8;_tmp1A1.f1=({struct Cyc_Absyn_ArrayInfo _tmp1A2;_tmp1A2.elt_type=Cyc_Absyn_void_star_typ();_tmp1A2.tq=Cyc_Absyn_empty_tqual(0);_tmp1A2.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp1A2.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp1A2.zt_loc=0;_tmp1A2;});_tmp1A1;});_tmp1A0;});
# 649
_tmp197=({struct Cyc_List_List*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->hd=({struct Cyc_Absyn_Aggrfield*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->name=_tmp19C->name;_tmp1A4->tq=Cyc_Toc_mt_tq;_tmp1A4->type=t;_tmp1A4->width=_tmp19C->width;_tmp1A4->attributes=atts;_tmp1A4->requires_clause=0;_tmp1A4;});_tmp1A3->tl=_tmp197;_tmp1A3;});}
# 651
_tmp197=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp197);{
struct Cyc_Absyn_Aggrdecl*_tmp1A5=({struct Cyc_Absyn_Aggrdecl*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->kind=Cyc_Absyn_StructA;_tmp1AA->sc=Cyc_Absyn_Public;_tmp1AA->name=({struct _tuple1*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->f1=
Cyc_Absyn_Rel_n(0);_tmp1AC->f2=xname;_tmp1AC;});_tmp1AA->tvs=0;_tmp1AA->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->exist_vars=0;_tmp1AB->rgn_po=0;_tmp1AB->fields=_tmp197;_tmp1AB->tagged=0;_tmp1AB;});_tmp1AA->attributes=0;_tmp1AA->expected_mem_kind=0;_tmp1AA;});
# 659
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1A8;_tmp1A8.tag=5;_tmp1A8.f1=_tmp1A5;_tmp1A8;});_tmp1A7;}),0);_tmp1A6->tl=Cyc_Toc_result_decls;_tmp1A6;});{
void*_tmp1A9=x;_npop_handler(0);return _tmp1A9;};};}
# 632
;_pop_region(r);};};}
# 663
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 667
struct _tuple23 env=({struct _tuple23 _tmp1B6;_tmp1B6.f1=struct_name;_tmp1B6.f2=type_vars;_tmp1B6.f3=type_args;_tmp1B6.f4=fields;_tmp1B6;});
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 674
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->f1=Cyc_Absyn_Loc_n;_tmp1B7->f2=({struct _dyneither_ptr*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1BB;_tmp1BB.tag=1;_tmp1BB.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;({void*_tmp1B9[1]={& _tmp1BB};Cyc_aprintf(({const char*_tmp1BA="_tmp%X";_tag_dyneither(_tmp1BA,sizeof(char),7);}),_tag_dyneither(_tmp1B9,sizeof(void*),1));});});_tmp1B8;});_tmp1B7;});}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 680
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp1BC=*env;struct _tuple26 _tmp1BD=_tmp1BC;struct Cyc_Xarray_Xarray*_tmp1C5;_LL82: _tmp1C5=(_tmp1BD.f1)->temp_labels;_LL83:;{
int _tmp1BE=Cyc_Toc_fresh_label_counter ++;
if(_tmp1BE < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp1C5))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp1C5,_tmp1BE);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1C4;_tmp1C4.tag=1;_tmp1C4.f1=(unsigned int)_tmp1BE;({void*_tmp1C2[1]={& _tmp1C4};Cyc_aprintf(({const char*_tmp1C3="_LL%X";_tag_dyneither(_tmp1C3,sizeof(char),6);}),_tag_dyneither(_tmp1C2,sizeof(void*),1));});});_tmp1C1;});
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp1C5,res)!= _tmp1BE)
({void*_tmp1BF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1C0="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp1C0,sizeof(char),43);}),_tag_dyneither(_tmp1BF,sizeof(void*),0));});
return res;};};}
# 691
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 698
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp1C6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp1C6))->hd)->name)!= 0){
++ ans;
_tmp1C6=_tmp1C6->tl;}
# 705
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 711
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 716
struct _tuple9 _tmp1C7=*a;struct _tuple9 _tmp1C8=_tmp1C7;struct _dyneither_ptr*_tmp1CC;struct Cyc_Absyn_Tqual _tmp1CB;void*_tmp1CA;_LL85: _tmp1CC=_tmp1C8.f1;_tmp1CB=_tmp1C8.f2;_tmp1CA=_tmp1C8.f3;_LL86:;
return({struct _tuple9*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->f1=_tmp1CC;_tmp1C9->f2=_tmp1CB;_tmp1C9->f3=Cyc_Toc_typ_to_c(_tmp1CA);_tmp1C9;});}
# 719
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1CD=*x;struct _tuple11 _tmp1CE=_tmp1CD;struct Cyc_Absyn_Tqual _tmp1D1;void*_tmp1D0;_LL88: _tmp1D1=_tmp1CE.f1;_tmp1D0=_tmp1CE.f2;_LL89:;
return({struct _tuple11*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->f1=_tmp1D1;_tmp1CF->f2=Cyc_Toc_typ_to_c(_tmp1D0);_tmp1CF;});}
# 738 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1D2=Cyc_Tcutil_compress(t);void*_tmp1D3=_tmp1D2;void*_tmp1D9;void*_tmp1D8;struct Cyc_Absyn_Tqual _tmp1D7;struct Cyc_Absyn_Exp*_tmp1D6;union Cyc_Absyn_Constraint*_tmp1D5;unsigned int _tmp1D4;switch(*((int*)_tmp1D3)){case 8: _LL8B: _tmp1D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D3)->f1).elt_type;_tmp1D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D3)->f1).tq;_tmp1D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D3)->f1).num_elts;_tmp1D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D3)->f1).zero_term;_tmp1D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D3)->f1).zt_loc;_LL8C:
# 741
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp1D8),_tmp1D7);case 1: _LL8D: _tmp1D9=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f2;if(_tmp1D9 != 0){_LL8E:
 return Cyc_Toc_typ_to_c_array(_tmp1D9);}else{goto _LL8F;}default: _LL8F: _LL90:
 return Cyc_Toc_typ_to_c(t);}_LL8A:;}
# 747
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 749
return({struct Cyc_Absyn_Aggrfield*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->name=f->name;_tmp1DA->tq=Cyc_Toc_mt_tq;_tmp1DA->type=
# 751
Cyc_Toc_typ_to_c(f->type);_tmp1DA->width=f->width;_tmp1DA->attributes=f->attributes;_tmp1DA->requires_clause=0;_tmp1DA;});}
# 756
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 758
return;}
# 761
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
cs=({void**_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp1DB;});
# 766
return*cs;}
# 768
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);_tmp1DC;});
# 773
return*r;}
# 775
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_tmp1DD;});
# 780
return*r;}
# 782
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1DE=Cyc_Tcutil_compress(t);void*_tmp1DF=_tmp1DE;struct Cyc_Absyn_Tvar*_tmp1E0;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DF)->tag == 2){_LL92: _tmp1E0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DF)->f1;_LL93:
# 785
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL94: _LL95:
 return 0;}_LL91:;}
# 789
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1E1=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1E2=_tmp1E1;if(((struct Cyc_Absyn_Kind*)_tmp1E2)->kind == Cyc_Absyn_AnyKind){_LL97: _LL98:
 return 1;}else{_LL99: _LL9A:
 return 0;}_LL96:;}
# 796
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1E3=t;struct Cyc_Absyn_Datatypedecl*_tmp22D;void**_tmp22C;struct Cyc_Absyn_Enumdecl*_tmp22B;struct Cyc_Absyn_Aggrdecl*_tmp22A;struct Cyc_Absyn_Exp*_tmp229;struct Cyc_Absyn_Exp*_tmp228;struct _tuple1*_tmp227;struct Cyc_List_List*_tmp226;struct Cyc_Absyn_Typedefdecl*_tmp225;void*_tmp224;struct Cyc_List_List*_tmp223;struct _tuple1*_tmp222;union Cyc_Absyn_AggrInfoU _tmp221;struct Cyc_List_List*_tmp220;enum Cyc_Absyn_AggrKind _tmp21F;struct Cyc_List_List*_tmp21E;struct Cyc_List_List*_tmp21D;struct Cyc_Absyn_Tqual _tmp21C;void*_tmp21B;struct Cyc_List_List*_tmp21A;int _tmp219;struct Cyc_Absyn_VarargInfo*_tmp218;struct Cyc_List_List*_tmp217;void*_tmp216;struct Cyc_Absyn_Tqual _tmp215;struct Cyc_Absyn_Exp*_tmp214;unsigned int _tmp213;void*_tmp212;struct Cyc_Absyn_Tqual _tmp211;union Cyc_Absyn_Constraint*_tmp210;struct Cyc_Absyn_Datatypedecl*_tmp20F;struct Cyc_Absyn_Datatypefield*_tmp20E;struct Cyc_Absyn_Tvar*_tmp20D;void**_tmp20C;switch(*((int*)_tmp1E3)){case 0: _LL9C: _LL9D:
 return t;case 1: _LL9E: _tmp20C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E3)->f2;_LL9F:
# 800
 if(*_tmp20C == 0){
*_tmp20C=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 804
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp20C));case 2: _LLA0: _tmp20D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1E3)->f1;_LLA1:
# 806
 if((Cyc_Tcutil_tvar_kind(_tmp20D,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 809
return(void*)& Cyc_Absyn_VoidType_val;else{
# 811
return Cyc_Absyn_void_star_typ();}case 3: _LLA2: _LLA3:
# 813
 return(void*)& Cyc_Absyn_VoidType_val;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1E3)->f1).field_info).KnownDatatypefield).tag == 2){_LLA4: _tmp20F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1E3)->f1).field_info).KnownDatatypefield).val).f1;_tmp20E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1E3)->f1).field_info).KnownDatatypefield).val).f2;_LLA5:
# 815
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp20E->name,_tmp20F->name));}else{_LLA6: _LLA7:
({void*_tmp1E4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E5="unresolved DatatypeFieldType";_tag_dyneither(_tmp1E5,sizeof(char),29);}),_tag_dyneither(_tmp1E4,sizeof(void*),0));});}case 5: _LLA8: _tmp212=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E3)->f1).elt_typ;_tmp211=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E3)->f1).elt_tq;_tmp210=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E3)->f1).ptr_atts).bounds;_LLA9:
# 820
 _tmp212=Cyc_Toc_typ_to_c(_tmp212);{
void*_tmp1E6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp210);void*_tmp1E7=_tmp1E6;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1E7)->tag == 0){_LLDD: _LLDE:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LLDF: _LLE0:
 return Cyc_Absyn_star_typ(_tmp212,(void*)& Cyc_Absyn_HeapRgn_val,_tmp211,Cyc_Absyn_false_conref);}_LLDC:;};case 6: _LLAA: _LLAB:
# 825
 goto _LLAD;case 7: _LLAC: _LLAD:
 return t;case 8: _LLAE: _tmp216=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E3)->f1).elt_type;_tmp215=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E3)->f1).tq;_tmp214=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E3)->f1).num_elts;_tmp213=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E3)->f1).zt_loc;_LLAF:
# 828
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp216),_tmp215,_tmp214,Cyc_Absyn_false_conref,_tmp213);case 9: _LLB0: _tmp21C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).ret_tqual;_tmp21B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).ret_typ;_tmp21A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).args;_tmp219=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).c_varargs;_tmp218=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).cyc_varargs;_tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).attributes;_LLB1: {
# 834
struct Cyc_List_List*_tmp1E8=0;
for(0;_tmp217 != 0;_tmp217=_tmp217->tl){
void*_tmp1E9=(void*)_tmp217->hd;void*_tmp1EA=_tmp1E9;switch(*((int*)_tmp1EA)){case 4: _LLE2: _LLE3:
 goto _LLE5;case 5: _LLE4: _LLE5:
 goto _LLE7;case 19: _LLE6: _LLE7:
 continue;case 22: _LLE8: _LLE9:
 continue;case 21: _LLEA: _LLEB:
 continue;case 20: _LLEC: _LLED:
 continue;default: _LLEE: _LLEF:
 _tmp1E8=({struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->hd=(void*)_tmp217->hd;_tmp1EB->tl=_tmp1E8;_tmp1EB;});goto _LLE1;}_LLE1:;}{
# 845
struct Cyc_List_List*_tmp1EC=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp21A);
if(_tmp218 != 0){
# 848
void*_tmp1ED=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp218->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp1EE=({struct _tuple9*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->f1=_tmp218->name;_tmp1F0->f2=_tmp218->tq;_tmp1F0->f3=_tmp1ED;_tmp1F0;});
_tmp1EC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1EC,({struct Cyc_List_List*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->hd=_tmp1EE;_tmp1EF->tl=0;_tmp1EF;}));}
# 852
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1F2;_tmp1F2.tag=9;_tmp1F2.f1=({struct Cyc_Absyn_FnInfo _tmp1F3;_tmp1F3.tvars=0;_tmp1F3.effect=0;_tmp1F3.ret_tqual=_tmp21C;_tmp1F3.ret_typ=Cyc_Toc_typ_to_c(_tmp21B);_tmp1F3.args=_tmp1EC;_tmp1F3.c_varargs=_tmp219;_tmp1F3.cyc_varargs=0;_tmp1F3.rgn_po=0;_tmp1F3.attributes=_tmp1E8;_tmp1F3.requires_clause=0;_tmp1F3.requires_relns=0;_tmp1F3.ensures_clause=0;_tmp1F3.ensures_relns=0;_tmp1F3;});_tmp1F2;});_tmp1F1;});};}case 10: _LLB2: _tmp21D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1E3)->f1;_LLB3:
# 857
 _tmp21D=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp21D);
return Cyc_Toc_add_tuple_type(_tmp21D);case 12: _LLB4: _tmp21F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1E3)->f1;_tmp21E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1E3)->f2;_LLB5:
# 862
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1F5;_tmp1F5.tag=12;_tmp1F5.f1=_tmp21F;_tmp1F5.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp21E);_tmp1F5;});_tmp1F4;});case 11: _LLB6: _tmp221=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1E3)->f1).aggr_info;_tmp220=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1E3)->f1).targs;_LLB7:
# 866
{union Cyc_Absyn_AggrInfoU _tmp1F6=_tmp221;if((_tmp1F6.UnknownAggr).tag == 1){_LLF1: _LLF2:
 return t;}else{_LLF3: _LLF4:
 goto _LLF0;}_LLF0:;}{
# 870
struct Cyc_Absyn_Aggrdecl*_tmp1F7=Cyc_Absyn_get_known_aggrdecl(_tmp221);
if(_tmp1F7->expected_mem_kind){
# 873
if(_tmp1F7->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1FB;_tmp1FB.tag=0;_tmp1FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 876
Cyc_Absynpp_qvar2string(_tmp1F7->name));({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0;_tmp1FA.f1=(struct _dyneither_ptr)(
# 875
_tmp1F7->kind == Cyc_Absyn_UnionA?({const char*_tmp1FC="union";_tag_dyneither(_tmp1FC,sizeof(char),6);}):({const char*_tmp1FD="struct";_tag_dyneither(_tmp1FD,sizeof(char),7);}));({void*_tmp1F8[2]={& _tmp1FA,& _tmp1FB};Cyc_Tcutil_warn(0,({const char*_tmp1F9="%s %s was never defined.";_tag_dyneither(_tmp1F9,sizeof(char),25);}),_tag_dyneither(_tmp1F8,sizeof(void*),2));});});});}
# 881
if(_tmp1F7->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1F7->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1FE=_tmp1F7->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F7->impl))->fields;
if(_tmp1FE == 0)return Cyc_Toc_aggrdecl_type(_tmp1F7->name,Cyc_Absyn_strctq);
for(0;_tmp1FE->tl != 0;_tmp1FE=_tmp1FE->tl){;}{
void*_tmp1FF=((struct Cyc_Absyn_Aggrfield*)_tmp1FE->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1FF))){
if(_tmp1F7->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp202;_tmp202.tag=0;_tmp202.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1F7->name));({void*_tmp200[1]={& _tmp202};Cyc_Tcutil_warn(0,({const char*_tmp201="struct %s ended up being abstract.";_tag_dyneither(_tmp201,sizeof(char),35);}),_tag_dyneither(_tmp200,sizeof(void*),1));});});{
# 895
struct _RegionHandle _tmp203=_new_region("r");struct _RegionHandle*r=& _tmp203;_push_region(r);
{struct Cyc_List_List*_tmp204=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1F7->tvs,_tmp220);
void*_tmp205=Cyc_Tcutil_rsubstitute(r,_tmp204,_tmp1FF);
if(Cyc_Toc_is_abstract_type(_tmp205)){void*_tmp206=Cyc_Toc_aggrdecl_type(_tmp1F7->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp206;}{
void*_tmp207=Cyc_Toc_add_struct_type(_tmp1F7->name,_tmp1F7->tvs,_tmp220,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F7->impl))->fields);_npop_handler(0);return _tmp207;};}
# 896
;_pop_region(r);};}
# 901
return Cyc_Toc_aggrdecl_type(_tmp1F7->name,Cyc_Absyn_strctq);};};};case 13: _LLB8: _tmp222=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1E3)->f1;_LLB9:
 return t;case 14: _LLBA: _tmp223=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1E3)->f1;_LLBB:
 Cyc_Toc_enumfields_to_c(_tmp223);return t;case 17: _LLBC: _tmp227=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1E3)->f1;_tmp226=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1E3)->f2;_tmp225=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1E3)->f3;_tmp224=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1E3)->f4;_LLBD:
# 905
 if(_tmp224 == 0  || Cyc_noexpand_r){
if(_tmp226 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp209;_tmp209.tag=17;_tmp209.f1=_tmp227;_tmp209.f2=0;_tmp209.f3=_tmp225;_tmp209.f4=0;_tmp209;});_tmp208;});else{
return t;}}else{
# 910
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp20B;_tmp20B.tag=17;_tmp20B.f1=_tmp227;_tmp20B.f2=0;_tmp20B.f3=_tmp225;_tmp20B.f4=Cyc_Toc_typ_to_c(_tmp224);_tmp20B;});_tmp20A;});}case 19: _LLBE: _LLBF:
 return Cyc_Absyn_uint_typ;case 15: _LLC0: _LLC1:
 return Cyc_Toc_rgn_typ();case 16: _LLC2: _LLC3:
 return Cyc_Toc_dyn_rgn_typ();case 20: _LLC4: _LLC5:
# 916
 goto _LLC7;case 21: _LLC6: _LLC7:
 goto _LLC9;case 22: _LLC8: _LLC9:
 goto _LLCB;case 23: _LLCA: _LLCB:
 goto _LLCD;case 24: _LLCC: _LLCD:
 goto _LLCF;case 25: _LLCE: _LLCF:
 return Cyc_Absyn_void_star_typ();case 18: _LLD0: _tmp228=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1E3)->f1;_LLD1:
# 926
 return t;case 27: _LLD2: _tmp229=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1E3)->f1;_LLD3:
# 928
 return t;case 28: _LLD4: _LLD5:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1E3)->f1)->r)){case 0: _LLD6: _tmp22A=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1E3)->f1)->r)->f1;_LLD7:
# 931
 Cyc_Toc_aggrdecl_to_c(_tmp22A,1);
if(_tmp22A->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp22A->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp22A->name,Cyc_Absyn_strctq);}case 1: _LLD8: _tmp22B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1E3)->f1)->r)->f1;_LLD9:
# 936
 Cyc_Toc_enumdecl_to_c(_tmp22B);
return t;default: _LLDA: _tmp22D=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1E3)->f1)->r)->f1;_tmp22C=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1E3)->f2;_LLDB:
# 939
 Cyc_Toc_datatypedecl_to_c(_tmp22D);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp22C)));}}_LL9B:;}
# 944
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp22E=t;void*_tmp230;struct Cyc_Absyn_Tqual _tmp22F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22E)->tag == 8){_LLF6: _tmp230=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22E)->f1).elt_type;_tmp22F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22E)->f1).tq;_LLF7:
# 947
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp230,(void*)& Cyc_Absyn_HeapRgn_val,_tmp22F,Cyc_Absyn_false_conref),e);}else{_LLF8: _LLF9:
 return Cyc_Toc_cast_it(t,e);}_LLF5:;}
# 954
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp231=Cyc_Tcutil_compress(t);void*_tmp232=_tmp231;struct Cyc_List_List*_tmp23E;struct Cyc_Absyn_Datatypedecl*_tmp23D;struct Cyc_Absyn_Datatypefield*_tmp23C;struct Cyc_List_List*_tmp23B;union Cyc_Absyn_AggrInfoU _tmp23A;void*_tmp239;switch(*((int*)_tmp232)){case 0: _LLFB: _LLFC:
 return 1;case 2: _LLFD: _LLFE:
 return 0;case 6: _LLFF: _LL100:
 goto _LL102;case 13: _LL101: _LL102:
 goto _LL104;case 14: _LL103: _LL104:
 goto _LL106;case 7: _LL105: _LL106:
 goto _LL108;case 9: _LL107: _LL108:
 goto _LL10A;case 19: _LL109: _LL10A:
 return 1;case 8: _LL10B: _tmp239=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp232)->f1).elt_type;_LL10C:
 return Cyc_Toc_atomic_typ(_tmp239);case 11: _LL10D: _tmp23A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp232)->f1).aggr_info;_LL10E:
# 971
{union Cyc_Absyn_AggrInfoU _tmp233=_tmp23A;if((_tmp233.UnknownAggr).tag == 1){_LL120: _LL121:
 return 0;}else{_LL122: _LL123:
 goto _LL11F;}_LL11F:;}{
# 975
struct Cyc_Absyn_Aggrdecl*_tmp234=Cyc_Absyn_get_known_aggrdecl(_tmp23A);
if(_tmp234->impl == 0)
return 0;
{struct Cyc_List_List*_tmp235=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp234->impl))->fields;for(0;_tmp235 != 0;_tmp235=_tmp235->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp235->hd)->type))return 0;}}
return 1;};case 12: _LL10F: _tmp23B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp232)->f2;_LL110:
# 982
 for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp23B->hd)->type))return 0;}
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp232)->f1).field_info).KnownDatatypefield).tag == 2){_LL111: _tmp23D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp232)->f1).field_info).KnownDatatypefield).val).f1;_tmp23C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp232)->f1).field_info).KnownDatatypefield).val).f2;_LL112:
# 986
 _tmp23E=_tmp23C->typs;goto _LL114;}else{goto _LL11D;}case 10: _LL113: _tmp23E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp232)->f1;_LL114:
# 988
 for(0;_tmp23E != 0;_tmp23E=_tmp23E->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp23E->hd)).f2))return 0;}
return 1;case 3: _LL115: _LL116:
# 993
 goto _LL118;case 5: _LL117: _LL118:
 goto _LL11A;case 16: _LL119: _LL11A:
 goto _LL11C;case 15: _LL11B: _LL11C:
 return 0;default: _LL11D: _LL11E:
({struct Cyc_String_pa_PrintArg_struct _tmp238;_tmp238.tag=0;_tmp238.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp236[1]={& _tmp238};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp237="atomic_typ:  bad type %s";_tag_dyneither(_tmp237,sizeof(char),25);}),_tag_dyneither(_tmp236,sizeof(void*),1));});});}_LLFA:;}
# 1001
static int Cyc_Toc_is_void_star(void*t){
void*_tmp23F=Cyc_Tcutil_compress(t);void*_tmp240=_tmp23F;void*_tmp243;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp240)->tag == 5){_LL125: _tmp243=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp240)->f1).elt_typ;_LL126: {
# 1004
void*_tmp241=Cyc_Tcutil_compress(_tmp243);void*_tmp242=_tmp241;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp242)->tag == 0){_LL12A: _LL12B:
 return 1;}else{_LL12C: _LL12D:
 return 0;}_LL129:;}}else{_LL127: _LL128:
# 1008
 return 0;}_LL124:;}
# 1012
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1016
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1021
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp244=Cyc_Tcutil_compress(t);void*_tmp245=_tmp244;struct Cyc_List_List*_tmp251;union Cyc_Absyn_AggrInfoU _tmp250;switch(*((int*)_tmp245)){case 11: _LL12F: _tmp250=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp245)->f1).aggr_info;_LL130: {
# 1024
struct Cyc_Absyn_Aggrdecl*_tmp246=Cyc_Absyn_get_known_aggrdecl(_tmp250);
if(_tmp246->impl == 0)
({void*_tmp247=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp248="is_poly_field: type missing fields";_tag_dyneither(_tmp248,sizeof(char),35);}),_tag_dyneither(_tmp247,sizeof(void*),0));});
_tmp251=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp246->impl))->fields;goto _LL132;}case 12: _LL131: _tmp251=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245)->f2;_LL132: {
# 1029
struct Cyc_Absyn_Aggrfield*_tmp249=Cyc_Absyn_lookup_field(_tmp251,f);
if(_tmp249 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp24C;_tmp24C.tag=0;_tmp24C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp24A[1]={& _tmp24C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp24B="is_poly_field: bad field %s";_tag_dyneither(_tmp24B,sizeof(char),28);}),_tag_dyneither(_tmp24A,sizeof(void*),1));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp249->type);}default: _LL133: _LL134:
({struct Cyc_String_pa_PrintArg_struct _tmp24F;_tmp24F.tag=0;_tmp24F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp24D[1]={& _tmp24F};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp24E="is_poly_field: bad type %s";_tag_dyneither(_tmp24E,sizeof(char),27);}),_tag_dyneither(_tmp24D,sizeof(void*),1));});});}_LL12E:;}
# 1040
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp252=e->r;void*_tmp253=_tmp252;struct Cyc_Absyn_Exp*_tmp25D;struct _dyneither_ptr*_tmp25C;struct Cyc_Absyn_Exp*_tmp25B;struct _dyneither_ptr*_tmp25A;switch(*((int*)_tmp253)){case 20: _LL136: _tmp25B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp253)->f1;_tmp25A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp253)->f2;_LL137:
# 1043
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp25B->topt),_tmp25A) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 21: _LL138: _tmp25D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp253)->f1;_tmp25C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp253)->f2;_LL139: {
# 1046
void*_tmp254=Cyc_Tcutil_compress((void*)_check_null(_tmp25D->topt));void*_tmp255=_tmp254;void*_tmp259;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp255)->tag == 5){_LL13D: _tmp259=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp255)->f1).elt_typ;_LL13E:
# 1048
 return Cyc_Toc_is_poly_field(_tmp259,_tmp25C) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LL13F: _LL140:
({struct Cyc_String_pa_PrintArg_struct _tmp258;_tmp258.tag=0;_tmp258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp25D->topt)));({void*_tmp256[1]={& _tmp258};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp257="is_poly_project: bad type %s";_tag_dyneither(_tmp257,sizeof(char),29);}),_tag_dyneither(_tmp256,sizeof(void*),1));});});}_LL13C:;}default: _LL13A: _LL13B:
# 1051
 return 0;}_LL135:;}
# 1056
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->hd=s;_tmp25E->tl=0;_tmp25E;}),0);}
# 1060
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->hd=s;_tmp25F->tl=0;_tmp25F;}),0);}
# 1064
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1070
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp260[2];_tmp260[1]=s;_tmp260[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp260,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1074
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__fast_region_malloc_e,({struct Cyc_Absyn_Exp*_tmp261[2];_tmp261[1]=s;_tmp261[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp261,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1078
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp262[2];_tmp262[1]=n;_tmp262[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp262,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{
# 1082
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp263[2];_tmp263[1]=n;_tmp263[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp263,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}
# 1085
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp264[3];_tmp264[2]=n;_tmp264[1]=s;_tmp264[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp264,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
# 1090
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265->hd=e;_tmp265->tl=0;_tmp265;}),0);}
# 1093
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,({struct Cyc_List_List*_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266->hd=e;_tmp266->tl=0;_tmp266;}),0);}
# 1097
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,({struct Cyc_Absyn_Exp*_tmp267[0];((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp267,sizeof(struct Cyc_Absyn_Exp*),0));}),0),0);}
# 1103
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1111
int is_string=0;
{void*_tmp268=e->r;void*_tmp269=_tmp268;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp269)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp269)->f1).Wstring_c).tag){case 8: _LL142: _LL143:
 is_string=1;goto _LL141;case 9: _LL144: _LL145:
 is_string=1;goto _LL141;default: goto _LL146;}else{_LL146: _LL147:
 goto _LL141;}_LL141:;}{
# 1117
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp26C;_tmp26C.tag=0;_tmp26C.f1=vd;_tmp26C;});_tmp26B;}),0);_tmp26A->tl=Cyc_Toc_result_decls;_tmp26A;});
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1127
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1129
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1131
struct Cyc_Absyn_Exp*urm_exp;
urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp26D[3];_tmp26D[2]=({struct _tuple19*_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270->f1=0;_tmp270->f2=xplussz;_tmp270;});_tmp26D[1]=({struct _tuple19*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->f1=0;_tmp26F->f2=xexp;_tmp26F;});_tmp26D[0]=({struct _tuple19*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->f1=0;_tmp26E->f2=xexp;_tmp26E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26D,sizeof(struct _tuple19*),3));}),0);
# 1135
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1174 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1176
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp271=nv;int _tmp272;_LL149: _tmp272=_tmp271->toplevel;_LL14A:;
return _tmp272;}
# 1180
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp273=nv;int*_tmp274;_LL14C: _tmp274=_tmp273->in_lhs;_LL14D:;
return*_tmp274;}
# 1186
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp275=_region_malloc(r,sizeof(*_tmp275));_tmp275->break_lab=(struct _dyneither_ptr**)0;_tmp275->continue_lab=(struct _dyneither_ptr**)0;_tmp275->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp275->toplevel=(int)1;_tmp275->in_lhs=(int*)({int*_tmp278=_region_malloc(r,sizeof(*_tmp278));_tmp278[0]=0;_tmp278;});_tmp275->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp275->struct_info).varsizeexp=({struct Cyc_Absyn_Exp**_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277[0]=0;_tmp277;});(_tmp275->struct_info).lhs_exp=0;_tmp275->struct_info;});_tmp275->in_sizeof=(int*)({int*_tmp276=_region_malloc(r,sizeof(*_tmp276));_tmp276[0]=0;_tmp276;});_tmp275->rgn=(struct _RegionHandle*)r;_tmp275;});}
# 1197
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp279=e;struct _dyneither_ptr**_tmp281;struct _dyneither_ptr**_tmp280;struct Cyc_Toc_FallthruInfo*_tmp27F;int _tmp27E;int*_tmp27D;struct Cyc_Toc_StructInfo _tmp27C;int*_tmp27B;_LL14F: _tmp281=_tmp279->break_lab;_tmp280=_tmp279->continue_lab;_tmp27F=_tmp279->fallthru_info;_tmp27E=_tmp279->toplevel;_tmp27D=_tmp279->in_lhs;_tmp27C=_tmp279->struct_info;_tmp27B=_tmp279->in_sizeof;_LL150:;
return({struct Cyc_Toc_Env*_tmp27A=_region_malloc(r,sizeof(*_tmp27A));_tmp27A->break_lab=(struct _dyneither_ptr**)_tmp281;_tmp27A->continue_lab=(struct _dyneither_ptr**)_tmp280;_tmp27A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp27F;_tmp27A->toplevel=(int)_tmp27E;_tmp27A->in_lhs=(int*)_tmp27D;_tmp27A->struct_info=(struct Cyc_Toc_StructInfo)_tmp27C;_tmp27A->in_sizeof=(int*)_tmp27B;_tmp27A->rgn=(struct _RegionHandle*)r;_tmp27A;});}
# 1202
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp282=e;struct _dyneither_ptr**_tmp28A;struct _dyneither_ptr**_tmp289;struct Cyc_Toc_FallthruInfo*_tmp288;int _tmp287;int*_tmp286;struct Cyc_Toc_StructInfo _tmp285;int*_tmp284;_LL152: _tmp28A=_tmp282->break_lab;_tmp289=_tmp282->continue_lab;_tmp288=_tmp282->fallthru_info;_tmp287=_tmp282->toplevel;_tmp286=_tmp282->in_lhs;_tmp285=_tmp282->struct_info;_tmp284=_tmp282->in_sizeof;_LL153:;
return({struct Cyc_Toc_Env*_tmp283=_region_malloc(r,sizeof(*_tmp283));_tmp283->break_lab=(struct _dyneither_ptr**)_tmp28A;_tmp283->continue_lab=(struct _dyneither_ptr**)_tmp289;_tmp283->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp288;_tmp283->toplevel=(int)0;_tmp283->in_lhs=(int*)_tmp286;_tmp283->struct_info=(struct Cyc_Toc_StructInfo)_tmp285;_tmp283->in_sizeof=(int*)_tmp284;_tmp283->rgn=(struct _RegionHandle*)r;_tmp283;});}
# 1206
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp28B=e;struct _dyneither_ptr**_tmp293;struct _dyneither_ptr**_tmp292;struct Cyc_Toc_FallthruInfo*_tmp291;int _tmp290;int*_tmp28F;struct Cyc_Toc_StructInfo _tmp28E;int*_tmp28D;_LL155: _tmp293=_tmp28B->break_lab;_tmp292=_tmp28B->continue_lab;_tmp291=_tmp28B->fallthru_info;_tmp290=_tmp28B->toplevel;_tmp28F=_tmp28B->in_lhs;_tmp28E=_tmp28B->struct_info;_tmp28D=_tmp28B->in_sizeof;_LL156:;
return({struct Cyc_Toc_Env*_tmp28C=_region_malloc(r,sizeof(*_tmp28C));_tmp28C->break_lab=(struct _dyneither_ptr**)_tmp293;_tmp28C->continue_lab=(struct _dyneither_ptr**)_tmp292;_tmp28C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp291;_tmp28C->toplevel=(int)1;_tmp28C->in_lhs=(int*)_tmp28F;_tmp28C->struct_info=(struct Cyc_Toc_StructInfo)_tmp28E;_tmp28C->in_sizeof=(int*)_tmp28D;_tmp28C->rgn=(struct _RegionHandle*)r;_tmp28C;});}
# 1210
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp294=e;int*_tmp295;_LL158: _tmp295=_tmp294->in_lhs;_LL159:;
*_tmp295=b;}
# 1214
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp296=e;int*_tmp298;_LL15B: _tmp298=_tmp296->in_sizeof;_LL15C:;{
int _tmp297=*_tmp298;
*_tmp298=b;
return _tmp297;};}
# 1220
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp299=e;int*_tmp29A;_LL15E: _tmp29A=_tmp299->in_sizeof;_LL15F:;
return*_tmp29A;}
# 1225
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1227
struct Cyc_Toc_Env*_tmp29B=e;struct _dyneither_ptr**_tmp2A3;struct _dyneither_ptr**_tmp2A2;struct Cyc_Toc_FallthruInfo*_tmp2A1;int _tmp2A0;int*_tmp29F;struct Cyc_Absyn_Exp**_tmp29E;int*_tmp29D;_LL161: _tmp2A3=_tmp29B->break_lab;_tmp2A2=_tmp29B->continue_lab;_tmp2A1=_tmp29B->fallthru_info;_tmp2A0=_tmp29B->toplevel;_tmp29F=_tmp29B->in_lhs;_tmp29E=(_tmp29B->struct_info).varsizeexp;_tmp29D=_tmp29B->in_sizeof;_LL162:;
return({struct Cyc_Toc_Env*_tmp29C=_region_malloc(r,sizeof(*_tmp29C));_tmp29C->break_lab=(struct _dyneither_ptr**)_tmp2A3;_tmp29C->continue_lab=(struct _dyneither_ptr**)_tmp2A2;_tmp29C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2A1;_tmp29C->toplevel=(int)_tmp2A0;_tmp29C->in_lhs=(int*)_tmp29F;_tmp29C->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp29C->struct_info).varsizeexp=_tmp29E;(_tmp29C->struct_info).lhs_exp=exp;_tmp29C->struct_info;});_tmp29C->in_sizeof=(int*)_tmp29D;_tmp29C->rgn=(struct _RegionHandle*)r;_tmp29C;});}
# 1233
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp2A4=e;struct _dyneither_ptr**_tmp2AC;struct _dyneither_ptr**_tmp2AB;struct Cyc_Toc_FallthruInfo*_tmp2AA;int _tmp2A9;int*_tmp2A8;struct Cyc_Toc_StructInfo _tmp2A7;int*_tmp2A6;_LL164: _tmp2AC=_tmp2A4->break_lab;_tmp2AB=_tmp2A4->continue_lab;_tmp2AA=_tmp2A4->fallthru_info;_tmp2A9=_tmp2A4->toplevel;_tmp2A8=_tmp2A4->in_lhs;_tmp2A7=_tmp2A4->struct_info;_tmp2A6=_tmp2A4->in_sizeof;_LL165:;
return({struct Cyc_Toc_Env*_tmp2A5=_region_malloc(r,sizeof(*_tmp2A5));_tmp2A5->break_lab=(struct _dyneither_ptr**)0;_tmp2A5->continue_lab=(struct _dyneither_ptr**)0;_tmp2A5->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2AA;_tmp2A5->toplevel=(int)_tmp2A9;_tmp2A5->in_lhs=(int*)_tmp2A8;_tmp2A5->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A7;_tmp2A5->in_sizeof=(int*)_tmp2A6;_tmp2A5->rgn=(struct _RegionHandle*)r;_tmp2A5;});}
# 1239
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1243
struct Cyc_Toc_Env*_tmp2AD=e;struct _dyneither_ptr**_tmp2B7;struct _dyneither_ptr**_tmp2B6;struct Cyc_Toc_FallthruInfo*_tmp2B5;int _tmp2B4;int*_tmp2B3;struct Cyc_Toc_StructInfo _tmp2B2;int*_tmp2B1;_LL167: _tmp2B7=_tmp2AD->break_lab;_tmp2B6=_tmp2AD->continue_lab;_tmp2B5=_tmp2AD->fallthru_info;_tmp2B4=_tmp2AD->toplevel;_tmp2B3=_tmp2AD->in_lhs;_tmp2B2=_tmp2AD->struct_info;_tmp2B1=_tmp2AD->in_sizeof;_LL168:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp2B0=_region_malloc(r,sizeof(*_tmp2B0));_tmp2B0->label=fallthru_l;_tmp2B0->binders=fallthru_binders;_tmp2B0;});
# 1246
return({struct Cyc_Toc_Env*_tmp2AE=_region_malloc(r,sizeof(*_tmp2AE));_tmp2AE->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2AF=_region_malloc(r,sizeof(*_tmp2AF));_tmp2AF[0]=break_l;_tmp2AF;});_tmp2AE->continue_lab=(struct _dyneither_ptr**)_tmp2B6;_tmp2AE->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi;_tmp2AE->toplevel=(int)_tmp2B4;_tmp2AE->in_lhs=(int*)_tmp2B3;_tmp2AE->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B2;_tmp2AE->in_sizeof=(int*)_tmp2B1;_tmp2AE->rgn=(struct _RegionHandle*)r;_tmp2AE;});};}
# 1249
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1251
struct Cyc_Toc_Env*_tmp2B8=e;struct _dyneither_ptr**_tmp2C1;struct _dyneither_ptr**_tmp2C0;struct Cyc_Toc_FallthruInfo*_tmp2BF;int _tmp2BE;int*_tmp2BD;struct Cyc_Toc_StructInfo _tmp2BC;int*_tmp2BB;_LL16A: _tmp2C1=_tmp2B8->break_lab;_tmp2C0=_tmp2B8->continue_lab;_tmp2BF=_tmp2B8->fallthru_info;_tmp2BE=_tmp2B8->toplevel;_tmp2BD=_tmp2B8->in_lhs;_tmp2BC=_tmp2B8->struct_info;_tmp2BB=_tmp2B8->in_sizeof;_LL16B:;
return({struct Cyc_Toc_Env*_tmp2B9=_region_malloc(r,sizeof(*_tmp2B9));_tmp2B9->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2BA=_region_malloc(r,sizeof(*_tmp2BA));_tmp2BA[0]=break_l;_tmp2BA;});_tmp2B9->continue_lab=(struct _dyneither_ptr**)_tmp2C0;_tmp2B9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp2B9->toplevel=(int)_tmp2BE;_tmp2B9->in_lhs=(int*)_tmp2BD;_tmp2B9->struct_info=(struct Cyc_Toc_StructInfo)_tmp2BC;_tmp2B9->in_sizeof=(int*)_tmp2BB;_tmp2B9->rgn=(struct _RegionHandle*)r;_tmp2B9;});}
# 1258
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1261
struct Cyc_Toc_Env*_tmp2C2=e;struct _dyneither_ptr**_tmp2CB;struct _dyneither_ptr**_tmp2CA;struct Cyc_Toc_FallthruInfo*_tmp2C9;int _tmp2C8;int*_tmp2C7;struct Cyc_Toc_StructInfo _tmp2C6;int*_tmp2C5;_LL16D: _tmp2CB=_tmp2C2->break_lab;_tmp2CA=_tmp2C2->continue_lab;_tmp2C9=_tmp2C2->fallthru_info;_tmp2C8=_tmp2C2->toplevel;_tmp2C7=_tmp2C2->in_lhs;_tmp2C6=_tmp2C2->struct_info;_tmp2C5=_tmp2C2->in_sizeof;_LL16E:;
return({struct Cyc_Toc_Env*_tmp2C3=_region_malloc(r,sizeof(*_tmp2C3));_tmp2C3->break_lab=(struct _dyneither_ptr**)0;_tmp2C3->continue_lab=(struct _dyneither_ptr**)_tmp2CA;_tmp2C3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2C4=_region_malloc(r,sizeof(*_tmp2C4));_tmp2C4->label=next_l;_tmp2C4->binders=0;_tmp2C4;});_tmp2C3->toplevel=(int)_tmp2C8;_tmp2C3->in_lhs=(int*)_tmp2C7;_tmp2C3->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C6;_tmp2C3->in_sizeof=(int*)_tmp2C5;_tmp2C3->rgn=(struct _RegionHandle*)r;_tmp2C3;});}
# 1274 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1277
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1279
void*_tmp2CC=e->annot;void*_tmp2CD=_tmp2CC;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2CD)->tag == Cyc_CfFlowInfo_UnknownZ){_LL170: _LL171:
# 1282
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2CD)->tag == Cyc_CfFlowInfo_NotZero){_LL172: _LL173:
# 1285
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2CD)->tag == Cyc_CfFlowInfo_IsZero){_LL174: _LL175:
# 1287
({void*_tmp2CE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2CF="dereference of NULL pointer";_tag_dyneither(_tmp2CF,sizeof(char),28);}),_tag_dyneither(_tmp2CE,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2CD)->tag == Cyc_Absyn_EmptyAnnot){_LL176: _LL177:
# 1293
 return 1;}else{_LL178: _LL179:
({void*_tmp2D0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2D1="need_null_check";_tag_dyneither(_tmp2D1,sizeof(char),16);}),_tag_dyneither(_tmp2D0,sizeof(void*),0));});}}}}_LL16F:;}
# 1298
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2D2=e->annot;void*_tmp2D3=_tmp2D2;struct Cyc_List_List*_tmp2D9;struct Cyc_List_List*_tmp2D8;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2D3)->tag == Cyc_CfFlowInfo_UnknownZ){_LL17B: _tmp2D8=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2D3)->f1;_LL17C:
 return _tmp2D8;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2D3)->tag == Cyc_CfFlowInfo_NotZero){_LL17D: _tmp2D9=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2D3)->f1;_LL17E:
 return _tmp2D9;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2D3)->tag == Cyc_CfFlowInfo_IsZero){_LL17F: _LL180:
# 1303
({void*_tmp2D4=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2D5="dereference of NULL pointer";_tag_dyneither(_tmp2D5,sizeof(char),28);}),_tag_dyneither(_tmp2D4,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2D3)->tag == Cyc_Absyn_EmptyAnnot){_LL181: _LL182:
 return 0;}else{_LL183: _LL184:
({void*_tmp2D6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2D7="get_relns";_tag_dyneither(_tmp2D7,sizeof(char),10);}),_tag_dyneither(_tmp2D6,sizeof(void*),0));});}}}}_LL17A:;}static char _tmp2DD[8]="*bogus*";
# 1313
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1323 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1326
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2DA=a->r;void*_tmp2DB=_tmp2DA;void*_tmp2DE;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2DB)->tag == 1){_LL186: _tmp2DE=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2DB)->f1;_LL187: {
# 1329
struct Cyc_Absyn_Vardecl*_tmp2DC=Cyc_Tcutil_nonesc_vardecl(_tmp2DE);
if(_tmp2DC == 0)goto _LL189;
x=_tmp2DC;
goto _LL185;}}else{_LL188: _LL189: {
# 1335
static struct _dyneither_ptr bogus_string={_tmp2DD,_tmp2DD,_tmp2DD + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1338
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1344
x=& bogus_vardecl;
x->type=a_typ;}}_LL185:;}{
# 1347
void*_tmp2DF=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1352
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2DF);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1361
if(valid_rop_i){
struct Cyc_List_List*_tmp2E0=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1369
if(!Cyc_Relations_consistent_relations(_tmp2E0))return 1;}
# 1372
inner_loop: {
void*_tmp2E1=i->r;void*_tmp2E2=_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E6;void*_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E4;switch(*((int*)_tmp2E2)){case 13: _LL18B: _tmp2E5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E2)->f1;_tmp2E4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E2)->f2;_LL18C:
 i=_tmp2E4;goto inner_loop;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E2)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E2)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E2)->f2)->tl != 0){_LL18D: _tmp2E7=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E2)->f2)->hd;_tmp2E6=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E2)->f2)->tl)->hd;_LL18E: {
# 1378
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp2E6,& rop_z)){
# 1381
struct Cyc_List_List*_tmp2E3=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1383
return !Cyc_Relations_consistent_relations(_tmp2E3);}
# 1385
goto _LL18A;}}else{goto _LL18F;}}else{goto _LL18F;}}else{goto _LL18F;}default: _LL18F: _LL190:
 goto _LL18A;}_LL18A:;}
# 1388
return 0;};}
# 1391
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2E8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E9="Missing type in primop ";_tag_dyneither(_tmp2E9,sizeof(char),24);}),_tag_dyneither(_tmp2E8,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1395
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2EA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2EB="Missing type in primop ";_tag_dyneither(_tmp2EB,sizeof(char),24);}),_tag_dyneither(_tmp2EA,sizeof(void*),0));});
return(void*)_check_null(e->topt);}
# 1399
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple11*_tmp2EC=_cycalloc(sizeof(*_tmp2EC));_tmp2EC->f1=Cyc_Toc_mt_tq;_tmp2EC->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp2EC;});}
# 1402
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);
return({struct _tuple19*_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED->f1=0;_tmp2ED->f2=e;_tmp2ED;});}
# 1409
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1412
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2EE=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp2EE=Cyc_Absyn_add_exp(_tmp2EE,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1420
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp2EE): Cyc_Toc_malloc_ptr(_tmp2EE);else{
# 1424
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp2EE);}}else{
# 1429
t=struct_typ;
eo=0;}
# 1432
if(do_declare){
void*_tmp2EF=x->r;void*_tmp2F0=_tmp2EF;void*_tmp2F3;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F0)->tag == 1){_LL192: _tmp2F3=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F0)->f1;_LL193:
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(Cyc_Absyn_binding2qvar(_tmp2F3),t,eo,s,0),0);}else{_LL194: _LL195:
({void*_tmp2F1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2F2="bogus x to make_struct";_tag_dyneither(_tmp2F2,sizeof(char),23);}),_tag_dyneither(_tmp2F1,sizeof(void*),0));});}_LL191:;}else{
# 1439
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1442
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1446
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp2F4=Cyc_Tcutil_compress(typ);void*_tmp2F5=_tmp2F4;struct Cyc_List_List*_tmp2F8;union Cyc_Absyn_AggrInfoU _tmp2F7;struct Cyc_List_List*_tmp2F6;switch(*((int*)_tmp2F5)){case 11: _LL197: _tmp2F7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F5)->f1).aggr_info;_tmp2F6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F5)->f1).targs;_LL198: {
# 1449
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp2F7);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp2F8=aggrfields;goto _LL19A;};}case 12: _LL199: _tmp2F8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F5)->f2;_LL19A:
# 1454
 if(_tmp2F8 == 0)return 0;
return 1;default: _LL19B: _LL19C:
# 1460
 return 0;}_LL196:;}
# 1464
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp2F9=Cyc_Tcutil_compress(t);void*_tmp2FA=_tmp2F9;void*_tmp2FE;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2FA)->tag == 8){_LL19E: _tmp2FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2FA)->f1).elt_type;_LL19F:
 return _tmp2FE;}else{_LL1A0: _LL1A1:
({struct Cyc_String_pa_PrintArg_struct _tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp2FB[1]={& _tmp2FD};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2FC="array_elt_type called on %s";_tag_dyneither(_tmp2FC,sizeof(char),28);}),_tag_dyneither(_tmp2FB,sizeof(void*),1));});});}_LL19D:;}
# 1471
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1477
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1482
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1487
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp2FF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp2FF != 0;_tmp2FF=_tmp2FF->tl){
# 1491
struct _tuple19*_tmp300=(struct _tuple19*)_tmp2FF->hd;struct _tuple19*_tmp301=_tmp300;struct Cyc_List_List*_tmp313;struct Cyc_Absyn_Exp*_tmp312;_LL1A3: _tmp313=_tmp301->f1;_tmp312=_tmp301->f2;_LL1A4:;{
# 1495
struct Cyc_Absyn_Exp*e_index;
if(_tmp313 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1499
if(_tmp313->tl != 0)({void*_tmp302=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp303="multiple designators in array";_tag_dyneither(_tmp303,sizeof(char),30);}),_tag_dyneither(_tmp302,sizeof(void*),0));});{
void*_tmp304=(void*)_tmp313->hd;
void*_tmp305=_tmp304;struct Cyc_Absyn_Exp*_tmp308;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp305)->tag == 0){_LL1A6: _tmp308=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp305)->f1;_LL1A7:
# 1503
 Cyc_Toc_exp_to_c(nv,_tmp308);
e_index=_tmp308;
goto _LL1A5;}else{_LL1A8: _LL1A9:
({void*_tmp306=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp307="field name designators in array";_tag_dyneither(_tmp307,sizeof(char),32);}),_tag_dyneither(_tmp306,sizeof(void*),0));});}_LL1A5:;};}{
# 1509
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp309=_tmp312->r;void*_tmp30A=_tmp309;void*_tmp311;struct Cyc_List_List*_tmp310;struct Cyc_Absyn_Vardecl*_tmp30F;struct Cyc_Absyn_Exp*_tmp30E;struct Cyc_Absyn_Exp*_tmp30D;int _tmp30C;struct Cyc_List_List*_tmp30B;switch(*((int*)_tmp30A)){case 25: _LL1AB: _tmp30B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_LL1AC:
# 1512
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp30B,s);
goto _LL1AA;case 26: _LL1AD: _tmp30F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp30E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp30D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30A)->f3;_tmp30C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30A)->f4;_LL1AE:
# 1515
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp30F,_tmp30E,_tmp30D,_tmp30C,s,0);
# 1517
goto _LL1AA;case 29: _LL1AF: _tmp311=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp310=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL1B0:
# 1519
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp311,_tmp310,s);
goto _LL1AA;case 27: _LL1B1: _LL1B2:
# 1522
 goto _LL1AA;default: _LL1B3: _LL1B4:
# 1524
 Cyc_Toc_exp_to_c(nv,_tmp312);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp312),0),s,0);
goto _LL1AA;}_LL1AA:;};};}}
# 1530
return s;}
# 1535
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1540
struct _tuple1*_tmp314=vd->name;
void*expected_elt_type;
{void*_tmp315=Cyc_Tcutil_compress(comprehension_type);void*_tmp316=_tmp315;void*_tmp31A;void*_tmp319;switch(*((int*)_tmp316)){case 8: _LL1B6: _tmp319=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp316)->f1).elt_type;_LL1B7:
# 1544
 expected_elt_type=_tmp319;goto _LL1B5;case 5: _LL1B8: _tmp31A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp316)->f1).elt_typ;_LL1B9:
# 1546
 expected_elt_type=_tmp31A;goto _LL1B5;default: _LL1BA: _LL1BB:
({void*_tmp317=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp318="init_comprehension passed a bad type!";_tag_dyneither(_tmp318,sizeof(char),38);}),_tag_dyneither(_tmp317,sizeof(void*),0));});}_LL1B5:;}{
# 1549
void*_tmp31B=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1553
{void*_tmp31C=e2->r;void*_tmp31D=_tmp31C;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp31D)->tag == 27){_LL1BD: _LL1BE:
 return Cyc_Absyn_skip_stmt(0);}else{_LL1BF: _LL1C0:
 goto _LL1BC;}_LL1BC:;}{
# 1557
struct _RegionHandle _tmp31E=_new_region("r2");struct _RegionHandle*r2=& _tmp31E;_push_region(r2);
{struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp314,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp314,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp314,0),0);
# 1564
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp314,0),0);
struct Cyc_Absyn_Stmt*body;
# 1568
{void*_tmp31F=e2->r;void*_tmp320=_tmp31F;void*_tmp327;struct Cyc_List_List*_tmp326;struct Cyc_Absyn_Vardecl*_tmp325;struct Cyc_Absyn_Exp*_tmp324;struct Cyc_Absyn_Exp*_tmp323;int _tmp322;struct Cyc_List_List*_tmp321;switch(*((int*)_tmp320)){case 25: _LL1C2: _tmp321=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp320)->f1;_LL1C3:
# 1570
 body=Cyc_Toc_init_array(nv,expected_elt_type,lval,_tmp321,Cyc_Toc_skip_stmt_dl());
goto _LL1C1;case 26: _LL1C4: _tmp325=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp320)->f1;_tmp324=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp320)->f2;_tmp323=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp320)->f3;_tmp322=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp320)->f4;_LL1C5:
# 1573
 body=Cyc_Toc_init_comprehension(nv,expected_elt_type,lval,_tmp325,_tmp324,_tmp323,_tmp322,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1C1;case 29: _LL1C6: _tmp327=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp320)->f1;_tmp326=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp320)->f2;_LL1C7:
# 1576
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp327,_tmp326,Cyc_Toc_skip_stmt_dl());
goto _LL1C1;default: _LL1C8: _LL1C9:
# 1579
 Cyc_Toc_exp_to_c(nv,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL1C1;}_LL1C1:;}{
# 1583
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1585
if(zero_term){
# 1590
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp31B,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1595
struct Cyc_Absyn_Stmt*_tmp328=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp314,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp328;};};}
# 1558
;_pop_region(r2);};};}
# 1601
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1604
{struct Cyc_List_List*_tmp329=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp329 != 0;_tmp329=_tmp329->tl){
struct _tuple19*_tmp32A=(struct _tuple19*)_tmp329->hd;struct _tuple19*_tmp32B=_tmp32A;struct Cyc_List_List*_tmp33F;struct Cyc_Absyn_Exp*_tmp33E;_LL1CB: _tmp33F=_tmp32B->f1;_tmp33E=_tmp32B->f2;_LL1CC:;
if(_tmp33F == 0)
({void*_tmp32C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp32D="empty designator list";_tag_dyneither(_tmp32D,sizeof(char),22);}),_tag_dyneither(_tmp32C,sizeof(void*),0));});
if(_tmp33F->tl != 0)
({void*_tmp32E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp32F="too many designators in anonymous struct";_tag_dyneither(_tmp32F,sizeof(char),41);}),_tag_dyneither(_tmp32E,sizeof(void*),0));});{
void*_tmp330=(void*)_tmp33F->hd;void*_tmp331=_tmp330;struct _dyneither_ptr*_tmp33D;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp331)->tag == 1){_LL1CE: _tmp33D=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp331)->f1;_LL1CF: {
# 1612
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp33D,0);
{void*_tmp332=_tmp33E->r;void*_tmp333=_tmp332;void*_tmp33A;struct Cyc_List_List*_tmp339;struct Cyc_Absyn_Vardecl*_tmp338;struct Cyc_Absyn_Exp*_tmp337;struct Cyc_Absyn_Exp*_tmp336;int _tmp335;struct Cyc_List_List*_tmp334;switch(*((int*)_tmp333)){case 25: _LL1D3: _tmp334=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp333)->f1;_LL1D4:
# 1615
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp33E->topt),lval,_tmp334,s);goto _LL1D2;case 26: _LL1D5: _tmp338=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp333)->f1;_tmp337=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp333)->f2;_tmp336=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp333)->f3;_tmp335=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp333)->f4;_LL1D6:
# 1617
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp33E->topt),lval,_tmp338,_tmp337,_tmp336,_tmp335,s,0);
goto _LL1D2;case 29: _LL1D7: _tmp33A=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp333)->f1;_tmp339=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp333)->f2;_LL1D8:
# 1620
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp33A,_tmp339,s);goto _LL1D2;case 27: _LL1D9: _LL1DA:
# 1622
 goto _LL1D2;default: _LL1DB: _LL1DC:
# 1624
 Cyc_Toc_exp_to_c(nv,_tmp33E);
# 1626
if(Cyc_Toc_is_poly_field(struct_type,_tmp33D))
_tmp33E=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp33E);
# 1629
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp33E,0),0),s,0);
goto _LL1D2;}_LL1D2:;}
# 1632
goto _LL1CD;}}else{_LL1D0: _LL1D1:
({void*_tmp33B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp33C="array designator in struct";_tag_dyneither(_tmp33C,sizeof(char),27);}),_tag_dyneither(_tmp33B,sizeof(void*),0));});}_LL1CD:;};}}
# 1636
return s;}
# 1641
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1645
struct _RegionHandle _tmp340=_new_region("r");struct _RegionHandle*r=& _tmp340;_push_region(r);
{struct Cyc_List_List*_tmp341=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp342=Cyc_Toc_add_tuple_type(_tmp341);
# 1649
struct _tuple1*_tmp343=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp344=Cyc_Absyn_var_exp(_tmp343,0);
struct Cyc_Absyn_Stmt*_tmp345=Cyc_Absyn_exp_stmt(_tmp344,0);
# 1653
struct Cyc_Absyn_Exp*(*_tmp346)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1655
int is_atomic=1;
struct Cyc_List_List*_tmp347=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp347);for(0;_tmp347 != 0;(_tmp347=_tmp347->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp347->hd;
struct Cyc_Absyn_Exp*lval=_tmp346(_tmp344,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp348=e->r;void*_tmp349=_tmp348;struct Cyc_Absyn_Vardecl*_tmp34E;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_Absyn_Exp*_tmp34C;int _tmp34B;struct Cyc_List_List*_tmp34A;switch(*((int*)_tmp349)){case 25: _LL1DE: _tmp34A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp349)->f1;_LL1DF:
# 1663
 _tmp345=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp34A,_tmp345);
goto _LL1DD;case 26: _LL1E0: _tmp34E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp349)->f1;_tmp34D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp349)->f2;_tmp34C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp349)->f3;_tmp34B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp349)->f4;_LL1E1:
# 1666
 _tmp345=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp34E,_tmp34D,_tmp34C,_tmp34B,_tmp345,0);
# 1668
goto _LL1DD;case 27: _LL1E2: _LL1E3:
# 1670
 goto _LL1DD;default: _LL1E4: _LL1E5:
# 1673
 Cyc_Toc_exp_to_c(nv,e);
_tmp345=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp346(_tmp344,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp345,0);
# 1676
goto _LL1DD;}_LL1DD:;};}}{
# 1679
struct Cyc_Absyn_Exp*_tmp34F=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp343,0),_tmp342,_tmp345,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp34F;};}
# 1646
;_pop_region(r);}
# 1683
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp350=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp350 != 0;_tmp350=_tmp350->tl){
struct Cyc_Absyn_Aggrfield*_tmp351=(struct Cyc_Absyn_Aggrfield*)_tmp350->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp351->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1690
({void*_tmp352=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp355;_tmp355.tag=0;_tmp355.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp353[1]={& _tmp355};Cyc_aprintf(({const char*_tmp354="get_member_offset %s failed";_tag_dyneither(_tmp354,sizeof(char),28);}),_tag_dyneither(_tmp353,sizeof(void*),1));});}),_tag_dyneither(_tmp352,sizeof(void*),0));});}
# 1693
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp356=Cyc_Tcutil_compress(*typ);void*_tmp357=_tmp356;void*_tmp35F;struct Cyc_Absyn_Tqual _tmp35E;struct Cyc_Absyn_Exp*_tmp35D;union Cyc_Absyn_Constraint*_tmp35C;unsigned int _tmp35B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp357)->tag == 8){_LL1E7: _tmp35F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp357)->f1).elt_type;_tmp35E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp357)->f1).tq;_tmp35D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp357)->f1).num_elts;_tmp35C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp357)->f1).zero_term;_tmp35B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp357)->f1).zt_loc;_LL1E8:
# 1698
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp35D))){
*typ=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp359;_tmp359.tag=8;_tmp359.f1=({struct Cyc_Absyn_ArrayInfo _tmp35A;_tmp35A.elt_type=_tmp35F;_tmp35A.tq=_tmp35E;_tmp35A.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp35A.zero_term=_tmp35C;_tmp35A.zt_loc=_tmp35B;_tmp35A;});_tmp359;});_tmp358;});
return 1;}
# 1702
goto _LL1E6;}else{_LL1E9: _LL1EA:
 goto _LL1E6;}_LL1E6:;}
# 1705
return 0;}
# 1708
static int Cyc_Toc_is_array_type(void*t){
void*_tmp360=Cyc_Tcutil_compress(t);void*_tmp361=_tmp360;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp361)->tag == 8){_LL1EC: _LL1ED:
 return 1;}else{_LL1EE: _LL1EF:
 return 0;}_LL1EB:;}
# 1715
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1717
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1721
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp362=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp363=_tmp362->attributes;
void*_tmp364=_tmp362->type;
if((!Cyc_Toc_is_array_type(_tmp364),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp364))))
_tmp363=({struct Cyc_List_List*_tmp365=_cycalloc(sizeof(*_tmp365));_tmp365->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp367;_tmp367.tag=6;_tmp367.f1=0;_tmp367;});_tmp366;});_tmp365->tl=_tmp363;_tmp365;});{
struct Cyc_Absyn_Aggrfield*_tmp368=({struct Cyc_Absyn_Aggrfield*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->name=_tmp362->name;_tmp36A->tq=Cyc_Toc_mt_tq;_tmp36A->type=new_field_type;_tmp36A->width=_tmp362->width;_tmp36A->attributes=_tmp363;_tmp36A->requires_clause=0;_tmp36A;});
# 1733
new_fields=({struct Cyc_List_List*_tmp369=_cycalloc(sizeof(*_tmp369));_tmp369->hd=_tmp368;_tmp369->tl=new_fields;_tmp369;});};}else{
# 1735
new_fields=({struct Cyc_List_List*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp36B->tl=new_fields;_tmp36B;});}}{
# 1737
struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp374;_tmp374.tag=1;_tmp374.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp372[1]={& _tmp374};Cyc_aprintf(({const char*_tmp373="_genStruct%d";_tag_dyneither(_tmp373,sizeof(char),13);}),_tag_dyneither(_tmp372,sizeof(void*),1));});});_tmp371;});
# 1740
struct _tuple1*qv=({struct _tuple1*_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370->f1=Cyc_Absyn_Abs_n(0,1);_tmp370->f2=name;_tmp370;});
struct Cyc_Absyn_AggrdeclImpl*_tmp36D=({struct Cyc_Absyn_AggrdeclImpl*_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;_tmp36F->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;_tmp36F->fields=_tmp36C;_tmp36F->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;_tmp36F;});
# 1745
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E->kind=ad->kind;_tmp36E->sc=ad->sc;_tmp36E->name=qv;_tmp36E->tvs=ad->tvs;_tmp36E->impl=_tmp36D;_tmp36E->expected_mem_kind=0;_tmp36E->attributes=ad->attributes;_tmp36E;});
# 1752
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1756
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1762
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp375=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp375,0);
do_declare=1;}else{
# 1770
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1773
struct Cyc_Absyn_Stmt*_tmp376=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1777
struct Cyc_Absyn_Exp**varexp;
{void*_tmp377=_tmp376->r;void*_tmp378=_tmp377;struct Cyc_Absyn_Exp**_tmp37B;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp378)->tag == 1){_LL1F1: _tmp37B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp378)->f1;_LL1F2:
 varexp=_tmp37B;goto _LL1F0;}else{_LL1F3: _LL1F4:
({void*_tmp379=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp37A="stmt not an expression!";_tag_dyneither(_tmp37A,sizeof(char),24);}),_tag_dyneither(_tmp379,sizeof(void*),0));});}_LL1F0:;}{
# 1783
struct Cyc_Absyn_Exp*(*_tmp37C)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1787
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp37D=Cyc_Tcutil_compress(struct_type);void*_tmp37E=_tmp37D;union Cyc_Absyn_AggrInfoU _tmp382;struct Cyc_List_List*_tmp381;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37E)->tag == 11){_LL1F6: _tmp382=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37E)->f1).aggr_info;_tmp381=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37E)->f1).targs;_LL1F7:
# 1794
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp382);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp381;
goto _LL1F5;}else{_LL1F8: _LL1F9:
({void*_tmp37F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp380="init_struct: bad struct type";_tag_dyneither(_tmp380,sizeof(char),29);}),_tag_dyneither(_tmp37F,sizeof(void*),0));});}_LL1F5:;}
# 1801
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp383=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp383->tl != 0;_tmp383=_tmp383->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp384=(struct Cyc_Absyn_Aggrfield*)_tmp383->hd;
struct _RegionHandle _tmp385=_new_region("temp");struct _RegionHandle*temp=& _tmp385;_push_region(temp);
{struct Cyc_List_List*_tmp386=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1810
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp386,_tmp384->type))){
struct Cyc_List_List*_tmp387=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp388=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp386,_tmp387);
# 1814
struct Cyc_List_List*new_fields=0;
for(_tmp383=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp383 != 0;_tmp383=_tmp383->tl){
struct Cyc_Absyn_Aggrfield*_tmp389=(struct Cyc_Absyn_Aggrfield*)_tmp383->hd;
struct Cyc_Absyn_Aggrfield*_tmp38A=({struct Cyc_Absyn_Aggrfield*_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D->name=_tmp389->name;_tmp38D->tq=Cyc_Toc_mt_tq;_tmp38D->type=
# 1819
Cyc_Tcutil_rsubstitute(temp,_tmp388,_tmp389->type);_tmp38D->width=_tmp389->width;_tmp38D->attributes=_tmp389->attributes;_tmp38D->requires_clause=0;_tmp38D;});
# 1827
if(_tmp383->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp38A->type))
index=({struct _dyneither_ptr**_tmp38B=_cycalloc(sizeof(*_tmp38B));_tmp38B[0]=_tmp38A->name;_tmp38B;});}
# 1834
new_fields=({struct Cyc_List_List*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C->hd=_tmp38A;_tmp38C->tl=new_fields;_tmp38C;});}
# 1836
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp399;_tmp399.tag=1;_tmp399.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp397[1]={& _tmp399};Cyc_aprintf(({const char*_tmp398="_genStruct%d";_tag_dyneither(_tmp398,sizeof(char),13);}),_tag_dyneither(_tmp397,sizeof(void*),1));});});_tmp396;});
# 1840
struct _tuple1*qv=({struct _tuple1*_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395->f1=Cyc_Absyn_Abs_n(0,1);_tmp395->f2=name;_tmp395;});
struct Cyc_Absyn_AggrdeclImpl*_tmp38E=({struct Cyc_Absyn_AggrdeclImpl*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->exist_vars=0;_tmp394->rgn_po=0;_tmp394->fields=aggrfields;_tmp394->tagged=0;_tmp394;});
# 1845
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->kind=Cyc_Absyn_StructA;_tmp393->sc=Cyc_Absyn_Public;_tmp393->name=qv;_tmp393->tvs=0;_tmp393->expected_mem_kind=0;_tmp393->impl=_tmp38E;_tmp393->attributes=0;_tmp393;});
# 1852
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp390;_tmp390.tag=11;_tmp390.f1=({struct Cyc_Absyn_AggrInfo _tmp391;_tmp391.aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392[0]=new_ad;_tmp392;}));_tmp391.targs=0;_tmp391;});_tmp390;});_tmp38F;});
# 1856
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1806
;_pop_region(temp);};}{
# 1860
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp39A=_new_region("r");struct _RegionHandle*r=& _tmp39A;_push_region(r);
{struct Cyc_List_List*_tmp39B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp39B != 0;_tmp39B=_tmp39B->tl){
struct _tuple19*_tmp39C=(struct _tuple19*)_tmp39B->hd;struct _tuple19*_tmp39D=_tmp39C;struct Cyc_List_List*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C1;_LL1FB: _tmp3C2=_tmp39D->f1;_tmp3C1=_tmp39D->f2;_LL1FC:;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3C1->topt));
if(_tmp3C2 == 0)
({void*_tmp39E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp39F="empty designator list";_tag_dyneither(_tmp39F,sizeof(char),22);}),_tag_dyneither(_tmp39E,sizeof(void*),0));});
if(_tmp3C2->tl != 0){
# 1870
struct _tuple1*_tmp3A0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A1=Cyc_Absyn_var_exp(_tmp3A0,0);
for(0;_tmp3C2 != 0;_tmp3C2=_tmp3C2->tl){
void*_tmp3A2=(void*)_tmp3C2->hd;void*_tmp3A3=_tmp3A2;struct _dyneither_ptr*_tmp3A6;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A3)->tag == 1){_LL1FE: _tmp3A6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A3)->f1;_LL1FF:
# 1876
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3A6))
_tmp3A1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A1);
_tmp376=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp37C(xexp,_tmp3A6,0),_tmp3A1,0),0),_tmp376,0);
# 1880
goto _LL1FD;}else{_LL200: _LL201:
({void*_tmp3A4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3A5="array designator in struct";_tag_dyneither(_tmp3A5,sizeof(char),27);}),_tag_dyneither(_tmp3A4,sizeof(void*),0));});}_LL1FD:;}
# 1884
Cyc_Toc_exp_to_c(nv,_tmp3C1);
_tmp376=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A1,_tmp3C1,0),0),_tmp376,0);}else{
# 1887
void*_tmp3A7=(void*)_tmp3C2->hd;void*_tmp3A8=_tmp3A7;struct _dyneither_ptr*_tmp3C0;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A8)->tag == 1){_LL203: _tmp3C0=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A8)->f1;_LL204: {
# 1889
struct Cyc_Absyn_Exp*lval=_tmp37C(xexp,_tmp3C0,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3C0);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp376=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp376,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1898
int e1_translated=0;
{void*_tmp3A9=_tmp3C1->r;void*_tmp3AA=_tmp3A9;void*_tmp3BD;struct Cyc_List_List*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BB;void*_tmp3BA;struct Cyc_Absyn_Vardecl*_tmp3B9;struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B7;int _tmp3B6;struct Cyc_List_List*_tmp3B5;switch(*((int*)_tmp3AA)){case 25: _LL208: _tmp3B5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_LL209:
# 1901
 _tmp376=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3C0)))->type,lval,_tmp3B5,_tmp376);
# 1903
goto _LL207;case 26: _LL20A: _tmp3B9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_tmp3B8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AA)->f2;_tmp3B7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AA)->f3;_tmp3B6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AA)->f4;_LL20B:
# 1905
 _tmp376=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3C0)))->type,lval,_tmp3B9,_tmp3B8,_tmp3B7,_tmp3B6,_tmp376,0);
# 1909
e1_translated=1;
_tmp3BB=_tmp3B8;_tmp3BA=(void*)_check_null(_tmp3B7->topt);goto _LL20D;case 27: _LL20C: _tmp3BB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_tmp3BA=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3AA)->f2;_LL20D:
# 1914
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3C0)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3BB);{
void*_tmp3AB=Cyc_Toc_typ_to_c(_tmp3BA);
*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3AC[2];_tmp3AC[1]=
Cyc_Absyn_sizeoftyp_exp(_tmp3AB,0);_tmp3AC[0]=_tmp3BB;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3AC,sizeof(struct Cyc_Absyn_Exp*),2));}),0),
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3AD;(_tmp3AD.Int_c).val=({struct _tuple6 _tmp3AE;_tmp3AE.f1=Cyc_Absyn_Unsigned;_tmp3AE.f2=(int)sizeof(double);_tmp3AE;});(_tmp3AD.Int_c).tag=5;_tmp3AD;}),0),0);};}
# 1922
goto _LL207;case 29: _LL20E: _tmp3BD=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_tmp3BC=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3AA)->f2;_LL20F:
# 1924
 _tmp376=Cyc_Toc_init_anon_struct(nv,lval,_tmp3BD,_tmp3BC,_tmp376);goto _LL207;default: _LL210: _LL211: {
# 1926
void*old_e_typ=(void*)_check_null(_tmp3C1->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3C1);
{void*_tmp3AF=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3AF)->tag == 11){_LL213: _LL214:
# 1933
 if(old_e_typ != _tmp3C1->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3C1->topt))){
# 1937
ad=Cyc_Toc_update_aggr_type(ad,_tmp3C0,(void*)_check_null(_tmp3C1->topt));
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3B1;_tmp3B1.tag=11;_tmp3B1.f1=({struct Cyc_Absyn_AggrInfo _tmp3B2;_tmp3B2.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3[0]=ad;_tmp3B3;}));_tmp3B2.targs=0;_tmp3B2;});_tmp3B1;});_tmp3B0;});
# 1941
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1943
goto _LL212;}else{_LL215: _LL216:
 goto _LL212;}_LL212:;}{
# 1946
struct Cyc_Absyn_Aggrfield*_tmp3B4=Cyc_Absyn_lookup_field(aggrfields,_tmp3C0);
# 1948
if(Cyc_Toc_is_poly_field(struct_type,_tmp3C0) && !was_ptr_type)
_tmp3C1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3C1);
# 1951
if(exist_types != 0)
_tmp3C1=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3B4))->type),_tmp3C1);
# 1954
_tmp376=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3C1,0),0),_tmp376,0);
goto _LL207;};}}_LL207:;}
# 1957
goto _LL202;};}}else{_LL205: _LL206:
({void*_tmp3BE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3BF="array designator in struct";_tag_dyneither(_tmp3BF,sizeof(char),27);}),_tag_dyneither(_tmp3BE,sizeof(void*),0));});}_LL202:;}}}
# 1964
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 1969
struct Cyc_Absyn_Exp*_tmp3C3=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp376,pointer,rgnopt,is_atomic,do_declare);
# 1971
_tmp3C3->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3C4=_tmp3C3;_npop_handler(0);return _tmp3C4;};};
# 1862
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1977
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1980
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1983
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5[0]=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3C6;_tmp3C6.tag=4;_tmp3C6.f1=e1;_tmp3C6.f2=incr;_tmp3C6;});_tmp3C5;}),0);}
# 1987
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1996
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1998
void*_tmp3C7=e1->r;void*_tmp3C8=_tmp3C7;struct Cyc_Absyn_Exp*_tmp3D0;struct _dyneither_ptr*_tmp3CF;int _tmp3CE;int _tmp3CD;void*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Stmt*_tmp3CA;switch(*((int*)_tmp3C8)){case 36: _LL218: _tmp3CA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1;_LL219:
 Cyc_Toc_lvalue_assign_stmt(_tmp3CA,fs,f,f_env);goto _LL217;case 13: _LL21A: _tmp3CC=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1;_tmp3CB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C8)->f2;_LL21B:
 Cyc_Toc_lvalue_assign(_tmp3CB,fs,f,f_env);goto _LL217;case 20: _LL21C: _tmp3D0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1;_tmp3CF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C8)->f2;_tmp3CE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C8)->f3;_tmp3CD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C8)->f4;_LL21D:
# 2003
 e1->r=_tmp3D0->r;
Cyc_Toc_lvalue_assign(e1,({struct Cyc_List_List*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->hd=_tmp3CF;_tmp3C9->tl=fs;_tmp3C9;}),f,f_env);
goto _LL217;default: _LL21E: _LL21F: {
# 2011
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2013
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL217;}}_LL217:;}
# 2019
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2021
void*_tmp3D1=s->r;void*_tmp3D2=_tmp3D1;struct Cyc_Absyn_Stmt*_tmp3D9;struct Cyc_Absyn_Decl*_tmp3D8;struct Cyc_Absyn_Stmt*_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D6;switch(*((int*)_tmp3D2)){case 1: _LL221: _tmp3D6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3D2)->f1;_LL222:
 Cyc_Toc_lvalue_assign(_tmp3D6,fs,f,f_env);goto _LL220;case 12: _LL223: _tmp3D8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3D2)->f1;_tmp3D7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3D2)->f2;_LL224:
# 2024
 Cyc_Toc_lvalue_assign_stmt(_tmp3D7,fs,f,f_env);goto _LL220;case 2: _LL225: _tmp3D9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3D2)->f2;_LL226:
 Cyc_Toc_lvalue_assign_stmt(_tmp3D9,fs,f,f_env);goto _LL220;default: _LL227: _LL228:
({struct Cyc_String_pa_PrintArg_struct _tmp3D5;_tmp3D5.tag=0;_tmp3D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3D3[1]={& _tmp3D5};Cyc_Toc_toc_impos(({const char*_tmp3D4="lvalue_assign_stmt: %s";_tag_dyneither(_tmp3D4,sizeof(char),23);}),_tag_dyneither(_tmp3D3,sizeof(void*),1));});});}_LL220:;}
# 2030
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2034
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3DA=e->r;void*_tmp3DB=_tmp3DA;struct Cyc_Absyn_Stmt*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E1;void**_tmp3E0;struct Cyc_Absyn_Exp**_tmp3DF;switch(*((int*)_tmp3DB)){case 13: _LL22A: _tmp3E0=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;_tmp3DF=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DB)->f2;_LL22B:
# 2037
*_tmp3DF=Cyc_Toc_push_address_exp(*_tmp3DF);
*_tmp3E0=Cyc_Absyn_cstar_typ(*_tmp3E0,Cyc_Toc_mt_tq);
return e;case 19: _LL22C: _tmp3E1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;_LL22D:
# 2041
 return _tmp3E1;case 36: _LL22E: _tmp3E2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3DB)->f1;_LL22F:
# 2045
 Cyc_Toc_push_address_stmt(_tmp3E2);
return e;default: _LL230: _LL231:
# 2048
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);
({struct Cyc_String_pa_PrintArg_struct _tmp3DE;_tmp3DE.tag=0;_tmp3DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp3DC[1]={& _tmp3DE};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3DD="can't take & of exp %s";_tag_dyneither(_tmp3DD,sizeof(char),23);}),_tag_dyneither(_tmp3DC,sizeof(void*),1));});});}_LL229:;}
# 2053
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3E3=s->r;void*_tmp3E4=_tmp3E3;struct Cyc_Absyn_Exp**_tmp3EA;struct Cyc_Absyn_Stmt*_tmp3E9;struct Cyc_Absyn_Stmt*_tmp3E8;switch(*((int*)_tmp3E4)){case 2: _LL233: _tmp3E8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3E4)->f2;_LL234:
 _tmp3E9=_tmp3E8;goto _LL236;case 12: _LL235: _tmp3E9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3E4)->f2;_LL236:
 Cyc_Toc_push_address_stmt(_tmp3E9);goto _LL232;case 1: _LL237: _tmp3EA=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3E4)->f1;_LL238:
*_tmp3EA=Cyc_Toc_push_address_exp(*_tmp3EA);goto _LL232;default: _LL239: _LL23A:
# 2059
({struct Cyc_String_pa_PrintArg_struct _tmp3E7;_tmp3E7.tag=0;_tmp3E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3E5[1]={& _tmp3E7};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3E6="can't take & of stmt %s";_tag_dyneither(_tmp3E6,sizeof(char),24);}),_tag_dyneither(_tmp3E5,sizeof(void*),1));});});}_LL232:;}
# 2064
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2066
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2068
if(x == 0)return 0;
result=({struct Cyc_List_List*_tmp3EB=_region_malloc(r2,sizeof(*_tmp3EB));_tmp3EB->hd=f(x->hd,env);_tmp3EB->tl=0;_tmp3EB;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmp3EC=_region_malloc(r2,sizeof(*_tmp3EC));_tmp3EC->hd=f(x->hd,env);_tmp3EC->tl=0;_tmp3EC;});
prev=prev->tl;}
# 2075
return result;}
# 2077
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2081
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED->f1=0;_tmp3ED->f2=e;_tmp3ED;});}
# 2085
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3EE=Cyc_Tcutil_compress(t);void*_tmp3EF=_tmp3EE;struct Cyc_Absyn_PtrInfo _tmp3F2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EF)->tag == 5){_LL23C: _tmp3F2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EF)->f1;_LL23D:
 return _tmp3F2;}else{_LL23E: _LL23F:
({void*_tmp3F0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3F1="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3F1,sizeof(char),28);}),_tag_dyneither(_tmp3F0,sizeof(void*),0));});}_LL23B:;}
# 2095
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3F3=Cyc_Tcutil_compress(t);void*_tmp3F4=_tmp3F3;int _tmp400;enum Cyc_Absyn_Sign _tmp3FF;enum Cyc_Absyn_Sign _tmp3FE;enum Cyc_Absyn_Sign _tmp3FD;enum Cyc_Absyn_Sign _tmp3FC;enum Cyc_Absyn_Sign _tmp3FB;switch(*((int*)_tmp3F4)){case 5: _LL241: _LL242:
 res=Cyc_Absyn_null_exp(0);goto _LL240;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4)->f2){case Cyc_Absyn_Char_sz: _LL243: _tmp3FB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4)->f1;_LL244:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3FB,'\000'),0);goto _LL240;case Cyc_Absyn_Short_sz: _LL245: _tmp3FC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4)->f1;_LL246:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3FC,0),0);goto _LL240;case Cyc_Absyn_Int_sz: _LL24B: _tmp3FD=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4)->f1;_LL24C:
# 2103
 _tmp3FE=_tmp3FD;goto _LL24E;case Cyc_Absyn_Long_sz: _LL24D: _tmp3FE=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4)->f1;_LL24E:
# 2105
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3FE,0),0);goto _LL240;default: _LL24F: _tmp3FF=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4)->f1;_LL250:
# 2107
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3FF,(long long)0),0);goto _LL240;}case 13: _LL247: _LL248:
# 2101
 goto _LL24A;case 14: _LL249: _LL24A:
 _tmp3FD=Cyc_Absyn_Unsigned;goto _LL24C;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F4)->f1){case 0: _LL251: _LL252:
# 2108
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F5="0.0F";_tag_dyneither(_tmp3F5,sizeof(char),5);}),0),0);goto _LL240;case 1: _LL253: _LL254:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F6="0.0";_tag_dyneither(_tmp3F6,sizeof(char),4);}),1),0);goto _LL240;default: _LL255: _tmp400=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F4)->f1;_LL256:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F7="0.0L";_tag_dyneither(_tmp3F7,sizeof(char),5);}),_tmp400),0);goto _LL240;}default: _LL257: _LL258:
# 2112
({struct Cyc_String_pa_PrintArg_struct _tmp3FA;_tmp3FA.tag=0;_tmp3FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp3F8[1]={& _tmp3FA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3F9="found non-zero type %s in generate_zero";_tag_dyneither(_tmp3F9,sizeof(char),40);}),_tag_dyneither(_tmp3F8,sizeof(void*),1));});});}_LL240:;}
# 2114
res->topt=t;
return res;}
# 2121
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2134 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp401=Cyc_Toc_typ_to_c(elt_type);
void*_tmp402=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp403=Cyc_Absyn_cstar_typ(_tmp401,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp404=({struct Cyc_Core_Opt*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->v=_tmp403;_tmp43B;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp405=e1->r;void*_tmp406=_tmp405;struct Cyc_Absyn_Exp*_tmp40C;struct Cyc_Absyn_Exp*_tmp40B;struct Cyc_Absyn_Exp*_tmp40A;switch(*((int*)_tmp406)){case 19: _LL25A: _tmp40A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp406)->f1;_LL25B:
# 2142
 if(!is_dyneither){
_tmp40A=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp40A,0,Cyc_Absyn_Other_coercion,0);
_tmp40A->topt=fat_ptr_type;}
# 2146
Cyc_Toc_exp_to_c(nv,_tmp40A);xinit=_tmp40A;goto _LL259;case 22: _LL25C: _tmp40C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp406)->f1;_tmp40B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp406)->f2;_LL25D:
# 2148
 if(!is_dyneither){
_tmp40C=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp40C,0,Cyc_Absyn_Other_coercion,0);
_tmp40C->topt=fat_ptr_type;}
# 2152
Cyc_Toc_exp_to_c(nv,_tmp40C);Cyc_Toc_exp_to_c(nv,_tmp40B);
xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp407[3];_tmp407[2]=_tmp40B;_tmp407[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp407[0]=_tmp40C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp407,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
goto _LL259;default: _LL25E: _LL25F:
({void*_tmp408=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp409="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp409,sizeof(char),53);}),_tag_dyneither(_tmp408,sizeof(void*),0));});}_LL259:;}{
# 2158
struct _tuple1*_tmp40D=Cyc_Toc_temp_var();
struct _RegionHandle _tmp40E=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp40E;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp40F=({struct Cyc_Absyn_Vardecl*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->sc=Cyc_Absyn_Public;_tmp43A->name=_tmp40D;_tmp43A->varloc=0;_tmp43A->tq=Cyc_Toc_mt_tq;_tmp43A->type=_tmp402;_tmp43A->initializer=xinit;_tmp43A->rgn=0;_tmp43A->attributes=0;_tmp43A->escapes=0;_tmp43A;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp410=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp439;_tmp439.tag=4;_tmp439.f1=_tmp40F;_tmp439;});_tmp438;});
struct Cyc_Absyn_Exp*_tmp411=Cyc_Absyn_varb_exp((void*)_tmp410,0);
_tmp411->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp412=Cyc_Absyn_deref_exp(_tmp411,0);
_tmp412->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp412);{
struct _tuple1*_tmp413=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp414=({struct Cyc_Absyn_Vardecl*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->sc=Cyc_Absyn_Public;_tmp437->name=_tmp413;_tmp437->varloc=0;_tmp437->tq=Cyc_Toc_mt_tq;_tmp437->type=_tmp401;_tmp437->initializer=_tmp412;_tmp437->rgn=0;_tmp437->attributes=0;_tmp437->escapes=0;_tmp437;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp415=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp436;_tmp436.tag=4;_tmp436.f1=_tmp414;_tmp436;});_tmp435;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp416=Cyc_Absyn_varb_exp((void*)_tmp415,0);
_tmp416->topt=_tmp412->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp416,e2,0);
z_init->topt=_tmp416->topt;}
# 2177
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp417=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp418=({struct Cyc_Absyn_Vardecl*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->sc=Cyc_Absyn_Public;_tmp434->name=_tmp417;_tmp434->varloc=0;_tmp434->tq=Cyc_Toc_mt_tq;_tmp434->type=_tmp401;_tmp434->initializer=z_init;_tmp434->rgn=0;_tmp434->attributes=0;_tmp434->escapes=0;_tmp434;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp419=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp433;_tmp433.tag=4;_tmp433.f1=_tmp418;_tmp433;});_tmp432;});
# 2183
struct Cyc_Absyn_Exp*_tmp41A=Cyc_Absyn_varb_exp((void*)_tmp415,0);_tmp41A->topt=_tmp412->topt;{
struct Cyc_Absyn_Exp*_tmp41B=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp41A,_tmp41B,0);
_tmp41C->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp41C);{
# 2189
struct Cyc_Absyn_Exp*_tmp41D=Cyc_Absyn_varb_exp((void*)_tmp419,0);_tmp41D->topt=_tmp412->topt;{
struct Cyc_Absyn_Exp*_tmp41E=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp41D,_tmp41E,0);
_tmp41F->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp41F);{
# 2195
struct Cyc_List_List*_tmp420=({struct Cyc_Absyn_Exp*_tmp431[2];_tmp431[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp431[0]=
# 2195
Cyc_Absyn_varb_exp((void*)_tmp410,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp431,sizeof(struct Cyc_Absyn_Exp*),2));});
# 2197
struct Cyc_Absyn_Exp*_tmp421=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp420,0),_tmp421,0);{
# 2202
struct Cyc_Absyn_Exp*_tmp422=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp41C,_tmp41F,0),0);
struct Cyc_Absyn_Stmt*_tmp423=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp424=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp((void*)_tmp410,0),Cyc_Toc_curr_sp,0);
_tmp424=Cyc_Toc_cast_it(_tmp403,_tmp424);{
struct Cyc_Absyn_Exp*_tmp425=Cyc_Absyn_deref_exp(_tmp424,0);
struct Cyc_Absyn_Exp*_tmp426=Cyc_Absyn_assign_exp(_tmp425,Cyc_Absyn_var_exp(_tmp417,0),0);
struct Cyc_Absyn_Stmt*_tmp427=Cyc_Absyn_exp_stmt(_tmp426,0);
_tmp427=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp422,_tmp423,Cyc_Absyn_skip_stmt(0),0),_tmp427,0);
_tmp427=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp42A;_tmp42A.tag=0;_tmp42A.f1=_tmp418;_tmp42A;});_tmp429;});_tmp428->loc=0;_tmp428;}),_tmp427,0);
_tmp427=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp42D;_tmp42D.tag=0;_tmp42D.f1=_tmp414;_tmp42D;});_tmp42C;});_tmp42B->loc=0;_tmp42B;}),_tmp427,0);
_tmp427=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp430;_tmp430.tag=0;_tmp430.f1=_tmp40F;_tmp430;});_tmp42F;});_tmp42E->loc=0;_tmp42E;}),_tmp427,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp427);};};};};};};};};};}
# 2160
;_pop_region(rgn2);};}
# 2228 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2232
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp43C=Cyc_Tcutil_compress(aggrtype);void*_tmp43D=_tmp43C;union Cyc_Absyn_AggrInfoU _tmp442;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43D)->tag == 11){_LL261: _tmp442=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43D)->f1).aggr_info;_LL262:
# 2235
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp442);goto _LL260;}else{_LL263: _LL264:
({void*_tmp43E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp441;_tmp441.tag=0;_tmp441.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));({void*_tmp43F[1]={& _tmp441};Cyc_aprintf(({const char*_tmp440="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp440,sizeof(char),51);}),_tag_dyneither(_tmp43F,sizeof(void*),1));});}),_tag_dyneither(_tmp43E,sizeof(void*),0));});}_LL260:;}{
# 2239
struct _tuple1*_tmp443=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp444=Cyc_Absyn_var_exp(_tmp443,0);
struct Cyc_Absyn_Exp*_tmp445=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp446=Cyc_Absyn_aggrarrow_exp(_tmp444,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp447=Cyc_Absyn_neq_exp(_tmp446,_tmp445,0);
struct Cyc_Absyn_Exp*_tmp448=Cyc_Absyn_aggrarrow_exp(_tmp444,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp449=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp448,0),0);
struct Cyc_Absyn_Stmt*_tmp44A=Cyc_Absyn_ifthenelse_stmt(_tmp447,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp44B=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp44D=Cyc_Absyn_declare_stmt(_tmp443,_tmp44B,_tmp44C,Cyc_Absyn_seq_stmt(_tmp44A,_tmp449,0),0);
return Cyc_Toc_stmt_exp_r(_tmp44D);}else{
# 2253
struct Cyc_Absyn_Exp*_tmp44E=Cyc_Toc_member_exp(aggrproj(_tmp444,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_neq_exp(_tmp44E,_tmp445,0);
struct Cyc_Absyn_Exp*_tmp450=Cyc_Toc_member_exp(aggrproj(_tmp444,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp451=Cyc_Absyn_exp_stmt(_tmp450,0);
struct Cyc_Absyn_Stmt*_tmp452=Cyc_Absyn_ifthenelse_stmt(_tmp44F,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp453=Cyc_Absyn_declare_stmt(_tmp443,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp452,_tmp451,0),0);
return Cyc_Toc_stmt_exp_r(_tmp453);}};}
# 2263
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2266
void*_tmp454=Cyc_Tcutil_compress(t);void*_tmp455=_tmp454;union Cyc_Absyn_AggrInfoU _tmp45C;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp455)->tag == 11){_LL266: _tmp45C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp455)->f1).aggr_info;_LL267: {
# 2268
struct Cyc_Absyn_Aggrdecl*_tmp456=Cyc_Absyn_get_known_aggrdecl(_tmp45C);
*f_tag=Cyc_Toc_get_member_offset(_tmp456,f);{
# 2271
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp45B;_tmp45B.tag=0;_tmp45B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp45A;_tmp45A.tag=0;_tmp45A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp456->name).f2);({void*_tmp458[2]={& _tmp45A,& _tmp45B};Cyc_aprintf(({const char*_tmp459="_union_%s_%s";_tag_dyneither(_tmp459,sizeof(char),13);}),_tag_dyneither(_tmp458,sizeof(void*),2));});});});
*tagged_member_type=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457[0]=str;_tmp457;}));
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp456->impl))->tagged;};}}else{_LL268: _LL269:
 return 0;}_LL265:;}
# 2282
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2285
void*_tmp45D=e->r;void*_tmp45E=_tmp45D;struct Cyc_Absyn_Exp*_tmp46A;struct _dyneither_ptr*_tmp469;int*_tmp468;struct Cyc_Absyn_Exp*_tmp467;struct _dyneither_ptr*_tmp466;int*_tmp465;struct Cyc_Absyn_Exp*_tmp464;switch(*((int*)_tmp45E)){case 13: _LL26B: _tmp464=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp45E)->f2;_LL26C:
({void*_tmp45F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp460="cast on lhs!";_tag_dyneither(_tmp460,sizeof(char),13);}),_tag_dyneither(_tmp45F,sizeof(void*),0));});case 20: _LL26D: _tmp467=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45E)->f1;_tmp466=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45E)->f2;_tmp465=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45E)->f4;_LL26E:
# 2288
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp467->topt),_tmp466,f_tag,tagged_member_type,clear_read,_tmp465);case 21: _LL26F: _tmp46A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45E)->f1;_tmp469=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45E)->f2;_tmp468=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45E)->f4;_LL270: {
# 2291
void*_tmp461=Cyc_Tcutil_compress((void*)_check_null(_tmp46A->topt));void*_tmp462=_tmp461;void*_tmp463;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp462)->tag == 5){_LL274: _tmp463=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp462)->f1).elt_typ;_LL275:
# 2293
 return Cyc_Toc_is_tagged_union_project_impl(_tmp463,_tmp469,f_tag,tagged_member_type,clear_read,_tmp468);}else{_LL276: _LL277:
# 2295
 return 0;}_LL273:;}default: _LL271: _LL272:
# 2297
 return 0;}_LL26A:;}
# 2310 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2314
struct _tuple1*_tmp46B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp46B,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2324
struct Cyc_Absyn_Exp*_tmp46C=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp46C,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2327
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp46B,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2333
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2348 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp46D=Cyc_Absyn_var_exp(max,0);
# 2356
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp46D=Cyc_Absyn_add_exp(_tmp46D,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2367
s=Cyc_Absyn_skip_stmt(0);}}{
# 2369
struct _RegionHandle _tmp46E=_new_region("r");struct _RegionHandle*r=& _tmp46E;_push_region(r);
{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp482=_region_malloc(r,sizeof(*_tmp482));_tmp482->hd=({struct _tuple28*_tmp483=_region_malloc(r,sizeof(*_tmp483));_tmp483->f1=max;_tmp483->f2=Cyc_Absyn_uint_typ;_tmp483->f3=e1;_tmp483;});_tmp482->tl=0;_tmp482;});
# 2372
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp46F[2];_tmp46F[1]=_tmp46D;_tmp46F[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46F,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{
# 2379
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp470[2];_tmp470[1]=_tmp46D;_tmp470[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp470,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{
# 2385
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
decls=({struct Cyc_List_List*_tmp471=_region_malloc(r,sizeof(*_tmp471));_tmp471->hd=({struct _tuple28*_tmp472=_region_malloc(r,sizeof(*_tmp472));_tmp472->f1=a;_tmp472->f2=ptr_typ;_tmp472->f3=ainit;_tmp472;});_tmp471->tl=decls;_tmp471;});
if(!Cyc_Tcutil_is_pointer_type(old_typ))
({void*_tmp473=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp474="new_comprehension_to_c: comprehension is not an array type";_tag_dyneither(_tmp474,sizeof(char),59);}),_tag_dyneither(_tmp473,sizeof(void*),0));});
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp475=Cyc_Toc_temp_var();
void*_tmp476=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp477=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp478=
Cyc_Absyn_fncall_exp(_tmp477,({struct Cyc_Absyn_Exp*_tmp47B[3];_tmp47B[2]=_tmp46D;_tmp47B[1]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp47B[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp47B,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 2397
decls=({struct Cyc_List_List*_tmp479=_region_malloc(r,sizeof(*_tmp479));_tmp479->hd=({struct _tuple28*_tmp47A=_region_malloc(r,sizeof(*_tmp47A));_tmp47A->f1=_tmp475;_tmp47A->f2=_tmp476;_tmp47A->f3=_tmp478;_tmp47A;});_tmp479->tl=decls;_tmp479;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp475,0),0),0);}else{
# 2400
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp47C=decls;for(0;_tmp47C != 0;_tmp47C=_tmp47C->tl){
struct _tuple28 _tmp47D=*((struct _tuple28*)_tmp47C->hd);struct _tuple28 _tmp47E=_tmp47D;struct _tuple1*_tmp481;void*_tmp480;struct Cyc_Absyn_Exp*_tmp47F;_LL279: _tmp481=_tmp47E.f1;_tmp480=_tmp47E.f2;_tmp47F=_tmp47E.f3;_LL27A:;
s=Cyc_Absyn_declare_stmt(_tmp481,_tmp480,_tmp47F,s,0);}}
# 2405
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2370
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2409
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp484=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp487;_tmp487.tag=0;_tmp487.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp485[1]={& _tmp487};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp486="exp_to_c: no type for %s";_tag_dyneither(_tmp486,sizeof(char),25);}),_tag_dyneither(_tmp485,sizeof(void*),1));});});
# 2415
if((nv->struct_info).lhs_exp != 0){
void*_tmp488=_tmp484;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp488)->tag == 28){_LL27C: _LL27D:
 goto _LL27B;}else{_LL27E: _LL27F:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}_LL27B:;}{
# 2423
void*old_typ=(void*)_check_null(e->topt);
void*_tmp489=_tmp484;struct Cyc_Absyn_Stmt*_tmp6A4;struct Cyc_Absyn_Exp*_tmp6A3;struct _dyneither_ptr*_tmp6A2;struct Cyc_Absyn_Exp*_tmp6A1;struct Cyc_Absyn_Exp*_tmp6A0;int _tmp69F;struct Cyc_Absyn_Exp*_tmp69E;void**_tmp69D;struct Cyc_Absyn_Exp*_tmp69C;int _tmp69B;int _tmp69A;struct Cyc_List_List*_tmp699;struct Cyc_Absyn_Datatypedecl*_tmp698;struct Cyc_Absyn_Datatypefield*_tmp697;void*_tmp696;struct Cyc_List_List*_tmp695;struct _tuple1*_tmp694;struct Cyc_List_List*_tmp693;struct Cyc_List_List*_tmp692;struct Cyc_Absyn_Aggrdecl*_tmp691;struct Cyc_Absyn_Exp*_tmp690;void*_tmp68F;int _tmp68E;struct Cyc_Absyn_Vardecl*_tmp68D;struct Cyc_Absyn_Exp*_tmp68C;struct Cyc_Absyn_Exp*_tmp68B;int _tmp68A;struct Cyc_List_List*_tmp689;struct Cyc_List_List*_tmp688;struct Cyc_Absyn_Exp*_tmp687;struct Cyc_Absyn_Exp*_tmp686;struct Cyc_Absyn_Exp*_tmp685;struct _dyneither_ptr*_tmp684;int _tmp683;int _tmp682;struct Cyc_Absyn_Exp*_tmp681;struct _dyneither_ptr*_tmp680;int _tmp67F;int _tmp67E;struct Cyc_Absyn_Exp*_tmp67D;void*_tmp67C;struct Cyc_List_List*_tmp67B;void*_tmp67A;struct Cyc_Absyn_Exp*_tmp679;struct Cyc_Absyn_Exp*_tmp678;struct Cyc_Absyn_Exp*_tmp677;struct Cyc_Absyn_Exp*_tmp676;void**_tmp675;struct Cyc_Absyn_Exp*_tmp674;int _tmp673;enum Cyc_Absyn_Coercion _tmp672;struct Cyc_Absyn_Exp*_tmp671;struct Cyc_List_List*_tmp670;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Exp*_tmp66E;int _tmp66D;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_List_List*_tmp66B;int _tmp66A;struct Cyc_List_List*_tmp669;struct Cyc_Absyn_VarargInfo*_tmp668;struct Cyc_Absyn_Exp*_tmp667;struct Cyc_List_List*_tmp666;struct Cyc_Absyn_Exp*_tmp665;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_Absyn_Exp*_tmp663;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_Absyn_Exp*_tmp661;struct Cyc_Absyn_Exp*_tmp660;struct Cyc_Absyn_Exp*_tmp65F;struct Cyc_Absyn_Exp*_tmp65E;struct Cyc_Absyn_Exp*_tmp65D;struct Cyc_Absyn_Exp*_tmp65C;struct Cyc_Core_Opt*_tmp65B;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp*_tmp659;enum Cyc_Absyn_Incrementor _tmp658;enum Cyc_Absyn_Primop _tmp657;struct Cyc_List_List*_tmp656;switch(*((int*)_tmp489)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp489)->f1).Null_c).tag == 1){_LL281: _LL282: {
# 2430
struct Cyc_Absyn_Exp*_tmp48A=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp48A,_tmp48A))->r;else{
# 2435
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp48B[3];_tmp48B[2]=_tmp48A;_tmp48B[1]=_tmp48A;_tmp48B[0]=_tmp48A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp48B,sizeof(struct Cyc_Absyn_Exp*),3));}));}}else{
# 2437
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2439
goto _LL280;}}else{_LL283: _LL284:
 goto _LL280;}case 1: _LL285: _LL286:
 goto _LL280;case 2: _LL287: _tmp657=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp656=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL288: {
# 2444
struct Cyc_List_List*_tmp48C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp656);
# 2446
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp656);
{enum Cyc_Absyn_Primop _tmp48D=_tmp657;switch(_tmp48D){case Cyc_Absyn_Numelts: _LL2D6: _LL2D7: {
# 2449
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp656))->hd;
{void*_tmp48E=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp48F=_tmp48E;void*_tmp4A1;union Cyc_Absyn_Constraint*_tmp4A0;union Cyc_Absyn_Constraint*_tmp49F;union Cyc_Absyn_Constraint*_tmp49E;struct Cyc_Absyn_Exp*_tmp49D;switch(*((int*)_tmp48F)){case 8: _LL2EB: _tmp49D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48F)->f1).num_elts;_LL2EC:
# 2453
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp49D)))
({void*_tmp490=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp491="can't calculate numelts";_tag_dyneither(_tmp491,sizeof(char),24);}),_tag_dyneither(_tmp490,sizeof(void*),0));});
e->r=_tmp49D->r;goto _LL2EA;case 5: _LL2ED: _tmp4A1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48F)->f1).elt_typ;_tmp4A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48F)->f1).ptr_atts).nullable;_tmp49F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48F)->f1).ptr_atts).bounds;_tmp49E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48F)->f1).ptr_atts).zero_term;_LL2EE:
# 2457
{void*_tmp492=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp49F);void*_tmp493=_tmp492;struct Cyc_Absyn_Exp*_tmp498;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp493)->tag == 0){_LL2F2: _LL2F3:
# 2459
 e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*_tmp494[2];_tmp494[1]=
# 2461
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4A1),0);_tmp494[0]=(struct Cyc_Absyn_Exp*)_tmp656->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp494,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL2F1;}else{_LL2F4: _tmp498=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp493)->f1;_LL2F5:
# 2465
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp49E)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp656->hd);
# 2469
e->r=Cyc_Toc_fncall_exp_r(function_e,({struct Cyc_Absyn_Exp*_tmp495[2];_tmp495[1]=_tmp498;_tmp495[0]=(struct Cyc_Absyn_Exp*)_tmp656->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp495,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4A0)){
if(!Cyc_Evexp_c_can_eval(_tmp498))
({void*_tmp496=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp497="can't calculate numelts";_tag_dyneither(_tmp497,sizeof(char),24);}),_tag_dyneither(_tmp496,sizeof(void*),0));});
# 2474
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp498,Cyc_Absyn_uint_exp(0,0));}else{
# 2476
e->r=_tmp498->r;goto _LL2F1;}}
# 2478
goto _LL2F1;}_LL2F1:;}
# 2480
goto _LL2EA;default: _LL2EF: _LL2F0:
# 2482
({struct Cyc_String_pa_PrintArg_struct _tmp49C;_tmp49C.tag=0;_tmp49C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp49B;_tmp49B.tag=0;_tmp49B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({void*_tmp499[2]={& _tmp49B,& _tmp49C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp49A="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp49A,sizeof(char),41);}),_tag_dyneither(_tmp499,sizeof(void*),2));});});});}_LL2EA:;}
# 2485
goto _LL2D5;}case Cyc_Absyn_Plus: _LL2D8: _LL2D9:
# 2490
{void*_tmp4A2=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp48C))->hd);void*_tmp4A3=_tmp4A2;void*_tmp4AB;union Cyc_Absyn_Constraint*_tmp4AA;union Cyc_Absyn_Constraint*_tmp4A9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A3)->tag == 5){_LL2F7: _tmp4AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A3)->f1).elt_typ;_tmp4AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A3)->f1).ptr_atts).bounds;_tmp4A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A3)->f1).ptr_atts).zero_term;_LL2F8:
# 2492
{void*_tmp4A4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4AA);void*_tmp4A5=_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4A5)->tag == 0){_LL2FC: _LL2FD: {
# 2494
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp656))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp656->tl))->hd;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4A6[3];_tmp4A6[2]=e2;_tmp4A6[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4AB),0);_tmp4A6[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A6,sizeof(struct Cyc_Absyn_Exp*),3));}));
goto _LL2FB;}}else{_LL2FE: _tmp4A8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4A5)->f1;_LL2FF:
# 2500
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4A9)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp656))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp656->tl))->hd;
e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*_tmp4A7[3];_tmp4A7[2]=e2;_tmp4A7[1]=_tmp4A8;_tmp4A7[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A7,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r;}
# 2505
goto _LL2FB;}_LL2FB:;}
# 2507
goto _LL2F6;}else{_LL2F9: _LL2FA:
# 2509
 goto _LL2F6;}_LL2F6:;}
# 2511
goto _LL2D5;case Cyc_Absyn_Minus: _LL2DA: _LL2DB: {
# 2516
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp48C))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp656))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp656->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp48C->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2526
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4AC[3];_tmp4AC[2]=
# 2528
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0);_tmp4AC[1]=
# 2527
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4AC[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4AC,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 2531
goto _LL2D5;}case Cyc_Absyn_Eq: _LL2DC: _LL2DD:
 goto _LL2DF;case Cyc_Absyn_Neq: _LL2DE: _LL2DF:
 goto _LL2E1;case Cyc_Absyn_Gt: _LL2E0: _LL2E1:
 goto _LL2E3;case Cyc_Absyn_Gte: _LL2E2: _LL2E3:
 goto _LL2E5;case Cyc_Absyn_Lt: _LL2E4: _LL2E5:
 goto _LL2E7;case Cyc_Absyn_Lte: _LL2E6: _LL2E7: {
# 2539
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp656))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp656->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp48C))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp48C->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
goto _LL2D5;}default: _LL2E8: _LL2E9:
 goto _LL2D5;}_LL2D5:;}
# 2551
goto _LL280;}case 4: _LL289: _tmp659=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp658=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL28A: {
# 2553
void*e2_cyc_typ=(void*)_check_null(_tmp659->topt);
# 2562 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp4C3="increment";_tag_dyneither(_tmp4C3,sizeof(char),10);});
if(_tmp658 == Cyc_Absyn_PreDec  || _tmp658 == Cyc_Absyn_PostDec)incr_str=({const char*_tmp4AD="decrement";_tag_dyneither(_tmp4AD,sizeof(char),10);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp659,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp4B0;_tmp4B0.tag=0;_tmp4B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4AE[1]={& _tmp4B0};Cyc_Tcutil_terr(e->loc,({const char*_tmp4AF="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp4AF,sizeof(char),74);}),_tag_dyneither(_tmp4AE,sizeof(void*),1));});});
({void*_tmp4B1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4B2="in-place inc/dec on zero-term";_tag_dyneither(_tmp4B2,sizeof(char),30);}),_tag_dyneither(_tmp4B1,sizeof(void*),0));});}{
# 2571
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp659,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4B3=Cyc_Absyn_signed_int_exp(1,0);
_tmp4B3->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4B4=_tmp658;switch(_tmp4B4){case Cyc_Absyn_PreInc: _LL301: _LL302:
# 2578
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4B6;_tmp4B6.tag=3;_tmp4B6.f1=_tmp659;_tmp4B6.f2=({struct Cyc_Core_Opt*_tmp4B7=_cycalloc_atomic(sizeof(*_tmp4B7));_tmp4B7->v=(void*)Cyc_Absyn_Plus;_tmp4B7;});_tmp4B6.f3=_tmp4B3;_tmp4B6;});_tmp4B5;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL303: _LL304:
# 2582
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4B9;_tmp4B9.tag=3;_tmp4B9.f1=_tmp659;_tmp4B9.f2=({struct Cyc_Core_Opt*_tmp4BA=_cycalloc_atomic(sizeof(*_tmp4BA));_tmp4BA->v=(void*)Cyc_Absyn_Minus;_tmp4BA;});_tmp4B9.f3=_tmp4B3;_tmp4B9;});_tmp4B8;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL305: _LL306:
# 2586
({struct Cyc_String_pa_PrintArg_struct _tmp4BD;_tmp4BD.tag=0;_tmp4BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4BB[1]={& _tmp4BD};Cyc_Tcutil_terr(e->loc,({const char*_tmp4BC="in-place post-%s is not supported on @tagged union members";_tag_dyneither(_tmp4BC,sizeof(char),59);}),_tag_dyneither(_tmp4BB,sizeof(void*),1));});});
# 2588
({void*_tmp4BE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4BF="in-place inc/dec on @tagged union";_tag_dyneither(_tmp4BF,sizeof(char),34);}),_tag_dyneither(_tmp4BE,sizeof(void*),0));});}_LL300:;};}
# 2591
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp659);
Cyc_Toc_set_lhs(nv,0);{
# 2596
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp658 == Cyc_Absyn_PostInc  || _tmp658 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2602
if(_tmp658 == Cyc_Absyn_PreDec  || _tmp658 == Cyc_Absyn_PostDec)
change=-1;
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4C0[3];_tmp4C0[2]=
# 2606
Cyc_Absyn_signed_int_exp(change,0);_tmp4C0[1]=
# 2605
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4C0[0]=
# 2604
Cyc_Toc_push_address_exp(_tmp659);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C0,sizeof(struct Cyc_Absyn_Exp*),3));}));}else{
# 2607
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2611
struct Cyc_Toc_functionSet*_tmp4C1=_tmp658 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2613
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp4C1,_tmp659);
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4C2[2];_tmp4C2[1]=
Cyc_Absyn_signed_int_exp(1,0);_tmp4C2[0]=
# 2614
Cyc_Toc_push_address_exp(_tmp659);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C2,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
# 2616
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp659)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp659,0,Cyc_Toc_incr_lvalue,_tmp658);
e->r=_tmp659->r;}}}
# 2620
goto _LL280;};};}case 3: _LL28B: _tmp65C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp65B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_tmp65A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp489)->f3;_LL28C: {
# 2639 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp65C->topt);
void*e2_old_typ=(void*)_check_null(_tmp65A->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp65C,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp65C);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp65A);
e->r=Cyc_Toc_tagged_union_assignop(_tmp65C,e1_old_typ,_tmp65B,_tmp65A,e2_old_typ,f_tag,tagged_member_struct_type);
# 2650
return;}{
# 2652
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp65C,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp65C,_tmp65B,_tmp65A,ptr_type,is_dyneither,elt_type);
# 2658
return;}{
# 2662
int e1_poly=Cyc_Toc_is_poly_project(_tmp65C);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp65C);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp65A);{
# 2668
int done=0;
if(_tmp65B != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp4C4=(enum Cyc_Absyn_Primop)_tmp65B->v;enum Cyc_Absyn_Primop _tmp4C5=_tmp4C4;switch(_tmp4C5){case Cyc_Absyn_Plus: _LL308: _LL309:
 change=_tmp65A;goto _LL307;case Cyc_Absyn_Minus: _LL30A: _LL30B:
# 2676
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp65A,0);goto _LL307;default: _LL30C: _LL30D:
({void*_tmp4C6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4C7="bad t ? pointer arithmetic";_tag_dyneither(_tmp4C7,sizeof(char),27);}),_tag_dyneither(_tmp4C6,sizeof(void*),0));});}_LL307:;}
# 2679
done=1;{
# 2681
struct Cyc_Absyn_Exp*_tmp4C8=Cyc_Toc__dyneither_ptr_inplace_plus_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp4C8,({struct Cyc_Absyn_Exp*_tmp4C9[3];_tmp4C9[2]=change;_tmp4C9[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4C9[0]=
# 2682
Cyc_Toc_push_address_exp(_tmp65C);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C9,sizeof(struct Cyc_Absyn_Exp*),3));}));};}else{
# 2685
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2688
enum Cyc_Absyn_Primop _tmp4CA=(enum Cyc_Absyn_Primop)_tmp65B->v;enum Cyc_Absyn_Primop _tmp4CB=_tmp4CA;if(_tmp4CB == Cyc_Absyn_Plus){_LL30F: _LL310:
# 2690
 done=1;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp65C),({struct Cyc_Absyn_Exp*_tmp4CC[2];_tmp4CC[1]=_tmp65A;_tmp4CC[0]=_tmp65C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CC,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL30E;}else{_LL311: _LL312:
({void*_tmp4CD=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4CE="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp4CE,sizeof(char),39);}),_tag_dyneither(_tmp4CD,sizeof(void*),0));});}_LL30E:;}}}
# 2697
if(!done){
# 2699
if(e1_poly)
_tmp65A->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp65A->r,0));
# 2705
if(!Cyc_Absyn_is_lvalue(_tmp65C)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp65C,0,Cyc_Toc_assignop_lvalue,({struct _tuple27*_tmp4CF=_cycalloc(sizeof(struct _tuple27)* 1);_tmp4CF[0]=(struct _tuple27)({struct _tuple27 _tmp4D0;_tmp4D0.f1=_tmp65B;_tmp4D0.f2=_tmp65A;_tmp4D0;});_tmp4CF;}));
e->r=_tmp65C->r;}}
# 2710
goto _LL280;};};};}case 5: _LL28D: _tmp65F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp65E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_tmp65D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp489)->f3;_LL28E:
# 2712
 Cyc_Toc_exp_to_c(nv,_tmp65F);
Cyc_Toc_exp_to_c(nv,_tmp65E);
Cyc_Toc_exp_to_c(nv,_tmp65D);
goto _LL280;case 6: _LL28F: _tmp661=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp660=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL290:
# 2717
 Cyc_Toc_exp_to_c(nv,_tmp661);
Cyc_Toc_exp_to_c(nv,_tmp660);
goto _LL280;case 7: _LL291: _tmp663=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp662=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL292:
# 2721
 Cyc_Toc_exp_to_c(nv,_tmp663);
Cyc_Toc_exp_to_c(nv,_tmp662);
goto _LL280;case 8: _LL293: _tmp665=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp664=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL294:
# 2725
 Cyc_Toc_exp_to_c(nv,_tmp665);
Cyc_Toc_exp_to_c(nv,_tmp664);
goto _LL280;case 9: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp489)->f3 == 0){_LL295: _tmp667=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp666=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL296:
# 2729
 Cyc_Toc_exp_to_c(nv,_tmp667);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp666);
goto _LL280;}else{_LL297: _tmp66C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp66B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_tmp66A=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp489)->f3)->num_varargs;_tmp669=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp489)->f3)->injectors;_tmp668=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp489)->f3)->vai;_LL298: {
# 2741 "toc.cyc"
struct _RegionHandle _tmp4D1=_new_region("r");struct _RegionHandle*r=& _tmp4D1;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp66A,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp668->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2749
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp66B);
int num_normargs=num_args - _tmp66A;
# 2753
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp66B=_tmp66B->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp66B))->hd);
new_args=({struct Cyc_List_List*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2->hd=(struct Cyc_Absyn_Exp*)_tmp66B->hd;_tmp4D2->tl=new_args;_tmp4D2;});}}
# 2758
new_args=({struct Cyc_List_List*_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp4D4[3];_tmp4D4[2]=num_varargs_exp;_tmp4D4[1]=
# 2760
Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp4D4[0]=argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D4,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp4D3->tl=new_args;_tmp4D3;});
# 2763
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2765
Cyc_Toc_exp_to_c(nv,_tmp66C);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp66C,new_args,0),0);
# 2769
if(_tmp668->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp4D5=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp668->type));void*_tmp4D6=_tmp4D5;struct Cyc_Absyn_Datatypedecl*_tmp4D9;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D6)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D6)->f1).datatype_info).KnownDatatype).tag == 2){_LL314: _tmp4D9=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D6)->f1).datatype_info).KnownDatatype).val;_LL315:
 tud=_tmp4D9;goto _LL313;}else{goto _LL316;}}else{_LL316: _LL317:
({void*_tmp4D7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4D8="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp4D8,sizeof(char),44);}),_tag_dyneither(_tmp4D7,sizeof(void*),0));});}_LL313:;}{
# 2775
struct _dyneither_ptr vs=({unsigned int _tmp4E2=(unsigned int)_tmp66A;struct _tuple1**_tmp4E3=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmp4E2));struct _dyneither_ptr _tmp4E5=_tag_dyneither(_tmp4E3,sizeof(struct _tuple1*),_tmp4E2);{unsigned int _tmp4E4=_tmp4E2;unsigned int i;for(i=0;i < _tmp4E4;i ++){_tmp4E3[i]=(struct _tuple1*)Cyc_Toc_temp_var();}}_tmp4E5;});
# 2777
if(_tmp66A != 0){
# 2779
struct Cyc_List_List*_tmp4DA=0;
{int i=_tmp66A - 1;for(0;i >= 0;-- i){
_tmp4DA=({struct Cyc_List_List*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp4DB->tl=_tmp4DA;_tmp4DB;});}}
# 2783
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp4DA,0),s,0);{
# 2786
int i=0;for(0;_tmp66B != 0;(((_tmp66B=_tmp66B->tl,_tmp669=_tmp669->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp66B->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp4DC=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp669))->hd;struct Cyc_Absyn_Datatypefield*_tmp4DD=_tmp4DC;struct _tuple1*_tmp4DF;struct Cyc_List_List*_tmp4DE;_LL319: _tmp4DF=_tmp4DD->name;_tmp4DE=_tmp4DD->typs;_LL31A:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp4DE))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2801
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2804
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp4DF),0),s,0);
# 2807
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4DF,tud->name)),0,s,0);};}};}else{
# 2814
struct Cyc_List_List*_tmp4E0=({struct _tuple19*_tmp4E1[3];_tmp4E1[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4E1[1]=
# 2814
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4E1[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E1,sizeof(struct _tuple19*),3));});
# 2816
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2826
{int i=0;for(0;_tmp66B != 0;(_tmp66B=_tmp66B->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp66B->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp66B->hd,0),s,0);}}
# 2832
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2845 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2847
_npop_handler(0);goto _LL280;
# 2741 "toc.cyc"
;_pop_region(r);}}case 10: _LL299: _tmp66E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp66D=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL29A:
# 2850 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp66E);{
struct Cyc_Absyn_Exp*_tmp4E6=_tmp66D?Cyc_Toc_newrethrow_exp(_tmp66E): Cyc_Toc_newthrow_exp(_tmp66E);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp4E6,0))->r;
goto _LL280;};case 11: _LL29B: _tmp66F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_LL29C:
 Cyc_Toc_exp_to_c(nv,_tmp66F);goto _LL280;case 12: _LL29D: _tmp671=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp670=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL29E:
# 2856
 Cyc_Toc_exp_to_c(nv,_tmp671);
# 2865 "toc.cyc"
for(0;_tmp670 != 0;_tmp670=_tmp670->tl){
enum Cyc_Absyn_KindQual _tmp4E7=(Cyc_Tcutil_typ_kind((void*)_tmp670->hd))->kind;
if(_tmp4E7 != Cyc_Absyn_EffKind  && _tmp4E7 != Cyc_Absyn_RgnKind){
{void*_tmp4E8=Cyc_Tcutil_compress((void*)_tmp670->hd);void*_tmp4E9=_tmp4E8;switch(*((int*)_tmp4E9)){case 2: _LL31C: _LL31D:
 goto _LL31F;case 3: _LL31E: _LL31F:
 continue;default: _LL320: _LL321:
# 2872
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp671,0))->r;
goto _LL31B;}_LL31B:;}
# 2875
break;}}
# 2878
goto _LL280;case 13: _LL29F: _tmp675=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp674=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_tmp673=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp489)->f3;_tmp672=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp489)->f4;_LL2A0: {
# 2880
void*old_t2=(void*)_check_null(_tmp674->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp675;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp675=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp674);
# 2887
{struct _tuple29 _tmp4EA=({struct _tuple29 _tmp525;_tmp525.f1=Cyc_Tcutil_compress(old_t2);_tmp525.f2=Cyc_Tcutil_compress(new_typ);_tmp525;});struct _tuple29 _tmp4EB=_tmp4EA;struct Cyc_Absyn_PtrInfo _tmp524;struct Cyc_Absyn_PtrInfo _tmp523;struct Cyc_Absyn_PtrInfo _tmp522;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EB.f1)->tag == 5)switch(*((int*)_tmp4EB.f2)){case 5: _LL323: _tmp523=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EB.f1)->f1;_tmp522=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EB.f2)->f1;_LL324: {
# 2889
int _tmp4EC=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp523.ptr_atts).nullable);
int _tmp4ED=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp522.ptr_atts).nullable);
void*_tmp4EE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp523.ptr_atts).bounds);
void*_tmp4EF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp522.ptr_atts).bounds);
int _tmp4F0=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp523.ptr_atts).zero_term);
int _tmp4F1=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp522.ptr_atts).zero_term);
{struct _tuple29 _tmp4F2=({struct _tuple29 _tmp51F;_tmp51F.f1=_tmp4EE;_tmp51F.f2=_tmp4EF;_tmp51F;});struct _tuple29 _tmp4F3=_tmp4F2;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51B;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F3.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F3.f2)->tag == 1){_LL32A: _tmp51C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F3.f1)->f1;_tmp51B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F3.f2)->f1;_LL32B:
# 2897
 if((!Cyc_Evexp_c_can_eval(_tmp51C) || !Cyc_Evexp_c_can_eval(_tmp51B)) && !
Cyc_Evexp_same_const_exp(_tmp51C,_tmp51B))
({void*_tmp4F4=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4F5="can't validate cast due to potential size differences";_tag_dyneither(_tmp4F5,sizeof(char),54);}),_tag_dyneither(_tmp4F4,sizeof(void*),0));});
if(_tmp4EC  && !_tmp4ED){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp4F6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4F7="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp4F7,sizeof(char),44);}),_tag_dyneither(_tmp4F6,sizeof(void*),0));});
# 2906
if(_tmp672 != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0;_tmp4FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp4F8[1]={& _tmp4FA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4F9="null-check conversion mis-classified: %s";_tag_dyneither(_tmp4F9,sizeof(char),41);}),_tag_dyneither(_tmp4F8,sizeof(void*),1));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp674);
if(do_null_check){
if(!_tmp673)
({void*_tmp4FB=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp4FC="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp4FC,sizeof(char),58);}),_tag_dyneither(_tmp4FB,sizeof(void*),0));});
# 2913
e->r=Cyc_Toc_cast_it_r(*_tmp675,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->hd=_tmp674;_tmp4FD->tl=0;_tmp4FD;}),0));}else{
# 2917
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp674->r;}};}else{
# 2922
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp674->r;}
# 2929
goto _LL329;}else{_LL32C: _tmp51D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F3.f1)->f1;_LL32D:
# 2931
 if(!Cyc_Evexp_c_can_eval(_tmp51D))
({void*_tmp4FE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4FF="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp4FF,sizeof(char),71);}),_tag_dyneither(_tmp4FE,sizeof(void*),0));});
# 2934
if(_tmp672 == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp502;_tmp502.tag=0;_tmp502.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp500[1]={& _tmp502};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp501="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp501,sizeof(char),44);}),_tag_dyneither(_tmp500,sizeof(void*),1));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2938
if((_tmp4F0  && !(_tmp522.elt_tq).real_const) && !_tmp4F1)
# 2941
_tmp51D=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp51D,Cyc_Absyn_uint_exp(1,0),0);
# 2943
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp51D,_tmp674))->r;}else{
# 2945
struct Cyc_Absyn_Exp*_tmp503=Cyc_Toc__tag_dyneither_e;
# 2947
if(_tmp4F0){
# 2952
struct _tuple1*_tmp504=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp505=Cyc_Absyn_var_exp(_tmp504,0);
struct Cyc_Absyn_Exp*arg3;
# 2957
{void*_tmp506=_tmp674->r;void*_tmp507=_tmp506;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp507)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp507)->f1).Wstring_c).tag){case 8: _LL333: _LL334:
# 2959
 arg3=_tmp51D;goto _LL332;case 9: _LL335: _LL336:
# 2961
 arg3=_tmp51D;goto _LL332;default: goto _LL337;}else{_LL337: _LL338:
# 2963
 arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp674),({struct Cyc_Absyn_Exp*_tmp508[2];_tmp508[1]=_tmp51D;_tmp508[0]=
# 2965
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp505);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp508,sizeof(struct Cyc_Absyn_Exp*),2));}),0);
goto _LL332;}_LL332:;}
# 2968
if(!_tmp4F1  && !(_tmp522.elt_tq).real_const)
# 2971
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2973
struct Cyc_Absyn_Exp*_tmp509=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp522.elt_typ),0);
struct Cyc_Absyn_Exp*_tmp50A=Cyc_Absyn_fncall_exp(_tmp503,({struct Cyc_Absyn_Exp*_tmp50C[3];_tmp50C[2]=arg3;_tmp50C[1]=_tmp509;_tmp50C[0]=_tmp505;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp50C,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
struct Cyc_Absyn_Stmt*_tmp50B=Cyc_Absyn_exp_stmt(_tmp50A,0);
_tmp50B=Cyc_Absyn_declare_stmt(_tmp504,Cyc_Toc_typ_to_c(old_t2),_tmp674,_tmp50B,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp50B);};}else{
# 2980
e->r=Cyc_Toc_fncall_exp_r(_tmp503,({struct Cyc_Absyn_Exp*_tmp50D[3];_tmp50D[2]=_tmp51D;_tmp50D[1]=
# 2982
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp522.elt_typ),0);_tmp50D[0]=_tmp674;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp50D,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 2986
goto _LL329;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F3.f2)->tag == 1){_LL32E: _tmp51E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F3.f2)->f1;_LL32F:
# 2988
 if(!Cyc_Evexp_c_can_eval(_tmp51E))
({void*_tmp50E=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp50F="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp50F,sizeof(char),71);}),_tag_dyneither(_tmp50E,sizeof(void*),0));});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp510=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp511="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp511,sizeof(char),45);}),_tag_dyneither(_tmp510,sizeof(void*),0));});{
# 3000 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp512=_tmp51E;
if(_tmp4F0  && !_tmp4F1)
_tmp512=Cyc_Absyn_add_exp(_tmp51E,Cyc_Absyn_uint_exp(1,0),0);{
# 3007
struct Cyc_Absyn_Exp*_tmp513=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp514=Cyc_Absyn_fncall_exp(_tmp513,({struct Cyc_Absyn_Exp*_tmp516[3];_tmp516[2]=_tmp512;_tmp516[1]=
# 3010
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp523.elt_typ),0);_tmp516[0]=_tmp674;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp516,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 3012
if(!_tmp4ED)
_tmp514->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515->hd=
Cyc_Absyn_copy_exp(_tmp514);_tmp515->tl=0;_tmp515;}));
e->r=Cyc_Toc_cast_it_r(*_tmp675,_tmp514);
goto _LL329;};};}else{_LL330: _LL331:
# 3020
 DynCast:
 if((_tmp4F0  && !_tmp4F1) && !(_tmp522.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp517=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp518="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp518,sizeof(char),70);}),_tag_dyneither(_tmp517,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp519=Cyc_Toc__dyneither_ptr_decrease_size_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp519,({struct Cyc_Absyn_Exp*_tmp51A[3];_tmp51A[2]=
# 3027
Cyc_Absyn_uint_exp(1,0);_tmp51A[1]=
# 3026
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp523.elt_typ),0);_tmp51A[0]=_tmp674;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp51A,sizeof(struct Cyc_Absyn_Exp*),3));}));};}
# 3029
goto _LL329;}}_LL329:;}
# 3031
goto _LL322;}case 6: _LL325: _tmp524=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EB.f1)->f1;_LL326:
# 3033
{void*_tmp520=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp524.ptr_atts).bounds);void*_tmp521=_tmp520;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp521)->tag == 0){_LL33A: _LL33B:
# 3035
 _tmp674->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp674->r,_tmp674->loc),Cyc_Toc_curr_sp);goto _LL339;}else{_LL33C: _LL33D:
 goto _LL339;}_LL339:;}
# 3038
goto _LL322;default: goto _LL327;}else{_LL327: _LL328:
# 3040
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp674->r;
goto _LL322;}_LL322:;}
# 3044
goto _LL280;}case 14: _LL2A1: _tmp676=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_LL2A2:
# 3048
{void*_tmp526=_tmp676->r;void*_tmp527=_tmp526;struct Cyc_List_List*_tmp532;struct _tuple1*_tmp531;struct Cyc_List_List*_tmp530;struct Cyc_List_List*_tmp52F;switch(*((int*)_tmp527)){case 28: _LL33F: _tmp531=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp527)->f1;_tmp530=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp527)->f2;_tmp52F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp527)->f3;_LL340:
# 3050
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp52A;_tmp52A.tag=0;_tmp52A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp676->loc));({void*_tmp528[1]={& _tmp52A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp529="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp529,sizeof(char),42);}),_tag_dyneither(_tmp528,sizeof(void*),1));});});{
struct Cyc_Absyn_Exp*_tmp52B=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp676->topt),_tmp530,1,0,_tmp52F,_tmp531);
e->r=_tmp52B->r;
e->topt=_tmp52B->topt;
goto _LL33E;};case 23: _LL341: _tmp532=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp527)->f1;_LL342:
# 3058
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp52E;_tmp52E.tag=0;_tmp52E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp676->loc));({void*_tmp52C[1]={& _tmp52E};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp52D="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp52D,sizeof(char),42);}),_tag_dyneither(_tmp52C,sizeof(void*),1));});});
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp532))->r;
goto _LL33E;default: _LL343: _LL344:
# 3064
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp676);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp676)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp676,0,Cyc_Toc_address_lvalue,1);
# 3070
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp676);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp676->topt))))
# 3074
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp676);}
# 3076
goto _LL33E;}_LL33E:;}
# 3078
goto _LL280;case 15: _LL2A3: _tmp678=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp677=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL2A4:
# 3081
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0;_tmp535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp677->loc));({void*_tmp533[1]={& _tmp535};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp534="%s: new at top-level";_tag_dyneither(_tmp534,sizeof(char),21);}),_tag_dyneither(_tmp533,sizeof(void*),1));});});{
void*new_e_type=(void*)_check_null(_tmp677->topt);
{void*_tmp536=_tmp677->r;void*_tmp537=_tmp536;struct Cyc_List_List*_tmp566;struct _tuple1*_tmp565;struct Cyc_List_List*_tmp564;struct Cyc_List_List*_tmp563;struct Cyc_Absyn_Aggrdecl*_tmp562;struct Cyc_Absyn_Exp*_tmp561;void*_tmp560;int _tmp55F;struct Cyc_Absyn_Vardecl*_tmp55E;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp55C;int _tmp55B;struct Cyc_List_List*_tmp55A;switch(*((int*)_tmp537)){case 25: _LL346: _tmp55A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp537)->f1;_LL347: {
# 3089
struct _tuple1*_tmp538=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp539=Cyc_Absyn_var_exp(_tmp538,0);
struct Cyc_Absyn_Stmt*_tmp53A=Cyc_Toc_init_array(nv,new_e_type,_tmp539,_tmp55A,Cyc_Absyn_exp_stmt(_tmp539,0));
void*old_elt_typ;
{void*_tmp53B=Cyc_Tcutil_compress(old_typ);void*_tmp53C=_tmp53B;void*_tmp541;struct Cyc_Absyn_Tqual _tmp540;union Cyc_Absyn_Constraint*_tmp53F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53C)->tag == 5){_LL353: _tmp541=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53C)->f1).elt_typ;_tmp540=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53C)->f1).elt_tq;_tmp53F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53C)->f1).ptr_atts).zero_term;_LL354:
# 3095
 old_elt_typ=_tmp541;goto _LL352;}else{_LL355: _LL356:
# 3097
 old_elt_typ=({void*_tmp53D=0;((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp53E="exp_to_c:new array expression doesn't have ptr type";_tag_dyneither(_tmp53E,sizeof(char),52);}),_tag_dyneither(_tmp53D,sizeof(void*),0));});}_LL352:;}{
# 3100
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp542=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp543=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp55A),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp678 == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp543);else{
# 3108
struct Cyc_Absyn_Exp*r=_tmp678;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp543);}
# 3113
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp538,_tmp542,e1,_tmp53A,0));
goto _LL345;};}case 26: _LL348: _tmp55E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp537)->f1;_tmp55D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp537)->f2;_tmp55C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp537)->f3;_tmp55B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp537)->f4;_LL349:
# 3117
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp678,old_typ,_tmp55D,(void*)_check_null(_tmp55C->topt),_tmp55B,_tmp55C,_tmp55E);
goto _LL345;case 27: _LL34A: _tmp561=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp537)->f1;_tmp560=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp537)->f2;_tmp55F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp537)->f3;_LL34B:
# 3121
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp678,old_typ,_tmp561,_tmp560,_tmp55F,0,0);
goto _LL345;case 28: _LL34C: _tmp565=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp537)->f1;_tmp564=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp537)->f2;_tmp563=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp537)->f3;_tmp562=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp537)->f4;_LL34D: {
# 3126
struct Cyc_Absyn_Exp*_tmp544=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp677->topt),_tmp564,1,_tmp678,_tmp563,_tmp565);
e->r=_tmp544->r;
e->topt=_tmp544->topt;
goto _LL345;}case 23: _LL34E: _tmp566=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp537)->f1;_LL34F:
# 3132
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp678,_tmp566))->r;
goto _LL345;default: _LL350: _LL351: {
# 3139
void*old_elt_typ=(void*)_check_null(_tmp677->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3142
struct _tuple1*_tmp545=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp546=Cyc_Absyn_var_exp(_tmp545,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp546,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp678 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3149
struct Cyc_Absyn_Exp*r=_tmp678;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3157
int done=0;
{void*_tmp547=_tmp677->r;void*_tmp548=_tmp547;void*_tmp556;struct Cyc_Absyn_Exp*_tmp555;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp548)->tag == 13){_LL358: _tmp556=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp548)->f1;_tmp555=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp548)->f2;_LL359:
# 3160
{struct _tuple29 _tmp549=({struct _tuple29 _tmp554;_tmp554.f1=Cyc_Tcutil_compress(_tmp556);_tmp554.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp555->topt));_tmp554;});struct _tuple29 _tmp54A=_tmp549;void*_tmp553;union Cyc_Absyn_Constraint*_tmp552;union Cyc_Absyn_Constraint*_tmp551;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f2)->tag == 5){_LL35D: _tmp553=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f1)->f1).elt_typ;_tmp552=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f1)->f1).ptr_atts).bounds;_tmp551=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f2)->f1).ptr_atts).bounds;_LL35E:
# 3163
{struct _tuple29 _tmp54B=({struct _tuple29 _tmp550;_tmp550.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp552);_tmp550.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp551);_tmp550;});struct _tuple29 _tmp54C=_tmp54B;struct Cyc_Absyn_Exp*_tmp54F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp54C.f1)->tag == 0){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp54C.f2)->tag == 1){_LL362: _tmp54F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp54C.f2)->f1;_LL363:
# 3165
 Cyc_Toc_exp_to_c(nv,_tmp555);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp54D=Cyc_Toc__init_dyneither_ptr_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp54D,({struct Cyc_Absyn_Exp*_tmp54E[4];_tmp54E[3]=_tmp54F;_tmp54E[2]=
# 3171
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp553),0);_tmp54E[1]=_tmp555;_tmp54E[0]=mexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp54E,sizeof(struct Cyc_Absyn_Exp*),4));}));
# 3173
goto _LL361;};}else{goto _LL364;}}else{_LL364: _LL365:
 goto _LL361;}_LL361:;}
# 3176
goto _LL35C;}else{goto _LL35F;}}else{_LL35F: _LL360:
 goto _LL35C;}_LL35C:;}
# 3179
goto _LL357;}else{_LL35A: _LL35B:
 goto _LL357;}_LL357:;}
# 3182
if(!done){
struct Cyc_Absyn_Stmt*_tmp557=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp546),0);
struct Cyc_Absyn_Exp*_tmp558=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp677);
_tmp557=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp546,_tmp558,0),_tmp677,0),_tmp557,0);{
# 3188
void*_tmp559=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp545,_tmp559,mexp,_tmp557,0));};}
# 3191
goto _LL345;};}}_LL345:;}
# 3193
goto _LL280;};case 17: _LL2A5: _tmp679=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_LL2A6: {
# 3196
int _tmp567=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp679);
Cyc_Toc_set_in_sizeof(nv,_tmp567);
goto _LL280;}case 16: _LL2A7: _tmp67A=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_LL2A8:
 e->r=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp569;_tmp569.tag=16;_tmp569.f1=Cyc_Toc_typ_to_c(_tmp67A);_tmp569;});_tmp568;});goto _LL280;case 18: _LL2A9: _tmp67C=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp67B=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL2AA: {
# 3202
void*_tmp56A=_tmp67C;
struct Cyc_List_List*_tmp56B=_tmp67B;
for(0;_tmp56B != 0;_tmp56B=_tmp56B->tl){
void*_tmp56C=(void*)_tmp56B->hd;void*_tmp56D=_tmp56C;unsigned int _tmp583;struct _dyneither_ptr*_tmp582;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp56D)->tag == 0){_LL367: _tmp582=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp56D)->f1;_LL368:
 goto _LL366;}else{_LL369: _tmp583=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp56D)->f1;_LL36A:
# 3208
{void*_tmp56E=Cyc_Tcutil_compress(_tmp56A);void*_tmp56F=_tmp56E;struct Cyc_Absyn_Datatypefield*_tmp581;struct Cyc_List_List*_tmp580;struct Cyc_List_List*_tmp57F;union Cyc_Absyn_AggrInfoU _tmp57E;switch(*((int*)_tmp56F)){case 11: _LL36C: _tmp57E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56F)->f1).aggr_info;_LL36D: {
# 3210
struct Cyc_Absyn_Aggrdecl*_tmp570=Cyc_Absyn_get_known_aggrdecl(_tmp57E);
if(_tmp570->impl == 0)
({void*_tmp571=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp572="struct fields must be known";_tag_dyneither(_tmp572,sizeof(char),28);}),_tag_dyneither(_tmp571,sizeof(void*),0));});
_tmp57F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp570->impl))->fields;goto _LL36F;}case 12: _LL36E: _tmp57F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp56F)->f2;_LL36F: {
# 3215
struct Cyc_Absyn_Aggrfield*_tmp573=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp57F,(int)_tmp583);
_tmp56B->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp575;_tmp575.tag=0;_tmp575.f1=_tmp573->name;_tmp575;});_tmp574;}));
_tmp56A=_tmp573->type;
goto _LL36B;}case 10: _LL370: _tmp580=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp56F)->f1;_LL371:
# 3220
 _tmp56B->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp577;_tmp577.tag=0;_tmp577.f1=Cyc_Absyn_fieldname((int)(_tmp583 + 1));_tmp577;});_tmp576;}));
_tmp56A=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp580,(int)_tmp583)).f2;
goto _LL36B;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp56F)->f1).field_info).KnownDatatypefield).tag == 2){_LL372: _tmp581=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp56F)->f1).field_info).KnownDatatypefield).val).f2;_LL373:
# 3224
 if(_tmp583 == 0)
_tmp56B->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp579;_tmp579.tag=0;_tmp579.f1=Cyc_Toc_tag_sp;_tmp579;});_tmp578;}));else{
# 3227
_tmp56A=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp581->typs,(int)(_tmp583 - 1))).f2;
_tmp56B->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp57B;_tmp57B.tag=0;_tmp57B.f1=Cyc_Absyn_fieldname((int)_tmp583);_tmp57B;});_tmp57A;}));}
# 3230
goto _LL36B;}else{goto _LL374;}default: _LL374: _LL375:
({void*_tmp57C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp57D="impossible type for offsetof tuple index";_tag_dyneither(_tmp57D,sizeof(char),41);}),_tag_dyneither(_tmp57C,sizeof(void*),0));});
goto _LL36B;}_LL36B:;}
# 3234
goto _LL366;}_LL366:;}
# 3237
e->r=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp585;_tmp585.tag=18;_tmp585.f1=Cyc_Toc_typ_to_c(_tmp67C);_tmp585.f2=_tmp67B;_tmp585;});_tmp584;});
goto _LL280;}case 19: _LL2AB: _tmp67D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_LL2AC: {
# 3240
int _tmp586=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp587=Cyc_Tcutil_compress((void*)_check_null(_tmp67D->topt));
{void*_tmp588=_tmp587;void*_tmp59D;struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59B;union Cyc_Absyn_Constraint*_tmp59A;union Cyc_Absyn_Constraint*_tmp599;union Cyc_Absyn_Constraint*_tmp598;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->tag == 5){_LL377: _tmp59D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).elt_typ;_tmp59C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).elt_tq;_tmp59B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).ptr_atts).rgn;_tmp59A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).ptr_atts).nullable;_tmp599=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).ptr_atts).bounds;_tmp598=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).ptr_atts).zero_term;_LL378:
# 3245
{void*_tmp589=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp599);void*_tmp58A=_tmp589;struct Cyc_Absyn_Exp*_tmp595;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58A)->tag == 1){_LL37C: _tmp595=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58A)->f1;_LL37D: {
# 3247
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp67D);
Cyc_Toc_exp_to_c(nv,_tmp67D);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp58B=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp58C="inserted null check due to dereference";_tag_dyneither(_tmp58C,sizeof(char),39);}),_tag_dyneither(_tmp58B,sizeof(void*),0));});
# 3253
_tmp67D->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp587),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D->hd=
Cyc_Absyn_copy_exp(_tmp67D);_tmp58D->tl=0;_tmp58D;}),0));}
# 3261
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp598)){
struct _tuple12 _tmp58E=Cyc_Evexp_eval_const_uint_exp(_tmp595);struct _tuple12 _tmp58F=_tmp58E;unsigned int _tmp593;int _tmp592;_LL381: _tmp593=_tmp58F.f1;_tmp592=_tmp58F.f2;_LL382:;
# 3267
if(!_tmp592  || _tmp593 <= 0)
({void*_tmp590=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp591="cannot determine dereference is in bounds";_tag_dyneither(_tmp591,sizeof(char),42);}),_tag_dyneither(_tmp590,sizeof(void*),0));});}
# 3270
goto _LL37B;}}else{_LL37E: _LL37F: {
# 3273
struct Cyc_Absyn_Exp*_tmp594=Cyc_Absyn_uint_exp(0,0);
_tmp594->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp67D,_tmp594);
Cyc_Toc_exp_to_c(nv,e);
goto _LL37B;}}_LL37B:;}
# 3279
goto _LL376;}else{_LL379: _LL37A:
({void*_tmp596=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp597="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp597,sizeof(char),29);}),_tag_dyneither(_tmp596,sizeof(void*),0));});}_LL376:;}
# 3282
Cyc_Toc_set_lhs(nv,_tmp586);
goto _LL280;};}case 20: _LL2AD: _tmp681=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp680=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_tmp67F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp489)->f3;_tmp67E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp489)->f4;_LL2AE: {
# 3285
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp681->topt);
Cyc_Toc_exp_to_c(nv,_tmp681);
if(_tmp67F  && _tmp67E)
e->r=Cyc_Toc_check_tagged_union(_tmp681,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp680,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3293
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL280;}case 21: _LL2AF: _tmp685=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp684=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_tmp683=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp489)->f3;_tmp682=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp489)->f4;_LL2B0: {
# 3297
int _tmp59E=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp685->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp685);
Cyc_Toc_exp_to_c(nv,_tmp685);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp59F=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp5A0=_tmp59F;void*_tmp5B6;struct Cyc_Absyn_Tqual _tmp5B5;void*_tmp5B4;union Cyc_Absyn_Constraint*_tmp5B3;union Cyc_Absyn_Constraint*_tmp5B2;union Cyc_Absyn_Constraint*_tmp5B1;_LL384: _tmp5B6=_tmp5A0.elt_typ;_tmp5B5=_tmp5A0.elt_tq;_tmp5B4=(_tmp5A0.ptr_atts).rgn;_tmp5B3=(_tmp5A0.ptr_atts).nullable;_tmp5B2=(_tmp5A0.ptr_atts).bounds;_tmp5B1=(_tmp5A0.ptr_atts).zero_term;_LL385:;
{void*_tmp5A1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5B2);void*_tmp5A2=_tmp5A1;struct Cyc_Absyn_Exp*_tmp5B0;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5A2)->tag == 1){_LL387: _tmp5B0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5A2)->f1;_LL388: {
# 3308
struct _tuple12 _tmp5A3=Cyc_Evexp_eval_const_uint_exp(_tmp5B0);struct _tuple12 _tmp5A4=_tmp5A3;unsigned int _tmp5AE;int _tmp5AD;_LL38C: _tmp5AE=_tmp5A4.f1;_tmp5AD=_tmp5A4.f2;_LL38D:;
if(_tmp5AD){
if(_tmp5AE < 1)
({void*_tmp5A5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5A6="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp5A6,sizeof(char),44);}),_tag_dyneither(_tmp5A5,sizeof(void*),0));});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5A7=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5A8="inserted null check due to dereference";_tag_dyneither(_tmp5A8,sizeof(char),39);}),_tag_dyneither(_tmp5A7,sizeof(void*),0));});
# 3316
_tmp685->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5A9[1];_tmp5A9[0]=
Cyc_Absyn_new_exp(_tmp685->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5A9,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}}else{
# 3321
if(!Cyc_Evexp_c_can_eval(_tmp5B0))
({void*_tmp5AA=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5AB="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp5AB,sizeof(char),47);}),_tag_dyneither(_tmp5AA,sizeof(void*),0));});
# 3325
_tmp685->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5AC[4];_tmp5AC[3]=
# 3329
Cyc_Absyn_uint_exp(0,0);_tmp5AC[2]=
# 3328
Cyc_Absyn_sizeoftyp_exp(_tmp5B6,0);_tmp5AC[1]=_tmp5B0;_tmp5AC[0]=
# 3327
Cyc_Absyn_new_exp(_tmp685->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5AC,sizeof(struct Cyc_Absyn_Exp*),4));}),0));}
# 3331
goto _LL386;}}else{_LL389: _LL38A: {
# 3334
void*ta1=Cyc_Toc_typ_to_c(_tmp5B6);
_tmp685->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp5B5),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,({struct Cyc_Absyn_Exp*_tmp5AF[3];_tmp5AF[2]=
# 3339
Cyc_Absyn_uint_exp(0,0);_tmp5AF[1]=
# 3338
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5AF[0]=
# 3337
Cyc_Absyn_new_exp(_tmp685->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5AF,sizeof(struct Cyc_Absyn_Exp*),3));}),0));
# 3342
goto _LL386;}}_LL386:;}
# 3344
if(_tmp683  && _tmp682)
e->r=Cyc_Toc_check_tagged_union(_tmp685,Cyc_Toc_typ_to_c(e1typ),_tmp5B6,_tmp684,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3347
if(is_poly  && _tmp682)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp59E);
goto _LL280;};};}case 22: _LL2B1: _tmp687=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp686=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL2B2: {
# 3352
int _tmp5B7=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5B8=Cyc_Tcutil_compress((void*)_check_null(_tmp687->topt));
# 3356
{void*_tmp5B9=_tmp5B8;void*_tmp5E3;struct Cyc_Absyn_Tqual _tmp5E2;void*_tmp5E1;union Cyc_Absyn_Constraint*_tmp5E0;union Cyc_Absyn_Constraint*_tmp5DF;union Cyc_Absyn_Constraint*_tmp5DE;struct Cyc_List_List*_tmp5DD;switch(*((int*)_tmp5B9)){case 10: _LL38F: _tmp5DD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5B9)->f1;_LL390:
# 3359
 Cyc_Toc_exp_to_c(nv,_tmp687);
Cyc_Toc_exp_to_c(nv,_tmp686);{
struct _tuple12 _tmp5BA=Cyc_Evexp_eval_const_uint_exp(_tmp686);struct _tuple12 _tmp5BB=_tmp5BA;unsigned int _tmp5BF;int _tmp5BE;_LL396: _tmp5BF=_tmp5BB.f1;_tmp5BE=_tmp5BB.f2;_LL397:;
if(!_tmp5BE)
({void*_tmp5BC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5BD="unknown tuple subscript in translation to C";_tag_dyneither(_tmp5BD,sizeof(char),44);}),_tag_dyneither(_tmp5BC,sizeof(void*),0));});
e->r=Cyc_Toc_aggrmember_exp_r(_tmp687,Cyc_Absyn_fieldname((int)(_tmp5BF + 1)));
goto _LL38E;};case 5: _LL391: _tmp5E3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B9)->f1).elt_typ;_tmp5E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B9)->f1).elt_tq;_tmp5E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B9)->f1).ptr_atts).rgn;_tmp5E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B9)->f1).ptr_atts).nullable;_tmp5DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B9)->f1).ptr_atts).bounds;_tmp5DE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B9)->f1).ptr_atts).zero_term;_LL392: {
# 3367
struct Cyc_List_List*_tmp5C0=Cyc_Toc_get_relns(_tmp687);
int _tmp5C1=Cyc_Toc_need_null_check(_tmp687);
int _tmp5C2=Cyc_Toc_in_sizeof(nv);
# 3375
int in_bnds=_tmp5C2;
{void*_tmp5C3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5DF);void*_tmp5C4=_tmp5C3;_LL399: _LL39A:
# 3378
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp5B8,_tmp5C0,_tmp687,_tmp686);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp5C7;_tmp5C7.tag=0;_tmp5C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp5C5[1]={& _tmp5C7};Cyc_Tcutil_warn(e->loc,({const char*_tmp5C6="bounds check necessary for %s";_tag_dyneither(_tmp5C6,sizeof(char),30);}),_tag_dyneither(_tmp5C5,sizeof(void*),1));});});_LL398:;}
# 3385
Cyc_Toc_exp_to_c(nv,_tmp687);
Cyc_Toc_exp_to_c(nv,_tmp686);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp5C8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5DF);void*_tmp5C9=_tmp5C8;struct Cyc_Absyn_Exp*_tmp5DA;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C9)->tag == 1){_LL39C: _tmp5DA=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C9)->f1;_LL39D: {
# 3390
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5E0) && _tmp5C1;
void*ta1=Cyc_Toc_typ_to_c(_tmp5E3);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5E2);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5CA=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5CB="inserted null check due to dereference";_tag_dyneither(_tmp5CB,sizeof(char),39);}),_tag_dyneither(_tmp5CA,sizeof(void*),0));});
_tmp687->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5CC[1];_tmp5CC[0]=
Cyc_Absyn_copy_exp(_tmp687);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5CC,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}else{
# 3402
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5DE)){
# 3404
if(!Cyc_Evexp_c_can_eval(_tmp5DA))
({void*_tmp5CD=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5CE="cannot determine subscript is in bounds";_tag_dyneither(_tmp5CE,sizeof(char),40);}),_tag_dyneither(_tmp5CD,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp687);
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,({struct Cyc_Absyn_Exp*_tmp5CF[3];_tmp5CF[2]=_tmp686;_tmp5CF[1]=_tmp5DA;_tmp5CF[0]=_tmp687;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5CF,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));};}else{
# 3410
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp5DA))
({void*_tmp5D0=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5D1="cannot determine subscript is in bounds";_tag_dyneither(_tmp5D1,sizeof(char),40);}),_tag_dyneither(_tmp5D0,sizeof(void*),0));});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5D2=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5D3="inserted null check due to dereference";_tag_dyneither(_tmp5D3,sizeof(char),39);}),_tag_dyneither(_tmp5D2,sizeof(void*),0));});
# 3416
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5D4[4];_tmp5D4[3]=_tmp686;_tmp5D4[2]=
# 3419
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5D4[1]=_tmp5DA;_tmp5D4[0]=_tmp687;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D4,sizeof(struct Cyc_Absyn_Exp*),4));}),0)));}else{
# 3422
if(!Cyc_Evexp_c_can_eval(_tmp5DA))
({void*_tmp5D5=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5D6="cannot determine subscript is in bounds";_tag_dyneither(_tmp5D6,sizeof(char),40);}),_tag_dyneither(_tmp5D5,sizeof(void*),0));});
# 3425
_tmp686->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({struct Cyc_Absyn_Exp*_tmp5D7[2];_tmp5D7[1]=
Cyc_Absyn_copy_exp(_tmp686);_tmp5D7[0]=_tmp5DA;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D7,sizeof(struct Cyc_Absyn_Exp*),2));}));}}}}
# 3428
goto _LL39B;}}else{_LL39E: _LL39F: {
# 3430
void*ta1=Cyc_Toc_typ_to_c(_tmp5E3);
if(in_bnds){
# 3434
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5E2),
Cyc_Toc_member_exp(_tmp687,Cyc_Toc_curr_sp,0)),_tmp686);}else{
# 3439
struct Cyc_Absyn_Exp*_tmp5D8=Cyc_Toc__check_dyneither_subscript_e;
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5E2),
Cyc_Absyn_fncall_exp(_tmp5D8,({struct Cyc_Absyn_Exp*_tmp5D9[3];_tmp5D9[2]=_tmp686;_tmp5D9[1]=
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5D9[0]=_tmp687;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D9,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}
# 3445
goto _LL39B;}}_LL39B:;}
# 3447
goto _LL38E;}default: _LL393: _LL394:
({void*_tmp5DB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5DC="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp5DC,sizeof(char),49);}),_tag_dyneither(_tmp5DB,sizeof(void*),0));});}_LL38E:;}
# 3450
Cyc_Toc_set_lhs(nv,_tmp5B7);
goto _LL280;};}case 23: _LL2B3: _tmp688=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_LL2B4:
# 3453
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp688))->r;else{
# 3458
struct Cyc_List_List*_tmp5E4=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp688);
void*_tmp5E5=Cyc_Toc_add_tuple_type(_tmp5E4);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp688 != 0;(_tmp688=_tmp688->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp688->hd);
dles=({struct Cyc_List_List*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->hd=({struct _tuple19*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->f1=0;_tmp5E7->f2=(struct Cyc_Absyn_Exp*)_tmp688->hd;_tmp5E7;});_tmp5E6->tl=dles;_tmp5E6;});}}
# 3465
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3468
goto _LL280;case 25: _LL2B5: _tmp689=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_LL2B6:
# 3472
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp689);
{struct Cyc_List_List*_tmp5E8=_tmp689;for(0;_tmp5E8 != 0;_tmp5E8=_tmp5E8->tl){
struct _tuple19*_tmp5E9=(struct _tuple19*)_tmp5E8->hd;struct _tuple19*_tmp5EA=_tmp5E9;struct Cyc_Absyn_Exp*_tmp5EB;_LL3A1: _tmp5EB=_tmp5EA->f2;_LL3A2:;
Cyc_Toc_exp_to_c(nv,_tmp5EB);}}
# 3477
goto _LL280;case 26: _LL2B7: _tmp68D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp68C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_tmp68B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp489)->f3;_tmp68A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp489)->f4;_LL2B8: {
# 3481
struct _tuple12 _tmp5EC=Cyc_Evexp_eval_const_uint_exp(_tmp68C);struct _tuple12 _tmp5ED=_tmp5EC;unsigned int _tmp5F7;int _tmp5F6;_LL3A4: _tmp5F7=_tmp5ED.f1;_tmp5F6=_tmp5ED.f2;_LL3A5:;{
void*_tmp5EE=Cyc_Toc_typ_to_c((void*)_check_null(_tmp68B->topt));
Cyc_Toc_exp_to_c(nv,_tmp68B);{
struct Cyc_List_List*es=0;
# 3486
if(!Cyc_Toc_is_zero(_tmp68B)){
if(!_tmp5F6)
({void*_tmp5EF=0;Cyc_Tcutil_terr(_tmp68C->loc,({const char*_tmp5F0="cannot determine value of constant";_tag_dyneither(_tmp5F0,sizeof(char),35);}),_tag_dyneither(_tmp5EF,sizeof(void*),0));});
{unsigned int i=0;for(0;i < _tmp5F7;++ i){
es=({struct Cyc_List_List*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->hd=({struct _tuple19*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->f1=0;_tmp5F2->f2=_tmp68B;_tmp5F2;});_tmp5F1->tl=es;_tmp5F1;});}}
# 3492
if(_tmp68A){
struct Cyc_Absyn_Exp*_tmp5F3=Cyc_Toc_cast_it(_tmp5EE,Cyc_Absyn_uint_exp(0,0));
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->hd=({struct _tuple19*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->f1=0;_tmp5F5->f2=_tmp5F3;_tmp5F5;});_tmp5F4->tl=0;_tmp5F4;}));}}
# 3497
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL280;};};}case 27: _LL2B9: _tmp690=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp68F=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_tmp68E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp489)->f3;_LL2BA:
# 3502
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL280;case 28: _LL2BB: _tmp694=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp693=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_tmp692=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp489)->f3;_tmp691=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp489)->f4;_LL2BC:
# 3507
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp5F8=Cyc_Toc_init_struct(nv,old_typ,_tmp693,0,0,_tmp692,_tmp694);
e->r=_tmp5F8->r;
e->topt=_tmp5F8->topt;}else{
# 3518
if(_tmp691 == 0)
({void*_tmp5F9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5FA="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp5FA,sizeof(char),38);}),_tag_dyneither(_tmp5F9,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp691;
# 3522
struct _RegionHandle _tmp5FB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5FB;_push_region(rgn);
{struct Cyc_List_List*_tmp5FC=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp692,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3526
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp691->impl))->tagged){
# 3528
struct _tuple30*_tmp5FD=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp5FC))->hd;struct _tuple30*_tmp5FE=_tmp5FD;struct Cyc_Absyn_Aggrfield*_tmp60B;struct Cyc_Absyn_Exp*_tmp60A;_LL3A7: _tmp60B=_tmp5FE->f1;_tmp60A=_tmp5FE->f2;_LL3A8:;{
void*_tmp5FF=(void*)_check_null(_tmp60A->topt);
void*_tmp600=_tmp60B->type;
Cyc_Toc_exp_to_c(nv,_tmp60A);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp600) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp5FF))
_tmp60A->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp60A->r,0));{
# 3537
int i=Cyc_Toc_get_member_offset(_tmp691,_tmp60B->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_List_List*_tmp601=({struct _tuple19*_tmp607[2];_tmp607[1]=({struct _tuple19*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->f1=0;_tmp609->f2=_tmp60A;_tmp609;});_tmp607[0]=({struct _tuple19*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->f1=0;_tmp608->f2=field_tag_exp;_tmp608;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp607,sizeof(struct _tuple19*),2));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp601,0);
struct Cyc_List_List*ds=({void*_tmp604[1];_tmp604[0]=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp606;_tmp606.tag=1;_tmp606.f1=_tmp60B->name;_tmp606;});_tmp605;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp604,sizeof(void*),1));});
struct Cyc_List_List*dles=({struct _tuple19*_tmp602[1];_tmp602[0]=({struct _tuple19*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->f1=ds;_tmp603->f2=umem;_tmp603;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp602,sizeof(struct _tuple19*),1));});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3546
struct Cyc_List_List*_tmp60C=0;
struct Cyc_List_List*_tmp60D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp60D != 0;_tmp60D=_tmp60D->tl){
struct Cyc_List_List*_tmp60E=_tmp5FC;for(0;_tmp60E != 0;_tmp60E=_tmp60E->tl){
if((*((struct _tuple30*)_tmp60E->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp60D->hd){
struct _tuple30*_tmp60F=(struct _tuple30*)_tmp60E->hd;struct _tuple30*_tmp610=_tmp60F;struct Cyc_Absyn_Aggrfield*_tmp616;struct Cyc_Absyn_Exp*_tmp615;_LL3AA: _tmp616=_tmp610->f1;_tmp615=_tmp610->f2;_LL3AB:;{
void*_tmp611=Cyc_Toc_typ_to_c(_tmp616->type);
void*_tmp612=Cyc_Toc_typ_to_c((void*)_check_null(_tmp615->topt));
Cyc_Toc_exp_to_c(nv,_tmp615);
# 3556
if(!Cyc_Tcutil_unify(_tmp611,_tmp612)){
# 3558
if(!Cyc_Tcutil_is_arithmetic_type(_tmp611) || !
Cyc_Tcutil_is_arithmetic_type(_tmp612))
_tmp615=Cyc_Toc_cast_it(_tmp611,Cyc_Absyn_copy_exp(_tmp615));}
_tmp60C=({struct Cyc_List_List*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->hd=({struct _tuple19*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->f1=0;_tmp614->f2=_tmp615;_tmp614;});_tmp613->tl=_tmp60C;_tmp613;});
break;};}}}
# 3565
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp60C));}}
# 3523
;_pop_region(rgn);};}
# 3569
goto _LL280;case 29: _LL2BD: _tmp696=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp695=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL2BE: {
# 3571
struct Cyc_List_List*fs;
{void*_tmp617=Cyc_Tcutil_compress(_tmp696);void*_tmp618=_tmp617;struct Cyc_List_List*_tmp61C;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp618)->tag == 12){_LL3AD: _tmp61C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp618)->f2;_LL3AE:
 fs=_tmp61C;goto _LL3AC;}else{_LL3AF: _LL3B0:
({struct Cyc_String_pa_PrintArg_struct _tmp61B;_tmp61B.tag=0;_tmp61B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp696));({void*_tmp619[1]={& _tmp61B};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp61A="anon struct has type %s";_tag_dyneither(_tmp61A,sizeof(char),24);}),_tag_dyneither(_tmp619,sizeof(void*),1));});});}_LL3AC:;}{
# 3576
struct _RegionHandle _tmp61D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp61D;_push_region(rgn);{
struct Cyc_List_List*_tmp61E=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp695,Cyc_Absyn_StructA,fs);
for(0;_tmp61E != 0;_tmp61E=_tmp61E->tl){
struct _tuple30*_tmp61F=(struct _tuple30*)_tmp61E->hd;struct _tuple30*_tmp620=_tmp61F;struct Cyc_Absyn_Aggrfield*_tmp624;struct Cyc_Absyn_Exp*_tmp623;_LL3B2: _tmp624=_tmp620->f1;_tmp623=_tmp620->f2;_LL3B3:;{
void*_tmp621=(void*)_check_null(_tmp623->topt);
void*_tmp622=_tmp624->type;
Cyc_Toc_exp_to_c(nv,_tmp623);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp622) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp621))
_tmp623->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp623->r,0));};}
# 3590
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp695);}
# 3592
_npop_handler(0);goto _LL280;
# 3576
;_pop_region(rgn);};}case 30: _LL2BF: _tmp699=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp698=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_tmp697=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp489)->f3;_LL2C0: {
# 3595
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp625=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp626=Cyc_Absyn_var_exp(_tmp625,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp697->name,_tmp698->name));
tag_exp=_tmp698->is_extensible?Cyc_Absyn_var_exp(_tmp697->name,0):
 Cyc_Toc_datatype_tag(_tmp698,_tmp697->name);
mem_exp=_tmp626;{
struct Cyc_List_List*_tmp627=_tmp697->typs;
# 3606
if(Cyc_Toc_is_toplevel(nv)){
# 3608
struct Cyc_List_List*dles=0;
for(0;_tmp699 != 0;(_tmp699=_tmp699->tl,_tmp627=_tmp627->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp699->hd;
void*_tmp628=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp627))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp628))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);
dles=({struct Cyc_List_List*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->hd=({struct _tuple19*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->f1=0;_tmp62A->f2=cur_e;_tmp62A;});_tmp629->tl=dles;_tmp629;});}
# 3619
dles=({struct Cyc_List_List*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->hd=({struct _tuple19*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->f1=0;_tmp62C->f2=tag_exp;_tmp62C;});_tmp62B->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp62B;});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3625
struct Cyc_List_List*_tmp62D=({struct Cyc_List_List*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->hd=
Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp634->tl=0;_tmp634;});
# 3628
{int i=1;for(0;_tmp699 != 0;(((_tmp699=_tmp699->tl,i ++)),_tmp627=_tmp627->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp699->hd;
void*_tmp62E=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp627))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp62E))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp62F=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3638
_tmp62D=({struct Cyc_List_List*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->hd=_tmp62F;_tmp630->tl=_tmp62D;_tmp630;});};}}{
# 3640
struct Cyc_Absyn_Stmt*_tmp631=Cyc_Absyn_exp_stmt(_tmp626,0);
struct Cyc_Absyn_Stmt*_tmp632=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->hd=_tmp631;_tmp633->tl=_tmp62D;_tmp633;})),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp625,datatype_ctype,0,_tmp632,0));};}
# 3644
goto _LL280;};}case 31: _LL2C1: _LL2C2:
# 3646
 goto _LL2C4;case 32: _LL2C3: _LL2C4:
 goto _LL280;case 33: _LL2C5: _tmp69F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp489)->f1).is_calloc;_tmp69E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp489)->f1).rgn;_tmp69D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp489)->f1).elt_type;_tmp69C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp489)->f1).num_elts;_tmp69B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp489)->f1).fat_result;_tmp69A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp489)->f1).inline_call;_LL2C6: {
# 3650
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp69D)));
Cyc_Toc_exp_to_c(nv,_tmp69C);
# 3654
if(_tmp69B){
struct _tuple1*_tmp635=Cyc_Toc_temp_var();
struct _tuple1*_tmp636=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp69F){
xexp=_tmp69C;
if(_tmp69E != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp69E;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp635,0));}else{
# 3665
pexp=Cyc_Toc_calloc_exp(*_tmp69D,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp635,0));}
# 3667
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp637[3];_tmp637[2]=
# 3669
Cyc_Absyn_var_exp(_tmp635,0);_tmp637[1]=
# 3668
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp637[0]=Cyc_Absyn_var_exp(_tmp636,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp637,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{
# 3671
if(_tmp69E != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp69E;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp69A)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,Cyc_Absyn_var_exp(_tmp635,0));else{
# 3677
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp635,0));}}else{
# 3679
pexp=Cyc_Toc_malloc_exp(*_tmp69D,Cyc_Absyn_var_exp(_tmp635,0));}
# 3681
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp638[3];_tmp638[2]=
Cyc_Absyn_var_exp(_tmp635,0);_tmp638[1]=
# 3681
Cyc_Absyn_uint_exp(1,0);_tmp638[0]=Cyc_Absyn_var_exp(_tmp636,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp638,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}{
# 3684
struct Cyc_Absyn_Stmt*_tmp639=Cyc_Absyn_declare_stmt(_tmp635,Cyc_Absyn_uint_typ,_tmp69C,
Cyc_Absyn_declare_stmt(_tmp636,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp639);};}else{
# 3689
if(_tmp69F){
if(_tmp69E != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp69E;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp69C))->r;}else{
# 3695
e->r=(Cyc_Toc_calloc_exp(*_tmp69D,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp69C))->r;}}else{
# 3698
if(_tmp69E != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp69E;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp69A)
e->r=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp69C))->r;else{
# 3704
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp69C))->r;}}else{
# 3706
e->r=(Cyc_Toc_malloc_exp(*_tmp69D,_tmp69C))->r;}}}
# 3710
goto _LL280;}case 34: _LL2C7: _tmp6A1=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp6A0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL2C8: {
# 3719
void*e1_old_typ=(void*)_check_null(_tmp6A1->topt);
void*e2_old_typ=(void*)_check_null(_tmp6A0->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp63A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp63B="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp63B,sizeof(char),57);}),_tag_dyneither(_tmp63A,sizeof(void*),0));});{
# 3727
unsigned int _tmp63C=e->loc;
struct _tuple1*_tmp63D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp63E=Cyc_Absyn_var_exp(_tmp63D,_tmp63C);_tmp63E->topt=e1_old_typ;{
struct _tuple1*_tmp63F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp640=Cyc_Absyn_var_exp(_tmp63F,_tmp63C);_tmp640->topt=e2_old_typ;{
# 3733
struct Cyc_Absyn_Exp*_tmp641=Cyc_Absyn_assign_exp(Cyc_Tcutil_deep_copy_exp(1,_tmp6A1),_tmp640,_tmp63C);_tmp641->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp642=Cyc_Absyn_exp_stmt(_tmp641,_tmp63C);
struct Cyc_Absyn_Exp*_tmp643=Cyc_Absyn_assign_exp(Cyc_Tcutil_deep_copy_exp(1,_tmp6A0),_tmp63E,_tmp63C);_tmp643->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp644=Cyc_Absyn_exp_stmt(_tmp643,_tmp63C);
# 3738
struct Cyc_Absyn_Stmt*_tmp645=Cyc_Absyn_declare_stmt(_tmp63D,e1_old_typ,_tmp6A1,
Cyc_Absyn_declare_stmt(_tmp63F,e2_old_typ,_tmp6A0,
Cyc_Absyn_seq_stmt(_tmp642,_tmp644,_tmp63C),_tmp63C),_tmp63C);
Cyc_Toc_stmt_to_c(nv,_tmp645);
e->r=Cyc_Toc_stmt_exp_r(_tmp645);
goto _LL280;};};};};};}case 37: _LL2C9: _tmp6A3=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_tmp6A2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp489)->f2;_LL2CA: {
# 3746
void*_tmp646=Cyc_Tcutil_compress((void*)_check_null(_tmp6A3->topt));
Cyc_Toc_exp_to_c(nv,_tmp6A3);
{void*_tmp647=_tmp646;struct Cyc_Absyn_Aggrdecl*_tmp64D;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp647)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp647)->f1).aggr_info).KnownAggr).tag == 2){_LL3B5: _tmp64D=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp647)->f1).aggr_info).KnownAggr).val;_LL3B6: {
# 3750
struct Cyc_Absyn_Exp*_tmp648=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp64D,_tmp6A2),0);
struct Cyc_Absyn_Exp*_tmp649=Cyc_Toc_member_exp(_tmp6A3,_tmp6A2,0);
struct Cyc_Absyn_Exp*_tmp64A=Cyc_Toc_member_exp(_tmp649,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp64A,_tmp648,0))->r;
goto _LL3B4;}}else{goto _LL3B7;}}else{_LL3B7: _LL3B8:
({void*_tmp64B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp64C="non-aggregate type in tagcheck";_tag_dyneither(_tmp64C,sizeof(char),31);}),_tag_dyneither(_tmp64B,sizeof(void*),0));});}_LL3B4:;}
# 3757
goto _LL280;}case 36: _LL2CB: _tmp6A4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp489)->f1;_LL2CC:
 Cyc_Toc_stmt_to_c(nv,_tmp6A4);goto _LL280;case 35: _LL2CD: _LL2CE:
({void*_tmp64E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp64F="UnresolvedMem";_tag_dyneither(_tmp64F,sizeof(char),14);}),_tag_dyneither(_tmp64E,sizeof(void*),0));});case 24: _LL2CF: _LL2D0:
({void*_tmp650=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp651="compoundlit";_tag_dyneither(_tmp651,sizeof(char),12);}),_tag_dyneither(_tmp650,sizeof(void*),0));});case 38: _LL2D1: _LL2D2:
({void*_tmp652=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp653="valueof(-)";_tag_dyneither(_tmp653,sizeof(char),11);}),_tag_dyneither(_tmp652,sizeof(void*),0));});default: _LL2D3: _LL2D4:
({void*_tmp654=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp655="__asm__";_tag_dyneither(_tmp655,sizeof(char),8);}),_tag_dyneither(_tmp654,sizeof(void*),0));});}_LL280:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3790 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3792
return({struct _tuple31*_tmp6A5=_region_malloc(r,sizeof(*_tmp6A5));_tmp6A5->f1=0;_tmp6A5->f2=Cyc_Toc_fresh_label();_tmp6A5->f3=Cyc_Toc_fresh_label();_tmp6A5->f4=sc;_tmp6A5;});}
# 3797
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp6A6=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3803
if((int)(((_tmp6A6->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp6A9=_tmp6A6->orig_pat;if((_tmp6A9.pattern).tag != 1)_throw_match();(_tmp6A9.pattern).val;})->topt);
void*_tmp6A7=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp6A8=_tmp6A7;switch(*((int*)_tmp6A8)){case 0: _LL3BA: _LL3BB:
# 3808
 goto _LL3BD;case 11: _LL3BC: _LL3BD:
 goto _LL3BF;case 12: _LL3BE: _LL3BF:
 goto _LL3B9;default: _LL3C0: _LL3C1:
 v=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(t),v);goto _LL3B9;}_LL3B9:;}{
# 3814
void*_tmp6AA=_tmp6A6->access;void*_tmp6AB=_tmp6AA;struct Cyc_Absyn_Datatypedecl*_tmp6B6;struct Cyc_Absyn_Datatypefield*_tmp6B5;unsigned int _tmp6B4;int _tmp6B3;struct _dyneither_ptr*_tmp6B2;unsigned int _tmp6B1;switch(*((int*)_tmp6AB)){case 0: _LL3C3: _LL3C4:
# 3819
 goto _LL3C2;case 1: _LL3C5: _LL3C6:
# 3824
 if(ps->tl != 0){
void*_tmp6AC=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp6AD=_tmp6AC;struct Cyc_Absyn_Datatypedecl*_tmp6B0;struct Cyc_Absyn_Datatypefield*_tmp6AF;unsigned int _tmp6AE;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AD)->tag == 3){_LL3CE: _tmp6B0=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AD)->f1;_tmp6AF=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AD)->f2;_tmp6AE=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AD)->f3;_LL3CF:
# 3827
 ps=ps->tl;
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6AF->name,_tmp6B0->name)),Cyc_Toc_mt_tq),v);
v=Cyc_Absyn_aggrarrow_exp(v,Cyc_Absyn_fieldname((int)(_tmp6AE + 1)),0);
continue;}else{_LL3D0: _LL3D1:
# 3832
 goto _LL3CD;}_LL3CD:;}
# 3835
v=Cyc_Absyn_deref_exp(v,0);
goto _LL3C2;case 2: _LL3C7: _tmp6B1=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp6AB)->f1;_LL3C8:
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6B1 + 1)),0);goto _LL3C2;case 4: _LL3C9: _tmp6B3=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6AB)->f1;_tmp6B2=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6AB)->f2;_LL3CA:
# 3839
 v=Cyc_Toc_member_exp(v,_tmp6B2,0);
if(_tmp6B3)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0);
goto _LL3C2;default: _LL3CB: _tmp6B6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AB)->f1;_tmp6B5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AB)->f2;_tmp6B4=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AB)->f3;_LL3CC:
# 3844
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6B4 + 1)),0);
goto _LL3C2;}_LL3C2:;};}
# 3848
return v;}
# 3853
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp6B7=t;struct Cyc_Absyn_Datatypedecl*_tmp6D2;struct Cyc_Absyn_Datatypefield*_tmp6D1;struct _dyneither_ptr*_tmp6D0;int _tmp6CF;int _tmp6CE;struct Cyc_Absyn_Datatypedecl*_tmp6CD;struct Cyc_Absyn_Datatypefield*_tmp6CC;unsigned int _tmp6CB;struct _dyneither_ptr _tmp6CA;int _tmp6C9;void*_tmp6C8;struct Cyc_Absyn_Enumfield*_tmp6C7;struct Cyc_Absyn_Enumdecl*_tmp6C6;struct Cyc_Absyn_Enumfield*_tmp6C5;struct Cyc_Absyn_Exp*_tmp6C4;switch(*((int*)_tmp6B7)){case 0: _LL3D3: _tmp6C4=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6B7)->f1;_LL3D4:
# 3857
 if(_tmp6C4 == 0)return v;else{return _tmp6C4;}case 1: _LL3D5: _LL3D6:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 2: _LL3D7: _LL3D8:
 return Cyc_Absyn_neq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 3: _LL3D9: _tmp6C6=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6B7)->f1;_tmp6C5=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6B7)->f2;_LL3DA:
# 3861
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6B9;_tmp6B9.tag=31;_tmp6B9.f1=_tmp6C6;_tmp6B9.f2=_tmp6C5;_tmp6B9;});_tmp6B8;}),0),0);case 4: _LL3DB: _tmp6C8=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6B7)->f1;_tmp6C7=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6B7)->f2;_LL3DC:
# 3863
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6BB;_tmp6BB.tag=32;_tmp6BB.f1=_tmp6C8;_tmp6BB.f2=_tmp6C7;_tmp6BB;});_tmp6BA;}),0),0);case 5: _LL3DD: _tmp6CA=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6B7)->f1;_tmp6C9=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6B7)->f2;_LL3DE:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_float_exp(_tmp6CA,_tmp6C9,0),0);case 6: _LL3DF: _tmp6CB=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6B7)->f1;_LL3E0:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp(_tmp6CB,0),0);case 7: _LL3E1: _tmp6CE=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6B7)->f1;_tmp6CD=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6B7)->f2;_tmp6CC=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6B7)->f3;_LL3E2:
# 3869
 LOOP1: {
void*_tmp6BC=v->r;void*_tmp6BD=_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BF;struct Cyc_Absyn_Exp*_tmp6BE;switch(*((int*)_tmp6BD)){case 13: _LL3E8: _tmp6BE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6BD)->f2;_LL3E9:
 v=_tmp6BE;goto LOOP1;case 19: _LL3EA: _tmp6BF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6BD)->f1;_LL3EB:
 v=_tmp6BF;goto _LL3E7;default: _LL3EC: _LL3ED:
 goto _LL3E7;}_LL3E7:;}
# 3876
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6CC->name,_tmp6CD->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_uint_exp((unsigned int)_tmp6CE,0),0);case 8: _LL3E3: _tmp6D0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6B7)->f1;_tmp6CF=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6B7)->f2;_LL3E4:
# 3879
 v=Cyc_Toc_member_exp(Cyc_Toc_member_exp(v,_tmp6D0,0),Cyc_Toc_tag_sp,0);
return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp((unsigned int)_tmp6CF,0),0);default: _LL3E5: _tmp6D2=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6B7)->f1;_tmp6D1=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6B7)->f2;_LL3E6:
# 3883
 LOOP2: {
void*_tmp6C0=v->r;void*_tmp6C1=_tmp6C0;struct Cyc_Absyn_Exp*_tmp6C3;struct Cyc_Absyn_Exp*_tmp6C2;switch(*((int*)_tmp6C1)){case 13: _LL3EF: _tmp6C2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6C1)->f2;_LL3F0:
 v=_tmp6C2;goto LOOP2;case 19: _LL3F1: _tmp6C3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6C1)->f1;_LL3F2:
 v=_tmp6C3;goto _LL3EE;default: _LL3F3: _LL3F4:
 goto _LL3EE;}_LL3EE:;}
# 3890
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6D1->name,_tmp6D2->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp6D1->name,0),0);}_LL3D2:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3902
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple32{int f1;void*f2;};
# 3910
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3912
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp6D5=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp6D6=_tmp6D5;void*_tmp6E0;_LL3F6: _tmp6E0=_tmp6D6.f1;_LL3F7:;{
void*_tmp6D7=_tmp6E0;struct Cyc_Absyn_Exp*_tmp6DF;struct _dyneither_ptr*_tmp6DE;switch(*((int*)_tmp6D7)){case 3: _LL3F9: _LL3FA:
# 3918
 goto _LL3FC;case 4: _LL3FB: _LL3FC:
 goto _LL3FE;case 6: _LL3FD: _LL3FE:
# 3921
 continue;case 8: _LL3FF: _tmp6DE=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6D7)->f1;_LL400:
# 3923
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmp6D9;_tmp6D9.tag=3;_tmp6D9.f1=_tmp6DE;_tmp6D9;});_tmp6D8;});
continue;case 7: _LL401: _LL402:
# 3927
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0: _LL403: _tmp6DF=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6D7)->f1;if(_tmp6DF != 0){_LL404:
# 3931
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA[0]=({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmp6DB;_tmp6DB.tag=2;_tmp6DB.f1=_tmp6DF;_tmp6DB;});_tmp6DA;});
return({struct _tuple32 _tmp6DC;_tmp6DC.f1=0;_tmp6DC.f2=k;_tmp6DC;});}else{_LL405: _LL406:
 goto _LL408;}case 1: _LL407: _LL408:
 goto _LL40A;case 2: _LL409: _LL40A:
 goto _LL40C;case 5: _LL40B: _LL40C:
 goto _LL40E;default: _LL40D: _LL40E:
 return({struct _tuple32 _tmp6DD;_tmp6DD.f1=0;_tmp6DD.f2=k;_tmp6DD;});}_LL3F8:;};}
# 3940
return({struct _tuple32 _tmp6E1;_tmp6E1.f1=c;_tmp6E1.f2=k;_tmp6E1;});}
# 3945
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp6E2=p;int _tmp6EF;int _tmp6EE;unsigned int _tmp6ED;void*_tmp6EC;struct Cyc_Absyn_Enumfield*_tmp6EB;struct Cyc_Absyn_Enumdecl*_tmp6EA;struct Cyc_Absyn_Enumfield*_tmp6E9;switch(*((int*)_tmp6E2)){case 3: _LL410: _tmp6EA=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6E2)->f1;_tmp6E9=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6E2)->f2;_LL411:
# 3949
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6E4;_tmp6E4.tag=31;_tmp6E4.f1=_tmp6EA;_tmp6E4.f2=_tmp6E9;_tmp6E4;});_tmp6E3;}),0);goto _LL40F;case 4: _LL412: _tmp6EC=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6E2)->f1;_tmp6EB=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6E2)->f2;_LL413:
# 3951
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6E6;_tmp6E6.tag=32;_tmp6E6.f1=_tmp6EC;_tmp6E6.f2=_tmp6EB;_tmp6E6;});_tmp6E5;}),0);goto _LL40F;case 6: _LL414: _tmp6ED=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6E2)->f1;_LL415:
 _tmp6EE=(int)_tmp6ED;goto _LL417;case 7: _LL416: _tmp6EE=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6E2)->f1;_LL417:
 _tmp6EF=_tmp6EE;goto _LL419;case 8: _LL418: _tmp6EF=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6E2)->f2;_LL419:
# 3955
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp6EF,0);
goto _LL40F;default: _LL41A: _LL41B:
({void*_tmp6E7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6E8="compile_pat_test_as_case!";_tag_dyneither(_tmp6E8,sizeof(char),26);}),_tag_dyneither(_tmp6E7,sizeof(void*),0));});}_LL40F:;}
# 3959
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0[0]=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp6F1;_tmp6F1.tag=17;_tmp6F1.f1=e;_tmp6F1;});_tmp6F0;}),0);}
# 3963
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0);}struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3972
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3975
void*t=(void*)_check_null(p->topt);
void*_tmp6F2=p->r;void*_tmp6F3=_tmp6F2;union Cyc_Absyn_AggrInfoU _tmp72B;struct Cyc_List_List*_tmp72A;struct Cyc_List_List*_tmp729;struct Cyc_List_List*_tmp728;struct Cyc_Absyn_Pat*_tmp727;struct Cyc_Absyn_Datatypedecl*_tmp726;struct Cyc_Absyn_Datatypefield*_tmp725;struct Cyc_List_List*_tmp724;struct Cyc_Absyn_Vardecl*_tmp723;struct Cyc_Absyn_Pat*_tmp722;struct Cyc_Absyn_Vardecl*_tmp721;struct Cyc_Absyn_Vardecl*_tmp720;struct Cyc_Absyn_Pat*_tmp71F;struct Cyc_Absyn_Vardecl*_tmp71E;switch(*((int*)_tmp6F3)){case 2: _LL41D: _tmp71E=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp6F3)->f2;_LL41E: {
# 3978
struct Cyc_Absyn_Pat*_tmp6F4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp6F4->topt=p->topt;
_tmp720=_tmp71E;_tmp71F=_tmp6F4;goto _LL420;}case 1: _LL41F: _tmp720=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6F3)->f1;_tmp71F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6F3)->f2;_LL420:
# 3983
*decls=({struct Cyc_List_List*_tmp6F5=_region_malloc(rgn,sizeof(*_tmp6F5));_tmp6F5->hd=_tmp720;_tmp6F5->tl=*decls;_tmp6F5;});{
struct Cyc_Absyn_Stmt*_tmp6F6=Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6F8;_tmp6F8.tag=4;_tmp6F8.f1=_tmp720;_tmp6F8;});_tmp6F7;}),0),Cyc_Absyn_copy_exp(path),0);
return Cyc_Toc_seq_stmt_opt(_tmp6F6,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp71F));};case 4: _LL421: _tmp721=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp6F3)->f2;_LL422:
# 3988
*decls=({struct Cyc_List_List*_tmp6F9=_region_malloc(rgn,sizeof(*_tmp6F9));_tmp6F9->hd=_tmp721;_tmp6F9->tl=*decls;_tmp6F9;});
return Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6FB;_tmp6FB.tag=4;_tmp6FB.f1=_tmp721;_tmp6FB;});_tmp6FA;}),0),Cyc_Absyn_copy_exp(path),0);case 0: _LL423: _LL424:
 return 0;case 3: _LL425: _tmp723=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6F3)->f1;_tmp722=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6F3)->f2;_LL426:
# 3993
*decls=({struct Cyc_List_List*_tmp6FC=_region_malloc(rgn,sizeof(*_tmp6FC));_tmp6FC->hd=_tmp723;_tmp6FC->tl=*decls;_tmp6FC;});
_tmp723->type=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);{
# 3996
struct Cyc_Absyn_Stmt*_tmp6FD=Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6FF;_tmp6FF.tag=4;_tmp6FF.f1=_tmp723;_tmp6FF;});_tmp6FE;}),0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path))),0);
return Cyc_Toc_seq_stmt_opt(_tmp6FD,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp722));};case 9: _LL427: _LL428:
# 4001
 goto _LL42A;case 10: _LL429: _LL42A:
 goto _LL42C;case 11: _LL42B: _LL42C:
 goto _LL42E;case 12: _LL42D: _LL42E:
 goto _LL430;case 13: _LL42F: _LL430:
 goto _LL432;case 14: _LL431: _LL432:
 return 0;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F3)->f1)->r)->tag == 8){_LL433: _tmp726=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F3)->f1)->r)->f1;_tmp725=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F3)->f1)->r)->f2;_tmp724=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F3)->f1)->r)->f3;_LL434:
# 4010
 if(_tmp724 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp725->name,_tmp726->name);
void*_tmp700=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
path=Cyc_Toc_cast_it(_tmp700,path);{
int cnt=1;
struct Cyc_List_List*_tmp701=_tmp725->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp724 != 0;(((_tmp724=_tmp724->tl,_tmp701=((struct Cyc_List_List*)_check_null(_tmp701))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp702=(struct Cyc_Absyn_Pat*)_tmp724->hd;
if(_tmp702->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp703=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp701))->hd)).f2;
void*_tmp704=(void*)_check_null(_tmp702->topt);
void*_tmp705=Cyc_Toc_typ_to_c_array(_tmp704);
struct Cyc_Absyn_Exp*_tmp706=Cyc_Absyn_aggrarrow_exp(path,Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp703)))
_tmp706=Cyc_Toc_cast_it(_tmp705,_tmp706);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp706,_tmp702));};}
# 4028
return s;};};}else{_LL43D: _tmp727=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F3)->f1;_LL43E:
# 4077
 return Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Absyn_deref_exp(path,0),_tmp727);}case 8: _LL435: _tmp728=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp6F3)->f3;_LL436:
# 4030
 _tmp729=_tmp728;goto _LL438;case 5: _LL437: _tmp729=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp6F3)->f1;_LL438: {
# 4032
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp729 != 0;(_tmp729=_tmp729->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp707=(struct Cyc_Absyn_Pat*)_tmp729->hd;
if(_tmp707->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp708=(void*)_check_null(_tmp707->topt);
struct _dyneither_ptr*_tmp709=Cyc_Absyn_fieldname(cnt);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Toc_member_exp(path,_tmp709,0),_tmp707));};}
# 4042
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F3)->f1 == 0){_LL439: _LL43A:
({void*_tmp70A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp70B="unresolved aggregate pattern!";_tag_dyneither(_tmp70B,sizeof(char),30);}),_tag_dyneither(_tmp70A,sizeof(void*),0));});}else{_LL43B: _tmp72B=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F3)->f1)->aggr_info;_tmp72A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F3)->f3;_LL43C: {
# 4046
struct Cyc_Absyn_Aggrdecl*_tmp70C=Cyc_Absyn_get_known_aggrdecl(_tmp72B);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp72A != 0;_tmp72A=_tmp72A->tl){
struct _tuple33*_tmp70D=(struct _tuple33*)_tmp72A->hd;
struct Cyc_Absyn_Pat*_tmp70E=(*_tmp70D).f2;
if(_tmp70E->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp70F=(void*)((struct Cyc_List_List*)_check_null((*_tmp70D).f1))->hd;void*_tmp710=_tmp70F;struct _dyneither_ptr*_tmp713;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp710)->tag == 1){_LL446: _tmp713=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp710)->f1;_LL447:
 f=_tmp713;goto _LL445;}else{_LL448: _LL449:
({void*_tmp711=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp712="multiple designators in pattern!";_tag_dyneither(_tmp712,sizeof(char),33);}),_tag_dyneither(_tmp711,sizeof(void*),0));});}_LL445:;}{
# 4058
void*_tmp714=(void*)_check_null(_tmp70E->topt);
void*_tmp715=Cyc_Toc_typ_to_c_array(_tmp714);
struct Cyc_Absyn_Exp*_tmp716=Cyc_Toc_member_exp(path,f,0);
# 4062
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp70C->impl))->tagged)_tmp716=Cyc_Toc_member_exp(_tmp716,Cyc_Toc_val_sp,0);{
void*_tmp717=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp70C->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp717))
_tmp716=Cyc_Toc_cast_it(_tmp715,_tmp716);else{
if(!Cyc_Toc_is_array_type(_tmp717) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp717)))
# 4069
_tmp716=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp715,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp716,0)),0);}
# 4072
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp716,_tmp70E));};};};}
# 4074
return s;}}case 15: _LL43F: _LL440:
# 4079
({void*_tmp718=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp719="unknownid pat";_tag_dyneither(_tmp719,sizeof(char),14);}),_tag_dyneither(_tmp718,sizeof(void*),0));});case 16: _LL441: _LL442:
({void*_tmp71A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp71B="unknowncall pat";_tag_dyneither(_tmp71B,sizeof(char),16);}),_tag_dyneither(_tmp71A,sizeof(void*),0));});default: _LL443: _LL444:
({void*_tmp71C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp71D="exp pat";_tag_dyneither(_tmp71D,sizeof(char),8);}),_tag_dyneither(_tmp71C,sizeof(void*),0));});}_LL41C:;}struct _tuple34{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4090
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4098
void*_tmp72C=dopt;struct Cyc_List_List*_tmp76B;struct Cyc_List_List*_tmp76A;void*_tmp769;struct Cyc_Tcpat_Rhs*_tmp768;if(_tmp72C == 0){_LL44B: _LL44C:
# 4100
 return Cyc_Absyn_skip_stmt(0);}else{switch(*((int*)_tmp72C)){case 0: _LL44D: _LL44E:
 return Cyc_Toc_throw_match_stmt();case 1: _LL44F: _tmp768=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp72C)->f1;_LL450:
# 4105
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp72D=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp72E=_tmp72D;int*_tmp735;struct _dyneither_ptr*_tmp734;struct _dyneither_ptr*_tmp733;struct Cyc_Absyn_Switch_clause*_tmp732;_LL454: _tmp735=(int*)& _tmp72E->f1;_tmp734=_tmp72E->f2;_tmp733=_tmp72E->f3;_tmp732=_tmp72E->f4;_LL455:;{
struct Cyc_Absyn_Stmt*_tmp72F=_tmp732->body;
if(_tmp72F == _tmp768->rhs){
# 4110
if(*_tmp735)
return Cyc_Absyn_goto_stmt(_tmp734,0);
*_tmp735=1;{
# 4114
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp732->pattern);
# 4117
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp733,_tmp732->body,0);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0);
res=Cyc_Absyn_label_stmt(_tmp734,res,0);
*bodies=({struct Cyc_List_List*_tmp730=_region_malloc(rgn,sizeof(*_tmp730));_tmp730->hd=({struct _tuple34*_tmp731=_region_malloc(rgn,sizeof(*_tmp731));_tmp731->f1=nv;_tmp731->f2=_tmp733;_tmp731->f3=_tmp72F;_tmp731;});_tmp730->tl=*bodies;_tmp730;});
return res;};}};}
# 4125
({void*_tmp736=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp737="couldn't find rhs!";_tag_dyneither(_tmp737,sizeof(char),19);}),_tag_dyneither(_tmp736,sizeof(void*),0));});default: _LL451: _tmp76B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp72C)->f1;_tmp76A=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp72C)->f2;_tmp769=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp72C)->f3;_LL452: {
# 4128
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp769,lscs,v);
# 4130
struct Cyc_Absyn_Exp*_tmp738=Cyc_Toc_compile_path(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp76B),Cyc_Absyn_var_exp(v,0));
struct Cyc_List_List*_tmp739=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp76A);
# 4133
struct _tuple32 _tmp73A=Cyc_Toc_admits_switch(_tmp739);struct _tuple32 _tmp73B=_tmp73A;int _tmp767;void*_tmp766;_LL457: _tmp767=_tmp73B.f1;_tmp766=_tmp73B.f2;_LL458:;
if(_tmp767 > 1){
# 4137
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C->hd=({struct Cyc_Absyn_Switch_clause*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp74D->pat_vars=0;_tmp74D->where_clause=0;_tmp74D->body=res;_tmp74D->loc=0;_tmp74D;});_tmp74C->tl=0;_tmp74C;});
# 4140
for(0;_tmp739 != 0;_tmp739=_tmp739->tl){
struct _tuple29 _tmp73C=*((struct _tuple29*)_tmp739->hd);struct _tuple29 _tmp73D=_tmp73C;void*_tmp743;void*_tmp742;_LL45A: _tmp743=_tmp73D.f1;_tmp742=_tmp73D.f2;_LL45B:;{
# 4143
struct Cyc_Absyn_Pat*_tmp73E=Cyc_Toc_compile_pat_test_as_case(_tmp743);
# 4145
struct Cyc_Absyn_Stmt*_tmp73F=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp742,lscs,v);
# 4147
new_lscs=({struct Cyc_List_List*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740->hd=({struct Cyc_Absyn_Switch_clause*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741->pattern=_tmp73E;_tmp741->pat_vars=0;_tmp741->where_clause=0;_tmp741->body=_tmp73F;_tmp741->loc=0;_tmp741;});_tmp740->tl=new_lscs;_tmp740;});};}
# 4149
{void*_tmp744=_tmp766;struct _dyneither_ptr*_tmp749;switch(*((int*)_tmp744)){case 1: _LL45D: _LL45E:
# 4152
 LOOP1: {
void*_tmp745=_tmp738->r;void*_tmp746=_tmp745;struct Cyc_Absyn_Exp*_tmp748;struct Cyc_Absyn_Exp*_tmp747;switch(*((int*)_tmp746)){case 13: _LL466: _tmp747=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp746)->f2;_LL467:
 _tmp738=_tmp747;goto LOOP1;case 19: _LL468: _tmp748=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp746)->f1;_LL469:
 _tmp738=_tmp748;goto _LL465;default: _LL46A: _LL46B:
 goto _LL465;}_LL465:;}
# 4158
_tmp738=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),_tmp738),0);goto _LL45C;case 3: _LL45F: _tmp749=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp744)->f1;_LL460:
# 4161
 _tmp738=Cyc_Toc_member_exp(Cyc_Toc_member_exp(_tmp738,_tmp749,0),Cyc_Toc_tag_sp,0);goto _LL45C;case 2: _LL461: _LL462:
# 4163
 goto _LL45C;default: _LL463: _LL464:
 goto _LL45C;}_LL45C:;}
# 4166
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp74B;_tmp74B.tag=10;_tmp74B.f1=_tmp738;_tmp74B.f2=new_lscs;_tmp74B.f3=0;_tmp74B;});_tmp74A;}),0);}else{
# 4170
void*_tmp74E=_tmp766;struct Cyc_Absyn_Exp*_tmp765;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp74E)->tag == 2){_LL46D: _tmp765=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp74E)->f1;_LL46E: {
# 4172
struct Cyc_List_List*_tmp74F=_tmp739;void*_tmp75E;struct Cyc_Tcpat_Rhs*_tmp75D;if(_tmp74F != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp74F)->hd)->f2)->tag == 1){if(((struct Cyc_List_List*)_tmp74F)->tl == 0){_LL472: _tmp75E=((struct _tuple29*)_tmp74F->hd)->f1;_tmp75D=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp74F->hd)->f2)->f1;_LL473:
# 4178
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp750=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp751=_tmp750;int*_tmp758;struct _dyneither_ptr*_tmp757;struct _dyneither_ptr*_tmp756;struct Cyc_Absyn_Switch_clause*_tmp755;_LL477: _tmp758=(int*)& _tmp751->f1;_tmp757=_tmp751->f2;_tmp756=_tmp751->f3;_tmp755=_tmp751->f4;_LL478:;{
struct Cyc_Absyn_Stmt*_tmp752=_tmp755->body;
if(_tmp752 == _tmp75D->rhs){
# 4183
if(*_tmp758)
return Cyc_Absyn_goto_stmt(_tmp757,0);
*_tmp758=1;{
# 4187
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp755->pattern);
# 4190
Cyc_Toc_exp_to_c(nv,_tmp765);{
# 4193
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp756,_tmp755->body,0);
r=Cyc_Absyn_ifthenelse_stmt(_tmp765,r,res,0);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0);
r=Cyc_Absyn_label_stmt(_tmp757,r,0);
*bodies=({struct Cyc_List_List*_tmp753=_region_malloc(rgn,sizeof(*_tmp753));_tmp753->hd=({struct _tuple34*_tmp754=_region_malloc(rgn,sizeof(*_tmp754));_tmp754->f1=nv;_tmp754->f2=_tmp756;_tmp754->f3=_tmp752;_tmp754;});_tmp753->tl=*bodies;_tmp753;});
return r;};};}};}
# 4202
({void*_tmp759=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp75A="couldn't find rhs!";_tag_dyneither(_tmp75A,sizeof(char),19);}),_tag_dyneither(_tmp759,sizeof(void*),0));});}else{goto _LL474;}}else{goto _LL474;}}else{_LL474: _LL475:
({void*_tmp75B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp75C="bad where clause in match compiler";_tag_dyneither(_tmp75C,sizeof(char),35);}),_tag_dyneither(_tmp75B,sizeof(void*),0));});}_LL471:;}}else{_LL46F: _LL470:
# 4207
 for(0;_tmp739 != 0;_tmp739=_tmp739->tl){
struct _tuple29 _tmp75F=*((struct _tuple29*)_tmp739->hd);struct _tuple29 _tmp760=_tmp75F;void*_tmp764;void*_tmp763;_LL47A: _tmp764=_tmp760.f1;_tmp763=_tmp760.f2;_LL47B:;{
struct Cyc_Absyn_Exp*_tmp761=Cyc_Toc_compile_pat_test(_tmp738,_tmp764);
struct Cyc_Absyn_Stmt*_tmp762=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp763,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp761,_tmp762,res,0);};}}_LL46C:;}
# 4215
return res;}}}_LL44A:;}
# 4225
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4227
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple34*_tmp76C=(struct _tuple34*)bodies->hd;struct _tuple34*_tmp76D=_tmp76C;struct Cyc_Toc_Env**_tmp76F;struct Cyc_Absyn_Stmt*_tmp76E;_LL47D: _tmp76F=(struct Cyc_Toc_Env**)& _tmp76D->f1;_tmp76E=_tmp76D->f3;_LL47E:;
if(_tmp76E == s)return _tmp76F;}
# 4233
return 0;}
# 4237
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4240
void*_tmp770=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4243
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp771=_new_region("rgn");struct _RegionHandle*rgn=& _tmp771;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp772=Cyc_Toc_share_env(rgn,nv);
# 4250
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4255
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp772,& mydecls,& mybodies,dopt,lscs,v);
# 4263
{struct Cyc_List_List*_tmp773=lscs;for(0;_tmp773 != 0;_tmp773=_tmp773->tl){
struct _tuple31*_tmp774=(struct _tuple31*)_tmp773->hd;struct _tuple31*_tmp775=_tmp774;struct _dyneither_ptr*_tmp77D;struct Cyc_Absyn_Switch_clause*_tmp77C;_LL480: _tmp77D=_tmp775->f3;_tmp77C=_tmp775->f4;_LL481:;{
struct Cyc_Absyn_Stmt*s=_tmp77C->body;
# 4267
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4270
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp776=*envp;
# 4273
if(_tmp773->tl != 0){
struct _tuple31*_tmp777=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp773->tl))->hd;struct _tuple31*_tmp778=_tmp777;struct _dyneither_ptr*_tmp77B;struct Cyc_Absyn_Switch_clause*_tmp77A;_LL483: _tmp77B=_tmp778->f3;_tmp77A=_tmp778->f4;_LL484:;{
# 4277
struct Cyc_Toc_Env**_tmp779=Cyc_Toc_find_case_env(mybodies,_tmp77A->body);
# 4282
if(_tmp779 == 0)
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp776,end_l),s);else{
# 4286
struct Cyc_List_List*vs=0;
if(_tmp77A->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp77A->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 4291
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn,_tmp776,end_l,_tmp77B,vs),s);}};}else{
# 4295
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp776,end_l),s);}};};}}{
# 4299
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4301
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp77E=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
_tmp77E->name=Cyc_Toc_temp_var();
_tmp77E->type=Cyc_Toc_typ_to_c_array(_tmp77E->type);
res=Cyc_Absyn_decl_stmt(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp780;_tmp780.tag=0;_tmp780.f1=_tmp77E;_tmp780;});_tmp77F;}),0),res,0);}
# 4308
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;
_npop_handler(0);return;};}
# 4247
;_pop_region(rgn);};}
# 4314
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 4316
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4321
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4323
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp781=_cycalloc(sizeof(*_tmp781));_tmp781->hd=
Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp781->tl=0;_tmp781;}),0),0);}
# 4327
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4332
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4334
while(1){
void*_tmp782=s->r;void*_tmp783=_tmp782;struct Cyc_Absyn_Exp*_tmp7E1;struct Cyc_Absyn_Stmt*_tmp7E0;struct Cyc_List_List*_tmp7DF;void*_tmp7DE;struct Cyc_Absyn_Stmt*_tmp7DD;struct Cyc_Absyn_Exp*_tmp7DC;struct _dyneither_ptr*_tmp7DB;struct Cyc_Absyn_Stmt*_tmp7DA;struct Cyc_Absyn_Decl*_tmp7D9;struct Cyc_Absyn_Stmt*_tmp7D8;struct Cyc_List_List*_tmp7D7;struct Cyc_Absyn_Switch_clause**_tmp7D6;struct Cyc_Absyn_Exp*_tmp7D5;struct Cyc_List_List*_tmp7D4;void*_tmp7D3;struct Cyc_Absyn_Exp*_tmp7D2;struct Cyc_Absyn_Exp*_tmp7D1;struct Cyc_Absyn_Exp*_tmp7D0;struct Cyc_Absyn_Stmt*_tmp7CF;struct Cyc_Absyn_Exp*_tmp7CE;struct Cyc_Absyn_Stmt*_tmp7CD;struct Cyc_Absyn_Exp*_tmp7CC;struct Cyc_Absyn_Stmt*_tmp7CB;struct Cyc_Absyn_Stmt*_tmp7CA;struct Cyc_Absyn_Exp*_tmp7C9;struct Cyc_Absyn_Stmt*_tmp7C8;struct Cyc_Absyn_Stmt*_tmp7C7;struct Cyc_Absyn_Exp*_tmp7C6;switch(*((int*)_tmp783)){case 0: _LL486: _LL487:
# 4337
 return;case 1: _LL488: _tmp7C6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_LL489:
# 4339
 Cyc_Toc_exp_to_c(nv,_tmp7C6);
return;case 2: _LL48A: _tmp7C8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_tmp7C7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp783)->f2;_LL48B:
# 4342
 Cyc_Toc_stmt_to_c(nv,_tmp7C8);
s=_tmp7C7;
continue;case 3: _LL48C: _tmp7C9=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_LL48D: {
# 4346
void*topt=0;
if(_tmp7C9 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7C9->topt));
Cyc_Toc_exp_to_c(nv,_tmp7C9);}{
# 4352
int _tmp784=Cyc_Toc_get_npop(s);
if(_tmp784 > 0){
if(topt != 0){
struct _tuple1*_tmp785=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp786=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp785,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp785,topt,_tmp7C9,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(_tmp784),_tmp786,0),0))->r;}else{
# 4361
Cyc_Toc_do_npop_before(_tmp784,s);}}
# 4363
return;};}case 4: _LL48E: _tmp7CC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_tmp7CB=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp783)->f2;_tmp7CA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp783)->f3;_LL48F:
# 4365
 Cyc_Toc_exp_to_c(nv,_tmp7CC);
Cyc_Toc_stmt_to_c(nv,_tmp7CB);
s=_tmp7CA;
continue;case 5: _LL490: _tmp7CE=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp783)->f1).f1;_tmp7CD=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp783)->f2;_LL491:
# 4370
 Cyc_Toc_exp_to_c(nv,_tmp7CE);{
struct _RegionHandle _tmp787=_new_region("temp");struct _RegionHandle*temp=& _tmp787;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7CD);
# 4374
_npop_handler(0);return;
# 4371
;_pop_region(temp);};case 6: _LL492: _LL493: {
# 4376
struct Cyc_Toc_Env*_tmp788=nv;struct _dyneither_ptr**_tmp789;_LL4A9: _tmp789=_tmp788->break_lab;_LL4AA:;
if(_tmp789 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp789);
# 4380
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;}case 7: _LL494: _LL495: {
# 4383
struct Cyc_Toc_Env*_tmp78A=nv;struct _dyneither_ptr**_tmp78B;_LL4AC: _tmp78B=_tmp78A->continue_lab;_LL4AD:;
if(_tmp78B != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp78B);
goto _LL497;}case 8: _LL496: _LL497:
# 4389
 Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;case 9: _LL498: _tmp7D2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_tmp7D1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp783)->f2).f1;_tmp7D0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp783)->f3).f1;_tmp7CF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp783)->f4;_LL499:
# 4393
 Cyc_Toc_exp_to_c(nv,_tmp7D2);Cyc_Toc_exp_to_c(nv,_tmp7D1);Cyc_Toc_exp_to_c(nv,_tmp7D0);{
struct _RegionHandle _tmp78C=_new_region("temp");struct _RegionHandle*temp=& _tmp78C;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7CF);
# 4397
_npop_handler(0);return;
# 4394
;_pop_region(temp);};case 10: _LL49A: _tmp7D5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_tmp7D4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp783)->f2;_tmp7D3=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp783)->f3;_LL49B:
# 4399
 Cyc_Toc_xlate_switch(nv,s,_tmp7D5,_tmp7D4,_tmp7D3);
return;case 11: _LL49C: _tmp7D7=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_tmp7D6=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp783)->f2;_LL49D: {
# 4402
struct Cyc_Toc_Env*_tmp78D=nv;struct Cyc_Toc_FallthruInfo*_tmp798;_LL4AF: _tmp798=_tmp78D->fallthru_info;_LL4B0:;
if(_tmp798 == 0)
({void*_tmp78E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp78F="fallthru in unexpected place";_tag_dyneither(_tmp78F,sizeof(char),29);}),_tag_dyneither(_tmp78E,sizeof(void*),0));});{
struct Cyc_Toc_FallthruInfo _tmp790=*_tmp798;struct Cyc_Toc_FallthruInfo _tmp791=_tmp790;struct _dyneither_ptr*_tmp797;struct Cyc_List_List*_tmp796;_LL4B2: _tmp797=_tmp791.label;_tmp796=_tmp791.binders;_LL4B3:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp797,0);
# 4408
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s2);{
struct Cyc_List_List*_tmp792=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp796);
struct Cyc_List_List*_tmp793=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7D7);
for(0;_tmp792 != 0;(_tmp792=_tmp792->tl,_tmp793=_tmp793->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp793))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp794=_cycalloc(sizeof(*_tmp794));_tmp794[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp795;_tmp795.tag=5;_tmp795.f1=(struct Cyc_Absyn_Vardecl*)_tmp792->hd;_tmp795;});_tmp794;}),0),(struct Cyc_Absyn_Exp*)_tmp793->hd,0),s2,0);}
# 4416
s->r=s2->r;
return;};};};}case 12: _LL49E: _tmp7D9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_tmp7D8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp783)->f2;_LL49F:
# 4422
{void*_tmp799=_tmp7D9->r;void*_tmp79A=_tmp799;struct Cyc_Absyn_Tvar*_tmp7B8;struct Cyc_Absyn_Vardecl*_tmp7B7;int _tmp7B6;struct Cyc_Absyn_Exp*_tmp7B5;struct Cyc_Absyn_Fndecl*_tmp7B4;struct Cyc_List_List*_tmp7B3;struct Cyc_Absyn_Pat*_tmp7B2;struct Cyc_Absyn_Exp*_tmp7B1;void*_tmp7B0;struct Cyc_Absyn_Vardecl*_tmp7AF;switch(*((int*)_tmp79A)){case 0: _LL4B5: _tmp7AF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp79A)->f1;_LL4B6:
 Cyc_Toc_local_decl_to_c(nv,_tmp7AF,_tmp7D8);goto _LL4B4;case 2: _LL4B7: _tmp7B2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79A)->f1;_tmp7B1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79A)->f3;_tmp7B0=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79A)->f4;_LL4B8:
# 4430
{void*_tmp79B=_tmp7B2->r;void*_tmp79C=_tmp79B;struct Cyc_Absyn_Vardecl*_tmp79F;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp79C)->tag == 1){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp79C)->f2)->r)->tag == 0){_LL4C2: _tmp79F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp79C)->f1;_LL4C3:
# 4432
 _tmp79F->name=Cyc_Toc_temp_var();
_tmp79F->initializer=_tmp7B1;
_tmp7D9->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp79D=_cycalloc(sizeof(*_tmp79D));_tmp79D[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp79E;_tmp79E.tag=0;_tmp79E.f1=_tmp79F;_tmp79E;});_tmp79D;});
Cyc_Toc_local_decl_to_c(nv,_tmp79F,_tmp7D8);
goto _LL4C1;}else{goto _LL4C4;}}else{_LL4C4: _LL4C5:
# 4441
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7B2,_tmp7B0,(void*)_check_null(_tmp7B1->topt),_tmp7B1,_tmp7D8))->r;
goto _LL4C1;}_LL4C1:;}
# 4444
goto _LL4B4;case 3: _LL4B9: _tmp7B3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp79A)->f1;_LL4BA: {
# 4448
struct Cyc_List_List*_tmp7A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7B3);
if(_tmp7A0 == 0)
({void*_tmp7A1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7A2="empty Letv_d";_tag_dyneither(_tmp7A2,sizeof(char),13);}),_tag_dyneither(_tmp7A1,sizeof(void*),0));});
_tmp7D9->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7A3=_cycalloc(sizeof(*_tmp7A3));_tmp7A3[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7A4;_tmp7A4.tag=0;_tmp7A4.f1=(struct Cyc_Absyn_Vardecl*)_tmp7A0->hd;_tmp7A4;});_tmp7A3;});
_tmp7A0=_tmp7A0->tl;
for(0;_tmp7A0 != 0;_tmp7A0=_tmp7A0->tl){
struct Cyc_Absyn_Decl*_tmp7A5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7A7;_tmp7A7.tag=0;_tmp7A7.f1=(struct Cyc_Absyn_Vardecl*)_tmp7A0->hd;_tmp7A7;});_tmp7A6;}),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7A5,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4457
Cyc_Toc_stmt_to_c(nv,s);
goto _LL4B4;}case 1: _LL4BB: _tmp7B4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp79A)->f1;_LL4BC:
# 4460
 Cyc_Toc_fndecl_to_c(nv,_tmp7B4,0);
Cyc_Toc_stmt_to_c(nv,_tmp7D8);
goto _LL4B4;case 4: _LL4BD: _tmp7B8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79A)->f1;_tmp7B7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79A)->f2;_tmp7B6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79A)->f3;_tmp7B5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79A)->f4;_LL4BE: {
# 4464
struct Cyc_Absyn_Stmt*_tmp7A8=_tmp7D8;
# 4472
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7B7->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4479
Cyc_Toc_stmt_to_c(nv,_tmp7A8);
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp7A8,0))->r;else{
if((unsigned int)_tmp7B5){
Cyc_Toc_exp_to_c(nv,_tmp7B5);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp7B5,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp7A8,0))->r;};}else{
# 4488
s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));_tmp7AA->hd=
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0);_tmp7AA->tl=0;_tmp7AA;}),0),
# 4493
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*_tmp7AB[1];_tmp7AB[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7AB,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),
Cyc_Absyn_seq_stmt(_tmp7A8,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp7AC[1];_tmp7AC[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7AC,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r;}}
# 4499
return;}default: _LL4BF: _LL4C0:
({void*_tmp7AD=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7AE="bad nested declaration within function";_tag_dyneither(_tmp7AE,sizeof(char),39);}),_tag_dyneither(_tmp7AD,sizeof(void*),0));});}_LL4B4:;}
# 4502
return;case 13: _LL4A0: _tmp7DB=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_tmp7DA=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp783)->f2;_LL4A1:
# 4504
 s=_tmp7DA;continue;case 14: _LL4A2: _tmp7DD=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_tmp7DC=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp783)->f2).f1;_LL4A3: {
# 4506
struct _RegionHandle _tmp7B9=_new_region("temp");struct _RegionHandle*temp=& _tmp7B9;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7DD);
Cyc_Toc_exp_to_c(nv,_tmp7DC);
# 4510
_npop_handler(0);return;
# 4506
;_pop_region(temp);}case 15: _LL4A4: _tmp7E0=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_tmp7DF=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp783)->f2;_tmp7DE=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp783)->f3;_LL4A5: {
# 4524 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4534
e_exp->topt=e_typ;{
struct _RegionHandle _tmp7BA=_new_region("temp");struct _RegionHandle*temp=& _tmp7BA;_push_region(temp);
# 4537
Cyc_Toc_stmt_to_c(nv,_tmp7E0);{
struct Cyc_Absyn_Stmt*_tmp7BB=Cyc_Absyn_seq_stmt(_tmp7E0,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4542
struct Cyc_Absyn_Stmt*_tmp7BC=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));_tmp7C3[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp7C4;_tmp7C4.tag=10;_tmp7C4.f1=e_exp;_tmp7C4.f2=_tmp7DF;_tmp7C4.f3=_tmp7DE;_tmp7C4;});_tmp7C3;}),0);
# 4544
Cyc_Toc_stmt_to_c(nv,_tmp7BC);{
# 4547
struct Cyc_Absyn_Exp*_tmp7BD=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp7C2=_cycalloc(sizeof(*_tmp7C2));_tmp7C2->hd=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp7C2->tl=0;_tmp7C2;}),0);
# 4551
struct Cyc_Absyn_Stmt*_tmp7BE=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));_tmp7C1->hd=
Cyc_Absyn_address_exp(h_exp,0);_tmp7C1->tl=0;_tmp7C1;}),0),0);
# 4555
struct Cyc_Absyn_Exp*_tmp7BF=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp7C0=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp7BE,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp7BF,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp7BD,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp7C0,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp7BB,
# 4566
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp7BC,0),0),0),0),0),0))->r;};};
# 4570
_npop_handler(0);return;
# 4535
;_pop_region(temp);};}default: _LL4A6: _tmp7E1=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp783)->f1;_LL4A7:
# 4572
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4575
Cyc_Toc_exp_to_c(nv,_tmp7E1);
s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp7C5=_cycalloc(sizeof(*_tmp7C5));_tmp7C5->hd=_tmp7E1;_tmp7C5->tl=0;_tmp7C5;}),0));}
# 4578
return;}_LL485:;}}
# 4587
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp7E2=_new_region("frgn");struct _RegionHandle*frgn=& _tmp7E2;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp7E3=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp7E4=f->args;for(0;_tmp7E4 != 0;_tmp7E4=_tmp7E4->tl){
struct _tuple1*_tmp7E5=({struct _tuple1*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6->f1=Cyc_Absyn_Loc_n;_tmp7E6->f2=(*((struct _tuple9*)_tmp7E4->hd)).f1;_tmp7E6;});
(*((struct _tuple9*)_tmp7E4->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp7E4->hd)).f3);}}
# 4603
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4607
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7[0]=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);_tmp7E7;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp7E8=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp7E9=_tmp7E8;struct _dyneither_ptr*_tmp7F2;struct Cyc_Absyn_Tqual _tmp7F1;void*_tmp7F0;int _tmp7EF;_LL4C7: _tmp7F2=_tmp7E9.name;_tmp7F1=_tmp7E9.tq;_tmp7F0=_tmp7E9.type;_tmp7EF=_tmp7E9.inject;_LL4C8:;{
void*_tmp7EA=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp7F0,(void*)& Cyc_Absyn_HeapRgn_val,_tmp7F1,Cyc_Absyn_false_conref));
struct _tuple1*_tmp7EB=({struct _tuple1*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE->f1=Cyc_Absyn_Loc_n;_tmp7EE->f2=(struct _dyneither_ptr*)_check_null(_tmp7F2);_tmp7EE;});
f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC->hd=({struct _tuple9*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED->f1=_tmp7F2;_tmp7ED->f2=_tmp7F1;_tmp7ED->f3=_tmp7EA;_tmp7ED;});_tmp7EC->tl=0;_tmp7EC;}));
f->cyc_varargs=0;};}
# 4616
{struct Cyc_List_List*_tmp7F3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp7F3 != 0;_tmp7F3=_tmp7F3->tl){
((struct Cyc_Absyn_Vardecl*)_tmp7F3->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp7F3->hd)->type);}}
# 4619
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp7E3),f->body);}
# 4596
;_pop_region(frgn);};}
# 4623
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp7F4=s;switch(_tmp7F4){case Cyc_Absyn_Abstract: _LL4CA: _LL4CB:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL4CC: _LL4CD:
 return Cyc_Absyn_Extern;default: _LL4CE: _LL4CF:
 return s;}_LL4C9:;}struct _tuple35{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple36{struct Cyc_Toc_TocState*f1;struct _tuple35*f2;};
# 4640 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple36*env){
# 4643
struct _tuple36 _tmp7F5=*env;struct _tuple36 _tmp7F6=_tmp7F5;struct Cyc_Toc_TocState*_tmp835;struct Cyc_Absyn_Aggrdecl*_tmp834;int _tmp833;_LL4D1: _tmp835=_tmp7F6.f1;_tmp834=(_tmp7F6.f2)->f1;_tmp833=(_tmp7F6.f2)->f2;_LL4D2:;{
struct _tuple1*_tmp7F7=_tmp834->name;
struct Cyc_Toc_TocState _tmp7F8=*_tmp835;struct Cyc_Toc_TocState _tmp7F9=_tmp7F8;struct Cyc_Dict_Dict*_tmp832;_LL4D4: _tmp832=_tmp7F9.aggrs_so_far;_LL4D5:;{
int seen_defn_before;
struct _tuple18**_tmp7FA=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp832,_tmp7F7);
if(_tmp7FA == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp834->kind == Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp7FB=_region_malloc(d,sizeof(*_tmp7FB));_tmp7FB->f1=_tmp834;_tmp7FB->f2=Cyc_Absyn_strctq(_tmp7F7);_tmp7FB;});else{
# 4654
v=({struct _tuple18*_tmp7FC=_region_malloc(d,sizeof(*_tmp7FC));_tmp7FC->f1=_tmp834;_tmp7FC->f2=Cyc_Absyn_unionq_typ(_tmp7F7);_tmp7FC;});}
*_tmp832=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp832,_tmp7F7,v);};}else{
# 4657
struct _tuple18*_tmp7FD=*_tmp7FA;struct _tuple18*_tmp7FE=_tmp7FD;struct Cyc_Absyn_Aggrdecl*_tmp801;void*_tmp800;_LL4D7: _tmp801=_tmp7FE->f1;_tmp800=_tmp7FE->f2;_LL4D8:;
if(_tmp801->impl == 0){
*_tmp832=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp832,_tmp7F7,({struct _tuple18*_tmp7FF=_region_malloc(d,sizeof(*_tmp7FF));_tmp7FF->f1=_tmp834;_tmp7FF->f2=_tmp800;_tmp7FF;}));
seen_defn_before=0;}else{
# 4662
seen_defn_before=1;}}{
# 4664
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831->kind=_tmp834->kind;_tmp831->sc=Cyc_Absyn_Public;_tmp831->name=_tmp834->name;_tmp831->tvs=0;_tmp831->impl=0;_tmp831->expected_mem_kind=0;_tmp831->attributes=_tmp834->attributes;_tmp831;});
# 4671
if(_tmp834->impl != 0  && !seen_defn_before){
new_ad->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802->exist_vars=0;_tmp802->rgn_po=0;_tmp802->fields=0;_tmp802->tagged=0;_tmp802;});{
# 4676
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp803=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp834->impl))->fields;for(0;_tmp803 != 0;_tmp803=_tmp803->tl){
# 4680
struct Cyc_Absyn_Aggrfield*_tmp804=(struct Cyc_Absyn_Aggrfield*)_tmp803->hd;
void*_tmp805=_tmp804->type;
struct Cyc_List_List*_tmp806=_tmp804->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp805)) && (
_tmp834->kind == Cyc_Absyn_StructA  && _tmp803->tl == 0  || _tmp834->kind == Cyc_Absyn_UnionA)){
# 4694 "toc.cyc"
void*_tmp807=Cyc_Tcutil_compress(_tmp805);void*_tmp808=_tmp807;void*_tmp815;struct Cyc_Absyn_Tqual _tmp814;union Cyc_Absyn_Constraint*_tmp813;unsigned int _tmp812;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp808)->tag == 8){_LL4DA: _tmp815=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp808)->f1).elt_type;_tmp814=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp808)->f1).tq;_tmp813=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp808)->f1).zero_term;_tmp812=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp808)->f1).zt_loc;_LL4DB:
# 4697
 _tmp805=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp80A;_tmp80A.tag=8;_tmp80A.f1=({struct Cyc_Absyn_ArrayInfo _tmp80B;_tmp80B.elt_type=_tmp815;_tmp80B.tq=_tmp814;_tmp80B.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp80B.zero_term=_tmp813;_tmp80B.zt_loc=_tmp812;_tmp80B;});_tmp80A;});_tmp809;});
goto _LL4D9;}else{_LL4DC: _LL4DD:
# 4700
 _tmp806=({struct Cyc_List_List*_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp80D=_cycalloc(sizeof(*_tmp80D));_tmp80D[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp80E;_tmp80E.tag=6;_tmp80E.f1=0;_tmp80E;});_tmp80D;});_tmp80C->tl=_tmp806;_tmp80C;});
_tmp805=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp80F=_cycalloc(sizeof(*_tmp80F));_tmp80F[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp810;_tmp810.tag=8;_tmp810.f1=({struct Cyc_Absyn_ArrayInfo _tmp811;_tmp811.elt_type=Cyc_Absyn_void_star_typ();_tmp811.tq=
Cyc_Absyn_empty_tqual(0);_tmp811.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp811.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp811.zt_loc=0;_tmp811;});_tmp810;});_tmp80F;});}_LL4D9:;}{
# 4707
struct Cyc_Absyn_Aggrfield*_tmp816=({struct Cyc_Absyn_Aggrfield*_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C->name=_tmp804->name;_tmp82C->tq=Cyc_Toc_mt_tq;_tmp82C->type=
# 4709
Cyc_Toc_typ_to_c(_tmp805);_tmp82C->width=_tmp804->width;_tmp82C->attributes=_tmp806;_tmp82C->requires_clause=0;_tmp82C;});
# 4717
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp834->impl))->tagged){
void*_tmp817=_tmp816->type;
struct _dyneither_ptr*_tmp818=_tmp816->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp82A;_tmp82A.tag=0;_tmp82A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp818);({struct Cyc_String_pa_PrintArg_struct _tmp829;_tmp829.tag=0;_tmp829.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp834->name).f2);({void*_tmp827[2]={& _tmp829,& _tmp82A};Cyc_aprintf(({const char*_tmp828="_union_%s_%s";_tag_dyneither(_tmp828,sizeof(char),13);}),_tag_dyneither(_tmp827,sizeof(void*),2));});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826[0]=s;_tmp826;});
struct Cyc_Absyn_Aggrfield*_tmp819=({struct Cyc_Absyn_Aggrfield*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825->name=Cyc_Toc_val_sp;_tmp825->tq=Cyc_Toc_mt_tq;_tmp825->type=_tmp817;_tmp825->width=0;_tmp825->attributes=0;_tmp825->requires_clause=0;_tmp825;});
struct Cyc_Absyn_Aggrfield*_tmp81A=({struct Cyc_Absyn_Aggrfield*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824->name=Cyc_Toc_tag_sp;_tmp824->tq=Cyc_Toc_mt_tq;_tmp824->type=Cyc_Absyn_sint_typ;_tmp824->width=0;_tmp824->attributes=0;_tmp824->requires_clause=0;_tmp824;});
struct Cyc_List_List*_tmp81B=({struct Cyc_Absyn_Aggrfield*_tmp823[2];_tmp823[1]=_tmp819;_tmp823[0]=_tmp81A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp823,sizeof(struct Cyc_Absyn_Aggrfield*),2));});
struct Cyc_Absyn_Aggrdecl*_tmp81C=({struct Cyc_Absyn_Aggrdecl*_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820->kind=Cyc_Absyn_StructA;_tmp820->sc=Cyc_Absyn_Public;_tmp820->name=({struct _tuple1*_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822->f1=Cyc_Absyn_Loc_n;_tmp822->f2=str;_tmp822;});_tmp820->tvs=0;_tmp820->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp821=_cycalloc(sizeof(*_tmp821));_tmp821->exist_vars=0;_tmp821->rgn_po=0;_tmp821->fields=_tmp81B;_tmp821->tagged=0;_tmp821;});_tmp820->expected_mem_kind=0;_tmp820->attributes=0;_tmp820;});
# 4730
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp81D=_cycalloc(sizeof(*_tmp81D));_tmp81D->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp81F;_tmp81F.tag=5;_tmp81F.f1=_tmp81C;_tmp81F;});_tmp81E;}),0);_tmp81D->tl=Cyc_Toc_result_decls;_tmp81D;});
_tmp816->type=Cyc_Absyn_strct(str);}
# 4733
new_fields=({struct Cyc_List_List*_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B->hd=_tmp816;_tmp82B->tl=new_fields;_tmp82B;});};}}
# 4735
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4737
if(_tmp833)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D->hd=({struct Cyc_Absyn_Decl*_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp830;_tmp830.tag=5;_tmp830.f1=new_ad;_tmp830;});_tmp82F;});_tmp82E->loc=0;_tmp82E;});_tmp82D->tl=Cyc_Toc_result_decls;_tmp82D;});
return 0;};};};}
# 4742
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple35 p=({struct _tuple35 _tmp836;_tmp836.f1=ad;_tmp836.f2=add_to_decls;_tmp836;});
((int(*)(struct _tuple35*arg,int(*f)(struct _RegionHandle*,struct _tuple36*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple37{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4771 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4774
struct _tuple37 _tmp837=*env;struct _tuple37 _tmp838=_tmp837;struct Cyc_Set_Set**_tmp84A;struct Cyc_Absyn_Datatypedecl*_tmp849;_LL4DF: _tmp84A=(_tmp838.f1)->datatypes_so_far;_tmp849=_tmp838.f2;_LL4E0:;{
struct _tuple1*_tmp839=_tmp849->name;
if(_tmp849->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp84A,_tmp839))
return 0;
*_tmp84A=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp84A,_tmp839);
{struct Cyc_List_List*_tmp83A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp849->fields))->v;for(0;_tmp83A != 0;_tmp83A=_tmp83A->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp83A->hd;
# 4782
struct Cyc_List_List*_tmp83B=0;
int i=1;
{struct Cyc_List_List*_tmp83C=f->typs;for(0;_tmp83C != 0;(_tmp83C=_tmp83C->tl,i ++)){
struct _dyneither_ptr*_tmp83D=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp83E=({struct Cyc_Absyn_Aggrfield*_tmp840=_cycalloc(sizeof(*_tmp840));_tmp840->name=_tmp83D;_tmp840->tq=(*((struct _tuple11*)_tmp83C->hd)).f1;_tmp840->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp83C->hd)).f2);_tmp840->width=0;_tmp840->attributes=0;_tmp840->requires_clause=0;_tmp840;});
_tmp83B=({struct Cyc_List_List*_tmp83F=_cycalloc(sizeof(*_tmp83F));_tmp83F->hd=_tmp83E;_tmp83F->tl=_tmp83B;_tmp83F;});}}
# 4790
_tmp83B=({struct Cyc_List_List*_tmp841=_cycalloc(sizeof(*_tmp841));_tmp841->hd=({struct Cyc_Absyn_Aggrfield*_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842->name=Cyc_Toc_tag_sp;_tmp842->tq=Cyc_Toc_mt_tq;_tmp842->type=Cyc_Absyn_sint_typ;_tmp842->width=0;_tmp842->attributes=0;_tmp842->requires_clause=0;_tmp842;});_tmp841->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp83B);_tmp841;});{
struct Cyc_Absyn_Aggrdecl*_tmp843=({struct Cyc_Absyn_Aggrdecl*_tmp847=_cycalloc(sizeof(*_tmp847));_tmp847->kind=Cyc_Absyn_StructA;_tmp847->sc=Cyc_Absyn_Public;_tmp847->name=
# 4794
Cyc_Toc_collapse_qvars(f->name,_tmp849->name);_tmp847->tvs=0;_tmp847->expected_mem_kind=0;_tmp847->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp848=_cycalloc(sizeof(*_tmp848));_tmp848->exist_vars=0;_tmp848->rgn_po=0;_tmp848->fields=_tmp83B;_tmp848->tagged=0;_tmp848;});_tmp847->attributes=0;_tmp847;});
# 4798
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp844=_cycalloc(sizeof(*_tmp844));_tmp844->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp845=_cycalloc(sizeof(*_tmp845));_tmp845[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp846;_tmp846.tag=5;_tmp846.f1=_tmp843;_tmp846;});_tmp845;}),0);_tmp844->tl=Cyc_Toc_result_decls;_tmp844;});};}}
# 4800
return 0;};}
# 4803
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4822 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4825
struct _tuple37 _tmp84B=*env;struct _tuple37 _tmp84C=_tmp84B;struct Cyc_Toc_TocState*_tmp871;struct Cyc_Absyn_Datatypedecl*_tmp870;_LL4E2: _tmp871=_tmp84C.f1;_tmp870=_tmp84C.f2;_LL4E3:;
if(_tmp870->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp84D=*_tmp871;struct Cyc_Toc_TocState _tmp84E=_tmp84D;struct Cyc_Dict_Dict*_tmp86F;_LL4E5: _tmp86F=_tmp84E.xdatatypes_so_far;_LL4E6:;{
struct _tuple1*_tmp84F=_tmp870->name;
{struct Cyc_List_List*_tmp850=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp870->fields))->v;for(0;_tmp850 != 0;_tmp850=_tmp850->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp850->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp851=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp852=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp851,Cyc_Absyn_false_conref,0);
# 4836
int*_tmp853=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp86F,f->name);int*_tmp854=_tmp853;if(_tmp854 == 0){_LL4E8: _LL4E9: {
# 4838
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4842
struct Cyc_Absyn_Vardecl*_tmp855=Cyc_Absyn_new_vardecl(0,f->name,_tmp852,initopt);
_tmp855->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp856=_cycalloc(sizeof(*_tmp856));_tmp856->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp857=_cycalloc(sizeof(*_tmp857));_tmp857[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp858;_tmp858.tag=0;_tmp858.f1=_tmp855;_tmp858;});_tmp857;}),0);_tmp856->tl=Cyc_Toc_result_decls;_tmp856;});
*_tmp86F=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp86F,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp859=f->typs;for(0;_tmp859 != 0;(_tmp859=_tmp859->tl,i ++)){
struct _dyneither_ptr*_tmp85A=({struct _dyneither_ptr*_tmp85E=_cycalloc(sizeof(*_tmp85E));_tmp85E[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp861;_tmp861.tag=1;_tmp861.f1=(unsigned long)i;({void*_tmp85F[1]={& _tmp861};Cyc_aprintf(({const char*_tmp860="f%d";_tag_dyneither(_tmp860,sizeof(char),4);}),_tag_dyneither(_tmp85F,sizeof(void*),1));});});_tmp85E;});
struct Cyc_Absyn_Aggrfield*_tmp85B=({struct Cyc_Absyn_Aggrfield*_tmp85D=_cycalloc(sizeof(*_tmp85D));_tmp85D->name=_tmp85A;_tmp85D->tq=(*((struct _tuple11*)_tmp859->hd)).f1;_tmp85D->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp859->hd)).f2);_tmp85D->width=0;_tmp85D->attributes=0;_tmp85D->requires_clause=0;_tmp85D;});
fields=({struct Cyc_List_List*_tmp85C=_cycalloc(sizeof(*_tmp85C));_tmp85C->hd=_tmp85B;_tmp85C->tl=fields;_tmp85C;});}}
# 4855
fields=({struct Cyc_List_List*_tmp862=_cycalloc(sizeof(*_tmp862));_tmp862->hd=({struct Cyc_Absyn_Aggrfield*_tmp863=_cycalloc(sizeof(*_tmp863));_tmp863->name=Cyc_Toc_tag_sp;_tmp863->tq=Cyc_Toc_mt_tq;_tmp863->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp863->width=0;_tmp863->attributes=0;_tmp863->requires_clause=0;_tmp863;});_tmp862->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp862;});{
struct Cyc_Absyn_Aggrdecl*_tmp864=({struct Cyc_Absyn_Aggrdecl*_tmp868=_cycalloc(sizeof(*_tmp868));_tmp868->kind=Cyc_Absyn_StructA;_tmp868->sc=Cyc_Absyn_Public;_tmp868->name=
# 4860
Cyc_Toc_collapse_qvars(f->name,_tmp870->name);_tmp868->tvs=0;_tmp868->expected_mem_kind=0;_tmp868->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp869=_cycalloc(sizeof(*_tmp869));_tmp869->exist_vars=0;_tmp869->rgn_po=0;_tmp869->fields=fields;_tmp869->tagged=0;_tmp869;});_tmp868->attributes=0;_tmp868;});
# 4864
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp865=_cycalloc(sizeof(*_tmp865));_tmp865->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp866=_cycalloc(sizeof(*_tmp866));_tmp866[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp867;_tmp867.tag=5;_tmp867.f1=_tmp864;_tmp867;});_tmp866;}),0);_tmp865->tl=Cyc_Toc_result_decls;_tmp865;});
# 4866
goto _LL4E7;};};};}}else{if(*((int*)_tmp854)== 0){_LL4EA: _LL4EB:
# 4868
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp86A=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp86B=Cyc_Absyn_new_vardecl(0,f->name,_tmp852,_tmp86A);
_tmp86B->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp86C=_cycalloc(sizeof(*_tmp86C));_tmp86C->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp86D=_cycalloc(sizeof(*_tmp86D));_tmp86D[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp86E;_tmp86E.tag=0;_tmp86E.f1=_tmp86B;_tmp86E;});_tmp86D;}),0);_tmp86C->tl=Cyc_Toc_result_decls;_tmp86C;});
*_tmp86F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp86F,f->name,1);}
# 4875
goto _LL4E7;}else{_LL4EC: _LL4ED:
 goto _LL4E7;}}_LL4E7:;}}
# 4879
return 0;};};}
# 4882
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4886
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4892
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4896
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp872=init->r;void*_tmp873=_tmp872;struct Cyc_Absyn_Vardecl*_tmp879;struct Cyc_Absyn_Exp*_tmp878;struct Cyc_Absyn_Exp*_tmp877;int _tmp876;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp873)->tag == 26){_LL4EF: _tmp879=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp873)->f1;_tmp878=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp873)->f2;_tmp877=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp873)->f3;_tmp876=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp873)->f4;_LL4F0:
# 4903
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp879,_tmp878,_tmp877,_tmp876,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL4EE;}else{_LL4F1: _LL4F2:
# 4908
 if(vd->sc == Cyc_Absyn_Static){
# 4912
struct _RegionHandle _tmp874=_new_region("temp");struct _RegionHandle*temp=& _tmp874;_push_region(temp);
{struct Cyc_Toc_Env*_tmp875=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp875,init);}
# 4913
;_pop_region(temp);}else{
# 4917
Cyc_Toc_exp_to_c(nv,init);}
goto _LL4EE;}_LL4EE:;}else{
# 4922
void*_tmp87A=Cyc_Tcutil_compress(old_typ);void*_tmp87B=_tmp87A;void*_tmp882;struct Cyc_Absyn_Exp*_tmp881;union Cyc_Absyn_Constraint*_tmp880;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87B)->tag == 8){_LL4F4: _tmp882=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87B)->f1).elt_type;_tmp881=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87B)->f1).num_elts;_tmp880=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87B)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp880)){_LL4F5:
 if(_tmp881 == 0)
({void*_tmp87C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp87D="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp87D,sizeof(char),55);}),_tag_dyneither(_tmp87C,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*num_elts=_tmp881;
struct Cyc_Absyn_Exp*_tmp87E=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4930
struct Cyc_Absyn_Exp*_tmp87F=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp87E,_tmp87F,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL4F3;};}else{goto _LL4F6;}}else{_LL4F6: _LL4F7:
 goto _LL4F3;}_LL4F3:;}}
# 4941
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp883=d;struct Cyc_List_List*_tmp88A;struct Cyc_List_List*_tmp889;void**_tmp888;struct Cyc_Tcpat_Rhs*_tmp887;switch(*((int*)_tmp883)){case 0: _LL4F9: _LL4FA:
 return d;case 1: _LL4FB: _tmp887=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp883)->f1;_LL4FC:
 _tmp887->rhs=success;return d;default: _LL4FD: _tmp88A=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp883)->f1;_tmp889=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp883)->f2;_tmp888=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp883)->f3;_LL4FE:
# 4946
*_tmp888=Cyc_Toc_rewrite_decision(*_tmp888,success);
for(0;_tmp889 != 0;_tmp889=_tmp889->tl){
struct _tuple29*_tmp884=(struct _tuple29*)_tmp889->hd;struct _tuple29*_tmp885=_tmp884;void**_tmp886;_LL500: _tmp886=(void**)& _tmp885->f2;_LL501:;
*_tmp886=Cyc_Toc_rewrite_decision(*_tmp886,success);}
# 4951
return d;}_LL4F8:;}
# 4962 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4964
struct _RegionHandle _tmp88B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp88B;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp88C=Cyc_Toc_share_env(rgn,nv);
void*_tmp88D=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp88C,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4974
struct Cyc_Absyn_Stmt*_tmp88E=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4978
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp88E);{
# 4980
struct Cyc_Absyn_Switch_clause*_tmp88F=({struct Cyc_Absyn_Switch_clause*_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C->pattern=p;_tmp89C->pat_vars=0;_tmp89C->where_clause=0;_tmp89C->body=_tmp88E;_tmp89C->loc=0;_tmp89C;});
struct Cyc_List_List*_tmp890=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,({struct Cyc_Absyn_Switch_clause*_tmp89B[1];_tmp89B[0]=_tmp88F;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(rgn,_tag_dyneither(_tmp89B,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));
# 4983
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4987
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp88C,& mydecls,& mybodies,dopt,_tmp890,v);
# 4990
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple34*_tmp891=(struct _tuple34*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple34*_tmp892=_tmp891;struct Cyc_Toc_Env*_tmp894;struct Cyc_Absyn_Stmt*_tmp893;_LL503: _tmp894=_tmp892->f1;_tmp893=_tmp892->f3;_LL504:;
if(_tmp893 == _tmp88E){senv=_tmp894;goto FOUND_ENV;}}
# 4995
({void*_tmp895=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp896="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp896,sizeof(char),33);}),_tag_dyneither(_tmp895,sizeof(void*),0));});
FOUND_ENV:
# 4999
 Cyc_Toc_stmt_to_c(senv,s);{
# 5001
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0);
# 5003
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp897=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
_tmp897->name=Cyc_Toc_temp_var();
_tmp897->type=Cyc_Toc_typ_to_c_array(_tmp897->type);
res=Cyc_Absyn_decl_stmt(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp898=_cycalloc(sizeof(*_tmp898));_tmp898[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp899;_tmp899.tag=0;_tmp899.f1=_tmp897;_tmp899;});_tmp898;}),0),res,0);}
# 5010
res=Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c(_tmp88D),e,res,0);{
struct Cyc_Absyn_Stmt*_tmp89A=res;_npop_handler(0);return _tmp89A;};};};};}
# 4965
;_pop_region(rgn);}
# 5018
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp89D=e->r;void*_tmp89E=_tmp89D;struct Cyc_Absyn_MallocInfo*_tmp8C8;struct Cyc_Absyn_Stmt*_tmp8C7;void**_tmp8C6;void**_tmp8C5;struct Cyc_List_List*_tmp8C4;struct Cyc_List_List*_tmp8C3;void**_tmp8C2;struct Cyc_List_List*_tmp8C1;void**_tmp8C0;struct Cyc_Absyn_Exp*_tmp8BF;struct Cyc_Absyn_Exp*_tmp8BE;struct Cyc_List_List*_tmp8BD;struct Cyc_Absyn_Exp*_tmp8BC;struct Cyc_Absyn_Exp*_tmp8BB;struct Cyc_Absyn_Exp*_tmp8BA;struct Cyc_Absyn_Exp*_tmp8B9;struct Cyc_Absyn_Exp*_tmp8B8;struct Cyc_Absyn_Exp*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_Absyn_Exp*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B4;struct Cyc_Absyn_Exp*_tmp8B3;struct Cyc_Absyn_Exp*_tmp8B2;struct Cyc_Absyn_Exp*_tmp8B1;struct Cyc_Absyn_Exp*_tmp8B0;struct Cyc_Absyn_Exp*_tmp8AF;struct Cyc_Absyn_Exp*_tmp8AE;struct Cyc_List_List*_tmp8AD;struct Cyc_Absyn_Exp*_tmp8AC;struct Cyc_Absyn_Exp*_tmp8AB;struct Cyc_Absyn_Exp*_tmp8AA;struct Cyc_Absyn_Exp*_tmp8A9;struct Cyc_Absyn_Exp*_tmp8A8;struct Cyc_Absyn_Exp*_tmp8A7;struct Cyc_Absyn_Exp*_tmp8A6;struct Cyc_Absyn_Exp*_tmp8A5;switch(*((int*)_tmp89E)){case 19: _LL506: _tmp8A5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL507:
 _tmp8A6=_tmp8A5;goto _LL509;case 20: _LL508: _tmp8A6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL509:
 _tmp8A7=_tmp8A6;goto _LL50B;case 21: _LL50A: _tmp8A7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL50B:
 _tmp8A8=_tmp8A7;goto _LL50D;case 14: _LL50C: _tmp8A8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL50D:
 _tmp8A9=_tmp8A8;goto _LL50F;case 10: _LL50E: _tmp8A9=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL50F:
 _tmp8AA=_tmp8A9;goto _LL511;case 11: _LL510: _tmp8AA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL511:
 _tmp8AB=_tmp8AA;goto _LL513;case 17: _LL512: _tmp8AB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL513:
 _tmp8AC=_tmp8AB;goto _LL515;case 4: _LL514: _tmp8AC=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL515:
 Cyc_Toc_exptypes_to_c(_tmp8AC);goto _LL505;case 2: _LL516: _tmp8AD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_LL517:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8AD);goto _LL505;case 6: _LL518: _tmp8AF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_tmp8AE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_LL519:
 _tmp8B1=_tmp8AF;_tmp8B0=_tmp8AE;goto _LL51B;case 7: _LL51A: _tmp8B1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_tmp8B0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_LL51B:
 _tmp8B3=_tmp8B1;_tmp8B2=_tmp8B0;goto _LL51D;case 8: _LL51C: _tmp8B3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_tmp8B2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_LL51D:
 _tmp8B5=_tmp8B3;_tmp8B4=_tmp8B2;goto _LL51F;case 22: _LL51E: _tmp8B5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_tmp8B4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_LL51F:
 _tmp8B7=_tmp8B5;_tmp8B6=_tmp8B4;goto _LL521;case 34: _LL520: _tmp8B7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_tmp8B6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_LL521:
 _tmp8B9=_tmp8B7;_tmp8B8=_tmp8B6;goto _LL523;case 3: _LL522: _tmp8B9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_tmp8B8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp89E)->f3;_LL523:
 Cyc_Toc_exptypes_to_c(_tmp8B9);Cyc_Toc_exptypes_to_c(_tmp8B8);goto _LL505;case 5: _LL524: _tmp8BC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_tmp8BB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_tmp8BA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89E)->f3;_LL525:
# 5036
 Cyc_Toc_exptypes_to_c(_tmp8BC);Cyc_Toc_exptypes_to_c(_tmp8BB);Cyc_Toc_exptypes_to_c(_tmp8BA);goto _LL505;case 9: _LL526: _tmp8BE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_tmp8BD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_LL527:
# 5038
 Cyc_Toc_exptypes_to_c(_tmp8BE);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8BD);goto _LL505;case 13: _LL528: _tmp8C0=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_tmp8BF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_LL529:
*_tmp8C0=Cyc_Toc_typ_to_c(*_tmp8C0);Cyc_Toc_exptypes_to_c(_tmp8BF);goto _LL505;case 24: _LL52A: _tmp8C2=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp89E)->f1)->f3;_tmp8C1=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_LL52B:
# 5041
*_tmp8C2=Cyc_Toc_typ_to_c(*_tmp8C2);
_tmp8C3=_tmp8C1;goto _LL52D;case 35: _LL52C: _tmp8C3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp89E)->f2;_LL52D:
 _tmp8C4=_tmp8C3;goto _LL52F;case 25: _LL52E: _tmp8C4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL52F:
# 5045
 for(0;_tmp8C4 != 0;_tmp8C4=_tmp8C4->tl){
struct _tuple19 _tmp89F=*((struct _tuple19*)_tmp8C4->hd);struct _tuple19 _tmp8A0=_tmp89F;struct Cyc_Absyn_Exp*_tmp8A1;_LL557: _tmp8A1=_tmp8A0.f2;_LL558:;
Cyc_Toc_exptypes_to_c(_tmp8A1);}
# 5049
goto _LL505;case 18: _LL530: _tmp8C5=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL531:
 _tmp8C6=_tmp8C5;goto _LL533;case 16: _LL532: _tmp8C6=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL533:
*_tmp8C6=Cyc_Toc_typ_to_c(*_tmp8C6);goto _LL505;case 36: _LL534: _tmp8C7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL535:
 Cyc_Toc_stmttypes_to_c(_tmp8C7);goto _LL505;case 33: _LL536: _tmp8C8=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp89E)->f1;_LL537:
# 5054
 if(_tmp8C8->elt_type != 0)
_tmp8C8->elt_type=({void**_tmp8A2=_cycalloc(sizeof(*_tmp8A2));_tmp8A2[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8C8->elt_type)));_tmp8A2;});
Cyc_Toc_exptypes_to_c(_tmp8C8->num_elts);
goto _LL505;case 0: _LL538: _LL539:
 goto _LL53B;case 1: _LL53A: _LL53B:
 goto _LL53D;case 31: _LL53C: _LL53D:
 goto _LL53F;case 39: _LL53E: _LL53F:
 goto _LL541;case 32: _LL540: _LL541:
 goto _LL505;case 29: _LL542: _LL543:
# 5064
 goto _LL545;case 30: _LL544: _LL545:
 goto _LL547;case 28: _LL546: _LL547:
 goto _LL549;case 26: _LL548: _LL549:
 goto _LL54B;case 27: _LL54A: _LL54B:
 goto _LL54D;case 23: _LL54C: _LL54D:
 goto _LL54F;case 12: _LL54E: _LL54F:
 goto _LL551;case 15: _LL550: _LL551:
 goto _LL553;case 38: _LL552: _LL553:
 goto _LL555;default: _LL554: _LL555:
# 5074
({void*_tmp8A3=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp8A4="Cyclone expression within C code";_tag_dyneither(_tmp8A4,sizeof(char),33);}),_tag_dyneither(_tmp8A3,sizeof(void*),0));});
goto _LL505;}_LL505:;}
# 5079
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp8C9=d->r;void*_tmp8CA=_tmp8C9;struct Cyc_Absyn_Typedefdecl*_tmp8D4;struct Cyc_Absyn_Enumdecl*_tmp8D3;struct Cyc_Absyn_Aggrdecl*_tmp8D2;struct Cyc_Absyn_Fndecl*_tmp8D1;struct Cyc_Absyn_Vardecl*_tmp8D0;switch(*((int*)_tmp8CA)){case 0: _LL55A: _tmp8D0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8CA)->f1;_LL55B:
# 5082
 _tmp8D0->type=Cyc_Toc_typ_to_c(_tmp8D0->type);
if(_tmp8D0->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8D0->initializer));
goto _LL559;case 1: _LL55C: _tmp8D1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8CA)->f1;_LL55D:
# 5086
 _tmp8D1->ret_type=Cyc_Toc_typ_to_c(_tmp8D1->ret_type);
{struct Cyc_List_List*_tmp8CB=_tmp8D1->args;for(0;_tmp8CB != 0;_tmp8CB=_tmp8CB->tl){
(*((struct _tuple9*)_tmp8CB->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8CB->hd)).f3);}}
# 5090
goto _LL559;case 5: _LL55E: _tmp8D2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8CA)->f1;_LL55F:
 Cyc_Toc_aggrdecl_to_c(_tmp8D2,1);goto _LL559;case 7: _LL560: _tmp8D3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8CA)->f1;_LL561:
# 5093
 if(_tmp8D3->fields != 0){
struct Cyc_List_List*_tmp8CC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8D3->fields))->v;for(0;_tmp8CC != 0;_tmp8CC=_tmp8CC->tl){
struct Cyc_Absyn_Enumfield*_tmp8CD=(struct Cyc_Absyn_Enumfield*)_tmp8CC->hd;
if(_tmp8CD->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8CD->tag));}}
# 5098
goto _LL559;case 8: _LL562: _tmp8D4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8CA)->f1;_LL563:
 _tmp8D4->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8D4->defn));goto _LL559;case 2: _LL564: _LL565:
 goto _LL567;case 3: _LL566: _LL567:
 goto _LL569;case 6: _LL568: _LL569:
 goto _LL56B;case 9: _LL56A: _LL56B:
 goto _LL56D;case 10: _LL56C: _LL56D:
 goto _LL56F;case 11: _LL56E: _LL56F:
 goto _LL571;case 12: _LL570: _LL571:
 goto _LL573;case 4: _LL572: _LL573:
# 5108
({void*_tmp8CE=0;Cyc_Tcutil_terr(d->loc,({const char*_tmp8CF="Cyclone declaration within C code";_tag_dyneither(_tmp8CF,sizeof(char),34);}),_tag_dyneither(_tmp8CE,sizeof(void*),0));});
goto _LL559;case 13: _LL574: _LL575:
 goto _LL577;default: _LL576: _LL577:
# 5112
 goto _LL559;}_LL559:;}
# 5116
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp8D5=s->r;void*_tmp8D6=_tmp8D5;struct Cyc_Absyn_Stmt*_tmp8F1;struct Cyc_Absyn_Stmt*_tmp8F0;struct Cyc_Absyn_Exp*_tmp8EF;struct Cyc_Absyn_Decl*_tmp8EE;struct Cyc_Absyn_Stmt*_tmp8ED;struct Cyc_Absyn_Exp*_tmp8EC;struct Cyc_List_List*_tmp8EB;void*_tmp8EA;struct Cyc_Absyn_Exp*_tmp8E9;struct Cyc_Absyn_Exp*_tmp8E8;struct Cyc_Absyn_Exp*_tmp8E7;struct Cyc_Absyn_Stmt*_tmp8E6;struct Cyc_Absyn_Exp*_tmp8E5;struct Cyc_Absyn_Stmt*_tmp8E4;struct Cyc_Absyn_Exp*_tmp8E3;struct Cyc_Absyn_Stmt*_tmp8E2;struct Cyc_Absyn_Stmt*_tmp8E1;struct Cyc_Absyn_Exp*_tmp8E0;struct Cyc_Absyn_Stmt*_tmp8DF;struct Cyc_Absyn_Stmt*_tmp8DE;struct Cyc_Absyn_Exp*_tmp8DD;switch(*((int*)_tmp8D6)){case 1: _LL579: _tmp8DD=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f1;_LL57A:
 Cyc_Toc_exptypes_to_c(_tmp8DD);goto _LL578;case 2: _LL57B: _tmp8DF=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f1;_tmp8DE=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f2;_LL57C:
 Cyc_Toc_stmttypes_to_c(_tmp8DF);Cyc_Toc_stmttypes_to_c(_tmp8DE);goto _LL578;case 3: _LL57D: _tmp8E0=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f1;_LL57E:
 if(_tmp8E0 != 0)Cyc_Toc_exptypes_to_c(_tmp8E0);goto _LL578;case 4: _LL57F: _tmp8E3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f1;_tmp8E2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f2;_tmp8E1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f3;_LL580:
# 5122
 Cyc_Toc_exptypes_to_c(_tmp8E3);Cyc_Toc_stmttypes_to_c(_tmp8E2);Cyc_Toc_stmttypes_to_c(_tmp8E1);goto _LL578;case 5: _LL581: _tmp8E5=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f1).f1;_tmp8E4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f2;_LL582:
# 5124
 Cyc_Toc_exptypes_to_c(_tmp8E5);Cyc_Toc_stmttypes_to_c(_tmp8E4);goto _LL578;case 9: _LL583: _tmp8E9=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f1;_tmp8E8=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f2).f1;_tmp8E7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f3).f1;_tmp8E6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f4;_LL584:
# 5126
 Cyc_Toc_exptypes_to_c(_tmp8E9);Cyc_Toc_exptypes_to_c(_tmp8E8);Cyc_Toc_exptypes_to_c(_tmp8E7);
Cyc_Toc_stmttypes_to_c(_tmp8E6);goto _LL578;case 10: _LL585: _tmp8EC=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f1;_tmp8EB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f2;_tmp8EA=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f3;_LL586:
# 5129
 Cyc_Toc_exptypes_to_c(_tmp8EC);
for(0;_tmp8EB != 0;_tmp8EB=_tmp8EB->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp8EB->hd)->body);}
goto _LL578;case 12: _LL587: _tmp8EE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f1;_tmp8ED=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f2;_LL588:
 Cyc_Toc_decltypes_to_c(_tmp8EE);Cyc_Toc_stmttypes_to_c(_tmp8ED);goto _LL578;case 14: _LL589: _tmp8F0=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f1;_tmp8EF=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f2).f1;_LL58A:
 Cyc_Toc_stmttypes_to_c(_tmp8F0);Cyc_Toc_exptypes_to_c(_tmp8EF);goto _LL578;case 13: _LL58B: _tmp8F1=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8D6)->f2;_LL58C:
 Cyc_Toc_stmttypes_to_c(_tmp8F1);goto _LL578;case 0: _LL58D: _LL58E:
 goto _LL590;case 6: _LL58F: _LL590:
 goto _LL592;case 7: _LL591: _LL592:
 goto _LL594;case 8: _LL593: _LL594:
# 5139
 goto _LL578;case 11: _LL595: _LL596:
# 5142
 s->r=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp8D7=_cycalloc_atomic(sizeof(*_tmp8D7));_tmp8D7[0]=({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmp8D8;_tmp8D8.tag=0;_tmp8D8;});_tmp8D7;});
goto _LL578;case 15: _LL597: _LL598:
 goto _LL59A;default: _LL599: _LL59A:
# 5146
({void*_tmp8D9=0;Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8DC;_tmp8DC.tag=0;_tmp8DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp8DA[1]={& _tmp8DC};Cyc_aprintf(({const char*_tmp8DB="Cyclone statement in C code: %s";_tag_dyneither(_tmp8DB,sizeof(char),32);}),_tag_dyneither(_tmp8DA,sizeof(void*),1));});}),_tag_dyneither(_tmp8D9,sizeof(void*),0));});
goto _LL578;}_LL578:;}
# 5154
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp8F2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp8F3="decls_to_c: not at toplevel!";_tag_dyneither(_tmp8F3,sizeof(char),29);}),_tag_dyneither(_tmp8F2,sizeof(void*),0));});{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp8F4=d->r;void*_tmp8F5=_tmp8F4;struct Cyc_List_List*_tmp90D;struct Cyc_List_List*_tmp90C;struct Cyc_List_List*_tmp90B;struct Cyc_List_List*_tmp90A;struct Cyc_Absyn_Typedefdecl*_tmp909;struct Cyc_Absyn_Enumdecl*_tmp908;struct Cyc_Absyn_Datatypedecl*_tmp907;struct Cyc_Absyn_Aggrdecl*_tmp906;struct Cyc_Absyn_Fndecl*_tmp905;struct Cyc_Absyn_Vardecl*_tmp904;switch(*((int*)_tmp8F5)){case 0: _LL59C: _tmp904=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8F5)->f1;_LL59D: {
# 5161
struct _tuple1*_tmp8F6=_tmp904->name;
# 5163
if(_tmp904->sc == Cyc_Absyn_ExternC)
_tmp8F6=({struct _tuple1*_tmp8F7=_cycalloc(sizeof(*_tmp8F7));_tmp8F7->f1=Cyc_Absyn_Abs_n(0,1);_tmp8F7->f2=(*_tmp8F6).f2;_tmp8F7;});
if(_tmp904->initializer != 0){
if(_tmp904->sc == Cyc_Absyn_ExternC)_tmp904->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp904->initializer));else{
# 5170
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp904->initializer));}}
# 5172
_tmp904->name=_tmp8F6;
_tmp904->sc=Cyc_Toc_scope_to_c(_tmp904->sc);
_tmp904->type=Cyc_Toc_typ_to_c(_tmp904->type);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8F8=_cycalloc(sizeof(*_tmp8F8));_tmp8F8->hd=d;_tmp8F8->tl=Cyc_Toc_result_decls;_tmp8F8;});
goto _LL59B;}case 1: _LL59E: _tmp905=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8F5)->f1;_LL59F: {
# 5178
struct _tuple1*_tmp8F9=_tmp905->name;
# 5180
if(_tmp905->sc == Cyc_Absyn_ExternC){
_tmp8F9=({struct _tuple1*_tmp8FA=_cycalloc(sizeof(*_tmp8FA));_tmp8FA->f1=Cyc_Absyn_Abs_n(0,1);_tmp8FA->f2=(*_tmp8F9).f2;_tmp8FA;});
_tmp905->sc=Cyc_Absyn_Public;}
# 5184
_tmp905->name=_tmp8F9;
Cyc_Toc_fndecl_to_c(nv,_tmp905,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8FB=_cycalloc(sizeof(*_tmp8FB));_tmp8FB->hd=d;_tmp8FB->tl=Cyc_Toc_result_decls;_tmp8FB;});
goto _LL59B;}case 2: _LL5A0: _LL5A1:
 goto _LL5A3;case 3: _LL5A2: _LL5A3:
({void*_tmp8FC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp8FD="letdecl at toplevel";_tag_dyneither(_tmp8FD,sizeof(char),20);}),_tag_dyneither(_tmp8FC,sizeof(void*),0));});case 4: _LL5A4: _LL5A5:
({void*_tmp8FE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp8FF="region decl at toplevel";_tag_dyneither(_tmp8FF,sizeof(char),24);}),_tag_dyneither(_tmp8FE,sizeof(void*),0));});case 5: _LL5A6: _tmp906=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8F5)->f1;_LL5A7:
# 5192
 Cyc_Toc_aggrdecl_to_c(_tmp906,1);
goto _LL59B;case 6: _LL5A8: _tmp907=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp8F5)->f1;_LL5A9:
# 5195
 if(_tmp907->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp907);else{
# 5198
Cyc_Toc_datatypedecl_to_c(_tmp907);}
goto _LL59B;case 7: _LL5AA: _tmp908=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8F5)->f1;_LL5AB:
# 5201
 Cyc_Toc_enumdecl_to_c(_tmp908);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp900=_cycalloc(sizeof(*_tmp900));_tmp900->hd=d;_tmp900->tl=Cyc_Toc_result_decls;_tmp900;});
goto _LL59B;case 8: _LL5AC: _tmp909=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8F5)->f1;_LL5AD:
# 5205
 _tmp909->tvs=0;
if(_tmp909->defn != 0)
_tmp909->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp909->defn));else{
# 5209
enum Cyc_Absyn_KindQual _tmp901=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp909->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp902=_tmp901;if(_tmp902 == Cyc_Absyn_BoxKind){_LL5BB: _LL5BC:
 _tmp909->defn=Cyc_Absyn_void_star_typ();goto _LL5BA;}else{_LL5BD: _LL5BE:
 _tmp909->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL5BA;}_LL5BA:;}
# 5214
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp903=_cycalloc(sizeof(*_tmp903));_tmp903->hd=d;_tmp903->tl=Cyc_Toc_result_decls;_tmp903;});
goto _LL59B;case 13: _LL5AE: _LL5AF:
 goto _LL5B1;case 14: _LL5B0: _LL5B1:
# 5218
 goto _LL59B;case 9: _LL5B2: _tmp90A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp8F5)->f2;_LL5B3:
 _tmp90B=_tmp90A;goto _LL5B5;case 10: _LL5B4: _tmp90B=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp8F5)->f2;_LL5B5:
 _tmp90C=_tmp90B;goto _LL5B7;case 11: _LL5B6: _tmp90C=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp8F5)->f1;_LL5B7:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp90C,top,cinclude);goto _LL59B;default: _LL5B8: _tmp90D=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp8F5)->f1;_LL5B9:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp90D,top,1);goto _LL59B;}_LL59B:;};}
# 5225
return nv;}
# 5229
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp90E=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp90F=_tmp90E;struct Cyc_Core_DynamicRegion*_tmp913;_LL5C0: _tmp913=_tmp90F.key;_LL5C1:;{
struct Cyc_Toc_TocState*_tmp910=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp913,0,Cyc_Toc_empty_toc_state);
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp911=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp911));_tmp911->dyn=(struct Cyc_Core_DynamicRegion*)_tmp913;_tmp911->state=(struct Cyc_Toc_TocState*)_tmp910;_tmp911;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;
Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmp912=_cycalloc(sizeof(struct _dyneither_ptr*)* 42);_tmp912[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str;_tmp912[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str;_tmp912[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str;_tmp912[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str;_tmp912[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str;_tmp912[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str;_tmp912[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str;_tmp912[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str;_tmp912[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str;_tmp912[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str;_tmp912[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str;_tmp912[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str;_tmp912[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str;_tmp912[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str;_tmp912[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str;_tmp912[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str;_tmp912[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str;_tmp912[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str;_tmp912[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp912[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str;_tmp912[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp912[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp912[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str;_tmp912[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str;_tmp912[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str;_tmp912[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str;_tmp912[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str;_tmp912[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str;_tmp912[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str;_tmp912[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str;_tmp912[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str;_tmp912[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str;_tmp912[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str;_tmp912[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str;_tmp912[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str;_tmp912[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str;_tmp912[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str;_tmp912[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp912[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str;_tmp912[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str;_tmp912[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str;_tmp912[41]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str;_tmp912;}),sizeof(struct _dyneither_ptr*),42);};}
# 5286
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp914=*ts;struct Cyc_Toc_TocState _tmp915=_tmp914;struct Cyc_Xarray_Xarray*_tmp916;_LL5C3: _tmp916=_tmp915.temp_labels;_LL5C4:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp916);
return 0;}
# 5294
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 5296
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp917=_cycalloc(sizeof(*_tmp917));_tmp917[0]=pop_tables;_tmp917;});
Cyc_Toc_init();
{struct _RegionHandle _tmp918=_new_region("start");struct _RegionHandle*start=& _tmp918;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);;_pop_region(start);}{
# 5306
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp919=ts;struct Cyc_Toc_TocStateWrap*_tmp91A=Cyc_Toc_toc_state;ts=_tmp91A;Cyc_Toc_toc_state=_tmp919;});{
struct Cyc_Toc_TocStateWrap _tmp91B=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp91C=_tmp91B;struct Cyc_Core_DynamicRegion*_tmp91E;struct Cyc_Toc_TocState*_tmp91D;_LL5C6: _tmp91E=_tmp91C.dyn;_tmp91D=_tmp91C.state;_LL5C7:;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp91E,_tmp91D,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp91E);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 5313
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);};};}
