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
({struct _dyneither_ptr _tmp789=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp788=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp788,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp789,ap);});}
# 80
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 82
({struct _dyneither_ptr _tmp78B=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp78A=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp78A,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp78B,ap);});}
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
(((((((((((((({struct Cyc_List_List**_tmp797=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});(*_tmp11).tuple_types=_tmp797;}),({
struct Cyc_List_List**_tmp796=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});(*_tmp11).anon_aggr_types=_tmp796;}))),({
struct Cyc_Dict_Dict*_tmp795=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp794=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp794;});_tmpC;});(*_tmp11).aggrs_so_far=_tmp795;}))),({
struct Cyc_List_List**_tmp793=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});(*_tmp11).abs_struct_types=_tmp793;}))),({
struct Cyc_Set_Set**_tmp792=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp791=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp791;});_tmpE;});(*_tmp11).datatypes_so_far=_tmp792;}))),({
struct Cyc_Dict_Dict*_tmp790=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp78F=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp78F;});_tmpF;});(*_tmp11).xdatatypes_so_far=_tmp790;}))),({
struct Cyc_Dict_Dict*_tmp78E=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp78D=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp78D;});_tmp10;});(*_tmp11).qvar_tags=_tmp78E;}))),({
struct Cyc_Xarray_Xarray*_tmp78C=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);(*_tmp11).temp_labels=_tmp78C;}));_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 160
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 166
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 169
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp798=({struct Cyc_Toc_TocStateWrap _tmp77C;(_tmp77C.dyn=_tmp1C,_tmp77C.state=_tmp1B);_tmp77C;});*ts=_tmp798;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp1A=({struct _tuple16 _tmp77D;(_tmp77D.f1=_tmp1B,_tmp77D.f2=arg);_tmp77D;});
({void*_tmp799=f(h,& _tmp1A);res=_tmp799;});}
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
struct _tuple17 env=({struct _tuple17 _tmp77E;(_tmp77E.f1=q,_tmp77E.f2=type_maker);_tmp77E;});
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
return Cyc_Absyn_skip_stmt(0U);}
# 315
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp71=e->r;void*_tmp72=_tmp71;struct Cyc_Absyn_Exp*_tmp73;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp73=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp73);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 321
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));((((((((*_tmp74).tag=14U,(*_tmp74).f1=t)),(*_tmp74).f2=e)),(*_tmp74).f3=0)),(*_tmp74).f4=Cyc_Absyn_No_coercion);_tmp74;});}
# 324
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));((*_tmp75).tag=20U,(*_tmp75).f1=e);_tmp75;});}
# 327
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));((((*_tmp76).tag=23U,(*_tmp76).f1=e1)),(*_tmp76).f2=e2);_tmp76;});}
# 330
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));((*_tmp77).tag=37U,(*_tmp77).f1=s);_tmp77;});}
# 333
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));((*_tmp78).tag=17U,(*_tmp78).f1=t);_tmp78;});}
# 336
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));((((((((*_tmp79).tag=10U,(*_tmp79).f1=e)),(*_tmp79).f2=es)),(*_tmp79).f3=0)),(*_tmp79).f4=1);_tmp79;});}
# 339
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));((*_tmp7A).tag=1U,(*_tmp7A).f1=e);_tmp7A;});}
# 342
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));((((*_tmp7B).tag=2U,(*_tmp7B).f1=s1)),(*_tmp7B).f2=s2);_tmp7B;});}
# 345
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));((((((*_tmp7C).tag=6U,(*_tmp7C).f1=e1)),(*_tmp7C).f2=e2)),(*_tmp7C).f3=e3);_tmp7C;});}
# 348
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));((((((((*_tmp7D).tag=21U,(*_tmp7D).f1=e)),(*_tmp7D).f2=n)),(*_tmp7D).f3=0)),(*_tmp7D).f4=0);_tmp7D;});}
# 351
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));((((((((*_tmp7E).tag=22U,(*_tmp7E).f1=e)),(*_tmp7E).f2=n)),(*_tmp7E).f3=0)),(*_tmp7E).f4=0);_tmp7E;});}
# 354
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 357
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));((((*_tmp7F).tag=36U,(*_tmp7F).f1=tdopt)),(*_tmp7F).f2=ds);_tmp7F;});}
# 359
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));((*_tmp80).tag=8U,(*_tmp80).f1=v);_tmp80;});}
# 362
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 366
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp81=e->r;void*_tmp82=_tmp81;struct Cyc_Absyn_Exp*_tmp83;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->tag == 20U){_LL1: _tmp83=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp83,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 385
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 394
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 403
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 412
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 423
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp84=Cyc_Tcutil_compress(t);void*_tmp85=_tmp84;enum Cyc_Absyn_Size_of _tmp8C;switch(*((int*)_tmp85)){case 6U: _LL1: _tmp8C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85)->f2;_LL2:
# 427
{enum Cyc_Absyn_Size_of _tmp86=_tmp8C;switch(_tmp86){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp87=0U;({struct _dyneither_ptr _tmp79A=({const char*_tmp88="impossible IntType (not char, short or int)";_tag_dyneither(_tmp88,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp79A,_tag_dyneither(_tmp87,sizeof(void*),0U));});});}_LLD:;}
# 433
goto _LL0;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp85)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}case 5U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0U;({struct _dyneither_ptr _tmp79B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp8B.f1=_tmp79B;});({void*_tmp89[1U];_tmp89[0]=& _tmp8B;({struct _dyneither_ptr _tmp79C=({const char*_tmp8A="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp8A,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp79C,_tag_dyneither(_tmp89,sizeof(void*),1U));});});});}_LL0:;}
# 440
return function;}
# 442
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8D=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8E=_tmp8D;void*_tmp91;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->tag == 5U){_LL1: _tmp91=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->f1).elt_typ;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp91);}else{_LL3: _LL4:
({void*_tmp8F=0U;({struct _dyneither_ptr _tmp79D=({const char*_tmp90="impossible type (not pointer)";_tag_dyneither(_tmp90,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp79D,_tag_dyneither(_tmp8F,sizeof(void*),0U));});});}_LL0:;}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 452
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp92=e->r;void*_tmp93=_tmp92;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;long long _tmp99;int _tmp98;short _tmp97;struct _dyneither_ptr _tmp96;char _tmp95;switch(*((int*)_tmp93)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Null_c).tag){case 2U: _LL1: _tmp95=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Char_c).val).f2;_LL2:
 return _tmp95 == '\000';case 3U: _LL3: _tmp96=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Wchar_c).val;_LL4: {
# 456
unsigned long _tmp94=Cyc_strlen((struct _dyneither_ptr)_tmp96);
int i=0;
if(_tmp94 >= 2  && *((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp96.curr)[1]== 'x'  && _tmp94 >= 3) && *((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp94;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 466
return 0;}}case 4U: _LL5: _tmp97=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Short_c).val).f2;_LL6:
 return _tmp97 == 0;case 5U: _LL7: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Int_c).val).f2;_LL8:
 return _tmp98 == 0;case 6U: _LL9: _tmp99=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).LongLong_c).val).f2;_LLA:
 return _tmp99 == 0;case 1U: _LLD: _LLE:
# 471
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 470
 goto _LLE;case 14U: _LLF: _tmp9A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL10:
# 472
 return Cyc_Toc_is_zero(_tmp9A);case 24U: _LL11: _tmp9B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9B);case 26U: _LL13: _tmp9C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL14:
 _tmp9D=_tmp9C;goto _LL16;case 29U: _LL15: _tmp9D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_LL16:
 _tmp9E=_tmp9D;goto _LL18;case 25U: _LL17: _tmp9E=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL18:
 _tmp9F=_tmp9E;goto _LL1A;case 36U: _LL19: _tmp9F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL1A:
# 478
 for(0;_tmp9F != 0;_tmp9F=_tmp9F->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple20*)_tmp9F->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 487
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA0=x;union Cyc_Absyn_Nmspace _tmpB2;struct _dyneither_ptr*_tmpB1;_LL1: _tmpB2=_tmpA0->f1;_tmpB1=_tmpA0->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA1=_tmpB2;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*_tmpAE;switch((_tmpA1.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAE=0;goto _LL7;case 1U: _LL6: _tmpAE=(_tmpA1.Rel_n).val;_LL7:
 _tmpAF=_tmpAE;goto _LL9;case 2U: _LL8: _tmpAF=(_tmpA1.Abs_n).val;_LL9:
 _tmpB0=_tmpAF;goto _LLB;default: _LLA: _tmpB0=(_tmpA1.C_n).val;_LLB:
# 496
 if(_tmpB0 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA5;_tmpA5.tag=0U;_tmpA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);({struct Cyc_String_pa_PrintArg_struct _tmpA4;_tmpA4.tag=0U;_tmpA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);({void*_tmpA2[2U];(_tmpA2[0]=& _tmpA4,_tmpA2[1]=& _tmpA5);({struct _dyneither_ptr _tmp79E=({const char*_tmpA3="%s_%s_struct";_tag_dyneither(_tmpA3,sizeof(char),13U);});Cyc_aprintf(_tmp79E,_tag_dyneither(_tmpA2,sizeof(void*),2U));});});});});{
struct _RegionHandle _tmpA6=_new_region("r");struct _RegionHandle*r=& _tmpA6;_push_region(r);
{struct _dyneither_ptr _tmpAD=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAB;_tmpAB.tag=0U;_tmpAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);({struct Cyc_String_pa_PrintArg_struct _tmpAA;_tmpAA.tag=0U;({struct _dyneither_ptr _tmp7A1=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp7A0=r;struct Cyc_List_List*_tmp79F=_tmpB0;Cyc_rstr_sepstr(_tmp7A0,_tmp79F,({const char*_tmpAC="_";_tag_dyneither(_tmpAC,sizeof(char),2U);}));}));_tmpAA.f1=_tmp7A1;});({struct Cyc_String_pa_PrintArg_struct _tmpA9;_tmpA9.tag=0U;_tmpA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);({void*_tmpA7[3U];(((_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA)),_tmpA7[2]=& _tmpAB);({struct _dyneither_ptr _tmp7A2=({const char*_tmpA8="%s_%s_%s_struct";_tag_dyneither(_tmpA8,sizeof(char),16U);});Cyc_aprintf(_tmp7A2,_tag_dyneither(_tmpA7,sizeof(void*),3U));});});});});});_npop_handler(0U);return _tmpAD;};_pop_region(r);};}_LL3:;};}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
# 509
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){
# 512
struct _tuple21 _tmpB3=*env;struct _tuple21 _tmpB4=_tmpB3;struct Cyc_Dict_Dict*_tmpC7;struct _tuple15*_tmpC6;_LL1: _tmpC7=(_tmpB4.f1)->qvar_tags;_tmpC6=_tmpB4.f2;_LL2:;{
struct _tuple15 _tmpB5=*_tmpC6;struct _tuple15 _tmpB6=_tmpB5;struct _tuple1*_tmpC5;struct _tuple1*_tmpC4;_LL4: _tmpC5=_tmpB6.f1;_tmpC4=_tmpB6.f2;_LL5:;{
struct _handler_cons _tmpB7;_push_handler(& _tmpB7);{int _tmpB9=0;if(setjmp(_tmpB7.handler))_tmpB9=1;if(!_tmpB9){{struct _tuple1*_tmpBA=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple15*,struct _tuple15*),struct _tuple15*k))Cyc_Dict_lookup_other)(*_tmpC7,Cyc_Toc_qvar_tag_cmp,_tmpC6);_npop_handler(0U);return _tmpBA;};_pop_handler();}else{void*_tmpB8=(void*)_exn_thrown;void*_tmpBB=_tmpB8;void*_tmpC3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBB)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 516
struct _tuple15*_tmpBC=({struct _tuple15*_tmpC2=_cycalloc(sizeof(*_tmpC2));((*_tmpC2).f1=_tmpC5,(*_tmpC2).f2=_tmpC4);_tmpC2;});
struct _tuple1*_tmpBD=_tmpC5;union Cyc_Absyn_Nmspace _tmpC1;struct _dyneither_ptr*_tmpC0;_LLC: _tmpC1=_tmpBD->f1;_tmpC0=_tmpBD->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC0,_tmpC4);
struct _tuple1*res=({struct _tuple1*_tmpBF=_cycalloc(sizeof(*_tmpBF));((*_tmpBF).f1=_tmpC1,({struct _dyneither_ptr*_tmp7A3=({struct _dyneither_ptr*_tmpBE=_cycalloc(sizeof(*_tmpBE));*_tmpBE=newvar;_tmpBE;});(*_tmpBF).f2=_tmp7A3;}));_tmpBF;});
({struct Cyc_Dict_Dict _tmp7A4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC7,_tmpBC,res);*_tmpC7=_tmp7A4;});
return res;};}}else{_LL9: _tmpC3=_tmpBB;_LLA:(int)_rethrow(_tmpC3);}_LL6:;}};};};}
# 525
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp77F;(_tmp77F.f1=fieldname,_tmp77F.f2=dtname);_tmp77F;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 532
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));((((((((((((*_tmpCA).kind=Cyc_Absyn_StructA,(*_tmpCA).sc=Cyc_Absyn_Public)),(*_tmpCA).tvs=0)),(*_tmpCA).attributes=0)),(*_tmpCA).expected_mem_kind=0)),({
# 535
struct _tuple1*_tmp7A7=({struct _tuple1*_tmpC8=_cycalloc(sizeof(*_tmpC8));(({union Cyc_Absyn_Nmspace _tmp7A6=Cyc_Absyn_Rel_n(0);(*_tmpC8).f1=_tmp7A6;}),(*_tmpC8).f2=name);_tmpC8;});(*_tmpCA).name=_tmp7A7;}))),({
struct Cyc_Absyn_AggrdeclImpl*_tmp7A5=({struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_cycalloc(sizeof(*_tmpC9));((((((*_tmpC9).exist_vars=0,(*_tmpC9).rgn_po=0)),(*_tmpC9).tagged=0)),(*_tmpC9).fields=fs);_tmpC9;});(*_tmpCA).impl=_tmp7A5;}));_tmpCA;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 544
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){
# 547
struct _tuple22 _tmpCB=*env;struct _tuple22 _tmpCC=_tmpCB;struct Cyc_List_List**_tmpE3;struct Cyc_List_List*_tmpE2;_LL1: _tmpE3=(_tmpCC.f1)->tuple_types;_tmpE2=_tmpCC.f2;_LL2:;
# 549
{struct Cyc_List_List*_tmpCD=*_tmpE3;for(0;_tmpCD != 0;_tmpCD=_tmpCD->tl){
struct _tuple23*_tmpCE=(struct _tuple23*)_tmpCD->hd;struct _tuple23*_tmpCF=_tmpCE;void*_tmpD2;struct Cyc_List_List*_tmpD1;_LL4: _tmpD2=_tmpCF->f1;_tmpD1=_tmpCF->f2;_LL5:;{
struct Cyc_List_List*_tmpD0=_tmpE2;
for(0;_tmpD0 != 0  && _tmpD1 != 0;(_tmpD0=_tmpD0->tl,_tmpD1=_tmpD1->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD0->hd)).f2,(void*)_tmpD1->hd))
break;}
if(_tmpD0 == 0  && _tmpD1 == 0)
return _tmpD2;};}}{
# 560
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE1=_cycalloc(sizeof(*_tmpE1));({struct _dyneither_ptr _tmp7A9=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE0;_tmpE0.tag=1U;_tmpE0.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmpDE[1U];_tmpDE[0]=& _tmpE0;({struct _dyneither_ptr _tmp7A8=({const char*_tmpDF="_tuple%d";_tag_dyneither(_tmpDF,sizeof(char),9U);});Cyc_aprintf(_tmp7A8,_tag_dyneither(_tmpDE,sizeof(void*),1U));});});});*_tmpE1=_tmp7A9;});_tmpE1;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE2);
struct Cyc_List_List*_tmpD3=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp7AC=({struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));(({struct Cyc_Absyn_Aggrfield*_tmp7AB=({struct Cyc_Absyn_Aggrfield*_tmpD4=_cycalloc(sizeof(*_tmpD4));(((((((((({struct _dyneither_ptr*_tmp7AA=Cyc_Absyn_fieldname(i);(*_tmpD4).name=_tmp7AA;}),(*_tmpD4).tq=Cyc_Toc_mt_tq)),(*_tmpD4).type=(void*)ts2->hd)),(*_tmpD4).width=0)),(*_tmpD4).attributes=0)),(*_tmpD4).requires_clause=0);_tmpD4;});(*_tmpD5).hd=_tmp7AB;}),(*_tmpD5).tl=_tmpD3);_tmpD5;});_tmpD3=_tmp7AC;});}}
({struct Cyc_List_List*_tmp7AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD3);_tmpD3=_tmp7AD;});{
struct Cyc_Absyn_Aggrdecl*_tmpD6=Cyc_Toc_make_c_struct_defn(xname,_tmpD3);
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD7=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));((*_tmpDD).tag=11U,((({union Cyc_Absyn_AggrInfoU _tmp7AE=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDC=_cycalloc(sizeof(*_tmpDC));*_tmpDC=_tmpD6;_tmpDC;}));((*_tmpDD).f1).aggr_info=_tmp7AE;}),((*_tmpDD).f1).targs=0)));_tmpDD;});
({struct Cyc_List_List*_tmp7B0=({struct Cyc_List_List*_tmpD9=_cycalloc(sizeof(*_tmpD9));(({struct Cyc_Absyn_Decl*_tmp7AF=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));((*_tmpD8).tag=5U,(*_tmpD8).f1=_tmpD6);_tmpD8;}),0U);(*_tmpD9).hd=_tmp7AF;}),(*_tmpD9).tl=Cyc_Toc_result_decls);_tmpD9;});Cyc_Toc_result_decls=_tmp7B0;});
({struct Cyc_List_List*_tmp7B2=({struct Cyc_List_List*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));(({struct _tuple23*_tmp7B1=({struct _tuple23*_tmpDA=_region_malloc(d,sizeof(*_tmpDA));((*_tmpDA).f1=(void*)_tmpD7,(*_tmpDA).f2=ts);_tmpDA;});(*_tmpDB).hd=_tmp7B1;}),(*_tmpDB).tl=*_tmpE3);_tmpDB;});*_tmpE3=_tmp7B2;});
return(void*)_tmpD7;};};}
# 573
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 578
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 581
struct _tuple25*_tmpE4=env;struct Cyc_List_List**_tmpF9;enum Cyc_Absyn_AggrKind _tmpF8;struct Cyc_List_List*_tmpF7;_LL1: _tmpF9=(_tmpE4->f1)->anon_aggr_types;_tmpF8=(_tmpE4->f2)->f1;_tmpF7=(_tmpE4->f2)->f2;_LL2:;
# 583
{struct Cyc_List_List*_tmpE5=*_tmpF9;for(0;_tmpE5 != 0;_tmpE5=_tmpE5->tl){
struct _tuple26*_tmpE6=(struct _tuple26*)_tmpE5->hd;struct _tuple26*_tmpE7=_tmpE6;void*_tmpEA;enum Cyc_Absyn_AggrKind _tmpE9;struct Cyc_List_List*_tmpE8;_LL4: _tmpEA=_tmpE7->f1;_tmpE9=_tmpE7->f2;_tmpE8=_tmpE7->f3;_LL5:;
if(_tmpF8 != _tmpE9)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpE8,_tmpF7))
return _tmpEA;}}{
# 592
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF6=_cycalloc(sizeof(*_tmpF6));({struct _dyneither_ptr _tmp7B4=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF5;_tmpF5.tag=1U;_tmpF5.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmpF3[1U];_tmpF3[0]=& _tmpF5;({struct _dyneither_ptr _tmp7B3=({const char*_tmpF4="_tuple%d";_tag_dyneither(_tmpF4,sizeof(char),9U);});Cyc_aprintf(_tmp7B3,_tag_dyneither(_tmpF3,sizeof(void*),1U));});});});*_tmpF6=_tmp7B4;});_tmpF6;});
struct Cyc_Absyn_Aggrdecl*_tmpEB=Cyc_Toc_make_c_struct_defn(xname,_tmpF7);
_tmpEB->kind=_tmpF8;{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpEC=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));((*_tmpF2).tag=11U,((({union Cyc_Absyn_AggrInfoU _tmp7B5=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF1=_cycalloc(sizeof(*_tmpF1));*_tmpF1=_tmpEB;_tmpF1;}));((*_tmpF2).f1).aggr_info=_tmp7B5;}),((*_tmpF2).f1).targs=0)));_tmpF2;});
({struct Cyc_List_List*_tmp7B7=({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*_tmpEE));(({struct Cyc_Absyn_Decl*_tmp7B6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpED=_cycalloc(sizeof(*_tmpED));((*_tmpED).tag=5U,(*_tmpED).f1=_tmpEB);_tmpED;}),0U);(*_tmpEE).hd=_tmp7B6;}),(*_tmpEE).tl=Cyc_Toc_result_decls);_tmpEE;});Cyc_Toc_result_decls=_tmp7B7;});
({struct Cyc_List_List*_tmp7B9=({struct Cyc_List_List*_tmpF0=_region_malloc(d,sizeof(*_tmpF0));(({struct _tuple26*_tmp7B8=({struct _tuple26*_tmpEF=_region_malloc(d,sizeof(*_tmpEF));((((*_tmpEF).f1=(void*)_tmpEC,(*_tmpEF).f2=_tmpF8)),(*_tmpEF).f3=_tmpF7);_tmpEF;});(*_tmpF0).hd=_tmp7B8;}),(*_tmpF0).tl=*_tmpF9);_tmpF0;});*_tmpF9=_tmp7B9;});
return(void*)_tmpEC;};};}
# 600
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpFA=_cycalloc(sizeof(*_tmpFA));((*_tmpFA).f1=ak,(*_tmpFA).f2=fs);_tmpFA;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 609
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 618
struct _tuple28 _tmpFB=*env;struct _tuple28 _tmpFC=_tmpFB;struct Cyc_List_List**_tmp11F;struct _tuple1*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_List_List*_tmp11C;struct Cyc_List_List*_tmp11B;_LL1: _tmp11F=(_tmpFC.f1)->abs_struct_types;_tmp11E=(_tmpFC.f2)->f1;_tmp11D=(_tmpFC.f2)->f2;_tmp11C=(_tmpFC.f2)->f3;_tmp11B=(_tmpFC.f2)->f4;_LL2:;
# 622
{struct Cyc_List_List*_tmpFD=*_tmp11F;for(0;_tmpFD != 0;_tmpFD=_tmpFD->tl){
struct _tuple29*_tmpFE=(struct _tuple29*)_tmpFD->hd;struct _tuple29*_tmpFF=_tmpFE;struct _tuple1*_tmp107;struct Cyc_List_List*_tmp106;void*_tmp105;_LL4: _tmp107=_tmpFF->f1;_tmp106=_tmpFF->f2;_tmp105=_tmpFF->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp107,_tmp11E)== 0  && ({
int _tmp7BA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11C);_tmp7BA == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp106);})){
int okay=1;
{struct Cyc_List_List*_tmp100=_tmp11C;for(0;_tmp100 != 0;(_tmp100=_tmp100->tl,_tmp106=_tmp106->tl)){
void*_tmp101=(void*)_tmp100->hd;
void*_tmp102=(void*)((struct Cyc_List_List*)_check_null(_tmp106))->hd;
{struct Cyc_Absyn_Kind*_tmp103=Cyc_Tcutil_typ_kind(_tmp101);struct Cyc_Absyn_Kind*_tmp104=_tmp103;switch(((struct Cyc_Absyn_Kind*)_tmp104)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 633
 continue;default: _LLB: _LLC:
# 636
 if(Cyc_Tcutil_unify(_tmp101,_tmp102) || ({void*_tmp7BB=Cyc_Toc_typ_to_c(_tmp101);Cyc_Tcutil_unify(_tmp7BB,Cyc_Toc_typ_to_c(_tmp102));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 641
break;}}
# 643
if(okay)
return _tmp105;}}}{
# 650
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp11A=_cycalloc(sizeof(*_tmp11A));({struct _dyneither_ptr _tmp7BD=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp119;_tmp119.tag=1U;_tmp119.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp117[1U];_tmp117[0]=& _tmp119;({struct _dyneither_ptr _tmp7BC=({const char*_tmp118="_tuple%d";_tag_dyneither(_tmp118,sizeof(char),9U);});Cyc_aprintf(_tmp7BC,_tag_dyneither(_tmp117,sizeof(void*),1U));});});});*_tmp11A=_tmp7BD;});_tmp11A;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp108=0;
# 654
({struct Cyc_List_List*_tmp7BF=({struct Cyc_List_List*_tmp10A=_region_malloc(d,sizeof(*_tmp10A));(({struct _tuple29*_tmp7BE=({struct _tuple29*_tmp109=_region_malloc(d,sizeof(*_tmp109));((((*_tmp109).f1=_tmp11E,(*_tmp109).f2=_tmp11C)),(*_tmp109).f3=x);_tmp109;});(*_tmp10A).hd=_tmp7BE;}),(*_tmp10A).tl=*_tmp11F);_tmp10A;});*_tmp11F=_tmp7BF;});{
# 657
struct _RegionHandle _tmp10B=_new_region("r");struct _RegionHandle*r=& _tmp10B;_push_region(r);
{struct Cyc_List_List*_tmp10C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11D,_tmp11C);
for(0;_tmp11B != 0;_tmp11B=_tmp11B->tl){
struct Cyc_Absyn_Aggrfield*_tmp10D=(struct Cyc_Absyn_Aggrfield*)_tmp11B->hd;
void*t=_tmp10D->type;
struct Cyc_List_List*atts=_tmp10D->attributes;
# 665
if(_tmp11B->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t)))
({struct Cyc_List_List*_tmp7C1=({struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*_tmp10F));(({void*_tmp7C0=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));((*_tmp10E).tag=6U,(*_tmp10E).f1=0);_tmp10E;});(*_tmp10F).hd=_tmp7C0;}),(*_tmp10F).tl=atts);_tmp10F;});atts=_tmp7C1;});
# 668
({void*_tmp7C2=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10C,t));t=_tmp7C2;});
# 671
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
({void*_tmp7C7=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp110=_cycalloc(sizeof(*_tmp110));((*_tmp110).tag=8U,((((((((({void*_tmp7C6=Cyc_Absyn_void_star_typ();((*_tmp110).f1).elt_type=_tmp7C6;}),({struct Cyc_Absyn_Tqual _tmp7C5=Cyc_Absyn_empty_tqual(0U);((*_tmp110).f1).tq=_tmp7C5;}))),({
struct Cyc_Absyn_Exp*_tmp7C4=Cyc_Absyn_uint_exp(0U,0U);((*_tmp110).f1).num_elts=_tmp7C4;}))),({union Cyc_Absyn_Constraint*_tmp7C3=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);((*_tmp110).f1).zero_term=_tmp7C3;}))),((*_tmp110).f1).zt_loc=0U)));_tmp110;});
# 672
t=_tmp7C7;});
# 675
({struct Cyc_List_List*_tmp7C9=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));(({struct Cyc_Absyn_Aggrfield*_tmp7C8=({struct Cyc_Absyn_Aggrfield*_tmp111=_cycalloc(sizeof(*_tmp111));((((((((((*_tmp111).name=_tmp10D->name,(*_tmp111).tq=Cyc_Toc_mt_tq)),(*_tmp111).type=t)),(*_tmp111).width=_tmp10D->width)),(*_tmp111).attributes=atts)),(*_tmp111).requires_clause=0);_tmp111;});(*_tmp112).hd=_tmp7C8;}),(*_tmp112).tl=_tmp108);_tmp112;});_tmp108=_tmp7C9;});}
# 677
({struct Cyc_List_List*_tmp7CA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp108);_tmp108=_tmp7CA;});{
struct Cyc_Absyn_Aggrdecl*_tmp113=Cyc_Toc_make_c_struct_defn(xname,_tmp108);
({struct Cyc_List_List*_tmp7CC=({struct Cyc_List_List*_tmp115=_cycalloc(sizeof(*_tmp115));(({struct Cyc_Absyn_Decl*_tmp7CB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp114=_cycalloc(sizeof(*_tmp114));((*_tmp114).tag=5U,(*_tmp114).f1=_tmp113);_tmp114;}),0U);(*_tmp115).hd=_tmp7CB;}),(*_tmp115).tl=Cyc_Toc_result_decls);_tmp115;});Cyc_Toc_result_decls=_tmp7CC;});{
void*_tmp116=x;_npop_handler(0U);return _tmp116;};};}
# 658
;_pop_region(r);};};}
# 683
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 687
struct _tuple27 env=({struct _tuple27 _tmp780;(((((_tmp780.f1=struct_name,_tmp780.f2=type_vars)),_tmp780.f3=type_args)),_tmp780.f4=fields);_tmp780;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 695
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp124=_cycalloc(sizeof(*_tmp124));((*_tmp124).f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp7CF=({struct _dyneither_ptr*_tmp123=_cycalloc(sizeof(*_tmp123));({struct _dyneither_ptr _tmp7CE=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp122;_tmp122.tag=1U;_tmp122.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;({void*_tmp120[1U];_tmp120[0]=& _tmp122;({struct _dyneither_ptr _tmp7CD=({const char*_tmp121="_tmp%X";_tag_dyneither(_tmp121,sizeof(char),7U);});Cyc_aprintf(_tmp7CD,_tag_dyneither(_tmp120,sizeof(void*),1U));});});});*_tmp123=_tmp7CE;});_tmp123;});(*_tmp124).f2=_tmp7CF;}));_tmp124;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 701
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp125=*env;struct _tuple30 _tmp126=_tmp125;struct Cyc_Xarray_Xarray*_tmp12E;_LL1: _tmp12E=(_tmp126.f1)->temp_labels;_LL2:;{
int _tmp127=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp7D0=_tmp127;_tmp7D0 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12E);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12E,_tmp127);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12D=_cycalloc(sizeof(*_tmp12D));({struct _dyneither_ptr _tmp7D2=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12C;_tmp12C.tag=1U;_tmp12C.f1=(unsigned int)_tmp127;({void*_tmp12A[1U];_tmp12A[0]=& _tmp12C;({struct _dyneither_ptr _tmp7D1=({const char*_tmp12B="_LL%X";_tag_dyneither(_tmp12B,sizeof(char),6U);});Cyc_aprintf(_tmp7D1,_tag_dyneither(_tmp12A,sizeof(void*),1U));});});});*_tmp12D=_tmp7D2;});_tmp12D;});
if(({int _tmp7D3=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12E,res);_tmp7D3 != _tmp127;}))
({void*_tmp128=0U;({struct _dyneither_ptr _tmp7D4=({const char*_tmp129="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp129,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7D4,_tag_dyneither(_tmp128,sizeof(void*),0U));});});
return res;};};}
# 712
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 718
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp12F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp12F))->hd)->name)!= 0){
++ ans;
_tmp12F=_tmp12F->tl;}
# 725
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 731
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp130=*a;struct _tuple9 _tmp131=_tmp130;struct _dyneither_ptr*_tmp135;struct Cyc_Absyn_Tqual _tmp134;void*_tmp133;_LL1: _tmp135=_tmp131.f1;_tmp134=_tmp131.f2;_tmp133=_tmp131.f3;_LL2:;
return({struct _tuple9*_tmp132=_cycalloc(sizeof(*_tmp132));((((*_tmp132).f1=_tmp135,(*_tmp132).f2=_tmp134)),({void*_tmp7D5=Cyc_Toc_typ_to_c(_tmp133);(*_tmp132).f3=_tmp7D5;}));_tmp132;});}
# 754 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp136=Cyc_Tcutil_compress(t);void*_tmp137=_tmp136;void*_tmp13D;void*_tmp13C;struct Cyc_Absyn_Tqual _tmp13B;struct Cyc_Absyn_Exp*_tmp13A;union Cyc_Absyn_Constraint*_tmp139;unsigned int _tmp138;switch(*((int*)_tmp137)){case 8U: _LL1: _tmp13C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp137)->f1).elt_type;_tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp137)->f1).tq;_tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp137)->f1).num_elts;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp137)->f1).zero_term;_tmp138=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp137)->f1).zt_loc;_LL2:
# 757
 return({void*_tmp7D6=Cyc_Toc_typ_to_c_array(_tmp13C);Cyc_Absyn_cstar_typ(_tmp7D6,_tmp13B);});case 1U: _LL3: _tmp13D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp137)->f2;if(_tmp13D != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13D);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 763
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 765
struct Cyc_Absyn_Aggrfield*_tmp13E=({struct Cyc_Absyn_Aggrfield*_tmp13F=_cycalloc(sizeof(*_tmp13F));*_tmp13F=*f;_tmp13F;});
_tmp13E->type=new_type;
_tmp13E->requires_clause=0;
_tmp13E->tq=Cyc_Toc_mt_tq;
return _tmp13E;}
# 772
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 774
return;}
# 777
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
({void**_tmp7D8=({void**_tmp140=_cycalloc(sizeof(*_tmp140));({void*_tmp7D7=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);*_tmp140=_tmp7D7;});_tmp140;});cs=_tmp7D8;});
return*cs;}
# 783
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp7DB=({void**_tmp141=_cycalloc(sizeof(*_tmp141));({void*_tmp7DA=({void*_tmp7D9=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp7D9,Cyc_Toc_mt_tq);});*_tmp141=_tmp7DA;});_tmp141;});r=_tmp7DB;});
return*r;}
# 789
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp7DE=({void**_tmp142=_cycalloc(sizeof(*_tmp142));({void*_tmp7DD=({void*_tmp7DC=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp7DC,Cyc_Toc_mt_tq);});*_tmp142=_tmp7DD;});_tmp142;});r=_tmp7DE;});
return*r;}
# 795
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp143=Cyc_Tcutil_compress(t);void*_tmp144=_tmp143;struct Cyc_Absyn_Tvar*_tmp145;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp144)->tag == 2U){_LL1: _tmp145=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp144)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 801
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp146=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp147=_tmp146;if(((struct Cyc_Absyn_Kind*)_tmp147)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 808
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp148=t;struct Cyc_Absyn_Datatypedecl*_tmp191;void**_tmp190;struct Cyc_Absyn_Enumdecl*_tmp18F;struct Cyc_Absyn_Aggrdecl*_tmp18E;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18C;struct _tuple1*_tmp18B;struct Cyc_List_List*_tmp18A;struct Cyc_Absyn_Typedefdecl*_tmp189;void*_tmp188;struct Cyc_List_List*_tmp187;struct _tuple1*_tmp186;union Cyc_Absyn_AggrInfoU _tmp185;struct Cyc_List_List*_tmp184;enum Cyc_Absyn_AggrKind _tmp183;struct Cyc_List_List*_tmp182;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Tqual _tmp180;void*_tmp17F;struct Cyc_List_List*_tmp17E;int _tmp17D;struct Cyc_Absyn_VarargInfo*_tmp17C;struct Cyc_List_List*_tmp17B;void*_tmp17A;struct Cyc_Absyn_Tqual _tmp179;struct Cyc_Absyn_Exp*_tmp178;unsigned int _tmp177;void*_tmp176;struct Cyc_Absyn_Tqual _tmp175;union Cyc_Absyn_Constraint*_tmp174;struct Cyc_Absyn_Datatypedecl*_tmp173;struct Cyc_Absyn_Datatypefield*_tmp172;struct Cyc_Absyn_Tvar*_tmp171;void**_tmp170;switch(*((int*)_tmp148)){case 0U: _LL1: _LL2:
 return t;case 1U: _LL3: _tmp170=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp148)->f2;_LL4:
# 812
 if(*_tmp170 == 0){
*_tmp170=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 816
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp170));case 2U: _LL5: _tmp171=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp148)->f1;_LL6:
# 818
 if((Cyc_Tcutil_tvar_kind(_tmp171,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 820
return(void*)& Cyc_Absyn_VoidType_val;else{
# 822
return Cyc_Absyn_void_star_typ();}case 3U: _LL7: _LL8:
# 824
 return(void*)& Cyc_Absyn_VoidType_val;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp148)->f1).field_info).KnownDatatypefield).tag == 2){_LL9: _tmp173=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp148)->f1).field_info).KnownDatatypefield).val).f1;_tmp172=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp148)->f1).field_info).KnownDatatypefield).val).f2;_LLA:
# 826
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp172->name,_tmp173->name));}else{_LLB: _LLC:
({void*_tmp149=0U;({struct _dyneither_ptr _tmp7DF=({const char*_tmp14A="unresolved DatatypeFieldType";_tag_dyneither(_tmp14A,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7DF,_tag_dyneither(_tmp149,sizeof(void*),0U));});});}case 5U: _LLD: _tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp148)->f1).elt_typ;_tmp175=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp148)->f1).elt_tq;_tmp174=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp148)->f1).ptr_atts).bounds;_LLE:
# 831
({void*_tmp7E0=Cyc_Toc_typ_to_c(_tmp176);_tmp176=_tmp7E0;});{
void*_tmp14B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp174);void*_tmp14C=_tmp14B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp14C)->tag == 0U){_LL42: _LL43:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LL44: _LL45:
 return Cyc_Absyn_star_typ(_tmp176,(void*)& Cyc_Absyn_HeapRgn_val,_tmp175,Cyc_Absyn_false_conref);}_LL41:;};case 6U: _LLF: _LL10:
# 836
 goto _LL12;case 7U: _LL11: _LL12:
 return t;case 8U: _LL13: _tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp148)->f1).elt_type;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp148)->f1).tq;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp148)->f1).num_elts;_tmp177=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp148)->f1).zt_loc;_LL14:
# 839
 return({void*_tmp7E4=Cyc_Toc_typ_to_c(_tmp17A);struct Cyc_Absyn_Tqual _tmp7E3=_tmp179;struct Cyc_Absyn_Exp*_tmp7E2=_tmp178;union Cyc_Absyn_Constraint*_tmp7E1=Cyc_Absyn_false_conref;Cyc_Absyn_array_typ(_tmp7E4,_tmp7E3,_tmp7E2,_tmp7E1,_tmp177);});case 9U: _LL15: _tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp148)->f1).ret_tqual;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp148)->f1).ret_typ;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp148)->f1).args;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp148)->f1).c_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp148)->f1).cyc_varargs;_tmp17B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp148)->f1).attributes;_LL16: {
# 845
struct Cyc_List_List*_tmp14D=0;
for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
void*_tmp14E=(void*)_tmp17B->hd;void*_tmp14F=_tmp14E;switch(*((int*)_tmp14F)){case 4U: _LL47: _LL48:
 goto _LL4A;case 5U: _LL49: _LL4A:
 goto _LL4C;case 19U: _LL4B: _LL4C:
 continue;case 22U: _LL4D: _LL4E:
 continue;case 21U: _LL4F: _LL50:
 continue;case 20U: _LL51: _LL52:
 continue;default: _LL53: _LL54:
({struct Cyc_List_List*_tmp7E5=({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*_tmp150));((*_tmp150).hd=(void*)_tmp17B->hd,(*_tmp150).tl=_tmp14D);_tmp150;});_tmp14D=_tmp7E5;});goto _LL46;}_LL46:;}{
# 856
struct Cyc_List_List*_tmp151=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17E);
if(_tmp17C != 0){
# 859
void*_tmp152=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp17C->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp153=({struct _tuple9*_tmp155=_cycalloc(sizeof(*_tmp155));((((*_tmp155).f1=_tmp17C->name,(*_tmp155).f2=_tmp17C->tq)),(*_tmp155).f3=_tmp152);_tmp155;});
({struct Cyc_List_List*_tmp7E7=({struct Cyc_List_List*_tmp7E6=_tmp151;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp7E6,({struct Cyc_List_List*_tmp154=_cycalloc(sizeof(*_tmp154));((*_tmp154).hd=_tmp153,(*_tmp154).tl=0);_tmp154;}));});_tmp151=_tmp7E7;});}
# 863
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp156=_cycalloc(sizeof(*_tmp156));((*_tmp156).tag=9U,((((((((((((((((((((((((((*_tmp156).f1).tvars=0,((*_tmp156).f1).effect=0)),((*_tmp156).f1).ret_tqual=_tmp180)),({void*_tmp7E8=Cyc_Toc_typ_to_c(_tmp17F);((*_tmp156).f1).ret_typ=_tmp7E8;}))),((*_tmp156).f1).args=_tmp151)),((*_tmp156).f1).c_varargs=_tmp17D)),((*_tmp156).f1).cyc_varargs=0)),((*_tmp156).f1).rgn_po=0)),((*_tmp156).f1).attributes=_tmp14D)),((*_tmp156).f1).requires_clause=0)),((*_tmp156).f1).requires_relns=0)),((*_tmp156).f1).ensures_clause=0)),((*_tmp156).f1).ensures_relns=0)));_tmp156;});};}case 10U: _LL17: _tmp181=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp148)->f1;_LL18: {
# 868
struct Cyc_List_List*_tmp157=0;
for(0;_tmp181 != 0;_tmp181=_tmp181->tl){
({struct Cyc_List_List*_tmp7EB=({struct Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));(({struct _tuple12*_tmp7EA=({struct _tuple12*_tmp158=_cycalloc(sizeof(*_tmp158));((*_tmp158).f1=(*((struct _tuple12*)_tmp181->hd)).f1,({void*_tmp7E9=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp181->hd)).f2);(*_tmp158).f2=_tmp7E9;}));_tmp158;});(*_tmp159).hd=_tmp7EA;}),(*_tmp159).tl=_tmp157);_tmp159;});_tmp157=_tmp7EB;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp157));}case 12U: _LL19: _tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp148)->f1;_tmp182=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp148)->f2;_LL1A: {
# 877
struct Cyc_List_List*_tmp15A=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp7EE=({struct Cyc_List_List*_tmp15B=_cycalloc(sizeof(*_tmp15B));(({struct Cyc_Absyn_Aggrfield*_tmp7ED=({struct Cyc_Absyn_Aggrfield*_tmp7EC=(struct Cyc_Absyn_Aggrfield*)_tmp182->hd;Cyc_Toc_aggrfield_to_c(_tmp7EC,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp182->hd)->type));});(*_tmp15B).hd=_tmp7ED;}),(*_tmp15B).tl=_tmp15A);_tmp15B;});_tmp15A=_tmp7EE;});}
return({enum Cyc_Absyn_AggrKind _tmp7EF=_tmp183;Cyc_Toc_add_anon_aggr_type(_tmp7EF,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15A));});}case 11U: _LL1B: _tmp185=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp148)->f1).aggr_info;_tmp184=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp148)->f1).targs;_LL1C:
# 883
{union Cyc_Absyn_AggrInfoU _tmp15C=_tmp185;if((_tmp15C.UnknownAggr).tag == 1){_LL56: _LL57:
 return t;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}{
# 887
struct Cyc_Absyn_Aggrdecl*_tmp15D=Cyc_Absyn_get_known_aggrdecl(_tmp185);
if(_tmp15D->expected_mem_kind){
# 890
if(_tmp15D->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp161;_tmp161.tag=0U;({struct _dyneither_ptr _tmp7F0=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 893
Cyc_Absynpp_qvar2string(_tmp15D->name));_tmp161.f1=_tmp7F0;});({struct Cyc_String_pa_PrintArg_struct _tmp160;_tmp160.tag=0U;({struct _dyneither_ptr _tmp7F1=(struct _dyneither_ptr)(
# 892
_tmp15D->kind == Cyc_Absyn_UnionA?({const char*_tmp162="union";_tag_dyneither(_tmp162,sizeof(char),6U);}):({const char*_tmp163="struct";_tag_dyneither(_tmp163,sizeof(char),7U);}));_tmp160.f1=_tmp7F1;});({void*_tmp15E[2U];(_tmp15E[0]=& _tmp160,_tmp15E[1]=& _tmp161);({struct _dyneither_ptr _tmp7F2=({const char*_tmp15F="%s %s was never defined.";_tag_dyneither(_tmp15F,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp7F2,_tag_dyneither(_tmp15E,sizeof(void*),2U));});});});});}
# 898
if(_tmp15D->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp164=_tmp15D->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15D->impl))->fields;
if(_tmp164 == 0)return Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp164))->tl != 0;_tmp164=_tmp164->tl){;}{
void*_tmp165=((struct Cyc_Absyn_Aggrfield*)_tmp164->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp165))){
if(_tmp15D->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp168;_tmp168.tag=0U;({struct _dyneither_ptr _tmp7F3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15D->name));_tmp168.f1=_tmp7F3;});({void*_tmp166[1U];_tmp166[0]=& _tmp168;({struct _dyneither_ptr _tmp7F4=({const char*_tmp167="struct %s ended up being abstract.";_tag_dyneither(_tmp167,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp7F4,_tag_dyneither(_tmp166,sizeof(void*),1U));});});});{
# 911
struct _RegionHandle _tmp169=_new_region("r");struct _RegionHandle*r=& _tmp169;_push_region(r);
{struct Cyc_List_List*_tmp16A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15D->tvs,_tmp184);
void*_tmp16B=Cyc_Tcutil_rsubstitute(r,_tmp16A,_tmp165);
if(Cyc_Toc_is_abstract_type(_tmp16B)){void*_tmp16C=Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp16C;}{
void*_tmp16D=Cyc_Toc_add_struct_type(_tmp15D->name,_tmp15D->tvs,_tmp184,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15D->impl))->fields);_npop_handler(0U);return _tmp16D;};}
# 912
;_pop_region(r);};}
# 917
return Cyc_Toc_aggrdecl_type(_tmp15D->name,Cyc_Absyn_strctq);};};};case 13U: _LL1D: _tmp186=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp148)->f1;_LL1E:
 return t;case 14U: _LL1F: _tmp187=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp148)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp187);return t;case 17U: _LL21: _tmp18B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp148)->f1;_tmp18A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp148)->f2;_tmp189=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp148)->f3;_tmp188=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp148)->f4;_LL22:
# 923
 if(_tmp188 == 0){
if(_tmp18A != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));((((((((*_tmp16E).tag=17U,(*_tmp16E).f1=_tmp18B)),(*_tmp16E).f2=0)),(*_tmp16E).f3=_tmp189)),(*_tmp16E).f4=0);_tmp16E;});else{
return t;}}else{
# 928
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));((((((((*_tmp16F).tag=17U,(*_tmp16F).f1=_tmp18B)),(*_tmp16F).f2=0)),(*_tmp16F).f3=_tmp189)),({void*_tmp7F5=Cyc_Toc_typ_to_c(_tmp188);(*_tmp16F).f4=_tmp7F5;}));_tmp16F;});}case 19U: _LL23: _LL24:
 return Cyc_Absyn_uint_typ;case 15U: _LL25: _LL26:
 return Cyc_Toc_rgn_typ();case 16U: _LL27: _LL28:
 return Cyc_Toc_dyn_rgn_typ();case 20U: _LL29: _LL2A:
# 934
 goto _LL2C;case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 23U: _LL2F: _LL30:
 goto _LL32;case 24U: _LL31: _LL32:
 goto _LL34;case 25U: _LL33: _LL34:
 return Cyc_Absyn_void_star_typ();case 18U: _LL35: _tmp18C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp148)->f1;_LL36:
# 944
 return t;case 27U: _LL37: _tmp18D=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp148)->f1;_LL38:
# 946
 return t;case 28U: _LL39: _LL3A:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)){case 0U: _LL3B: _tmp18E=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)->f1;_LL3C:
# 949
 Cyc_Toc_aggrdecl_to_c(_tmp18E,1);
if(_tmp18E->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18E->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp18E->name,Cyc_Absyn_strctq);}case 1U: _LL3D: _tmp18F=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)->f1;_LL3E:
# 954
 Cyc_Toc_enumdecl_to_c(_tmp18F);
return t;default: _LL3F: _tmp191=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)->f1;_tmp190=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f2;_LL40:
# 957
 Cyc_Toc_datatypedecl_to_c(_tmp191);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp190)));}}_LL0:;}
# 962
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp192=t;void*_tmp194;struct Cyc_Absyn_Tqual _tmp193;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192)->tag == 8U){_LL1: _tmp194=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192)->f1).elt_type;_tmp193=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192)->f1).tq;_LL2:
# 965
 return({void*_tmp7F6=Cyc_Absyn_star_typ(_tmp194,(void*)& Cyc_Absyn_HeapRgn_val,_tmp193,Cyc_Absyn_false_conref);Cyc_Toc_cast_it(_tmp7F6,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 972
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp195=Cyc_Tcutil_compress(t);void*_tmp196=_tmp195;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_Datatypedecl*_tmp1A1;struct Cyc_Absyn_Datatypefield*_tmp1A0;struct Cyc_List_List*_tmp19F;union Cyc_Absyn_AggrInfoU _tmp19E;void*_tmp19D;switch(*((int*)_tmp196)){case 0U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 0;case 6U: _LL5: _LL6:
 goto _LL8;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;case 9U: _LLD: _LLE:
 goto _LL10;case 19U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmp19D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp196)->f1).elt_type;_LL12:
 return Cyc_Toc_atomic_typ(_tmp19D);case 11U: _LL13: _tmp19E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp196)->f1).aggr_info;_LL14:
# 989
{union Cyc_Absyn_AggrInfoU _tmp197=_tmp19E;if((_tmp197.UnknownAggr).tag == 1){_LL26: _LL27:
 return 0;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}{
# 993
struct Cyc_Absyn_Aggrdecl*_tmp198=Cyc_Absyn_get_known_aggrdecl(_tmp19E);
if(_tmp198->impl == 0)
return 0;
{struct Cyc_List_List*_tmp199=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp198->impl))->fields;for(0;_tmp199 != 0;_tmp199=_tmp199->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp199->hd)->type))return 0;}}
return 1;};case 12U: _LL15: _tmp19F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp196)->f2;_LL16:
# 1000
 for(0;_tmp19F != 0;_tmp19F=_tmp19F->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp19F->hd)->type))return 0;}
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp196)->f1).field_info).KnownDatatypefield).tag == 2){_LL17: _tmp1A1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp196)->f1).field_info).KnownDatatypefield).val).f1;_tmp1A0=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp196)->f1).field_info).KnownDatatypefield).val).f2;_LL18:
# 1004
 _tmp1A2=_tmp1A0->typs;goto _LL1A;}else{goto _LL23;}case 10U: _LL19: _tmp1A2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp196)->f1;_LL1A:
# 1006
 for(0;_tmp1A2 != 0;_tmp1A2=_tmp1A2->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple12*)_tmp1A2->hd)).f2))return 0;}
return 1;case 3U: _LL1B: _LL1C:
# 1011
 goto _LL1E;case 5U: _LL1D: _LL1E:
 goto _LL20;case 16U: _LL1F: _LL20:
 goto _LL22;case 15U: _LL21: _LL22:
 return 0;default: _LL23: _LL24:
({struct Cyc_String_pa_PrintArg_struct _tmp19C;_tmp19C.tag=0U;({struct _dyneither_ptr _tmp7F7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp19C.f1=_tmp7F7;});({void*_tmp19A[1U];_tmp19A[0]=& _tmp19C;({struct _dyneither_ptr _tmp7F8=({const char*_tmp19B="atomic_typ:  bad type %s";_tag_dyneither(_tmp19B,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F8,_tag_dyneither(_tmp19A,sizeof(void*),1U));});});});}_LL0:;}
# 1019
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A3=Cyc_Tcutil_compress(t);void*_tmp1A4=_tmp1A3;void*_tmp1A7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->tag == 5U){_LL1: _tmp1A7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1).elt_typ;_LL2: {
# 1022
void*_tmp1A5=Cyc_Tcutil_compress(_tmp1A7);void*_tmp1A6=_tmp1A5;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1A6)->tag == 0U){_LL6: _LL7:
 return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 1026
 return 0;}_LL0:;}
# 1030
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1034
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1039
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1A8=Cyc_Tcutil_compress(t);void*_tmp1A9=_tmp1A8;struct Cyc_List_List*_tmp1B5;union Cyc_Absyn_AggrInfoU _tmp1B4;switch(*((int*)_tmp1A9)){case 11U: _LL1: _tmp1B4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A9)->f1).aggr_info;_LL2: {
# 1042
struct Cyc_Absyn_Aggrdecl*_tmp1AA=Cyc_Absyn_get_known_aggrdecl(_tmp1B4);
if(_tmp1AA->impl == 0)
({void*_tmp1AB=0U;({struct _dyneither_ptr _tmp7F9=({const char*_tmp1AC="is_poly_field: type missing fields";_tag_dyneither(_tmp1AC,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F9,_tag_dyneither(_tmp1AB,sizeof(void*),0U));});});
_tmp1B5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AA->impl))->fields;goto _LL4;}case 12U: _LL3: _tmp1B5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A9)->f2;_LL4: {
# 1047
struct Cyc_Absyn_Aggrfield*_tmp1AD=Cyc_Absyn_lookup_field(_tmp1B5,f);
if(_tmp1AD == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B0;_tmp1B0.tag=0U;_tmp1B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp1AE[1U];_tmp1AE[0]=& _tmp1B0;({struct _dyneither_ptr _tmp7FA=({const char*_tmp1AF="is_poly_field: bad field %s";_tag_dyneither(_tmp1AF,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7FA,_tag_dyneither(_tmp1AE,sizeof(void*),1U));});});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AD->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0U;({struct _dyneither_ptr _tmp7FB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1B3.f1=_tmp7FB;});({void*_tmp1B1[1U];_tmp1B1[0]=& _tmp1B3;({struct _dyneither_ptr _tmp7FC=({const char*_tmp1B2="is_poly_field: bad type %s";_tag_dyneither(_tmp1B2,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7FC,_tag_dyneither(_tmp1B1,sizeof(void*),1U));});});});}_LL0:;}
# 1058
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B6=e->r;void*_tmp1B7=_tmp1B6;struct Cyc_Absyn_Exp*_tmp1C1;struct _dyneither_ptr*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1BF;struct _dyneither_ptr*_tmp1BE;switch(*((int*)_tmp1B7)){case 21U: _LL1: _tmp1BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B7)->f1;_tmp1BE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B7)->f2;_LL2:
# 1061
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1BF->topt),_tmp1BE) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B7)->f1;_tmp1C0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B7)->f2;_LL4: {
# 1064
void*_tmp1B8=Cyc_Tcutil_compress((void*)_check_null(_tmp1C1->topt));void*_tmp1B9=_tmp1B8;void*_tmp1BD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B9)->tag == 5U){_LL8: _tmp1BD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B9)->f1).elt_typ;_LL9:
# 1066
 return Cyc_Toc_is_poly_field(_tmp1BD,_tmp1C0) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BC;_tmp1BC.tag=0U;({struct _dyneither_ptr _tmp7FD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C1->topt)));_tmp1BC.f1=_tmp7FD;});({void*_tmp1BA[1U];_tmp1BA[0]=& _tmp1BC;({struct _dyneither_ptr _tmp7FE=({const char*_tmp1BB="is_poly_project: bad type %s";_tag_dyneither(_tmp1BB,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7FE,_tag_dyneither(_tmp1BA,sizeof(void*),1U));});});});}_LL7:;}default: _LL5: _LL6:
# 1069
 return 0;}_LL0:;}
# 1074
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp7FF=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp7FF,({struct Cyc_List_List*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));((*_tmp1C2).hd=s,(*_tmp1C2).tl=0);_tmp1C2;}),0U);});}
# 1078
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp800=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp800,({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));((*_tmp1C3).hd=s,(*_tmp1C3).tl=0);_tmp1C3;}),0U);});}
# 1082
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1088
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp801=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp801,({struct Cyc_Absyn_Exp*_tmp1C4[2U];(_tmp1C4[0]=rgn,_tmp1C4[1]=s);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C4,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1092
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp802=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp802,({struct Cyc_Absyn_Exp*_tmp1C5[2U];(_tmp1C5[0]=rgn,_tmp1C5[1]=s);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1096
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return({struct Cyc_Absyn_Exp*_tmp803=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp803,({struct Cyc_Absyn_Exp*_tmp1C6[2U];(_tmp1C6[0]=s,_tmp1C6[1]=n);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1100
return({struct Cyc_Absyn_Exp*_tmp804=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp804,({struct Cyc_Absyn_Exp*_tmp1C7[2U];(_tmp1C7[0]=s,_tmp1C7[1]=n);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1103
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp805=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp805,({struct Cyc_Absyn_Exp*_tmp1C8[3U];(((_tmp1C8[0]=rgn,_tmp1C8[1]=s)),_tmp1C8[2]=n);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1107
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp806=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp806,({struct Cyc_List_List*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));((*_tmp1C9).hd=e,(*_tmp1C9).tl=0);_tmp1C9;}),0U);});}
# 1110
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp807=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp807,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));((*_tmp1CA).hd=e,(*_tmp1CA).tl=0);_tmp1CA;}),0U);});}
# 1114
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp808=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp808,({void*_tmp1CB=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CB,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1119
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1127
int is_string=0;
{void*_tmp1CC=e->r;void*_tmp1CD=_tmp1CC;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CD)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CD)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4:
 is_string=1;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 1133
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp80A=({struct Cyc_List_List*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));(({struct Cyc_Absyn_Decl*_tmp809=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));((*_tmp1CE).tag=0U,(*_tmp1CE).f1=vd);_tmp1CE;}),0U);(*_tmp1CF).hd=_tmp809;}),(*_tmp1CF).tl=Cyc_Toc_result_decls);_tmp1CF;});Cyc_Toc_result_decls=_tmp80A;});
({struct Cyc_Absyn_Exp*_tmp80B=Cyc_Absyn_var_exp(x,0U);xexp=_tmp80B;});
({struct Cyc_Absyn_Exp*_tmp80C=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp80C;});}else{
# 1143
({struct Cyc_Absyn_Exp*_tmp80E=({void*_tmp80D=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp80D,e);});xexp=_tmp80E;});
# 1145
({struct Cyc_Absyn_Exp*_tmp810=({void*_tmp80F=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp80F,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp810;});}
# 1147
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1D0[3U];(((({struct _tuple20*_tmp813=({struct _tuple20*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));((*_tmp1D1).f1=0,(*_tmp1D1).f2=xexp);_tmp1D1;});_tmp1D0[0]=_tmp813;}),({
struct _tuple20*_tmp812=({struct _tuple20*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));((*_tmp1D2).f1=0,(*_tmp1D2).f2=xexp);_tmp1D2;});_tmp1D0[1]=_tmp812;}))),({
struct _tuple20*_tmp811=({struct _tuple20*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));((*_tmp1D3).f1=0,(*_tmp1D3).f2=xplussz);_tmp1D3;});_tmp1D0[2]=_tmp811;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D0,sizeof(struct _tuple20*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
# 1189 "toc.cyc"
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D4=nv;int _tmp1D5;_LL1: _tmp1D5=_tmp1D4->toplevel;_LL2:;
return _tmp1D5;}
# 1193
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D6=nv;int*_tmp1D7;_LL1: _tmp1D7=_tmp1D6->in_lhs;_LL2:;
return*_tmp1D7;}
# 1199
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));((((((((((((*_tmp1DA).break_lab=0,(*_tmp1DA).continue_lab=0)),(*_tmp1DA).fallthru_info=0)),(*_tmp1DA).toplevel=1)),({
# 1204
int*_tmp815=({int*_tmp1D8=_region_malloc(r,sizeof(*_tmp1D8));*_tmp1D8=0;_tmp1D8;});(*_tmp1DA).in_lhs=_tmp815;}))),({
int*_tmp814=({int*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));*_tmp1D9=0;_tmp1D9;});(*_tmp1DA).in_sizeof=_tmp814;}))),(*_tmp1DA).rgn=r);_tmp1DA;});}
# 1209
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DB=e;struct _dyneither_ptr**_tmp1E2;struct _dyneither_ptr**_tmp1E1;struct Cyc_Toc_FallthruInfo*_tmp1E0;int _tmp1DF;int*_tmp1DE;int*_tmp1DD;_LL1: _tmp1E2=_tmp1DB->break_lab;_tmp1E1=_tmp1DB->continue_lab;_tmp1E0=_tmp1DB->fallthru_info;_tmp1DF=_tmp1DB->toplevel;_tmp1DE=_tmp1DB->in_lhs;_tmp1DD=_tmp1DB->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DC=_region_malloc(r,sizeof(*_tmp1DC));((((((((((((*_tmp1DC).break_lab=_tmp1E2,(*_tmp1DC).continue_lab=_tmp1E1)),(*_tmp1DC).fallthru_info=_tmp1E0)),(*_tmp1DC).toplevel=_tmp1DF)),(*_tmp1DC).in_lhs=_tmp1DE)),(*_tmp1DC).in_sizeof=_tmp1DD)),(*_tmp1DC).rgn=r);_tmp1DC;});}
# 1214
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E3=e;struct _dyneither_ptr**_tmp1EA;struct _dyneither_ptr**_tmp1E9;struct Cyc_Toc_FallthruInfo*_tmp1E8;int _tmp1E7;int*_tmp1E6;int*_tmp1E5;_LL1: _tmp1EA=_tmp1E3->break_lab;_tmp1E9=_tmp1E3->continue_lab;_tmp1E8=_tmp1E3->fallthru_info;_tmp1E7=_tmp1E3->toplevel;_tmp1E6=_tmp1E3->in_lhs;_tmp1E5=_tmp1E3->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E4=_region_malloc(r,sizeof(*_tmp1E4));((((((((((((*_tmp1E4).break_lab=_tmp1EA,(*_tmp1E4).continue_lab=_tmp1E9)),(*_tmp1E4).fallthru_info=_tmp1E8)),(*_tmp1E4).toplevel=0)),(*_tmp1E4).in_lhs=_tmp1E6)),(*_tmp1E4).in_sizeof=_tmp1E5)),(*_tmp1E4).rgn=r);_tmp1E4;});}
# 1218
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EB=e;struct _dyneither_ptr**_tmp1F2;struct _dyneither_ptr**_tmp1F1;struct Cyc_Toc_FallthruInfo*_tmp1F0;int _tmp1EF;int*_tmp1EE;int*_tmp1ED;_LL1: _tmp1F2=_tmp1EB->break_lab;_tmp1F1=_tmp1EB->continue_lab;_tmp1F0=_tmp1EB->fallthru_info;_tmp1EF=_tmp1EB->toplevel;_tmp1EE=_tmp1EB->in_lhs;_tmp1ED=_tmp1EB->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1EC=_region_malloc(r,sizeof(*_tmp1EC));((((((((((((*_tmp1EC).break_lab=_tmp1F2,(*_tmp1EC).continue_lab=_tmp1F1)),(*_tmp1EC).fallthru_info=_tmp1F0)),(*_tmp1EC).toplevel=1)),(*_tmp1EC).in_lhs=_tmp1EE)),(*_tmp1EC).in_sizeof=_tmp1ED)),(*_tmp1EC).rgn=r);_tmp1EC;});}
# 1222
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F3=e;int*_tmp1F4;_LL1: _tmp1F4=_tmp1F3->in_lhs;_LL2:;
*_tmp1F4=b;}
# 1226
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F5=e;int*_tmp1F7;_LL1: _tmp1F7=_tmp1F5->in_sizeof;_LL2:;{
int _tmp1F6=*_tmp1F7;
*_tmp1F7=b;
return _tmp1F6;};}
# 1232
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F8=e;int*_tmp1F9;_LL1: _tmp1F9=_tmp1F8->in_sizeof;_LL2:;
return*_tmp1F9;}
# 1239
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FA=e;struct _dyneither_ptr**_tmp201;struct _dyneither_ptr**_tmp200;struct Cyc_Toc_FallthruInfo*_tmp1FF;int _tmp1FE;int*_tmp1FD;int*_tmp1FC;_LL1: _tmp201=_tmp1FA->break_lab;_tmp200=_tmp1FA->continue_lab;_tmp1FF=_tmp1FA->fallthru_info;_tmp1FE=_tmp1FA->toplevel;_tmp1FD=_tmp1FA->in_lhs;_tmp1FC=_tmp1FA->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1FB=_region_malloc(r,sizeof(*_tmp1FB));((((((((((((*_tmp1FB).break_lab=0,(*_tmp1FB).continue_lab=0)),(*_tmp1FB).fallthru_info=_tmp1FF)),(*_tmp1FB).toplevel=_tmp1FE)),(*_tmp1FB).in_lhs=_tmp1FD)),(*_tmp1FB).in_sizeof=_tmp1FC)),(*_tmp1FB).rgn=r);_tmp1FB;});}
# 1245
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1249
struct Cyc_Toc_Env*_tmp202=e;struct _dyneither_ptr**_tmp20B;struct _dyneither_ptr**_tmp20A;struct Cyc_Toc_FallthruInfo*_tmp209;int _tmp208;int*_tmp207;int*_tmp206;_LL1: _tmp20B=_tmp202->break_lab;_tmp20A=_tmp202->continue_lab;_tmp209=_tmp202->fallthru_info;_tmp208=_tmp202->toplevel;_tmp207=_tmp202->in_lhs;_tmp206=_tmp202->in_sizeof;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp205=_region_malloc(r,sizeof(*_tmp205));
((*_tmp205).label=fallthru_l,(*_tmp205).binders=fallthru_binders);_tmp205;});
return({struct Cyc_Toc_Env*_tmp204=_region_malloc(r,sizeof(*_tmp204));(((((((((((({struct _dyneither_ptr**_tmp816=({struct _dyneither_ptr**_tmp203=_region_malloc(r,sizeof(*_tmp203));*_tmp203=break_l;_tmp203;});(*_tmp204).break_lab=_tmp816;}),(*_tmp204).continue_lab=_tmp20A)),(*_tmp204).fallthru_info=fi)),(*_tmp204).toplevel=_tmp208)),(*_tmp204).in_lhs=_tmp207)),(*_tmp204).in_sizeof=_tmp206)),(*_tmp204).rgn=r);_tmp204;});};}
# 1255
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1257
struct Cyc_Toc_Env*_tmp20C=e;struct _dyneither_ptr**_tmp214;struct _dyneither_ptr**_tmp213;struct Cyc_Toc_FallthruInfo*_tmp212;int _tmp211;int*_tmp210;int*_tmp20F;_LL1: _tmp214=_tmp20C->break_lab;_tmp213=_tmp20C->continue_lab;_tmp212=_tmp20C->fallthru_info;_tmp211=_tmp20C->toplevel;_tmp210=_tmp20C->in_lhs;_tmp20F=_tmp20C->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp20E=_region_malloc(r,sizeof(*_tmp20E));(((((((((((({struct _dyneither_ptr**_tmp817=({struct _dyneither_ptr**_tmp20D=_region_malloc(r,sizeof(*_tmp20D));*_tmp20D=break_l;_tmp20D;});(*_tmp20E).break_lab=_tmp817;}),(*_tmp20E).continue_lab=_tmp213)),(*_tmp20E).fallthru_info=0)),(*_tmp20E).toplevel=_tmp211)),(*_tmp20E).in_lhs=_tmp210)),(*_tmp20E).in_sizeof=_tmp20F)),(*_tmp20E).rgn=r);_tmp20E;});}
# 1264
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1267
struct Cyc_Toc_Env*_tmp215=e;struct _dyneither_ptr**_tmp21D;struct _dyneither_ptr**_tmp21C;struct Cyc_Toc_FallthruInfo*_tmp21B;int _tmp21A;int*_tmp219;int*_tmp218;_LL1: _tmp21D=_tmp215->break_lab;_tmp21C=_tmp215->continue_lab;_tmp21B=_tmp215->fallthru_info;_tmp21A=_tmp215->toplevel;_tmp219=_tmp215->in_lhs;_tmp218=_tmp215->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp217=_region_malloc(r,sizeof(*_tmp217));((((((((((((*_tmp217).break_lab=0,(*_tmp217).continue_lab=_tmp21C)),({struct Cyc_Toc_FallthruInfo*_tmp818=({struct Cyc_Toc_FallthruInfo*_tmp216=_region_malloc(r,sizeof(*_tmp216));((*_tmp216).label=next_l,(*_tmp216).binders=0);_tmp216;});(*_tmp217).fallthru_info=_tmp818;}))),(*_tmp217).toplevel=_tmp21A)),(*_tmp217).in_lhs=_tmp219)),(*_tmp217).in_sizeof=_tmp218)),(*_tmp217).rgn=r);_tmp217;});}
# 1280 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1283
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1285
void*_tmp21E=e->annot;void*_tmp21F=_tmp21E;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp21F)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _LL2: {
# 1288
void*_tmp220=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp221=_tmp220;union Cyc_Absyn_Constraint*_tmp224;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp221)->tag == 5U){_LLC: _tmp224=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp221)->f1).ptr_atts).nullable;_LLD:
# 1291
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp224);}else{_LLE: _LLF:
({void*_tmp222=0U;({struct _dyneither_ptr _tmp819=({const char*_tmp223="Toc::is_nullable";_tag_dyneither(_tmp223,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp819,_tag_dyneither(_tmp222,sizeof(void*),0U));});});}_LLB:;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp21F)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _LL4:
# 1296
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp21F)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1298
({void*_tmp225=0U;({unsigned int _tmp81B=e->loc;struct _dyneither_ptr _tmp81A=({const char*_tmp226="NULL pointer check will definitely fail";_tag_dyneither(_tmp226,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp81B,_tmp81A,_tag_dyneither(_tmp225,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp21F)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
# 1304
 return 1;}else{_LL9: _LLA:
({void*_tmp227=0U;({struct _dyneither_ptr _tmp81C=({const char*_tmp228="need_null_check";_tag_dyneither(_tmp228,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81C,_tag_dyneither(_tmp227,sizeof(void*),0U));});});}}}}_LL0:;}
# 1309
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp229=e->annot;void*_tmp22A=_tmp229;struct Cyc_List_List*_tmp230;struct Cyc_List_List*_tmp22F;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22A)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp22F=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22A)->f1;_LL2:
 return _tmp22F;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22A)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp230=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22A)->f1;_LL4:
 return _tmp230;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp22A)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1314
({void*_tmp22B=0U;({unsigned int _tmp81E=e->loc;struct _dyneither_ptr _tmp81D=({const char*_tmp22C="dereference of NULL pointer";_tag_dyneither(_tmp22C,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp81E,_tmp81D,_tag_dyneither(_tmp22B,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp22A)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
({void*_tmp22D=0U;({struct _dyneither_ptr _tmp81F=({const char*_tmp22E="get_relns";_tag_dyneither(_tmp22E,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81F,_tag_dyneither(_tmp22D,sizeof(void*),0U));});});}}}}_LL0:;}static char _tmp234[8U]="*bogus*";
# 1324
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1334 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1337
struct Cyc_Absyn_Vardecl*x;
{void*_tmp231=a->r;void*_tmp232=_tmp231;void*_tmp235;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp232)->tag == 1U){_LL1: _tmp235=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp232)->f1;_LL2: {
# 1340
struct Cyc_Absyn_Vardecl*_tmp233=Cyc_Tcutil_nonesc_vardecl(_tmp235);
if(_tmp233 == 0)goto _LL4;
x=_tmp233;
goto _LL0;}}else{_LL3: _LL4: {
# 1346
static struct _dyneither_ptr bogus_string={_tmp234,_tmp234,_tmp234 + 8U};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1349
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1355
x=& bogus_vardecl;
x->type=a_typ;}}_LL0:;}{
# 1358
void*_tmp236=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1363
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp236);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
({struct Cyc_List_List*_tmp820=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);relns=_tmp820;});}
# 1372
if(valid_rop_i){
struct Cyc_List_List*_tmp237=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1380
if(!Cyc_Relations_consistent_relations(_tmp237))return 1;}
# 1383
inner_loop: {
void*_tmp238=i->r;void*_tmp239=_tmp238;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Exp*_tmp23D;void*_tmp23C;struct Cyc_Absyn_Exp*_tmp23B;switch(*((int*)_tmp239)){case 14U: _LL6: _tmp23C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp23B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL7:
 i=_tmp23B;goto inner_loop;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp239)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp239)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp239)->f2)->tl != 0){_LL8: _tmp23E=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp239)->f2)->hd;_tmp23D=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp239)->f2)->tl)->hd;_LL9: {
# 1389
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp23D,& rop_z)){
# 1392
struct Cyc_List_List*_tmp23A=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1394
return !Cyc_Relations_consistent_relations(_tmp23A);}
# 1396
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1399
return 0;};}
# 1402
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp23F=0U;({struct _dyneither_ptr _tmp821=({const char*_tmp240="Missing type in primop ";_tag_dyneither(_tmp240,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp821,_tag_dyneither(_tmp23F,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1406
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp241=_cycalloc(sizeof(*_tmp241));((*_tmp241).f1=Cyc_Toc_mt_tq,({void*_tmp822=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));(*_tmp241).f2=_tmp822;}));_tmp241;});}
# 1411
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp242=e->r;void*_tmp243=_tmp242;struct Cyc_Absyn_Exp*_tmp24B;int _tmp24A;struct Cyc_Absyn_Exp*_tmp249;int _tmp248;struct Cyc_List_List*_tmp247;switch(*((int*)_tmp243)){case 26U: _LL1: _tmp247=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp243)->f1;_LL2:
# 1414
{struct Cyc_List_List*_tmp244=_tmp247;for(0;_tmp244 != 0;_tmp244=_tmp244->tl){
if((*((struct _tuple20*)_tmp244->hd)).f1 != 0)
({void*_tmp245=0U;({struct _dyneither_ptr _tmp823=({const char*_tmp246="array designators for abstract-field initialization";_tag_dyneither(_tmp246,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp823,_tag_dyneither(_tmp245,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp824=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp247),0U);_tmp249=_tmp824;});_tmp248=0;goto _LL4;case 27U: _LL3: _tmp249=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp243)->f2;_tmp248=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp243)->f4;_LL4:
 _tmp24B=_tmp249;_tmp24A=_tmp248;goto _LL6;case 28U: _LL5: _tmp24B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp243)->f1;_tmp24A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp243)->f3;_LL6:
# 1420
({struct Cyc_Absyn_Exp*_tmp825=Cyc_Absyn_copy_exp(_tmp24B);_tmp24B=_tmp825;});
return _tmp24A?({struct Cyc_Absyn_Exp*_tmp826=_tmp24B;Cyc_Absyn_add_exp(_tmp826,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp24B;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1429
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1437
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp24C=e->r;void*_tmp24D=_tmp24C;struct Cyc_List_List*_tmp24E;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp24D)->tag == 29U){_LL1: _tmp24E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp24D)->f3;_LL2:
 dles=_tmp24E;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1443
{void*_tmp24F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp250=_tmp24F;struct Cyc_Absyn_Aggrdecl*_tmp251;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp250)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp250)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp251=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp250)->f1).aggr_info).KnownAggr).val;_LL7:
# 1445
 if(_tmp251->impl == 0)
return 0;
if(_tmp251->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp251->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{_LL8: _LL9:
# 1453
 return 0;}_LL5:;}
# 1455
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp252=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp253=(struct _tuple20*)dles->hd;struct _tuple20*_tmp254=_tmp253;struct Cyc_List_List*_tmp25D;struct Cyc_Absyn_Exp*_tmp25C;_LLB: _tmp25D=_tmp254->f1;_tmp25C=_tmp254->f2;_LLC:;{
struct _dyneither_ptr*_tmp255=Cyc_Absyn_designatorlist_to_fieldname(_tmp25D);
if(!Cyc_strptrcmp(_tmp255,_tmp252->name)){
struct Cyc_Absyn_Exp*_tmp256=Cyc_Toc_get_varsizeexp(_tmp25C);
if(_tmp256 != 0)
return _tmp256;{
void*_tmp257=Cyc_Tcutil_compress(_tmp252->type);void*_tmp258=_tmp257;void*_tmp25B;struct Cyc_Absyn_Exp*_tmp25A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp258)->tag == 8U){_LLE: _tmp25B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp258)->f1).elt_type;_tmp25A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp258)->f1).num_elts;_LLF:
# 1470
 if(_tmp25A == 0  || !Cyc_Toc_is_zero(_tmp25A))
return 0;
# 1476
return({struct Cyc_Absyn_Exp*_tmp829=({struct Cyc_Absyn_Exp*_tmp828=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp828,({struct Cyc_Absyn_Exp*_tmp259[2U];(_tmp259[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp25C)),({
# 1478
struct Cyc_Absyn_Exp*_tmp827=Cyc_Absyn_sizeoftyp_exp(_tmp25B,0U);_tmp259[1]=_tmp827;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp259,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1476
Cyc_Absyn_add_exp(_tmp829,
# 1479
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1484
({void*_tmp25E=0U;({struct _dyneither_ptr _tmp82A=({const char*_tmp25F="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp25F,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp82A,_tag_dyneither(_tmp25E,sizeof(void*),0U));});});};}
# 1487
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp260=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp260 != 0;_tmp260=_tmp260->tl){
struct Cyc_Absyn_Aggrfield*_tmp261=(struct Cyc_Absyn_Aggrfield*)_tmp260->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp261->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1494
({void*_tmp262=0U;({struct _dyneither_ptr _tmp82C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp265;_tmp265.tag=0U;_tmp265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp263[1U];_tmp263[0]=& _tmp265;({struct _dyneither_ptr _tmp82B=({const char*_tmp264="get_member_offset %s failed";_tag_dyneither(_tmp264,sizeof(char),28U);});Cyc_aprintf(_tmp82B,_tag_dyneither(_tmp263,sizeof(void*),1U));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp82C,_tag_dyneither(_tmp262,sizeof(void*),0U));});});}
# 1497
static int Cyc_Toc_is_array_type(void*t){
void*_tmp266=Cyc_Tcutil_compress(t);void*_tmp267=_tmp266;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp267)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1505
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1508
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1511
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp268=_cycalloc(sizeof(*_tmp268));((((*_tmp268).tag=5U,(*_tmp268).f1=e1)),(*_tmp268).f2=incr);_tmp268;}),0U);}
# 1515
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1524
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1526
void*_tmp269=e1->r;void*_tmp26A=_tmp269;struct Cyc_Absyn_Exp*_tmp272;struct _dyneither_ptr*_tmp271;int _tmp270;int _tmp26F;void*_tmp26E;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_Absyn_Stmt*_tmp26C;switch(*((int*)_tmp26A)){case 37U: _LL1: _tmp26C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26A)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp26C,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp26E=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26A)->f1;_tmp26D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26A)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp26D,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp272=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26A)->f1;_tmp271=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26A)->f2;_tmp270=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26A)->f3;_tmp26F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26A)->f4;_LL6:
# 1531
 e1->r=_tmp272->r;
({struct Cyc_Absyn_Exp*_tmp82F=e1;struct Cyc_List_List*_tmp82E=({struct Cyc_List_List*_tmp26B=_cycalloc(sizeof(*_tmp26B));((*_tmp26B).hd=_tmp271,(*_tmp26B).tl=fs);_tmp26B;});struct Cyc_Absyn_Exp*(*_tmp82D)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp82F,_tmp82E,_tmp82D,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1539
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1541
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp830=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp830;});}
({void*_tmp831=(f(e1_copy,f_env))->r;e1->r=_tmp831;});
goto _LL0;}}_LL0:;}
# 1547
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1549
void*_tmp273=s->r;void*_tmp274=_tmp273;struct Cyc_Absyn_Stmt*_tmp27B;struct Cyc_Absyn_Decl*_tmp27A;struct Cyc_Absyn_Stmt*_tmp279;struct Cyc_Absyn_Exp*_tmp278;switch(*((int*)_tmp274)){case 1U: _LL1: _tmp278=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp274)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp278,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp27A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp274)->f1;_tmp279=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp274)->f2;_LL4:
# 1552
 Cyc_Toc_lvalue_assign_stmt(_tmp279,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp27B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp274)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp27B,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp277;_tmp277.tag=0U;({struct _dyneither_ptr _tmp832=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp277.f1=_tmp832;});({void*_tmp275[1U];_tmp275[0]=& _tmp277;({struct _dyneither_ptr _tmp833=({const char*_tmp276="lvalue_assign_stmt: %s";_tag_dyneither(_tmp276,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp833,_tag_dyneither(_tmp275,sizeof(void*),1U));});});});}_LL0:;}
# 1558
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1562
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp27C=e->r;void*_tmp27D=_tmp27C;struct Cyc_Absyn_Stmt*_tmp284;struct Cyc_Absyn_Exp*_tmp283;void**_tmp282;struct Cyc_Absyn_Exp**_tmp281;switch(*((int*)_tmp27D)){case 14U: _LL1: _tmp282=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27D)->f1;_tmp281=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27D)->f2;_LL2:
# 1565
({struct Cyc_Absyn_Exp*_tmp834=Cyc_Toc_push_address_exp(*_tmp281);*_tmp281=_tmp834;});
({void*_tmp835=Cyc_Absyn_cstar_typ(*_tmp282,Cyc_Toc_mt_tq);*_tmp282=_tmp835;});
return e;case 20U: _LL3: _tmp283=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp27D)->f1;_LL4:
# 1569
 return _tmp283;case 37U: _LL5: _tmp284=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp27D)->f1;_LL6:
# 1573
 Cyc_Toc_push_address_stmt(_tmp284);
return e;default: _LL7: _LL8:
# 1576
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp280;_tmp280.tag=0U;({struct _dyneither_ptr _tmp836=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp280.f1=_tmp836;});({void*_tmp27E[1U];_tmp27E[0]=& _tmp280;({struct _dyneither_ptr _tmp837=({const char*_tmp27F="can't take & of exp %s";_tag_dyneither(_tmp27F,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp837,_tag_dyneither(_tmp27E,sizeof(void*),1U));});});});}_LL0:;}
# 1581
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp285=s->r;void*_tmp286=_tmp285;struct Cyc_Absyn_Exp**_tmp28C;struct Cyc_Absyn_Stmt*_tmp28B;struct Cyc_Absyn_Stmt*_tmp28A;switch(*((int*)_tmp286)){case 2U: _LL1: _tmp28A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp286)->f2;_LL2:
 _tmp28B=_tmp28A;goto _LL4;case 12U: _LL3: _tmp28B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp286)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp28B);goto _LL0;case 1U: _LL5: _tmp28C=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp286)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp838=Cyc_Toc_push_address_exp(*_tmp28C);*_tmp28C=_tmp838;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp289;_tmp289.tag=0U;({struct _dyneither_ptr _tmp839=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp289.f1=_tmp839;});({void*_tmp287[1U];_tmp287[0]=& _tmp289;({struct _dyneither_ptr _tmp83A=({const char*_tmp288="can't take & of stmt %s";_tag_dyneither(_tmp288,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp83A,_tag_dyneither(_tmp287,sizeof(void*),1U));});});});}_LL0:;}
# 1590
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp28D=Cyc_Tcutil_compress(t);void*_tmp28E=_tmp28D;struct Cyc_Absyn_PtrInfo _tmp291;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28E)->tag == 5U){_LL1: _tmp291=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28E)->f1;_LL2:
 return _tmp291;}else{_LL3: _LL4:
({void*_tmp28F=0U;({struct _dyneither_ptr _tmp83B=({const char*_tmp290="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp290,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp83B,_tag_dyneither(_tmp28F,sizeof(void*),0U));});});}_LL0:;}
# 1600
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp292=Cyc_Tcutil_compress(t);void*_tmp293=_tmp292;int _tmp29F;enum Cyc_Absyn_Sign _tmp29E;enum Cyc_Absyn_Sign _tmp29D;enum Cyc_Absyn_Sign _tmp29C;enum Cyc_Absyn_Sign _tmp29B;enum Cyc_Absyn_Sign _tmp29A;switch(*((int*)_tmp293)){case 5U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp83C=Cyc_Absyn_null_exp(0U);res=_tmp83C;});goto _LL0;case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp293)->f2){case Cyc_Absyn_Char_sz: _LL3: _tmp29A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp293)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmp83D=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp29A,'\000'),0U);res=_tmp83D;});goto _LL0;case Cyc_Absyn_Short_sz: _LL5: _tmp29B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp293)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp83E=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp29B,0),0U);res=_tmp83E;});goto _LL0;case Cyc_Absyn_Int_sz: _LLB: _tmp29C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp293)->f1;_LLC:
# 1608
 _tmp29D=_tmp29C;goto _LLE;case Cyc_Absyn_Long_sz: _LLD: _tmp29D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp293)->f1;_LLE:
# 1610
({struct Cyc_Absyn_Exp*_tmp83F=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp29D,0),0U);res=_tmp83F;});goto _LL0;default: _LLF: _tmp29E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp293)->f1;_LL10:
# 1612
({struct Cyc_Absyn_Exp*_tmp840=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp29E,(long long)0),0U);res=_tmp840;});goto _LL0;}case 13U: _LL7: _LL8:
# 1606
 goto _LLA;case 14U: _LL9: _LLA:
 _tmp29C=Cyc_Absyn_Unsigned;goto _LLC;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp293)->f1){case 0U: _LL11: _LL12:
# 1613
({struct Cyc_Absyn_Exp*_tmp841=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp294="0.0F";_tag_dyneither(_tmp294,sizeof(char),5U);}),0),0U);res=_tmp841;});goto _LL0;case 1U: _LL13: _LL14:
({struct Cyc_Absyn_Exp*_tmp842=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp295="0.0";_tag_dyneither(_tmp295,sizeof(char),4U);}),1),0U);res=_tmp842;});goto _LL0;default: _LL15: _tmp29F=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp293)->f1;_LL16:
({struct Cyc_Absyn_Exp*_tmp844=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp843=({const char*_tmp296="0.0L";_tag_dyneither(_tmp296,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp843,_tmp29F);}),0U);res=_tmp844;});goto _LL0;}default: _LL17: _LL18:
# 1617
({struct Cyc_String_pa_PrintArg_struct _tmp299;_tmp299.tag=0U;({struct _dyneither_ptr _tmp845=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp299.f1=_tmp845;});({void*_tmp297[1U];_tmp297[0]=& _tmp299;({struct _dyneither_ptr _tmp846=({const char*_tmp298="found non-zero type %s in generate_zero";_tag_dyneither(_tmp298,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp846,_tag_dyneither(_tmp297,sizeof(void*),1U));});});});}_LL0:;}
# 1619
res->topt=t;
return res;}
# 1626
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1639 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp2A0=Cyc_Toc_typ_to_c(elt_type);
void*_tmp2A1=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp2A2=Cyc_Absyn_cstar_typ(_tmp2A0,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp2A3=({struct Cyc_Core_Opt*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));(*_tmp2D4).v=_tmp2A2;_tmp2D4;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp2A4=e1->r;void*_tmp2A5=_tmp2A4;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;switch(*((int*)_tmp2A5)){case 20U: _LL1: _tmp2A9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2A5)->f1;_LL2:
# 1647
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp847=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2A9,0,Cyc_Absyn_Other_coercion,0U);_tmp2A9=_tmp847;});
_tmp2A9->topt=fat_ptr_type;}
# 1651
Cyc_Toc_exp_to_c(nv,_tmp2A9);xinit=_tmp2A9;goto _LL0;case 23U: _LL3: _tmp2AB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A5)->f1;_tmp2AA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A5)->f2;_LL4:
# 1653
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp848=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AB,0,Cyc_Absyn_Other_coercion,0U);_tmp2AB=_tmp848;});
_tmp2AB->topt=fat_ptr_type;}
# 1657
Cyc_Toc_exp_to_c(nv,_tmp2AB);Cyc_Toc_exp_to_c(nv,_tmp2AA);
({struct Cyc_Absyn_Exp*_tmp84B=({struct Cyc_Absyn_Exp*_tmp84A=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp84A,({struct Cyc_Absyn_Exp*_tmp2A6[3U];(((_tmp2A6[0]=_tmp2AB,({
struct Cyc_Absyn_Exp*_tmp849=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2A6[1]=_tmp849;}))),_tmp2A6[2]=_tmp2AA);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A6,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1658
xinit=_tmp84B;});
# 1660
goto _LL0;default: _LL5: _LL6:
({void*_tmp2A7=0U;({struct _dyneither_ptr _tmp84C=({const char*_tmp2A8="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp2A8,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp84C,_tag_dyneither(_tmp2A7,sizeof(void*),0U));});});}_LL0:;}{
# 1663
struct _tuple1*_tmp2AC=Cyc_Toc_temp_var();
struct _RegionHandle _tmp2AD=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp2AD;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp2AE=({struct Cyc_Absyn_Vardecl*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));((((((((((((((((*_tmp2D3).sc=Cyc_Absyn_Public,(*_tmp2D3).name=_tmp2AC)),(*_tmp2D3).varloc=0U)),(*_tmp2D3).tq=Cyc_Toc_mt_tq)),(*_tmp2D3).type=_tmp2A1)),(*_tmp2D3).initializer=xinit)),(*_tmp2D3).rgn=0)),(*_tmp2D3).attributes=0)),(*_tmp2D3).escapes=0);_tmp2D3;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2AF=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));((*_tmp2D2).tag=4U,(*_tmp2D2).f1=_tmp2AE);_tmp2D2;});
struct Cyc_Absyn_Exp*_tmp2B0=Cyc_Absyn_varb_exp((void*)_tmp2AF,0U);
_tmp2B0->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp2B1=Cyc_Absyn_deref_exp(_tmp2B0,0U);
_tmp2B1->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp2B1);{
struct _tuple1*_tmp2B2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B3=({struct Cyc_Absyn_Vardecl*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));((((((((((((((((*_tmp2D1).sc=Cyc_Absyn_Public,(*_tmp2D1).name=_tmp2B2)),(*_tmp2D1).varloc=0U)),(*_tmp2D1).tq=Cyc_Toc_mt_tq)),(*_tmp2D1).type=_tmp2A0)),(*_tmp2D1).initializer=_tmp2B1)),(*_tmp2D1).rgn=0)),(*_tmp2D1).attributes=0)),(*_tmp2D1).escapes=0);_tmp2D1;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B4=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));((*_tmp2D0).tag=4U,(*_tmp2D0).f1=_tmp2B3);_tmp2D0;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2B5=Cyc_Absyn_varb_exp((void*)_tmp2B4,0U);
_tmp2B5->topt=_tmp2B1->topt;
({struct Cyc_Absyn_Exp*_tmp84D=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2B5,e2,0U);z_init=_tmp84D;});
z_init->topt=_tmp2B5->topt;}
# 1682
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2B6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B7=({struct Cyc_Absyn_Vardecl*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));((((((((((((((((*_tmp2CF).sc=Cyc_Absyn_Public,(*_tmp2CF).name=_tmp2B6)),(*_tmp2CF).varloc=0U)),(*_tmp2CF).tq=Cyc_Toc_mt_tq)),(*_tmp2CF).type=_tmp2A0)),(*_tmp2CF).initializer=z_init)),(*_tmp2CF).rgn=0)),(*_tmp2CF).attributes=0)),(*_tmp2CF).escapes=0);_tmp2CF;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B8=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));((*_tmp2CE).tag=4U,(*_tmp2CE).f1=_tmp2B7);_tmp2CE;});
# 1688
struct Cyc_Absyn_Exp*_tmp2B9=Cyc_Absyn_varb_exp((void*)_tmp2B4,0U);_tmp2B9->topt=_tmp2B1->topt;{
struct Cyc_Absyn_Exp*_tmp2BA=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BB=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2B9,_tmp2BA,0U);
_tmp2BB->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2BB);{
# 1694
struct Cyc_Absyn_Exp*_tmp2BC=Cyc_Absyn_varb_exp((void*)_tmp2B8,0U);_tmp2BC->topt=_tmp2B1->topt;{
struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BE=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2BC,_tmp2BD,0U);
_tmp2BE->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2BE);{
# 1700
struct Cyc_List_List*_tmp2BF=({struct Cyc_Absyn_Exp*_tmp2CD[2U];(({struct Cyc_Absyn_Exp*_tmp84F=Cyc_Absyn_varb_exp((void*)_tmp2AF,0U);_tmp2CD[0]=_tmp84F;}),({
struct Cyc_Absyn_Exp*_tmp84E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2CD[1]=_tmp84E;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2CD,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2C0=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp851=({struct Cyc_Absyn_Exp*_tmp850=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2BF,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp850,_tmp2C0,0U);});xsize=_tmp851;});{
# 1707
struct Cyc_Absyn_Exp*_tmp2C1=({struct Cyc_Absyn_Exp*_tmp852=xsize;Cyc_Absyn_and_exp(_tmp852,Cyc_Absyn_and_exp(_tmp2BB,_tmp2BE,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C2=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2C3=({struct Cyc_Absyn_Exp*_tmp853=Cyc_Absyn_varb_exp((void*)_tmp2AF,0U);Cyc_Toc_member_exp(_tmp853,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp854=Cyc_Toc_cast_it(_tmp2A2,_tmp2C3);_tmp2C3=_tmp854;});{
struct Cyc_Absyn_Exp*_tmp2C4=Cyc_Absyn_deref_exp(_tmp2C3,0U);
struct Cyc_Absyn_Exp*_tmp2C5=({struct Cyc_Absyn_Exp*_tmp855=_tmp2C4;Cyc_Absyn_assign_exp(_tmp855,Cyc_Absyn_var_exp(_tmp2B6,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C6=Cyc_Absyn_exp_stmt(_tmp2C5,0U);
({struct Cyc_Absyn_Stmt*_tmp859=({struct Cyc_Absyn_Stmt*_tmp858=({struct Cyc_Absyn_Exp*_tmp857=_tmp2C1;struct Cyc_Absyn_Stmt*_tmp856=_tmp2C2;Cyc_Absyn_ifthenelse_stmt(_tmp857,_tmp856,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp858,_tmp2C6,0U);});_tmp2C6=_tmp859;});
({struct Cyc_Absyn_Stmt*_tmp85C=({struct Cyc_Absyn_Decl*_tmp85B=({struct Cyc_Absyn_Decl*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));(({void*_tmp85A=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));((*_tmp2C7).tag=0U,(*_tmp2C7).f1=_tmp2B7);_tmp2C7;});(*_tmp2C8).r=_tmp85A;}),(*_tmp2C8).loc=0U);_tmp2C8;});Cyc_Absyn_decl_stmt(_tmp85B,_tmp2C6,0U);});_tmp2C6=_tmp85C;});
({struct Cyc_Absyn_Stmt*_tmp85F=({struct Cyc_Absyn_Decl*_tmp85E=({struct Cyc_Absyn_Decl*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));(({void*_tmp85D=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));((*_tmp2C9).tag=0U,(*_tmp2C9).f1=_tmp2B3);_tmp2C9;});(*_tmp2CA).r=_tmp85D;}),(*_tmp2CA).loc=0U);_tmp2CA;});Cyc_Absyn_decl_stmt(_tmp85E,_tmp2C6,0U);});_tmp2C6=_tmp85F;});
({struct Cyc_Absyn_Stmt*_tmp862=({struct Cyc_Absyn_Decl*_tmp861=({struct Cyc_Absyn_Decl*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));(({void*_tmp860=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));((*_tmp2CB).tag=0U,(*_tmp2CB).f1=_tmp2AE);_tmp2CB;});(*_tmp2CC).r=_tmp860;}),(*_tmp2CC).loc=0U);_tmp2CC;});Cyc_Absyn_decl_stmt(_tmp861,_tmp2C6,0U);});_tmp2C6=_tmp862;});
({void*_tmp863=Cyc_Toc_stmt_exp_r(_tmp2C6);e->r=_tmp863;});};};};};};};};};};}
# 1665
;_pop_region(rgn2);};}
# 1733 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1737
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2D5=Cyc_Tcutil_compress(aggrtype);void*_tmp2D6=_tmp2D5;union Cyc_Absyn_AggrInfoU _tmp2DB;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D6)->tag == 11U){_LL1: _tmp2DB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D6)->f1).aggr_info;_LL2:
# 1740
({struct Cyc_Absyn_Aggrdecl*_tmp864=Cyc_Absyn_get_known_aggrdecl(_tmp2DB);ad=_tmp864;});goto _LL0;}else{_LL3: _LL4:
({void*_tmp2D7=0U;({struct _dyneither_ptr _tmp867=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DA;_tmp2DA.tag=0U;({struct _dyneither_ptr _tmp865=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(aggrtype));_tmp2DA.f1=_tmp865;});({void*_tmp2D8[1U];_tmp2D8[0]=& _tmp2DA;({struct _dyneither_ptr _tmp866=({const char*_tmp2D9="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2D9,sizeof(char),51U);});Cyc_aprintf(_tmp866,_tag_dyneither(_tmp2D8,sizeof(void*),1U));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp867,_tag_dyneither(_tmp2D7,sizeof(void*),0U));});});}_LL0:;}{
# 1744
struct _tuple1*_tmp2DC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2DD=Cyc_Absyn_var_exp(_tmp2DC,0U);
struct Cyc_Absyn_Exp*_tmp2DE=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2DF=Cyc_Absyn_aggrarrow_exp(_tmp2DD,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2E0=Cyc_Absyn_neq_exp(_tmp2DF,_tmp2DE,0U);
struct Cyc_Absyn_Exp*_tmp2E1=Cyc_Absyn_aggrarrow_exp(_tmp2DD,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2E2=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2E1,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E3=({struct Cyc_Absyn_Exp*_tmp869=_tmp2E0;struct Cyc_Absyn_Stmt*_tmp868=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp869,_tmp868,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2E4=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2E5=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E6=({struct _tuple1*_tmp86C=_tmp2DC;void*_tmp86B=_tmp2E4;struct Cyc_Absyn_Exp*_tmp86A=_tmp2E5;Cyc_Absyn_declare_stmt(_tmp86C,_tmp86B,_tmp86A,Cyc_Absyn_seq_stmt(_tmp2E3,_tmp2E2,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2E6);}else{
# 1758
struct Cyc_Absyn_Exp*_tmp2E7=({struct Cyc_Absyn_Exp*_tmp86D=aggrproj(_tmp2DD,f,0U);Cyc_Toc_member_exp(_tmp86D,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2E8=Cyc_Absyn_neq_exp(_tmp2E7,_tmp2DE,0U);
struct Cyc_Absyn_Exp*_tmp2E9=({struct Cyc_Absyn_Exp*_tmp86E=aggrproj(_tmp2DD,f,0U);Cyc_Toc_member_exp(_tmp86E,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2EA=Cyc_Absyn_exp_stmt(_tmp2E9,0U);
struct Cyc_Absyn_Stmt*_tmp2EB=({struct Cyc_Absyn_Exp*_tmp870=_tmp2E8;struct Cyc_Absyn_Stmt*_tmp86F=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp870,_tmp86F,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2EC=({struct _tuple1*_tmp873=_tmp2DC;void*_tmp872=e1_c_type;struct Cyc_Absyn_Exp*_tmp871=e1;Cyc_Absyn_declare_stmt(_tmp873,_tmp872,_tmp871,Cyc_Absyn_seq_stmt(_tmp2EB,_tmp2EA,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2EC);}};}
# 1768
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1771
void*_tmp2ED=Cyc_Tcutil_compress(t);void*_tmp2EE=_tmp2ED;union Cyc_Absyn_AggrInfoU _tmp2F5;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2EE)->tag == 11U){_LL1: _tmp2F5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2EE)->f1).aggr_info;_LL2: {
# 1773
struct Cyc_Absyn_Aggrdecl*_tmp2EF=Cyc_Absyn_get_known_aggrdecl(_tmp2F5);
({int _tmp874=Cyc_Toc_get_member_offset(_tmp2EF,f);*f_tag=_tmp874;});{
# 1776
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F4;_tmp2F4.tag=0U;_tmp2F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=0U;_tmp2F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2EF->name).f2);({void*_tmp2F1[2U];(_tmp2F1[0]=& _tmp2F3,_tmp2F1[1]=& _tmp2F4);({struct _dyneither_ptr _tmp875=({const char*_tmp2F2="_union_%s_%s";_tag_dyneither(_tmp2F2,sizeof(char),13U);});Cyc_aprintf(_tmp875,_tag_dyneither(_tmp2F1,sizeof(void*),2U));});});});});
({void*_tmp876=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));*_tmp2F0=str;_tmp2F0;}));*tagged_member_type=_tmp876;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2EF->impl))->tagged;};}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1787
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1790
void*_tmp2F6=e->r;void*_tmp2F7=_tmp2F6;struct Cyc_Absyn_Exp*_tmp303;struct _dyneither_ptr*_tmp302;int*_tmp301;struct Cyc_Absyn_Exp*_tmp300;struct _dyneither_ptr*_tmp2FF;int*_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FD;switch(*((int*)_tmp2F7)){case 14U: _LL1: _tmp2FD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL2:
({void*_tmp2F8=0U;({struct _dyneither_ptr _tmp877=({const char*_tmp2F9="cast on lhs!";_tag_dyneither(_tmp2F9,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp877,_tag_dyneither(_tmp2F8,sizeof(void*),0U));});});case 21U: _LL3: _tmp300=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp2FF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp2FE=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL4:
# 1793
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp300->topt),_tmp2FF,f_tag,tagged_member_type,clear_read,_tmp2FE);case 22U: _LL5: _tmp303=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp302=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp301=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL6: {
# 1796
void*_tmp2FA=Cyc_Tcutil_compress((void*)_check_null(_tmp303->topt));void*_tmp2FB=_tmp2FA;void*_tmp2FC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FB)->tag == 5U){_LLA: _tmp2FC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FB)->f1).elt_typ;_LLB:
# 1798
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2FC,_tmp302,f_tag,tagged_member_type,clear_read,_tmp301);}else{_LLC: _LLD:
# 1800
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1802
 return 0;}_LL0:;}
# 1815 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1819
struct _tuple1*_tmp304=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp304,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp878=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp878;});else{
# 1829
struct Cyc_Absyn_Exp*_tmp305=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp87B=({struct Cyc_Absyn_Exp*_tmp87A=_tmp305;struct Cyc_Absyn_Stmt*_tmp879=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp87A,_tmp879,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp87B;});}{
# 1832
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp87E=_tmp304;void*_tmp87D=Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp87C=
Cyc_Toc_push_address_exp(e1);
# 1832
Cyc_Absyn_declare_stmt(_tmp87E,_tmp87D,_tmp87C,
# 1834
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple32{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple33{void*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1838
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp306=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp309;_tmp309.tag=0U;({struct _dyneither_ptr _tmp87F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp309.f1=_tmp87F;});({void*_tmp307[1U];_tmp307[0]=& _tmp309;({struct _dyneither_ptr _tmp880=({const char*_tmp308="exp_to_c: no type for %s";_tag_dyneither(_tmp308,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp880,_tag_dyneither(_tmp307,sizeof(void*),1U));});});});{
void*old_typ=(void*)_check_null(e->topt);
void*_tmp30A=_tmp306;struct Cyc_Absyn_Stmt*_tmp532;struct Cyc_Absyn_Exp*_tmp531;struct _dyneither_ptr*_tmp530;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*_tmp52E;int _tmp52D;struct Cyc_Absyn_Exp*_tmp52C;void**_tmp52B;struct Cyc_Absyn_Exp*_tmp52A;int _tmp529;int _tmp528;struct Cyc_List_List*_tmp527;struct Cyc_Absyn_Datatypedecl*_tmp526;struct Cyc_Absyn_Datatypefield*_tmp525;struct _tuple1**_tmp524;struct Cyc_List_List*_tmp523;struct Cyc_List_List*_tmp522;struct Cyc_Absyn_Aggrdecl*_tmp521;void*_tmp520;struct Cyc_List_List*_tmp51F;struct Cyc_Absyn_Exp*_tmp51E;void*_tmp51D;int _tmp51C;struct Cyc_Absyn_Vardecl*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp519;int _tmp518;struct Cyc_List_List*_tmp517;struct Cyc_List_List*_tmp516;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp513;struct _dyneither_ptr*_tmp512;int _tmp511;int _tmp510;struct Cyc_Absyn_Exp*_tmp50F;struct _dyneither_ptr*_tmp50E;int _tmp50D;int _tmp50C;struct Cyc_Absyn_Exp*_tmp50B;void*_tmp50A;struct Cyc_List_List*_tmp509;void*_tmp508;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*_tmp504;void**_tmp503;struct Cyc_Absyn_Exp*_tmp502;int _tmp501;enum Cyc_Absyn_Coercion _tmp500;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_List_List*_tmp4FE;struct Cyc_Absyn_Exp*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FC;int _tmp4FB;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_List_List*_tmp4F9;int _tmp4F8;struct Cyc_List_List*_tmp4F7;struct Cyc_Absyn_VarargInfo*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_List_List*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Core_Opt*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;enum Cyc_Absyn_Incrementor _tmp4E6;enum Cyc_Absyn_Primop _tmp4E5;struct Cyc_List_List*_tmp4E4;switch(*((int*)_tmp30A)){case 2U: _LL1: _LL2:
# 1845
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL0;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp30A)->f1).Null_c).tag == 1){_LL3: _LL4: {
# 1852
struct Cyc_Absyn_Exp*_tmp30B=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp881=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp30B,_tmp30B))->r;e->r=_tmp881;});else{
# 1857
({void*_tmp883=({struct Cyc_Absyn_Exp*_tmp882=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp882,({struct Cyc_Absyn_Exp*_tmp30C[3U];(((_tmp30C[0]=_tmp30B,_tmp30C[1]=_tmp30B)),_tmp30C[2]=_tmp30B);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp30C,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp883;});}}else{
# 1859
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1861
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}case 1U: _LL7: _LL8:
 goto _LL0;case 3U: _LL9: _tmp4E5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4E4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LLA: {
# 1866
struct Cyc_List_List*_tmp30D=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4E4);
# 1868
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4E4);
{enum Cyc_Absyn_Primop _tmp30E=_tmp4E5;switch(_tmp30E){case Cyc_Absyn_Numelts: _LL58: _LL59: {
# 1871
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd;
{void*_tmp30F=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp310=_tmp30F;void*_tmp322;union Cyc_Absyn_Constraint*_tmp321;union Cyc_Absyn_Constraint*_tmp320;union Cyc_Absyn_Constraint*_tmp31F;struct Cyc_Absyn_Exp*_tmp31E;switch(*((int*)_tmp310)){case 8U: _LL6D: _tmp31E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp310)->f1).num_elts;_LL6E:
# 1875
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp31E)))
({void*_tmp311=0U;({unsigned int _tmp885=e->loc;struct _dyneither_ptr _tmp884=({const char*_tmp312="can't calculate numelts";_tag_dyneither(_tmp312,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp885,_tmp884,_tag_dyneither(_tmp311,sizeof(void*),0U));});});
e->r=_tmp31E->r;goto _LL6C;case 5U: _LL6F: _tmp322=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).elt_typ;_tmp321=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).ptr_atts).nullable;_tmp320=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).ptr_atts).bounds;_tmp31F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp310)->f1).ptr_atts).zero_term;_LL70:
# 1879
{void*_tmp313=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp320);void*_tmp314=_tmp313;struct Cyc_Absyn_Exp*_tmp319;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp314)->tag == 0U){_LL74: _LL75:
# 1881
({void*_tmp888=({struct Cyc_Absyn_Exp*_tmp887=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp887,({struct Cyc_Absyn_Exp*_tmp315[2U];(_tmp315[0]=(struct Cyc_Absyn_Exp*)_tmp4E4->hd,({
# 1883
struct Cyc_Absyn_Exp*_tmp886=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp322),0U);_tmp315[1]=_tmp886;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp315,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1881
e->r=_tmp888;});
# 1884
goto _LL73;}else{_LL76: _tmp319=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp314)->f1;_LL77:
# 1887
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp31F)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4E4->hd);
# 1891
({void*_tmp88A=({struct Cyc_Absyn_Exp*_tmp889=function_e;Cyc_Toc_fncall_exp_r(_tmp889,({struct Cyc_Absyn_Exp*_tmp316[2U];(_tmp316[0]=(struct Cyc_Absyn_Exp*)_tmp4E4->hd,_tmp316[1]=_tmp319);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp316,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp88A;});}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp321)){
if(!Cyc_Evexp_c_can_eval(_tmp319))
({void*_tmp317=0U;({unsigned int _tmp88C=e->loc;struct _dyneither_ptr _tmp88B=({const char*_tmp318="can't calculate numelts";_tag_dyneither(_tmp318,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp88C,_tmp88B,_tag_dyneither(_tmp317,sizeof(void*),0U));});});
# 1896
({void*_tmp88F=({struct Cyc_Absyn_Exp*_tmp88E=arg;struct Cyc_Absyn_Exp*_tmp88D=_tmp319;Cyc_Toc_conditional_exp_r(_tmp88E,_tmp88D,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp88F;});}else{
# 1898
e->r=_tmp319->r;goto _LL73;}}
# 1900
goto _LL73;}_LL73:;}
# 1902
goto _LL6C;default: _LL71: _LL72:
# 1904
({struct Cyc_String_pa_PrintArg_struct _tmp31D;_tmp31D.tag=0U;({struct _dyneither_ptr _tmp890=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp31D.f1=_tmp890;});({struct Cyc_String_pa_PrintArg_struct _tmp31C;_tmp31C.tag=0U;({struct _dyneither_ptr _tmp891=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp31C.f1=_tmp891;});({void*_tmp31A[2U];(_tmp31A[0]=& _tmp31C,_tmp31A[1]=& _tmp31D);({struct _dyneither_ptr _tmp892=({const char*_tmp31B="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp31B,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp892,_tag_dyneither(_tmp31A,sizeof(void*),2U));});});});});}_LL6C:;}
# 1907
goto _LL57;}case Cyc_Absyn_Plus: _LL5A: _LL5B:
# 1912
{void*_tmp323=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp30D))->hd);void*_tmp324=_tmp323;void*_tmp32C;union Cyc_Absyn_Constraint*_tmp32B;union Cyc_Absyn_Constraint*_tmp32A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->tag == 5U){_LL79: _tmp32C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->f1).elt_typ;_tmp32B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->f1).ptr_atts).bounds;_tmp32A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->f1).ptr_atts).zero_term;_LL7A:
# 1914
{void*_tmp325=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp32B);void*_tmp326=_tmp325;struct Cyc_Absyn_Exp*_tmp329;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp326)->tag == 0U){_LL7E: _LL7F: {
# 1916
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4->tl))->hd;
({void*_tmp895=({struct Cyc_Absyn_Exp*_tmp894=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp894,({struct Cyc_Absyn_Exp*_tmp327[3U];(((_tmp327[0]=e1,({
struct Cyc_Absyn_Exp*_tmp893=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp32C),0U);_tmp327[1]=_tmp893;}))),_tmp327[2]=e2);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp327,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1918
e->r=_tmp895;});
# 1920
goto _LL7D;}}else{_LL80: _tmp329=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp326)->f1;_LL81:
# 1922
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp32A)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4->tl))->hd;
({void*_tmp897=({struct Cyc_Absyn_Exp*_tmp896=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp896,({struct Cyc_Absyn_Exp*_tmp328[3U];(((_tmp328[0]=e1,_tmp328[1]=_tmp329)),_tmp328[2]=e2);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp328,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp897;});}
# 1927
goto _LL7D;}_LL7D:;}
# 1929
goto _LL78;}else{_LL7B: _LL7C:
 goto _LL78;}_LL78:;}
# 1932
goto _LL57;case Cyc_Absyn_Minus: _LL5C: _LL5D: {
# 1937
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp30D))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp30D->tl))->hd)){
({void*_tmp899=({struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp898,Cyc_Toc_curr_sp);});e1->r=_tmp899;});
({void*_tmp89B=({struct Cyc_Absyn_Exp*_tmp89A=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp89A,Cyc_Toc_curr_sp);});e2->r=_tmp89B;});
({void*_tmp89D=({void*_tmp89C=Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_typ,Cyc_Toc_mt_tq);e2->topt=_tmp89C;});e1->topt=_tmp89D;});
({void*_tmp89F=({struct Cyc_Absyn_Exp*_tmp89E=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp89E,
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U),0U);})->r;
# 1945
e->r=_tmp89F;});}else{
# 1948
({void*_tmp8A3=({struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8A2,({struct Cyc_Absyn_Exp*_tmp32D[3U];(((_tmp32D[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8A1=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp32D[1]=_tmp8A1;}))),({
struct Cyc_Absyn_Exp*_tmp8A0=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp32D[2]=_tmp8A0;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32D,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1948
e->r=_tmp8A3;});}}
# 1953
goto _LL57;}case Cyc_Absyn_Eq: _LL5E: _LL5F:
 goto _LL61;case Cyc_Absyn_Neq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Gt: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Gte: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Lt: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lte: _LL68: _LL69: {
# 1961
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp30D))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp30D->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ)){
void*_tmp32E=({void*_tmp8A4=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8A4,Cyc_Toc_mt_tq);});
({void*_tmp8A7=({void*_tmp8A6=_tmp32E;Cyc_Toc_cast_it_r(_tmp8A6,({struct Cyc_Absyn_Exp*_tmp8A5=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp8A5,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp8A7;});
e1->topt=_tmp32E;}
# 1971
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
void*_tmp32F=({void*_tmp8A8=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8A8,Cyc_Toc_mt_tq);});
({void*_tmp8AB=({void*_tmp8AA=_tmp32F;Cyc_Toc_cast_it_r(_tmp8AA,({struct Cyc_Absyn_Exp*_tmp8A9=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp8A9,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp8AB;});
e2->topt=_tmp32F;}
# 1976
goto _LL57;}default: _LL6A: _LL6B:
 goto _LL57;}_LL57:;}
# 1979
goto _LL0;}case 5U: _LLB: _tmp4E7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4E6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LLC: {
# 1981
void*e2_cyc_typ=(void*)_check_null(_tmp4E7->topt);
# 1990 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp349="increment";_tag_dyneither(_tmp349,sizeof(char),10U);});
if(_tmp4E6 == Cyc_Absyn_PreDec  || _tmp4E6 == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp8AC=({const char*_tmp330="decrement";_tag_dyneither(_tmp330,sizeof(char),10U);});incr_str=_tmp8AC;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E7,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp333;_tmp333.tag=0U;_tmp333.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp331[1U];_tmp331[0]=& _tmp333;({unsigned int _tmp8AE=e->loc;struct _dyneither_ptr _tmp8AD=({const char*_tmp332="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp332,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp8AE,_tmp8AD,_tag_dyneither(_tmp331,sizeof(void*),1U));});});});
({void*_tmp334=0U;({struct _dyneither_ptr _tmp8AF=({const char*_tmp335="in-place inc/dec on zero-term";_tag_dyneither(_tmp335,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8AF,_tag_dyneither(_tmp334,sizeof(void*),0U));});});}{
# 1999
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4E7,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp336=Cyc_Absyn_signed_int_exp(1,0U);
_tmp336->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp337=_tmp4E6;switch(_tmp337){case Cyc_Absyn_PreInc: _LL83: _LL84:
# 2006
({void*_tmp8B1=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp339=_cycalloc(sizeof(*_tmp339));((((((*_tmp339).tag=4U,(*_tmp339).f1=_tmp4E7)),({struct Cyc_Core_Opt*_tmp8B0=({struct Cyc_Core_Opt*_tmp338=_cycalloc(sizeof(*_tmp338));(*_tmp338).v=(void*)Cyc_Absyn_Plus;_tmp338;});(*_tmp339).f2=_tmp8B0;}))),(*_tmp339).f3=_tmp336);_tmp339;});e->r=_tmp8B1;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL85: _LL86:
# 2010
({void*_tmp8B3=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp33B=_cycalloc(sizeof(*_tmp33B));((((((*_tmp33B).tag=4U,(*_tmp33B).f1=_tmp4E7)),({struct Cyc_Core_Opt*_tmp8B2=({struct Cyc_Core_Opt*_tmp33A=_cycalloc(sizeof(*_tmp33A));(*_tmp33A).v=(void*)Cyc_Absyn_Minus;_tmp33A;});(*_tmp33B).f2=_tmp8B2;}))),(*_tmp33B).f3=_tmp336);_tmp33B;});e->r=_tmp8B3;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL87: _LL88:
# 2014
({struct Cyc_String_pa_PrintArg_struct _tmp33E;_tmp33E.tag=0U;_tmp33E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp33C[1U];_tmp33C[0]=& _tmp33E;({unsigned int _tmp8B5=e->loc;struct _dyneither_ptr _tmp8B4=({const char*_tmp33D="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp33D,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp8B5,_tmp8B4,_tag_dyneither(_tmp33C,sizeof(void*),1U));});});});
# 2016
({void*_tmp33F=0U;({struct _dyneither_ptr _tmp8B6=({const char*_tmp340="in-place inc/dec on @tagged union";_tag_dyneither(_tmp340,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8B6,_tag_dyneither(_tmp33F,sizeof(void*),0U));});});}_LL82:;};}{
# 2019
int need_null_ck=(Cyc_Tcutil_is_pointer_type(old_typ) && 
Cyc_Tcutil_is_nullable_pointer_type(old_typ)) && Cyc_Toc_need_null_check(_tmp4E7);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E7);
Cyc_Toc_set_lhs(nv,0);{
# 2026
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4E6 == Cyc_Absyn_PostInc  || _tmp4E6 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2032
if(_tmp4E6 == Cyc_Absyn_PreDec  || _tmp4E6 == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp8BB=({struct Cyc_Absyn_Exp*_tmp8BA=fn_e;Cyc_Toc_fncall_exp_r(_tmp8BA,({struct Cyc_Absyn_Exp*_tmp341[3U];(((({struct Cyc_Absyn_Exp*_tmp8B9=Cyc_Toc_push_address_exp(_tmp4E7);_tmp341[0]=_tmp8B9;}),({
struct Cyc_Absyn_Exp*_tmp8B8=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp341[1]=_tmp8B8;}))),({
struct Cyc_Absyn_Exp*_tmp8B7=Cyc_Absyn_signed_int_exp(change,0U);_tmp341[2]=_tmp8B7;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp341,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2034
e->r=_tmp8BB;});}else{
# 2037
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2043
if(_tmp4E6 != Cyc_Absyn_PostInc){
struct _tuple1*_tmp342=Cyc_Toc_temp_var();
void*_tmp343=({void*_tmp8BC=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_typ(_tmp8BC,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp344=Cyc_Toc_push_address_exp(_tmp4E7);
struct Cyc_Absyn_Exp*_tmp345=({struct Cyc_Absyn_Exp*_tmp8BD=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp342,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp8BD,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(need_null_ck)
({struct Cyc_Absyn_Exp*_tmp8C0=({struct Cyc_Absyn_Exp*_tmp8BF=({struct Cyc_Absyn_Exp*_tmp8BE=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp342,0U),0U);Cyc_Absyn_neq_exp(_tmp8BE,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 2050
Cyc_Absyn_and_exp(_tmp8BF,_tmp345,0U);});_tmp345=_tmp8C0;});{
# 2052
struct Cyc_Absyn_Stmt*_tmp346=({struct Cyc_Absyn_Exp*_tmp8C2=_tmp345;struct Cyc_Absyn_Stmt*_tmp8C1=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp342,0U),0U),0U),0U);
# 2052
Cyc_Absyn_ifthenelse_stmt(_tmp8C2,_tmp8C1,
# 2054
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp8C4=({struct Cyc_Absyn_Stmt*_tmp8C3=_tmp346;Cyc_Absyn_seq_stmt(_tmp8C3,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp342,0U),0U),0U),0U);});_tmp346=_tmp8C4;});
({void*_tmp8C5=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp342,_tmp343,_tmp344,_tmp346,0U),0U))->r;e->r=_tmp8C5;});};}else{
# 2059
struct Cyc_Toc_functionSet*_tmp347=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp347,_tmp4E7);
({void*_tmp8C9=({struct Cyc_Absyn_Exp*_tmp8C8=fn_e;Cyc_Toc_fncall_exp_r(_tmp8C8,({struct Cyc_Absyn_Exp*_tmp348[2U];(({struct Cyc_Absyn_Exp*_tmp8C7=Cyc_Toc_push_address_exp(_tmp4E7);_tmp348[0]=_tmp8C7;}),({
struct Cyc_Absyn_Exp*_tmp8C6=Cyc_Absyn_signed_int_exp(1,0U);_tmp348[1]=_tmp8C6;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp348,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2061
e->r=_tmp8C9;});}}else{
# 2064
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4E7)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4E7,0,Cyc_Toc_incr_lvalue,_tmp4E6);
e->r=_tmp4E7->r;}}}
# 2068
goto _LL0;};};};}case 4U: _LLD: _tmp4EA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4E9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp4E8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30A)->f3;_LLE: {
# 2087 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4EA->topt);
void*e2_old_typ=(void*)_check_null(_tmp4E8->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4EA,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4EA);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E8);
({void*_tmp8CA=Cyc_Toc_tagged_union_assignop(_tmp4EA,e1_old_typ,_tmp4E9,_tmp4E8,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp8CA;});
# 2098
return;}{
# 2100
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4EA,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4EA,_tmp4E9,_tmp4E8,ptr_type,is_dyneither,elt_type);
# 2106
return;}{
# 2110
int e1_poly=Cyc_Toc_is_poly_project(_tmp4EA);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4EA);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E8);{
# 2116
int done=0;
if(_tmp4E9 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp34A=(enum Cyc_Absyn_Primop)_tmp4E9->v;enum Cyc_Absyn_Primop _tmp34B=_tmp34A;switch(_tmp34B){case Cyc_Absyn_Plus: _LL8A: _LL8B:
 change=_tmp4E8;goto _LL89;case Cyc_Absyn_Minus: _LL8C: _LL8D:
# 2124
({struct Cyc_Absyn_Exp*_tmp8CB=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4E8,0U);change=_tmp8CB;});goto _LL89;default: _LL8E: _LL8F:
({void*_tmp34C=0U;({struct _dyneither_ptr _tmp8CC=({const char*_tmp34D="bad t ? pointer arithmetic";_tag_dyneither(_tmp34D,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8CC,_tag_dyneither(_tmp34C,sizeof(void*),0U));});});}_LL89:;}
# 2127
done=1;{
# 2129
struct Cyc_Absyn_Exp*_tmp34E=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp8D0=({struct Cyc_Absyn_Exp*_tmp8CF=_tmp34E;Cyc_Toc_fncall_exp_r(_tmp8CF,({struct Cyc_Absyn_Exp*_tmp34F[3U];(((({struct Cyc_Absyn_Exp*_tmp8CE=Cyc_Toc_push_address_exp(_tmp4EA);_tmp34F[0]=_tmp8CE;}),({
struct Cyc_Absyn_Exp*_tmp8CD=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp34F[1]=_tmp8CD;}))),_tmp34F[2]=change);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34F,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2130
e->r=_tmp8D0;});};}else{
# 2133
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2136
enum Cyc_Absyn_Primop _tmp350=(enum Cyc_Absyn_Primop)_tmp4E9->v;enum Cyc_Absyn_Primop _tmp351=_tmp350;if(_tmp351 == Cyc_Absyn_Plus){_LL91: _LL92:
# 2138
 done=1;
({void*_tmp8D2=({struct Cyc_Absyn_Exp*_tmp8D1=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4EA);Cyc_Toc_fncall_exp_r(_tmp8D1,({struct Cyc_Absyn_Exp*_tmp352[2U];(_tmp352[0]=_tmp4EA,_tmp352[1]=_tmp4E8);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp352,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8D2;});
goto _LL90;}else{_LL93: _LL94:
({void*_tmp353=0U;({struct _dyneither_ptr _tmp8D3=({const char*_tmp354="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp354,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8D3,_tag_dyneither(_tmp353,sizeof(void*),0U));});});}_LL90:;}}}
# 2145
if(!done){
# 2147
if(e1_poly)
({void*_tmp8D5=({void*_tmp8D4=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmp8D4,Cyc_Absyn_new_exp(_tmp4E8->r,0U));});_tmp4E8->r=_tmp8D5;});
# 2153
if(!Cyc_Absyn_is_lvalue(_tmp4EA)){
({struct Cyc_Absyn_Exp*_tmp8D6=_tmp4EA;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp8D6,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp356=1;struct _tuple31*_tmp355=_cycalloc(_check_times(_tmp356,sizeof(struct _tuple31)));((_tmp355[0]).f1=_tmp4E9,(_tmp355[0]).f2=_tmp4E8);_tmp355;}));});
e->r=_tmp4EA->r;}}
# 2158
goto _LL0;};};};}case 6U: _LLF: _tmp4ED=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4EC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp4EB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30A)->f3;_LL10:
# 2160
 Cyc_Toc_exp_to_c(nv,_tmp4ED);
Cyc_Toc_exp_to_c(nv,_tmp4EC);
Cyc_Toc_exp_to_c(nv,_tmp4EB);
goto _LL0;case 7U: _LL11: _tmp4EF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4EE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL12:
 _tmp4F1=_tmp4EF;_tmp4F0=_tmp4EE;goto _LL14;case 8U: _LL13: _tmp4F1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4F0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL14:
 _tmp4F3=_tmp4F1;_tmp4F2=_tmp4F0;goto _LL16;case 9U: _LL15: _tmp4F3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4F2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL16:
# 2167
 Cyc_Toc_exp_to_c(nv,_tmp4F3);
Cyc_Toc_exp_to_c(nv,_tmp4F2);
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30A)->f3 == 0){_LL17: _tmp4F5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4F4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL18:
# 2171
 Cyc_Toc_exp_to_c(nv,_tmp4F5);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4F4);
goto _LL0;}else{_LL19: _tmp4FA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4F9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp4F8=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30A)->f3)->num_varargs;_tmp4F7=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30A)->f3)->injectors;_tmp4F6=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30A)->f3)->vai;_LL1A: {
# 2183 "toc.cyc"
struct _RegionHandle _tmp357=_new_region("r");struct _RegionHandle*r=& _tmp357;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4F8,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4F6->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0U);
# 2191
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4F9);
int num_normargs=num_args - _tmp4F8;
# 2195
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4F9=_tmp4F9->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4F9))->hd);
({struct Cyc_List_List*_tmp8D7=({struct Cyc_List_List*_tmp358=_cycalloc(sizeof(*_tmp358));((*_tmp358).hd=(struct Cyc_Absyn_Exp*)_tmp4F9->hd,(*_tmp358).tl=new_args);_tmp358;});new_args=_tmp8D7;});}}
# 2200
({struct Cyc_List_List*_tmp8DB=({struct Cyc_List_List*_tmp35A=_cycalloc(sizeof(*_tmp35A));(({struct Cyc_Absyn_Exp*_tmp8DA=({struct Cyc_Absyn_Exp*_tmp8D9=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8D9,({struct Cyc_Absyn_Exp*_tmp359[3U];(((_tmp359[0]=argvexp,({
# 2202
struct Cyc_Absyn_Exp*_tmp8D8=Cyc_Absyn_sizeoftyp_exp(cva_type,0U);_tmp359[1]=_tmp8D8;}))),_tmp359[2]=num_varargs_exp);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp359,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2200
(*_tmp35A).hd=_tmp8DA;}),(*_tmp35A).tl=new_args);_tmp35A;});new_args=_tmp8DB;});
# 2205
({struct Cyc_List_List*_tmp8DC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp8DC;});
# 2207
Cyc_Toc_exp_to_c(nv,_tmp4FA);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4FA,new_args,0U),0U);
# 2211
if(_tmp4F6->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp35B=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4F6->type));void*_tmp35C=_tmp35B;struct Cyc_Absyn_Datatypedecl*_tmp35F;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35C)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35C)->f1).datatype_info).KnownDatatype).tag == 2){_LL96: _tmp35F=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35C)->f1).datatype_info).KnownDatatype).val;_LL97:
 tud=_tmp35F;goto _LL95;}else{goto _LL98;}}else{_LL98: _LL99:
({void*_tmp35D=0U;({struct _dyneither_ptr _tmp8DD=({const char*_tmp35E="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp35E,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8DD,_tag_dyneither(_tmp35D,sizeof(void*),0U));});});}_LL95:;}{
# 2217
struct _dyneither_ptr vs=({unsigned int _tmp36C=(unsigned int)_tmp4F8;struct _tuple1**_tmp36B=({struct _RegionHandle*_tmp8DE=r;_region_malloc(_tmp8DE,_check_times(_tmp36C,sizeof(struct _tuple1*)));});({{unsigned int _tmp781=(unsigned int)_tmp4F8;unsigned int i;for(i=0;i < _tmp781;++ i){({struct _tuple1*_tmp8DF=Cyc_Toc_temp_var();_tmp36B[i]=_tmp8DF;});}}0;});_tag_dyneither(_tmp36B,sizeof(struct _tuple1*),_tmp36C);});
# 2219
if(_tmp4F8 != 0){
# 2223
struct Cyc_List_List*_tmp360=0;
{int i=_tmp4F8 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp8E1=({struct Cyc_List_List*_tmp361=_cycalloc(sizeof(*_tmp361));(({struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);(*_tmp361).hd=_tmp8E0;}),(*_tmp361).tl=_tmp360);_tmp361;});_tmp360=_tmp8E1;});}}
({struct Cyc_Absyn_Stmt*_tmp8E5=({struct _tuple1*_tmp8E4=argv;void*_tmp8E3=arr_type;struct Cyc_Absyn_Exp*_tmp8E2=Cyc_Absyn_array_exp(_tmp360,0U);Cyc_Absyn_declare_stmt(_tmp8E4,_tmp8E3,_tmp8E2,s,0U);});s=_tmp8E5;});{
# 2228
int i=0;for(0;_tmp4F9 != 0;(((_tmp4F9=_tmp4F9->tl,_tmp4F7=_tmp4F7->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4F9->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp362=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4F7))->hd;struct Cyc_Absyn_Datatypefield*_tmp363=_tmp362;struct _tuple1*_tmp365;struct Cyc_List_List*_tmp364;_LL9B: _tmp365=_tmp363->name;_tmp364=_tmp363->typs;_LL9C:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp364))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp8E6=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp8E6;});
# 2243
({struct Cyc_Absyn_Stmt*_tmp8EA=({struct Cyc_Absyn_Stmt*_tmp8E9=({struct Cyc_Absyn_Exp*_tmp8E8=({struct Cyc_Absyn_Exp*_tmp8E7=varexp;Cyc_Toc_member_exp(_tmp8E7,Cyc_Absyn_fieldname(1),0U);});Cyc_Absyn_assign_stmt(_tmp8E8,arg,0U);});Cyc_Absyn_seq_stmt(_tmp8E9,s,0U);});s=_tmp8EA;});
# 2246
({struct Cyc_Absyn_Stmt*_tmp8ED=({struct Cyc_Absyn_Stmt*_tmp8EC=({struct Cyc_Absyn_Exp*_tmp8EB=Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0U);Cyc_Absyn_assign_stmt(_tmp8EB,
Cyc_Toc_datatype_tag(tud,_tmp365),0U);});
# 2246
Cyc_Absyn_seq_stmt(_tmp8EC,s,0U);});s=_tmp8ED;});
# 2249
({struct Cyc_Absyn_Stmt*_tmp8F0=({struct _tuple1*_tmp8EF=var;void*_tmp8EE=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp365,tud->name));Cyc_Absyn_declare_stmt(_tmp8EF,_tmp8EE,0,s,0U);});s=_tmp8F0;});};}};}else{
# 2256
struct Cyc_List_List*_tmp366=({struct _tuple32*_tmp367[3U];(((({struct _tuple32*_tmp8F9=({struct _tuple32*_tmp368=_cycalloc(sizeof(*_tmp368));(({struct _dyneither_ptr _tmp8F8=_tag_dyneither(0,0,0);(*_tmp368).f1=_tmp8F8;}),({struct Cyc_Absyn_Exp*_tmp8F7=Cyc_Absyn_uint_exp(0U,0U);(*_tmp368).f2=_tmp8F7;}));_tmp368;});_tmp367[0]=_tmp8F9;}),({
struct _tuple32*_tmp8F6=({struct _tuple32*_tmp369=_cycalloc(sizeof(*_tmp369));(({struct _dyneither_ptr _tmp8F5=_tag_dyneither(0,0,0);(*_tmp369).f1=_tmp8F5;}),({struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Absyn_uint_exp(0U,0U);(*_tmp369).f2=_tmp8F4;}));_tmp369;});_tmp367[1]=_tmp8F6;}))),({
struct _tuple32*_tmp8F3=({struct _tuple32*_tmp36A=_cycalloc(sizeof(*_tmp36A));(({struct _dyneither_ptr _tmp8F2=_tag_dyneither(0,0,0);(*_tmp36A).f1=_tmp8F2;}),({struct Cyc_Absyn_Exp*_tmp8F1=Cyc_Absyn_uint_exp(0U,0U);(*_tmp36A).f2=_tmp8F1;}));_tmp36A;});_tmp367[2]=_tmp8F3;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp367,sizeof(struct _tuple32*),3U));});
({struct Cyc_Absyn_Stmt*_tmp8FD=({struct _tuple1*_tmp8FC=argv;void*_tmp8FB=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp8FA=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8FC,_tmp8FB,_tmp8FA,s,0U);});s=_tmp8FD;});}};}else{
# 2264
if(_tmp4F8 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp4F9 != 0;_tmp4F9=_tmp4F9->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F9->hd);
({struct Cyc_List_List*_tmp8FE=({struct Cyc_List_List*_tmp36D=_cycalloc(sizeof(*_tmp36D));((*_tmp36D).hd=(struct Cyc_Absyn_Exp*)_tmp4F9->hd,(*_tmp36D).tl=array_args);_tmp36D;});array_args=_tmp8FE;});}{
# 2270
struct Cyc_Absyn_Exp*_tmp36E=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp8FF=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp36E,s,0U);s=_tmp8FF;});};}else{
# 2273
({struct Cyc_Absyn_Stmt*_tmp903=({struct _tuple1*_tmp902=argv;void*_tmp901=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp902,_tmp901,_tmp900,s,0U);});s=_tmp903;});}}
# 2276
({void*_tmp904=Cyc_Toc_stmt_exp_r(s);e->r=_tmp904;});};}
# 2278
_npop_handler(0U);goto _LL0;
# 2183
;_pop_region(r);}}case 11U: _LL1B: _tmp4FC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4FB=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL1C:
# 2281
 Cyc_Toc_exp_to_c(nv,_tmp4FC);{
struct Cyc_Absyn_Exp*_tmp36F=_tmp4FB?Cyc_Toc_newrethrow_exp(_tmp4FC): Cyc_Toc_newthrow_exp(_tmp4FC);
({void*_tmp906=({void*_tmp905=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp905,_tmp36F,0U);})->r;e->r=_tmp906;});
goto _LL0;};case 12U: _LL1D: _tmp4FD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_LL1E:
 Cyc_Toc_exp_to_c(nv,_tmp4FD);goto _LL0;case 13U: _LL1F: _tmp4FF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp4FE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL20:
# 2287
 Cyc_Toc_exp_to_c(nv,_tmp4FF);
# 2296 "toc.cyc"
for(0;_tmp4FE != 0;_tmp4FE=_tmp4FE->tl){
enum Cyc_Absyn_KindQual _tmp370=(Cyc_Tcutil_typ_kind((void*)_tmp4FE->hd))->kind;
if(_tmp370 != Cyc_Absyn_EffKind  && _tmp370 != Cyc_Absyn_RgnKind){
{void*_tmp371=Cyc_Tcutil_compress((void*)_tmp4FE->hd);void*_tmp372=_tmp371;switch(*((int*)_tmp372)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 3U: _LLA0: _LLA1:
 continue;default: _LLA2: _LLA3:
# 2303
({void*_tmp908=({void*_tmp907=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp907,_tmp4FF,0U);})->r;e->r=_tmp908;});
goto _LL9D;}_LL9D:;}
# 2306
break;}}
# 2309
goto _LL0;case 14U: _LL21: _tmp503=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp502=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp501=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30A)->f3;_tmp500=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30A)->f4;_LL22: {
# 2311
void*old_t2=(void*)_check_null(_tmp502->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp503;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp503=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp502);
# 2318
{struct _tuple33 _tmp373=({struct _tuple33 _tmp783;(({void*_tmp90A=Cyc_Tcutil_compress(old_t2);_tmp783.f1=_tmp90A;}),({void*_tmp909=Cyc_Tcutil_compress(new_typ);_tmp783.f2=_tmp909;}));_tmp783;});struct _tuple33 _tmp374=_tmp373;struct Cyc_Absyn_PtrInfo _tmp3AC;struct Cyc_Absyn_PtrInfo _tmp3AB;struct Cyc_Absyn_PtrInfo _tmp3AA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->tag == 5U)switch(*((int*)_tmp374.f2)){case 5U: _LLA5: _tmp3AB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3AA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1;_LLA6: {
# 2320
int _tmp375=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3AB.ptr_atts).nullable);
int _tmp376=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3AA.ptr_atts).nullable);
void*_tmp377=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3AB.ptr_atts).bounds);
void*_tmp378=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3AA.ptr_atts).bounds);
int _tmp379=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3AB.ptr_atts).zero_term);
int _tmp37A=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3AA.ptr_atts).zero_term);
{struct _tuple33 _tmp37B=({struct _tuple33 _tmp782;(_tmp782.f1=_tmp377,_tmp782.f2=_tmp378);_tmp782;});struct _tuple33 _tmp37C=_tmp37B;struct Cyc_Absyn_Exp*_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A6;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A4;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37C.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37C.f2)->tag == 1U){_LLAC: _tmp3A5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37C.f1)->f1;_tmp3A4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37C.f2)->f1;_LLAD:
# 2328
 if((!Cyc_Evexp_c_can_eval(_tmp3A5) || !Cyc_Evexp_c_can_eval(_tmp3A4)) && !
Cyc_Evexp_same_const_exp(_tmp3A5,_tmp3A4))
({void*_tmp37D=0U;({unsigned int _tmp90C=e->loc;struct _dyneither_ptr _tmp90B=({const char*_tmp37E="can't validate cast due to potential size differences";_tag_dyneither(_tmp37E,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp90C,_tmp90B,_tag_dyneither(_tmp37D,sizeof(void*),0U));});});
if(_tmp375  && !_tmp376){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp37F=0U;({struct _dyneither_ptr _tmp90D=({const char*_tmp380="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp380,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp90D,_tag_dyneither(_tmp37F,sizeof(void*),0U));});});
# 2337
if(_tmp500 != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp383;_tmp383.tag=0U;({struct _dyneither_ptr _tmp90E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp383.f1=_tmp90E;});({void*_tmp381[1U];_tmp381[0]=& _tmp383;({struct _dyneither_ptr _tmp90F=({const char*_tmp382="null-check conversion mis-classified: %s";_tag_dyneither(_tmp382,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp90F,_tag_dyneither(_tmp381,sizeof(void*),1U));});});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp502);
if(do_null_check){
if(!_tmp501)
({void*_tmp384=0U;({unsigned int _tmp911=e->loc;struct _dyneither_ptr _tmp910=({const char*_tmp385="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp385,sizeof(char),58U);});Cyc_Tcutil_warn(_tmp911,_tmp910,_tag_dyneither(_tmp384,sizeof(void*),0U));});});
# 2344
({void*_tmp914=({void*_tmp913=*_tmp503;Cyc_Toc_cast_it_r(_tmp913,({struct Cyc_Absyn_Exp*_tmp912=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp912,({struct Cyc_List_List*_tmp386=_cycalloc(sizeof(*_tmp386));
((*_tmp386).hd=_tmp502,(*_tmp386).tl=0);_tmp386;}),0U);}));});
# 2344
e->r=_tmp914;});}else{
# 2348
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp502->r;}};}else{
# 2353
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp502->r;}
# 2360
goto _LLAB;}else{_LLAE: _tmp3A6=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37C.f1)->f1;_LLAF:
# 2362
 if(!Cyc_Evexp_c_can_eval(_tmp3A6))
({void*_tmp387=0U;({unsigned int _tmp916=e->loc;struct _dyneither_ptr _tmp915=({const char*_tmp388="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp388,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp916,_tmp915,_tag_dyneither(_tmp387,sizeof(void*),0U));});});
# 2365
if(_tmp500 == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp38B;_tmp38B.tag=0U;({struct _dyneither_ptr _tmp917=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp38B.f1=_tmp917;});({void*_tmp389[1U];_tmp389[0]=& _tmp38B;({struct _dyneither_ptr _tmp918=({const char*_tmp38A="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp38A,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp918,_tag_dyneither(_tmp389,sizeof(void*),1U));});});});
if(Cyc_Toc_is_toplevel(nv)){
# 2369
if((_tmp379  && !(_tmp3AA.elt_tq).real_const) && !_tmp37A)
# 2372
({struct Cyc_Absyn_Exp*_tmp91A=({struct Cyc_Absyn_Exp*_tmp919=_tmp3A6;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp919,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3A6=_tmp91A;});
# 2374
({void*_tmp91B=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp3A6,_tmp502))->r;e->r=_tmp91B;});}else{
# 2376
struct Cyc_Absyn_Exp*_tmp38C=Cyc_Toc__tag_dyneither_e;
# 2378
if(_tmp379){
# 2383
struct _tuple1*_tmp38D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp38E=Cyc_Absyn_var_exp(_tmp38D,0U);
struct Cyc_Absyn_Exp*arg3;
# 2388
{void*_tmp38F=_tmp502->r;void*_tmp390=_tmp38F;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp390)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp390)->f1).Wstring_c).tag){case 8U: _LLB5: _LLB6:
# 2390
 arg3=_tmp3A6;goto _LLB4;case 9U: _LLB7: _LLB8:
# 2392
 arg3=_tmp3A6;goto _LLB4;default: goto _LLB9;}else{_LLB9: _LLBA:
# 2394
({struct Cyc_Absyn_Exp*_tmp91F=({struct Cyc_Absyn_Exp*_tmp91E=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp502);Cyc_Absyn_fncall_exp(_tmp91E,({struct Cyc_Absyn_Exp*_tmp391[2U];(({
# 2396
struct Cyc_Absyn_Exp*_tmp91D=({void*_tmp91C=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp91C,_tmp38E);});_tmp391[0]=_tmp91D;}),_tmp391[1]=_tmp3A6);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp391,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2394
arg3=_tmp91F;});
# 2397
goto _LLB4;}_LLB4:;}
# 2399
if(!_tmp37A  && !(_tmp3AA.elt_tq).real_const)
# 2402
({struct Cyc_Absyn_Exp*_tmp921=({struct Cyc_Absyn_Exp*_tmp920=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp920,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp921;});{
# 2404
struct Cyc_Absyn_Exp*_tmp392=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3AA.elt_typ),0U);
struct Cyc_Absyn_Exp*_tmp393=({struct Cyc_Absyn_Exp*_tmp922=_tmp38C;Cyc_Absyn_fncall_exp(_tmp922,({struct Cyc_Absyn_Exp*_tmp395[3U];(((_tmp395[0]=_tmp38E,_tmp395[1]=_tmp392)),_tmp395[2]=arg3);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp395,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp394=Cyc_Absyn_exp_stmt(_tmp393,0U);
({struct Cyc_Absyn_Stmt*_tmp926=({struct _tuple1*_tmp925=_tmp38D;void*_tmp924=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp923=_tmp502;Cyc_Absyn_declare_stmt(_tmp925,_tmp924,_tmp923,_tmp394,0U);});_tmp394=_tmp926;});
({void*_tmp927=Cyc_Toc_stmt_exp_r(_tmp394);e->r=_tmp927;});};}else{
# 2411
({void*_tmp92A=({struct Cyc_Absyn_Exp*_tmp929=_tmp38C;Cyc_Toc_fncall_exp_r(_tmp929,({struct Cyc_Absyn_Exp*_tmp396[3U];(((_tmp396[0]=_tmp502,({
# 2413
struct Cyc_Absyn_Exp*_tmp928=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3AA.elt_typ),0U);_tmp396[1]=_tmp928;}))),_tmp396[2]=_tmp3A6);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp396,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2411
e->r=_tmp92A;});}}
# 2417
goto _LLAB;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37C.f2)->tag == 1U){_LLB0: _tmp3A7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37C.f2)->f1;_LLB1:
# 2419
 if(!Cyc_Evexp_c_can_eval(_tmp3A7))
({void*_tmp397=0U;({unsigned int _tmp92C=e->loc;struct _dyneither_ptr _tmp92B=({const char*_tmp398="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp398,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp92C,_tmp92B,_tag_dyneither(_tmp397,sizeof(void*),0U));});});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp399=0U;({struct _dyneither_ptr _tmp92D=({const char*_tmp39A="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp39A,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp92D,_tag_dyneither(_tmp399,sizeof(void*),0U));});});{
# 2431 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp39B=_tmp3A7;
if(_tmp379  && !_tmp37A)
({struct Cyc_Absyn_Exp*_tmp92F=({struct Cyc_Absyn_Exp*_tmp92E=_tmp3A7;Cyc_Absyn_add_exp(_tmp92E,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp39B=_tmp92F;});{
# 2438
struct Cyc_Absyn_Exp*_tmp39C=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp39D=({struct Cyc_Absyn_Exp*_tmp931=_tmp39C;Cyc_Absyn_fncall_exp(_tmp931,({struct Cyc_Absyn_Exp*_tmp39F[3U];(((_tmp39F[0]=_tmp502,({
# 2441
struct Cyc_Absyn_Exp*_tmp930=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3AB.elt_typ),0U);_tmp39F[1]=_tmp930;}))),_tmp39F[2]=_tmp39B);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp39F,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2443
if(!_tmp376)
({void*_tmp934=({struct Cyc_Absyn_Exp*_tmp933=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp933,({struct Cyc_List_List*_tmp39E=_cycalloc(sizeof(*_tmp39E));
(({struct Cyc_Absyn_Exp*_tmp932=Cyc_Absyn_copy_exp(_tmp39D);(*_tmp39E).hd=_tmp932;}),(*_tmp39E).tl=0);_tmp39E;}));});
# 2444
_tmp39D->r=_tmp934;});
# 2446
({void*_tmp935=Cyc_Toc_cast_it_r(*_tmp503,_tmp39D);e->r=_tmp935;});
goto _LLAB;};};}else{_LLB2: _LLB3:
# 2451
 DynCast:
 if((_tmp379  && !_tmp37A) && !(_tmp3AA.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3A0=0U;({struct _dyneither_ptr _tmp936=({const char*_tmp3A1="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp3A1,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp936,_tag_dyneither(_tmp3A0,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp3A2=Cyc_Toc__dyneither_ptr_decrease_size_e;
({void*_tmp93A=({struct Cyc_Absyn_Exp*_tmp939=_tmp3A2;Cyc_Toc_fncall_exp_r(_tmp939,({struct Cyc_Absyn_Exp*_tmp3A3[3U];(((_tmp3A3[0]=_tmp502,({
struct Cyc_Absyn_Exp*_tmp938=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3AB.elt_typ),0U);_tmp3A3[1]=_tmp938;}))),({
struct Cyc_Absyn_Exp*_tmp937=Cyc_Absyn_uint_exp(1U,0U);_tmp3A3[2]=_tmp937;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A3,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2456
e->r=_tmp93A;});};}
# 2460
goto _LLAB;}}_LLAB:;}
# 2462
goto _LLA4;}case 6U: _LLA7: _tmp3AC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1;_LLA8:
# 2464
{void*_tmp3A8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3AC.ptr_atts).bounds);void*_tmp3A9=_tmp3A8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3A9)->tag == 0U){_LLBC: _LLBD:
# 2466
({void*_tmp93C=({struct Cyc_Absyn_Exp*_tmp93B=Cyc_Absyn_new_exp(_tmp502->r,_tmp502->loc);Cyc_Toc_aggrmember_exp_r(_tmp93B,Cyc_Toc_curr_sp);});_tmp502->r=_tmp93C;});
_tmp502->topt=new_typ_c;
goto _LLBB;}else{_LLBE: _LLBF:
 goto _LLBB;}_LLBB:;}
# 2471
goto _LLA4;default: goto _LLA9;}else{_LLA9: _LLAA:
# 2473
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp502->r;
goto _LLA4;}_LLA4:;}
# 2477
goto _LL0;}case 15U: _LL23: _tmp504=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_LL24:
# 2480
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp504);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp504)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp504,0,Cyc_Toc_address_lvalue,1);
# 2486
({void*_tmp93E=({void*_tmp93D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp93D,_tmp504);});e->r=_tmp93E;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp504->topt))))
# 2490
({void*_tmp940=({void*_tmp93F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp93F,_tmp504);});e->r=_tmp940;});}
# 2492
goto _LL0;case 16U: _LL25: _tmp506=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp505=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL26:
# 2495
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp3AF;_tmp3AF.tag=0U;({struct _dyneither_ptr _tmp941=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp505->loc));_tmp3AF.f1=_tmp941;});({void*_tmp3AD[1U];_tmp3AD[0]=& _tmp3AF;({struct _dyneither_ptr _tmp942=({const char*_tmp3AE="%s: new at top-level";_tag_dyneither(_tmp3AE,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp942,_tag_dyneither(_tmp3AD,sizeof(void*),1U));});});});{
void*new_e_type=(void*)_check_null(_tmp505->topt);
# 2502
Cyc_Toc_exp_to_c(nv,_tmp505);{
# 2504
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp505->topt));
struct _tuple1*_tmp3B0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3B1=Cyc_Absyn_var_exp(_tmp3B0,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3B2=Cyc_Tcutil_compress(elt_typ);void*_tmp3B3=_tmp3B2;void*_tmp3B4;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3)->tag == 8U){_LLC1: _tmp3B4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3)->f1).elt_type;_LLC2:
# 2510
({void*_tmp943=Cyc_Toc_typ_to_c(_tmp3B4);elt_typ=_tmp943;});
({struct Cyc_Absyn_Exp*_tmp944=Cyc_Absyn_copy_exp(_tmp3B1);lhs=_tmp944;});
goto _LLC0;}else{_LLC3: _LLC4:
# 2514
({struct Cyc_Absyn_Exp*_tmp945=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp3B1),0U);lhs=_tmp945;});
goto _LLC0;}_LLC0:;}{
# 2517
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp505);
struct _tuple1*_tmp3B5=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp3B6=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp3B5),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp948=({struct Cyc_Absyn_Exp*_tmp947=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp947,({struct Cyc_Absyn_Exp*_tmp3B7[2U];(_tmp3B7[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp3B6),({
struct Cyc_Absyn_Exp*_tmp946=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3B7[1]=_tmp946;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3B7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2522
mexp=_tmp948;});else{
# 2525
({struct Cyc_Absyn_Exp*_tmp949=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp3B1,0U),0U);mexp=_tmp949;});}{
# 2528
struct Cyc_Absyn_Exp*_tmp3B8=Cyc_Toc_get_varsizeexp(_tmp505);
if(_tmp3B8 != 0)
({struct Cyc_Absyn_Exp*_tmp94A=Cyc_Absyn_add_exp(mexp,_tmp3B8,0U);mexp=_tmp94A;});
# 2532
if(_tmp506 == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp94B=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp94B;});else{
# 2535
struct Cyc_Absyn_Exp*r=_tmp506;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp94C=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp94C;});}{
# 2539
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp951=({struct Cyc_Absyn_Exp*_tmp950=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp950,({struct Cyc_Absyn_Exp*_tmp3B9[3U];(((({
struct Cyc_Absyn_Exp*_tmp94F=Cyc_Absyn_copy_exp(_tmp3B1);_tmp3B9[0]=_tmp94F;}),({
struct Cyc_Absyn_Exp*_tmp94E=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3B9[1]=_tmp94E;}))),
(unsigned int)_tmp3B6?_tmp3B9[2]=_tmp3B6:({struct Cyc_Absyn_Exp*_tmp94D=Cyc_Absyn_uint_exp(1U,0U);_tmp3B9[2]=_tmp94D;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3B9,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2541
answer=_tmp951;});else{
# 2546
({struct Cyc_Absyn_Exp*_tmp952=Cyc_Absyn_copy_exp(_tmp3B1);answer=_tmp952;});}
({void*_tmp953=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));((*_tmp3BA).tag=Cyc_Toc_Dest,(*_tmp3BA).f1=_tmp3B1);_tmp3BA;});e->annot=_tmp953;});{
struct Cyc_Absyn_Stmt*_tmp3BB=({struct _tuple1*_tmp957=_tmp3B0;void*_tmp956=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp955=mexp;Cyc_Absyn_declare_stmt(_tmp957,_tmp956,_tmp955,({
struct Cyc_Absyn_Stmt*_tmp954=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp954,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2552
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp958=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp3B5),Cyc_Absyn_uint_typ,array_len,_tmp3BB,0U);_tmp3BB=_tmp958;});
({void*_tmp959=Cyc_Toc_stmt_exp_r(_tmp3BB);e->r=_tmp959;});
if(_tmp3B8 != 0)
({void*_tmp95B=({void*_tmp95A=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp95A,Cyc_Absyn_copy_exp(e));});e->r=_tmp95B;});
goto _LL0;};};};};};};case 18U: _LL27: _tmp507=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_LL28: {
# 2561
int _tmp3BC=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp507);
Cyc_Toc_set_in_sizeof(nv,_tmp3BC);
goto _LL0;}case 17U: _LL29: _tmp508=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_LL2A:
({void*_tmp95D=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));((*_tmp3BD).tag=17U,({void*_tmp95C=Cyc_Toc_typ_to_c(_tmp508);(*_tmp3BD).f1=_tmp95C;}));_tmp3BD;});e->r=_tmp95D;});goto _LL0;case 19U: _LL2B: _tmp50A=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp509=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL2C: {
# 2567
void*_tmp3BE=_tmp50A;
struct Cyc_List_List*_tmp3BF=_tmp509;
for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
void*_tmp3C0=(void*)_tmp3BF->hd;void*_tmp3C1=_tmp3C0;unsigned int _tmp3D3;struct _dyneither_ptr*_tmp3D2;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3C1)->tag == 0U){_LLC6: _tmp3D2=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3C1)->f1;_LLC7:
 goto _LLC5;}else{_LLC8: _tmp3D3=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3C1)->f1;_LLC9:
# 2573
{void*_tmp3C2=Cyc_Tcutil_compress(_tmp3BE);void*_tmp3C3=_tmp3C2;struct Cyc_Absyn_Datatypefield*_tmp3D1;struct Cyc_List_List*_tmp3D0;struct Cyc_List_List*_tmp3CF;union Cyc_Absyn_AggrInfoU _tmp3CE;switch(*((int*)_tmp3C3)){case 11U: _LLCB: _tmp3CE=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3C3)->f1).aggr_info;_LLCC: {
# 2575
struct Cyc_Absyn_Aggrdecl*_tmp3C4=Cyc_Absyn_get_known_aggrdecl(_tmp3CE);
if(_tmp3C4->impl == 0)
({void*_tmp3C5=0U;({struct _dyneither_ptr _tmp95E=({const char*_tmp3C6="struct fields must be known";_tag_dyneither(_tmp3C6,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp95E,_tag_dyneither(_tmp3C5,sizeof(void*),0U));});});
_tmp3CF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C4->impl))->fields;goto _LLCE;}case 12U: _LLCD: _tmp3CF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C3)->f2;_LLCE: {
# 2580
struct Cyc_Absyn_Aggrfield*_tmp3C7=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3CF,(int)_tmp3D3);
({void*_tmp95F=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));((*_tmp3C8).tag=0U,(*_tmp3C8).f1=_tmp3C7->name);_tmp3C8;}));_tmp3BF->hd=_tmp95F;});
_tmp3BE=_tmp3C7->type;
goto _LLCA;}case 10U: _LLCF: _tmp3D0=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3C3)->f1;_LLD0:
# 2585
({void*_tmp961=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));((*_tmp3C9).tag=0U,({struct _dyneither_ptr*_tmp960=Cyc_Absyn_fieldname((int)(_tmp3D3 + 1));(*_tmp3C9).f1=_tmp960;}));_tmp3C9;}));_tmp3BF->hd=_tmp961;});
({void*_tmp962=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3D0,(int)_tmp3D3)).f2;_tmp3BE=_tmp962;});
goto _LLCA;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3C3)->f1).field_info).KnownDatatypefield).tag == 2){_LLD1: _tmp3D1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3C3)->f1).field_info).KnownDatatypefield).val).f2;_LLD2:
# 2589
 if(_tmp3D3 == 0)
({void*_tmp963=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));((*_tmp3CA).tag=0U,(*_tmp3CA).f1=Cyc_Toc_tag_sp);_tmp3CA;}));_tmp3BF->hd=_tmp963;});else{
# 2592
({void*_tmp964=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3D1->typs,(int)(_tmp3D3 - 1))).f2;_tmp3BE=_tmp964;});
({void*_tmp966=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));((*_tmp3CB).tag=0U,({struct _dyneither_ptr*_tmp965=Cyc_Absyn_fieldname((int)_tmp3D3);(*_tmp3CB).f1=_tmp965;}));_tmp3CB;}));_tmp3BF->hd=_tmp966;});}
# 2595
goto _LLCA;}else{goto _LLD3;}default: _LLD3: _LLD4:
({void*_tmp3CC=0U;({struct _dyneither_ptr _tmp967=({const char*_tmp3CD="impossible type for offsetof tuple index";_tag_dyneither(_tmp3CD,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp967,_tag_dyneither(_tmp3CC,sizeof(void*),0U));});});
goto _LLCA;}_LLCA:;}
# 2599
goto _LLC5;}_LLC5:;}
# 2602
({void*_tmp969=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));((((*_tmp3D4).tag=19U,({void*_tmp968=Cyc_Toc_typ_to_c(_tmp50A);(*_tmp3D4).f1=_tmp968;}))),(*_tmp3D4).f2=_tmp509);_tmp3D4;});e->r=_tmp969;});
goto _LL0;}case 20U: _LL2D: _tmp50B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_LL2E: {
# 2605
int _tmp3D5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp3D6=Cyc_Tcutil_compress((void*)_check_null(_tmp50B->topt));
{void*_tmp3D7=_tmp3D6;void*_tmp3EC;struct Cyc_Absyn_Tqual _tmp3EB;void*_tmp3EA;union Cyc_Absyn_Constraint*_tmp3E9;union Cyc_Absyn_Constraint*_tmp3E8;union Cyc_Absyn_Constraint*_tmp3E7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->tag == 5U){_LLD6: _tmp3EC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).elt_typ;_tmp3EB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).elt_tq;_tmp3EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).ptr_atts).rgn;_tmp3E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).ptr_atts).nullable;_tmp3E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).ptr_atts).bounds;_tmp3E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).ptr_atts).zero_term;_LLD7:
# 2610
{void*_tmp3D8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3E8);void*_tmp3D9=_tmp3D8;struct Cyc_Absyn_Exp*_tmp3E4;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3D9)->tag == 1U){_LLDB: _tmp3E4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3D9)->f1;_LLDC: {
# 2612
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp50B);
Cyc_Toc_exp_to_c(nv,_tmp50B);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3DA=0U;({unsigned int _tmp96B=e->loc;struct _dyneither_ptr _tmp96A=({const char*_tmp3DB="inserted null check due to dereference";_tag_dyneither(_tmp3DB,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp96B,_tmp96A,_tag_dyneither(_tmp3DA,sizeof(void*),0U));});});
# 2618
({void*_tmp96F=({void*_tmp96E=Cyc_Toc_typ_to_c(_tmp3D6);Cyc_Toc_cast_it_r(_tmp96E,({
struct Cyc_Absyn_Exp*_tmp96D=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp96D,({struct Cyc_List_List*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));
(({struct Cyc_Absyn_Exp*_tmp96C=Cyc_Absyn_copy_exp(_tmp50B);(*_tmp3DC).hd=_tmp96C;}),(*_tmp3DC).tl=0);_tmp3DC;}),0U);}));});
# 2618
_tmp50B->r=_tmp96F;});}
# 2626
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3E7)){
struct _tuple13 _tmp3DD=Cyc_Evexp_eval_const_uint_exp(_tmp3E4);struct _tuple13 _tmp3DE=_tmp3DD;unsigned int _tmp3E2;int _tmp3E1;_LLE0: _tmp3E2=_tmp3DE.f1;_tmp3E1=_tmp3DE.f2;_LLE1:;
# 2632
if(!_tmp3E1  || _tmp3E2 <= 0)
({void*_tmp3DF=0U;({unsigned int _tmp971=e->loc;struct _dyneither_ptr _tmp970=({const char*_tmp3E0="cannot determine dereference is in bounds";_tag_dyneither(_tmp3E0,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp971,_tmp970,_tag_dyneither(_tmp3DF,sizeof(void*),0U));});});}
# 2635
goto _LLDA;}}else{_LLDD: _LLDE: {
# 2638
struct Cyc_Absyn_Exp*_tmp3E3=Cyc_Absyn_uint_exp(0U,0U);
_tmp3E3->topt=Cyc_Absyn_uint_typ;
({void*_tmp972=Cyc_Toc_subscript_exp_r(_tmp50B,_tmp3E3);e->r=_tmp972;});
Cyc_Toc_exp_to_c(nv,e);
goto _LLDA;}}_LLDA:;}
# 2644
goto _LLD5;}else{_LLD8: _LLD9:
({void*_tmp3E5=0U;({struct _dyneither_ptr _tmp973=({const char*_tmp3E6="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp3E6,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp973,_tag_dyneither(_tmp3E5,sizeof(void*),0U));});});}_LLD5:;}
# 2647
Cyc_Toc_set_lhs(nv,_tmp3D5);
goto _LL0;};}case 21U: _LL2F: _tmp50F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp50E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp50D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30A)->f3;_tmp50C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30A)->f4;_LL30: {
# 2650
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp50F->topt);
Cyc_Toc_exp_to_c(nv,_tmp50F);
if(_tmp50D  && _tmp50C)
({void*_tmp978=({struct Cyc_Absyn_Exp*_tmp977=_tmp50F;void*_tmp976=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp975=e1_cyc_type;struct _dyneither_ptr*_tmp974=_tmp50E;Cyc_Toc_check_tagged_union(_tmp977,_tmp976,_tmp975,_tmp974,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2654
e->r=_tmp978;});
# 2658
if(is_poly)
({void*_tmp97A=({void*_tmp979=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp979,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp97A;});
goto _LL0;}case 22U: _LL31: _tmp513=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp512=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp511=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30A)->f3;_tmp510=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30A)->f4;_LL32: {
# 2662
int _tmp3ED=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp513->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp513);
Cyc_Toc_exp_to_c(nv,_tmp513);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp3EE=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp3EF=_tmp3EE;void*_tmp405;struct Cyc_Absyn_Tqual _tmp404;void*_tmp403;union Cyc_Absyn_Constraint*_tmp402;union Cyc_Absyn_Constraint*_tmp401;union Cyc_Absyn_Constraint*_tmp400;_LLE3: _tmp405=_tmp3EF.elt_typ;_tmp404=_tmp3EF.elt_tq;_tmp403=(_tmp3EF.ptr_atts).rgn;_tmp402=(_tmp3EF.ptr_atts).nullable;_tmp401=(_tmp3EF.ptr_atts).bounds;_tmp400=(_tmp3EF.ptr_atts).zero_term;_LLE4:;
{void*_tmp3F0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp401);void*_tmp3F1=_tmp3F0;struct Cyc_Absyn_Exp*_tmp3FF;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F1)->tag == 1U){_LLE6: _tmp3FF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F1)->f1;_LLE7: {
# 2673
struct _tuple13 _tmp3F2=Cyc_Evexp_eval_const_uint_exp(_tmp3FF);struct _tuple13 _tmp3F3=_tmp3F2;unsigned int _tmp3FD;int _tmp3FC;_LLEB: _tmp3FD=_tmp3F3.f1;_tmp3FC=_tmp3F3.f2;_LLEC:;
if(_tmp3FC){
if(_tmp3FD < 1)
({void*_tmp3F4=0U;({struct _dyneither_ptr _tmp97B=({const char*_tmp3F5="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp3F5,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp97B,_tag_dyneither(_tmp3F4,sizeof(void*),0U));});});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3F6=0U;({unsigned int _tmp97D=e->loc;struct _dyneither_ptr _tmp97C=({const char*_tmp3F7="inserted null check due to dereference";_tag_dyneither(_tmp3F7,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp97D,_tmp97C,_tag_dyneither(_tmp3F6,sizeof(void*),0U));});});
# 2681
({void*_tmp981=({void*_tmp980=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp980,({
struct Cyc_Absyn_Exp*_tmp97F=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp97F,({struct Cyc_Absyn_Exp*_tmp3F8[1U];({struct Cyc_Absyn_Exp*_tmp97E=
Cyc_Absyn_new_exp(_tmp513->r,0U);_tmp3F8[0]=_tmp97E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3F8,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2681
_tmp513->r=_tmp981;});}}else{
# 2686
if(!Cyc_Evexp_c_can_eval(_tmp3FF))
({void*_tmp3F9=0U;({unsigned int _tmp983=e->loc;struct _dyneither_ptr _tmp982=({const char*_tmp3FA="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp3FA,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp983,_tmp982,_tag_dyneither(_tmp3F9,sizeof(void*),0U));});});
# 2690
({void*_tmp989=({void*_tmp988=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp988,({
struct Cyc_Absyn_Exp*_tmp987=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp987,({struct Cyc_Absyn_Exp*_tmp3FB[4U];(((((({
struct Cyc_Absyn_Exp*_tmp986=Cyc_Absyn_new_exp(_tmp513->r,0U);_tmp3FB[0]=_tmp986;}),_tmp3FB[1]=_tmp3FF)),({
struct Cyc_Absyn_Exp*_tmp985=Cyc_Absyn_sizeoftyp_exp(_tmp405,0U);_tmp3FB[2]=_tmp985;}))),({
struct Cyc_Absyn_Exp*_tmp984=Cyc_Absyn_uint_exp(0U,0U);_tmp3FB[3]=_tmp984;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3FB,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 2690
_tmp513->r=_tmp989;});}
# 2696
goto _LLE5;}}else{_LLE8: _LLE9: {
# 2699
void*ta1=Cyc_Toc_typ_to_c(_tmp405);
({void*_tmp98F=({void*_tmp98E=Cyc_Absyn_cstar_typ(ta1,_tmp404);Cyc_Toc_cast_it_r(_tmp98E,({
struct Cyc_Absyn_Exp*_tmp98D=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp98D,({struct Cyc_Absyn_Exp*_tmp3FE[3U];(((({
struct Cyc_Absyn_Exp*_tmp98C=Cyc_Absyn_new_exp(_tmp513->r,0U);_tmp3FE[0]=_tmp98C;}),({
struct Cyc_Absyn_Exp*_tmp98B=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp3FE[1]=_tmp98B;}))),({
struct Cyc_Absyn_Exp*_tmp98A=Cyc_Absyn_uint_exp(0U,0U);_tmp3FE[2]=_tmp98A;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3FE,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 2700
_tmp513->r=_tmp98F;});
# 2707
goto _LLE5;}}_LLE5:;}
# 2709
if(_tmp511  && _tmp510)
({void*_tmp994=({struct Cyc_Absyn_Exp*_tmp993=_tmp513;void*_tmp992=Cyc_Toc_typ_to_c(e1typ);void*_tmp991=_tmp405;struct _dyneither_ptr*_tmp990=_tmp512;Cyc_Toc_check_tagged_union(_tmp993,_tmp992,_tmp991,_tmp990,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp994;});
# 2712
if(is_poly  && _tmp510)
({void*_tmp996=({void*_tmp995=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp995,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp996;});
Cyc_Toc_set_lhs(nv,_tmp3ED);
goto _LL0;};};}case 23U: _LL33: _tmp515=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp514=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL34: {
# 2717
int _tmp406=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp407=Cyc_Tcutil_compress((void*)_check_null(_tmp515->topt));
# 2721
{void*_tmp408=_tmp407;void*_tmp432;struct Cyc_Absyn_Tqual _tmp431;void*_tmp430;union Cyc_Absyn_Constraint*_tmp42F;union Cyc_Absyn_Constraint*_tmp42E;union Cyc_Absyn_Constraint*_tmp42D;struct Cyc_List_List*_tmp42C;switch(*((int*)_tmp408)){case 10U: _LLEE: _tmp42C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp408)->f1;_LLEF:
# 2724
 Cyc_Toc_exp_to_c(nv,_tmp515);
Cyc_Toc_exp_to_c(nv,_tmp514);{
struct _tuple13 _tmp409=Cyc_Evexp_eval_const_uint_exp(_tmp514);struct _tuple13 _tmp40A=_tmp409;unsigned int _tmp40E;int _tmp40D;_LLF5: _tmp40E=_tmp40A.f1;_tmp40D=_tmp40A.f2;_LLF6:;
if(!_tmp40D)
({void*_tmp40B=0U;({struct _dyneither_ptr _tmp997=({const char*_tmp40C="unknown tuple subscript in translation to C";_tag_dyneither(_tmp40C,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp997,_tag_dyneither(_tmp40B,sizeof(void*),0U));});});
({void*_tmp999=({struct Cyc_Absyn_Exp*_tmp998=_tmp515;Cyc_Toc_aggrmember_exp_r(_tmp998,Cyc_Absyn_fieldname((int)(_tmp40E + 1)));});e->r=_tmp999;});
goto _LLED;};case 5U: _LLF0: _tmp432=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).elt_typ;_tmp431=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).elt_tq;_tmp430=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).ptr_atts).rgn;_tmp42F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).ptr_atts).nullable;_tmp42E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).ptr_atts).bounds;_tmp42D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).ptr_atts).zero_term;_LLF1: {
# 2732
struct Cyc_List_List*_tmp40F=Cyc_Toc_get_relns(_tmp515);
int _tmp410=Cyc_Toc_need_null_check(_tmp515);
int _tmp411=Cyc_Toc_in_sizeof(nv);
# 2740
int in_bnds=_tmp411;
{void*_tmp412=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp42E);void*_tmp413=_tmp412;_LLF8: _LLF9:
# 2743
({int _tmp99A=in_bnds  || Cyc_Toc_check_bounds(_tmp407,_tmp40F,_tmp515,_tmp514);in_bnds=_tmp99A;});
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp416;_tmp416.tag=0U;({struct _dyneither_ptr _tmp99B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp416.f1=_tmp99B;});({void*_tmp414[1U];_tmp414[0]=& _tmp416;({unsigned int _tmp99D=e->loc;struct _dyneither_ptr _tmp99C=({const char*_tmp415="bounds check necessary for %s";_tag_dyneither(_tmp415,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp99D,_tmp99C,_tag_dyneither(_tmp414,sizeof(void*),1U));});});});_LLF7:;}
# 2750
Cyc_Toc_exp_to_c(nv,_tmp515);
Cyc_Toc_exp_to_c(nv,_tmp514);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp417=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp42E);void*_tmp418=_tmp417;struct Cyc_Absyn_Exp*_tmp429;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp418)->tag == 1U){_LLFB: _tmp429=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp418)->f1;_LLFC: {
# 2755
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp42F) && _tmp410;
void*ta1=Cyc_Toc_typ_to_c(_tmp432);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp431);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp419=0U;({unsigned int _tmp99F=e->loc;struct _dyneither_ptr _tmp99E=({const char*_tmp41A="inserted null check due to dereference";_tag_dyneither(_tmp41A,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp99F,_tmp99E,_tag_dyneither(_tmp419,sizeof(void*),0U));});});
({void*_tmp9A3=({void*_tmp9A2=ta2;Cyc_Toc_cast_it_r(_tmp9A2,({struct Cyc_Absyn_Exp*_tmp9A1=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9A1,({struct Cyc_Absyn_Exp*_tmp41B[1U];({struct Cyc_Absyn_Exp*_tmp9A0=
Cyc_Absyn_copy_exp(_tmp515);_tmp41B[0]=_tmp9A0;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp41B,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2764
_tmp515->r=_tmp9A3;});}else{
# 2767
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp42D)){
# 2769
if(!Cyc_Evexp_c_can_eval(_tmp429))
({void*_tmp41C=0U;({unsigned int _tmp9A5=e->loc;struct _dyneither_ptr _tmp9A4=({const char*_tmp41D="cannot determine subscript is in bounds";_tag_dyneither(_tmp41D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9A5,_tmp9A4,_tag_dyneither(_tmp41C,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp515);
({void*_tmp9A8=Cyc_Toc_deref_exp_r(({void*_tmp9A7=ta2;Cyc_Toc_cast_it(_tmp9A7,({
struct Cyc_Absyn_Exp*_tmp9A6=function_e;Cyc_Absyn_fncall_exp(_tmp9A6,({struct Cyc_Absyn_Exp*_tmp41E[3U];(((_tmp41E[0]=_tmp515,_tmp41E[1]=_tmp429)),_tmp41E[2]=_tmp514);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp41E,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2772
e->r=_tmp9A8;});};}else{
# 2775
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp429))
({void*_tmp41F=0U;({unsigned int _tmp9AA=e->loc;struct _dyneither_ptr _tmp9A9=({const char*_tmp420="cannot determine subscript is in bounds";_tag_dyneither(_tmp420,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9AA,_tmp9A9,_tag_dyneither(_tmp41F,sizeof(void*),0U));});});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp421=0U;({unsigned int _tmp9AC=e->loc;struct _dyneither_ptr _tmp9AB=({const char*_tmp422="inserted null check due to dereference";_tag_dyneither(_tmp422,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9AC,_tmp9AB,_tag_dyneither(_tmp421,sizeof(void*),0U));});});
# 2781
({void*_tmp9B0=Cyc_Toc_deref_exp_r(({void*_tmp9AF=ta2;Cyc_Toc_cast_it(_tmp9AF,({
struct Cyc_Absyn_Exp*_tmp9AE=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9AE,({struct Cyc_Absyn_Exp*_tmp423[4U];(((((_tmp423[0]=_tmp515,_tmp423[1]=_tmp429)),({
# 2784
struct Cyc_Absyn_Exp*_tmp9AD=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp423[2]=_tmp9AD;}))),_tmp423[3]=_tmp514);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp423,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 2781
e->r=_tmp9B0;});}else{
# 2787
if(!Cyc_Evexp_c_can_eval(_tmp429))
({void*_tmp424=0U;({unsigned int _tmp9B2=e->loc;struct _dyneither_ptr _tmp9B1=({const char*_tmp425="cannot determine subscript is in bounds";_tag_dyneither(_tmp425,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9B2,_tmp9B1,_tag_dyneither(_tmp424,sizeof(void*),0U));});});
# 2790
({void*_tmp9B5=({struct Cyc_Absyn_Exp*_tmp9B4=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmp9B4,({struct Cyc_Absyn_Exp*_tmp426[2U];(_tmp426[0]=_tmp429,({
struct Cyc_Absyn_Exp*_tmp9B3=Cyc_Absyn_copy_exp(_tmp514);_tmp426[1]=_tmp9B3;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp426,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2790
_tmp514->r=_tmp9B5;});}}}}
# 2793
goto _LLFA;}}else{_LLFD: _LLFE: {
# 2795
void*ta1=Cyc_Toc_typ_to_c(_tmp432);
if(in_bnds){
# 2799
++ Cyc_Toc_bounds_checks_eliminated;
({void*_tmp9B8=({struct Cyc_Absyn_Exp*_tmp9B7=({void*_tmp9B6=Cyc_Absyn_cstar_typ(ta1,_tmp431);Cyc_Toc_cast_it(_tmp9B6,
Cyc_Toc_member_exp(_tmp515,Cyc_Toc_curr_sp,0U));});
# 2800
Cyc_Toc_subscript_exp_r(_tmp9B7,_tmp514);});e->r=_tmp9B8;});}else{
# 2804
struct Cyc_Absyn_Exp*_tmp427=Cyc_Toc__check_dyneither_subscript_e;
({void*_tmp9BC=Cyc_Toc_deref_exp_r(({void*_tmp9BB=Cyc_Absyn_cstar_typ(ta1,_tmp431);Cyc_Toc_cast_it(_tmp9BB,({
struct Cyc_Absyn_Exp*_tmp9BA=_tmp427;Cyc_Absyn_fncall_exp(_tmp9BA,({struct Cyc_Absyn_Exp*_tmp428[3U];(((_tmp428[0]=_tmp515,({
struct Cyc_Absyn_Exp*_tmp9B9=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp428[1]=_tmp9B9;}))),_tmp428[2]=_tmp514);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp428,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2805
e->r=_tmp9BC;});}
# 2810
goto _LLFA;}}_LLFA:;}
# 2812
goto _LLED;}default: _LLF2: _LLF3:
({void*_tmp42A=0U;({struct _dyneither_ptr _tmp9BD=({const char*_tmp42B="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp42B,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9BD,_tag_dyneither(_tmp42A,sizeof(void*),0U));});});}_LLED:;}
# 2815
Cyc_Toc_set_lhs(nv,_tmp406);
goto _LL0;};}case 24U: _LL35: _tmp516=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_LL36:
# 2818
 if(!Cyc_Toc_is_toplevel(nv)){
# 2820
void*_tmp433=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp516));
{void*_tmp434=_tmp433;union Cyc_Absyn_AggrInfoU _tmp43C;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp434)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp434)->f1).targs == 0){_LL100: _tmp43C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp434)->f1).aggr_info;_LL101: {
# 2823
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp43C);
{int i=1;for(0;_tmp516 != 0;(_tmp516=_tmp516->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp516->hd);
({struct Cyc_List_List*_tmp9C2=({struct Cyc_List_List*_tmp438=_cycalloc(sizeof(*_tmp438));(({struct _tuple20*_tmp9C1=({struct _tuple20*_tmp437=_cycalloc(sizeof(*_tmp437));(({struct Cyc_List_List*_tmp9C0=({struct Cyc_List_List*_tmp436=_cycalloc(sizeof(*_tmp436));(({void*_tmp9BF=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp435=_cycalloc(sizeof(*_tmp435));((*_tmp435).tag=1U,({struct _dyneither_ptr*_tmp9BE=Cyc_Absyn_fieldname(i);(*_tmp435).f1=_tmp9BE;}));_tmp435;});(*_tmp436).hd=_tmp9BF;}),(*_tmp436).tl=0);_tmp436;});(*_tmp437).f1=_tmp9C0;}),(*_tmp437).f2=(struct Cyc_Absyn_Exp*)_tmp516->hd);_tmp437;});(*_tmp438).hd=_tmp9C1;}),(*_tmp438).tl=dles);_tmp438;});dles=_tmp9C2;});}}
# 2831
({void*_tmp9C4=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp439=_cycalloc(sizeof(*_tmp439));((((((((*_tmp439).tag=29U,(*_tmp439).f1=sd->name)),(*_tmp439).f2=0)),({struct Cyc_List_List*_tmp9C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(*_tmp439).f3=_tmp9C3;}))),(*_tmp439).f4=sd);_tmp439;});e->r=_tmp9C4;});
e->topt=_tmp433;
goto _LLFF;}}else{goto _LL102;}}else{_LL102: _LL103:
({void*_tmp43A=0U;({struct _dyneither_ptr _tmp9C5=({const char*_tmp43B="tuple type not an aggregate";_tag_dyneither(_tmp43B,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9C5,_tag_dyneither(_tmp43A,sizeof(void*),0U));});});}_LLFF:;}
# 2836
goto _LL0;}else{
# 2840
struct Cyc_List_List*dles=0;
for(0;_tmp516 != 0;_tmp516=_tmp516->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp516->hd);
({struct Cyc_List_List*_tmp9C7=({struct Cyc_List_List*_tmp43E=_cycalloc(sizeof(*_tmp43E));(({struct _tuple20*_tmp9C6=({struct _tuple20*_tmp43D=_cycalloc(sizeof(*_tmp43D));((*_tmp43D).f1=0,(*_tmp43D).f2=(struct Cyc_Absyn_Exp*)_tmp516->hd);_tmp43D;});(*_tmp43E).hd=_tmp9C6;}),(*_tmp43E).tl=dles);_tmp43E;});dles=_tmp9C7;});}
# 2845
({void*_tmp9C8=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9C8;});}
# 2847
goto _LL0;case 26U: _LL37: _tmp517=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_LL38:
# 2853
{struct Cyc_List_List*_tmp43F=_tmp517;for(0;_tmp43F != 0;_tmp43F=_tmp43F->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp43F->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9C9=Cyc_Toc_unresolvedmem_exp_r(0,_tmp517);e->r=_tmp9C9;});
goto _LL0;case 27U: _LL39: _tmp51B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp51A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp519=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30A)->f3;_tmp518=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30A)->f4;_LL3A:
# 2861
 Cyc_Toc_exp_to_c(nv,_tmp51A);
Cyc_Toc_exp_to_c(nv,_tmp519);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp440=Cyc_Evexp_eval_const_uint_exp(_tmp51A);struct _tuple13 _tmp441=_tmp440;unsigned int _tmp44B;int _tmp44A;_LL105: _tmp44B=_tmp441.f1;_tmp44A=_tmp441.f2;_LL106:;{
void*_tmp442=Cyc_Toc_typ_to_c((void*)_check_null(_tmp519->topt));
struct Cyc_List_List*es=0;
# 2868
if(!Cyc_Toc_is_zero(_tmp519)){
if(!_tmp44A)
({void*_tmp443=0U;({unsigned int _tmp9CB=_tmp51A->loc;struct _dyneither_ptr _tmp9CA=({const char*_tmp444="cannot determine value of constant";_tag_dyneither(_tmp444,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9CB,_tmp9CA,_tag_dyneither(_tmp443,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp44B;++ i){
({struct Cyc_List_List*_tmp9CD=({struct Cyc_List_List*_tmp446=_cycalloc(sizeof(*_tmp446));(({struct _tuple20*_tmp9CC=({struct _tuple20*_tmp445=_cycalloc(sizeof(*_tmp445));((*_tmp445).f1=0,(*_tmp445).f2=_tmp519);_tmp445;});(*_tmp446).hd=_tmp9CC;}),(*_tmp446).tl=es);_tmp446;});es=_tmp9CD;});}}
# 2874
if(_tmp518){
struct Cyc_Absyn_Exp*_tmp447=({void*_tmp9CE=_tmp442;Cyc_Toc_cast_it(_tmp9CE,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp9D1=({struct Cyc_List_List*_tmp9D0=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9D0,({struct Cyc_List_List*_tmp449=_cycalloc(sizeof(*_tmp449));(({struct _tuple20*_tmp9CF=({struct _tuple20*_tmp448=_cycalloc(sizeof(*_tmp448));((*_tmp448).f1=0,(*_tmp448).f2=_tmp447);_tmp448;});(*_tmp449).hd=_tmp9CF;}),(*_tmp449).tl=0);_tmp449;}));});es=_tmp9D1;});}}
# 2879
({void*_tmp9D2=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9D2;});};}
# 2881
goto _LL0;case 28U: _LL3B: _tmp51E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp51D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp51C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30A)->f3;_LL3C:
# 2888
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9D3=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9D3;});else{
# 2891
Cyc_Toc_exp_to_c(nv,_tmp51E);}
goto _LL0;case 30U: _LL3D: _tmp520=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp51F=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL3E:
# 2895
{struct Cyc_List_List*_tmp44C=_tmp51F;for(0;_tmp44C != 0;_tmp44C=_tmp44C->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp44C->hd)).f2);}}{
void*_tmp44D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp44D;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp44E=Cyc_Tcutil_compress(_tmp44D);void*_tmp44F=_tmp44E;union Cyc_Absyn_AggrInfoU _tmp453;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44F)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44F)->f1).targs == 0){_LL108: _tmp453=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44F)->f1).aggr_info;_LL109: {
# 2902
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp453);
({void*_tmp9D4=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp450=_cycalloc(sizeof(*_tmp450));((((((((*_tmp450).tag=29U,(*_tmp450).f1=sd->name)),(*_tmp450).f2=0)),(*_tmp450).f3=_tmp51F)),(*_tmp450).f4=sd);_tmp450;});e->r=_tmp9D4;});
e->topt=_tmp44D;
goto _LL107;}}else{goto _LL10A;}}else{_LL10A: _LL10B:
({void*_tmp451=0U;({struct _dyneither_ptr _tmp9D5=({const char*_tmp452="anonStruct type not an aggregate";_tag_dyneither(_tmp452,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D5,_tag_dyneither(_tmp451,sizeof(void*),0U));});});}_LL107:;}else{
# 2909
({void*_tmp9D6=Cyc_Toc_unresolvedmem_exp_r(0,_tmp51F);e->r=_tmp9D6;});}
goto _LL0;};case 29U: _LL3F: _tmp524=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp523=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp522=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30A)->f3;_tmp521=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30A)->f4;_LL40:
# 2915
 if(_tmp521 == 0  || _tmp521->impl == 0)
({void*_tmp454=0U;({struct _dyneither_ptr _tmp9D7=({const char*_tmp455="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp455,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D7,_tag_dyneither(_tmp454,sizeof(void*),0U));});});{
void*_tmp456=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp457=Cyc_Tcutil_compress(_tmp456);void*_tmp458=_tmp457;union Cyc_Absyn_AggrInfoU _tmp45B;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp458)->tag == 11U){_LL10D: _tmp45B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp458)->f1).aggr_info;_LL10E:
({struct _tuple1*_tmp9D8=(Cyc_Absyn_aggr_kinded_name(_tmp45B)).f2;*_tmp524=_tmp9D8;});goto _LL10C;}else{_LL10F: _LL110:
({void*_tmp459=0U;({struct _dyneither_ptr _tmp9D9=({const char*_tmp45A="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp45A,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D9,_tag_dyneither(_tmp459,sizeof(void*),0U));});});}_LL10C:;}{
# 2926
struct Cyc_List_List*_tmp45C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->fields;
if(_tmp45C == 0)return;
for(0;((struct Cyc_List_List*)_check_null(_tmp45C))->tl != 0;_tmp45C=_tmp45C->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp45D=(struct Cyc_Absyn_Aggrfield*)_tmp45C->hd;
struct Cyc_List_List*_tmp45E=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp522,_tmp521->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->fields);
# 2933
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp45F=Cyc_Tcutil_compress(old_typ);void*_tmp460=_tmp45F;struct Cyc_List_List*_tmp496;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp460)->tag == 11U){_LL112: _tmp496=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp460)->f1).targs;_LL113:
# 2937
{struct Cyc_List_List*_tmp461=_tmp45E;for(0;_tmp461 != 0;_tmp461=_tmp461->tl){
struct _tuple34*_tmp462=(struct _tuple34*)_tmp461->hd;struct _tuple34*_tmp463=_tmp462;struct Cyc_Absyn_Aggrfield*_tmp493;struct Cyc_Absyn_Exp*_tmp492;_LL117: _tmp493=_tmp463->f1;_tmp492=_tmp463->f2;_LL118:;{
void*_tmp464=_tmp492->topt;
Cyc_Toc_exp_to_c(nv,_tmp492);
# 2942
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp493->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp492->topt)))
({void*_tmp9DB=({void*_tmp9DA=Cyc_Toc_typ_to_c(_tmp493->type);Cyc_Toc_cast_it(_tmp9DA,
Cyc_Absyn_copy_exp(_tmp492));})->r;
# 2944
_tmp492->r=_tmp9DB;});
# 2947
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->tagged){
struct _dyneither_ptr*_tmp465=_tmp493->name;
struct Cyc_Absyn_Exp*_tmp466=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp521,_tmp465),0U);
struct _tuple20*_tmp467=({struct _tuple20*_tmp47C=_cycalloc(sizeof(*_tmp47C));(({struct Cyc_List_List*_tmp9DD=({void*_tmp47A[1U];({void*_tmp9DC=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));((*_tmp47B).tag=1U,(*_tmp47B).f1=Cyc_Toc_tag_sp);_tmp47B;});_tmp47A[0]=_tmp9DC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp47A,sizeof(void*),1U));});(*_tmp47C).f1=_tmp9DD;}),(*_tmp47C).f2=_tmp466);_tmp47C;});
# 2952
struct _tuple20*_tmp468=({struct _tuple20*_tmp479=_cycalloc(sizeof(*_tmp479));(({struct Cyc_List_List*_tmp9DF=({void*_tmp477[1U];({void*_tmp9DE=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp478=_cycalloc(sizeof(*_tmp478));((*_tmp478).tag=1U,(*_tmp478).f1=Cyc_Toc_val_sp);_tmp478;});_tmp477[0]=_tmp9DE;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp477,sizeof(void*),1U));});(*_tmp479).f1=_tmp9DF;}),(*_tmp479).f2=_tmp492);_tmp479;});
# 2954
struct _tuple1*s=({struct _tuple1*_tmp476=_cycalloc(sizeof(*_tmp476));(({union Cyc_Absyn_Nmspace _tmp9E3=Cyc_Absyn_Abs_n(0,1);(*_tmp476).f1=_tmp9E3;}),({
struct _dyneither_ptr*_tmp9E2=({struct _dyneither_ptr*_tmp475=_cycalloc(sizeof(*_tmp475));({struct _dyneither_ptr _tmp9E1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp474;_tmp474.tag=0U;_tmp474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp465);({struct Cyc_String_pa_PrintArg_struct _tmp473;_tmp473.tag=0U;_tmp473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp521->name).f2);({void*_tmp471[2U];(_tmp471[0]=& _tmp473,_tmp471[1]=& _tmp474);({struct _dyneither_ptr _tmp9E0=({const char*_tmp472="_union_%s_%s";_tag_dyneither(_tmp472,sizeof(char),13U);});Cyc_aprintf(_tmp9E0,_tag_dyneither(_tmp471,sizeof(void*),2U));});});});});*_tmp475=_tmp9E1;});_tmp475;});(*_tmp476).f2=_tmp9E2;}));_tmp476;});
# 2957
struct _tuple20*_tmp469=({struct _tuple20*_tmp470=_cycalloc(sizeof(*_tmp470));(({struct Cyc_List_List*_tmp9E7=({void*_tmp46C[1U];({void*_tmp9E6=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));((*_tmp46D).tag=1U,(*_tmp46D).f1=_tmp465);_tmp46D;});_tmp46C[0]=_tmp9E6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46C,sizeof(void*),1U));});(*_tmp470).f1=_tmp9E7;}),({
struct Cyc_Absyn_Exp*_tmp9E5=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp46F=_cycalloc(sizeof(*_tmp46F));((((((((*_tmp46F).tag=29U,(*_tmp46F).f1=s)),(*_tmp46F).f2=0)),({struct Cyc_List_List*_tmp9E4=({struct _tuple20*_tmp46E[2U];(_tmp46E[0]=_tmp467,_tmp46E[1]=_tmp468);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46E,sizeof(struct _tuple20*),2U));});(*_tmp46F).f3=_tmp9E4;}))),(*_tmp46F).f4=0);_tmp46F;}),0U);(*_tmp470).f2=_tmp9E5;}));_tmp470;});
# 2960
({void*_tmp9E9=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp46B=_cycalloc(sizeof(*_tmp46B));((((((((*_tmp46B).tag=29U,(*_tmp46B).f1=*_tmp524)),(*_tmp46B).f2=0)),({struct Cyc_List_List*_tmp9E8=({struct _tuple20*_tmp46A[1U];_tmp46A[0]=_tmp469;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46A,sizeof(struct _tuple20*),1U));});(*_tmp46B).f3=_tmp9E8;}))),(*_tmp46B).f4=_tmp521);_tmp46B;});e->r=_tmp9E9;});}
# 2966
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp45D == _tmp493){
struct Cyc_List_List*_tmp47D=({struct Cyc_List_List*_tmp9EA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->exist_vars,_tmp523);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9EA,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp521->tvs,_tmp496));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp47E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->fields;for(0;_tmp47E != 0;_tmp47E=_tmp47E->tl){
struct Cyc_Absyn_Aggrfield*_tmp47F=(struct Cyc_Absyn_Aggrfield*)_tmp47E->hd;
void*_tmp480=Cyc_Tcutil_substitute(_tmp47D,_tmp47F->type);
struct Cyc_Absyn_Aggrfield*_tmp481=({struct Cyc_Absyn_Aggrfield*_tmp9EB=_tmp47F;Cyc_Toc_aggrfield_to_c(_tmp9EB,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp47D,_tmp480)));});
({struct Cyc_List_List*_tmp9EC=({struct Cyc_List_List*_tmp482=_cycalloc(sizeof(*_tmp482));((*_tmp482).hd=_tmp481,(*_tmp482).tl=new_fields);_tmp482;});new_fields=_tmp9EC;});
# 2981
if(_tmp47E->tl == 0){
{void*_tmp483=Cyc_Tcutil_compress(_tmp481->type);void*_tmp484=_tmp483;struct Cyc_Absyn_ArrayInfo _tmp489;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp484)->tag == 8U){_LL11A: _tmp489=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp484)->f1;_LL11B:
# 2984
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp489.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp485=_tmp489;
({struct Cyc_Absyn_Exp*_tmp9ED=Cyc_Absyn_uint_exp(0U,0U);_tmp485.num_elts=_tmp9ED;});
({void*_tmp9EE=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp486=_cycalloc(sizeof(*_tmp486));((*_tmp486).tag=8U,(*_tmp486).f1=_tmp485);_tmp486;});_tmp481->type=_tmp9EE;});}
# 2989
goto _LL119;}else{_LL11C: _LL11D:
# 2993
 if(_tmp492->topt == 0)
goto _LL119;
{void*_tmp487=Cyc_Tcutil_compress((void*)_check_null(_tmp492->topt));void*_tmp488=_tmp487;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp488)->tag == 11U){_LL11F: _LL120:
# 2997
 _tmp481->type=(void*)_check_null(_tmp492->topt);goto _LL11E;}else{_LL121: _LL122:
 goto _LL11E;}_LL11E:;}
# 3000
goto _LL119;}_LL119:;}
# 3002
if(!Cyc_Toc_is_array_type(_tmp47F->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp47F->type)))
({struct Cyc_List_List*_tmp9F0=({struct Cyc_List_List*_tmp48B=_cycalloc(sizeof(*_tmp48B));(({void*_tmp9EF=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp48A=_cycalloc(sizeof(*_tmp48A));((*_tmp48A).tag=6U,(*_tmp48A).f1=0);_tmp48A;});(*_tmp48B).hd=_tmp9EF;}),(*_tmp48B).tl=_tmp481->attributes);_tmp48B;});_tmp481->attributes=_tmp9F0;});}}}
# 3008
({struct Cyc_Absyn_Aggrdecl*_tmp9F4=({struct _dyneither_ptr*_tmp9F3=({struct _dyneither_ptr*_tmp48F=_cycalloc(sizeof(*_tmp48F));({struct _dyneither_ptr _tmp9F2=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp48E;_tmp48E.tag=1U;_tmp48E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp48C[1U];_tmp48C[0]=& _tmp48E;({struct _dyneither_ptr _tmp9F1=({const char*_tmp48D="_genStruct%d";_tag_dyneither(_tmp48D,sizeof(char),13U);});Cyc_aprintf(_tmp9F1,_tag_dyneither(_tmp48C,sizeof(void*),1U));});});});*_tmp48F=_tmp9F2;});_tmp48F;});Cyc_Toc_make_c_struct_defn(_tmp9F3,
# 3010
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 3008
_tmp521=_tmp9F4;});
# 3011
*_tmp524=_tmp521->name;
Cyc_Toc_aggrdecl_to_c(_tmp521,1);
# 3014
({void*_tmp9F6=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp491=_cycalloc(sizeof(*_tmp491));((*_tmp491).tag=11U,((({union Cyc_Absyn_AggrInfoU _tmp9F5=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp490=_cycalloc(sizeof(*_tmp490));*_tmp490=_tmp521;_tmp490;}));((*_tmp491).f1).aggr_info=_tmp9F5;}),((*_tmp491).f1).targs=0)));_tmp491;});e->topt=_tmp9F6;});}};}}
# 3017
goto _LL111;}else{_LL114: _LL115:
({void*_tmp494=0U;({struct _dyneither_ptr _tmp9F7=({const char*_tmp495="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp495,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F7,_tag_dyneither(_tmp494,sizeof(void*),0U));});});}_LL111:;}else{
# 3026
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->tagged){
# 3028
struct _tuple34*_tmp497=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp45E))->hd;struct _tuple34*_tmp498=_tmp497;struct Cyc_Absyn_Aggrfield*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A3;_LL124: _tmp4A4=_tmp498->f1;_tmp4A3=_tmp498->f2;_LL125:;{
void*_tmp499=(void*)_check_null(_tmp4A3->topt);
void*_tmp49A=_tmp4A4->type;
Cyc_Toc_exp_to_c(nv,_tmp4A3);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp49A) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp499))
({void*_tmp9F9=({
void*_tmp9F8=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmp9F8,Cyc_Absyn_new_exp(_tmp4A3->r,0U));});
# 3034
_tmp4A3->r=_tmp9F9;});{
# 3037
int i=Cyc_Toc_get_member_offset(_tmp521,_tmp4A4->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp49B=({struct _tuple20*_tmp4A0[2U];(({struct _tuple20*_tmp9FB=({struct _tuple20*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));((*_tmp4A1).f1=0,(*_tmp4A1).f2=field_tag_exp);_tmp4A1;});_tmp4A0[0]=_tmp9FB;}),({struct _tuple20*_tmp9FA=({struct _tuple20*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));((*_tmp4A2).f1=0,(*_tmp4A2).f2=_tmp4A3);_tmp4A2;});_tmp4A0[1]=_tmp9FA;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A0,sizeof(struct _tuple20*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp49B,0U);
struct Cyc_List_List*ds=({void*_tmp49E[1U];({void*_tmp9FC=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp49F=_cycalloc(sizeof(*_tmp49F));((*_tmp49F).tag=1U,(*_tmp49F).f1=_tmp4A4->name);_tmp49F;});_tmp49E[0]=_tmp9FC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp49E,sizeof(void*),1U));});
({void*_tmp9FE=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp49C[1U];({struct _tuple20*_tmp9FD=({struct _tuple20*_tmp49D=_cycalloc(sizeof(*_tmp49D));((*_tmp49D).f1=ds,(*_tmp49D).f2=umem);_tmp49D;});_tmp49C[0]=_tmp9FD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp49C,sizeof(struct _tuple20*),1U));}));e->r=_tmp9FE;});};};}else{
# 3045
struct Cyc_List_List*_tmp4A5=0;
struct Cyc_List_List*_tmp4A6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->fields;
for(0;_tmp4A6 != 0;_tmp4A6=_tmp4A6->tl){
struct Cyc_List_List*_tmp4A7=_tmp45E;for(0;_tmp4A7 != 0;_tmp4A7=_tmp4A7->tl){
if((*((struct _tuple34*)_tmp4A7->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp4A6->hd){
struct _tuple34*_tmp4A8=(struct _tuple34*)_tmp4A7->hd;struct _tuple34*_tmp4A9=_tmp4A8;struct Cyc_Absyn_Aggrfield*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4AE;_LL127: _tmp4AF=_tmp4A9->f1;_tmp4AE=_tmp4A9->f2;_LL128:;{
void*_tmp4AA=Cyc_Toc_typ_to_c(_tmp4AF->type);
void*_tmp4AB=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4AE->topt));
Cyc_Toc_exp_to_c(nv,_tmp4AE);
# 3055
if(!Cyc_Tcutil_unify(_tmp4AA,_tmp4AB)){
# 3057
if(!Cyc_Tcutil_is_arithmetic_type(_tmp4AA) || !
Cyc_Tcutil_is_arithmetic_type(_tmp4AB))
({struct Cyc_Absyn_Exp*_tmpA00=({void*_tmp9FF=_tmp4AA;Cyc_Toc_cast_it(_tmp9FF,Cyc_Absyn_copy_exp(_tmp4AE));});_tmp4AE=_tmpA00;});}
({struct Cyc_List_List*_tmpA02=({struct Cyc_List_List*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));(({struct _tuple20*_tmpA01=({struct _tuple20*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));((*_tmp4AC).f1=0,(*_tmp4AC).f2=_tmp4AE);_tmp4AC;});(*_tmp4AD).hd=_tmpA01;}),(*_tmp4AD).tl=_tmp4A5);_tmp4AD;});_tmp4A5=_tmpA02;});
break;};}}}
# 3064
({void*_tmpA03=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4A5));e->r=_tmpA03;});}}
# 3067
goto _LL0;};};};case 31U: _LL41: _tmp527=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp526=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_tmp525=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30A)->f3;_LL42: {
# 3071
struct Cyc_List_List*_tmp4B0=_tmp525->typs;
{struct Cyc_List_List*_tmp4B1=_tmp527;for(0;_tmp4B1 != 0;(_tmp4B1=_tmp4B1->tl,_tmp4B0=_tmp4B0->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4B1->hd;
void*_tmp4B2=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4B0))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp4B2))
({void*_tmpA04=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmpA04;});}}{
# 3082
struct Cyc_Absyn_Exp*_tmp4B3=_tmp526->is_extensible?Cyc_Absyn_var_exp(_tmp525->name,0U):
 Cyc_Toc_datatype_tag(_tmp526,_tmp525->name);
# 3085
if(!Cyc_Toc_is_toplevel(nv)){
# 3087
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp527 != 0;(_tmp527=_tmp527->tl,++ i)){
({struct Cyc_List_List*_tmpA09=({struct Cyc_List_List*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));(({struct _tuple20*_tmpA08=({struct _tuple20*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));(({struct Cyc_List_List*_tmpA07=({struct Cyc_List_List*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));(({void*_tmpA06=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));((*_tmp4B4).tag=1U,({struct _dyneither_ptr*_tmpA05=Cyc_Absyn_fieldname(i);(*_tmp4B4).f1=_tmpA05;}));_tmp4B4;});(*_tmp4B5).hd=_tmpA06;}),(*_tmp4B5).tl=0);_tmp4B5;});(*_tmp4B6).f1=_tmpA07;}),(*_tmp4B6).f2=(struct Cyc_Absyn_Exp*)_tmp527->hd);_tmp4B6;});(*_tmp4B7).hd=_tmpA08;}),(*_tmp4B7).tl=dles);_tmp4B7;});dles=_tmpA09;});}}{
# 3095
struct _tuple20*_tmp4B8=({struct _tuple20*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));(({struct Cyc_List_List*_tmpA0B=({struct Cyc_List_List*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));(({void*_tmpA0A=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));((*_tmp4BB).tag=1U,(*_tmp4BB).f1=Cyc_Toc_tag_sp);_tmp4BB;});(*_tmp4BC).hd=_tmpA0A;}),(*_tmp4BC).tl=0);_tmp4BC;});(*_tmp4BD).f1=_tmpA0B;}),(*_tmp4BD).f2=_tmp4B3);_tmp4BD;});
# 3097
({void*_tmpA0F=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));((((((((*_tmp4BA).tag=29U,({struct _tuple1*_tmpA0E=Cyc_Toc_collapse_qvars(_tmp525->name,_tmp526->name);(*_tmp4BA).f1=_tmpA0E;}))),(*_tmp4BA).f2=0)),({
struct Cyc_List_List*_tmpA0D=({struct Cyc_List_List*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));((*_tmp4B9).hd=_tmp4B8,({struct Cyc_List_List*_tmpA0C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(*_tmp4B9).tl=_tmpA0C;}));_tmp4B9;});(*_tmp4BA).f3=_tmpA0D;}))),(*_tmp4BA).f4=0);_tmp4BA;});
# 3097
e->r=_tmpA0F;});};}else{
# 3102
struct Cyc_List_List*_tmp4BE=0;
for(0;_tmp527 != 0;_tmp527=_tmp527->tl){
({struct Cyc_List_List*_tmpA11=({struct Cyc_List_List*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));(({struct _tuple20*_tmpA10=({struct _tuple20*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));((*_tmp4BF).f1=0,(*_tmp4BF).f2=(struct Cyc_Absyn_Exp*)_tmp527->hd);_tmp4BF;});(*_tmp4C0).hd=_tmpA10;}),(*_tmp4C0).tl=_tmp4BE);_tmp4C0;});_tmp4BE=_tmpA11;});}
({void*_tmpA14=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));(({struct _tuple20*_tmpA13=({struct _tuple20*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));((*_tmp4C1).f1=0,(*_tmp4C1).f2=_tmp4B3);_tmp4C1;});(*_tmp4C2).hd=_tmpA13;}),({
struct Cyc_List_List*_tmpA12=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4BE);(*_tmp4C2).tl=_tmpA12;}));_tmp4C2;}));
# 3105
e->r=_tmpA14;});}
# 3108
goto _LL0;};}case 32U: _LL43: _LL44:
# 3110
 goto _LL46;case 33U: _LL45: _LL46:
 goto _LL0;case 34U: _LL47: _tmp52D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30A)->f1).is_calloc;_tmp52C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30A)->f1).rgn;_tmp52B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30A)->f1).elt_type;_tmp52A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30A)->f1).num_elts;_tmp529=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30A)->f1).fat_result;_tmp528=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30A)->f1).inline_call;_LL48: {
# 3114
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp52B)));
Cyc_Toc_exp_to_c(nv,_tmp52A);
# 3118
if(_tmp529){
struct _tuple1*_tmp4C3=Cyc_Toc_temp_var();
struct _tuple1*_tmp4C4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp52D){
xexp=_tmp52A;
if(_tmp52C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52C;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmpA17=({struct Cyc_Absyn_Exp*_tmpA16=rgn;struct Cyc_Absyn_Exp*_tmpA15=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA16,_tmpA15,Cyc_Absyn_var_exp(_tmp4C3,0U));});pexp=_tmpA17;});}else{
# 3129
({struct Cyc_Absyn_Exp*_tmpA1A=({void*_tmpA19=*_tmp52B;struct Cyc_Absyn_Exp*_tmpA18=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA19,_tmpA18,Cyc_Absyn_var_exp(_tmp4C3,0U));});pexp=_tmpA1A;});}
# 3131
({struct Cyc_Absyn_Exp*_tmpA1F=({struct Cyc_Absyn_Exp*_tmpA1E=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA1E,({struct Cyc_Absyn_Exp*_tmp4C5[3U];(((({
struct Cyc_Absyn_Exp*_tmpA1D=Cyc_Absyn_var_exp(_tmp4C4,0U);_tmp4C5[0]=_tmpA1D;}),({struct Cyc_Absyn_Exp*_tmpA1C=Cyc_Absyn_sizeoftyp_exp(t_c,0U);_tmp4C5[1]=_tmpA1C;}))),({
struct Cyc_Absyn_Exp*_tmpA1B=Cyc_Absyn_var_exp(_tmp4C3,0U);_tmp4C5[2]=_tmpA1B;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C5,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3131
rexp=_tmpA1F;});}else{
# 3135
if(_tmp52C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52C;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp528)
({struct Cyc_Absyn_Exp*_tmpA21=({struct Cyc_Absyn_Exp*_tmpA20=rgn;Cyc_Toc_rmalloc_inline_exp(_tmpA20,Cyc_Absyn_var_exp(_tmp4C3,0U));});pexp=_tmpA21;});else{
# 3141
({struct Cyc_Absyn_Exp*_tmpA23=({struct Cyc_Absyn_Exp*_tmpA22=rgn;Cyc_Toc_rmalloc_exp(_tmpA22,Cyc_Absyn_var_exp(_tmp4C3,0U));});pexp=_tmpA23;});}}else{
# 3143
({struct Cyc_Absyn_Exp*_tmpA25=({void*_tmpA24=*_tmp52B;Cyc_Toc_malloc_exp(_tmpA24,Cyc_Absyn_var_exp(_tmp4C3,0U));});pexp=_tmpA25;});}
# 3145
({struct Cyc_Absyn_Exp*_tmpA2A=({struct Cyc_Absyn_Exp*_tmpA29=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA29,({struct Cyc_Absyn_Exp*_tmp4C6[3U];(((({struct Cyc_Absyn_Exp*_tmpA28=Cyc_Absyn_var_exp(_tmp4C4,0U);_tmp4C6[0]=_tmpA28;}),({struct Cyc_Absyn_Exp*_tmpA27=Cyc_Absyn_uint_exp(1U,0U);_tmp4C6[1]=_tmpA27;}))),({
struct Cyc_Absyn_Exp*_tmpA26=Cyc_Absyn_var_exp(_tmp4C3,0U);_tmp4C6[2]=_tmpA26;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C6,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3145
rexp=_tmpA2A;});}{
# 3148
struct Cyc_Absyn_Stmt*_tmp4C7=({struct _tuple1*_tmpA30=_tmp4C3;void*_tmpA2F=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpA2E=_tmp52A;Cyc_Absyn_declare_stmt(_tmpA30,_tmpA2F,_tmpA2E,({
struct _tuple1*_tmpA2D=_tmp4C4;void*_tmpA2C=Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpA2B=pexp;Cyc_Absyn_declare_stmt(_tmpA2D,_tmpA2C,_tmpA2B,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmpA31=Cyc_Toc_stmt_exp_r(_tmp4C7);e->r=_tmpA31;});};}else{
# 3153
if(_tmp52D){
if(_tmp52C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52C;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmpA34=({struct Cyc_Absyn_Exp*_tmpA33=rgn;struct Cyc_Absyn_Exp*_tmpA32=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA33,_tmpA32,_tmp52A);})->r;e->r=_tmpA34;});}else{
# 3159
({void*_tmpA37=({void*_tmpA36=*_tmp52B;struct Cyc_Absyn_Exp*_tmpA35=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA36,_tmpA35,_tmp52A);})->r;e->r=_tmpA37;});}}else{
# 3162
if(_tmp52C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52C;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp528)
({void*_tmpA38=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp52A))->r;e->r=_tmpA38;});else{
# 3168
({void*_tmpA39=(Cyc_Toc_rmalloc_exp(rgn,_tmp52A))->r;e->r=_tmpA39;});}}else{
# 3170
({void*_tmpA3A=(Cyc_Toc_malloc_exp(*_tmp52B,_tmp52A))->r;e->r=_tmpA3A;});}}}
# 3174
goto _LL0;}case 35U: _LL49: _tmp52F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp52E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL4A: {
# 3183
void*e1_old_typ=(void*)_check_null(_tmp52F->topt);
void*e2_old_typ=(void*)_check_null(_tmp52E->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp4C8=0U;({struct _dyneither_ptr _tmpA3B=({const char*_tmp4C9="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp4C9,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA3B,_tag_dyneither(_tmp4C8,sizeof(void*),0U));});});{
# 3191
unsigned int _tmp4CA=e->loc;
struct _tuple1*_tmp4CB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4CC=Cyc_Absyn_var_exp(_tmp4CB,_tmp4CA);_tmp4CC->topt=e1_old_typ;{
struct _tuple1*_tmp4CD=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4CE=Cyc_Absyn_var_exp(_tmp4CD,_tmp4CA);_tmp4CE->topt=e2_old_typ;{
# 3197
struct Cyc_Absyn_Exp*_tmp4CF=({struct Cyc_Absyn_Exp*_tmpA3D=Cyc_Tcutil_deep_copy_exp(1,_tmp52F);struct Cyc_Absyn_Exp*_tmpA3C=_tmp4CE;Cyc_Absyn_assign_exp(_tmpA3D,_tmpA3C,_tmp4CA);});_tmp4CF->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4D0=Cyc_Absyn_exp_stmt(_tmp4CF,_tmp4CA);
struct Cyc_Absyn_Exp*_tmp4D1=({struct Cyc_Absyn_Exp*_tmpA3F=Cyc_Tcutil_deep_copy_exp(1,_tmp52E);struct Cyc_Absyn_Exp*_tmpA3E=_tmp4CC;Cyc_Absyn_assign_exp(_tmpA3F,_tmpA3E,_tmp4CA);});_tmp4D1->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4D2=Cyc_Absyn_exp_stmt(_tmp4D1,_tmp4CA);
# 3202
struct Cyc_Absyn_Stmt*_tmp4D3=({struct _tuple1*_tmpA47=_tmp4CB;void*_tmpA46=e1_old_typ;struct Cyc_Absyn_Exp*_tmpA45=_tmp52F;struct Cyc_Absyn_Stmt*_tmpA44=({
struct _tuple1*_tmpA43=_tmp4CD;void*_tmpA42=e2_old_typ;struct Cyc_Absyn_Exp*_tmpA41=_tmp52E;struct Cyc_Absyn_Stmt*_tmpA40=
Cyc_Absyn_seq_stmt(_tmp4D0,_tmp4D2,_tmp4CA);
# 3203
Cyc_Absyn_declare_stmt(_tmpA43,_tmpA42,_tmpA41,_tmpA40,_tmp4CA);});
# 3202
Cyc_Absyn_declare_stmt(_tmpA47,_tmpA46,_tmpA45,_tmpA44,_tmp4CA);});
# 3205
Cyc_Toc_stmt_to_c(nv,_tmp4D3);
({void*_tmpA48=Cyc_Toc_stmt_exp_r(_tmp4D3);e->r=_tmpA48;});
goto _LL0;};};};};};}case 38U: _LL4B: _tmp531=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_tmp530=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30A)->f2;_LL4C: {
# 3210
void*_tmp4D4=Cyc_Tcutil_compress((void*)_check_null(_tmp531->topt));
Cyc_Toc_exp_to_c(nv,_tmp531);
{void*_tmp4D5=_tmp4D4;struct Cyc_Absyn_Aggrdecl*_tmp4DB;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4D5)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4D5)->f1).aggr_info).KnownAggr).tag == 2){_LL12A: _tmp4DB=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4D5)->f1).aggr_info).KnownAggr).val;_LL12B: {
# 3214
struct Cyc_Absyn_Exp*_tmp4D6=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp4DB,_tmp530),0U);
struct Cyc_Absyn_Exp*_tmp4D7=Cyc_Toc_member_exp(_tmp531,_tmp530,0U);
struct Cyc_Absyn_Exp*_tmp4D8=Cyc_Toc_member_exp(_tmp4D7,Cyc_Toc_tag_sp,0U);
({void*_tmpA49=(Cyc_Absyn_eq_exp(_tmp4D8,_tmp4D6,0U))->r;e->r=_tmpA49;});
goto _LL129;}}else{goto _LL12C;}}else{_LL12C: _LL12D:
({void*_tmp4D9=0U;({struct _dyneither_ptr _tmpA4A=({const char*_tmp4DA="non-aggregate type in tagcheck";_tag_dyneither(_tmp4DA,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA4A,_tag_dyneither(_tmp4D9,sizeof(void*),0U));});});}_LL129:;}
# 3221
goto _LL0;}case 37U: _LL4D: _tmp532=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp30A)->f1;_LL4E:
 Cyc_Toc_stmt_to_c(nv,_tmp532);goto _LL0;case 36U: _LL4F: _LL50:
({void*_tmp4DC=0U;({struct _dyneither_ptr _tmpA4B=({const char*_tmp4DD="UnresolvedMem";_tag_dyneither(_tmp4DD,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA4B,_tag_dyneither(_tmp4DC,sizeof(void*),0U));});});case 25U: _LL51: _LL52:
({void*_tmp4DE=0U;({struct _dyneither_ptr _tmpA4C=({const char*_tmp4DF="compoundlit";_tag_dyneither(_tmp4DF,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpA4C,_tag_dyneither(_tmp4DE,sizeof(void*),0U));});});case 39U: _LL53: _LL54:
({void*_tmp4E0=0U;({struct _dyneither_ptr _tmpA4D=({const char*_tmp4E1="valueof(-)";_tag_dyneither(_tmp4E1,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA4D,_tag_dyneither(_tmp4E0,sizeof(void*),0U));});});default: _LL55: _LL56:
({void*_tmp4E2=0U;({struct _dyneither_ptr _tmpA4E=({const char*_tmp4E3="__asm__";_tag_dyneither(_tmp4E3,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA4E,_tag_dyneither(_tmp4E2,sizeof(void*),0U));});});}_LL0:;};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3254 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3256
return({struct _tuple35*_tmp533=_region_malloc(r,sizeof(*_tmp533));((((((*_tmp533).f1=0,({struct _dyneither_ptr*_tmpA50=Cyc_Toc_fresh_label();(*_tmp533).f2=_tmpA50;}))),({struct _dyneither_ptr*_tmpA4F=Cyc_Toc_fresh_label();(*_tmp533).f3=_tmpA4F;}))),(*_tmp533).f4=sc);_tmp533;});}
# 3261
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp534=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3267
if((int)(((_tmp534->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp537=_tmp534->orig_pat;if((_tmp537.pattern).tag != 1)_throw_match();(_tmp537.pattern).val;})->topt);
void*_tmp535=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp536=_tmp535;switch(*((int*)_tmp536)){case 0U: _LL1: _LL2:
# 3272
 goto _LL4;case 11U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpA52=({void*_tmpA51=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA51,v);});v=_tmpA52;});goto _LL0;}_LL0:;}{
# 3278
void*_tmp538=_tmp534->access;void*_tmp539=_tmp538;struct Cyc_Absyn_Datatypedecl*_tmp544;struct Cyc_Absyn_Datatypefield*_tmp543;unsigned int _tmp542;int _tmp541;struct _dyneither_ptr*_tmp540;unsigned int _tmp53F;switch(*((int*)_tmp539)){case 0U: _LLA: _LLB:
# 3283
 goto _LL9;case 1U: _LLC: _LLD:
# 3288
 if(ps->tl != 0){
void*_tmp53A=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp53B=_tmp53A;struct Cyc_Absyn_Datatypedecl*_tmp53E;struct Cyc_Absyn_Datatypefield*_tmp53D;unsigned int _tmp53C;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp53B)->tag == 3U){_LL15: _tmp53E=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp53B)->f1;_tmp53D=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp53B)->f2;_tmp53C=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp53B)->f3;_LL16:
# 3291
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpA55=({void*_tmpA54=({void*_tmpA53=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp53D->name,_tmp53E->name));Cyc_Absyn_cstar_typ(_tmpA53,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA54,v);});v=_tmpA55;});
({struct Cyc_Absyn_Exp*_tmpA57=({struct Cyc_Absyn_Exp*_tmpA56=v;Cyc_Absyn_aggrarrow_exp(_tmpA56,Cyc_Absyn_fieldname((int)(_tmp53C + 1)),0U);});v=_tmpA57;});
continue;}else{_LL17: _LL18:
# 3296
 goto _LL14;}_LL14:;}
# 3299
({struct Cyc_Absyn_Exp*_tmpA58=Cyc_Absyn_deref_exp(v,0U);v=_tmpA58;});
goto _LL9;case 2U: _LLE: _tmp53F=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp539)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpA5A=({struct Cyc_Absyn_Exp*_tmpA59=v;Cyc_Toc_member_exp(_tmpA59,Cyc_Absyn_fieldname((int)(_tmp53F + 1)),0U);});v=_tmpA5A;});goto _LL9;case 4U: _LL10: _tmp541=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp539)->f1;_tmp540=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp539)->f2;_LL11:
# 3303
({struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Toc_member_exp(v,_tmp540,0U);v=_tmpA5B;});
if(_tmp541)
({struct Cyc_Absyn_Exp*_tmpA5C=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpA5C;});
goto _LL9;default: _LL12: _tmp544=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp539)->f1;_tmp543=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp539)->f2;_tmp542=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp539)->f3;_LL13:
# 3308
({struct Cyc_Absyn_Exp*_tmpA5E=({struct Cyc_Absyn_Exp*_tmpA5D=v;Cyc_Toc_member_exp(_tmpA5D,Cyc_Absyn_fieldname((int)(_tmp542 + 1)),0U);});v=_tmpA5E;});
goto _LL9;}_LL9:;};}
# 3312
return v;}
# 3317
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp545=t;struct Cyc_Absyn_Datatypedecl*_tmp55E;struct Cyc_Absyn_Datatypefield*_tmp55D;struct _dyneither_ptr*_tmp55C;int _tmp55B;int _tmp55A;struct Cyc_Absyn_Datatypedecl*_tmp559;struct Cyc_Absyn_Datatypefield*_tmp558;unsigned int _tmp557;struct _dyneither_ptr _tmp556;int _tmp555;void*_tmp554;struct Cyc_Absyn_Enumfield*_tmp553;struct Cyc_Absyn_Enumdecl*_tmp552;struct Cyc_Absyn_Enumfield*_tmp551;struct Cyc_Absyn_Exp*_tmp550;switch(*((int*)_tmp545)){case 0U: _LL1: _tmp550=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp545)->f1;_LL2:
# 3321
 if(_tmp550 == 0)return v;else{return _tmp550;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA5F=v;Cyc_Absyn_eq_exp(_tmpA5F,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA60=v;Cyc_Absyn_neq_exp(_tmpA60,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp552=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp551=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp545)->f2;_LL8:
# 3325
 return({struct Cyc_Absyn_Exp*_tmpA61=v;Cyc_Absyn_eq_exp(_tmpA61,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp546=_cycalloc(sizeof(*_tmp546));((((*_tmp546).tag=32U,(*_tmp546).f1=_tmp552)),(*_tmp546).f2=_tmp551);_tmp546;}),0U),0U);});case 4U: _LL9: _tmp554=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp553=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp545)->f2;_LLA:
# 3327
 return({struct Cyc_Absyn_Exp*_tmpA62=v;Cyc_Absyn_eq_exp(_tmpA62,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp547=_cycalloc(sizeof(*_tmp547));((((*_tmp547).tag=33U,(*_tmp547).f1=_tmp554)),(*_tmp547).f2=_tmp553);_tmp547;}),0U),0U);});case 5U: _LLB: _tmp556=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp555=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp545)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpA63=v;Cyc_Absyn_eq_exp(_tmpA63,Cyc_Absyn_float_exp(_tmp556,_tmp555,0U),0U);});case 6U: _LLD: _tmp557=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp545)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpA64=v;Cyc_Absyn_eq_exp(_tmpA64,Cyc_Absyn_signed_int_exp((int)_tmp557,0U),0U);});case 7U: _LLF: _tmp55A=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp559=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp545)->f2;_tmp558=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp545)->f3;_LL10:
# 3333
 LOOP1: {
void*_tmp548=v->r;void*_tmp549=_tmp548;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54A;switch(*((int*)_tmp549)){case 14U: _LL16: _tmp54A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp549)->f2;_LL17:
 v=_tmp54A;goto LOOP1;case 20U: _LL18: _tmp54B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp549)->f1;_LL19:
 v=_tmp54B;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3340
({struct Cyc_Absyn_Exp*_tmpA67=({void*_tmpA66=({void*_tmpA65=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp558->name,_tmp559->name));Cyc_Absyn_cstar_typ(_tmpA65,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA66,v);});v=_tmpA67;});
return({struct Cyc_Absyn_Exp*_tmpA68=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA68,Cyc_Absyn_uint_exp((unsigned int)_tmp55A,0U),0U);});case 8U: _LL11: _tmp55C=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp55B=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp545)->f2;_LL12:
# 3345
({struct Cyc_Absyn_Exp*_tmpA6A=({struct Cyc_Absyn_Exp*_tmpA69=Cyc_Toc_member_exp(v,_tmp55C,0U);Cyc_Toc_member_exp(_tmpA69,Cyc_Toc_tag_sp,0U);});v=_tmpA6A;});
return({struct Cyc_Absyn_Exp*_tmpA6B=v;Cyc_Absyn_eq_exp(_tmpA6B,Cyc_Absyn_signed_int_exp(_tmp55B,0U),0U);});default: _LL13: _tmp55E=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp545)->f1;_tmp55D=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp545)->f2;_LL14:
# 3349
 LOOP2: {
void*_tmp54C=v->r;void*_tmp54D=_tmp54C;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Exp*_tmp54E;switch(*((int*)_tmp54D)){case 14U: _LL1D: _tmp54E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp54D)->f2;_LL1E:
 v=_tmp54E;goto LOOP2;case 20U: _LL1F: _tmp54F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp54D)->f1;_LL20:
 v=_tmp54F;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3356
({struct Cyc_Absyn_Exp*_tmpA6E=({void*_tmpA6D=({void*_tmpA6C=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp55D->name,_tmp55E->name));Cyc_Absyn_cstar_typ(_tmpA6C,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA6D,v);});v=_tmpA6E;});
return({struct Cyc_Absyn_Exp*_tmpA6F=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA6F,Cyc_Absyn_var_exp(_tmp55D->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3370
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3378
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3380
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple33 _tmp55F=*((struct _tuple33*)ss->hd);struct _tuple33 _tmp560=_tmp55F;void*_tmp566;_LL1: _tmp566=_tmp560.f1;_LL2:;{
void*_tmp561=_tmp566;struct Cyc_Absyn_Exp*_tmp565;struct _dyneither_ptr*_tmp564;switch(*((int*)_tmp561)){case 3U: _LL4: _LL5:
# 3386
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3389
 continue;case 8U: _LLA: _tmp564=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp561)->f1;_LLB:
# 3391
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpA70=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp562=_cycalloc(sizeof(*_tmp562));((*_tmp562).tag=3U,(*_tmp562).f1=_tmp564);_tmp562;});k=_tmpA70;});
continue;case 7U: _LLC: _LLD:
# 3395
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp565=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp561)->f1;if(_tmp565 != 0){_LLF:
# 3399
({void*_tmpA71=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp563=_cycalloc(sizeof(*_tmp563));((*_tmp563).tag=2U,(*_tmp563).f1=_tmp565);_tmp563;});k=_tmpA71;});
return({struct _tuple36 _tmp784;(_tmp784.f1=0,_tmp784.f2=k);_tmp784;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp785;(_tmp785.f1=0,_tmp785.f2=k);_tmp785;});}_LL3:;};}
# 3408
return({struct _tuple36 _tmp786;(_tmp786.f1=c,_tmp786.f2=k);_tmp786;});}
# 3413
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp567=p;int _tmp572;int _tmp571;unsigned int _tmp570;void*_tmp56F;struct Cyc_Absyn_Enumfield*_tmp56E;struct Cyc_Absyn_Enumdecl*_tmp56D;struct Cyc_Absyn_Enumfield*_tmp56C;switch(*((int*)_tmp567)){case 3U: _LL1: _tmp56D=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp567)->f1;_tmp56C=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp567)->f2;_LL2:
# 3417
({struct Cyc_Absyn_Exp*_tmpA72=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp568=_cycalloc(sizeof(*_tmp568));((((*_tmp568).tag=32U,(*_tmp568).f1=_tmp56D)),(*_tmp568).f2=_tmp56C);_tmp568;}),0U);e=_tmpA72;});goto _LL0;case 4U: _LL3: _tmp56F=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp567)->f1;_tmp56E=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp567)->f2;_LL4:
# 3419
({struct Cyc_Absyn_Exp*_tmpA73=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp569=_cycalloc(sizeof(*_tmp569));((((*_tmp569).tag=33U,(*_tmp569).f1=_tmp56F)),(*_tmp569).f2=_tmp56E);_tmp569;}),0U);e=_tmpA73;});goto _LL0;case 6U: _LL5: _tmp570=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp567)->f1;_LL6:
 _tmp571=(int)_tmp570;goto _LL8;case 7U: _LL7: _tmp571=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp567)->f1;_LL8:
 _tmp572=_tmp571;goto _LLA;case 8U: _LL9: _tmp572=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp567)->f2;_LLA:
# 3423
({struct Cyc_Absyn_Exp*_tmpA74=Cyc_Absyn_uint_exp((unsigned int)_tmp572,0U);e=_tmpA74;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp56A=0U;({struct _dyneither_ptr _tmpA75=({const char*_tmp56B="compile_pat_test_as_case!";_tag_dyneither(_tmp56B,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA75,_tag_dyneither(_tmp56A,sizeof(void*),0U));});});}_LL0:;}
# 3427
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp573=_cycalloc(sizeof(*_tmp573));((*_tmp573).tag=17U,(*_tmp573).f1=e);_tmp573;}),0U);}
# 3431
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3440
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3443
void*t=(void*)_check_null(p->topt);
void*_tmp574=p->r;void*_tmp575=_tmp574;union Cyc_Absyn_AggrInfoU _tmp5A6;struct Cyc_List_List*_tmp5A5;struct Cyc_List_List*_tmp5A4;struct Cyc_List_List*_tmp5A3;struct Cyc_Absyn_Pat*_tmp5A2;struct Cyc_Absyn_Datatypedecl*_tmp5A1;struct Cyc_Absyn_Datatypefield*_tmp5A0;struct Cyc_List_List*_tmp59F;struct Cyc_Absyn_Vardecl*_tmp59E;struct Cyc_Absyn_Pat*_tmp59D;struct Cyc_Absyn_Vardecl*_tmp59C;struct Cyc_Absyn_Vardecl*_tmp59B;struct Cyc_Absyn_Pat*_tmp59A;struct Cyc_Absyn_Vardecl*_tmp599;switch(*((int*)_tmp575)){case 2U: _LL1: _tmp599=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp575)->f2;_LL2: {
# 3446
struct Cyc_Absyn_Pat*_tmp576=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp576->topt=p->topt;
_tmp59B=_tmp599;_tmp59A=_tmp576;goto _LL4;}case 1U: _LL3: _tmp59B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp575)->f1;_tmp59A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp575)->f2;_LL4:
# 3451
({struct Cyc_List_List*_tmpA76=({struct Cyc_List_List*_tmp577=_region_malloc(rgn,sizeof(*_tmp577));((*_tmp577).hd=_tmp59B,(*_tmp577).tl=*decls);_tmp577;});*decls=_tmpA76;});{
struct Cyc_Absyn_Stmt*_tmp578=({struct Cyc_Absyn_Exp*_tmpA77=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp579=_cycalloc(sizeof(*_tmp579));((*_tmp579).tag=4U,(*_tmp579).f1=_tmp59B);_tmp579;}),0U);Cyc_Absyn_assign_stmt(_tmpA77,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA78=_tmp578;Cyc_Toc_seq_stmt_opt(_tmpA78,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp59A));});};case 4U: _LL5: _tmp59C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp575)->f2;_LL6:
# 3456
({struct Cyc_List_List*_tmpA79=({struct Cyc_List_List*_tmp57A=_region_malloc(rgn,sizeof(*_tmp57A));((*_tmp57A).hd=_tmp59C,(*_tmp57A).tl=*decls);_tmp57A;});*decls=_tmpA79;});
return({struct Cyc_Absyn_Exp*_tmpA7A=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));((*_tmp57B).tag=4U,(*_tmp57B).f1=_tmp59C);_tmp57B;}),0U);Cyc_Absyn_assign_stmt(_tmpA7A,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp59E=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp575)->f1;_tmp59D=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp575)->f2;_LLA:
# 3461
({struct Cyc_List_List*_tmpA7B=({struct Cyc_List_List*_tmp57C=_region_malloc(rgn,sizeof(*_tmp57C));((*_tmp57C).hd=_tmp59E,(*_tmp57C).tl=*decls);_tmp57C;});*decls=_tmpA7B;});
({void*_tmpA7C=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);_tmp59E->type=_tmpA7C;});{
# 3464
struct Cyc_Absyn_Stmt*_tmp57D=({struct Cyc_Absyn_Exp*_tmpA7F=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp57E=_cycalloc(sizeof(*_tmp57E));((*_tmp57E).tag=4U,(*_tmp57E).f1=_tmp59E);_tmp57E;}),0U);Cyc_Absyn_assign_stmt(_tmpA7F,({
void*_tmpA7E=({void*_tmpA7D=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_typ(_tmpA7D,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA7E,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA80=_tmp57D;Cyc_Toc_seq_stmt_opt(_tmpA80,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp59D));});};case 9U: _LLB: _LLC:
# 3469
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp575)->f1)->r)->tag == 8U){_LL17: _tmp5A1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp575)->f1)->r)->f1;_tmp5A0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp575)->f1)->r)->f2;_tmp59F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp575)->f1)->r)->f3;_LL18:
# 3478
 if(_tmp59F == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp5A0->name,_tmp5A1->name);
void*_tmp57F=({void*_tmpA81=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_typ(_tmpA81,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpA82=Cyc_Toc_cast_it(_tmp57F,path);path=_tmpA82;});{
int cnt=1;
struct Cyc_List_List*_tmp580=_tmp5A0->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp59F != 0;(((_tmp59F=_tmp59F->tl,_tmp580=((struct Cyc_List_List*)_check_null(_tmp580))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp581=(struct Cyc_Absyn_Pat*)_tmp59F->hd;
if(_tmp581->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp582=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp580))->hd)).f2;
void*_tmp583=(void*)_check_null(_tmp581->topt);
void*_tmp584=Cyc_Toc_typ_to_c_array(_tmp583);
struct Cyc_Absyn_Exp*_tmp585=({struct Cyc_Absyn_Exp*_tmpA83=path;Cyc_Absyn_aggrarrow_exp(_tmpA83,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp582)))
({struct Cyc_Absyn_Exp*_tmpA84=Cyc_Toc_cast_it(_tmp584,_tmp585);_tmp585=_tmpA84;});
({struct Cyc_Absyn_Stmt*_tmpA86=({struct Cyc_Absyn_Stmt*_tmpA85=s;Cyc_Toc_seq_stmt_opt(_tmpA85,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp585,_tmp581));});s=_tmpA86;});};}
# 3496
return s;};};}else{_LL21: _tmp5A2=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp575)->f1;_LL22:
# 3541
 return({struct _RegionHandle*_tmpA8A=rgn;struct Cyc_Toc_Env**_tmpA89=nv;struct Cyc_List_List**_tmpA88=decls;struct Cyc_Absyn_Exp*_tmpA87=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA8A,_tmpA89,_tmpA88,_tmpA87,_tmp5A2);});}case 8U: _LL19: _tmp5A3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp575)->f3;_LL1A:
# 3498
 _tmp5A4=_tmp5A3;goto _LL1C;case 5U: _LL1B: _tmp5A4=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp575)->f1;_LL1C: {
# 3500
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp5A4 != 0;(_tmp5A4=_tmp5A4->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp586=(struct Cyc_Absyn_Pat*)_tmp5A4->hd;
if(_tmp586->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp587=(void*)_check_null(_tmp586->topt);
struct _dyneither_ptr*_tmp588=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpA90=({struct Cyc_Absyn_Stmt*_tmpA8F=s;Cyc_Toc_seq_stmt_opt(_tmpA8F,({struct _RegionHandle*_tmpA8E=rgn;struct Cyc_Toc_Env**_tmpA8D=nv;struct Cyc_List_List**_tmpA8C=decls;struct Cyc_Absyn_Exp*_tmpA8B=Cyc_Toc_member_exp(path,_tmp588,0U);Cyc_Toc_extract_pattern_vars(_tmpA8E,_tmpA8D,_tmpA8C,_tmpA8B,_tmp586);}));});s=_tmpA90;});};}
# 3510
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp575)->f1 == 0){_LL1D: _LL1E:
({void*_tmp589=0U;({struct _dyneither_ptr _tmpA91=({const char*_tmp58A="unresolved aggregate pattern!";_tag_dyneither(_tmp58A,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA91,_tag_dyneither(_tmp589,sizeof(void*),0U));});});}else{_LL1F: _tmp5A6=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp575)->f1)->aggr_info;_tmp5A5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp575)->f3;_LL20: {
# 3514
struct Cyc_Absyn_Aggrdecl*_tmp58B=Cyc_Absyn_get_known_aggrdecl(_tmp5A6);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp5A5 != 0;_tmp5A5=_tmp5A5->tl){
struct _tuple37*_tmp58C=(struct _tuple37*)_tmp5A5->hd;
struct Cyc_Absyn_Pat*_tmp58D=(*_tmp58C).f2;
if(_tmp58D->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp58E=Cyc_Absyn_designatorlist_to_fieldname((*_tmp58C).f1);
void*_tmp58F=(void*)_check_null(_tmp58D->topt);
void*_tmp590=Cyc_Toc_typ_to_c_array(_tmp58F);
struct Cyc_Absyn_Exp*_tmp591=Cyc_Toc_member_exp(path,_tmp58E,0U);
# 3526
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp58B->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpA92=Cyc_Toc_member_exp(_tmp591,Cyc_Toc_val_sp,0U);_tmp591=_tmpA92;});{
void*_tmp592=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp58B->impl))->fields,_tmp58E)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp592))
({struct Cyc_Absyn_Exp*_tmpA93=Cyc_Toc_cast_it(_tmp590,_tmp591);_tmp591=_tmpA93;});else{
if(!Cyc_Toc_is_array_type(_tmp592) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp592)))
# 3533
({struct Cyc_Absyn_Exp*_tmpA95=Cyc_Absyn_deref_exp(({void*_tmpA94=Cyc_Absyn_cstar_typ(_tmp590,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA94,
Cyc_Absyn_address_exp(_tmp591,0U));}),0U);
# 3533
_tmp591=_tmpA95;});}
# 3536
({struct Cyc_Absyn_Stmt*_tmpA97=({struct Cyc_Absyn_Stmt*_tmpA96=s;Cyc_Toc_seq_stmt_opt(_tmpA96,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp591,_tmp58D));});s=_tmpA97;});};};}
# 3538
return s;}}case 15U: _LL23: _LL24:
# 3543
({void*_tmp593=0U;({struct _dyneither_ptr _tmpA98=({const char*_tmp594="unknownid pat";_tag_dyneither(_tmp594,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA98,_tag_dyneither(_tmp593,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp595=0U;({struct _dyneither_ptr _tmpA99=({const char*_tmp596="unknowncall pat";_tag_dyneither(_tmp596,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA99,_tag_dyneither(_tmp595,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp597=0U;({struct _dyneither_ptr _tmpA9A=({const char*_tmp598="exp pat";_tag_dyneither(_tmp598,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA9A,_tag_dyneither(_tmp597,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3554
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3562
void*_tmp5A7=dopt;struct Cyc_List_List*_tmp5E5;struct Cyc_List_List*_tmp5E4;void*_tmp5E3;struct Cyc_Tcpat_Rhs*_tmp5E2;if(_tmp5A7 == 0){_LL1: _LL2:
# 3564
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp5A7)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp5E2=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp5A7)->f1;_LL6:
# 3569
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp5A8=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp5A9=_tmp5A8;int*_tmp5B0;struct _dyneither_ptr*_tmp5AF;struct _dyneither_ptr*_tmp5AE;struct Cyc_Absyn_Switch_clause*_tmp5AD;_LLA: _tmp5B0=(int*)& _tmp5A9->f1;_tmp5AF=_tmp5A9->f2;_tmp5AE=_tmp5A9->f3;_tmp5AD=_tmp5A9->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp5AA=_tmp5AD->body;
if(_tmp5AA == _tmp5E2->rhs){
# 3574
if(*_tmp5B0)
return Cyc_Absyn_goto_stmt(_tmp5AF,0U);
*_tmp5B0=1;{
# 3578
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA9D=rgn;struct Cyc_List_List**_tmpA9C=decls;struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA9D,& nv,_tmpA9C,_tmpA9B,_tmp5AD->pattern);});
# 3581
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp5AE,_tmp5AD->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpA9E=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpA9E;});
({struct Cyc_Absyn_Stmt*_tmpA9F=Cyc_Absyn_label_stmt(_tmp5AF,res,0U);res=_tmpA9F;});
({struct Cyc_List_List*_tmpAA1=({struct Cyc_List_List*_tmp5AC=_region_malloc(rgn,sizeof(*_tmp5AC));(({struct _tuple38*_tmpAA0=({struct _tuple38*_tmp5AB=_region_malloc(rgn,sizeof(*_tmp5AB));((((*_tmp5AB).f1=nv,(*_tmp5AB).f2=_tmp5AE)),(*_tmp5AB).f3=_tmp5AA);_tmp5AB;});(*_tmp5AC).hd=_tmpAA0;}),(*_tmp5AC).tl=*bodies);_tmp5AC;});*bodies=_tmpAA1;});
return res;};}};}
# 3589
({void*_tmp5B1=0U;({struct _dyneither_ptr _tmpAA2=({const char*_tmp5B2="couldn't find rhs!";_tag_dyneither(_tmp5B2,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA2,_tag_dyneither(_tmp5B1,sizeof(void*),0U));});});default: _LL7: _tmp5E5=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A7)->f1;_tmp5E4=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A7)->f2;_tmp5E3=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A7)->f3;_LL8: {
# 3592
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5E3,lscs,v);
# 3594
struct Cyc_Absyn_Exp*_tmp5B3=({struct Cyc_List_List*_tmpAA3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5E5);Cyc_Toc_compile_path(_tmpAA3,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp5B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5E4);
# 3597
struct _tuple36 _tmp5B5=Cyc_Toc_admits_switch(_tmp5B4);struct _tuple36 _tmp5B6=_tmp5B5;int _tmp5E1;void*_tmp5E0;_LLD: _tmp5E1=_tmp5B6.f1;_tmp5E0=_tmp5B6.f2;_LLE:;
if(_tmp5E1 > 1){
# 3601
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));
(({struct Cyc_Absyn_Switch_clause*_tmpAA5=({struct Cyc_Absyn_Switch_clause*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));(((((((({struct Cyc_Absyn_Pat*_tmpAA4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);(*_tmp5C6).pattern=_tmpAA4;}),(*_tmp5C6).pat_vars=0)),(*_tmp5C6).where_clause=0)),(*_tmp5C6).body=res)),(*_tmp5C6).loc=0U);_tmp5C6;});(*_tmp5C7).hd=_tmpAA5;}),(*_tmp5C7).tl=0);_tmp5C7;});
# 3604
for(0;_tmp5B4 != 0;_tmp5B4=_tmp5B4->tl){
struct _tuple33 _tmp5B7=*((struct _tuple33*)_tmp5B4->hd);struct _tuple33 _tmp5B8=_tmp5B7;void*_tmp5BE;void*_tmp5BD;_LL10: _tmp5BE=_tmp5B8.f1;_tmp5BD=_tmp5B8.f2;_LL11:;{
# 3607
struct Cyc_Absyn_Pat*_tmp5B9=Cyc_Toc_compile_pat_test_as_case(_tmp5BE);
# 3609
struct Cyc_Absyn_Stmt*_tmp5BA=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5BD,lscs,v);
# 3611
({struct Cyc_List_List*_tmpAA7=({struct Cyc_List_List*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));(({struct Cyc_Absyn_Switch_clause*_tmpAA6=({struct Cyc_Absyn_Switch_clause*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));((((((((*_tmp5BB).pattern=_tmp5B9,(*_tmp5BB).pat_vars=0)),(*_tmp5BB).where_clause=0)),(*_tmp5BB).body=_tmp5BA)),(*_tmp5BB).loc=0U);_tmp5BB;});(*_tmp5BC).hd=_tmpAA6;}),(*_tmp5BC).tl=new_lscs);_tmp5BC;});new_lscs=_tmpAA7;});};}
# 3613
{void*_tmp5BF=_tmp5E0;struct _dyneither_ptr*_tmp5C4;switch(*((int*)_tmp5BF)){case 1U: _LL13: _LL14:
# 3616
 LOOP1: {
void*_tmp5C0=_tmp5B3->r;void*_tmp5C1=_tmp5C0;struct Cyc_Absyn_Exp*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C2;switch(*((int*)_tmp5C1)){case 14U: _LL1C: _tmp5C2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C1)->f2;_LL1D:
 _tmp5B3=_tmp5C2;goto LOOP1;case 20U: _LL1E: _tmp5C3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5C1)->f1;_LL1F:
 _tmp5B3=_tmp5C3;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3622
({struct Cyc_Absyn_Exp*_tmpAA9=Cyc_Absyn_deref_exp(({void*_tmpAA8=Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpAA8,_tmp5B3);}),0U);_tmp5B3=_tmpAA9;});goto _LL12;case 3U: _LL15: _tmp5C4=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp5BF)->f1;_LL16:
# 3625
({struct Cyc_Absyn_Exp*_tmpAAB=({struct Cyc_Absyn_Exp*_tmpAAA=Cyc_Toc_member_exp(_tmp5B3,_tmp5C4,0U);Cyc_Toc_member_exp(_tmpAAA,Cyc_Toc_tag_sp,0U);});_tmp5B3=_tmpAAB;});goto _LL12;case 2U: _LL17: _LL18:
# 3627
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3630
({struct Cyc_Absyn_Stmt*_tmpAAC=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));((((((*_tmp5C5).tag=10U,(*_tmp5C5).f1=_tmp5B3)),(*_tmp5C5).f2=new_lscs)),(*_tmp5C5).f3=0);_tmp5C5;}),0U);res=_tmpAAC;});}else{
# 3634
void*_tmp5C8=_tmp5E0;struct Cyc_Absyn_Exp*_tmp5DF;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5C8)->tag == 2U){_LL23: _tmp5DF=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5C8)->f1;_LL24: {
# 3636
struct Cyc_List_List*_tmp5C9=_tmp5B4;void*_tmp5D8;struct Cyc_Tcpat_Rhs*_tmp5D7;if(_tmp5C9 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)((struct Cyc_List_List*)_tmp5C9)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5C9)->tl == 0){_LL28: _tmp5D8=((struct _tuple33*)_tmp5C9->hd)->f1;_tmp5D7=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)_tmp5C9->hd)->f2)->f1;_LL29:
# 3642
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp5CA=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp5CB=_tmp5CA;int*_tmp5D2;struct _dyneither_ptr*_tmp5D1;struct _dyneither_ptr*_tmp5D0;struct Cyc_Absyn_Switch_clause*_tmp5CF;_LL2D: _tmp5D2=(int*)& _tmp5CB->f1;_tmp5D1=_tmp5CB->f2;_tmp5D0=_tmp5CB->f3;_tmp5CF=_tmp5CB->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp5CC=_tmp5CF->body;
if(_tmp5CC == _tmp5D7->rhs){
# 3647
if(*_tmp5D2)
return Cyc_Absyn_goto_stmt(_tmp5D1,0U);
*_tmp5D2=1;{
# 3651
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAAF=rgn;struct Cyc_List_List**_tmpAAE=decls;struct Cyc_Absyn_Exp*_tmpAAD=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAAF,& nv,_tmpAAE,_tmpAAD,_tmp5CF->pattern);});
# 3654
Cyc_Toc_exp_to_c(nv,_tmp5DF);{
# 3657
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp5D0,_tmp5CF->body,0U);
({struct Cyc_Absyn_Stmt*_tmpAB0=Cyc_Absyn_ifthenelse_stmt(_tmp5DF,r,res,0U);r=_tmpAB0;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAB1=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpAB1;});
({struct Cyc_Absyn_Stmt*_tmpAB2=Cyc_Absyn_label_stmt(_tmp5D1,r,0U);r=_tmpAB2;});
({struct Cyc_List_List*_tmpAB4=({struct Cyc_List_List*_tmp5CE=_region_malloc(rgn,sizeof(*_tmp5CE));(({struct _tuple38*_tmpAB3=({struct _tuple38*_tmp5CD=_region_malloc(rgn,sizeof(*_tmp5CD));((((*_tmp5CD).f1=nv,(*_tmp5CD).f2=_tmp5D0)),(*_tmp5CD).f3=_tmp5CC);_tmp5CD;});(*_tmp5CE).hd=_tmpAB3;}),(*_tmp5CE).tl=*bodies);_tmp5CE;});*bodies=_tmpAB4;});
return r;};};}};}
# 3666
({void*_tmp5D3=0U;({struct _dyneither_ptr _tmpAB5=({const char*_tmp5D4="couldn't find rhs!";_tag_dyneither(_tmp5D4,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB5,_tag_dyneither(_tmp5D3,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp5D5=0U;({struct _dyneither_ptr _tmpAB6=({const char*_tmp5D6="bad where clause in match compiler";_tag_dyneither(_tmp5D6,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB6,_tag_dyneither(_tmp5D5,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3671
 for(0;_tmp5B4 != 0;_tmp5B4=_tmp5B4->tl){
struct _tuple33 _tmp5D9=*((struct _tuple33*)_tmp5B4->hd);struct _tuple33 _tmp5DA=_tmp5D9;void*_tmp5DE;void*_tmp5DD;_LL30: _tmp5DE=_tmp5DA.f1;_tmp5DD=_tmp5DA.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp5DB=Cyc_Toc_compile_pat_test(_tmp5B3,_tmp5DE);
struct Cyc_Absyn_Stmt*_tmp5DC=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5DD,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpAB7=Cyc_Absyn_ifthenelse_stmt(_tmp5DB,_tmp5DC,res,0U);res=_tmpAB7;});};}}_LL22:;}
# 3679
return res;}}}_LL0:;}
# 3689
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3691
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp5E6=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp5E7=_tmp5E6;struct Cyc_Toc_Env**_tmp5E9;struct Cyc_Absyn_Stmt*_tmp5E8;_LL1: _tmp5E9=(struct Cyc_Toc_Env**)& _tmp5E7->f1;_tmp5E8=_tmp5E7->f3;_LL2:;
if(_tmp5E8 == s)return _tmp5E9;}
# 3697
return 0;}
# 3701
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3704
void*_tmp5EA=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3707
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5EB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5EB;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5EC=Cyc_Toc_share_env(rgn,nv);
# 3714
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3719
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp5EC,& mydecls,& mybodies,dopt,lscs,v);
# 3727
{struct Cyc_List_List*_tmp5ED=lscs;for(0;_tmp5ED != 0;_tmp5ED=_tmp5ED->tl){
struct _tuple35*_tmp5EE=(struct _tuple35*)_tmp5ED->hd;struct _tuple35*_tmp5EF=_tmp5EE;struct _dyneither_ptr*_tmp5F7;struct Cyc_Absyn_Switch_clause*_tmp5F6;_LL1: _tmp5F7=_tmp5EF->f3;_tmp5F6=_tmp5EF->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp5F6->body;
# 3731
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3734
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5F0=*envp;
# 3737
if(_tmp5ED->tl != 0){
struct _tuple35*_tmp5F1=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp5ED->tl))->hd;struct _tuple35*_tmp5F2=_tmp5F1;struct _dyneither_ptr*_tmp5F5;struct Cyc_Absyn_Switch_clause*_tmp5F4;_LL4: _tmp5F5=_tmp5F2->f3;_tmp5F4=_tmp5F2->f4;_LL5:;{
# 3741
struct Cyc_Toc_Env**_tmp5F3=Cyc_Toc_find_case_env(mybodies,_tmp5F4->body);
# 3746
if(_tmp5F3 == 0)
({struct Cyc_Toc_Env*_tmpAB8=Cyc_Toc_last_switchclause_env(rgn,_tmp5F0,end_l);Cyc_Toc_stmt_to_c(_tmpAB8,s);});else{
# 3750
struct Cyc_List_List*vs=0;
if(_tmp5F4->pat_vars != 0){
({struct Cyc_List_List*_tmpAB9=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5F4->pat_vars))->v)).f1);vs=_tmpAB9;});
({struct Cyc_List_List*_tmpABA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpABA;});}
# 3755
({struct Cyc_Toc_Env*_tmpABB=Cyc_Toc_non_last_switchclause_env(rgn,_tmp5F0,end_l,_tmp5F5,vs);Cyc_Toc_stmt_to_c(_tmpABB,s);});}};}else{
# 3759
({struct Cyc_Toc_Env*_tmpABC=Cyc_Toc_last_switchclause_env(rgn,_tmp5F0,end_l);Cyc_Toc_stmt_to_c(_tmpABC,s);});}};};}}{
# 3763
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpABE=test_tree;Cyc_Absyn_seq_stmt(_tmpABE,({struct _dyneither_ptr*_tmpABD=end_l;Cyc_Absyn_label_stmt(_tmpABD,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3765
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp5F8=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpABF=Cyc_Toc_temp_var();_tmp5F8->name=_tmpABF;});
({void*_tmpAC0=Cyc_Toc_typ_to_c_array(_tmp5F8->type);_tmp5F8->type=_tmpAC0;});
({struct Cyc_Absyn_Stmt*_tmpAC2=({struct Cyc_Absyn_Decl*_tmpAC1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));((*_tmp5F9).tag=0U,(*_tmp5F9).f1=_tmp5F8);_tmp5F9;}),0U);Cyc_Absyn_decl_stmt(_tmpAC1,res,0U);});res=_tmpAC2;});}
# 3772
({void*_tmpAC6=({struct _tuple1*_tmpAC5=v;void*_tmpAC4=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpAC3=e;Cyc_Absyn_declare_stmt(_tmpAC5,_tmpAC4,_tmpAC3,res,0U);})->r;whole_s->r=_tmpAC6;});
_npop_handler(0U);return;};}
# 3711
;_pop_region(rgn);};}
# 3778
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3780
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3785
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3787
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAC8=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpAC8,({struct Cyc_List_List*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));
(({struct Cyc_Absyn_Exp*_tmpAC7=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);(*_tmp5FA).hd=_tmpAC7;}),(*_tmp5FA).tl=0);_tmp5FA;}),0U);}),0U);}
# 3791
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpACA=({struct Cyc_Absyn_Stmt*_tmpAC9=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpAC9,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpACA;});}
# 3796
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3798
while(1){
void*_tmp5FB=s->r;void*_tmp5FC=_tmp5FB;struct Cyc_Absyn_Stmt*_tmp653;struct Cyc_List_List*_tmp652;void*_tmp651;struct Cyc_Absyn_Stmt*_tmp650;struct Cyc_Absyn_Exp*_tmp64F;struct _dyneither_ptr*_tmp64E;struct Cyc_Absyn_Stmt*_tmp64D;struct Cyc_Absyn_Decl*_tmp64C;struct Cyc_Absyn_Stmt*_tmp64B;struct Cyc_List_List*_tmp64A;struct Cyc_Absyn_Switch_clause**_tmp649;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_List_List*_tmp647;void*_tmp646;struct Cyc_Absyn_Exp*_tmp645;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Exp*_tmp643;struct Cyc_Absyn_Stmt*_tmp642;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Stmt*_tmp640;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Stmt*_tmp63E;struct Cyc_Absyn_Stmt*_tmp63D;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Stmt*_tmp63B;struct Cyc_Absyn_Stmt*_tmp63A;struct Cyc_Absyn_Exp*_tmp639;switch(*((int*)_tmp5FC)){case 0U: _LL1: _LL2:
# 3801
 return;case 1U: _LL3: _tmp639=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_LL4:
# 3803
 Cyc_Toc_exp_to_c(nv,_tmp639);
return;case 2U: _LL5: _tmp63B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp63A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_LL6:
# 3806
 Cyc_Toc_stmt_to_c(nv,_tmp63B);
s=_tmp63A;
continue;case 3U: _LL7: _tmp63C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_LL8: {
# 3810
void*topt=0;
if(_tmp63C != 0){
({void*_tmpACB=Cyc_Toc_typ_to_c((void*)_check_null(_tmp63C->topt));topt=_tmpACB;});
Cyc_Toc_exp_to_c(nv,_tmp63C);}{
# 3816
int _tmp5FD=Cyc_Toc_get_npop(s);
if(_tmp5FD > 0){
if(topt != 0){
struct _tuple1*_tmp5FE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp5FF=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp5FE,0U),0U);
({void*_tmpAD0=({struct _tuple1*_tmpACF=_tmp5FE;void*_tmpACE=topt;struct Cyc_Absyn_Exp*_tmpACD=_tmp63C;Cyc_Absyn_declare_stmt(_tmpACF,_tmpACE,_tmpACD,({
struct Cyc_Absyn_Stmt*_tmpACC=Cyc_Toc_make_npop_handler(_tmp5FD);Cyc_Absyn_seq_stmt(_tmpACC,_tmp5FF,0U);}),0U);})->r;
# 3821
s->r=_tmpAD0;});}else{
# 3825
Cyc_Toc_do_npop_before(_tmp5FD,s);}}
# 3827
return;};}case 4U: _LL9: _tmp63F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp63E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_tmp63D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f3;_LLA:
# 3829
 Cyc_Toc_exp_to_c(nv,_tmp63F);
Cyc_Toc_stmt_to_c(nv,_tmp63E);
s=_tmp63D;
continue;case 5U: _LLB: _tmp641=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1).f1;_tmp640=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_LLC:
# 3834
 Cyc_Toc_exp_to_c(nv,_tmp641);{
struct _RegionHandle _tmp600=_new_region("temp");struct _RegionHandle*temp=& _tmp600;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAD1=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAD1,_tmp640);});
# 3838
_npop_handler(0U);return;
# 3835
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3840
struct Cyc_Toc_Env*_tmp601=nv;struct _dyneither_ptr**_tmp602;_LL22: _tmp602=_tmp601->break_lab;_LL23:;
if(_tmp602 != 0)
({void*_tmpAD2=Cyc_Toc_goto_stmt_r(*_tmp602);s->r=_tmpAD2;});
# 3844
({int _tmpAD3=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAD3,s);});
return;}case 7U: _LLF: _LL10: {
# 3847
struct Cyc_Toc_Env*_tmp603=nv;struct _dyneither_ptr**_tmp604;_LL25: _tmp604=_tmp603->continue_lab;_LL26:;
if(_tmp604 != 0)
({void*_tmpAD4=Cyc_Toc_goto_stmt_r(*_tmp604);s->r=_tmpAD4;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3853
({int _tmpAD5=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAD5,s);});
return;case 9U: _LL13: _tmp645=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp644=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2).f1;_tmp643=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f3).f1;_tmp642=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f4;_LL14:
# 3857
 Cyc_Toc_exp_to_c(nv,_tmp645);Cyc_Toc_exp_to_c(nv,_tmp644);Cyc_Toc_exp_to_c(nv,_tmp643);{
struct _RegionHandle _tmp605=_new_region("temp");struct _RegionHandle*temp=& _tmp605;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAD6=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAD6,_tmp642);});
# 3861
_npop_handler(0U);return;
# 3858
;_pop_region(temp);};case 10U: _LL15: _tmp648=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp647=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_tmp646=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f3;_LL16:
# 3863
 Cyc_Toc_xlate_switch(nv,s,_tmp648,_tmp647,_tmp646);
return;case 11U: _LL17: _tmp64A=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp649=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_LL18: {
# 3866
struct Cyc_Toc_Env*_tmp606=nv;struct Cyc_Toc_FallthruInfo*_tmp610;_LL28: _tmp610=_tmp606->fallthru_info;_LL29:;
if(_tmp610 == 0)
({void*_tmp607=0U;({struct _dyneither_ptr _tmpAD7=({const char*_tmp608="fallthru in unexpected place";_tag_dyneither(_tmp608,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAD7,_tag_dyneither(_tmp607,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp609=*_tmp610;struct Cyc_Toc_FallthruInfo _tmp60A=_tmp609;struct _dyneither_ptr*_tmp60F;struct Cyc_List_List*_tmp60E;_LL2B: _tmp60F=_tmp60A.label;_tmp60E=_tmp60A.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp60F,0U);
# 3872
({int _tmpAD8=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAD8,s2);});{
struct Cyc_List_List*_tmp60B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp60E);
struct Cyc_List_List*_tmp60C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp64A);
for(0;_tmp60B != 0;(_tmp60B=_tmp60B->tl,_tmp60C=_tmp60C->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp60C))->hd);
({struct Cyc_Absyn_Stmt*_tmpADB=({struct Cyc_Absyn_Stmt*_tmpADA=({struct Cyc_Absyn_Exp*_tmpAD9=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp60D=_cycalloc(sizeof(*_tmp60D));((*_tmp60D).tag=5U,(*_tmp60D).f1=(struct Cyc_Absyn_Vardecl*)_tmp60B->hd);_tmp60D;}),0U);Cyc_Absyn_assign_stmt(_tmpAD9,(struct Cyc_Absyn_Exp*)_tmp60C->hd,0U);});Cyc_Absyn_seq_stmt(_tmpADA,s2,0U);});s2=_tmpADB;});}
# 3880
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp64C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp64B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_LL1A:
# 3886
{void*_tmp611=_tmp64C->r;void*_tmp612=_tmp611;struct Cyc_Absyn_Tvar*_tmp62D;struct Cyc_Absyn_Vardecl*_tmp62C;struct Cyc_Absyn_Exp*_tmp62B;struct Cyc_Absyn_Fndecl*_tmp62A;struct Cyc_List_List*_tmp629;struct Cyc_Absyn_Pat*_tmp628;struct Cyc_Absyn_Exp*_tmp627;void*_tmp626;struct Cyc_Absyn_Vardecl*_tmp625;switch(*((int*)_tmp612)){case 0U: _LL2E: _tmp625=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp612)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp625,_tmp64B);goto _LL2D;case 2U: _LL30: _tmp628=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp612)->f1;_tmp627=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp612)->f3;_tmp626=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp612)->f4;_LL31:
# 3894
{void*_tmp613=_tmp628->r;void*_tmp614=_tmp613;struct Cyc_Absyn_Vardecl*_tmp616;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp614)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp614)->f2)->r)->tag == 0U){_LL3B: _tmp616=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp614)->f1;_LL3C:
# 3896
({struct _tuple1*_tmpADC=Cyc_Toc_temp_var();_tmp616->name=_tmpADC;});
_tmp616->initializer=_tmp627;
({void*_tmpADD=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp615=_cycalloc(sizeof(*_tmp615));((*_tmp615).tag=0U,(*_tmp615).f1=_tmp616);_tmp615;});_tmp64C->r=_tmpADD;});
Cyc_Toc_local_decl_to_c(nv,_tmp616,_tmp64B);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3905
({void*_tmpADE=(Cyc_Toc_letdecl_to_c(nv,_tmp628,_tmp626,(void*)_check_null(_tmp627->topt),_tmp627,_tmp64B))->r;s->r=_tmpADE;});
goto _LL3A;}_LL3A:;}
# 3908
goto _LL2D;case 3U: _LL32: _tmp629=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp612)->f1;_LL33: {
# 3912
struct Cyc_List_List*_tmp617=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp629);
if(_tmp617 == 0)
({void*_tmp618=0U;({struct _dyneither_ptr _tmpADF=({const char*_tmp619="empty Letv_d";_tag_dyneither(_tmp619,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpADF,_tag_dyneither(_tmp618,sizeof(void*),0U));});});
({void*_tmpAE0=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));((*_tmp61A).tag=0U,(*_tmp61A).f1=(struct Cyc_Absyn_Vardecl*)_tmp617->hd);_tmp61A;});_tmp64C->r=_tmpAE0;});
_tmp617=_tmp617->tl;
for(0;_tmp617 != 0;_tmp617=_tmp617->tl){
struct Cyc_Absyn_Decl*_tmp61B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));((*_tmp61C).tag=0U,(*_tmp61C).f1=(struct Cyc_Absyn_Vardecl*)_tmp617->hd);_tmp61C;}),0U);
({void*_tmpAE2=({struct Cyc_Absyn_Decl*_tmpAE1=_tmp61B;Cyc_Absyn_decl_stmt(_tmpAE1,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpAE2;});}
# 3921
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp62A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp612)->f1;_LL35:
# 3924
 Cyc_Toc_fndecl_to_c(nv,_tmp62A,0);
Cyc_Toc_stmt_to_c(nv,_tmp64B);
goto _LL2D;case 4U: _LL36: _tmp62D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp612)->f1;_tmp62C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp612)->f2;_tmp62B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp612)->f3;_LL37: {
# 3928
struct Cyc_Absyn_Stmt*_tmp61D=_tmp64B;
# 3936
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp62C->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3943
Cyc_Toc_stmt_to_c(nv,_tmp61D);
if(Cyc_Absyn_no_regions)
({void*_tmpAE6=({struct _tuple1*_tmpAE5=x_var;void*_tmpAE4=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpAE3=
Cyc_Absyn_uint_exp(0U,0U);
# 3945
Cyc_Absyn_declare_stmt(_tmpAE5,_tmpAE4,_tmpAE3,_tmp61D,0U);})->r;s->r=_tmpAE6;});else{
# 3947
if((unsigned int)_tmp62B){
Cyc_Toc_exp_to_c(nv,_tmp62B);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpAE8=_tmp62B;Cyc_Absyn_aggrarrow_exp(_tmpAE8,({struct _dyneither_ptr*_tmp61F=_cycalloc(sizeof(*_tmp61F));({struct _dyneither_ptr _tmpAE7=({const char*_tmp61E="h";_tag_dyneither(_tmp61E,sizeof(char),2U);});*_tmp61F=_tmpAE7;});_tmp61F;}),0U);}),0U);
({void*_tmpAE9=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp61D,0U))->r;s->r=_tmpAE9;});};}else{
# 3952
({void*_tmpAF6=({
struct _tuple1*_tmpAF5=rh_var;void*_tmpAF4=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpAF3=({
struct Cyc_Absyn_Exp*_tmpAEB=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpAEB,({struct Cyc_List_List*_tmp620=_cycalloc(sizeof(*_tmp620));
(({struct Cyc_Absyn_Exp*_tmpAEA=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);(*_tmp620).hd=_tmpAEA;}),(*_tmp620).tl=0);_tmp620;}),0U);});
# 3953
Cyc_Absyn_declare_stmt(_tmpAF5,_tmpAF4,_tmpAF3,({
# 3957
struct _tuple1*_tmpAF2=x_var;void*_tmpAF1=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpAF0=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpAF2,_tmpAF1,_tmpAF0,({
struct Cyc_Absyn_Stmt*_tmpAEF=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAEC=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpAEC,({struct Cyc_Absyn_Exp*_tmp621[1U];_tmp621[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp621,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpAEF,({
struct Cyc_Absyn_Stmt*_tmpAEE=_tmp61D;Cyc_Absyn_seq_stmt(_tmpAEE,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAED=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpAED,({struct Cyc_Absyn_Exp*_tmp622[1U];_tmp622[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp622,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3952
s->r=_tmpAF6;});}}
# 3963
return;}default: _LL38: _LL39:
({void*_tmp623=0U;({struct _dyneither_ptr _tmpAF7=({const char*_tmp624="bad nested declaration within function";_tag_dyneither(_tmp624,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF7,_tag_dyneither(_tmp623,sizeof(void*),0U));});});}_LL2D:;}
# 3966
return;case 13U: _LL1B: _tmp64E=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp64D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_LL1C:
# 3968
 s=_tmp64D;continue;case 14U: _LL1D: _tmp650=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp64F=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2).f1;_LL1E: {
# 3970
struct _RegionHandle _tmp62E=_new_region("temp");struct _RegionHandle*temp=& _tmp62E;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAF8=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAF8,_tmp650);});
Cyc_Toc_exp_to_c(nv,_tmp64F);
# 3974
_npop_handler(0U);return;
# 3970
;_pop_region(temp);}default: _LL1F: _tmp653=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f1;_tmp652=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f2;_tmp651=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5FC)->f3;_LL20: {
# 3988 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 3998
e_exp->topt=e_typ;{
struct _RegionHandle _tmp62F=_new_region("temp");struct _RegionHandle*temp=& _tmp62F;_push_region(temp);
# 4001
Cyc_Toc_stmt_to_c(nv,_tmp653);{
struct Cyc_Absyn_Stmt*_tmp630=({struct Cyc_Absyn_Stmt*_tmpAF9=_tmp653;Cyc_Absyn_seq_stmt(_tmpAF9,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 4006
struct Cyc_Absyn_Stmt*_tmp631=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp638=_cycalloc(sizeof(*_tmp638));((((((*_tmp638).tag=10U,(*_tmp638).f1=e_exp)),(*_tmp638).f2=_tmp652)),(*_tmp638).f3=_tmp651);_tmp638;}),0U);
# 4008
Cyc_Toc_stmt_to_c(nv,_tmp631);{
# 4011
struct Cyc_Absyn_Exp*_tmp632=({
struct Cyc_Absyn_Exp*_tmpAFB=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpAFB,({struct Cyc_List_List*_tmp637=_cycalloc(sizeof(*_tmp637));
(({struct Cyc_Absyn_Exp*_tmpAFA=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);(*_tmp637).hd=_tmpAFA;}),(*_tmp637).tl=0);_tmp637;}),0U);});
# 4015
struct Cyc_Absyn_Stmt*_tmp633=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAFD=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpAFD,({struct Cyc_List_List*_tmp636=_cycalloc(sizeof(*_tmp636));
(({struct Cyc_Absyn_Exp*_tmpAFC=Cyc_Absyn_address_exp(h_exp,0U);(*_tmp636).hd=_tmpAFC;}),(*_tmp636).tl=0);_tmp636;}),0U);}),0U);
# 4019
struct Cyc_Absyn_Exp*_tmp634=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp635=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpB0C=({
struct _tuple1*_tmpB0B=h_var;void*_tmpB0A=h_typ;Cyc_Absyn_declare_stmt(_tmpB0B,_tmpB0A,0,({
struct Cyc_Absyn_Stmt*_tmpB09=_tmp633;Cyc_Absyn_seq_stmt(_tmpB09,({
struct _tuple1*_tmpB08=was_thrown_var;void*_tmpB07=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpB06=_tmp634;Cyc_Absyn_declare_stmt(_tmpB08,_tmpB07,_tmpB06,({
struct Cyc_Absyn_Stmt*_tmpB05=({struct Cyc_Absyn_Exp*_tmpAFF=_tmp632;struct Cyc_Absyn_Stmt*_tmpAFE=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp635,0U);
# 4025
Cyc_Absyn_ifthenelse_stmt(_tmpAFF,_tmpAFE,
# 4027
Cyc_Toc_skip_stmt_dl(),0U);});
# 4025
Cyc_Absyn_seq_stmt(_tmpB05,({
# 4028
struct Cyc_Absyn_Exp*_tmpB04=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpB03=_tmp630;Cyc_Absyn_ifthenelse_stmt(_tmpB04,_tmpB03,({
# 4030
struct _tuple1*_tmpB02=e_var;void*_tmpB01=e_typ;struct Cyc_Absyn_Exp*_tmpB00=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 4030
Cyc_Absyn_declare_stmt(_tmpB02,_tmpB01,_tmpB00,_tmp631,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 4021
s->r=_tmpB0C;});};};
# 4034
_npop_handler(0U);return;
# 3999
;_pop_region(temp);};}}_LL0:;}}
# 4043
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpB0D=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpB0D;});{
struct _RegionHandle _tmp654=_new_region("frgn");struct _RegionHandle*frgn=& _tmp654;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp655=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp656=f->args;for(0;_tmp656 != 0;_tmp656=_tmp656->tl){
struct _tuple1*_tmp657=({struct _tuple1*_tmp658=_cycalloc(sizeof(*_tmp658));((*_tmp658).f1=Cyc_Absyn_Loc_n,(*_tmp658).f2=(*((struct _tuple9*)_tmp656->hd)).f1);_tmp658;});
({void*_tmpB0E=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp656->hd)).f3);(*((struct _tuple9*)_tmp656->hd)).f3=_tmpB0E;});}}
# 4059
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 4063
({struct Cyc_Hashtable_Table**_tmpB10=({struct Cyc_Hashtable_Table**_tmp659=_cycalloc(sizeof(*_tmp659));({struct Cyc_Hashtable_Table*_tmpB0F=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp659=_tmpB0F;});_tmp659;});Cyc_Toc_fn_pop_table=_tmpB10;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp65A=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp65B=_tmp65A;struct _dyneither_ptr*_tmp664;struct Cyc_Absyn_Tqual _tmp663;void*_tmp662;int _tmp661;_LL1: _tmp664=_tmp65B.name;_tmp663=_tmp65B.tq;_tmp662=_tmp65B.type;_tmp661=_tmp65B.inject;_LL2:;{
void*_tmp65C=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp662,(void*)& Cyc_Absyn_HeapRgn_val,_tmp663,Cyc_Absyn_false_conref));
struct _tuple1*_tmp65D=({struct _tuple1*_tmp660=_cycalloc(sizeof(*_tmp660));((*_tmp660).f1=Cyc_Absyn_Loc_n,(*_tmp660).f2=(struct _dyneither_ptr*)_check_null(_tmp664));_tmp660;});
({struct Cyc_List_List*_tmpB13=({struct Cyc_List_List*_tmpB12=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB12,({struct Cyc_List_List*_tmp65F=_cycalloc(sizeof(*_tmp65F));(({struct _tuple9*_tmpB11=({struct _tuple9*_tmp65E=_cycalloc(sizeof(*_tmp65E));((((*_tmp65E).f1=_tmp664,(*_tmp65E).f2=_tmp663)),(*_tmp65E).f3=_tmp65C);_tmp65E;});(*_tmp65F).hd=_tmpB11;}),(*_tmp65F).tl=0);_tmp65F;}));});f->args=_tmpB13;});
f->cyc_varargs=0;};}
# 4072
{struct Cyc_List_List*_tmp665=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp665 != 0;_tmp665=_tmp665->tl){
({void*_tmpB14=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp665->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp665->hd)->type=_tmpB14;});}}
# 4075
({struct Cyc_Toc_Env*_tmpB15=Cyc_Toc_clear_toplevel(frgn,_tmp655);Cyc_Toc_stmt_to_c(_tmpB15,f->body);});}
# 4052
;_pop_region(frgn);};}
# 4079
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp666=s;switch(_tmp666){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple40{struct Cyc_Toc_TocState*f1;struct _tuple39*f2;};
# 4096 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 4099
struct _tuple40 _tmp667=*env;struct _tuple40 _tmp668=_tmp667;struct Cyc_Toc_TocState*_tmp69D;struct Cyc_Absyn_Aggrdecl*_tmp69C;int _tmp69B;_LL1: _tmp69D=_tmp668.f1;_tmp69C=(_tmp668.f2)->f1;_tmp69B=(_tmp668.f2)->f2;_LL2:;{
struct _tuple1*_tmp669=_tmp69C->name;
struct Cyc_Toc_TocState _tmp66A=*_tmp69D;struct Cyc_Toc_TocState _tmp66B=_tmp66A;struct Cyc_Dict_Dict*_tmp69A;_LL4: _tmp69A=_tmp66B.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple19**_tmp66C=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp69A,_tmp669);
if(_tmp66C == 0){
seen_defn_before=0;{
struct _tuple19*v;
if(_tmp69C->kind == Cyc_Absyn_StructA)
({struct _tuple19*_tmpB17=({struct _tuple19*_tmp66D=_region_malloc(d,sizeof(*_tmp66D));((*_tmp66D).f1=_tmp69C,({void*_tmpB16=Cyc_Absyn_strctq(_tmp669);(*_tmp66D).f2=_tmpB16;}));_tmp66D;});v=_tmpB17;});else{
# 4110
({struct _tuple19*_tmpB19=({struct _tuple19*_tmp66E=_region_malloc(d,sizeof(*_tmp66E));((*_tmp66E).f1=_tmp69C,({void*_tmpB18=Cyc_Absyn_unionq_typ(_tmp669);(*_tmp66E).f2=_tmpB18;}));_tmp66E;});v=_tmpB19;});}
({struct Cyc_Dict_Dict _tmpB1A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*_tmp69A,_tmp669,v);*_tmp69A=_tmpB1A;});};}else{
# 4113
struct _tuple19*_tmp66F=*_tmp66C;struct _tuple19*_tmp670=_tmp66F;struct Cyc_Absyn_Aggrdecl*_tmp673;void*_tmp672;_LL7: _tmp673=_tmp670->f1;_tmp672=_tmp670->f2;_LL8:;
if(_tmp673->impl == 0){
({struct Cyc_Dict_Dict _tmpB1D=({struct Cyc_Dict_Dict _tmpB1C=*_tmp69A;struct _tuple1*_tmpB1B=_tmp669;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpB1C,_tmpB1B,({struct _tuple19*_tmp671=_region_malloc(d,sizeof(*_tmp671));((*_tmp671).f1=_tmp69C,(*_tmp671).f2=_tmp672);_tmp671;}));});*_tmp69A=_tmpB1D;});
seen_defn_before=0;}else{
# 4118
seen_defn_before=1;}}{
# 4120
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp699=_cycalloc(sizeof(*_tmp699));((((((((((((*_tmp699).kind=_tmp69C->kind,(*_tmp699).sc=Cyc_Absyn_Public)),(*_tmp699).name=_tmp69C->name)),(*_tmp699).tvs=0)),(*_tmp699).impl=0)),(*_tmp699).expected_mem_kind=0)),(*_tmp699).attributes=_tmp69C->attributes);_tmp699;});
# 4127
if(_tmp69C->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpB1E=({struct Cyc_Absyn_AggrdeclImpl*_tmp674=_cycalloc(sizeof(*_tmp674));((((((*_tmp674).exist_vars=0,(*_tmp674).rgn_po=0)),(*_tmp674).fields=0)),(*_tmp674).tagged=0);_tmp674;});new_ad->impl=_tmpB1E;});{
# 4132
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp675=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69C->impl))->fields;for(0;_tmp675 != 0;_tmp675=_tmp675->tl){
# 4136
struct Cyc_Absyn_Aggrfield*_tmp676=(struct Cyc_Absyn_Aggrfield*)_tmp675->hd;
void*_tmp677=_tmp676->type;
struct Cyc_List_List*_tmp678=_tmp676->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp677)) && (
_tmp69C->kind == Cyc_Absyn_StructA  && _tmp675->tl == 0  || _tmp69C->kind == Cyc_Absyn_UnionA)){
# 4150 "toc.cyc"
void*_tmp679=Cyc_Tcutil_compress(_tmp677);void*_tmp67A=_tmp679;void*_tmp682;struct Cyc_Absyn_Tqual _tmp681;union Cyc_Absyn_Constraint*_tmp680;unsigned int _tmp67F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67A)->tag == 8U){_LLA: _tmp682=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67A)->f1).elt_type;_tmp681=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67A)->f1).tq;_tmp680=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67A)->f1).zero_term;_tmp67F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67A)->f1).zt_loc;_LLB:
# 4153
({void*_tmpB20=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp67B=_cycalloc(sizeof(*_tmp67B));((*_tmp67B).tag=8U,((((((((((*_tmp67B).f1).elt_type=_tmp682,((*_tmp67B).f1).tq=_tmp681)),({struct Cyc_Absyn_Exp*_tmpB1F=Cyc_Absyn_uint_exp(0U,0U);((*_tmp67B).f1).num_elts=_tmpB1F;}))),((*_tmp67B).f1).zero_term=_tmp680)),((*_tmp67B).f1).zt_loc=_tmp67F)));_tmp67B;});_tmp677=_tmpB20;});
goto _LL9;}else{_LLC: _LLD:
# 4156
({struct Cyc_List_List*_tmpB22=({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*_tmp67D));(({void*_tmpB21=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp67C=_cycalloc(sizeof(*_tmp67C));((*_tmp67C).tag=6U,(*_tmp67C).f1=0);_tmp67C;});(*_tmp67D).hd=_tmpB21;}),(*_tmp67D).tl=_tmp678);_tmp67D;});_tmp678=_tmpB22;});
({void*_tmpB27=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp67E=_cycalloc(sizeof(*_tmp67E));((*_tmp67E).tag=8U,((((((((({void*_tmpB26=Cyc_Absyn_void_star_typ();((*_tmp67E).f1).elt_type=_tmpB26;}),({
struct Cyc_Absyn_Tqual _tmpB25=Cyc_Absyn_empty_tqual(0U);((*_tmp67E).f1).tq=_tmpB25;}))),({
struct Cyc_Absyn_Exp*_tmpB24=Cyc_Absyn_uint_exp(0U,0U);((*_tmp67E).f1).num_elts=_tmpB24;}))),({
union Cyc_Absyn_Constraint*_tmpB23=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);((*_tmp67E).f1).zero_term=_tmpB23;}))),((*_tmp67E).f1).zt_loc=0U)));_tmp67E;});
# 4157
_tmp677=_tmpB27;});}_LL9:;}{
# 4163
struct Cyc_Absyn_Aggrfield*_tmp683=({struct Cyc_Absyn_Aggrfield*_tmp695=_cycalloc(sizeof(*_tmp695));((((((((((*_tmp695).name=_tmp676->name,(*_tmp695).tq=Cyc_Toc_mt_tq)),({
# 4165
void*_tmpB28=Cyc_Toc_typ_to_c(_tmp677);(*_tmp695).type=_tmpB28;}))),(*_tmp695).width=_tmp676->width)),(*_tmp695).attributes=_tmp678)),(*_tmp695).requires_clause=0);_tmp695;});
# 4173
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69C->impl))->tagged){
void*_tmp684=_tmp683->type;
struct _dyneither_ptr*_tmp685=_tmp683->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp693;_tmp693.tag=0U;_tmp693.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp685);({struct Cyc_String_pa_PrintArg_struct _tmp692;_tmp692.tag=0U;_tmp692.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp69C->name).f2);({void*_tmp690[2U];(_tmp690[0]=& _tmp692,_tmp690[1]=& _tmp693);({struct _dyneither_ptr _tmpB29=({const char*_tmp691="_union_%s_%s";_tag_dyneither(_tmp691,sizeof(char),13U);});Cyc_aprintf(_tmpB29,_tag_dyneither(_tmp690,sizeof(void*),2U));});});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp68F=_cycalloc(sizeof(*_tmp68F));*_tmp68F=s;_tmp68F;});
struct Cyc_Absyn_Aggrfield*_tmp686=({struct Cyc_Absyn_Aggrfield*_tmp68E=_cycalloc(sizeof(*_tmp68E));((((((((((*_tmp68E).name=Cyc_Toc_val_sp,(*_tmp68E).tq=Cyc_Toc_mt_tq)),(*_tmp68E).type=_tmp684)),(*_tmp68E).width=0)),(*_tmp68E).attributes=0)),(*_tmp68E).requires_clause=0);_tmp68E;});
struct Cyc_Absyn_Aggrfield*_tmp687=({struct Cyc_Absyn_Aggrfield*_tmp68D=_cycalloc(sizeof(*_tmp68D));((((((((((*_tmp68D).name=Cyc_Toc_tag_sp,(*_tmp68D).tq=Cyc_Toc_mt_tq)),(*_tmp68D).type=Cyc_Absyn_sint_typ)),(*_tmp68D).width=0)),(*_tmp68D).attributes=0)),(*_tmp68D).requires_clause=0);_tmp68D;});
struct Cyc_List_List*_tmp688=({struct Cyc_Absyn_Aggrfield*_tmp68C[2U];(_tmp68C[0]=_tmp687,_tmp68C[1]=_tmp686);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp68C,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp689=Cyc_Toc_make_c_struct_defn(str,_tmp688);
({struct Cyc_List_List*_tmpB2B=({struct Cyc_List_List*_tmp68B=_cycalloc(sizeof(*_tmp68B));(({struct Cyc_Absyn_Decl*_tmpB2A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp68A=_cycalloc(sizeof(*_tmp68A));((*_tmp68A).tag=5U,(*_tmp68A).f1=_tmp689);_tmp68A;}),0U);(*_tmp68B).hd=_tmpB2A;}),(*_tmp68B).tl=Cyc_Toc_result_decls);_tmp68B;});Cyc_Toc_result_decls=_tmpB2B;});
({void*_tmpB2C=Cyc_Absyn_strct(str);_tmp683->type=_tmpB2C;});}
# 4185
({struct Cyc_List_List*_tmpB2D=({struct Cyc_List_List*_tmp694=_cycalloc(sizeof(*_tmp694));((*_tmp694).hd=_tmp683,(*_tmp694).tl=new_fields);_tmp694;});new_fields=_tmpB2D;});};}}
# 4187
({struct Cyc_List_List*_tmpB2E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpB2E;});};}
# 4189
if(_tmp69B)
({struct Cyc_List_List*_tmpB31=({struct Cyc_List_List*_tmp698=_cycalloc(sizeof(*_tmp698));(({struct Cyc_Absyn_Decl*_tmpB30=({struct Cyc_Absyn_Decl*_tmp697=_cycalloc(sizeof(*_tmp697));(({void*_tmpB2F=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp696=_cycalloc(sizeof(*_tmp696));((*_tmp696).tag=5U,(*_tmp696).f1=new_ad);_tmp696;});(*_tmp697).r=_tmpB2F;}),(*_tmp697).loc=0U);_tmp697;});(*_tmp698).hd=_tmpB30;}),(*_tmp698).tl=Cyc_Toc_result_decls);_tmp698;});Cyc_Toc_result_decls=_tmpB31;});
return 0;};};};}
# 4194
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple39 p=({struct _tuple39 _tmp787;(_tmp787.f1=ad,_tmp787.f2=add_to_decls);_tmp787;});
((int(*)(struct _tuple39*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple41{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4223 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4226
struct _tuple41 _tmp69E=*env;struct _tuple41 _tmp69F=_tmp69E;struct Cyc_Set_Set**_tmp6B0;struct Cyc_Absyn_Datatypedecl*_tmp6AF;_LL1: _tmp6B0=(_tmp69F.f1)->datatypes_so_far;_tmp6AF=_tmp69F.f2;_LL2:;{
struct _tuple1*_tmp6A0=_tmp6AF->name;
if(_tmp6AF->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp6B0,_tmp6A0))
return 0;
({struct Cyc_Set_Set*_tmpB32=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp6B0,_tmp6A0);*_tmp6B0=_tmpB32;});
{struct Cyc_List_List*_tmp6A1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6AF->fields))->v;for(0;_tmp6A1 != 0;_tmp6A1=_tmp6A1->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp6A1->hd;
# 4234
struct Cyc_List_List*_tmp6A2=0;
int i=1;
{struct Cyc_List_List*_tmp6A3=f->typs;for(0;_tmp6A3 != 0;(_tmp6A3=_tmp6A3->tl,i ++)){
struct _dyneither_ptr*_tmp6A4=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp6A5=({struct Cyc_Absyn_Aggrfield*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));((((((((((*_tmp6A7).name=_tmp6A4,(*_tmp6A7).tq=(*((struct _tuple12*)_tmp6A3->hd)).f1)),({
void*_tmpB33=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp6A3->hd)).f2);(*_tmp6A7).type=_tmpB33;}))),(*_tmp6A7).width=0)),(*_tmp6A7).attributes=0)),(*_tmp6A7).requires_clause=0);_tmp6A7;});
({struct Cyc_List_List*_tmpB34=({struct Cyc_List_List*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));((*_tmp6A6).hd=_tmp6A5,(*_tmp6A6).tl=_tmp6A2);_tmp6A6;});_tmp6A2=_tmpB34;});}}
# 4242
({struct Cyc_List_List*_tmpB37=({struct Cyc_List_List*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));(({struct Cyc_Absyn_Aggrfield*_tmpB36=({struct Cyc_Absyn_Aggrfield*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));((((((((((*_tmp6A8).name=Cyc_Toc_tag_sp,(*_tmp6A8).tq=Cyc_Toc_mt_tq)),(*_tmp6A8).type=Cyc_Absyn_sint_typ)),(*_tmp6A8).width=0)),(*_tmp6A8).attributes=0)),(*_tmp6A8).requires_clause=0);_tmp6A8;});(*_tmp6A9).hd=_tmpB36;}),({
struct Cyc_List_List*_tmpB35=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6A2);(*_tmp6A9).tl=_tmpB35;}));_tmp6A9;});
# 4242
_tmp6A2=_tmpB37;});{
# 4244
struct Cyc_Absyn_Aggrdecl*_tmp6AA=({struct _dyneither_ptr*_tmpB39=({struct _dyneither_ptr*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));({struct _dyneither_ptr _tmpB38=({const char*_tmp6AD="";_tag_dyneither(_tmp6AD,sizeof(char),1U);});*_tmp6AE=_tmpB38;});_tmp6AE;});Cyc_Toc_make_c_struct_defn(_tmpB39,_tmp6A2);});
({struct _tuple1*_tmpB3A=Cyc_Toc_collapse_qvars(f->name,_tmp6AF->name);_tmp6AA->name=_tmpB3A;});
({struct Cyc_List_List*_tmpB3C=({struct Cyc_List_List*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));(({struct Cyc_Absyn_Decl*_tmpB3B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));((*_tmp6AB).tag=5U,(*_tmp6AB).f1=_tmp6AA);_tmp6AB;}),0U);(*_tmp6AC).hd=_tmpB3B;}),(*_tmp6AC).tl=Cyc_Toc_result_decls);_tmp6AC;});Cyc_Toc_result_decls=_tmpB3C;});};}}
# 4248
return 0;};}
# 4251
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4270 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4273
struct _tuple41 _tmp6B1=*env;struct _tuple41 _tmp6B2=_tmp6B1;struct Cyc_Toc_TocState*_tmp6D4;struct Cyc_Absyn_Datatypedecl*_tmp6D3;_LL1: _tmp6D4=_tmp6B2.f1;_tmp6D3=_tmp6B2.f2;_LL2:;
if(_tmp6D3->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6B3=*_tmp6D4;struct Cyc_Toc_TocState _tmp6B4=_tmp6B3;struct Cyc_Dict_Dict*_tmp6D2;_LL4: _tmp6D2=_tmp6B4.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp6B5=_tmp6D3->name;
{struct Cyc_List_List*_tmp6B6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6D3->fields))->v;for(0;_tmp6B6 != 0;_tmp6B6=_tmp6B6->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp6B6->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6B7=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp6B8=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp6B7,Cyc_Absyn_false_conref,0U);
# 4284
int*_tmp6B9=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp6D2,f->name);int*_tmp6BA=_tmp6B9;if(_tmp6BA == 0){_LL7: _LL8: {
# 4286
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpB3D=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpB3D;});{
# 4290
struct Cyc_Absyn_Vardecl*_tmp6BB=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6B8,initopt);
_tmp6BB->sc=f->sc;
({struct Cyc_List_List*_tmpB3F=({struct Cyc_List_List*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));(({struct Cyc_Absyn_Decl*_tmpB3E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));((*_tmp6BC).tag=0U,(*_tmp6BC).f1=_tmp6BB);_tmp6BC;}),0U);(*_tmp6BD).hd=_tmpB3E;}),(*_tmp6BD).tl=Cyc_Toc_result_decls);_tmp6BD;});Cyc_Toc_result_decls=_tmpB3F;});
({struct Cyc_Dict_Dict _tmpB40=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6D2,f->name,f->sc != Cyc_Absyn_Extern);
# 4293
*_tmp6D2=_tmpB40;});{
# 4295
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6BE=f->typs;for(0;_tmp6BE != 0;(_tmp6BE=_tmp6BE->tl,i ++)){
struct _dyneither_ptr*_tmp6BF=({struct _dyneither_ptr*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));({struct _dyneither_ptr _tmpB42=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6C5;_tmp6C5.tag=1U;_tmp6C5.f1=(unsigned long)i;({void*_tmp6C3[1U];_tmp6C3[0]=& _tmp6C5;({struct _dyneither_ptr _tmpB41=({const char*_tmp6C4="f%d";_tag_dyneither(_tmp6C4,sizeof(char),4U);});Cyc_aprintf(_tmpB41,_tag_dyneither(_tmp6C3,sizeof(void*),1U));});});});*_tmp6C6=_tmpB42;});_tmp6C6;});
struct Cyc_Absyn_Aggrfield*_tmp6C0=({struct Cyc_Absyn_Aggrfield*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));((((((((((*_tmp6C2).name=_tmp6BF,(*_tmp6C2).tq=(*((struct _tuple12*)_tmp6BE->hd)).f1)),({
void*_tmpB43=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp6BE->hd)).f2);(*_tmp6C2).type=_tmpB43;}))),(*_tmp6C2).width=0)),(*_tmp6C2).attributes=0)),(*_tmp6C2).requires_clause=0);_tmp6C2;});
({struct Cyc_List_List*_tmpB44=({struct Cyc_List_List*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));((*_tmp6C1).hd=_tmp6C0,(*_tmp6C1).tl=fields);_tmp6C1;});fields=_tmpB44;});}}
# 4303
({struct Cyc_List_List*_tmpB48=({struct Cyc_List_List*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));(({struct Cyc_Absyn_Aggrfield*_tmpB47=({struct Cyc_Absyn_Aggrfield*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));((((((((((*_tmp6C7).name=Cyc_Toc_tag_sp,(*_tmp6C7).tq=Cyc_Toc_mt_tq)),({
void*_tmpB46=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);(*_tmp6C7).type=_tmpB46;}))),(*_tmp6C7).width=0)),(*_tmp6C7).attributes=0)),(*_tmp6C7).requires_clause=0);_tmp6C7;});
# 4303
(*_tmp6C8).hd=_tmpB47;}),({
# 4305
struct Cyc_List_List*_tmpB45=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);(*_tmp6C8).tl=_tmpB45;}));_tmp6C8;});
# 4303
fields=_tmpB48;});{
# 4306
struct Cyc_Absyn_Aggrdecl*_tmp6C9=({struct _dyneither_ptr*_tmpB4A=({struct _dyneither_ptr*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));({struct _dyneither_ptr _tmpB49=({const char*_tmp6CC="";_tag_dyneither(_tmp6CC,sizeof(char),1U);});*_tmp6CD=_tmpB49;});_tmp6CD;});Cyc_Toc_make_c_struct_defn(_tmpB4A,fields);});
({struct _tuple1*_tmpB4B=Cyc_Toc_collapse_qvars(f->name,_tmp6D3->name);_tmp6C9->name=_tmpB4B;});
({struct Cyc_List_List*_tmpB4D=({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));(({struct Cyc_Absyn_Decl*_tmpB4C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));((*_tmp6CA).tag=5U,(*_tmp6CA).f1=_tmp6C9);_tmp6CA;}),0U);(*_tmp6CB).hd=_tmpB4C;}),(*_tmp6CB).tl=Cyc_Toc_result_decls);_tmp6CB;});Cyc_Toc_result_decls=_tmpB4D;});
goto _LL6;};};};}}else{if(*((int*)_tmp6BA)== 0){_LL9: _LLA:
# 4311
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6CE=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp6CF=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6B8,_tmp6CE);
_tmp6CF->sc=f->sc;
({struct Cyc_List_List*_tmpB4F=({struct Cyc_List_List*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));(({struct Cyc_Absyn_Decl*_tmpB4E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));((*_tmp6D0).tag=0U,(*_tmp6D0).f1=_tmp6CF);_tmp6D0;}),0U);(*_tmp6D1).hd=_tmpB4E;}),(*_tmp6D1).tl=Cyc_Toc_result_decls);_tmp6D1;});Cyc_Toc_result_decls=_tmpB4F;});
({struct Cyc_Dict_Dict _tmpB50=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6D2,f->name,1);*_tmp6D2=_tmpB50;});}
# 4318
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4322
return 0;};};}
# 4325
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4329
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4335
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpB51=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpB51;});
# 4339
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4348
struct _RegionHandle _tmp6D5=_new_region("temp");struct _RegionHandle*temp=& _tmp6D5;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6D6=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp6D6,init);}
# 4349
;_pop_region(temp);}else{
# 4353
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4356
void*_tmp6D7=Cyc_Tcutil_compress(old_typ);void*_tmp6D8=_tmp6D7;void*_tmp6DF;struct Cyc_Absyn_Exp*_tmp6DE;union Cyc_Absyn_Constraint*_tmp6DD;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->tag == 8U){_LL1: _tmp6DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->f1).elt_type;_tmp6DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->f1).num_elts;_tmp6DD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->f1).zero_term;_LL2:
# 4358
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6DD)){
if(_tmp6DE == 0)
({void*_tmp6D9=0U;({struct _dyneither_ptr _tmpB52=({const char*_tmp6DA="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp6DA,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB52,_tag_dyneither(_tmp6D9,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp6DE;
struct Cyc_Absyn_Exp*_tmp6DB=({struct Cyc_Absyn_Exp*_tmpB54=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpB54,({
struct Cyc_Absyn_Exp*_tmpB53=num_elts;Cyc_Absyn_add_exp(_tmpB53,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4365
struct Cyc_Absyn_Exp*_tmp6DC=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpB56=({struct Cyc_Absyn_Stmt*_tmpB55=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp6DB,_tmp6DC,0U),0U);Cyc_Toc_seq_stmt_r(_tmpB55,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4366
s->r=_tmpB56;});};}
# 4369
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4377
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6E0=d;struct Cyc_List_List*_tmp6E7;struct Cyc_List_List*_tmp6E6;void**_tmp6E5;struct Cyc_Tcpat_Rhs*_tmp6E4;switch(*((int*)_tmp6E0)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6E4=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6E0)->f1;_LL4:
 _tmp6E4->rhs=success;return d;default: _LL5: _tmp6E7=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E0)->f1;_tmp6E6=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E0)->f2;_tmp6E5=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E0)->f3;_LL6:
# 4382
({void*_tmpB57=Cyc_Toc_rewrite_decision(*_tmp6E5,success);*_tmp6E5=_tmpB57;});
for(0;_tmp6E6 != 0;_tmp6E6=_tmp6E6->tl){
struct _tuple33*_tmp6E1=(struct _tuple33*)_tmp6E6->hd;struct _tuple33*_tmp6E2=_tmp6E1;void**_tmp6E3;_LL8: _tmp6E3=(void**)& _tmp6E2->f2;_LL9:;
({void*_tmpB58=Cyc_Toc_rewrite_decision(*_tmp6E3,success);*_tmp6E3=_tmpB58;});}
# 4387
return d;}_LL0:;}
# 4398 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4400
struct _RegionHandle _tmp6E8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6E8;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6E9=Cyc_Toc_share_env(rgn,nv);
void*_tmp6EA=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp6E9,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4410
struct Cyc_Absyn_Stmt*_tmp6EB=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4414
({void*_tmpB59=Cyc_Toc_rewrite_decision(dopt,_tmp6EB);dopt=_tmpB59;});{
# 4416
struct Cyc_Absyn_Switch_clause*_tmp6EC=({struct Cyc_Absyn_Switch_clause*_tmp6F8=_cycalloc(sizeof(*_tmp6F8));((((((((*_tmp6F8).pattern=p,(*_tmp6F8).pat_vars=0)),(*_tmp6F8).where_clause=0)),(*_tmp6F8).body=_tmp6EB)),(*_tmp6F8).loc=0U);_tmp6F8;});
struct Cyc_List_List*_tmp6ED=({struct _RegionHandle*_tmpB5C=rgn;struct _RegionHandle*_tmpB5B=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpB5C,Cyc_Toc_gen_labels,_tmpB5B,({struct Cyc_Absyn_Switch_clause*_tmp6F7[1U];_tmp6F7[0]=_tmp6EC;({struct _RegionHandle*_tmpB5A=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpB5A,_tag_dyneither(_tmp6F7,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4419
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4423
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp6E9,& mydecls,& mybodies,dopt,_tmp6ED,v);
# 4426
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp6EE=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp6EF=_tmp6EE;struct Cyc_Toc_Env*_tmp6F1;struct Cyc_Absyn_Stmt*_tmp6F0;_LL1: _tmp6F1=_tmp6EF->f1;_tmp6F0=_tmp6EF->f3;_LL2:;
if(_tmp6F0 == _tmp6EB){senv=_tmp6F1;goto FOUND_ENV;}}
# 4431
({void*_tmp6F2=0U;({struct _dyneither_ptr _tmpB5D=({const char*_tmp6F3="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp6F3,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB5D,_tag_dyneither(_tmp6F2,sizeof(void*),0U));});});
FOUND_ENV:
# 4435
 Cyc_Toc_stmt_to_c(senv,s);{
# 4437
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4439
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp6F4=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpB5E=Cyc_Toc_temp_var();_tmp6F4->name=_tmpB5E;});
({void*_tmpB5F=Cyc_Toc_typ_to_c_array(_tmp6F4->type);_tmp6F4->type=_tmpB5F;});
({struct Cyc_Absyn_Stmt*_tmpB61=({struct Cyc_Absyn_Decl*_tmpB60=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));((*_tmp6F5).tag=0U,(*_tmp6F5).f1=_tmp6F4);_tmp6F5;}),0U);Cyc_Absyn_decl_stmt(_tmpB60,res,0U);});res=_tmpB61;});}
# 4446
({struct Cyc_Absyn_Stmt*_tmpB65=({struct _tuple1*_tmpB64=v;void*_tmpB63=Cyc_Toc_typ_to_c(_tmp6EA);struct Cyc_Absyn_Exp*_tmpB62=e;Cyc_Absyn_declare_stmt(_tmpB64,_tmpB63,_tmpB62,res,0U);});res=_tmpB65;});{
struct Cyc_Absyn_Stmt*_tmp6F6=res;_npop_handler(0U);return _tmp6F6;};};};};}
# 4401
;_pop_region(rgn);}
# 4454
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6F9=e->r;void*_tmp6FA=_tmp6F9;struct Cyc_Absyn_MallocInfo*_tmp724;struct Cyc_Absyn_Stmt*_tmp723;void**_tmp722;void**_tmp721;struct Cyc_List_List*_tmp720;struct Cyc_List_List*_tmp71F;void**_tmp71E;struct Cyc_List_List*_tmp71D;void**_tmp71C;struct Cyc_Absyn_Exp*_tmp71B;struct Cyc_Absyn_Exp*_tmp71A;struct Cyc_List_List*_tmp719;struct Cyc_Absyn_Exp*_tmp718;struct Cyc_Absyn_Exp*_tmp717;struct Cyc_Absyn_Exp*_tmp716;struct Cyc_Absyn_Exp*_tmp715;struct Cyc_Absyn_Exp*_tmp714;struct Cyc_Absyn_Exp*_tmp713;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_Absyn_Exp*_tmp711;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_Absyn_Exp*_tmp70A;struct Cyc_List_List*_tmp709;struct Cyc_Absyn_Exp*_tmp708;struct Cyc_Absyn_Exp*_tmp707;struct Cyc_Absyn_Exp*_tmp706;struct Cyc_Absyn_Exp*_tmp705;struct Cyc_Absyn_Exp*_tmp704;struct Cyc_Absyn_Exp*_tmp703;struct Cyc_Absyn_Exp*_tmp702;struct Cyc_Absyn_Exp*_tmp701;switch(*((int*)_tmp6FA)){case 20U: _LL1: _tmp701=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL2:
 _tmp702=_tmp701;goto _LL4;case 21U: _LL3: _tmp702=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL4:
 _tmp703=_tmp702;goto _LL6;case 22U: _LL5: _tmp703=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL6:
 _tmp704=_tmp703;goto _LL8;case 15U: _LL7: _tmp704=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL8:
 _tmp705=_tmp704;goto _LLA;case 11U: _LL9: _tmp705=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LLA:
 _tmp706=_tmp705;goto _LLC;case 12U: _LLB: _tmp706=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LLC:
 _tmp707=_tmp706;goto _LLE;case 18U: _LLD: _tmp707=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LLE:
 _tmp708=_tmp707;goto _LL10;case 5U: _LLF: _tmp708=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL10:
 Cyc_Toc_exptypes_to_c(_tmp708);goto _LL0;case 3U: _LL11: _tmp709=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL12:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp709);goto _LL0;case 7U: _LL13: _tmp70B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp70A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL14:
 _tmp70D=_tmp70B;_tmp70C=_tmp70A;goto _LL16;case 8U: _LL15: _tmp70D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp70C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL16:
 _tmp70F=_tmp70D;_tmp70E=_tmp70C;goto _LL18;case 9U: _LL17: _tmp70F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp70E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL18:
 _tmp711=_tmp70F;_tmp710=_tmp70E;goto _LL1A;case 23U: _LL19: _tmp711=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp710=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL1A:
 _tmp713=_tmp711;_tmp712=_tmp710;goto _LL1C;case 35U: _LL1B: _tmp713=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp712=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL1C:
 _tmp715=_tmp713;_tmp714=_tmp712;goto _LL1E;case 4U: _LL1D: _tmp715=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp714=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f3;_LL1E:
 Cyc_Toc_exptypes_to_c(_tmp715);Cyc_Toc_exptypes_to_c(_tmp714);goto _LL0;case 6U: _LL1F: _tmp718=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp717=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_tmp716=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FA)->f3;_LL20:
# 4472
 Cyc_Toc_exptypes_to_c(_tmp718);Cyc_Toc_exptypes_to_c(_tmp717);Cyc_Toc_exptypes_to_c(_tmp716);goto _LL0;case 10U: _LL21: _tmp71A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp719=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL22:
# 4474
 Cyc_Toc_exptypes_to_c(_tmp71A);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp719);goto _LL0;case 14U: _LL23: _tmp71C=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_tmp71B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL24:
({void*_tmpB66=Cyc_Toc_typ_to_c(*_tmp71C);*_tmp71C=_tmpB66;});Cyc_Toc_exptypes_to_c(_tmp71B);goto _LL0;case 25U: _LL25: _tmp71E=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1)->f3;_tmp71D=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL26:
# 4477
({void*_tmpB67=Cyc_Toc_typ_to_c(*_tmp71E);*_tmp71E=_tmpB67;});
_tmp71F=_tmp71D;goto _LL28;case 36U: _LL27: _tmp71F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6FA)->f2;_LL28:
 _tmp720=_tmp71F;goto _LL2A;case 26U: _LL29: _tmp720=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL2A:
# 4481
 for(0;_tmp720 != 0;_tmp720=_tmp720->tl){
struct _tuple20 _tmp6FB=*((struct _tuple20*)_tmp720->hd);struct _tuple20 _tmp6FC=_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FD;_LL54: _tmp6FD=_tmp6FC.f2;_LL55:;
Cyc_Toc_exptypes_to_c(_tmp6FD);}
# 4485
goto _LL0;case 19U: _LL2B: _tmp721=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL2C:
 _tmp722=_tmp721;goto _LL2E;case 17U: _LL2D: _tmp722=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL2E:
({void*_tmpB68=Cyc_Toc_typ_to_c(*_tmp722);*_tmp722=_tmpB68;});goto _LL0;case 37U: _LL2F: _tmp723=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL30:
 Cyc_Toc_stmttypes_to_c(_tmp723);goto _LL0;case 34U: _LL31: _tmp724=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6FA)->f1;_LL32:
# 4490
 if(_tmp724->elt_type != 0)
({void**_tmpB6A=({void**_tmp6FE=_cycalloc(sizeof(*_tmp6FE));({void*_tmpB69=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp724->elt_type)));*_tmp6FE=_tmpB69;});_tmp6FE;});_tmp724->elt_type=_tmpB6A;});
Cyc_Toc_exptypes_to_c(_tmp724->num_elts);
goto _LL0;case 0U: _LL33: _LL34:
 goto _LL36;case 1U: _LL35: _LL36:
 goto _LL38;case 32U: _LL37: _LL38:
 goto _LL3A;case 40U: _LL39: _LL3A:
 goto _LL3C;case 33U: _LL3B: _LL3C:
 goto _LL0;case 2U: _LL3D: _LL3E:
# 4500
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
({void*_tmp6FF=0U;({unsigned int _tmpB6C=e->loc;struct _dyneither_ptr _tmpB6B=({const char*_tmp700="Cyclone expression within C code";_tag_dyneither(_tmp700,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpB6C,_tmpB6B,_tag_dyneither(_tmp6FF,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4514
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp725=d->r;void*_tmp726=_tmp725;struct Cyc_Absyn_Typedefdecl*_tmp730;struct Cyc_Absyn_Enumdecl*_tmp72F;struct Cyc_Absyn_Aggrdecl*_tmp72E;struct Cyc_Absyn_Fndecl*_tmp72D;struct Cyc_Absyn_Vardecl*_tmp72C;switch(*((int*)_tmp726)){case 0U: _LL1: _tmp72C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp726)->f1;_LL2:
# 4517
({void*_tmpB6D=Cyc_Toc_typ_to_c(_tmp72C->type);_tmp72C->type=_tmpB6D;});
if(_tmp72C->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp72C->initializer));
goto _LL0;case 1U: _LL3: _tmp72D=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp726)->f1;_LL4:
# 4521
({void*_tmpB6E=Cyc_Toc_typ_to_c(_tmp72D->ret_type);_tmp72D->ret_type=_tmpB6E;});
{struct Cyc_List_List*_tmp727=_tmp72D->args;for(0;_tmp727 != 0;_tmp727=_tmp727->tl){
({void*_tmpB6F=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp727->hd)).f3);(*((struct _tuple9*)_tmp727->hd)).f3=_tmpB6F;});}}
# 4525
goto _LL0;case 5U: _LL5: _tmp72E=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp726)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp72E,1);goto _LL0;case 7U: _LL7: _tmp72F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp726)->f1;_LL8:
# 4528
 if(_tmp72F->fields != 0){
struct Cyc_List_List*_tmp728=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp72F->fields))->v;for(0;_tmp728 != 0;_tmp728=_tmp728->tl){
struct Cyc_Absyn_Enumfield*_tmp729=(struct Cyc_Absyn_Enumfield*)_tmp728->hd;
if(_tmp729->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp729->tag));}}
# 4533
goto _LL0;case 8U: _LL9: _tmp730=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp726)->f1;_LLA:
({void*_tmpB70=Cyc_Toc_typ_to_c((void*)_check_null(_tmp730->defn));_tmp730->defn=_tmpB70;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4543
({void*_tmp72A=0U;({unsigned int _tmpB72=d->loc;struct _dyneither_ptr _tmpB71=({const char*_tmp72B="Cyclone declaration within C code";_tag_dyneither(_tmp72B,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpB72,_tmpB71,_tag_dyneither(_tmp72A,sizeof(void*),0U));});});
goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 4547
 goto _LL0;}_LL0:;}
# 4551
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp731=s->r;void*_tmp732=_tmp731;struct Cyc_Absyn_Stmt*_tmp74C;struct Cyc_Absyn_Stmt*_tmp74B;struct Cyc_Absyn_Exp*_tmp74A;struct Cyc_Absyn_Decl*_tmp749;struct Cyc_Absyn_Stmt*_tmp748;struct Cyc_Absyn_Exp*_tmp747;struct Cyc_List_List*_tmp746;void*_tmp745;struct Cyc_Absyn_Exp*_tmp744;struct Cyc_Absyn_Exp*_tmp743;struct Cyc_Absyn_Exp*_tmp742;struct Cyc_Absyn_Stmt*_tmp741;struct Cyc_Absyn_Exp*_tmp740;struct Cyc_Absyn_Stmt*_tmp73F;struct Cyc_Absyn_Exp*_tmp73E;struct Cyc_Absyn_Stmt*_tmp73D;struct Cyc_Absyn_Stmt*_tmp73C;struct Cyc_Absyn_Exp*_tmp73B;struct Cyc_Absyn_Stmt*_tmp73A;struct Cyc_Absyn_Stmt*_tmp739;struct Cyc_Absyn_Exp*_tmp738;switch(*((int*)_tmp732)){case 1U: _LL1: _tmp738=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp732)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp738);goto _LL0;case 2U: _LL3: _tmp73A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp732)->f1;_tmp739=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp732)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp73A);Cyc_Toc_stmttypes_to_c(_tmp739);goto _LL0;case 3U: _LL5: _tmp73B=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp732)->f1;_LL6:
 if(_tmp73B != 0)Cyc_Toc_exptypes_to_c(_tmp73B);goto _LL0;case 4U: _LL7: _tmp73E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp732)->f1;_tmp73D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp732)->f2;_tmp73C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp732)->f3;_LL8:
# 4557
 Cyc_Toc_exptypes_to_c(_tmp73E);Cyc_Toc_stmttypes_to_c(_tmp73D);Cyc_Toc_stmttypes_to_c(_tmp73C);goto _LL0;case 5U: _LL9: _tmp740=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp732)->f1).f1;_tmp73F=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp732)->f2;_LLA:
# 4559
 Cyc_Toc_exptypes_to_c(_tmp740);Cyc_Toc_stmttypes_to_c(_tmp73F);goto _LL0;case 9U: _LLB: _tmp744=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp732)->f1;_tmp743=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp732)->f2).f1;_tmp742=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp732)->f3).f1;_tmp741=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp732)->f4;_LLC:
# 4561
 Cyc_Toc_exptypes_to_c(_tmp744);Cyc_Toc_exptypes_to_c(_tmp743);Cyc_Toc_exptypes_to_c(_tmp742);
Cyc_Toc_stmttypes_to_c(_tmp741);goto _LL0;case 10U: _LLD: _tmp747=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp732)->f1;_tmp746=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp732)->f2;_tmp745=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp732)->f3;_LLE:
# 4564
 Cyc_Toc_exptypes_to_c(_tmp747);
for(0;_tmp746 != 0;_tmp746=_tmp746->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp746->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp749=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp732)->f1;_tmp748=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp732)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp749);Cyc_Toc_stmttypes_to_c(_tmp748);goto _LL0;case 14U: _LL11: _tmp74B=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp732)->f1;_tmp74A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp732)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp74B);Cyc_Toc_exptypes_to_c(_tmp74A);goto _LL0;case 13U: _LL13: _tmp74C=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp732)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp74C);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4574
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4577
({void*_tmpB73=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp733=_cycalloc(sizeof(*_tmp733));(*_tmp733).tag=0U;_tmp733;});s->r=_tmpB73;});
goto _LL0;default: _LL1F: _LL20:
# 4580
({void*_tmp734=0U;({unsigned int _tmpB77=s->loc;struct _dyneither_ptr _tmpB76=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp737;_tmp737.tag=0U;({struct _dyneither_ptr _tmpB74=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp737.f1=_tmpB74;});({void*_tmp735[1U];_tmp735[0]=& _tmp737;({struct _dyneither_ptr _tmpB75=({const char*_tmp736="Cyclone statement in C code: %s";_tag_dyneither(_tmp736,sizeof(char),32U);});Cyc_aprintf(_tmpB75,_tag_dyneither(_tmp735,sizeof(void*),1U));});});});Cyc_Tcutil_terr(_tmpB77,_tmpB76,_tag_dyneither(_tmp734,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 4588
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp74D=0U;({struct _dyneither_ptr _tmpB78=({const char*_tmp74E="decls_to_c: not at toplevel!";_tag_dyneither(_tmp74E,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB78,_tag_dyneither(_tmp74D,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp74F=d->r;void*_tmp750=_tmp74F;struct Cyc_List_List*_tmp768;struct Cyc_List_List*_tmp767;struct Cyc_List_List*_tmp766;struct Cyc_List_List*_tmp765;struct Cyc_Absyn_Typedefdecl*_tmp764;struct Cyc_Absyn_Enumdecl*_tmp763;struct Cyc_Absyn_Datatypedecl*_tmp762;struct Cyc_Absyn_Aggrdecl*_tmp761;struct Cyc_Absyn_Fndecl*_tmp760;struct Cyc_Absyn_Vardecl*_tmp75F;switch(*((int*)_tmp750)){case 0U: _LL1: _tmp75F=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL2: {
# 4596
struct _tuple1*_tmp751=_tmp75F->name;
# 4598
if(_tmp75F->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpB7A=({struct _tuple1*_tmp752=_cycalloc(sizeof(*_tmp752));(({union Cyc_Absyn_Nmspace _tmpB79=Cyc_Absyn_Abs_n(0,1);(*_tmp752).f1=_tmpB79;}),(*_tmp752).f2=(*_tmp751).f2);_tmp752;});_tmp751=_tmpB7A;});
if(_tmp75F->initializer != 0){
if(_tmp75F->sc == Cyc_Absyn_ExternC)_tmp75F->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp75F->initializer));else{
# 4605
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp75F->initializer));}}
# 4607
_tmp75F->name=_tmp751;
({enum Cyc_Absyn_Scope _tmpB7B=Cyc_Toc_scope_to_c(_tmp75F->sc);_tmp75F->sc=_tmpB7B;});
({void*_tmpB7C=Cyc_Toc_typ_to_c(_tmp75F->type);_tmp75F->type=_tmpB7C;});
({struct Cyc_List_List*_tmpB7D=({struct Cyc_List_List*_tmp753=_cycalloc(sizeof(*_tmp753));((*_tmp753).hd=d,(*_tmp753).tl=Cyc_Toc_result_decls);_tmp753;});Cyc_Toc_result_decls=_tmpB7D;});
goto _LL0;}case 1U: _LL3: _tmp760=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL4: {
# 4613
struct _tuple1*_tmp754=_tmp760->name;
# 4615
if(_tmp760->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB7F=({struct _tuple1*_tmp755=_cycalloc(sizeof(*_tmp755));(({union Cyc_Absyn_Nmspace _tmpB7E=Cyc_Absyn_Abs_n(0,1);(*_tmp755).f1=_tmpB7E;}),(*_tmp755).f2=(*_tmp754).f2);_tmp755;});_tmp754=_tmpB7F;});
_tmp760->sc=Cyc_Absyn_Public;}
# 4619
_tmp760->name=_tmp754;
Cyc_Toc_fndecl_to_c(nv,_tmp760,cinclude);
({struct Cyc_List_List*_tmpB80=({struct Cyc_List_List*_tmp756=_cycalloc(sizeof(*_tmp756));((*_tmp756).hd=d,(*_tmp756).tl=Cyc_Toc_result_decls);_tmp756;});Cyc_Toc_result_decls=_tmpB80;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp757=0U;({struct _dyneither_ptr _tmpB81=({const char*_tmp758="letdecl at toplevel";_tag_dyneither(_tmp758,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB81,_tag_dyneither(_tmp757,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp759=0U;({struct _dyneither_ptr _tmpB82=({const char*_tmp75A="region decl at toplevel";_tag_dyneither(_tmp75A,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB82,_tag_dyneither(_tmp759,sizeof(void*),0U));});});case 5U: _LLB: _tmp761=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LLC:
# 4627
 Cyc_Toc_aggrdecl_to_c(_tmp761,1);
goto _LL0;case 6U: _LLD: _tmp762=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LLE:
# 4630
 if(_tmp762->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp762);else{
# 4633
Cyc_Toc_datatypedecl_to_c(_tmp762);}
goto _LL0;case 7U: _LLF: _tmp763=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL10:
# 4636
 Cyc_Toc_enumdecl_to_c(_tmp763);
({struct Cyc_List_List*_tmpB83=({struct Cyc_List_List*_tmp75B=_cycalloc(sizeof(*_tmp75B));((*_tmp75B).hd=d,(*_tmp75B).tl=Cyc_Toc_result_decls);_tmp75B;});Cyc_Toc_result_decls=_tmpB83;});
goto _LL0;case 8U: _LL11: _tmp764=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL12:
# 4640
 _tmp764->tvs=0;
if(_tmp764->defn != 0)
({void*_tmpB84=Cyc_Toc_typ_to_c((void*)_check_null(_tmp764->defn));_tmp764->defn=_tmpB84;});else{
# 4644
enum Cyc_Absyn_KindQual _tmp75C=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp764->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp75D=_tmp75C;if(_tmp75D == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpB85=Cyc_Absyn_void_star_typ();_tmp764->defn=_tmpB85;});goto _LL1F;}else{_LL22: _LL23:
 _tmp764->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL1F;}_LL1F:;}
# 4652
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpB86=({struct Cyc_List_List*_tmp75E=_cycalloc(sizeof(*_tmp75E));((*_tmp75E).hd=d,(*_tmp75E).tl=Cyc_Toc_result_decls);_tmp75E;});Cyc_Toc_result_decls=_tmpB86;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
# 4657
 goto _LL0;case 9U: _LL17: _tmp765=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp750)->f2;_LL18:
 _tmp766=_tmp765;goto _LL1A;case 10U: _LL19: _tmp766=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp750)->f2;_LL1A:
 _tmp767=_tmp766;goto _LL1C;case 11U: _LL1B: _tmp767=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpB87=Cyc_Toc_decls_to_c(r,nv,_tmp767,top,cinclude);nv=_tmpB87;});goto _LL0;default: _LL1D: _tmp768=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpB88=Cyc_Toc_decls_to_c(r,nv,_tmp768,top,1);nv=_tmpB88;});goto _LL0;}_LL0:;};}
# 4664
return nv;}
# 4668
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp769=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp76A=_tmp769;struct Cyc_Core_DynamicRegion*_tmp76F;_LL1: _tmp76F=_tmp76A.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp76F->h;
({struct Cyc_Toc_TocState*_tmpB89=Cyc_Toc_empty_toc_state(h);ts=_tmpB89;});;}
({struct Cyc_Toc_TocStateWrap*_tmpB8A=({struct Cyc_Toc_TocStateWrap*_tmp76C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp76C));((*_tmp76C).dyn=_tmp76F,(*_tmp76C).state=ts);_tmp76C;});Cyc_Toc_toc_state=_tmpB8A;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0U;
Cyc_Toc_bounds_checks_eliminated=0U;
({struct _dyneither_ptr _tmpB8B=_tag_dyneither(({unsigned int _tmp76E=39;struct _dyneither_ptr**_tmp76D=_cycalloc(_check_times(_tmp76E,sizeof(struct _dyneither_ptr*)));(((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((_tmp76D[0]=& Cyc_Toc__throw_str,_tmp76D[1]=& Cyc_Toc_setjmp_str)),_tmp76D[2]=& Cyc_Toc__push_handler_str)),_tmp76D[3]=& Cyc_Toc__pop_handler_str)),_tmp76D[4]=& Cyc_Toc__exn_thrown_str)),_tmp76D[5]=& Cyc_Toc__npop_handler_str)),_tmp76D[6]=& Cyc_Toc__check_null_str)),_tmp76D[7]=& Cyc_Toc__check_known_subscript_null_str)),_tmp76D[8]=& Cyc_Toc__check_known_subscript_notnull_str)),_tmp76D[9]=& Cyc_Toc__check_dyneither_subscript_str)),_tmp76D[10]=& Cyc_Toc__dyneither_ptr_str)),_tmp76D[11]=& Cyc_Toc__tag_dyneither_str)),_tmp76D[12]=& Cyc_Toc__init_dyneither_ptr_str)),_tmp76D[13]=& Cyc_Toc__untag_dyneither_ptr_str)),_tmp76D[14]=& Cyc_Toc__get_dyneither_size_str)),_tmp76D[15]=& Cyc_Toc__get_zero_arr_size_str)),_tmp76D[16]=& Cyc_Toc__dyneither_ptr_plus_str)),_tmp76D[17]=& Cyc_Toc__zero_arr_plus_str)),_tmp76D[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str)),_tmp76D[19]=& Cyc_Toc__zero_arr_inplace_plus_str)),_tmp76D[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str)),_tmp76D[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str)),_tmp76D[22]=& Cyc_Toc__cycalloc_str)),_tmp76D[23]=& Cyc_Toc__cyccalloc_str)),_tmp76D[24]=& Cyc_Toc__cycalloc_atomic_str)),_tmp76D[25]=& Cyc_Toc__cyccalloc_atomic_str)),_tmp76D[26]=& Cyc_Toc__region_malloc_str)),_tmp76D[27]=& Cyc_Toc__region_calloc_str)),_tmp76D[28]=& Cyc_Toc__check_times_str)),_tmp76D[29]=& Cyc_Toc__new_region_str)),_tmp76D[30]=& Cyc_Toc__push_region_str)),_tmp76D[31]=& Cyc_Toc__pop_region_str)),_tmp76D[32]=& Cyc_Toc__open_dynregion_str)),_tmp76D[33]=& Cyc_Toc__push_dynregion_str)),_tmp76D[34]=& Cyc_Toc__pop_dynregion_str)),_tmp76D[35]=& Cyc_Toc__throw_arraybounds_str)),_tmp76D[36]=& Cyc_Toc__dyneither_ptr_decrease_size_str)),_tmp76D[37]=& Cyc_Toc__throw_match_str)),_tmp76D[38]=& Cyc_Toc__fast_region_malloc_str);_tmp76D;}),sizeof(struct _dyneither_ptr*),39U);Cyc_Toc_globals=_tmpB8B;});};}
# 4723
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp770=ts;struct Cyc_Toc_TocStateWrap*_tmp771=Cyc_Toc_toc_state;ts=_tmp771;Cyc_Toc_toc_state=_tmp770;});{
struct Cyc_Toc_TocStateWrap _tmp772=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp773=_tmp772;struct Cyc_Core_DynamicRegion*_tmp779;struct Cyc_Toc_TocState*_tmp778;_LL1: _tmp779=_tmp773.dyn;_tmp778=_tmp773.state;_LL2:;
# 4728
{struct _RegionHandle*h=& _tmp779->h;
{struct Cyc_Toc_TocState _tmp775=*_tmp778;struct Cyc_Toc_TocState _tmp776=_tmp775;struct Cyc_Xarray_Xarray*_tmp777;_LL4: _tmp777=_tmp776.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp777);}
# 4729
;}
# 4732
Cyc_Core_free_rckey(_tmp779);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4735
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4741
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4743
({struct Cyc_Hashtable_Table**_tmpB8C=({struct Cyc_Hashtable_Table**_tmp77A=_cycalloc(sizeof(*_tmp77A));*_tmp77A=pop_tables;_tmp77A;});Cyc_Toc_gpop_tables=_tmpB8C;});
Cyc_Toc_init();
{struct _RegionHandle _tmp77B=_new_region("start");struct _RegionHandle*start=& _tmp77B;_push_region(start);
({struct _RegionHandle*_tmpB8E=start;struct Cyc_Toc_Env*_tmpB8D=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpB8E,_tmpB8D,ds,1,0);});;_pop_region(start);}
# 4752
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
