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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
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
void Cyc_Core_ufree(void*ptr);struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 290 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef void*Cyc_Core___nn_cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 40
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 45
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);
# 51
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
# 54
struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty();
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 60
struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int,void*);
# 63
struct Cyc_Xarray_Xarray*Cyc_Xarray_rsingleton(struct _RegionHandle*,int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 72
struct _dyneither_ptr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*);
# 75
struct _dyneither_ptr Cyc_Xarray_rto_array(struct _RegionHandle*,struct Cyc_Xarray_Xarray*);
# 78
struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _dyneither_ptr arr);
# 81
struct Cyc_Xarray_Xarray*Cyc_Xarray_rfrom_array(struct _RegionHandle*,struct _dyneither_ptr arr);
# 85
struct Cyc_Xarray_Xarray*Cyc_Xarray_append(struct Cyc_Xarray_Xarray*,struct Cyc_Xarray_Xarray*);
# 89
struct Cyc_Xarray_Xarray*Cyc_Xarray_rappend(struct _RegionHandle*,struct Cyc_Xarray_Xarray*,struct Cyc_Xarray_Xarray*);
# 93
void Cyc_Xarray_app(void*(*f)(void*),struct Cyc_Xarray_Xarray*);
# 97
void Cyc_Xarray_app_c(void*(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 100
void Cyc_Xarray_iter(void(*f)(void*),struct Cyc_Xarray_Xarray*);
# 104
void Cyc_Xarray_iter_c(void(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 107
struct Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),struct Cyc_Xarray_Xarray*);
# 110
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_Xarray_Xarray*);
# 114
struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 117
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);
# 124
void Cyc_Xarray_delete(struct Cyc_Xarray_Xarray*xarr,int num);
# 126
void Cyc_Xarray_remove(struct Cyc_Xarray_Xarray*xarr,int i);
# 25 "xarray.cyc"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*xarr){
return xarr->num_elmts;}
# 29
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*xarr,int i){
if(i < 0  || i >= xarr->num_elmts)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp1;_tmp1.tag=Cyc_Core_Invalid_argument;_tmp1.f1=({const char*_tmp2="Xarray::get: bad index";_tag_dyneither(_tmp2,sizeof(char),23);});_tmp1;});_tmp0;}));
return*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i));}
# 35
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*xarr,int i,void*a){
if(i < 0  || i >= xarr->num_elmts)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp4;_tmp4.tag=Cyc_Core_Invalid_argument;_tmp4.f1=({const char*_tmp5="Xarray::set: bad index";_tag_dyneither(_tmp5,sizeof(char),23);});_tmp4;});_tmp3;}));
*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i))=a;}
# 41
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*r){
struct _dyneither_ptr x=_tag_dyneither(0,0,0);
return({struct Cyc_Xarray_Xarray*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6->elmts=x;_tmp6->num_elmts=0;_tmp6;});}
# 45
struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty(){return Cyc_Xarray_rcreate_empty(Cyc_Core_heap_region);}
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*r,int len,void*a){
if(len < 0)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp8;_tmp8.tag=Cyc_Core_Invalid_argument;_tmp8.f1=({const char*_tmp9="xarrays must have a non-negative size buffer";_tag_dyneither(_tmp9,sizeof(char),45);});_tmp8;});_tmp7;}));
return({struct Cyc_Xarray_Xarray*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->elmts=({unsigned int _tmpB=(unsigned int)len;void**_tmpC=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmpB));struct _dyneither_ptr _tmpE=_tag_dyneither(_tmpC,sizeof(void*),_tmpB);{unsigned int _tmpD=_tmpB;unsigned int i;for(i=0;i < _tmpD;i ++){_tmpC[i]=(void*)a;}}_tmpE;});_tmpA->num_elmts=0;_tmpA;});}
# 54
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int len,void*a){
return Cyc_Xarray_rcreate(Cyc_Core_heap_region,len,a);}
# 58
struct Cyc_Xarray_Xarray*Cyc_Xarray_rsingleton(struct _RegionHandle*r,int len,void*a){
if(len < 1)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp10;_tmp10.tag=Cyc_Core_Invalid_argument;_tmp10.f1=({const char*_tmp11="singleton xarray must have size >=1";_tag_dyneither(_tmp11,sizeof(char),36);});_tmp10;});_tmpF;}));{
struct Cyc_Xarray_Xarray*x=Cyc_Xarray_rcreate(r,len,a);
x->num_elmts=1;
return x;};}
# 65
struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int len,void*a){
return Cyc_Xarray_rsingleton(Cyc_Core_heap_region,len,a);}
# 69
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*xarr,void*a){
if(xarr->num_elmts == _get_dyneither_size(xarr->elmts,sizeof(void*))){
if(xarr->num_elmts == 0)
xarr->elmts=_tag_dyneither(({unsigned int _tmp12=(unsigned int)10;void**_tmp13=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp12));{unsigned int _tmp14=_tmp12;unsigned int i;for(i=0;i < _tmp14;i ++){_tmp13[i]=(void*)a;}}_tmp13;}),sizeof(void*),(unsigned int)10);else{
# 74
struct _dyneither_ptr newarr=({unsigned int _tmp17=(unsigned int)(xarr->num_elmts * 2);void**_tmp18=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp17));struct _dyneither_ptr _tmp1A=_tag_dyneither(_tmp18,sizeof(void*),_tmp17);{unsigned int _tmp19=_tmp17;unsigned int i;for(i=0;i < _tmp19;i ++){_tmp18[i]=(void*)*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),0));}}_tmp1A;});
{int i=1;for(0;i < xarr->num_elmts;++ i){
*((void**)_check_dyneither_subscript(newarr,sizeof(void*),i))=*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i));}}
({struct _dyneither_ptr _tmp15=xarr->elmts;struct _dyneither_ptr _tmp16=newarr;xarr->elmts=_tmp16;newarr=_tmp15;});
Cyc_Core_ufree((void**)_untag_dyneither_ptr(newarr,sizeof(void*),1));}}
# 81
*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),xarr->num_elmts ++))=a;}
# 84
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*xarr,void*a){
Cyc_Xarray_add(xarr,a);
return xarr->num_elmts - 1;}
# 89
struct _dyneither_ptr Cyc_Xarray_rto_array(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)
return _tag_dyneither(({void**_tmp1B=_region_malloc(r,sizeof(void*)* 0);_tmp1B;}),sizeof(void*),0);{
struct _dyneither_ptr ans=({unsigned int _tmp1C=(unsigned int)xarr->num_elmts;void**_tmp1D=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp1C));struct _dyneither_ptr _tmp1F=_tag_dyneither(_tmp1D,sizeof(void*),_tmp1C);{unsigned int _tmp1E=_tmp1C;unsigned int i;for(i=0;i < _tmp1E;i ++){_tmp1D[i]=(void*)*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),(int)i));}}_tmp1F;});
return ans;};}
# 96
struct _dyneither_ptr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rto_array(Cyc_Core_heap_region,xarr);}
# 100
struct Cyc_Xarray_Xarray*Cyc_Xarray_rfrom_array(struct _RegionHandle*r,struct _dyneither_ptr arr){
if(_get_dyneither_size(arr,sizeof(void*))== 0)
return Cyc_Xarray_rcreate_empty(r);{
struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp20=_region_malloc(r,sizeof(*_tmp20));_tmp20->elmts=({unsigned int _tmp21=
_get_dyneither_size(arr,sizeof(void*));void**_tmp22=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp21));struct _dyneither_ptr _tmp24=_tag_dyneither(_tmp22,sizeof(void*),_tmp21);{unsigned int _tmp23=_tmp21;unsigned int i;for(i=0;i < _tmp23;i ++){_tmp22[i]=(void*)((void**)arr.curr)[(int)i];}}_tmp24;});_tmp20->num_elmts=(int)
_get_dyneither_size(arr,sizeof(void*));_tmp20;});
return ans;};}
# 109
struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _dyneither_ptr arr){
return Cyc_Xarray_rfrom_array(Cyc_Core_heap_region,arr);}
# 114
struct Cyc_Xarray_Xarray*Cyc_Xarray_rappend(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){
int newsz=(int)(_get_dyneither_size(xarr1->elmts,sizeof(void*))+ _get_dyneither_size(xarr2->elmts,sizeof(void*)));
if(newsz == 0)
return Cyc_Xarray_rcreate_empty(r);{
void*init=_get_dyneither_size(xarr1->elmts,sizeof(void*))== 0?*((void**)_check_dyneither_subscript(xarr2->elmts,sizeof(void*),0)):*((void**)_check_dyneither_subscript(xarr1->elmts,sizeof(void*),0));
struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp25=_region_malloc(r,sizeof(*_tmp25));_tmp25->elmts=({unsigned int _tmp26=(unsigned int)newsz;void**_tmp27=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp26));struct _dyneither_ptr _tmp29=_tag_dyneither(_tmp27,sizeof(void*),_tmp26);{unsigned int _tmp28=_tmp26;unsigned int i;for(i=0;i < _tmp28;i ++){_tmp27[i]=(void*)init;}}_tmp29;});_tmp25->num_elmts=0;_tmp25;});
# 121
{int i=0;for(0;i < xarr1->num_elmts;++ i){
Cyc_Xarray_add(ans,*((void**)_check_dyneither_subscript(xarr1->elmts,sizeof(void*),i)));}}
{int i=0;for(0;i < xarr2->num_elmts;++ i){
Cyc_Xarray_add(ans,*((void**)_check_dyneither_subscript(xarr2->elmts,sizeof(void*),i)));}}
return ans;};}
# 128
struct Cyc_Xarray_Xarray*Cyc_Xarray_append(struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){
return Cyc_Xarray_rappend(Cyc_Core_heap_region,xarr1,xarr2);}
# 132
void Cyc_Xarray_app(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 137
void Cyc_Xarray_app_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 142
void Cyc_Xarray_iter(void(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 147
void Cyc_Xarray_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 152
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)return Cyc_Xarray_rcreate_empty(r);{
# 155
struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp2A=_region_malloc(r,sizeof(*_tmp2A));_tmp2A->elmts=({unsigned int _tmp2B=
_get_dyneither_size(xarr->elmts,sizeof(void*));void**_tmp2C=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp2B));struct _dyneither_ptr _tmp2E=_tag_dyneither(_tmp2C,sizeof(void*),_tmp2B);{unsigned int _tmp2D=_tmp2B;unsigned int i;for(i=0;i < _tmp2D;i ++){_tmp2C[i]=(void*)f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),0)));}}_tmp2E;});_tmp2A->num_elmts=xarr->num_elmts;_tmp2A;});
# 158
{int i=1;for(0;i < xarr->num_elmts;++ i){
*((void**)_check_dyneither_subscript(ans->elmts,sizeof(void*),i))=f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
return ans;};}
# 163
struct Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rmap(Cyc_Core_heap_region,f,xarr);}
# 167
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)return Cyc_Xarray_rcreate_empty(r);{
# 170
struct Cyc_Xarray_Xarray*ans=({struct Cyc_Xarray_Xarray*_tmp2F=_region_malloc(r,sizeof(*_tmp2F));_tmp2F->elmts=({unsigned int _tmp30=
_get_dyneither_size(xarr->elmts,sizeof(void*));void**_tmp31=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp30));struct _dyneither_ptr _tmp33=_tag_dyneither(_tmp31,sizeof(void*),_tmp30);{unsigned int _tmp32=_tmp30;unsigned int i;for(i=0;i < _tmp32;i ++){_tmp31[i]=(void*)f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),0)));}}_tmp33;});_tmp2F->num_elmts=xarr->num_elmts;_tmp2F;});
# 173
{int i=1;for(0;i < xarr->num_elmts;++ i){
*((void**)_check_dyneither_subscript(ans->elmts,sizeof(void*),i))=f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
return ans;};}
# 178
struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rmap_c(Cyc_Core_heap_region,f,env,xarr);}
# 182
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr){
struct _dyneither_ptr newarr=_tag_dyneither(0,0,0);
({struct _dyneither_ptr _tmp34=newarr;struct _dyneither_ptr _tmp35=xarr->elmts;newarr=_tmp35;xarr->elmts=_tmp34;});
xarr->num_elmts=0;
Cyc_Core_ufree((void**)_untag_dyneither_ptr(newarr,sizeof(void*),1));}
# 189
void Cyc_Xarray_delete(struct Cyc_Xarray_Xarray*xarr,int num){
if(num > Cyc_Xarray_length(xarr))
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp37;_tmp37.tag=Cyc_Core_Invalid_argument;_tmp37.f1=({const char*_tmp38="number deleted is greater than length of xarray";_tag_dyneither(_tmp38,sizeof(char),48);});_tmp37;});_tmp36;}));
xarr->num_elmts -=num;}
# 195
void Cyc_Xarray_remove(struct Cyc_Xarray_Xarray*xarr,int i){
if(i < 0  || i > xarr->num_elmts - 1)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp3A;_tmp3A.tag=Cyc_Core_Invalid_argument;_tmp3A.f1=({const char*_tmp3B="xarray index out of bounds";_tag_dyneither(_tmp3B,sizeof(char),27);});_tmp3A;});_tmp39;}));
{int j=i;for(0;j < xarr->num_elmts - 1;++ j){
*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),j))=*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),j + 1));}}
-- xarr->num_elmts;}
