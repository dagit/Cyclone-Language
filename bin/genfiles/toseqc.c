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
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1009 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1094
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 309 "tcutil.h"
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 31 "toc.h"
typedef struct Cyc_Hashtable_Table*Cyc_Toc_table_t;
# 38
void*Cyc_Toc_typ_to_c(void*t);
void Cyc_Toc_finish();
# 42
struct _tuple1*Cyc_Toc_temp_var();
# 27 "toseqc.h"
struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*decls);
# 35 "toseqc.cyc"
enum Cyc_Toseqc_SideEffect{Cyc_Toseqc_Const  = 0,Cyc_Toseqc_NoEffect  = 1,Cyc_Toseqc_ExnEffect  = 2,Cyc_Toseqc_AnyEffect  = 3};
# 39
static struct _dyneither_ptr Cyc_Toseqc_eff_to_string(enum Cyc_Toseqc_SideEffect e){
enum Cyc_Toseqc_SideEffect _tmp0=e;switch(_tmp0){case Cyc_Toseqc_Const: _LL1: _LL2:
 return({const char*_tmp1="Const";_tag_dyneither(_tmp1,sizeof(char),6);});case Cyc_Toseqc_NoEffect: _LL3: _LL4:
 return({const char*_tmp2="NoEffect";_tag_dyneither(_tmp2,sizeof(char),9);});case Cyc_Toseqc_ExnEffect: _LL5: _LL6:
 return({const char*_tmp3="ExnEffect";_tag_dyneither(_tmp3,sizeof(char),10);});default: _LL7: _LL8:
 return({const char*_tmp4="AnyEffect";_tag_dyneither(_tmp4,sizeof(char),10);});}_LL0:;}struct _tuple11{enum Cyc_Toseqc_SideEffect f1;enum Cyc_Toseqc_SideEffect f2;};
# 48
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_join_side_effect(enum Cyc_Toseqc_SideEffect e1,enum Cyc_Toseqc_SideEffect e2){
struct _tuple11 _tmp5=({struct _tuple11 _tmp7;_tmp7.f1=e1;_tmp7.f2=e2;_tmp7;});struct _tuple11 _tmp6=_tmp5;if(_tmp6.f1 == Cyc_Toseqc_AnyEffect){_LLA: _LLB:
 return Cyc_Toseqc_AnyEffect;}else{if(_tmp6.f2 == Cyc_Toseqc_AnyEffect){_LLC: _LLD:
 return Cyc_Toseqc_AnyEffect;}else{if(_tmp6.f1 == Cyc_Toseqc_ExnEffect){if(_tmp6.f2 == Cyc_Toseqc_ExnEffect){_LLE: _LLF:
# 53
 return Cyc_Toseqc_AnyEffect;}else{_LL10: _LL11:
 return Cyc_Toseqc_ExnEffect;}}else{if(_tmp6.f2 == Cyc_Toseqc_ExnEffect){_LL12: _LL13:
 return Cyc_Toseqc_ExnEffect;}else{if(_tmp6.f1 == Cyc_Toseqc_NoEffect){_LL14: _LL15:
 return Cyc_Toseqc_NoEffect;}else{if(_tmp6.f2 == Cyc_Toseqc_NoEffect){_LL16: _LL17:
 return Cyc_Toseqc_NoEffect;}else{_LL18: _LL19:
 return Cyc_Toseqc_Const;}}}}}}_LL9:;}static char _tmp8[20]="_get_dyneither_size";static char _tmp9[24]="_get_zero_arr_size_char";static char _tmpA[25]="_get_zero_arr_size_short";static char _tmpB[23]="_get_zero_arr_size_int";static char _tmpC[25]="_get_zero_arr_size_float";static char _tmpD[26]="_get_zero_arr_size_double";static char _tmpE[30]="_get_zero_arr_size_longdouble";static char _tmpF[28]="_get_zero_arr_size_voidstar";
# 63
static struct _dyneither_ptr Cyc_Toseqc_pure_funs[8]={{_tmp8,_tmp8,_tmp8 + 20},{_tmp9,_tmp9,_tmp9 + 24},{_tmpA,_tmpA,_tmpA + 25},{_tmpB,_tmpB,_tmpB + 23},{_tmpC,_tmpC,_tmpC + 25},{_tmpD,_tmpD,_tmpD + 26},{_tmpE,_tmpE,_tmpE + 30},{_tmpF,_tmpF,_tmpF + 28}};static char _tmp10[12]="_check_null";static char _tmp11[28]="_check_known_subscript_null";static char _tmp12[31]="_check_known_subscript_notnull";static char _tmp13[27]="_check_dyneither_subscript";static char _tmp14[21]="_untag_dyneither_ptr";
# 74
static struct _dyneither_ptr Cyc_Toseqc_exn_effect_funs[5]={{_tmp10,_tmp10,_tmp10 + 12},{_tmp11,_tmp11,_tmp11 + 28},{_tmp12,_tmp12,_tmp12 + 31},{_tmp13,_tmp13,_tmp13 + 27},{_tmp14,_tmp14,_tmp14 + 21}};
# 81
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_fun_effect(struct _dyneither_ptr fn){
{int i=0;for(0;i < 8;++ i){
if(!Cyc_strcmp((struct _dyneither_ptr)Cyc_Toseqc_pure_funs[i],(struct _dyneither_ptr)fn))return Cyc_Toseqc_NoEffect;}}
# 85
{int i=0;for(0;i < 5;++ i){
if(!Cyc_strcmp((struct _dyneither_ptr)Cyc_Toseqc_exn_effect_funs[i],(struct _dyneither_ptr)fn))return Cyc_Toseqc_ExnEffect;}}
# 88
return Cyc_Toseqc_AnyEffect;}
# 91
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*e);
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*s);
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exps_effect(struct Cyc_List_List*es){
enum Cyc_Toseqc_SideEffect res=Cyc_Toseqc_Const;
# 97
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
enum Cyc_Toseqc_SideEffect _tmp15=Cyc_Toseqc_exp_effect((struct Cyc_Absyn_Exp*)x->hd);
({enum Cyc_Toseqc_SideEffect _tmpBC=Cyc_Toseqc_join_side_effect(_tmp15,res);res=_tmpBC;});
# 102
if(res == Cyc_Toseqc_AnyEffect)
# 104
return res;}}
# 108
return res;}
# 112
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*e){
struct _dyneither_ptr bad_form=({const char*_tmp4E="";_tag_dyneither(_tmp4E,sizeof(char),1);});
{void*_tmp16=e->r;void*_tmp17=_tmp16;struct Cyc_Absyn_Stmt*_tmp47;struct Cyc_Absyn_Exp*_tmp46;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct Cyc_Absyn_Exp*_tmp43;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_Absyn_Exp*_tmp41;struct Cyc_Absyn_Exp*_tmp40;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_Absyn_Exp*_tmp3C;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Exp*_tmp3A;struct Cyc_Absyn_Exp*_tmp39;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_Absyn_Exp*_tmp37;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_Absyn_Exp*_tmp35;struct Cyc_Absyn_Exp*_tmp34;struct Cyc_Absyn_Exp*_tmp33;struct Cyc_List_List*_tmp32;struct Cyc_List_List*_tmp31;struct Cyc_Absyn_Exp*_tmp30;switch(*((int*)_tmp17)){case 0: _LL1B: _LL1C:
 goto _LL1E;case 16: _LL1D: _LL1E:
 goto _LL20;case 18: _LL1F: _LL20:
 goto _LL22;case 32: _LL21: _LL22:
 goto _LL24;case 31: _LL23: _LL24:
 return Cyc_Toseqc_Const;case 1: _LL25: _LL26:
 return Cyc_Toseqc_NoEffect;case 3: _LL27: _LL28:
# 122
 goto _LL2A;case 39: _LL29: _LL2A:
 return Cyc_Toseqc_AnyEffect;case 9: _LL2B: _tmp30=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL2C: {
# 126
void*_tmp18=_tmp30->r;void*_tmp19=_tmp18;struct _tuple1*_tmp1C;struct Cyc_Absyn_Vardecl*_tmp1B;struct Cyc_Absyn_Fndecl*_tmp1A;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19)->tag == 1)switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19)->f1)){case 2: _LL6C: _tmp1A=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19)->f1)->f1;_LL6D:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp1A->name));case 1: _LL6E: _tmp1B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19)->f1)->f1;_LL6F:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp1B->name));case 0: _LL70: _tmp1C=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19)->f1)->f1;_LL71:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp1C));default: goto _LL72;}else{_LL72: _LL73:
# 131
 return Cyc_Toseqc_AnyEffect;}_LL6B:;}case 35: _LL2D: _tmp31=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL2E: {
# 135
struct _tuple0 _tmp1D=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp31);struct _tuple0 _tmp1E=_tmp1D;struct Cyc_List_List*_tmp1F;_LL75: _tmp1F=_tmp1E.f2;_LL76:;
_tmp32=_tmp1F;goto _LL30;}case 2: _LL2F: _tmp32=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL30:
# 139
 return Cyc_Toseqc_exps_effect(_tmp32);case 5: _LL31: _tmp35=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_tmp34=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_tmp33=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17)->f3;_LL32:
# 142
 return Cyc_Toseqc_exps_effect(({struct Cyc_Absyn_Exp*_tmp20[3];_tmp20[2]=_tmp33;_tmp20[1]=_tmp34;_tmp20[0]=_tmp35;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp20,sizeof(struct Cyc_Absyn_Exp*),3));}));case 22: _LL33: _tmp37=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_tmp36=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL34:
# 144
 _tmp39=_tmp37;_tmp38=_tmp36;goto _LL36;case 6: _LL35: _tmp39=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_tmp38=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL36:
 _tmp3B=_tmp39;_tmp3A=_tmp38;goto _LL38;case 7: _LL37: _tmp3B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_tmp3A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL38:
 _tmp3D=_tmp3B;_tmp3C=_tmp3A;goto _LL3A;case 8: _LL39: _tmp3D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_tmp3C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL3A:
# 148
 return Cyc_Toseqc_exps_effect(({struct Cyc_Absyn_Exp*_tmp21[2];_tmp21[1]=_tmp3C;_tmp21[0]=_tmp3D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21,sizeof(struct Cyc_Absyn_Exp*),2));}));case 11: _LL3B: _tmp3E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL3C:
# 150
 _tmp3F=_tmp3E;goto _LL3E;case 12: _LL3D: _tmp3F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL3E:
 _tmp40=_tmp3F;goto _LL40;case 13: _LL3F: _tmp40=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL40:
 _tmp41=_tmp40;goto _LL42;case 14: _LL41: _tmp41=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL42:
 _tmp42=_tmp41;goto _LL44;case 17: _LL43: _tmp42=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL44:
 _tmp43=_tmp42;goto _LL46;case 19: _LL45: _tmp43=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL46:
 _tmp44=_tmp43;goto _LL48;case 20: _LL47: _tmp44=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL48:
 _tmp45=_tmp44;goto _LL4A;case 21: _LL49: _tmp45=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL4A:
 _tmp46=_tmp45;goto _LL4C;case 4: _LL4B: _tmp46=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL4C:
# 159
 return Cyc_Toseqc_exp_effect(_tmp46);case 36: _LL4D: _tmp47=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL4E:
# 162
 return Cyc_Toseqc_stmt_effect(_tmp47);case 10: _LL4F: _LL50:
# 164
({struct _dyneither_ptr _tmpBD=({const char*_tmp22="Throw_e";_tag_dyneither(_tmp22,sizeof(char),8);});bad_form=_tmpBD;});goto _LL1A;case 15: _LL51: _LL52:
({struct _dyneither_ptr _tmpBE=({const char*_tmp23="New_e";_tag_dyneither(_tmp23,sizeof(char),6);});bad_form=_tmpBE;});goto _LL1A;case 23: _LL53: _LL54:
({struct _dyneither_ptr _tmpBF=({const char*_tmp24="Tuple_e";_tag_dyneither(_tmp24,sizeof(char),8);});bad_form=_tmpBF;});goto _LL1A;case 24: _LL55: _LL56:
({struct _dyneither_ptr _tmpC0=({const char*_tmp25="CompoundLit_e";_tag_dyneither(_tmp25,sizeof(char),14);});bad_form=_tmpC0;});goto _LL1A;case 25: _LL57: _LL58:
({struct _dyneither_ptr _tmpC1=({const char*_tmp26="Array_e";_tag_dyneither(_tmp26,sizeof(char),8);});bad_form=_tmpC1;});goto _LL1A;case 26: _LL59: _LL5A:
({struct _dyneither_ptr _tmpC2=({const char*_tmp27="Comprehension_e";_tag_dyneither(_tmp27,sizeof(char),16);});bad_form=_tmpC2;});goto _LL1A;case 27: _LL5B: _LL5C:
({struct _dyneither_ptr _tmpC3=({const char*_tmp28="ComprehensionNoinit_e";_tag_dyneither(_tmp28,sizeof(char),22);});bad_form=_tmpC3;});goto _LL1A;case 28: _LL5D: _LL5E:
({struct _dyneither_ptr _tmpC4=({const char*_tmp29="Aggregate_e";_tag_dyneither(_tmp29,sizeof(char),12);});bad_form=_tmpC4;});goto _LL1A;case 29: _LL5F: _LL60:
({struct _dyneither_ptr _tmpC5=({const char*_tmp2A="AnonStruct_e";_tag_dyneither(_tmp2A,sizeof(char),13);});bad_form=_tmpC5;});goto _LL1A;case 30: _LL61: _LL62:
({struct _dyneither_ptr _tmpC6=({const char*_tmp2B="Datatype_e";_tag_dyneither(_tmp2B,sizeof(char),11);});bad_form=_tmpC6;});goto _LL1A;case 33: _LL63: _LL64:
({struct _dyneither_ptr _tmpC7=({const char*_tmp2C="Malloc_e";_tag_dyneither(_tmp2C,sizeof(char),9);});bad_form=_tmpC7;});goto _LL1A;case 34: _LL65: _LL66:
({struct _dyneither_ptr _tmpC8=({const char*_tmp2D="Swap_e";_tag_dyneither(_tmp2D,sizeof(char),7);});bad_form=_tmpC8;});goto _LL1A;case 37: _LL67: _LL68:
({struct _dyneither_ptr _tmpC9=({const char*_tmp2E="Tagcheck_e";_tag_dyneither(_tmp2E,sizeof(char),11);});bad_form=_tmpC9;});goto _LL1A;default: _LL69: _LL6A:
({struct _dyneither_ptr _tmpCA=({const char*_tmp2F="Valueof_e";_tag_dyneither(_tmp2F,sizeof(char),10);});bad_form=_tmpCA;});goto _LL1A;}_LL1A:;}
# 179
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp48=_cycalloc(sizeof(*_tmp48));({struct Cyc_Core_Impossible_exn_struct _tmpCE=({struct Cyc_Core_Impossible_exn_struct _tmp49;_tmp49.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmpCD=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4D;_tmp4D.tag=0;({struct _dyneither_ptr _tmpCB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp4D.f1=_tmpCB;});({struct Cyc_String_pa_PrintArg_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)bad_form);({void*_tmp4A[2]={& _tmp4C,& _tmp4D};({struct _dyneither_ptr _tmpCC=({const char*_tmp4B="bad exp form %s (exp=|%s|) after xlation to C";_tag_dyneither(_tmp4B,sizeof(char),46);});Cyc_aprintf(_tmpCC,_tag_dyneither(_tmp4A,sizeof(void*),2));});});});});_tmp49.f1=_tmpCD;});_tmp49;});_tmp48[0]=_tmpCE;});_tmp48;}));}
# 182
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*s){
# 184
enum Cyc_Toseqc_SideEffect res=Cyc_Toseqc_Const;
while(1){
void*_tmp4F=s->r;void*_tmp50=_tmp4F;struct Cyc_Absyn_Stmt*_tmp57;struct Cyc_Absyn_Stmt*_tmp56;struct Cyc_Absyn_Stmt*_tmp55;struct Cyc_Absyn_Exp*_tmp54;switch(*((int*)_tmp50)){case 0: _LL78: _LL79:
 return res;case 1: _LL7A: _tmp54=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp50)->f1;_LL7B:
 return({enum Cyc_Toseqc_SideEffect _tmpCF=res;Cyc_Toseqc_join_side_effect(_tmpCF,Cyc_Toseqc_exp_effect(_tmp54));});case 13: _LL7C: _tmp55=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp50)->f2;_LL7D:
# 191
 s=_tmp55;
continue;case 2: _LL7E: _tmp57=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp50)->f1;_tmp56=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp50)->f2;_LL7F:
# 194
({enum Cyc_Toseqc_SideEffect _tmpD1=({enum Cyc_Toseqc_SideEffect _tmpD0=res;Cyc_Toseqc_join_side_effect(_tmpD0,Cyc_Toseqc_stmt_effect(_tmp57));});res=_tmpD1;});
s=_tmp56;
continue;case 6: _LL80: _LL81:
# 198
 goto _LL83;case 7: _LL82: _LL83:
 goto _LL85;case 8: _LL84: _LL85:
 goto _LL87;case 3: _LL86: _LL87:
 goto _LL89;case 4: _LL88: _LL89:
 goto _LL8B;case 5: _LL8A: _LL8B:
 goto _LL8D;case 9: _LL8C: _LL8D:
 goto _LL8F;case 14: _LL8E: _LL8F:
 goto _LL91;case 12: _LL90: _LL91:
 goto _LL93;case 10: _LL92: _LL93:
 return Cyc_Toseqc_AnyEffect;default: _LL94: _LL95:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp51=_cycalloc(sizeof(*_tmp51));({struct Cyc_Core_Impossible_exn_struct _tmpD3=({struct Cyc_Core_Impossible_exn_struct _tmp52;_tmp52.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmpD2=({const char*_tmp53="bad stmt after xlation to C";_tag_dyneither(_tmp53,sizeof(char),28);});_tmp52.f1=_tmpD2;});_tmp52;});_tmp51[0]=_tmpD3;});_tmp51;}));}_LL77:;}}
# 213
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*s);
# 231 "toseqc.cyc"
static void Cyc_Toseqc_exps_to_seqc(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,int is_assign){
# 233
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toseqc_exp_to_seqc,es);
# 236
if(({((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)<= 1;}) || ({Cyc_Toseqc_exps_effect(es)!= Cyc_Toseqc_AnyEffect;}))return;{
# 240
struct Cyc_Absyn_Stmt*stmt=({struct Cyc_Absyn_Exp*_tmpD4=Cyc_Absyn_copy_exp(e);Cyc_Absyn_exp_stmt(_tmpD4,e->loc);});
int did_skip=0;
int did_convert_lval=0;
{struct Cyc_List_List*x=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(es);for(0;x != 0;x=x->tl){
struct Cyc_Absyn_Exp*_tmp58=(struct Cyc_Absyn_Exp*)x->hd;
# 247
if(Cyc_Tcutil_is_const_exp(_tmp58))continue;
# 252
if(!did_skip){
did_skip=1;
# 259
continue;}{
# 263
struct _tuple1*_tmp59=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5A=Cyc_Absyn_var_exp(_tmp59,_tmp58->loc);
# 267
struct Cyc_Absyn_Exp*_tmp5B=Cyc_Absyn_new_exp(_tmp58->r,_tmp58->loc);
_tmp5B->annot=_tmp58->annot;
_tmp5B->topt=_tmp58->topt;{
# 271
void*t=_tmp5B->topt == 0?(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpD5=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp5D;_tmp5D.tag=27;_tmp5D.f1=_tmp5B;_tmp5D;});_tmp5C[0]=_tmpD5;});_tmp5C;}): Cyc_Toc_typ_to_c((void*)_check_null(_tmp5B->topt));
# 275
({struct Cyc_Absyn_Stmt*_tmpD6=Cyc_Absyn_declare_stmt(_tmp59,t,_tmp5B,stmt,e->loc);stmt=_tmpD6;});
# 278
_tmp58->r=_tmp5A->r;};};}}
# 282
({void*_tmpD7=(Cyc_Absyn_stmt_exp(stmt,e->loc))->r;e->r=_tmpD7;});};}
# 287
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*e){
struct _dyneither_ptr bad_form=({const char*_tmp93="";_tag_dyneither(_tmp93,sizeof(char),1);});
{void*_tmp5E=e->r;void*_tmp5F=_tmp5E;struct Cyc_Absyn_Stmt*_tmp8C;struct Cyc_List_List*_tmp8B;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_Absyn_Exp*_tmp81;struct Cyc_Absyn_Exp*_tmp80;struct Cyc_Absyn_Exp*_tmp7F;struct Cyc_Absyn_Exp*_tmp7E;struct Cyc_Absyn_Exp*_tmp7D;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Exp*_tmp7B;struct Cyc_Absyn_Exp*_tmp7A;struct Cyc_Absyn_Exp*_tmp79;struct Cyc_Absyn_Exp*_tmp78;struct Cyc_Absyn_Exp*_tmp77;struct Cyc_Absyn_Exp*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_List_List*_tmp74;struct Cyc_Absyn_Exp*_tmp73;struct Cyc_List_List*_tmp72;switch(*((int*)_tmp5F)){case 0: _LL97: _LL98:
 return;case 1: _LL99: _LL9A:
 return;case 9: _LL9B: _tmp73=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_tmp72=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5F)->f2;_LL9C:
# 295
 Cyc_Toseqc_exp_to_seqc(_tmp73);_tmp74=_tmp72;goto _LL9E;case 2: _LL9D: _tmp74=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5F)->f2;_LL9E:
# 297
 Cyc_Toseqc_exps_to_seqc(e,_tmp74,0);
return;case 22: _LL9F: _tmp76=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_tmp75=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5F)->f2;_LLA0:
# 300
 _tmp78=_tmp76;_tmp77=_tmp75;goto _LLA2;case 3: _LLA1: _tmp78=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_tmp77=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5F)->f3;_LLA2:
# 303
({struct Cyc_Absyn_Exp*_tmpD8=e;Cyc_Toseqc_exps_to_seqc(_tmpD8,({struct Cyc_Absyn_Exp*_tmp60[2];_tmp60[1]=_tmp78;_tmp60[0]=_tmp77;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60,sizeof(struct Cyc_Absyn_Exp*),2));}),1);});
return;case 5: _LLA3: _tmp7B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_tmp7A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5F)->f2;_tmp79=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5F)->f3;_LLA4:
# 307
 Cyc_Toseqc_exp_to_seqc(_tmp79);_tmp7D=_tmp7B;_tmp7C=_tmp7A;goto _LLA6;case 6: _LLA5: _tmp7D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_tmp7C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5F)->f2;_LLA6:
 _tmp7F=_tmp7D;_tmp7E=_tmp7C;goto _LLA8;case 7: _LLA7: _tmp7F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_tmp7E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5F)->f2;_LLA8:
 _tmp81=_tmp7F;_tmp80=_tmp7E;goto _LLAA;case 8: _LLA9: _tmp81=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_tmp80=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5F)->f2;_LLAA:
# 311
 Cyc_Toseqc_exp_to_seqc(_tmp80);_tmp82=_tmp81;goto _LLAC;case 11: _LLAB: _tmp82=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_LLAC:
# 313
 _tmp83=_tmp82;goto _LLAE;case 12: _LLAD: _tmp83=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_LLAE:
 _tmp84=_tmp83;goto _LLB0;case 13: _LLAF: _tmp84=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5F)->f2;_LLB0:
 _tmp85=_tmp84;goto _LLB2;case 14: _LLB1: _tmp85=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_LLB2:
 _tmp86=_tmp85;goto _LLB4;case 17: _LLB3: _tmp86=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_LLB4:
 _tmp87=_tmp86;goto _LLB6;case 19: _LLB5: _tmp87=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_LLB6:
 _tmp88=_tmp87;goto _LLB8;case 20: _LLB7: _tmp88=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_LLB8:
 _tmp89=_tmp88;goto _LLBA;case 21: _LLB9: _tmp89=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_LLBA:
 _tmp8A=_tmp89;goto _LLBC;case 4: _LLBB: _tmp8A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_LLBC:
# 322
 Cyc_Toseqc_exp_to_seqc(_tmp8A);
return;case 16: _LLBD: _LLBE:
# 325
 goto _LLC0;case 18: _LLBF: _LLC0:
 goto _LLC2;case 32: _LLC1: _LLC2:
 goto _LLC4;case 31: _LLC3: _LLC4:
 return;case 35: _LLC5: _tmp8B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5F)->f2;_LLC6: {
# 331
struct _tuple0 _tmp61=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp8B);struct _tuple0 _tmp62=_tmp61;struct Cyc_List_List*_tmp63;_LLE8: _tmp63=_tmp62.f2;_LLE9:;
Cyc_Toseqc_exps_to_seqc(e,_tmp63,0);
return;}case 36: _LLC7: _tmp8C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp5F)->f1;_LLC8:
# 336
 Cyc_Toseqc_stmt_to_seqc(_tmp8C);
return;case 39: _LLC9: _LLCA:
# 339
 return;case 10: _LLCB: _LLCC:
# 341
({struct _dyneither_ptr _tmpD9=({const char*_tmp64="Throw_e";_tag_dyneither(_tmp64,sizeof(char),8);});bad_form=_tmpD9;});goto _LL96;case 15: _LLCD: _LLCE:
({struct _dyneither_ptr _tmpDA=({const char*_tmp65="New_e";_tag_dyneither(_tmp65,sizeof(char),6);});bad_form=_tmpDA;});goto _LL96;case 23: _LLCF: _LLD0:
({struct _dyneither_ptr _tmpDB=({const char*_tmp66="Tuple_e";_tag_dyneither(_tmp66,sizeof(char),8);});bad_form=_tmpDB;});goto _LL96;case 24: _LLD1: _LLD2:
({struct _dyneither_ptr _tmpDC=({const char*_tmp67="CompoundLit_e";_tag_dyneither(_tmp67,sizeof(char),14);});bad_form=_tmpDC;});goto _LL96;case 25: _LLD3: _LLD4:
({struct _dyneither_ptr _tmpDD=({const char*_tmp68="Array_e";_tag_dyneither(_tmp68,sizeof(char),8);});bad_form=_tmpDD;});goto _LL96;case 26: _LLD5: _LLD6:
({struct _dyneither_ptr _tmpDE=({const char*_tmp69="Comprehension_e";_tag_dyneither(_tmp69,sizeof(char),16);});bad_form=_tmpDE;});goto _LL96;case 27: _LLD7: _LLD8:
({struct _dyneither_ptr _tmpDF=({const char*_tmp6A="ComprehensionNoinit_e";_tag_dyneither(_tmp6A,sizeof(char),22);});bad_form=_tmpDF;});goto _LL96;case 28: _LLD9: _LLDA:
({struct _dyneither_ptr _tmpE0=({const char*_tmp6B="Aggregate_e";_tag_dyneither(_tmp6B,sizeof(char),12);});bad_form=_tmpE0;});goto _LL96;case 29: _LLDB: _LLDC:
({struct _dyneither_ptr _tmpE1=({const char*_tmp6C="AnonStruct_e";_tag_dyneither(_tmp6C,sizeof(char),13);});bad_form=_tmpE1;});goto _LL96;case 30: _LLDD: _LLDE:
# 351
({struct _dyneither_ptr _tmpE2=({const char*_tmp6D="Datatype_e";_tag_dyneither(_tmp6D,sizeof(char),11);});bad_form=_tmpE2;});goto _LL96;case 33: _LLDF: _LLE0:
({struct _dyneither_ptr _tmpE3=({const char*_tmp6E="Malloc_e";_tag_dyneither(_tmp6E,sizeof(char),9);});bad_form=_tmpE3;});goto _LL96;case 34: _LLE1: _LLE2:
({struct _dyneither_ptr _tmpE4=({const char*_tmp6F="Swap_e";_tag_dyneither(_tmp6F,sizeof(char),7);});bad_form=_tmpE4;});goto _LL96;case 37: _LLE3: _LLE4:
({struct _dyneither_ptr _tmpE5=({const char*_tmp70="Tagcheck_e";_tag_dyneither(_tmp70,sizeof(char),11);});bad_form=_tmpE5;});goto _LL96;default: _LLE5: _LLE6:
({struct _dyneither_ptr _tmpE6=({const char*_tmp71="Valueof_e";_tag_dyneither(_tmp71,sizeof(char),10);});bad_form=_tmpE6;});goto _LL96;}_LL96:;}
# 357
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));({struct Cyc_Core_Impossible_exn_struct _tmpEA=({struct Cyc_Core_Impossible_exn_struct _tmp8E;_tmp8E.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmpE9=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp92;_tmp92.tag=0;({struct _dyneither_ptr _tmpE7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp92.f1=_tmpE7;});({struct Cyc_String_pa_PrintArg_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)bad_form);({void*_tmp8F[2]={& _tmp91,& _tmp92};({struct _dyneither_ptr _tmpE8=({const char*_tmp90="bad exp form %s (exp=|%s|) after xlation to C";_tag_dyneither(_tmp90,sizeof(char),46);});Cyc_aprintf(_tmpE8,_tag_dyneither(_tmp8F,sizeof(void*),2));});});});});_tmp8E.f1=_tmpE9;});_tmp8E;});_tmp8D[0]=_tmpEA;});_tmp8D;}));}
# 361
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*s){
# 363
while(1){
void*_tmp94=s->r;void*_tmp95=_tmp94;struct Cyc_Absyn_Decl*_tmpAF;struct Cyc_Absyn_Stmt*_tmpAE;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_List_List*_tmpAC;struct Cyc_Absyn_Stmt*_tmpAB;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Stmt*_tmpA6;struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Stmt*_tmpA2;struct Cyc_Absyn_Stmt*_tmpA1;struct Cyc_Absyn_Stmt*_tmpA0;struct Cyc_Absyn_Stmt*_tmp9F;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Stmt*_tmp9C;switch(*((int*)_tmp95)){case 0: _LLEB: _LLEC:
 goto _LLEE;case 6: _LLED: _LLEE:
 goto _LLF0;case 7: _LLEF: _LLF0:
 goto _LLF2;case 8: _LLF1: _LLF2:
 return;case 13: _LLF3: _tmp9C=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp95)->f2;_LLF4:
# 370
 s=_tmp9C;
continue;case 3: _LLF5: _tmp9D=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp95)->f1;_LLF6:
# 373
 if(_tmp9D == 0)
return;
_tmp9E=_tmp9D;goto _LLF8;case 1: _LLF7: _tmp9E=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp95)->f1;_LLF8:
# 377
 Cyc_Toseqc_exp_to_seqc(_tmp9E);
# 385
return;case 2: _LLF9: _tmpA0=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp95)->f1;_tmp9F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp95)->f2;_LLFA:
# 387
 Cyc_Toseqc_stmt_to_seqc(_tmpA0);
s=_tmp9F;
continue;case 4: _LLFB: _tmpA3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp95)->f1;_tmpA2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp95)->f2;_tmpA1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp95)->f3;_LLFC:
# 391
 Cyc_Toseqc_exp_to_seqc(_tmpA3);
Cyc_Toseqc_stmt_to_seqc(_tmpA2);
s=_tmpA1;
continue;case 5: _LLFD: _tmpA5=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp95)->f1).f1;_tmpA4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp95)->f2;_LLFE:
# 396
 Cyc_Toseqc_exp_to_seqc(_tmpA5);
s=_tmpA4;
continue;case 9: _LLFF: _tmpA9=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp95)->f1;_tmpA8=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp95)->f2).f1;_tmpA7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp95)->f3).f1;_tmpA6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp95)->f4;_LL100:
# 400
 Cyc_Toseqc_exp_to_seqc(_tmpA9);
Cyc_Toseqc_exp_to_seqc(_tmpA8);
Cyc_Toseqc_exp_to_seqc(_tmpA7);
s=_tmpA6;
continue;case 14: _LL101: _tmpAB=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp95)->f1;_tmpAA=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp95)->f2).f1;_LL102:
# 406
 Cyc_Toseqc_exp_to_seqc(_tmpAA);
s=_tmpAB;
continue;case 10: _LL103: _tmpAD=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp95)->f1;_tmpAC=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp95)->f2;_LL104:
# 412
 Cyc_Toseqc_exp_to_seqc(_tmpAD);
for(0;_tmpAC != 0;_tmpAC=_tmpAC->tl){
Cyc_Toseqc_stmt_to_seqc(((struct Cyc_Absyn_Switch_clause*)_tmpAC->hd)->body);}
return;case 12: _LL105: _tmpAF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp95)->f1;_tmpAE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp95)->f2;_LL106:
# 417
{void*_tmp96=_tmpAF->r;void*_tmp97=_tmp96;struct Cyc_Absyn_Vardecl*_tmp98;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp97)->tag == 0){_LL10A: _tmp98=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp97)->f1;_LL10B:
# 419
 if(_tmp98->initializer != 0)
Cyc_Toseqc_exp_to_seqc((struct Cyc_Absyn_Exp*)_check_null(_tmp98->initializer));
s=_tmpAE;
continue;}else{_LL10C: _LL10D:
 goto _LL109;}_LL109:;}
# 425
goto _LL108;default: _LL107: _LL108:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp99=_cycalloc(sizeof(*_tmp99));({struct Cyc_Core_Impossible_exn_struct _tmpEC=({struct Cyc_Core_Impossible_exn_struct _tmp9A;_tmp9A.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmpEB=({const char*_tmp9B="bad stmt after xlation to C";_tag_dyneither(_tmp9B,sizeof(char),28);});_tmp9A.f1=_tmpEB;});_tmp9A;});_tmp99[0]=_tmpEC;});_tmp99;}));}_LLEA:;}}
# 431
struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmpB0=ds;
# 434
for(0;_tmpB0 != 0;_tmpB0=_tmpB0->tl){
struct Cyc_Absyn_Decl*_tmpB1=(struct Cyc_Absyn_Decl*)_tmpB0->hd;
void*_tmpB2=_tmpB1->r;void*_tmpB3=_tmpB2;struct Cyc_List_List*_tmpBB;struct Cyc_List_List*_tmpBA;struct Cyc_List_List*_tmpB9;struct Cyc_List_List*_tmpB8;struct Cyc_Absyn_Fndecl*_tmpB7;switch(*((int*)_tmpB3)){case 1: _LL10F: _tmpB7=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpB3)->f1;_LL110:
# 438
 Cyc_Toseqc_stmt_to_seqc(_tmpB7->body);
goto _LL10E;case 0: _LL111: _LL112:
 goto _LL114;case 2: _LL113: _LL114:
 goto _LL116;case 3: _LL115: _LL116:
 goto _LL118;case 4: _LL117: _LL118:
 goto _LL11A;case 5: _LL119: _LL11A:
 goto _LL11C;case 6: _LL11B: _LL11C:
 goto _LL11E;case 7: _LL11D: _LL11E:
 goto _LL120;case 8: _LL11F: _LL120:
# 448
 goto _LL10E;case 9: _LL121: _tmpB8=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpB3)->f2;_LL122:
 _tmpB9=_tmpB8;goto _LL124;case 10: _LL123: _tmpB9=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpB3)->f2;_LL124:
 _tmpBA=_tmpB9;goto _LL126;case 11: _LL125: _tmpBA=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpB3)->f1;_LL126:
 _tmpBB=_tmpBA;goto _LL128;case 12: _LL127: _tmpBB=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpB3)->f1;_LL128:
 goto _LL12A;case 13: _LL129: _LL12A:
 goto _LL12C;default: _LL12B: _LL12C:
# 455
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct Cyc_Core_Impossible_exn_struct _tmpEE=({struct Cyc_Core_Impossible_exn_struct _tmpB5;_tmpB5.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmpED=({const char*_tmpB6="nested translation unit after translation to C";_tag_dyneither(_tmpB6,sizeof(char),47);});_tmpB5.f1=_tmpED;});_tmpB5;});_tmpB4[0]=_tmpEE;});_tmpB4;}));}_LL10E:;}
# 458
return ds;}
