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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 723 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 908
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 915
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 928
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 935
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 938
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 953
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 955
extern void*Cyc_Absyn_sint_typ;
# 967
void*Cyc_Absyn_exn_typ();
# 978
extern void*Cyc_Absyn_bounds_one;
# 986
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 992
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 994
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 997
void*Cyc_Absyn_void_star_typ();
# 999
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1005
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1074
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1090
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1093
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1097
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1099
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1104
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1108
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1113
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1156
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1159
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1169
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1173
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1183
struct _tuple1*Cyc_Absyn_binding2qvar(void*);
# 1188
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
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 48
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
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_Hashtable_Table;
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
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmpE9,_tmpE9,_tmpE9 + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_dynregion_bnd={0,& Cyc_Toc__pop_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,(void*)& Cyc_Toc__pop_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmpEC[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpEC,_tmpEC,_tmpEC + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpEF[29]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpEF,_tmpEF,_tmpEF + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpF2[11]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpF2,_tmpF2,_tmpF2 + 11};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__swap_word_bnd={0,& Cyc_Toc__swap_word_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,(void*)& Cyc_Toc__swap_word_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpF5[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpF5,_tmpF5,_tmpF5 + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__swap_dyneither_bnd={0,& Cyc_Toc__swap_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,(void*)& Cyc_Toc__swap_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpF8[13]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpF8,_tmpF8,_tmpF8 + 13};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmpFB[9]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmpFB,_tmpFB,_tmpFB + 9};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmpFE[20]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmpFE,_tmpFE,_tmpFE + 20};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 290
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 293
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 295
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 297
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0);}
# 303
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp102=e->r;void*_tmp103=_tmp102;struct Cyc_Absyn_Exp*_tmp104;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp103)->tag == 13){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp103)->f4 == Cyc_Absyn_No_coercion){_LL10: _tmp104=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL11:
 return Cyc_Toc_cast_it(t,_tmp104);}else{goto _LL12;}}else{_LL12: _LL13:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}_LLF:;}
# 309
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp106;_tmp106.tag=13;_tmp106.f1=t;_tmp106.f2=e;_tmp106.f3=0;_tmp106.f4=Cyc_Absyn_No_coercion;_tmp106;});_tmp105;});}
# 312
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp108;_tmp108.tag=19;_tmp108.f1=e;_tmp108;});_tmp107;});}
# 315
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp10A;_tmp10A.tag=22;_tmp10A.f1=e1;_tmp10A.f2=e2;_tmp10A;});_tmp109;});}
# 318
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp10C;_tmp10C.tag=36;_tmp10C.f1=s;_tmp10C;});_tmp10B;});}
# 321
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp10E;_tmp10E.tag=16;_tmp10E.f1=t;_tmp10E;});_tmp10D;});}
# 324
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp110;_tmp110.tag=9;_tmp110.f1=e;_tmp110.f2=es;_tmp110.f3=0;_tmp110.f4=1;_tmp110;});_tmp10F;});}
# 327
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp112;_tmp112.tag=1;_tmp112.f1=e;_tmp112;});_tmp111;});}
# 330
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp114;_tmp114.tag=2;_tmp114.f1=s1;_tmp114.f2=s2;_tmp114;});_tmp113;});}
# 333
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp116;_tmp116.tag=5;_tmp116.f1=e1;_tmp116.f2=e2;_tmp116.f3=e3;_tmp116;});_tmp115;});}
# 336
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp118;_tmp118.tag=20;_tmp118.f1=e;_tmp118.f2=n;_tmp118.f3=0;_tmp118.f4=0;_tmp118;});_tmp117;});}
# 339
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp11A;_tmp11A.tag=21;_tmp11A.f1=e;_tmp11A.f2=n;_tmp11A.f3=0;_tmp11A.f4=0;_tmp11A;});_tmp119;});}
# 342
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 345
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp11C;_tmp11C.tag=35;_tmp11C.f1=tdopt;_tmp11C.f2=ds;_tmp11C;});_tmp11B;});}
# 347
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp11E;_tmp11E.tag=8;_tmp11E.f1=v;_tmp11E;});_tmp11D;});}
# 350
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 354
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp120=e->r;void*_tmp121=_tmp120;struct Cyc_Absyn_Exp*_tmp122;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp121)->tag == 19){_LL15: _tmp122=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp121)->f1;_LL16:
 return Cyc_Absyn_aggrarrow_exp(_tmp122,f,loc);}else{_LL17: _LL18:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL14:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 373
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 382
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 391
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 400
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 411
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp123=Cyc_Tcutil_compress(t);void*_tmp124=_tmp123;enum Cyc_Absyn_Size_of _tmp12B;switch(*((int*)_tmp124)){case 6: _LL1A: _tmp12B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp124)->f2;_LL1B:
# 415
{enum Cyc_Absyn_Size_of _tmp125=_tmp12B;switch(_tmp125){case Cyc_Absyn_Char_sz: _LL27: _LL28:
 function=fS->fchar;goto _LL26;case Cyc_Absyn_Short_sz: _LL29: _LL2A:
 function=fS->fshort;goto _LL26;case Cyc_Absyn_Int_sz: _LL2B: _LL2C:
 function=fS->fint;goto _LL26;default: _LL2D: _LL2E:
({void*_tmp126=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp127="impossible IntType (not char, short or int)";_tag_dyneither(_tmp127,sizeof(char),44);}),_tag_dyneither(_tmp126,sizeof(void*),0));});}_LL26:;}
# 421
goto _LL19;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp124)->f1){case 0: _LL1C: _LL1D:
 function=fS->ffloat;goto _LL19;case 1: _LL1E: _LL1F:
 function=fS->fdouble;goto _LL19;default: _LL20: _LL21:
 function=fS->flongdouble;goto _LL19;}case 5: _LL22: _LL23:
 function=fS->fvoidstar;goto _LL19;default: _LL24: _LL25:
({struct Cyc_String_pa_PrintArg_struct _tmp12A;_tmp12A.tag=0;_tmp12A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp128[1]={& _tmp12A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp129="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp129,sizeof(char),67);}),_tag_dyneither(_tmp128,sizeof(void*),1));});});}_LL19:;}
# 428
return function;}
# 430
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp12C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp12D=_tmp12C;void*_tmp130;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12D)->tag == 5){_LL30: _tmp130=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12D)->f1).elt_typ;_LL31:
 return Cyc_Toc_getFunctionType(fS,_tmp130);}else{_LL32: _LL33:
({void*_tmp12E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp12F="impossible type (not pointer)";_tag_dyneither(_tmp12F,sizeof(char),30);}),_tag_dyneither(_tmp12E,sizeof(void*),0));});}_LL2F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 440
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp131=e->r;void*_tmp132=_tmp131;struct Cyc_List_List*_tmp13E;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;long long _tmp138;int _tmp137;short _tmp136;struct _dyneither_ptr _tmp135;char _tmp134;switch(*((int*)_tmp132)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).Null_c).tag){case 2: _LL35: _tmp134=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).Char_c).val).f2;_LL36:
 return _tmp134 == '\000';case 3: _LL37: _tmp135=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).Wchar_c).val;_LL38: {
# 444
unsigned long _tmp133=Cyc_strlen((struct _dyneither_ptr)_tmp135);
int i=0;
if(_tmp133 >= 2  && *((const char*)_check_dyneither_subscript(_tmp135,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp135,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp135,sizeof(char),1))== 'x'  && _tmp133 >= 3) && *((const char*)_check_dyneither_subscript(_tmp135,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp133;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp135,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 454
return 0;}}case 4: _LL39: _tmp136=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).Short_c).val).f2;_LL3A:
 return _tmp136 == 0;case 5: _LL3B: _tmp137=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).Int_c).val).f2;_LL3C:
 return _tmp137 == 0;case 6: _LL3D: _tmp138=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp132)->f1).LongLong_c).val).f2;_LL3E:
 return _tmp138 == 0;case 1: _LL3F: _LL40:
 return 1;default: goto _LL4D;}case 13: _LL41: _tmp139=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp132)->f2;_LL42:
 return Cyc_Toc_is_zero(_tmp139);case 23: _LL43: _tmp13A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp132)->f1;_LL44:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp13A);case 25: _LL45: _tmp13B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp132)->f1;_LL46:
 _tmp13C=_tmp13B;goto _LL48;case 28: _LL47: _tmp13C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp132)->f3;_LL48:
 _tmp13D=_tmp13C;goto _LL4A;case 24: _LL49: _tmp13D=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp132)->f2;_LL4A:
 _tmp13E=_tmp13D;goto _LL4C;case 35: _LL4B: _tmp13E=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp132)->f2;_LL4C:
# 465
 for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp13E->hd)).f2))return 0;}
return 1;default: _LL4D: _LL4E:
 return 0;}_LL34:;}
# 473
static int Cyc_Toc_is_nullable(void*t){
void*_tmp13F=Cyc_Tcutil_compress(t);void*_tmp140=_tmp13F;union Cyc_Absyn_Constraint*_tmp143;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp140)->tag == 5){_LL50: _tmp143=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp140)->f1).ptr_atts).nullable;_LL51:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp143);}else{_LL52: _LL53:
({void*_tmp141=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp142="Toc::is_nullable";_tag_dyneither(_tmp142,sizeof(char),17);}),_tag_dyneither(_tmp141,sizeof(void*),0));});}_LL4F:;}
# 482
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmp144=x;union Cyc_Absyn_Nmspace _tmp156;struct _dyneither_ptr*_tmp155;_LL55: _tmp156=_tmp144->f1;_tmp155=_tmp144->f2;_LL56:;{
union Cyc_Absyn_Nmspace _tmp145=_tmp156;struct Cyc_List_List*_tmp154;struct Cyc_List_List*_tmp153;struct Cyc_List_List*_tmp152;switch((_tmp145.Abs_n).tag){case 4: _LL58: _LL59:
 _tmp152=0;goto _LL5B;case 1: _LL5A: _tmp152=(_tmp145.Rel_n).val;_LL5B:
 _tmp153=_tmp152;goto _LL5D;case 2: _LL5C: _tmp153=(_tmp145.Abs_n).val;_LL5D:
 _tmp154=_tmp153;goto _LL5F;default: _LL5E: _tmp154=(_tmp145.C_n).val;_LL5F:
# 491
 if(_tmp154 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0;_tmp149.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp155);({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);({void*_tmp146[2]={& _tmp148,& _tmp149};Cyc_aprintf(({const char*_tmp147="%s_%s_struct";_tag_dyneither(_tmp147,sizeof(char),13);}),_tag_dyneither(_tmp146,sizeof(void*),2));});});});{
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct _dyneither_ptr _tmp151=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp155);({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_rstr_sepstr(r,_tmp154,({const char*_tmp150="_";_tag_dyneither(_tmp150,sizeof(char),2);})));({struct Cyc_String_pa_PrintArg_struct _tmp14D;_tmp14D.tag=0;_tmp14D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);({void*_tmp14B[3]={& _tmp14D,& _tmp14E,& _tmp14F};Cyc_aprintf(({const char*_tmp14C="%s_%s_%s_struct";_tag_dyneither(_tmp14C,sizeof(char),16);}),_tag_dyneither(_tmp14B,sizeof(void*),3));});});});});_npop_handler(0);return _tmp151;};_pop_region(r);};}_LL57:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 504
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 507
struct _tuple20 _tmp157=*env;struct _tuple20 _tmp158=_tmp157;struct Cyc_Dict_Dict*_tmp16B;struct _tuple14*_tmp16A;_LL61: _tmp16B=(_tmp158.f1)->qvar_tags;_tmp16A=_tmp158.f2;_LL62:;{
struct _tuple14 _tmp159=*_tmp16A;struct _tuple14 _tmp15A=_tmp159;struct _tuple1*_tmp169;struct _tuple1*_tmp168;_LL64: _tmp169=_tmp15A.f1;_tmp168=_tmp15A.f2;_LL65:;{
struct _handler_cons _tmp15B;_push_handler(& _tmp15B);{int _tmp15D=0;if(setjmp(_tmp15B.handler))_tmp15D=1;if(!_tmp15D){{struct _tuple1*_tmp15E=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp16B,Cyc_Toc_qvar_tag_cmp,_tmp16A);_npop_handler(0);return _tmp15E;};_pop_handler();}else{void*_tmp15C=(void*)_exn_thrown;void*_tmp15F=_tmp15C;void*_tmp167;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15F)->tag == Cyc_Dict_Absent){_LL67: _LL68: {
# 511
struct _tuple14*_tmp160=({struct _tuple14*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->f1=_tmp169;_tmp166->f2=_tmp168;_tmp166;});
struct _tuple1*_tmp161=_tmp169;union Cyc_Absyn_Nmspace _tmp165;struct _dyneither_ptr*_tmp164;_LL6C: _tmp165=_tmp161->f1;_tmp164=_tmp161->f2;_LL6D:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp164,_tmp168);
struct _tuple1*res=({struct _tuple1*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->f1=_tmp165;_tmp162->f2=({struct _dyneither_ptr*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163[0]=newvar;_tmp163;});_tmp162;});
*_tmp16B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp16B,_tmp160,res);
return res;};}}else{_LL69: _tmp167=_tmp15F;_LL6A:(int)_rethrow(_tmp167);}_LL66:;}};};};}
# 520
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp16C;_tmp16C.f1=fieldname;_tmp16C.f2=dtname;_tmp16C;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 525
void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 529
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 532
struct _tuple21 _tmp16D=*env;struct _tuple21 _tmp16E=_tmp16D;struct Cyc_List_List**_tmp186;struct Cyc_List_List*_tmp185;_LL6F: _tmp186=(_tmp16E.f1)->tuple_types;_tmp185=_tmp16E.f2;_LL70:;
# 534
{struct Cyc_List_List*_tmp16F=*_tmp186;for(0;_tmp16F != 0;_tmp16F=_tmp16F->tl){
struct _tuple22*_tmp170=(struct _tuple22*)_tmp16F->hd;struct _tuple22*_tmp171=_tmp170;void*_tmp174;struct Cyc_List_List*_tmp173;_LL72: _tmp174=_tmp171->f1;_tmp173=_tmp171->f2;_LL73:;{
struct Cyc_List_List*_tmp172=_tmp185;
for(0;_tmp172 != 0  && _tmp173 != 0;(_tmp172=_tmp172->tl,_tmp173=_tmp173->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp172->hd)).f2,(void*)_tmp173->hd))
break;}
if(_tmp172 == 0  && _tmp173 == 0)
return _tmp174;};}}{
# 544
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp184;_tmp184.tag=1;_tmp184.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp182[1]={& _tmp184};Cyc_aprintf(({const char*_tmp183="_tuple%d";_tag_dyneither(_tmp183,sizeof(char),9);}),_tag_dyneither(_tmp182,sizeof(void*),1));});});_tmp181;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp185);
struct Cyc_List_List*_tmp175=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmp175=({struct Cyc_List_List*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->hd=({struct Cyc_Absyn_Aggrfield*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->name=Cyc_Absyn_fieldname(i);_tmp177->tq=Cyc_Toc_mt_tq;_tmp177->type=(void*)ts2->hd;_tmp177->width=0;_tmp177->attributes=0;_tmp177->requires_clause=0;_tmp177;});_tmp176->tl=_tmp175;_tmp176;});}}
_tmp175=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp175);{
struct Cyc_Absyn_Aggrdecl*_tmp178=({struct Cyc_Absyn_Aggrdecl*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->kind=Cyc_Absyn_StructA;_tmp17E->sc=Cyc_Absyn_Public;_tmp17E->name=({struct _tuple1*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->f1=
Cyc_Absyn_Rel_n(0);_tmp180->f2=xname;_tmp180;});_tmp17E->tvs=0;_tmp17E->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->exist_vars=0;_tmp17F->rgn_po=0;_tmp17F->fields=_tmp175;_tmp17F->tagged=0;_tmp17F;});_tmp17E->attributes=0;_tmp17E->expected_mem_kind=0;_tmp17E;});
# 559
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp17B;_tmp17B.tag=5;_tmp17B.f1=_tmp178;_tmp17B;});_tmp17A;}),0);_tmp179->tl=Cyc_Toc_result_decls;_tmp179;});
*_tmp186=({struct Cyc_List_List*_tmp17C=_region_malloc(d,sizeof(*_tmp17C));_tmp17C->hd=({struct _tuple22*_tmp17D=_region_malloc(d,sizeof(*_tmp17D));_tmp17D->f1=x;_tmp17D->f2=ts;_tmp17D;});_tmp17C->tl=*_tmp186;_tmp17C;});
return x;};};}
# 564
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 573
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 581
struct _tuple24 _tmp187=*env;struct _tuple24 _tmp188=_tmp187;struct Cyc_List_List**_tmp1B2;struct _tuple1*_tmp1B1;struct Cyc_List_List*_tmp1B0;struct Cyc_List_List*_tmp1AF;struct Cyc_List_List*_tmp1AE;_LL75: _tmp1B2=(_tmp188.f1)->abs_struct_types;_tmp1B1=(_tmp188.f2)->f1;_tmp1B0=(_tmp188.f2)->f2;_tmp1AF=(_tmp188.f2)->f3;_tmp1AE=(_tmp188.f2)->f4;_LL76:;
# 586
{struct Cyc_List_List*_tmp189=*_tmp1B2;for(0;_tmp189 != 0;_tmp189=_tmp189->tl){
struct _tuple25*_tmp18A=(struct _tuple25*)_tmp189->hd;struct _tuple25*_tmp18B=_tmp18A;struct _tuple1*_tmp193;struct Cyc_List_List*_tmp192;void*_tmp191;_LL78: _tmp193=_tmp18B->f1;_tmp192=_tmp18B->f2;_tmp191=_tmp18B->f3;_LL79:;
# 589
if(Cyc_Absyn_qvar_cmp(_tmp193,_tmp1B1)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1AF)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp192)){
int okay=1;
{struct Cyc_List_List*_tmp18C=_tmp1AF;for(0;_tmp18C != 0;(_tmp18C=_tmp18C->tl,_tmp192=_tmp192->tl)){
void*_tmp18D=(void*)_tmp18C->hd;
void*_tmp18E=(void*)((struct Cyc_List_List*)_check_null(_tmp192))->hd;
# 596
{struct Cyc_Absyn_Kind*_tmp18F=Cyc_Tcutil_typ_kind(_tmp18D);struct Cyc_Absyn_Kind*_tmp190=_tmp18F;switch(((struct Cyc_Absyn_Kind*)_tmp190)->kind){case Cyc_Absyn_EffKind: _LL7B: _LL7C:
 goto _LL7E;case Cyc_Absyn_RgnKind: _LL7D: _LL7E:
# 601
 continue;default: _LL7F: _LL80:
# 604
 if(Cyc_Tcutil_unify(_tmp18D,_tmp18E) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp18D),Cyc_Toc_typ_to_c(_tmp18E)))
# 606
continue;
# 609
okay=0;
goto _LL7A;}_LL7A:;}
# 612
break;}}
# 614
if(okay)
# 616
return _tmp191;}}}{
# 623
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1AD;_tmp1AD.tag=1;_tmp1AD.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp1AB[1]={& _tmp1AD};Cyc_aprintf(({const char*_tmp1AC="_tuple%d";_tag_dyneither(_tmp1AC,sizeof(char),9);}),_tag_dyneither(_tmp1AB,sizeof(void*),1));});});_tmp1AA;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp194=0;
# 627
*_tmp1B2=({struct Cyc_List_List*_tmp195=_region_malloc(d,sizeof(*_tmp195));_tmp195->hd=({struct _tuple25*_tmp196=_region_malloc(d,sizeof(*_tmp196));_tmp196->f1=_tmp1B1;_tmp196->f2=_tmp1AF;_tmp196->f3=x;_tmp196;});_tmp195->tl=*_tmp1B2;_tmp195;});{
# 630
struct _RegionHandle _tmp197=_new_region("r");struct _RegionHandle*r=& _tmp197;_push_region(r);
{struct Cyc_List_List*_tmp198=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1B0,_tmp1AF);
for(0;_tmp1AE != 0;_tmp1AE=_tmp1AE->tl){
struct Cyc_Absyn_Aggrfield*_tmp199=(struct Cyc_Absyn_Aggrfield*)_tmp1AE->hd;
void*t=_tmp199->type;
struct Cyc_List_List*atts=_tmp199->attributes;
# 638
if(_tmp1AE->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t)))
atts=({struct Cyc_List_List*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp19C;_tmp19C.tag=6;_tmp19C.f1=0;_tmp19C;});_tmp19B;});_tmp19A->tl=atts;_tmp19A;});
# 641
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp198,t));
# 644
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp19E;_tmp19E.tag=8;_tmp19E.f1=({struct Cyc_Absyn_ArrayInfo _tmp19F;_tmp19F.elt_type=Cyc_Absyn_void_star_typ();_tmp19F.tq=Cyc_Absyn_empty_tqual(0);_tmp19F.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp19F.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp19F.zt_loc=0;_tmp19F;});_tmp19E;});_tmp19D;});
# 648
_tmp194=({struct Cyc_List_List*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->hd=({struct Cyc_Absyn_Aggrfield*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->name=_tmp199->name;_tmp1A1->tq=Cyc_Toc_mt_tq;_tmp1A1->type=t;_tmp1A1->width=_tmp199->width;_tmp1A1->attributes=atts;_tmp1A1->requires_clause=0;_tmp1A1;});_tmp1A0->tl=_tmp194;_tmp1A0;});}
# 650
_tmp194=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp194);{
struct Cyc_Absyn_Aggrdecl*_tmp1A2=({struct Cyc_Absyn_Aggrdecl*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->kind=Cyc_Absyn_StructA;_tmp1A7->sc=Cyc_Absyn_Public;_tmp1A7->name=({struct _tuple1*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->f1=
Cyc_Absyn_Rel_n(0);_tmp1A9->f2=xname;_tmp1A9;});_tmp1A7->tvs=0;_tmp1A7->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->exist_vars=0;_tmp1A8->rgn_po=0;_tmp1A8->fields=_tmp194;_tmp1A8->tagged=0;_tmp1A8;});_tmp1A7->attributes=0;_tmp1A7->expected_mem_kind=0;_tmp1A7;});
# 658
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1A5;_tmp1A5.tag=5;_tmp1A5.f1=_tmp1A2;_tmp1A5;});_tmp1A4;}),0);_tmp1A3->tl=Cyc_Toc_result_decls;_tmp1A3;});{
void*_tmp1A6=x;_npop_handler(0);return _tmp1A6;};};}
# 631
;_pop_region(r);};};}
# 662
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 666
struct _tuple23 env=({struct _tuple23 _tmp1B3;_tmp1B3.f1=struct_name;_tmp1B3.f2=type_vars;_tmp1B3.f3=type_args;_tmp1B3.f4=fields;_tmp1B3;});
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 673
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->f1=Cyc_Absyn_Loc_n;_tmp1B4->f2=({struct _dyneither_ptr*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1B8;_tmp1B8.tag=1;_tmp1B8.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;({void*_tmp1B6[1]={& _tmp1B8};Cyc_aprintf(({const char*_tmp1B7="_tmp%X";_tag_dyneither(_tmp1B7,sizeof(char),7);}),_tag_dyneither(_tmp1B6,sizeof(void*),1));});});_tmp1B5;});_tmp1B4;});}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 679
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp1B9=*env;struct _tuple26 _tmp1BA=_tmp1B9;struct Cyc_Xarray_Xarray*_tmp1C2;_LL82: _tmp1C2=(_tmp1BA.f1)->temp_labels;_LL83:;{
int _tmp1BB=Cyc_Toc_fresh_label_counter ++;
if(_tmp1BB < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp1C2))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp1C2,_tmp1BB);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1C1;_tmp1C1.tag=1;_tmp1C1.f1=(unsigned int)_tmp1BB;({void*_tmp1BF[1]={& _tmp1C1};Cyc_aprintf(({const char*_tmp1C0="_LL%X";_tag_dyneither(_tmp1C0,sizeof(char),6);}),_tag_dyneither(_tmp1BF,sizeof(void*),1));});});_tmp1BE;});
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp1C2,res)!= _tmp1BB)
({void*_tmp1BC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1BD="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp1BD,sizeof(char),43);}),_tag_dyneither(_tmp1BC,sizeof(void*),0));});
return res;};};}
# 690
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 697
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp1C3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp1C3))->hd)->name)!= 0){
++ ans;
_tmp1C3=_tmp1C3->tl;}
# 704
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 710
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 715
struct _tuple9 _tmp1C4=*a;struct _tuple9 _tmp1C5=_tmp1C4;struct _dyneither_ptr*_tmp1C9;struct Cyc_Absyn_Tqual _tmp1C8;void*_tmp1C7;_LL85: _tmp1C9=_tmp1C5.f1;_tmp1C8=_tmp1C5.f2;_tmp1C7=_tmp1C5.f3;_LL86:;
return({struct _tuple9*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->f1=_tmp1C9;_tmp1C6->f2=_tmp1C8;_tmp1C6->f3=Cyc_Toc_typ_to_c(_tmp1C7);_tmp1C6;});}
# 718
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1CA=*x;struct _tuple11 _tmp1CB=_tmp1CA;struct Cyc_Absyn_Tqual _tmp1CE;void*_tmp1CD;_LL88: _tmp1CE=_tmp1CB.f1;_tmp1CD=_tmp1CB.f2;_LL89:;
return({struct _tuple11*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->f1=_tmp1CE;_tmp1CC->f2=Cyc_Toc_typ_to_c(_tmp1CD);_tmp1CC;});}
# 737 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1CF=Cyc_Tcutil_compress(t);void*_tmp1D0=_tmp1CF;void*_tmp1D6;void*_tmp1D5;struct Cyc_Absyn_Tqual _tmp1D4;struct Cyc_Absyn_Exp*_tmp1D3;union Cyc_Absyn_Constraint*_tmp1D2;unsigned int _tmp1D1;switch(*((int*)_tmp1D0)){case 8: _LL8B: _tmp1D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).elt_type;_tmp1D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).tq;_tmp1D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).num_elts;_tmp1D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).zero_term;_tmp1D1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).zt_loc;_LL8C:
# 740
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp1D5),_tmp1D4);case 1: _LL8D: _tmp1D6=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D0)->f2;if(_tmp1D6 != 0){_LL8E:
 return Cyc_Toc_typ_to_c_array(_tmp1D6);}else{goto _LL8F;}default: _LL8F: _LL90:
 return Cyc_Toc_typ_to_c(t);}_LL8A:;}
# 746
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 748
return({struct Cyc_Absyn_Aggrfield*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->name=f->name;_tmp1D7->tq=Cyc_Toc_mt_tq;_tmp1D7->type=
# 750
Cyc_Toc_typ_to_c(f->type);_tmp1D7->width=f->width;_tmp1D7->attributes=f->attributes;_tmp1D7->requires_clause=0;_tmp1D7;});}
# 755
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 757
return;}
# 760
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
cs=({void**_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp1D8;});
# 765
return*cs;}
# 767
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);_tmp1D9;});
# 772
return*r;}
# 774
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_tmp1DA;});
# 779
return*r;}
# 781
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1DB=Cyc_Tcutil_compress(t);void*_tmp1DC=_tmp1DB;struct Cyc_Absyn_Tvar*_tmp1DD;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DC)->tag == 2){_LL92: _tmp1DD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DC)->f1;_LL93:
# 784
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL94: _LL95:
 return 0;}_LL91:;}
# 788
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1DE=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1DF=_tmp1DE;if(((struct Cyc_Absyn_Kind*)_tmp1DF)->kind == Cyc_Absyn_AnyKind){_LL97: _LL98:
 return 1;}else{_LL99: _LL9A:
 return 0;}_LL96:;}
# 795
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1E0=t;struct Cyc_Absyn_Datatypedecl*_tmp22A;void**_tmp229;struct Cyc_Absyn_Enumdecl*_tmp228;struct Cyc_Absyn_Aggrdecl*_tmp227;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Exp*_tmp225;struct _tuple1*_tmp224;struct Cyc_List_List*_tmp223;struct Cyc_Absyn_Typedefdecl*_tmp222;void*_tmp221;struct Cyc_List_List*_tmp220;struct _tuple1*_tmp21F;union Cyc_Absyn_AggrInfoU _tmp21E;struct Cyc_List_List*_tmp21D;enum Cyc_Absyn_AggrKind _tmp21C;struct Cyc_List_List*_tmp21B;struct Cyc_List_List*_tmp21A;struct Cyc_Absyn_Tqual _tmp219;void*_tmp218;struct Cyc_List_List*_tmp217;int _tmp216;struct Cyc_Absyn_VarargInfo*_tmp215;struct Cyc_List_List*_tmp214;void*_tmp213;struct Cyc_Absyn_Tqual _tmp212;struct Cyc_Absyn_Exp*_tmp211;unsigned int _tmp210;void*_tmp20F;struct Cyc_Absyn_Tqual _tmp20E;union Cyc_Absyn_Constraint*_tmp20D;struct Cyc_Absyn_Datatypedecl*_tmp20C;struct Cyc_Absyn_Datatypefield*_tmp20B;struct Cyc_Absyn_Tvar*_tmp20A;void**_tmp209;switch(*((int*)_tmp1E0)){case 0: _LL9C: _LL9D:
 return t;case 1: _LL9E: _tmp209=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E0)->f2;_LL9F:
# 799
 if(*_tmp209 == 0){
*_tmp209=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 803
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp209));case 2: _LLA0: _tmp20A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1E0)->f1;_LLA1:
# 805
 if((Cyc_Tcutil_tvar_kind(_tmp20A,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 808
return(void*)& Cyc_Absyn_VoidType_val;else{
# 810
return Cyc_Absyn_void_star_typ();}case 3: _LLA2: _LLA3:
# 812
 return(void*)& Cyc_Absyn_VoidType_val;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1E0)->f1).field_info).KnownDatatypefield).tag == 2){_LLA4: _tmp20C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1E0)->f1).field_info).KnownDatatypefield).val).f1;_tmp20B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1E0)->f1).field_info).KnownDatatypefield).val).f2;_LLA5:
# 814
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp20B->name,_tmp20C->name));}else{_LLA6: _LLA7:
({void*_tmp1E1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E2="unresolved DatatypeFieldType";_tag_dyneither(_tmp1E2,sizeof(char),29);}),_tag_dyneither(_tmp1E1,sizeof(void*),0));});}case 5: _LLA8: _tmp20F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->f1).elt_typ;_tmp20E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->f1).elt_tq;_tmp20D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->f1).ptr_atts).bounds;_LLA9:
# 819
 _tmp20F=Cyc_Toc_typ_to_c(_tmp20F);{
void*_tmp1E3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp20D);void*_tmp1E4=_tmp1E3;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1E4)->tag == 0){_LLDD: _LLDE:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LLDF: _LLE0:
 return Cyc_Absyn_star_typ(_tmp20F,(void*)& Cyc_Absyn_HeapRgn_val,_tmp20E,Cyc_Absyn_false_conref);}_LLDC:;};case 6: _LLAA: _LLAB:
# 824
 goto _LLAD;case 7: _LLAC: _LLAD:
 return t;case 8: _LLAE: _tmp213=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E0)->f1).elt_type;_tmp212=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E0)->f1).tq;_tmp211=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E0)->f1).num_elts;_tmp210=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E0)->f1).zt_loc;_LLAF:
# 827
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp213),_tmp212,_tmp211,Cyc_Absyn_false_conref,_tmp210);case 9: _LLB0: _tmp219=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E0)->f1).ret_tqual;_tmp218=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E0)->f1).ret_typ;_tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E0)->f1).args;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E0)->f1).c_varargs;_tmp215=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E0)->f1).cyc_varargs;_tmp214=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E0)->f1).attributes;_LLB1: {
# 833
struct Cyc_List_List*_tmp1E5=0;
for(0;_tmp214 != 0;_tmp214=_tmp214->tl){
void*_tmp1E6=(void*)_tmp214->hd;void*_tmp1E7=_tmp1E6;switch(*((int*)_tmp1E7)){case 4: _LLE2: _LLE3:
 goto _LLE5;case 5: _LLE4: _LLE5:
 goto _LLE7;case 19: _LLE6: _LLE7:
 continue;case 22: _LLE8: _LLE9:
 continue;case 21: _LLEA: _LLEB:
 continue;case 20: _LLEC: _LLED:
 continue;default: _LLEE: _LLEF:
 _tmp1E5=({struct Cyc_List_List*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->hd=(void*)_tmp214->hd;_tmp1E8->tl=_tmp1E5;_tmp1E8;});goto _LLE1;}_LLE1:;}{
# 844
struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp217);
if(_tmp215 != 0){
# 847
void*_tmp1EA=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp215->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp1EB=({struct _tuple9*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->f1=_tmp215->name;_tmp1ED->f2=_tmp215->tq;_tmp1ED->f3=_tmp1EA;_tmp1ED;});
_tmp1E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1E9,({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->hd=_tmp1EB;_tmp1EC->tl=0;_tmp1EC;}));}
# 851
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1EF;_tmp1EF.tag=9;_tmp1EF.f1=({struct Cyc_Absyn_FnInfo _tmp1F0;_tmp1F0.tvars=0;_tmp1F0.effect=0;_tmp1F0.ret_tqual=_tmp219;_tmp1F0.ret_typ=Cyc_Toc_typ_to_c(_tmp218);_tmp1F0.args=_tmp1E9;_tmp1F0.c_varargs=_tmp216;_tmp1F0.cyc_varargs=0;_tmp1F0.rgn_po=0;_tmp1F0.attributes=_tmp1E5;_tmp1F0.requires_clause=0;_tmp1F0.requires_relns=0;_tmp1F0.ensures_clause=0;_tmp1F0.ensures_relns=0;_tmp1F0;});_tmp1EF;});_tmp1EE;});};}case 10: _LLB2: _tmp21A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1E0)->f1;_LLB3:
# 856
 _tmp21A=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp21A);
return Cyc_Toc_add_tuple_type(_tmp21A);case 12: _LLB4: _tmp21C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1E0)->f1;_tmp21B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1E0)->f2;_LLB5:
# 861
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1F2;_tmp1F2.tag=12;_tmp1F2.f1=_tmp21C;_tmp1F2.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp21B);_tmp1F2;});_tmp1F1;});case 11: _LLB6: _tmp21E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1E0)->f1).aggr_info;_tmp21D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1E0)->f1).targs;_LLB7:
# 865
{union Cyc_Absyn_AggrInfoU _tmp1F3=_tmp21E;if((_tmp1F3.UnknownAggr).tag == 1){_LLF1: _LLF2:
 return t;}else{_LLF3: _LLF4:
 goto _LLF0;}_LLF0:;}{
# 869
struct Cyc_Absyn_Aggrdecl*_tmp1F4=Cyc_Absyn_get_known_aggrdecl(_tmp21E);
if(_tmp1F4->expected_mem_kind){
# 872
if(_tmp1F4->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1F8;_tmp1F8.tag=0;_tmp1F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 875
Cyc_Absynpp_qvar2string(_tmp1F4->name));({struct Cyc_String_pa_PrintArg_struct _tmp1F7;_tmp1F7.tag=0;_tmp1F7.f1=(struct _dyneither_ptr)(
# 874
_tmp1F4->kind == Cyc_Absyn_UnionA?({const char*_tmp1F9="union";_tag_dyneither(_tmp1F9,sizeof(char),6);}):({const char*_tmp1FA="struct";_tag_dyneither(_tmp1FA,sizeof(char),7);}));({void*_tmp1F5[2]={& _tmp1F7,& _tmp1F8};Cyc_Tcutil_warn(0,({const char*_tmp1F6="%s %s was never defined.";_tag_dyneither(_tmp1F6,sizeof(char),25);}),_tag_dyneither(_tmp1F5,sizeof(void*),2));});});});}
# 880
if(_tmp1F4->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1F4->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1FB=_tmp1F4->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F4->impl))->fields;
if(_tmp1FB == 0)return Cyc_Toc_aggrdecl_type(_tmp1F4->name,Cyc_Absyn_strctq);
for(0;_tmp1FB->tl != 0;_tmp1FB=_tmp1FB->tl){;}{
void*_tmp1FC=((struct Cyc_Absyn_Aggrfield*)_tmp1FB->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1FC))){
if(_tmp1F4->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp1FF;_tmp1FF.tag=0;_tmp1FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1F4->name));({void*_tmp1FD[1]={& _tmp1FF};Cyc_Tcutil_warn(0,({const char*_tmp1FE="struct %s ended up being abstract.";_tag_dyneither(_tmp1FE,sizeof(char),35);}),_tag_dyneither(_tmp1FD,sizeof(void*),1));});});{
# 894
struct _RegionHandle _tmp200=_new_region("r");struct _RegionHandle*r=& _tmp200;_push_region(r);
{struct Cyc_List_List*_tmp201=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1F4->tvs,_tmp21D);
void*_tmp202=Cyc_Tcutil_rsubstitute(r,_tmp201,_tmp1FC);
if(Cyc_Toc_is_abstract_type(_tmp202)){void*_tmp203=Cyc_Toc_aggrdecl_type(_tmp1F4->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp203;}{
void*_tmp204=Cyc_Toc_add_struct_type(_tmp1F4->name,_tmp1F4->tvs,_tmp21D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F4->impl))->fields);_npop_handler(0);return _tmp204;};}
# 895
;_pop_region(r);};}
# 900
return Cyc_Toc_aggrdecl_type(_tmp1F4->name,Cyc_Absyn_strctq);};};};case 13: _LLB8: _tmp21F=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1E0)->f1;_LLB9:
 return t;case 14: _LLBA: _tmp220=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1E0)->f1;_LLBB:
 Cyc_Toc_enumfields_to_c(_tmp220);return t;case 17: _LLBC: _tmp224=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1E0)->f1;_tmp223=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1E0)->f2;_tmp222=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1E0)->f3;_tmp221=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1E0)->f4;_LLBD:
# 904
 if(_tmp221 == 0  || Cyc_noexpand_r){
if(_tmp223 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp206;_tmp206.tag=17;_tmp206.f1=_tmp224;_tmp206.f2=0;_tmp206.f3=_tmp222;_tmp206.f4=0;_tmp206;});_tmp205;});else{
return t;}}else{
# 909
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp208;_tmp208.tag=17;_tmp208.f1=_tmp224;_tmp208.f2=0;_tmp208.f3=_tmp222;_tmp208.f4=Cyc_Toc_typ_to_c(_tmp221);_tmp208;});_tmp207;});}case 19: _LLBE: _LLBF:
 return Cyc_Absyn_uint_typ;case 15: _LLC0: _LLC1:
 return Cyc_Toc_rgn_typ();case 16: _LLC2: _LLC3:
 return Cyc_Toc_dyn_rgn_typ();case 20: _LLC4: _LLC5:
# 915
 goto _LLC7;case 21: _LLC6: _LLC7:
 goto _LLC9;case 22: _LLC8: _LLC9:
 goto _LLCB;case 23: _LLCA: _LLCB:
 goto _LLCD;case 24: _LLCC: _LLCD:
 goto _LLCF;case 25: _LLCE: _LLCF:
 return Cyc_Absyn_void_star_typ();case 18: _LLD0: _tmp225=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1E0)->f1;_LLD1:
# 925
 return t;case 27: _LLD2: _tmp226=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1E0)->f1;_LLD3:
# 927
 return t;case 28: _LLD4: _LLD5:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1E0)->f1)->r)){case 0: _LLD6: _tmp227=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1E0)->f1)->r)->f1;_LLD7:
# 930
 Cyc_Toc_aggrdecl_to_c(_tmp227,1);
if(_tmp227->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp227->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp227->name,Cyc_Absyn_strctq);}case 1: _LLD8: _tmp228=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1E0)->f1)->r)->f1;_LLD9:
# 935
 Cyc_Toc_enumdecl_to_c(_tmp228);
return t;default: _LLDA: _tmp22A=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1E0)->f1)->r)->f1;_tmp229=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1E0)->f2;_LLDB:
# 938
 Cyc_Toc_datatypedecl_to_c(_tmp22A);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp229)));}}_LL9B:;}
# 943
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp22B=t;void*_tmp22D;struct Cyc_Absyn_Tqual _tmp22C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22B)->tag == 8){_LLF6: _tmp22D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22B)->f1).elt_type;_tmp22C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22B)->f1).tq;_LLF7:
# 946
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp22D,(void*)& Cyc_Absyn_HeapRgn_val,_tmp22C,Cyc_Absyn_false_conref),e);}else{_LLF8: _LLF9:
 return Cyc_Toc_cast_it(t,e);}_LLF5:;}
# 953
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp22E=Cyc_Tcutil_compress(t);void*_tmp22F=_tmp22E;struct Cyc_List_List*_tmp23B;struct Cyc_Absyn_Datatypedecl*_tmp23A;struct Cyc_Absyn_Datatypefield*_tmp239;struct Cyc_List_List*_tmp238;union Cyc_Absyn_AggrInfoU _tmp237;void*_tmp236;switch(*((int*)_tmp22F)){case 0: _LLFB: _LLFC:
 return 1;case 2: _LLFD: _LLFE:
 return 0;case 6: _LLFF: _LL100:
 goto _LL102;case 13: _LL101: _LL102:
 goto _LL104;case 14: _LL103: _LL104:
 goto _LL106;case 7: _LL105: _LL106:
 goto _LL108;case 9: _LL107: _LL108:
 goto _LL10A;case 19: _LL109: _LL10A:
 return 1;case 8: _LL10B: _tmp236=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22F)->f1).elt_type;_LL10C:
 return Cyc_Toc_atomic_typ(_tmp236);case 11: _LL10D: _tmp237=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp22F)->f1).aggr_info;_LL10E:
# 970
{union Cyc_Absyn_AggrInfoU _tmp230=_tmp237;if((_tmp230.UnknownAggr).tag == 1){_LL120: _LL121:
 return 0;}else{_LL122: _LL123:
 goto _LL11F;}_LL11F:;}{
# 974
struct Cyc_Absyn_Aggrdecl*_tmp231=Cyc_Absyn_get_known_aggrdecl(_tmp237);
if(_tmp231->impl == 0)
return 0;
{struct Cyc_List_List*_tmp232=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp231->impl))->fields;for(0;_tmp232 != 0;_tmp232=_tmp232->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp232->hd)->type))return 0;}}
return 1;};case 12: _LL10F: _tmp238=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp22F)->f2;_LL110:
# 981
 for(0;_tmp238 != 0;_tmp238=_tmp238->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp238->hd)->type))return 0;}
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp22F)->f1).field_info).KnownDatatypefield).tag == 2){_LL111: _tmp23A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp22F)->f1).field_info).KnownDatatypefield).val).f1;_tmp239=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp22F)->f1).field_info).KnownDatatypefield).val).f2;_LL112:
# 985
 _tmp23B=_tmp239->typs;goto _LL114;}else{goto _LL11D;}case 10: _LL113: _tmp23B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp22F)->f1;_LL114:
# 987
 for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp23B->hd)).f2))return 0;}
return 1;case 3: _LL115: _LL116:
# 992
 goto _LL118;case 5: _LL117: _LL118:
 goto _LL11A;case 16: _LL119: _LL11A:
 goto _LL11C;case 15: _LL11B: _LL11C:
 return 0;default: _LL11D: _LL11E:
({struct Cyc_String_pa_PrintArg_struct _tmp235;_tmp235.tag=0;_tmp235.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp233[1]={& _tmp235};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp234="atomic_typ:  bad type %s";_tag_dyneither(_tmp234,sizeof(char),25);}),_tag_dyneither(_tmp233,sizeof(void*),1));});});}_LLFA:;}
# 1000
static int Cyc_Toc_is_void_star(void*t){
void*_tmp23C=Cyc_Tcutil_compress(t);void*_tmp23D=_tmp23C;void*_tmp240;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23D)->tag == 5){_LL125: _tmp240=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23D)->f1).elt_typ;_LL126: {
# 1003
void*_tmp23E=Cyc_Tcutil_compress(_tmp240);void*_tmp23F=_tmp23E;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp23F)->tag == 0){_LL12A: _LL12B:
 return 1;}else{_LL12C: _LL12D:
 return 0;}_LL129:;}}else{_LL127: _LL128:
# 1007
 return 0;}_LL124:;}
# 1011
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1015
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1020
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp241=Cyc_Tcutil_compress(t);void*_tmp242=_tmp241;struct Cyc_List_List*_tmp24E;union Cyc_Absyn_AggrInfoU _tmp24D;switch(*((int*)_tmp242)){case 11: _LL12F: _tmp24D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp242)->f1).aggr_info;_LL130: {
# 1023
struct Cyc_Absyn_Aggrdecl*_tmp243=Cyc_Absyn_get_known_aggrdecl(_tmp24D);
if(_tmp243->impl == 0)
({void*_tmp244=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp245="is_poly_field: type missing fields";_tag_dyneither(_tmp245,sizeof(char),35);}),_tag_dyneither(_tmp244,sizeof(void*),0));});
_tmp24E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp243->impl))->fields;goto _LL132;}case 12: _LL131: _tmp24E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp242)->f2;_LL132: {
# 1028
struct Cyc_Absyn_Aggrfield*_tmp246=Cyc_Absyn_lookup_field(_tmp24E,f);
if(_tmp246 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp249;_tmp249.tag=0;_tmp249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp247[1]={& _tmp249};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp248="is_poly_field: bad field %s";_tag_dyneither(_tmp248,sizeof(char),28);}),_tag_dyneither(_tmp247,sizeof(void*),1));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp246->type);}default: _LL133: _LL134:
({struct Cyc_String_pa_PrintArg_struct _tmp24C;_tmp24C.tag=0;_tmp24C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp24A[1]={& _tmp24C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp24B="is_poly_field: bad type %s";_tag_dyneither(_tmp24B,sizeof(char),27);}),_tag_dyneither(_tmp24A,sizeof(void*),1));});});}_LL12E:;}
# 1039
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp24F=e->r;void*_tmp250=_tmp24F;struct Cyc_Absyn_Exp*_tmp25A;struct _dyneither_ptr*_tmp259;struct Cyc_Absyn_Exp*_tmp258;struct _dyneither_ptr*_tmp257;switch(*((int*)_tmp250)){case 20: _LL136: _tmp258=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp250)->f1;_tmp257=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp250)->f2;_LL137:
# 1042
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp258->topt),_tmp257) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 21: _LL138: _tmp25A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp250)->f1;_tmp259=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp250)->f2;_LL139: {
# 1045
void*_tmp251=Cyc_Tcutil_compress((void*)_check_null(_tmp25A->topt));void*_tmp252=_tmp251;void*_tmp256;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp252)->tag == 5){_LL13D: _tmp256=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp252)->f1).elt_typ;_LL13E:
# 1047
 return Cyc_Toc_is_poly_field(_tmp256,_tmp259) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LL13F: _LL140:
({struct Cyc_String_pa_PrintArg_struct _tmp255;_tmp255.tag=0;_tmp255.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp25A->topt)));({void*_tmp253[1]={& _tmp255};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp254="is_poly_project: bad type %s";_tag_dyneither(_tmp254,sizeof(char),29);}),_tag_dyneither(_tmp253,sizeof(void*),1));});});}_LL13C:;}default: _LL13A: _LL13B:
# 1050
 return 0;}_LL135:;}
# 1055
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B->hd=s;_tmp25B->tl=0;_tmp25B;}),0);}
# 1059
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp25C=_cycalloc(sizeof(*_tmp25C));_tmp25C->hd=s;_tmp25C->tl=0;_tmp25C;}),0);}
# 1063
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1069
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp25D[2];_tmp25D[1]=s;_tmp25D[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25D,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1073
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__fast_region_malloc_e,({struct Cyc_Absyn_Exp*_tmp25E[2];_tmp25E[1]=s;_tmp25E[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25E,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1077
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp25F[2];_tmp25F[1]=n;_tmp25F[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25F,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{
# 1081
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp260[2];_tmp260[1]=n;_tmp260[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp260,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}
# 1084
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp261[3];_tmp261[2]=n;_tmp261[1]=s;_tmp261[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp261,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
# 1089
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262->hd=e;_tmp262->tl=0;_tmp262;}),0);}
# 1092
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,({struct Cyc_List_List*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->hd=e;_tmp263->tl=0;_tmp263;}),0);}
# 1096
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,({struct Cyc_Absyn_Exp*_tmp264[0];((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp264,sizeof(struct Cyc_Absyn_Exp*),0));}),0),0);}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1110
int is_string=0;
{void*_tmp265=e->r;void*_tmp266=_tmp265;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp266)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp266)->f1).Wstring_c).tag){case 8: _LL142: _LL143:
 is_string=1;goto _LL141;case 9: _LL144: _LL145:
 is_string=1;goto _LL141;default: goto _LL146;}else{_LL146: _LL147:
 goto _LL141;}_LL141:;}{
# 1116
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp269;_tmp269.tag=0;_tmp269.f1=vd;_tmp269;});_tmp268;}),0);_tmp267->tl=Cyc_Toc_result_decls;_tmp267;});
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1126
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1128
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1130
struct Cyc_Absyn_Exp*urm_exp;
urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp26A[3];_tmp26A[2]=({struct _tuple19*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->f1=0;_tmp26D->f2=xplussz;_tmp26D;});_tmp26A[1]=({struct _tuple19*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->f1=0;_tmp26C->f2=xexp;_tmp26C;});_tmp26A[0]=({struct _tuple19*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->f1=0;_tmp26B->f2=xexp;_tmp26B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26A,sizeof(struct _tuple19*),3));}),0);
# 1134
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1173 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1175
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp26E=nv;int _tmp26F;_LL149: _tmp26F=_tmp26E->toplevel;_LL14A:;
return _tmp26F;}
# 1179
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp270=nv;int*_tmp271;_LL14C: _tmp271=_tmp270->in_lhs;_LL14D:;
return*_tmp271;}
# 1185
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp272=_region_malloc(r,sizeof(*_tmp272));_tmp272->break_lab=(struct _dyneither_ptr**)0;_tmp272->continue_lab=(struct _dyneither_ptr**)0;_tmp272->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp272->toplevel=(int)1;_tmp272->in_lhs=(int*)({int*_tmp275=_region_malloc(r,sizeof(*_tmp275));_tmp275[0]=0;_tmp275;});_tmp272->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp272->struct_info).varsizeexp=({struct Cyc_Absyn_Exp**_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274[0]=0;_tmp274;});(_tmp272->struct_info).lhs_exp=0;_tmp272->struct_info;});_tmp272->in_sizeof=(int*)({int*_tmp273=_region_malloc(r,sizeof(*_tmp273));_tmp273[0]=0;_tmp273;});_tmp272->rgn=(struct _RegionHandle*)r;_tmp272;});}
# 1196
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp276=e;struct _dyneither_ptr**_tmp27E;struct _dyneither_ptr**_tmp27D;struct Cyc_Toc_FallthruInfo*_tmp27C;int _tmp27B;int*_tmp27A;struct Cyc_Toc_StructInfo _tmp279;int*_tmp278;_LL14F: _tmp27E=_tmp276->break_lab;_tmp27D=_tmp276->continue_lab;_tmp27C=_tmp276->fallthru_info;_tmp27B=_tmp276->toplevel;_tmp27A=_tmp276->in_lhs;_tmp279=_tmp276->struct_info;_tmp278=_tmp276->in_sizeof;_LL150:;
return({struct Cyc_Toc_Env*_tmp277=_region_malloc(r,sizeof(*_tmp277));_tmp277->break_lab=(struct _dyneither_ptr**)_tmp27E;_tmp277->continue_lab=(struct _dyneither_ptr**)_tmp27D;_tmp277->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp27C;_tmp277->toplevel=(int)_tmp27B;_tmp277->in_lhs=(int*)_tmp27A;_tmp277->struct_info=(struct Cyc_Toc_StructInfo)_tmp279;_tmp277->in_sizeof=(int*)_tmp278;_tmp277->rgn=(struct _RegionHandle*)r;_tmp277;});}
# 1201
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp27F=e;struct _dyneither_ptr**_tmp287;struct _dyneither_ptr**_tmp286;struct Cyc_Toc_FallthruInfo*_tmp285;int _tmp284;int*_tmp283;struct Cyc_Toc_StructInfo _tmp282;int*_tmp281;_LL152: _tmp287=_tmp27F->break_lab;_tmp286=_tmp27F->continue_lab;_tmp285=_tmp27F->fallthru_info;_tmp284=_tmp27F->toplevel;_tmp283=_tmp27F->in_lhs;_tmp282=_tmp27F->struct_info;_tmp281=_tmp27F->in_sizeof;_LL153:;
return({struct Cyc_Toc_Env*_tmp280=_region_malloc(r,sizeof(*_tmp280));_tmp280->break_lab=(struct _dyneither_ptr**)_tmp287;_tmp280->continue_lab=(struct _dyneither_ptr**)_tmp286;_tmp280->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp285;_tmp280->toplevel=(int)0;_tmp280->in_lhs=(int*)_tmp283;_tmp280->struct_info=(struct Cyc_Toc_StructInfo)_tmp282;_tmp280->in_sizeof=(int*)_tmp281;_tmp280->rgn=(struct _RegionHandle*)r;_tmp280;});}
# 1205
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp288=e;struct _dyneither_ptr**_tmp290;struct _dyneither_ptr**_tmp28F;struct Cyc_Toc_FallthruInfo*_tmp28E;int _tmp28D;int*_tmp28C;struct Cyc_Toc_StructInfo _tmp28B;int*_tmp28A;_LL155: _tmp290=_tmp288->break_lab;_tmp28F=_tmp288->continue_lab;_tmp28E=_tmp288->fallthru_info;_tmp28D=_tmp288->toplevel;_tmp28C=_tmp288->in_lhs;_tmp28B=_tmp288->struct_info;_tmp28A=_tmp288->in_sizeof;_LL156:;
return({struct Cyc_Toc_Env*_tmp289=_region_malloc(r,sizeof(*_tmp289));_tmp289->break_lab=(struct _dyneither_ptr**)_tmp290;_tmp289->continue_lab=(struct _dyneither_ptr**)_tmp28F;_tmp289->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp28E;_tmp289->toplevel=(int)1;_tmp289->in_lhs=(int*)_tmp28C;_tmp289->struct_info=(struct Cyc_Toc_StructInfo)_tmp28B;_tmp289->in_sizeof=(int*)_tmp28A;_tmp289->rgn=(struct _RegionHandle*)r;_tmp289;});}
# 1209
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp291=e;int*_tmp292;_LL158: _tmp292=_tmp291->in_lhs;_LL159:;
*_tmp292=b;}
# 1213
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp293=e;int*_tmp295;_LL15B: _tmp295=_tmp293->in_sizeof;_LL15C:;{
int _tmp294=*_tmp295;
*_tmp295=b;
return _tmp294;};}
# 1219
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp296=e;int*_tmp297;_LL15E: _tmp297=_tmp296->in_sizeof;_LL15F:;
return*_tmp297;}
# 1224
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1226
struct Cyc_Toc_Env*_tmp298=e;struct _dyneither_ptr**_tmp2A0;struct _dyneither_ptr**_tmp29F;struct Cyc_Toc_FallthruInfo*_tmp29E;int _tmp29D;int*_tmp29C;struct Cyc_Absyn_Exp**_tmp29B;int*_tmp29A;_LL161: _tmp2A0=_tmp298->break_lab;_tmp29F=_tmp298->continue_lab;_tmp29E=_tmp298->fallthru_info;_tmp29D=_tmp298->toplevel;_tmp29C=_tmp298->in_lhs;_tmp29B=(_tmp298->struct_info).varsizeexp;_tmp29A=_tmp298->in_sizeof;_LL162:;
return({struct Cyc_Toc_Env*_tmp299=_region_malloc(r,sizeof(*_tmp299));_tmp299->break_lab=(struct _dyneither_ptr**)_tmp2A0;_tmp299->continue_lab=(struct _dyneither_ptr**)_tmp29F;_tmp299->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp29E;_tmp299->toplevel=(int)_tmp29D;_tmp299->in_lhs=(int*)_tmp29C;_tmp299->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp299->struct_info).varsizeexp=_tmp29B;(_tmp299->struct_info).lhs_exp=exp;_tmp299->struct_info;});_tmp299->in_sizeof=(int*)_tmp29A;_tmp299->rgn=(struct _RegionHandle*)r;_tmp299;});}
# 1232
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp2A1=e;struct _dyneither_ptr**_tmp2A9;struct _dyneither_ptr**_tmp2A8;struct Cyc_Toc_FallthruInfo*_tmp2A7;int _tmp2A6;int*_tmp2A5;struct Cyc_Toc_StructInfo _tmp2A4;int*_tmp2A3;_LL164: _tmp2A9=_tmp2A1->break_lab;_tmp2A8=_tmp2A1->continue_lab;_tmp2A7=_tmp2A1->fallthru_info;_tmp2A6=_tmp2A1->toplevel;_tmp2A5=_tmp2A1->in_lhs;_tmp2A4=_tmp2A1->struct_info;_tmp2A3=_tmp2A1->in_sizeof;_LL165:;
return({struct Cyc_Toc_Env*_tmp2A2=_region_malloc(r,sizeof(*_tmp2A2));_tmp2A2->break_lab=(struct _dyneither_ptr**)0;_tmp2A2->continue_lab=(struct _dyneither_ptr**)0;_tmp2A2->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2A7;_tmp2A2->toplevel=(int)_tmp2A6;_tmp2A2->in_lhs=(int*)_tmp2A5;_tmp2A2->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A4;_tmp2A2->in_sizeof=(int*)_tmp2A3;_tmp2A2->rgn=(struct _RegionHandle*)r;_tmp2A2;});}
# 1238
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1242
struct Cyc_Toc_Env*_tmp2AA=e;struct _dyneither_ptr**_tmp2B4;struct _dyneither_ptr**_tmp2B3;struct Cyc_Toc_FallthruInfo*_tmp2B2;int _tmp2B1;int*_tmp2B0;struct Cyc_Toc_StructInfo _tmp2AF;int*_tmp2AE;_LL167: _tmp2B4=_tmp2AA->break_lab;_tmp2B3=_tmp2AA->continue_lab;_tmp2B2=_tmp2AA->fallthru_info;_tmp2B1=_tmp2AA->toplevel;_tmp2B0=_tmp2AA->in_lhs;_tmp2AF=_tmp2AA->struct_info;_tmp2AE=_tmp2AA->in_sizeof;_LL168:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp2AD=_region_malloc(r,sizeof(*_tmp2AD));_tmp2AD->label=fallthru_l;_tmp2AD->binders=fallthru_binders;_tmp2AD;});
# 1245
return({struct Cyc_Toc_Env*_tmp2AB=_region_malloc(r,sizeof(*_tmp2AB));_tmp2AB->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2AC=_region_malloc(r,sizeof(*_tmp2AC));_tmp2AC[0]=break_l;_tmp2AC;});_tmp2AB->continue_lab=(struct _dyneither_ptr**)_tmp2B3;_tmp2AB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi;_tmp2AB->toplevel=(int)_tmp2B1;_tmp2AB->in_lhs=(int*)_tmp2B0;_tmp2AB->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AF;_tmp2AB->in_sizeof=(int*)_tmp2AE;_tmp2AB->rgn=(struct _RegionHandle*)r;_tmp2AB;});};}
# 1248
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1250
struct Cyc_Toc_Env*_tmp2B5=e;struct _dyneither_ptr**_tmp2BE;struct _dyneither_ptr**_tmp2BD;struct Cyc_Toc_FallthruInfo*_tmp2BC;int _tmp2BB;int*_tmp2BA;struct Cyc_Toc_StructInfo _tmp2B9;int*_tmp2B8;_LL16A: _tmp2BE=_tmp2B5->break_lab;_tmp2BD=_tmp2B5->continue_lab;_tmp2BC=_tmp2B5->fallthru_info;_tmp2BB=_tmp2B5->toplevel;_tmp2BA=_tmp2B5->in_lhs;_tmp2B9=_tmp2B5->struct_info;_tmp2B8=_tmp2B5->in_sizeof;_LL16B:;
return({struct Cyc_Toc_Env*_tmp2B6=_region_malloc(r,sizeof(*_tmp2B6));_tmp2B6->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2B7=_region_malloc(r,sizeof(*_tmp2B7));_tmp2B7[0]=break_l;_tmp2B7;});_tmp2B6->continue_lab=(struct _dyneither_ptr**)_tmp2BD;_tmp2B6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp2B6->toplevel=(int)_tmp2BB;_tmp2B6->in_lhs=(int*)_tmp2BA;_tmp2B6->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B9;_tmp2B6->in_sizeof=(int*)_tmp2B8;_tmp2B6->rgn=(struct _RegionHandle*)r;_tmp2B6;});}
# 1257
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1260
struct Cyc_Toc_Env*_tmp2BF=e;struct _dyneither_ptr**_tmp2C8;struct _dyneither_ptr**_tmp2C7;struct Cyc_Toc_FallthruInfo*_tmp2C6;int _tmp2C5;int*_tmp2C4;struct Cyc_Toc_StructInfo _tmp2C3;int*_tmp2C2;_LL16D: _tmp2C8=_tmp2BF->break_lab;_tmp2C7=_tmp2BF->continue_lab;_tmp2C6=_tmp2BF->fallthru_info;_tmp2C5=_tmp2BF->toplevel;_tmp2C4=_tmp2BF->in_lhs;_tmp2C3=_tmp2BF->struct_info;_tmp2C2=_tmp2BF->in_sizeof;_LL16E:;
return({struct Cyc_Toc_Env*_tmp2C0=_region_malloc(r,sizeof(*_tmp2C0));_tmp2C0->break_lab=(struct _dyneither_ptr**)0;_tmp2C0->continue_lab=(struct _dyneither_ptr**)_tmp2C7;_tmp2C0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2C1=_region_malloc(r,sizeof(*_tmp2C1));_tmp2C1->label=next_l;_tmp2C1->binders=0;_tmp2C1;});_tmp2C0->toplevel=(int)_tmp2C5;_tmp2C0->in_lhs=(int*)_tmp2C4;_tmp2C0->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C3;_tmp2C0->in_sizeof=(int*)_tmp2C2;_tmp2C0->rgn=(struct _RegionHandle*)r;_tmp2C0;});}
# 1273 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1276
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1278
void*_tmp2C9=e->annot;void*_tmp2CA=_tmp2C9;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2CA)->tag == Cyc_CfFlowInfo_UnknownZ){_LL170: _LL171:
# 1281
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2CA)->tag == Cyc_CfFlowInfo_NotZero){_LL172: _LL173:
# 1284
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2CA)->tag == Cyc_CfFlowInfo_IsZero){_LL174: _LL175:
# 1286
({void*_tmp2CB=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2CC="dereference of NULL pointer";_tag_dyneither(_tmp2CC,sizeof(char),28);}),_tag_dyneither(_tmp2CB,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2CA)->tag == Cyc_Absyn_EmptyAnnot){_LL176: _LL177:
# 1292
 return 1;}else{_LL178: _LL179:
({void*_tmp2CD=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2CE="need_null_check";_tag_dyneither(_tmp2CE,sizeof(char),16);}),_tag_dyneither(_tmp2CD,sizeof(void*),0));});}}}}_LL16F:;}
# 1297
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2CF=e->annot;void*_tmp2D0=_tmp2CF;struct Cyc_List_List*_tmp2D6;struct Cyc_List_List*_tmp2D5;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_CfFlowInfo_UnknownZ){_LL17B: _tmp2D5=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2D0)->f1;_LL17C:
 return _tmp2D5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_CfFlowInfo_NotZero){_LL17D: _tmp2D6=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2D0)->f1;_LL17E:
 return _tmp2D6;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_CfFlowInfo_IsZero){_LL17F: _LL180:
# 1302
({void*_tmp2D1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2D2="dereference of NULL pointer";_tag_dyneither(_tmp2D2,sizeof(char),28);}),_tag_dyneither(_tmp2D1,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2D0)->tag == Cyc_Absyn_EmptyAnnot){_LL181: _LL182:
 return 0;}else{_LL183: _LL184:
({void*_tmp2D3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2D4="get_relns";_tag_dyneither(_tmp2D4,sizeof(char),10);}),_tag_dyneither(_tmp2D3,sizeof(void*),0));});}}}}_LL17A:;}static char _tmp2DA[8]="*bogus*";
# 1312
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1322 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1325
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2D7=a->r;void*_tmp2D8=_tmp2D7;void*_tmp2DB;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D8)->tag == 1){_LL186: _tmp2DB=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL187: {
# 1328
struct Cyc_Absyn_Vardecl*_tmp2D9=Cyc_Tcutil_nonesc_vardecl(_tmp2DB);
if(_tmp2D9 == 0)goto _LL189;
x=_tmp2D9;
goto _LL185;}}else{_LL188: _LL189: {
# 1334
static struct _dyneither_ptr bogus_string={_tmp2DA,_tmp2DA,_tmp2DA + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1337
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1343
x=& bogus_vardecl;
x->type=a_typ;}}_LL185:;}{
# 1346
void*_tmp2DC=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1351
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2DC);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1360
if(valid_rop_i){
struct Cyc_List_List*_tmp2DD=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1368
if(!Cyc_Relations_consistent_relations(_tmp2DD))return 1;}
# 1371
inner_loop: {
void*_tmp2DE=i->r;void*_tmp2DF=_tmp2DE;struct Cyc_Absyn_Exp*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E3;void*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E1;switch(*((int*)_tmp2DF)){case 13: _LL18B: _tmp2E2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1;_tmp2E1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2;_LL18C:
 i=_tmp2E1;goto inner_loop;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2)->tl != 0){_LL18D: _tmp2E4=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2)->hd;_tmp2E3=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2DF)->f2)->tl)->hd;_LL18E: {
# 1377
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp2E3,& rop_z)){
# 1380
struct Cyc_List_List*_tmp2E0=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1382
return !Cyc_Relations_consistent_relations(_tmp2E0);}
# 1384
goto _LL18A;}}else{goto _LL18F;}}else{goto _LL18F;}}else{goto _LL18F;}default: _LL18F: _LL190:
 goto _LL18A;}_LL18A:;}
# 1387
return 0;};}
# 1390
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2E5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E6="Missing type in primop ";_tag_dyneither(_tmp2E6,sizeof(char),24);}),_tag_dyneither(_tmp2E5,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1394
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2E7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E8="Missing type in primop ";_tag_dyneither(_tmp2E8,sizeof(char),24);}),_tag_dyneither(_tmp2E7,sizeof(void*),0));});
return(void*)_check_null(e->topt);}
# 1398
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple11*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9->f1=Cyc_Toc_mt_tq;_tmp2E9->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp2E9;});}
# 1401
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);
return({struct _tuple19*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA->f1=0;_tmp2EA->f2=e;_tmp2EA;});}
# 1408
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1411
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2EB=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp2EB=Cyc_Absyn_add_exp(_tmp2EB,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1419
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp2EB): Cyc_Toc_malloc_ptr(_tmp2EB);else{
# 1423
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp2EB);}}else{
# 1428
t=struct_typ;
eo=0;}
# 1431
if(do_declare){
void*_tmp2EC=x->r;void*_tmp2ED=_tmp2EC;void*_tmp2F0;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2ED)->tag == 1){_LL192: _tmp2F0=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1;_LL193:
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(Cyc_Absyn_binding2qvar(_tmp2F0),t,eo,s,0),0);}else{_LL194: _LL195:
({void*_tmp2EE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2EF="bogus x to make_struct";_tag_dyneither(_tmp2EF,sizeof(char),23);}),_tag_dyneither(_tmp2EE,sizeof(void*),0));});}_LL191:;}else{
# 1438
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1441
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1445
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp2F1=Cyc_Tcutil_compress(typ);void*_tmp2F2=_tmp2F1;struct Cyc_List_List*_tmp2F5;union Cyc_Absyn_AggrInfoU _tmp2F4;struct Cyc_List_List*_tmp2F3;switch(*((int*)_tmp2F2)){case 11: _LL197: _tmp2F4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F2)->f1).aggr_info;_tmp2F3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F2)->f1).targs;_LL198: {
# 1448
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp2F4);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp2F5=aggrfields;goto _LL19A;};}case 12: _LL199: _tmp2F5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F2)->f2;_LL19A:
# 1453
 if(_tmp2F5 == 0)return 0;
return 1;default: _LL19B: _LL19C:
# 1459
 return 0;}_LL196:;}
# 1463
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp2F6=Cyc_Tcutil_compress(t);void*_tmp2F7=_tmp2F6;void*_tmp2FB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F7)->tag == 8){_LL19E: _tmp2FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F7)->f1).elt_type;_LL19F:
 return _tmp2FB;}else{_LL1A0: _LL1A1:
({struct Cyc_String_pa_PrintArg_struct _tmp2FA;_tmp2FA.tag=0;_tmp2FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp2F8[1]={& _tmp2FA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2F9="array_elt_type called on %s";_tag_dyneither(_tmp2F9,sizeof(char),28);}),_tag_dyneither(_tmp2F8,sizeof(void*),1));});});}_LL19D:;}
# 1470
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1476
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1481
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1486
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp2FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp2FC != 0;_tmp2FC=_tmp2FC->tl){
# 1490
struct _tuple19*_tmp2FD=(struct _tuple19*)_tmp2FC->hd;struct _tuple19*_tmp2FE=_tmp2FD;struct Cyc_List_List*_tmp310;struct Cyc_Absyn_Exp*_tmp30F;_LL1A3: _tmp310=_tmp2FE->f1;_tmp30F=_tmp2FE->f2;_LL1A4:;{
# 1494
struct Cyc_Absyn_Exp*e_index;
if(_tmp310 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1498
if(_tmp310->tl != 0)({void*_tmp2FF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp300="multiple designators in array";_tag_dyneither(_tmp300,sizeof(char),30);}),_tag_dyneither(_tmp2FF,sizeof(void*),0));});{
void*_tmp301=(void*)_tmp310->hd;
void*_tmp302=_tmp301;struct Cyc_Absyn_Exp*_tmp305;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp302)->tag == 0){_LL1A6: _tmp305=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp302)->f1;_LL1A7:
# 1502
 Cyc_Toc_exp_to_c(nv,_tmp305);
e_index=_tmp305;
goto _LL1A5;}else{_LL1A8: _LL1A9:
({void*_tmp303=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp304="field name designators in array";_tag_dyneither(_tmp304,sizeof(char),32);}),_tag_dyneither(_tmp303,sizeof(void*),0));});}_LL1A5:;};}{
# 1508
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp306=_tmp30F->r;void*_tmp307=_tmp306;void*_tmp30E;struct Cyc_List_List*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30C;struct Cyc_Absyn_Exp*_tmp30B;struct Cyc_Absyn_Exp*_tmp30A;int _tmp309;struct Cyc_List_List*_tmp308;switch(*((int*)_tmp307)){case 25: _LL1AB: _tmp308=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp307)->f1;_LL1AC:
# 1511
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp308,s);
goto _LL1AA;case 26: _LL1AD: _tmp30C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f1;_tmp30B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f2;_tmp30A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f3;_tmp309=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307)->f4;_LL1AE:
# 1514
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp30C,_tmp30B,_tmp30A,_tmp309,s,0);
# 1516
goto _LL1AA;case 29: _LL1AF: _tmp30E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp307)->f1;_tmp30D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp307)->f2;_LL1B0:
# 1518
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp30E,_tmp30D,s);
goto _LL1AA;case 27: _LL1B1: _LL1B2:
# 1521
 goto _LL1AA;default: _LL1B3: _LL1B4:
# 1523
 Cyc_Toc_exp_to_c(nv,_tmp30F);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp30F),0),s,0);
goto _LL1AA;}_LL1AA:;};};}}
# 1529
return s;}
# 1534
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1539
struct _tuple1*_tmp311=vd->name;
void*expected_elt_type;
{void*_tmp312=Cyc_Tcutil_compress(comprehension_type);void*_tmp313=_tmp312;void*_tmp317;void*_tmp316;switch(*((int*)_tmp313)){case 8: _LL1B6: _tmp316=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313)->f1).elt_type;_LL1B7:
# 1543
 expected_elt_type=_tmp316;goto _LL1B5;case 5: _LL1B8: _tmp317=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313)->f1).elt_typ;_LL1B9:
# 1545
 expected_elt_type=_tmp317;goto _LL1B5;default: _LL1BA: _LL1BB:
({void*_tmp314=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp315="init_comprehension passed a bad type!";_tag_dyneither(_tmp315,sizeof(char),38);}),_tag_dyneither(_tmp314,sizeof(void*),0));});}_LL1B5:;}{
# 1548
void*_tmp318=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1552
{void*_tmp319=e2->r;void*_tmp31A=_tmp319;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp31A)->tag == 27){_LL1BD: _LL1BE:
 return Cyc_Absyn_skip_stmt(0);}else{_LL1BF: _LL1C0:
 goto _LL1BC;}_LL1BC:;}{
# 1556
struct _RegionHandle _tmp31B=_new_region("r2");struct _RegionHandle*r2=& _tmp31B;_push_region(r2);
{struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp311,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp311,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp311,0),0);
# 1563
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp311,0),0);
struct Cyc_Absyn_Stmt*body;
# 1567
{void*_tmp31C=e2->r;void*_tmp31D=_tmp31C;void*_tmp324;struct Cyc_List_List*_tmp323;struct Cyc_Absyn_Vardecl*_tmp322;struct Cyc_Absyn_Exp*_tmp321;struct Cyc_Absyn_Exp*_tmp320;int _tmp31F;struct Cyc_List_List*_tmp31E;switch(*((int*)_tmp31D)){case 25: _LL1C2: _tmp31E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp31D)->f1;_LL1C3:
# 1569
 body=Cyc_Toc_init_array(nv,expected_elt_type,lval,_tmp31E,Cyc_Toc_skip_stmt_dl());
goto _LL1C1;case 26: _LL1C4: _tmp322=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f1;_tmp321=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f2;_tmp320=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f3;_tmp31F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D)->f4;_LL1C5:
# 1572
 body=Cyc_Toc_init_comprehension(nv,expected_elt_type,lval,_tmp322,_tmp321,_tmp320,_tmp31F,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1C1;case 29: _LL1C6: _tmp324=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp31D)->f1;_tmp323=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp31D)->f2;_LL1C7:
# 1575
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp324,_tmp323,Cyc_Toc_skip_stmt_dl());
goto _LL1C1;default: _LL1C8: _LL1C9:
# 1578
 Cyc_Toc_exp_to_c(nv,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL1C1;}_LL1C1:;}{
# 1582
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1584
if(zero_term){
# 1589
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp318,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1594
struct Cyc_Absyn_Stmt*_tmp325=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp311,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp325;};};}
# 1557
;_pop_region(r2);};};}
# 1600
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1603
{struct Cyc_List_List*_tmp326=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp326 != 0;_tmp326=_tmp326->tl){
struct _tuple19*_tmp327=(struct _tuple19*)_tmp326->hd;struct _tuple19*_tmp328=_tmp327;struct Cyc_List_List*_tmp33C;struct Cyc_Absyn_Exp*_tmp33B;_LL1CB: _tmp33C=_tmp328->f1;_tmp33B=_tmp328->f2;_LL1CC:;
if(_tmp33C == 0)
({void*_tmp329=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp32A="empty designator list";_tag_dyneither(_tmp32A,sizeof(char),22);}),_tag_dyneither(_tmp329,sizeof(void*),0));});
if(_tmp33C->tl != 0)
({void*_tmp32B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp32C="too many designators in anonymous struct";_tag_dyneither(_tmp32C,sizeof(char),41);}),_tag_dyneither(_tmp32B,sizeof(void*),0));});{
void*_tmp32D=(void*)_tmp33C->hd;void*_tmp32E=_tmp32D;struct _dyneither_ptr*_tmp33A;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp32E)->tag == 1){_LL1CE: _tmp33A=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp32E)->f1;_LL1CF: {
# 1611
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp33A,0);
{void*_tmp32F=_tmp33B->r;void*_tmp330=_tmp32F;void*_tmp337;struct Cyc_List_List*_tmp336;struct Cyc_Absyn_Vardecl*_tmp335;struct Cyc_Absyn_Exp*_tmp334;struct Cyc_Absyn_Exp*_tmp333;int _tmp332;struct Cyc_List_List*_tmp331;switch(*((int*)_tmp330)){case 25: _LL1D3: _tmp331=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp330)->f1;_LL1D4:
# 1614
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp33B->topt),lval,_tmp331,s);goto _LL1D2;case 26: _LL1D5: _tmp335=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f1;_tmp334=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f2;_tmp333=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f3;_tmp332=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp330)->f4;_LL1D6:
# 1616
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp33B->topt),lval,_tmp335,_tmp334,_tmp333,_tmp332,s,0);
goto _LL1D2;case 29: _LL1D7: _tmp337=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp330)->f1;_tmp336=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp330)->f2;_LL1D8:
# 1619
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp337,_tmp336,s);goto _LL1D2;case 27: _LL1D9: _LL1DA:
# 1621
 goto _LL1D2;default: _LL1DB: _LL1DC:
# 1623
 Cyc_Toc_exp_to_c(nv,_tmp33B);
# 1625
if(Cyc_Toc_is_poly_field(struct_type,_tmp33A))
_tmp33B=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp33B);
# 1628
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp33B,0),0),s,0);
goto _LL1D2;}_LL1D2:;}
# 1631
goto _LL1CD;}}else{_LL1D0: _LL1D1:
({void*_tmp338=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp339="array designator in struct";_tag_dyneither(_tmp339,sizeof(char),27);}),_tag_dyneither(_tmp338,sizeof(void*),0));});}_LL1CD:;};}}
# 1635
return s;}
# 1640
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1644
struct _RegionHandle _tmp33D=_new_region("r");struct _RegionHandle*r=& _tmp33D;_push_region(r);
{struct Cyc_List_List*_tmp33E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp33F=Cyc_Toc_add_tuple_type(_tmp33E);
# 1648
struct _tuple1*_tmp340=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp341=Cyc_Absyn_var_exp(_tmp340,0);
struct Cyc_Absyn_Stmt*_tmp342=Cyc_Absyn_exp_stmt(_tmp341,0);
# 1652
struct Cyc_Absyn_Exp*(*_tmp343)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1654
int is_atomic=1;
struct Cyc_List_List*_tmp344=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp344);for(0;_tmp344 != 0;(_tmp344=_tmp344->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp344->hd;
struct Cyc_Absyn_Exp*lval=_tmp343(_tmp341,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp345=e->r;void*_tmp346=_tmp345;struct Cyc_Absyn_Vardecl*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;int _tmp348;struct Cyc_List_List*_tmp347;switch(*((int*)_tmp346)){case 25: _LL1DE: _tmp347=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp346)->f1;_LL1DF:
# 1662
 _tmp342=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp347,_tmp342);
goto _LL1DD;case 26: _LL1E0: _tmp34B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f1;_tmp34A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f2;_tmp349=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f3;_tmp348=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346)->f4;_LL1E1:
# 1665
 _tmp342=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp34B,_tmp34A,_tmp349,_tmp348,_tmp342,0);
# 1667
goto _LL1DD;case 27: _LL1E2: _LL1E3:
# 1669
 goto _LL1DD;default: _LL1E4: _LL1E5:
# 1672
 Cyc_Toc_exp_to_c(nv,e);
_tmp342=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp343(_tmp341,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp342,0);
# 1675
goto _LL1DD;}_LL1DD:;};}}{
# 1678
struct Cyc_Absyn_Exp*_tmp34C=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp340,0),_tmp33F,_tmp342,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp34C;};}
# 1645
;_pop_region(r);}
# 1682
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp34D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp34D != 0;_tmp34D=_tmp34D->tl){
struct Cyc_Absyn_Aggrfield*_tmp34E=(struct Cyc_Absyn_Aggrfield*)_tmp34D->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp34E->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1689
({void*_tmp34F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp352;_tmp352.tag=0;_tmp352.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp350[1]={& _tmp352};Cyc_aprintf(({const char*_tmp351="get_member_offset %s failed";_tag_dyneither(_tmp351,sizeof(char),28);}),_tag_dyneither(_tmp350,sizeof(void*),1));});}),_tag_dyneither(_tmp34F,sizeof(void*),0));});}
# 1692
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp353=Cyc_Tcutil_compress(*typ);void*_tmp354=_tmp353;void*_tmp35C;struct Cyc_Absyn_Tqual _tmp35B;struct Cyc_Absyn_Exp*_tmp35A;union Cyc_Absyn_Constraint*_tmp359;unsigned int _tmp358;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->tag == 8){_LL1E7: _tmp35C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).elt_type;_tmp35B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).tq;_tmp35A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).num_elts;_tmp359=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).zero_term;_tmp358=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).zt_loc;_LL1E8:
# 1697
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp35A))){
*typ=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp356;_tmp356.tag=8;_tmp356.f1=({struct Cyc_Absyn_ArrayInfo _tmp357;_tmp357.elt_type=_tmp35C;_tmp357.tq=_tmp35B;_tmp357.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp357.zero_term=_tmp359;_tmp357.zt_loc=_tmp358;_tmp357;});_tmp356;});_tmp355;});
return 1;}
# 1701
goto _LL1E6;}else{_LL1E9: _LL1EA:
 goto _LL1E6;}_LL1E6:;}
# 1704
return 0;}
# 1707
static int Cyc_Toc_is_array_type(void*t){
void*_tmp35D=Cyc_Tcutil_compress(t);void*_tmp35E=_tmp35D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35E)->tag == 8){_LL1EC: _LL1ED:
 return 1;}else{_LL1EE: _LL1EF:
 return 0;}_LL1EB:;}
# 1714
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1716
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1720
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp35F=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp360=_tmp35F->attributes;
void*_tmp361=_tmp35F->type;
if((!Cyc_Toc_is_array_type(_tmp361),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp361))))
_tmp360=({struct Cyc_List_List*_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp363=_cycalloc(sizeof(*_tmp363));_tmp363[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp364;_tmp364.tag=6;_tmp364.f1=0;_tmp364;});_tmp363;});_tmp362->tl=_tmp360;_tmp362;});{
struct Cyc_Absyn_Aggrfield*_tmp365=({struct Cyc_Absyn_Aggrfield*_tmp367=_cycalloc(sizeof(*_tmp367));_tmp367->name=_tmp35F->name;_tmp367->tq=Cyc_Toc_mt_tq;_tmp367->type=new_field_type;_tmp367->width=_tmp35F->width;_tmp367->attributes=_tmp360;_tmp367->requires_clause=0;_tmp367;});
# 1732
new_fields=({struct Cyc_List_List*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->hd=_tmp365;_tmp366->tl=new_fields;_tmp366;});};}else{
# 1734
new_fields=({struct Cyc_List_List*_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp368->tl=new_fields;_tmp368;});}}{
# 1736
struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp371;_tmp371.tag=1;_tmp371.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp36F[1]={& _tmp371};Cyc_aprintf(({const char*_tmp370="_genStruct%d";_tag_dyneither(_tmp370,sizeof(char),13);}),_tag_dyneither(_tmp36F,sizeof(void*),1));});});_tmp36E;});
# 1739
struct _tuple1*qv=({struct _tuple1*_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D->f1=Cyc_Absyn_Abs_n(0,1);_tmp36D->f2=name;_tmp36D;});
struct Cyc_Absyn_AggrdeclImpl*_tmp36A=({struct Cyc_Absyn_AggrdeclImpl*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;_tmp36C->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;_tmp36C->fields=_tmp369;_tmp36C->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;_tmp36C;});
# 1744
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->kind=ad->kind;_tmp36B->sc=ad->sc;_tmp36B->name=qv;_tmp36B->tvs=ad->tvs;_tmp36B->impl=_tmp36A;_tmp36B->expected_mem_kind=0;_tmp36B->attributes=ad->attributes;_tmp36B;});
# 1751
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1755
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1761
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp372=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp372,0);
do_declare=1;}else{
# 1769
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1772
struct Cyc_Absyn_Stmt*_tmp373=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1776
struct Cyc_Absyn_Exp**varexp;
{void*_tmp374=_tmp373->r;void*_tmp375=_tmp374;struct Cyc_Absyn_Exp**_tmp378;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp375)->tag == 1){_LL1F1: _tmp378=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp375)->f1;_LL1F2:
 varexp=_tmp378;goto _LL1F0;}else{_LL1F3: _LL1F4:
({void*_tmp376=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp377="stmt not an expression!";_tag_dyneither(_tmp377,sizeof(char),24);}),_tag_dyneither(_tmp376,sizeof(void*),0));});}_LL1F0:;}{
# 1782
struct Cyc_Absyn_Exp*(*_tmp379)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1786
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp37A=Cyc_Tcutil_compress(struct_type);void*_tmp37B=_tmp37A;union Cyc_Absyn_AggrInfoU _tmp37F;struct Cyc_List_List*_tmp37E;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37B)->tag == 11){_LL1F6: _tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37B)->f1).aggr_info;_tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37B)->f1).targs;_LL1F7:
# 1793
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp37F);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp37E;
goto _LL1F5;}else{_LL1F8: _LL1F9:
({void*_tmp37C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp37D="init_struct: bad struct type";_tag_dyneither(_tmp37D,sizeof(char),29);}),_tag_dyneither(_tmp37C,sizeof(void*),0));});}_LL1F5:;}
# 1800
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp380=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp380->tl != 0;_tmp380=_tmp380->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp381=(struct Cyc_Absyn_Aggrfield*)_tmp380->hd;
struct _RegionHandle _tmp382=_new_region("temp");struct _RegionHandle*temp=& _tmp382;_push_region(temp);
{struct Cyc_List_List*_tmp383=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1809
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp383,_tmp381->type))){
struct Cyc_List_List*_tmp384=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp385=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp383,_tmp384);
# 1813
struct Cyc_List_List*new_fields=0;
for(_tmp380=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp380 != 0;_tmp380=_tmp380->tl){
struct Cyc_Absyn_Aggrfield*_tmp386=(struct Cyc_Absyn_Aggrfield*)_tmp380->hd;
struct Cyc_Absyn_Aggrfield*_tmp387=({struct Cyc_Absyn_Aggrfield*_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A->name=_tmp386->name;_tmp38A->tq=Cyc_Toc_mt_tq;_tmp38A->type=
# 1818
Cyc_Tcutil_rsubstitute(temp,_tmp385,_tmp386->type);_tmp38A->width=_tmp386->width;_tmp38A->attributes=_tmp386->attributes;_tmp38A->requires_clause=0;_tmp38A;});
# 1826
if(_tmp380->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp387->type))
index=({struct _dyneither_ptr**_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388[0]=_tmp387->name;_tmp388;});}
# 1833
new_fields=({struct Cyc_List_List*_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389->hd=_tmp387;_tmp389->tl=new_fields;_tmp389;});}
# 1835
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp396;_tmp396.tag=1;_tmp396.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp394[1]={& _tmp396};Cyc_aprintf(({const char*_tmp395="_genStruct%d";_tag_dyneither(_tmp395,sizeof(char),13);}),_tag_dyneither(_tmp394,sizeof(void*),1));});});_tmp393;});
# 1839
struct _tuple1*qv=({struct _tuple1*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392->f1=Cyc_Absyn_Abs_n(0,1);_tmp392->f2=name;_tmp392;});
struct Cyc_Absyn_AggrdeclImpl*_tmp38B=({struct Cyc_Absyn_AggrdeclImpl*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->exist_vars=0;_tmp391->rgn_po=0;_tmp391->fields=aggrfields;_tmp391->tagged=0;_tmp391;});
# 1844
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390->kind=Cyc_Absyn_StructA;_tmp390->sc=Cyc_Absyn_Public;_tmp390->name=qv;_tmp390->tvs=0;_tmp390->expected_mem_kind=0;_tmp390->impl=_tmp38B;_tmp390->attributes=0;_tmp390;});
# 1851
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp38D;_tmp38D.tag=11;_tmp38D.f1=({struct Cyc_Absyn_AggrInfo _tmp38E;_tmp38E.aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=new_ad;_tmp38F;}));_tmp38E.targs=0;_tmp38E;});_tmp38D;});_tmp38C;});
# 1855
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1805
;_pop_region(temp);};}{
# 1859
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp397=_new_region("r");struct _RegionHandle*r=& _tmp397;_push_region(r);
{struct Cyc_List_List*_tmp398=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp398 != 0;_tmp398=_tmp398->tl){
struct _tuple19*_tmp399=(struct _tuple19*)_tmp398->hd;struct _tuple19*_tmp39A=_tmp399;struct Cyc_List_List*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3BE;_LL1FB: _tmp3BF=_tmp39A->f1;_tmp3BE=_tmp39A->f2;_LL1FC:;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3BE->topt));
if(_tmp3BF == 0)
({void*_tmp39B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp39C="empty designator list";_tag_dyneither(_tmp39C,sizeof(char),22);}),_tag_dyneither(_tmp39B,sizeof(void*),0));});
if(_tmp3BF->tl != 0){
# 1869
struct _tuple1*_tmp39D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39E=Cyc_Absyn_var_exp(_tmp39D,0);
for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
void*_tmp39F=(void*)_tmp3BF->hd;void*_tmp3A0=_tmp39F;struct _dyneither_ptr*_tmp3A3;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A0)->tag == 1){_LL1FE: _tmp3A3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A0)->f1;_LL1FF:
# 1875
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3A3))
_tmp39E=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp39E);
_tmp373=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp379(xexp,_tmp3A3,0),_tmp39E,0),0),_tmp373,0);
# 1879
goto _LL1FD;}else{_LL200: _LL201:
({void*_tmp3A1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3A2="array designator in struct";_tag_dyneither(_tmp3A2,sizeof(char),27);}),_tag_dyneither(_tmp3A1,sizeof(void*),0));});}_LL1FD:;}
# 1883
Cyc_Toc_exp_to_c(nv,_tmp3BE);
_tmp373=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp39E,_tmp3BE,0),0),_tmp373,0);}else{
# 1886
void*_tmp3A4=(void*)_tmp3BF->hd;void*_tmp3A5=_tmp3A4;struct _dyneither_ptr*_tmp3BD;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A5)->tag == 1){_LL203: _tmp3BD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A5)->f1;_LL204: {
# 1888
struct Cyc_Absyn_Exp*lval=_tmp379(xexp,_tmp3BD,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3BD);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp373=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp373,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1897
int e1_translated=0;
{void*_tmp3A6=_tmp3BE->r;void*_tmp3A7=_tmp3A6;void*_tmp3BA;struct Cyc_List_List*_tmp3B9;struct Cyc_Absyn_Exp*_tmp3B8;void*_tmp3B7;struct Cyc_Absyn_Vardecl*_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B4;int _tmp3B3;struct Cyc_List_List*_tmp3B2;switch(*((int*)_tmp3A7)){case 25: _LL208: _tmp3B2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_LL209:
# 1900
 _tmp373=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3BD)))->type,lval,_tmp3B2,_tmp373);
# 1902
goto _LL207;case 26: _LL20A: _tmp3B6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_tmp3B5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f2;_tmp3B4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f3;_tmp3B3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A7)->f4;_LL20B:
# 1904
 _tmp373=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3BD)))->type,lval,_tmp3B6,_tmp3B5,_tmp3B4,_tmp3B3,_tmp373,0);
# 1908
e1_translated=1;
_tmp3B8=_tmp3B5;_tmp3B7=(void*)_check_null(_tmp3B4->topt);goto _LL20D;case 27: _LL20C: _tmp3B8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_tmp3B7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3A7)->f2;_LL20D:
# 1913
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3BD)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3B8);{
void*_tmp3A8=Cyc_Toc_typ_to_c(_tmp3B7);
*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3A9[2];_tmp3A9[1]=
Cyc_Absyn_sizeoftyp_exp(_tmp3A8,0);_tmp3A9[0]=_tmp3B8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A9,sizeof(struct Cyc_Absyn_Exp*),2));}),0),
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3AA;(_tmp3AA.Int_c).val=({struct _tuple6 _tmp3AB;_tmp3AB.f1=Cyc_Absyn_Unsigned;_tmp3AB.f2=(int)sizeof(double);_tmp3AB;});(_tmp3AA.Int_c).tag=5;_tmp3AA;}),0),0);};}
# 1921
goto _LL207;case 29: _LL20E: _tmp3BA=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3A7)->f1;_tmp3B9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3A7)->f2;_LL20F:
# 1923
 _tmp373=Cyc_Toc_init_anon_struct(nv,lval,_tmp3BA,_tmp3B9,_tmp373);goto _LL207;default: _LL210: _LL211: {
# 1925
void*old_e_typ=(void*)_check_null(_tmp3BE->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3BE);
{void*_tmp3AC=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3AC)->tag == 11){_LL213: _LL214:
# 1932
 if(old_e_typ != _tmp3BE->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3BE->topt))){
# 1936
ad=Cyc_Toc_update_aggr_type(ad,_tmp3BD,(void*)_check_null(_tmp3BE->topt));
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3AE;_tmp3AE.tag=11;_tmp3AE.f1=({struct Cyc_Absyn_AggrInfo _tmp3AF;_tmp3AF.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0[0]=ad;_tmp3B0;}));_tmp3AF.targs=0;_tmp3AF;});_tmp3AE;});_tmp3AD;});
# 1940
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1942
goto _LL212;}else{_LL215: _LL216:
 goto _LL212;}_LL212:;}{
# 1945
struct Cyc_Absyn_Aggrfield*_tmp3B1=Cyc_Absyn_lookup_field(aggrfields,_tmp3BD);
# 1947
if(Cyc_Toc_is_poly_field(struct_type,_tmp3BD) && !was_ptr_type)
_tmp3BE=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3BE);
# 1950
if(exist_types != 0)
_tmp3BE=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3B1))->type),_tmp3BE);
# 1953
_tmp373=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3BE,0),0),_tmp373,0);
goto _LL207;};}}_LL207:;}
# 1956
goto _LL202;};}}else{_LL205: _LL206:
({void*_tmp3BB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3BC="array designator in struct";_tag_dyneither(_tmp3BC,sizeof(char),27);}),_tag_dyneither(_tmp3BB,sizeof(void*),0));});}_LL202:;}}}
# 1963
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 1968
struct Cyc_Absyn_Exp*_tmp3C0=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp373,pointer,rgnopt,is_atomic,do_declare);
# 1970
_tmp3C0->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3C1=_tmp3C0;_npop_handler(0);return _tmp3C1;};};
# 1861
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1976
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1979
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1982
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2[0]=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3C3;_tmp3C3.tag=4;_tmp3C3.f1=e1;_tmp3C3.f2=incr;_tmp3C3;});_tmp3C2;}),0);}
# 1986
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1995
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1997
void*_tmp3C4=e1->r;void*_tmp3C5=_tmp3C4;struct Cyc_Absyn_Exp*_tmp3CD;struct _dyneither_ptr*_tmp3CC;int _tmp3CB;int _tmp3CA;void*_tmp3C9;struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C7;switch(*((int*)_tmp3C5)){case 36: _LL218: _tmp3C7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3C5)->f1;_LL219:
 Cyc_Toc_lvalue_assign_stmt(_tmp3C7,fs,f,f_env);goto _LL217;case 13: _LL21A: _tmp3C9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C5)->f1;_tmp3C8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C5)->f2;_LL21B:
 Cyc_Toc_lvalue_assign(_tmp3C8,fs,f,f_env);goto _LL217;case 20: _LL21C: _tmp3CD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f1;_tmp3CC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f2;_tmp3CB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f3;_tmp3CA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C5)->f4;_LL21D:
# 2002
 e1->r=_tmp3CD->r;
Cyc_Toc_lvalue_assign(e1,({struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->hd=_tmp3CC;_tmp3C6->tl=fs;_tmp3C6;}),f,f_env);
goto _LL217;default: _LL21E: _LL21F: {
# 2010
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2012
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL217;}}_LL217:;}
# 2018
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2020
void*_tmp3CE=s->r;void*_tmp3CF=_tmp3CE;struct Cyc_Absyn_Stmt*_tmp3D6;struct Cyc_Absyn_Decl*_tmp3D5;struct Cyc_Absyn_Stmt*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D3;switch(*((int*)_tmp3CF)){case 1: _LL221: _tmp3D3=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f1;_LL222:
 Cyc_Toc_lvalue_assign(_tmp3D3,fs,f,f_env);goto _LL220;case 12: _LL223: _tmp3D5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f1;_tmp3D4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f2;_LL224:
# 2023
 Cyc_Toc_lvalue_assign_stmt(_tmp3D4,fs,f,f_env);goto _LL220;case 2: _LL225: _tmp3D6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3CF)->f2;_LL226:
 Cyc_Toc_lvalue_assign_stmt(_tmp3D6,fs,f,f_env);goto _LL220;default: _LL227: _LL228:
({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0;_tmp3D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3D0[1]={& _tmp3D2};Cyc_Toc_toc_impos(({const char*_tmp3D1="lvalue_assign_stmt: %s";_tag_dyneither(_tmp3D1,sizeof(char),23);}),_tag_dyneither(_tmp3D0,sizeof(void*),1));});});}_LL220:;}
# 2029
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2033
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3D7=e->r;void*_tmp3D8=_tmp3D7;struct Cyc_Absyn_Stmt*_tmp3DF;struct Cyc_Absyn_Exp*_tmp3DE;void**_tmp3DD;struct Cyc_Absyn_Exp**_tmp3DC;switch(*((int*)_tmp3D8)){case 13: _LL22A: _tmp3DD=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D8)->f1;_tmp3DC=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D8)->f2;_LL22B:
# 2036
*_tmp3DC=Cyc_Toc_push_address_exp(*_tmp3DC);
*_tmp3DD=Cyc_Absyn_cstar_typ(*_tmp3DD,Cyc_Toc_mt_tq);
return e;case 19: _LL22C: _tmp3DE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3D8)->f1;_LL22D:
# 2040
 return _tmp3DE;case 36: _LL22E: _tmp3DF=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3D8)->f1;_LL22F:
# 2044
 Cyc_Toc_push_address_stmt(_tmp3DF);
return e;default: _LL230: _LL231:
# 2047
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);
({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0;_tmp3DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp3D9[1]={& _tmp3DB};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3DA="can't take & of exp %s";_tag_dyneither(_tmp3DA,sizeof(char),23);}),_tag_dyneither(_tmp3D9,sizeof(void*),1));});});}_LL229:;}
# 2052
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3E0=s->r;void*_tmp3E1=_tmp3E0;struct Cyc_Absyn_Exp**_tmp3E7;struct Cyc_Absyn_Stmt*_tmp3E6;struct Cyc_Absyn_Stmt*_tmp3E5;switch(*((int*)_tmp3E1)){case 2: _LL233: _tmp3E5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3E1)->f2;_LL234:
 _tmp3E6=_tmp3E5;goto _LL236;case 12: _LL235: _tmp3E6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3E1)->f2;_LL236:
 Cyc_Toc_push_address_stmt(_tmp3E6);goto _LL232;case 1: _LL237: _tmp3E7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3E1)->f1;_LL238:
*_tmp3E7=Cyc_Toc_push_address_exp(*_tmp3E7);goto _LL232;default: _LL239: _LL23A:
# 2058
({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0;_tmp3E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3E2[1]={& _tmp3E4};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3E3="can't take & of stmt %s";_tag_dyneither(_tmp3E3,sizeof(char),24);}),_tag_dyneither(_tmp3E2,sizeof(void*),1));});});}_LL232:;}
# 2063
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2065
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2067
if(x == 0)return 0;
result=({struct Cyc_List_List*_tmp3E8=_region_malloc(r2,sizeof(*_tmp3E8));_tmp3E8->hd=f(x->hd,env);_tmp3E8->tl=0;_tmp3E8;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmp3E9=_region_malloc(r2,sizeof(*_tmp3E9));_tmp3E9->hd=f(x->hd,env);_tmp3E9->tl=0;_tmp3E9;});
prev=prev->tl;}
# 2074
return result;}
# 2076
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2080
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->f1=0;_tmp3EA->f2=e;_tmp3EA;});}
# 2084
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3EB=Cyc_Tcutil_compress(t);void*_tmp3EC=_tmp3EB;struct Cyc_Absyn_PtrInfo _tmp3EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC)->tag == 5){_LL23C: _tmp3EF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC)->f1;_LL23D:
 return _tmp3EF;}else{_LL23E: _LL23F:
({void*_tmp3ED=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3EE="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3EE,sizeof(char),28);}),_tag_dyneither(_tmp3ED,sizeof(void*),0));});}_LL23B:;}
# 2094
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3F0=Cyc_Tcutil_compress(t);void*_tmp3F1=_tmp3F0;int _tmp3FD;enum Cyc_Absyn_Sign _tmp3FC;enum Cyc_Absyn_Sign _tmp3FB;enum Cyc_Absyn_Sign _tmp3FA;enum Cyc_Absyn_Sign _tmp3F9;enum Cyc_Absyn_Sign _tmp3F8;switch(*((int*)_tmp3F1)){case 5: _LL241: _LL242:
 res=Cyc_Absyn_null_exp(0);goto _LL240;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f2){case Cyc_Absyn_Char_sz: _LL243: _tmp3F8=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL244:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3F8,'\000'),0);goto _LL240;case Cyc_Absyn_Short_sz: _LL245: _tmp3F9=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL246:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3F9,0),0);goto _LL240;case Cyc_Absyn_Int_sz: _LL24B: _tmp3FA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL24C:
# 2102
 _tmp3FB=_tmp3FA;goto _LL24E;case Cyc_Absyn_Long_sz: _LL24D: _tmp3FB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL24E:
# 2104
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3FB,0),0);goto _LL240;default: _LL24F: _tmp3FC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1)->f1;_LL250:
# 2106
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3FC,(long long)0),0);goto _LL240;}case 13: _LL247: _LL248:
# 2100
 goto _LL24A;case 14: _LL249: _LL24A:
 _tmp3FA=Cyc_Absyn_Unsigned;goto _LL24C;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1)->f1){case 0: _LL251: _LL252:
# 2107
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F2="0.0F";_tag_dyneither(_tmp3F2,sizeof(char),5);}),0),0);goto _LL240;case 1: _LL253: _LL254:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F3="0.0";_tag_dyneither(_tmp3F3,sizeof(char),4);}),1),0);goto _LL240;default: _LL255: _tmp3FD=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1)->f1;_LL256:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F4="0.0L";_tag_dyneither(_tmp3F4,sizeof(char),5);}),_tmp3FD),0);goto _LL240;}default: _LL257: _LL258:
# 2111
({struct Cyc_String_pa_PrintArg_struct _tmp3F7;_tmp3F7.tag=0;_tmp3F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp3F5[1]={& _tmp3F7};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3F6="found non-zero type %s in generate_zero";_tag_dyneither(_tmp3F6,sizeof(char),40);}),_tag_dyneither(_tmp3F5,sizeof(void*),1));});});}_LL240:;}
# 2113
res->topt=t;
return res;}
# 2120
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2133 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp3FE=Cyc_Toc_typ_to_c(elt_type);
void*_tmp3FF=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp400=Cyc_Absyn_cstar_typ(_tmp3FE,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp401=({struct Cyc_Core_Opt*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->v=_tmp400;_tmp438;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp402=e1->r;void*_tmp403=_tmp402;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp407;switch(*((int*)_tmp403)){case 19: _LL25A: _tmp407=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_LL25B:
# 2141
 if(!is_dyneither){
_tmp407=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp407,0,Cyc_Absyn_Other_coercion,0);
_tmp407->topt=fat_ptr_type;}
# 2145
Cyc_Toc_exp_to_c(nv,_tmp407);xinit=_tmp407;goto _LL259;case 22: _LL25C: _tmp409=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_tmp408=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp403)->f2;_LL25D:
# 2147
 if(!is_dyneither){
_tmp409=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp409,0,Cyc_Absyn_Other_coercion,0);
_tmp409->topt=fat_ptr_type;}
# 2151
Cyc_Toc_exp_to_c(nv,_tmp409);Cyc_Toc_exp_to_c(nv,_tmp408);
xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp404[3];_tmp404[2]=_tmp408;_tmp404[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp404[0]=_tmp409;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp404,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
goto _LL259;default: _LL25E: _LL25F:
({void*_tmp405=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp406="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp406,sizeof(char),53);}),_tag_dyneither(_tmp405,sizeof(void*),0));});}_LL259:;}{
# 2157
struct _tuple1*_tmp40A=Cyc_Toc_temp_var();
struct _RegionHandle _tmp40B=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp40B;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp40C=({struct Cyc_Absyn_Vardecl*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->sc=Cyc_Absyn_Public;_tmp437->name=_tmp40A;_tmp437->varloc=0;_tmp437->tq=Cyc_Toc_mt_tq;_tmp437->type=_tmp3FF;_tmp437->initializer=xinit;_tmp437->rgn=0;_tmp437->attributes=0;_tmp437->escapes=0;_tmp437;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp40D=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp436;_tmp436.tag=4;_tmp436.f1=_tmp40C;_tmp436;});_tmp435;});
struct Cyc_Absyn_Exp*_tmp40E=Cyc_Absyn_varb_exp((void*)_tmp40D,0);
_tmp40E->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp40F=Cyc_Absyn_deref_exp(_tmp40E,0);
_tmp40F->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp40F);{
struct _tuple1*_tmp410=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp411=({struct Cyc_Absyn_Vardecl*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->sc=Cyc_Absyn_Public;_tmp434->name=_tmp410;_tmp434->varloc=0;_tmp434->tq=Cyc_Toc_mt_tq;_tmp434->type=_tmp3FE;_tmp434->initializer=_tmp40F;_tmp434->rgn=0;_tmp434->attributes=0;_tmp434->escapes=0;_tmp434;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp412=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp433;_tmp433.tag=4;_tmp433.f1=_tmp411;_tmp433;});_tmp432;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp413=Cyc_Absyn_varb_exp((void*)_tmp412,0);
_tmp413->topt=_tmp40F->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp413,e2,0);
z_init->topt=_tmp413->topt;}
# 2176
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp414=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp415=({struct Cyc_Absyn_Vardecl*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->sc=Cyc_Absyn_Public;_tmp431->name=_tmp414;_tmp431->varloc=0;_tmp431->tq=Cyc_Toc_mt_tq;_tmp431->type=_tmp3FE;_tmp431->initializer=z_init;_tmp431->rgn=0;_tmp431->attributes=0;_tmp431->escapes=0;_tmp431;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp416=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp430;_tmp430.tag=4;_tmp430.f1=_tmp415;_tmp430;});_tmp42F;});
# 2182
struct Cyc_Absyn_Exp*_tmp417=Cyc_Absyn_varb_exp((void*)_tmp412,0);_tmp417->topt=_tmp40F->topt;{
struct Cyc_Absyn_Exp*_tmp418=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp419=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp417,_tmp418,0);
_tmp419->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp419);{
# 2188
struct Cyc_Absyn_Exp*_tmp41A=Cyc_Absyn_varb_exp((void*)_tmp416,0);_tmp41A->topt=_tmp40F->topt;{
struct Cyc_Absyn_Exp*_tmp41B=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp41A,_tmp41B,0);
_tmp41C->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp41C);{
# 2194
struct Cyc_List_List*_tmp41D=({struct Cyc_Absyn_Exp*_tmp42E[2];_tmp42E[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp42E[0]=
# 2194
Cyc_Absyn_varb_exp((void*)_tmp40D,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42E,sizeof(struct Cyc_Absyn_Exp*),2));});
# 2196
struct Cyc_Absyn_Exp*_tmp41E=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp41D,0),_tmp41E,0);{
# 2201
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp419,_tmp41C,0),0);
struct Cyc_Absyn_Stmt*_tmp420=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp421=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp((void*)_tmp40D,0),Cyc_Toc_curr_sp,0);
_tmp421=Cyc_Toc_cast_it(_tmp400,_tmp421);{
struct Cyc_Absyn_Exp*_tmp422=Cyc_Absyn_deref_exp(_tmp421,0);
struct Cyc_Absyn_Exp*_tmp423=Cyc_Absyn_assign_exp(_tmp422,Cyc_Absyn_var_exp(_tmp414,0),0);
struct Cyc_Absyn_Stmt*_tmp424=Cyc_Absyn_exp_stmt(_tmp423,0);
_tmp424=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp41F,_tmp420,Cyc_Absyn_skip_stmt(0),0),_tmp424,0);
_tmp424=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp427;_tmp427.tag=0;_tmp427.f1=_tmp415;_tmp427;});_tmp426;});_tmp425->loc=0;_tmp425;}),_tmp424,0);
_tmp424=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp42A;_tmp42A.tag=0;_tmp42A.f1=_tmp411;_tmp42A;});_tmp429;});_tmp428->loc=0;_tmp428;}),_tmp424,0);
_tmp424=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp42D;_tmp42D.tag=0;_tmp42D.f1=_tmp40C;_tmp42D;});_tmp42C;});_tmp42B->loc=0;_tmp42B;}),_tmp424,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp424);};};};};};};};};};}
# 2159
;_pop_region(rgn2);};}
# 2227 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2231
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp439=Cyc_Tcutil_compress(aggrtype);void*_tmp43A=_tmp439;union Cyc_Absyn_AggrInfoU _tmp43F;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43A)->tag == 11){_LL261: _tmp43F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43A)->f1).aggr_info;_LL262:
# 2234
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp43F);goto _LL260;}else{_LL263: _LL264:
({void*_tmp43B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp43E;_tmp43E.tag=0;_tmp43E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));({void*_tmp43C[1]={& _tmp43E};Cyc_aprintf(({const char*_tmp43D="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp43D,sizeof(char),51);}),_tag_dyneither(_tmp43C,sizeof(void*),1));});}),_tag_dyneither(_tmp43B,sizeof(void*),0));});}_LL260:;}{
# 2238
struct _tuple1*_tmp440=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp441=Cyc_Absyn_var_exp(_tmp440,0);
struct Cyc_Absyn_Exp*_tmp442=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp443=Cyc_Absyn_aggrarrow_exp(_tmp441,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp444=Cyc_Absyn_neq_exp(_tmp443,_tmp442,0);
struct Cyc_Absyn_Exp*_tmp445=Cyc_Absyn_aggrarrow_exp(_tmp441,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp446=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp445,0),0);
struct Cyc_Absyn_Stmt*_tmp447=Cyc_Absyn_ifthenelse_stmt(_tmp444,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp448=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp449=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp44A=Cyc_Absyn_declare_stmt(_tmp440,_tmp448,_tmp449,Cyc_Absyn_seq_stmt(_tmp447,_tmp446,0),0);
return Cyc_Toc_stmt_exp_r(_tmp44A);}else{
# 2252
struct Cyc_Absyn_Exp*_tmp44B=Cyc_Toc_member_exp(aggrproj(_tmp441,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_neq_exp(_tmp44B,_tmp442,0);
struct Cyc_Absyn_Exp*_tmp44D=Cyc_Toc_member_exp(aggrproj(_tmp441,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp44E=Cyc_Absyn_exp_stmt(_tmp44D,0);
struct Cyc_Absyn_Stmt*_tmp44F=Cyc_Absyn_ifthenelse_stmt(_tmp44C,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp450=Cyc_Absyn_declare_stmt(_tmp440,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp44F,_tmp44E,0),0);
return Cyc_Toc_stmt_exp_r(_tmp450);}};}
# 2262
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2265
void*_tmp451=Cyc_Tcutil_compress(t);void*_tmp452=_tmp451;union Cyc_Absyn_AggrInfoU _tmp459;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp452)->tag == 11){_LL266: _tmp459=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp452)->f1).aggr_info;_LL267: {
# 2267
struct Cyc_Absyn_Aggrdecl*_tmp453=Cyc_Absyn_get_known_aggrdecl(_tmp459);
*f_tag=Cyc_Toc_get_member_offset(_tmp453,f);{
# 2270
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp458;_tmp458.tag=0;_tmp458.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp457;_tmp457.tag=0;_tmp457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp453->name).f2);({void*_tmp455[2]={& _tmp457,& _tmp458};Cyc_aprintf(({const char*_tmp456="_union_%s_%s";_tag_dyneither(_tmp456,sizeof(char),13);}),_tag_dyneither(_tmp455,sizeof(void*),2));});});});
*tagged_member_type=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454[0]=str;_tmp454;}));
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp453->impl))->tagged;};}}else{_LL268: _LL269:
 return 0;}_LL265:;}
# 2281
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2284
void*_tmp45A=e->r;void*_tmp45B=_tmp45A;struct Cyc_Absyn_Exp*_tmp467;struct _dyneither_ptr*_tmp466;int*_tmp465;struct Cyc_Absyn_Exp*_tmp464;struct _dyneither_ptr*_tmp463;int*_tmp462;struct Cyc_Absyn_Exp*_tmp461;switch(*((int*)_tmp45B)){case 13: _LL26B: _tmp461=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_LL26C:
({void*_tmp45C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp45D="cast on lhs!";_tag_dyneither(_tmp45D,sizeof(char),13);}),_tag_dyneither(_tmp45C,sizeof(void*),0));});case 20: _LL26D: _tmp464=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp463=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_tmp462=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45B)->f4;_LL26E:
# 2287
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp464->topt),_tmp463,f_tag,tagged_member_type,clear_read,_tmp462);case 21: _LL26F: _tmp467=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp466=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_tmp465=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45B)->f4;_LL270: {
# 2290
void*_tmp45E=Cyc_Tcutil_compress((void*)_check_null(_tmp467->topt));void*_tmp45F=_tmp45E;void*_tmp460;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->tag == 5){_LL274: _tmp460=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->f1).elt_typ;_LL275:
# 2292
 return Cyc_Toc_is_tagged_union_project_impl(_tmp460,_tmp466,f_tag,tagged_member_type,clear_read,_tmp465);}else{_LL276: _LL277:
# 2294
 return 0;}_LL273:;}default: _LL271: _LL272:
# 2296
 return 0;}_LL26A:;}
# 2309 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2313
struct _tuple1*_tmp468=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp468,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2323
struct Cyc_Absyn_Exp*_tmp469=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp469,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2326
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp468,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2332
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2347 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp46A=Cyc_Absyn_var_exp(max,0);
# 2355
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp46A=Cyc_Absyn_add_exp(_tmp46A,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2366
s=Cyc_Absyn_skip_stmt(0);}}{
# 2368
struct _RegionHandle _tmp46B=_new_region("r");struct _RegionHandle*r=& _tmp46B;_push_region(r);
{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp47F=_region_malloc(r,sizeof(*_tmp47F));_tmp47F->hd=({struct _tuple28*_tmp480=_region_malloc(r,sizeof(*_tmp480));_tmp480->f1=max;_tmp480->f2=Cyc_Absyn_uint_typ;_tmp480->f3=e1;_tmp480;});_tmp47F->tl=0;_tmp47F;});
# 2371
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp46C[2];_tmp46C[1]=_tmp46A;_tmp46C[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46C,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{
# 2378
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp46D[2];_tmp46D[1]=_tmp46A;_tmp46D[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46D,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{
# 2384
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
decls=({struct Cyc_List_List*_tmp46E=_region_malloc(r,sizeof(*_tmp46E));_tmp46E->hd=({struct _tuple28*_tmp46F=_region_malloc(r,sizeof(*_tmp46F));_tmp46F->f1=a;_tmp46F->f2=ptr_typ;_tmp46F->f3=ainit;_tmp46F;});_tmp46E->tl=decls;_tmp46E;});
if(!Cyc_Tcutil_is_pointer_type(old_typ))
({void*_tmp470=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp471="new_comprehension_to_c: comprehension is not an array type";_tag_dyneither(_tmp471,sizeof(char),59);}),_tag_dyneither(_tmp470,sizeof(void*),0));});
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp472=Cyc_Toc_temp_var();
void*_tmp473=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp474=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp475=
Cyc_Absyn_fncall_exp(_tmp474,({struct Cyc_Absyn_Exp*_tmp478[3];_tmp478[2]=_tmp46A;_tmp478[1]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp478[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp478,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 2396
decls=({struct Cyc_List_List*_tmp476=_region_malloc(r,sizeof(*_tmp476));_tmp476->hd=({struct _tuple28*_tmp477=_region_malloc(r,sizeof(*_tmp477));_tmp477->f1=_tmp472;_tmp477->f2=_tmp473;_tmp477->f3=_tmp475;_tmp477;});_tmp476->tl=decls;_tmp476;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp472,0),0),0);}else{
# 2399
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp479=decls;for(0;_tmp479 != 0;_tmp479=_tmp479->tl){
struct _tuple28 _tmp47A=*((struct _tuple28*)_tmp479->hd);struct _tuple28 _tmp47B=_tmp47A;struct _tuple1*_tmp47E;void*_tmp47D;struct Cyc_Absyn_Exp*_tmp47C;_LL279: _tmp47E=_tmp47B.f1;_tmp47D=_tmp47B.f2;_tmp47C=_tmp47B.f3;_LL27A:;
s=Cyc_Absyn_declare_stmt(_tmp47E,_tmp47D,_tmp47C,s,0);}}
# 2404
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2369
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2408
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp481=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp484;_tmp484.tag=0;_tmp484.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp482[1]={& _tmp484};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp483="exp_to_c: no type for %s";_tag_dyneither(_tmp483,sizeof(char),25);}),_tag_dyneither(_tmp482,sizeof(void*),1));});});
# 2414
if((nv->struct_info).lhs_exp != 0){
void*_tmp485=_tmp481;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp485)->tag == 28){_LL27C: _LL27D:
 goto _LL27B;}else{_LL27E: _LL27F:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}_LL27B:;}{
# 2422
void*old_typ=(void*)_check_null(e->topt);
void*_tmp486=_tmp481;struct Cyc_Absyn_Stmt*_tmp6A1;struct Cyc_Absyn_Exp*_tmp6A0;struct _dyneither_ptr*_tmp69F;struct Cyc_Absyn_Exp*_tmp69E;struct Cyc_Absyn_Exp*_tmp69D;int _tmp69C;struct Cyc_Absyn_Exp*_tmp69B;void**_tmp69A;struct Cyc_Absyn_Exp*_tmp699;int _tmp698;int _tmp697;struct Cyc_List_List*_tmp696;struct Cyc_Absyn_Datatypedecl*_tmp695;struct Cyc_Absyn_Datatypefield*_tmp694;void*_tmp693;struct Cyc_List_List*_tmp692;struct _tuple1*_tmp691;struct Cyc_List_List*_tmp690;struct Cyc_List_List*_tmp68F;struct Cyc_Absyn_Aggrdecl*_tmp68E;struct Cyc_Absyn_Exp*_tmp68D;void*_tmp68C;int _tmp68B;struct Cyc_Absyn_Vardecl*_tmp68A;struct Cyc_Absyn_Exp*_tmp689;struct Cyc_Absyn_Exp*_tmp688;int _tmp687;struct Cyc_List_List*_tmp686;struct Cyc_List_List*_tmp685;struct Cyc_Absyn_Exp*_tmp684;struct Cyc_Absyn_Exp*_tmp683;struct Cyc_Absyn_Exp*_tmp682;struct _dyneither_ptr*_tmp681;int _tmp680;int _tmp67F;struct Cyc_Absyn_Exp*_tmp67E;struct _dyneither_ptr*_tmp67D;int _tmp67C;int _tmp67B;struct Cyc_Absyn_Exp*_tmp67A;void*_tmp679;struct Cyc_List_List*_tmp678;void*_tmp677;struct Cyc_Absyn_Exp*_tmp676;struct Cyc_Absyn_Exp*_tmp675;struct Cyc_Absyn_Exp*_tmp674;struct Cyc_Absyn_Exp*_tmp673;void**_tmp672;struct Cyc_Absyn_Exp*_tmp671;int _tmp670;enum Cyc_Absyn_Coercion _tmp66F;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_List_List*_tmp66D;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp*_tmp66B;int _tmp66A;struct Cyc_Absyn_Exp*_tmp669;struct Cyc_List_List*_tmp668;int _tmp667;struct Cyc_List_List*_tmp666;struct Cyc_Absyn_VarargInfo*_tmp665;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_List_List*_tmp663;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_Absyn_Exp*_tmp661;struct Cyc_Absyn_Exp*_tmp660;struct Cyc_Absyn_Exp*_tmp65F;struct Cyc_Absyn_Exp*_tmp65E;struct Cyc_Absyn_Exp*_tmp65D;struct Cyc_Absyn_Exp*_tmp65C;struct Cyc_Absyn_Exp*_tmp65B;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp*_tmp659;struct Cyc_Core_Opt*_tmp658;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*_tmp656;enum Cyc_Absyn_Incrementor _tmp655;enum Cyc_Absyn_Primop _tmp654;struct Cyc_List_List*_tmp653;switch(*((int*)_tmp486)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp486)->f1).Null_c).tag == 1){_LL281: _LL282: {
# 2429
struct Cyc_Absyn_Exp*_tmp487=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp487,_tmp487))->r;else{
# 2434
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp488[3];_tmp488[2]=_tmp487;_tmp488[1]=_tmp487;_tmp488[0]=_tmp487;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp488,sizeof(struct Cyc_Absyn_Exp*),3));}));}}else{
# 2436
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2438
goto _LL280;}}else{_LL283: _LL284:
 goto _LL280;}case 1: _LL285: _LL286:
 goto _LL280;case 2: _LL287: _tmp654=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp653=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL288: {
# 2443
struct Cyc_List_List*_tmp489=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp653);
# 2445
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp653);
{enum Cyc_Absyn_Primop _tmp48A=_tmp654;switch(_tmp48A){case Cyc_Absyn_Numelts: _LL2D6: _LL2D7: {
# 2448
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp653))->hd;
{void*_tmp48B=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp48C=_tmp48B;void*_tmp49E;union Cyc_Absyn_Constraint*_tmp49D;union Cyc_Absyn_Constraint*_tmp49C;union Cyc_Absyn_Constraint*_tmp49B;struct Cyc_Absyn_Exp*_tmp49A;switch(*((int*)_tmp48C)){case 8: _LL2EB: _tmp49A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48C)->f1).num_elts;_LL2EC:
# 2452
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp49A)))
({void*_tmp48D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp48E="can't calculate numelts";_tag_dyneither(_tmp48E,sizeof(char),24);}),_tag_dyneither(_tmp48D,sizeof(void*),0));});
e->r=_tmp49A->r;goto _LL2EA;case 5: _LL2ED: _tmp49E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).elt_typ;_tmp49D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).ptr_atts).nullable;_tmp49C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).ptr_atts).bounds;_tmp49B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->f1).ptr_atts).zero_term;_LL2EE:
# 2456
{void*_tmp48F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp49C);void*_tmp490=_tmp48F;struct Cyc_Absyn_Exp*_tmp495;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp490)->tag == 0){_LL2F2: _LL2F3:
# 2458
 e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*_tmp491[2];_tmp491[1]=
# 2460
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp49E),0);_tmp491[0]=(struct Cyc_Absyn_Exp*)_tmp653->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp491,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL2F1;}else{_LL2F4: _tmp495=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp490)->f1;_LL2F5:
# 2464
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp49B)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp653->hd);
# 2468
e->r=Cyc_Toc_fncall_exp_r(function_e,({struct Cyc_Absyn_Exp*_tmp492[2];_tmp492[1]=_tmp495;_tmp492[0]=(struct Cyc_Absyn_Exp*)_tmp653->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp492,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp49D)){
if(!Cyc_Evexp_c_can_eval(_tmp495))
({void*_tmp493=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp494="can't calculate numelts";_tag_dyneither(_tmp494,sizeof(char),24);}),_tag_dyneither(_tmp493,sizeof(void*),0));});
# 2473
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp495,Cyc_Absyn_uint_exp(0,0));}else{
# 2475
e->r=_tmp495->r;goto _LL2F1;}}
# 2477
goto _LL2F1;}_LL2F1:;}
# 2479
goto _LL2EA;default: _LL2EF: _LL2F0:
# 2481
({struct Cyc_String_pa_PrintArg_struct _tmp499;_tmp499.tag=0;_tmp499.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp498;_tmp498.tag=0;_tmp498.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({void*_tmp496[2]={& _tmp498,& _tmp499};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp497="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp497,sizeof(char),41);}),_tag_dyneither(_tmp496,sizeof(void*),2));});});});}_LL2EA:;}
# 2484
goto _LL2D5;}case Cyc_Absyn_Plus: _LL2D8: _LL2D9:
# 2489
{void*_tmp49F=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp489))->hd);void*_tmp4A0=_tmp49F;void*_tmp4A8;union Cyc_Absyn_Constraint*_tmp4A7;union Cyc_Absyn_Constraint*_tmp4A6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->tag == 5){_LL2F7: _tmp4A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).elt_typ;_tmp4A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).ptr_atts).bounds;_tmp4A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).ptr_atts).zero_term;_LL2F8:
# 2491
{void*_tmp4A1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4A7);void*_tmp4A2=_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A5;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4A2)->tag == 0){_LL2FC: _LL2FD: {
# 2493
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp653))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp653->tl))->hd;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4A3[3];_tmp4A3[2]=e2;_tmp4A3[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4A8),0);_tmp4A3[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A3,sizeof(struct Cyc_Absyn_Exp*),3));}));
goto _LL2FB;}}else{_LL2FE: _tmp4A5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4A2)->f1;_LL2FF:
# 2499
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4A6)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp653))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp653->tl))->hd;
e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*_tmp4A4[3];_tmp4A4[2]=e2;_tmp4A4[1]=_tmp4A5;_tmp4A4[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A4,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r;}
# 2504
goto _LL2FB;}_LL2FB:;}
# 2506
goto _LL2F6;}else{_LL2F9: _LL2FA:
# 2508
 goto _LL2F6;}_LL2F6:;}
# 2510
goto _LL2D5;case Cyc_Absyn_Minus: _LL2DA: _LL2DB: {
# 2515
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp489))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp653))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp653->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp489->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2525
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4A9[3];_tmp4A9[2]=
# 2527
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0);_tmp4A9[1]=
# 2526
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4A9[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A9,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 2530
goto _LL2D5;}case Cyc_Absyn_Eq: _LL2DC: _LL2DD:
 goto _LL2DF;case Cyc_Absyn_Neq: _LL2DE: _LL2DF:
 goto _LL2E1;case Cyc_Absyn_Gt: _LL2E0: _LL2E1:
 goto _LL2E3;case Cyc_Absyn_Gte: _LL2E2: _LL2E3:
 goto _LL2E5;case Cyc_Absyn_Lt: _LL2E4: _LL2E5:
 goto _LL2E7;case Cyc_Absyn_Lte: _LL2E6: _LL2E7: {
# 2538
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp653))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp653->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp489))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp489->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
goto _LL2D5;}default: _LL2E8: _LL2E9:
 goto _LL2D5;}_LL2D5:;}
# 2550
goto _LL280;}case 4: _LL289: _tmp656=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp655=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL28A: {
# 2552
void*e2_cyc_typ=(void*)_check_null(_tmp656->topt);
# 2561 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp4C0="increment";_tag_dyneither(_tmp4C0,sizeof(char),10);});
if(_tmp655 == Cyc_Absyn_PreDec  || _tmp655 == Cyc_Absyn_PostDec)incr_str=({const char*_tmp4AA="decrement";_tag_dyneither(_tmp4AA,sizeof(char),10);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp656,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp4AD;_tmp4AD.tag=0;_tmp4AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4AB[1]={& _tmp4AD};Cyc_Tcutil_terr(e->loc,({const char*_tmp4AC="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp4AC,sizeof(char),74);}),_tag_dyneither(_tmp4AB,sizeof(void*),1));});});
({void*_tmp4AE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4AF="in-place inc/dec on zero-term";_tag_dyneither(_tmp4AF,sizeof(char),30);}),_tag_dyneither(_tmp4AE,sizeof(void*),0));});}{
# 2570
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp656,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4B0=Cyc_Absyn_signed_int_exp(1,0);
_tmp4B0->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4B1=_tmp655;switch(_tmp4B1){case Cyc_Absyn_PreInc: _LL301: _LL302:
# 2577
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4B3;_tmp4B3.tag=3;_tmp4B3.f1=_tmp656;_tmp4B3.f2=({struct Cyc_Core_Opt*_tmp4B4=_cycalloc_atomic(sizeof(*_tmp4B4));_tmp4B4->v=(void*)Cyc_Absyn_Plus;_tmp4B4;});_tmp4B3.f3=_tmp4B0;_tmp4B3;});_tmp4B2;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL303: _LL304:
# 2581
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4B6;_tmp4B6.tag=3;_tmp4B6.f1=_tmp656;_tmp4B6.f2=({struct Cyc_Core_Opt*_tmp4B7=_cycalloc_atomic(sizeof(*_tmp4B7));_tmp4B7->v=(void*)Cyc_Absyn_Minus;_tmp4B7;});_tmp4B6.f3=_tmp4B0;_tmp4B6;});_tmp4B5;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL305: _LL306:
# 2585
({struct Cyc_String_pa_PrintArg_struct _tmp4BA;_tmp4BA.tag=0;_tmp4BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4B8[1]={& _tmp4BA};Cyc_Tcutil_terr(e->loc,({const char*_tmp4B9="in-place post-%s is not supported on @tagged union members";_tag_dyneither(_tmp4B9,sizeof(char),59);}),_tag_dyneither(_tmp4B8,sizeof(void*),1));});});
# 2587
({void*_tmp4BB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4BC="in-place inc/dec on @tagged union";_tag_dyneither(_tmp4BC,sizeof(char),34);}),_tag_dyneither(_tmp4BB,sizeof(void*),0));});}_LL300:;};}
# 2590
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp656);
Cyc_Toc_set_lhs(nv,0);{
# 2595
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp655 == Cyc_Absyn_PostInc  || _tmp655 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2601
if(_tmp655 == Cyc_Absyn_PreDec  || _tmp655 == Cyc_Absyn_PostDec)
change=-1;
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4BD[3];_tmp4BD[2]=
# 2605
Cyc_Absyn_signed_int_exp(change,0);_tmp4BD[1]=
# 2604
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4BD[0]=
# 2603
Cyc_Toc_push_address_exp(_tmp656);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BD,sizeof(struct Cyc_Absyn_Exp*),3));}));}else{
# 2606
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2610
struct Cyc_Toc_functionSet*_tmp4BE=_tmp655 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2612
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp4BE,_tmp656);
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4BF[2];_tmp4BF[1]=
Cyc_Absyn_signed_int_exp(1,0);_tmp4BF[0]=
# 2613
Cyc_Toc_push_address_exp(_tmp656);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BF,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
# 2615
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp656)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp656,0,Cyc_Toc_incr_lvalue,_tmp655);
e->r=_tmp656->r;}}}
# 2619
goto _LL280;};};}case 3: _LL28B: _tmp659=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp658=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp657=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL28C: {
# 2638 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp659->topt);
void*e2_old_typ=(void*)_check_null(_tmp657->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp659,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp659);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp657);
e->r=Cyc_Toc_tagged_union_assignop(_tmp659,e1_old_typ,_tmp658,_tmp657,e2_old_typ,f_tag,tagged_member_struct_type);
# 2649
return;}{
# 2651
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp659,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp659,_tmp658,_tmp657,ptr_type,is_dyneither,elt_type);
# 2657
return;}{
# 2661
int e1_poly=Cyc_Toc_is_poly_project(_tmp659);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp659);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp657);{
# 2667
int done=0;
if(_tmp658 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp4C1=(enum Cyc_Absyn_Primop)_tmp658->v;enum Cyc_Absyn_Primop _tmp4C2=_tmp4C1;switch(_tmp4C2){case Cyc_Absyn_Plus: _LL308: _LL309:
 change=_tmp657;goto _LL307;case Cyc_Absyn_Minus: _LL30A: _LL30B:
# 2675
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp657,0);goto _LL307;default: _LL30C: _LL30D:
({void*_tmp4C3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4C4="bad t ? pointer arithmetic";_tag_dyneither(_tmp4C4,sizeof(char),27);}),_tag_dyneither(_tmp4C3,sizeof(void*),0));});}_LL307:;}
# 2678
done=1;{
# 2680
struct Cyc_Absyn_Exp*_tmp4C5=Cyc_Toc__dyneither_ptr_inplace_plus_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp4C5,({struct Cyc_Absyn_Exp*_tmp4C6[3];_tmp4C6[2]=change;_tmp4C6[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4C6[0]=
# 2681
Cyc_Toc_push_address_exp(_tmp659);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C6,sizeof(struct Cyc_Absyn_Exp*),3));}));};}else{
# 2684
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2687
enum Cyc_Absyn_Primop _tmp4C7=(enum Cyc_Absyn_Primop)_tmp658->v;enum Cyc_Absyn_Primop _tmp4C8=_tmp4C7;if(_tmp4C8 == Cyc_Absyn_Plus){_LL30F: _LL310:
# 2689
 done=1;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp659),({struct Cyc_Absyn_Exp*_tmp4C9[2];_tmp4C9[1]=_tmp657;_tmp4C9[0]=_tmp659;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C9,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL30E;}else{_LL311: _LL312:
({void*_tmp4CA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4CB="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp4CB,sizeof(char),39);}),_tag_dyneither(_tmp4CA,sizeof(void*),0));});}_LL30E:;}}}
# 2696
if(!done){
# 2698
if(e1_poly)
_tmp657->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp657->r,0));
# 2704
if(!Cyc_Absyn_is_lvalue(_tmp659)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp659,0,Cyc_Toc_assignop_lvalue,({struct _tuple27*_tmp4CC=_cycalloc(sizeof(struct _tuple27)* 1);_tmp4CC[0]=(struct _tuple27)({struct _tuple27 _tmp4CD;_tmp4CD.f1=_tmp658;_tmp4CD.f2=_tmp657;_tmp4CD;});_tmp4CC;}));
e->r=_tmp659->r;}}
# 2709
goto _LL280;};};};}case 5: _LL28D: _tmp65C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp65B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp65A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL28E:
# 2711
 Cyc_Toc_exp_to_c(nv,_tmp65C);
Cyc_Toc_exp_to_c(nv,_tmp65B);
Cyc_Toc_exp_to_c(nv,_tmp65A);
goto _LL280;case 6: _LL28F: _tmp65E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp65D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL290:
# 2716
 Cyc_Toc_exp_to_c(nv,_tmp65E);
Cyc_Toc_exp_to_c(nv,_tmp65D);
goto _LL280;case 7: _LL291: _tmp660=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp65F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL292:
# 2720
 Cyc_Toc_exp_to_c(nv,_tmp660);
Cyc_Toc_exp_to_c(nv,_tmp65F);
goto _LL280;case 8: _LL293: _tmp662=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp661=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL294:
# 2724
 Cyc_Toc_exp_to_c(nv,_tmp662);
Cyc_Toc_exp_to_c(nv,_tmp661);
goto _LL280;case 9: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3 == 0){_LL295: _tmp664=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp663=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL296:
# 2728
 Cyc_Toc_exp_to_c(nv,_tmp664);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp663);
goto _LL280;}else{_LL297: _tmp669=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp668=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp667=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3)->num_varargs;_tmp666=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3)->injectors;_tmp665=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp486)->f3)->vai;_LL298: {
# 2740 "toc.cyc"
struct _RegionHandle _tmp4CE=_new_region("r");struct _RegionHandle*r=& _tmp4CE;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp667,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp665->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2748
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp668);
int num_normargs=num_args - _tmp667;
# 2752
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp668=_tmp668->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp668))->hd);
new_args=({struct Cyc_List_List*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF->hd=(struct Cyc_Absyn_Exp*)_tmp668->hd;_tmp4CF->tl=new_args;_tmp4CF;});}}
# 2757
new_args=({struct Cyc_List_List*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp4D1[3];_tmp4D1[2]=num_varargs_exp;_tmp4D1[1]=
# 2759
Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp4D1[0]=argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D1,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp4D0->tl=new_args;_tmp4D0;});
# 2762
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2764
Cyc_Toc_exp_to_c(nv,_tmp669);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp669,new_args,0),0);
# 2768
if(_tmp665->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp4D2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp665->type));void*_tmp4D3=_tmp4D2;struct Cyc_Absyn_Datatypedecl*_tmp4D6;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D3)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D3)->f1).datatype_info).KnownDatatype).tag == 2){_LL314: _tmp4D6=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D3)->f1).datatype_info).KnownDatatype).val;_LL315:
 tud=_tmp4D6;goto _LL313;}else{goto _LL316;}}else{_LL316: _LL317:
({void*_tmp4D4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4D5="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp4D5,sizeof(char),44);}),_tag_dyneither(_tmp4D4,sizeof(void*),0));});}_LL313:;}{
# 2774
struct _dyneither_ptr vs=({unsigned int _tmp4DF=(unsigned int)_tmp667;struct _tuple1**_tmp4E0=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmp4DF));struct _dyneither_ptr _tmp4E2=_tag_dyneither(_tmp4E0,sizeof(struct _tuple1*),_tmp4DF);{unsigned int _tmp4E1=_tmp4DF;unsigned int i;for(i=0;i < _tmp4E1;i ++){_tmp4E0[i]=(struct _tuple1*)Cyc_Toc_temp_var();}}_tmp4E2;});
# 2776
if(_tmp667 != 0){
# 2778
struct Cyc_List_List*_tmp4D7=0;
{int i=_tmp667 - 1;for(0;i >= 0;-- i){
_tmp4D7=({struct Cyc_List_List*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp4D8->tl=_tmp4D7;_tmp4D8;});}}
# 2782
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp4D7,0),s,0);{
# 2785
int i=0;for(0;_tmp668 != 0;(((_tmp668=_tmp668->tl,_tmp666=_tmp666->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp668->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp4D9=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp666))->hd;struct Cyc_Absyn_Datatypefield*_tmp4DA=_tmp4D9;struct _tuple1*_tmp4DC;struct Cyc_List_List*_tmp4DB;_LL319: _tmp4DC=_tmp4DA->name;_tmp4DB=_tmp4DA->typs;_LL31A:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp4DB))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2800
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2803
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp4DC),0),s,0);
# 2806
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4DC,tud->name)),0,s,0);};}};}else{
# 2813
struct Cyc_List_List*_tmp4DD=({struct _tuple19*_tmp4DE[3];_tmp4DE[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4DE[1]=
# 2813
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4DE[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4DE,sizeof(struct _tuple19*),3));});
# 2815
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2825
{int i=0;for(0;_tmp668 != 0;(_tmp668=_tmp668->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp668->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp668->hd,0),s,0);}}
# 2831
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2844 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2846
_npop_handler(0);goto _LL280;
# 2740 "toc.cyc"
;_pop_region(r);}}case 10: _LL299: _tmp66B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp66A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL29A:
# 2849 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp66B);{
struct Cyc_Absyn_Exp*_tmp4E3=_tmp66A?Cyc_Toc_newrethrow_exp(_tmp66B): Cyc_Toc_newthrow_exp(_tmp66B);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp4E3,0))->r;
goto _LL280;};case 11: _LL29B: _tmp66C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL29C:
 Cyc_Toc_exp_to_c(nv,_tmp66C);goto _LL280;case 12: _LL29D: _tmp66E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp66D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL29E:
# 2855
 Cyc_Toc_exp_to_c(nv,_tmp66E);
# 2864 "toc.cyc"
for(0;_tmp66D != 0;_tmp66D=_tmp66D->tl){
enum Cyc_Absyn_KindQual _tmp4E4=(Cyc_Tcutil_typ_kind((void*)_tmp66D->hd))->kind;
if(_tmp4E4 != Cyc_Absyn_EffKind  && _tmp4E4 != Cyc_Absyn_RgnKind){
{void*_tmp4E5=Cyc_Tcutil_compress((void*)_tmp66D->hd);void*_tmp4E6=_tmp4E5;switch(*((int*)_tmp4E6)){case 2: _LL31C: _LL31D:
 goto _LL31F;case 3: _LL31E: _LL31F:
 continue;default: _LL320: _LL321:
# 2871
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp66E,0))->r;
goto _LL31B;}_LL31B:;}
# 2874
break;}}
# 2877
goto _LL280;case 13: _LL29F: _tmp672=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp671=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp670=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp66F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL2A0: {
# 2879
void*old_t2=(void*)_check_null(_tmp671->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp672;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp672=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp671);
# 2886
{struct _tuple29 _tmp4E7=({struct _tuple29 _tmp522;_tmp522.f1=Cyc_Tcutil_compress(old_t2);_tmp522.f2=Cyc_Tcutil_compress(new_typ);_tmp522;});struct _tuple29 _tmp4E8=_tmp4E7;struct Cyc_Absyn_PtrInfo _tmp521;struct Cyc_Absyn_PtrInfo _tmp520;struct Cyc_Absyn_PtrInfo _tmp51F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E8.f1)->tag == 5)switch(*((int*)_tmp4E8.f2)){case 5: _LL323: _tmp520=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E8.f1)->f1;_tmp51F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E8.f2)->f1;_LL324: {
# 2888
int _tmp4E9=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp520.ptr_atts).nullable);
int _tmp4EA=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp51F.ptr_atts).nullable);
void*_tmp4EB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp520.ptr_atts).bounds);
void*_tmp4EC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp51F.ptr_atts).bounds);
int _tmp4ED=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp520.ptr_atts).zero_term);
int _tmp4EE=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp51F.ptr_atts).zero_term);
{struct _tuple29 _tmp4EF=({struct _tuple29 _tmp51C;_tmp51C.f1=_tmp4EB;_tmp51C.f2=_tmp4EC;_tmp51C;});struct _tuple29 _tmp4F0=_tmp4EF;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp518;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F0.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F0.f2)->tag == 1){_LL32A: _tmp519=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F0.f1)->f1;_tmp518=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F0.f2)->f1;_LL32B:
# 2896
 if((!Cyc_Evexp_c_can_eval(_tmp519) || !Cyc_Evexp_c_can_eval(_tmp518)) && !
Cyc_Evexp_same_const_exp(_tmp519,_tmp518))
({void*_tmp4F1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4F2="can't validate cast due to potential size differences";_tag_dyneither(_tmp4F2,sizeof(char),54);}),_tag_dyneither(_tmp4F1,sizeof(void*),0));});
if(_tmp4E9  && !_tmp4EA){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp4F3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4F4="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp4F4,sizeof(char),44);}),_tag_dyneither(_tmp4F3,sizeof(void*),0));});
# 2905
if(_tmp66F != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0;_tmp4F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp4F5[1]={& _tmp4F7};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4F6="null-check conversion mis-classified: %s";_tag_dyneither(_tmp4F6,sizeof(char),41);}),_tag_dyneither(_tmp4F5,sizeof(void*),1));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp671);
if(do_null_check){
if(!_tmp670)
({void*_tmp4F8=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp4F9="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp4F9,sizeof(char),58);}),_tag_dyneither(_tmp4F8,sizeof(void*),0));});
# 2912
e->r=Cyc_Toc_cast_it_r(*_tmp672,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->hd=_tmp671;_tmp4FA->tl=0;_tmp4FA;}),0));}else{
# 2916
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp671->r;}};}else{
# 2921
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp671->r;}
# 2928
goto _LL329;}else{_LL32C: _tmp51A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F0.f1)->f1;_LL32D:
# 2930
 if(!Cyc_Evexp_c_can_eval(_tmp51A))
({void*_tmp4FB=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4FC="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp4FC,sizeof(char),71);}),_tag_dyneither(_tmp4FB,sizeof(void*),0));});
# 2933
if(_tmp66F == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp4FF;_tmp4FF.tag=0;_tmp4FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp4FD[1]={& _tmp4FF};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4FE="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp4FE,sizeof(char),44);}),_tag_dyneither(_tmp4FD,sizeof(void*),1));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2937
if((_tmp4ED  && !(_tmp51F.elt_tq).real_const) && !_tmp4EE)
# 2940
_tmp51A=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp51A,Cyc_Absyn_uint_exp(1,0),0);
# 2942
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp51A,_tmp671))->r;}else{
# 2944
struct Cyc_Absyn_Exp*_tmp500=Cyc_Toc__tag_dyneither_e;
# 2946
if(_tmp4ED){
# 2951
struct _tuple1*_tmp501=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp502=Cyc_Absyn_var_exp(_tmp501,0);
struct Cyc_Absyn_Exp*arg3;
# 2956
{void*_tmp503=_tmp671->r;void*_tmp504=_tmp503;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp504)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp504)->f1).Wstring_c).tag){case 8: _LL333: _LL334:
# 2958
 arg3=_tmp51A;goto _LL332;case 9: _LL335: _LL336:
# 2960
 arg3=_tmp51A;goto _LL332;default: goto _LL337;}else{_LL337: _LL338:
# 2962
 arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp671),({struct Cyc_Absyn_Exp*_tmp505[2];_tmp505[1]=_tmp51A;_tmp505[0]=
# 2964
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp502);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp505,sizeof(struct Cyc_Absyn_Exp*),2));}),0);
goto _LL332;}_LL332:;}
# 2967
if(!_tmp4EE  && !(_tmp51F.elt_tq).real_const)
# 2970
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2972
struct Cyc_Absyn_Exp*_tmp506=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp51F.elt_typ),0);
struct Cyc_Absyn_Exp*_tmp507=Cyc_Absyn_fncall_exp(_tmp500,({struct Cyc_Absyn_Exp*_tmp509[3];_tmp509[2]=arg3;_tmp509[1]=_tmp506;_tmp509[0]=_tmp502;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp509,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
struct Cyc_Absyn_Stmt*_tmp508=Cyc_Absyn_exp_stmt(_tmp507,0);
_tmp508=Cyc_Absyn_declare_stmt(_tmp501,Cyc_Toc_typ_to_c(old_t2),_tmp671,_tmp508,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp508);};}else{
# 2979
e->r=Cyc_Toc_fncall_exp_r(_tmp500,({struct Cyc_Absyn_Exp*_tmp50A[3];_tmp50A[2]=_tmp51A;_tmp50A[1]=
# 2981
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp51F.elt_typ),0);_tmp50A[0]=_tmp671;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp50A,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 2985
goto _LL329;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F0.f2)->tag == 1){_LL32E: _tmp51B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4F0.f2)->f1;_LL32F:
# 2987
 if(!Cyc_Evexp_c_can_eval(_tmp51B))
({void*_tmp50B=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp50C="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp50C,sizeof(char),71);}),_tag_dyneither(_tmp50B,sizeof(void*),0));});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp50D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp50E="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp50E,sizeof(char),45);}),_tag_dyneither(_tmp50D,sizeof(void*),0));});{
# 2999 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp50F=_tmp51B;
if(_tmp4ED  && !_tmp4EE)
_tmp50F=Cyc_Absyn_add_exp(_tmp51B,Cyc_Absyn_uint_exp(1,0),0);{
# 3006
struct Cyc_Absyn_Exp*_tmp510=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp511=Cyc_Absyn_fncall_exp(_tmp510,({struct Cyc_Absyn_Exp*_tmp513[3];_tmp513[2]=_tmp50F;_tmp513[1]=
# 3009
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp520.elt_typ),0);_tmp513[0]=_tmp671;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp513,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 3011
if(!_tmp4EA)
_tmp511->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->hd=
Cyc_Absyn_copy_exp(_tmp511);_tmp512->tl=0;_tmp512;}));
e->r=Cyc_Toc_cast_it_r(*_tmp672,_tmp511);
goto _LL329;};};}else{_LL330: _LL331:
# 3019
 DynCast:
 if((_tmp4ED  && !_tmp4EE) && !(_tmp51F.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp514=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp515="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp515,sizeof(char),70);}),_tag_dyneither(_tmp514,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp516=Cyc_Toc__dyneither_ptr_decrease_size_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp516,({struct Cyc_Absyn_Exp*_tmp517[3];_tmp517[2]=
# 3026
Cyc_Absyn_uint_exp(1,0);_tmp517[1]=
# 3025
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp520.elt_typ),0);_tmp517[0]=_tmp671;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp517,sizeof(struct Cyc_Absyn_Exp*),3));}));};}
# 3028
goto _LL329;}}_LL329:;}
# 3030
goto _LL322;}case 6: _LL325: _tmp521=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E8.f1)->f1;_LL326:
# 3032
{void*_tmp51D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp521.ptr_atts).bounds);void*_tmp51E=_tmp51D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp51E)->tag == 0){_LL33A: _LL33B:
# 3034
 _tmp671->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp671->r,_tmp671->loc),Cyc_Toc_curr_sp);goto _LL339;}else{_LL33C: _LL33D:
 goto _LL339;}_LL339:;}
# 3037
goto _LL322;default: goto _LL327;}else{_LL327: _LL328:
# 3039
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp671->r;
goto _LL322;}_LL322:;}
# 3043
goto _LL280;}case 14: _LL2A1: _tmp673=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2A2:
# 3047
{void*_tmp523=_tmp673->r;void*_tmp524=_tmp523;struct Cyc_List_List*_tmp52F;struct _tuple1*_tmp52E;struct Cyc_List_List*_tmp52D;struct Cyc_List_List*_tmp52C;switch(*((int*)_tmp524)){case 28: _LL33F: _tmp52E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp524)->f1;_tmp52D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp524)->f2;_tmp52C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp524)->f3;_LL340:
# 3049
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0;_tmp527.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp673->loc));({void*_tmp525[1]={& _tmp527};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp526="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp526,sizeof(char),42);}),_tag_dyneither(_tmp525,sizeof(void*),1));});});{
struct Cyc_Absyn_Exp*_tmp528=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp673->topt),_tmp52D,1,0,_tmp52C,_tmp52E);
e->r=_tmp528->r;
e->topt=_tmp528->topt;
goto _LL33E;};case 23: _LL341: _tmp52F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp524)->f1;_LL342:
# 3057
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp52B;_tmp52B.tag=0;_tmp52B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp673->loc));({void*_tmp529[1]={& _tmp52B};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp52A="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp52A,sizeof(char),42);}),_tag_dyneither(_tmp529,sizeof(void*),1));});});
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp52F))->r;
goto _LL33E;default: _LL343: _LL344:
# 3063
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp673);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp673)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp673,0,Cyc_Toc_address_lvalue,1);
# 3069
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp673);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp673->topt))))
# 3073
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp673);}
# 3075
goto _LL33E;}_LL33E:;}
# 3077
goto _LL280;case 15: _LL2A3: _tmp675=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp674=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL2A4:
# 3080
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp532;_tmp532.tag=0;_tmp532.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp674->loc));({void*_tmp530[1]={& _tmp532};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp531="%s: new at top-level";_tag_dyneither(_tmp531,sizeof(char),21);}),_tag_dyneither(_tmp530,sizeof(void*),1));});});{
void*new_e_type=(void*)_check_null(_tmp674->topt);
{void*_tmp533=_tmp674->r;void*_tmp534=_tmp533;struct Cyc_List_List*_tmp563;struct _tuple1*_tmp562;struct Cyc_List_List*_tmp561;struct Cyc_List_List*_tmp560;struct Cyc_Absyn_Aggrdecl*_tmp55F;struct Cyc_Absyn_Exp*_tmp55E;void*_tmp55D;int _tmp55C;struct Cyc_Absyn_Vardecl*_tmp55B;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Exp*_tmp559;int _tmp558;struct Cyc_List_List*_tmp557;switch(*((int*)_tmp534)){case 25: _LL346: _tmp557=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp534)->f1;_LL347: {
# 3088
struct _tuple1*_tmp535=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp536=Cyc_Absyn_var_exp(_tmp535,0);
struct Cyc_Absyn_Stmt*_tmp537=Cyc_Toc_init_array(nv,new_e_type,_tmp536,_tmp557,Cyc_Absyn_exp_stmt(_tmp536,0));
void*old_elt_typ;
{void*_tmp538=Cyc_Tcutil_compress(old_typ);void*_tmp539=_tmp538;void*_tmp53E;struct Cyc_Absyn_Tqual _tmp53D;union Cyc_Absyn_Constraint*_tmp53C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp539)->tag == 5){_LL353: _tmp53E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp539)->f1).elt_typ;_tmp53D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp539)->f1).elt_tq;_tmp53C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp539)->f1).ptr_atts).zero_term;_LL354:
# 3094
 old_elt_typ=_tmp53E;goto _LL352;}else{_LL355: _LL356:
# 3096
 old_elt_typ=({void*_tmp53A=0;((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp53B="exp_to_c:new array expression doesn't have ptr type";_tag_dyneither(_tmp53B,sizeof(char),52);}),_tag_dyneither(_tmp53A,sizeof(void*),0));});}_LL352:;}{
# 3099
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp53F=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp540=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp557),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp675 == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp540);else{
# 3107
struct Cyc_Absyn_Exp*r=_tmp675;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp540);}
# 3112
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp535,_tmp53F,e1,_tmp537,0));
goto _LL345;};}case 26: _LL348: _tmp55B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp534)->f1;_tmp55A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp534)->f2;_tmp559=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp534)->f3;_tmp558=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp534)->f4;_LL349:
# 3116
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp675,old_typ,_tmp55A,(void*)_check_null(_tmp559->topt),_tmp558,_tmp559,_tmp55B);
goto _LL345;case 27: _LL34A: _tmp55E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp534)->f1;_tmp55D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp534)->f2;_tmp55C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp534)->f3;_LL34B:
# 3120
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp675,old_typ,_tmp55E,_tmp55D,_tmp55C,0,0);
goto _LL345;case 28: _LL34C: _tmp562=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp534)->f1;_tmp561=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp534)->f2;_tmp560=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp534)->f3;_tmp55F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp534)->f4;_LL34D: {
# 3125
struct Cyc_Absyn_Exp*_tmp541=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp674->topt),_tmp561,1,_tmp675,_tmp560,_tmp562);
e->r=_tmp541->r;
e->topt=_tmp541->topt;
goto _LL345;}case 23: _LL34E: _tmp563=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp534)->f1;_LL34F:
# 3131
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp675,_tmp563))->r;
goto _LL345;default: _LL350: _LL351: {
# 3138
void*old_elt_typ=(void*)_check_null(_tmp674->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3141
struct _tuple1*_tmp542=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp543=Cyc_Absyn_var_exp(_tmp542,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp543,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp675 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3148
struct Cyc_Absyn_Exp*r=_tmp675;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3156
int done=0;
{void*_tmp544=_tmp674->r;void*_tmp545=_tmp544;void*_tmp553;struct Cyc_Absyn_Exp*_tmp552;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp545)->tag == 13){_LL358: _tmp553=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp545)->f1;_tmp552=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp545)->f2;_LL359:
# 3159
{struct _tuple29 _tmp546=({struct _tuple29 _tmp551;_tmp551.f1=Cyc_Tcutil_compress(_tmp553);_tmp551.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp552->topt));_tmp551;});struct _tuple29 _tmp547=_tmp546;void*_tmp550;union Cyc_Absyn_Constraint*_tmp54F;union Cyc_Absyn_Constraint*_tmp54E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f2)->tag == 5){_LL35D: _tmp550=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f1)->f1).elt_typ;_tmp54F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f1)->f1).ptr_atts).bounds;_tmp54E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f2)->f1).ptr_atts).bounds;_LL35E:
# 3162
{struct _tuple29 _tmp548=({struct _tuple29 _tmp54D;_tmp54D.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp54F);_tmp54D.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp54E);_tmp54D;});struct _tuple29 _tmp549=_tmp548;struct Cyc_Absyn_Exp*_tmp54C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp549.f1)->tag == 0){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp549.f2)->tag == 1){_LL362: _tmp54C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp549.f2)->f1;_LL363:
# 3164
 Cyc_Toc_exp_to_c(nv,_tmp552);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp54A=Cyc_Toc__init_dyneither_ptr_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp54A,({struct Cyc_Absyn_Exp*_tmp54B[4];_tmp54B[3]=_tmp54C;_tmp54B[2]=
# 3170
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp550),0);_tmp54B[1]=_tmp552;_tmp54B[0]=mexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp54B,sizeof(struct Cyc_Absyn_Exp*),4));}));
# 3172
goto _LL361;};}else{goto _LL364;}}else{_LL364: _LL365:
 goto _LL361;}_LL361:;}
# 3175
goto _LL35C;}else{goto _LL35F;}}else{_LL35F: _LL360:
 goto _LL35C;}_LL35C:;}
# 3178
goto _LL357;}else{_LL35A: _LL35B:
 goto _LL357;}_LL357:;}
# 3181
if(!done){
struct Cyc_Absyn_Stmt*_tmp554=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp543),0);
struct Cyc_Absyn_Exp*_tmp555=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp674);
_tmp554=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp543,_tmp555,0),_tmp674,0),_tmp554,0);{
# 3187
void*_tmp556=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp542,_tmp556,mexp,_tmp554,0));};}
# 3190
goto _LL345;};}}_LL345:;}
# 3192
goto _LL280;};case 17: _LL2A5: _tmp676=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2A6: {
# 3195
int _tmp564=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp676);
Cyc_Toc_set_in_sizeof(nv,_tmp564);
goto _LL280;}case 16: _LL2A7: _tmp677=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2A8:
 e->r=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp566;_tmp566.tag=16;_tmp566.f1=Cyc_Toc_typ_to_c(_tmp677);_tmp566;});_tmp565;});goto _LL280;case 18: _LL2A9: _tmp679=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp678=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL2AA: {
# 3201
void*_tmp567=_tmp679;
struct Cyc_List_List*_tmp568=_tmp678;
for(0;_tmp568 != 0;_tmp568=_tmp568->tl){
void*_tmp569=(void*)_tmp568->hd;void*_tmp56A=_tmp569;unsigned int _tmp580;struct _dyneither_ptr*_tmp57F;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp56A)->tag == 0){_LL367: _tmp57F=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp56A)->f1;_LL368:
 goto _LL366;}else{_LL369: _tmp580=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp56A)->f1;_LL36A:
# 3207
{void*_tmp56B=Cyc_Tcutil_compress(_tmp567);void*_tmp56C=_tmp56B;struct Cyc_Absyn_Datatypefield*_tmp57E;struct Cyc_List_List*_tmp57D;struct Cyc_List_List*_tmp57C;union Cyc_Absyn_AggrInfoU _tmp57B;switch(*((int*)_tmp56C)){case 11: _LL36C: _tmp57B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56C)->f1).aggr_info;_LL36D: {
# 3209
struct Cyc_Absyn_Aggrdecl*_tmp56D=Cyc_Absyn_get_known_aggrdecl(_tmp57B);
if(_tmp56D->impl == 0)
({void*_tmp56E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp56F="struct fields must be known";_tag_dyneither(_tmp56F,sizeof(char),28);}),_tag_dyneither(_tmp56E,sizeof(void*),0));});
_tmp57C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp56D->impl))->fields;goto _LL36F;}case 12: _LL36E: _tmp57C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp56C)->f2;_LL36F: {
# 3214
struct Cyc_Absyn_Aggrfield*_tmp570=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp57C,(int)_tmp580);
_tmp568->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp572;_tmp572.tag=0;_tmp572.f1=_tmp570->name;_tmp572;});_tmp571;}));
_tmp567=_tmp570->type;
goto _LL36B;}case 10: _LL370: _tmp57D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp56C)->f1;_LL371:
# 3219
 _tmp568->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp574;_tmp574.tag=0;_tmp574.f1=Cyc_Absyn_fieldname((int)(_tmp580 + 1));_tmp574;});_tmp573;}));
_tmp567=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp57D,(int)_tmp580)).f2;
goto _LL36B;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp56C)->f1).field_info).KnownDatatypefield).tag == 2){_LL372: _tmp57E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp56C)->f1).field_info).KnownDatatypefield).val).f2;_LL373:
# 3223
 if(_tmp580 == 0)
_tmp568->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp576;_tmp576.tag=0;_tmp576.f1=Cyc_Toc_tag_sp;_tmp576;});_tmp575;}));else{
# 3226
_tmp567=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp57E->typs,(int)(_tmp580 - 1))).f2;
_tmp568->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp578;_tmp578.tag=0;_tmp578.f1=Cyc_Absyn_fieldname((int)_tmp580);_tmp578;});_tmp577;}));}
# 3229
goto _LL36B;}else{goto _LL374;}default: _LL374: _LL375:
({void*_tmp579=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp57A="impossible type for offsetof tuple index";_tag_dyneither(_tmp57A,sizeof(char),41);}),_tag_dyneither(_tmp579,sizeof(void*),0));});
goto _LL36B;}_LL36B:;}
# 3233
goto _LL366;}_LL366:;}
# 3236
e->r=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp582;_tmp582.tag=18;_tmp582.f1=Cyc_Toc_typ_to_c(_tmp679);_tmp582.f2=_tmp678;_tmp582;});_tmp581;});
goto _LL280;}case 19: _LL2AB: _tmp67A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2AC: {
# 3239
int _tmp583=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp584=Cyc_Tcutil_compress((void*)_check_null(_tmp67A->topt));
{void*_tmp585=_tmp584;void*_tmp59A;struct Cyc_Absyn_Tqual _tmp599;void*_tmp598;union Cyc_Absyn_Constraint*_tmp597;union Cyc_Absyn_Constraint*_tmp596;union Cyc_Absyn_Constraint*_tmp595;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp585)->tag == 5){_LL377: _tmp59A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp585)->f1).elt_typ;_tmp599=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp585)->f1).elt_tq;_tmp598=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp585)->f1).ptr_atts).rgn;_tmp597=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp585)->f1).ptr_atts).nullable;_tmp596=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp585)->f1).ptr_atts).bounds;_tmp595=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp585)->f1).ptr_atts).zero_term;_LL378:
# 3244
{void*_tmp586=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp596);void*_tmp587=_tmp586;struct Cyc_Absyn_Exp*_tmp592;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp587)->tag == 1){_LL37C: _tmp592=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp587)->f1;_LL37D: {
# 3246
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp67A);
Cyc_Toc_exp_to_c(nv,_tmp67A);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp588=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp589="inserted null check due to dereference";_tag_dyneither(_tmp589,sizeof(char),39);}),_tag_dyneither(_tmp588,sizeof(void*),0));});
# 3252
_tmp67A->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp584),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->hd=
Cyc_Absyn_copy_exp(_tmp67A);_tmp58A->tl=0;_tmp58A;}),0));}
# 3260
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp595)){
struct _tuple12 _tmp58B=Cyc_Evexp_eval_const_uint_exp(_tmp592);struct _tuple12 _tmp58C=_tmp58B;unsigned int _tmp590;int _tmp58F;_LL381: _tmp590=_tmp58C.f1;_tmp58F=_tmp58C.f2;_LL382:;
# 3266
if(!_tmp58F  || _tmp590 <= 0)
({void*_tmp58D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp58E="cannot determine dereference is in bounds";_tag_dyneither(_tmp58E,sizeof(char),42);}),_tag_dyneither(_tmp58D,sizeof(void*),0));});}
# 3269
goto _LL37B;}}else{_LL37E: _LL37F: {
# 3272
struct Cyc_Absyn_Exp*_tmp591=Cyc_Absyn_uint_exp(0,0);
_tmp591->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp67A,_tmp591);
Cyc_Toc_exp_to_c(nv,e);
goto _LL37B;}}_LL37B:;}
# 3278
goto _LL376;}else{_LL379: _LL37A:
({void*_tmp593=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp594="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp594,sizeof(char),29);}),_tag_dyneither(_tmp593,sizeof(void*),0));});}_LL376:;}
# 3281
Cyc_Toc_set_lhs(nv,_tmp583);
goto _LL280;};}case 20: _LL2AD: _tmp67E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp67D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp67C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp67B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL2AE: {
# 3284
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp67E->topt);
Cyc_Toc_exp_to_c(nv,_tmp67E);
if(_tmp67C  && _tmp67B)
e->r=Cyc_Toc_check_tagged_union(_tmp67E,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp67D,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3292
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL280;}case 21: _LL2AF: _tmp682=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp681=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp680=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp67F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL2B0: {
# 3296
int _tmp59B=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp682->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp682);
Cyc_Toc_exp_to_c(nv,_tmp682);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp59C=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp59D=_tmp59C;void*_tmp5B3;struct Cyc_Absyn_Tqual _tmp5B2;void*_tmp5B1;union Cyc_Absyn_Constraint*_tmp5B0;union Cyc_Absyn_Constraint*_tmp5AF;union Cyc_Absyn_Constraint*_tmp5AE;_LL384: _tmp5B3=_tmp59D.elt_typ;_tmp5B2=_tmp59D.elt_tq;_tmp5B1=(_tmp59D.ptr_atts).rgn;_tmp5B0=(_tmp59D.ptr_atts).nullable;_tmp5AF=(_tmp59D.ptr_atts).bounds;_tmp5AE=(_tmp59D.ptr_atts).zero_term;_LL385:;
{void*_tmp59E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5AF);void*_tmp59F=_tmp59E;struct Cyc_Absyn_Exp*_tmp5AD;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59F)->tag == 1){_LL387: _tmp5AD=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59F)->f1;_LL388: {
# 3307
struct _tuple12 _tmp5A0=Cyc_Evexp_eval_const_uint_exp(_tmp5AD);struct _tuple12 _tmp5A1=_tmp5A0;unsigned int _tmp5AB;int _tmp5AA;_LL38C: _tmp5AB=_tmp5A1.f1;_tmp5AA=_tmp5A1.f2;_LL38D:;
if(_tmp5AA){
if(_tmp5AB < 1)
({void*_tmp5A2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5A3="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp5A3,sizeof(char),44);}),_tag_dyneither(_tmp5A2,sizeof(void*),0));});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5A4=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5A5="inserted null check due to dereference";_tag_dyneither(_tmp5A5,sizeof(char),39);}),_tag_dyneither(_tmp5A4,sizeof(void*),0));});
# 3315
_tmp682->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5A6[1];_tmp5A6[0]=
Cyc_Absyn_new_exp(_tmp682->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5A6,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}}else{
# 3320
if(!Cyc_Evexp_c_can_eval(_tmp5AD))
({void*_tmp5A7=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5A8="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp5A8,sizeof(char),47);}),_tag_dyneither(_tmp5A7,sizeof(void*),0));});
# 3324
_tmp682->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5A9[4];_tmp5A9[3]=
# 3328
Cyc_Absyn_uint_exp(0,0);_tmp5A9[2]=
# 3327
Cyc_Absyn_sizeoftyp_exp(_tmp5B3,0);_tmp5A9[1]=_tmp5AD;_tmp5A9[0]=
# 3326
Cyc_Absyn_new_exp(_tmp682->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5A9,sizeof(struct Cyc_Absyn_Exp*),4));}),0));}
# 3330
goto _LL386;}}else{_LL389: _LL38A: {
# 3333
void*ta1=Cyc_Toc_typ_to_c(_tmp5B3);
_tmp682->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp5B2),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,({struct Cyc_Absyn_Exp*_tmp5AC[3];_tmp5AC[2]=
# 3338
Cyc_Absyn_uint_exp(0,0);_tmp5AC[1]=
# 3337
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5AC[0]=
# 3336
Cyc_Absyn_new_exp(_tmp682->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5AC,sizeof(struct Cyc_Absyn_Exp*),3));}),0));
# 3341
goto _LL386;}}_LL386:;}
# 3343
if(_tmp680  && _tmp67F)
e->r=Cyc_Toc_check_tagged_union(_tmp682,Cyc_Toc_typ_to_c(e1typ),_tmp5B3,_tmp681,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3346
if(is_poly  && _tmp67F)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp59B);
goto _LL280;};};}case 22: _LL2B1: _tmp684=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp683=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL2B2: {
# 3351
int _tmp5B4=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5B5=Cyc_Tcutil_compress((void*)_check_null(_tmp684->topt));
# 3355
{void*_tmp5B6=_tmp5B5;void*_tmp5E0;struct Cyc_Absyn_Tqual _tmp5DF;void*_tmp5DE;union Cyc_Absyn_Constraint*_tmp5DD;union Cyc_Absyn_Constraint*_tmp5DC;union Cyc_Absyn_Constraint*_tmp5DB;struct Cyc_List_List*_tmp5DA;switch(*((int*)_tmp5B6)){case 10: _LL38F: _tmp5DA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5B6)->f1;_LL390:
# 3358
 Cyc_Toc_exp_to_c(nv,_tmp684);
Cyc_Toc_exp_to_c(nv,_tmp683);{
struct _tuple12 _tmp5B7=Cyc_Evexp_eval_const_uint_exp(_tmp683);struct _tuple12 _tmp5B8=_tmp5B7;unsigned int _tmp5BC;int _tmp5BB;_LL396: _tmp5BC=_tmp5B8.f1;_tmp5BB=_tmp5B8.f2;_LL397:;
if(!_tmp5BB)
({void*_tmp5B9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5BA="unknown tuple subscript in translation to C";_tag_dyneither(_tmp5BA,sizeof(char),44);}),_tag_dyneither(_tmp5B9,sizeof(void*),0));});
e->r=Cyc_Toc_aggrmember_exp_r(_tmp684,Cyc_Absyn_fieldname((int)(_tmp5BC + 1)));
goto _LL38E;};case 5: _LL391: _tmp5E0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B6)->f1).elt_typ;_tmp5DF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B6)->f1).elt_tq;_tmp5DE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B6)->f1).ptr_atts).rgn;_tmp5DD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B6)->f1).ptr_atts).nullable;_tmp5DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B6)->f1).ptr_atts).bounds;_tmp5DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B6)->f1).ptr_atts).zero_term;_LL392: {
# 3366
struct Cyc_List_List*_tmp5BD=Cyc_Toc_get_relns(_tmp684);
int _tmp5BE=Cyc_Toc_need_null_check(_tmp684);
int _tmp5BF=Cyc_Toc_in_sizeof(nv);
# 3374
int in_bnds=_tmp5BF;
{void*_tmp5C0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5DC);void*_tmp5C1=_tmp5C0;_LL399: _LL39A:
# 3377
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp5B5,_tmp5BD,_tmp684,_tmp683);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp5C4;_tmp5C4.tag=0;_tmp5C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp5C2[1]={& _tmp5C4};Cyc_Tcutil_warn(e->loc,({const char*_tmp5C3="bounds check necessary for %s";_tag_dyneither(_tmp5C3,sizeof(char),30);}),_tag_dyneither(_tmp5C2,sizeof(void*),1));});});_LL398:;}
# 3384
Cyc_Toc_exp_to_c(nv,_tmp684);
Cyc_Toc_exp_to_c(nv,_tmp683);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp5C5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5DC);void*_tmp5C6=_tmp5C5;struct Cyc_Absyn_Exp*_tmp5D7;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C6)->tag == 1){_LL39C: _tmp5D7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C6)->f1;_LL39D: {
# 3389
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5DD) && _tmp5BE;
void*ta1=Cyc_Toc_typ_to_c(_tmp5E0);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5DF);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5C7=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5C8="inserted null check due to dereference";_tag_dyneither(_tmp5C8,sizeof(char),39);}),_tag_dyneither(_tmp5C7,sizeof(void*),0));});
_tmp684->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5C9[1];_tmp5C9[0]=
Cyc_Absyn_copy_exp(_tmp684);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5C9,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}else{
# 3401
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5DB)){
# 3403
if(!Cyc_Evexp_c_can_eval(_tmp5D7))
({void*_tmp5CA=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5CB="cannot determine subscript is in bounds";_tag_dyneither(_tmp5CB,sizeof(char),40);}),_tag_dyneither(_tmp5CA,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp684);
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,({struct Cyc_Absyn_Exp*_tmp5CC[3];_tmp5CC[2]=_tmp683;_tmp5CC[1]=_tmp5D7;_tmp5CC[0]=_tmp684;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5CC,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));};}else{
# 3409
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp5D7))
({void*_tmp5CD=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5CE="cannot determine subscript is in bounds";_tag_dyneither(_tmp5CE,sizeof(char),40);}),_tag_dyneither(_tmp5CD,sizeof(void*),0));});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5CF=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5D0="inserted null check due to dereference";_tag_dyneither(_tmp5D0,sizeof(char),39);}),_tag_dyneither(_tmp5CF,sizeof(void*),0));});
# 3415
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5D1[4];_tmp5D1[3]=_tmp683;_tmp5D1[2]=
# 3418
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5D1[1]=_tmp5D7;_tmp5D1[0]=_tmp684;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D1,sizeof(struct Cyc_Absyn_Exp*),4));}),0)));}else{
# 3421
if(!Cyc_Evexp_c_can_eval(_tmp5D7))
({void*_tmp5D2=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5D3="cannot determine subscript is in bounds";_tag_dyneither(_tmp5D3,sizeof(char),40);}),_tag_dyneither(_tmp5D2,sizeof(void*),0));});
# 3424
_tmp683->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({struct Cyc_Absyn_Exp*_tmp5D4[2];_tmp5D4[1]=
Cyc_Absyn_copy_exp(_tmp683);_tmp5D4[0]=_tmp5D7;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D4,sizeof(struct Cyc_Absyn_Exp*),2));}));}}}}
# 3427
goto _LL39B;}}else{_LL39E: _LL39F: {
# 3429
void*ta1=Cyc_Toc_typ_to_c(_tmp5E0);
if(in_bnds){
# 3433
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5DF),
Cyc_Toc_member_exp(_tmp684,Cyc_Toc_curr_sp,0)),_tmp683);}else{
# 3438
struct Cyc_Absyn_Exp*_tmp5D5=Cyc_Toc__check_dyneither_subscript_e;
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5DF),
Cyc_Absyn_fncall_exp(_tmp5D5,({struct Cyc_Absyn_Exp*_tmp5D6[3];_tmp5D6[2]=_tmp683;_tmp5D6[1]=
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5D6[0]=_tmp684;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D6,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}
# 3444
goto _LL39B;}}_LL39B:;}
# 3446
goto _LL38E;}default: _LL393: _LL394:
({void*_tmp5D8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5D9="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp5D9,sizeof(char),49);}),_tag_dyneither(_tmp5D8,sizeof(void*),0));});}_LL38E:;}
# 3449
Cyc_Toc_set_lhs(nv,_tmp5B4);
goto _LL280;};}case 23: _LL2B3: _tmp685=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2B4:
# 3452
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp685))->r;else{
# 3457
struct Cyc_List_List*_tmp5E1=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp685);
void*_tmp5E2=Cyc_Toc_add_tuple_type(_tmp5E1);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp685 != 0;(_tmp685=_tmp685->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp685->hd);
dles=({struct Cyc_List_List*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3->hd=({struct _tuple19*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->f1=0;_tmp5E4->f2=(struct Cyc_Absyn_Exp*)_tmp685->hd;_tmp5E4;});_tmp5E3->tl=dles;_tmp5E3;});}}
# 3464
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3467
goto _LL280;case 25: _LL2B5: _tmp686=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2B6:
# 3471
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp686);
{struct Cyc_List_List*_tmp5E5=_tmp686;for(0;_tmp5E5 != 0;_tmp5E5=_tmp5E5->tl){
struct _tuple19*_tmp5E6=(struct _tuple19*)_tmp5E5->hd;struct _tuple19*_tmp5E7=_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E8;_LL3A1: _tmp5E8=_tmp5E7->f2;_LL3A2:;
Cyc_Toc_exp_to_c(nv,_tmp5E8);}}
# 3476
goto _LL280;case 26: _LL2B7: _tmp68A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp689=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp688=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp687=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL2B8: {
# 3480
struct _tuple12 _tmp5E9=Cyc_Evexp_eval_const_uint_exp(_tmp689);struct _tuple12 _tmp5EA=_tmp5E9;unsigned int _tmp5F4;int _tmp5F3;_LL3A4: _tmp5F4=_tmp5EA.f1;_tmp5F3=_tmp5EA.f2;_LL3A5:;{
void*_tmp5EB=Cyc_Toc_typ_to_c((void*)_check_null(_tmp688->topt));
Cyc_Toc_exp_to_c(nv,_tmp688);{
struct Cyc_List_List*es=0;
# 3485
if(!Cyc_Toc_is_zero(_tmp688)){
if(!_tmp5F3)
({void*_tmp5EC=0;Cyc_Tcutil_terr(_tmp689->loc,({const char*_tmp5ED="cannot determine value of constant";_tag_dyneither(_tmp5ED,sizeof(char),35);}),_tag_dyneither(_tmp5EC,sizeof(void*),0));});
{unsigned int i=0;for(0;i < _tmp5F4;++ i){
es=({struct Cyc_List_List*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->hd=({struct _tuple19*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->f1=0;_tmp5EF->f2=_tmp688;_tmp5EF;});_tmp5EE->tl=es;_tmp5EE;});}}
# 3491
if(_tmp687){
struct Cyc_Absyn_Exp*_tmp5F0=Cyc_Toc_cast_it(_tmp5EB,Cyc_Absyn_uint_exp(0,0));
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->hd=({struct _tuple19*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->f1=0;_tmp5F2->f2=_tmp5F0;_tmp5F2;});_tmp5F1->tl=0;_tmp5F1;}));}}
# 3496
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL280;};};}case 27: _LL2B9: _tmp68D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp68C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp68B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL2BA:
# 3501
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL280;case 28: _LL2BB: _tmp691=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp690=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp68F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_tmp68E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp486)->f4;_LL2BC:
# 3506
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp5F5=Cyc_Toc_init_struct(nv,old_typ,_tmp690,0,0,_tmp68F,_tmp691);
e->r=_tmp5F5->r;
e->topt=_tmp5F5->topt;}else{
# 3517
if(_tmp68E == 0)
({void*_tmp5F6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5F7="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp5F7,sizeof(char),38);}),_tag_dyneither(_tmp5F6,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp68E;
# 3521
struct _RegionHandle _tmp5F8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5F8;_push_region(rgn);
{struct Cyc_List_List*_tmp5F9=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp68F,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3525
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68E->impl))->tagged){
# 3527
struct _tuple30*_tmp5FA=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp5F9))->hd;struct _tuple30*_tmp5FB=_tmp5FA;struct Cyc_Absyn_Aggrfield*_tmp608;struct Cyc_Absyn_Exp*_tmp607;_LL3A7: _tmp608=_tmp5FB->f1;_tmp607=_tmp5FB->f2;_LL3A8:;{
void*_tmp5FC=(void*)_check_null(_tmp607->topt);
void*_tmp5FD=_tmp608->type;
Cyc_Toc_exp_to_c(nv,_tmp607);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp5FD) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp5FC))
_tmp607->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp607->r,0));{
# 3536
int i=Cyc_Toc_get_member_offset(_tmp68E,_tmp608->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_List_List*_tmp5FE=({struct _tuple19*_tmp604[2];_tmp604[1]=({struct _tuple19*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->f1=0;_tmp606->f2=_tmp607;_tmp606;});_tmp604[0]=({struct _tuple19*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->f1=0;_tmp605->f2=field_tag_exp;_tmp605;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp604,sizeof(struct _tuple19*),2));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp5FE,0);
struct Cyc_List_List*ds=({void*_tmp601[1];_tmp601[0]=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp603;_tmp603.tag=1;_tmp603.f1=_tmp608->name;_tmp603;});_tmp602;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp601,sizeof(void*),1));});
struct Cyc_List_List*dles=({struct _tuple19*_tmp5FF[1];_tmp5FF[0]=({struct _tuple19*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->f1=ds;_tmp600->f2=umem;_tmp600;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5FF,sizeof(struct _tuple19*),1));});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3545
struct Cyc_List_List*_tmp609=0;
struct Cyc_List_List*_tmp60A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp60A != 0;_tmp60A=_tmp60A->tl){
struct Cyc_List_List*_tmp60B=_tmp5F9;for(0;_tmp60B != 0;_tmp60B=_tmp60B->tl){
if((*((struct _tuple30*)_tmp60B->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp60A->hd){
struct _tuple30*_tmp60C=(struct _tuple30*)_tmp60B->hd;struct _tuple30*_tmp60D=_tmp60C;struct Cyc_Absyn_Aggrfield*_tmp613;struct Cyc_Absyn_Exp*_tmp612;_LL3AA: _tmp613=_tmp60D->f1;_tmp612=_tmp60D->f2;_LL3AB:;{
void*_tmp60E=Cyc_Toc_typ_to_c(_tmp613->type);
void*_tmp60F=Cyc_Toc_typ_to_c((void*)_check_null(_tmp612->topt));
Cyc_Toc_exp_to_c(nv,_tmp612);
# 3555
if(!Cyc_Tcutil_unify(_tmp60E,_tmp60F)){
# 3557
if(!Cyc_Tcutil_is_arithmetic_type(_tmp60E) || !
Cyc_Tcutil_is_arithmetic_type(_tmp60F))
_tmp612=Cyc_Toc_cast_it(_tmp60E,Cyc_Absyn_copy_exp(_tmp612));}
_tmp609=({struct Cyc_List_List*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->hd=({struct _tuple19*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->f1=0;_tmp611->f2=_tmp612;_tmp611;});_tmp610->tl=_tmp609;_tmp610;});
break;};}}}
# 3564
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp609));}}
# 3522
;_pop_region(rgn);};}
# 3568
goto _LL280;case 29: _LL2BD: _tmp693=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp692=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL2BE: {
# 3570
struct Cyc_List_List*fs;
{void*_tmp614=Cyc_Tcutil_compress(_tmp693);void*_tmp615=_tmp614;struct Cyc_List_List*_tmp619;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp615)->tag == 12){_LL3AD: _tmp619=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp615)->f2;_LL3AE:
 fs=_tmp619;goto _LL3AC;}else{_LL3AF: _LL3B0:
({struct Cyc_String_pa_PrintArg_struct _tmp618;_tmp618.tag=0;_tmp618.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp693));({void*_tmp616[1]={& _tmp618};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp617="anon struct has type %s";_tag_dyneither(_tmp617,sizeof(char),24);}),_tag_dyneither(_tmp616,sizeof(void*),1));});});}_LL3AC:;}{
# 3575
struct _RegionHandle _tmp61A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp61A;_push_region(rgn);{
struct Cyc_List_List*_tmp61B=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp692,Cyc_Absyn_StructA,fs);
for(0;_tmp61B != 0;_tmp61B=_tmp61B->tl){
struct _tuple30*_tmp61C=(struct _tuple30*)_tmp61B->hd;struct _tuple30*_tmp61D=_tmp61C;struct Cyc_Absyn_Aggrfield*_tmp621;struct Cyc_Absyn_Exp*_tmp620;_LL3B2: _tmp621=_tmp61D->f1;_tmp620=_tmp61D->f2;_LL3B3:;{
void*_tmp61E=(void*)_check_null(_tmp620->topt);
void*_tmp61F=_tmp621->type;
Cyc_Toc_exp_to_c(nv,_tmp620);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp61F) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp61E))
_tmp620->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp620->r,0));};}
# 3589
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp692);}
# 3591
_npop_handler(0);goto _LL280;
# 3575
;_pop_region(rgn);};}case 30: _LL2BF: _tmp696=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp695=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_tmp694=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp486)->f3;_LL2C0: {
# 3594
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp622=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp623=Cyc_Absyn_var_exp(_tmp622,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp694->name,_tmp695->name));
tag_exp=_tmp695->is_extensible?Cyc_Absyn_var_exp(_tmp694->name,0):
 Cyc_Toc_datatype_tag(_tmp695,_tmp694->name);
mem_exp=_tmp623;{
struct Cyc_List_List*_tmp624=_tmp694->typs;
# 3605
if(Cyc_Toc_is_toplevel(nv)){
# 3607
struct Cyc_List_List*dles=0;
for(0;_tmp696 != 0;(_tmp696=_tmp696->tl,_tmp624=_tmp624->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp696->hd;
void*_tmp625=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp624))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp625))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);
dles=({struct Cyc_List_List*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->hd=({struct _tuple19*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->f1=0;_tmp627->f2=cur_e;_tmp627;});_tmp626->tl=dles;_tmp626;});}
# 3618
dles=({struct Cyc_List_List*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->hd=({struct _tuple19*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->f1=0;_tmp629->f2=tag_exp;_tmp629;});_tmp628->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp628;});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3624
struct Cyc_List_List*_tmp62A=({struct Cyc_List_List*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->hd=
Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp631->tl=0;_tmp631;});
# 3627
{int i=1;for(0;_tmp696 != 0;(((_tmp696=_tmp696->tl,i ++)),_tmp624=_tmp624->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp696->hd;
void*_tmp62B=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp624))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp62B))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp62C=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3637
_tmp62A=({struct Cyc_List_List*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->hd=_tmp62C;_tmp62D->tl=_tmp62A;_tmp62D;});};}}{
# 3639
struct Cyc_Absyn_Stmt*_tmp62E=Cyc_Absyn_exp_stmt(_tmp623,0);
struct Cyc_Absyn_Stmt*_tmp62F=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->hd=_tmp62E;_tmp630->tl=_tmp62A;_tmp630;})),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp622,datatype_ctype,0,_tmp62F,0));};}
# 3643
goto _LL280;};}case 31: _LL2C1: _LL2C2:
# 3645
 goto _LL2C4;case 32: _LL2C3: _LL2C4:
 goto _LL280;case 33: _LL2C5: _tmp69C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).is_calloc;_tmp69B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).rgn;_tmp69A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).elt_type;_tmp699=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).num_elts;_tmp698=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).fat_result;_tmp697=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp486)->f1).inline_call;_LL2C6: {
# 3649
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp69A)));
Cyc_Toc_exp_to_c(nv,_tmp699);
# 3653
if(_tmp698){
struct _tuple1*_tmp632=Cyc_Toc_temp_var();
struct _tuple1*_tmp633=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp69C){
xexp=_tmp699;
if(_tmp69B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp69B;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp632,0));}else{
# 3664
pexp=Cyc_Toc_calloc_exp(*_tmp69A,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp632,0));}
# 3666
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp634[3];_tmp634[2]=
# 3668
Cyc_Absyn_var_exp(_tmp632,0);_tmp634[1]=
# 3667
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp634[0]=Cyc_Absyn_var_exp(_tmp633,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp634,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{
# 3670
if(_tmp69B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp69B;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp697)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,Cyc_Absyn_var_exp(_tmp632,0));else{
# 3676
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp632,0));}}else{
# 3678
pexp=Cyc_Toc_malloc_exp(*_tmp69A,Cyc_Absyn_var_exp(_tmp632,0));}
# 3680
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp635[3];_tmp635[2]=
Cyc_Absyn_var_exp(_tmp632,0);_tmp635[1]=
# 3680
Cyc_Absyn_uint_exp(1,0);_tmp635[0]=Cyc_Absyn_var_exp(_tmp633,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp635,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}{
# 3683
struct Cyc_Absyn_Stmt*_tmp636=Cyc_Absyn_declare_stmt(_tmp632,Cyc_Absyn_uint_typ,_tmp699,
Cyc_Absyn_declare_stmt(_tmp633,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp636);};}else{
# 3688
if(_tmp69C){
if(_tmp69B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp69B;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp699))->r;}else{
# 3694
e->r=(Cyc_Toc_calloc_exp(*_tmp69A,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp699))->r;}}else{
# 3697
if(_tmp69B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp69B;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp697)
e->r=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp699))->r;else{
# 3703
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp699))->r;}}else{
# 3705
e->r=(Cyc_Toc_malloc_exp(*_tmp69A,_tmp699))->r;}}}
# 3709
goto _LL280;}case 34: _LL2C7: _tmp69E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp69D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL2C8: {
# 3718
void*e1_old_typ=(void*)_check_null(_tmp69E->topt);
void*e2_old_typ=(void*)_check_null(_tmp69D->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp637=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp638="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp638,sizeof(char),57);}),_tag_dyneither(_tmp637,sizeof(void*),0));});{
# 3726
unsigned int _tmp639=e->loc;
struct _tuple1*_tmp63A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp63B=Cyc_Absyn_var_exp(_tmp63A,_tmp639);_tmp63B->topt=e1_old_typ;{
struct _tuple1*_tmp63C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp63D=Cyc_Absyn_var_exp(_tmp63C,_tmp639);_tmp63D->topt=e2_old_typ;{
# 3732
struct Cyc_Absyn_Exp*_tmp63E=Cyc_Absyn_assign_exp(Cyc_Tcutil_deep_copy_exp(1,_tmp69E),_tmp63D,_tmp639);_tmp63E->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp63F=Cyc_Absyn_exp_stmt(_tmp63E,_tmp639);
struct Cyc_Absyn_Exp*_tmp640=Cyc_Absyn_assign_exp(Cyc_Tcutil_deep_copy_exp(1,_tmp69D),_tmp63B,_tmp639);_tmp640->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp641=Cyc_Absyn_exp_stmt(_tmp640,_tmp639);
# 3737
struct Cyc_Absyn_Stmt*_tmp642=Cyc_Absyn_declare_stmt(_tmp63A,e1_old_typ,_tmp69E,
Cyc_Absyn_declare_stmt(_tmp63C,e2_old_typ,_tmp69D,
Cyc_Absyn_seq_stmt(_tmp63F,_tmp641,_tmp639),_tmp639),_tmp639);
Cyc_Toc_stmt_to_c(nv,_tmp642);
e->r=Cyc_Toc_stmt_exp_r(_tmp642);
goto _LL280;};};};};};}case 37: _LL2C9: _tmp6A0=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_tmp69F=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp486)->f2;_LL2CA: {
# 3745
void*_tmp643=Cyc_Tcutil_compress((void*)_check_null(_tmp6A0->topt));
Cyc_Toc_exp_to_c(nv,_tmp6A0);
{void*_tmp644=_tmp643;struct Cyc_Absyn_Aggrdecl*_tmp64A;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp644)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp644)->f1).aggr_info).KnownAggr).tag == 2){_LL3B5: _tmp64A=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp644)->f1).aggr_info).KnownAggr).val;_LL3B6: {
# 3749
struct Cyc_Absyn_Exp*_tmp645=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp64A,_tmp69F),0);
struct Cyc_Absyn_Exp*_tmp646=Cyc_Toc_member_exp(_tmp6A0,_tmp69F,0);
struct Cyc_Absyn_Exp*_tmp647=Cyc_Toc_member_exp(_tmp646,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp647,_tmp645,0))->r;
goto _LL3B4;}}else{goto _LL3B7;}}else{_LL3B7: _LL3B8:
({void*_tmp648=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp649="non-aggregate type in tagcheck";_tag_dyneither(_tmp649,sizeof(char),31);}),_tag_dyneither(_tmp648,sizeof(void*),0));});}_LL3B4:;}
# 3756
goto _LL280;}case 36: _LL2CB: _tmp6A1=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2CC:
 Cyc_Toc_stmt_to_c(nv,_tmp6A1);goto _LL280;case 35: _LL2CD: _LL2CE:
({void*_tmp64B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp64C="UnresolvedMem";_tag_dyneither(_tmp64C,sizeof(char),14);}),_tag_dyneither(_tmp64B,sizeof(void*),0));});case 24: _LL2CF: _LL2D0:
({void*_tmp64D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp64E="compoundlit";_tag_dyneither(_tmp64E,sizeof(char),12);}),_tag_dyneither(_tmp64D,sizeof(void*),0));});case 38: _LL2D1: _LL2D2:
({void*_tmp64F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp650="valueof(-)";_tag_dyneither(_tmp650,sizeof(char),11);}),_tag_dyneither(_tmp64F,sizeof(void*),0));});default: _LL2D3: _LL2D4:
({void*_tmp651=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp652="__asm__";_tag_dyneither(_tmp652,sizeof(char),8);}),_tag_dyneither(_tmp651,sizeof(void*),0));});}_LL280:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3789 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3791
return({struct _tuple31*_tmp6A2=_region_malloc(r,sizeof(*_tmp6A2));_tmp6A2->f1=0;_tmp6A2->f2=Cyc_Toc_fresh_label();_tmp6A2->f3=Cyc_Toc_fresh_label();_tmp6A2->f4=sc;_tmp6A2;});}
# 3796
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp6A3=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3802
if((int)(((_tmp6A3->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp6A6=_tmp6A3->orig_pat;if((_tmp6A6.pattern).tag != 1)_throw_match();(_tmp6A6.pattern).val;})->topt);
void*_tmp6A4=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp6A5=_tmp6A4;switch(*((int*)_tmp6A5)){case 0: _LL3BA: _LL3BB:
# 3807
 goto _LL3BD;case 11: _LL3BC: _LL3BD:
 goto _LL3BF;case 12: _LL3BE: _LL3BF:
 goto _LL3B9;default: _LL3C0: _LL3C1:
 v=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(t),v);goto _LL3B9;}_LL3B9:;}{
# 3813
void*_tmp6A7=_tmp6A3->access;void*_tmp6A8=_tmp6A7;struct Cyc_Absyn_Datatypedecl*_tmp6B3;struct Cyc_Absyn_Datatypefield*_tmp6B2;unsigned int _tmp6B1;int _tmp6B0;struct _dyneither_ptr*_tmp6AF;unsigned int _tmp6AE;switch(*((int*)_tmp6A8)){case 0: _LL3C3: _LL3C4:
# 3818
 goto _LL3C2;case 1: _LL3C5: _LL3C6:
# 3823
 if(ps->tl != 0){
void*_tmp6A9=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp6AA=_tmp6A9;struct Cyc_Absyn_Datatypedecl*_tmp6AD;struct Cyc_Absyn_Datatypefield*_tmp6AC;unsigned int _tmp6AB;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AA)->tag == 3){_LL3CE: _tmp6AD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AA)->f1;_tmp6AC=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AA)->f2;_tmp6AB=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AA)->f3;_LL3CF:
# 3826
 ps=ps->tl;
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6AC->name,_tmp6AD->name)),Cyc_Toc_mt_tq),v);
v=Cyc_Absyn_aggrarrow_exp(v,Cyc_Absyn_fieldname((int)(_tmp6AB + 1)),0);
continue;}else{_LL3D0: _LL3D1:
# 3831
 goto _LL3CD;}_LL3CD:;}
# 3834
v=Cyc_Absyn_deref_exp(v,0);
goto _LL3C2;case 2: _LL3C7: _tmp6AE=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp6A8)->f1;_LL3C8:
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6AE + 1)),0);goto _LL3C2;case 4: _LL3C9: _tmp6B0=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6A8)->f1;_tmp6AF=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6A8)->f2;_LL3CA:
# 3838
 v=Cyc_Toc_member_exp(v,_tmp6AF,0);
if(_tmp6B0)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0);
goto _LL3C2;default: _LL3CB: _tmp6B3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6A8)->f1;_tmp6B2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6A8)->f2;_tmp6B1=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6A8)->f3;_LL3CC:
# 3843
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6B1 + 1)),0);
goto _LL3C2;}_LL3C2:;};}
# 3847
return v;}
# 3852
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp6B4=t;struct Cyc_Absyn_Datatypedecl*_tmp6CF;struct Cyc_Absyn_Datatypefield*_tmp6CE;struct _dyneither_ptr*_tmp6CD;int _tmp6CC;int _tmp6CB;struct Cyc_Absyn_Datatypedecl*_tmp6CA;struct Cyc_Absyn_Datatypefield*_tmp6C9;unsigned int _tmp6C8;struct _dyneither_ptr _tmp6C7;int _tmp6C6;void*_tmp6C5;struct Cyc_Absyn_Enumfield*_tmp6C4;struct Cyc_Absyn_Enumdecl*_tmp6C3;struct Cyc_Absyn_Enumfield*_tmp6C2;struct Cyc_Absyn_Exp*_tmp6C1;switch(*((int*)_tmp6B4)){case 0: _LL3D3: _tmp6C1=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6B4)->f1;_LL3D4:
# 3856
 if(_tmp6C1 == 0)return v;else{return _tmp6C1;}case 1: _LL3D5: _LL3D6:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 2: _LL3D7: _LL3D8:
 return Cyc_Absyn_neq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 3: _LL3D9: _tmp6C3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6B4)->f1;_tmp6C2=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6B4)->f2;_LL3DA:
# 3860
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6B6;_tmp6B6.tag=31;_tmp6B6.f1=_tmp6C3;_tmp6B6.f2=_tmp6C2;_tmp6B6;});_tmp6B5;}),0),0);case 4: _LL3DB: _tmp6C5=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6B4)->f1;_tmp6C4=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6B4)->f2;_LL3DC:
# 3862
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6B8;_tmp6B8.tag=32;_tmp6B8.f1=_tmp6C5;_tmp6B8.f2=_tmp6C4;_tmp6B8;});_tmp6B7;}),0),0);case 5: _LL3DD: _tmp6C7=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6B4)->f1;_tmp6C6=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6B4)->f2;_LL3DE:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_float_exp(_tmp6C7,_tmp6C6,0),0);case 6: _LL3DF: _tmp6C8=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6B4)->f1;_LL3E0:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp(_tmp6C8,0),0);case 7: _LL3E1: _tmp6CB=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6B4)->f1;_tmp6CA=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6B4)->f2;_tmp6C9=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6B4)->f3;_LL3E2:
# 3868
 LOOP1: {
void*_tmp6B9=v->r;void*_tmp6BA=_tmp6B9;struct Cyc_Absyn_Exp*_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BB;switch(*((int*)_tmp6BA)){case 13: _LL3E8: _tmp6BB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6BA)->f2;_LL3E9:
 v=_tmp6BB;goto LOOP1;case 19: _LL3EA: _tmp6BC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6BA)->f1;_LL3EB:
 v=_tmp6BC;goto _LL3E7;default: _LL3EC: _LL3ED:
 goto _LL3E7;}_LL3E7:;}
# 3875
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6C9->name,_tmp6CA->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_uint_exp((unsigned int)_tmp6CB,0),0);case 8: _LL3E3: _tmp6CD=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6B4)->f1;_tmp6CC=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6B4)->f2;_LL3E4:
# 3878
 v=Cyc_Toc_member_exp(Cyc_Toc_member_exp(v,_tmp6CD,0),Cyc_Toc_tag_sp,0);
return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp((unsigned int)_tmp6CC,0),0);default: _LL3E5: _tmp6CF=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6B4)->f1;_tmp6CE=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6B4)->f2;_LL3E6:
# 3882
 LOOP2: {
void*_tmp6BD=v->r;void*_tmp6BE=_tmp6BD;struct Cyc_Absyn_Exp*_tmp6C0;struct Cyc_Absyn_Exp*_tmp6BF;switch(*((int*)_tmp6BE)){case 13: _LL3EF: _tmp6BF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6BE)->f2;_LL3F0:
 v=_tmp6BF;goto LOOP2;case 19: _LL3F1: _tmp6C0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6BE)->f1;_LL3F2:
 v=_tmp6C0;goto _LL3EE;default: _LL3F3: _LL3F4:
 goto _LL3EE;}_LL3EE:;}
# 3889
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6CE->name,_tmp6CF->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp6CE->name,0),0);}_LL3D2:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3901
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple32{int f1;void*f2;};
# 3909
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3911
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp6D2=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp6D3=_tmp6D2;void*_tmp6DD;_LL3F6: _tmp6DD=_tmp6D3.f1;_LL3F7:;{
void*_tmp6D4=_tmp6DD;struct Cyc_Absyn_Exp*_tmp6DC;struct _dyneither_ptr*_tmp6DB;switch(*((int*)_tmp6D4)){case 3: _LL3F9: _LL3FA:
# 3917
 goto _LL3FC;case 4: _LL3FB: _LL3FC:
 goto _LL3FE;case 6: _LL3FD: _LL3FE:
# 3920
 continue;case 8: _LL3FF: _tmp6DB=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6D4)->f1;_LL400:
# 3922
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5[0]=({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmp6D6;_tmp6D6.tag=3;_tmp6D6.f1=_tmp6DB;_tmp6D6;});_tmp6D5;});
continue;case 7: _LL401: _LL402:
# 3926
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0: _LL403: _tmp6DC=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6D4)->f1;if(_tmp6DC != 0){_LL404:
# 3930
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmp6D8;_tmp6D8.tag=2;_tmp6D8.f1=_tmp6DC;_tmp6D8;});_tmp6D7;});
return({struct _tuple32 _tmp6D9;_tmp6D9.f1=0;_tmp6D9.f2=k;_tmp6D9;});}else{_LL405: _LL406:
 goto _LL408;}case 1: _LL407: _LL408:
 goto _LL40A;case 2: _LL409: _LL40A:
 goto _LL40C;case 5: _LL40B: _LL40C:
 goto _LL40E;default: _LL40D: _LL40E:
 return({struct _tuple32 _tmp6DA;_tmp6DA.f1=0;_tmp6DA.f2=k;_tmp6DA;});}_LL3F8:;};}
# 3939
return({struct _tuple32 _tmp6DE;_tmp6DE.f1=c;_tmp6DE.f2=k;_tmp6DE;});}
# 3944
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp6DF=p;int _tmp6EC;int _tmp6EB;unsigned int _tmp6EA;void*_tmp6E9;struct Cyc_Absyn_Enumfield*_tmp6E8;struct Cyc_Absyn_Enumdecl*_tmp6E7;struct Cyc_Absyn_Enumfield*_tmp6E6;switch(*((int*)_tmp6DF)){case 3: _LL410: _tmp6E7=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6DF)->f1;_tmp6E6=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6DF)->f2;_LL411:
# 3948
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6E1;_tmp6E1.tag=31;_tmp6E1.f1=_tmp6E7;_tmp6E1.f2=_tmp6E6;_tmp6E1;});_tmp6E0;}),0);goto _LL40F;case 4: _LL412: _tmp6E9=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6DF)->f1;_tmp6E8=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6DF)->f2;_LL413:
# 3950
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6E3;_tmp6E3.tag=32;_tmp6E3.f1=_tmp6E9;_tmp6E3.f2=_tmp6E8;_tmp6E3;});_tmp6E2;}),0);goto _LL40F;case 6: _LL414: _tmp6EA=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6DF)->f1;_LL415:
 _tmp6EB=(int)_tmp6EA;goto _LL417;case 7: _LL416: _tmp6EB=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6DF)->f1;_LL417:
 _tmp6EC=_tmp6EB;goto _LL419;case 8: _LL418: _tmp6EC=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6DF)->f2;_LL419:
# 3954
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp6EC,0);
goto _LL40F;default: _LL41A: _LL41B:
({void*_tmp6E4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6E5="compile_pat_test_as_case!";_tag_dyneither(_tmp6E5,sizeof(char),26);}),_tag_dyneither(_tmp6E4,sizeof(void*),0));});}_LL40F:;}
# 3958
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED[0]=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp6EE;_tmp6EE.tag=17;_tmp6EE.f1=e;_tmp6EE;});_tmp6ED;}),0);}
# 3962
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0);}struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3971
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3974
void*t=(void*)_check_null(p->topt);
void*_tmp6EF=p->r;void*_tmp6F0=_tmp6EF;union Cyc_Absyn_AggrInfoU _tmp728;struct Cyc_List_List*_tmp727;struct Cyc_List_List*_tmp726;struct Cyc_List_List*_tmp725;struct Cyc_Absyn_Pat*_tmp724;struct Cyc_Absyn_Datatypedecl*_tmp723;struct Cyc_Absyn_Datatypefield*_tmp722;struct Cyc_List_List*_tmp721;struct Cyc_Absyn_Vardecl*_tmp720;struct Cyc_Absyn_Pat*_tmp71F;struct Cyc_Absyn_Vardecl*_tmp71E;struct Cyc_Absyn_Vardecl*_tmp71D;struct Cyc_Absyn_Pat*_tmp71C;struct Cyc_Absyn_Vardecl*_tmp71B;switch(*((int*)_tmp6F0)){case 2: _LL41D: _tmp71B=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp6F0)->f2;_LL41E: {
# 3977
struct Cyc_Absyn_Pat*_tmp6F1=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp6F1->topt=p->topt;
_tmp71D=_tmp71B;_tmp71C=_tmp6F1;goto _LL420;}case 1: _LL41F: _tmp71D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6F0)->f1;_tmp71C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6F0)->f2;_LL420:
# 3982
*decls=({struct Cyc_List_List*_tmp6F2=_region_malloc(rgn,sizeof(*_tmp6F2));_tmp6F2->hd=_tmp71D;_tmp6F2->tl=*decls;_tmp6F2;});{
struct Cyc_Absyn_Stmt*_tmp6F3=Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6F5;_tmp6F5.tag=4;_tmp6F5.f1=_tmp71D;_tmp6F5;});_tmp6F4;}),0),Cyc_Absyn_copy_exp(path),0);
return Cyc_Toc_seq_stmt_opt(_tmp6F3,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp71C));};case 4: _LL421: _tmp71E=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp6F0)->f2;_LL422:
# 3987
*decls=({struct Cyc_List_List*_tmp6F6=_region_malloc(rgn,sizeof(*_tmp6F6));_tmp6F6->hd=_tmp71E;_tmp6F6->tl=*decls;_tmp6F6;});
return Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6F8;_tmp6F8.tag=4;_tmp6F8.f1=_tmp71E;_tmp6F8;});_tmp6F7;}),0),Cyc_Absyn_copy_exp(path),0);case 0: _LL423: _LL424:
 return 0;case 3: _LL425: _tmp720=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6F0)->f1;_tmp71F=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6F0)->f2;_LL426:
# 3992
*decls=({struct Cyc_List_List*_tmp6F9=_region_malloc(rgn,sizeof(*_tmp6F9));_tmp6F9->hd=_tmp720;_tmp6F9->tl=*decls;_tmp6F9;});
_tmp720->type=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);{
# 3995
struct Cyc_Absyn_Stmt*_tmp6FA=Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6FC;_tmp6FC.tag=4;_tmp6FC.f1=_tmp720;_tmp6FC;});_tmp6FB;}),0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path))),0);
return Cyc_Toc_seq_stmt_opt(_tmp6FA,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp71F));};case 9: _LL427: _LL428:
# 4000
 goto _LL42A;case 10: _LL429: _LL42A:
 goto _LL42C;case 11: _LL42B: _LL42C:
 goto _LL42E;case 12: _LL42D: _LL42E:
 goto _LL430;case 13: _LL42F: _LL430:
 goto _LL432;case 14: _LL431: _LL432:
 return 0;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F0)->f1)->r)->tag == 8){_LL433: _tmp723=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F0)->f1)->r)->f1;_tmp722=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F0)->f1)->r)->f2;_tmp721=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F0)->f1)->r)->f3;_LL434:
# 4009
 if(_tmp721 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp722->name,_tmp723->name);
void*_tmp6FD=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
path=Cyc_Toc_cast_it(_tmp6FD,path);{
int cnt=1;
struct Cyc_List_List*_tmp6FE=_tmp722->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp721 != 0;(((_tmp721=_tmp721->tl,_tmp6FE=((struct Cyc_List_List*)_check_null(_tmp6FE))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp6FF=(struct Cyc_Absyn_Pat*)_tmp721->hd;
if(_tmp6FF->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp700=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp6FE))->hd)).f2;
void*_tmp701=(void*)_check_null(_tmp6FF->topt);
void*_tmp702=Cyc_Toc_typ_to_c_array(_tmp701);
struct Cyc_Absyn_Exp*_tmp703=Cyc_Absyn_aggrarrow_exp(path,Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp700)))
_tmp703=Cyc_Toc_cast_it(_tmp702,_tmp703);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp703,_tmp6FF));};}
# 4027
return s;};};}else{_LL43D: _tmp724=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F0)->f1;_LL43E:
# 4076
 return Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Absyn_deref_exp(path,0),_tmp724);}case 8: _LL435: _tmp725=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp6F0)->f3;_LL436:
# 4029
 _tmp726=_tmp725;goto _LL438;case 5: _LL437: _tmp726=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp6F0)->f1;_LL438: {
# 4031
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp726 != 0;(_tmp726=_tmp726->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp704=(struct Cyc_Absyn_Pat*)_tmp726->hd;
if(_tmp704->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp705=(void*)_check_null(_tmp704->topt);
struct _dyneither_ptr*_tmp706=Cyc_Absyn_fieldname(cnt);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Toc_member_exp(path,_tmp706,0),_tmp704));};}
# 4041
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F0)->f1 == 0){_LL439: _LL43A:
({void*_tmp707=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp708="unresolved aggregate pattern!";_tag_dyneither(_tmp708,sizeof(char),30);}),_tag_dyneither(_tmp707,sizeof(void*),0));});}else{_LL43B: _tmp728=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F0)->f1)->aggr_info;_tmp727=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F0)->f3;_LL43C: {
# 4045
struct Cyc_Absyn_Aggrdecl*_tmp709=Cyc_Absyn_get_known_aggrdecl(_tmp728);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp727 != 0;_tmp727=_tmp727->tl){
struct _tuple33*_tmp70A=(struct _tuple33*)_tmp727->hd;
struct Cyc_Absyn_Pat*_tmp70B=(*_tmp70A).f2;
if(_tmp70B->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp70C=(void*)((struct Cyc_List_List*)_check_null((*_tmp70A).f1))->hd;void*_tmp70D=_tmp70C;struct _dyneither_ptr*_tmp710;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp70D)->tag == 1){_LL446: _tmp710=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp70D)->f1;_LL447:
 f=_tmp710;goto _LL445;}else{_LL448: _LL449:
({void*_tmp70E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp70F="multiple designators in pattern!";_tag_dyneither(_tmp70F,sizeof(char),33);}),_tag_dyneither(_tmp70E,sizeof(void*),0));});}_LL445:;}{
# 4057
void*_tmp711=(void*)_check_null(_tmp70B->topt);
void*_tmp712=Cyc_Toc_typ_to_c_array(_tmp711);
struct Cyc_Absyn_Exp*_tmp713=Cyc_Toc_member_exp(path,f,0);
# 4061
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp709->impl))->tagged)_tmp713=Cyc_Toc_member_exp(_tmp713,Cyc_Toc_val_sp,0);{
void*_tmp714=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp709->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp714))
_tmp713=Cyc_Toc_cast_it(_tmp712,_tmp713);else{
if(!Cyc_Toc_is_array_type(_tmp714) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp714)))
# 4068
_tmp713=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp712,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp713,0)),0);}
# 4071
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp713,_tmp70B));};};};}
# 4073
return s;}}case 15: _LL43F: _LL440:
# 4078
({void*_tmp715=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp716="unknownid pat";_tag_dyneither(_tmp716,sizeof(char),14);}),_tag_dyneither(_tmp715,sizeof(void*),0));});case 16: _LL441: _LL442:
({void*_tmp717=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp718="unknowncall pat";_tag_dyneither(_tmp718,sizeof(char),16);}),_tag_dyneither(_tmp717,sizeof(void*),0));});default: _LL443: _LL444:
({void*_tmp719=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp71A="exp pat";_tag_dyneither(_tmp71A,sizeof(char),8);}),_tag_dyneither(_tmp719,sizeof(void*),0));});}_LL41C:;}struct _tuple34{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4089
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4097
void*_tmp729=dopt;struct Cyc_List_List*_tmp768;struct Cyc_List_List*_tmp767;void*_tmp766;struct Cyc_Tcpat_Rhs*_tmp765;if(_tmp729 == 0){_LL44B: _LL44C:
# 4099
 return Cyc_Absyn_skip_stmt(0);}else{switch(*((int*)_tmp729)){case 0: _LL44D: _LL44E:
 return Cyc_Toc_throw_match_stmt();case 1: _LL44F: _tmp765=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp729)->f1;_LL450:
# 4104
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp72A=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp72B=_tmp72A;int*_tmp732;struct _dyneither_ptr*_tmp731;struct _dyneither_ptr*_tmp730;struct Cyc_Absyn_Switch_clause*_tmp72F;_LL454: _tmp732=(int*)& _tmp72B->f1;_tmp731=_tmp72B->f2;_tmp730=_tmp72B->f3;_tmp72F=_tmp72B->f4;_LL455:;{
struct Cyc_Absyn_Stmt*_tmp72C=_tmp72F->body;
if(_tmp72C == _tmp765->rhs){
# 4109
if(*_tmp732)
return Cyc_Absyn_goto_stmt(_tmp731,0);
*_tmp732=1;{
# 4113
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp72F->pattern);
# 4116
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp730,_tmp72F->body,0);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0);
res=Cyc_Absyn_label_stmt(_tmp731,res,0);
*bodies=({struct Cyc_List_List*_tmp72D=_region_malloc(rgn,sizeof(*_tmp72D));_tmp72D->hd=({struct _tuple34*_tmp72E=_region_malloc(rgn,sizeof(*_tmp72E));_tmp72E->f1=nv;_tmp72E->f2=_tmp730;_tmp72E->f3=_tmp72C;_tmp72E;});_tmp72D->tl=*bodies;_tmp72D;});
return res;};}};}
# 4124
({void*_tmp733=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp734="couldn't find rhs!";_tag_dyneither(_tmp734,sizeof(char),19);}),_tag_dyneither(_tmp733,sizeof(void*),0));});default: _LL451: _tmp768=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp729)->f1;_tmp767=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp729)->f2;_tmp766=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp729)->f3;_LL452: {
# 4127
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp766,lscs,v);
# 4129
struct Cyc_Absyn_Exp*_tmp735=Cyc_Toc_compile_path(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp768),Cyc_Absyn_var_exp(v,0));
struct Cyc_List_List*_tmp736=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp767);
# 4132
struct _tuple32 _tmp737=Cyc_Toc_admits_switch(_tmp736);struct _tuple32 _tmp738=_tmp737;int _tmp764;void*_tmp763;_LL457: _tmp764=_tmp738.f1;_tmp763=_tmp738.f2;_LL458:;
if(_tmp764 > 1){
# 4136
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749->hd=({struct Cyc_Absyn_Switch_clause*_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp74A->pat_vars=0;_tmp74A->where_clause=0;_tmp74A->body=res;_tmp74A->loc=0;_tmp74A;});_tmp749->tl=0;_tmp749;});
# 4139
for(0;_tmp736 != 0;_tmp736=_tmp736->tl){
struct _tuple29 _tmp739=*((struct _tuple29*)_tmp736->hd);struct _tuple29 _tmp73A=_tmp739;void*_tmp740;void*_tmp73F;_LL45A: _tmp740=_tmp73A.f1;_tmp73F=_tmp73A.f2;_LL45B:;{
# 4142
struct Cyc_Absyn_Pat*_tmp73B=Cyc_Toc_compile_pat_test_as_case(_tmp740);
# 4144
struct Cyc_Absyn_Stmt*_tmp73C=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp73F,lscs,v);
# 4146
new_lscs=({struct Cyc_List_List*_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D->hd=({struct Cyc_Absyn_Switch_clause*_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E->pattern=_tmp73B;_tmp73E->pat_vars=0;_tmp73E->where_clause=0;_tmp73E->body=_tmp73C;_tmp73E->loc=0;_tmp73E;});_tmp73D->tl=new_lscs;_tmp73D;});};}
# 4148
{void*_tmp741=_tmp763;struct _dyneither_ptr*_tmp746;switch(*((int*)_tmp741)){case 1: _LL45D: _LL45E:
# 4151
 LOOP1: {
void*_tmp742=_tmp735->r;void*_tmp743=_tmp742;struct Cyc_Absyn_Exp*_tmp745;struct Cyc_Absyn_Exp*_tmp744;switch(*((int*)_tmp743)){case 13: _LL466: _tmp744=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp743)->f2;_LL467:
 _tmp735=_tmp744;goto LOOP1;case 19: _LL468: _tmp745=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp743)->f1;_LL469:
 _tmp735=_tmp745;goto _LL465;default: _LL46A: _LL46B:
 goto _LL465;}_LL465:;}
# 4157
_tmp735=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),_tmp735),0);goto _LL45C;case 3: _LL45F: _tmp746=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp741)->f1;_LL460:
# 4160
 _tmp735=Cyc_Toc_member_exp(Cyc_Toc_member_exp(_tmp735,_tmp746,0),Cyc_Toc_tag_sp,0);goto _LL45C;case 2: _LL461: _LL462:
# 4162
 goto _LL45C;default: _LL463: _LL464:
 goto _LL45C;}_LL45C:;}
# 4165
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp747=_cycalloc(sizeof(*_tmp747));_tmp747[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp748;_tmp748.tag=10;_tmp748.f1=_tmp735;_tmp748.f2=new_lscs;_tmp748.f3=0;_tmp748;});_tmp747;}),0);}else{
# 4169
void*_tmp74B=_tmp763;struct Cyc_Absyn_Exp*_tmp762;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp74B)->tag == 2){_LL46D: _tmp762=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp74B)->f1;_LL46E: {
# 4171
struct Cyc_List_List*_tmp74C=_tmp736;void*_tmp75B;struct Cyc_Tcpat_Rhs*_tmp75A;if(_tmp74C != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp74C)->hd)->f2)->tag == 1){if(((struct Cyc_List_List*)_tmp74C)->tl == 0){_LL472: _tmp75B=((struct _tuple29*)_tmp74C->hd)->f1;_tmp75A=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp74C->hd)->f2)->f1;_LL473:
# 4177
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp74D=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp74E=_tmp74D;int*_tmp755;struct _dyneither_ptr*_tmp754;struct _dyneither_ptr*_tmp753;struct Cyc_Absyn_Switch_clause*_tmp752;_LL477: _tmp755=(int*)& _tmp74E->f1;_tmp754=_tmp74E->f2;_tmp753=_tmp74E->f3;_tmp752=_tmp74E->f4;_LL478:;{
struct Cyc_Absyn_Stmt*_tmp74F=_tmp752->body;
if(_tmp74F == _tmp75A->rhs){
# 4182
if(*_tmp755)
return Cyc_Absyn_goto_stmt(_tmp754,0);
*_tmp755=1;{
# 4186
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp752->pattern);
# 4189
Cyc_Toc_exp_to_c(nv,_tmp762);{
# 4192
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp753,_tmp752->body,0);
r=Cyc_Absyn_ifthenelse_stmt(_tmp762,r,res,0);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0);
r=Cyc_Absyn_label_stmt(_tmp754,r,0);
*bodies=({struct Cyc_List_List*_tmp750=_region_malloc(rgn,sizeof(*_tmp750));_tmp750->hd=({struct _tuple34*_tmp751=_region_malloc(rgn,sizeof(*_tmp751));_tmp751->f1=nv;_tmp751->f2=_tmp753;_tmp751->f3=_tmp74F;_tmp751;});_tmp750->tl=*bodies;_tmp750;});
return r;};};}};}
# 4201
({void*_tmp756=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp757="couldn't find rhs!";_tag_dyneither(_tmp757,sizeof(char),19);}),_tag_dyneither(_tmp756,sizeof(void*),0));});}else{goto _LL474;}}else{goto _LL474;}}else{_LL474: _LL475:
({void*_tmp758=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp759="bad where clause in match compiler";_tag_dyneither(_tmp759,sizeof(char),35);}),_tag_dyneither(_tmp758,sizeof(void*),0));});}_LL471:;}}else{_LL46F: _LL470:
# 4206
 for(0;_tmp736 != 0;_tmp736=_tmp736->tl){
struct _tuple29 _tmp75C=*((struct _tuple29*)_tmp736->hd);struct _tuple29 _tmp75D=_tmp75C;void*_tmp761;void*_tmp760;_LL47A: _tmp761=_tmp75D.f1;_tmp760=_tmp75D.f2;_LL47B:;{
struct Cyc_Absyn_Exp*_tmp75E=Cyc_Toc_compile_pat_test(_tmp735,_tmp761);
struct Cyc_Absyn_Stmt*_tmp75F=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp760,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp75E,_tmp75F,res,0);};}}_LL46C:;}
# 4214
return res;}}}_LL44A:;}
# 4224
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4226
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple34*_tmp769=(struct _tuple34*)bodies->hd;struct _tuple34*_tmp76A=_tmp769;struct Cyc_Toc_Env**_tmp76C;struct Cyc_Absyn_Stmt*_tmp76B;_LL47D: _tmp76C=(struct Cyc_Toc_Env**)& _tmp76A->f1;_tmp76B=_tmp76A->f3;_LL47E:;
if(_tmp76B == s)return _tmp76C;}
# 4232
return 0;}
# 4236
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4239
void*_tmp76D=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4242
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp76E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp76E;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp76F=Cyc_Toc_share_env(rgn,nv);
# 4249
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4254
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp76F,& mydecls,& mybodies,dopt,lscs,v);
# 4262
{struct Cyc_List_List*_tmp770=lscs;for(0;_tmp770 != 0;_tmp770=_tmp770->tl){
struct _tuple31*_tmp771=(struct _tuple31*)_tmp770->hd;struct _tuple31*_tmp772=_tmp771;struct _dyneither_ptr*_tmp77A;struct Cyc_Absyn_Switch_clause*_tmp779;_LL480: _tmp77A=_tmp772->f3;_tmp779=_tmp772->f4;_LL481:;{
struct Cyc_Absyn_Stmt*s=_tmp779->body;
# 4266
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4269
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp773=*envp;
# 4272
if(_tmp770->tl != 0){
struct _tuple31*_tmp774=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp770->tl))->hd;struct _tuple31*_tmp775=_tmp774;struct _dyneither_ptr*_tmp778;struct Cyc_Absyn_Switch_clause*_tmp777;_LL483: _tmp778=_tmp775->f3;_tmp777=_tmp775->f4;_LL484:;{
# 4276
struct Cyc_Toc_Env**_tmp776=Cyc_Toc_find_case_env(mybodies,_tmp777->body);
# 4281
if(_tmp776 == 0)
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp773,end_l),s);else{
# 4285
struct Cyc_List_List*vs=0;
if(_tmp777->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp777->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 4290
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn,_tmp773,end_l,_tmp778,vs),s);}};}else{
# 4294
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp773,end_l),s);}};};}}{
# 4298
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4300
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp77B=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
_tmp77B->name=Cyc_Toc_temp_var();
_tmp77B->type=Cyc_Toc_typ_to_c_array(_tmp77B->type);
res=Cyc_Absyn_decl_stmt(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp77D;_tmp77D.tag=0;_tmp77D.f1=_tmp77B;_tmp77D;});_tmp77C;}),0),res,0);}
# 4307
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;
_npop_handler(0);return;};}
# 4246
;_pop_region(rgn);};}
# 4313
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 4315
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4320
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4322
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E->hd=
Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp77E->tl=0;_tmp77E;}),0),0);}
# 4326
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4331
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4333
while(1){
void*_tmp77F=s->r;void*_tmp780=_tmp77F;struct Cyc_Absyn_Stmt*_tmp7DB;struct Cyc_List_List*_tmp7DA;void*_tmp7D9;struct Cyc_Absyn_Stmt*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7D7;struct _dyneither_ptr*_tmp7D6;struct Cyc_Absyn_Stmt*_tmp7D5;struct Cyc_Absyn_Decl*_tmp7D4;struct Cyc_Absyn_Stmt*_tmp7D3;struct Cyc_List_List*_tmp7D2;struct Cyc_Absyn_Switch_clause**_tmp7D1;struct Cyc_Absyn_Exp*_tmp7D0;struct Cyc_List_List*_tmp7CF;void*_tmp7CE;struct Cyc_Absyn_Exp*_tmp7CD;struct Cyc_Absyn_Exp*_tmp7CC;struct Cyc_Absyn_Exp*_tmp7CB;struct Cyc_Absyn_Stmt*_tmp7CA;struct Cyc_Absyn_Exp*_tmp7C9;struct Cyc_Absyn_Stmt*_tmp7C8;struct Cyc_Absyn_Exp*_tmp7C7;struct Cyc_Absyn_Stmt*_tmp7C6;struct Cyc_Absyn_Stmt*_tmp7C5;struct Cyc_Absyn_Exp*_tmp7C4;struct Cyc_Absyn_Stmt*_tmp7C3;struct Cyc_Absyn_Stmt*_tmp7C2;struct Cyc_Absyn_Exp*_tmp7C1;switch(*((int*)_tmp780)){case 0: _LL486: _LL487:
# 4336
 return;case 1: _LL488: _tmp7C1=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_LL489:
# 4338
 Cyc_Toc_exp_to_c(nv,_tmp7C1);
return;case 2: _LL48A: _tmp7C3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_tmp7C2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp780)->f2;_LL48B:
# 4341
 Cyc_Toc_stmt_to_c(nv,_tmp7C3);
s=_tmp7C2;
continue;case 3: _LL48C: _tmp7C4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_LL48D: {
# 4345
void*topt=0;
if(_tmp7C4 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7C4->topt));
Cyc_Toc_exp_to_c(nv,_tmp7C4);}{
# 4351
int _tmp781=Cyc_Toc_get_npop(s);
if(_tmp781 > 0){
if(topt != 0){
struct _tuple1*_tmp782=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp783=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp782,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp782,topt,_tmp7C4,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(_tmp781),_tmp783,0),0))->r;}else{
# 4360
Cyc_Toc_do_npop_before(_tmp781,s);}}
# 4362
return;};}case 4: _LL48E: _tmp7C7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_tmp7C6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp780)->f2;_tmp7C5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp780)->f3;_LL48F:
# 4364
 Cyc_Toc_exp_to_c(nv,_tmp7C7);
Cyc_Toc_stmt_to_c(nv,_tmp7C6);
s=_tmp7C5;
continue;case 5: _LL490: _tmp7C9=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp780)->f1).f1;_tmp7C8=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp780)->f2;_LL491:
# 4369
 Cyc_Toc_exp_to_c(nv,_tmp7C9);{
struct _RegionHandle _tmp784=_new_region("temp");struct _RegionHandle*temp=& _tmp784;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7C8);
# 4373
_npop_handler(0);return;
# 4370
;_pop_region(temp);};case 6: _LL492: _LL493: {
# 4375
struct Cyc_Toc_Env*_tmp785=nv;struct _dyneither_ptr**_tmp786;_LL4A7: _tmp786=_tmp785->break_lab;_LL4A8:;
if(_tmp786 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp786);
# 4379
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;}case 7: _LL494: _LL495: {
# 4382
struct Cyc_Toc_Env*_tmp787=nv;struct _dyneither_ptr**_tmp788;_LL4AA: _tmp788=_tmp787->continue_lab;_LL4AB:;
if(_tmp788 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp788);
goto _LL497;}case 8: _LL496: _LL497:
# 4388
 Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;case 9: _LL498: _tmp7CD=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_tmp7CC=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp780)->f2).f1;_tmp7CB=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp780)->f3).f1;_tmp7CA=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp780)->f4;_LL499:
# 4392
 Cyc_Toc_exp_to_c(nv,_tmp7CD);Cyc_Toc_exp_to_c(nv,_tmp7CC);Cyc_Toc_exp_to_c(nv,_tmp7CB);{
struct _RegionHandle _tmp789=_new_region("temp");struct _RegionHandle*temp=& _tmp789;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7CA);
# 4396
_npop_handler(0);return;
# 4393
;_pop_region(temp);};case 10: _LL49A: _tmp7D0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_tmp7CF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp780)->f2;_tmp7CE=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp780)->f3;_LL49B:
# 4398
 Cyc_Toc_xlate_switch(nv,s,_tmp7D0,_tmp7CF,_tmp7CE);
return;case 11: _LL49C: _tmp7D2=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_tmp7D1=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp780)->f2;_LL49D: {
# 4401
struct Cyc_Toc_Env*_tmp78A=nv;struct Cyc_Toc_FallthruInfo*_tmp795;_LL4AD: _tmp795=_tmp78A->fallthru_info;_LL4AE:;
if(_tmp795 == 0)
({void*_tmp78B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp78C="fallthru in unexpected place";_tag_dyneither(_tmp78C,sizeof(char),29);}),_tag_dyneither(_tmp78B,sizeof(void*),0));});{
struct Cyc_Toc_FallthruInfo _tmp78D=*_tmp795;struct Cyc_Toc_FallthruInfo _tmp78E=_tmp78D;struct _dyneither_ptr*_tmp794;struct Cyc_List_List*_tmp793;_LL4B0: _tmp794=_tmp78E.label;_tmp793=_tmp78E.binders;_LL4B1:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp794,0);
# 4407
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s2);{
struct Cyc_List_List*_tmp78F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp793);
struct Cyc_List_List*_tmp790=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7D2);
for(0;_tmp78F != 0;(_tmp78F=_tmp78F->tl,_tmp790=_tmp790->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp790))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp791=_cycalloc(sizeof(*_tmp791));_tmp791[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp792;_tmp792.tag=5;_tmp792.f1=(struct Cyc_Absyn_Vardecl*)_tmp78F->hd;_tmp792;});_tmp791;}),0),(struct Cyc_Absyn_Exp*)_tmp790->hd,0),s2,0);}
# 4415
s->r=s2->r;
return;};};};}case 12: _LL49E: _tmp7D4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_tmp7D3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp780)->f2;_LL49F:
# 4421
{void*_tmp796=_tmp7D4->r;void*_tmp797=_tmp796;struct Cyc_Absyn_Tvar*_tmp7B4;struct Cyc_Absyn_Vardecl*_tmp7B3;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Fndecl*_tmp7B1;struct Cyc_List_List*_tmp7B0;struct Cyc_Absyn_Pat*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7AE;void*_tmp7AD;struct Cyc_Absyn_Vardecl*_tmp7AC;switch(*((int*)_tmp797)){case 0: _LL4B3: _tmp7AC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp797)->f1;_LL4B4:
 Cyc_Toc_local_decl_to_c(nv,_tmp7AC,_tmp7D3);goto _LL4B2;case 2: _LL4B5: _tmp7AF=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp797)->f1;_tmp7AE=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp797)->f3;_tmp7AD=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp797)->f4;_LL4B6:
# 4429
{void*_tmp798=_tmp7AF->r;void*_tmp799=_tmp798;struct Cyc_Absyn_Vardecl*_tmp79C;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp799)->tag == 1){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp799)->f2)->r)->tag == 0){_LL4C0: _tmp79C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp799)->f1;_LL4C1:
# 4431
 _tmp79C->name=Cyc_Toc_temp_var();
_tmp79C->initializer=_tmp7AE;
_tmp7D4->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp79A=_cycalloc(sizeof(*_tmp79A));_tmp79A[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp79B;_tmp79B.tag=0;_tmp79B.f1=_tmp79C;_tmp79B;});_tmp79A;});
Cyc_Toc_local_decl_to_c(nv,_tmp79C,_tmp7D3);
goto _LL4BF;}else{goto _LL4C2;}}else{_LL4C2: _LL4C3:
# 4440
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7AF,_tmp7AD,(void*)_check_null(_tmp7AE->topt),_tmp7AE,_tmp7D3))->r;
goto _LL4BF;}_LL4BF:;}
# 4443
goto _LL4B2;case 3: _LL4B7: _tmp7B0=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp797)->f1;_LL4B8: {
# 4447
struct Cyc_List_List*_tmp79D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7B0);
if(_tmp79D == 0)
({void*_tmp79E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp79F="empty Letv_d";_tag_dyneither(_tmp79F,sizeof(char),13);}),_tag_dyneither(_tmp79E,sizeof(void*),0));});
_tmp7D4->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7A1;_tmp7A1.tag=0;_tmp7A1.f1=(struct Cyc_Absyn_Vardecl*)_tmp79D->hd;_tmp7A1;});_tmp7A0;});
_tmp79D=_tmp79D->tl;
for(0;_tmp79D != 0;_tmp79D=_tmp79D->tl){
struct Cyc_Absyn_Decl*_tmp7A2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7A3=_cycalloc(sizeof(*_tmp7A3));_tmp7A3[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7A4;_tmp7A4.tag=0;_tmp7A4.f1=(struct Cyc_Absyn_Vardecl*)_tmp79D->hd;_tmp7A4;});_tmp7A3;}),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7A2,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4456
Cyc_Toc_stmt_to_c(nv,s);
goto _LL4B2;}case 1: _LL4B9: _tmp7B1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp797)->f1;_LL4BA:
# 4459
 Cyc_Toc_fndecl_to_c(nv,_tmp7B1,0);
Cyc_Toc_stmt_to_c(nv,_tmp7D3);
goto _LL4B2;case 4: _LL4BB: _tmp7B4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp797)->f1;_tmp7B3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp797)->f2;_tmp7B2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp797)->f3;_LL4BC: {
# 4463
struct Cyc_Absyn_Stmt*_tmp7A5=_tmp7D3;
# 4471
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7B3->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4478
Cyc_Toc_stmt_to_c(nv,_tmp7A5);
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp7A5,0))->r;else{
if((unsigned int)_tmp7B2){
Cyc_Toc_exp_to_c(nv,_tmp7B2);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp7B2,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp7A5,0))->r;};}else{
# 4487
s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp7A7=_cycalloc(sizeof(*_tmp7A7));_tmp7A7->hd=
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0);_tmp7A7->tl=0;_tmp7A7;}),0),
# 4492
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*_tmp7A8[1];_tmp7A8[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7A8,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),
Cyc_Absyn_seq_stmt(_tmp7A5,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp7A9[1];_tmp7A9[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7A9,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r;}}
# 4498
return;}default: _LL4BD: _LL4BE:
({void*_tmp7AA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7AB="bad nested declaration within function";_tag_dyneither(_tmp7AB,sizeof(char),39);}),_tag_dyneither(_tmp7AA,sizeof(void*),0));});}_LL4B2:;}
# 4501
return;case 13: _LL4A0: _tmp7D6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_tmp7D5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp780)->f2;_LL4A1:
# 4503
 s=_tmp7D5;continue;case 14: _LL4A2: _tmp7D8=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_tmp7D7=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp780)->f2).f1;_LL4A3: {
# 4505
struct _RegionHandle _tmp7B5=_new_region("temp");struct _RegionHandle*temp=& _tmp7B5;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7D8);
Cyc_Toc_exp_to_c(nv,_tmp7D7);
# 4509
_npop_handler(0);return;
# 4505
;_pop_region(temp);}default: _LL4A4: _tmp7DB=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp780)->f1;_tmp7DA=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp780)->f2;_tmp7D9=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp780)->f3;_LL4A5: {
# 4523 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4533
e_exp->topt=e_typ;{
struct _RegionHandle _tmp7B6=_new_region("temp");struct _RegionHandle*temp=& _tmp7B6;_push_region(temp);
# 4536
Cyc_Toc_stmt_to_c(nv,_tmp7DB);{
struct Cyc_Absyn_Stmt*_tmp7B7=Cyc_Absyn_seq_stmt(_tmp7DB,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4541
struct Cyc_Absyn_Stmt*_tmp7B8=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));_tmp7BF[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp7C0;_tmp7C0.tag=10;_tmp7C0.f1=e_exp;_tmp7C0.f2=_tmp7DA;_tmp7C0.f3=_tmp7D9;_tmp7C0;});_tmp7BF;}),0);
# 4543
Cyc_Toc_stmt_to_c(nv,_tmp7B8);{
# 4546
struct Cyc_Absyn_Exp*_tmp7B9=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp7BE=_cycalloc(sizeof(*_tmp7BE));_tmp7BE->hd=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp7BE->tl=0;_tmp7BE;}),0);
# 4550
struct Cyc_Absyn_Stmt*_tmp7BA=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD->hd=
Cyc_Absyn_address_exp(h_exp,0);_tmp7BD->tl=0;_tmp7BD;}),0),0);
# 4554
struct Cyc_Absyn_Exp*_tmp7BB=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp7BC=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp7BA,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp7BB,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp7B9,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp7BC,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp7B7,
# 4565
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp7B8,0),0),0),0),0),0))->r;};};
# 4569
_npop_handler(0);return;
# 4534
;_pop_region(temp);};}}_LL485:;}}
# 4578
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp7DC=_new_region("frgn");struct _RegionHandle*frgn=& _tmp7DC;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp7DD=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp7DE=f->args;for(0;_tmp7DE != 0;_tmp7DE=_tmp7DE->tl){
struct _tuple1*_tmp7DF=({struct _tuple1*_tmp7E0=_cycalloc(sizeof(*_tmp7E0));_tmp7E0->f1=Cyc_Absyn_Loc_n;_tmp7E0->f2=(*((struct _tuple9*)_tmp7DE->hd)).f1;_tmp7E0;});
(*((struct _tuple9*)_tmp7DE->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp7DE->hd)).f3);}}
# 4594
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4598
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp7E1=_cycalloc(sizeof(*_tmp7E1));_tmp7E1[0]=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);_tmp7E1;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp7E2=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp7E3=_tmp7E2;struct _dyneither_ptr*_tmp7EC;struct Cyc_Absyn_Tqual _tmp7EB;void*_tmp7EA;int _tmp7E9;_LL4C5: _tmp7EC=_tmp7E3.name;_tmp7EB=_tmp7E3.tq;_tmp7EA=_tmp7E3.type;_tmp7E9=_tmp7E3.inject;_LL4C6:;{
void*_tmp7E4=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp7EA,(void*)& Cyc_Absyn_HeapRgn_val,_tmp7EB,Cyc_Absyn_false_conref));
struct _tuple1*_tmp7E5=({struct _tuple1*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));_tmp7E8->f1=Cyc_Absyn_Loc_n;_tmp7E8->f2=(struct _dyneither_ptr*)_check_null(_tmp7EC);_tmp7E8;});
f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6->hd=({struct _tuple9*_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7->f1=_tmp7EC;_tmp7E7->f2=_tmp7EB;_tmp7E7->f3=_tmp7E4;_tmp7E7;});_tmp7E6->tl=0;_tmp7E6;}));
f->cyc_varargs=0;};}
# 4607
{struct Cyc_List_List*_tmp7ED=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp7ED != 0;_tmp7ED=_tmp7ED->tl){
((struct Cyc_Absyn_Vardecl*)_tmp7ED->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp7ED->hd)->type);}}
# 4610
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp7DD),f->body);}
# 4587
;_pop_region(frgn);};}
# 4614
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp7EE=s;switch(_tmp7EE){case Cyc_Absyn_Abstract: _LL4C8: _LL4C9:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL4CA: _LL4CB:
 return Cyc_Absyn_Extern;default: _LL4CC: _LL4CD:
 return s;}_LL4C7:;}struct _tuple35{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple36{struct Cyc_Toc_TocState*f1;struct _tuple35*f2;};
# 4631 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple36*env){
# 4634
struct _tuple36 _tmp7EF=*env;struct _tuple36 _tmp7F0=_tmp7EF;struct Cyc_Toc_TocState*_tmp82F;struct Cyc_Absyn_Aggrdecl*_tmp82E;int _tmp82D;_LL4CF: _tmp82F=_tmp7F0.f1;_tmp82E=(_tmp7F0.f2)->f1;_tmp82D=(_tmp7F0.f2)->f2;_LL4D0:;{
struct _tuple1*_tmp7F1=_tmp82E->name;
struct Cyc_Toc_TocState _tmp7F2=*_tmp82F;struct Cyc_Toc_TocState _tmp7F3=_tmp7F2;struct Cyc_Dict_Dict*_tmp82C;_LL4D2: _tmp82C=_tmp7F3.aggrs_so_far;_LL4D3:;{
int seen_defn_before;
struct _tuple18**_tmp7F4=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp82C,_tmp7F1);
if(_tmp7F4 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp82E->kind == Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp7F5=_region_malloc(d,sizeof(*_tmp7F5));_tmp7F5->f1=_tmp82E;_tmp7F5->f2=Cyc_Absyn_strctq(_tmp7F1);_tmp7F5;});else{
# 4645
v=({struct _tuple18*_tmp7F6=_region_malloc(d,sizeof(*_tmp7F6));_tmp7F6->f1=_tmp82E;_tmp7F6->f2=Cyc_Absyn_unionq_typ(_tmp7F1);_tmp7F6;});}
*_tmp82C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp82C,_tmp7F1,v);};}else{
# 4648
struct _tuple18*_tmp7F7=*_tmp7F4;struct _tuple18*_tmp7F8=_tmp7F7;struct Cyc_Absyn_Aggrdecl*_tmp7FB;void*_tmp7FA;_LL4D5: _tmp7FB=_tmp7F8->f1;_tmp7FA=_tmp7F8->f2;_LL4D6:;
if(_tmp7FB->impl == 0){
*_tmp82C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp82C,_tmp7F1,({struct _tuple18*_tmp7F9=_region_malloc(d,sizeof(*_tmp7F9));_tmp7F9->f1=_tmp82E;_tmp7F9->f2=_tmp7FA;_tmp7F9;}));
seen_defn_before=0;}else{
# 4653
seen_defn_before=1;}}{
# 4655
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B->kind=_tmp82E->kind;_tmp82B->sc=Cyc_Absyn_Public;_tmp82B->name=_tmp82E->name;_tmp82B->tvs=0;_tmp82B->impl=0;_tmp82B->expected_mem_kind=0;_tmp82B->attributes=_tmp82E->attributes;_tmp82B;});
# 4662
if(_tmp82E->impl != 0  && !seen_defn_before){
new_ad->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC->exist_vars=0;_tmp7FC->rgn_po=0;_tmp7FC->fields=0;_tmp7FC->tagged=0;_tmp7FC;});{
# 4667
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp7FD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82E->impl))->fields;for(0;_tmp7FD != 0;_tmp7FD=_tmp7FD->tl){
# 4671
struct Cyc_Absyn_Aggrfield*_tmp7FE=(struct Cyc_Absyn_Aggrfield*)_tmp7FD->hd;
void*_tmp7FF=_tmp7FE->type;
struct Cyc_List_List*_tmp800=_tmp7FE->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp7FF)) && (
_tmp82E->kind == Cyc_Absyn_StructA  && _tmp7FD->tl == 0  || _tmp82E->kind == Cyc_Absyn_UnionA)){
# 4685 "toc.cyc"
void*_tmp801=Cyc_Tcutil_compress(_tmp7FF);void*_tmp802=_tmp801;void*_tmp80F;struct Cyc_Absyn_Tqual _tmp80E;union Cyc_Absyn_Constraint*_tmp80D;unsigned int _tmp80C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp802)->tag == 8){_LL4D8: _tmp80F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp802)->f1).elt_type;_tmp80E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp802)->f1).tq;_tmp80D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp802)->f1).zero_term;_tmp80C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp802)->f1).zt_loc;_LL4D9:
# 4688
 _tmp7FF=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp803=_cycalloc(sizeof(*_tmp803));_tmp803[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp804;_tmp804.tag=8;_tmp804.f1=({struct Cyc_Absyn_ArrayInfo _tmp805;_tmp805.elt_type=_tmp80F;_tmp805.tq=_tmp80E;_tmp805.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp805.zero_term=_tmp80D;_tmp805.zt_loc=_tmp80C;_tmp805;});_tmp804;});_tmp803;});
goto _LL4D7;}else{_LL4DA: _LL4DB:
# 4691
 _tmp800=({struct Cyc_List_List*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp808;_tmp808.tag=6;_tmp808.f1=0;_tmp808;});_tmp807;});_tmp806->tl=_tmp800;_tmp806;});
_tmp7FF=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp80A;_tmp80A.tag=8;_tmp80A.f1=({struct Cyc_Absyn_ArrayInfo _tmp80B;_tmp80B.elt_type=Cyc_Absyn_void_star_typ();_tmp80B.tq=
Cyc_Absyn_empty_tqual(0);_tmp80B.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp80B.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp80B.zt_loc=0;_tmp80B;});_tmp80A;});_tmp809;});}_LL4D7:;}{
# 4698
struct Cyc_Absyn_Aggrfield*_tmp810=({struct Cyc_Absyn_Aggrfield*_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826->name=_tmp7FE->name;_tmp826->tq=Cyc_Toc_mt_tq;_tmp826->type=
# 4700
Cyc_Toc_typ_to_c(_tmp7FF);_tmp826->width=_tmp7FE->width;_tmp826->attributes=_tmp800;_tmp826->requires_clause=0;_tmp826;});
# 4708
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82E->impl))->tagged){
void*_tmp811=_tmp810->type;
struct _dyneither_ptr*_tmp812=_tmp810->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp824;_tmp824.tag=0;_tmp824.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp812);({struct Cyc_String_pa_PrintArg_struct _tmp823;_tmp823.tag=0;_tmp823.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp82E->name).f2);({void*_tmp821[2]={& _tmp823,& _tmp824};Cyc_aprintf(({const char*_tmp822="_union_%s_%s";_tag_dyneither(_tmp822,sizeof(char),13);}),_tag_dyneither(_tmp821,sizeof(void*),2));});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820[0]=s;_tmp820;});
struct Cyc_Absyn_Aggrfield*_tmp813=({struct Cyc_Absyn_Aggrfield*_tmp81F=_cycalloc(sizeof(*_tmp81F));_tmp81F->name=Cyc_Toc_val_sp;_tmp81F->tq=Cyc_Toc_mt_tq;_tmp81F->type=_tmp811;_tmp81F->width=0;_tmp81F->attributes=0;_tmp81F->requires_clause=0;_tmp81F;});
struct Cyc_Absyn_Aggrfield*_tmp814=({struct Cyc_Absyn_Aggrfield*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E->name=Cyc_Toc_tag_sp;_tmp81E->tq=Cyc_Toc_mt_tq;_tmp81E->type=Cyc_Absyn_sint_typ;_tmp81E->width=0;_tmp81E->attributes=0;_tmp81E->requires_clause=0;_tmp81E;});
struct Cyc_List_List*_tmp815=({struct Cyc_Absyn_Aggrfield*_tmp81D[2];_tmp81D[1]=_tmp813;_tmp81D[0]=_tmp814;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp81D,sizeof(struct Cyc_Absyn_Aggrfield*),2));});
struct Cyc_Absyn_Aggrdecl*_tmp816=({struct Cyc_Absyn_Aggrdecl*_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A->kind=Cyc_Absyn_StructA;_tmp81A->sc=Cyc_Absyn_Public;_tmp81A->name=({struct _tuple1*_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C->f1=Cyc_Absyn_Loc_n;_tmp81C->f2=str;_tmp81C;});_tmp81A->tvs=0;_tmp81A->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B->exist_vars=0;_tmp81B->rgn_po=0;_tmp81B->fields=_tmp815;_tmp81B->tagged=0;_tmp81B;});_tmp81A->expected_mem_kind=0;_tmp81A->attributes=0;_tmp81A;});
# 4721
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp817=_cycalloc(sizeof(*_tmp817));_tmp817->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp819;_tmp819.tag=5;_tmp819.f1=_tmp816;_tmp819;});_tmp818;}),0);_tmp817->tl=Cyc_Toc_result_decls;_tmp817;});
_tmp810->type=Cyc_Absyn_strct(str);}
# 4724
new_fields=({struct Cyc_List_List*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825->hd=_tmp810;_tmp825->tl=new_fields;_tmp825;});};}}
# 4726
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4728
if(_tmp82D)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp827=_cycalloc(sizeof(*_tmp827));_tmp827->hd=({struct Cyc_Absyn_Decl*_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp829=_cycalloc(sizeof(*_tmp829));_tmp829[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp82A;_tmp82A.tag=5;_tmp82A.f1=new_ad;_tmp82A;});_tmp829;});_tmp828->loc=0;_tmp828;});_tmp827->tl=Cyc_Toc_result_decls;_tmp827;});
return 0;};};};}
# 4733
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple35 p=({struct _tuple35 _tmp830;_tmp830.f1=ad;_tmp830.f2=add_to_decls;_tmp830;});
((int(*)(struct _tuple35*arg,int(*f)(struct _RegionHandle*,struct _tuple36*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple37{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4762 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4765
struct _tuple37 _tmp831=*env;struct _tuple37 _tmp832=_tmp831;struct Cyc_Set_Set**_tmp844;struct Cyc_Absyn_Datatypedecl*_tmp843;_LL4DD: _tmp844=(_tmp832.f1)->datatypes_so_far;_tmp843=_tmp832.f2;_LL4DE:;{
struct _tuple1*_tmp833=_tmp843->name;
if(_tmp843->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp844,_tmp833))
return 0;
*_tmp844=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp844,_tmp833);
{struct Cyc_List_List*_tmp834=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp843->fields))->v;for(0;_tmp834 != 0;_tmp834=_tmp834->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp834->hd;
# 4773
struct Cyc_List_List*_tmp835=0;
int i=1;
{struct Cyc_List_List*_tmp836=f->typs;for(0;_tmp836 != 0;(_tmp836=_tmp836->tl,i ++)){
struct _dyneither_ptr*_tmp837=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp838=({struct Cyc_Absyn_Aggrfield*_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A->name=_tmp837;_tmp83A->tq=(*((struct _tuple11*)_tmp836->hd)).f1;_tmp83A->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp836->hd)).f2);_tmp83A->width=0;_tmp83A->attributes=0;_tmp83A->requires_clause=0;_tmp83A;});
_tmp835=({struct Cyc_List_List*_tmp839=_cycalloc(sizeof(*_tmp839));_tmp839->hd=_tmp838;_tmp839->tl=_tmp835;_tmp839;});}}
# 4781
_tmp835=({struct Cyc_List_List*_tmp83B=_cycalloc(sizeof(*_tmp83B));_tmp83B->hd=({struct Cyc_Absyn_Aggrfield*_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C->name=Cyc_Toc_tag_sp;_tmp83C->tq=Cyc_Toc_mt_tq;_tmp83C->type=Cyc_Absyn_sint_typ;_tmp83C->width=0;_tmp83C->attributes=0;_tmp83C->requires_clause=0;_tmp83C;});_tmp83B->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp835);_tmp83B;});{
struct Cyc_Absyn_Aggrdecl*_tmp83D=({struct Cyc_Absyn_Aggrdecl*_tmp841=_cycalloc(sizeof(*_tmp841));_tmp841->kind=Cyc_Absyn_StructA;_tmp841->sc=Cyc_Absyn_Public;_tmp841->name=
# 4785
Cyc_Toc_collapse_qvars(f->name,_tmp843->name);_tmp841->tvs=0;_tmp841->expected_mem_kind=0;_tmp841->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842->exist_vars=0;_tmp842->rgn_po=0;_tmp842->fields=_tmp835;_tmp842->tagged=0;_tmp842;});_tmp841->attributes=0;_tmp841;});
# 4789
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp83F=_cycalloc(sizeof(*_tmp83F));_tmp83F[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp840;_tmp840.tag=5;_tmp840.f1=_tmp83D;_tmp840;});_tmp83F;}),0);_tmp83E->tl=Cyc_Toc_result_decls;_tmp83E;});};}}
# 4791
return 0;};}
# 4794
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4813 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4816
struct _tuple37 _tmp845=*env;struct _tuple37 _tmp846=_tmp845;struct Cyc_Toc_TocState*_tmp86B;struct Cyc_Absyn_Datatypedecl*_tmp86A;_LL4E0: _tmp86B=_tmp846.f1;_tmp86A=_tmp846.f2;_LL4E1:;
if(_tmp86A->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp847=*_tmp86B;struct Cyc_Toc_TocState _tmp848=_tmp847;struct Cyc_Dict_Dict*_tmp869;_LL4E3: _tmp869=_tmp848.xdatatypes_so_far;_LL4E4:;{
struct _tuple1*_tmp849=_tmp86A->name;
{struct Cyc_List_List*_tmp84A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp86A->fields))->v;for(0;_tmp84A != 0;_tmp84A=_tmp84A->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp84A->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp84B=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp84C=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp84B,Cyc_Absyn_false_conref,0);
# 4827
int*_tmp84D=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp869,f->name);int*_tmp84E=_tmp84D;if(_tmp84E == 0){_LL4E6: _LL4E7: {
# 4829
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4833
struct Cyc_Absyn_Vardecl*_tmp84F=Cyc_Absyn_new_vardecl(0,f->name,_tmp84C,initopt);
_tmp84F->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp850=_cycalloc(sizeof(*_tmp850));_tmp850->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp851=_cycalloc(sizeof(*_tmp851));_tmp851[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp852;_tmp852.tag=0;_tmp852.f1=_tmp84F;_tmp852;});_tmp851;}),0);_tmp850->tl=Cyc_Toc_result_decls;_tmp850;});
*_tmp869=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp869,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp853=f->typs;for(0;_tmp853 != 0;(_tmp853=_tmp853->tl,i ++)){
struct _dyneither_ptr*_tmp854=({struct _dyneither_ptr*_tmp858=_cycalloc(sizeof(*_tmp858));_tmp858[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp85B;_tmp85B.tag=1;_tmp85B.f1=(unsigned long)i;({void*_tmp859[1]={& _tmp85B};Cyc_aprintf(({const char*_tmp85A="f%d";_tag_dyneither(_tmp85A,sizeof(char),4);}),_tag_dyneither(_tmp859,sizeof(void*),1));});});_tmp858;});
struct Cyc_Absyn_Aggrfield*_tmp855=({struct Cyc_Absyn_Aggrfield*_tmp857=_cycalloc(sizeof(*_tmp857));_tmp857->name=_tmp854;_tmp857->tq=(*((struct _tuple11*)_tmp853->hd)).f1;_tmp857->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp853->hd)).f2);_tmp857->width=0;_tmp857->attributes=0;_tmp857->requires_clause=0;_tmp857;});
fields=({struct Cyc_List_List*_tmp856=_cycalloc(sizeof(*_tmp856));_tmp856->hd=_tmp855;_tmp856->tl=fields;_tmp856;});}}
# 4846
fields=({struct Cyc_List_List*_tmp85C=_cycalloc(sizeof(*_tmp85C));_tmp85C->hd=({struct Cyc_Absyn_Aggrfield*_tmp85D=_cycalloc(sizeof(*_tmp85D));_tmp85D->name=Cyc_Toc_tag_sp;_tmp85D->tq=Cyc_Toc_mt_tq;_tmp85D->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp85D->width=0;_tmp85D->attributes=0;_tmp85D->requires_clause=0;_tmp85D;});_tmp85C->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp85C;});{
struct Cyc_Absyn_Aggrdecl*_tmp85E=({struct Cyc_Absyn_Aggrdecl*_tmp862=_cycalloc(sizeof(*_tmp862));_tmp862->kind=Cyc_Absyn_StructA;_tmp862->sc=Cyc_Absyn_Public;_tmp862->name=
# 4851
Cyc_Toc_collapse_qvars(f->name,_tmp86A->name);_tmp862->tvs=0;_tmp862->expected_mem_kind=0;_tmp862->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp863=_cycalloc(sizeof(*_tmp863));_tmp863->exist_vars=0;_tmp863->rgn_po=0;_tmp863->fields=fields;_tmp863->tagged=0;_tmp863;});_tmp862->attributes=0;_tmp862;});
# 4855
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp85F=_cycalloc(sizeof(*_tmp85F));_tmp85F->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp860=_cycalloc(sizeof(*_tmp860));_tmp860[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp861;_tmp861.tag=5;_tmp861.f1=_tmp85E;_tmp861;});_tmp860;}),0);_tmp85F->tl=Cyc_Toc_result_decls;_tmp85F;});
# 4857
goto _LL4E5;};};};}}else{if(*((int*)_tmp84E)== 0){_LL4E8: _LL4E9:
# 4859
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp864=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp865=Cyc_Absyn_new_vardecl(0,f->name,_tmp84C,_tmp864);
_tmp865->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp866=_cycalloc(sizeof(*_tmp866));_tmp866->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp867=_cycalloc(sizeof(*_tmp867));_tmp867[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp868;_tmp868.tag=0;_tmp868.f1=_tmp865;_tmp868;});_tmp867;}),0);_tmp866->tl=Cyc_Toc_result_decls;_tmp866;});
*_tmp869=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp869,f->name,1);}
# 4866
goto _LL4E5;}else{_LL4EA: _LL4EB:
 goto _LL4E5;}}_LL4E5:;}}
# 4870
return 0;};};}
# 4873
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4877
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4883
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4887
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp86C=init->r;void*_tmp86D=_tmp86C;struct Cyc_Absyn_Vardecl*_tmp873;struct Cyc_Absyn_Exp*_tmp872;struct Cyc_Absyn_Exp*_tmp871;int _tmp870;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp86D)->tag == 26){_LL4ED: _tmp873=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp86D)->f1;_tmp872=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp86D)->f2;_tmp871=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp86D)->f3;_tmp870=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp86D)->f4;_LL4EE:
# 4894
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp873,_tmp872,_tmp871,_tmp870,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL4EC;}else{_LL4EF: _LL4F0:
# 4899
 if(vd->sc == Cyc_Absyn_Static){
# 4903
struct _RegionHandle _tmp86E=_new_region("temp");struct _RegionHandle*temp=& _tmp86E;_push_region(temp);
{struct Cyc_Toc_Env*_tmp86F=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp86F,init);}
# 4904
;_pop_region(temp);}else{
# 4908
Cyc_Toc_exp_to_c(nv,init);}
goto _LL4EC;}_LL4EC:;}else{
# 4913
void*_tmp874=Cyc_Tcutil_compress(old_typ);void*_tmp875=_tmp874;void*_tmp87C;struct Cyc_Absyn_Exp*_tmp87B;union Cyc_Absyn_Constraint*_tmp87A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp875)->tag == 8){_LL4F2: _tmp87C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp875)->f1).elt_type;_tmp87B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp875)->f1).num_elts;_tmp87A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp875)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp87A)){_LL4F3:
 if(_tmp87B == 0)
({void*_tmp876=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp877="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp877,sizeof(char),55);}),_tag_dyneither(_tmp876,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*num_elts=_tmp87B;
struct Cyc_Absyn_Exp*_tmp878=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4921
struct Cyc_Absyn_Exp*_tmp879=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp878,_tmp879,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL4F1;};}else{goto _LL4F4;}}else{_LL4F4: _LL4F5:
 goto _LL4F1;}_LL4F1:;}}
# 4932
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp87D=d;struct Cyc_List_List*_tmp884;struct Cyc_List_List*_tmp883;void**_tmp882;struct Cyc_Tcpat_Rhs*_tmp881;switch(*((int*)_tmp87D)){case 0: _LL4F7: _LL4F8:
 return d;case 1: _LL4F9: _tmp881=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp87D)->f1;_LL4FA:
 _tmp881->rhs=success;return d;default: _LL4FB: _tmp884=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp87D)->f1;_tmp883=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp87D)->f2;_tmp882=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp87D)->f3;_LL4FC:
# 4937
*_tmp882=Cyc_Toc_rewrite_decision(*_tmp882,success);
for(0;_tmp883 != 0;_tmp883=_tmp883->tl){
struct _tuple29*_tmp87E=(struct _tuple29*)_tmp883->hd;struct _tuple29*_tmp87F=_tmp87E;void**_tmp880;_LL4FE: _tmp880=(void**)& _tmp87F->f2;_LL4FF:;
*_tmp880=Cyc_Toc_rewrite_decision(*_tmp880,success);}
# 4942
return d;}_LL4F6:;}
# 4953 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4955
struct _RegionHandle _tmp885=_new_region("rgn");struct _RegionHandle*rgn=& _tmp885;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp886=Cyc_Toc_share_env(rgn,nv);
void*_tmp887=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp886,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4965
struct Cyc_Absyn_Stmt*_tmp888=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4969
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp888);{
# 4971
struct Cyc_Absyn_Switch_clause*_tmp889=({struct Cyc_Absyn_Switch_clause*_tmp896=_cycalloc(sizeof(*_tmp896));_tmp896->pattern=p;_tmp896->pat_vars=0;_tmp896->where_clause=0;_tmp896->body=_tmp888;_tmp896->loc=0;_tmp896;});
struct Cyc_List_List*_tmp88A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,({struct Cyc_Absyn_Switch_clause*_tmp895[1];_tmp895[0]=_tmp889;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(rgn,_tag_dyneither(_tmp895,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));
# 4974
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4978
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp886,& mydecls,& mybodies,dopt,_tmp88A,v);
# 4981
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple34*_tmp88B=(struct _tuple34*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple34*_tmp88C=_tmp88B;struct Cyc_Toc_Env*_tmp88E;struct Cyc_Absyn_Stmt*_tmp88D;_LL501: _tmp88E=_tmp88C->f1;_tmp88D=_tmp88C->f3;_LL502:;
if(_tmp88D == _tmp888){senv=_tmp88E;goto FOUND_ENV;}}
# 4986
({void*_tmp88F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp890="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp890,sizeof(char),33);}),_tag_dyneither(_tmp88F,sizeof(void*),0));});
FOUND_ENV:
# 4990
 Cyc_Toc_stmt_to_c(senv,s);{
# 4992
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0);
# 4994
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp891=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
_tmp891->name=Cyc_Toc_temp_var();
_tmp891->type=Cyc_Toc_typ_to_c_array(_tmp891->type);
res=Cyc_Absyn_decl_stmt(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp892=_cycalloc(sizeof(*_tmp892));_tmp892[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp893;_tmp893.tag=0;_tmp893.f1=_tmp891;_tmp893;});_tmp892;}),0),res,0);}
# 5001
res=Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c(_tmp887),e,res,0);{
struct Cyc_Absyn_Stmt*_tmp894=res;_npop_handler(0);return _tmp894;};};};};}
# 4956
;_pop_region(rgn);}
# 5009
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp897=e->r;void*_tmp898=_tmp897;struct Cyc_Absyn_MallocInfo*_tmp8C2;struct Cyc_Absyn_Stmt*_tmp8C1;void**_tmp8C0;void**_tmp8BF;struct Cyc_List_List*_tmp8BE;struct Cyc_List_List*_tmp8BD;void**_tmp8BC;struct Cyc_List_List*_tmp8BB;void**_tmp8BA;struct Cyc_Absyn_Exp*_tmp8B9;struct Cyc_Absyn_Exp*_tmp8B8;struct Cyc_List_List*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_Absyn_Exp*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B4;struct Cyc_Absyn_Exp*_tmp8B3;struct Cyc_Absyn_Exp*_tmp8B2;struct Cyc_Absyn_Exp*_tmp8B1;struct Cyc_Absyn_Exp*_tmp8B0;struct Cyc_Absyn_Exp*_tmp8AF;struct Cyc_Absyn_Exp*_tmp8AE;struct Cyc_Absyn_Exp*_tmp8AD;struct Cyc_Absyn_Exp*_tmp8AC;struct Cyc_Absyn_Exp*_tmp8AB;struct Cyc_Absyn_Exp*_tmp8AA;struct Cyc_Absyn_Exp*_tmp8A9;struct Cyc_Absyn_Exp*_tmp8A8;struct Cyc_List_List*_tmp8A7;struct Cyc_Absyn_Exp*_tmp8A6;struct Cyc_Absyn_Exp*_tmp8A5;struct Cyc_Absyn_Exp*_tmp8A4;struct Cyc_Absyn_Exp*_tmp8A3;struct Cyc_Absyn_Exp*_tmp8A2;struct Cyc_Absyn_Exp*_tmp8A1;struct Cyc_Absyn_Exp*_tmp8A0;struct Cyc_Absyn_Exp*_tmp89F;switch(*((int*)_tmp898)){case 19: _LL504: _tmp89F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL505:
 _tmp8A0=_tmp89F;goto _LL507;case 20: _LL506: _tmp8A0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL507:
 _tmp8A1=_tmp8A0;goto _LL509;case 21: _LL508: _tmp8A1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL509:
 _tmp8A2=_tmp8A1;goto _LL50B;case 14: _LL50A: _tmp8A2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL50B:
 _tmp8A3=_tmp8A2;goto _LL50D;case 10: _LL50C: _tmp8A3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL50D:
 _tmp8A4=_tmp8A3;goto _LL50F;case 11: _LL50E: _tmp8A4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL50F:
 _tmp8A5=_tmp8A4;goto _LL511;case 17: _LL510: _tmp8A5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL511:
 _tmp8A6=_tmp8A5;goto _LL513;case 4: _LL512: _tmp8A6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL513:
 Cyc_Toc_exptypes_to_c(_tmp8A6);goto _LL503;case 2: _LL514: _tmp8A7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_LL515:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8A7);goto _LL503;case 6: _LL516: _tmp8A9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_tmp8A8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_LL517:
 _tmp8AB=_tmp8A9;_tmp8AA=_tmp8A8;goto _LL519;case 7: _LL518: _tmp8AB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_tmp8AA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_LL519:
 _tmp8AD=_tmp8AB;_tmp8AC=_tmp8AA;goto _LL51B;case 8: _LL51A: _tmp8AD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_tmp8AC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_LL51B:
 _tmp8AF=_tmp8AD;_tmp8AE=_tmp8AC;goto _LL51D;case 22: _LL51C: _tmp8AF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_tmp8AE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_LL51D:
 _tmp8B1=_tmp8AF;_tmp8B0=_tmp8AE;goto _LL51F;case 34: _LL51E: _tmp8B1=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_tmp8B0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_LL51F:
 _tmp8B3=_tmp8B1;_tmp8B2=_tmp8B0;goto _LL521;case 3: _LL520: _tmp8B3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_tmp8B2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp898)->f3;_LL521:
 Cyc_Toc_exptypes_to_c(_tmp8B3);Cyc_Toc_exptypes_to_c(_tmp8B2);goto _LL503;case 5: _LL522: _tmp8B6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_tmp8B5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_tmp8B4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp898)->f3;_LL523:
# 5027
 Cyc_Toc_exptypes_to_c(_tmp8B6);Cyc_Toc_exptypes_to_c(_tmp8B5);Cyc_Toc_exptypes_to_c(_tmp8B4);goto _LL503;case 9: _LL524: _tmp8B8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_tmp8B7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_LL525:
# 5029
 Cyc_Toc_exptypes_to_c(_tmp8B8);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8B7);goto _LL503;case 13: _LL526: _tmp8BA=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_tmp8B9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_LL527:
*_tmp8BA=Cyc_Toc_typ_to_c(*_tmp8BA);Cyc_Toc_exptypes_to_c(_tmp8B9);goto _LL503;case 24: _LL528: _tmp8BC=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp898)->f1)->f3;_tmp8BB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_LL529:
# 5032
*_tmp8BC=Cyc_Toc_typ_to_c(*_tmp8BC);
_tmp8BD=_tmp8BB;goto _LL52B;case 35: _LL52A: _tmp8BD=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp898)->f2;_LL52B:
 _tmp8BE=_tmp8BD;goto _LL52D;case 25: _LL52C: _tmp8BE=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL52D:
# 5036
 for(0;_tmp8BE != 0;_tmp8BE=_tmp8BE->tl){
struct _tuple19 _tmp899=*((struct _tuple19*)_tmp8BE->hd);struct _tuple19 _tmp89A=_tmp899;struct Cyc_Absyn_Exp*_tmp89B;_LL555: _tmp89B=_tmp89A.f2;_LL556:;
Cyc_Toc_exptypes_to_c(_tmp89B);}
# 5040
goto _LL503;case 18: _LL52E: _tmp8BF=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL52F:
 _tmp8C0=_tmp8BF;goto _LL531;case 16: _LL530: _tmp8C0=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL531:
*_tmp8C0=Cyc_Toc_typ_to_c(*_tmp8C0);goto _LL503;case 36: _LL532: _tmp8C1=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL533:
 Cyc_Toc_stmttypes_to_c(_tmp8C1);goto _LL503;case 33: _LL534: _tmp8C2=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp898)->f1;_LL535:
# 5045
 if(_tmp8C2->elt_type != 0)
_tmp8C2->elt_type=({void**_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8C2->elt_type)));_tmp89C;});
Cyc_Toc_exptypes_to_c(_tmp8C2->num_elts);
goto _LL503;case 0: _LL536: _LL537:
 goto _LL539;case 1: _LL538: _LL539:
 goto _LL53B;case 31: _LL53A: _LL53B:
 goto _LL53D;case 39: _LL53C: _LL53D:
 goto _LL53F;case 32: _LL53E: _LL53F:
 goto _LL503;case 29: _LL540: _LL541:
# 5055
 goto _LL543;case 30: _LL542: _LL543:
 goto _LL545;case 28: _LL544: _LL545:
 goto _LL547;case 26: _LL546: _LL547:
 goto _LL549;case 27: _LL548: _LL549:
 goto _LL54B;case 23: _LL54A: _LL54B:
 goto _LL54D;case 12: _LL54C: _LL54D:
 goto _LL54F;case 15: _LL54E: _LL54F:
 goto _LL551;case 38: _LL550: _LL551:
 goto _LL553;default: _LL552: _LL553:
# 5065
({void*_tmp89D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp89E="Cyclone expression within C code";_tag_dyneither(_tmp89E,sizeof(char),33);}),_tag_dyneither(_tmp89D,sizeof(void*),0));});
goto _LL503;}_LL503:;}
# 5070
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp8C3=d->r;void*_tmp8C4=_tmp8C3;struct Cyc_Absyn_Typedefdecl*_tmp8CE;struct Cyc_Absyn_Enumdecl*_tmp8CD;struct Cyc_Absyn_Aggrdecl*_tmp8CC;struct Cyc_Absyn_Fndecl*_tmp8CB;struct Cyc_Absyn_Vardecl*_tmp8CA;switch(*((int*)_tmp8C4)){case 0: _LL558: _tmp8CA=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8C4)->f1;_LL559:
# 5073
 _tmp8CA->type=Cyc_Toc_typ_to_c(_tmp8CA->type);
if(_tmp8CA->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8CA->initializer));
goto _LL557;case 1: _LL55A: _tmp8CB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8C4)->f1;_LL55B:
# 5077
 _tmp8CB->ret_type=Cyc_Toc_typ_to_c(_tmp8CB->ret_type);
{struct Cyc_List_List*_tmp8C5=_tmp8CB->args;for(0;_tmp8C5 != 0;_tmp8C5=_tmp8C5->tl){
(*((struct _tuple9*)_tmp8C5->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8C5->hd)).f3);}}
# 5081
goto _LL557;case 5: _LL55C: _tmp8CC=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8C4)->f1;_LL55D:
 Cyc_Toc_aggrdecl_to_c(_tmp8CC,1);goto _LL557;case 7: _LL55E: _tmp8CD=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8C4)->f1;_LL55F:
# 5084
 if(_tmp8CD->fields != 0){
struct Cyc_List_List*_tmp8C6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8CD->fields))->v;for(0;_tmp8C6 != 0;_tmp8C6=_tmp8C6->tl){
struct Cyc_Absyn_Enumfield*_tmp8C7=(struct Cyc_Absyn_Enumfield*)_tmp8C6->hd;
if(_tmp8C7->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8C7->tag));}}
# 5089
goto _LL557;case 8: _LL560: _tmp8CE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8C4)->f1;_LL561:
 _tmp8CE->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8CE->defn));goto _LL557;case 2: _LL562: _LL563:
 goto _LL565;case 3: _LL564: _LL565:
 goto _LL567;case 6: _LL566: _LL567:
 goto _LL569;case 9: _LL568: _LL569:
 goto _LL56B;case 10: _LL56A: _LL56B:
 goto _LL56D;case 11: _LL56C: _LL56D:
 goto _LL56F;case 12: _LL56E: _LL56F:
 goto _LL571;case 4: _LL570: _LL571:
# 5099
({void*_tmp8C8=0;Cyc_Tcutil_terr(d->loc,({const char*_tmp8C9="Cyclone declaration within C code";_tag_dyneither(_tmp8C9,sizeof(char),34);}),_tag_dyneither(_tmp8C8,sizeof(void*),0));});
goto _LL557;case 13: _LL572: _LL573:
 goto _LL575;default: _LL574: _LL575:
# 5103
 goto _LL557;}_LL557:;}
# 5107
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp8CF=s->r;void*_tmp8D0=_tmp8CF;struct Cyc_Absyn_Stmt*_tmp8EB;struct Cyc_Absyn_Stmt*_tmp8EA;struct Cyc_Absyn_Exp*_tmp8E9;struct Cyc_Absyn_Decl*_tmp8E8;struct Cyc_Absyn_Stmt*_tmp8E7;struct Cyc_Absyn_Exp*_tmp8E6;struct Cyc_List_List*_tmp8E5;void*_tmp8E4;struct Cyc_Absyn_Exp*_tmp8E3;struct Cyc_Absyn_Exp*_tmp8E2;struct Cyc_Absyn_Exp*_tmp8E1;struct Cyc_Absyn_Stmt*_tmp8E0;struct Cyc_Absyn_Exp*_tmp8DF;struct Cyc_Absyn_Stmt*_tmp8DE;struct Cyc_Absyn_Exp*_tmp8DD;struct Cyc_Absyn_Stmt*_tmp8DC;struct Cyc_Absyn_Stmt*_tmp8DB;struct Cyc_Absyn_Exp*_tmp8DA;struct Cyc_Absyn_Stmt*_tmp8D9;struct Cyc_Absyn_Stmt*_tmp8D8;struct Cyc_Absyn_Exp*_tmp8D7;switch(*((int*)_tmp8D0)){case 1: _LL577: _tmp8D7=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f1;_LL578:
 Cyc_Toc_exptypes_to_c(_tmp8D7);goto _LL576;case 2: _LL579: _tmp8D9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f1;_tmp8D8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f2;_LL57A:
 Cyc_Toc_stmttypes_to_c(_tmp8D9);Cyc_Toc_stmttypes_to_c(_tmp8D8);goto _LL576;case 3: _LL57B: _tmp8DA=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f1;_LL57C:
 if(_tmp8DA != 0)Cyc_Toc_exptypes_to_c(_tmp8DA);goto _LL576;case 4: _LL57D: _tmp8DD=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f1;_tmp8DC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f2;_tmp8DB=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f3;_LL57E:
# 5113
 Cyc_Toc_exptypes_to_c(_tmp8DD);Cyc_Toc_stmttypes_to_c(_tmp8DC);Cyc_Toc_stmttypes_to_c(_tmp8DB);goto _LL576;case 5: _LL57F: _tmp8DF=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f1).f1;_tmp8DE=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f2;_LL580:
# 5115
 Cyc_Toc_exptypes_to_c(_tmp8DF);Cyc_Toc_stmttypes_to_c(_tmp8DE);goto _LL576;case 9: _LL581: _tmp8E3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f1;_tmp8E2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f2).f1;_tmp8E1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f3).f1;_tmp8E0=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f4;_LL582:
# 5117
 Cyc_Toc_exptypes_to_c(_tmp8E3);Cyc_Toc_exptypes_to_c(_tmp8E2);Cyc_Toc_exptypes_to_c(_tmp8E1);
Cyc_Toc_stmttypes_to_c(_tmp8E0);goto _LL576;case 10: _LL583: _tmp8E6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f1;_tmp8E5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f2;_tmp8E4=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f3;_LL584:
# 5120
 Cyc_Toc_exptypes_to_c(_tmp8E6);
for(0;_tmp8E5 != 0;_tmp8E5=_tmp8E5->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp8E5->hd)->body);}
goto _LL576;case 12: _LL585: _tmp8E8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f1;_tmp8E7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f2;_LL586:
 Cyc_Toc_decltypes_to_c(_tmp8E8);Cyc_Toc_stmttypes_to_c(_tmp8E7);goto _LL576;case 14: _LL587: _tmp8EA=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f1;_tmp8E9=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f2).f1;_LL588:
 Cyc_Toc_stmttypes_to_c(_tmp8EA);Cyc_Toc_exptypes_to_c(_tmp8E9);goto _LL576;case 13: _LL589: _tmp8EB=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8D0)->f2;_LL58A:
 Cyc_Toc_stmttypes_to_c(_tmp8EB);goto _LL576;case 0: _LL58B: _LL58C:
 goto _LL58E;case 6: _LL58D: _LL58E:
 goto _LL590;case 7: _LL58F: _LL590:
 goto _LL592;case 8: _LL591: _LL592:
# 5130
 goto _LL576;case 11: _LL593: _LL594:
# 5133
 s->r=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp8D1=_cycalloc_atomic(sizeof(*_tmp8D1));_tmp8D1[0]=({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmp8D2;_tmp8D2.tag=0;_tmp8D2;});_tmp8D1;});
goto _LL576;default: _LL595: _LL596:
# 5136
({void*_tmp8D3=0;Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8D6;_tmp8D6.tag=0;_tmp8D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp8D4[1]={& _tmp8D6};Cyc_aprintf(({const char*_tmp8D5="Cyclone statement in C code: %s";_tag_dyneither(_tmp8D5,sizeof(char),32);}),_tag_dyneither(_tmp8D4,sizeof(void*),1));});}),_tag_dyneither(_tmp8D3,sizeof(void*),0));});
goto _LL576;}_LL576:;}
# 5144
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp8EC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp8ED="decls_to_c: not at toplevel!";_tag_dyneither(_tmp8ED,sizeof(char),29);}),_tag_dyneither(_tmp8EC,sizeof(void*),0));});{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp8EE=d->r;void*_tmp8EF=_tmp8EE;struct Cyc_List_List*_tmp907;struct Cyc_List_List*_tmp906;struct Cyc_List_List*_tmp905;struct Cyc_List_List*_tmp904;struct Cyc_Absyn_Typedefdecl*_tmp903;struct Cyc_Absyn_Enumdecl*_tmp902;struct Cyc_Absyn_Datatypedecl*_tmp901;struct Cyc_Absyn_Aggrdecl*_tmp900;struct Cyc_Absyn_Fndecl*_tmp8FF;struct Cyc_Absyn_Vardecl*_tmp8FE;switch(*((int*)_tmp8EF)){case 0: _LL598: _tmp8FE=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8EF)->f1;_LL599: {
# 5151
struct _tuple1*_tmp8F0=_tmp8FE->name;
# 5153
if(_tmp8FE->sc == Cyc_Absyn_ExternC)
_tmp8F0=({struct _tuple1*_tmp8F1=_cycalloc(sizeof(*_tmp8F1));_tmp8F1->f1=Cyc_Absyn_Abs_n(0,1);_tmp8F1->f2=(*_tmp8F0).f2;_tmp8F1;});
if(_tmp8FE->initializer != 0){
if(_tmp8FE->sc == Cyc_Absyn_ExternC)_tmp8FE->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8FE->initializer));else{
# 5160
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp8FE->initializer));}}
# 5162
_tmp8FE->name=_tmp8F0;
_tmp8FE->sc=Cyc_Toc_scope_to_c(_tmp8FE->sc);
_tmp8FE->type=Cyc_Toc_typ_to_c(_tmp8FE->type);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8F2=_cycalloc(sizeof(*_tmp8F2));_tmp8F2->hd=d;_tmp8F2->tl=Cyc_Toc_result_decls;_tmp8F2;});
goto _LL597;}case 1: _LL59A: _tmp8FF=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8EF)->f1;_LL59B: {
# 5168
struct _tuple1*_tmp8F3=_tmp8FF->name;
# 5170
if(_tmp8FF->sc == Cyc_Absyn_ExternC){
_tmp8F3=({struct _tuple1*_tmp8F4=_cycalloc(sizeof(*_tmp8F4));_tmp8F4->f1=Cyc_Absyn_Abs_n(0,1);_tmp8F4->f2=(*_tmp8F3).f2;_tmp8F4;});
_tmp8FF->sc=Cyc_Absyn_Public;}
# 5174
_tmp8FF->name=_tmp8F3;
Cyc_Toc_fndecl_to_c(nv,_tmp8FF,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8F5=_cycalloc(sizeof(*_tmp8F5));_tmp8F5->hd=d;_tmp8F5->tl=Cyc_Toc_result_decls;_tmp8F5;});
goto _LL597;}case 2: _LL59C: _LL59D:
 goto _LL59F;case 3: _LL59E: _LL59F:
({void*_tmp8F6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp8F7="letdecl at toplevel";_tag_dyneither(_tmp8F7,sizeof(char),20);}),_tag_dyneither(_tmp8F6,sizeof(void*),0));});case 4: _LL5A0: _LL5A1:
({void*_tmp8F8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp8F9="region decl at toplevel";_tag_dyneither(_tmp8F9,sizeof(char),24);}),_tag_dyneither(_tmp8F8,sizeof(void*),0));});case 5: _LL5A2: _tmp900=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8EF)->f1;_LL5A3:
# 5182
 Cyc_Toc_aggrdecl_to_c(_tmp900,1);
goto _LL597;case 6: _LL5A4: _tmp901=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp8EF)->f1;_LL5A5:
# 5185
 if(_tmp901->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp901);else{
# 5188
Cyc_Toc_datatypedecl_to_c(_tmp901);}
goto _LL597;case 7: _LL5A6: _tmp902=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8EF)->f1;_LL5A7:
# 5191
 Cyc_Toc_enumdecl_to_c(_tmp902);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8FA=_cycalloc(sizeof(*_tmp8FA));_tmp8FA->hd=d;_tmp8FA->tl=Cyc_Toc_result_decls;_tmp8FA;});
goto _LL597;case 8: _LL5A8: _tmp903=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8EF)->f1;_LL5A9:
# 5195
 _tmp903->tvs=0;
if(_tmp903->defn != 0)
_tmp903->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp903->defn));else{
# 5199
enum Cyc_Absyn_KindQual _tmp8FB=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp903->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp8FC=_tmp8FB;if(_tmp8FC == Cyc_Absyn_BoxKind){_LL5B7: _LL5B8:
 _tmp903->defn=Cyc_Absyn_void_star_typ();goto _LL5B6;}else{_LL5B9: _LL5BA:
 _tmp903->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL5B6;}_LL5B6:;}
# 5204
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8FD=_cycalloc(sizeof(*_tmp8FD));_tmp8FD->hd=d;_tmp8FD->tl=Cyc_Toc_result_decls;_tmp8FD;});
goto _LL597;case 13: _LL5AA: _LL5AB:
 goto _LL5AD;case 14: _LL5AC: _LL5AD:
# 5208
 goto _LL597;case 9: _LL5AE: _tmp904=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp8EF)->f2;_LL5AF:
 _tmp905=_tmp904;goto _LL5B1;case 10: _LL5B0: _tmp905=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp8EF)->f2;_LL5B1:
 _tmp906=_tmp905;goto _LL5B3;case 11: _LL5B2: _tmp906=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp8EF)->f1;_LL5B3:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp906,top,cinclude);goto _LL597;default: _LL5B4: _tmp907=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp8EF)->f1;_LL5B5:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp907,top,1);goto _LL597;}_LL597:;};}
# 5215
return nv;}
# 5219
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp908=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp909=_tmp908;struct Cyc_Core_DynamicRegion*_tmp90D;_LL5BC: _tmp90D=_tmp909.key;_LL5BD:;{
struct Cyc_Toc_TocState*_tmp90A=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp90D,0,Cyc_Toc_empty_toc_state);
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp90B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp90B));_tmp90B->dyn=(struct Cyc_Core_DynamicRegion*)_tmp90D;_tmp90B->state=(struct Cyc_Toc_TocState*)_tmp90A;_tmp90B;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;
Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmp90C=_cycalloc(sizeof(struct _dyneither_ptr*)* 41);_tmp90C[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str;_tmp90C[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str;_tmp90C[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str;_tmp90C[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str;_tmp90C[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str;_tmp90C[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str;_tmp90C[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str;_tmp90C[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str;_tmp90C[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str;_tmp90C[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str;_tmp90C[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str;_tmp90C[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str;_tmp90C[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str;_tmp90C[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str;_tmp90C[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str;_tmp90C[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str;_tmp90C[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str;_tmp90C[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str;_tmp90C[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp90C[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str;_tmp90C[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp90C[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp90C[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str;_tmp90C[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str;_tmp90C[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str;_tmp90C[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str;_tmp90C[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str;_tmp90C[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str;_tmp90C[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str;_tmp90C[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str;_tmp90C[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str;_tmp90C[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str;_tmp90C[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str;_tmp90C[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str;_tmp90C[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str;_tmp90C[35]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str;_tmp90C[36]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp90C[37]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str;_tmp90C[38]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str;_tmp90C[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str;_tmp90C[40]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str;_tmp90C;}),sizeof(struct _dyneither_ptr*),41);};}
# 5275
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp90E=*ts;struct Cyc_Toc_TocState _tmp90F=_tmp90E;struct Cyc_Xarray_Xarray*_tmp910;_LL5BF: _tmp910=_tmp90F.temp_labels;_LL5C0:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp910);
return 0;}
# 5283
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 5285
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp911=_cycalloc(sizeof(*_tmp911));_tmp911[0]=pop_tables;_tmp911;});
Cyc_Toc_init();
{struct _RegionHandle _tmp912=_new_region("start");struct _RegionHandle*start=& _tmp912;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);;_pop_region(start);}{
# 5295
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp913=ts;struct Cyc_Toc_TocStateWrap*_tmp914=Cyc_Toc_toc_state;ts=_tmp914;Cyc_Toc_toc_state=_tmp913;});{
struct Cyc_Toc_TocStateWrap _tmp915=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp916=_tmp915;struct Cyc_Core_DynamicRegion*_tmp918;struct Cyc_Toc_TocState*_tmp917;_LL5C2: _tmp918=_tmp916.dyn;_tmp917=_tmp916.state;_LL5C3:;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp918,_tmp917,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp918);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 5302
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);};};}
