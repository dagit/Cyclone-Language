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
# 152
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 239 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 245
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 247
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 249
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 259
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 317
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);
# 346
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 350
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 353
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
({struct _dyneither_ptr _tmp924=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp923=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp923,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp924,ap);});}
# 75
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 77
({struct _dyneither_ptr _tmp926=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp925=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp925,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp926,ap);});}
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
return({struct Cyc_Toc_TocState*_tmpA=_region_malloc(d,sizeof(*_tmpA));({struct Cyc_List_List**_tmp927=({struct Cyc_List_List**_tmp10=_region_malloc(d,sizeof(*_tmp10));_tmp10[0]=0;_tmp10;});_tmpA->tuple_types=_tmp927;});({struct Cyc_Dict_Dict*_tmp929=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp928=
# 136
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpF[0]=_tmp928;});_tmpF;});_tmpA->aggrs_so_far=_tmp929;});({struct Cyc_List_List**_tmp92A=({struct Cyc_List_List**_tmpE=_region_malloc(d,sizeof(*_tmpE));_tmpE[0]=0;_tmpE;});_tmpA->abs_struct_types=_tmp92A;});({struct Cyc_Set_Set**_tmp92C=({struct Cyc_Set_Set**_tmpD=_region_malloc(d,sizeof(*_tmpD));({struct Cyc_Set_Set*_tmp92B=
# 138
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpD[0]=_tmp92B;});_tmpD;});_tmpA->datatypes_so_far=_tmp92C;});({struct Cyc_Dict_Dict*_tmp92E=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp92D=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpC[0]=_tmp92D;});_tmpC;});_tmpA->xdatatypes_so_far=_tmp92E;});({struct Cyc_Dict_Dict*_tmp930=({struct Cyc_Dict_Dict*_tmpB=_region_malloc(d,sizeof(*_tmpB));({struct Cyc_Dict_Dict _tmp92F=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmpB[0]=_tmp92F;});_tmpB;});_tmpA->qvar_tags=_tmp930;});({struct Cyc_Xarray_Xarray*_tmp931=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmpA->temp_labels=_tmp931;});_tmpA;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 151
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 157
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 160
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp11=ts;struct Cyc_Toc_TocStateWrap*_tmp12=Cyc_Toc_toc_state;ts=_tmp12;Cyc_Toc_toc_state=_tmp11;});{
struct Cyc_Toc_TocStateWrap _tmp13=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp14=_tmp13;struct Cyc_Core_DynamicRegion*_tmp1D;struct Cyc_Toc_TocState*_tmp1C;_LL1: _tmp1D=_tmp14.dyn;_tmp1C=_tmp14.state;_LL2:;{
struct _dyneither_ptr _tmp15=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1D,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp932=({struct Cyc_Toc_TocStateWrap _tmp16;_tmp16.dyn=(struct Cyc_Core_DynamicRegion*)_tmp1D;_tmp16.state=(struct Cyc_Toc_TocState*)_tmp1C;_tmp16;});*ts=_tmp932;});
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
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp105=_cycalloc(sizeof(*_tmp105));({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp933=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp106;_tmp106.tag=14U;_tmp106.f1=t;_tmp106.f2=e;_tmp106.f3=0;_tmp106.f4=Cyc_Absyn_No_coercion;_tmp106;});_tmp105[0]=_tmp933;});_tmp105;});}
# 315
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp934=({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp108;_tmp108.tag=20U;_tmp108.f1=e;_tmp108;});_tmp107[0]=_tmp934;});_tmp107;});}
# 318
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp109=_cycalloc(sizeof(*_tmp109));({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp935=({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp10A;_tmp10A.tag=23U;_tmp10A.f1=e1;_tmp10A.f2=e2;_tmp10A;});_tmp109[0]=_tmp935;});_tmp109;});}
# 321
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp936=({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp10C;_tmp10C.tag=37U;_tmp10C.f1=s;_tmp10C;});_tmp10B[0]=_tmp936;});_tmp10B;});}
# 324
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp937=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp10E;_tmp10E.tag=17U;_tmp10E.f1=t;_tmp10E;});_tmp10D[0]=_tmp937;});_tmp10D;});}
# 327
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp938=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp110;_tmp110.tag=10U;_tmp110.f1=e;_tmp110.f2=es;_tmp110.f3=0;_tmp110.f4=1;_tmp110;});_tmp10F[0]=_tmp938;});_tmp10F;});}
# 330
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp111=_cycalloc(sizeof(*_tmp111));({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp939=({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp112;_tmp112.tag=1U;_tmp112.f1=e;_tmp112;});_tmp111[0]=_tmp939;});_tmp111;});}
# 333
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp93A=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp114;_tmp114.tag=2U;_tmp114.f1=s1;_tmp114.f2=s2;_tmp114;});_tmp113[0]=_tmp93A;});_tmp113;});}
# 336
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp115=_cycalloc(sizeof(*_tmp115));({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp93B=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp116;_tmp116.tag=6U;_tmp116.f1=e1;_tmp116.f2=e2;_tmp116.f3=e3;_tmp116;});_tmp115[0]=_tmp93B;});_tmp115;});}
# 339
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp117=_cycalloc(sizeof(*_tmp117));({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp93C=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp118;_tmp118.tag=21U;_tmp118.f1=e;_tmp118.f2=n;_tmp118.f3=0;_tmp118.f4=0;_tmp118;});_tmp117[0]=_tmp93C;});_tmp117;});}
# 342
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp119=_cycalloc(sizeof(*_tmp119));({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp93D=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp11A;_tmp11A.tag=22U;_tmp11A.f1=e;_tmp11A.f2=n;_tmp11A.f3=0;_tmp11A.f4=0;_tmp11A;});_tmp119[0]=_tmp93D;});_tmp119;});}
# 345
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 348
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp93E=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp11C;_tmp11C.tag=36U;_tmp11C.f1=tdopt;_tmp11C.f2=ds;_tmp11C;});_tmp11B[0]=_tmp93E;});_tmp11B;});}
# 350
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp93F=({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp11E;_tmp11E.tag=8U;_tmp11E.f1=v;_tmp11E;});_tmp11D[0]=_tmp93F;});_tmp11D;});}
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
({void*_tmp126=0U;({struct _dyneither_ptr _tmp940=({const char*_tmp127="impossible IntType (not char, short or int)";_tag_dyneither(_tmp127,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp940,_tag_dyneither(_tmp126,sizeof(void*),0U));});});}_LLD:;}
# 424
goto _LL0;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp124)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}case 5U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp12A;_tmp12A.tag=0U;({struct _dyneither_ptr _tmp941=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp12A.f1=_tmp941;});({void*_tmp128[1U]={& _tmp12A};({struct _dyneither_ptr _tmp942=({const char*_tmp129="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp129,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp942,_tag_dyneither(_tmp128,sizeof(void*),1U));});});});}_LL0:;}
# 431
return function;}
# 433
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp12C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp12D=_tmp12C;void*_tmp130;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12D)->tag == 5U){_LL1: _tmp130=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12D)->f1).elt_typ;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp130);}else{_LL3: _LL4:
({void*_tmp12E=0U;({struct _dyneither_ptr _tmp943=({const char*_tmp12F="impossible type (not pointer)";_tag_dyneither(_tmp12F,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp943,_tag_dyneither(_tmp12E,sizeof(void*),0U));});});}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp144;_tmp144.tag=0U;_tmp144.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp150);({struct Cyc_String_pa_PrintArg_struct _tmp143;_tmp143.tag=0U;_tmp143.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);({void*_tmp141[2U]={& _tmp143,& _tmp144};({struct _dyneither_ptr _tmp944=({const char*_tmp142="%s_%s_struct";_tag_dyneither(_tmp142,sizeof(char),13U);});Cyc_aprintf(_tmp944,_tag_dyneither(_tmp141,sizeof(void*),2U));});});});});{
struct _RegionHandle _tmp145=_new_region("r");struct _RegionHandle*r=& _tmp145;_push_region(r);
{struct _dyneither_ptr _tmp14C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0U;_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp150);({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0U;({struct _dyneither_ptr _tmp947=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp946=r;struct Cyc_List_List*_tmp945=_tmp14F;Cyc_rstr_sepstr(_tmp946,_tmp945,({const char*_tmp14B="_";_tag_dyneither(_tmp14B,sizeof(char),2U);}));}));_tmp149.f1=_tmp947;});({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0U;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);({void*_tmp146[3U]={& _tmp148,& _tmp149,& _tmp14A};({struct _dyneither_ptr _tmp948=({const char*_tmp147="%s_%s_%s_struct";_tag_dyneither(_tmp147,sizeof(char),16U);});Cyc_aprintf(_tmp948,_tag_dyneither(_tmp146,sizeof(void*),3U));});});});});});_npop_handler(0U);return _tmp14C;};_pop_region(r);};}_LL3:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
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
struct _tuple1*res=({struct _tuple1*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->f1=_tmp160;({struct _dyneither_ptr*_tmp949=({struct _dyneither_ptr*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E[0]=newvar;_tmp15E;});_tmp15D->f2=_tmp949;});_tmp15D;});
({struct Cyc_Dict_Dict _tmp94A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp166,_tmp15B,res);*_tmp166=_tmp94A;});
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
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp17C=_cycalloc(sizeof(*_tmp17C));({struct _dyneither_ptr _tmp94C=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp17F;_tmp17F.tag=1U;_tmp17F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp17D[1U]={& _tmp17F};({struct _dyneither_ptr _tmp94B=({const char*_tmp17E="_tuple%d";_tag_dyneither(_tmp17E,sizeof(char),9U);});Cyc_aprintf(_tmp94B,_tag_dyneither(_tmp17D,sizeof(void*),1U));});});});_tmp17C[0]=_tmp94C;});_tmp17C;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp180);
struct Cyc_List_List*_tmp170=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp94F=({struct Cyc_List_List*_tmp171=_cycalloc(sizeof(*_tmp171));({struct Cyc_Absyn_Aggrfield*_tmp94E=({struct Cyc_Absyn_Aggrfield*_tmp172=_cycalloc(sizeof(*_tmp172));({struct _dyneither_ptr*_tmp94D=Cyc_Absyn_fieldname(i);_tmp172->name=_tmp94D;});_tmp172->tq=Cyc_Toc_mt_tq;_tmp172->type=(void*)ts2->hd;_tmp172->width=0;_tmp172->attributes=0;_tmp172->requires_clause=0;_tmp172;});_tmp171->hd=_tmp94E;});_tmp171->tl=_tmp170;_tmp171;});_tmp170=_tmp94F;});}}
({struct Cyc_List_List*_tmp950=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp170);_tmp170=_tmp950;});{
struct Cyc_Absyn_Aggrdecl*_tmp173=({struct Cyc_Absyn_Aggrdecl*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->kind=Cyc_Absyn_StructA;_tmp179->sc=Cyc_Absyn_Public;({struct _tuple1*_tmp952=({struct _tuple1*_tmp17B=_cycalloc(sizeof(*_tmp17B));({union Cyc_Absyn_Nmspace _tmp951=
Cyc_Absyn_Rel_n(0);_tmp17B->f1=_tmp951;});_tmp17B->f2=xname;_tmp17B;});_tmp179->name=_tmp952;});_tmp179->tvs=0;({struct Cyc_Absyn_AggrdeclImpl*_tmp953=({struct Cyc_Absyn_AggrdeclImpl*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->exist_vars=0;_tmp17A->rgn_po=0;_tmp17A->fields=_tmp170;_tmp17A->tagged=0;_tmp17A;});_tmp179->impl=_tmp953;});_tmp179->attributes=0;_tmp179->expected_mem_kind=0;_tmp179;});
# 555
({struct Cyc_List_List*_tmp956=({struct Cyc_List_List*_tmp174=_cycalloc(sizeof(*_tmp174));({struct Cyc_Absyn_Decl*_tmp955=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp175=_cycalloc(sizeof(*_tmp175));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp954=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp176;_tmp176.tag=5U;_tmp176.f1=_tmp173;_tmp176;});_tmp175[0]=_tmp954;});_tmp175;}),0U);_tmp174->hd=_tmp955;});_tmp174->tl=Cyc_Toc_result_decls;_tmp174;});Cyc_Toc_result_decls=_tmp956;});
({struct Cyc_List_List*_tmp958=({struct Cyc_List_List*_tmp177=_region_malloc(d,sizeof(*_tmp177));({struct _tuple22*_tmp957=({struct _tuple22*_tmp178=_region_malloc(d,sizeof(*_tmp178));_tmp178->f1=x;_tmp178->f2=ts;_tmp178;});_tmp177->hd=_tmp957;});_tmp177->tl=*_tmp181;_tmp177;});*_tmp181=_tmp958;});
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
int _tmp959=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1AA);_tmp959 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp18D);})){
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
 if(Cyc_Tcutil_unify(_tmp188,_tmp189) || ({void*_tmp95A=Cyc_Toc_typ_to_c(_tmp188);Cyc_Tcutil_unify(_tmp95A,Cyc_Toc_typ_to_c(_tmp189));}))
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
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));({struct _dyneither_ptr _tmp95C=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1A8;_tmp1A8.tag=1U;_tmp1A8.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp1A6[1U]={& _tmp1A8};({struct _dyneither_ptr _tmp95B=({const char*_tmp1A7="_tuple%d";_tag_dyneither(_tmp1A7,sizeof(char),9U);});Cyc_aprintf(_tmp95B,_tag_dyneither(_tmp1A6,sizeof(void*),1U));});});});_tmp1A5[0]=_tmp95C;});_tmp1A5;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp18F=0;
# 623
({struct Cyc_List_List*_tmp95E=({struct Cyc_List_List*_tmp190=_region_malloc(d,sizeof(*_tmp190));({struct _tuple25*_tmp95D=({struct _tuple25*_tmp191=_region_malloc(d,sizeof(*_tmp191));_tmp191->f1=_tmp1AC;_tmp191->f2=_tmp1AA;_tmp191->f3=x;_tmp191;});_tmp190->hd=_tmp95D;});_tmp190->tl=*_tmp1AD;_tmp190;});*_tmp1AD=_tmp95E;});{
# 626
struct _RegionHandle _tmp192=_new_region("r");struct _RegionHandle*r=& _tmp192;_push_region(r);
{struct Cyc_List_List*_tmp193=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1AB,_tmp1AA);
for(0;_tmp1A9 != 0;_tmp1A9=_tmp1A9->tl){
struct Cyc_Absyn_Aggrfield*_tmp194=(struct Cyc_Absyn_Aggrfield*)_tmp1A9->hd;
void*t=_tmp194->type;
struct Cyc_List_List*atts=_tmp194->attributes;
# 634
if(_tmp1A9->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t)))
({struct Cyc_List_List*_tmp961=({struct Cyc_List_List*_tmp195=_cycalloc(sizeof(*_tmp195));({void*_tmp960=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp196=_cycalloc(sizeof(*_tmp196));({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp95F=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp197;_tmp197.tag=6U;_tmp197.f1=0;_tmp197;});_tmp196[0]=_tmp95F;});_tmp196;});_tmp195->hd=_tmp960;});_tmp195->tl=atts;_tmp195;});atts=_tmp961;});
# 637
({void*_tmp962=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp193,t));t=_tmp962;});
# 640
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
({void*_tmp969=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp198=_cycalloc(sizeof(*_tmp198));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp968=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp199;_tmp199.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmp967=({struct Cyc_Absyn_ArrayInfo _tmp19A;({void*_tmp963=Cyc_Absyn_void_star_typ();_tmp19A.elt_type=_tmp963;});({struct Cyc_Absyn_Tqual _tmp964=Cyc_Absyn_empty_tqual(0U);_tmp19A.tq=_tmp964;});({struct Cyc_Absyn_Exp*_tmp965=
Cyc_Absyn_uint_exp(0U,0U);_tmp19A.num_elts=_tmp965;});({union Cyc_Absyn_Constraint*_tmp966=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp19A.zero_term=_tmp966;});_tmp19A.zt_loc=0U;_tmp19A;});_tmp199.f1=_tmp967;});_tmp199;});_tmp198[0]=_tmp968;});_tmp198;});
# 641
t=_tmp969;});
# 644
({struct Cyc_List_List*_tmp96B=({struct Cyc_List_List*_tmp19B=_cycalloc(sizeof(*_tmp19B));({struct Cyc_Absyn_Aggrfield*_tmp96A=({struct Cyc_Absyn_Aggrfield*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->name=_tmp194->name;_tmp19C->tq=Cyc_Toc_mt_tq;_tmp19C->type=t;_tmp19C->width=_tmp194->width;_tmp19C->attributes=atts;_tmp19C->requires_clause=0;_tmp19C;});_tmp19B->hd=_tmp96A;});_tmp19B->tl=_tmp18F;_tmp19B;});_tmp18F=_tmp96B;});}
# 646
({struct Cyc_List_List*_tmp96C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp18F);_tmp18F=_tmp96C;});{
struct Cyc_Absyn_Aggrdecl*_tmp19D=({struct Cyc_Absyn_Aggrdecl*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->kind=Cyc_Absyn_StructA;_tmp1A2->sc=Cyc_Absyn_Public;({struct _tuple1*_tmp96E=({struct _tuple1*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));({union Cyc_Absyn_Nmspace _tmp96D=
Cyc_Absyn_Rel_n(0);_tmp1A4->f1=_tmp96D;});_tmp1A4->f2=xname;_tmp1A4;});_tmp1A2->name=_tmp96E;});_tmp1A2->tvs=0;({struct Cyc_Absyn_AggrdeclImpl*_tmp96F=({struct Cyc_Absyn_AggrdeclImpl*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->exist_vars=0;_tmp1A3->rgn_po=0;_tmp1A3->fields=_tmp18F;_tmp1A3->tagged=0;_tmp1A3;});_tmp1A2->impl=_tmp96F;});_tmp1A2->attributes=0;_tmp1A2->expected_mem_kind=0;_tmp1A2;});
# 654
({struct Cyc_List_List*_tmp972=({struct Cyc_List_List*_tmp19E=_cycalloc(sizeof(*_tmp19E));({struct Cyc_Absyn_Decl*_tmp971=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp19F=_cycalloc(sizeof(*_tmp19F));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp970=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1A0;_tmp1A0.tag=5U;_tmp1A0.f1=_tmp19D;_tmp1A0;});_tmp19F[0]=_tmp970;});_tmp19F;}),0U);_tmp19E->hd=_tmp971;});_tmp19E->tl=Cyc_Toc_result_decls;_tmp19E;});Cyc_Toc_result_decls=_tmp972;});{
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
return({struct _tuple1*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp975=({struct _dyneither_ptr*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));({struct _dyneither_ptr _tmp974=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=1U;_tmp1B3.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;({void*_tmp1B1[1U]={& _tmp1B3};({struct _dyneither_ptr _tmp973=({const char*_tmp1B2="_tmp%X";_tag_dyneither(_tmp1B2,sizeof(char),7U);});Cyc_aprintf(_tmp973,_tag_dyneither(_tmp1B1,sizeof(void*),1U));});});});_tmp1B0[0]=_tmp974;});_tmp1B0;});_tmp1AF->f2=_tmp975;});_tmp1AF;});}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 675
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp1B4=*env;struct _tuple26 _tmp1B5=_tmp1B4;struct Cyc_Xarray_Xarray*_tmp1BD;_LL1: _tmp1BD=(_tmp1B5.f1)->temp_labels;_LL2:;{
int _tmp1B6=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp976=_tmp1B6;_tmp976 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp1BD);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp1BD,_tmp1B6);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));({struct _dyneither_ptr _tmp978=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1BC;_tmp1BC.tag=1U;_tmp1BC.f1=(unsigned int)_tmp1B6;({void*_tmp1BA[1U]={& _tmp1BC};({struct _dyneither_ptr _tmp977=({const char*_tmp1BB="_LL%X";_tag_dyneither(_tmp1BB,sizeof(char),6U);});Cyc_aprintf(_tmp977,_tag_dyneither(_tmp1BA,sizeof(void*),1U));});});});_tmp1B9[0]=_tmp978;});_tmp1B9;});
if(({int _tmp979=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp1BD,res);_tmp979 != _tmp1B6;}))
({void*_tmp1B7=0U;({struct _dyneither_ptr _tmp97A=({const char*_tmp1B8="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp1B8,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp97A,_tag_dyneither(_tmp1B7,sizeof(void*),0U));});});
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
return({struct _tuple9*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->f1=_tmp1C4;_tmp1C1->f2=_tmp1C3;({void*_tmp97B=Cyc_Toc_typ_to_c(_tmp1C2);_tmp1C1->f3=_tmp97B;});_tmp1C1;});}
# 712
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1C5=*x;struct _tuple11 _tmp1C6=_tmp1C5;struct Cyc_Absyn_Tqual _tmp1C9;void*_tmp1C8;_LL1: _tmp1C9=_tmp1C6.f1;_tmp1C8=_tmp1C6.f2;_LL2:;
return({struct _tuple11*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->f1=_tmp1C9;({void*_tmp97C=Cyc_Toc_typ_to_c(_tmp1C8);_tmp1C7->f2=_tmp97C;});_tmp1C7;});}
# 731 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1CA=Cyc_Tcutil_compress(t);void*_tmp1CB=_tmp1CA;void*_tmp1D1;void*_tmp1D0;struct Cyc_Absyn_Tqual _tmp1CF;struct Cyc_Absyn_Exp*_tmp1CE;union Cyc_Absyn_Constraint*_tmp1CD;unsigned int _tmp1CC;switch(*((int*)_tmp1CB)){case 8U: _LL1: _tmp1D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).elt_type;_tmp1CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).tq;_tmp1CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).num_elts;_tmp1CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).zero_term;_tmp1CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CB)->f1).zt_loc;_LL2:
# 734
 return({void*_tmp97D=Cyc_Toc_typ_to_c_array(_tmp1D0);Cyc_Absyn_cstar_typ(_tmp97D,_tmp1CF);});case 1U: _LL3: _tmp1D1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CB)->f2;if(_tmp1D1 != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp1D1);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 740
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 742
return({struct Cyc_Absyn_Aggrfield*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->name=f->name;_tmp1D2->tq=Cyc_Toc_mt_tq;({void*_tmp97E=
# 744
Cyc_Toc_typ_to_c(f->type);_tmp1D2->type=_tmp97E;});_tmp1D2->width=f->width;_tmp1D2->attributes=f->attributes;_tmp1D2->requires_clause=0;_tmp1D2;});}
# 749
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 751
return;}
# 754
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
({void**_tmp980=({void**_tmp1D3=_cycalloc(sizeof(*_tmp1D3));({void*_tmp97F=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp1D3[0]=_tmp97F;});_tmp1D3;});cs=_tmp980;});
return*cs;}
# 760
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp983=({void**_tmp1D4=_cycalloc(sizeof(*_tmp1D4));({void*_tmp982=({void*_tmp981=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp981,Cyc_Toc_mt_tq);});_tmp1D4[0]=_tmp982;});_tmp1D4;});r=_tmp983;});
return*r;}
# 766
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp986=({void**_tmp1D5=_cycalloc(sizeof(*_tmp1D5));({void*_tmp985=({void*_tmp984=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp984,Cyc_Toc_mt_tq);});_tmp1D5[0]=_tmp985;});_tmp1D5;});r=_tmp986;});
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
({void*_tmp1DC=0U;({struct _dyneither_ptr _tmp987=({const char*_tmp1DD="unresolved DatatypeFieldType";_tag_dyneither(_tmp1DD,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp987,_tag_dyneither(_tmp1DC,sizeof(void*),0U));});});}case 5U: _LLD: _tmp20A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DB)->f1).elt_typ;_tmp209=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DB)->f1).elt_tq;_tmp208=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DB)->f1).ptr_atts).bounds;_LLE:
# 808
({void*_tmp988=Cyc_Toc_typ_to_c(_tmp20A);_tmp20A=_tmp988;});{
void*_tmp1DE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp208);void*_tmp1DF=_tmp1DE;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1DF)->tag == 0U){_LL42: _LL43:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LL44: _LL45:
 return Cyc_Absyn_star_typ(_tmp20A,(void*)& Cyc_Absyn_HeapRgn_val,_tmp209,Cyc_Absyn_false_conref);}_LL41:;};case 6U: _LLF: _LL10:
# 813
 goto _LL12;case 7U: _LL11: _LL12:
 return t;case 8U: _LL13: _tmp20E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DB)->f1).elt_type;_tmp20D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DB)->f1).tq;_tmp20C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DB)->f1).num_elts;_tmp20B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DB)->f1).zt_loc;_LL14:
# 816
 return({void*_tmp98C=Cyc_Toc_typ_to_c(_tmp20E);struct Cyc_Absyn_Tqual _tmp98B=_tmp20D;struct Cyc_Absyn_Exp*_tmp98A=_tmp20C;union Cyc_Absyn_Constraint*_tmp989=Cyc_Absyn_false_conref;Cyc_Absyn_array_typ(_tmp98C,_tmp98B,_tmp98A,_tmp989,_tmp20B);});case 9U: _LL15: _tmp214=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).ret_tqual;_tmp213=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).ret_typ;_tmp212=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).args;_tmp211=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).c_varargs;_tmp210=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).cyc_varargs;_tmp20F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).attributes;_LL16: {
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
({struct Cyc_List_List*_tmp98D=({struct Cyc_List_List*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->hd=(void*)_tmp20F->hd;_tmp1E3->tl=_tmp1E0;_tmp1E3;});_tmp1E0=_tmp98D;});goto _LL46;}_LL46:;}{
# 833
struct Cyc_List_List*_tmp1E4=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp212);
if(_tmp210 != 0){
# 836
void*_tmp1E5=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp210->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp1E6=({struct _tuple9*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->f1=_tmp210->name;_tmp1E8->f2=_tmp210->tq;_tmp1E8->f3=_tmp1E5;_tmp1E8;});
({struct Cyc_List_List*_tmp98F=({struct Cyc_List_List*_tmp98E=_tmp1E4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp98E,({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=_tmp1E6;_tmp1E7->tl=0;_tmp1E7;}));});_tmp1E4=_tmp98F;});}
# 840
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp992=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1EA;_tmp1EA.tag=9U;({struct Cyc_Absyn_FnInfo _tmp991=({struct Cyc_Absyn_FnInfo _tmp1EB;_tmp1EB.tvars=0;_tmp1EB.effect=0;_tmp1EB.ret_tqual=_tmp214;({void*_tmp990=Cyc_Toc_typ_to_c(_tmp213);_tmp1EB.ret_typ=_tmp990;});_tmp1EB.args=_tmp1E4;_tmp1EB.c_varargs=_tmp211;_tmp1EB.cyc_varargs=0;_tmp1EB.rgn_po=0;_tmp1EB.attributes=_tmp1E0;_tmp1EB.requires_clause=0;_tmp1EB.requires_relns=0;_tmp1EB.ensures_clause=0;_tmp1EB.ensures_relns=0;_tmp1EB;});_tmp1EA.f1=_tmp991;});_tmp1EA;});_tmp1E9[0]=_tmp992;});_tmp1E9;});};}case 10U: _LL17: _tmp215=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1DB)->f1;_LL18:
# 845
({struct Cyc_List_List*_tmp993=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp215);_tmp215=_tmp993;});
return Cyc_Toc_add_tuple_type(_tmp215);case 12U: _LL19: _tmp217=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1DB)->f1;_tmp216=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1DB)->f2;_LL1A:
# 850
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp995=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1ED;_tmp1ED.tag=12U;_tmp1ED.f1=_tmp217;({struct Cyc_List_List*_tmp994=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp216);_tmp1ED.f2=_tmp994;});_tmp1ED;});_tmp1EC[0]=_tmp995;});_tmp1EC;});case 11U: _LL1B: _tmp219=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1DB)->f1).aggr_info;_tmp218=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1DB)->f1).targs;_LL1C:
# 854
{union Cyc_Absyn_AggrInfoU _tmp1EE=_tmp219;if((_tmp1EE.UnknownAggr).tag == 1){_LL56: _LL57:
 return t;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}{
# 858
struct Cyc_Absyn_Aggrdecl*_tmp1EF=Cyc_Absyn_get_known_aggrdecl(_tmp219);
if(_tmp1EF->expected_mem_kind){
# 861
if(_tmp1EF->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1F3;_tmp1F3.tag=0U;({struct _dyneither_ptr _tmp996=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 864
Cyc_Absynpp_qvar2string(_tmp1EF->name));_tmp1F3.f1=_tmp996;});({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0U;({struct _dyneither_ptr _tmp997=(struct _dyneither_ptr)(
# 863
_tmp1EF->kind == Cyc_Absyn_UnionA?({const char*_tmp1F4="union";_tag_dyneither(_tmp1F4,sizeof(char),6U);}):({const char*_tmp1F5="struct";_tag_dyneither(_tmp1F5,sizeof(char),7U);}));_tmp1F2.f1=_tmp997;});({void*_tmp1F0[2U]={& _tmp1F2,& _tmp1F3};({struct _dyneither_ptr _tmp998=({const char*_tmp1F1="%s %s was never defined.";_tag_dyneither(_tmp1F1,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp998,_tag_dyneither(_tmp1F0,sizeof(void*),2U));});});});});}
# 869
if(_tmp1EF->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1EF->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1F6=_tmp1EF->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1EF->impl))->fields;
if(_tmp1F6 == 0)return Cyc_Toc_aggrdecl_type(_tmp1EF->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp1F6))->tl != 0;_tmp1F6=_tmp1F6->tl){;}{
void*_tmp1F7=((struct Cyc_Absyn_Aggrfield*)_tmp1F6->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1F7))){
if(_tmp1EF->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0U;({struct _dyneither_ptr _tmp999=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp1EF->name));_tmp1FA.f1=_tmp999;});({void*_tmp1F8[1U]={& _tmp1FA};({struct _dyneither_ptr _tmp99A=({const char*_tmp1F9="struct %s ended up being abstract.";_tag_dyneither(_tmp1F9,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp99A,_tag_dyneither(_tmp1F8,sizeof(void*),1U));});});});{
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
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp200=_cycalloc(sizeof(*_tmp200));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp99B=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp201;_tmp201.tag=17U;_tmp201.f1=_tmp21F;_tmp201.f2=0;_tmp201.f3=_tmp21D;_tmp201.f4=0;_tmp201;});_tmp200[0]=_tmp99B;});_tmp200;});else{
return t;}}else{
# 899
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp202=_cycalloc(sizeof(*_tmp202));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp99D=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp203;_tmp203.tag=17U;_tmp203.f1=_tmp21F;_tmp203.f2=0;_tmp203.f3=_tmp21D;({void*_tmp99C=Cyc_Toc_typ_to_c(_tmp21C);_tmp203.f4=_tmp99C;});_tmp203;});_tmp202[0]=_tmp99D;});_tmp202;});}case 19U: _LL23: _LL24:
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
 return({void*_tmp99E=Cyc_Absyn_star_typ(_tmp228,(void*)& Cyc_Absyn_HeapRgn_val,_tmp227,Cyc_Absyn_false_conref);Cyc_Toc_cast_it(_tmp99E,e);});}else{_LL3: _LL4:
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
({struct Cyc_String_pa_PrintArg_struct _tmp230;_tmp230.tag=0U;({struct _dyneither_ptr _tmp99F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp230.f1=_tmp99F;});({void*_tmp22E[1U]={& _tmp230};({struct _dyneither_ptr _tmp9A0=({const char*_tmp22F="atomic_typ:  bad type %s";_tag_dyneither(_tmp22F,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9A0,_tag_dyneither(_tmp22E,sizeof(void*),1U));});});});}_LL0:;}
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
({void*_tmp23F=0U;({struct _dyneither_ptr _tmp9A1=({const char*_tmp240="is_poly_field: type missing fields";_tag_dyneither(_tmp240,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9A1,_tag_dyneither(_tmp23F,sizeof(void*),0U));});});
_tmp249=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->fields;goto _LL4;}case 12U: _LL3: _tmp249=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23D)->f2;_LL4: {
# 1018
struct Cyc_Absyn_Aggrfield*_tmp241=Cyc_Absyn_lookup_field(_tmp249,f);
if(_tmp241 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp244;_tmp244.tag=0U;_tmp244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp242[1U]={& _tmp244};({struct _dyneither_ptr _tmp9A2=({const char*_tmp243="is_poly_field: bad field %s";_tag_dyneither(_tmp243,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9A2,_tag_dyneither(_tmp242,sizeof(void*),1U));});});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp241->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp247;_tmp247.tag=0U;({struct _dyneither_ptr _tmp9A3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp247.f1=_tmp9A3;});({void*_tmp245[1U]={& _tmp247};({struct _dyneither_ptr _tmp9A4=({const char*_tmp246="is_poly_field: bad type %s";_tag_dyneither(_tmp246,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9A4,_tag_dyneither(_tmp245,sizeof(void*),1U));});});});}_LL0:;}
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
({struct Cyc_String_pa_PrintArg_struct _tmp250;_tmp250.tag=0U;({struct _dyneither_ptr _tmp9A5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp255->topt)));_tmp250.f1=_tmp9A5;});({void*_tmp24E[1U]={& _tmp250};({struct _dyneither_ptr _tmp9A6=({const char*_tmp24F="is_poly_project: bad type %s";_tag_dyneither(_tmp24F,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9A6,_tag_dyneither(_tmp24E,sizeof(void*),1U));});});});}_LL7:;}default: _LL5: _LL6:
# 1040
 return 0;}_LL0:;}
# 1045
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp9A7=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp9A7,({struct Cyc_List_List*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256->hd=s;_tmp256->tl=0;_tmp256;}),0U);});}
# 1049
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp9A8=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp9A8,({struct Cyc_List_List*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257->hd=s;_tmp257->tl=0;_tmp257;}),0U);});}
# 1053
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1059
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp9A9=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp9A9,({struct Cyc_Absyn_Exp*_tmp258[2U];_tmp258[1U]=s;_tmp258[0U]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp258,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1063
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp9AA=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp9AA,({struct Cyc_Absyn_Exp*_tmp259[2U];_tmp259[1U]=s;_tmp259[0U]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp259,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1067
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return({struct Cyc_Absyn_Exp*_tmp9AB=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp9AB,({struct Cyc_Absyn_Exp*_tmp25A[2U];_tmp25A[1U]=n;_tmp25A[0U]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25A,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1071
return({struct Cyc_Absyn_Exp*_tmp9AC=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp9AC,({struct Cyc_Absyn_Exp*_tmp25B[2U];_tmp25B[1U]=n;_tmp25B[0U]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25B,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1074
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp9AD=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp9AD,({struct Cyc_Absyn_Exp*_tmp25C[3U];_tmp25C[2U]=n;_tmp25C[1U]=s;_tmp25C[0U]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25C,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1079
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp9AE=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp9AE,({struct Cyc_List_List*_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D->hd=e;_tmp25D->tl=0;_tmp25D;}),0U);});}
# 1082
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp9AF=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp9AF,({struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->hd=e;_tmp25E->tl=0;_tmp25E;}),0U);});}
# 1086
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp9B0,({struct Cyc_Absyn_Exp*_tmp25F[0U];((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25F,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
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
({struct Cyc_List_List*_tmp9B3=({struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));({struct Cyc_Absyn_Decl*_tmp9B2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp263=_cycalloc(sizeof(*_tmp263));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp9B1=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp264;_tmp264.tag=0U;_tmp264.f1=vd;_tmp264;});_tmp263[0]=_tmp9B1;});_tmp263;}),0U);_tmp262->hd=_tmp9B2;});_tmp262->tl=Cyc_Toc_result_decls;_tmp262;});Cyc_Toc_result_decls=_tmp9B3;});
({struct Cyc_Absyn_Exp*_tmp9B4=Cyc_Absyn_var_exp(x,0U);xexp=_tmp9B4;});
({struct Cyc_Absyn_Exp*_tmp9B5=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp9B5;});}else{
# 1116
({struct Cyc_Absyn_Exp*_tmp9B7=({void*_tmp9B6=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp9B6,e);});xexp=_tmp9B7;});
# 1118
({struct Cyc_Absyn_Exp*_tmp9B9=({void*_tmp9B8=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp9B8,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp9B9;});}
# 1120
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp265[3U];({struct _tuple19*_tmp9BA=({struct _tuple19*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->f1=0;_tmp268->f2=xplussz;_tmp268;});_tmp265[2U]=_tmp9BA;});({struct _tuple19*_tmp9BB=({struct _tuple19*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->f1=0;_tmp267->f2=xexp;_tmp267;});_tmp265[1U]=_tmp9BB;});({struct _tuple19*_tmp9BC=({struct _tuple19*_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266->f1=0;_tmp266->f2=xexp;_tmp266;});_tmp265[0U]=_tmp9BC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp265,sizeof(struct _tuple19*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
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
return({struct Cyc_Toc_Env*_tmp26D=_region_malloc(r,sizeof(*_tmp26D));_tmp26D->break_lab=(struct _dyneither_ptr**)0;_tmp26D->continue_lab=(struct _dyneither_ptr**)0;_tmp26D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp26D->toplevel=(int)1;({typeof((int*)({int*_tmp270=_region_malloc(r,sizeof(*_tmp270));_tmp270[0]=0;_tmp270;}))_tmp9BD=(int*)({int*_tmp270=_region_malloc(r,sizeof(*_tmp270));_tmp270[0]=0;_tmp270;});_tmp26D->in_lhs=_tmp9BD;});({typeof((struct Cyc_Toc_StructInfo)({({struct Cyc_Absyn_Exp**_tmp9BE=({struct Cyc_Absyn_Exp**_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=0;_tmp26F;});(_tmp26D->struct_info).varsizeexp=_tmp9BE;});(_tmp26D->struct_info).lhs_exp=0;_tmp26D->struct_info;}))_tmp9BF=(struct Cyc_Toc_StructInfo)({({struct Cyc_Absyn_Exp**_tmp9BE=({struct Cyc_Absyn_Exp**_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=0;_tmp26F;});(_tmp26D->struct_info).varsizeexp=_tmp9BE;});(_tmp26D->struct_info).lhs_exp=0;_tmp26D->struct_info;});_tmp26D->struct_info=_tmp9BF;});({typeof((int*)({int*_tmp26E=_region_malloc(r,sizeof(*_tmp26E));_tmp26E[0]=0;_tmp26E;}))_tmp9C0=(int*)({int*_tmp26E=_region_malloc(r,sizeof(*_tmp26E));_tmp26E[0]=0;_tmp26E;});_tmp26D->in_sizeof=_tmp9C0;});_tmp26D->rgn=(struct _RegionHandle*)r;_tmp26D;});}
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
return({struct Cyc_Toc_Env*_tmp294=_region_malloc(r,sizeof(*_tmp294));_tmp294->break_lab=(struct _dyneither_ptr**)_tmp29B;_tmp294->continue_lab=(struct _dyneither_ptr**)_tmp29A;_tmp294->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp299;_tmp294->toplevel=(int)_tmp298;_tmp294->in_lhs=(int*)_tmp297;({typeof((struct Cyc_Toc_StructInfo)({(_tmp294->struct_info).varsizeexp=_tmp296;(_tmp294->struct_info).lhs_exp=exp;_tmp294->struct_info;}))_tmp9C1=(struct Cyc_Toc_StructInfo)({(_tmp294->struct_info).varsizeexp=_tmp296;(_tmp294->struct_info).lhs_exp=exp;_tmp294->struct_info;});_tmp294->struct_info=_tmp9C1;});_tmp294->in_sizeof=(int*)_tmp295;_tmp294->rgn=(struct _RegionHandle*)r;_tmp294;});}
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
return({struct Cyc_Toc_Env*_tmp2A6=_region_malloc(r,sizeof(*_tmp2A6));({typeof((struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2A7=_region_malloc(r,sizeof(*_tmp2A7));_tmp2A7[0]=break_l;_tmp2A7;}))_tmp9C2=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2A7=_region_malloc(r,sizeof(*_tmp2A7));_tmp2A7[0]=break_l;_tmp2A7;});_tmp2A6->break_lab=_tmp9C2;});_tmp2A6->continue_lab=(struct _dyneither_ptr**)_tmp2AE;_tmp2A6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi;_tmp2A6->toplevel=(int)_tmp2AC;_tmp2A6->in_lhs=(int*)_tmp2AB;_tmp2A6->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AA;_tmp2A6->in_sizeof=(int*)_tmp2A9;_tmp2A6->rgn=(struct _RegionHandle*)r;_tmp2A6;});};}
# 1236
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1238
struct Cyc_Toc_Env*_tmp2B0=e;struct _dyneither_ptr**_tmp2B9;struct _dyneither_ptr**_tmp2B8;struct Cyc_Toc_FallthruInfo*_tmp2B7;int _tmp2B6;int*_tmp2B5;struct Cyc_Toc_StructInfo _tmp2B4;int*_tmp2B3;_LL1: _tmp2B9=_tmp2B0->break_lab;_tmp2B8=_tmp2B0->continue_lab;_tmp2B7=_tmp2B0->fallthru_info;_tmp2B6=_tmp2B0->toplevel;_tmp2B5=_tmp2B0->in_lhs;_tmp2B4=_tmp2B0->struct_info;_tmp2B3=_tmp2B0->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp2B1=_region_malloc(r,sizeof(*_tmp2B1));({typeof((struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2B2=_region_malloc(r,sizeof(*_tmp2B2));_tmp2B2[0]=break_l;_tmp2B2;}))_tmp9C3=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2B2=_region_malloc(r,sizeof(*_tmp2B2));_tmp2B2[0]=break_l;_tmp2B2;});_tmp2B1->break_lab=_tmp9C3;});_tmp2B1->continue_lab=(struct _dyneither_ptr**)_tmp2B8;_tmp2B1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp2B1->toplevel=(int)_tmp2B6;_tmp2B1->in_lhs=(int*)_tmp2B5;_tmp2B1->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B4;_tmp2B1->in_sizeof=(int*)_tmp2B3;_tmp2B1->rgn=(struct _RegionHandle*)r;_tmp2B1;});}
# 1245
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1248
struct Cyc_Toc_Env*_tmp2BA=e;struct _dyneither_ptr**_tmp2C3;struct _dyneither_ptr**_tmp2C2;struct Cyc_Toc_FallthruInfo*_tmp2C1;int _tmp2C0;int*_tmp2BF;struct Cyc_Toc_StructInfo _tmp2BE;int*_tmp2BD;_LL1: _tmp2C3=_tmp2BA->break_lab;_tmp2C2=_tmp2BA->continue_lab;_tmp2C1=_tmp2BA->fallthru_info;_tmp2C0=_tmp2BA->toplevel;_tmp2BF=_tmp2BA->in_lhs;_tmp2BE=_tmp2BA->struct_info;_tmp2BD=_tmp2BA->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp2BB=_region_malloc(r,sizeof(*_tmp2BB));_tmp2BB->break_lab=(struct _dyneither_ptr**)0;_tmp2BB->continue_lab=(struct _dyneither_ptr**)_tmp2C2;({typeof((struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2BC=_region_malloc(r,sizeof(*_tmp2BC));_tmp2BC->label=next_l;_tmp2BC->binders=0;_tmp2BC;}))_tmp9C4=(struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2BC=_region_malloc(r,sizeof(*_tmp2BC));_tmp2BC->label=next_l;_tmp2BC->binders=0;_tmp2BC;});_tmp2BB->fallthru_info=_tmp9C4;});_tmp2BB->toplevel=(int)_tmp2C0;_tmp2BB->in_lhs=(int*)_tmp2BF;_tmp2BB->struct_info=(struct Cyc_Toc_StructInfo)_tmp2BE;_tmp2BB->in_sizeof=(int*)_tmp2BD;_tmp2BB->rgn=(struct _RegionHandle*)r;_tmp2BB;});}
# 1261 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1264
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1266
void*_tmp2C4=e->annot;void*_tmp2C5=_tmp2C4;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2C5)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _LL2: {
# 1269
void*_tmp2C6=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2C7=_tmp2C6;union Cyc_Absyn_Constraint*_tmp2CA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7)->tag == 5U){_LLC: _tmp2CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7)->f1).ptr_atts).nullable;_LLD:
# 1272
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2CA);}else{_LLE: _LLF:
({void*_tmp2C8=0U;({struct _dyneither_ptr _tmp9C5=({const char*_tmp2C9="Toc::is_nullable";_tag_dyneither(_tmp2C9,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9C5,_tag_dyneither(_tmp2C8,sizeof(void*),0U));});});}_LLB:;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2C5)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _LL4:
# 1277
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2C5)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1279
({void*_tmp2CB=0U;({unsigned int _tmp9C7=e->loc;struct _dyneither_ptr _tmp9C6=({const char*_tmp2CC="NULL pointer check will definitely fail";_tag_dyneither(_tmp2CC,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9C7,_tmp9C6,_tag_dyneither(_tmp2CB,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2C5)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
# 1285
 return 1;}else{_LL9: _LLA:
({void*_tmp2CD=0U;({struct _dyneither_ptr _tmp9C8=({const char*_tmp2CE="need_null_check";_tag_dyneither(_tmp2CE,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9C8,_tag_dyneither(_tmp2CD,sizeof(void*),0U));});});}}}}_LL0:;}
# 1290
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2CF=e->annot;void*_tmp2D0=_tmp2CF;struct Cyc_List_List*_tmp2D6;struct Cyc_List_List*_tmp2D5;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp2D5=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2D0)->f1;_LL2:
 return _tmp2D5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp2D6=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2D0)->f1;_LL4:
 return _tmp2D6;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1295
({void*_tmp2D1=0U;({unsigned int _tmp9CA=e->loc;struct _dyneither_ptr _tmp9C9=({const char*_tmp2D2="dereference of NULL pointer";_tag_dyneither(_tmp2D2,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp9CA,_tmp9C9,_tag_dyneither(_tmp2D1,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
({void*_tmp2D3=0U;({struct _dyneither_ptr _tmp9CB=({const char*_tmp2D4="get_relns";_tag_dyneither(_tmp2D4,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9CB,_tag_dyneither(_tmp2D3,sizeof(void*),0U));});});}}}}_LL0:;}static char _tmp2DA[8U]="*bogus*";
# 1305
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1315 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1318
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2D7=a->r;void*_tmp2D8=_tmp2D7;void*_tmp2DB;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D8)->tag == 1U){_LL1: _tmp2DB=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL2: {
# 1321
struct Cyc_Absyn_Vardecl*_tmp2D9=Cyc_Tcutil_nonesc_vardecl(_tmp2DB);
if(_tmp2D9 == 0)goto _LL4;
x=_tmp2D9;
goto _LL0;}}else{_LL3: _LL4: {
# 1327
static struct _dyneither_ptr bogus_string={_tmp2DA,_tmp2DA,_tmp2DA + 8U};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1330
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1336
x=& bogus_vardecl;
x->type=a_typ;}}_LL0:;}{
# 1339
void*_tmp2DC=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1344
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2DC);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
({struct Cyc_List_List*_tmp9CC=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);relns=_tmp9CC;});}
# 1353
if(valid_rop_i){
struct Cyc_List_List*_tmp2DD=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1361
if(!Cyc_Relations_consistent_relations(_tmp2DD))return 1;}
# 1364
inner_loop: {
void*_tmp2DE=i->r;void*_tmp2DF=_tmp2DE;struct Cyc_Absyn_Exp*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E3;void*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E1;switch(*((int*)_tmp2DF)){case 14U: _LL6: _tmp2E2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp2E1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL7:
 i=_tmp2E1;goto inner_loop;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2)->tl != 0){_LL8: _tmp2E4=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2)->hd;_tmp2E3=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2)->tl)->hd;_LL9: {
# 1370
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp2E3,& rop_z)){
# 1373
struct Cyc_List_List*_tmp2E0=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1375
return !Cyc_Relations_consistent_relations(_tmp2E0);}
# 1377
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1380
return 0;};}
# 1383
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2E5=0U;({struct _dyneither_ptr _tmp9CD=({const char*_tmp2E6="Missing type in primop ";_tag_dyneither(_tmp2E6,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9CD,_tag_dyneither(_tmp2E5,sizeof(void*),0U));});});
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1387
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2E7=0U;({struct _dyneither_ptr _tmp9CE=({const char*_tmp2E8="Missing type in primop ";_tag_dyneither(_tmp2E8,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9CE,_tag_dyneither(_tmp2E7,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1391
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple11*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9->f1=Cyc_Toc_mt_tq;({void*_tmp9CF=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp2E9->f2=_tmp9CF;});_tmp2E9;});}
# 1394
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);
return({struct _tuple19*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA->f1=0;_tmp2EA->f2=e;_tmp2EA;});}
# 1401
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1404
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2EB=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0U),0U);
if(*(nv->struct_info).varsizeexp != 0){
({struct Cyc_Absyn_Exp*_tmp9D0=Cyc_Absyn_add_exp(_tmp2EB,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0U);_tmp2EB=_tmp9D0;});
*(nv->struct_info).varsizeexp=0;}
# 1412
({void*_tmp9D1=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);t=_tmp9D1;});
if(rgnopt == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp9D2=is_atomic?Cyc_Toc_malloc_atomic(_tmp2EB): Cyc_Toc_malloc_ptr(_tmp2EB);eo=_tmp9D2;});else{
# 1416
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp9D3=Cyc_Toc_rmalloc_exp(r,_tmp2EB);eo=_tmp9D3;});}}else{
# 1421
t=struct_typ;
eo=0;}{
# 1424
struct Cyc_Absyn_Exp*res;
if(do_declare){
void*_tmp2EC=x->r;void*_tmp2ED=_tmp2EC;void*_tmp2F0;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2ED)->tag == 1U){_LL1: _tmp2F0=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1;_LL2:
# 1428
({struct Cyc_Absyn_Exp*_tmp9D7=Cyc_Absyn_stmt_exp(({struct _tuple1*_tmp9D6=Cyc_Absyn_binding2qvar(_tmp2F0);void*_tmp9D5=t;struct Cyc_Absyn_Exp*_tmp9D4=eo;Cyc_Absyn_declare_stmt(_tmp9D6,_tmp9D5,_tmp9D4,s,0U);}),0U);res=_tmp9D7;});
res->topt=t;
return res;}else{_LL3: _LL4:
({void*_tmp2EE=0U;({struct _dyneither_ptr _tmp9D8=({const char*_tmp2EF="bogus x to make_struct";_tag_dyneither(_tmp2EF,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D8,_tag_dyneither(_tmp2EE,sizeof(void*),0U));});});}_LL0:;}else{
# 1435
if(eo != 0)
({struct Cyc_Absyn_Exp*_tmp9DA=Cyc_Absyn_stmt_exp(({struct Cyc_Absyn_Stmt*_tmp9D9=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0U),0U);Cyc_Absyn_seq_stmt(_tmp9D9,s,0U);}),0U);res=_tmp9DA;});else{
# 1438
({struct Cyc_Absyn_Exp*_tmp9DB=Cyc_Absyn_stmt_exp(s,0U);res=_tmp9DB;});}
res->topt=t;
return res;}};}
# 1444
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp2F1=Cyc_Tcutil_compress(typ);void*_tmp2F2=_tmp2F1;struct Cyc_List_List*_tmp2F5;union Cyc_Absyn_AggrInfoU _tmp2F4;struct Cyc_List_List*_tmp2F3;switch(*((int*)_tmp2F2)){case 11U: _LL1: _tmp2F4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F2)->f1).aggr_info;_tmp2F3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F2)->f1).targs;_LL2: {
# 1447
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp2F4);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp2F5=aggrfields;goto _LL4;};}case 12U: _LL3: _tmp2F5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F2)->f2;_LL4:
# 1452
 if(_tmp2F5 == 0)return 0;
return 1;default: _LL5: _LL6:
# 1458
 return 0;}_LL0:;}
# 1462
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp2F6=Cyc_Tcutil_compress(t);void*_tmp2F7=_tmp2F6;void*_tmp2FB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F7)->tag == 8U){_LL1: _tmp2FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F7)->f1).elt_type;_LL2:
 return _tmp2FB;}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp2FA;_tmp2FA.tag=0U;({struct _dyneither_ptr _tmp9DC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp2FA.f1=_tmp9DC;});({void*_tmp2F8[1U]={& _tmp2FA};({struct _dyneither_ptr _tmp9DD=({const char*_tmp2F9="array_elt_type called on %s";_tag_dyneither(_tmp2F9,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9DD,_tag_dyneither(_tmp2F8,sizeof(void*),1U));});});});}_LL0:;}
# 1469
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1475
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1480
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1485
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp2FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp2FC != 0;_tmp2FC=_tmp2FC->tl){
# 1489
struct _tuple19*_tmp2FD=(struct _tuple19*)_tmp2FC->hd;struct _tuple19*_tmp2FE=_tmp2FD;struct Cyc_List_List*_tmp310;struct Cyc_Absyn_Exp*_tmp30F;_LL1: _tmp310=_tmp2FE->f1;_tmp30F=_tmp2FE->f2;_LL2:;{
# 1493
struct Cyc_Absyn_Exp*e_index;
if(_tmp310 == 0)
({struct Cyc_Absyn_Exp*_tmp9DE=Cyc_Absyn_signed_int_exp(count --,0U);e_index=_tmp9DE;});else{
# 1497
if(_tmp310->tl != 0)({void*_tmp2FF=0U;({struct _dyneither_ptr _tmp9DF=({const char*_tmp300="multiple designators in array";_tag_dyneither(_tmp300,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp9DF,_tag_dyneither(_tmp2FF,sizeof(void*),0U));});});{
void*_tmp301=(void*)_tmp310->hd;
void*_tmp302=_tmp301;struct Cyc_Absyn_Exp*_tmp305;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp302)->tag == 0U){_LL4: _tmp305=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp302)->f1;_LL5:
# 1501
 Cyc_Toc_exp_to_c(nv,_tmp305);
e_index=_tmp305;
goto _LL3;}else{_LL6: _LL7:
({void*_tmp303=0U;({struct _dyneither_ptr _tmp9E0=({const char*_tmp304="field name designators in array";_tag_dyneither(_tmp304,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp9E0,_tag_dyneither(_tmp303,sizeof(void*),0U));});});}_LL3:;};}{
# 1507
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0U);
void*_tmp306=_tmp30F->r;void*_tmp307=_tmp306;void*_tmp30E;struct Cyc_List_List*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30C;struct Cyc_Absyn_Exp*_tmp30B;struct Cyc_Absyn_Exp*_tmp30A;int _tmp309;struct Cyc_List_List*_tmp308;switch(*((int*)_tmp307)){case 26U: _LL9: _tmp308=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp307)->f1;_LLA:
# 1510
({struct Cyc_Absyn_Stmt*_tmp9E1=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp308,s);s=_tmp9E1;});
goto _LL8;case 27U: _LLB: _tmp30C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f1;_tmp30B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f2;_tmp30A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f3;_tmp309=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f4;_LLC:
# 1513
({struct Cyc_Absyn_Stmt*_tmp9E2=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp30C,_tmp30B,_tmp30A,_tmp309,s,0);s=_tmp9E2;});
# 1515
goto _LL8;case 30U: _LLD: _tmp30E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp307)->f1;_tmp30D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp307)->f2;_LLE:
# 1517
({struct Cyc_Absyn_Stmt*_tmp9E3=Cyc_Toc_init_anon_struct(nv,lval,_tmp30E,_tmp30D,s);s=_tmp9E3;});
goto _LL8;case 28U: _LLF: _LL10:
# 1520
 goto _LL8;default: _LL11: _LL12:
# 1522
 Cyc_Toc_exp_to_c(nv,_tmp30F);
({struct Cyc_Absyn_Stmt*_tmp9E7=({struct Cyc_Absyn_Stmt*_tmp9E6=({struct Cyc_Absyn_Exp*_tmp9E5=Cyc_Absyn_subscript_exp(lhs,e_index,0U);Cyc_Absyn_assign_stmt(_tmp9E5,({
void*_tmp9E4=Cyc_Toc_typ_to_c(orig_elt_type);Cyc_Toc_cast_it(_tmp9E4,_tmp30F);}),0U);});
# 1523
Cyc_Absyn_seq_stmt(_tmp9E6,s,0U);});s=_tmp9E7;});
# 1525
goto _LL8;}_LL8:;};};}}
# 1528
return s;}
# 1533
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1538
struct _tuple1*_tmp311=vd->name;
void*expected_elt_type;
{void*_tmp312=Cyc_Tcutil_compress(comprehension_type);void*_tmp313=_tmp312;void*_tmp317;void*_tmp316;switch(*((int*)_tmp313)){case 8U: _LL1: _tmp316=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313)->f1).elt_type;_LL2:
# 1542
 expected_elt_type=_tmp316;goto _LL0;case 5U: _LL3: _tmp317=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313)->f1).elt_typ;_LL4:
# 1544
 expected_elt_type=_tmp317;goto _LL0;default: _LL5: _LL6:
({void*_tmp314=0U;({struct _dyneither_ptr _tmp9E8=({const char*_tmp315="init_comprehension passed a bad type!";_tag_dyneither(_tmp315,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E8,_tag_dyneither(_tmp314,sizeof(void*),0U));});});}_LL0:;}{
# 1547
void*_tmp318=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1551
{void*_tmp319=e2->r;void*_tmp31A=_tmp319;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp31A)->tag == 28U){_LL8: _LL9:
 return Cyc_Absyn_skip_stmt(0U);}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}{
# 1555
struct _RegionHandle _tmp31B=_new_region("r2");struct _RegionHandle*r2=& _tmp31B;_push_region(r2);
{struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=({struct Cyc_Absyn_Exp*_tmp9E9=Cyc_Absyn_var_exp(_tmp311,0U);Cyc_Absyn_assign_exp(_tmp9E9,Cyc_Absyn_signed_int_exp(0,0U),0U);});
struct Cyc_Absyn_Exp*eb=({struct Cyc_Absyn_Exp*_tmp9EA=Cyc_Absyn_var_exp(_tmp311,0U);Cyc_Absyn_lt_exp(_tmp9EA,Cyc_Absyn_var_exp(max,0U),0U);});
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp311,0U),0U);
# 1562
struct Cyc_Absyn_Exp*lval=({struct Cyc_Absyn_Exp*_tmp9EB=lhs;Cyc_Absyn_subscript_exp(_tmp9EB,Cyc_Absyn_var_exp(_tmp311,0U),0U);});
struct Cyc_Absyn_Stmt*body;
# 1566
{void*_tmp31C=e2->r;void*_tmp31D=_tmp31C;void*_tmp324;struct Cyc_List_List*_tmp323;struct Cyc_Absyn_Vardecl*_tmp322;struct Cyc_Absyn_Exp*_tmp321;struct Cyc_Absyn_Exp*_tmp320;int _tmp31F;struct Cyc_List_List*_tmp31E;switch(*((int*)_tmp31D)){case 26U: _LLD: _tmp31E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp31D)->f1;_LLE:
# 1568
({struct Cyc_Absyn_Stmt*_tmp9F0=({struct Cyc_Toc_Env*_tmp9EF=nv;void*_tmp9EE=expected_elt_type;struct Cyc_Absyn_Exp*_tmp9ED=lval;struct Cyc_List_List*_tmp9EC=_tmp31E;Cyc_Toc_init_array(_tmp9EF,_tmp9EE,_tmp9ED,_tmp9EC,Cyc_Toc_skip_stmt_dl());});body=_tmp9F0;});
goto _LLC;case 27U: _LLF: _tmp322=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f1;_tmp321=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f2;_tmp320=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f3;_tmp31F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f4;_LL10:
# 1571
({struct Cyc_Absyn_Stmt*_tmp9F8=({struct Cyc_Toc_Env*_tmp9F7=nv;void*_tmp9F6=expected_elt_type;struct Cyc_Absyn_Exp*_tmp9F5=lval;struct Cyc_Absyn_Vardecl*_tmp9F4=_tmp322;struct Cyc_Absyn_Exp*_tmp9F3=_tmp321;struct Cyc_Absyn_Exp*_tmp9F2=_tmp320;int _tmp9F1=_tmp31F;Cyc_Toc_init_comprehension(_tmp9F7,_tmp9F6,_tmp9F5,_tmp9F4,_tmp9F3,_tmp9F2,_tmp9F1,Cyc_Toc_skip_stmt_dl(),0);});body=_tmp9F8;});
goto _LLC;case 30U: _LL11: _tmp324=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp31D)->f1;_tmp323=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp31D)->f2;_LL12:
# 1574
({struct Cyc_Absyn_Stmt*_tmp9FD=({struct Cyc_Toc_Env*_tmp9FC=nv;struct Cyc_Absyn_Exp*_tmp9FB=lval;void*_tmp9FA=_tmp324;struct Cyc_List_List*_tmp9F9=_tmp323;Cyc_Toc_init_anon_struct(_tmp9FC,_tmp9FB,_tmp9FA,_tmp9F9,Cyc_Toc_skip_stmt_dl());});body=_tmp9FD;});
goto _LLC;default: _LL13: _LL14:
# 1577
 Cyc_Toc_exp_to_c(nv,e2);
({struct Cyc_Absyn_Stmt*_tmpA00=({struct Cyc_Absyn_Exp*_tmp9FF=lval;Cyc_Absyn_assign_stmt(_tmp9FF,({void*_tmp9FE=Cyc_Toc_typ_to_c(expected_elt_type);Cyc_Toc_cast_it(_tmp9FE,e2);}),0U);});body=_tmpA00;});
goto _LLC;}_LLC:;}{
# 1581
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0U);
# 1583
if(zero_term){
# 1588
struct Cyc_Absyn_Exp*ex=({struct Cyc_Absyn_Exp*_tmpA03=({struct Cyc_Absyn_Exp*_tmpA01=Cyc_Absyn_new_exp(lhs->r,0U);Cyc_Absyn_subscript_exp(_tmpA01,Cyc_Absyn_var_exp(max,0U),0U);});Cyc_Absyn_assign_exp(_tmpA03,({
void*_tmpA02=_tmp318;Cyc_Toc_cast_it(_tmpA02,Cyc_Absyn_uint_exp(0U,0U));}),0U);});
({struct Cyc_Absyn_Stmt*_tmpA05=({struct Cyc_Absyn_Stmt*_tmpA04=s2;Cyc_Absyn_seq_stmt(_tmpA04,Cyc_Absyn_exp_stmt(ex,0U),0U);});s2=_tmpA05;});}{
# 1593
struct Cyc_Absyn_Stmt*_tmp325=({struct Cyc_Absyn_Stmt*_tmpA09=({struct _tuple1*_tmpA08=max;void*_tmpA07=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpA06=e1;Cyc_Absyn_declare_stmt(_tmpA08,_tmpA07,_tmpA06,
Cyc_Absyn_declare_stmt(_tmp311,Cyc_Absyn_uint_typ,0,s2,0U),0U);});
# 1593
Cyc_Absyn_seq_stmt(_tmpA09,s,0U);});_npop_handler(0U);return _tmp325;};};}
# 1556
;_pop_region(r2);};};}
# 1599
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1602
{struct Cyc_List_List*_tmp326=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp326 != 0;_tmp326=_tmp326->tl){
struct _tuple19*_tmp327=(struct _tuple19*)_tmp326->hd;struct _tuple19*_tmp328=_tmp327;struct Cyc_List_List*_tmp33C;struct Cyc_Absyn_Exp*_tmp33B;_LL1: _tmp33C=_tmp328->f1;_tmp33B=_tmp328->f2;_LL2:;
if(_tmp33C == 0)
({void*_tmp329=0U;({struct _dyneither_ptr _tmpA0A=({const char*_tmp32A="empty designator list";_tag_dyneither(_tmp32A,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA0A,_tag_dyneither(_tmp329,sizeof(void*),0U));});});
if(_tmp33C->tl != 0)
({void*_tmp32B=0U;({struct _dyneither_ptr _tmpA0B=({const char*_tmp32C="too many designators in anonymous struct";_tag_dyneither(_tmp32C,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA0B,_tag_dyneither(_tmp32B,sizeof(void*),0U));});});{
void*_tmp32D=(void*)_tmp33C->hd;void*_tmp32E=_tmp32D;struct _dyneither_ptr*_tmp33A;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp32E)->tag == 1U){_LL4: _tmp33A=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp32E)->f1;_LL5: {
# 1610
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp33A,0U);
{void*_tmp32F=_tmp33B->r;void*_tmp330=_tmp32F;void*_tmp337;struct Cyc_List_List*_tmp336;struct Cyc_Absyn_Vardecl*_tmp335;struct Cyc_Absyn_Exp*_tmp334;struct Cyc_Absyn_Exp*_tmp333;int _tmp332;struct Cyc_List_List*_tmp331;switch(*((int*)_tmp330)){case 26U: _LL9: _tmp331=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp330)->f1;_LLA:
# 1613
({struct Cyc_Absyn_Stmt*_tmpA0C=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp33B->topt),lval,_tmp331,s);s=_tmpA0C;});goto _LL8;case 27U: _LLB: _tmp335=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f1;_tmp334=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f2;_tmp333=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f3;_tmp332=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f4;_LLC:
# 1615
({struct Cyc_Absyn_Stmt*_tmpA0D=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp33B->topt),lval,_tmp335,_tmp334,_tmp333,_tmp332,s,0);s=_tmpA0D;});
goto _LL8;case 30U: _LLD: _tmp337=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp330)->f1;_tmp336=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp330)->f2;_LLE:
# 1618
({struct Cyc_Absyn_Stmt*_tmpA0E=Cyc_Toc_init_anon_struct(nv,lval,_tmp337,_tmp336,s);s=_tmpA0E;});goto _LL8;case 28U: _LLF: _LL10:
# 1620
 goto _LL8;default: _LL11: _LL12:
# 1622
 Cyc_Toc_exp_to_c(nv,_tmp33B);
# 1624
if(Cyc_Toc_is_poly_field(struct_type,_tmp33A))
({struct Cyc_Absyn_Exp*_tmpA10=({void*_tmpA0F=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmpA0F,_tmp33B);});_tmp33B=_tmpA10;});
# 1627
({struct Cyc_Absyn_Stmt*_tmpA12=({struct Cyc_Absyn_Stmt*_tmpA11=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp33B,0U),0U);Cyc_Absyn_seq_stmt(_tmpA11,s,0U);});s=_tmpA12;});
goto _LL8;}_LL8:;}
# 1630
goto _LL3;}}else{_LL6: _LL7:
({void*_tmp338=0U;({struct _dyneither_ptr _tmpA13=({const char*_tmp339="array designator in struct";_tag_dyneither(_tmp339,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA13,_tag_dyneither(_tmp338,sizeof(void*),0U));});});}_LL3:;};}}
# 1634
return s;}
# 1639
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1643
struct _RegionHandle _tmp33D=_new_region("r");struct _RegionHandle*r=& _tmp33D;_push_region(r);
{struct Cyc_List_List*_tmp33E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp33F=Cyc_Toc_add_tuple_type(_tmp33E);
# 1647
struct _tuple1*_tmp340=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp341=Cyc_Absyn_var_exp(_tmp340,0U);
struct Cyc_Absyn_Stmt*_tmp342=Cyc_Absyn_exp_stmt(_tmp341,0U);
# 1651
struct Cyc_Absyn_Exp*(*_tmp343)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1653
int is_atomic=1;
struct Cyc_List_List*_tmp344=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp344);for(0;_tmp344 != 0;(_tmp344=_tmp344->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp344->hd;
struct Cyc_Absyn_Exp*lval=({struct Cyc_Absyn_Exp*(*_tmpA15)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=_tmp343;struct Cyc_Absyn_Exp*_tmpA14=_tmp341;_tmpA15(_tmpA14,Cyc_Absyn_fieldname(i),0U);});
({int _tmpA16=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));is_atomic=_tmpA16;});{
void*_tmp345=e->r;void*_tmp346=_tmp345;struct Cyc_Absyn_Vardecl*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;int _tmp348;struct Cyc_List_List*_tmp347;switch(*((int*)_tmp346)){case 26U: _LL1: _tmp347=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp346)->f1;_LL2:
# 1661
({struct Cyc_Absyn_Stmt*_tmpA17=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp347,_tmp342);_tmp342=_tmpA17;});
goto _LL0;case 27U: _LL3: _tmp34B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f1;_tmp34A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f2;_tmp349=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f3;_tmp348=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f4;_LL4:
# 1664
({struct Cyc_Absyn_Stmt*_tmpA18=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp34B,_tmp34A,_tmp349,_tmp348,_tmp342,0);_tmp342=_tmpA18;});
# 1666
goto _LL0;case 28U: _LL5: _LL6:
# 1668
 goto _LL0;default: _LL7: _LL8:
# 1671
 Cyc_Toc_exp_to_c(nv,e);
({struct Cyc_Absyn_Stmt*_tmpA1D=({struct Cyc_Absyn_Stmt*_tmpA1C=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA1B=({struct Cyc_Absyn_Exp*(*_tmpA1A)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=_tmp343;struct Cyc_Absyn_Exp*_tmpA19=_tmp341;_tmpA1A(_tmpA19,Cyc_Absyn_fieldname(i),0U);});Cyc_Absyn_assign_exp(_tmpA1B,e,0U);}),0U);Cyc_Absyn_seq_stmt(_tmpA1C,_tmp342,0U);});_tmp342=_tmpA1D;});
# 1674
goto _LL0;}_LL0:;};}}{
# 1677
struct Cyc_Absyn_Exp*_tmp34C=({struct Cyc_Toc_Env*_tmpA23=nv;struct Cyc_Absyn_Exp*_tmpA22=Cyc_Absyn_var_exp(_tmp340,0U);void*_tmpA21=_tmp33F;struct Cyc_Absyn_Stmt*_tmpA20=_tmp342;int _tmpA1F=pointer;struct Cyc_Absyn_Exp*_tmpA1E=rgnopt;Cyc_Toc_make_struct(_tmpA23,_tmpA22,_tmpA21,_tmpA20,_tmpA1F,_tmpA1E,is_atomic,1);});_npop_handler(0U);return _tmp34C;};}
# 1644
;_pop_region(r);}
# 1681
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp34D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp34D != 0;_tmp34D=_tmp34D->tl){
struct Cyc_Absyn_Aggrfield*_tmp34E=(struct Cyc_Absyn_Aggrfield*)_tmp34D->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp34E->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1688
({void*_tmp34F=0U;({struct _dyneither_ptr _tmpA25=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp352;_tmp352.tag=0U;_tmp352.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp350[1U]={& _tmp352};({struct _dyneither_ptr _tmpA24=({const char*_tmp351="get_member_offset %s failed";_tag_dyneither(_tmp351,sizeof(char),28U);});Cyc_aprintf(_tmpA24,_tag_dyneither(_tmp350,sizeof(void*),1U));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA25,_tag_dyneither(_tmp34F,sizeof(void*),0U));});});}
# 1691
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp353=Cyc_Tcutil_compress(*typ);void*_tmp354=_tmp353;void*_tmp35C;struct Cyc_Absyn_Tqual _tmp35B;struct Cyc_Absyn_Exp*_tmp35A;union Cyc_Absyn_Constraint*_tmp359;unsigned int _tmp358;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->tag == 8U){_LL1: _tmp35C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).elt_type;_tmp35B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).tq;_tmp35A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).num_elts;_tmp359=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).zero_term;_tmp358=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).zt_loc;_LL2:
# 1696
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp35A))){
({void*_tmpA29=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp355=_cycalloc(sizeof(*_tmp355));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpA28=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp356;_tmp356.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmpA27=({struct Cyc_Absyn_ArrayInfo _tmp357;_tmp357.elt_type=_tmp35C;_tmp357.tq=_tmp35B;({struct Cyc_Absyn_Exp*_tmpA26=Cyc_Absyn_uint_exp(0U,0U);_tmp357.num_elts=_tmpA26;});_tmp357.zero_term=_tmp359;_tmp357.zt_loc=_tmp358;_tmp357;});_tmp356.f1=_tmpA27;});_tmp356;});_tmp355[0]=_tmpA28;});_tmp355;});*typ=_tmpA29;});
return 1;}
# 1700
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1703
return 0;}
# 1706
static int Cyc_Toc_is_array_type(void*t){
void*_tmp35D=Cyc_Tcutil_compress(t);void*_tmp35E=_tmp35D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35E)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1713
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1715
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1719
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp35F=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp360=_tmp35F->attributes;
void*_tmp361=_tmp35F->type;
if((!Cyc_Toc_is_array_type(_tmp361),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp361))))
({struct Cyc_List_List*_tmpA2C=({struct Cyc_List_List*_tmp362=_cycalloc(sizeof(*_tmp362));({void*_tmpA2B=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp363=_cycalloc(sizeof(*_tmp363));({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA2A=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp364;_tmp364.tag=6U;_tmp364.f1=0;_tmp364;});_tmp363[0]=_tmpA2A;});_tmp363;});_tmp362->hd=_tmpA2B;});_tmp362->tl=_tmp360;_tmp362;});_tmp360=_tmpA2C;});{
struct Cyc_Absyn_Aggrfield*_tmp365=({struct Cyc_Absyn_Aggrfield*_tmp367=_cycalloc(sizeof(*_tmp367));_tmp367->name=_tmp35F->name;_tmp367->tq=Cyc_Toc_mt_tq;_tmp367->type=new_field_type;_tmp367->width=_tmp35F->width;_tmp367->attributes=_tmp360;_tmp367->requires_clause=0;_tmp367;});
# 1731
({struct Cyc_List_List*_tmpA2D=({struct Cyc_List_List*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->hd=_tmp365;_tmp366->tl=new_fields;_tmp366;});new_fields=_tmpA2D;});};}else{
# 1733
({struct Cyc_List_List*_tmpA2E=({struct Cyc_List_List*_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp368->tl=new_fields;_tmp368;});new_fields=_tmpA2E;});}}{
# 1735
struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp36E=_cycalloc(sizeof(*_tmp36E));({struct _dyneither_ptr _tmpA30=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp371;_tmp371.tag=1U;_tmp371.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp36F[1U]={& _tmp371};({struct _dyneither_ptr _tmpA2F=({const char*_tmp370="_genStruct%d";_tag_dyneither(_tmp370,sizeof(char),13U);});Cyc_aprintf(_tmpA2F,_tag_dyneither(_tmp36F,sizeof(void*),1U));});});});_tmp36E[0]=_tmpA30;});_tmp36E;});
# 1738
struct _tuple1*qv=({struct _tuple1*_tmp36D=_cycalloc(sizeof(*_tmp36D));({union Cyc_Absyn_Nmspace _tmpA31=Cyc_Absyn_Abs_n(0,1);_tmp36D->f1=_tmpA31;});_tmp36D->f2=name;_tmp36D;});
struct Cyc_Absyn_AggrdeclImpl*_tmp36A=({struct Cyc_Absyn_AggrdeclImpl*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;_tmp36C->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;_tmp36C->fields=_tmp369;_tmp36C->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;_tmp36C;});
# 1743
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->kind=ad->kind;_tmp36B->sc=ad->sc;_tmp36B->name=qv;_tmp36B->tvs=ad->tvs;_tmp36B->impl=_tmp36A;_tmp36B->expected_mem_kind=0;_tmp36B->attributes=ad->attributes;_tmp36B;});
# 1750
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1754
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1760
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp372=Cyc_Toc_temp_var();
({struct Cyc_Absyn_Exp*_tmpA32=Cyc_Absyn_var_exp(_tmp372,0U);xexp=_tmpA32;});
do_declare=1;}else{
# 1768
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
({struct Cyc_Toc_Env*_tmpA33=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);nv=_tmpA33;});}{
# 1771
struct Cyc_Absyn_Stmt*_tmp373=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0U);
# 1775
struct Cyc_Absyn_Exp**varexp;
{void*_tmp374=_tmp373->r;void*_tmp375=_tmp374;struct Cyc_Absyn_Exp**_tmp378;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp375)->tag == 1U){_LL1: _tmp378=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp375)->f1;_LL2:
 varexp=_tmp378;goto _LL0;}else{_LL3: _LL4:
({void*_tmp376=0U;({struct _dyneither_ptr _tmpA34=({const char*_tmp377="stmt not an expression!";_tag_dyneither(_tmp377,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA34,_tag_dyneither(_tmp376,sizeof(void*),0U));});});}_LL0:;}{
# 1781
struct Cyc_Absyn_Exp*(*_tmp379)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1785
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp37A=Cyc_Tcutil_compress(struct_type);void*_tmp37B=_tmp37A;union Cyc_Absyn_AggrInfoU _tmp37F;struct Cyc_List_List*_tmp37E;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37B)->tag == 11U){_LL6: _tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37B)->f1).aggr_info;_tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37B)->f1).targs;_LL7:
# 1792
({struct Cyc_Absyn_Aggrdecl*_tmpA35=Cyc_Absyn_get_known_aggrdecl(_tmp37F);ad=_tmpA35;});
({struct Cyc_List_List*_tmpA36=aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;orig_aggrfields=_tmpA36;});
forall_types=_tmp37E;
goto _LL5;}else{_LL8: _LL9:
({void*_tmp37C=0U;({struct _dyneither_ptr _tmpA37=({const char*_tmp37D="init_struct: bad struct type";_tag_dyneither(_tmp37D,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA37,_tag_dyneither(_tmp37C,sizeof(void*),0U));});});}_LL5:;}
# 1799
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp380=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;((struct Cyc_List_List*)_check_null(_tmp380))->tl != 0;_tmp380=_tmp380->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp381=(struct Cyc_Absyn_Aggrfield*)_tmp380->hd;
struct _RegionHandle _tmp382=_new_region("temp");struct _RegionHandle*temp=& _tmp382;_push_region(temp);
{struct Cyc_List_List*_tmp383=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1808
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp383,_tmp381->type))){
struct Cyc_List_List*_tmp384=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp385=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp383,_tmp384);
# 1812
struct Cyc_List_List*new_fields=0;
for(_tmp380=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp380 != 0;_tmp380=_tmp380->tl){
struct Cyc_Absyn_Aggrfield*_tmp386=(struct Cyc_Absyn_Aggrfield*)_tmp380->hd;
struct Cyc_Absyn_Aggrfield*_tmp387=({struct Cyc_Absyn_Aggrfield*_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A->name=_tmp386->name;_tmp38A->tq=Cyc_Toc_mt_tq;({void*_tmpA38=
# 1817
Cyc_Tcutil_rsubstitute(temp,_tmp385,_tmp386->type);_tmp38A->type=_tmpA38;});_tmp38A->width=_tmp386->width;_tmp38A->attributes=_tmp386->attributes;_tmp38A->requires_clause=0;_tmp38A;});
# 1825
if(_tmp380->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp387->type))
({struct _dyneither_ptr**_tmpA39=({struct _dyneither_ptr**_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388[0]=_tmp387->name;_tmp388;});index=_tmpA39;});}
# 1832
({struct Cyc_List_List*_tmpA3A=({struct Cyc_List_List*_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389->hd=_tmp387;_tmp389->tl=new_fields;_tmp389;});new_fields=_tmpA3A;});}
# 1834
exist_types=0;
({struct Cyc_List_List*_tmpA3B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);aggrfields=_tmpA3B;});{
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp393=_cycalloc(sizeof(*_tmp393));({struct _dyneither_ptr _tmpA3D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp396;_tmp396.tag=1U;_tmp396.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp394[1U]={& _tmp396};({struct _dyneither_ptr _tmpA3C=({const char*_tmp395="_genStruct%d";_tag_dyneither(_tmp395,sizeof(char),13U);});Cyc_aprintf(_tmpA3C,_tag_dyneither(_tmp394,sizeof(void*),1U));});});});_tmp393[0]=_tmpA3D;});_tmp393;});
# 1838
struct _tuple1*qv=({struct _tuple1*_tmp392=_cycalloc(sizeof(*_tmp392));({union Cyc_Absyn_Nmspace _tmpA3E=Cyc_Absyn_Abs_n(0,1);_tmp392->f1=_tmpA3E;});_tmp392->f2=name;_tmp392;});
struct Cyc_Absyn_AggrdeclImpl*_tmp38B=({struct Cyc_Absyn_AggrdeclImpl*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->exist_vars=0;_tmp391->rgn_po=0;_tmp391->fields=aggrfields;_tmp391->tagged=0;_tmp391;});
# 1843
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390->kind=Cyc_Absyn_StructA;_tmp390->sc=Cyc_Absyn_Public;_tmp390->name=qv;_tmp390->tvs=0;_tmp390->expected_mem_kind=0;_tmp390->impl=_tmp38B;_tmp390->attributes=0;_tmp390;});
# 1850
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
({void*_tmpA42=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp38C=_cycalloc(sizeof(*_tmp38C));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpA41=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp38D;_tmp38D.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpA40=({struct Cyc_Absyn_AggrInfo _tmp38E;({union Cyc_Absyn_AggrInfoU _tmpA3F=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=new_ad;_tmp38F;}));_tmp38E.aggr_info=_tmpA3F;});_tmp38E.targs=0;_tmp38E;});_tmp38D.f1=_tmpA40;});_tmp38D;});_tmp38C[0]=_tmpA41;});_tmp38C;});struct_type=_tmpA42;});
# 1854
({void*_tmpA43=Cyc_Toc_typ_to_c(struct_type);aggr_typ=_tmpA43;});};}}
# 1804
;_pop_region(temp);};}{
# 1858
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp397=_new_region("r");struct _RegionHandle*r=& _tmp397;_push_region(r);
{struct Cyc_List_List*_tmp398=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp398 != 0;_tmp398=_tmp398->tl){
struct _tuple19*_tmp399=(struct _tuple19*)_tmp398->hd;struct _tuple19*_tmp39A=_tmp399;struct Cyc_List_List*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3BE;_LLB: _tmp3BF=_tmp39A->f1;_tmp3BE=_tmp39A->f2;_LLC:;
({int _tmpA44=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3BE->topt));is_atomic=_tmpA44;});
if(_tmp3BF == 0)
({void*_tmp39B=0U;({struct _dyneither_ptr _tmpA45=({const char*_tmp39C="empty designator list";_tag_dyneither(_tmp39C,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA45,_tag_dyneither(_tmp39B,sizeof(void*),0U));});});
if(_tmp3BF->tl != 0){
# 1868
struct _tuple1*_tmp39D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39E=Cyc_Absyn_var_exp(_tmp39D,0U);
for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
void*_tmp39F=(void*)_tmp3BF->hd;void*_tmp3A0=_tmp39F;struct _dyneither_ptr*_tmp3A3;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A0)->tag == 1U){_LLE: _tmp3A3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A0)->f1;_LLF:
# 1874
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3A3))
({struct Cyc_Absyn_Exp*_tmpA47=({void*_tmpA46=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmpA46,_tmp39E);});_tmp39E=_tmpA47;});
({struct Cyc_Absyn_Stmt*_tmpA4A=({struct Cyc_Absyn_Stmt*_tmpA49=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA48=_tmp379(xexp,_tmp3A3,0U);Cyc_Absyn_assign_exp(_tmpA48,_tmp39E,0U);}),0U);Cyc_Absyn_seq_stmt(_tmpA49,_tmp373,0U);});_tmp373=_tmpA4A;});
# 1878
goto _LLD;}else{_LL10: _LL11:
({void*_tmp3A1=0U;({struct _dyneither_ptr _tmpA4B=({const char*_tmp3A2="array designator in struct";_tag_dyneither(_tmp3A2,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA4B,_tag_dyneither(_tmp3A1,sizeof(void*),0U));});});}_LLD:;}
# 1882
Cyc_Toc_exp_to_c(nv,_tmp3BE);
({struct Cyc_Absyn_Stmt*_tmpA4D=({struct Cyc_Absyn_Stmt*_tmpA4C=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp39E,_tmp3BE,0U),0U);Cyc_Absyn_seq_stmt(_tmpA4C,_tmp373,0U);});_tmp373=_tmpA4D;});}else{
# 1885
void*_tmp3A4=(void*)_tmp3BF->hd;void*_tmp3A5=_tmp3A4;struct _dyneither_ptr*_tmp3BD;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A5)->tag == 1U){_LL13: _tmp3BD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A5)->f1;_LL14: {
# 1887
struct Cyc_Absyn_Exp*lval=_tmp379(xexp,_tmp3BD,0U);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3BD);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0U);
({struct Cyc_Absyn_Stmt*_tmpA4F=({struct Cyc_Absyn_Stmt*_tmpA4E=Cyc_Absyn_exp_stmt(assn_exp,0U);Cyc_Absyn_seq_stmt(_tmpA4E,_tmp373,0U);});_tmp373=_tmpA4F;});
({struct Cyc_Absyn_Exp*_tmpA50=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0U);lval=_tmpA50;});}{
# 1896
int e1_translated=0;
{void*_tmp3A6=_tmp3BE->r;void*_tmp3A7=_tmp3A6;void*_tmp3BA;struct Cyc_List_List*_tmp3B9;struct Cyc_Absyn_Exp*_tmp3B8;void*_tmp3B7;struct Cyc_Absyn_Vardecl*_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B4;int _tmp3B3;struct Cyc_List_List*_tmp3B2;switch(*((int*)_tmp3A7)){case 26U: _LL18: _tmp3B2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_LL19:
# 1899
({struct Cyc_Absyn_Stmt*_tmpA51=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3BD)))->type,lval,_tmp3B2,_tmp373);_tmp373=_tmpA51;});
# 1901
goto _LL17;case 27U: _LL1A: _tmp3B6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_tmp3B5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f2;_tmp3B4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f3;_tmp3B3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f4;_LL1B:
# 1903
({struct Cyc_Absyn_Stmt*_tmpA52=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3BD)))->type,lval,_tmp3B6,_tmp3B5,_tmp3B4,_tmp3B3,_tmp373,0);_tmp373=_tmpA52;});
# 1907
e1_translated=1;
_tmp3B8=_tmp3B5;_tmp3B7=(void*)_check_null(_tmp3B4->topt);goto _LL1D;case 28U: _LL1C: _tmp3B8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_tmp3B7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3A7)->f2;_LL1D:
# 1912
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3BD)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3B8);{
void*_tmp3A8=Cyc_Toc_typ_to_c(_tmp3B7);
({struct Cyc_Absyn_Exp*_tmpA57=({
struct Cyc_Absyn_Exp*_tmpA56=({struct Cyc_Absyn_Exp*_tmpA54=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmpA54,({struct Cyc_Absyn_Exp*_tmp3A9[2U];({struct Cyc_Absyn_Exp*_tmpA53=
Cyc_Absyn_sizeoftyp_exp(_tmp3A8,0U);_tmp3A9[1U]=_tmpA53;});_tmp3A9[0U]=_tmp3B8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A9,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1916
Cyc_Absyn_add_exp(_tmpA56,
# 1918
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3AA;({struct _tuple6 _tmpA55=({struct _tuple6 _tmp3AB;_tmp3AB.f1=Cyc_Absyn_Unsigned;_tmp3AB.f2=(int)sizeof(double);_tmp3AB;});(_tmp3AA.Int_c).val=_tmpA55;});(_tmp3AA.Int_c).tag=5;_tmp3AA;}),0U),0U);});
# 1915
*(nv->struct_info).varsizeexp=_tmpA57;});};}
# 1920
goto _LL17;case 30U: _LL1E: _tmp3BA=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_tmp3B9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3A7)->f2;_LL1F:
# 1922
({struct Cyc_Absyn_Stmt*_tmpA58=Cyc_Toc_init_anon_struct(nv,lval,_tmp3BA,_tmp3B9,_tmp373);_tmp373=_tmpA58;});goto _LL17;default: _LL20: _LL21: {
# 1924
void*old_e_typ=(void*)_check_null(_tmp3BE->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
({struct Cyc_Toc_Env*_tmpA59=Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv);Cyc_Toc_exp_to_c(_tmpA59,_tmp3BE);});
{void*_tmp3AC=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3AC)->tag == 11U){_LL23: _LL24:
# 1931
 if(old_e_typ != _tmp3BE->topt  && !({void*_tmpA5A=Cyc_Toc_typ_to_c(old_e_typ);Cyc_Tcutil_unify(_tmpA5A,(void*)_check_null(_tmp3BE->topt));})){
# 1935
({struct Cyc_Absyn_Aggrdecl*_tmpA5B=Cyc_Toc_update_aggr_type(ad,_tmp3BD,(void*)_check_null(_tmp3BE->topt));ad=_tmpA5B;});
({void*_tmpA5F=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpA5E=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3AE;_tmp3AE.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpA5D=({struct Cyc_Absyn_AggrInfo _tmp3AF;({union Cyc_Absyn_AggrInfoU _tmpA5C=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0[0]=ad;_tmp3B0;}));_tmp3AF.aggr_info=_tmpA5C;});_tmp3AF.targs=0;_tmp3AF;});_tmp3AE.f1=_tmpA5D;});_tmp3AE;});_tmp3AD[0]=_tmpA5E;});_tmp3AD;});
# 1936
struct_type=_tmpA5F;});
# 1939
({void*_tmpA60=Cyc_Toc_typ_to_c(struct_type);aggr_typ=_tmpA60;});}
# 1941
goto _LL22;}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 1944
struct Cyc_Absyn_Aggrfield*_tmp3B1=Cyc_Absyn_lookup_field(aggrfields,_tmp3BD);
# 1946
if(Cyc_Toc_is_poly_field(struct_type,_tmp3BD) && !was_ptr_type)
({struct Cyc_Absyn_Exp*_tmpA62=({void*_tmpA61=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmpA61,_tmp3BE);});_tmp3BE=_tmpA62;});
# 1949
if(exist_types != 0)
({struct Cyc_Absyn_Exp*_tmpA64=({void*_tmpA63=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3B1))->type);Cyc_Toc_cast_it(_tmpA63,_tmp3BE);});_tmp3BE=_tmpA64;});
# 1952
({struct Cyc_Absyn_Stmt*_tmpA66=({struct Cyc_Absyn_Stmt*_tmpA65=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3BE,0U),0U);Cyc_Absyn_seq_stmt(_tmpA65,_tmp373,0U);});_tmp373=_tmpA66;});
goto _LL17;};}}_LL17:;}
# 1955
goto _LL12;};}}else{_LL15: _LL16:
({void*_tmp3BB=0U;({struct _dyneither_ptr _tmpA67=({const char*_tmp3BC="array designator in struct";_tag_dyneither(_tmp3BC,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA67,_tag_dyneither(_tmp3BB,sizeof(void*),0U));});});}_LL12:;}}}
# 1962
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
({struct Cyc_Absyn_Exp*_tmpA68=Cyc_Toc_cast_it(cast_type,*varexp);*varexp=_tmpA68;});}{
# 1967
struct Cyc_Absyn_Exp*_tmp3C0=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp373,pointer,rgnopt,is_atomic,do_declare);
# 1969
({void*_tmpA69=pointer?Cyc_Absyn_cstar_typ(struct_type,Cyc_Toc_mt_tq): struct_type;_tmp3C0->topt=_tmpA69;});{
struct Cyc_Absyn_Exp*_tmp3C1=_tmp3C0;_npop_handler(0U);return _tmp3C1;};};
# 1860
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1975
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1978
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1981
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpA6A=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3C3;_tmp3C3.tag=5U;_tmp3C3.f1=e1;_tmp3C3.f2=incr;_tmp3C3;});_tmp3C2[0]=_tmpA6A;});_tmp3C2;}),0U);}
# 1985
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1994
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1996
void*_tmp3C4=e1->r;void*_tmp3C5=_tmp3C4;struct Cyc_Absyn_Exp*_tmp3CD;struct _dyneither_ptr*_tmp3CC;int _tmp3CB;int _tmp3CA;void*_tmp3C9;struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C7;switch(*((int*)_tmp3C5)){case 37U: _LL1: _tmp3C7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3C5)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp3C7,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp3C9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C5)->f1;_tmp3C8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C5)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp3C8,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp3CD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f1;_tmp3CC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f2;_tmp3CB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f3;_tmp3CA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f4;_LL6:
# 2001
 e1->r=_tmp3CD->r;
({struct Cyc_Absyn_Exp*_tmpA6D=e1;struct Cyc_List_List*_tmpA6C=({struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->hd=_tmp3CC;_tmp3C6->tl=fs;_tmp3C6;});struct Cyc_Absyn_Exp*(*_tmpA6B)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmpA6D,_tmpA6C,_tmpA6B,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 2009
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2011
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmpA6E=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmpA6E;});}
({void*_tmpA6F=(f(e1_copy,f_env))->r;e1->r=_tmpA6F;});
goto _LL0;}}_LL0:;}
# 2017
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2019
void*_tmp3CE=s->r;void*_tmp3CF=_tmp3CE;struct Cyc_Absyn_Stmt*_tmp3D6;struct Cyc_Absyn_Decl*_tmp3D5;struct Cyc_Absyn_Stmt*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D3;switch(*((int*)_tmp3CF)){case 1U: _LL1: _tmp3D3=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp3D3,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp3D5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f1;_tmp3D4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f2;_LL4:
# 2022
 Cyc_Toc_lvalue_assign_stmt(_tmp3D4,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp3D6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp3D6,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0U;({struct _dyneither_ptr _tmpA70=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp3D2.f1=_tmpA70;});({void*_tmp3D0[1U]={& _tmp3D2};({struct _dyneither_ptr _tmpA71=({const char*_tmp3D1="lvalue_assign_stmt: %s";_tag_dyneither(_tmp3D1,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmpA71,_tag_dyneither(_tmp3D0,sizeof(void*),1U));});});});}_LL0:;}
# 2028
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2032
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3D7=e->r;void*_tmp3D8=_tmp3D7;struct Cyc_Absyn_Stmt*_tmp3DF;struct Cyc_Absyn_Exp*_tmp3DE;void**_tmp3DD;struct Cyc_Absyn_Exp**_tmp3DC;switch(*((int*)_tmp3D8)){case 14U: _LL1: _tmp3DD=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D8)->f1;_tmp3DC=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D8)->f2;_LL2:
# 2035
({struct Cyc_Absyn_Exp*_tmpA72=Cyc_Toc_push_address_exp(*_tmp3DC);*_tmp3DC=_tmpA72;});
({void*_tmpA73=Cyc_Absyn_cstar_typ(*_tmp3DD,Cyc_Toc_mt_tq);*_tmp3DD=_tmpA73;});
return e;case 20U: _LL3: _tmp3DE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3D8)->f1;_LL4:
# 2039
 return _tmp3DE;case 37U: _LL5: _tmp3DF=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3D8)->f1;_LL6:
# 2043
 Cyc_Toc_push_address_stmt(_tmp3DF);
return e;default: _LL7: _LL8:
# 2046
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0U;({struct _dyneither_ptr _tmpA74=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp3DB.f1=_tmpA74;});({void*_tmp3D9[1U]={& _tmp3DB};({struct _dyneither_ptr _tmpA75=({const char*_tmp3DA="can't take & of exp %s";_tag_dyneither(_tmp3DA,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA75,_tag_dyneither(_tmp3D9,sizeof(void*),1U));});});});}_LL0:;}
# 2051
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3E0=s->r;void*_tmp3E1=_tmp3E0;struct Cyc_Absyn_Exp**_tmp3E7;struct Cyc_Absyn_Stmt*_tmp3E6;struct Cyc_Absyn_Stmt*_tmp3E5;switch(*((int*)_tmp3E1)){case 2U: _LL1: _tmp3E5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3E1)->f2;_LL2:
 _tmp3E6=_tmp3E5;goto _LL4;case 12U: _LL3: _tmp3E6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3E1)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp3E6);goto _LL0;case 1U: _LL5: _tmp3E7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3E1)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpA76=Cyc_Toc_push_address_exp(*_tmp3E7);*_tmp3E7=_tmpA76;});goto _LL0;default: _LL7: _LL8:
# 2057
({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0U;({struct _dyneither_ptr _tmpA77=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp3E4.f1=_tmpA77;});({void*_tmp3E2[1U]={& _tmp3E4};({struct _dyneither_ptr _tmpA78=({const char*_tmp3E3="can't take & of stmt %s";_tag_dyneither(_tmp3E3,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA78,_tag_dyneither(_tmp3E2,sizeof(void*),1U));});});});}_LL0:;}
# 2062
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2064
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2066
if(x == 0)return 0;
({struct Cyc_List_List*_tmpA7A=({struct Cyc_List_List*_tmp3E8=_region_malloc(r2,sizeof(*_tmp3E8));({void*_tmpA79=f(x->hd,env);_tmp3E8->hd=_tmpA79;});_tmp3E8->tl=0;_tmp3E8;});result=_tmpA7A;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
({struct Cyc_List_List*_tmpA7C=({struct Cyc_List_List*_tmp3E9=_region_malloc(r2,sizeof(*_tmp3E9));({void*_tmpA7B=f(x->hd,env);_tmp3E9->hd=_tmpA7B;});_tmp3E9->tl=0;_tmp3E9;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmpA7C;});
prev=prev->tl;}
# 2073
return result;}
# 2075
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2079
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->f1=0;_tmp3EA->f2=e;_tmp3EA;});}
# 2083
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3EB=Cyc_Tcutil_compress(t);void*_tmp3EC=_tmp3EB;struct Cyc_Absyn_PtrInfo _tmp3EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC)->tag == 5U){_LL1: _tmp3EF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC)->f1;_LL2:
 return _tmp3EF;}else{_LL3: _LL4:
({void*_tmp3ED=0U;({struct _dyneither_ptr _tmpA7D=({const char*_tmp3EE="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3EE,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA7D,_tag_dyneither(_tmp3ED,sizeof(void*),0U));});});}_LL0:;}
# 2093
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3F0=Cyc_Tcutil_compress(t);void*_tmp3F1=_tmp3F0;int _tmp3FD;enum Cyc_Absyn_Sign _tmp3FC;enum Cyc_Absyn_Sign _tmp3FB;enum Cyc_Absyn_Sign _tmp3FA;enum Cyc_Absyn_Sign _tmp3F9;enum Cyc_Absyn_Sign _tmp3F8;switch(*((int*)_tmp3F1)){case 5U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_Absyn_null_exp(0U);res=_tmpA7E;});goto _LL0;case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f2){case Cyc_Absyn_Char_sz: _LL3: _tmp3F8=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmpA7F=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3F8,'\000'),0U);res=_tmpA7F;});goto _LL0;case Cyc_Absyn_Short_sz: _LL5: _tmp3F9=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpA80=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3F9,0),0U);res=_tmpA80;});goto _LL0;case Cyc_Absyn_Int_sz: _LLB: _tmp3FA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LLC:
# 2101
 _tmp3FB=_tmp3FA;goto _LLE;case Cyc_Absyn_Long_sz: _LLD: _tmp3FB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LLE:
# 2103
({struct Cyc_Absyn_Exp*_tmpA81=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3FB,0),0U);res=_tmpA81;});goto _LL0;default: _LLF: _tmp3FC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL10:
# 2105
({struct Cyc_Absyn_Exp*_tmpA82=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3FC,(long long)0),0U);res=_tmpA82;});goto _LL0;}case 13U: _LL7: _LL8:
# 2099
 goto _LLA;case 14U: _LL9: _LLA:
 _tmp3FA=Cyc_Absyn_Unsigned;goto _LLC;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1)->f1){case 0U: _LL11: _LL12:
# 2106
({struct Cyc_Absyn_Exp*_tmpA83=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F2="0.0F";_tag_dyneither(_tmp3F2,sizeof(char),5U);}),0),0U);res=_tmpA83;});goto _LL0;case 1U: _LL13: _LL14:
({struct Cyc_Absyn_Exp*_tmpA84=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F3="0.0";_tag_dyneither(_tmp3F3,sizeof(char),4U);}),1),0U);res=_tmpA84;});goto _LL0;default: _LL15: _tmp3FD=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1)->f1;_LL16:
({struct Cyc_Absyn_Exp*_tmpA86=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmpA85=({const char*_tmp3F4="0.0L";_tag_dyneither(_tmp3F4,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmpA85,_tmp3FD);}),0U);res=_tmpA86;});goto _LL0;}default: _LL17: _LL18:
# 2110
({struct Cyc_String_pa_PrintArg_struct _tmp3F7;_tmp3F7.tag=0U;({struct _dyneither_ptr _tmpA87=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp3F7.f1=_tmpA87;});({void*_tmp3F5[1U]={& _tmp3F7};({struct _dyneither_ptr _tmpA88=({const char*_tmp3F6="found non-zero type %s in generate_zero";_tag_dyneither(_tmp3F6,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA88,_tag_dyneither(_tmp3F5,sizeof(void*),1U));});});});}_LL0:;}
# 2112
res->topt=t;
return res;}
# 2119
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2132 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp3FE=Cyc_Toc_typ_to_c(elt_type);
void*_tmp3FF=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp400=Cyc_Absyn_cstar_typ(_tmp3FE,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp401=({struct Cyc_Core_Opt*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->v=_tmp400;_tmp438;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp402=e1->r;void*_tmp403=_tmp402;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp407;switch(*((int*)_tmp403)){case 20U: _LL1: _tmp407=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_LL2:
# 2140
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmpA89=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp407,0,Cyc_Absyn_Other_coercion,0U);_tmp407=_tmpA89;});
_tmp407->topt=fat_ptr_type;}
# 2144
Cyc_Toc_exp_to_c(nv,_tmp407);xinit=_tmp407;goto _LL0;case 23U: _LL3: _tmp409=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_tmp408=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp403)->f2;_LL4:
# 2146
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmpA8A=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp409,0,Cyc_Absyn_Other_coercion,0U);_tmp409=_tmpA8A;});
_tmp409->topt=fat_ptr_type;}
# 2150
Cyc_Toc_exp_to_c(nv,_tmp409);Cyc_Toc_exp_to_c(nv,_tmp408);
({struct Cyc_Absyn_Exp*_tmpA8D=({struct Cyc_Absyn_Exp*_tmpA8C=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmpA8C,({struct Cyc_Absyn_Exp*_tmp404[3U];_tmp404[2U]=_tmp408;({struct Cyc_Absyn_Exp*_tmpA8B=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp404[1U]=_tmpA8B;});_tmp404[0U]=_tmp409;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp404,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2151
xinit=_tmpA8D;});
# 2153
goto _LL0;default: _LL5: _LL6:
({void*_tmp405=0U;({struct _dyneither_ptr _tmpA8E=({const char*_tmp406="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp406,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA8E,_tag_dyneither(_tmp405,sizeof(void*),0U));});});}_LL0:;}{
# 2156
struct _tuple1*_tmp40A=Cyc_Toc_temp_var();
struct _RegionHandle _tmp40B=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp40B;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp40C=({struct Cyc_Absyn_Vardecl*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->sc=Cyc_Absyn_Public;_tmp437->name=_tmp40A;_tmp437->varloc=0U;_tmp437->tq=Cyc_Toc_mt_tq;_tmp437->type=_tmp3FF;_tmp437->initializer=xinit;_tmp437->rgn=0;_tmp437->attributes=0;_tmp437->escapes=0;_tmp437;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp40D=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp435=_cycalloc(sizeof(*_tmp435));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpA8F=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp436;_tmp436.tag=4U;_tmp436.f1=_tmp40C;_tmp436;});_tmp435[0]=_tmpA8F;});_tmp435;});
struct Cyc_Absyn_Exp*_tmp40E=Cyc_Absyn_varb_exp((void*)_tmp40D,0U);
_tmp40E->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp40F=Cyc_Absyn_deref_exp(_tmp40E,0U);
_tmp40F->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp40F);{
struct _tuple1*_tmp410=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp411=({struct Cyc_Absyn_Vardecl*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->sc=Cyc_Absyn_Public;_tmp434->name=_tmp410;_tmp434->varloc=0U;_tmp434->tq=Cyc_Toc_mt_tq;_tmp434->type=_tmp3FE;_tmp434->initializer=_tmp40F;_tmp434->rgn=0;_tmp434->attributes=0;_tmp434->escapes=0;_tmp434;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp412=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp432=_cycalloc(sizeof(*_tmp432));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpA90=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp433;_tmp433.tag=4U;_tmp433.f1=_tmp411;_tmp433;});_tmp432[0]=_tmpA90;});_tmp432;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp413=Cyc_Absyn_varb_exp((void*)_tmp412,0U);
_tmp413->topt=_tmp40F->topt;
({struct Cyc_Absyn_Exp*_tmpA91=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp413,e2,0U);z_init=_tmpA91;});
z_init->topt=_tmp413->topt;}
# 2175
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp414=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp415=({struct Cyc_Absyn_Vardecl*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->sc=Cyc_Absyn_Public;_tmp431->name=_tmp414;_tmp431->varloc=0U;_tmp431->tq=Cyc_Toc_mt_tq;_tmp431->type=_tmp3FE;_tmp431->initializer=z_init;_tmp431->rgn=0;_tmp431->attributes=0;_tmp431->escapes=0;_tmp431;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp416=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42F=_cycalloc(sizeof(*_tmp42F));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpA92=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp430;_tmp430.tag=4U;_tmp430.f1=_tmp415;_tmp430;});_tmp42F[0]=_tmpA92;});_tmp42F;});
# 2181
struct Cyc_Absyn_Exp*_tmp417=Cyc_Absyn_varb_exp((void*)_tmp412,0U);_tmp417->topt=_tmp40F->topt;{
struct Cyc_Absyn_Exp*_tmp418=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp419=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp417,_tmp418,0U);
_tmp419->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp419);{
# 2187
struct Cyc_Absyn_Exp*_tmp41A=Cyc_Absyn_varb_exp((void*)_tmp416,0U);_tmp41A->topt=_tmp40F->topt;{
struct Cyc_Absyn_Exp*_tmp41B=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp41A,_tmp41B,0U);
_tmp41C->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp41C);{
# 2193
struct Cyc_List_List*_tmp41D=({struct Cyc_Absyn_Exp*_tmp42E[2U];({struct Cyc_Absyn_Exp*_tmpA93=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp42E[1U]=_tmpA93;});({struct Cyc_Absyn_Exp*_tmpA94=
# 2193
Cyc_Absyn_varb_exp((void*)_tmp40D,0U);_tmp42E[0U]=_tmpA94;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42E,sizeof(struct Cyc_Absyn_Exp*),2U));});
# 2195
struct Cyc_Absyn_Exp*_tmp41E=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmpA96=({struct Cyc_Absyn_Exp*_tmpA95=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp41D,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmpA95,_tmp41E,0U);});xsize=_tmpA96;});{
# 2200
struct Cyc_Absyn_Exp*_tmp41F=({struct Cyc_Absyn_Exp*_tmpA97=xsize;Cyc_Absyn_and_exp(_tmpA97,Cyc_Absyn_and_exp(_tmp419,_tmp41C,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp420=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp421=({struct Cyc_Absyn_Exp*_tmpA98=Cyc_Absyn_varb_exp((void*)_tmp40D,0U);Cyc_Toc_member_exp(_tmpA98,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmpA99=Cyc_Toc_cast_it(_tmp400,_tmp421);_tmp421=_tmpA99;});{
struct Cyc_Absyn_Exp*_tmp422=Cyc_Absyn_deref_exp(_tmp421,0U);
struct Cyc_Absyn_Exp*_tmp423=({struct Cyc_Absyn_Exp*_tmpA9A=_tmp422;Cyc_Absyn_assign_exp(_tmpA9A,Cyc_Absyn_var_exp(_tmp414,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp424=Cyc_Absyn_exp_stmt(_tmp423,0U);
({struct Cyc_Absyn_Stmt*_tmpA9E=({struct Cyc_Absyn_Stmt*_tmpA9D=({struct Cyc_Absyn_Exp*_tmpA9C=_tmp41F;struct Cyc_Absyn_Stmt*_tmpA9B=_tmp420;Cyc_Absyn_ifthenelse_stmt(_tmpA9C,_tmpA9B,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmpA9D,_tmp424,0U);});_tmp424=_tmpA9E;});
({struct Cyc_Absyn_Stmt*_tmpAA2=({struct Cyc_Absyn_Decl*_tmpAA1=({struct Cyc_Absyn_Decl*_tmp425=_cycalloc(sizeof(*_tmp425));({void*_tmpAA0=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp426=_cycalloc(sizeof(*_tmp426));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpA9F=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp427;_tmp427.tag=0U;_tmp427.f1=_tmp415;_tmp427;});_tmp426[0]=_tmpA9F;});_tmp426;});_tmp425->r=_tmpAA0;});_tmp425->loc=0U;_tmp425;});Cyc_Absyn_decl_stmt(_tmpAA1,_tmp424,0U);});_tmp424=_tmpAA2;});
({struct Cyc_Absyn_Stmt*_tmpAA6=({struct Cyc_Absyn_Decl*_tmpAA5=({struct Cyc_Absyn_Decl*_tmp428=_cycalloc(sizeof(*_tmp428));({void*_tmpAA4=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp429=_cycalloc(sizeof(*_tmp429));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAA3=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp42A;_tmp42A.tag=0U;_tmp42A.f1=_tmp411;_tmp42A;});_tmp429[0]=_tmpAA3;});_tmp429;});_tmp428->r=_tmpAA4;});_tmp428->loc=0U;_tmp428;});Cyc_Absyn_decl_stmt(_tmpAA5,_tmp424,0U);});_tmp424=_tmpAA6;});
({struct Cyc_Absyn_Stmt*_tmpAAA=({struct Cyc_Absyn_Decl*_tmpAA9=({struct Cyc_Absyn_Decl*_tmp42B=_cycalloc(sizeof(*_tmp42B));({void*_tmpAA8=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAA7=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp42D;_tmp42D.tag=0U;_tmp42D.f1=_tmp40C;_tmp42D;});_tmp42C[0]=_tmpAA7;});_tmp42C;});_tmp42B->r=_tmpAA8;});_tmp42B->loc=0U;_tmp42B;});Cyc_Absyn_decl_stmt(_tmpAA9,_tmp424,0U);});_tmp424=_tmpAAA;});
({void*_tmpAAB=Cyc_Toc_stmt_exp_r(_tmp424);e->r=_tmpAAB;});};};};};};};};};};}
# 2158
;_pop_region(rgn2);};}
# 2226 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2230
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp439=Cyc_Tcutil_compress(aggrtype);void*_tmp43A=_tmp439;union Cyc_Absyn_AggrInfoU _tmp43F;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43A)->tag == 11U){_LL1: _tmp43F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43A)->f1).aggr_info;_LL2:
# 2233
({struct Cyc_Absyn_Aggrdecl*_tmpAAC=Cyc_Absyn_get_known_aggrdecl(_tmp43F);ad=_tmpAAC;});goto _LL0;}else{_LL3: _LL4:
({void*_tmp43B=0U;({struct _dyneither_ptr _tmpAAF=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp43E;_tmp43E.tag=0U;({struct _dyneither_ptr _tmpAAD=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(aggrtype));_tmp43E.f1=_tmpAAD;});({void*_tmp43C[1U]={& _tmp43E};({struct _dyneither_ptr _tmpAAE=({const char*_tmp43D="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp43D,sizeof(char),51U);});Cyc_aprintf(_tmpAAE,_tag_dyneither(_tmp43C,sizeof(void*),1U));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAAF,_tag_dyneither(_tmp43B,sizeof(void*),0U));});});}_LL0:;}{
# 2237
struct _tuple1*_tmp440=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp441=Cyc_Absyn_var_exp(_tmp440,0U);
struct Cyc_Absyn_Exp*_tmp442=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp443=Cyc_Absyn_aggrarrow_exp(_tmp441,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp444=Cyc_Absyn_neq_exp(_tmp443,_tmp442,0U);
struct Cyc_Absyn_Exp*_tmp445=Cyc_Absyn_aggrarrow_exp(_tmp441,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp446=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp445,0U),0U);
struct Cyc_Absyn_Stmt*_tmp447=({struct Cyc_Absyn_Exp*_tmpAB1=_tmp444;struct Cyc_Absyn_Stmt*_tmpAB0=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmpAB1,_tmpAB0,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp448=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp449=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp44A=({struct _tuple1*_tmpAB4=_tmp440;void*_tmpAB3=_tmp448;struct Cyc_Absyn_Exp*_tmpAB2=_tmp449;Cyc_Absyn_declare_stmt(_tmpAB4,_tmpAB3,_tmpAB2,Cyc_Absyn_seq_stmt(_tmp447,_tmp446,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp44A);}else{
# 2251
struct Cyc_Absyn_Exp*_tmp44B=({struct Cyc_Absyn_Exp*_tmpAB5=aggrproj(_tmp441,f,0U);Cyc_Toc_member_exp(_tmpAB5,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_neq_exp(_tmp44B,_tmp442,0U);
struct Cyc_Absyn_Exp*_tmp44D=({struct Cyc_Absyn_Exp*_tmpAB6=aggrproj(_tmp441,f,0U);Cyc_Toc_member_exp(_tmpAB6,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp44E=Cyc_Absyn_exp_stmt(_tmp44D,0U);
struct Cyc_Absyn_Stmt*_tmp44F=({struct Cyc_Absyn_Exp*_tmpAB8=_tmp44C;struct Cyc_Absyn_Stmt*_tmpAB7=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmpAB8,_tmpAB7,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp450=({struct _tuple1*_tmpABB=_tmp440;void*_tmpABA=e1_c_type;struct Cyc_Absyn_Exp*_tmpAB9=e1;Cyc_Absyn_declare_stmt(_tmpABB,_tmpABA,_tmpAB9,Cyc_Absyn_seq_stmt(_tmp44F,_tmp44E,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp450);}};}
# 2261
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2264
void*_tmp451=Cyc_Tcutil_compress(t);void*_tmp452=_tmp451;union Cyc_Absyn_AggrInfoU _tmp459;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp452)->tag == 11U){_LL1: _tmp459=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp452)->f1).aggr_info;_LL2: {
# 2266
struct Cyc_Absyn_Aggrdecl*_tmp453=Cyc_Absyn_get_known_aggrdecl(_tmp459);
({int _tmpABC=Cyc_Toc_get_member_offset(_tmp453,f);*f_tag=_tmpABC;});{
# 2269
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp458;_tmp458.tag=0U;_tmp458.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp457;_tmp457.tag=0U;_tmp457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp453->name).f2);({void*_tmp455[2U]={& _tmp457,& _tmp458};({struct _dyneither_ptr _tmpABD=({const char*_tmp456="_union_%s_%s";_tag_dyneither(_tmp456,sizeof(char),13U);});Cyc_aprintf(_tmpABD,_tag_dyneither(_tmp455,sizeof(void*),2U));});});});});
({void*_tmpABE=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454[0]=str;_tmp454;}));*tagged_member_type=_tmpABE;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp453->impl))->tagged;};}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2280
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2283
void*_tmp45A=e->r;void*_tmp45B=_tmp45A;struct Cyc_Absyn_Exp*_tmp467;struct _dyneither_ptr*_tmp466;int*_tmp465;struct Cyc_Absyn_Exp*_tmp464;struct _dyneither_ptr*_tmp463;int*_tmp462;struct Cyc_Absyn_Exp*_tmp461;switch(*((int*)_tmp45B)){case 14U: _LL1: _tmp461=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_LL2:
({void*_tmp45C=0U;({struct _dyneither_ptr _tmpABF=({const char*_tmp45D="cast on lhs!";_tag_dyneither(_tmp45D,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpABF,_tag_dyneither(_tmp45C,sizeof(void*),0U));});});case 21U: _LL3: _tmp464=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp463=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_tmp462=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45B)->f4;_LL4:
# 2286
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp464->topt),_tmp463,f_tag,tagged_member_type,clear_read,_tmp462);case 22U: _LL5: _tmp467=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp466=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_tmp465=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45B)->f4;_LL6: {
# 2289
void*_tmp45E=Cyc_Tcutil_compress((void*)_check_null(_tmp467->topt));void*_tmp45F=_tmp45E;void*_tmp460;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->tag == 5U){_LLA: _tmp460=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->f1).elt_typ;_LLB:
# 2291
 return Cyc_Toc_is_tagged_union_project_impl(_tmp460,_tmp466,f_tag,tagged_member_type,clear_read,_tmp465);}else{_LLC: _LLD:
# 2293
 return 0;}_LL9:;}default: _LL7: _LL8:
# 2295
 return 0;}_LL0:;}
# 2308 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2312
struct _tuple1*_tmp468=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp468,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmpAC0=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmpAC0;});else{
# 2322
struct Cyc_Absyn_Exp*_tmp469=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmpAC3=({struct Cyc_Absyn_Exp*_tmpAC2=_tmp469;struct Cyc_Absyn_Stmt*_tmpAC1=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmpAC2,_tmpAC1,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmpAC3;});}{
# 2325
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmpAC6=_tmp468;void*_tmpAC5=Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpAC4=
Cyc_Toc_push_address_exp(e1);
# 2325
Cyc_Absyn_declare_stmt(_tmpAC6,_tmpAC5,_tmpAC4,
# 2327
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2331
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2346 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp46A=Cyc_Absyn_var_exp(max,0U);
# 2354
struct Cyc_Absyn_Stmt*s;
if(zero_term)
({struct Cyc_Absyn_Exp*_tmpAC8=({struct Cyc_Absyn_Exp*_tmpAC7=_tmp46A;Cyc_Absyn_add_exp(_tmpAC7,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp46A=_tmpAC8;});
if(init_exp != 0)
({struct Cyc_Absyn_Stmt*_tmpAD0=({struct Cyc_Toc_Env*_tmpACF=nv;void*_tmpACE=new_e_type;struct Cyc_Absyn_Exp*_tmpACD=
Cyc_Absyn_var_exp(a,0U);
# 2358
struct Cyc_Absyn_Vardecl*_tmpACC=(struct Cyc_Absyn_Vardecl*)_check_null(vd);struct Cyc_Absyn_Exp*_tmpACB=
Cyc_Absyn_var_exp(max,0U);
# 2358
struct Cyc_Absyn_Exp*_tmpACA=init_exp;int _tmpAC9=zero_term;Cyc_Toc_init_comprehension(_tmpACF,_tmpACE,_tmpACD,_tmpACC,_tmpACB,_tmpACA,_tmpAC9,
# 2360
Cyc_Toc_skip_stmt_dl(),1);});
# 2358
s=_tmpAD0;});else{
# 2361
if(zero_term)
({struct Cyc_Absyn_Stmt*_tmpAD4=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAD3=({struct Cyc_Absyn_Exp*_tmpAD1=Cyc_Absyn_var_exp(a,0U);Cyc_Absyn_subscript_exp(_tmpAD1,Cyc_Absyn_var_exp(max,0U),0U);});Cyc_Absyn_assign_exp(_tmpAD3,({
void*_tmpAD2=elt_typ;Cyc_Toc_cast_it(_tmpAD2,Cyc_Absyn_uint_exp(0U,0U));}),0U);}),0U);
# 2362
s=_tmpAD4;});else{
# 2365
({struct Cyc_Absyn_Stmt*_tmpAD5=Cyc_Absyn_skip_stmt(0U);s=_tmpAD5;});}}{
# 2367
struct _RegionHandle _tmp46B=_new_region("r");struct _RegionHandle*r=& _tmp46B;_push_region(r);
{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp47F=_region_malloc(r,sizeof(*_tmp47F));({struct _tuple28*_tmpAD6=({struct _tuple28*_tmp480=_region_malloc(r,sizeof(*_tmp480));_tmp480->f1=max;_tmp480->f2=Cyc_Absyn_uint_typ;_tmp480->f3=e1;_tmp480;});_tmp47F->hd=_tmpAD6;});_tmp47F->tl=0;_tmp47F;});
# 2370
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmpADA=({void*_tmpAD9=old_elt_typ;Cyc_Toc_malloc_exp(_tmpAD9,({
struct Cyc_Absyn_Exp*_tmpAD8=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmpAD8,({struct Cyc_Absyn_Exp*_tmp46C[2U];_tmp46C[1U]=_tmp46A;({struct Cyc_Absyn_Exp*_tmpAD7=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp46C[0U]=_tmpAD7;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46C,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);}));});
# 2372
ai=_tmpADA;});else{
# 2377
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmpADE=({struct Cyc_Absyn_Exp*_tmpADD=r;Cyc_Toc_rmalloc_exp(_tmpADD,({struct Cyc_Absyn_Exp*_tmpADC=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmpADC,({struct Cyc_Absyn_Exp*_tmp46D[2U];_tmp46D[1U]=_tmp46A;({struct Cyc_Absyn_Exp*_tmpADB=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp46D[0U]=_tmpADB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46D,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);}));});
# 2379
ai=_tmpADE;});}{
# 2383
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
({struct Cyc_List_List*_tmpAE0=({struct Cyc_List_List*_tmp46E=_region_malloc(r,sizeof(*_tmp46E));({struct _tuple28*_tmpADF=({struct _tuple28*_tmp46F=_region_malloc(r,sizeof(*_tmp46F));_tmp46F->f1=a;_tmp46F->f2=ptr_typ;_tmp46F->f3=ainit;_tmp46F;});_tmp46E->hd=_tmpADF;});_tmp46E->tl=decls;_tmp46E;});decls=_tmpAE0;});
if(!Cyc_Tcutil_is_pointer_type(old_typ))
({void*_tmp470=0U;({struct _dyneither_ptr _tmpAE1=({const char*_tmp471="new_comprehension_to_c: comprehension is not an array type";_tag_dyneither(_tmp471,sizeof(char),59U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAE1,_tag_dyneither(_tmp470,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp472=Cyc_Toc_temp_var();
void*_tmp473=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp474=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp475=({
struct Cyc_Absyn_Exp*_tmpAE4=_tmp474;Cyc_Absyn_fncall_exp(_tmpAE4,({struct Cyc_Absyn_Exp*_tmp478[3U];_tmp478[2U]=_tmp46A;({struct Cyc_Absyn_Exp*_tmpAE2=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp478[1U]=_tmpAE2;});({struct Cyc_Absyn_Exp*_tmpAE3=Cyc_Absyn_var_exp(a,0U);_tmp478[0U]=_tmpAE3;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp478,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2395
({struct Cyc_List_List*_tmpAE6=({struct Cyc_List_List*_tmp476=_region_malloc(r,sizeof(*_tmp476));({struct _tuple28*_tmpAE5=({struct _tuple28*_tmp477=_region_malloc(r,sizeof(*_tmp477));_tmp477->f1=_tmp472;_tmp477->f2=_tmp473;_tmp477->f3=_tmp475;_tmp477;});_tmp476->hd=_tmpAE5;});_tmp476->tl=decls;_tmp476;});decls=_tmpAE6;});
({struct Cyc_Absyn_Stmt*_tmpAE8=({struct Cyc_Absyn_Stmt*_tmpAE7=s;Cyc_Absyn_seq_stmt(_tmpAE7,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp472,0U),0U),0U);});s=_tmpAE8;});}else{
# 2398
({struct Cyc_Absyn_Stmt*_tmpAEA=({struct Cyc_Absyn_Stmt*_tmpAE9=s;Cyc_Absyn_seq_stmt(_tmpAE9,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0U),0U),0U);});s=_tmpAEA;});}
{struct Cyc_List_List*_tmp479=decls;for(0;_tmp479 != 0;_tmp479=_tmp479->tl){
struct _tuple28 _tmp47A=*((struct _tuple28*)_tmp479->hd);struct _tuple28 _tmp47B=_tmp47A;struct _tuple1*_tmp47E;void*_tmp47D;struct Cyc_Absyn_Exp*_tmp47C;_LL1: _tmp47E=_tmp47B.f1;_tmp47D=_tmp47B.f2;_tmp47C=_tmp47B.f3;_LL2:;
({struct Cyc_Absyn_Stmt*_tmpAEB=Cyc_Absyn_declare_stmt(_tmp47E,_tmp47D,_tmp47C,s,0U);s=_tmpAEB;});}}
# 2403
({void*_tmpAEC=Cyc_Toc_stmt_exp_r(s);e->r=_tmpAEC;});};}
# 2368
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2407
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp481=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp484;_tmp484.tag=0U;({struct _dyneither_ptr _tmpAED=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp484.f1=_tmpAED;});({void*_tmp482[1U]={& _tmp484};({struct _dyneither_ptr _tmpAEE=({const char*_tmp483="exp_to_c: no type for %s";_tag_dyneither(_tmp483,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAEE,_tag_dyneither(_tmp482,sizeof(void*),1U));});});});
# 2413
if((nv->struct_info).lhs_exp != 0){
void*_tmp485=_tmp481;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp485)->tag == 29U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
({struct Cyc_Toc_Env*_tmpAEF=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);nv=_tmpAEF;});}_LL0:;}{
# 2421
void*old_typ=(void*)_check_null(e->topt);
void*_tmp486=_tmp481;struct Cyc_Absyn_Stmt*_tmp6AB;struct Cyc_Absyn_Exp*_tmp6AA;struct _dyneither_ptr*_tmp6A9;struct Cyc_Absyn_Exp*_tmp6A8;struct Cyc_Absyn_Exp*_tmp6A7;int _tmp6A6;struct Cyc_Absyn_Exp*_tmp6A5;void**_tmp6A4;struct Cyc_Absyn_Exp*_tmp6A3;int _tmp6A2;int _tmp6A1;struct Cyc_List_List*_tmp6A0;struct Cyc_Absyn_Datatypedecl*_tmp69F;struct Cyc_Absyn_Datatypefield*_tmp69E;void*_tmp69D;struct Cyc_List_List*_tmp69C;struct _tuple1*_tmp69B;struct Cyc_List_List*_tmp69A;struct Cyc_List_List*_tmp699;struct Cyc_Absyn_Aggrdecl*_tmp698;struct Cyc_Absyn_Exp*_tmp697;void*_tmp696;int _tmp695;struct Cyc_Absyn_Vardecl*_tmp694;struct Cyc_Absyn_Exp*_tmp693;struct Cyc_Absyn_Exp*_tmp692;int _tmp691;struct Cyc_List_List*_tmp690;struct Cyc_List_List*_tmp68F;struct Cyc_Absyn_Exp*_tmp68E;struct Cyc_Absyn_Exp*_tmp68D;struct Cyc_Absyn_Exp*_tmp68C;struct _dyneither_ptr*_tmp68B;int _tmp68A;int _tmp689;struct Cyc_Absyn_Exp*_tmp688;struct _dyneither_ptr*_tmp687;int _tmp686;int _tmp685;struct Cyc_Absyn_Exp*_tmp684;void*_tmp683;struct Cyc_List_List*_tmp682;void*_tmp681;struct Cyc_Absyn_Exp*_tmp680;struct Cyc_Absyn_Exp*_tmp67F;struct Cyc_Absyn_Exp*_tmp67E;struct Cyc_Absyn_Exp*_tmp67D;void**_tmp67C;struct Cyc_Absyn_Exp*_tmp67B;int _tmp67A;enum Cyc_Absyn_Coercion _tmp679;struct Cyc_Absyn_Exp*_tmp678;struct Cyc_List_List*_tmp677;struct Cyc_Absyn_Exp*_tmp676;struct Cyc_Absyn_Exp*_tmp675;int _tmp674;struct Cyc_Absyn_Exp*_tmp673;struct Cyc_List_List*_tmp672;int _tmp671;struct Cyc_List_List*_tmp670;struct Cyc_Absyn_VarargInfo*_tmp66F;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_List_List*_tmp66D;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp*_tmp66B;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_Absyn_Exp*_tmp669;struct Cyc_Absyn_Exp*_tmp668;struct Cyc_Absyn_Exp*_tmp667;struct Cyc_Absyn_Exp*_tmp666;struct Cyc_Absyn_Exp*_tmp665;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_Absyn_Exp*_tmp663;struct Cyc_Core_Opt*_tmp662;struct Cyc_Absyn_Exp*_tmp661;struct Cyc_Absyn_Exp*_tmp660;enum Cyc_Absyn_Incrementor _tmp65F;enum Cyc_Absyn_Primop _tmp65E;struct Cyc_List_List*_tmp65D;switch(*((int*)_tmp486)){case 2U: _LL6: _LL7:
# 2424
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL5;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp486)->f1).Null_c).tag == 1){_LL8: _LL9: {
# 2431
struct Cyc_Absyn_Exp*_tmp487=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmpAF0=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp487,_tmp487))->r;e->r=_tmpAF0;});else{
# 2436
({void*_tmpAF2=({struct Cyc_Absyn_Exp*_tmpAF1=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmpAF1,({struct Cyc_Absyn_Exp*_tmp488[3U];_tmp488[2U]=_tmp487;_tmp488[1U]=_tmp487;_tmp488[0U]=_tmp487;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp488,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmpAF2;});}}else{
# 2438
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2440
goto _LL5;}}else{_LLA: _LLB:
 goto _LL5;}case 1U: _LLC: _LLD:
 goto _LL5;case 3U: _LLE: _tmp65E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp65D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LLF: {
# 2445
struct Cyc_List_List*_tmp489=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp65D);
# 2447
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp65D);
{enum Cyc_Absyn_Primop _tmp48A=_tmp65E;switch(_tmp48A){case Cyc_Absyn_Numelts: _LL5D: _LL5E: {
# 2450
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65D))->hd;
{void*_tmp48B=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp48C=_tmp48B;void*_tmp49E;union Cyc_Absyn_Constraint*_tmp49D;union Cyc_Absyn_Constraint*_tmp49C;union Cyc_Absyn_Constraint*_tmp49B;struct Cyc_Absyn_Exp*_tmp49A;switch(*((int*)_tmp48C)){case 8U: _LL72: _tmp49A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48C)->f1).num_elts;_LL73:
# 2454
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp49A)))
({void*_tmp48D=0U;({unsigned int _tmpAF4=e->loc;struct _dyneither_ptr _tmpAF3=({const char*_tmp48E="can't calculate numelts";_tag_dyneither(_tmp48E,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpAF4,_tmpAF3,_tag_dyneither(_tmp48D,sizeof(void*),0U));});});
e->r=_tmp49A->r;goto _LL71;case 5U: _LL74: _tmp49E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).elt_typ;_tmp49D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).ptr_atts).nullable;_tmp49C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).ptr_atts).bounds;_tmp49B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).ptr_atts).zero_term;_LL75:
# 2458
{void*_tmp48F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp49C);void*_tmp490=_tmp48F;struct Cyc_Absyn_Exp*_tmp495;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp490)->tag == 0U){_LL79: _LL7A:
# 2460
({void*_tmpAF7=({struct Cyc_Absyn_Exp*_tmpAF6=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmpAF6,({struct Cyc_Absyn_Exp*_tmp491[2U];({struct Cyc_Absyn_Exp*_tmpAF5=
# 2462
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp49E),0U);_tmp491[1U]=_tmpAF5;});_tmp491[0U]=(struct Cyc_Absyn_Exp*)_tmp65D->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp491,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2460
e->r=_tmpAF7;});
# 2463
goto _LL78;}else{_LL7B: _tmp495=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp490)->f1;_LL7C:
# 2466
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp49B)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp65D->hd);
# 2470
({void*_tmpAF9=({struct Cyc_Absyn_Exp*_tmpAF8=function_e;Cyc_Toc_fncall_exp_r(_tmpAF8,({struct Cyc_Absyn_Exp*_tmp492[2U];_tmp492[1U]=_tmp495;_tmp492[0U]=(struct Cyc_Absyn_Exp*)_tmp65D->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp492,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmpAF9;});}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp49D)){
if(!Cyc_Evexp_c_can_eval(_tmp495))
({void*_tmp493=0U;({unsigned int _tmpAFB=e->loc;struct _dyneither_ptr _tmpAFA=({const char*_tmp494="can't calculate numelts";_tag_dyneither(_tmp494,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpAFB,_tmpAFA,_tag_dyneither(_tmp493,sizeof(void*),0U));});});
# 2475
({void*_tmpAFE=({struct Cyc_Absyn_Exp*_tmpAFD=arg;struct Cyc_Absyn_Exp*_tmpAFC=_tmp495;Cyc_Toc_conditional_exp_r(_tmpAFD,_tmpAFC,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmpAFE;});}else{
# 2477
e->r=_tmp495->r;goto _LL78;}}
# 2479
goto _LL78;}_LL78:;}
# 2481
goto _LL71;default: _LL76: _LL77:
# 2483
({struct Cyc_String_pa_PrintArg_struct _tmp499;_tmp499.tag=0U;({struct _dyneither_ptr _tmpAFF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp499.f1=_tmpAFF;});({struct Cyc_String_pa_PrintArg_struct _tmp498;_tmp498.tag=0U;({struct _dyneither_ptr _tmpB00=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp498.f1=_tmpB00;});({void*_tmp496[2U]={& _tmp498,& _tmp499};({struct _dyneither_ptr _tmpB01=({const char*_tmp497="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp497,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB01,_tag_dyneither(_tmp496,sizeof(void*),2U));});});});});}_LL71:;}
# 2486
goto _LL5C;}case Cyc_Absyn_Plus: _LL5F: _LL60:
# 2491
{void*_tmp49F=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp489))->hd);void*_tmp4A0=_tmp49F;void*_tmp4A8;union Cyc_Absyn_Constraint*_tmp4A7;union Cyc_Absyn_Constraint*_tmp4A6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->tag == 5U){_LL7E: _tmp4A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).elt_typ;_tmp4A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).ptr_atts).bounds;_tmp4A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).ptr_atts).zero_term;_LL7F:
# 2493
{void*_tmp4A1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4A7);void*_tmp4A2=_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A5;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4A2)->tag == 0U){_LL83: _LL84: {
# 2495
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65D->tl))->hd;
({void*_tmpB04=({struct Cyc_Absyn_Exp*_tmpB03=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmpB03,({struct Cyc_Absyn_Exp*_tmp4A3[3U];_tmp4A3[2U]=e2;({struct Cyc_Absyn_Exp*_tmpB02=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4A8),0U);_tmp4A3[1U]=_tmpB02;});_tmp4A3[0U]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A3,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2497
e->r=_tmpB04;});
# 2499
goto _LL82;}}else{_LL85: _tmp4A5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4A2)->f1;_LL86:
# 2501
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4A6)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65D->tl))->hd;
({void*_tmpB06=({struct Cyc_Absyn_Exp*_tmpB05=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmpB05,({struct Cyc_Absyn_Exp*_tmp4A4[3U];_tmp4A4[2U]=e2;_tmp4A4[1U]=_tmp4A5;_tmp4A4[0U]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A4,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmpB06;});}
# 2506
goto _LL82;}_LL82:;}
# 2508
goto _LL7D;}else{_LL80: _LL81:
# 2510
 goto _LL7D;}_LL7D:;}
# 2512
goto _LL5C;case Cyc_Absyn_Minus: _LL61: _LL62: {
# 2517
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp489))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65D->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp489->tl))->hd)){
({void*_tmpB08=({struct Cyc_Absyn_Exp*_tmpB07=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmpB07,Cyc_Toc_curr_sp);});e1->r=_tmpB08;});
({void*_tmpB0A=({struct Cyc_Absyn_Exp*_tmpB09=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmpB09,Cyc_Toc_curr_sp);});e2->r=_tmpB0A;});
({void*_tmpB0C=({void*_tmpB0B=Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_typ,Cyc_Toc_mt_tq);e2->topt=_tmpB0B;});e1->topt=_tmpB0C;});
({void*_tmpB0E=({struct Cyc_Absyn_Exp*_tmpB0D=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmpB0D,
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U),0U);})->r;
# 2525
e->r=_tmpB0E;});}else{
# 2528
({void*_tmpB12=({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmpB11,({struct Cyc_Absyn_Exp*_tmp4A9[3U];({struct Cyc_Absyn_Exp*_tmpB0F=
# 2530
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp4A9[2U]=_tmpB0F;});({struct Cyc_Absyn_Exp*_tmpB10=
# 2529
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp4A9[1U]=_tmpB10;});_tmp4A9[0U]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A9,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2528
e->r=_tmpB12;});}}
# 2533
goto _LL5C;}case Cyc_Absyn_Eq: _LL63: _LL64:
 goto _LL66;case Cyc_Absyn_Neq: _LL65: _LL66:
 goto _LL68;case Cyc_Absyn_Gt: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Gte: _LL69: _LL6A:
 goto _LL6C;case Cyc_Absyn_Lt: _LL6B: _LL6C:
 goto _LL6E;case Cyc_Absyn_Lte: _LL6D: _LL6E: {
# 2541
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65D->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp489))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp489->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ)){
void*_tmp4AA=({void*_tmpB13=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmpB13,Cyc_Toc_mt_tq);});
({void*_tmpB16=({void*_tmpB15=_tmp4AA;Cyc_Toc_cast_it_r(_tmpB15,({struct Cyc_Absyn_Exp*_tmpB14=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmpB14,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmpB16;});
e1->topt=_tmp4AA;}
# 2551
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
void*_tmp4AB=({void*_tmpB17=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmpB17,Cyc_Toc_mt_tq);});
({void*_tmpB1A=({void*_tmpB19=_tmp4AB;Cyc_Toc_cast_it_r(_tmpB19,({struct Cyc_Absyn_Exp*_tmpB18=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmpB18,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmpB1A;});
e2->topt=_tmp4AB;}
# 2556
goto _LL5C;}default: _LL6F: _LL70:
 goto _LL5C;}_LL5C:;}
# 2559
goto _LL5;}case 5U: _LL10: _tmp660=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp65F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL11: {
# 2561
void*e2_cyc_typ=(void*)_check_null(_tmp660->topt);
# 2570 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp4C7="increment";_tag_dyneither(_tmp4C7,sizeof(char),10U);});
if(_tmp65F == Cyc_Absyn_PreDec  || _tmp65F == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmpB1B=({const char*_tmp4AC="decrement";_tag_dyneither(_tmp4AC,sizeof(char),10U);});incr_str=_tmpB1B;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp660,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp4AF;_tmp4AF.tag=0U;_tmp4AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4AD[1U]={& _tmp4AF};({unsigned int _tmpB1D=e->loc;struct _dyneither_ptr _tmpB1C=({const char*_tmp4AE="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp4AE,sizeof(char),74U);});Cyc_Tcutil_terr(_tmpB1D,_tmpB1C,_tag_dyneither(_tmp4AD,sizeof(void*),1U));});});});
({void*_tmp4B0=0U;({struct _dyneither_ptr _tmpB1E=({const char*_tmp4B1="in-place inc/dec on zero-term";_tag_dyneither(_tmp4B1,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB1E,_tag_dyneither(_tmp4B0,sizeof(void*),0U));});});}{
# 2579
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp660,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_signed_int_exp(1,0U);
_tmp4B2->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4B3=_tmp65F;switch(_tmp4B3){case Cyc_Absyn_PreInc: _LL88: _LL89:
# 2586
({void*_tmpB21=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB20=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4B5;_tmp4B5.tag=4U;_tmp4B5.f1=_tmp660;({struct Cyc_Core_Opt*_tmpB1F=({struct Cyc_Core_Opt*_tmp4B6=_cycalloc_atomic(sizeof(*_tmp4B6));_tmp4B6->v=(void*)Cyc_Absyn_Plus;_tmp4B6;});_tmp4B5.f2=_tmpB1F;});_tmp4B5.f3=_tmp4B2;_tmp4B5;});_tmp4B4[0]=_tmpB20;});_tmp4B4;});e->r=_tmpB21;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL8A: _LL8B:
# 2590
({void*_tmpB24=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB23=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4B8;_tmp4B8.tag=4U;_tmp4B8.f1=_tmp660;({struct Cyc_Core_Opt*_tmpB22=({struct Cyc_Core_Opt*_tmp4B9=_cycalloc_atomic(sizeof(*_tmp4B9));_tmp4B9->v=(void*)Cyc_Absyn_Minus;_tmp4B9;});_tmp4B8.f2=_tmpB22;});_tmp4B8.f3=_tmp4B2;_tmp4B8;});_tmp4B7[0]=_tmpB23;});_tmp4B7;});e->r=_tmpB24;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL8C: _LL8D:
# 2594
({struct Cyc_String_pa_PrintArg_struct _tmp4BC;_tmp4BC.tag=0U;_tmp4BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4BA[1U]={& _tmp4BC};({unsigned int _tmpB26=e->loc;struct _dyneither_ptr _tmpB25=({const char*_tmp4BB="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp4BB,sizeof(char),58U);});Cyc_Tcutil_terr(_tmpB26,_tmpB25,_tag_dyneither(_tmp4BA,sizeof(void*),1U));});});});
# 2596
({void*_tmp4BD=0U;({struct _dyneither_ptr _tmpB27=({const char*_tmp4BE="in-place inc/dec on @tagged union";_tag_dyneither(_tmp4BE,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB27,_tag_dyneither(_tmp4BD,sizeof(void*),0U));});});}_LL87:;};}{
# 2599
int need_null_ck=(Cyc_Tcutil_is_pointer_type(old_typ) && 
Cyc_Tcutil_is_nullable_pointer_type(old_typ)) && Cyc_Toc_need_null_check(_tmp660);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp660);
Cyc_Toc_set_lhs(nv,0);{
# 2606
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp65F == Cyc_Absyn_PostInc  || _tmp65F == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2612
if(_tmp65F == Cyc_Absyn_PreDec  || _tmp65F == Cyc_Absyn_PostDec)
change=-1;
({void*_tmpB2C=({struct Cyc_Absyn_Exp*_tmpB2B=fn_e;Cyc_Toc_fncall_exp_r(_tmpB2B,({struct Cyc_Absyn_Exp*_tmp4BF[3U];({struct Cyc_Absyn_Exp*_tmpB28=
# 2616
Cyc_Absyn_signed_int_exp(change,0U);_tmp4BF[2U]=_tmpB28;});({struct Cyc_Absyn_Exp*_tmpB29=
# 2615
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp4BF[1U]=_tmpB29;});({struct Cyc_Absyn_Exp*_tmpB2A=
# 2614
Cyc_Toc_push_address_exp(_tmp660);_tmp4BF[0U]=_tmpB2A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BF,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmpB2C;});}else{
# 2617
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2623
if(_tmp65F != Cyc_Absyn_PostInc){
struct _tuple1*_tmp4C0=Cyc_Toc_temp_var();
void*_tmp4C1=({void*_tmpB2D=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_typ(_tmpB2D,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp4C2=Cyc_Toc_push_address_exp(_tmp660);
struct Cyc_Absyn_Exp*_tmp4C3=({struct Cyc_Absyn_Exp*_tmpB2E=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp4C0,0U),0U),0U);Cyc_Absyn_neq_exp(_tmpB2E,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(need_null_ck)
({struct Cyc_Absyn_Exp*_tmpB31=({struct Cyc_Absyn_Exp*_tmpB30=({struct Cyc_Absyn_Exp*_tmpB2F=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp4C0,0U),0U);Cyc_Absyn_neq_exp(_tmpB2F,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 2630
Cyc_Absyn_and_exp(_tmpB30,_tmp4C3,0U);});_tmp4C3=_tmpB31;});{
# 2632
struct Cyc_Absyn_Stmt*_tmp4C4=({struct Cyc_Absyn_Exp*_tmpB33=_tmp4C3;struct Cyc_Absyn_Stmt*_tmpB32=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp4C0,0U),0U),0U),0U);
# 2632
Cyc_Absyn_ifthenelse_stmt(_tmpB33,_tmpB32,
# 2634
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmpB35=({struct Cyc_Absyn_Stmt*_tmpB34=_tmp4C4;Cyc_Absyn_seq_stmt(_tmpB34,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp4C0,0U),0U),0U),0U);});_tmp4C4=_tmpB35;});
({void*_tmpB36=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp4C0,_tmp4C1,_tmp4C2,_tmp4C4,0U),0U))->r;e->r=_tmpB36;});};}else{
# 2639
struct Cyc_Toc_functionSet*_tmp4C5=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp4C5,_tmp660);
({void*_tmpB3A=({struct Cyc_Absyn_Exp*_tmpB39=fn_e;Cyc_Toc_fncall_exp_r(_tmpB39,({struct Cyc_Absyn_Exp*_tmp4C6[2U];({struct Cyc_Absyn_Exp*_tmpB37=
Cyc_Absyn_signed_int_exp(1,0U);_tmp4C6[1U]=_tmpB37;});({struct Cyc_Absyn_Exp*_tmpB38=
# 2641
Cyc_Toc_push_address_exp(_tmp660);_tmp4C6[0U]=_tmpB38;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C6,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmpB3A;});}}else{
# 2644
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp660)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp660,0,Cyc_Toc_incr_lvalue,_tmp65F);
e->r=_tmp660->r;}}}
# 2648
goto _LL5;};};};}case 4U: _LL12: _tmp663=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp662=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp661=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL13: {
# 2667 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp663->topt);
void*e2_old_typ=(void*)_check_null(_tmp661->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp663,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp663);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp661);
({void*_tmpB3B=Cyc_Toc_tagged_union_assignop(_tmp663,e1_old_typ,_tmp662,_tmp661,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmpB3B;});
# 2678
return;}{
# 2680
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp663,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp663,_tmp662,_tmp661,ptr_type,is_dyneither,elt_type);
# 2686
return;}{
# 2690
int e1_poly=Cyc_Toc_is_poly_project(_tmp663);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp663);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp661);{
# 2696
int done=0;
if(_tmp662 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp4C8=(enum Cyc_Absyn_Primop)_tmp662->v;enum Cyc_Absyn_Primop _tmp4C9=_tmp4C8;switch(_tmp4C9){case Cyc_Absyn_Plus: _LL8F: _LL90:
 change=_tmp661;goto _LL8E;case Cyc_Absyn_Minus: _LL91: _LL92:
# 2704
({struct Cyc_Absyn_Exp*_tmpB3C=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp661,0U);change=_tmpB3C;});goto _LL8E;default: _LL93: _LL94:
({void*_tmp4CA=0U;({struct _dyneither_ptr _tmpB3D=({const char*_tmp4CB="bad t ? pointer arithmetic";_tag_dyneither(_tmp4CB,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB3D,_tag_dyneither(_tmp4CA,sizeof(void*),0U));});});}_LL8E:;}
# 2707
done=1;{
# 2709
struct Cyc_Absyn_Exp*_tmp4CC=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmpB41=({struct Cyc_Absyn_Exp*_tmpB40=_tmp4CC;Cyc_Toc_fncall_exp_r(_tmpB40,({struct Cyc_Absyn_Exp*_tmp4CD[3U];_tmp4CD[2U]=change;({struct Cyc_Absyn_Exp*_tmpB3E=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp4CD[1U]=_tmpB3E;});({struct Cyc_Absyn_Exp*_tmpB3F=
# 2710
Cyc_Toc_push_address_exp(_tmp663);_tmp4CD[0U]=_tmpB3F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CD,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmpB41;});};}else{
# 2713
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2716
enum Cyc_Absyn_Primop _tmp4CE=(enum Cyc_Absyn_Primop)_tmp662->v;enum Cyc_Absyn_Primop _tmp4CF=_tmp4CE;if(_tmp4CF == Cyc_Absyn_Plus){_LL96: _LL97:
# 2718
 done=1;
({void*_tmpB43=({struct Cyc_Absyn_Exp*_tmpB42=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp663);Cyc_Toc_fncall_exp_r(_tmpB42,({struct Cyc_Absyn_Exp*_tmp4D0[2U];_tmp4D0[1U]=_tmp661;_tmp4D0[0U]=_tmp663;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D0,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmpB43;});
goto _LL95;}else{_LL98: _LL99:
({void*_tmp4D1=0U;({struct _dyneither_ptr _tmpB44=({const char*_tmp4D2="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp4D2,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB44,_tag_dyneither(_tmp4D1,sizeof(void*),0U));});});}_LL95:;}}}
# 2725
if(!done){
# 2727
if(e1_poly)
({void*_tmpB46=({void*_tmpB45=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpB45,Cyc_Absyn_new_exp(_tmp661->r,0U));});_tmp661->r=_tmpB46;});
# 2733
if(!Cyc_Absyn_is_lvalue(_tmp663)){
({struct Cyc_Absyn_Exp*_tmpB48=_tmp663;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmpB48,0,Cyc_Toc_assignop_lvalue,({struct _tuple27*_tmp4D3=_cycalloc(sizeof(struct _tuple27)* 1);({typeof((struct _tuple27)({struct _tuple27 _tmp4D4;_tmp4D4.f1=_tmp662;_tmp4D4.f2=_tmp661;_tmp4D4;}))_tmpB47=(struct _tuple27)({struct _tuple27 _tmp4D4;_tmp4D4.f1=_tmp662;_tmp4D4.f2=_tmp661;_tmp4D4;});_tmp4D3[0]=_tmpB47;});_tmp4D3;}));});
e->r=_tmp663->r;}}
# 2738
goto _LL5;};};};}case 6U: _LL14: _tmp666=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp665=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp664=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL15:
# 2740
 Cyc_Toc_exp_to_c(nv,_tmp666);
Cyc_Toc_exp_to_c(nv,_tmp665);
Cyc_Toc_exp_to_c(nv,_tmp664);
goto _LL5;case 7U: _LL16: _tmp668=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp667=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL17:
# 2745
 Cyc_Toc_exp_to_c(nv,_tmp668);
Cyc_Toc_exp_to_c(nv,_tmp667);
goto _LL5;case 8U: _LL18: _tmp66A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp669=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL19:
# 2749
 Cyc_Toc_exp_to_c(nv,_tmp66A);
Cyc_Toc_exp_to_c(nv,_tmp669);
goto _LL5;case 9U: _LL1A: _tmp66C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp66B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL1B:
# 2753
 Cyc_Toc_exp_to_c(nv,_tmp66C);
Cyc_Toc_exp_to_c(nv,_tmp66B);
goto _LL5;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3 == 0){_LL1C: _tmp66E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp66D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL1D:
# 2757
 Cyc_Toc_exp_to_c(nv,_tmp66E);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp66D);
goto _LL5;}else{_LL1E: _tmp673=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp672=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp671=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3)->num_varargs;_tmp670=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3)->injectors;_tmp66F=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3)->vai;_LL1F: {
# 2769 "toc.cyc"
struct _RegionHandle _tmp4D5=_new_region("r");struct _RegionHandle*r=& _tmp4D5;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp671,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp66F->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0U);
# 2777
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp672);
int num_normargs=num_args - _tmp671;
# 2781
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp672=_tmp672->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp672))->hd);
({struct Cyc_List_List*_tmpB49=({struct Cyc_List_List*_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6->hd=(struct Cyc_Absyn_Exp*)_tmp672->hd;_tmp4D6->tl=new_args;_tmp4D6;});new_args=_tmpB49;});}}
# 2786
({struct Cyc_List_List*_tmpB4D=({struct Cyc_List_List*_tmp4D7=_cycalloc(sizeof(*_tmp4D7));({struct Cyc_Absyn_Exp*_tmpB4C=({struct Cyc_Absyn_Exp*_tmpB4B=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpB4B,({struct Cyc_Absyn_Exp*_tmp4D8[3U];_tmp4D8[2U]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmpB4A=
# 2788
Cyc_Absyn_sizeoftyp_exp(cva_type,0U);_tmp4D8[1U]=_tmpB4A;});_tmp4D8[0U]=argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D8,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});_tmp4D7->hd=_tmpB4C;});_tmp4D7->tl=new_args;_tmp4D7;});
# 2786
new_args=_tmpB4D;});
# 2791
({struct Cyc_List_List*_tmpB4E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmpB4E;});
# 2793
Cyc_Toc_exp_to_c(nv,_tmp673);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp673,new_args,0U),0U);
# 2797
if(_tmp66F->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp4D9=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp66F->type));void*_tmp4DA=_tmp4D9;struct Cyc_Absyn_Datatypedecl*_tmp4DD;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4DA)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4DA)->f1).datatype_info).KnownDatatype).tag == 2){_LL9B: _tmp4DD=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4DA)->f1).datatype_info).KnownDatatype).val;_LL9C:
 tud=_tmp4DD;goto _LL9A;}else{goto _LL9D;}}else{_LL9D: _LL9E:
({void*_tmp4DB=0U;({struct _dyneither_ptr _tmpB4F=({const char*_tmp4DC="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp4DC,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB4F,_tag_dyneither(_tmp4DB,sizeof(void*),0U));});});}_LL9A:;}{
# 2803
struct _dyneither_ptr vs=({unsigned int _tmp4E6=(unsigned int)_tmp671;struct _tuple1**_tmp4E7=(struct _tuple1**)({struct _RegionHandle*_tmpB50=r;_region_malloc(_tmpB50,_check_times(sizeof(struct _tuple1*),_tmp4E6));});struct _dyneither_ptr _tmp4E9=_tag_dyneither(_tmp4E7,sizeof(struct _tuple1*),_tmp4E6);{unsigned int _tmp4E8=_tmp4E6;unsigned int i;for(i=0;i < _tmp4E8;i ++){({typeof((struct _tuple1*)Cyc_Toc_temp_var())_tmpB51=(struct _tuple1*)Cyc_Toc_temp_var();_tmp4E7[i]=_tmpB51;});}}_tmp4E9;});
# 2805
if(_tmp671 != 0){
# 2807
struct Cyc_List_List*_tmp4DE=0;
{int i=_tmp671 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmpB53=({struct Cyc_List_List*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));({struct _tuple19*_tmpB52=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U));_tmp4DF->hd=_tmpB52;});_tmp4DF->tl=_tmp4DE;_tmp4DF;});_tmp4DE=_tmpB53;});}}
# 2811
({struct Cyc_Absyn_Stmt*_tmpB57=({struct _tuple1*_tmpB56=argv;void*_tmpB55=arr_type;struct Cyc_Absyn_Exp*_tmpB54=Cyc_Absyn_unresolvedmem_exp(0,_tmp4DE,0U);Cyc_Absyn_declare_stmt(_tmpB56,_tmpB55,_tmpB54,s,0U);});s=_tmpB57;});{
# 2814
int i=0;for(0;_tmp672 != 0;(((_tmp672=_tmp672->tl,_tmp670=_tmp670->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp672->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp4E0=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp670))->hd;struct Cyc_Absyn_Datatypefield*_tmp4E1=_tmp4E0;struct _tuple1*_tmp4E3;struct Cyc_List_List*_tmp4E2;_LLA0: _tmp4E3=_tmp4E1->name;_tmp4E2=_tmp4E1->typs;_LLA1:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp4E2))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmpB58=Cyc_Toc_cast_it(field_typ,arg);arg=_tmpB58;});
# 2829
({struct Cyc_Absyn_Stmt*_tmpB5C=({struct Cyc_Absyn_Stmt*_tmpB5B=({struct Cyc_Absyn_Exp*_tmpB5A=({struct Cyc_Absyn_Exp*_tmpB59=varexp;Cyc_Toc_member_exp(_tmpB59,Cyc_Absyn_fieldname(1),0U);});Cyc_Absyn_assign_stmt(_tmpB5A,arg,0U);});Cyc_Absyn_seq_stmt(_tmpB5B,s,0U);});s=_tmpB5C;});
# 2832
({struct Cyc_Absyn_Stmt*_tmpB5F=({struct Cyc_Absyn_Stmt*_tmpB5E=({struct Cyc_Absyn_Exp*_tmpB5D=Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0U);Cyc_Absyn_assign_stmt(_tmpB5D,
Cyc_Toc_datatype_tag(tud,_tmp4E3),0U);});
# 2832
Cyc_Absyn_seq_stmt(_tmpB5E,s,0U);});s=_tmpB5F;});
# 2835
({struct Cyc_Absyn_Stmt*_tmpB62=({struct _tuple1*_tmpB61=var;void*_tmpB60=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4E3,tud->name));Cyc_Absyn_declare_stmt(_tmpB61,_tmpB60,0,s,0U);});s=_tmpB62;});};}};}else{
# 2842
struct Cyc_List_List*_tmp4E4=({struct _tuple19*_tmp4E5[3U];({struct _tuple19*_tmpB63=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0U,0U));_tmp4E5[2U]=_tmpB63;});({struct _tuple19*_tmpB64=
# 2842
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0U,0U));_tmp4E5[1U]=_tmpB64;});({struct _tuple19*_tmpB65=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0U,0U));_tmp4E5[0U]=_tmpB65;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E5,sizeof(struct _tuple19*),3U));});
# 2844
({struct Cyc_Absyn_Stmt*_tmpB69=({struct _tuple1*_tmpB68=argv;void*_tmpB67=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmpB66=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmpB68,_tmpB67,_tmpB66,s,0U);});s=_tmpB69;});}};}else{
# 2854
{int i=0;for(0;_tmp672 != 0;(_tmp672=_tmp672->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp672->hd);
({struct Cyc_Absyn_Stmt*_tmpB6D=({struct Cyc_Absyn_Stmt*_tmpB6C=({struct Cyc_Absyn_Exp*_tmpB6B=({struct Cyc_Absyn_Exp*_tmpB6A=argvexp;Cyc_Absyn_subscript_exp(_tmpB6A,Cyc_Absyn_uint_exp((unsigned int)i,0U),0U);});Cyc_Absyn_assign_stmt(_tmpB6B,(struct Cyc_Absyn_Exp*)_tmp672->hd,0U);});Cyc_Absyn_seq_stmt(_tmpB6C,s,0U);});s=_tmpB6D;});}}
# 2860
({struct Cyc_Absyn_Stmt*_tmpB6E=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0U);s=_tmpB6E;});}
# 2873 "toc.cyc"
({void*_tmpB6F=Cyc_Toc_stmt_exp_r(s);e->r=_tmpB6F;});};}
# 2875
_npop_handler(0U);goto _LL5;
# 2769 "toc.cyc"
;_pop_region(r);}}case 11U: _LL20: _tmp675=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp674=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL21:
# 2878 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp675);{
struct Cyc_Absyn_Exp*_tmp4EA=_tmp674?Cyc_Toc_newrethrow_exp(_tmp675): Cyc_Toc_newthrow_exp(_tmp675);
({void*_tmpB71=({void*_tmpB70=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmpB70,_tmp4EA,0U);})->r;e->r=_tmpB71;});
goto _LL5;};case 12U: _LL22: _tmp676=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL23:
 Cyc_Toc_exp_to_c(nv,_tmp676);goto _LL5;case 13U: _LL24: _tmp678=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp677=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL25:
# 2884
 Cyc_Toc_exp_to_c(nv,_tmp678);
# 2893 "toc.cyc"
for(0;_tmp677 != 0;_tmp677=_tmp677->tl){
enum Cyc_Absyn_KindQual _tmp4EB=(Cyc_Tcutil_typ_kind((void*)_tmp677->hd))->kind;
if(_tmp4EB != Cyc_Absyn_EffKind  && _tmp4EB != Cyc_Absyn_RgnKind){
{void*_tmp4EC=Cyc_Tcutil_compress((void*)_tmp677->hd);void*_tmp4ED=_tmp4EC;switch(*((int*)_tmp4ED)){case 2U: _LLA3: _LLA4:
 goto _LLA6;case 3U: _LLA5: _LLA6:
 continue;default: _LLA7: _LLA8:
# 2900
({void*_tmpB73=({void*_tmpB72=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmpB72,_tmp678,0U);})->r;e->r=_tmpB73;});
goto _LLA2;}_LLA2:;}
# 2903
break;}}
# 2906
goto _LL5;case 14U: _LL26: _tmp67C=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp67B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp67A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp679=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL27: {
# 2908
void*old_t2=(void*)_check_null(_tmp67B->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp67C;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp67C=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp67B);
# 2915
{struct _tuple29 _tmp4EE=({struct _tuple29 _tmp529;({void*_tmpB74=Cyc_Tcutil_compress(old_t2);_tmp529.f1=_tmpB74;});({void*_tmpB75=Cyc_Tcutil_compress(new_typ);_tmp529.f2=_tmpB75;});_tmp529;});struct _tuple29 _tmp4EF=_tmp4EE;struct Cyc_Absyn_PtrInfo _tmp528;struct Cyc_Absyn_PtrInfo _tmp527;struct Cyc_Absyn_PtrInfo _tmp526;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EF.f1)->tag == 5U)switch(*((int*)_tmp4EF.f2)){case 5U: _LLAA: _tmp527=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EF.f1)->f1;_tmp526=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EF.f2)->f1;_LLAB: {
# 2917
int _tmp4F0=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp527.ptr_atts).nullable);
int _tmp4F1=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp526.ptr_atts).nullable);
void*_tmp4F2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp527.ptr_atts).bounds);
void*_tmp4F3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp526.ptr_atts).bounds);
int _tmp4F4=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp527.ptr_atts).zero_term);
int _tmp4F5=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp526.ptr_atts).zero_term);
{struct _tuple29 _tmp4F6=({struct _tuple29 _tmp523;_tmp523.f1=_tmp4F2;_tmp523.f2=_tmp4F3;_tmp523;});struct _tuple29 _tmp4F7=_tmp4F6;struct Cyc_Absyn_Exp*_tmp522;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Exp*_tmp520;struct Cyc_Absyn_Exp*_tmp51F;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F7.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F7.f2)->tag == 1U){_LLB1: _tmp520=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F7.f1)->f1;_tmp51F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F7.f2)->f1;_LLB2:
# 2925
 if((!Cyc_Evexp_c_can_eval(_tmp520) || !Cyc_Evexp_c_can_eval(_tmp51F)) && !
Cyc_Evexp_same_const_exp(_tmp520,_tmp51F))
({void*_tmp4F8=0U;({unsigned int _tmpB77=e->loc;struct _dyneither_ptr _tmpB76=({const char*_tmp4F9="can't validate cast due to potential size differences";_tag_dyneither(_tmp4F9,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpB77,_tmpB76,_tag_dyneither(_tmp4F8,sizeof(void*),0U));});});
if(_tmp4F0  && !_tmp4F1){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp4FA=0U;({struct _dyneither_ptr _tmpB78=({const char*_tmp4FB="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp4FB,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpB78,_tag_dyneither(_tmp4FA,sizeof(void*),0U));});});
# 2934
if(_tmp679 != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0U;({struct _dyneither_ptr _tmpB79=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp4FE.f1=_tmpB79;});({void*_tmp4FC[1U]={& _tmp4FE};({struct _dyneither_ptr _tmpB7A=({const char*_tmp4FD="null-check conversion mis-classified: %s";_tag_dyneither(_tmp4FD,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB7A,_tag_dyneither(_tmp4FC,sizeof(void*),1U));});});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp67B);
if(do_null_check){
if(!_tmp67A)
({void*_tmp4FF=0U;({unsigned int _tmpB7C=e->loc;struct _dyneither_ptr _tmpB7B=({const char*_tmp500="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp500,sizeof(char),58U);});Cyc_Tcutil_warn(_tmpB7C,_tmpB7B,_tag_dyneither(_tmp4FF,sizeof(void*),0U));});});
# 2941
({void*_tmpB7F=({void*_tmpB7E=*_tmp67C;Cyc_Toc_cast_it_r(_tmpB7E,({struct Cyc_Absyn_Exp*_tmpB7D=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmpB7D,({struct Cyc_List_List*_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->hd=_tmp67B;_tmp501->tl=0;_tmp501;}),0U);}));});e->r=_tmpB7F;});}else{
# 2945
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp67B->r;}};}else{
# 2950
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp67B->r;}
# 2957
goto _LLB0;}else{_LLB3: _tmp521=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F7.f1)->f1;_LLB4:
# 2959
 if(!Cyc_Evexp_c_can_eval(_tmp521))
({void*_tmp502=0U;({unsigned int _tmpB81=e->loc;struct _dyneither_ptr _tmpB80=({const char*_tmp503="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp503,sizeof(char),71U);});Cyc_Tcutil_terr(_tmpB81,_tmpB80,_tag_dyneither(_tmp502,sizeof(void*),0U));});});
# 2962
if(_tmp679 == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0U;({struct _dyneither_ptr _tmpB82=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp506.f1=_tmpB82;});({void*_tmp504[1U]={& _tmp506};({struct _dyneither_ptr _tmpB83=({const char*_tmp505="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp505,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB83,_tag_dyneither(_tmp504,sizeof(void*),1U));});});});
if(Cyc_Toc_is_toplevel(nv)){
# 2966
if((_tmp4F4  && !(_tmp526.elt_tq).real_const) && !_tmp4F5)
# 2969
({struct Cyc_Absyn_Exp*_tmpB85=({struct Cyc_Absyn_Exp*_tmpB84=_tmp521;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB84,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp521=_tmpB85;});
# 2971
({void*_tmpB86=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp521,_tmp67B))->r;e->r=_tmpB86;});}else{
# 2973
struct Cyc_Absyn_Exp*_tmp507=Cyc_Toc__tag_dyneither_e;
# 2975
if(_tmp4F4){
# 2980
struct _tuple1*_tmp508=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp509=Cyc_Absyn_var_exp(_tmp508,0U);
struct Cyc_Absyn_Exp*arg3;
# 2985
{void*_tmp50A=_tmp67B->r;void*_tmp50B=_tmp50A;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp50B)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp50B)->f1).Wstring_c).tag){case 8U: _LLBA: _LLBB:
# 2987
 arg3=_tmp521;goto _LLB9;case 9U: _LLBC: _LLBD:
# 2989
 arg3=_tmp521;goto _LLB9;default: goto _LLBE;}else{_LLBE: _LLBF:
# 2991
({struct Cyc_Absyn_Exp*_tmpB8A=({struct Cyc_Absyn_Exp*_tmpB89=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp67B);Cyc_Absyn_fncall_exp(_tmpB89,({struct Cyc_Absyn_Exp*_tmp50C[2U];_tmp50C[1U]=_tmp521;({struct Cyc_Absyn_Exp*_tmpB88=({
# 2993
void*_tmpB87=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmpB87,_tmp509);});_tmp50C[0U]=_tmpB88;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp50C,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2991
arg3=_tmpB8A;});
# 2994
goto _LLB9;}_LLB9:;}
# 2996
if(!_tmp4F5  && !(_tmp526.elt_tq).real_const)
# 2999
({struct Cyc_Absyn_Exp*_tmpB8C=({struct Cyc_Absyn_Exp*_tmpB8B=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB8B,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmpB8C;});{
# 3001
struct Cyc_Absyn_Exp*_tmp50D=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp526.elt_typ),0U);
struct Cyc_Absyn_Exp*_tmp50E=({struct Cyc_Absyn_Exp*_tmpB8D=_tmp507;Cyc_Absyn_fncall_exp(_tmpB8D,({struct Cyc_Absyn_Exp*_tmp510[3U];_tmp510[2U]=arg3;_tmp510[1U]=_tmp50D;_tmp510[0U]=_tmp509;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp510,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp50F=Cyc_Absyn_exp_stmt(_tmp50E,0U);
({struct Cyc_Absyn_Stmt*_tmpB91=({struct _tuple1*_tmpB90=_tmp508;void*_tmpB8F=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmpB8E=_tmp67B;Cyc_Absyn_declare_stmt(_tmpB90,_tmpB8F,_tmpB8E,_tmp50F,0U);});_tmp50F=_tmpB91;});
({void*_tmpB92=Cyc_Toc_stmt_exp_r(_tmp50F);e->r=_tmpB92;});};}else{
# 3008
({void*_tmpB95=({struct Cyc_Absyn_Exp*_tmpB94=_tmp507;Cyc_Toc_fncall_exp_r(_tmpB94,({struct Cyc_Absyn_Exp*_tmp511[3U];_tmp511[2U]=_tmp521;({struct Cyc_Absyn_Exp*_tmpB93=
# 3010
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp526.elt_typ),0U);_tmp511[1U]=_tmpB93;});_tmp511[0U]=_tmp67B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp511,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 3008
e->r=_tmpB95;});}}
# 3014
goto _LLB0;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F7.f2)->tag == 1U){_LLB5: _tmp522=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F7.f2)->f1;_LLB6:
# 3016
 if(!Cyc_Evexp_c_can_eval(_tmp522))
({void*_tmp512=0U;({unsigned int _tmpB97=e->loc;struct _dyneither_ptr _tmpB96=({const char*_tmp513="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp513,sizeof(char),71U);});Cyc_Tcutil_terr(_tmpB97,_tmpB96,_tag_dyneither(_tmp512,sizeof(void*),0U));});});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp514=0U;({struct _dyneither_ptr _tmpB98=({const char*_tmp515="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp515,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpB98,_tag_dyneither(_tmp514,sizeof(void*),0U));});});{
# 3028 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp516=_tmp522;
if(_tmp4F4  && !_tmp4F5)
({struct Cyc_Absyn_Exp*_tmpB9A=({struct Cyc_Absyn_Exp*_tmpB99=_tmp522;Cyc_Absyn_add_exp(_tmpB99,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp516=_tmpB9A;});{
# 3035
struct Cyc_Absyn_Exp*_tmp517=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp518=({struct Cyc_Absyn_Exp*_tmpB9C=_tmp517;Cyc_Absyn_fncall_exp(_tmpB9C,({struct Cyc_Absyn_Exp*_tmp51A[3U];_tmp51A[2U]=_tmp516;({struct Cyc_Absyn_Exp*_tmpB9B=
# 3038
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp527.elt_typ),0U);_tmp51A[1U]=_tmpB9B;});_tmp51A[0U]=_tmp67B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp51A,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3040
if(!_tmp4F1)
({void*_tmpB9F=({struct Cyc_Absyn_Exp*_tmpB9E=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmpB9E,({struct Cyc_List_List*_tmp519=_cycalloc(sizeof(*_tmp519));({struct Cyc_Absyn_Exp*_tmpB9D=
Cyc_Absyn_copy_exp(_tmp518);_tmp519->hd=_tmpB9D;});_tmp519->tl=0;_tmp519;}));});
# 3041
_tmp518->r=_tmpB9F;});
# 3043
({void*_tmpBA0=Cyc_Toc_cast_it_r(*_tmp67C,_tmp518);e->r=_tmpBA0;});
goto _LLB0;};};}else{_LLB7: _LLB8:
# 3048
 DynCast:
 if((_tmp4F4  && !_tmp4F5) && !(_tmp526.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp51B=0U;({struct _dyneither_ptr _tmpBA1=({const char*_tmp51C="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp51C,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpBA1,_tag_dyneither(_tmp51B,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp51D=Cyc_Toc__dyneither_ptr_decrease_size_e;
({void*_tmpBA5=({struct Cyc_Absyn_Exp*_tmpBA4=_tmp51D;Cyc_Toc_fncall_exp_r(_tmpBA4,({struct Cyc_Absyn_Exp*_tmp51E[3U];({struct Cyc_Absyn_Exp*_tmpBA2=
# 3055
Cyc_Absyn_uint_exp(1U,0U);_tmp51E[2U]=_tmpBA2;});({struct Cyc_Absyn_Exp*_tmpBA3=
# 3054
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp527.elt_typ),0U);_tmp51E[1U]=_tmpBA3;});_tmp51E[0U]=_tmp67B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp51E,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 3053
e->r=_tmpBA5;});};}
# 3057
goto _LLB0;}}_LLB0:;}
# 3059
goto _LLA9;}case 6U: _LLAC: _tmp528=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EF.f1)->f1;_LLAD:
# 3061
{void*_tmp524=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp528.ptr_atts).bounds);void*_tmp525=_tmp524;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp525)->tag == 0U){_LLC1: _LLC2:
# 3063
({void*_tmpBA7=({struct Cyc_Absyn_Exp*_tmpBA6=Cyc_Absyn_new_exp(_tmp67B->r,_tmp67B->loc);Cyc_Toc_aggrmember_exp_r(_tmpBA6,Cyc_Toc_curr_sp);});_tmp67B->r=_tmpBA7;});
_tmp67B->topt=new_typ_c;
goto _LLC0;}else{_LLC3: _LLC4:
 goto _LLC0;}_LLC0:;}
# 3068
goto _LLA9;default: goto _LLAE;}else{_LLAE: _LLAF:
# 3070
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp67B->r;
goto _LLA9;}_LLA9:;}
# 3074
goto _LL5;}case 15U: _LL28: _tmp67D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL29:
# 3078
{void*_tmp52A=_tmp67D->r;void*_tmp52B=_tmp52A;struct Cyc_List_List*_tmp537;struct _tuple1*_tmp536;struct Cyc_List_List*_tmp535;struct Cyc_List_List*_tmp534;switch(*((int*)_tmp52B)){case 29U: _LLC6: _tmp536=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_tmp535=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp52B)->f2;_tmp534=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp52B)->f3;_LLC7:
# 3080
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp52E;_tmp52E.tag=0U;({struct _dyneither_ptr _tmpBA8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp67D->loc));_tmp52E.f1=_tmpBA8;});({void*_tmp52C[1U]={& _tmp52E};({struct _dyneither_ptr _tmpBA9=({const char*_tmp52D="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp52D,sizeof(char),42U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpBA9,_tag_dyneither(_tmp52C,sizeof(void*),1U));});});});{
struct Cyc_Absyn_Exp*_tmp52F=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp67D->topt),_tmp535,1,0,_tmp534,_tmp536);
e->r=_tmp52F->r;
e->topt=_tmp52F->topt;
goto _LLC5;};case 24U: _LLC8: _tmp537=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LLC9:
# 3088
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp532;_tmp532.tag=0U;({struct _dyneither_ptr _tmpBAA=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp67D->loc));_tmp532.f1=_tmpBAA;});({void*_tmp530[1U]={& _tmp532};({struct _dyneither_ptr _tmpBAB=({const char*_tmp531="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp531,sizeof(char),42U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpBAB,_tag_dyneither(_tmp530,sizeof(void*),1U));});});});{
struct Cyc_Absyn_Exp*_tmp533=Cyc_Toc_init_tuple(nv,1,0,_tmp537);
e->r=_tmp533->r;
e->topt=_tmp533->topt;
goto _LLC5;};default: _LLCA: _LLCB:
# 3096
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp67D);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp67D)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp67D,0,Cyc_Toc_address_lvalue,1);
# 3102
({void*_tmpBAD=({void*_tmpBAC=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmpBAC,_tmp67D);});e->r=_tmpBAD;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp67D->topt))))
# 3106
({void*_tmpBAF=({void*_tmpBAE=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmpBAE,_tmp67D);});e->r=_tmpBAF;});}
# 3108
goto _LLC5;}_LLC5:;}
# 3110
goto _LL5;case 16U: _LL2A: _tmp67F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp67E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL2B:
# 3113
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp53A;_tmp53A.tag=0U;({struct _dyneither_ptr _tmpBB0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp67E->loc));_tmp53A.f1=_tmpBB0;});({void*_tmp538[1U]={& _tmp53A};({struct _dyneither_ptr _tmpBB1=({const char*_tmp539="%s: new at top-level";_tag_dyneither(_tmp539,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpBB1,_tag_dyneither(_tmp538,sizeof(void*),1U));});});});{
void*new_e_type=(void*)_check_null(_tmp67E->topt);
{void*_tmp53B=_tmp67E->r;void*_tmp53C=_tmp53B;struct Cyc_List_List*_tmp56C;struct _tuple1*_tmp56B;struct Cyc_List_List*_tmp56A;struct Cyc_List_List*_tmp569;struct Cyc_Absyn_Aggrdecl*_tmp568;struct Cyc_Absyn_Exp*_tmp567;void*_tmp566;int _tmp565;struct Cyc_Absyn_Vardecl*_tmp564;struct Cyc_Absyn_Exp*_tmp563;struct Cyc_Absyn_Exp*_tmp562;int _tmp561;struct Cyc_List_List*_tmp560;switch(*((int*)_tmp53C)){case 26U: _LLCD: _tmp560=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LLCE: {
# 3121
struct _tuple1*_tmp53D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp53E=Cyc_Absyn_var_exp(_tmp53D,0U);
struct Cyc_Absyn_Stmt*_tmp53F=({struct Cyc_Toc_Env*_tmpBB5=nv;void*_tmpBB4=new_e_type;struct Cyc_Absyn_Exp*_tmpBB3=_tmp53E;struct Cyc_List_List*_tmpBB2=_tmp560;Cyc_Toc_init_array(_tmpBB5,_tmpBB4,_tmpBB3,_tmpBB2,Cyc_Absyn_exp_stmt(_tmp53E,0U));});
void*old_elt_typ;
{void*_tmp540=Cyc_Tcutil_compress(old_typ);void*_tmp541=_tmp540;void*_tmp546;struct Cyc_Absyn_Tqual _tmp545;union Cyc_Absyn_Constraint*_tmp544;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp541)->tag == 5U){_LLDA: _tmp546=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp541)->f1).elt_typ;_tmp545=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp541)->f1).elt_tq;_tmp544=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp541)->f1).ptr_atts).zero_term;_LLDB:
# 3127
 old_elt_typ=_tmp546;goto _LLD9;}else{_LLDC: _LLDD:
# 3129
({void*_tmpBB7=({void*_tmp542=0U;({struct _dyneither_ptr _tmpBB6=({const char*_tmp543="exp_to_c:new array expression doesn't have ptr type";_tag_dyneither(_tmp543,sizeof(char),52U);});((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpBB6,_tag_dyneither(_tmp542,sizeof(void*),0U));});});old_elt_typ=_tmpBB7;});}_LLD9:;}{
# 3132
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp547=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp548=({struct Cyc_Absyn_Exp*_tmpBB8=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);Cyc_Absyn_times_exp(_tmpBB8,
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp560),0U),0U);});
struct Cyc_Absyn_Exp*e1;
if(_tmp67F == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmpBB9=Cyc_Toc_malloc_exp(old_elt_typ,_tmp548);e1=_tmpBB9;});else{
# 3140
struct Cyc_Absyn_Exp*r=_tmp67F;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmpBBA=Cyc_Toc_rmalloc_exp(r,_tmp548);e1=_tmpBBA;});}
# 3145
({void*_tmpBBB=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp53D,_tmp547,e1,_tmp53F,0U));e->r=_tmpBBB;});
goto _LLCC;};}case 27U: _LLCF: _tmp564=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_tmp563=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_tmp562=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp53C)->f3;_tmp561=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp53C)->f4;_LLD0:
# 3149
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp67F,old_typ,_tmp563,(void*)_check_null(_tmp562->topt),_tmp561,_tmp562,_tmp564);
goto _LLCC;case 28U: _LLD1: _tmp567=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_tmp566=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_tmp565=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp53C)->f3;_LLD2:
# 3153
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp67F,old_typ,_tmp567,_tmp566,_tmp565,0,0);
goto _LLCC;case 29U: _LLD3: _tmp56B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_tmp56A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_tmp569=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp53C)->f3;_tmp568=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp53C)->f4;_LLD4: {
# 3158
struct Cyc_Absyn_Exp*_tmp549=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp67E->topt),_tmp56A,1,_tmp67F,_tmp569,_tmp56B);
e->r=_tmp549->r;
e->topt=_tmp549->topt;
goto _LLCC;}case 24U: _LLD5: _tmp56C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LLD6: {
# 3164
struct Cyc_Absyn_Exp*_tmp54A=Cyc_Toc_init_tuple(nv,1,_tmp67F,_tmp56C);
e->r=_tmp54A->r;
e->topt=_tmp54A->topt;
goto _LLCC;}default: _LLD7: _LLD8: {
# 3173
void*old_elt_typ=(void*)_check_null(_tmp67E->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3176
struct _tuple1*_tmp54B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp54C=Cyc_Absyn_var_exp(_tmp54B,0U);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp54C,0U),0U);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp67F == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmpBBC=Cyc_Toc_malloc_exp(old_elt_typ,mexp);mexp=_tmpBBC;});else{
# 3183
struct Cyc_Absyn_Exp*r=_tmp67F;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmpBBD=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmpBBD;});}{
# 3191
int done=0;
{void*_tmp54D=_tmp67E->r;void*_tmp54E=_tmp54D;void*_tmp55C;struct Cyc_Absyn_Exp*_tmp55B;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp54E)->tag == 14U){_LLDF: _tmp55C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp54E)->f1;_tmp55B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp54E)->f2;_LLE0:
# 3194
{struct _tuple29 _tmp54F=({struct _tuple29 _tmp55A;({void*_tmpBBE=Cyc_Tcutil_compress(_tmp55C);_tmp55A.f1=_tmpBBE;});({void*_tmpBBF=Cyc_Tcutil_compress((void*)_check_null(_tmp55B->topt));_tmp55A.f2=_tmpBBF;});_tmp55A;});struct _tuple29 _tmp550=_tmp54F;void*_tmp559;union Cyc_Absyn_Constraint*_tmp558;union Cyc_Absyn_Constraint*_tmp557;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f1)->tag == 5U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f2)->tag == 5U){_LLE4: _tmp559=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f1)->f1).elt_typ;_tmp558=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f1)->f1).ptr_atts).bounds;_tmp557=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f2)->f1).ptr_atts).bounds;_LLE5:
# 3197
{struct _tuple29 _tmp551=({struct _tuple29 _tmp556;({void*_tmpBC0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp558);_tmp556.f1=_tmpBC0;});({void*_tmpBC1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp557);_tmp556.f2=_tmpBC1;});_tmp556;});struct _tuple29 _tmp552=_tmp551;struct Cyc_Absyn_Exp*_tmp555;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp552.f1)->tag == 0U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp552.f2)->tag == 1U){_LLE9: _tmp555=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp552.f2)->f1;_LLEA:
# 3199
 Cyc_Toc_exp_to_c(nv,_tmp55B);
({void*_tmpBC2=Cyc_Toc_sizeoftyp_exp_r(elt_typ);inner_mexp->r=_tmpBC2;});
done=1;{
struct Cyc_Absyn_Exp*_tmp553=Cyc_Toc__init_dyneither_ptr_e;
({void*_tmpBC5=({struct Cyc_Absyn_Exp*_tmpBC4=_tmp553;Cyc_Toc_fncall_exp_r(_tmpBC4,({struct Cyc_Absyn_Exp*_tmp554[4U];_tmp554[3U]=_tmp555;({struct Cyc_Absyn_Exp*_tmpBC3=
# 3205
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp559),0U);_tmp554[2U]=_tmpBC3;});_tmp554[1U]=_tmp55B;_tmp554[0U]=mexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp554,sizeof(struct Cyc_Absyn_Exp*),4U));}));});
# 3203
e->r=_tmpBC5;});
# 3207
goto _LLE8;};}else{goto _LLEB;}}else{_LLEB: _LLEC:
 goto _LLE8;}_LLE8:;}
# 3210
goto _LLE3;}else{goto _LLE6;}}else{_LLE6: _LLE7:
 goto _LLE3;}_LLE3:;}
# 3213
goto _LLDE;}else{_LLE1: _LLE2:
 goto _LLDE;}_LLDE:;}
# 3216
if(!done){
struct Cyc_Absyn_Stmt*_tmp55D=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp54C),0U);
struct Cyc_Absyn_Exp*_tmp55E=Cyc_Absyn_signed_int_exp(0,0U);
Cyc_Toc_exp_to_c(nv,_tmp67E);
({struct Cyc_Absyn_Stmt*_tmpBC8=({struct Cyc_Absyn_Stmt*_tmpBC7=({struct Cyc_Absyn_Exp*_tmpBC6=Cyc_Absyn_subscript_exp(_tmp54C,_tmp55E,0U);Cyc_Absyn_assign_stmt(_tmpBC6,_tmp67E,0U);});Cyc_Absyn_seq_stmt(_tmpBC7,_tmp55D,0U);});_tmp55D=_tmpBC8;});{
# 3222
void*_tmp55F=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
({void*_tmpBC9=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp54B,_tmp55F,mexp,_tmp55D,0U));e->r=_tmpBC9;});};}
# 3225
goto _LLCC;};}}_LLCC:;}
# 3227
goto _LL5;};case 18U: _LL2C: _tmp680=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2D: {
# 3230
int _tmp56D=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp680);
Cyc_Toc_set_in_sizeof(nv,_tmp56D);
goto _LL5;}case 17U: _LL2E: _tmp681=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2F:
({void*_tmpBCC=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp56E=_cycalloc(sizeof(*_tmp56E));({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpBCB=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp56F;_tmp56F.tag=17U;({void*_tmpBCA=Cyc_Toc_typ_to_c(_tmp681);_tmp56F.f1=_tmpBCA;});_tmp56F;});_tmp56E[0]=_tmpBCB;});_tmp56E;});e->r=_tmpBCC;});goto _LL5;case 19U: _LL30: _tmp683=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp682=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL31: {
# 3236
void*_tmp570=_tmp683;
struct Cyc_List_List*_tmp571=_tmp682;
for(0;_tmp571 != 0;_tmp571=_tmp571->tl){
void*_tmp572=(void*)_tmp571->hd;void*_tmp573=_tmp572;unsigned int _tmp589;struct _dyneither_ptr*_tmp588;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp573)->tag == 0U){_LLEE: _tmp588=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp573)->f1;_LLEF:
 goto _LLED;}else{_LLF0: _tmp589=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp573)->f1;_LLF1:
# 3242
{void*_tmp574=Cyc_Tcutil_compress(_tmp570);void*_tmp575=_tmp574;struct Cyc_Absyn_Datatypefield*_tmp587;struct Cyc_List_List*_tmp586;struct Cyc_List_List*_tmp585;union Cyc_Absyn_AggrInfoU _tmp584;switch(*((int*)_tmp575)){case 11U: _LLF3: _tmp584=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp575)->f1).aggr_info;_LLF4: {
# 3244
struct Cyc_Absyn_Aggrdecl*_tmp576=Cyc_Absyn_get_known_aggrdecl(_tmp584);
if(_tmp576->impl == 0)
({void*_tmp577=0U;({struct _dyneither_ptr _tmpBCD=({const char*_tmp578="struct fields must be known";_tag_dyneither(_tmp578,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpBCD,_tag_dyneither(_tmp577,sizeof(void*),0U));});});
_tmp585=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp576->impl))->fields;goto _LLF6;}case 12U: _LLF5: _tmp585=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp575)->f2;_LLF6: {
# 3249
struct Cyc_Absyn_Aggrfield*_tmp579=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp585,(int)_tmp589);
({void*_tmpBCF=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp57A=_cycalloc(sizeof(*_tmp57A));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBCE=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp57B;_tmp57B.tag=0U;_tmp57B.f1=_tmp579->name;_tmp57B;});_tmp57A[0]=_tmpBCE;});_tmp57A;}));_tmp571->hd=_tmpBCF;});
_tmp570=_tmp579->type;
goto _LLF2;}case 10U: _LLF7: _tmp586=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp575)->f1;_LLF8:
# 3254
({void*_tmpBD2=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp57C=_cycalloc(sizeof(*_tmp57C));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBD1=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp57D;_tmp57D.tag=0U;({struct _dyneither_ptr*_tmpBD0=Cyc_Absyn_fieldname((int)(_tmp589 + 1));_tmp57D.f1=_tmpBD0;});_tmp57D;});_tmp57C[0]=_tmpBD1;});_tmp57C;}));_tmp571->hd=_tmpBD2;});
({void*_tmpBD3=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp586,(int)_tmp589)).f2;_tmp570=_tmpBD3;});
goto _LLF2;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp575)->f1).field_info).KnownDatatypefield).tag == 2){_LLF9: _tmp587=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp575)->f1).field_info).KnownDatatypefield).val).f2;_LLFA:
# 3258
 if(_tmp589 == 0)
({void*_tmpBD5=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp57E=_cycalloc(sizeof(*_tmp57E));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBD4=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp57F;_tmp57F.tag=0U;_tmp57F.f1=Cyc_Toc_tag_sp;_tmp57F;});_tmp57E[0]=_tmpBD4;});_tmp57E;}));_tmp571->hd=_tmpBD5;});else{
# 3261
({void*_tmpBD6=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp587->typs,(int)(_tmp589 - 1))).f2;_tmp570=_tmpBD6;});
({void*_tmpBD9=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp580=_cycalloc(sizeof(*_tmp580));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBD8=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp581;_tmp581.tag=0U;({struct _dyneither_ptr*_tmpBD7=Cyc_Absyn_fieldname((int)_tmp589);_tmp581.f1=_tmpBD7;});_tmp581;});_tmp580[0]=_tmpBD8;});_tmp580;}));_tmp571->hd=_tmpBD9;});}
# 3264
goto _LLF2;}else{goto _LLFB;}default: _LLFB: _LLFC:
({void*_tmp582=0U;({struct _dyneither_ptr _tmpBDA=({const char*_tmp583="impossible type for offsetof tuple index";_tag_dyneither(_tmp583,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpBDA,_tag_dyneither(_tmp582,sizeof(void*),0U));});});
goto _LLF2;}_LLF2:;}
# 3268
goto _LLED;}_LLED:;}
# 3271
({void*_tmpBDD=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp58A=_cycalloc(sizeof(*_tmp58A));({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBDC=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp58B;_tmp58B.tag=19U;({void*_tmpBDB=Cyc_Toc_typ_to_c(_tmp683);_tmp58B.f1=_tmpBDB;});_tmp58B.f2=_tmp682;_tmp58B;});_tmp58A[0]=_tmpBDC;});_tmp58A;});e->r=_tmpBDD;});
goto _LL5;}case 20U: _LL32: _tmp684=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL33: {
# 3274
int _tmp58C=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp58D=Cyc_Tcutil_compress((void*)_check_null(_tmp684->topt));
{void*_tmp58E=_tmp58D;void*_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A1;union Cyc_Absyn_Constraint*_tmp5A0;union Cyc_Absyn_Constraint*_tmp59F;union Cyc_Absyn_Constraint*_tmp59E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58E)->tag == 5U){_LLFE: _tmp5A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58E)->f1).elt_typ;_tmp5A2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58E)->f1).elt_tq;_tmp5A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58E)->f1).ptr_atts).rgn;_tmp5A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58E)->f1).ptr_atts).nullable;_tmp59F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58E)->f1).ptr_atts).bounds;_tmp59E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58E)->f1).ptr_atts).zero_term;_LLFF:
# 3279
{void*_tmp58F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp59F);void*_tmp590=_tmp58F;struct Cyc_Absyn_Exp*_tmp59B;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp590)->tag == 1U){_LL103: _tmp59B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp590)->f1;_LL104: {
# 3281
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp684);
Cyc_Toc_exp_to_c(nv,_tmp684);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp591=0U;({unsigned int _tmpBDF=e->loc;struct _dyneither_ptr _tmpBDE=({const char*_tmp592="inserted null check due to dereference";_tag_dyneither(_tmp592,sizeof(char),39U);});Cyc_Tcutil_warn(_tmpBDF,_tmpBDE,_tag_dyneither(_tmp591,sizeof(void*),0U));});});
# 3287
({void*_tmpBE3=({void*_tmpBE2=Cyc_Toc_typ_to_c(_tmp58D);Cyc_Toc_cast_it_r(_tmpBE2,({
struct Cyc_Absyn_Exp*_tmpBE1=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmpBE1,({struct Cyc_List_List*_tmp593=_cycalloc(sizeof(*_tmp593));({struct Cyc_Absyn_Exp*_tmpBE0=
Cyc_Absyn_copy_exp(_tmp684);_tmp593->hd=_tmpBE0;});_tmp593->tl=0;_tmp593;}),0U);}));});
# 3287
_tmp684->r=_tmpBE3;});}
# 3295
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp59E)){
struct _tuple12 _tmp594=Cyc_Evexp_eval_const_uint_exp(_tmp59B);struct _tuple12 _tmp595=_tmp594;unsigned int _tmp599;int _tmp598;_LL108: _tmp599=_tmp595.f1;_tmp598=_tmp595.f2;_LL109:;
# 3301
if(!_tmp598  || _tmp599 <= 0)
({void*_tmp596=0U;({unsigned int _tmpBE5=e->loc;struct _dyneither_ptr _tmpBE4=({const char*_tmp597="cannot determine dereference is in bounds";_tag_dyneither(_tmp597,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpBE5,_tmpBE4,_tag_dyneither(_tmp596,sizeof(void*),0U));});});}
# 3304
goto _LL102;}}else{_LL105: _LL106: {
# 3307
struct Cyc_Absyn_Exp*_tmp59A=Cyc_Absyn_uint_exp(0U,0U);
_tmp59A->topt=Cyc_Absyn_uint_typ;
({void*_tmpBE6=Cyc_Toc_subscript_exp_r(_tmp684,_tmp59A);e->r=_tmpBE6;});
Cyc_Toc_exp_to_c(nv,e);
goto _LL102;}}_LL102:;}
# 3313
goto _LLFD;}else{_LL100: _LL101:
({void*_tmp59C=0U;({struct _dyneither_ptr _tmpBE7=({const char*_tmp59D="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp59D,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpBE7,_tag_dyneither(_tmp59C,sizeof(void*),0U));});});}_LLFD:;}
# 3316
Cyc_Toc_set_lhs(nv,_tmp58C);
goto _LL5;};}case 21U: _LL34: _tmp688=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp687=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp686=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp685=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL35: {
# 3319
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp688->topt);
Cyc_Toc_exp_to_c(nv,_tmp688);
if(_tmp686  && _tmp685)
({void*_tmpBEC=({struct Cyc_Absyn_Exp*_tmpBEB=_tmp688;void*_tmpBEA=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmpBE9=e1_cyc_type;struct _dyneither_ptr*_tmpBE8=_tmp687;Cyc_Toc_check_tagged_union(_tmpBEB,_tmpBEA,_tmpBE9,_tmpBE8,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 3323
e->r=_tmpBEC;});
# 3327
if(is_poly)
({void*_tmpBEE=({void*_tmpBED=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmpBED,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmpBEE;});
goto _LL5;}case 22U: _LL36: _tmp68C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp68B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp68A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp689=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL37: {
# 3331
int _tmp5A4=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp68C->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp68C);
Cyc_Toc_exp_to_c(nv,_tmp68C);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp5A5=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp5A6=_tmp5A5;void*_tmp5BC;struct Cyc_Absyn_Tqual _tmp5BB;void*_tmp5BA;union Cyc_Absyn_Constraint*_tmp5B9;union Cyc_Absyn_Constraint*_tmp5B8;union Cyc_Absyn_Constraint*_tmp5B7;_LL10B: _tmp5BC=_tmp5A6.elt_typ;_tmp5BB=_tmp5A6.elt_tq;_tmp5BA=(_tmp5A6.ptr_atts).rgn;_tmp5B9=(_tmp5A6.ptr_atts).nullable;_tmp5B8=(_tmp5A6.ptr_atts).bounds;_tmp5B7=(_tmp5A6.ptr_atts).zero_term;_LL10C:;
{void*_tmp5A7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5B8);void*_tmp5A8=_tmp5A7;struct Cyc_Absyn_Exp*_tmp5B6;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5A8)->tag == 1U){_LL10E: _tmp5B6=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5A8)->f1;_LL10F: {
# 3342
struct _tuple12 _tmp5A9=Cyc_Evexp_eval_const_uint_exp(_tmp5B6);struct _tuple12 _tmp5AA=_tmp5A9;unsigned int _tmp5B4;int _tmp5B3;_LL113: _tmp5B4=_tmp5AA.f1;_tmp5B3=_tmp5AA.f2;_LL114:;
if(_tmp5B3){
if(_tmp5B4 < 1)
({void*_tmp5AB=0U;({struct _dyneither_ptr _tmpBEF=({const char*_tmp5AC="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp5AC,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBEF,_tag_dyneither(_tmp5AB,sizeof(void*),0U));});});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5AD=0U;({unsigned int _tmpBF1=e->loc;struct _dyneither_ptr _tmpBF0=({const char*_tmp5AE="inserted null check due to dereference";_tag_dyneither(_tmp5AE,sizeof(char),39U);});Cyc_Tcutil_warn(_tmpBF1,_tmpBF0,_tag_dyneither(_tmp5AD,sizeof(void*),0U));});});
# 3350
({void*_tmpBF5=({void*_tmpBF4=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmpBF4,({
struct Cyc_Absyn_Exp*_tmpBF3=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmpBF3,({struct Cyc_Absyn_Exp*_tmp5AF[1U];({struct Cyc_Absyn_Exp*_tmpBF2=
Cyc_Absyn_new_exp(_tmp68C->r,0U);_tmp5AF[0U]=_tmpBF2;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5AF,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 3350
_tmp68C->r=_tmpBF5;});}}else{
# 3355
if(!Cyc_Evexp_c_can_eval(_tmp5B6))
({void*_tmp5B0=0U;({unsigned int _tmpBF7=e->loc;struct _dyneither_ptr _tmpBF6=({const char*_tmp5B1="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp5B1,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpBF7,_tmpBF6,_tag_dyneither(_tmp5B0,sizeof(void*),0U));});});
# 3359
({void*_tmpBFD=({void*_tmpBFC=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmpBFC,({
struct Cyc_Absyn_Exp*_tmpBFB=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmpBFB,({struct Cyc_Absyn_Exp*_tmp5B2[4U];({struct Cyc_Absyn_Exp*_tmpBF8=
# 3363
Cyc_Absyn_uint_exp(0U,0U);_tmp5B2[3U]=_tmpBF8;});({struct Cyc_Absyn_Exp*_tmpBF9=
# 3362
Cyc_Absyn_sizeoftyp_exp(_tmp5BC,0U);_tmp5B2[2U]=_tmpBF9;});_tmp5B2[1U]=_tmp5B6;({struct Cyc_Absyn_Exp*_tmpBFA=
# 3361
Cyc_Absyn_new_exp(_tmp68C->r,0U);_tmp5B2[0U]=_tmpBFA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5B2,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 3359
_tmp68C->r=_tmpBFD;});}
# 3365
goto _LL10D;}}else{_LL110: _LL111: {
# 3368
void*ta1=Cyc_Toc_typ_to_c(_tmp5BC);
({void*_tmpC03=({void*_tmpC02=Cyc_Absyn_cstar_typ(ta1,_tmp5BB);Cyc_Toc_cast_it_r(_tmpC02,({
struct Cyc_Absyn_Exp*_tmpC01=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmpC01,({struct Cyc_Absyn_Exp*_tmp5B5[3U];({struct Cyc_Absyn_Exp*_tmpBFE=
# 3373
Cyc_Absyn_uint_exp(0U,0U);_tmp5B5[2U]=_tmpBFE;});({struct Cyc_Absyn_Exp*_tmpBFF=
# 3372
Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp5B5[1U]=_tmpBFF;});({struct Cyc_Absyn_Exp*_tmpC00=
# 3371
Cyc_Absyn_new_exp(_tmp68C->r,0U);_tmp5B5[0U]=_tmpC00;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5B5,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 3369
_tmp68C->r=_tmpC03;});
# 3376
goto _LL10D;}}_LL10D:;}
# 3378
if(_tmp68A  && _tmp689)
({void*_tmpC08=({struct Cyc_Absyn_Exp*_tmpC07=_tmp68C;void*_tmpC06=Cyc_Toc_typ_to_c(e1typ);void*_tmpC05=_tmp5BC;struct _dyneither_ptr*_tmpC04=_tmp68B;Cyc_Toc_check_tagged_union(_tmpC07,_tmpC06,_tmpC05,_tmpC04,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmpC08;});
# 3381
if(is_poly  && _tmp689)
({void*_tmpC0A=({void*_tmpC09=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmpC09,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmpC0A;});
Cyc_Toc_set_lhs(nv,_tmp5A4);
goto _LL5;};};}case 23U: _LL38: _tmp68E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp68D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL39: {
# 3386
int _tmp5BD=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5BE=Cyc_Tcutil_compress((void*)_check_null(_tmp68E->topt));
# 3390
{void*_tmp5BF=_tmp5BE;void*_tmp5E9;struct Cyc_Absyn_Tqual _tmp5E8;void*_tmp5E7;union Cyc_Absyn_Constraint*_tmp5E6;union Cyc_Absyn_Constraint*_tmp5E5;union Cyc_Absyn_Constraint*_tmp5E4;struct Cyc_List_List*_tmp5E3;switch(*((int*)_tmp5BF)){case 10U: _LL116: _tmp5E3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5BF)->f1;_LL117:
# 3393
 Cyc_Toc_exp_to_c(nv,_tmp68E);
Cyc_Toc_exp_to_c(nv,_tmp68D);{
struct _tuple12 _tmp5C0=Cyc_Evexp_eval_const_uint_exp(_tmp68D);struct _tuple12 _tmp5C1=_tmp5C0;unsigned int _tmp5C5;int _tmp5C4;_LL11D: _tmp5C5=_tmp5C1.f1;_tmp5C4=_tmp5C1.f2;_LL11E:;
if(!_tmp5C4)
({void*_tmp5C2=0U;({struct _dyneither_ptr _tmpC0B=({const char*_tmp5C3="unknown tuple subscript in translation to C";_tag_dyneither(_tmp5C3,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC0B,_tag_dyneither(_tmp5C2,sizeof(void*),0U));});});
({void*_tmpC0D=({struct Cyc_Absyn_Exp*_tmpC0C=_tmp68E;Cyc_Toc_aggrmember_exp_r(_tmpC0C,Cyc_Absyn_fieldname((int)(_tmp5C5 + 1)));});e->r=_tmpC0D;});
goto _LL115;};case 5U: _LL118: _tmp5E9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BF)->f1).elt_typ;_tmp5E8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BF)->f1).elt_tq;_tmp5E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BF)->f1).ptr_atts).rgn;_tmp5E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BF)->f1).ptr_atts).nullable;_tmp5E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BF)->f1).ptr_atts).bounds;_tmp5E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BF)->f1).ptr_atts).zero_term;_LL119: {
# 3401
struct Cyc_List_List*_tmp5C6=Cyc_Toc_get_relns(_tmp68E);
int _tmp5C7=Cyc_Toc_need_null_check(_tmp68E);
int _tmp5C8=Cyc_Toc_in_sizeof(nv);
# 3409
int in_bnds=_tmp5C8;
{void*_tmp5C9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E5);void*_tmp5CA=_tmp5C9;_LL120: _LL121:
# 3412
({int _tmpC0E=in_bnds  || Cyc_Toc_check_bounds(_tmp5BE,_tmp5C6,_tmp68E,_tmp68D);in_bnds=_tmpC0E;});
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp5CD;_tmp5CD.tag=0U;({struct _dyneither_ptr _tmpC0F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5CD.f1=_tmpC0F;});({void*_tmp5CB[1U]={& _tmp5CD};({unsigned int _tmpC11=e->loc;struct _dyneither_ptr _tmpC10=({const char*_tmp5CC="bounds check necessary for %s";_tag_dyneither(_tmp5CC,sizeof(char),30U);});Cyc_Tcutil_warn(_tmpC11,_tmpC10,_tag_dyneither(_tmp5CB,sizeof(void*),1U));});});});_LL11F:;}
# 3419
Cyc_Toc_exp_to_c(nv,_tmp68E);
Cyc_Toc_exp_to_c(nv,_tmp68D);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp5CE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E5);void*_tmp5CF=_tmp5CE;struct Cyc_Absyn_Exp*_tmp5E0;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CF)->tag == 1U){_LL123: _tmp5E0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CF)->f1;_LL124: {
# 3424
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5E6) && _tmp5C7;
void*ta1=Cyc_Toc_typ_to_c(_tmp5E9);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5E8);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5D0=0U;({unsigned int _tmpC13=e->loc;struct _dyneither_ptr _tmpC12=({const char*_tmp5D1="inserted null check due to dereference";_tag_dyneither(_tmp5D1,sizeof(char),39U);});Cyc_Tcutil_warn(_tmpC13,_tmpC12,_tag_dyneither(_tmp5D0,sizeof(void*),0U));});});
({void*_tmpC17=({void*_tmpC16=ta2;Cyc_Toc_cast_it_r(_tmpC16,({struct Cyc_Absyn_Exp*_tmpC15=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmpC15,({struct Cyc_Absyn_Exp*_tmp5D2[1U];({struct Cyc_Absyn_Exp*_tmpC14=
Cyc_Absyn_copy_exp(_tmp68E);_tmp5D2[0U]=_tmpC14;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D2,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 3433
_tmp68E->r=_tmpC17;});}else{
# 3436
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5E4)){
# 3438
if(!Cyc_Evexp_c_can_eval(_tmp5E0))
({void*_tmp5D3=0U;({unsigned int _tmpC19=e->loc;struct _dyneither_ptr _tmpC18=({const char*_tmp5D4="cannot determine subscript is in bounds";_tag_dyneither(_tmp5D4,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC19,_tmpC18,_tag_dyneither(_tmp5D3,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp68E);
({void*_tmpC1C=Cyc_Toc_deref_exp_r(({void*_tmpC1B=ta2;Cyc_Toc_cast_it(_tmpC1B,({
struct Cyc_Absyn_Exp*_tmpC1A=function_e;Cyc_Absyn_fncall_exp(_tmpC1A,({struct Cyc_Absyn_Exp*_tmp5D5[3U];_tmp5D5[2U]=_tmp68D;_tmp5D5[1U]=_tmp5E0;_tmp5D5[0U]=_tmp68E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D5,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 3441
e->r=_tmpC1C;});};}else{
# 3444
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp5E0))
({void*_tmp5D6=0U;({unsigned int _tmpC1E=e->loc;struct _dyneither_ptr _tmpC1D=({const char*_tmp5D7="cannot determine subscript is in bounds";_tag_dyneither(_tmp5D7,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC1E,_tmpC1D,_tag_dyneither(_tmp5D6,sizeof(void*),0U));});});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5D8=0U;({unsigned int _tmpC20=e->loc;struct _dyneither_ptr _tmpC1F=({const char*_tmp5D9="inserted null check due to dereference";_tag_dyneither(_tmp5D9,sizeof(char),39U);});Cyc_Tcutil_warn(_tmpC20,_tmpC1F,_tag_dyneither(_tmp5D8,sizeof(void*),0U));});});
# 3450
({void*_tmpC24=Cyc_Toc_deref_exp_r(({void*_tmpC23=ta2;Cyc_Toc_cast_it(_tmpC23,({
struct Cyc_Absyn_Exp*_tmpC22=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmpC22,({struct Cyc_Absyn_Exp*_tmp5DA[4U];_tmp5DA[3U]=_tmp68D;({struct Cyc_Absyn_Exp*_tmpC21=
# 3453
Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp5DA[2U]=_tmpC21;});_tmp5DA[1U]=_tmp5E0;_tmp5DA[0U]=_tmp68E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5DA,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 3450
e->r=_tmpC24;});}else{
# 3456
if(!Cyc_Evexp_c_can_eval(_tmp5E0))
({void*_tmp5DB=0U;({unsigned int _tmpC26=e->loc;struct _dyneither_ptr _tmpC25=({const char*_tmp5DC="cannot determine subscript is in bounds";_tag_dyneither(_tmp5DC,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC26,_tmpC25,_tag_dyneither(_tmp5DB,sizeof(void*),0U));});});
# 3459
({void*_tmpC29=({struct Cyc_Absyn_Exp*_tmpC28=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmpC28,({struct Cyc_Absyn_Exp*_tmp5DD[2U];({struct Cyc_Absyn_Exp*_tmpC27=
Cyc_Absyn_copy_exp(_tmp68D);_tmp5DD[1U]=_tmpC27;});_tmp5DD[0U]=_tmp5E0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5DD,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 3459
_tmp68D->r=_tmpC29;});}}}}
# 3462
goto _LL122;}}else{_LL125: _LL126: {
# 3464
void*ta1=Cyc_Toc_typ_to_c(_tmp5E9);
if(in_bnds){
# 3468
++ Cyc_Toc_bounds_checks_eliminated;
({void*_tmpC2C=({struct Cyc_Absyn_Exp*_tmpC2B=({void*_tmpC2A=Cyc_Absyn_cstar_typ(ta1,_tmp5E8);Cyc_Toc_cast_it(_tmpC2A,
Cyc_Toc_member_exp(_tmp68E,Cyc_Toc_curr_sp,0U));});
# 3469
Cyc_Toc_subscript_exp_r(_tmpC2B,_tmp68D);});e->r=_tmpC2C;});}else{
# 3473
struct Cyc_Absyn_Exp*_tmp5DE=Cyc_Toc__check_dyneither_subscript_e;
({void*_tmpC30=Cyc_Toc_deref_exp_r(({void*_tmpC2F=Cyc_Absyn_cstar_typ(ta1,_tmp5E8);Cyc_Toc_cast_it(_tmpC2F,({
struct Cyc_Absyn_Exp*_tmpC2E=_tmp5DE;Cyc_Absyn_fncall_exp(_tmpC2E,({struct Cyc_Absyn_Exp*_tmp5DF[3U];_tmp5DF[2U]=_tmp68D;({struct Cyc_Absyn_Exp*_tmpC2D=
Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp5DF[1U]=_tmpC2D;});_tmp5DF[0U]=_tmp68E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5DF,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 3474
e->r=_tmpC30;});}
# 3479
goto _LL122;}}_LL122:;}
# 3481
goto _LL115;}default: _LL11A: _LL11B:
({void*_tmp5E1=0U;({struct _dyneither_ptr _tmpC31=({const char*_tmp5E2="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp5E2,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC31,_tag_dyneither(_tmp5E1,sizeof(void*),0U));});});}_LL115:;}
# 3484
Cyc_Toc_set_lhs(nv,_tmp5BD);
goto _LL5;};}case 24U: _LL3A: _tmp68F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL3B:
# 3487
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp5EA=Cyc_Toc_init_tuple(nv,0,0,_tmp68F);
e->r=_tmp5EA->r;
e->topt=_tmp5EA->topt;}else{
# 3495
struct Cyc_List_List*_tmp5EB=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp68F);
void*_tmp5EC=Cyc_Toc_add_tuple_type(_tmp5EB);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp68F != 0;(_tmp68F=_tmp68F->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp68F->hd);
({struct Cyc_List_List*_tmpC33=({struct Cyc_List_List*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));({struct _tuple19*_tmpC32=({struct _tuple19*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->f1=0;_tmp5EE->f2=(struct Cyc_Absyn_Exp*)_tmp68F->hd;_tmp5EE;});_tmp5ED->hd=_tmpC32;});_tmp5ED->tl=dles;_tmp5ED;});dles=_tmpC33;});}}
# 3502
({struct Cyc_List_List*_tmpC34=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);dles=_tmpC34;});
({void*_tmpC35=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmpC35;});}
# 3505
goto _LL5;case 26U: _LL3C: _tmp690=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL3D:
# 3509
({void*_tmpC36=Cyc_Toc_unresolvedmem_exp_r(0,_tmp690);e->r=_tmpC36;});
{struct Cyc_List_List*_tmp5EF=_tmp690;for(0;_tmp5EF != 0;_tmp5EF=_tmp5EF->tl){
struct _tuple19*_tmp5F0=(struct _tuple19*)_tmp5EF->hd;struct _tuple19*_tmp5F1=_tmp5F0;struct Cyc_Absyn_Exp*_tmp5F2;_LL128: _tmp5F2=_tmp5F1->f2;_LL129:;
Cyc_Toc_exp_to_c(nv,_tmp5F2);}}
# 3514
goto _LL5;case 27U: _LL3E: _tmp694=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp693=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp692=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp691=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL3F: {
# 3518
struct _tuple12 _tmp5F3=Cyc_Evexp_eval_const_uint_exp(_tmp693);struct _tuple12 _tmp5F4=_tmp5F3;unsigned int _tmp5FE;int _tmp5FD;_LL12B: _tmp5FE=_tmp5F4.f1;_tmp5FD=_tmp5F4.f2;_LL12C:;{
void*_tmp5F5=Cyc_Toc_typ_to_c((void*)_check_null(_tmp692->topt));
Cyc_Toc_exp_to_c(nv,_tmp692);{
struct Cyc_List_List*es=0;
# 3523
if(!Cyc_Toc_is_zero(_tmp692)){
if(!_tmp5FD)
({void*_tmp5F6=0U;({unsigned int _tmpC38=_tmp693->loc;struct _dyneither_ptr _tmpC37=({const char*_tmp5F7="cannot determine value of constant";_tag_dyneither(_tmp5F7,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpC38,_tmpC37,_tag_dyneither(_tmp5F6,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp5FE;++ i){
({struct Cyc_List_List*_tmpC3A=({struct Cyc_List_List*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));({struct _tuple19*_tmpC39=({struct _tuple19*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->f1=0;_tmp5F9->f2=_tmp692;_tmp5F9;});_tmp5F8->hd=_tmpC39;});_tmp5F8->tl=es;_tmp5F8;});es=_tmpC3A;});}}
# 3529
if(_tmp691){
struct Cyc_Absyn_Exp*_tmp5FA=({void*_tmpC3B=_tmp5F5;Cyc_Toc_cast_it(_tmpC3B,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmpC3E=({struct Cyc_List_List*_tmpC3D=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC3D,({struct Cyc_List_List*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));({struct _tuple19*_tmpC3C=({struct _tuple19*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->f1=0;_tmp5FC->f2=_tmp5FA;_tmp5FC;});_tmp5FB->hd=_tmpC3C;});_tmp5FB->tl=0;_tmp5FB;}));});es=_tmpC3E;});}}
# 3534
({void*_tmpC3F=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmpC3F;});
goto _LL5;};};}case 28U: _LL40: _tmp697=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp696=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp695=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL41:
# 3538
({void*_tmpC40=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmpC40;});
goto _LL5;case 29U: _LL42: _tmp69B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp69A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp699=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp698=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL43:
# 3543
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp5FF=Cyc_Toc_init_struct(nv,old_typ,_tmp69A,0,0,_tmp699,_tmp69B);
e->r=_tmp5FF->r;
e->topt=_tmp5FF->topt;}else{
# 3554
if(_tmp698 == 0)
({void*_tmp600=0U;({struct _dyneither_ptr _tmpC41=({const char*_tmp601="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp601,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC41,_tag_dyneither(_tmp600,sizeof(void*),0U));});});{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp698;
# 3558
struct _RegionHandle _tmp602=_new_region("rgn");struct _RegionHandle*rgn=& _tmp602;_push_region(rgn);
{struct Cyc_List_List*_tmp603=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp699,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3562
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp698->impl))->tagged){
# 3564
struct _tuple30*_tmp604=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp603))->hd;struct _tuple30*_tmp605=_tmp604;struct Cyc_Absyn_Aggrfield*_tmp612;struct Cyc_Absyn_Exp*_tmp611;_LL12E: _tmp612=_tmp605->f1;_tmp611=_tmp605->f2;_LL12F:;{
void*_tmp606=(void*)_check_null(_tmp611->topt);
void*_tmp607=_tmp612->type;
Cyc_Toc_exp_to_c(nv,_tmp611);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp607) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp606))
({void*_tmpC43=({
void*_tmpC42=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpC42,Cyc_Absyn_new_exp(_tmp611->r,0U));});
# 3570
_tmp611->r=_tmpC43;});{
# 3573
int i=Cyc_Toc_get_member_offset(_tmp698,_tmp612->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp608=({struct _tuple19*_tmp60E[2U];({struct _tuple19*_tmpC44=({struct _tuple19*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->f1=0;_tmp610->f2=_tmp611;_tmp610;});_tmp60E[1U]=_tmpC44;});({struct _tuple19*_tmpC45=({struct _tuple19*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->f1=0;_tmp60F->f2=field_tag_exp;_tmp60F;});_tmp60E[0U]=_tmpC45;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60E,sizeof(struct _tuple19*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp608,0U);
struct Cyc_List_List*ds=({void*_tmp60B[1U];({void*_tmpC47=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC46=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp60D;_tmp60D.tag=1U;_tmp60D.f1=_tmp612->name;_tmp60D;});_tmp60C[0]=_tmpC46;});_tmp60C;});_tmp60B[0U]=_tmpC47;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60B,sizeof(void*),1U));});
struct Cyc_List_List*dles=({struct _tuple19*_tmp609[1U];({struct _tuple19*_tmpC48=({struct _tuple19*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->f1=ds;_tmp60A->f2=umem;_tmp60A;});_tmp609[0U]=_tmpC48;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp609,sizeof(struct _tuple19*),1U));});
({void*_tmpC49=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmpC49;});};};}else{
# 3582
struct Cyc_List_List*_tmp613=0;
struct Cyc_List_List*_tmp614=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp614 != 0;_tmp614=_tmp614->tl){
struct Cyc_List_List*_tmp615=_tmp603;for(0;_tmp615 != 0;_tmp615=_tmp615->tl){
if((*((struct _tuple30*)_tmp615->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp614->hd){
struct _tuple30*_tmp616=(struct _tuple30*)_tmp615->hd;struct _tuple30*_tmp617=_tmp616;struct Cyc_Absyn_Aggrfield*_tmp61D;struct Cyc_Absyn_Exp*_tmp61C;_LL131: _tmp61D=_tmp617->f1;_tmp61C=_tmp617->f2;_LL132:;{
void*_tmp618=Cyc_Toc_typ_to_c(_tmp61D->type);
void*_tmp619=Cyc_Toc_typ_to_c((void*)_check_null(_tmp61C->topt));
Cyc_Toc_exp_to_c(nv,_tmp61C);
# 3592
if(!Cyc_Tcutil_unify(_tmp618,_tmp619)){
# 3594
if(!Cyc_Tcutil_is_arithmetic_type(_tmp618) || !
Cyc_Tcutil_is_arithmetic_type(_tmp619))
({struct Cyc_Absyn_Exp*_tmpC4B=({void*_tmpC4A=_tmp618;Cyc_Toc_cast_it(_tmpC4A,Cyc_Absyn_copy_exp(_tmp61C));});_tmp61C=_tmpC4B;});}
({struct Cyc_List_List*_tmpC4D=({struct Cyc_List_List*_tmp61A=_cycalloc(sizeof(*_tmp61A));({struct _tuple19*_tmpC4C=({struct _tuple19*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->f1=0;_tmp61B->f2=_tmp61C;_tmp61B;});_tmp61A->hd=_tmpC4C;});_tmp61A->tl=_tmp613;_tmp61A;});_tmp613=_tmpC4D;});
break;};}}}
# 3601
({void*_tmpC4E=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp613));e->r=_tmpC4E;});}}
# 3559
;_pop_region(rgn);};}
# 3605
goto _LL5;case 30U: _LL44: _tmp69D=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp69C=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL45: {
# 3607
struct Cyc_List_List*fs;
{void*_tmp61E=Cyc_Tcutil_compress(_tmp69D);void*_tmp61F=_tmp61E;struct Cyc_List_List*_tmp623;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp61F)->tag == 12U){_LL134: _tmp623=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp61F)->f2;_LL135:
 fs=_tmp623;goto _LL133;}else{_LL136: _LL137:
({struct Cyc_String_pa_PrintArg_struct _tmp622;_tmp622.tag=0U;({struct _dyneither_ptr _tmpC4F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp69D));_tmp622.f1=_tmpC4F;});({void*_tmp620[1U]={& _tmp622};({struct _dyneither_ptr _tmpC50=({const char*_tmp621="anon struct has type %s";_tag_dyneither(_tmp621,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC50,_tag_dyneither(_tmp620,sizeof(void*),1U));});});});}_LL133:;}{
# 3612
struct _RegionHandle _tmp624=_new_region("rgn");struct _RegionHandle*rgn=& _tmp624;_push_region(rgn);{
struct Cyc_List_List*_tmp625=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp69C,Cyc_Absyn_StructA,fs);
for(0;_tmp625 != 0;_tmp625=_tmp625->tl){
struct _tuple30*_tmp626=(struct _tuple30*)_tmp625->hd;struct _tuple30*_tmp627=_tmp626;struct Cyc_Absyn_Aggrfield*_tmp62B;struct Cyc_Absyn_Exp*_tmp62A;_LL139: _tmp62B=_tmp627->f1;_tmp62A=_tmp627->f2;_LL13A:;{
void*_tmp628=(void*)_check_null(_tmp62A->topt);
void*_tmp629=_tmp62B->type;
Cyc_Toc_exp_to_c(nv,_tmp62A);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp629) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp628))
({void*_tmpC52=({void*_tmpC51=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpC51,Cyc_Absyn_new_exp(_tmp62A->r,0U));});_tmp62A->r=_tmpC52;});};}
# 3626
({void*_tmpC53=Cyc_Toc_unresolvedmem_exp_r(0,_tmp69C);e->r=_tmpC53;});}
# 3628
_npop_handler(0U);goto _LL5;
# 3612
;_pop_region(rgn);};}case 31U: _LL46: _tmp6A0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp69F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp69E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL47: {
# 3631
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp62C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp62D=Cyc_Absyn_var_exp(_tmp62C,0U);
struct Cyc_Absyn_Exp*mem_exp;
({void*_tmpC54=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp69E->name,_tmp69F->name));datatype_ctype=_tmpC54;});
({struct Cyc_Absyn_Exp*_tmpC55=_tmp69F->is_extensible?Cyc_Absyn_var_exp(_tmp69E->name,0U):
 Cyc_Toc_datatype_tag(_tmp69F,_tmp69E->name);
# 3637
tag_exp=_tmpC55;});
# 3639
mem_exp=_tmp62D;{
struct Cyc_List_List*_tmp62E=_tmp69E->typs;
# 3642
if(Cyc_Toc_is_toplevel(nv)){
# 3644
struct Cyc_List_List*dles=0;
for(0;_tmp6A0 != 0;(_tmp6A0=_tmp6A0->tl,_tmp62E=_tmp62E->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp6A0->hd;
void*_tmp62F=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp62E))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp62F))
({struct Cyc_Absyn_Exp*_tmpC56=Cyc_Toc_cast_it(field_typ,cur_e);cur_e=_tmpC56;});
({struct Cyc_List_List*_tmpC58=({struct Cyc_List_List*_tmp630=_cycalloc(sizeof(*_tmp630));({struct _tuple19*_tmpC57=({struct _tuple19*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->f1=0;_tmp631->f2=cur_e;_tmp631;});_tmp630->hd=_tmpC57;});_tmp630->tl=dles;_tmp630;});dles=_tmpC58;});}
# 3655
({struct Cyc_List_List*_tmpC5B=({struct Cyc_List_List*_tmp632=_cycalloc(sizeof(*_tmp632));({struct _tuple19*_tmpC59=({struct _tuple19*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->f1=0;_tmp633->f2=tag_exp;_tmp633;});_tmp632->hd=_tmpC59;});({struct Cyc_List_List*_tmpC5A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp632->tl=_tmpC5A;});_tmp632;});dles=_tmpC5B;});
({void*_tmpC5C=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmpC5C;});}else{
# 3661
struct Cyc_List_List*_tmp634=({struct Cyc_List_List*_tmp63B=_cycalloc(sizeof(*_tmp63B));({struct Cyc_Absyn_Stmt*_tmpC5E=({
struct Cyc_Absyn_Exp*_tmpC5D=Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0U);Cyc_Absyn_assign_stmt(_tmpC5D,tag_exp,0U);});_tmp63B->hd=_tmpC5E;});_tmp63B->tl=0;_tmp63B;});
# 3664
{int i=1;for(0;_tmp6A0 != 0;(((_tmp6A0=_tmp6A0->tl,i ++)),_tmp62E=_tmp62E->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp6A0->hd;
void*_tmp635=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp62E))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp635))
({struct Cyc_Absyn_Exp*_tmpC5F=Cyc_Toc_cast_it(field_typ,cur_e);cur_e=_tmpC5F;});{
struct Cyc_Absyn_Stmt*_tmp636=({struct Cyc_Absyn_Exp*_tmpC61=({struct Cyc_Absyn_Exp*_tmpC60=mem_exp;Cyc_Toc_member_exp(_tmpC60,Cyc_Absyn_fieldname(i),0U);});Cyc_Absyn_assign_stmt(_tmpC61,cur_e,0U);});
# 3674
({struct Cyc_List_List*_tmpC62=({struct Cyc_List_List*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->hd=_tmp636;_tmp637->tl=_tmp634;_tmp637;});_tmp634=_tmpC62;});};}}{
# 3676
struct Cyc_Absyn_Stmt*_tmp638=Cyc_Absyn_exp_stmt(_tmp62D,0U);
struct Cyc_Absyn_Stmt*_tmp639=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->hd=_tmp638;_tmp63A->tl=_tmp634;_tmp63A;})),0U);
({void*_tmpC63=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp62C,datatype_ctype,0,_tmp639,0U));e->r=_tmpC63;});};}
# 3680
goto _LL5;};}case 32U: _LL48: _LL49:
# 3682
 goto _LL4B;case 33U: _LL4A: _LL4B:
 goto _LL5;case 34U: _LL4C: _tmp6A6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).is_calloc;_tmp6A5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).rgn;_tmp6A4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).elt_type;_tmp6A3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).num_elts;_tmp6A2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).fat_result;_tmp6A1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).inline_call;_LL4D: {
# 3686
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp6A4)));
Cyc_Toc_exp_to_c(nv,_tmp6A3);
# 3690
if(_tmp6A2){
struct _tuple1*_tmp63C=Cyc_Toc_temp_var();
struct _tuple1*_tmp63D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp6A6){
xexp=_tmp6A3;
if(_tmp6A5 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A5;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmpC66=({struct Cyc_Absyn_Exp*_tmpC65=rgn;struct Cyc_Absyn_Exp*_tmpC64=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpC65,_tmpC64,Cyc_Absyn_var_exp(_tmp63C,0U));});pexp=_tmpC66;});}else{
# 3701
({struct Cyc_Absyn_Exp*_tmpC69=({void*_tmpC68=*_tmp6A4;struct Cyc_Absyn_Exp*_tmpC67=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpC68,_tmpC67,Cyc_Absyn_var_exp(_tmp63C,0U));});pexp=_tmpC69;});}
# 3703
({struct Cyc_Absyn_Exp*_tmpC6E=({struct Cyc_Absyn_Exp*_tmpC6D=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpC6D,({struct Cyc_Absyn_Exp*_tmp63E[3U];({struct Cyc_Absyn_Exp*_tmpC6A=
# 3705
Cyc_Absyn_var_exp(_tmp63C,0U);_tmp63E[2U]=_tmpC6A;});({struct Cyc_Absyn_Exp*_tmpC6B=
# 3704
Cyc_Absyn_sizeoftyp_exp(t_c,0U);_tmp63E[1U]=_tmpC6B;});({struct Cyc_Absyn_Exp*_tmpC6C=Cyc_Absyn_var_exp(_tmp63D,0U);_tmp63E[0U]=_tmpC6C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp63E,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3703
rexp=_tmpC6E;});}else{
# 3707
if(_tmp6A5 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A5;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp6A1)
({struct Cyc_Absyn_Exp*_tmpC70=({struct Cyc_Absyn_Exp*_tmpC6F=rgn;Cyc_Toc_rmalloc_inline_exp(_tmpC6F,Cyc_Absyn_var_exp(_tmp63C,0U));});pexp=_tmpC70;});else{
# 3713
({struct Cyc_Absyn_Exp*_tmpC72=({struct Cyc_Absyn_Exp*_tmpC71=rgn;Cyc_Toc_rmalloc_exp(_tmpC71,Cyc_Absyn_var_exp(_tmp63C,0U));});pexp=_tmpC72;});}}else{
# 3715
({struct Cyc_Absyn_Exp*_tmpC74=({void*_tmpC73=*_tmp6A4;Cyc_Toc_malloc_exp(_tmpC73,Cyc_Absyn_var_exp(_tmp63C,0U));});pexp=_tmpC74;});}
# 3717
({struct Cyc_Absyn_Exp*_tmpC79=({struct Cyc_Absyn_Exp*_tmpC78=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpC78,({struct Cyc_Absyn_Exp*_tmp63F[3U];({struct Cyc_Absyn_Exp*_tmpC75=
Cyc_Absyn_var_exp(_tmp63C,0U);_tmp63F[2U]=_tmpC75;});({struct Cyc_Absyn_Exp*_tmpC76=
# 3717
Cyc_Absyn_uint_exp(1U,0U);_tmp63F[1U]=_tmpC76;});({struct Cyc_Absyn_Exp*_tmpC77=Cyc_Absyn_var_exp(_tmp63D,0U);_tmp63F[0U]=_tmpC77;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp63F,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});rexp=_tmpC79;});}{
# 3720
struct Cyc_Absyn_Stmt*_tmp640=({struct _tuple1*_tmpC7F=_tmp63C;void*_tmpC7E=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpC7D=_tmp6A3;Cyc_Absyn_declare_stmt(_tmpC7F,_tmpC7E,_tmpC7D,({
struct _tuple1*_tmpC7C=_tmp63D;void*_tmpC7B=Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpC7A=pexp;Cyc_Absyn_declare_stmt(_tmpC7C,_tmpC7B,_tmpC7A,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmpC80=Cyc_Toc_stmt_exp_r(_tmp640);e->r=_tmpC80;});};}else{
# 3725
if(_tmp6A6){
if(_tmp6A5 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A5;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmpC83=({struct Cyc_Absyn_Exp*_tmpC82=rgn;struct Cyc_Absyn_Exp*_tmpC81=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpC82,_tmpC81,_tmp6A3);})->r;e->r=_tmpC83;});}else{
# 3731
({void*_tmpC86=({void*_tmpC85=*_tmp6A4;struct Cyc_Absyn_Exp*_tmpC84=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpC85,_tmpC84,_tmp6A3);})->r;e->r=_tmpC86;});}}else{
# 3734
if(_tmp6A5 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A5;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp6A1)
({void*_tmpC87=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp6A3))->r;e->r=_tmpC87;});else{
# 3740
({void*_tmpC88=(Cyc_Toc_rmalloc_exp(rgn,_tmp6A3))->r;e->r=_tmpC88;});}}else{
# 3742
({void*_tmpC89=(Cyc_Toc_malloc_exp(*_tmp6A4,_tmp6A3))->r;e->r=_tmpC89;});}}}
# 3746
goto _LL5;}case 35U: _LL4E: _tmp6A8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp6A7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL4F: {
# 3755
void*e1_old_typ=(void*)_check_null(_tmp6A8->topt);
void*e2_old_typ=(void*)_check_null(_tmp6A7->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp641=0U;({struct _dyneither_ptr _tmpC8A=({const char*_tmp642="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp642,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC8A,_tag_dyneither(_tmp641,sizeof(void*),0U));});});{
# 3763
unsigned int _tmp643=e->loc;
struct _tuple1*_tmp644=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp645=Cyc_Absyn_var_exp(_tmp644,_tmp643);_tmp645->topt=e1_old_typ;{
struct _tuple1*_tmp646=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp647=Cyc_Absyn_var_exp(_tmp646,_tmp643);_tmp647->topt=e2_old_typ;{
# 3769
struct Cyc_Absyn_Exp*_tmp648=({struct Cyc_Absyn_Exp*_tmpC8C=Cyc_Tcutil_deep_copy_exp(1,_tmp6A8);struct Cyc_Absyn_Exp*_tmpC8B=_tmp647;Cyc_Absyn_assign_exp(_tmpC8C,_tmpC8B,_tmp643);});_tmp648->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp649=Cyc_Absyn_exp_stmt(_tmp648,_tmp643);
struct Cyc_Absyn_Exp*_tmp64A=({struct Cyc_Absyn_Exp*_tmpC8E=Cyc_Tcutil_deep_copy_exp(1,_tmp6A7);struct Cyc_Absyn_Exp*_tmpC8D=_tmp645;Cyc_Absyn_assign_exp(_tmpC8E,_tmpC8D,_tmp643);});_tmp64A->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp64B=Cyc_Absyn_exp_stmt(_tmp64A,_tmp643);
# 3774
struct Cyc_Absyn_Stmt*_tmp64C=({struct _tuple1*_tmpC96=_tmp644;void*_tmpC95=e1_old_typ;struct Cyc_Absyn_Exp*_tmpC94=_tmp6A8;struct Cyc_Absyn_Stmt*_tmpC93=({
struct _tuple1*_tmpC92=_tmp646;void*_tmpC91=e2_old_typ;struct Cyc_Absyn_Exp*_tmpC90=_tmp6A7;struct Cyc_Absyn_Stmt*_tmpC8F=
Cyc_Absyn_seq_stmt(_tmp649,_tmp64B,_tmp643);
# 3775
Cyc_Absyn_declare_stmt(_tmpC92,_tmpC91,_tmpC90,_tmpC8F,_tmp643);});
# 3774
Cyc_Absyn_declare_stmt(_tmpC96,_tmpC95,_tmpC94,_tmpC93,_tmp643);});
# 3777
Cyc_Toc_stmt_to_c(nv,_tmp64C);
({void*_tmpC97=Cyc_Toc_stmt_exp_r(_tmp64C);e->r=_tmpC97;});
goto _LL5;};};};};};}case 38U: _LL50: _tmp6AA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp6A9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL51: {
# 3782
void*_tmp64D=Cyc_Tcutil_compress((void*)_check_null(_tmp6AA->topt));
Cyc_Toc_exp_to_c(nv,_tmp6AA);
{void*_tmp64E=_tmp64D;struct Cyc_Absyn_Aggrdecl*_tmp654;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp64E)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp64E)->f1).aggr_info).KnownAggr).tag == 2){_LL13C: _tmp654=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp64E)->f1).aggr_info).KnownAggr).val;_LL13D: {
# 3786
struct Cyc_Absyn_Exp*_tmp64F=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp654,_tmp6A9),0U);
struct Cyc_Absyn_Exp*_tmp650=Cyc_Toc_member_exp(_tmp6AA,_tmp6A9,0U);
struct Cyc_Absyn_Exp*_tmp651=Cyc_Toc_member_exp(_tmp650,Cyc_Toc_tag_sp,0U);
({void*_tmpC98=(Cyc_Absyn_eq_exp(_tmp651,_tmp64F,0U))->r;e->r=_tmpC98;});
goto _LL13B;}}else{goto _LL13E;}}else{_LL13E: _LL13F:
({void*_tmp652=0U;({struct _dyneither_ptr _tmpC99=({const char*_tmp653="non-aggregate type in tagcheck";_tag_dyneither(_tmp653,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC99,_tag_dyneither(_tmp652,sizeof(void*),0U));});});}_LL13B:;}
# 3793
goto _LL5;}case 37U: _LL52: _tmp6AB=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL53:
 Cyc_Toc_stmt_to_c(nv,_tmp6AB);goto _LL5;case 36U: _LL54: _LL55:
({void*_tmp655=0U;({struct _dyneither_ptr _tmpC9A=({const char*_tmp656="UnresolvedMem";_tag_dyneither(_tmp656,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC9A,_tag_dyneither(_tmp655,sizeof(void*),0U));});});case 25U: _LL56: _LL57:
({void*_tmp657=0U;({struct _dyneither_ptr _tmpC9B=({const char*_tmp658="compoundlit";_tag_dyneither(_tmp658,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpC9B,_tag_dyneither(_tmp657,sizeof(void*),0U));});});case 39U: _LL58: _LL59:
({void*_tmp659=0U;({struct _dyneither_ptr _tmpC9C=({const char*_tmp65A="valueof(-)";_tag_dyneither(_tmp65A,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC9C,_tag_dyneither(_tmp659,sizeof(void*),0U));});});default: _LL5A: _LL5B:
({void*_tmp65B=0U;({struct _dyneither_ptr _tmpC9D=({const char*_tmp65C="__asm__";_tag_dyneither(_tmp65C,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpC9D,_tag_dyneither(_tmp65B,sizeof(void*),0U));});});}_LL5:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3826 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3828
return({struct _tuple31*_tmp6AC=_region_malloc(r,sizeof(*_tmp6AC));_tmp6AC->f1=0;({struct _dyneither_ptr*_tmpC9E=Cyc_Toc_fresh_label();_tmp6AC->f2=_tmpC9E;});({struct _dyneither_ptr*_tmpC9F=Cyc_Toc_fresh_label();_tmp6AC->f3=_tmpC9F;});_tmp6AC->f4=sc;_tmp6AC;});}
# 3833
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp6AD=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3839
if((int)(((_tmp6AD->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp6B0=_tmp6AD->orig_pat;if((_tmp6B0.pattern).tag != 1)_throw_match();(_tmp6B0.pattern).val;})->topt);
void*_tmp6AE=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp6AF=_tmp6AE;switch(*((int*)_tmp6AF)){case 0U: _LL1: _LL2:
# 3844
 goto _LL4;case 11U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpCA1=({void*_tmpCA0=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpCA0,v);});v=_tmpCA1;});goto _LL0;}_LL0:;}{
# 3850
void*_tmp6B1=_tmp6AD->access;void*_tmp6B2=_tmp6B1;struct Cyc_Absyn_Datatypedecl*_tmp6BD;struct Cyc_Absyn_Datatypefield*_tmp6BC;unsigned int _tmp6BB;int _tmp6BA;struct _dyneither_ptr*_tmp6B9;unsigned int _tmp6B8;switch(*((int*)_tmp6B2)){case 0U: _LLA: _LLB:
# 3855
 goto _LL9;case 1U: _LLC: _LLD:
# 3860
 if(ps->tl != 0){
void*_tmp6B3=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp6B4=_tmp6B3;struct Cyc_Absyn_Datatypedecl*_tmp6B7;struct Cyc_Absyn_Datatypefield*_tmp6B6;unsigned int _tmp6B5;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B4)->tag == 3U){_LL15: _tmp6B7=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B4)->f1;_tmp6B6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B4)->f2;_tmp6B5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B4)->f3;_LL16:
# 3863
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpCA4=({void*_tmpCA3=({void*_tmpCA2=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6B6->name,_tmp6B7->name));Cyc_Absyn_cstar_typ(_tmpCA2,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpCA3,v);});v=_tmpCA4;});
({struct Cyc_Absyn_Exp*_tmpCA6=({struct Cyc_Absyn_Exp*_tmpCA5=v;Cyc_Absyn_aggrarrow_exp(_tmpCA5,Cyc_Absyn_fieldname((int)(_tmp6B5 + 1)),0U);});v=_tmpCA6;});
continue;}else{_LL17: _LL18:
# 3868
 goto _LL14;}_LL14:;}
# 3871
({struct Cyc_Absyn_Exp*_tmpCA7=Cyc_Absyn_deref_exp(v,0U);v=_tmpCA7;});
goto _LL9;case 2U: _LLE: _tmp6B8=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp6B2)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpCA9=({struct Cyc_Absyn_Exp*_tmpCA8=v;Cyc_Toc_member_exp(_tmpCA8,Cyc_Absyn_fieldname((int)(_tmp6B8 + 1)),0U);});v=_tmpCA9;});goto _LL9;case 4U: _LL10: _tmp6BA=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6B2)->f1;_tmp6B9=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6B2)->f2;_LL11:
# 3875
({struct Cyc_Absyn_Exp*_tmpCAA=Cyc_Toc_member_exp(v,_tmp6B9,0U);v=_tmpCAA;});
if(_tmp6BA)
({struct Cyc_Absyn_Exp*_tmpCAB=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpCAB;});
goto _LL9;default: _LL12: _tmp6BD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B2)->f1;_tmp6BC=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B2)->f2;_tmp6BB=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B2)->f3;_LL13:
# 3880
({struct Cyc_Absyn_Exp*_tmpCAD=({struct Cyc_Absyn_Exp*_tmpCAC=v;Cyc_Toc_member_exp(_tmpCAC,Cyc_Absyn_fieldname((int)(_tmp6BB + 1)),0U);});v=_tmpCAD;});
goto _LL9;}_LL9:;};}
# 3884
return v;}
# 3889
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp6BE=t;struct Cyc_Absyn_Datatypedecl*_tmp6D9;struct Cyc_Absyn_Datatypefield*_tmp6D8;struct _dyneither_ptr*_tmp6D7;int _tmp6D6;int _tmp6D5;struct Cyc_Absyn_Datatypedecl*_tmp6D4;struct Cyc_Absyn_Datatypefield*_tmp6D3;unsigned int _tmp6D2;struct _dyneither_ptr _tmp6D1;int _tmp6D0;void*_tmp6CF;struct Cyc_Absyn_Enumfield*_tmp6CE;struct Cyc_Absyn_Enumdecl*_tmp6CD;struct Cyc_Absyn_Enumfield*_tmp6CC;struct Cyc_Absyn_Exp*_tmp6CB;switch(*((int*)_tmp6BE)){case 0U: _LL1: _tmp6CB=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6BE)->f1;_LL2:
# 3893
 if(_tmp6CB == 0)return v;else{return _tmp6CB;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpCAE=v;Cyc_Absyn_eq_exp(_tmpCAE,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpCAF=v;Cyc_Absyn_neq_exp(_tmpCAF,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp6CD=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6BE)->f1;_tmp6CC=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6BE)->f2;_LL8:
# 3897
 return({struct Cyc_Absyn_Exp*_tmpCB1=v;Cyc_Absyn_eq_exp(_tmpCB1,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCB0=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6C0;_tmp6C0.tag=32U;_tmp6C0.f1=_tmp6CD;_tmp6C0.f2=_tmp6CC;_tmp6C0;});_tmp6BF[0]=_tmpCB0;});_tmp6BF;}),0U),0U);});case 4U: _LL9: _tmp6CF=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6BE)->f1;_tmp6CE=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6BE)->f2;_LLA:
# 3899
 return({struct Cyc_Absyn_Exp*_tmpCB3=v;Cyc_Absyn_eq_exp(_tmpCB3,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCB2=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6C2;_tmp6C2.tag=33U;_tmp6C2.f1=_tmp6CF;_tmp6C2.f2=_tmp6CE;_tmp6C2;});_tmp6C1[0]=_tmpCB2;});_tmp6C1;}),0U),0U);});case 5U: _LLB: _tmp6D1=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6BE)->f1;_tmp6D0=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6BE)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpCB4=v;Cyc_Absyn_eq_exp(_tmpCB4,Cyc_Absyn_float_exp(_tmp6D1,_tmp6D0,0U),0U);});case 6U: _LLD: _tmp6D2=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6BE)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpCB5=v;Cyc_Absyn_eq_exp(_tmpCB5,Cyc_Absyn_signed_int_exp((int)_tmp6D2,0U),0U);});case 7U: _LLF: _tmp6D5=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6BE)->f1;_tmp6D4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6BE)->f2;_tmp6D3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6BE)->f3;_LL10:
# 3905
 LOOP1: {
void*_tmp6C3=v->r;void*_tmp6C4=_tmp6C3;struct Cyc_Absyn_Exp*_tmp6C6;struct Cyc_Absyn_Exp*_tmp6C5;switch(*((int*)_tmp6C4)){case 14U: _LL16: _tmp6C5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6C4)->f2;_LL17:
 v=_tmp6C5;goto LOOP1;case 20U: _LL18: _tmp6C6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6C4)->f1;_LL19:
 v=_tmp6C6;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3912
({struct Cyc_Absyn_Exp*_tmpCB8=({void*_tmpCB7=({void*_tmpCB6=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6D3->name,_tmp6D4->name));Cyc_Absyn_cstar_typ(_tmpCB6,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpCB7,v);});v=_tmpCB8;});
return({struct Cyc_Absyn_Exp*_tmpCB9=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpCB9,Cyc_Absyn_uint_exp((unsigned int)_tmp6D5,0U),0U);});case 8U: _LL11: _tmp6D7=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6BE)->f1;_tmp6D6=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6BE)->f2;_LL12:
# 3917
({struct Cyc_Absyn_Exp*_tmpCBB=({struct Cyc_Absyn_Exp*_tmpCBA=Cyc_Toc_member_exp(v,_tmp6D7,0U);Cyc_Toc_member_exp(_tmpCBA,Cyc_Toc_tag_sp,0U);});v=_tmpCBB;});
return({struct Cyc_Absyn_Exp*_tmpCBC=v;Cyc_Absyn_eq_exp(_tmpCBC,Cyc_Absyn_signed_int_exp(_tmp6D6,0U),0U);});default: _LL13: _tmp6D9=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6BE)->f1;_tmp6D8=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6BE)->f2;_LL14:
# 3921
 LOOP2: {
void*_tmp6C7=v->r;void*_tmp6C8=_tmp6C7;struct Cyc_Absyn_Exp*_tmp6CA;struct Cyc_Absyn_Exp*_tmp6C9;switch(*((int*)_tmp6C8)){case 14U: _LL1D: _tmp6C9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6C8)->f2;_LL1E:
 v=_tmp6C9;goto LOOP2;case 20U: _LL1F: _tmp6CA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6C8)->f1;_LL20:
 v=_tmp6CA;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3928
({struct Cyc_Absyn_Exp*_tmpCBF=({void*_tmpCBE=({void*_tmpCBD=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6D8->name,_tmp6D9->name));Cyc_Absyn_cstar_typ(_tmpCBD,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpCBE,v);});v=_tmpCBF;});
return({struct Cyc_Absyn_Exp*_tmpCC0=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpCC0,Cyc_Absyn_var_exp(_tmp6D8->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3942
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple32{int f1;void*f2;};
# 3950
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3952
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp6DC=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp6DD=_tmp6DC;void*_tmp6E7;_LL1: _tmp6E7=_tmp6DD.f1;_LL2:;{
void*_tmp6DE=_tmp6E7;struct Cyc_Absyn_Exp*_tmp6E6;struct _dyneither_ptr*_tmp6E5;switch(*((int*)_tmp6DE)){case 3U: _LL4: _LL5:
# 3958
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3961
 continue;case 8U: _LLA: _tmp6E5=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6DE)->f1;_LLB:
# 3963
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpCC2=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp6DF=_cycalloc(sizeof(*_tmp6DF));({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmpCC1=({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmp6E0;_tmp6E0.tag=3U;_tmp6E0.f1=_tmp6E5;_tmp6E0;});_tmp6DF[0]=_tmpCC1;});_tmp6DF;});k=_tmpCC2;});
continue;case 7U: _LLC: _LLD:
# 3967
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp6E6=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6DE)->f1;if(_tmp6E6 != 0){_LLF:
# 3971
({void*_tmpCC4=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp6E1=_cycalloc(sizeof(*_tmp6E1));({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmpCC3=({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmp6E2;_tmp6E2.tag=2U;_tmp6E2.f1=_tmp6E6;_tmp6E2;});_tmp6E1[0]=_tmpCC3;});_tmp6E1;});k=_tmpCC4;});
return({struct _tuple32 _tmp6E3;_tmp6E3.f1=0;_tmp6E3.f2=k;_tmp6E3;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple32 _tmp6E4;_tmp6E4.f1=0;_tmp6E4.f2=k;_tmp6E4;});}_LL3:;};}
# 3980
return({struct _tuple32 _tmp6E8;_tmp6E8.f1=c;_tmp6E8.f2=k;_tmp6E8;});}
# 3985
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp6E9=p;int _tmp6F6;int _tmp6F5;unsigned int _tmp6F4;void*_tmp6F3;struct Cyc_Absyn_Enumfield*_tmp6F2;struct Cyc_Absyn_Enumdecl*_tmp6F1;struct Cyc_Absyn_Enumfield*_tmp6F0;switch(*((int*)_tmp6E9)){case 3U: _LL1: _tmp6F1=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6E9)->f1;_tmp6F0=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6E9)->f2;_LL2:
# 3989
({struct Cyc_Absyn_Exp*_tmpCC6=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6EA=_cycalloc(sizeof(*_tmp6EA));({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCC5=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6EB;_tmp6EB.tag=32U;_tmp6EB.f1=_tmp6F1;_tmp6EB.f2=_tmp6F0;_tmp6EB;});_tmp6EA[0]=_tmpCC5;});_tmp6EA;}),0U);e=_tmpCC6;});goto _LL0;case 4U: _LL3: _tmp6F3=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6E9)->f1;_tmp6F2=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6E9)->f2;_LL4:
# 3991
({struct Cyc_Absyn_Exp*_tmpCC8=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6EC=_cycalloc(sizeof(*_tmp6EC));({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCC7=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6ED;_tmp6ED.tag=33U;_tmp6ED.f1=_tmp6F3;_tmp6ED.f2=_tmp6F2;_tmp6ED;});_tmp6EC[0]=_tmpCC7;});_tmp6EC;}),0U);e=_tmpCC8;});goto _LL0;case 6U: _LL5: _tmp6F4=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6E9)->f1;_LL6:
 _tmp6F5=(int)_tmp6F4;goto _LL8;case 7U: _LL7: _tmp6F5=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6E9)->f1;_LL8:
 _tmp6F6=_tmp6F5;goto _LLA;case 8U: _LL9: _tmp6F6=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6E9)->f2;_LLA:
# 3995
({struct Cyc_Absyn_Exp*_tmpCC9=Cyc_Absyn_uint_exp((unsigned int)_tmp6F6,0U);e=_tmpCC9;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp6EE=0U;({struct _dyneither_ptr _tmpCCA=({const char*_tmp6EF="compile_pat_test_as_case!";_tag_dyneither(_tmp6EF,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCCA,_tag_dyneither(_tmp6EE,sizeof(void*),0U));});});}_LL0:;}
# 3999
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp6F7=_cycalloc(sizeof(*_tmp6F7));({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmpCCB=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp6F8;_tmp6F8.tag=17U;_tmp6F8.f1=e;_tmp6F8;});_tmp6F7[0]=_tmpCCB;});_tmp6F7;}),0U);}
# 4003
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 4012
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 4015
void*t=(void*)_check_null(p->topt);
void*_tmp6F9=p->r;void*_tmp6FA=_tmp6F9;union Cyc_Absyn_AggrInfoU _tmp732;struct Cyc_List_List*_tmp731;struct Cyc_List_List*_tmp730;struct Cyc_List_List*_tmp72F;struct Cyc_Absyn_Pat*_tmp72E;struct Cyc_Absyn_Datatypedecl*_tmp72D;struct Cyc_Absyn_Datatypefield*_tmp72C;struct Cyc_List_List*_tmp72B;struct Cyc_Absyn_Vardecl*_tmp72A;struct Cyc_Absyn_Pat*_tmp729;struct Cyc_Absyn_Vardecl*_tmp728;struct Cyc_Absyn_Vardecl*_tmp727;struct Cyc_Absyn_Pat*_tmp726;struct Cyc_Absyn_Vardecl*_tmp725;switch(*((int*)_tmp6FA)){case 2U: _LL1: _tmp725=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp6FA)->f2;_LL2: {
# 4018
struct Cyc_Absyn_Pat*_tmp6FB=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp6FB->topt=p->topt;
_tmp727=_tmp725;_tmp726=_tmp6FB;goto _LL4;}case 1U: _LL3: _tmp727=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6FA)->f1;_tmp726=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6FA)->f2;_LL4:
# 4023
({struct Cyc_List_List*_tmpCCC=({struct Cyc_List_List*_tmp6FC=_region_malloc(rgn,sizeof(*_tmp6FC));_tmp6FC->hd=_tmp727;_tmp6FC->tl=*decls;_tmp6FC;});*decls=_tmpCCC;});{
struct Cyc_Absyn_Stmt*_tmp6FD=({struct Cyc_Absyn_Exp*_tmpCCE=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCCD=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6FF;_tmp6FF.tag=4U;_tmp6FF.f1=_tmp727;_tmp6FF;});_tmp6FE[0]=_tmpCCD;});_tmp6FE;}),0U);Cyc_Absyn_assign_stmt(_tmpCCE,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpCCF=_tmp6FD;Cyc_Toc_seq_stmt_opt(_tmpCCF,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp726));});};case 4U: _LL5: _tmp728=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp6FA)->f2;_LL6:
# 4028
({struct Cyc_List_List*_tmpCD0=({struct Cyc_List_List*_tmp700=_region_malloc(rgn,sizeof(*_tmp700));_tmp700->hd=_tmp728;_tmp700->tl=*decls;_tmp700;});*decls=_tmpCD0;});
return({struct Cyc_Absyn_Exp*_tmpCD2=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp701=_cycalloc(sizeof(*_tmp701));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCD1=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp702;_tmp702.tag=4U;_tmp702.f1=_tmp728;_tmp702;});_tmp701[0]=_tmpCD1;});_tmp701;}),0U);Cyc_Absyn_assign_stmt(_tmpCD2,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp72A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6FA)->f1;_tmp729=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6FA)->f2;_LLA:
# 4033
({struct Cyc_List_List*_tmpCD3=({struct Cyc_List_List*_tmp703=_region_malloc(rgn,sizeof(*_tmp703));_tmp703->hd=_tmp72A;_tmp703->tl=*decls;_tmp703;});*decls=_tmpCD3;});
({void*_tmpCD4=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);_tmp72A->type=_tmpCD4;});{
# 4036
struct Cyc_Absyn_Stmt*_tmp704=({struct Cyc_Absyn_Exp*_tmpCD8=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp705=_cycalloc(sizeof(*_tmp705));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCD5=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp706;_tmp706.tag=4U;_tmp706.f1=_tmp72A;_tmp706;});_tmp705[0]=_tmpCD5;});_tmp705;}),0U);Cyc_Absyn_assign_stmt(_tmpCD8,({
void*_tmpCD7=({void*_tmpCD6=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_typ(_tmpCD6,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpCD7,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpCD9=_tmp704;Cyc_Toc_seq_stmt_opt(_tmpCD9,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp729));});};case 9U: _LLB: _LLC:
# 4041
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6FA)->f1)->r)->tag == 8U){_LL17: _tmp72D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6FA)->f1)->r)->f1;_tmp72C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6FA)->f1)->r)->f2;_tmp72B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6FA)->f1)->r)->f3;_LL18:
# 4050
 if(_tmp72B == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp72C->name,_tmp72D->name);
void*_tmp707=({void*_tmpCDA=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_typ(_tmpCDA,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpCDB=Cyc_Toc_cast_it(_tmp707,path);path=_tmpCDB;});{
int cnt=1;
struct Cyc_List_List*_tmp708=_tmp72C->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp72B != 0;(((_tmp72B=_tmp72B->tl,_tmp708=((struct Cyc_List_List*)_check_null(_tmp708))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp709=(struct Cyc_Absyn_Pat*)_tmp72B->hd;
if(_tmp709->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp70A=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp708))->hd)).f2;
void*_tmp70B=(void*)_check_null(_tmp709->topt);
void*_tmp70C=Cyc_Toc_typ_to_c_array(_tmp70B);
struct Cyc_Absyn_Exp*_tmp70D=({struct Cyc_Absyn_Exp*_tmpCDC=path;Cyc_Absyn_aggrarrow_exp(_tmpCDC,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp70A)))
({struct Cyc_Absyn_Exp*_tmpCDD=Cyc_Toc_cast_it(_tmp70C,_tmp70D);_tmp70D=_tmpCDD;});
({struct Cyc_Absyn_Stmt*_tmpCDF=({struct Cyc_Absyn_Stmt*_tmpCDE=s;Cyc_Toc_seq_stmt_opt(_tmpCDE,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp70D,_tmp709));});s=_tmpCDF;});};}
# 4068
return s;};};}else{_LL21: _tmp72E=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6FA)->f1;_LL22:
# 4117
 return({struct _RegionHandle*_tmpCE3=rgn;struct Cyc_Toc_Env**_tmpCE2=nv;struct Cyc_List_List**_tmpCE1=decls;struct Cyc_Absyn_Exp*_tmpCE0=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpCE3,_tmpCE2,_tmpCE1,_tmpCE0,_tmp72E);});}case 8U: _LL19: _tmp72F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp6FA)->f3;_LL1A:
# 4070
 _tmp730=_tmp72F;goto _LL1C;case 5U: _LL1B: _tmp730=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp6FA)->f1;_LL1C: {
# 4072
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp730 != 0;(_tmp730=_tmp730->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp70E=(struct Cyc_Absyn_Pat*)_tmp730->hd;
if(_tmp70E->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp70F=(void*)_check_null(_tmp70E->topt);
struct _dyneither_ptr*_tmp710=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpCE9=({struct Cyc_Absyn_Stmt*_tmpCE8=s;Cyc_Toc_seq_stmt_opt(_tmpCE8,({struct _RegionHandle*_tmpCE7=rgn;struct Cyc_Toc_Env**_tmpCE6=nv;struct Cyc_List_List**_tmpCE5=decls;struct Cyc_Absyn_Exp*_tmpCE4=Cyc_Toc_member_exp(path,_tmp710,0U);Cyc_Toc_extract_pattern_vars(_tmpCE7,_tmpCE6,_tmpCE5,_tmpCE4,_tmp70E);}));});s=_tmpCE9;});};}
# 4082
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6FA)->f1 == 0){_LL1D: _LL1E:
({void*_tmp711=0U;({struct _dyneither_ptr _tmpCEA=({const char*_tmp712="unresolved aggregate pattern!";_tag_dyneither(_tmp712,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCEA,_tag_dyneither(_tmp711,sizeof(void*),0U));});});}else{_LL1F: _tmp732=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6FA)->f1)->aggr_info;_tmp731=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6FA)->f3;_LL20: {
# 4086
struct Cyc_Absyn_Aggrdecl*_tmp713=Cyc_Absyn_get_known_aggrdecl(_tmp732);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp731 != 0;_tmp731=_tmp731->tl){
struct _tuple33*_tmp714=(struct _tuple33*)_tmp731->hd;
struct Cyc_Absyn_Pat*_tmp715=(*_tmp714).f2;
if(_tmp715->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp716=(void*)((struct Cyc_List_List*)_check_null((*_tmp714).f1))->hd;void*_tmp717=_tmp716;struct _dyneither_ptr*_tmp71A;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp717)->tag == 1U){_LL2A: _tmp71A=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp717)->f1;_LL2B:
 f=_tmp71A;goto _LL29;}else{_LL2C: _LL2D:
({void*_tmp718=0U;({struct _dyneither_ptr _tmpCEB=({const char*_tmp719="multiple designators in pattern!";_tag_dyneither(_tmp719,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCEB,_tag_dyneither(_tmp718,sizeof(void*),0U));});});}_LL29:;}{
# 4098
void*_tmp71B=(void*)_check_null(_tmp715->topt);
void*_tmp71C=Cyc_Toc_typ_to_c_array(_tmp71B);
struct Cyc_Absyn_Exp*_tmp71D=Cyc_Toc_member_exp(path,f,0U);
# 4102
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp713->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpCEC=Cyc_Toc_member_exp(_tmp71D,Cyc_Toc_val_sp,0U);_tmp71D=_tmpCEC;});{
void*_tmp71E=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp713->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp71E))
({struct Cyc_Absyn_Exp*_tmpCED=Cyc_Toc_cast_it(_tmp71C,_tmp71D);_tmp71D=_tmpCED;});else{
if(!Cyc_Toc_is_array_type(_tmp71E) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp71E)))
# 4109
({struct Cyc_Absyn_Exp*_tmpCEF=Cyc_Absyn_deref_exp(({void*_tmpCEE=Cyc_Absyn_cstar_typ(_tmp71C,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpCEE,
Cyc_Absyn_address_exp(_tmp71D,0U));}),0U);
# 4109
_tmp71D=_tmpCEF;});}
# 4112
({struct Cyc_Absyn_Stmt*_tmpCF1=({struct Cyc_Absyn_Stmt*_tmpCF0=s;Cyc_Toc_seq_stmt_opt(_tmpCF0,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp71D,_tmp715));});s=_tmpCF1;});};};};}
# 4114
return s;}}case 15U: _LL23: _LL24:
# 4119
({void*_tmp71F=0U;({struct _dyneither_ptr _tmpCF2=({const char*_tmp720="unknownid pat";_tag_dyneither(_tmp720,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCF2,_tag_dyneither(_tmp71F,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp721=0U;({struct _dyneither_ptr _tmpCF3=({const char*_tmp722="unknowncall pat";_tag_dyneither(_tmp722,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCF3,_tag_dyneither(_tmp721,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp723=0U;({struct _dyneither_ptr _tmpCF4=({const char*_tmp724="exp pat";_tag_dyneither(_tmp724,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCF4,_tag_dyneither(_tmp723,sizeof(void*),0U));});});}_LL0:;}struct _tuple34{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4130
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4138
void*_tmp733=dopt;struct Cyc_List_List*_tmp772;struct Cyc_List_List*_tmp771;void*_tmp770;struct Cyc_Tcpat_Rhs*_tmp76F;if(_tmp733 == 0){_LL1: _LL2:
# 4140
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp733)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp76F=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp733)->f1;_LL6:
# 4145
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp734=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp735=_tmp734;int*_tmp73C;struct _dyneither_ptr*_tmp73B;struct _dyneither_ptr*_tmp73A;struct Cyc_Absyn_Switch_clause*_tmp739;_LLA: _tmp73C=(int*)& _tmp735->f1;_tmp73B=_tmp735->f2;_tmp73A=_tmp735->f3;_tmp739=_tmp735->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp736=_tmp739->body;
if(_tmp736 == _tmp76F->rhs){
# 4150
if(*_tmp73C)
return Cyc_Absyn_goto_stmt(_tmp73B,0U);
*_tmp73C=1;{
# 4154
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpCF7=rgn;struct Cyc_List_List**_tmpCF6=decls;struct Cyc_Absyn_Exp*_tmpCF5=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpCF7,& nv,_tmpCF6,_tmpCF5,_tmp739->pattern);});
# 4157
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp73A,_tmp739->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpCF8=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpCF8;});
({struct Cyc_Absyn_Stmt*_tmpCF9=Cyc_Absyn_label_stmt(_tmp73B,res,0U);res=_tmpCF9;});
({struct Cyc_List_List*_tmpCFB=({struct Cyc_List_List*_tmp737=_region_malloc(rgn,sizeof(*_tmp737));({struct _tuple34*_tmpCFA=({struct _tuple34*_tmp738=_region_malloc(rgn,sizeof(*_tmp738));_tmp738->f1=nv;_tmp738->f2=_tmp73A;_tmp738->f3=_tmp736;_tmp738;});_tmp737->hd=_tmpCFA;});_tmp737->tl=*bodies;_tmp737;});*bodies=_tmpCFB;});
return res;};}};}
# 4165
({void*_tmp73D=0U;({struct _dyneither_ptr _tmpCFC=({const char*_tmp73E="couldn't find rhs!";_tag_dyneither(_tmp73E,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpCFC,_tag_dyneither(_tmp73D,sizeof(void*),0U));});});default: _LL7: _tmp772=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp733)->f1;_tmp771=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp733)->f2;_tmp770=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp733)->f3;_LL8: {
# 4168
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp770,lscs,v);
# 4170
struct Cyc_Absyn_Exp*_tmp73F=({struct Cyc_List_List*_tmpCFD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp772);Cyc_Toc_compile_path(_tmpCFD,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp740=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp771);
# 4173
struct _tuple32 _tmp741=Cyc_Toc_admits_switch(_tmp740);struct _tuple32 _tmp742=_tmp741;int _tmp76E;void*_tmp76D;_LLD: _tmp76E=_tmp742.f1;_tmp76D=_tmp742.f2;_LLE:;
if(_tmp76E > 1){
# 4177
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp753=_cycalloc(sizeof(*_tmp753));({struct Cyc_Absyn_Switch_clause*_tmpCFF=({struct Cyc_Absyn_Switch_clause*_tmp754=_cycalloc(sizeof(*_tmp754));({struct Cyc_Absyn_Pat*_tmpCFE=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp754->pattern=_tmpCFE;});_tmp754->pat_vars=0;_tmp754->where_clause=0;_tmp754->body=res;_tmp754->loc=0U;_tmp754;});_tmp753->hd=_tmpCFF;});_tmp753->tl=0;_tmp753;});
# 4180
for(0;_tmp740 != 0;_tmp740=_tmp740->tl){
struct _tuple29 _tmp743=*((struct _tuple29*)_tmp740->hd);struct _tuple29 _tmp744=_tmp743;void*_tmp74A;void*_tmp749;_LL10: _tmp74A=_tmp744.f1;_tmp749=_tmp744.f2;_LL11:;{
# 4183
struct Cyc_Absyn_Pat*_tmp745=Cyc_Toc_compile_pat_test_as_case(_tmp74A);
# 4185
struct Cyc_Absyn_Stmt*_tmp746=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp749,lscs,v);
# 4187
({struct Cyc_List_List*_tmpD01=({struct Cyc_List_List*_tmp747=_cycalloc(sizeof(*_tmp747));({struct Cyc_Absyn_Switch_clause*_tmpD00=({struct Cyc_Absyn_Switch_clause*_tmp748=_cycalloc(sizeof(*_tmp748));_tmp748->pattern=_tmp745;_tmp748->pat_vars=0;_tmp748->where_clause=0;_tmp748->body=_tmp746;_tmp748->loc=0U;_tmp748;});_tmp747->hd=_tmpD00;});_tmp747->tl=new_lscs;_tmp747;});new_lscs=_tmpD01;});};}
# 4189
{void*_tmp74B=_tmp76D;struct _dyneither_ptr*_tmp750;switch(*((int*)_tmp74B)){case 1U: _LL13: _LL14:
# 4192
 LOOP1: {
void*_tmp74C=_tmp73F->r;void*_tmp74D=_tmp74C;struct Cyc_Absyn_Exp*_tmp74F;struct Cyc_Absyn_Exp*_tmp74E;switch(*((int*)_tmp74D)){case 14U: _LL1C: _tmp74E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp74D)->f2;_LL1D:
 _tmp73F=_tmp74E;goto LOOP1;case 20U: _LL1E: _tmp74F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp74D)->f1;_LL1F:
 _tmp73F=_tmp74F;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 4198
({struct Cyc_Absyn_Exp*_tmpD03=Cyc_Absyn_deref_exp(({void*_tmpD02=Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpD02,_tmp73F);}),0U);_tmp73F=_tmpD03;});goto _LL12;case 3U: _LL15: _tmp750=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp74B)->f1;_LL16:
# 4201
({struct Cyc_Absyn_Exp*_tmpD05=({struct Cyc_Absyn_Exp*_tmpD04=Cyc_Toc_member_exp(_tmp73F,_tmp750,0U);Cyc_Toc_member_exp(_tmpD04,Cyc_Toc_tag_sp,0U);});_tmp73F=_tmpD05;});goto _LL12;case 2U: _LL17: _LL18:
# 4203
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 4206
({struct Cyc_Absyn_Stmt*_tmpD07=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp751=_cycalloc(sizeof(*_tmp751));({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpD06=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp752;_tmp752.tag=10U;_tmp752.f1=_tmp73F;_tmp752.f2=new_lscs;_tmp752.f3=0;_tmp752;});_tmp751[0]=_tmpD06;});_tmp751;}),0U);res=_tmpD07;});}else{
# 4210
void*_tmp755=_tmp76D;struct Cyc_Absyn_Exp*_tmp76C;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp755)->tag == 2U){_LL23: _tmp76C=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp755)->f1;_LL24: {
# 4212
struct Cyc_List_List*_tmp756=_tmp740;void*_tmp765;struct Cyc_Tcpat_Rhs*_tmp764;if(_tmp756 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp756)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp756)->tl == 0){_LL28: _tmp765=((struct _tuple29*)_tmp756->hd)->f1;_tmp764=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp756->hd)->f2)->f1;_LL29:
# 4218
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp757=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp758=_tmp757;int*_tmp75F;struct _dyneither_ptr*_tmp75E;struct _dyneither_ptr*_tmp75D;struct Cyc_Absyn_Switch_clause*_tmp75C;_LL2D: _tmp75F=(int*)& _tmp758->f1;_tmp75E=_tmp758->f2;_tmp75D=_tmp758->f3;_tmp75C=_tmp758->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp759=_tmp75C->body;
if(_tmp759 == _tmp764->rhs){
# 4223
if(*_tmp75F)
return Cyc_Absyn_goto_stmt(_tmp75E,0U);
*_tmp75F=1;{
# 4227
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpD0A=rgn;struct Cyc_List_List**_tmpD09=decls;struct Cyc_Absyn_Exp*_tmpD08=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpD0A,& nv,_tmpD09,_tmpD08,_tmp75C->pattern);});
# 4230
Cyc_Toc_exp_to_c(nv,_tmp76C);{
# 4233
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp75D,_tmp75C->body,0U);
({struct Cyc_Absyn_Stmt*_tmpD0B=Cyc_Absyn_ifthenelse_stmt(_tmp76C,r,res,0U);r=_tmpD0B;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpD0C=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpD0C;});
({struct Cyc_Absyn_Stmt*_tmpD0D=Cyc_Absyn_label_stmt(_tmp75E,r,0U);r=_tmpD0D;});
({struct Cyc_List_List*_tmpD0F=({struct Cyc_List_List*_tmp75A=_region_malloc(rgn,sizeof(*_tmp75A));({struct _tuple34*_tmpD0E=({struct _tuple34*_tmp75B=_region_malloc(rgn,sizeof(*_tmp75B));_tmp75B->f1=nv;_tmp75B->f2=_tmp75D;_tmp75B->f3=_tmp759;_tmp75B;});_tmp75A->hd=_tmpD0E;});_tmp75A->tl=*bodies;_tmp75A;});*bodies=_tmpD0F;});
return r;};};}};}
# 4242
({void*_tmp760=0U;({struct _dyneither_ptr _tmpD10=({const char*_tmp761="couldn't find rhs!";_tag_dyneither(_tmp761,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpD10,_tag_dyneither(_tmp760,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp762=0U;({struct _dyneither_ptr _tmpD11=({const char*_tmp763="bad where clause in match compiler";_tag_dyneither(_tmp763,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpD11,_tag_dyneither(_tmp762,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 4247
 for(0;_tmp740 != 0;_tmp740=_tmp740->tl){
struct _tuple29 _tmp766=*((struct _tuple29*)_tmp740->hd);struct _tuple29 _tmp767=_tmp766;void*_tmp76B;void*_tmp76A;_LL30: _tmp76B=_tmp767.f1;_tmp76A=_tmp767.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp768=Cyc_Toc_compile_pat_test(_tmp73F,_tmp76B);
struct Cyc_Absyn_Stmt*_tmp769=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp76A,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpD12=Cyc_Absyn_ifthenelse_stmt(_tmp768,_tmp769,res,0U);res=_tmpD12;});};}}_LL22:;}
# 4255
return res;}}}_LL0:;}
# 4265
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4267
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple34*_tmp773=(struct _tuple34*)bodies->hd;struct _tuple34*_tmp774=_tmp773;struct Cyc_Toc_Env**_tmp776;struct Cyc_Absyn_Stmt*_tmp775;_LL1: _tmp776=(struct Cyc_Toc_Env**)& _tmp774->f1;_tmp775=_tmp774->f3;_LL2:;
if(_tmp775 == s)return _tmp776;}
# 4273
return 0;}
# 4277
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4280
void*_tmp777=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4283
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp778=_new_region("rgn");struct _RegionHandle*rgn=& _tmp778;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp779=Cyc_Toc_share_env(rgn,nv);
# 4290
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4295
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp779,& mydecls,& mybodies,dopt,lscs,v);
# 4303
{struct Cyc_List_List*_tmp77A=lscs;for(0;_tmp77A != 0;_tmp77A=_tmp77A->tl){
struct _tuple31*_tmp77B=(struct _tuple31*)_tmp77A->hd;struct _tuple31*_tmp77C=_tmp77B;struct _dyneither_ptr*_tmp784;struct Cyc_Absyn_Switch_clause*_tmp783;_LL1: _tmp784=_tmp77C->f3;_tmp783=_tmp77C->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp783->body;
# 4307
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4310
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp77D=*envp;
# 4313
if(_tmp77A->tl != 0){
struct _tuple31*_tmp77E=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp77A->tl))->hd;struct _tuple31*_tmp77F=_tmp77E;struct _dyneither_ptr*_tmp782;struct Cyc_Absyn_Switch_clause*_tmp781;_LL4: _tmp782=_tmp77F->f3;_tmp781=_tmp77F->f4;_LL5:;{
# 4317
struct Cyc_Toc_Env**_tmp780=Cyc_Toc_find_case_env(mybodies,_tmp781->body);
# 4322
if(_tmp780 == 0)
({struct Cyc_Toc_Env*_tmpD13=Cyc_Toc_last_switchclause_env(rgn,_tmp77D,end_l);Cyc_Toc_stmt_to_c(_tmpD13,s);});else{
# 4326
struct Cyc_List_List*vs=0;
if(_tmp781->pat_vars != 0){
({struct Cyc_List_List*_tmpD14=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp781->pat_vars))->v)).f1);vs=_tmpD14;});
({struct Cyc_List_List*_tmpD15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpD15;});}
# 4331
({struct Cyc_Toc_Env*_tmpD16=Cyc_Toc_non_last_switchclause_env(rgn,_tmp77D,end_l,_tmp782,vs);Cyc_Toc_stmt_to_c(_tmpD16,s);});}};}else{
# 4335
({struct Cyc_Toc_Env*_tmpD17=Cyc_Toc_last_switchclause_env(rgn,_tmp77D,end_l);Cyc_Toc_stmt_to_c(_tmpD17,s);});}};};}}{
# 4339
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpD19=test_tree;Cyc_Absyn_seq_stmt(_tmpD19,({struct _dyneither_ptr*_tmpD18=end_l;Cyc_Absyn_label_stmt(_tmpD18,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 4341
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp785=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpD1A=Cyc_Toc_temp_var();_tmp785->name=_tmpD1A;});
({void*_tmpD1B=Cyc_Toc_typ_to_c_array(_tmp785->type);_tmp785->type=_tmpD1B;});
({struct Cyc_Absyn_Stmt*_tmpD1E=({struct Cyc_Absyn_Decl*_tmpD1D=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp786=_cycalloc(sizeof(*_tmp786));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD1C=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp787;_tmp787.tag=0U;_tmp787.f1=_tmp785;_tmp787;});_tmp786[0]=_tmpD1C;});_tmp786;}),0U);Cyc_Absyn_decl_stmt(_tmpD1D,res,0U);});res=_tmpD1E;});}
# 4348
({void*_tmpD22=({struct _tuple1*_tmpD21=v;void*_tmpD20=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpD1F=e;Cyc_Absyn_declare_stmt(_tmpD21,_tmpD20,_tmpD1F,res,0U);})->r;whole_s->r=_tmpD22;});
_npop_handler(0U);return;};}
# 4287
;_pop_region(rgn);};}
# 4354
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 4356
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4361
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4363
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpD24=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpD24,({struct Cyc_List_List*_tmp788=_cycalloc(sizeof(*_tmp788));({struct Cyc_Absyn_Exp*_tmpD23=
Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp788->hd=_tmpD23;});_tmp788->tl=0;_tmp788;}),0U);}),0U);}
# 4367
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpD26=({struct Cyc_Absyn_Stmt*_tmpD25=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpD25,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpD26;});}
# 4372
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4374
while(1){
void*_tmp789=s->r;void*_tmp78A=_tmp789;struct Cyc_Absyn_Stmt*_tmp7E5;struct Cyc_List_List*_tmp7E4;void*_tmp7E3;struct Cyc_Absyn_Stmt*_tmp7E2;struct Cyc_Absyn_Exp*_tmp7E1;struct _dyneither_ptr*_tmp7E0;struct Cyc_Absyn_Stmt*_tmp7DF;struct Cyc_Absyn_Decl*_tmp7DE;struct Cyc_Absyn_Stmt*_tmp7DD;struct Cyc_List_List*_tmp7DC;struct Cyc_Absyn_Switch_clause**_tmp7DB;struct Cyc_Absyn_Exp*_tmp7DA;struct Cyc_List_List*_tmp7D9;void*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7D7;struct Cyc_Absyn_Exp*_tmp7D6;struct Cyc_Absyn_Exp*_tmp7D5;struct Cyc_Absyn_Stmt*_tmp7D4;struct Cyc_Absyn_Exp*_tmp7D3;struct Cyc_Absyn_Stmt*_tmp7D2;struct Cyc_Absyn_Exp*_tmp7D1;struct Cyc_Absyn_Stmt*_tmp7D0;struct Cyc_Absyn_Stmt*_tmp7CF;struct Cyc_Absyn_Exp*_tmp7CE;struct Cyc_Absyn_Stmt*_tmp7CD;struct Cyc_Absyn_Stmt*_tmp7CC;struct Cyc_Absyn_Exp*_tmp7CB;switch(*((int*)_tmp78A)){case 0U: _LL1: _LL2:
# 4377
 return;case 1U: _LL3: _tmp7CB=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_LL4:
# 4379
 Cyc_Toc_exp_to_c(nv,_tmp7CB);
return;case 2U: _LL5: _tmp7CD=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_tmp7CC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp78A)->f2;_LL6:
# 4382
 Cyc_Toc_stmt_to_c(nv,_tmp7CD);
s=_tmp7CC;
continue;case 3U: _LL7: _tmp7CE=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_LL8: {
# 4386
void*topt=0;
if(_tmp7CE != 0){
({void*_tmpD27=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7CE->topt));topt=_tmpD27;});
Cyc_Toc_exp_to_c(nv,_tmp7CE);}{
# 4392
int _tmp78B=Cyc_Toc_get_npop(s);
if(_tmp78B > 0){
if(topt != 0){
struct _tuple1*_tmp78C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp78D=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp78C,0U),0U);
({void*_tmpD2C=({struct _tuple1*_tmpD2B=_tmp78C;void*_tmpD2A=topt;struct Cyc_Absyn_Exp*_tmpD29=_tmp7CE;Cyc_Absyn_declare_stmt(_tmpD2B,_tmpD2A,_tmpD29,({
struct Cyc_Absyn_Stmt*_tmpD28=Cyc_Toc_make_npop_handler(_tmp78B);Cyc_Absyn_seq_stmt(_tmpD28,_tmp78D,0U);}),0U);})->r;
# 4397
s->r=_tmpD2C;});}else{
# 4401
Cyc_Toc_do_npop_before(_tmp78B,s);}}
# 4403
return;};}case 4U: _LL9: _tmp7D1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_tmp7D0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp78A)->f2;_tmp7CF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp78A)->f3;_LLA:
# 4405
 Cyc_Toc_exp_to_c(nv,_tmp7D1);
Cyc_Toc_stmt_to_c(nv,_tmp7D0);
s=_tmp7CF;
continue;case 5U: _LLB: _tmp7D3=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1).f1;_tmp7D2=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp78A)->f2;_LLC:
# 4410
 Cyc_Toc_exp_to_c(nv,_tmp7D3);{
struct _RegionHandle _tmp78E=_new_region("temp");struct _RegionHandle*temp=& _tmp78E;_push_region(temp);
({struct Cyc_Toc_Env*_tmpD2D=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpD2D,_tmp7D2);});
# 4414
_npop_handler(0U);return;
# 4411
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 4416
struct Cyc_Toc_Env*_tmp78F=nv;struct _dyneither_ptr**_tmp790;_LL22: _tmp790=_tmp78F->break_lab;_LL23:;
if(_tmp790 != 0)
({void*_tmpD2E=Cyc_Toc_goto_stmt_r(*_tmp790);s->r=_tmpD2E;});
# 4420
({int _tmpD2F=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpD2F,s);});
return;}case 7U: _LLF: _LL10: {
# 4423
struct Cyc_Toc_Env*_tmp791=nv;struct _dyneither_ptr**_tmp792;_LL25: _tmp792=_tmp791->continue_lab;_LL26:;
if(_tmp792 != 0)
({void*_tmpD30=Cyc_Toc_goto_stmt_r(*_tmp792);s->r=_tmpD30;});
goto _LL12;}case 8U: _LL11: _LL12:
# 4429
({int _tmpD31=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpD31,s);});
return;case 9U: _LL13: _tmp7D7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_tmp7D6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp78A)->f2).f1;_tmp7D5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp78A)->f3).f1;_tmp7D4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp78A)->f4;_LL14:
# 4433
 Cyc_Toc_exp_to_c(nv,_tmp7D7);Cyc_Toc_exp_to_c(nv,_tmp7D6);Cyc_Toc_exp_to_c(nv,_tmp7D5);{
struct _RegionHandle _tmp793=_new_region("temp");struct _RegionHandle*temp=& _tmp793;_push_region(temp);
({struct Cyc_Toc_Env*_tmpD32=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpD32,_tmp7D4);});
# 4437
_npop_handler(0U);return;
# 4434
;_pop_region(temp);};case 10U: _LL15: _tmp7DA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_tmp7D9=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp78A)->f2;_tmp7D8=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp78A)->f3;_LL16:
# 4439
 Cyc_Toc_xlate_switch(nv,s,_tmp7DA,_tmp7D9,_tmp7D8);
return;case 11U: _LL17: _tmp7DC=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_tmp7DB=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp78A)->f2;_LL18: {
# 4442
struct Cyc_Toc_Env*_tmp794=nv;struct Cyc_Toc_FallthruInfo*_tmp79F;_LL28: _tmp79F=_tmp794->fallthru_info;_LL29:;
if(_tmp79F == 0)
({void*_tmp795=0U;({struct _dyneither_ptr _tmpD33=({const char*_tmp796="fallthru in unexpected place";_tag_dyneither(_tmp796,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpD33,_tag_dyneither(_tmp795,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp797=*_tmp79F;struct Cyc_Toc_FallthruInfo _tmp798=_tmp797;struct _dyneither_ptr*_tmp79E;struct Cyc_List_List*_tmp79D;_LL2B: _tmp79E=_tmp798.label;_tmp79D=_tmp798.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp79E,0U);
# 4448
({int _tmpD34=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpD34,s2);});{
struct Cyc_List_List*_tmp799=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp79D);
struct Cyc_List_List*_tmp79A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7DC);
for(0;_tmp799 != 0;(_tmp799=_tmp799->tl,_tmp79A=_tmp79A->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp79A))->hd);
({struct Cyc_Absyn_Stmt*_tmpD38=({struct Cyc_Absyn_Stmt*_tmpD37=({struct Cyc_Absyn_Exp*_tmpD36=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp79B=_cycalloc(sizeof(*_tmp79B));({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmpD35=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp79C;_tmp79C.tag=5U;_tmp79C.f1=(struct Cyc_Absyn_Vardecl*)_tmp799->hd;_tmp79C;});_tmp79B[0]=_tmpD35;});_tmp79B;}),0U);Cyc_Absyn_assign_stmt(_tmpD36,(struct Cyc_Absyn_Exp*)_tmp79A->hd,0U);});Cyc_Absyn_seq_stmt(_tmpD37,s2,0U);});s2=_tmpD38;});}
# 4456
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp7DE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_tmp7DD=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp78A)->f2;_LL1A:
# 4462
{void*_tmp7A0=_tmp7DE->r;void*_tmp7A1=_tmp7A0;struct Cyc_Absyn_Tvar*_tmp7BE;struct Cyc_Absyn_Vardecl*_tmp7BD;struct Cyc_Absyn_Exp*_tmp7BC;struct Cyc_Absyn_Fndecl*_tmp7BB;struct Cyc_List_List*_tmp7BA;struct Cyc_Absyn_Pat*_tmp7B9;struct Cyc_Absyn_Exp*_tmp7B8;void*_tmp7B7;struct Cyc_Absyn_Vardecl*_tmp7B6;switch(*((int*)_tmp7A1)){case 0U: _LL2E: _tmp7B6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7A1)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp7B6,_tmp7DD);goto _LL2D;case 2U: _LL30: _tmp7B9=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7A1)->f1;_tmp7B8=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7A1)->f3;_tmp7B7=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7A1)->f4;_LL31:
# 4470
{void*_tmp7A2=_tmp7B9->r;void*_tmp7A3=_tmp7A2;struct Cyc_Absyn_Vardecl*_tmp7A6;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7A3)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7A3)->f2)->r)->tag == 0U){_LL3B: _tmp7A6=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7A3)->f1;_LL3C:
# 4472
({struct _tuple1*_tmpD39=Cyc_Toc_temp_var();_tmp7A6->name=_tmpD39;});
_tmp7A6->initializer=_tmp7B8;
({void*_tmpD3B=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7A4=_cycalloc(sizeof(*_tmp7A4));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD3A=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7A5;_tmp7A5.tag=0U;_tmp7A5.f1=_tmp7A6;_tmp7A5;});_tmp7A4[0]=_tmpD3A;});_tmp7A4;});_tmp7DE->r=_tmpD3B;});
Cyc_Toc_local_decl_to_c(nv,_tmp7A6,_tmp7DD);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 4481
({void*_tmpD3C=(Cyc_Toc_letdecl_to_c(nv,_tmp7B9,_tmp7B7,(void*)_check_null(_tmp7B8->topt),_tmp7B8,_tmp7DD))->r;s->r=_tmpD3C;});
goto _LL3A;}_LL3A:;}
# 4484
goto _LL2D;case 3U: _LL32: _tmp7BA=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7A1)->f1;_LL33: {
# 4488
struct Cyc_List_List*_tmp7A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7BA);
if(_tmp7A7 == 0)
({void*_tmp7A8=0U;({struct _dyneither_ptr _tmpD3D=({const char*_tmp7A9="empty Letv_d";_tag_dyneither(_tmp7A9,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD3D,_tag_dyneither(_tmp7A8,sizeof(void*),0U));});});
({void*_tmpD3F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD3E=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7AB;_tmp7AB.tag=0U;_tmp7AB.f1=(struct Cyc_Absyn_Vardecl*)_tmp7A7->hd;_tmp7AB;});_tmp7AA[0]=_tmpD3E;});_tmp7AA;});_tmp7DE->r=_tmpD3F;});
_tmp7A7=_tmp7A7->tl;
for(0;_tmp7A7 != 0;_tmp7A7=_tmp7A7->tl){
struct Cyc_Absyn_Decl*_tmp7AC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7AD=_cycalloc(sizeof(*_tmp7AD));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD40=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7AE;_tmp7AE.tag=0U;_tmp7AE.f1=(struct Cyc_Absyn_Vardecl*)_tmp7A7->hd;_tmp7AE;});_tmp7AD[0]=_tmpD40;});_tmp7AD;}),0U);
({void*_tmpD42=({struct Cyc_Absyn_Decl*_tmpD41=_tmp7AC;Cyc_Absyn_decl_stmt(_tmpD41,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpD42;});}
# 4497
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp7BB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7A1)->f1;_LL35:
# 4500
 Cyc_Toc_fndecl_to_c(nv,_tmp7BB,0);
Cyc_Toc_stmt_to_c(nv,_tmp7DD);
goto _LL2D;case 4U: _LL36: _tmp7BE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A1)->f1;_tmp7BD=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A1)->f2;_tmp7BC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7A1)->f3;_LL37: {
# 4504
struct Cyc_Absyn_Stmt*_tmp7AF=_tmp7DD;
# 4512
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7BD->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 4519
Cyc_Toc_stmt_to_c(nv,_tmp7AF);
if(Cyc_Absyn_no_regions)
({void*_tmpD46=({struct _tuple1*_tmpD45=x_var;void*_tmpD44=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpD43=
Cyc_Absyn_uint_exp(0U,0U);
# 4521
Cyc_Absyn_declare_stmt(_tmpD45,_tmpD44,_tmpD43,_tmp7AF,0U);})->r;s->r=_tmpD46;});else{
# 4523
if((unsigned int)_tmp7BC){
Cyc_Toc_exp_to_c(nv,_tmp7BC);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpD47=_tmp7BC;Cyc_Absyn_aggrarrow_exp(_tmpD47,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2U),0U);}),0U);
({void*_tmpD48=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp7AF,0U))->r;s->r=_tmpD48;});};}else{
# 4528
({void*_tmpD55=({
struct _tuple1*_tmpD54=rh_var;void*_tmpD53=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpD52=({
struct Cyc_Absyn_Exp*_tmpD4A=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpD4A,({struct Cyc_List_List*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));({struct Cyc_Absyn_Exp*_tmpD49=
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp7B1->hd=_tmpD49;});_tmp7B1->tl=0;_tmp7B1;}),0U);});
# 4529
Cyc_Absyn_declare_stmt(_tmpD54,_tmpD53,_tmpD52,({
# 4533
struct _tuple1*_tmpD51=x_var;void*_tmpD50=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpD4F=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpD51,_tmpD50,_tmpD4F,({
struct Cyc_Absyn_Stmt*_tmpD4E=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpD4B=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpD4B,({struct Cyc_Absyn_Exp*_tmp7B2[1U];_tmp7B2[0U]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7B2,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpD4E,({
struct Cyc_Absyn_Stmt*_tmpD4D=_tmp7AF;Cyc_Absyn_seq_stmt(_tmpD4D,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpD4C=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpD4C,({struct Cyc_Absyn_Exp*_tmp7B3[1U];_tmp7B3[0U]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7B3,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 4528
s->r=_tmpD55;});}}
# 4539
return;}default: _LL38: _LL39:
({void*_tmp7B4=0U;({struct _dyneither_ptr _tmpD56=({const char*_tmp7B5="bad nested declaration within function";_tag_dyneither(_tmp7B5,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD56,_tag_dyneither(_tmp7B4,sizeof(void*),0U));});});}_LL2D:;}
# 4542
return;case 13U: _LL1B: _tmp7E0=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_tmp7DF=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp78A)->f2;_LL1C:
# 4544
 s=_tmp7DF;continue;case 14U: _LL1D: _tmp7E2=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_tmp7E1=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp78A)->f2).f1;_LL1E: {
# 4546
struct _RegionHandle _tmp7BF=_new_region("temp");struct _RegionHandle*temp=& _tmp7BF;_push_region(temp);
({struct Cyc_Toc_Env*_tmpD57=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpD57,_tmp7E2);});
Cyc_Toc_exp_to_c(nv,_tmp7E1);
# 4550
_npop_handler(0U);return;
# 4546
;_pop_region(temp);}default: _LL1F: _tmp7E5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp78A)->f1;_tmp7E4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp78A)->f2;_tmp7E3=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp78A)->f3;_LL20: {
# 4564 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4574
e_exp->topt=e_typ;{
struct _RegionHandle _tmp7C0=_new_region("temp");struct _RegionHandle*temp=& _tmp7C0;_push_region(temp);
# 4577
Cyc_Toc_stmt_to_c(nv,_tmp7E5);{
struct Cyc_Absyn_Stmt*_tmp7C1=({struct Cyc_Absyn_Stmt*_tmpD58=_tmp7E5;Cyc_Absyn_seq_stmt(_tmpD58,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 4582
struct Cyc_Absyn_Stmt*_tmp7C2=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7C9=_cycalloc(sizeof(*_tmp7C9));({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpD59=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp7CA;_tmp7CA.tag=10U;_tmp7CA.f1=e_exp;_tmp7CA.f2=_tmp7E4;_tmp7CA.f3=_tmp7E3;_tmp7CA;});_tmp7C9[0]=_tmpD59;});_tmp7C9;}),0U);
# 4584
Cyc_Toc_stmt_to_c(nv,_tmp7C2);{
# 4587
struct Cyc_Absyn_Exp*_tmp7C3=({
struct Cyc_Absyn_Exp*_tmpD5B=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpD5B,({struct Cyc_List_List*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));({struct Cyc_Absyn_Exp*_tmpD5A=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp7C8->hd=_tmpD5A;});_tmp7C8->tl=0;_tmp7C8;}),0U);});
# 4591
struct Cyc_Absyn_Stmt*_tmp7C4=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpD5D=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpD5D,({struct Cyc_List_List*_tmp7C7=_cycalloc(sizeof(*_tmp7C7));({struct Cyc_Absyn_Exp*_tmpD5C=
Cyc_Absyn_address_exp(h_exp,0U);_tmp7C7->hd=_tmpD5C;});_tmp7C7->tl=0;_tmp7C7;}),0U);}),0U);
# 4595
struct Cyc_Absyn_Exp*_tmp7C5=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp7C6=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpD6C=({
struct _tuple1*_tmpD6B=h_var;void*_tmpD6A=h_typ;Cyc_Absyn_declare_stmt(_tmpD6B,_tmpD6A,0,({
struct Cyc_Absyn_Stmt*_tmpD69=_tmp7C4;Cyc_Absyn_seq_stmt(_tmpD69,({
struct _tuple1*_tmpD68=was_thrown_var;void*_tmpD67=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpD66=_tmp7C5;Cyc_Absyn_declare_stmt(_tmpD68,_tmpD67,_tmpD66,({
struct Cyc_Absyn_Stmt*_tmpD65=({struct Cyc_Absyn_Exp*_tmpD5F=_tmp7C3;struct Cyc_Absyn_Stmt*_tmpD5E=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp7C6,0U);
# 4601
Cyc_Absyn_ifthenelse_stmt(_tmpD5F,_tmpD5E,
# 4603
Cyc_Toc_skip_stmt_dl(),0U);});
# 4601
Cyc_Absyn_seq_stmt(_tmpD65,({
# 4604
struct Cyc_Absyn_Exp*_tmpD64=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpD63=_tmp7C1;Cyc_Absyn_ifthenelse_stmt(_tmpD64,_tmpD63,({
# 4606
struct _tuple1*_tmpD62=e_var;void*_tmpD61=e_typ;struct Cyc_Absyn_Exp*_tmpD60=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 4606
Cyc_Absyn_declare_stmt(_tmpD62,_tmpD61,_tmpD60,_tmp7C2,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 4597
s->r=_tmpD6C;});};};
# 4610
_npop_handler(0U);return;
# 4575
;_pop_region(temp);};}}_LL0:;}}
# 4619
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpD6D=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpD6D;});{
struct _RegionHandle _tmp7E6=_new_region("frgn");struct _RegionHandle*frgn=& _tmp7E6;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp7E7=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp7E8=f->args;for(0;_tmp7E8 != 0;_tmp7E8=_tmp7E8->tl){
struct _tuple1*_tmp7E9=({struct _tuple1*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA->f1=Cyc_Absyn_Loc_n;_tmp7EA->f2=(*((struct _tuple9*)_tmp7E8->hd)).f1;_tmp7EA;});
({void*_tmpD6E=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp7E8->hd)).f3);(*((struct _tuple9*)_tmp7E8->hd)).f3=_tmpD6E;});}}
# 4635
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 4639
({struct Cyc_Hashtable_Table**_tmpD70=({struct Cyc_Hashtable_Table**_tmp7EB=_cycalloc(sizeof(*_tmp7EB));({struct Cyc_Hashtable_Table*_tmpD6F=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);_tmp7EB[0]=_tmpD6F;});_tmp7EB;});Cyc_Toc_fn_pop_table=_tmpD70;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp7EC=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp7ED=_tmp7EC;struct _dyneither_ptr*_tmp7F6;struct Cyc_Absyn_Tqual _tmp7F5;void*_tmp7F4;int _tmp7F3;_LL1: _tmp7F6=_tmp7ED.name;_tmp7F5=_tmp7ED.tq;_tmp7F4=_tmp7ED.type;_tmp7F3=_tmp7ED.inject;_LL2:;{
void*_tmp7EE=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp7F4,(void*)& Cyc_Absyn_HeapRgn_val,_tmp7F5,Cyc_Absyn_false_conref));
struct _tuple1*_tmp7EF=({struct _tuple1*_tmp7F2=_cycalloc(sizeof(*_tmp7F2));_tmp7F2->f1=Cyc_Absyn_Loc_n;_tmp7F2->f2=(struct _dyneither_ptr*)_check_null(_tmp7F6);_tmp7F2;});
({struct Cyc_List_List*_tmpD73=({struct Cyc_List_List*_tmpD72=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpD72,({struct Cyc_List_List*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));({struct _tuple9*_tmpD71=({struct _tuple9*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1->f1=_tmp7F6;_tmp7F1->f2=_tmp7F5;_tmp7F1->f3=_tmp7EE;_tmp7F1;});_tmp7F0->hd=_tmpD71;});_tmp7F0->tl=0;_tmp7F0;}));});f->args=_tmpD73;});
f->cyc_varargs=0;};}
# 4648
{struct Cyc_List_List*_tmp7F7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp7F7 != 0;_tmp7F7=_tmp7F7->tl){
({void*_tmpD74=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp7F7->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp7F7->hd)->type=_tmpD74;});}}
# 4651
({struct Cyc_Toc_Env*_tmpD75=Cyc_Toc_clear_toplevel(frgn,_tmp7E7);Cyc_Toc_stmt_to_c(_tmpD75,f->body);});}
# 4628
;_pop_region(frgn);};}
# 4655
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp7F8=s;switch(_tmp7F8){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple35{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple36{struct Cyc_Toc_TocState*f1;struct _tuple35*f2;};
# 4672 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple36*env){
# 4675
struct _tuple36 _tmp7F9=*env;struct _tuple36 _tmp7FA=_tmp7F9;struct Cyc_Toc_TocState*_tmp839;struct Cyc_Absyn_Aggrdecl*_tmp838;int _tmp837;_LL1: _tmp839=_tmp7FA.f1;_tmp838=(_tmp7FA.f2)->f1;_tmp837=(_tmp7FA.f2)->f2;_LL2:;{
struct _tuple1*_tmp7FB=_tmp838->name;
struct Cyc_Toc_TocState _tmp7FC=*_tmp839;struct Cyc_Toc_TocState _tmp7FD=_tmp7FC;struct Cyc_Dict_Dict*_tmp836;_LL4: _tmp836=_tmp7FD.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple18**_tmp7FE=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp836,_tmp7FB);
if(_tmp7FE == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp838->kind == Cyc_Absyn_StructA)
({struct _tuple18*_tmpD77=({struct _tuple18*_tmp7FF=_region_malloc(d,sizeof(*_tmp7FF));_tmp7FF->f1=_tmp838;({void*_tmpD76=Cyc_Absyn_strctq(_tmp7FB);_tmp7FF->f2=_tmpD76;});_tmp7FF;});v=_tmpD77;});else{
# 4686
({struct _tuple18*_tmpD79=({struct _tuple18*_tmp800=_region_malloc(d,sizeof(*_tmp800));_tmp800->f1=_tmp838;({void*_tmpD78=Cyc_Absyn_unionq_typ(_tmp7FB);_tmp800->f2=_tmpD78;});_tmp800;});v=_tmpD79;});}
({struct Cyc_Dict_Dict _tmpD7A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp836,_tmp7FB,v);*_tmp836=_tmpD7A;});};}else{
# 4689
struct _tuple18*_tmp801=*_tmp7FE;struct _tuple18*_tmp802=_tmp801;struct Cyc_Absyn_Aggrdecl*_tmp805;void*_tmp804;_LL7: _tmp805=_tmp802->f1;_tmp804=_tmp802->f2;_LL8:;
if(_tmp805->impl == 0){
({struct Cyc_Dict_Dict _tmpD7D=({struct Cyc_Dict_Dict _tmpD7C=*_tmp836;struct _tuple1*_tmpD7B=_tmp7FB;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmpD7C,_tmpD7B,({struct _tuple18*_tmp803=_region_malloc(d,sizeof(*_tmp803));_tmp803->f1=_tmp838;_tmp803->f2=_tmp804;_tmp803;}));});*_tmp836=_tmpD7D;});
seen_defn_before=0;}else{
# 4694
seen_defn_before=1;}}{
# 4696
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp835=_cycalloc(sizeof(*_tmp835));_tmp835->kind=_tmp838->kind;_tmp835->sc=Cyc_Absyn_Public;_tmp835->name=_tmp838->name;_tmp835->tvs=0;_tmp835->impl=0;_tmp835->expected_mem_kind=0;_tmp835->attributes=_tmp838->attributes;_tmp835;});
# 4703
if(_tmp838->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpD7E=({struct Cyc_Absyn_AggrdeclImpl*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806->exist_vars=0;_tmp806->rgn_po=0;_tmp806->fields=0;_tmp806->tagged=0;_tmp806;});new_ad->impl=_tmpD7E;});{
# 4708
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp807=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp838->impl))->fields;for(0;_tmp807 != 0;_tmp807=_tmp807->tl){
# 4712
struct Cyc_Absyn_Aggrfield*_tmp808=(struct Cyc_Absyn_Aggrfield*)_tmp807->hd;
void*_tmp809=_tmp808->type;
struct Cyc_List_List*_tmp80A=_tmp808->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp809)) && (
_tmp838->kind == Cyc_Absyn_StructA  && _tmp807->tl == 0  || _tmp838->kind == Cyc_Absyn_UnionA)){
# 4726 "toc.cyc"
void*_tmp80B=Cyc_Tcutil_compress(_tmp809);void*_tmp80C=_tmp80B;void*_tmp819;struct Cyc_Absyn_Tqual _tmp818;union Cyc_Absyn_Constraint*_tmp817;unsigned int _tmp816;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80C)->tag == 8U){_LLA: _tmp819=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80C)->f1).elt_type;_tmp818=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80C)->f1).tq;_tmp817=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80C)->f1).zero_term;_tmp816=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80C)->f1).zt_loc;_LLB:
# 4729
({void*_tmpD82=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp80D=_cycalloc(sizeof(*_tmp80D));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD81=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp80E;_tmp80E.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmpD80=({struct Cyc_Absyn_ArrayInfo _tmp80F;_tmp80F.elt_type=_tmp819;_tmp80F.tq=_tmp818;({struct Cyc_Absyn_Exp*_tmpD7F=Cyc_Absyn_uint_exp(0U,0U);_tmp80F.num_elts=_tmpD7F;});_tmp80F.zero_term=_tmp817;_tmp80F.zt_loc=_tmp816;_tmp80F;});_tmp80E.f1=_tmpD80;});_tmp80E;});_tmp80D[0]=_tmpD81;});_tmp80D;});_tmp809=_tmpD82;});
goto _LL9;}else{_LLC: _LLD:
# 4732
({struct Cyc_List_List*_tmpD85=({struct Cyc_List_List*_tmp810=_cycalloc(sizeof(*_tmp810));({void*_tmpD84=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp811=_cycalloc(sizeof(*_tmp811));({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpD83=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp812;_tmp812.tag=6U;_tmp812.f1=0;_tmp812;});_tmp811[0]=_tmpD83;});_tmp811;});_tmp810->hd=_tmpD84;});_tmp810->tl=_tmp80A;_tmp810;});_tmp80A=_tmpD85;});
({void*_tmpD8C=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp813=_cycalloc(sizeof(*_tmp813));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD8B=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp814;_tmp814.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmpD8A=({struct Cyc_Absyn_ArrayInfo _tmp815;({void*_tmpD86=Cyc_Absyn_void_star_typ();_tmp815.elt_type=_tmpD86;});({struct Cyc_Absyn_Tqual _tmpD87=
Cyc_Absyn_empty_tqual(0U);_tmp815.tq=_tmpD87;});({struct Cyc_Absyn_Exp*_tmpD88=
Cyc_Absyn_uint_exp(0U,0U);_tmp815.num_elts=_tmpD88;});({union Cyc_Absyn_Constraint*_tmpD89=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp815.zero_term=_tmpD89;});_tmp815.zt_loc=0U;_tmp815;});_tmp814.f1=_tmpD8A;});_tmp814;});_tmp813[0]=_tmpD8B;});_tmp813;});
# 4733
_tmp809=_tmpD8C;});}_LL9:;}{
# 4739
struct Cyc_Absyn_Aggrfield*_tmp81A=({struct Cyc_Absyn_Aggrfield*_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830->name=_tmp808->name;_tmp830->tq=Cyc_Toc_mt_tq;({void*_tmpD8D=
# 4741
Cyc_Toc_typ_to_c(_tmp809);_tmp830->type=_tmpD8D;});_tmp830->width=_tmp808->width;_tmp830->attributes=_tmp80A;_tmp830->requires_clause=0;_tmp830;});
# 4749
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp838->impl))->tagged){
void*_tmp81B=_tmp81A->type;
struct _dyneither_ptr*_tmp81C=_tmp81A->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp82E;_tmp82E.tag=0U;_tmp82E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp81C);({struct Cyc_String_pa_PrintArg_struct _tmp82D;_tmp82D.tag=0U;_tmp82D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp838->name).f2);({void*_tmp82B[2U]={& _tmp82D,& _tmp82E};({struct _dyneither_ptr _tmpD8E=({const char*_tmp82C="_union_%s_%s";_tag_dyneither(_tmp82C,sizeof(char),13U);});Cyc_aprintf(_tmpD8E,_tag_dyneither(_tmp82B,sizeof(void*),2U));});});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp82A=_cycalloc(sizeof(*_tmp82A));_tmp82A[0]=s;_tmp82A;});
struct Cyc_Absyn_Aggrfield*_tmp81D=({struct Cyc_Absyn_Aggrfield*_tmp829=_cycalloc(sizeof(*_tmp829));_tmp829->name=Cyc_Toc_val_sp;_tmp829->tq=Cyc_Toc_mt_tq;_tmp829->type=_tmp81B;_tmp829->width=0;_tmp829->attributes=0;_tmp829->requires_clause=0;_tmp829;});
struct Cyc_Absyn_Aggrfield*_tmp81E=({struct Cyc_Absyn_Aggrfield*_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828->name=Cyc_Toc_tag_sp;_tmp828->tq=Cyc_Toc_mt_tq;_tmp828->type=Cyc_Absyn_sint_typ;_tmp828->width=0;_tmp828->attributes=0;_tmp828->requires_clause=0;_tmp828;});
struct Cyc_List_List*_tmp81F=({struct Cyc_Absyn_Aggrfield*_tmp827[2U];_tmp827[1U]=_tmp81D;_tmp827[0U]=_tmp81E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp827,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp820=({struct Cyc_Absyn_Aggrdecl*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824->kind=Cyc_Absyn_StructA;_tmp824->sc=Cyc_Absyn_Public;({struct _tuple1*_tmpD8F=({struct _tuple1*_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826->f1=Cyc_Absyn_Loc_n;_tmp826->f2=str;_tmp826;});_tmp824->name=_tmpD8F;});_tmp824->tvs=0;({struct Cyc_Absyn_AggrdeclImpl*_tmpD90=({struct Cyc_Absyn_AggrdeclImpl*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825->exist_vars=0;_tmp825->rgn_po=0;_tmp825->fields=_tmp81F;_tmp825->tagged=0;_tmp825;});_tmp824->impl=_tmpD90;});_tmp824->expected_mem_kind=0;_tmp824->attributes=0;_tmp824;});
# 4762
({struct Cyc_List_List*_tmpD93=({struct Cyc_List_List*_tmp821=_cycalloc(sizeof(*_tmp821));({struct Cyc_Absyn_Decl*_tmpD92=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp822=_cycalloc(sizeof(*_tmp822));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD91=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp823;_tmp823.tag=5U;_tmp823.f1=_tmp820;_tmp823;});_tmp822[0]=_tmpD91;});_tmp822;}),0U);_tmp821->hd=_tmpD92;});_tmp821->tl=Cyc_Toc_result_decls;_tmp821;});Cyc_Toc_result_decls=_tmpD93;});
({void*_tmpD94=Cyc_Absyn_strct(str);_tmp81A->type=_tmpD94;});}
# 4765
({struct Cyc_List_List*_tmpD95=({struct Cyc_List_List*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F->hd=_tmp81A;_tmp82F->tl=new_fields;_tmp82F;});new_fields=_tmpD95;});};}}
# 4767
({struct Cyc_List_List*_tmpD96=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpD96;});};}
# 4769
if(_tmp837)
({struct Cyc_List_List*_tmpD9A=({struct Cyc_List_List*_tmp831=_cycalloc(sizeof(*_tmp831));({struct Cyc_Absyn_Decl*_tmpD99=({struct Cyc_Absyn_Decl*_tmp832=_cycalloc(sizeof(*_tmp832));({void*_tmpD98=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp833=_cycalloc(sizeof(*_tmp833));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD97=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp834;_tmp834.tag=5U;_tmp834.f1=new_ad;_tmp834;});_tmp833[0]=_tmpD97;});_tmp833;});_tmp832->r=_tmpD98;});_tmp832->loc=0U;_tmp832;});_tmp831->hd=_tmpD99;});_tmp831->tl=Cyc_Toc_result_decls;_tmp831;});Cyc_Toc_result_decls=_tmpD9A;});
return 0;};};};}
# 4774
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple35 p=({struct _tuple35 _tmp83A;_tmp83A.f1=ad;_tmp83A.f2=add_to_decls;_tmp83A;});
((int(*)(struct _tuple35*arg,int(*f)(struct _RegionHandle*,struct _tuple36*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple37{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4803 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4806
struct _tuple37 _tmp83B=*env;struct _tuple37 _tmp83C=_tmp83B;struct Cyc_Set_Set**_tmp84E;struct Cyc_Absyn_Datatypedecl*_tmp84D;_LL1: _tmp84E=(_tmp83C.f1)->datatypes_so_far;_tmp84D=_tmp83C.f2;_LL2:;{
struct _tuple1*_tmp83D=_tmp84D->name;
if(_tmp84D->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp84E,_tmp83D))
return 0;
({struct Cyc_Set_Set*_tmpD9B=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp84E,_tmp83D);*_tmp84E=_tmpD9B;});
{struct Cyc_List_List*_tmp83E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp84D->fields))->v;for(0;_tmp83E != 0;_tmp83E=_tmp83E->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp83E->hd;
# 4814
struct Cyc_List_List*_tmp83F=0;
int i=1;
{struct Cyc_List_List*_tmp840=f->typs;for(0;_tmp840 != 0;(_tmp840=_tmp840->tl,i ++)){
struct _dyneither_ptr*_tmp841=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp842=({struct Cyc_Absyn_Aggrfield*_tmp844=_cycalloc(sizeof(*_tmp844));_tmp844->name=_tmp841;_tmp844->tq=(*((struct _tuple11*)_tmp840->hd)).f1;({void*_tmpD9C=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp840->hd)).f2);_tmp844->type=_tmpD9C;});_tmp844->width=0;_tmp844->attributes=0;_tmp844->requires_clause=0;_tmp844;});
({struct Cyc_List_List*_tmpD9D=({struct Cyc_List_List*_tmp843=_cycalloc(sizeof(*_tmp843));_tmp843->hd=_tmp842;_tmp843->tl=_tmp83F;_tmp843;});_tmp83F=_tmpD9D;});}}
# 4822
({struct Cyc_List_List*_tmpDA0=({struct Cyc_List_List*_tmp845=_cycalloc(sizeof(*_tmp845));({struct Cyc_Absyn_Aggrfield*_tmpD9E=({struct Cyc_Absyn_Aggrfield*_tmp846=_cycalloc(sizeof(*_tmp846));_tmp846->name=Cyc_Toc_tag_sp;_tmp846->tq=Cyc_Toc_mt_tq;_tmp846->type=Cyc_Absyn_sint_typ;_tmp846->width=0;_tmp846->attributes=0;_tmp846->requires_clause=0;_tmp846;});_tmp845->hd=_tmpD9E;});({struct Cyc_List_List*_tmpD9F=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp83F);_tmp845->tl=_tmpD9F;});_tmp845;});
# 4822
_tmp83F=_tmpDA0;});{
# 4824
struct Cyc_Absyn_Aggrdecl*_tmp847=({struct Cyc_Absyn_Aggrdecl*_tmp84B=_cycalloc(sizeof(*_tmp84B));_tmp84B->kind=Cyc_Absyn_StructA;_tmp84B->sc=Cyc_Absyn_Public;({struct _tuple1*_tmpDA1=
# 4826
Cyc_Toc_collapse_qvars(f->name,_tmp84D->name);_tmp84B->name=_tmpDA1;});_tmp84B->tvs=0;_tmp84B->expected_mem_kind=0;({struct Cyc_Absyn_AggrdeclImpl*_tmpDA2=({struct Cyc_Absyn_AggrdeclImpl*_tmp84C=_cycalloc(sizeof(*_tmp84C));_tmp84C->exist_vars=0;_tmp84C->rgn_po=0;_tmp84C->fields=_tmp83F;_tmp84C->tagged=0;_tmp84C;});_tmp84B->impl=_tmpDA2;});_tmp84B->attributes=0;_tmp84B;});
# 4830
({struct Cyc_List_List*_tmpDA5=({struct Cyc_List_List*_tmp848=_cycalloc(sizeof(*_tmp848));({struct Cyc_Absyn_Decl*_tmpDA4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp849=_cycalloc(sizeof(*_tmp849));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDA3=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp84A;_tmp84A.tag=5U;_tmp84A.f1=_tmp847;_tmp84A;});_tmp849[0]=_tmpDA3;});_tmp849;}),0U);_tmp848->hd=_tmpDA4;});_tmp848->tl=Cyc_Toc_result_decls;_tmp848;});Cyc_Toc_result_decls=_tmpDA5;});};}}
# 4832
return 0;};}
# 4835
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4854 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4857
struct _tuple37 _tmp84F=*env;struct _tuple37 _tmp850=_tmp84F;struct Cyc_Toc_TocState*_tmp875;struct Cyc_Absyn_Datatypedecl*_tmp874;_LL1: _tmp875=_tmp850.f1;_tmp874=_tmp850.f2;_LL2:;
if(_tmp874->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp851=*_tmp875;struct Cyc_Toc_TocState _tmp852=_tmp851;struct Cyc_Dict_Dict*_tmp873;_LL4: _tmp873=_tmp852.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp853=_tmp874->name;
{struct Cyc_List_List*_tmp854=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp874->fields))->v;for(0;_tmp854 != 0;_tmp854=_tmp854->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp854->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp855=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp856=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp855,Cyc_Absyn_false_conref,0U);
# 4868
int*_tmp857=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp873,f->name);int*_tmp858=_tmp857;if(_tmp858 == 0){_LL7: _LL8: {
# 4870
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpDA6=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpDA6;});{
# 4874
struct Cyc_Absyn_Vardecl*_tmp859=Cyc_Absyn_new_vardecl(0U,f->name,_tmp856,initopt);
_tmp859->sc=f->sc;
({struct Cyc_List_List*_tmpDA9=({struct Cyc_List_List*_tmp85A=_cycalloc(sizeof(*_tmp85A));({struct Cyc_Absyn_Decl*_tmpDA8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp85B=_cycalloc(sizeof(*_tmp85B));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDA7=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp85C;_tmp85C.tag=0U;_tmp85C.f1=_tmp859;_tmp85C;});_tmp85B[0]=_tmpDA7;});_tmp85B;}),0U);_tmp85A->hd=_tmpDA8;});_tmp85A->tl=Cyc_Toc_result_decls;_tmp85A;});Cyc_Toc_result_decls=_tmpDA9;});
({struct Cyc_Dict_Dict _tmpDAA=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp873,f->name,f->sc != Cyc_Absyn_Extern);
# 4877
*_tmp873=_tmpDAA;});{
# 4879
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp85D=f->typs;for(0;_tmp85D != 0;(_tmp85D=_tmp85D->tl,i ++)){
struct _dyneither_ptr*_tmp85E=({struct _dyneither_ptr*_tmp862=_cycalloc(sizeof(*_tmp862));({struct _dyneither_ptr _tmpDAC=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp865;_tmp865.tag=1U;_tmp865.f1=(unsigned long)i;({void*_tmp863[1U]={& _tmp865};({struct _dyneither_ptr _tmpDAB=({const char*_tmp864="f%d";_tag_dyneither(_tmp864,sizeof(char),4U);});Cyc_aprintf(_tmpDAB,_tag_dyneither(_tmp863,sizeof(void*),1U));});});});_tmp862[0]=_tmpDAC;});_tmp862;});
struct Cyc_Absyn_Aggrfield*_tmp85F=({struct Cyc_Absyn_Aggrfield*_tmp861=_cycalloc(sizeof(*_tmp861));_tmp861->name=_tmp85E;_tmp861->tq=(*((struct _tuple11*)_tmp85D->hd)).f1;({void*_tmpDAD=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp85D->hd)).f2);_tmp861->type=_tmpDAD;});_tmp861->width=0;_tmp861->attributes=0;_tmp861->requires_clause=0;_tmp861;});
({struct Cyc_List_List*_tmpDAE=({struct Cyc_List_List*_tmp860=_cycalloc(sizeof(*_tmp860));_tmp860->hd=_tmp85F;_tmp860->tl=fields;_tmp860;});fields=_tmpDAE;});}}
# 4887
({struct Cyc_List_List*_tmpDB2=({struct Cyc_List_List*_tmp866=_cycalloc(sizeof(*_tmp866));({struct Cyc_Absyn_Aggrfield*_tmpDB0=({struct Cyc_Absyn_Aggrfield*_tmp867=_cycalloc(sizeof(*_tmp867));_tmp867->name=Cyc_Toc_tag_sp;_tmp867->tq=Cyc_Toc_mt_tq;({void*_tmpDAF=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp867->type=_tmpDAF;});_tmp867->width=0;_tmp867->attributes=0;_tmp867->requires_clause=0;_tmp867;});_tmp866->hd=_tmpDB0;});({struct Cyc_List_List*_tmpDB1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp866->tl=_tmpDB1;});_tmp866;});
# 4887
fields=_tmpDB2;});{
# 4890
struct Cyc_Absyn_Aggrdecl*_tmp868=({struct Cyc_Absyn_Aggrdecl*_tmp86C=_cycalloc(sizeof(*_tmp86C));_tmp86C->kind=Cyc_Absyn_StructA;_tmp86C->sc=Cyc_Absyn_Public;({struct _tuple1*_tmpDB3=
# 4892
Cyc_Toc_collapse_qvars(f->name,_tmp874->name);_tmp86C->name=_tmpDB3;});_tmp86C->tvs=0;_tmp86C->expected_mem_kind=0;({struct Cyc_Absyn_AggrdeclImpl*_tmpDB4=({struct Cyc_Absyn_AggrdeclImpl*_tmp86D=_cycalloc(sizeof(*_tmp86D));_tmp86D->exist_vars=0;_tmp86D->rgn_po=0;_tmp86D->fields=fields;_tmp86D->tagged=0;_tmp86D;});_tmp86C->impl=_tmpDB4;});_tmp86C->attributes=0;_tmp86C;});
# 4896
({struct Cyc_List_List*_tmpDB7=({struct Cyc_List_List*_tmp869=_cycalloc(sizeof(*_tmp869));({struct Cyc_Absyn_Decl*_tmpDB6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp86A=_cycalloc(sizeof(*_tmp86A));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDB5=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp86B;_tmp86B.tag=5U;_tmp86B.f1=_tmp868;_tmp86B;});_tmp86A[0]=_tmpDB5;});_tmp86A;}),0U);_tmp869->hd=_tmpDB6;});_tmp869->tl=Cyc_Toc_result_decls;_tmp869;});Cyc_Toc_result_decls=_tmpDB7;});
# 4898
goto _LL6;};};};}}else{if(*((int*)_tmp858)== 0){_LL9: _LLA:
# 4900
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp86E=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp86F=Cyc_Absyn_new_vardecl(0U,f->name,_tmp856,_tmp86E);
_tmp86F->sc=f->sc;
({struct Cyc_List_List*_tmpDBA=({struct Cyc_List_List*_tmp870=_cycalloc(sizeof(*_tmp870));({struct Cyc_Absyn_Decl*_tmpDB9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp871=_cycalloc(sizeof(*_tmp871));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDB8=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp872;_tmp872.tag=0U;_tmp872.f1=_tmp86F;_tmp872;});_tmp871[0]=_tmpDB8;});_tmp871;}),0U);_tmp870->hd=_tmpDB9;});_tmp870->tl=Cyc_Toc_result_decls;_tmp870;});Cyc_Toc_result_decls=_tmpDBA;});
({struct Cyc_Dict_Dict _tmpDBB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp873,f->name,1);*_tmp873=_tmpDBB;});}
# 4907
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4911
return 0;};};}
# 4914
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4918
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4924
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpDBC=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpDBC;});
# 4928
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp876=init->r;void*_tmp877=_tmp876;struct Cyc_Absyn_Vardecl*_tmp87D;struct Cyc_Absyn_Exp*_tmp87C;struct Cyc_Absyn_Exp*_tmp87B;int _tmp87A;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp877)->tag == 27U){_LL1: _tmp87D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp877)->f1;_tmp87C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp877)->f2;_tmp87B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp877)->f3;_tmp87A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp877)->f4;_LL2:
# 4935
 vd->initializer=0;
({void*_tmpDC4=({struct Cyc_Toc_Env*_tmpDC3=nv;void*_tmpDC2=old_typ;struct Cyc_Absyn_Exp*_tmpDC1=Cyc_Absyn_var_exp(vd->name,0U);struct Cyc_Absyn_Vardecl*_tmpDC0=_tmp87D;struct Cyc_Absyn_Exp*_tmpDBF=_tmp87C;struct Cyc_Absyn_Exp*_tmpDBE=_tmp87B;int _tmpDBD=_tmp87A;Cyc_Toc_init_comprehension(_tmpDC3,_tmpDC2,_tmpDC1,_tmpDC0,_tmpDBF,_tmpDBE,_tmpDBD,
Cyc_Absyn_new_stmt(s->r,0U),0);})->r;
# 4936
s->r=_tmpDC4;});
# 4938
goto _LL0;}else{_LL3: _LL4:
# 4940
 if(vd->sc == Cyc_Absyn_Static){
# 4944
struct _RegionHandle _tmp878=_new_region("temp");struct _RegionHandle*temp=& _tmp878;_push_region(temp);
{struct Cyc_Toc_Env*_tmp879=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp879,init);}
# 4945
;_pop_region(temp);}else{
# 4949
Cyc_Toc_exp_to_c(nv,init);}
goto _LL0;}_LL0:;}else{
# 4954
void*_tmp87E=Cyc_Tcutil_compress(old_typ);void*_tmp87F=_tmp87E;void*_tmp886;struct Cyc_Absyn_Exp*_tmp885;union Cyc_Absyn_Constraint*_tmp884;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87F)->tag == 8U){_LL6: _tmp886=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87F)->f1).elt_type;_tmp885=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87F)->f1).num_elts;_tmp884=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87F)->f1).zero_term;_LL7:
# 4956
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp884)){
if(_tmp885 == 0)
({void*_tmp880=0U;({struct _dyneither_ptr _tmpDC5=({const char*_tmp881="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp881,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpDC5,_tag_dyneither(_tmp880,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp885;
struct Cyc_Absyn_Exp*_tmp882=({struct Cyc_Absyn_Exp*_tmpDC7=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpDC7,({
struct Cyc_Absyn_Exp*_tmpDC6=num_elts;Cyc_Absyn_add_exp(_tmpDC6,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4963
struct Cyc_Absyn_Exp*_tmp883=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpDC9=({struct Cyc_Absyn_Stmt*_tmpDC8=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp882,_tmp883,0U),0U);Cyc_Toc_seq_stmt_r(_tmpDC8,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4964
s->r=_tmpDC9;});};}
# 4967
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}}
# 4975
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp887=d;struct Cyc_List_List*_tmp88E;struct Cyc_List_List*_tmp88D;void**_tmp88C;struct Cyc_Tcpat_Rhs*_tmp88B;switch(*((int*)_tmp887)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp88B=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp887)->f1;_LL4:
 _tmp88B->rhs=success;return d;default: _LL5: _tmp88E=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp887)->f1;_tmp88D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp887)->f2;_tmp88C=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp887)->f3;_LL6:
# 4980
({void*_tmpDCA=Cyc_Toc_rewrite_decision(*_tmp88C,success);*_tmp88C=_tmpDCA;});
for(0;_tmp88D != 0;_tmp88D=_tmp88D->tl){
struct _tuple29*_tmp888=(struct _tuple29*)_tmp88D->hd;struct _tuple29*_tmp889=_tmp888;void**_tmp88A;_LL8: _tmp88A=(void**)& _tmp889->f2;_LL9:;
({void*_tmpDCB=Cyc_Toc_rewrite_decision(*_tmp88A,success);*_tmp88A=_tmpDCB;});}
# 4985
return d;}_LL0:;}
# 4996 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4998
struct _RegionHandle _tmp88F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp88F;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp890=Cyc_Toc_share_env(rgn,nv);
void*_tmp891=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp890,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 5008
struct Cyc_Absyn_Stmt*_tmp892=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 5012
({void*_tmpDCC=Cyc_Toc_rewrite_decision(dopt,_tmp892);dopt=_tmpDCC;});{
# 5014
struct Cyc_Absyn_Switch_clause*_tmp893=({struct Cyc_Absyn_Switch_clause*_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0->pattern=p;_tmp8A0->pat_vars=0;_tmp8A0->where_clause=0;_tmp8A0->body=_tmp892;_tmp8A0->loc=0U;_tmp8A0;});
struct Cyc_List_List*_tmp894=({struct _RegionHandle*_tmpDCF=rgn;struct _RegionHandle*_tmpDCE=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpDCF,Cyc_Toc_gen_labels,_tmpDCE,({struct Cyc_Absyn_Switch_clause*_tmp89F[1U];_tmp89F[0U]=_tmp893;({struct _RegionHandle*_tmpDCD=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpDCD,_tag_dyneither(_tmp89F,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 5017
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 5021
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp890,& mydecls,& mybodies,dopt,_tmp894,v);
# 5024
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple34*_tmp895=(struct _tuple34*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple34*_tmp896=_tmp895;struct Cyc_Toc_Env*_tmp898;struct Cyc_Absyn_Stmt*_tmp897;_LL1: _tmp898=_tmp896->f1;_tmp897=_tmp896->f3;_LL2:;
if(_tmp897 == _tmp892){senv=_tmp898;goto FOUND_ENV;}}
# 5029
({void*_tmp899=0U;({struct _dyneither_ptr _tmpDD0=({const char*_tmp89A="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp89A,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpDD0,_tag_dyneither(_tmp899,sizeof(void*),0U));});});
FOUND_ENV:
# 5033
 Cyc_Toc_stmt_to_c(senv,s);{
# 5035
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 5037
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp89B=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpDD1=Cyc_Toc_temp_var();_tmp89B->name=_tmpDD1;});
({void*_tmpDD2=Cyc_Toc_typ_to_c_array(_tmp89B->type);_tmp89B->type=_tmpDD2;});
({struct Cyc_Absyn_Stmt*_tmpDD5=({struct Cyc_Absyn_Decl*_tmpDD4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp89C=_cycalloc(sizeof(*_tmp89C));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDD3=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp89D;_tmp89D.tag=0U;_tmp89D.f1=_tmp89B;_tmp89D;});_tmp89C[0]=_tmpDD3;});_tmp89C;}),0U);Cyc_Absyn_decl_stmt(_tmpDD4,res,0U);});res=_tmpDD5;});}
# 5044
({struct Cyc_Absyn_Stmt*_tmpDD9=({struct _tuple1*_tmpDD8=v;void*_tmpDD7=Cyc_Toc_typ_to_c(_tmp891);struct Cyc_Absyn_Exp*_tmpDD6=e;Cyc_Absyn_declare_stmt(_tmpDD8,_tmpDD7,_tmpDD6,res,0U);});res=_tmpDD9;});{
struct Cyc_Absyn_Stmt*_tmp89E=res;_npop_handler(0U);return _tmp89E;};};};};}
# 4999
;_pop_region(rgn);}
# 5052
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8A1=e->r;void*_tmp8A2=_tmp8A1;struct Cyc_Absyn_MallocInfo*_tmp8CC;struct Cyc_Absyn_Stmt*_tmp8CB;void**_tmp8CA;void**_tmp8C9;struct Cyc_List_List*_tmp8C8;struct Cyc_List_List*_tmp8C7;void**_tmp8C6;struct Cyc_List_List*_tmp8C5;void**_tmp8C4;struct Cyc_Absyn_Exp*_tmp8C3;struct Cyc_Absyn_Exp*_tmp8C2;struct Cyc_List_List*_tmp8C1;struct Cyc_Absyn_Exp*_tmp8C0;struct Cyc_Absyn_Exp*_tmp8BF;struct Cyc_Absyn_Exp*_tmp8BE;struct Cyc_Absyn_Exp*_tmp8BD;struct Cyc_Absyn_Exp*_tmp8BC;struct Cyc_Absyn_Exp*_tmp8BB;struct Cyc_Absyn_Exp*_tmp8BA;struct Cyc_Absyn_Exp*_tmp8B9;struct Cyc_Absyn_Exp*_tmp8B8;struct Cyc_Absyn_Exp*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_Absyn_Exp*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B4;struct Cyc_Absyn_Exp*_tmp8B3;struct Cyc_Absyn_Exp*_tmp8B2;struct Cyc_List_List*_tmp8B1;struct Cyc_Absyn_Exp*_tmp8B0;struct Cyc_Absyn_Exp*_tmp8AF;struct Cyc_Absyn_Exp*_tmp8AE;struct Cyc_Absyn_Exp*_tmp8AD;struct Cyc_Absyn_Exp*_tmp8AC;struct Cyc_Absyn_Exp*_tmp8AB;struct Cyc_Absyn_Exp*_tmp8AA;struct Cyc_Absyn_Exp*_tmp8A9;switch(*((int*)_tmp8A2)){case 20U: _LL1: _tmp8A9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LL2:
 _tmp8AA=_tmp8A9;goto _LL4;case 21U: _LL3: _tmp8AA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LL4:
 _tmp8AB=_tmp8AA;goto _LL6;case 22U: _LL5: _tmp8AB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LL6:
 _tmp8AC=_tmp8AB;goto _LL8;case 15U: _LL7: _tmp8AC=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LL8:
 _tmp8AD=_tmp8AC;goto _LLA;case 11U: _LL9: _tmp8AD=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LLA:
 _tmp8AE=_tmp8AD;goto _LLC;case 12U: _LLB: _tmp8AE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LLC:
 _tmp8AF=_tmp8AE;goto _LLE;case 18U: _LLD: _tmp8AF=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LLE:
 _tmp8B0=_tmp8AF;goto _LL10;case 5U: _LLF: _tmp8B0=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LL10:
 Cyc_Toc_exptypes_to_c(_tmp8B0);goto _LL0;case 3U: _LL11: _tmp8B1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_LL12:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8B1);goto _LL0;case 7U: _LL13: _tmp8B3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_tmp8B2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_LL14:
 _tmp8B5=_tmp8B3;_tmp8B4=_tmp8B2;goto _LL16;case 8U: _LL15: _tmp8B5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_tmp8B4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_LL16:
 _tmp8B7=_tmp8B5;_tmp8B6=_tmp8B4;goto _LL18;case 9U: _LL17: _tmp8B7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_tmp8B6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_LL18:
 _tmp8B9=_tmp8B7;_tmp8B8=_tmp8B6;goto _LL1A;case 23U: _LL19: _tmp8B9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_tmp8B8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_LL1A:
 _tmp8BB=_tmp8B9;_tmp8BA=_tmp8B8;goto _LL1C;case 35U: _LL1B: _tmp8BB=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_tmp8BA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_LL1C:
 _tmp8BD=_tmp8BB;_tmp8BC=_tmp8BA;goto _LL1E;case 4U: _LL1D: _tmp8BD=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_tmp8BC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8A2)->f3;_LL1E:
 Cyc_Toc_exptypes_to_c(_tmp8BD);Cyc_Toc_exptypes_to_c(_tmp8BC);goto _LL0;case 6U: _LL1F: _tmp8C0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_tmp8BF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_tmp8BE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8A2)->f3;_LL20:
# 5070
 Cyc_Toc_exptypes_to_c(_tmp8C0);Cyc_Toc_exptypes_to_c(_tmp8BF);Cyc_Toc_exptypes_to_c(_tmp8BE);goto _LL0;case 10U: _LL21: _tmp8C2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_tmp8C1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_LL22:
# 5072
 Cyc_Toc_exptypes_to_c(_tmp8C2);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8C1);goto _LL0;case 14U: _LL23: _tmp8C4=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_tmp8C3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_LL24:
({void*_tmpDDA=Cyc_Toc_typ_to_c(*_tmp8C4);*_tmp8C4=_tmpDDA;});Cyc_Toc_exptypes_to_c(_tmp8C3);goto _LL0;case 25U: _LL25: _tmp8C6=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1)->f3;_tmp8C5=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_LL26:
# 5075
({void*_tmpDDB=Cyc_Toc_typ_to_c(*_tmp8C6);*_tmp8C6=_tmpDDB;});
_tmp8C7=_tmp8C5;goto _LL28;case 36U: _LL27: _tmp8C7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8A2)->f2;_LL28:
 _tmp8C8=_tmp8C7;goto _LL2A;case 26U: _LL29: _tmp8C8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LL2A:
# 5079
 for(0;_tmp8C8 != 0;_tmp8C8=_tmp8C8->tl){
struct _tuple19 _tmp8A3=*((struct _tuple19*)_tmp8C8->hd);struct _tuple19 _tmp8A4=_tmp8A3;struct Cyc_Absyn_Exp*_tmp8A5;_LL54: _tmp8A5=_tmp8A4.f2;_LL55:;
Cyc_Toc_exptypes_to_c(_tmp8A5);}
# 5083
goto _LL0;case 19U: _LL2B: _tmp8C9=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LL2C:
 _tmp8CA=_tmp8C9;goto _LL2E;case 17U: _LL2D: _tmp8CA=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LL2E:
({void*_tmpDDC=Cyc_Toc_typ_to_c(*_tmp8CA);*_tmp8CA=_tmpDDC;});goto _LL0;case 37U: _LL2F: _tmp8CB=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LL30:
 Cyc_Toc_stmttypes_to_c(_tmp8CB);goto _LL0;case 34U: _LL31: _tmp8CC=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8A2)->f1;_LL32:
# 5088
 if(_tmp8CC->elt_type != 0)
({void**_tmpDDE=({void**_tmp8A6=_cycalloc(sizeof(*_tmp8A6));({void*_tmpDDD=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8CC->elt_type)));_tmp8A6[0]=_tmpDDD;});_tmp8A6;});_tmp8CC->elt_type=_tmpDDE;});
Cyc_Toc_exptypes_to_c(_tmp8CC->num_elts);
goto _LL0;case 0U: _LL33: _LL34:
 goto _LL36;case 1U: _LL35: _LL36:
 goto _LL38;case 32U: _LL37: _LL38:
 goto _LL3A;case 40U: _LL39: _LL3A:
 goto _LL3C;case 33U: _LL3B: _LL3C:
 goto _LL0;case 2U: _LL3D: _LL3E:
# 5098
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
({void*_tmp8A7=0U;({unsigned int _tmpDE0=e->loc;struct _dyneither_ptr _tmpDDF=({const char*_tmp8A8="Cyclone expression within C code";_tag_dyneither(_tmp8A8,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpDE0,_tmpDDF,_tag_dyneither(_tmp8A7,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 5112
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp8CD=d->r;void*_tmp8CE=_tmp8CD;struct Cyc_Absyn_Typedefdecl*_tmp8D8;struct Cyc_Absyn_Enumdecl*_tmp8D7;struct Cyc_Absyn_Aggrdecl*_tmp8D6;struct Cyc_Absyn_Fndecl*_tmp8D5;struct Cyc_Absyn_Vardecl*_tmp8D4;switch(*((int*)_tmp8CE)){case 0U: _LL1: _tmp8D4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8CE)->f1;_LL2:
# 5115
({void*_tmpDE1=Cyc_Toc_typ_to_c(_tmp8D4->type);_tmp8D4->type=_tmpDE1;});
if(_tmp8D4->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8D4->initializer));
goto _LL0;case 1U: _LL3: _tmp8D5=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8CE)->f1;_LL4:
# 5119
({void*_tmpDE2=Cyc_Toc_typ_to_c(_tmp8D5->ret_type);_tmp8D5->ret_type=_tmpDE2;});
{struct Cyc_List_List*_tmp8CF=_tmp8D5->args;for(0;_tmp8CF != 0;_tmp8CF=_tmp8CF->tl){
({void*_tmpDE3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8CF->hd)).f3);(*((struct _tuple9*)_tmp8CF->hd)).f3=_tmpDE3;});}}
# 5123
goto _LL0;case 5U: _LL5: _tmp8D6=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8CE)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp8D6,1);goto _LL0;case 7U: _LL7: _tmp8D7=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8CE)->f1;_LL8:
# 5126
 if(_tmp8D7->fields != 0){
struct Cyc_List_List*_tmp8D0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8D7->fields))->v;for(0;_tmp8D0 != 0;_tmp8D0=_tmp8D0->tl){
struct Cyc_Absyn_Enumfield*_tmp8D1=(struct Cyc_Absyn_Enumfield*)_tmp8D0->hd;
if(_tmp8D1->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8D1->tag));}}
# 5131
goto _LL0;case 8U: _LL9: _tmp8D8=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8CE)->f1;_LLA:
({void*_tmpDE4=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8D8->defn));_tmp8D8->defn=_tmpDE4;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 5141
({void*_tmp8D2=0U;({unsigned int _tmpDE6=d->loc;struct _dyneither_ptr _tmpDE5=({const char*_tmp8D3="Cyclone declaration within C code";_tag_dyneither(_tmp8D3,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpDE6,_tmpDE5,_tag_dyneither(_tmp8D2,sizeof(void*),0U));});});
goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 5145
 goto _LL0;}_LL0:;}
# 5149
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp8D9=s->r;void*_tmp8DA=_tmp8D9;struct Cyc_Absyn_Stmt*_tmp8F5;struct Cyc_Absyn_Stmt*_tmp8F4;struct Cyc_Absyn_Exp*_tmp8F3;struct Cyc_Absyn_Decl*_tmp8F2;struct Cyc_Absyn_Stmt*_tmp8F1;struct Cyc_Absyn_Exp*_tmp8F0;struct Cyc_List_List*_tmp8EF;void*_tmp8EE;struct Cyc_Absyn_Exp*_tmp8ED;struct Cyc_Absyn_Exp*_tmp8EC;struct Cyc_Absyn_Exp*_tmp8EB;struct Cyc_Absyn_Stmt*_tmp8EA;struct Cyc_Absyn_Exp*_tmp8E9;struct Cyc_Absyn_Stmt*_tmp8E8;struct Cyc_Absyn_Exp*_tmp8E7;struct Cyc_Absyn_Stmt*_tmp8E6;struct Cyc_Absyn_Stmt*_tmp8E5;struct Cyc_Absyn_Exp*_tmp8E4;struct Cyc_Absyn_Stmt*_tmp8E3;struct Cyc_Absyn_Stmt*_tmp8E2;struct Cyc_Absyn_Exp*_tmp8E1;switch(*((int*)_tmp8DA)){case 1U: _LL1: _tmp8E1=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp8E1);goto _LL0;case 2U: _LL3: _tmp8E3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f1;_tmp8E2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp8E3);Cyc_Toc_stmttypes_to_c(_tmp8E2);goto _LL0;case 3U: _LL5: _tmp8E4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f1;_LL6:
 if(_tmp8E4 != 0)Cyc_Toc_exptypes_to_c(_tmp8E4);goto _LL0;case 4U: _LL7: _tmp8E7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f1;_tmp8E6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f2;_tmp8E5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f3;_LL8:
# 5155
 Cyc_Toc_exptypes_to_c(_tmp8E7);Cyc_Toc_stmttypes_to_c(_tmp8E6);Cyc_Toc_stmttypes_to_c(_tmp8E5);goto _LL0;case 5U: _LL9: _tmp8E9=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f1).f1;_tmp8E8=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f2;_LLA:
# 5157
 Cyc_Toc_exptypes_to_c(_tmp8E9);Cyc_Toc_stmttypes_to_c(_tmp8E8);goto _LL0;case 9U: _LLB: _tmp8ED=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f1;_tmp8EC=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f2).f1;_tmp8EB=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f3).f1;_tmp8EA=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f4;_LLC:
# 5159
 Cyc_Toc_exptypes_to_c(_tmp8ED);Cyc_Toc_exptypes_to_c(_tmp8EC);Cyc_Toc_exptypes_to_c(_tmp8EB);
Cyc_Toc_stmttypes_to_c(_tmp8EA);goto _LL0;case 10U: _LLD: _tmp8F0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f1;_tmp8EF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f2;_tmp8EE=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f3;_LLE:
# 5162
 Cyc_Toc_exptypes_to_c(_tmp8F0);
for(0;_tmp8EF != 0;_tmp8EF=_tmp8EF->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp8EF->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp8F2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f1;_tmp8F1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp8F2);Cyc_Toc_stmttypes_to_c(_tmp8F1);goto _LL0;case 14U: _LL11: _tmp8F4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f1;_tmp8F3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp8F4);Cyc_Toc_exptypes_to_c(_tmp8F3);goto _LL0;case 13U: _LL13: _tmp8F5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8DA)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp8F5);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 5172
 goto _LL0;case 11U: _LL1D: _LL1E:
# 5175
({void*_tmpDE8=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp8DB=_cycalloc_atomic(sizeof(*_tmp8DB));({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmpDE7=({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmp8DC;_tmp8DC.tag=0U;_tmp8DC;});_tmp8DB[0]=_tmpDE7;});_tmp8DB;});s->r=_tmpDE8;});
goto _LL0;default: _LL1F: _LL20:
# 5178
({void*_tmp8DD=0U;({unsigned int _tmpDEC=s->loc;struct _dyneither_ptr _tmpDEB=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8E0;_tmp8E0.tag=0U;({struct _dyneither_ptr _tmpDE9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp8E0.f1=_tmpDE9;});({void*_tmp8DE[1U]={& _tmp8E0};({struct _dyneither_ptr _tmpDEA=({const char*_tmp8DF="Cyclone statement in C code: %s";_tag_dyneither(_tmp8DF,sizeof(char),32U);});Cyc_aprintf(_tmpDEA,_tag_dyneither(_tmp8DE,sizeof(void*),1U));});});});Cyc_Tcutil_terr(_tmpDEC,_tmpDEB,_tag_dyneither(_tmp8DD,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 5186
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp8F6=0U;({struct _dyneither_ptr _tmpDED=({const char*_tmp8F7="decls_to_c: not at toplevel!";_tag_dyneither(_tmp8F7,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDED,_tag_dyneither(_tmp8F6,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp8F8=d->r;void*_tmp8F9=_tmp8F8;struct Cyc_List_List*_tmp911;struct Cyc_List_List*_tmp910;struct Cyc_List_List*_tmp90F;struct Cyc_List_List*_tmp90E;struct Cyc_Absyn_Typedefdecl*_tmp90D;struct Cyc_Absyn_Enumdecl*_tmp90C;struct Cyc_Absyn_Datatypedecl*_tmp90B;struct Cyc_Absyn_Aggrdecl*_tmp90A;struct Cyc_Absyn_Fndecl*_tmp909;struct Cyc_Absyn_Vardecl*_tmp908;switch(*((int*)_tmp8F9)){case 0U: _LL1: _tmp908=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8F9)->f1;_LL2: {
# 5194
struct _tuple1*_tmp8FA=_tmp908->name;
# 5196
if(_tmp908->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpDEF=({struct _tuple1*_tmp8FB=_cycalloc(sizeof(*_tmp8FB));({union Cyc_Absyn_Nmspace _tmpDEE=Cyc_Absyn_Abs_n(0,1);_tmp8FB->f1=_tmpDEE;});_tmp8FB->f2=(*_tmp8FA).f2;_tmp8FB;});_tmp8FA=_tmpDEF;});
if(_tmp908->initializer != 0){
if(_tmp908->sc == Cyc_Absyn_ExternC)_tmp908->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp908->initializer));else{
# 5203
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp908->initializer));}}
# 5205
_tmp908->name=_tmp8FA;
({enum Cyc_Absyn_Scope _tmpDF0=Cyc_Toc_scope_to_c(_tmp908->sc);_tmp908->sc=_tmpDF0;});
({void*_tmpDF1=Cyc_Toc_typ_to_c(_tmp908->type);_tmp908->type=_tmpDF1;});
({struct Cyc_List_List*_tmpDF2=({struct Cyc_List_List*_tmp8FC=_cycalloc(sizeof(*_tmp8FC));_tmp8FC->hd=d;_tmp8FC->tl=Cyc_Toc_result_decls;_tmp8FC;});Cyc_Toc_result_decls=_tmpDF2;});
goto _LL0;}case 1U: _LL3: _tmp909=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8F9)->f1;_LL4: {
# 5211
struct _tuple1*_tmp8FD=_tmp909->name;
# 5213
if(_tmp909->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpDF4=({struct _tuple1*_tmp8FE=_cycalloc(sizeof(*_tmp8FE));({union Cyc_Absyn_Nmspace _tmpDF3=Cyc_Absyn_Abs_n(0,1);_tmp8FE->f1=_tmpDF3;});_tmp8FE->f2=(*_tmp8FD).f2;_tmp8FE;});_tmp8FD=_tmpDF4;});
_tmp909->sc=Cyc_Absyn_Public;}
# 5217
_tmp909->name=_tmp8FD;
Cyc_Toc_fndecl_to_c(nv,_tmp909,cinclude);
({struct Cyc_List_List*_tmpDF5=({struct Cyc_List_List*_tmp8FF=_cycalloc(sizeof(*_tmp8FF));_tmp8FF->hd=d;_tmp8FF->tl=Cyc_Toc_result_decls;_tmp8FF;});Cyc_Toc_result_decls=_tmpDF5;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp900=0U;({struct _dyneither_ptr _tmpDF6=({const char*_tmp901="letdecl at toplevel";_tag_dyneither(_tmp901,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpDF6,_tag_dyneither(_tmp900,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp902=0U;({struct _dyneither_ptr _tmpDF7=({const char*_tmp903="region decl at toplevel";_tag_dyneither(_tmp903,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpDF7,_tag_dyneither(_tmp902,sizeof(void*),0U));});});case 5U: _LLB: _tmp90A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8F9)->f1;_LLC:
# 5225
 Cyc_Toc_aggrdecl_to_c(_tmp90A,1);
goto _LL0;case 6U: _LLD: _tmp90B=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp8F9)->f1;_LLE:
# 5228
 if(_tmp90B->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp90B);else{
# 5231
Cyc_Toc_datatypedecl_to_c(_tmp90B);}
goto _LL0;case 7U: _LLF: _tmp90C=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8F9)->f1;_LL10:
# 5234
 Cyc_Toc_enumdecl_to_c(_tmp90C);
({struct Cyc_List_List*_tmpDF8=({struct Cyc_List_List*_tmp904=_cycalloc(sizeof(*_tmp904));_tmp904->hd=d;_tmp904->tl=Cyc_Toc_result_decls;_tmp904;});Cyc_Toc_result_decls=_tmpDF8;});
goto _LL0;case 8U: _LL11: _tmp90D=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8F9)->f1;_LL12:
# 5238
 _tmp90D->tvs=0;
if(_tmp90D->defn != 0)
({void*_tmpDF9=Cyc_Toc_typ_to_c((void*)_check_null(_tmp90D->defn));_tmp90D->defn=_tmpDF9;});else{
# 5242
enum Cyc_Absyn_KindQual _tmp905=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp90D->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp906=_tmp905;if(_tmp906 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpDFA=Cyc_Absyn_void_star_typ();_tmp90D->defn=_tmpDFA;});goto _LL1F;}else{_LL22: _LL23:
 _tmp90D->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL1F;}_LL1F:;}
# 5249
if(Cyc_noexpand_r  || Cyc_Absynpp_is_anon_aggrtype((void*)_check_null(_tmp90D->defn)))
({struct Cyc_List_List*_tmpDFB=({struct Cyc_List_List*_tmp907=_cycalloc(sizeof(*_tmp907));_tmp907->hd=d;_tmp907->tl=Cyc_Toc_result_decls;_tmp907;});Cyc_Toc_result_decls=_tmpDFB;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
# 5254
 goto _LL0;case 9U: _LL17: _tmp90E=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp8F9)->f2;_LL18:
 _tmp90F=_tmp90E;goto _LL1A;case 10U: _LL19: _tmp90F=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp8F9)->f2;_LL1A:
 _tmp910=_tmp90F;goto _LL1C;case 11U: _LL1B: _tmp910=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp8F9)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpDFC=Cyc_Toc_decls_to_c(r,nv,_tmp910,top,cinclude);nv=_tmpDFC;});goto _LL0;default: _LL1D: _tmp911=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp8F9)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpDFD=Cyc_Toc_decls_to_c(r,nv,_tmp911,top,1);nv=_tmpDFD;});goto _LL0;}_LL0:;};}
# 5261
return nv;}
# 5265
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp912=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp913=_tmp912;struct Cyc_Core_DynamicRegion*_tmp917;_LL1: _tmp917=_tmp913.key;_LL2:;{
struct Cyc_Toc_TocState*_tmp914=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp917,0,Cyc_Toc_empty_toc_state);
({struct Cyc_Toc_TocStateWrap*_tmpDFE=({struct Cyc_Toc_TocStateWrap*_tmp915=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp915));_tmp915->dyn=(struct Cyc_Core_DynamicRegion*)_tmp917;_tmp915->state=(struct Cyc_Toc_TocState*)_tmp914;_tmp915;});Cyc_Toc_toc_state=_tmpDFE;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0U;
Cyc_Toc_bounds_checks_eliminated=0U;
({struct _dyneither_ptr _tmpDFF=_tag_dyneither(({struct _dyneither_ptr**_tmp916=_cycalloc(sizeof(struct _dyneither_ptr*)* 41);_tmp916[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str;_tmp916[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str;_tmp916[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str;_tmp916[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str;_tmp916[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str;_tmp916[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str;_tmp916[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str;_tmp916[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str;_tmp916[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str;_tmp916[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str;_tmp916[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str;_tmp916[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str;_tmp916[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str;_tmp916[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str;_tmp916[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str;_tmp916[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str;_tmp916[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str;_tmp916[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str;_tmp916[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp916[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str;_tmp916[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp916[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp916[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str;_tmp916[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str;_tmp916[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str;_tmp916[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str;_tmp916[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str;_tmp916[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str;_tmp916[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str;_tmp916[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str;_tmp916[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str;_tmp916[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str;_tmp916[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str;_tmp916[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str;_tmp916[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str;_tmp916[35]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str;_tmp916[36]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp916[37]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str;_tmp916[38]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str;_tmp916[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str;_tmp916[40]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str;_tmp916;}),sizeof(struct _dyneither_ptr*),41U);Cyc_Toc_globals=_tmpDFF;});};}
# 5321
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp918=*ts;struct Cyc_Toc_TocState _tmp919=_tmp918;struct Cyc_Xarray_Xarray*_tmp91A;_LL1: _tmp91A=_tmp919.temp_labels;_LL2:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp91A);
return 0;}
# 5327
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp91B=ts;struct Cyc_Toc_TocStateWrap*_tmp91C=Cyc_Toc_toc_state;ts=_tmp91C;Cyc_Toc_toc_state=_tmp91B;});{
struct Cyc_Toc_TocStateWrap _tmp91D=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp91E=_tmp91D;struct Cyc_Core_DynamicRegion*_tmp920;struct Cyc_Toc_TocState*_tmp91F;_LL1: _tmp920=_tmp91E.dyn;_tmp91F=_tmp91E.state;_LL2:;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp920,_tmp91F,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp920);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 5335
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 5341
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 5343
({struct Cyc_Hashtable_Table**_tmpE00=({struct Cyc_Hashtable_Table**_tmp921=_cycalloc(sizeof(*_tmp921));_tmp921[0]=pop_tables;_tmp921;});Cyc_Toc_gpop_tables=_tmpE00;});
Cyc_Toc_init();
{struct _RegionHandle _tmp922=_new_region("start");struct _RegionHandle*start=& _tmp922;_push_region(start);
({struct _RegionHandle*_tmpE02=start;struct Cyc_Toc_Env*_tmpE01=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpE02,_tmpE01,ds,1,0);});;_pop_region(start);}
# 5352
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
