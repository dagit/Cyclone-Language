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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
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
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 281 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 221
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 916 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 924
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 962
extern struct _tuple0*Cyc_Absyn_exn_name;
# 987
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
# 1078
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1175
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_RgnOrder_RgnPO;
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 34 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 172
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 31 "toc.h"
typedef struct Cyc_Hashtable_Table*Cyc_Toc_table_t;
# 38
void*Cyc_Toc_typ_to_c(void*t);
void Cyc_Toc_finish();
# 42
struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);
# 75 "tovc.cyc"
int Cyc_Tovc_elim_array_initializers=1;struct Cyc_Tovc_BoxingEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};
# 88
typedef struct Cyc_Tovc_BoxingEnv Cyc_Tovc_boxingenv_t;struct Cyc_Tovc_ToExpEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};
# 97
typedef struct Cyc_Tovc_ToExpEnv Cyc_Tovc_toexpenv_t;struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 103
static struct Cyc_Absyn_Exp*Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_List_List*_tmp47;struct Cyc_Absyn_Exp**_tmp46;struct Cyc_Absyn_Exp**_tmp45;struct Cyc_Absyn_Exp**_tmp44;struct Cyc_Absyn_Exp**_tmp43;struct Cyc_Absyn_Exp**_tmp42;struct Cyc_Absyn_Exp**_tmp41;struct Cyc_Absyn_Exp**_tmp40;struct Cyc_Absyn_Exp**_tmp3F;struct Cyc_Absyn_Exp**_tmp3E;struct Cyc_Absyn_Exp**_tmp3D;struct Cyc_Absyn_Exp**_tmp3C;struct Cyc_Absyn_Exp**_tmp3B;struct Cyc_Absyn_Exp**_tmp3A;struct Cyc_Absyn_Exp**_tmp39;struct Cyc_Absyn_Exp**_tmp38;struct Cyc_Absyn_Exp**_tmp37;struct Cyc_List_List*_tmp36;struct Cyc_Absyn_Exp**_tmp35;struct Cyc_List_List*_tmp34;struct Cyc_Absyn_Exp*_tmp33;struct _dyneither_ptr*_tmp32;int _tmp31;int _tmp30;struct Cyc_Absyn_Exp*_tmp2F;struct _dyneither_ptr*_tmp2E;int _tmp2D;int _tmp2C;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp28;void*_tmp27;switch(*((int*)_tmp1)){case 1U: _LL1: _tmp27=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2: {
# 107
struct _tuple0*_tmp2=Cyc_Absyn_binding2qvar(_tmp27);struct _tuple0*_tmp3=_tmp2;struct _dyneither_ptr*_tmpC;if(((((struct _tuple0*)_tmp3)->f1).Loc_n).tag == 4U){_LL32: _tmpC=_tmp3->f2;_LL33:
# 111
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(env.varmap,_tmpC)){
({void*_tmp13C=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(env.varmap,_tmpC))->r;e->r=_tmp13C;});
continue;}
# 116
{int i=0;for(0;i < _get_dyneither_size(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*));++ i){
if(Cyc_strptrcmp(_tmpC,*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*),i)))== 0)
return 0;}}
# 120
for(0;env.boundvars != 0;env.boundvars=(env.boundvars)->tl){
if(Cyc_strptrcmp(_tmpC,(*((struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)== 0)
return 0;}
# 124
for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
struct Cyc_Absyn_Vardecl*_tmp4=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;
if(Cyc_strptrcmp(_tmpC,(*_tmp4->name).f2)== 0){
({struct Cyc_Set_Set*_tmp13D=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Absyn_Vardecl*elt))Cyc_Set_rinsert)(env.rgn,*env.freevars,_tmp4);*env.freevars=_tmp13D;});{
# 129
void*_tmp5=_tmp4->type;void*_tmp6=_tmp5;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6)->tag == 8U){_LL37: _LL38:
 return 0;}else{_LL39: _LL3A:
 return Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp4->name,0U),0U);}_LL36:;};}}
# 138
for(0;env.all_locals != 0;env.all_locals=((struct Cyc_List_List*)_check_null(env.all_locals))->tl){
if(Cyc_strptrcmp(_tmpC,(*((struct _tuple0*)_check_null(((struct Cyc_Absyn_Vardecl*)_check_null((struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(env.all_locals))->hd))->name))).f2)== 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp7=_cycalloc(sizeof(*_tmp7));({struct Cyc_Core_Impossible_exn_struct _tmp140=({struct Cyc_Core_Impossible_exn_struct _tmp8;_tmp8.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp13F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpB;_tmpB.tag=0U;_tmpB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(_tmpC)));({void*_tmp9[1U]={& _tmpB};({struct _dyneither_ptr _tmp13E=({const char*_tmpA="unbound variable %s found in box_free_vars_exp";_tag_dyneither(_tmpA,sizeof(char),47U);});Cyc_aprintf(_tmp13E,_tag_dyneither(_tmp9,sizeof(void*),1U));});});});_tmp8.f1=_tmp13F;});_tmp8;});_tmp7[0]=_tmp140;});_tmp7;}));}
# 145
return 0;}else{_LL34: _LL35:
 return 0;}_LL31:;}case 23U: _LL3: _tmp29=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp28=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL4: {
# 152
struct Cyc_Absyn_Exp*_tmpD=Cyc_Tovc_box_free_vars_exp(env,_tmp29);
struct Cyc_Absyn_Exp*_tmpE=Cyc_Tovc_box_free_vars_exp(env,_tmp28);
if(_tmpD != 0){
if(_tmpE != 0){
struct Cyc_Absyn_Exp*_tmpF=Cyc_Absyn_subscript_exp(_tmpD,_tmpE,e->loc);
_tmpF->topt=e->topt;
_tmpF->annot=e->annot;
return _tmpF;}else{
# 162
struct Cyc_Absyn_Exp*_tmp10=Cyc_Absyn_subscript_exp(_tmpD,_tmp28,e->loc);
_tmp10->topt=e->topt;
_tmp10->annot=e->annot;
return _tmp10;}}else{
# 168
if(_tmpE != 0){
struct Cyc_Absyn_Exp*_tmp11=Cyc_Absyn_subscript_exp(_tmp29,_tmpE,e->loc);
_tmp11->topt=e->topt;
_tmp11->annot=e->annot;
return _tmp11;}}
# 174
return 0;}case 15U: _LL5: _tmp2A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL6: {
# 177
struct Cyc_Absyn_Exp*_tmp12=Cyc_Tovc_box_free_vars_exp(env,_tmp2A);
if(_tmp12 != 0){
struct Cyc_Absyn_Exp*_tmp13=Cyc_Absyn_address_exp(_tmp12,e->loc);
_tmp13->topt=e->topt;
_tmp13->annot=e->annot;
return _tmp13;}
# 184
return 0;}case 20U: _LL7: _tmp2B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL8: {
# 187
struct Cyc_Absyn_Exp*_tmp14=Cyc_Tovc_box_free_vars_exp(env,_tmp2B);
if(_tmp14 != 0){
struct Cyc_Absyn_Exp*_tmp15=Cyc_Absyn_deref_exp(_tmp14,e->loc);
_tmp15->topt=e->topt;
_tmp15->annot=e->annot;
return _tmp15;}
# 194
return 0;}case 21U: _LL9: _tmp2F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp2E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp2D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_tmp2C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f4;_LLA: {
# 197
struct Cyc_Absyn_Exp*_tmp16=Cyc_Tovc_box_free_vars_exp(env,_tmp2F);
if(_tmp16 != 0){
struct Cyc_Absyn_Exp*_tmp17=({void*_tmp142=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp18=_cycalloc(sizeof(*_tmp18));({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp141=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp19;_tmp19.tag=21U;_tmp19.f1=_tmp16;_tmp19.f2=_tmp2E;_tmp19.f3=_tmp2D;_tmp19.f4=_tmp2C;_tmp19;});_tmp18[0]=_tmp141;});_tmp18;});Cyc_Absyn_new_exp(_tmp142,e->loc);});
_tmp17->topt=e->topt;
_tmp17->annot=e->annot;
return _tmp17;}
# 204
return 0;}case 22U: _LLB: _tmp33=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp32=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp31=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_tmp30=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f4;_LLC: {
# 207
struct Cyc_Absyn_Exp*_tmp1A=Cyc_Tovc_box_free_vars_exp(env,_tmp33);
if(_tmp1A != 0){
struct Cyc_Absyn_Exp*_tmp1B=({void*_tmp144=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp143=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp1D;_tmp1D.tag=22U;_tmp1D.f1=_tmp1A;_tmp1D.f2=_tmp32;_tmp1D.f3=_tmp31;_tmp1D.f4=_tmp30;_tmp1D;});_tmp1C[0]=_tmp143;});_tmp1C;});Cyc_Absyn_new_exp(_tmp144,e->loc);});
_tmp1B->topt=e->topt;
_tmp1B->annot=e->annot;
return _tmp1B;}
# 214
return 0;}case 0U: _LLD: _LLE:
# 217
 return 0;case 10U: _LLF: _tmp35=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp34=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL10:
# 219
{struct Cyc_Absyn_Exp*_tmp1E=Cyc_Tovc_box_free_vars_exp(env,*_tmp35);if(_tmp1E != 0)*_tmp35=_tmp1E;}
_tmp36=_tmp34;goto _LL12;case 3U: _LL11: _tmp36=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL12:
# 222
 for(0;_tmp36 != 0;_tmp36=_tmp36->tl){
struct Cyc_Absyn_Exp*_tmp1F=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_tmp36->hd);if(_tmp1F != 0)_tmp36->hd=(void*)_tmp1F;}
# 225
return 0;case 6U: _LL13: _tmp39=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp38=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp37=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL14:
# 228
{struct Cyc_Absyn_Exp*_tmp20=Cyc_Tovc_box_free_vars_exp(env,*_tmp37);if(_tmp20 != 0)*_tmp37=_tmp20;}
_tmp3B=_tmp39;_tmp3A=_tmp38;goto _LL16;case 7U: _LL15: _tmp3B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3A=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL16:
 _tmp3D=_tmp3B;_tmp3C=_tmp3A;goto _LL18;case 8U: _LL17: _tmp3D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3C=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL18:
 _tmp3F=_tmp3D;_tmp3E=_tmp3C;goto _LL1A;case 9U: _LL19: _tmp3F=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3E=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL1A:
 _tmp41=_tmp3F;_tmp40=_tmp3E;goto _LL1C;case 4U: _LL1B: _tmp41=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp40=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL1C:
# 234
{struct Cyc_Absyn_Exp*_tmp21=Cyc_Tovc_box_free_vars_exp(env,*_tmp40);if(_tmp21 != 0)*_tmp40=_tmp21;}
_tmp42=_tmp41;goto _LL1E;case 12U: _LL1D: _tmp42=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL1E:
# 237
 _tmp43=_tmp42;goto _LL20;case 13U: _LL1F: _tmp43=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL20:
 _tmp44=_tmp43;goto _LL22;case 14U: _LL21: _tmp44=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL22:
 _tmp45=_tmp44;goto _LL24;case 18U: _LL23: _tmp45=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL24:
 _tmp46=_tmp45;goto _LL26;case 5U: _LL25: _tmp46=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL26:
# 242
{struct Cyc_Absyn_Exp*_tmp22=Cyc_Tovc_box_free_vars_exp(env,*_tmp46);if(_tmp22 != 0)*_tmp46=_tmp22;}
return 0;case 17U: _LL27: _LL28:
# 245
 goto _LL2A;case 19U: _LL29: _LL2A:
 goto _LL2C;case 32U: _LL2B: _LL2C:
 return 0;case 36U: _LL2D: _tmp47=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL2E:
# 250
 for(0;_tmp47 != 0;_tmp47=_tmp47->tl){
struct Cyc_Absyn_Exp*_tmp23=Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple10*)_tmp47->hd)).f2);if(_tmp23 != 0)(*((struct _tuple10*)_tmp47->hd)).f2=_tmp23;}
# 253
return 0;default: _LL2F: _LL30:
# 255
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp24=_cycalloc(sizeof(*_tmp24));({struct Cyc_Core_Impossible_exn_struct _tmp146=({struct Cyc_Core_Impossible_exn_struct _tmp25;_tmp25.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp145=({const char*_tmp26="bad exp form in Tocv::box_free_vars_exp";_tag_dyneither(_tmp26,sizeof(char),40U);});_tmp25.f1=_tmp145;});_tmp25;});_tmp24[0]=_tmp146;});_tmp24;}));}_LL0:;}}
# 259
static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){
# 263
while(1){
void*_tmp48=s->r;void*_tmp49=_tmp48;struct Cyc_Absyn_Decl*_tmp76;struct Cyc_Absyn_Stmt*_tmp75;struct Cyc_Absyn_Exp**_tmp74;struct Cyc_List_List*_tmp73;struct Cyc_Absyn_Stmt*_tmp72;struct Cyc_Absyn_Exp**_tmp71;struct Cyc_Absyn_Exp**_tmp70;struct Cyc_Absyn_Exp**_tmp6F;struct Cyc_Absyn_Exp**_tmp6E;struct Cyc_Absyn_Stmt*_tmp6D;struct Cyc_Absyn_Exp**_tmp6C;struct Cyc_Absyn_Stmt*_tmp6B;struct Cyc_Absyn_Exp**_tmp6A;struct Cyc_Absyn_Stmt*_tmp69;struct Cyc_Absyn_Stmt*_tmp68;struct Cyc_Absyn_Stmt*_tmp67;struct Cyc_Absyn_Stmt*_tmp66;struct Cyc_Absyn_Exp**_tmp65;struct Cyc_Absyn_Exp**_tmp64;struct Cyc_Absyn_Stmt*_tmp63;switch(*((int*)_tmp49)){case 0U: _LL3C: _LL3D:
 return;case 6U: _LL3E: _LL3F:
# 267
({void*_tmp4A=0U;({unsigned int _tmp148=s->loc;struct _dyneither_ptr _tmp147=({const char*_tmp4B="break in eliminated statement expression";_tag_dyneither(_tmp4B,sizeof(char),41U);});Cyc_Tcutil_warn(_tmp148,_tmp147,_tag_dyneither(_tmp4A,sizeof(void*),0U));});});
return;case 7U: _LL40: _LL41:
# 270
({void*_tmp4C=0U;({unsigned int _tmp14A=s->loc;struct _dyneither_ptr _tmp149=({const char*_tmp4D="continue in eliminated statement expression";_tag_dyneither(_tmp4D,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp14A,_tmp149,_tag_dyneither(_tmp4C,sizeof(void*),0U));});});
# 273
return;case 8U: _LL42: _LL43:
# 275
({void*_tmp4E=0U;({unsigned int _tmp14C=s->loc;struct _dyneither_ptr _tmp14B=({const char*_tmp4F="goto in eliminated statement expression";_tag_dyneither(_tmp4F,sizeof(char),40U);});Cyc_Tcutil_warn(_tmp14C,_tmp14B,_tag_dyneither(_tmp4E,sizeof(void*),0U));});});
return;case 13U: _LL44: _tmp63=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_LL45:
# 278
 s=_tmp63;
continue;case 3U: _LL46: _tmp64=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_LL47:
# 281
({void*_tmp50=0U;({unsigned int _tmp14E=s->loc;struct _dyneither_ptr _tmp14D=({const char*_tmp51="return in eliminated statement expression";_tag_dyneither(_tmp51,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp14E,_tmp14D,_tag_dyneither(_tmp50,sizeof(void*),0U));});});
# 283
if(*_tmp64 != 0){
struct Cyc_Absyn_Exp*_tmp52=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(*_tmp64));
if(_tmp52 != 0)*_tmp64=_tmp52;}
# 287
return;case 1U: _LL48: _tmp65=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_LL49:
# 289
{struct Cyc_Absyn_Exp*_tmp53=Cyc_Tovc_box_free_vars_exp(env,*_tmp65);if(_tmp53 != 0)*_tmp65=_tmp53;}
return;case 2U: _LL4A: _tmp67=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp66=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_LL4B:
# 292
 Cyc_Tovc_box_free_vars_stmt(env,_tmp67);
s=_tmp66;
continue;case 4U: _LL4C: _tmp6A=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp69=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_tmp68=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp49)->f3;_LL4D:
# 296
{struct Cyc_Absyn_Exp*_tmp54=Cyc_Tovc_box_free_vars_exp(env,*_tmp6A);if(_tmp54 != 0)*_tmp6A=_tmp54;}
Cyc_Tovc_box_free_vars_stmt(env,_tmp69);
s=_tmp68;
continue;case 5U: _LL4E: _tmp6C=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp49)->f1).f1;_tmp6B=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_LL4F:
# 301
{struct Cyc_Absyn_Exp*_tmp55=Cyc_Tovc_box_free_vars_exp(env,*_tmp6C);if(_tmp55 != 0)*_tmp6C=_tmp55;}
s=_tmp6B;
continue;case 9U: _LL50: _tmp70=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp6F=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp49)->f2).f1;_tmp6E=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp49)->f3).f1;_tmp6D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp49)->f4;_LL51:
# 305
{struct Cyc_Absyn_Exp*_tmp56=Cyc_Tovc_box_free_vars_exp(env,*_tmp70);if(_tmp56 != 0)*_tmp70=_tmp56;}
{struct Cyc_Absyn_Exp*_tmp57=Cyc_Tovc_box_free_vars_exp(env,*_tmp6F);if(_tmp57 != 0)*_tmp6F=_tmp57;}
{struct Cyc_Absyn_Exp*_tmp58=Cyc_Tovc_box_free_vars_exp(env,*_tmp6E);if(_tmp58 != 0)*_tmp6E=_tmp58;}
s=_tmp6D;
continue;case 14U: _LL52: _tmp72=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp71=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp49)->f2).f1;_LL53:
# 311
{struct Cyc_Absyn_Exp*_tmp59=Cyc_Tovc_box_free_vars_exp(env,*_tmp71);if(_tmp59 != 0)*_tmp71=_tmp59;}
s=_tmp72;
continue;case 10U: _LL54: _tmp74=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp73=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_LL55:
# 316
{struct Cyc_Absyn_Exp*_tmp5A=Cyc_Tovc_box_free_vars_exp(env,*_tmp74);if(_tmp5A != 0)*_tmp74=_tmp5A;}
for(0;_tmp73 != 0;_tmp73=_tmp73->tl){
Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp73->hd)->body);}
return;case 12U: _LL56: _tmp76=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp75=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_LL57:
# 321
{void*_tmp5B=_tmp76->r;void*_tmp5C=_tmp5B;struct Cyc_Absyn_Vardecl*_tmp5F;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5C)->tag == 0U){_LL5B: _tmp5F=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5C)->f1;_LL5C:
# 323
({struct Cyc_List_List*_tmp14F=({struct Cyc_List_List*_tmp5D=_region_malloc(env.rgn,sizeof(*_tmp5D));_tmp5D->hd=_tmp5F;_tmp5D->tl=env.boundvars;_tmp5D;});env.boundvars=_tmp14F;});
({struct Cyc_Dict_Dict _tmp150=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete_same)(env.varmap,(*_tmp5F->name).f2);env.varmap=_tmp150;});
if(_tmp5F->initializer != 0){
struct Cyc_Absyn_Exp*_tmp5E=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp5F->initializer));
if(_tmp5E != 0)_tmp5F->initializer=_tmp5E;}
# 329
s=_tmp75;
continue;}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}
# 333
goto _LL59;default: _LL58: _LL59:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp60=_cycalloc(sizeof(*_tmp60));({struct Cyc_Core_Impossible_exn_struct _tmp152=({struct Cyc_Core_Impossible_exn_struct _tmp61;_tmp61.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp151=({const char*_tmp62="bad stmt after xlation to C";_tag_dyneither(_tmp62,sizeof(char),28U);});_tmp61.f1=_tmp151;});_tmp61;});_tmp60[0]=_tmp152;});_tmp60;}));}_LL3B:;}}
# 344
static int Cyc_Tovc_stmt_to_fun_cmp(struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){
return Cyc_Absyn_qvar_cmp(x->name,y->name);}
# 348
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){
# 351
struct Cyc_Set_Set*freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty)(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
struct Cyc_Tovc_BoxingEnv _tmp77=({struct Cyc_Tovc_BoxingEnv _tmp87;_tmp87.rgn=env.rgn;_tmp87.all_locals=env.all_locals;_tmp87.varmap=env.varmap;_tmp87.boundvars=0;_tmp87.freevars=& freevars;_tmp87;});
Cyc_Tovc_box_free_vars_stmt(_tmp77,s);{
struct Cyc_List_List*params=0;
struct Cyc_List_List*args=0;
# 357
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,freevars);
# 359
struct Cyc_Absyn_Vardecl*_tmp78=({struct Cyc_Absyn_Vardecl*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->sc=Cyc_Absyn_Public;_tmp86->name=Cyc_Absyn_exn_name;_tmp86->varloc=0U;({struct Cyc_Absyn_Tqual _tmp153=Cyc_Absyn_empty_tqual(0U);_tmp86->tq=_tmp153;});_tmp86->type=(void*)& Cyc_Absyn_VoidType_val;_tmp86->initializer=0;_tmp86->rgn=0;_tmp86->attributes=0;_tmp86->escapes=0;_tmp86;});
# 362
while(((int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(iter,& _tmp78)){
void*_tmp79=_tmp78->type;void*_tmp7A=_tmp79;void*_tmp81;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7A)->tag == 8U){_LL60: _tmp81=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7A)->f1).elt_type;_LL61:
# 365
({struct Cyc_List_List*_tmp157=({struct Cyc_List_List*_tmp7B=_cycalloc(sizeof(*_tmp7B));({struct _tuple8*_tmp156=({struct _tuple8*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=(*_tmp78->name).f2;_tmp7C->f2=_tmp78->tq;({void*_tmp155=({
void*_tmp154=_tmp81;Cyc_Absyn_cstar_typ(_tmp154,Cyc_Absyn_empty_tqual(0U));});_tmp7C->f3=_tmp155;});_tmp7C;});_tmp7B->hd=_tmp156;});_tmp7B->tl=params;_tmp7B;});
# 365
params=_tmp157;});
# 368
({struct Cyc_List_List*_tmp159=({struct Cyc_List_List*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct Cyc_Absyn_Exp*_tmp158=Cyc_Absyn_var_exp(_tmp78->name,0U);_tmp7D->hd=_tmp158;});_tmp7D->tl=args;_tmp7D;});args=_tmp159;});
goto _LL5F;}else{_LL62: _LL63:
# 371
({struct Cyc_List_List*_tmp15D=({struct Cyc_List_List*_tmp7E=_cycalloc(sizeof(*_tmp7E));({struct _tuple8*_tmp15C=({struct _tuple8*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->f1=(*_tmp78->name).f2;_tmp7F->f2=_tmp78->tq;({void*_tmp15B=({
void*_tmp15A=_tmp78->type;Cyc_Absyn_cstar_typ(_tmp15A,Cyc_Absyn_empty_tqual(0U));});_tmp7F->f3=_tmp15B;});_tmp7F;});_tmp7E->hd=_tmp15C;});_tmp7E->tl=params;_tmp7E;});
# 371
params=_tmp15D;});
# 374
({struct Cyc_List_List*_tmp15F=({struct Cyc_List_List*_tmp80=_cycalloc(sizeof(*_tmp80));({struct Cyc_Absyn_Exp*_tmp15E=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(_tmp78->name,0U),0U);_tmp80->hd=_tmp15E;});_tmp80->tl=args;_tmp80;});args=_tmp15F;});
goto _LL5F;}_LL5F:;}{
# 377
struct _tuple0*funname=Cyc_Toc_temp_var();
({struct Cyc_List_List*_tmp164=({struct Cyc_List_List*_tmp82=_cycalloc(sizeof(*_tmp82));({struct Cyc_Absyn_Decl*_tmp163=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp83=_cycalloc(sizeof(*_tmp83));({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp162=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp84;_tmp84.tag=1U;({struct Cyc_Absyn_Fndecl*_tmp161=({struct Cyc_Absyn_Fndecl*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->sc=Cyc_Absyn_Static;_tmp85->is_inline=0;_tmp85->name=funname;_tmp85->tvs=0;_tmp85->effect=0;({struct Cyc_Absyn_Tqual _tmp160=
Cyc_Absyn_empty_tqual(0U);_tmp85->ret_tqual=_tmp160;});_tmp85->ret_type=rettype;_tmp85->args=params;_tmp85->c_varargs=0;_tmp85->cyc_varargs=0;_tmp85->rgn_po=0;_tmp85->body=s;_tmp85->cached_typ=0;_tmp85->param_vardecls=0;_tmp85->fn_vardecl=0;_tmp85->attributes=0;_tmp85->requires_clause=0;_tmp85->requires_relns=0;_tmp85->ensures_clause=0;_tmp85->ensures_relns=0;_tmp85;});_tmp84.f1=_tmp161;});_tmp84;});_tmp83[0]=_tmp162;});_tmp83;}),0U);_tmp82->hd=_tmp163;});_tmp82->tl=*env.gen_ds;_tmp82;});
# 378
*env.gen_ds=_tmp164;});
# 384
return({struct Cyc_Absyn_Exp*_tmp165=Cyc_Absyn_var_exp(funname,0U);Cyc_Absyn_fncall_exp(_tmp165,args,0U);});};};}
# 387
static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){
void*_tmp88=e->r;void*_tmp89=_tmp88;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_List_List*_tmp91;void*_tmp90;switch(*((int*)_tmp89)){case 1U: _LL65: _tmp90=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL66: {
# 390
struct _tuple0*_tmp8A=Cyc_Absyn_binding2qvar(_tmp90);struct _tuple0*_tmp8B=_tmp8A;struct _dyneither_ptr*_tmp8C;if(((((struct _tuple0*)_tmp8B)->f1).Loc_n).tag == 4U){_LL96: _tmp8C=_tmp8B->f2;_LL97:
# 392
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(varmap,_tmp8C))
({void*_tmp166=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(varmap,_tmp8C))->r;e->r=_tmp166;});
return;}else{_LL98: _LL99:
 return;}_LL95:;}case 0U: _LL67: _LL68:
# 399
 return;case 10U: _LL69: _tmp92=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp91=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL6A:
# 401
 Cyc_Tovc_apply_varmap(varmap,_tmp92);_tmp93=_tmp91;goto _LL6C;case 3U: _LL6B: _tmp93=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL6C:
# 403
 for(0;_tmp93 != 0;_tmp93=_tmp93->tl){
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)_tmp93->hd);}
return;case 6U: _LL6D: _tmp96=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp95=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_tmp94=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89)->f3;_LL6E:
# 408
 Cyc_Tovc_apply_varmap(varmap,_tmp94);_tmp98=_tmp96;_tmp97=_tmp95;goto _LL70;case 7U: _LL6F: _tmp98=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp97=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL70:
 _tmp9A=_tmp98;_tmp99=_tmp97;goto _LL72;case 8U: _LL71: _tmp9A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp99=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL72:
 _tmp9C=_tmp9A;_tmp9B=_tmp99;goto _LL74;case 23U: _LL73: _tmp9C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp9B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL74:
 _tmp9E=_tmp9C;_tmp9D=_tmp9B;goto _LL76;case 9U: _LL75: _tmp9E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp9D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL76:
 _tmpA0=_tmp9E;_tmp9F=_tmp9D;goto _LL78;case 4U: _LL77: _tmpA0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp9F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp89)->f3;_LL78:
# 414
 Cyc_Tovc_apply_varmap(varmap,_tmp9F);_tmpA1=_tmpA0;goto _LL7A;case 12U: _LL79: _tmpA1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL7A:
# 416
 _tmpA2=_tmpA1;goto _LL7C;case 13U: _LL7B: _tmpA2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL7C:
 _tmpA3=_tmpA2;goto _LL7E;case 14U: _LL7D: _tmpA3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL7E:
 _tmpA4=_tmpA3;goto _LL80;case 15U: _LL7F: _tmpA4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL80:
 _tmpA5=_tmpA4;goto _LL82;case 18U: _LL81: _tmpA5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL82:
 _tmpA6=_tmpA5;goto _LL84;case 20U: _LL83: _tmpA6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL84:
 _tmpA7=_tmpA6;goto _LL86;case 21U: _LL85: _tmpA7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL86:
 _tmpA8=_tmpA7;goto _LL88;case 22U: _LL87: _tmpA8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL88:
 _tmpA9=_tmpA8;goto _LL8A;case 5U: _LL89: _tmpA9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL8A:
# 425
 Cyc_Tovc_apply_varmap(varmap,_tmpA9);
return;case 17U: _LL8B: _LL8C:
# 428
 goto _LL8E;case 19U: _LL8D: _LL8E:
 goto _LL90;case 32U: _LL8F: _LL90:
 return;case 36U: _LL91: _tmpAA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL92:
# 433
 for(0;_tmpAA != 0;_tmpAA=_tmpAA->tl){
Cyc_Tovc_apply_varmap(varmap,(*((struct _tuple10*)_tmpAA->hd)).f2);}
return;default: _LL93: _LL94:
# 437
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));({struct Cyc_Core_Impossible_exn_struct _tmp168=({struct Cyc_Core_Impossible_exn_struct _tmp8E;_tmp8E.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp167=({const char*_tmp8F="bad exp form in Tocv::apply_varmap";_tag_dyneither(_tmp8F,sizeof(char),35U);});_tmp8E.f1=_tmp167;});_tmp8E;});_tmp8D[0]=_tmp168;});_tmp8D;}));}_LL64:;}
# 441
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
void*_tmpAB=s->r;void*_tmpAC=_tmpAB;struct Cyc_Absyn_Decl*_tmpC8;struct Cyc_Absyn_Stmt*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Stmt*_tmpC5;struct Cyc_Absyn_Stmt*_tmpC4;struct Cyc_Absyn_Stmt*_tmpC3;struct Cyc_Absyn_Stmt*_tmpC2;struct Cyc_Absyn_Exp*_tmpC1;switch(*((int*)_tmpAC)){case 0U: _LL9B: _LL9C:
 return Cyc_Absyn_true_exp(0U);case 1U: _LL9D: _tmpC1=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpAC)->f1;_LL9E:
# 445
 Cyc_Tovc_apply_varmap(env.varmap,_tmpC1);
return _tmpC1;case 2U: _LL9F: _tmpC3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpAC)->f1;_tmpC2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpAC)->f2;_LLA0: {
# 449
struct Cyc_Absyn_Exp*_tmpAD=Cyc_Tovc_stmt_to_exp(env,_tmpC3);
struct Cyc_Absyn_Exp*_tmpAE=Cyc_Tovc_stmt_to_exp(env,_tmpC2);
return Cyc_Absyn_seq_exp(_tmpAD,_tmpAE,0U);}case 4U: _LLA1: _tmpC6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpAC)->f1;_tmpC5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpAC)->f2;_tmpC4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpAC)->f3;_LLA2:
# 453
 Cyc_Tovc_apply_varmap(env.varmap,_tmpC6);{
# 455
struct Cyc_Absyn_Exp*_tmpAF=Cyc_Tovc_stmt_to_exp(env,_tmpC5);
struct Cyc_Absyn_Exp*_tmpB0=Cyc_Tovc_stmt_to_exp(env,_tmpC4);
return Cyc_Absyn_conditional_exp(_tmpC6,_tmpAF,_tmpB0,0U);};case 12U: _LLA3: _tmpC8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpAC)->f1;_tmpC7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpAC)->f2;_LLA4: {
# 459
void*_tmpB1=_tmpC8->r;void*_tmpB2=_tmpB1;struct Cyc_Absyn_Vardecl*_tmpC0;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpB2)->tag == 0U){_LLA8: _tmpC0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpB2)->f1;_LLA9: {
# 462
struct _dyneither_ptr*_tmpB3=(*_tmpC0->name).f2;
struct _tuple0*_tmpB4=Cyc_Toc_temp_var();
_tmpC0->name=_tmpB4;
({struct Cyc_Dict_Dict _tmp16B=({struct Cyc_Dict_Dict _tmp16A=env.varmap;struct _dyneither_ptr*_tmp169=_tmpB3;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(_tmp16A,_tmp169,Cyc_Absyn_var_exp(_tmpB4,0U));});env.varmap=_tmp16B;});{
struct Cyc_List_List*_tmpB5=0;
if(_tmpC0->initializer != 0){
Cyc_Tovc_apply_varmap(env.varmap,(struct Cyc_Absyn_Exp*)_check_null(_tmpC0->initializer));{
void*_tmpB6=((struct Cyc_Absyn_Exp*)_check_null(_tmpC0->initializer))->r;void*_tmpB7=_tmpB6;struct Cyc_List_List*_tmpBA;if(((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB7)->tag == 36U){_LLAD: _tmpBA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LLAE: {
# 481 "tovc.cyc"
struct Cyc_Absyn_Exp*_tmpB8=Cyc_Absyn_var_exp(_tmpC0->name,0U);
{unsigned int i=0U;for(0;_tmpBA != 0;(_tmpBA=_tmpBA->tl,++ i)){
({struct Cyc_List_List*_tmp16F=({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*_tmpB9));({struct Cyc_Absyn_Exp*_tmp16E=({struct Cyc_Absyn_Exp*_tmp16D=({struct Cyc_Absyn_Exp*_tmp16C=_tmpB8;Cyc_Absyn_subscript_exp(_tmp16C,Cyc_Absyn_uint_exp(i,0U),0U);});Cyc_Absyn_assign_exp(_tmp16D,(*((struct _tuple10*)_tmpBA->hd)).f2,0U);});_tmpB9->hd=_tmp16E;});_tmpB9->tl=_tmpB5;_tmpB9;});_tmpB5=_tmp16F;});}}
# 487
_tmpC0->initializer=0;
goto _LLAC;}}else{_LLAF: _LLB0:
 goto _LLAC;}_LLAC:;};}
# 492
({struct Cyc_List_List*_tmp170=({struct Cyc_List_List*_tmpBB=_region_malloc(env.rgn,sizeof(*_tmpBB));_tmpBB->hd=_tmpC0;_tmpBB->tl=env.all_locals;_tmpBB;});env.all_locals=_tmp170;});
# 494
({void*_tmp172=({struct Cyc_Absyn_Decl*_tmp171=_tmpC8;Cyc_Absyn_decl_stmt(_tmp171,Cyc_Absyn_new_stmt((env.encloser)->r,0U),0U);})->r;(env.encloser)->r=_tmp172;});{
struct Cyc_Absyn_Exp*_tmpBC=Cyc_Tovc_stmt_to_exp(env,_tmpC7);
if(_tmpC0->initializer != 0){
({struct Cyc_Absyn_Exp*_tmp175=({struct Cyc_Absyn_Exp*_tmp174=({struct Cyc_Absyn_Exp*_tmp173=Cyc_Absyn_var_exp(_tmpC0->name,0U);Cyc_Absyn_assign_exp(_tmp173,(struct Cyc_Absyn_Exp*)_check_null(_tmpC0->initializer),0U);});Cyc_Absyn_seq_exp(_tmp174,_tmpBC,0U);});_tmpBC=_tmp175;});
# 499
_tmpC0->initializer=0;}
# 501
for(0;_tmpB5 != 0;_tmpB5=_tmpB5->tl){
({struct Cyc_Absyn_Exp*_tmp176=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)_tmpB5->hd,_tmpBC,0U);_tmpBC=_tmp176;});}
return _tmpBC;};};}}else{_LLAA: _LLAB:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));({struct Cyc_Core_Impossible_exn_struct _tmp178=({struct Cyc_Core_Impossible_exn_struct _tmpBE;_tmpBE.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp177=({const char*_tmpBF="bad local decl in Tovc::stmt_to_exp";_tag_dyneither(_tmpBF,sizeof(char),36U);});_tmpBE.f1=_tmp177;});_tmpBE;});_tmpBD[0]=_tmp178;});_tmpBD;}));}_LLA7:;}default: _LLA5: _LLA6:
# 507
 return Cyc_Tovc_stmt_to_fun(env,s,(void*)& Cyc_Absyn_VoidType_val);}_LL9A:;}
# 511
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);
# 513
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
struct _dyneither_ptr bad_form=({const char*_tmpFB="";_tag_dyneither(_tmpFB,sizeof(char),1U);});
{void*_tmpC9=e->r;void*_tmpCA=_tmpC9;struct Cyc_Absyn_Stmt*_tmpF4;struct Cyc_List_List*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_List_List*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_List_List*_tmpDA;switch(*((int*)_tmpCA)){case 0U: _LLB2: _LLB3:
 return;case 1U: _LLB4: _LLB5:
 return;case 10U: _LLB6: _tmpDB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpDA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLB7:
# 520
 Cyc_Tovc_exp_to_vc(env,_tmpDB);_tmpDC=_tmpDA;goto _LLB9;case 3U: _LLB8: _tmpDC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLB9:
# 522
 for(0;_tmpDC != 0;_tmpDC=_tmpDC->tl){
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_tmpDC->hd);}
return;case 6U: _LLBA: _tmpDF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpDE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_tmpDD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCA)->f3;_LLBB:
# 527
 Cyc_Tovc_exp_to_vc(env,_tmpDD);_tmpE1=_tmpDF;_tmpE0=_tmpDE;goto _LLBD;case 7U: _LLBC: _tmpE1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpE0=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLBD:
 _tmpE3=_tmpE1;_tmpE2=_tmpE0;goto _LLBF;case 8U: _LLBE: _tmpE3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpE2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLBF:
 _tmpE5=_tmpE3;_tmpE4=_tmpE2;goto _LLC1;case 23U: _LLC0: _tmpE5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpE4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLC1:
 _tmpE7=_tmpE5;_tmpE6=_tmpE4;goto _LLC3;case 9U: _LLC2: _tmpE7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpE6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLC3:
 _tmpE9=_tmpE7;_tmpE8=_tmpE6;goto _LLC5;case 4U: _LLC4: _tmpE9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpE8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpCA)->f3;_LLC5:
# 533
 Cyc_Tovc_exp_to_vc(env,_tmpE8);_tmpEA=_tmpE9;goto _LLC7;case 12U: _LLC6: _tmpEA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLC7:
# 535
 _tmpEB=_tmpEA;goto _LLC9;case 13U: _LLC8: _tmpEB=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLC9:
 _tmpEC=_tmpEB;goto _LLCB;case 14U: _LLCA: _tmpEC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLCB:
 _tmpED=_tmpEC;goto _LLCD;case 15U: _LLCC: _tmpED=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLCD:
 _tmpEE=_tmpED;goto _LLCF;case 18U: _LLCE: _tmpEE=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLCF:
 _tmpEF=_tmpEE;goto _LLD1;case 20U: _LLD0: _tmpEF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLD1:
 _tmpF0=_tmpEF;goto _LLD3;case 21U: _LLD2: _tmpF0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLD3:
 _tmpF1=_tmpF0;goto _LLD5;case 22U: _LLD4: _tmpF1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLD5:
 _tmpF2=_tmpF1;goto _LLD7;case 5U: _LLD6: _tmpF2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLD7:
# 544
 Cyc_Tovc_exp_to_vc(env,_tmpF2);
return;case 17U: _LLD8: _LLD9:
# 547
 goto _LLDB;case 19U: _LLDA: _LLDB:
 goto _LLDD;case 33U: _LLDC: _LLDD:
 goto _LLDF;case 32U: _LLDE: _LLDF:
 return;case 36U: _LLE0: _tmpF3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLE1:
# 556
 for(0;_tmpF3 != 0;_tmpF3=_tmpF3->tl){
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple10*)_tmpF3->hd)).f2);}
return;case 37U: _LLE2: _tmpF4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLE3:
# 564
 Cyc_Tovc_stmt_to_vc(env,_tmpF4);
# 566
({void*_tmp179=(Cyc_Tovc_stmt_to_exp(env,_tmpF4))->r;e->r=_tmp179;});
return;case 40U: _LLE4: _LLE5:
# 569
 return;case 2U: _LLE6: _LLE7:
# 571
({struct _dyneither_ptr _tmp17A=({const char*_tmpCB="Pragma_e";_tag_dyneither(_tmpCB,sizeof(char),9U);});bad_form=_tmp17A;});goto _LLB1;case 11U: _LLE8: _LLE9:
({struct _dyneither_ptr _tmp17B=({const char*_tmpCC="Throw_e";_tag_dyneither(_tmpCC,sizeof(char),8U);});bad_form=_tmp17B;});goto _LLB1;case 16U: _LLEA: _LLEB:
({struct _dyneither_ptr _tmp17C=({const char*_tmpCD="New_e";_tag_dyneither(_tmpCD,sizeof(char),6U);});bad_form=_tmp17C;});goto _LLB1;case 24U: _LLEC: _LLED:
({struct _dyneither_ptr _tmp17D=({const char*_tmpCE="Tuple_e";_tag_dyneither(_tmpCE,sizeof(char),8U);});bad_form=_tmp17D;});goto _LLB1;case 25U: _LLEE: _LLEF:
({struct _dyneither_ptr _tmp17E=({const char*_tmpCF="CompoundLit_e";_tag_dyneither(_tmpCF,sizeof(char),14U);});bad_form=_tmp17E;});goto _LLB1;case 26U: _LLF0: _LLF1:
({struct _dyneither_ptr _tmp17F=({const char*_tmpD0="Array_e";_tag_dyneither(_tmpD0,sizeof(char),8U);});bad_form=_tmp17F;});goto _LLB1;case 27U: _LLF2: _LLF3:
({struct _dyneither_ptr _tmp180=({const char*_tmpD1="Comprehension_e";_tag_dyneither(_tmpD1,sizeof(char),16U);});bad_form=_tmp180;});goto _LLB1;case 28U: _LLF4: _LLF5:
({struct _dyneither_ptr _tmp181=({const char*_tmpD2="ComprehensionNoinit_e";_tag_dyneither(_tmpD2,sizeof(char),22U);});bad_form=_tmp181;});goto _LLB1;case 29U: _LLF6: _LLF7:
({struct _dyneither_ptr _tmp182=({const char*_tmpD3="Aggregate_e";_tag_dyneither(_tmpD3,sizeof(char),12U);});bad_form=_tmp182;});goto _LLB1;case 30U: _LLF8: _LLF9:
({struct _dyneither_ptr _tmp183=({const char*_tmpD4="AnonStruct_e";_tag_dyneither(_tmpD4,sizeof(char),13U);});bad_form=_tmp183;});goto _LLB1;case 31U: _LLFA: _LLFB:
# 582
({struct _dyneither_ptr _tmp184=({const char*_tmpD5="Datatype_e";_tag_dyneither(_tmpD5,sizeof(char),11U);});bad_form=_tmp184;});goto _LLB1;case 34U: _LLFC: _LLFD:
({struct _dyneither_ptr _tmp185=({const char*_tmpD6="Malloc_e";_tag_dyneither(_tmpD6,sizeof(char),9U);});bad_form=_tmp185;});goto _LLB1;case 35U: _LLFE: _LLFF:
({struct _dyneither_ptr _tmp186=({const char*_tmpD7="Swap_e";_tag_dyneither(_tmpD7,sizeof(char),7U);});bad_form=_tmp186;});goto _LLB1;case 38U: _LL100: _LL101:
({struct _dyneither_ptr _tmp187=({const char*_tmpD8="Tagcheck_e";_tag_dyneither(_tmpD8,sizeof(char),11U);});bad_form=_tmp187;});goto _LLB1;default: _LL102: _LL103:
({struct _dyneither_ptr _tmp188=({const char*_tmpD9="Valueof_e";_tag_dyneither(_tmpD9,sizeof(char),10U);});bad_form=_tmp188;});goto _LLB1;}_LLB1:;}
# 588
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));({struct Cyc_Core_Impossible_exn_struct _tmp18C=({struct Cyc_Core_Impossible_exn_struct _tmpF6;_tmpF6.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp18B=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpFA;_tmpFA.tag=0U;({struct _dyneither_ptr _tmp189=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmpFA.f1=_tmp189;});({struct Cyc_String_pa_PrintArg_struct _tmpF9;_tmpF9.tag=0U;_tmpF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)bad_form);({void*_tmpF7[2U]={& _tmpF9,& _tmpFA};({struct _dyneither_ptr _tmp18A=({const char*_tmpF8="bad exp form %s (exp=|%s|) after xlation to C";_tag_dyneither(_tmpF8,sizeof(char),46U);});Cyc_aprintf(_tmp18A,_tag_dyneither(_tmpF7,sizeof(void*),2U));});});});});_tmpF6.f1=_tmp18B;});_tmpF6;});_tmpF5[0]=_tmp18C;});_tmpF5;}));}
# 592
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 594
while(1){
env.encloser=s;{
void*_tmpFC=s->r;void*_tmpFD=_tmpFC;struct Cyc_Absyn_Decl*_tmp123;struct Cyc_Absyn_Stmt*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_List_List*_tmp120;struct Cyc_Absyn_Stmt*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Stmt*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Stmt*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Stmt*_tmp116;struct Cyc_Absyn_Stmt*_tmp115;struct Cyc_Absyn_Stmt*_tmp114;struct Cyc_Absyn_Stmt*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Stmt*_tmp110;switch(*((int*)_tmpFD)){case 0U: _LL105: _LL106:
 goto _LL108;case 6U: _LL107: _LL108:
 goto _LL10A;case 7U: _LL109: _LL10A:
 goto _LL10C;case 8U: _LL10B: _LL10C:
 return;case 13U: _LL10D: _tmp110=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmpFD)->f2;_LL10E:
# 602
 s=_tmp110;
continue;case 3U: _LL10F: _tmp111=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmpFD)->f1;_LL110:
# 605
 if(_tmp111 == 0)
return;
_tmp112=_tmp111;goto _LL112;case 1U: _LL111: _tmp112=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpFD)->f1;_LL112:
# 609
 Cyc_Tovc_exp_to_vc(env,_tmp112);
return;case 2U: _LL113: _tmp114=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpFD)->f1;_tmp113=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpFD)->f2;_LL114:
# 612
 Cyc_Tovc_stmt_to_vc(env,_tmp114);
s=_tmp113;
continue;case 4U: _LL115: _tmp117=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpFD)->f1;_tmp116=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpFD)->f2;_tmp115=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpFD)->f3;_LL116:
# 616
 Cyc_Tovc_exp_to_vc(env,_tmp117);
Cyc_Tovc_stmt_to_vc(env,_tmp116);
s=_tmp115;
continue;case 5U: _LL117: _tmp119=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpFD)->f1).f1;_tmp118=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpFD)->f2;_LL118:
# 621
 Cyc_Tovc_exp_to_vc(env,_tmp119);
s=_tmp118;
continue;case 9U: _LL119: _tmp11D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpFD)->f1;_tmp11C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpFD)->f2).f1;_tmp11B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpFD)->f3).f1;_tmp11A=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpFD)->f4;_LL11A:
# 625
 Cyc_Tovc_exp_to_vc(env,_tmp11D);
Cyc_Tovc_exp_to_vc(env,_tmp11C);
Cyc_Tovc_exp_to_vc(env,_tmp11B);
s=_tmp11A;
continue;case 14U: _LL11B: _tmp11F=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpFD)->f1;_tmp11E=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpFD)->f2).f1;_LL11C:
# 631
 Cyc_Tovc_exp_to_vc(env,_tmp11E);
s=_tmp11F;
continue;case 10U: _LL11D: _tmp121=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpFD)->f1;_tmp120=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpFD)->f2;_LL11E:
# 637
 Cyc_Tovc_exp_to_vc(env,_tmp121);
for(0;_tmp120 != 0;_tmp120=_tmp120->tl){
Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)_tmp120->hd)->body);}
return;case 12U: _LL11F: _tmp123=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpFD)->f1;_tmp122=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpFD)->f2;_LL120:
# 642
{void*_tmpFE=_tmp123->r;void*_tmpFF=_tmpFE;struct Cyc_Absyn_Vardecl*_tmp10C;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpFF)->tag == 0U){_LL124: _tmp10C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpFF)->f1;_LL125:
# 644
 if(Cyc_Tovc_elim_array_initializers){
# 649
void*_tmp100=Cyc_Tcutil_compress(_tmp10C->type);void*_tmp101=_tmp100;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp101)->tag == 8U){_LL129: _LL12A:
# 651
 if(_tmp10C->initializer != 0){
void*_tmp102=((struct Cyc_Absyn_Exp*)_check_null(_tmp10C->initializer))->r;void*_tmp103=_tmp102;struct Cyc_List_List*_tmp10A;if(((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp103)->tag == 36U){_LL12E: _tmp10A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL12F:
# 654
 _tmp10C->initializer=0;{
# 656
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(_tmp10C->name,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(_tmp122->r,0U);
{unsigned int i=0U;for(0;_tmp10A != 0;(_tmp10A=_tmp10A->tl,++ i)){
({struct Cyc_Absyn_Stmt*_tmp190=({struct Cyc_Absyn_Stmt*_tmp18F=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp18E=({struct Cyc_Absyn_Exp*_tmp18D=arr;Cyc_Absyn_subscript_exp(_tmp18D,
Cyc_Absyn_uint_exp(i,0U),0U);});
# 659
Cyc_Absyn_assign_exp(_tmp18E,(*((struct _tuple10*)_tmp10A->hd)).f2,0U);}),0U);Cyc_Absyn_seq_stmt(_tmp18F,s3,0U);});s3=_tmp190;});}}
# 664
_tmp122->r=s3->r;
goto _LL12D;};}else{_LL130: _LL131: {
# 667
struct _dyneither_ptr _tmp104=({struct Cyc_String_pa_PrintArg_struct _tmp109;_tmp109.tag=0U;({struct _dyneither_ptr _tmp191=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s));_tmp109.f1=_tmp191;});({void*_tmp107[1U]={& _tmp109};({struct _dyneither_ptr _tmp192=({const char*_tmp108="|%s|: array type bad initializer";_tag_dyneither(_tmp108,sizeof(char),33U);});Cyc_aprintf(_tmp192,_tag_dyneither(_tmp107,sizeof(void*),1U));});});});
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp105=_cycalloc(sizeof(*_tmp105));({struct Cyc_Core_Impossible_exn_struct _tmp193=({struct Cyc_Core_Impossible_exn_struct _tmp106;_tmp106.tag=Cyc_Core_Impossible;_tmp106.f1=(struct _dyneither_ptr)_tmp104;_tmp106;});_tmp105[0]=_tmp193;});_tmp105;}));}}_LL12D:;}
# 671
goto _LL128;}else{_LL12B: _LL12C:
 goto _LL128;}_LL128:;}
# 675
if(_tmp10C->initializer != 0)
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp10C->initializer));
({struct Cyc_List_List*_tmp194=({struct Cyc_List_List*_tmp10B=_region_malloc(env.rgn,sizeof(*_tmp10B));_tmp10B->hd=_tmp10C;_tmp10B->tl=env.all_locals;_tmp10B;});env.all_locals=_tmp194;});
s=_tmp122;
continue;}else{_LL126: _LL127:
 goto _LL123;}_LL123:;}
# 682
goto _LL122;default: _LL121: _LL122:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));({struct Cyc_Core_Impossible_exn_struct _tmp196=({struct Cyc_Core_Impossible_exn_struct _tmp10E;_tmp10E.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp195=({const char*_tmp10F="bad stmt after xlation to C";_tag_dyneither(_tmp10F,sizeof(char),28U);});_tmp10E.f1=_tmp195;});_tmp10E;});_tmp10D[0]=_tmp196;});_tmp10D;}));}_LL104:;};}}
# 688
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){
struct Cyc_List_List*_tmp124=0;
for(0;old_ds != 0;old_ds=old_ds->tl){
struct Cyc_Absyn_Decl*_tmp125=(struct Cyc_Absyn_Decl*)old_ds->hd;
void*_tmp126=_tmp125->r;void*_tmp127=_tmp126;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Fndecl*_tmp137;switch(*((int*)_tmp127)){case 1U: _LL133: _tmp137=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp127)->f1;_LL134:
# 697
 _tmp137->cached_typ=0;
# 704
if(_tmp137->param_vardecls != 0){
struct _RegionHandle _tmp128=_new_region("rgn");struct _RegionHandle*rgn=& _tmp128;_push_region(rgn);
{struct Cyc_List_List**_tmp129=({struct Cyc_List_List**_tmp132=_region_malloc(rgn,sizeof(*_tmp132));_tmp132[0]=0;_tmp132;});
if(_tmp137->param_vardecls == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp12C;_tmp12C.tag=0U;_tmp12C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp137->name).f2);({void*_tmp12A[1U]={& _tmp12C};({struct Cyc___cycFILE*_tmp198=Cyc_stderr;struct _dyneither_ptr _tmp197=({const char*_tmp12B="No param vardecls for function %s\n";_tag_dyneither(_tmp12B,sizeof(char),35U);});Cyc_fprintf(_tmp198,_tmp197,_tag_dyneither(_tmp12A,sizeof(void*),1U));});});});
Cyc_fflush(Cyc_stderr);}
# 711
({struct Cyc_Tovc_ToExpEnv _tmp19B=({struct Cyc_Tovc_ToExpEnv _tmp12D;_tmp12D.rgn=rgn;({struct Cyc_List_List*_tmp199=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp137->param_vardecls))->v);_tmp12D.all_locals=_tmp199;});({struct Cyc_Dict_Dict _tmp19A=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(rgn,Cyc_strptrcmp);_tmp12D.varmap=_tmp19A;});_tmp12D.encloser=_tmp137->body;_tmp12D.gen_ds=_tmp129;_tmp12D;});
# 711
Cyc_Tovc_stmt_to_vc(_tmp19B,_tmp137->body);});
# 717
if(*_tmp129 != 0){
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->sc=_tmp137->sc;_tmp131->name=_tmp137->name;_tmp131->varloc=0U;({struct Cyc_Absyn_Tqual _tmp19C=Cyc_Absyn_const_tqual(0U);_tmp131->tq=_tmp19C;});({void*_tmp19D=
Cyc_Tcutil_fndecl2typ(_tmp137);_tmp131->type=_tmp19D;});_tmp131->initializer=0;_tmp131->rgn=0;_tmp131->attributes=_tmp137->attributes;_tmp131->escapes=0;_tmp131;});
({struct Cyc_List_List*_tmp1A0=({struct Cyc_List_List*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct Cyc_Absyn_Decl*_tmp19F=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp19E=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp130;_tmp130.tag=0U;_tmp130.f1=vd;_tmp130;});_tmp12F[0]=_tmp19E;});_tmp12F;}),0U);_tmp12E->hd=_tmp19F;});_tmp12E->tl=_tmp124;_tmp12E;});_tmp124=_tmp1A0;});
# 722
({struct Cyc_List_List*_tmp1A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*_tmp129,_tmp124);_tmp124=_tmp1A1;});}}
# 706
;_pop_region(rgn);}
# 726
goto _LL136;case 0U: _LL135: _LL136:
 goto _LL138;case 2U: _LL137: _LL138:
 goto _LL13A;case 3U: _LL139: _LL13A:
 goto _LL13C;case 4U: _LL13B: _LL13C:
 goto _LL13E;case 5U: _LL13D: _LL13E:
 goto _LL140;case 6U: _LL13F: _LL140:
 goto _LL142;case 7U: _LL141: _LL142:
 goto _LL144;case 8U: _LL143: _LL144:
# 735
({struct Cyc_List_List*_tmp1A2=({struct Cyc_List_List*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->hd=_tmp125;_tmp133->tl=_tmp124;_tmp133;});_tmp124=_tmp1A2;});
goto _LL132;case 9U: _LL145: _tmp138=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp127)->f2;_LL146:
 _tmp139=_tmp138;goto _LL148;case 10U: _LL147: _tmp139=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp127)->f2;_LL148:
 _tmp13A=_tmp139;goto _LL14A;case 11U: _LL149: _tmp13A=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp127)->f1;_LL14A:
 _tmp13B=_tmp13A;goto _LL14C;case 12U: _LL14B: _tmp13B=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp127)->f1;_LL14C:
 goto _LL14E;case 13U: _LL14D: _LL14E:
 goto _LL150;default: _LL14F: _LL150:
# 743
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp134=_cycalloc(sizeof(*_tmp134));({struct Cyc_Core_Impossible_exn_struct _tmp1A4=({struct Cyc_Core_Impossible_exn_struct _tmp135;_tmp135.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp1A3=({const char*_tmp136="nested translation unit after translation to C";_tag_dyneither(_tmp136,sizeof(char),47U);});_tmp135.f1=_tmp1A3;});_tmp135;});_tmp134[0]=_tmp1A4;});_tmp134;}));}_LL132:;}
# 746
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp124);}
