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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1004 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1078
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1090
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 84
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
({enum Cyc_Toseqc_SideEffect _tmpD9=Cyc_Toseqc_join_side_effect(_tmp15,res);res=_tmpD9;});
# 102
if(res == Cyc_Toseqc_AnyEffect)
# 104
return res;}}
# 108
return res;}
# 112
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*e){
struct _dyneither_ptr bad_form=({const char*_tmp4F="";_tag_dyneither(_tmp4F,sizeof(char),1);});
{void*_tmp16=e->r;void*_tmp17=_tmp16;struct Cyc_Absyn_Stmt*_tmp48;struct Cyc_Absyn_Exp*_tmp47;struct Cyc_Absyn_Exp*_tmp46;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct Cyc_Absyn_Exp*_tmp43;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_Absyn_Exp*_tmp41;struct Cyc_Absyn_Exp*_tmp40;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_Absyn_Exp*_tmp3C;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Exp*_tmp3A;struct Cyc_Absyn_Exp*_tmp39;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_Absyn_Exp*_tmp37;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_Absyn_Exp*_tmp35;struct Cyc_Absyn_Exp*_tmp34;struct Cyc_List_List*_tmp33;struct Cyc_List_List*_tmp32;struct Cyc_Absyn_Exp*_tmp31;switch(*((int*)_tmp17)){case 0: _LL1B: _LL1C:
 goto _LL1E;case 17: _LL1D: _LL1E:
 goto _LL20;case 19: _LL1F: _LL20:
 goto _LL22;case 33: _LL21: _LL22:
 goto _LL24;case 32: _LL23: _LL24:
 return Cyc_Toseqc_Const;case 1: _LL25: _LL26:
 return Cyc_Toseqc_NoEffect;case 4: _LL27: _LL28:
# 122
 goto _LL2A;case 40: _LL29: _LL2A:
 return Cyc_Toseqc_AnyEffect;case 10: _LL2B: _tmp31=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL2C: {
# 126
void*_tmp18=_tmp31->r;void*_tmp19=_tmp18;struct _tuple1*_tmp1C;struct Cyc_Absyn_Vardecl*_tmp1B;struct Cyc_Absyn_Fndecl*_tmp1A;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19)->tag == 1)switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19)->f1)){case 2: _LL6E: _tmp1A=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19)->f1)->f1;_LL6F:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp1A->name));case 1: _LL70: _tmp1B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19)->f1)->f1;_LL71:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp1B->name));case 0: _LL72: _tmp1C=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19)->f1)->f1;_LL73:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp1C));default: goto _LL74;}else{_LL74: _LL75:
# 131
 return Cyc_Toseqc_AnyEffect;}_LL6D:;}case 36: _LL2D: _tmp32=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL2E: {
# 135
struct _tuple0 _tmp1D=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp32);struct _tuple0 _tmp1E=_tmp1D;struct Cyc_List_List*_tmp1F;_LL77: _tmp1F=_tmp1E.f2;_LL78:;
_tmp33=_tmp1F;goto _LL30;}case 3: _LL2F: _tmp33=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL30:
# 139
 return Cyc_Toseqc_exps_effect(_tmp33);case 6: _LL31: _tmp36=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_tmp35=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_tmp34=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17)->f3;_LL32:
# 142
 return Cyc_Toseqc_exps_effect(({struct Cyc_Absyn_Exp*_tmp20[3];_tmp20[2]=_tmp34;_tmp20[1]=_tmp35;_tmp20[0]=_tmp36;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp20,sizeof(struct Cyc_Absyn_Exp*),3));}));case 23: _LL33: _tmp38=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_tmp37=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL34:
# 144
 _tmp3A=_tmp38;_tmp39=_tmp37;goto _LL36;case 7: _LL35: _tmp3A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_tmp39=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL36:
 _tmp3C=_tmp3A;_tmp3B=_tmp39;goto _LL38;case 8: _LL37: _tmp3C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_tmp3B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL38:
 _tmp3E=_tmp3C;_tmp3D=_tmp3B;goto _LL3A;case 9: _LL39: _tmp3E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_tmp3D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL3A:
# 148
 return Cyc_Toseqc_exps_effect(({struct Cyc_Absyn_Exp*_tmp21[2];_tmp21[1]=_tmp3D;_tmp21[0]=_tmp3E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21,sizeof(struct Cyc_Absyn_Exp*),2));}));case 12: _LL3B: _tmp3F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL3C:
# 150
 _tmp40=_tmp3F;goto _LL3E;case 13: _LL3D: _tmp40=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL3E:
 _tmp41=_tmp40;goto _LL40;case 14: _LL3F: _tmp41=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp17)->f2;_LL40:
 _tmp42=_tmp41;goto _LL42;case 15: _LL41: _tmp42=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL42:
 _tmp43=_tmp42;goto _LL44;case 18: _LL43: _tmp43=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL44:
 _tmp44=_tmp43;goto _LL46;case 20: _LL45: _tmp44=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL46:
 _tmp45=_tmp44;goto _LL48;case 21: _LL47: _tmp45=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL48:
 _tmp46=_tmp45;goto _LL4A;case 22: _LL49: _tmp46=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL4A:
 _tmp47=_tmp46;goto _LL4C;case 5: _LL4B: _tmp47=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL4C:
# 159
 return Cyc_Toseqc_exp_effect(_tmp47);case 37: _LL4D: _tmp48=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp17)->f1;_LL4E:
# 162
 return Cyc_Toseqc_stmt_effect(_tmp48);case 2: _LL4F: _LL50:
# 164
({struct _dyneither_ptr _tmpDA=({const char*_tmp22="Pragma_e";_tag_dyneither(_tmp22,sizeof(char),9);});bad_form=_tmpDA;});goto _LL1A;case 11: _LL51: _LL52:
({struct _dyneither_ptr _tmpDB=({const char*_tmp23="Throw_e";_tag_dyneither(_tmp23,sizeof(char),8);});bad_form=_tmpDB;});goto _LL1A;case 16: _LL53: _LL54:
({struct _dyneither_ptr _tmpDC=({const char*_tmp24="New_e";_tag_dyneither(_tmp24,sizeof(char),6);});bad_form=_tmpDC;});goto _LL1A;case 24: _LL55: _LL56:
({struct _dyneither_ptr _tmpDD=({const char*_tmp25="Tuple_e";_tag_dyneither(_tmp25,sizeof(char),8);});bad_form=_tmpDD;});goto _LL1A;case 25: _LL57: _LL58:
({struct _dyneither_ptr _tmpDE=({const char*_tmp26="CompoundLit_e";_tag_dyneither(_tmp26,sizeof(char),14);});bad_form=_tmpDE;});goto _LL1A;case 26: _LL59: _LL5A:
({struct _dyneither_ptr _tmpDF=({const char*_tmp27="Array_e";_tag_dyneither(_tmp27,sizeof(char),8);});bad_form=_tmpDF;});goto _LL1A;case 27: _LL5B: _LL5C:
({struct _dyneither_ptr _tmpE0=({const char*_tmp28="Comprehension_e";_tag_dyneither(_tmp28,sizeof(char),16);});bad_form=_tmpE0;});goto _LL1A;case 28: _LL5D: _LL5E:
({struct _dyneither_ptr _tmpE1=({const char*_tmp29="ComprehensionNoinit_e";_tag_dyneither(_tmp29,sizeof(char),22);});bad_form=_tmpE1;});goto _LL1A;case 29: _LL5F: _LL60:
({struct _dyneither_ptr _tmpE2=({const char*_tmp2A="Aggregate_e";_tag_dyneither(_tmp2A,sizeof(char),12);});bad_form=_tmpE2;});goto _LL1A;case 30: _LL61: _LL62:
({struct _dyneither_ptr _tmpE3=({const char*_tmp2B="AnonStruct_e";_tag_dyneither(_tmp2B,sizeof(char),13);});bad_form=_tmpE3;});goto _LL1A;case 31: _LL63: _LL64:
({struct _dyneither_ptr _tmpE4=({const char*_tmp2C="Datatype_e";_tag_dyneither(_tmp2C,sizeof(char),11);});bad_form=_tmpE4;});goto _LL1A;case 34: _LL65: _LL66:
({struct _dyneither_ptr _tmpE5=({const char*_tmp2D="Malloc_e";_tag_dyneither(_tmp2D,sizeof(char),9);});bad_form=_tmpE5;});goto _LL1A;case 35: _LL67: _LL68:
({struct _dyneither_ptr _tmpE6=({const char*_tmp2E="Swap_e";_tag_dyneither(_tmp2E,sizeof(char),7);});bad_form=_tmpE6;});goto _LL1A;case 38: _LL69: _LL6A:
({struct _dyneither_ptr _tmpE7=({const char*_tmp2F="Tagcheck_e";_tag_dyneither(_tmp2F,sizeof(char),11);});bad_form=_tmpE7;});goto _LL1A;default: _LL6B: _LL6C:
({struct _dyneither_ptr _tmpE8=({const char*_tmp30="Valueof_e";_tag_dyneither(_tmp30,sizeof(char),10);});bad_form=_tmpE8;});goto _LL1A;}_LL1A:;}
# 180
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp49=_cycalloc(sizeof(*_tmp49));({struct Cyc_Core_Impossible_exn_struct _tmpEC=({struct Cyc_Core_Impossible_exn_struct _tmp4A;_tmp4A.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmpEB=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4E;_tmp4E.tag=0;({struct _dyneither_ptr _tmpE9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp4E.f1=_tmpE9;});({struct Cyc_String_pa_PrintArg_struct _tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)bad_form);({void*_tmp4B[2]={& _tmp4D,& _tmp4E};({struct _dyneither_ptr _tmpEA=({const char*_tmp4C="bad exp form %s (exp=|%s|) after xlation to C";_tag_dyneither(_tmp4C,sizeof(char),46);});Cyc_aprintf(_tmpEA,_tag_dyneither(_tmp4B,sizeof(void*),2));});});});});_tmp4A.f1=_tmpEB;});_tmp4A;});_tmp49[0]=_tmpEC;});_tmp49;}));}
# 183
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*s){
# 185
enum Cyc_Toseqc_SideEffect res=Cyc_Toseqc_Const;
while(1){
void*_tmp50=s->r;void*_tmp51=_tmp50;struct Cyc_Absyn_Stmt*_tmp58;struct Cyc_Absyn_Stmt*_tmp57;struct Cyc_Absyn_Stmt*_tmp56;struct Cyc_Absyn_Exp*_tmp55;switch(*((int*)_tmp51)){case 0: _LL7A: _LL7B:
 return res;case 1: _LL7C: _tmp55=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp51)->f1;_LL7D:
 return({enum Cyc_Toseqc_SideEffect _tmpED=res;Cyc_Toseqc_join_side_effect(_tmpED,Cyc_Toseqc_exp_effect(_tmp55));});case 13: _LL7E: _tmp56=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp51)->f2;_LL7F:
# 192
 s=_tmp56;
continue;case 2: _LL80: _tmp58=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp51)->f1;_tmp57=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp51)->f2;_LL81:
# 195
({enum Cyc_Toseqc_SideEffect _tmpEF=({enum Cyc_Toseqc_SideEffect _tmpEE=res;Cyc_Toseqc_join_side_effect(_tmpEE,Cyc_Toseqc_stmt_effect(_tmp58));});res=_tmpEF;});
s=_tmp57;
continue;case 6: _LL82: _LL83:
# 199
 goto _LL85;case 7: _LL84: _LL85:
 goto _LL87;case 8: _LL86: _LL87:
 goto _LL89;case 3: _LL88: _LL89:
 goto _LL8B;case 4: _LL8A: _LL8B:
 goto _LL8D;case 5: _LL8C: _LL8D:
 goto _LL8F;case 9: _LL8E: _LL8F:
 goto _LL91;case 14: _LL90: _LL91:
 goto _LL93;case 12: _LL92: _LL93:
 goto _LL95;case 10: _LL94: _LL95:
 return Cyc_Toseqc_AnyEffect;default: _LL96: _LL97:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp52=_cycalloc(sizeof(*_tmp52));({struct Cyc_Core_Impossible_exn_struct _tmpF1=({struct Cyc_Core_Impossible_exn_struct _tmp53;_tmp53.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmpF0=({const char*_tmp54="bad stmt after xlation to C";_tag_dyneither(_tmp54,sizeof(char),28);});_tmp53.f1=_tmpF0;});_tmp53;});_tmp52[0]=_tmpF1;});_tmp52;}));}_LL79:;}}
# 216
static int Cyc_Toseqc_is_toc_var(struct Cyc_Absyn_Exp*e){
void*_tmp59=e->r;void*_tmp5A=_tmp59;struct _tuple1*_tmp5B;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5A)->tag == 1){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5A)->f1)->tag == 0){_LL99: _tmp5B=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5A)->f1)->f1;_LL9A:
 return 1;}else{goto _LL9B;}}else{_LL9B: _LL9C:
 return 0;}_LL98:;}
# 223
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*s);
# 241 "toseqc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toseqc_exps_to_seqc(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
# 243
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toseqc_exp_to_seqc,es);
# 246
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)<= 1  || Cyc_Toseqc_exps_effect(es)!= Cyc_Toseqc_AnyEffect)return 0;{
# 250
struct Cyc_Absyn_Stmt*stmt=({struct Cyc_Absyn_Exp*_tmpF2=Cyc_Absyn_copy_exp(e);Cyc_Absyn_exp_stmt(_tmpF2,e->loc);});
struct Cyc_Absyn_Stmt*laststmt=stmt;
int did_skip=0;
int did_convert=0;
{struct Cyc_List_List*x=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(es);for(0;x != 0;x=x->tl){
struct Cyc_Absyn_Exp*_tmp5C=(struct Cyc_Absyn_Exp*)x->hd;
# 258
if(Cyc_Tcutil_is_const_exp(_tmp5C) || Cyc_Toseqc_is_toc_var(_tmp5C))continue;
# 263
if(!did_skip){
did_skip=1;
continue;}
# 267
did_convert=1;{
# 270
struct _tuple1*_tmp5D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5E=Cyc_Absyn_var_exp(_tmp5D,_tmp5C->loc);
# 274
struct Cyc_Absyn_Exp*_tmp5F=Cyc_Absyn_new_exp(_tmp5C->r,_tmp5C->loc);
_tmp5F->annot=_tmp5C->annot;
_tmp5F->topt=_tmp5C->topt;{
# 278
void*t=_tmp5F->topt == 0?(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp60=_cycalloc(sizeof(*_tmp60));({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpF3=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp61;_tmp61.tag=27;_tmp61.f1=_tmp5F;_tmp61;});_tmp60[0]=_tmpF3;});_tmp60;}): Cyc_Toc_typ_to_c((void*)_check_null(_tmp5F->topt));
# 282
({struct Cyc_Absyn_Stmt*_tmpF4=Cyc_Absyn_declare_stmt(_tmp5D,t,_tmp5F,stmt,e->loc);stmt=_tmpF4;});
# 285
_tmp5C->r=_tmp5E->r;};};}}
# 289
if(did_convert){
({void*_tmpF5=(Cyc_Absyn_stmt_exp(stmt,e->loc))->r;e->r=_tmpF5;});
return laststmt;}else{
# 293
return 0;}};}
# 299
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*e){
struct _dyneither_ptr bad_form=({const char*_tmp99="";_tag_dyneither(_tmp99,sizeof(char),1);});
{void*_tmp62=e->r;void*_tmp63=_tmp62;struct Cyc_Absyn_Stmt*_tmp92;struct Cyc_List_List*_tmp91;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_Absyn_Exp*_tmp81;struct Cyc_Absyn_Exp*_tmp80;struct Cyc_Absyn_Exp*_tmp7F;struct Cyc_Absyn_Exp*_tmp7E;struct Cyc_Absyn_Exp*_tmp7D;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Exp*_tmp7B;struct Cyc_List_List*_tmp7A;struct Cyc_Absyn_Exp*_tmp79;struct Cyc_List_List*_tmp78;switch(*((int*)_tmp63)){case 0: _LL9E: _LL9F:
 return;case 1: _LLA0: _LLA1:
 return;case 10: _LLA2: _tmp79=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_tmp78=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp63)->f2;_LLA3:
# 305
({struct Cyc_List_List*_tmpF6=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=_tmp79;_tmp64->tl=_tmp78;_tmp64;});_tmp7A=_tmpF6;});goto _LLA5;case 3: _LLA4: _tmp7A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp63)->f2;_LLA5:
# 307
 Cyc_Toseqc_exps_to_seqc(e,_tmp7A);
return;case 23: _LLA6: _tmp7C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_tmp7B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp63)->f2;_LLA7:
# 310
 _tmp7E=_tmp7C;_tmp7D=_tmp7B;goto _LLA9;case 4: _LLA8: _tmp7E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_tmp7D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp63)->f3;_LLA9:
# 313
({struct Cyc_Absyn_Exp*_tmpF7=e;Cyc_Toseqc_exps_to_seqc(_tmpF7,({struct Cyc_Absyn_Exp*_tmp65[2];_tmp65[1]=_tmp7E;_tmp65[0]=_tmp7D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp65,sizeof(struct Cyc_Absyn_Exp*),2));}));});
return;case 6: _LLAA: _tmp81=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_tmp80=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp63)->f2;_tmp7F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp63)->f3;_LLAB:
# 317
 Cyc_Toseqc_exp_to_seqc(_tmp7F);_tmp83=_tmp81;_tmp82=_tmp80;goto _LLAD;case 7: _LLAC: _tmp83=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_tmp82=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp63)->f2;_LLAD:
 _tmp85=_tmp83;_tmp84=_tmp82;goto _LLAF;case 8: _LLAE: _tmp85=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_tmp84=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp63)->f2;_LLAF:
 _tmp87=_tmp85;_tmp86=_tmp84;goto _LLB1;case 9: _LLB0: _tmp87=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_tmp86=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp63)->f2;_LLB1:
# 321
 Cyc_Toseqc_exp_to_seqc(_tmp86);_tmp88=_tmp87;goto _LLB3;case 12: _LLB2: _tmp88=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_LLB3:
# 323
 _tmp89=_tmp88;goto _LLB5;case 13: _LLB4: _tmp89=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_LLB5:
 _tmp8A=_tmp89;goto _LLB7;case 14: _LLB6: _tmp8A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp63)->f2;_LLB7:
 _tmp8B=_tmp8A;goto _LLB9;case 15: _LLB8: _tmp8B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_LLB9:
 _tmp8C=_tmp8B;goto _LLBB;case 18: _LLBA: _tmp8C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_LLBB:
 _tmp8D=_tmp8C;goto _LLBD;case 20: _LLBC: _tmp8D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_LLBD:
 _tmp8E=_tmp8D;goto _LLBF;case 21: _LLBE: _tmp8E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_LLBF:
 _tmp8F=_tmp8E;goto _LLC1;case 22: _LLC0: _tmp8F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_LLC1:
 _tmp90=_tmp8F;goto _LLC3;case 5: _LLC2: _tmp90=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_LLC3:
# 332
 Cyc_Toseqc_exp_to_seqc(_tmp90);
return;case 17: _LLC4: _LLC5:
# 335
 goto _LLC7;case 19: _LLC6: _LLC7:
 goto _LLC9;case 33: _LLC8: _LLC9:
 goto _LLCB;case 32: _LLCA: _LLCB:
 return;case 36: _LLCC: _tmp91=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp63)->f2;_LLCD: {
# 341
struct _tuple0 _tmp66=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp91);struct _tuple0 _tmp67=_tmp66;struct Cyc_List_List*_tmp68;_LLF1: _tmp68=_tmp67.f2;_LLF2:;
Cyc_Toseqc_exps_to_seqc(e,_tmp68);
return;}case 37: _LLCE: _tmp92=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp63)->f1;_LLCF:
# 346
 Cyc_Toseqc_stmt_to_seqc(_tmp92);
return;case 40: _LLD0: _LLD1:
# 349
 return;case 2: _LLD2: _LLD3:
# 351
({struct _dyneither_ptr _tmpF8=({const char*_tmp69="Pragma_e";_tag_dyneither(_tmp69,sizeof(char),9);});bad_form=_tmpF8;});goto _LL9D;case 11: _LLD4: _LLD5:
({struct _dyneither_ptr _tmpF9=({const char*_tmp6A="Throw_e";_tag_dyneither(_tmp6A,sizeof(char),8);});bad_form=_tmpF9;});goto _LL9D;case 16: _LLD6: _LLD7:
({struct _dyneither_ptr _tmpFA=({const char*_tmp6B="New_e";_tag_dyneither(_tmp6B,sizeof(char),6);});bad_form=_tmpFA;});goto _LL9D;case 24: _LLD8: _LLD9:
({struct _dyneither_ptr _tmpFB=({const char*_tmp6C="Tuple_e";_tag_dyneither(_tmp6C,sizeof(char),8);});bad_form=_tmpFB;});goto _LL9D;case 25: _LLDA: _LLDB:
({struct _dyneither_ptr _tmpFC=({const char*_tmp6D="CompoundLit_e";_tag_dyneither(_tmp6D,sizeof(char),14);});bad_form=_tmpFC;});goto _LL9D;case 26: _LLDC: _LLDD:
({struct _dyneither_ptr _tmpFD=({const char*_tmp6E="Array_e";_tag_dyneither(_tmp6E,sizeof(char),8);});bad_form=_tmpFD;});goto _LL9D;case 27: _LLDE: _LLDF:
({struct _dyneither_ptr _tmpFE=({const char*_tmp6F="Comprehension_e";_tag_dyneither(_tmp6F,sizeof(char),16);});bad_form=_tmpFE;});goto _LL9D;case 28: _LLE0: _LLE1:
({struct _dyneither_ptr _tmpFF=({const char*_tmp70="ComprehensionNoinit_e";_tag_dyneither(_tmp70,sizeof(char),22);});bad_form=_tmpFF;});goto _LL9D;case 29: _LLE2: _LLE3:
({struct _dyneither_ptr _tmp100=({const char*_tmp71="Aggregate_e";_tag_dyneither(_tmp71,sizeof(char),12);});bad_form=_tmp100;});goto _LL9D;case 30: _LLE4: _LLE5:
({struct _dyneither_ptr _tmp101=({const char*_tmp72="AnonStruct_e";_tag_dyneither(_tmp72,sizeof(char),13);});bad_form=_tmp101;});goto _LL9D;case 31: _LLE6: _LLE7:
# 362
({struct _dyneither_ptr _tmp102=({const char*_tmp73="Datatype_e";_tag_dyneither(_tmp73,sizeof(char),11);});bad_form=_tmp102;});goto _LL9D;case 34: _LLE8: _LLE9:
({struct _dyneither_ptr _tmp103=({const char*_tmp74="Malloc_e";_tag_dyneither(_tmp74,sizeof(char),9);});bad_form=_tmp103;});goto _LL9D;case 35: _LLEA: _LLEB:
({struct _dyneither_ptr _tmp104=({const char*_tmp75="Swap_e";_tag_dyneither(_tmp75,sizeof(char),7);});bad_form=_tmp104;});goto _LL9D;case 38: _LLEC: _LLED:
({struct _dyneither_ptr _tmp105=({const char*_tmp76="Tagcheck_e";_tag_dyneither(_tmp76,sizeof(char),11);});bad_form=_tmp105;});goto _LL9D;default: _LLEE: _LLEF:
({struct _dyneither_ptr _tmp106=({const char*_tmp77="Valueof_e";_tag_dyneither(_tmp77,sizeof(char),10);});bad_form=_tmp106;});goto _LL9D;}_LL9D:;}
# 368
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp93=_cycalloc(sizeof(*_tmp93));({struct Cyc_Core_Impossible_exn_struct _tmp10A=({struct Cyc_Core_Impossible_exn_struct _tmp94;_tmp94.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp109=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp98;_tmp98.tag=0;({struct _dyneither_ptr _tmp107=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp98.f1=_tmp107;});({struct Cyc_String_pa_PrintArg_struct _tmp97;_tmp97.tag=0;_tmp97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)bad_form);({void*_tmp95[2]={& _tmp97,& _tmp98};({struct _dyneither_ptr _tmp108=({const char*_tmp96="bad exp form %s (exp=|%s|) after xlation to C";_tag_dyneither(_tmp96,sizeof(char),46);});Cyc_aprintf(_tmp108,_tag_dyneither(_tmp95,sizeof(void*),2));});});});});_tmp94.f1=_tmp109;});_tmp94;});_tmp93[0]=_tmp10A;});_tmp93;}));}
# 372
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*s){
# 374
while(1){
void*_tmp9A=s->r;void*_tmp9B=_tmp9A;struct Cyc_Absyn_Decl*_tmpCC;struct Cyc_Absyn_Stmt*_tmpCB;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_List_List*_tmpC9;struct Cyc_Absyn_Stmt*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Stmt*_tmpC3;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Stmt*_tmpC1;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Absyn_Stmt*_tmpBF;struct Cyc_Absyn_Stmt*_tmpBE;struct Cyc_Absyn_Stmt*_tmpBD;struct Cyc_Absyn_Stmt*_tmpBC;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Stmt*_tmpB9;switch(*((int*)_tmp9B)){case 0: _LLF4: _LLF5:
 goto _LLF7;case 6: _LLF6: _LLF7:
 goto _LLF9;case 7: _LLF8: _LLF9:
 goto _LLFB;case 8: _LLFA: _LLFB:
 return;case 13: _LLFC: _tmpB9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9B)->f2;_LLFD:
# 381
 s=_tmpB9;
continue;case 3: _LLFE: _tmpBA=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9B)->f1;_LLFF:
# 384
 if(_tmpBA == 0)
return;
_tmpBB=_tmpBA;goto _LL101;case 1: _LL100: _tmpBB=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9B)->f1;_LL101:
# 388
 Cyc_Toseqc_exp_to_seqc(_tmpBB);
# 396
return;case 2: _LL102: _tmpBD=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9B)->f1;_tmpBC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9B)->f2;_LL103:
# 398
 Cyc_Toseqc_stmt_to_seqc(_tmpBD);
s=_tmpBC;
continue;case 4: _LL104: _tmpC0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9B)->f1;_tmpBF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9B)->f2;_tmpBE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9B)->f3;_LL105:
# 402
 Cyc_Toseqc_exp_to_seqc(_tmpC0);
Cyc_Toseqc_stmt_to_seqc(_tmpBF);
s=_tmpBE;
continue;case 5: _LL106: _tmpC2=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9B)->f1).f1;_tmpC1=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9B)->f2;_LL107:
# 407
 Cyc_Toseqc_exp_to_seqc(_tmpC2);
s=_tmpC1;
continue;case 9: _LL108: _tmpC6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9B)->f1;_tmpC5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9B)->f2).f1;_tmpC4=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9B)->f3).f1;_tmpC3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9B)->f4;_LL109:
# 411
 Cyc_Toseqc_exp_to_seqc(_tmpC6);
Cyc_Toseqc_exp_to_seqc(_tmpC5);
Cyc_Toseqc_exp_to_seqc(_tmpC4);
s=_tmpC3;
continue;case 14: _LL10A: _tmpC8=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9B)->f1;_tmpC7=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9B)->f2).f1;_LL10B:
# 417
 Cyc_Toseqc_exp_to_seqc(_tmpC7);
s=_tmpC8;
continue;case 10: _LL10C: _tmpCA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9B)->f1;_tmpC9=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9B)->f2;_LL10D:
# 423
 Cyc_Toseqc_exp_to_seqc(_tmpCA);
for(0;_tmpC9 != 0;_tmpC9=_tmpC9->tl){
Cyc_Toseqc_stmt_to_seqc(((struct Cyc_Absyn_Switch_clause*)_tmpC9->hd)->body);}
return;case 12: _LL10E: _tmpCC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9B)->f1;_tmpCB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9B)->f2;_LL10F:
# 428
{void*_tmp9C=_tmpCC->r;void*_tmp9D=_tmp9C;struct Cyc_Absyn_Vardecl*_tmpB5;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9D)->tag == 0){_LL113: _tmpB5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9D)->f1;_LL114:
# 430
 if(_tmpB5->initializer != 0){
void*_tmp9E=((struct Cyc_Absyn_Exp*)_check_null(_tmpB5->initializer))->r;void*_tmp9F=_tmp9E;struct Cyc_List_List*_tmpB4;if(((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp9F)->tag == 36){_LL118: _tmpB4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp9F)->f2;_LL119: {
# 444 "toseqc.cyc"
struct _tuple0 _tmpA0=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpB4);struct _tuple0 _tmpA1=_tmpA0;struct Cyc_List_List*_tmpB3;_LL11D: _tmpB3=_tmpA1.f2;_LL11E:;{
struct Cyc_Absyn_Stmt*_tmpA2=Cyc_Toseqc_exps_to_seqc((struct Cyc_Absyn_Exp*)_check_null(_tmpB5->initializer),_tmpB3);
# 447
if(_tmpA2 == 0)goto _LL117;
# 453
{void*_tmpA3=_tmpA2->r;void*_tmpA4=_tmpA3;struct Cyc_Absyn_Exp*_tmpB2;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA4)->tag == 1){_LL120: _tmpB2=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA4)->f1;_LL121:
# 455
{void*_tmpA5=((struct Cyc_Absyn_Exp*)_check_null(_tmpB5->initializer))->r;void*_tmpA6=_tmpA5;struct Cyc_Absyn_Stmt*_tmpAC;if(((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA6)->tag == 37){_LL125: _tmpAC=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA6)->f1;_LL126:
# 457
 _tmpB5->initializer=_tmpB2;
_tmpA2->r=s->r;
s->r=_tmpAC->r;
goto _LL124;}else{_LL127: _LL128:
# 462
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));({struct Cyc_Core_Impossible_exn_struct _tmp10E=({struct Cyc_Core_Impossible_exn_struct _tmpA8;_tmpA8.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp10D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAB;_tmpAB.tag=0;({struct _dyneither_ptr _tmp10B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmpB5->initializer)));_tmpAB.f1=_tmp10B;});({void*_tmpA9[1]={& _tmpAB};({struct _dyneither_ptr _tmp10C=({const char*_tmpAA="exps_to_seqc updated to non-stmt-exp |%s|";_tag_dyneither(_tmpAA,sizeof(char),42);});Cyc_aprintf(_tmp10C,_tag_dyneither(_tmpA9,sizeof(void*),1));});});});_tmpA8.f1=_tmp10D;});_tmpA8;});_tmpA7[0]=_tmp10E;});_tmpA7;}));}_LL124:;}
# 464
goto _LL11F;}else{_LL122: _LL123:
# 466
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct Cyc_Core_Impossible_exn_struct _tmp112=({struct Cyc_Core_Impossible_exn_struct _tmpAE;_tmpAE.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp111=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpB1;_tmpB1.tag=0;({struct _dyneither_ptr _tmp10F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmpA2));_tmpB1.f1=_tmp10F;});({void*_tmpAF[1]={& _tmpB1};({struct _dyneither_ptr _tmp110=({const char*_tmpB0="exps_to_seqc returned non-exp-stmt |%s|";_tag_dyneither(_tmpB0,sizeof(char),40);});Cyc_aprintf(_tmp110,_tag_dyneither(_tmpAF,sizeof(void*),1));});});});_tmpAE.f1=_tmp111;});_tmpAE;});_tmpAD[0]=_tmp112;});_tmpAD;}));}_LL11F:;}
# 468
goto _LL117;};}}else{_LL11A: _LL11B:
# 471
 Cyc_Toseqc_exp_to_seqc((struct Cyc_Absyn_Exp*)_check_null(_tmpB5->initializer));
goto _LL117;}_LL117:;}
# 475
s=_tmpCB;
continue;}else{_LL115: _LL116:
 goto _LL112;}_LL112:;}
# 479
goto _LL111;default: _LL110: _LL111:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct Cyc_Core_Impossible_exn_struct _tmp114=({struct Cyc_Core_Impossible_exn_struct _tmpB7;_tmpB7.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp113=({const char*_tmpB8="bad stmt after xlation to C";_tag_dyneither(_tmpB8,sizeof(char),28);});_tmpB7.f1=_tmp113;});_tmpB7;});_tmpB6[0]=_tmp114;});_tmpB6;}));}_LLF3:;}}
# 485
struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmpCD=ds;
# 488
for(0;_tmpCD != 0;_tmpCD=_tmpCD->tl){
struct Cyc_Absyn_Decl*_tmpCE=(struct Cyc_Absyn_Decl*)_tmpCD->hd;
void*_tmpCF=_tmpCE->r;void*_tmpD0=_tmpCF;struct Cyc_List_List*_tmpD8;struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD6;struct Cyc_List_List*_tmpD5;struct Cyc_Absyn_Fndecl*_tmpD4;switch(*((int*)_tmpD0)){case 1: _LL12A: _tmpD4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpD0)->f1;_LL12B:
# 492
 Cyc_Toseqc_stmt_to_seqc(_tmpD4->body);
goto _LL129;case 0: _LL12C: _LL12D:
 goto _LL12F;case 2: _LL12E: _LL12F:
 goto _LL131;case 3: _LL130: _LL131:
 goto _LL133;case 4: _LL132: _LL133:
 goto _LL135;case 5: _LL134: _LL135:
 goto _LL137;case 6: _LL136: _LL137:
 goto _LL139;case 7: _LL138: _LL139:
 goto _LL13B;case 8: _LL13A: _LL13B:
# 502
 goto _LL129;case 9: _LL13C: _tmpD5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD0)->f2;_LL13D:
 _tmpD6=_tmpD5;goto _LL13F;case 10: _LL13E: _tmpD6=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpD0)->f2;_LL13F:
 _tmpD7=_tmpD6;goto _LL141;case 11: _LL140: _tmpD7=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpD0)->f1;_LL141:
 _tmpD8=_tmpD7;goto _LL143;case 12: _LL142: _tmpD8=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpD0)->f1;_LL143:
 goto _LL145;case 13: _LL144: _LL145:
 goto _LL147;default: _LL146: _LL147:
# 509
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));({struct Cyc_Core_Impossible_exn_struct _tmp116=({struct Cyc_Core_Impossible_exn_struct _tmpD2;_tmpD2.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp115=({const char*_tmpD3="nested translation unit after translation to C";_tag_dyneither(_tmpD3,sizeof(char),47);});_tmpD2.f1=_tmp115;});_tmpD2;});_tmpD1[0]=_tmp116;});_tmpD1;}));}_LL129:;}
# 512
return ds;}
