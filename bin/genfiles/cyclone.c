#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[52];
#endif
extern int setjmp(jmp_buf);
#endif
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file
   lib/runtime_cyc.c
*/
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

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

/* Need one of these per thread (we don't have threads)
   The runtime maintains a stack that contains either _handler_cons
   structs or _RegionHandle structs.  The tag is 0 for a handler_cons
   and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; /* 0 for an exception handler, 1 for a region handle */
  struct _RuntimeStack *next;
};

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
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
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
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
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

/* the next two routines swap [x] and [y]; not thread safe! */
static _INLINE void _swap_word(void *x, void *y) {
  unsigned long *lx = (unsigned long *)x, *ly = (unsigned long *)y, tmp;
  tmp = *lx;
  *lx = *ly;
  *ly = tmp;
}
static _INLINE void _swap_dyneither(struct _dyneither_ptr *x, 
				   struct _dyneither_ptr *y) {
  struct _dyneither_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}

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
# 69 "core.h"
void exit(int);struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 84
int Cyc_feof(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 108
unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 140 "cycboot.h"
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 167
int remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 271 "cycboot.h"
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);
void Cyc_file_close(struct Cyc___cycFILE*);
# 300
int isspace(int);
# 313
char*getenv(const char*);
# 318
int system(const char*);
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 234 "core.h"
struct _dyneither_ptr Cstring_to_string(char*);
# 244
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 249
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 319
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 347
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 374
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 102 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 118
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
# 46
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 32 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 37
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 61
extern int Cyc_Position_use_gcc_style_location;
# 63
extern int Cyc_Position_max_errors;
# 65
int Cyc_Position_error_p();
# 80 "absyn.h"
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 104
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
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
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
# 155
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 176
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 181
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 188
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 208
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 210
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 219
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1172 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1174
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 32 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_FlatList{struct Cyc_FlatList*tl;};
# 36
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 47
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 52
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _tuple10*tl;struct _tuple11 hd;};
typedef struct _tuple10*Cyc_declarator_list_t;
# 55
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 59
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 67
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 71
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY20{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple16 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 75 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 31 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 33 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 35
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 42
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 51
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 70
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 74
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
# 85
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 96
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 114 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 44 "tcutil.h"
void Cyc_Tcutil_flush_warnings();
# 83
extern int Cyc_Tcutil_warn_alias_coerce;
# 86
extern int Cyc_Tcutil_warn_region_coerce;
# 338 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple18{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple18 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
# 79
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};
# 94
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct _dyneither_ptr f1;void*f2;};
# 140
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 147
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple19{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple19 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 163 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 182
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 261 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 35 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Interface_I;
# 37 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 40
struct Cyc_Interface_I*Cyc_Interface_empty();
# 49 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 53
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct _tuple20{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 61
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple20*info);
# 76 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 82
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 85
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 88
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 91
typedef struct Cyc_Interface_I*Cyc_interface_t;
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 42 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 45
extern int Cyc_Lex_compile_for_boot_r;
void Cyc_Lex_pos_init();
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 50
int Cyc_Toc_warn_bounds_checks;
int Cyc_Toc_warn_all_null_deref;
# 60
static int Cyc_pp_r=0;
int Cyc_noexpand_r=0;
static int Cyc_noshake_r=0;
static int Cyc_stop_after_cpp_r=0;
static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;
static int Cyc_cf_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_toc_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;
static int Cyc_elim_se_r=0;
static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;
static int Cyc_save_c_r=0;
static int Cyc_nogc_r=0;
static int Cyc_pa_r=0;
static int Cyc_pg_r=0;
static int Cyc_nocheck_r=0;
static int Cyc_add_cyc_namespace_r=1;
static int Cyc_generate_line_directives_r=1;
static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;
static int Cyc_print_all_kinds_r=0;
static int Cyc_print_all_effects_r=0;
static int Cyc_nocyc_setjmp_r=0;
static int Cyc_generate_interface_r=0;
# 91
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3D5;struct Cyc_List_List*_tmp3D4;Cyc_ccargs=((_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((_tmp3D4->hd=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5[0]=s,_tmp3D5)))),((_tmp3D4->tl=Cyc_ccargs,_tmp3D4))))));}
# 99
void Cyc_set_c_compiler(struct _dyneither_ptr s){
const char*_tmp3D6;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3D6="vc",_tag_dyneither(_tmp3D6,sizeof(char),3))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;{
const char*_tmp3D7;Cyc_add_ccarg(((_tmp3D7="-DVC_C",_tag_dyneither(_tmp3D7,sizeof(char),7))));};}else{
# 104
const char*_tmp3D8;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3D8="gcc",_tag_dyneither(_tmp3D8,sizeof(char),4))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{
const char*_tmp3D9;Cyc_add_ccarg(((_tmp3D9="-DGCC_C",_tag_dyneither(_tmp3D9,sizeof(char),8))));};}}}
# 112
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 120
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3DA;Cyc_output_file=((_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA[0]=s,_tmp3DA))));}
# 125
static struct _dyneither_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _dyneither_ptr s){
Cyc_specified_interface=s;}
# 134
extern char*Cdef_inc_path;
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Ccomp;
extern char*Cversion;static char _tmp7[1]="";
# 141
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s){
Cyc_cpp=s;}
# 146
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3DD;struct Cyc_List_List*_tmp3DC;Cyc_cppargs=((_tmp3DC=_cycalloc(sizeof(*_tmp3DC)),((_tmp3DC->hd=((_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD[0]=s,_tmp3DD)))),((_tmp3DC->tl=Cyc_cppargs,_tmp3DC))))));}
# 151
static void Cyc_add_iprefix(struct _dyneither_ptr s){
const char*_tmp3E1;void*_tmp3E0[1];struct Cyc_String_pa_PrintArg_struct _tmp3DF;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3DF.tag=0,((_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E0[0]=& _tmp3DF,Cyc_aprintf(((_tmp3E1="-iprefix %s",_tag_dyneither(_tmp3E1,sizeof(char),12))),_tag_dyneither(_tmp3E0,sizeof(void*),1)))))))));}
# 154
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
const char*_tmp3E5;void*_tmp3E4[1];struct Cyc_String_pa_PrintArg_struct _tmp3E3;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E3.tag=0,((_tmp3E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E4[0]=& _tmp3E3,Cyc_aprintf(((_tmp3E5="-iwithprefix %s",_tag_dyneither(_tmp3E5,sizeof(char),16))),_tag_dyneither(_tmp3E4,sizeof(void*),1)))))))));}
# 157
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
const char*_tmp3E9;void*_tmp3E8[1];struct Cyc_String_pa_PrintArg_struct _tmp3E7;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E7.tag=0,((_tmp3E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E8[0]=& _tmp3E7,Cyc_aprintf(((_tmp3E9="-iwithprefixbefore %s",_tag_dyneither(_tmp3E9,sizeof(char),22))),_tag_dyneither(_tmp3E8,sizeof(void*),1)))))))));}
# 160
static void Cyc_add_isystem(struct _dyneither_ptr s){
const char*_tmp3ED;void*_tmp3EC[1];struct Cyc_String_pa_PrintArg_struct _tmp3EB;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3EB.tag=0,((_tmp3EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3EC[0]=& _tmp3EB,Cyc_aprintf(((_tmp3ED="-isystem %s",_tag_dyneither(_tmp3ED,sizeof(char),12))),_tag_dyneither(_tmp3EC,sizeof(void*),1)))))))));}
# 163
static void Cyc_add_idirafter(struct _dyneither_ptr s){
const char*_tmp3F1;void*_tmp3F0[1];struct Cyc_String_pa_PrintArg_struct _tmp3EF;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3EF.tag=0,((_tmp3EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3F0[0]=& _tmp3EF,Cyc_aprintf(((_tmp3F1="-idirafter %s",_tag_dyneither(_tmp3F1,sizeof(char),14))),_tag_dyneither(_tmp3F0,sizeof(void*),1)))))))));}
# 167
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _dyneither_ptr s){
Cyc_toc_r=1;
Cyc_target_arch=s;}
# 174
static void Cyc_print_version(){
{const char*_tmp3F5;void*_tmp3F4[1];struct Cyc_String_pa_PrintArg_struct _tmp3F3;(_tmp3F3.tag=0,((_tmp3F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp3F4[0]=& _tmp3F3,Cyc_printf(((_tmp3F5="The Cyclone compiler, version %s\n",_tag_dyneither(_tmp3F5,sizeof(char),34))),_tag_dyneither(_tmp3F4,sizeof(void*),1)))))));}
{const char*_tmp3FA;void*_tmp3F9[2];struct Cyc_String_pa_PrintArg_struct _tmp3F8;struct Cyc_String_pa_PrintArg_struct _tmp3F7;(_tmp3F7.tag=0,((_tmp3F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3F8.tag=0,((_tmp3F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3F9[0]=& _tmp3F8,((_tmp3F9[1]=& _tmp3F7,Cyc_printf(((_tmp3FA="Compiled for architecture: %s CC=\"%s\"\n",_tag_dyneither(_tmp3FA,sizeof(char),39))),_tag_dyneither(_tmp3F9,sizeof(void*),2)))))))))))));}
{const char*_tmp3FE;void*_tmp3FD[1];struct Cyc_String_pa_PrintArg_struct _tmp3FC;(_tmp3FC.tag=0,((_tmp3FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path)),((_tmp3FD[0]=& _tmp3FC,Cyc_printf(((_tmp3FE="Standard library directory: %s\n",_tag_dyneither(_tmp3FE,sizeof(char),32))),_tag_dyneither(_tmp3FD,sizeof(void*),1)))))));}
{const char*_tmp402;void*_tmp401[1];struct Cyc_String_pa_PrintArg_struct _tmp400;(_tmp400.tag=0,((_tmp400.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path)),((_tmp401[0]=& _tmp400,Cyc_printf(((_tmp402="Standard include directory: %s\n",_tag_dyneither(_tmp402,sizeof(char),32))),_tag_dyneither(_tmp401,sizeof(void*),1)))))));}
exit(0);}
# 183
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp26=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp26 <= 4)return 0;else{
const char*_tmp403;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp26 - 4)),((_tmp403=".cyc",_tag_dyneither(_tmp403,sizeof(char),5))))== 0;}}
# 189
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp28=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp28 <= 2)return;{
struct _dyneither_ptr _tmp29=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp28 - 2);
struct _dyneither_ptr*_tmp406;struct Cyc_List_List*_tmp405;Cyc_cyclone_exec_path=((_tmp405=_cycalloc(sizeof(*_tmp405)),((_tmp405->hd=((_tmp406=_cycalloc(sizeof(*_tmp406)),((_tmp406[0]=_tmp29,_tmp406)))),((_tmp405->tl=Cyc_cyclone_exec_path,_tmp405))))));};}
# 202
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
const char*_tmp407;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp407="-lxml",_tag_dyneither(_tmp407,sizeof(char),6))))== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 208
const char*_tmp408;Cyc_add_ccarg(((_tmp408="-lxml_a",_tag_dyneither(_tmp408,sizeof(char),8))));}}else{
# 210
struct _dyneither_ptr*_tmp40B;struct Cyc_List_List*_tmp40A;Cyc_libargs=((_tmp40A=_cycalloc(sizeof(*_tmp40A)),((_tmp40A->hd=((_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B[0]=s,_tmp40B)))),((_tmp40A->tl=Cyc_libargs,_tmp40A))))));}}
# 213
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 217
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{
const char*_tmp40C;Cyc_add_ccarg(((_tmp40C="-save-temps",_tag_dyneither(_tmp40C,sizeof(char),12))));};}
# 222
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;{
const char*_tmp40D;Cyc_add_cpparg(((_tmp40D="-M",_tag_dyneither(_tmp40D,sizeof(char),3))));};}
# 229
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp40E;Cyc_dependencies_target=((_tmp40E=_cycalloc(sizeof(*_tmp40E)),((_tmp40E[0]=s,_tmp40E))));}
# 234
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;{
const char*_tmp40F;Cyc_add_ccarg(((_tmp40F="-c",_tag_dyneither(_tmp40F,sizeof(char),3))));};}
# 239
static void Cyc_set_nocppprecomp(){
{const char*_tmp410;Cyc_add_cpparg(((_tmp410="-no-cpp-precomp",_tag_dyneither(_tmp410,sizeof(char),16))));}{
const char*_tmp411;Cyc_add_ccarg(((_tmp411="-no-cpp-precomp",_tag_dyneither(_tmp411,sizeof(char),16))));};}
# 244
static void Cyc_clear_lineno(){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 248
static void Cyc_set_inline_functions(){
Cyc_inline_functions_r=1;}
# 251
static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;
Cyc_set_inline_functions();}
# 255
static void Cyc_set_tovc(){
Cyc_Cyclone_tovc_r=1;
{const char*_tmp412;Cyc_add_ccarg(((_tmp412="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp412,sizeof(char),18))));}
Cyc_set_elim_se();}
# 260
static void Cyc_set_noboundschecks(){
const char*_tmp413;Cyc_add_ccarg(((_tmp413="-DNO_CYC_BOUNDS_CHECKS",_tag_dyneither(_tmp413,sizeof(char),23))));}
# 263
static void Cyc_set_nonullchecks(){
const char*_tmp414;Cyc_add_ccarg(((_tmp414="-DNO_CYC_NULL_CHECKS",_tag_dyneither(_tmp414,sizeof(char),21))));}
# 266
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 272
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;{
const char*_tmp415;Cyc_add_ccarg(((_tmp415="-DNO_CYC_PREFIX",_tag_dyneither(_tmp415,sizeof(char),16))));};}
# 277
static void Cyc_set_pa(){
Cyc_pa_r=1;
{const char*_tmp416;Cyc_add_ccarg(((_tmp416="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp416,sizeof(char),21))));}{
const char*_tmp417;Cyc_add_cpparg(((_tmp417="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp417,sizeof(char),21))));};}
# 283
static void Cyc_set_pg(){
Cyc_pg_r=1;{
const char*_tmp418;Cyc_add_ccarg(((_tmp418="-pg",_tag_dyneither(_tmp418,sizeof(char),4))));};}
# 288
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;{
const char*_tmp419;Cyc_add_ccarg(((_tmp419="-S",_tag_dyneither(_tmp419,sizeof(char),3))));};}
# 293
static void Cyc_set_all_warnings(){
Cyc_Toc_warn_bounds_checks=1;
Cyc_Toc_warn_all_null_deref=1;
Cyc_NewControlFlow_warn_lose_unique=1;
Cyc_Tcutil_warn_alias_coerce=1;
Cyc_Tcutil_warn_region_coerce=1;}
# 302
enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE  = 1};
# 306
typedef enum Cyc_inputtype Cyc_inputtype_t;
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _dyneither_ptr s){
const char*_tmp41A;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp41A="cyc",_tag_dyneither(_tmp41A,sizeof(char),4))))== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp41B;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp41B="none",_tag_dyneither(_tmp41B,sizeof(char),5))))== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 314
const char*_tmp41F;void*_tmp41E[1];struct Cyc_String_pa_PrintArg_struct _tmp41D;(_tmp41D.tag=0,((_tmp41D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp41E[0]=& _tmp41D,Cyc_fprintf(Cyc_stderr,((_tmp41F="Input type '%s' not supported\n",_tag_dyneither(_tmp41F,sizeof(char),31))),_tag_dyneither(_tmp41E,sizeof(void*),1)))))));}}}
# 318
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct _dyneither_ptr*output_file){
struct _dyneither_ptr _tmp43=Cyc_Filename_dirname(output_file == 0?s:*output_file);
struct _dyneither_ptr _tmp44=Cyc_Filename_chop_extension((struct _dyneither_ptr)Cyc_Filename_basename(s));
# 324
struct _dyneither_ptr _tmp45=Cyc_strlen((struct _dyneither_ptr)_tmp43)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp43,(struct _dyneither_ptr)_tmp44): _tmp44;
return(struct _dyneither_ptr)_tmp45;}
# 330
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 332
static void Cyc_add_other(struct _dyneither_ptr s){
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 335
{struct _dyneither_ptr*_tmp422;struct Cyc_List_List*_tmp421;Cyc_cyclone_files=((_tmp421=_cycalloc(sizeof(*_tmp421)),((_tmp421->hd=((_tmp422=_cycalloc(sizeof(*_tmp422)),((_tmp422[0]=s,_tmp422)))),((_tmp421->tl=Cyc_cyclone_files,_tmp421))))));}{
# 338
struct _dyneither_ptr _tmp48=Cyc_make_base_filename(s,Cyc_output_file);
const char*_tmp423;struct _dyneither_ptr _tmp49=Cyc_strconcat((struct _dyneither_ptr)_tmp48,((_tmp423=".c",_tag_dyneither(_tmp423,sizeof(char),3))));
Cyc_add_ccarg((struct _dyneither_ptr)_tmp49);};}else{
# 342
Cyc_add_ccarg(s);}}
# 345
static void Cyc_remove_file(struct _dyneither_ptr s){
if(Cyc_save_temps_r)return;else{
remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple21{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 356
struct Cyc_List_List*Cyc_read_specs(struct _dyneither_ptr file){
struct Cyc_List_List*_tmp4B=0;
int c;
int i;
char strname[256];
char strvalue[4096];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_untag_dyneither_ptr(file,sizeof(char),1),"r");
# 364
if(spec_file == 0){
{const char*_tmp427;void*_tmp426[1];struct Cyc_String_pa_PrintArg_struct _tmp425;(_tmp425.tag=0,((_tmp425.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp426[0]=& _tmp425,Cyc_fprintf(Cyc_stderr,((_tmp427="Error opening spec file %s\n",_tag_dyneither(_tmp427,sizeof(char),28))),_tag_dyneither(_tmp426,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 0;}
# 370
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
if(c == '*')break;
if(c != - 1){
{const char*_tmp42C;void*_tmp42B[2];struct Cyc_String_pa_PrintArg_struct _tmp42A;struct Cyc_Int_pa_PrintArg_struct _tmp429;(_tmp429.tag=1,((_tmp429.f1=(unsigned long)c,((_tmp42A.tag=0,((_tmp42A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp42B[0]=& _tmp42A,((_tmp42B[1]=& _tmp429,Cyc_fprintf(Cyc_stderr,((_tmp42C="Error reading spec file %s: unexpected character '%c'\n",_tag_dyneither(_tmp42C,sizeof(char),55))),_tag_dyneither(_tmp42B,sizeof(void*),2)))))))))))));}
# 379
Cyc_fflush(Cyc_stderr);}
# 381
goto CLEANUP_AND_RETURN;}
# 383
JUST_AFTER_STAR:
 i=0;
while(1){
c=Cyc_fgetc(spec_file);
if(c == - 1){
{const char*_tmp430;void*_tmp42F[1];struct Cyc_String_pa_PrintArg_struct _tmp42E;(_tmp42E.tag=0,((_tmp42E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp42F[0]=& _tmp42E,Cyc_fprintf(Cyc_stderr,((_tmp430="Error reading spec file %s: unexpected EOF\n",_tag_dyneither(_tmp430,sizeof(char),44))),_tag_dyneither(_tmp42F,sizeof(void*),1)))))));}
# 391
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}
# 394
if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';
break;}
# 398
strname[_check_known_subscript_notnull(256,i)]=(char)c;
++ i;
if(i >= 256){
{const char*_tmp434;void*_tmp433[1];struct Cyc_String_pa_PrintArg_struct _tmp432;(_tmp432.tag=0,((_tmp432.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp433[0]=& _tmp432,Cyc_fprintf(Cyc_stderr,((_tmp434="Error reading spec file %s: string name too long\n",_tag_dyneither(_tmp434,sizeof(char),50))),_tag_dyneither(_tmp433,sizeof(void*),1)))))));}
# 404
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 408
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
break;}
# 413
if(c == '*'){
# 415
{struct _tuple21*_tmp441;struct _dyneither_ptr*_tmp440;const char*_tmp43F;struct _dyneither_ptr*_tmp43E;struct Cyc_List_List*_tmp43D;_tmp4B=((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D->hd=((_tmp441=_cycalloc(sizeof(*_tmp441)),((_tmp441->f1=((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp440)))),((_tmp441->f2=(
(_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E[0]=(struct _dyneither_ptr)Cyc_strdup(((_tmp43F="",_tag_dyneither(_tmp43F,sizeof(char),1)))),_tmp43E)))),_tmp441)))))),((_tmp43D->tl=_tmp4B,_tmp43D))))));}
goto JUST_AFTER_STAR;}
# 420
strvalue[0]=(char)c;
i=1;
while(1){
c=Cyc_fgetc(spec_file);
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
++ i;
if(i >= 4096){
{const char*_tmp446;void*_tmp445[2];struct Cyc_String_pa_PrintArg_struct _tmp444;struct Cyc_String_pa_PrintArg_struct _tmp443;(_tmp443.tag=0,((_tmp443.f1=(struct _dyneither_ptr)
# 430
_tag_dyneither(strname,sizeof(char),256),((_tmp444.tag=0,((_tmp444.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp445[0]=& _tmp444,((_tmp445[1]=& _tmp443,Cyc_fprintf(Cyc_stderr,((_tmp446="Error reading spec file %s: value of %s too long\n",_tag_dyneither(_tmp446,sizeof(char),50))),_tag_dyneither(_tmp445,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 436
{struct _tuple21*_tmp44F;struct _dyneither_ptr*_tmp44E;struct _dyneither_ptr*_tmp44D;struct Cyc_List_List*_tmp44C;_tmp4B=((_tmp44C=_cycalloc(sizeof(*_tmp44C)),((_tmp44C->hd=((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F->f1=((_tmp44E=_cycalloc(sizeof(*_tmp44E)),((_tmp44E[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp44E)))),((_tmp44F->f2=(
(_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096)),_tmp44D)))),_tmp44F)))))),((_tmp44C->tl=_tmp4B,_tmp44C))))));}
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 441
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp4B;}struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline);static void _tmp459(struct _dyneither_ptr*_tmp6C,unsigned int*_tmp458,unsigned int*_tmp457,struct _dyneither_ptr**_tmp455){for(*_tmp458=0;*_tmp458 < *_tmp457;(*_tmp458)++){(*_tmp455)[*_tmp458]=*((struct _dyneither_ptr**)(*_tmp6C).curr)[(int)*_tmp458];}}
# 447
struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline){
if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);{
unsigned long _tmp66=Cyc_strlen((struct _dyneither_ptr)cmdline);
struct Cyc_List_List*_tmp67=0;
char buf[4096];
int i=0;
int j=0;
if(_tmp66 > 4096)goto DONE;
while(1){
# 457
while(1){
if(i >= _tmp66)goto DONE;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
if(!isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
++ i;}
# 463
j=0;
# 468
while(1){
if(i >= _tmp66)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
if(isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){
# 474
++ i;
if(i >= _tmp66)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}else{
# 481
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}
# 484
++ i;}
# 486
if(j < 4096)
buf[_check_known_subscript_notnull(4096,j)]='\000';{
# 489
struct _dyneither_ptr*_tmp452;struct Cyc_List_List*_tmp451;_tmp67=((_tmp451=_cycalloc(sizeof(*_tmp451)),((_tmp451->hd=((_tmp452=_cycalloc(sizeof(*_tmp452)),((_tmp452[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096)),_tmp452)))),((_tmp451->tl=_tmp67,_tmp451))))));};}
# 491
DONE:
 _tmp67=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp67);
{struct Cyc_List_List*_tmp453;_tmp67=((_tmp453=_cycalloc(sizeof(*_tmp453)),((_tmp453->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmp453->tl=_tmp67,_tmp453))))));}{
struct _dyneither_ptr _tmp6C=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp67);
unsigned int _tmp458;unsigned int _tmp457;struct _dyneither_ptr _tmp456;struct _dyneither_ptr*_tmp455;unsigned int _tmp454;struct _dyneither_ptr _tmp6D=(_tmp454=_get_dyneither_size(_tmp6C,sizeof(struct _dyneither_ptr*)),((_tmp455=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp454)),((_tmp456=_tag_dyneither(_tmp455,sizeof(struct _dyneither_ptr),_tmp454),((((_tmp457=_tmp454,_tmp459(& _tmp6C,& _tmp458,& _tmp457,& _tmp455))),_tmp456)))))));
return _tmp6D;};};}
# 501
int Cyc_compile_failure=0;
# 503
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp72;_push_handler(& _tmp72);{int _tmp74=0;if(setjmp(_tmp72.handler))_tmp74=1;if(!_tmp74){{struct Cyc___cycFILE*_tmp75=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp75;};_pop_handler();}else{void*_tmp73=(void*)_exn_thrown;void*_tmp77=_tmp73;void*_tmp78;_LL1:;_LL2:
# 507
 Cyc_compile_failure=1;
{const char*_tmp45E;void*_tmp45D[2];struct Cyc_String_pa_PrintArg_struct _tmp45C;struct Cyc_String_pa_PrintArg_struct _tmp45B;(_tmp45B.tag=0,((_tmp45B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp45C.tag=0,((_tmp45C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp45D[0]=& _tmp45C,((_tmp45D[1]=& _tmp45B,Cyc_fprintf(Cyc_stderr,((_tmp45E="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp45E,sizeof(char),29))),_tag_dyneither(_tmp45D,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
return 0;_LL3: _tmp78=_tmp77;_LL4:(void)_throw(_tmp78);_LL0:;}};}
# 514
void CYCALLOCPROFILE_mark(const char*s);
# 516
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){
# 520
const char*_tmp45F;struct _dyneither_ptr exn_string=(_tmp45F="",_tag_dyneither(_tmp45F,sizeof(char),1));
const char*_tmp460;struct _dyneither_ptr explain_string=(_tmp460="",_tag_dyneither(_tmp460,sizeof(char),1));
int other_exn=0;
struct Cyc_Core_Impossible_exn_struct _tmp466;const char*_tmp465;struct Cyc_Core_Impossible_exn_struct*_tmp464;void*ex=(void*)((_tmp464=_cycalloc(sizeof(*_tmp464)),((_tmp464[0]=((_tmp466.tag=Cyc_Core_Impossible,((_tmp466.f1=((_tmp465="",_tag_dyneither(_tmp465,sizeof(char),1))),_tmp466)))),_tmp464))));
struct Cyc_List_List*_tmp7D=0;
# 526
CYCALLOCPROFILE_mark((const char*)_check_null(_untag_dyneither_ptr(stage_name,sizeof(char),1)));
# 528
{struct _handler_cons _tmp7E;_push_handler(& _tmp7E);{int _tmp80=0;if(setjmp(_tmp7E.handler))_tmp80=1;if(!_tmp80){_tmp7D=f(env,tds);;_pop_handler();}else{void*_tmp7F=(void*)_exn_thrown;void*_tmp82=_tmp7F;struct _dyneither_ptr _tmp84;struct _dyneither_ptr _tmp87;struct _dyneither_ptr _tmp89;void*_tmp8A;void*_tmp8B;_LL6: {struct Cyc_Core_Impossible_exn_struct*_tmp83=(struct Cyc_Core_Impossible_exn_struct*)_tmp82;if(_tmp83->tag != Cyc_Core_Impossible)goto _LL8;else{_tmp84=_tmp83->f1;}}_LL7:
# 531
{const char*_tmp467;exn_string=((_tmp467="Exception Core::Impossible",_tag_dyneither(_tmp467,sizeof(char),27)));}
explain_string=_tmp84;
goto _LL5;_LL8: {struct Cyc_Dict_Absent_exn_struct*_tmp85=(struct Cyc_Dict_Absent_exn_struct*)_tmp82;if(_tmp85->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:
# 535
{const char*_tmp468;exn_string=((_tmp468="Exception Dict::Absent",_tag_dyneither(_tmp468,sizeof(char),23)));}
goto _LL5;_LLA: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp86=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp82;if(_tmp86->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp87=_tmp86->f1;}}_LLB:
# 538
{const char*_tmp469;exn_string=((_tmp469="Exception Core::Invalid_argument",_tag_dyneither(_tmp469,sizeof(char),33)));}
explain_string=_tmp87;
goto _LL5;_LLC: {struct Cyc_Core_Failure_exn_struct*_tmp88=(struct Cyc_Core_Failure_exn_struct*)_tmp82;if(_tmp88->tag != Cyc_Core_Failure)goto _LLE;else{_tmp89=_tmp88->f1;}}_LLD:
# 542
{const char*_tmp46A;exn_string=((_tmp46A="Exception Core::Failure",_tag_dyneither(_tmp46A,sizeof(char),24)));}
explain_string=_tmp89;
goto _LL5;_LLE: _tmp8A=_tmp82;_LLF:
# 546
 ex=_tmp8A;
other_exn=1;
{const char*_tmp46B;exn_string=((_tmp46B="Uncaught exception",_tag_dyneither(_tmp46B,sizeof(char),19)));}
goto _LL5;_LL10: _tmp8B=_tmp82;_LL11:(void)_throw(_tmp8B);_LL5:;}};}
# 551
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
{const char*_tmp46C;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp46C="",_tag_dyneither(_tmp46C,sizeof(char),1))))!= 0){
Cyc_compile_failure=1;{
const char*_tmp472;void*_tmp471[3];struct Cyc_String_pa_PrintArg_struct _tmp470;struct Cyc_String_pa_PrintArg_struct _tmp46F;struct Cyc_String_pa_PrintArg_struct _tmp46E;(_tmp46E.tag=0,((_tmp46E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp46F.tag=0,((_tmp46F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp470.tag=0,((_tmp470.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp471[0]=& _tmp470,((_tmp471[1]=& _tmp46F,((_tmp471[2]=& _tmp46E,Cyc_fprintf(Cyc_stderr,((_tmp472="\n%s thrown during %s: %s",_tag_dyneither(_tmp472,sizeof(char),25))),_tag_dyneither(_tmp471,sizeof(void*),3)))))))))))))))))));};}}
# 558
if(Cyc_compile_failure){
{const char*_tmp475;void*_tmp474;(_tmp474=0,Cyc_fprintf(Cyc_stderr,((_tmp475="\nCOMPILATION FAILED!\n",_tag_dyneither(_tmp475,sizeof(char),22))),_tag_dyneither(_tmp474,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);
if(other_exn)
(int)_throw(ex);
return _tmp7D;}else{
# 565
if(Cyc_v_r){
{const char*_tmp479;void*_tmp478[1];struct Cyc_String_pa_PrintArg_struct _tmp477;(_tmp477.tag=0,((_tmp477.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp478[0]=& _tmp477,Cyc_fprintf(Cyc_stderr,((_tmp479="%s completed.\n",_tag_dyneither(_tmp479,sizeof(char),15))),_tag_dyneither(_tmp478,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return _tmp7D;}}
# 570
return _tmp7D;}
# 573
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*_tmpA1=Cyc_Parse_parse_file(f);
Cyc_Lex_lex_init(1);
return _tmpA1;};}struct _tuple22{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 581
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple22*tcenv,struct Cyc_List_List*tds){
# 583
struct _RegionHandle*_tmpA3;struct Cyc_Tcenv_Tenv*_tmpA4;struct _tuple22 _tmpA2=*tcenv;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;
Cyc_Tc_tc(_tmpA3,_tmpA4,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmpA4,tds);
return tds;}
# 590
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);
return tds;}struct _tuple23{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 597
struct Cyc_List_List*Cyc_do_interface(struct _tuple23*params,struct Cyc_List_List*tds){
struct Cyc_Tcenv_Tenv*_tmpA6;struct Cyc___cycFILE*_tmpA7;struct Cyc___cycFILE*_tmpA8;struct _tuple23*_tmpA5=params;_tmpA6=_tmpA5->f1;_tmpA7=_tmpA5->f2;_tmpA8=_tmpA5->f3;{
struct Cyc_Interface_I*_tmpA9=Cyc_Interface_extract(_tmpA6->ae);
if(_tmpA7 == 0)
Cyc_Interface_save(_tmpA9,_tmpA8);else{
# 603
struct Cyc_Interface_I*_tmpAA=Cyc_Interface_parse(_tmpA7);
const char*_tmp47E;const char*_tmp47D;struct _tuple20*_tmp47C;if(!Cyc_Interface_is_subinterface(_tmpAA,_tmpA9,((_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C->f1=((_tmp47D="written interface",_tag_dyneither(_tmp47D,sizeof(char),18))),((_tmp47C->f2=((_tmp47E="maximal interface",_tag_dyneither(_tmp47E,sizeof(char),18))),_tmp47C))))))))
Cyc_compile_failure=1;else{
# 607
Cyc_Interface_save(_tmpAA,_tmpA8);}}
# 609
return tds;};}
# 612
struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*tds){
return Cyc_Toc_toc(tds);}
# 615
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 620
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 622
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 624
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
Cyc_cyc_include=f;}
# 629
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 632
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{const char*_tmp481;void*_tmp480;(_tmp480=0,Cyc_fprintf(Cyc_stderr,((_tmp481="Internal error: copy_internal_file called with NULL\n",_tag_dyneither(_tmp481,sizeof(char),53))),_tag_dyneither(_tmp480,sizeof(void*),0)));}
return 1;}{
# 636
const char*_tmp483;const char*_tmp482;struct Cyc___cycFILE*_tmpB0=Cyc_try_file_open(file,((_tmp482="r",_tag_dyneither(_tmp482,sizeof(char),2))),((_tmp483="internal compiler file",_tag_dyneither(_tmp483,sizeof(char),23))));
if(_tmpB0 == 0)return 1;{
unsigned long n_read=1024;
unsigned long n_written;
char buf[1024];
{unsigned int _tmpB7=1024;unsigned int i;for(i=0;i < _tmpB7;i ++){buf[i]='\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,_tmpB0);
if(n_read != 1024  && !Cyc_feof(_tmpB0)){
Cyc_compile_failure=1;
{const char*_tmp487;void*_tmp486[1];struct Cyc_String_pa_PrintArg_struct _tmp485;(_tmp485.tag=0,((_tmp485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp486[0]=& _tmp485,Cyc_fprintf(Cyc_stderr,((_tmp487="\nError: problem copying %s\n",_tag_dyneither(_tmp487,sizeof(char),28))),_tag_dyneither(_tmp486,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}
# 649
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
if(n_read != n_written){
Cyc_compile_failure=1;
{const char*_tmp48B;void*_tmp48A[1];struct Cyc_String_pa_PrintArg_struct _tmp489;(_tmp489.tag=0,((_tmp489.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp48A[0]=& _tmp489,Cyc_fprintf(Cyc_stderr,((_tmp48B="\nError: problem copying %s\n",_tag_dyneither(_tmp48B,sizeof(char),28))),_tag_dyneither(_tmp48A,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}}
# 657
Cyc_fclose(_tmpB0);
return 0;};};}
# 662
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){
struct Cyc_Absynpp_Params _tmpBA=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmpBA.expand_typedefs=!Cyc_noexpand_r;
_tmpBA.to_VC=Cyc_Cyclone_tovc_r;
_tmpBA.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmpBA.generate_line_directives=Cyc_generate_line_directives_r;
_tmpBA.print_full_evars=Cyc_print_full_evars_r;
_tmpBA.print_all_tvars=Cyc_print_all_tvars_r;
_tmpBA.print_all_kinds=Cyc_print_all_kinds_r;
_tmpBA.print_all_effects=Cyc_print_all_effects_r;
# 673
if(Cyc_inline_functions_r){
const char*_tmp48E;void*_tmp48D;(_tmp48D=0,Cyc_fprintf(out_file,((_tmp48E="#define _INLINE_FUNCTIONS\n",_tag_dyneither(_tmp48E,sizeof(char),27))),_tag_dyneither(_tmp48D,sizeof(void*),0)));}
# 677
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
# 684
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}
# 686
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 688
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpBA);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 692
Cyc_Absyndump_set_params(& _tmpBA);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 695
Cyc_fflush(out_file);
return tds;}
# 701
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 709
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;{
struct Cyc_List_List*_tmpBD=0;
unsigned long _tmpBE=Cyc_strlen((struct _dyneither_ptr)s);
# 714
while(_tmpBE > 0){
struct _dyneither_ptr _tmpBF=Cyc_strchr(s,c);
if((char*)_tmpBF.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{struct _dyneither_ptr*_tmp491;struct Cyc_List_List*_tmp490;_tmpBD=((_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490->hd=((_tmp491=_cycalloc(sizeof(*_tmp491)),((_tmp491[0]=s,_tmp491)))),((_tmp490->tl=_tmpBD,_tmp490))))));}
break;}else{
# 721
{struct _dyneither_ptr*_tmp494;struct Cyc_List_List*_tmp493;_tmpBD=((_tmp493=_cycalloc(sizeof(*_tmp493)),((_tmp493->hd=((_tmp494=_cycalloc(sizeof(*_tmp494)),((_tmp494[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpBF).curr - s.curr)/ sizeof(char))),_tmp494)))),((_tmp493->tl=_tmpBD,_tmp493))))));}
_tmpBE -=(((struct _dyneither_ptr)_tmpBF).curr - s.curr)/ sizeof(char);
s=_dyneither_ptr_plus(_tmpBF,sizeof(char),1);}}
# 726
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBD);};}
# 730
static int Cyc_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmpC4;_push_handler(& _tmpC4);{int _tmpC6=0;if(setjmp(_tmpC4.handler))_tmpC6=1;if(!_tmpC6){{const char*_tmp495;f=Cyc_file_open(file,((_tmp495="r",_tag_dyneither(_tmp495,sizeof(char),2))));};_pop_handler();}else{void*_tmpC5=(void*)_exn_thrown;void*_tmpC9=_tmpC5;void*_tmpCA;_LL13:;_LL14: goto _LL12;_LL15: _tmpCA=_tmpC9;_LL16:(void)_throw(_tmpCA);_LL12:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 738
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
const char*_tmp496;struct _dyneither_ptr tmp=(_tmp496="",_tag_dyneither(_tmp496,sizeof(char),1));
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpCB=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpCB.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCB)== 0)continue;
{const char*_tmp497;_tmpCB=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCB,((_tmp497=":",_tag_dyneither(_tmp497,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCB,(struct _dyneither_ptr)tmp);}
# 746
return tmp;}
# 751
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpCE=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpCE.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCE)== 0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpCE,file);
if(Cyc_file_exists(s)){struct _dyneither_ptr*_tmp498;return(_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498[0]=s,_tmp498)));}};}
# 759
return 0;}
# 762
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmpD0=Cyc_find(dirs,file);
if(_tmpD0 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
{const char*_tmp49D;void*_tmp49C[2];struct Cyc_String_pa_PrintArg_struct _tmp49B;struct Cyc_String_pa_PrintArg_struct _tmp49A;(_tmp49A.tag=0,((_tmp49A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp49B.tag=0,((_tmp49B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp49C[0]=& _tmp49B,((_tmp49C[1]=& _tmp49A,Cyc_fprintf(Cyc_stderr,((_tmp49D="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp49D,sizeof(char),56))),_tag_dyneither(_tmp49C,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Failure_exn_struct _tmp4AD;const char*_tmp4AC;void*_tmp4AB[2];struct Cyc_String_pa_PrintArg_struct _tmp4AA;struct Cyc_String_pa_PrintArg_struct _tmp4A9;struct Cyc_Core_Failure_exn_struct*_tmp4A8;(int)_throw((void*)((_tmp4A8=_cycalloc(sizeof(*_tmp4A8)),((_tmp4A8[0]=((_tmp4AD.tag=Cyc_Core_Failure,((_tmp4AD.f1=(struct _dyneither_ptr)((_tmp4A9.tag=0,((_tmp4A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp4AA.tag=0,((_tmp4AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp4AB[0]=& _tmp4AA,((_tmp4AB[1]=& _tmp4A9,Cyc_aprintf(((_tmp4AC="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp4AC,sizeof(char),56))),_tag_dyneither(_tmp4AB,sizeof(void*),2)))))))))))))),_tmp4AD)))),_tmp4A8)))));};}
# 774
return*_tmpD0;}
# 779
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 781
struct Cyc_List_List*_tmpDB=0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr*_tmp4B0;struct Cyc_List_List*_tmp4AF;_tmpDB=((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF->hd=((_tmp4B0=_cycalloc(sizeof(*_tmp4B0)),((_tmp4B0[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4B0)))),((_tmp4AF->tl=_tmpDB,_tmp4AF))))));}
# 785
_tmpDB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDB);
return _tmpDB;}
# 791
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 793
struct Cyc_List_List*_tmpDE=0;
for(0;dirs != 0;dirs=dirs->tl){
{struct Cyc_List_List*_tmp4B1;_tmpDE=((_tmp4B1=_cycalloc(sizeof(*_tmp4B1)),((_tmp4B1->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp4B1->tl=_tmpDE,_tmp4B1))))));}{
struct _dyneither_ptr*_tmp4B4;struct Cyc_List_List*_tmp4B3;_tmpDE=((_tmp4B3=_cycalloc(sizeof(*_tmp4B3)),((_tmp4B3->hd=((_tmp4B4=_cycalloc(sizeof(*_tmp4B4)),((_tmp4B4[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4B4)))),((_tmp4B3->tl=_tmpDE,_tmp4B3))))));};}
# 798
_tmpDE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDE);
return _tmpDE;}
# 803
static int Cyc_is_other_special(char c){
switch(c){case '\\': _LL17:
 goto _LL18;case '"': _LL18:
 goto _LL19;case ';': _LL19:
 goto _LL1A;case '&': _LL1A:
 goto _LL1B;case '(': _LL1B:
 goto _LL1C;case ')': _LL1C:
 goto _LL1D;case '|': _LL1D:
 goto _LL1E;case '^': _LL1E:
 goto _LL1F;case '<': _LL1F:
 goto _LL20;case '>': _LL20:
 goto _LL21;case '\n': _LL21:
# 818
 goto _LL22;case '\t': _LL22:
 return 1;default: _LL23:
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp4BD(unsigned int*_tmp4BC,unsigned int*_tmp4BB,char**_tmp4B9){for(*_tmp4BC=0;*_tmp4BC < *_tmp4BB;(*_tmp4BC)++){(*_tmp4B9)[*_tmp4BC]='\000';}}
# 825
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpE2=Cyc_strlen((struct _dyneither_ptr)s);
# 829
int _tmpE3=0;
int _tmpE4=0;
{int i=0;for(0;i < _tmpE2;++ i){
char _tmpE5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpE5 == '\'')++ _tmpE3;else{
if(Cyc_is_other_special(_tmpE5))++ _tmpE4;}}}
# 838
if(_tmpE3 == 0  && _tmpE4 == 0)
return s;
# 842
if(_tmpE3 == 0){
struct _dyneither_ptr*_tmp4B7;struct _dyneither_ptr*_tmp4B6[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp4B6[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp4B6[1]=((_tmp4B7=_cycalloc(sizeof(*_tmp4B7)),((_tmp4B7[0]=(struct _dyneither_ptr)s,_tmp4B7)))),((_tmp4B6[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B6,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 846
unsigned long _tmpEA=(_tmpE2 + _tmpE3)+ _tmpE4;
unsigned int _tmp4BC;unsigned int _tmp4BB;struct _dyneither_ptr _tmp4BA;char*_tmp4B9;unsigned int _tmp4B8;struct _dyneither_ptr s2=(_tmp4B8=_tmpEA + 1,((_tmp4B9=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp4B8 + 1)),((_tmp4BA=_tag_dyneither(_tmp4B9,sizeof(char),_tmp4B8 + 1),((((_tmp4BB=_tmp4B8,((_tmp4BD(& _tmp4BC,& _tmp4BB,& _tmp4B9),_tmp4B9[_tmp4BB]=(char)0)))),_tmp4BA)))))));
int _tmpEB=0;
int _tmpEC=0;
for(0;_tmpEB < _tmpE2;++ _tmpEB){
char _tmpED=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpEB));
if(_tmpED == '\''  || Cyc_is_other_special(_tmpED)){
char _tmp4C0;char _tmp4BF;struct _dyneither_ptr _tmp4BE;(_tmp4BE=_dyneither_ptr_plus(s2,sizeof(char),_tmpEC ++),((_tmp4BF=*((char*)_check_dyneither_subscript(_tmp4BE,sizeof(char),0)),((_tmp4C0='\\',((_get_dyneither_size(_tmp4BE,sizeof(char))== 1  && (_tmp4BF == '\000'  && _tmp4C0 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4BE.curr)=_tmp4C0)))))));}{
char _tmp4C3;char _tmp4C2;struct _dyneither_ptr _tmp4C1;(_tmp4C1=_dyneither_ptr_plus(s2,sizeof(char),_tmpEC ++),((_tmp4C2=*((char*)_check_dyneither_subscript(_tmp4C1,sizeof(char),0)),((_tmp4C3=_tmpED,((_get_dyneither_size(_tmp4C1,sizeof(char))== 1  && (_tmp4C2 == '\000'  && _tmp4C3 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4C1.curr)=_tmp4C3)))))));};}
# 856
return(struct _dyneither_ptr)s2;};}
# 858
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp4C4;return(_tmp4C4=_cycalloc(sizeof(*_tmp4C4)),((_tmp4C4[0]=Cyc_sh_escape_string(*sp),_tmp4C4)));}
# 862
static void Cyc_process_file(struct _dyneither_ptr filename){
# 864
struct _dyneither_ptr _tmpF9=Cyc_make_base_filename(filename,Cyc_output_file);
const char*_tmp4C5;struct _dyneither_ptr _tmpFA=Cyc_strconcat((struct _dyneither_ptr)_tmpF9,((_tmp4C5=".cyp",_tag_dyneither(_tmp4C5,sizeof(char),5))));
const char*_tmp4C6;struct _dyneither_ptr _tmpFB=(char*)Cyc_specified_interface.curr != (char*)(_tag_dyneither(0,0,0)).curr?Cyc_specified_interface:(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpF9,((_tmp4C6=".cyci",_tag_dyneither(_tmp4C6,sizeof(char),6))));
const char*_tmp4C7;struct _dyneither_ptr _tmpFC=Cyc_strconcat((struct _dyneither_ptr)_tmpF9,((_tmp4C7=".cycio",_tag_dyneither(_tmp4C7,sizeof(char),7))));
const char*_tmp4C8;struct _dyneither_ptr _tmpFD=Cyc_strconcat((struct _dyneither_ptr)_tmpF9,((_tmp4C8=".c",_tag_dyneither(_tmp4C8,sizeof(char),3))));
# 871
if(Cyc_v_r){const char*_tmp4CC;void*_tmp4CB[1];struct Cyc_String_pa_PrintArg_struct _tmp4CA;(_tmp4CA.tag=0,((_tmp4CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4CB[0]=& _tmp4CA,Cyc_fprintf(Cyc_stderr,((_tmp4CC="Compiling %s\n",_tag_dyneither(_tmp4CC,sizeof(char),14))),_tag_dyneither(_tmp4CB,sizeof(void*),1)))))));}{
# 874
const char*_tmp4CE;const char*_tmp4CD;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp4CD="r",_tag_dyneither(_tmp4CD,sizeof(char),2))),((_tmp4CE="input file",_tag_dyneither(_tmp4CE,sizeof(char),11))));
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 881
const char*_tmp4D5;struct _dyneither_ptr*_tmp4D4;const char*_tmp4D3;struct Cyc_List_List*_tmp4D2;struct _dyneither_ptr _tmp101=
Cyc_str_sepstr(((_tmp4D2=_cycalloc(sizeof(*_tmp4D2)),((_tmp4D2->hd=((_tmp4D4=_cycalloc(sizeof(*_tmp4D4)),((_tmp4D4[0]=(struct _dyneither_ptr)((_tmp4D3="",_tag_dyneither(_tmp4D3,sizeof(char),1))),_tmp4D4)))),((_tmp4D2->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp4D2)))))),(
(_tmp4D5=" ",_tag_dyneither(_tmp4D5,sizeof(char),2))));
# 891
struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmp102=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
{const char*_tmp4D6;_tmp102=Cyc_add_subdir(_tmp102,((_tmp4D6="include",_tag_dyneither(_tmp4D6,sizeof(char),8))));}
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0){
struct _dyneither_ptr*_tmp4D9;struct Cyc_List_List*_tmp4D8;_tmp102=((_tmp4D8=_cycalloc(sizeof(*_tmp4D8)),((_tmp4D8->hd=((_tmp4D9=_cycalloc(sizeof(*_tmp4D9)),((_tmp4D9[0]=def_inc_path,_tmp4D9)))),((_tmp4D8->tl=_tmp102,_tmp4D8))))));}{
# 897
char*_tmp106=getenv("CYCLONE_INCLUDE_PATH");
if(_tmp106 != 0){
char*_tmp4DA;_tmp102=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4DA=_tmp106,_tag_dyneither(_tmp4DA,sizeof(char),_get_zero_arr_size_char((void*)_tmp4DA,1)))),':'),_tmp102);}{
# 902
const char*_tmp4E1;struct _dyneither_ptr*_tmp4E0;const char*_tmp4DF;struct Cyc_List_List*_tmp4DE;struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
Cyc_str_sepstr(((_tmp4DE=_cycalloc(sizeof(*_tmp4DE)),((_tmp4DE->hd=((_tmp4E0=_cycalloc(sizeof(*_tmp4E0)),((_tmp4E0[0]=(struct _dyneither_ptr)((_tmp4DF="",_tag_dyneither(_tmp4DF,sizeof(char),1))),_tmp4E0)))),((_tmp4DE->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmp102),_tmp4DE)))))),(
(_tmp4E1=" -I",_tag_dyneither(_tmp4E1,sizeof(char),4))));
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){
const char*_tmp4E5;void*_tmp4E4[1];struct Cyc_String_pa_PrintArg_struct _tmp4E3;ofile_string=(struct _dyneither_ptr)((_tmp4E3.tag=0,((_tmp4E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((_tmp4E4[0]=& _tmp4E3,Cyc_aprintf(((_tmp4E5=" > %s",_tag_dyneither(_tmp4E5,sizeof(char),6))),_tag_dyneither(_tmp4E4,sizeof(void*),1))))))));}else{
# 911
const char*_tmp4E6;ofile_string=((_tmp4E6="",_tag_dyneither(_tmp4E6,sizeof(char),1)));}}else{
# 913
const char*_tmp4EA;void*_tmp4E9[1];struct Cyc_String_pa_PrintArg_struct _tmp4E8;ofile_string=(struct _dyneither_ptr)((_tmp4E8.tag=0,((_tmp4E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpFA)),((_tmp4E9[0]=& _tmp4E8,Cyc_aprintf(((_tmp4EA=" > %s",_tag_dyneither(_tmp4EA,sizeof(char),6))),_tag_dyneither(_tmp4E9,sizeof(void*),1))))))));}{
# 915
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 919
if(Cyc_dependencies_target == 0){
# 923
const char*_tmp4EB;fixup_string=((_tmp4EB=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",_tag_dyneither(_tmp4EB,sizeof(char),35)));}else{
# 928
const char*_tmp4EF;void*_tmp4EE[1];struct Cyc_String_pa_PrintArg_struct _tmp4ED;fixup_string=(struct _dyneither_ptr)(
(_tmp4ED.tag=0,((_tmp4ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4EE[0]=& _tmp4ED,Cyc_aprintf(((_tmp4EF=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4EF,sizeof(char),29))),_tag_dyneither(_tmp4EE,sizeof(void*),1))))))));}}else{
# 932
const char*_tmp4F0;fixup_string=((_tmp4F0="",_tag_dyneither(_tmp4F0,sizeof(char),1)));}{
# 934
const char*_tmp4F9;void*_tmp4F8[6];struct Cyc_String_pa_PrintArg_struct _tmp4F7;struct Cyc_String_pa_PrintArg_struct _tmp4F6;struct Cyc_String_pa_PrintArg_struct _tmp4F5;struct Cyc_String_pa_PrintArg_struct _tmp4F4;struct Cyc_String_pa_PrintArg_struct _tmp4F3;struct Cyc_String_pa_PrintArg_struct _tmp4F2;struct _dyneither_ptr _tmp114=(_tmp4F2.tag=0,((_tmp4F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string),((_tmp4F3.tag=0,((_tmp4F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string),((_tmp4F4.tag=0,((_tmp4F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 937
Cyc_sh_escape_string(filename)),((_tmp4F5.tag=0,((_tmp4F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((_tmp4F6.tag=0,((_tmp4F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp101),((_tmp4F7.tag=0,((_tmp4F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((_tmp4F8[0]=& _tmp4F7,((_tmp4F8[1]=& _tmp4F6,((_tmp4F8[2]=& _tmp4F5,((_tmp4F8[3]=& _tmp4F4,((_tmp4F8[4]=& _tmp4F3,((_tmp4F8[5]=& _tmp4F2,Cyc_aprintf(((_tmp4F9="%s %s%s %s%s%s",_tag_dyneither(_tmp4F9,sizeof(char),15))),_tag_dyneither(_tmp4F8,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
# 940
if(Cyc_v_r){const char*_tmp4FD;void*_tmp4FC[1];struct Cyc_String_pa_PrintArg_struct _tmp4FB;(_tmp4FB.tag=0,((_tmp4FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp114),((_tmp4FC[0]=& _tmp4FB,Cyc_fprintf(Cyc_stderr,((_tmp4FD="%s\n",_tag_dyneither(_tmp4FD,sizeof(char),4))),_tag_dyneither(_tmp4FC,sizeof(void*),1)))))));}
if(system((const char*)_untag_dyneither_ptr(_tmp114,sizeof(char),1))!= 0){
Cyc_compile_failure=1;
{const char*_tmp500;void*_tmp4FF;(_tmp4FF=0,Cyc_fprintf(Cyc_stderr,((_tmp500="\nError: preprocessing\n",_tag_dyneither(_tmp500,sizeof(char),23))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}
return;}
# 946
if(Cyc_stop_after_cpp_r)return;
# 949
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpFA);{
const char*_tmp502;const char*_tmp501;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpFA,((_tmp501="r",_tag_dyneither(_tmp501,sizeof(char),2))),((_tmp502="file",_tag_dyneither(_tmp502,sizeof(char),5))));
if(Cyc_compile_failure)return;{
# 953
struct Cyc_List_List*tds=0;
# 956
{struct _handler_cons _tmp11A;_push_handler(& _tmp11A);{int _tmp11C=0;if(setjmp(_tmp11A.handler))_tmp11C=1;if(!_tmp11C){
{const char*_tmp503;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp503="parsing",_tag_dyneither(_tmp503,sizeof(char),8))),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp11B=(void*)_exn_thrown;void*_tmp11F=_tmp11B;void*_tmp120;void*_tmp121;_LL26: _tmp120=_tmp11F;_LL27:
# 960
 Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
Cyc_remove_file((struct _dyneither_ptr)_tmpFA);
(int)_throw(_tmp120);_LL28: _tmp121=_tmp11F;_LL29:(void)_throw(_tmp121);_LL25:;}};}
# 964
Cyc_file_close(in_file);
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFA);
return;}
# 970
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}{
# 975
struct _RegionHandle _tmp122=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp122;_push_region(tc_rgn);{
struct Cyc_Tcenv_Tenv*_tmp123=Cyc_Tcenv_tc_init(tc_rgn);
if(Cyc_parseonly_r)goto PRINTC;{
# 980
struct _tuple22 _tmp504;struct _tuple22 _tmp124=(_tmp504.f1=tc_rgn,((_tmp504.f2=_tmp123,_tmp504)));
{const char*_tmp505;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp505="type checking",_tag_dyneither(_tmp505,sizeof(char),14))),Cyc_do_typecheck,& _tmp124,tds);}
# 983
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFA);
_npop_handler(0);return;}
# 987
if(Cyc_tc_r)goto PRINTC;
{const char*_tmp506;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp506="control-flow checking",_tag_dyneither(_tmp506,sizeof(char),22))),Cyc_do_cfcheck,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFA);
_npop_handler(0);return;}
# 995
if(Cyc_generate_interface_r){
const char*_tmp509;const char*_tmp508;const char*_tmp507;struct Cyc___cycFILE*inter_file=Cyc_try_file_open(((_tmp507=(const char*)_untag_dyneither_ptr(_tmpFB,sizeof(char),1),_tag_dyneither(_tmp507,sizeof(char),_get_zero_arr_size_char((void*)_tmp507,1)))),((_tmp508="w",_tag_dyneither(_tmp508,sizeof(char),2))),((_tmp509="interface file",_tag_dyneither(_tmp509,sizeof(char),15))));
if(inter_file == 0){
Cyc_compile_failure=1;_npop_handler(0);return;}
# 1000
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 1007
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_dyneither_ptr(_tmpFB,sizeof(char),1),"r");
const char*_tmp50B;const char*_tmp50A;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpFC,((_tmp50A="w",_tag_dyneither(_tmp50A,sizeof(char),2))),((_tmp50B="interface object file",_tag_dyneither(_tmp50B,sizeof(char),22))));
if(inter_objfile == 0){
Cyc_compile_failure=1;
_npop_handler(0);return;}
# 1014
Cyc_Position_reset_position(_tmpFB);{
struct _tuple23 _tmp50C;struct _tuple23 _tmp12A=(_tmp50C.f1=_tmp123,((_tmp50C.f2=inter_file,((_tmp50C.f3=inter_objfile,_tmp50C)))));
{const char*_tmp50D;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp50D="interface checking",_tag_dyneither(_tmp50D,sizeof(char),19))),Cyc_do_interface,& _tmp12A,tds);}
if(inter_file != 0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);};}};}
# 1022
if(Cyc_cf_r)goto PRINTC;
# 1025
{const char*_tmp50E;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp50E="translation to C",_tag_dyneither(_tmp50E,sizeof(char),17))),Cyc_do_translate,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFA);
_npop_handler(0);return;}
# 1031
if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmpFA);
if(Cyc_compile_failure){_npop_handler(0);return;}
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){
# 1039
const char*_tmp50F;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp50F="post-pass to VC",_tag_dyneither(_tmp50F,sizeof(char),16))),Cyc_do_tovc,1,tds);}
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1046
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0){
const char*_tmp511;const char*_tmp510;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp510="w",_tag_dyneither(_tmp510,sizeof(char),2))),((_tmp511="output file",_tag_dyneither(_tmp511,sizeof(char),12))));}else{
# 1052
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0){
const char*_tmp513;const char*_tmp512;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp512="w",_tag_dyneither(_tmp512,sizeof(char),2))),((_tmp513="output file",_tag_dyneither(_tmp513,sizeof(char),12))));}else{
# 1056
const char*_tmp515;const char*_tmp514;out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpFD,((_tmp514="w",_tag_dyneither(_tmp514,sizeof(char),2))),((_tmp515="output file",_tag_dyneither(_tmp515,sizeof(char),12))));}}
# 1058
if(Cyc_compile_failure  || !((unsigned int)out_file)){_npop_handler(0);return;}
# 1060
{struct _handler_cons _tmp138;_push_handler(& _tmp138);{int _tmp13A=0;if(setjmp(_tmp138.handler))_tmp13A=1;if(!_tmp13A){
if(!Cyc_noprint_r){
const char*_tmp516;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp516="printing",_tag_dyneither(_tmp516,sizeof(char),9))),Cyc_do_print,out_file,tds);}
# 1061
;_pop_handler();}else{void*_tmp139=(void*)_exn_thrown;void*_tmp13D=_tmp139;void*_tmp13E;void*_tmp13F;_LL2B: _tmp13E=_tmp13D;_LL2C:
# 1065
 Cyc_compile_failure=1;
Cyc_file_close(out_file);
{struct _dyneither_ptr*_tmp519;struct Cyc_List_List*_tmp518;Cyc_cfiles=((_tmp518=_cycalloc(sizeof(*_tmp518)),((_tmp518->hd=((_tmp519=_cycalloc(sizeof(*_tmp519)),((_tmp519[0]=(struct _dyneither_ptr)_tmpFD,_tmp519)))),((_tmp518->tl=Cyc_cfiles,_tmp518))))));}
(int)_throw(_tmp13E);_LL2D: _tmp13F=_tmp13D;_LL2E:(void)_throw(_tmp13F);_LL2A:;}};}
# 1070
Cyc_file_close(out_file);{
struct _dyneither_ptr*_tmp51C;struct Cyc_List_List*_tmp51B;Cyc_cfiles=((_tmp51B=_cycalloc(sizeof(*_tmp51B)),((_tmp51B->hd=((_tmp51C=_cycalloc(sizeof(*_tmp51C)),((_tmp51C[0]=(struct _dyneither_ptr)_tmpFD,_tmp51C)))),((_tmp51B->tl=Cyc_cfiles,_tmp51B))))));};}
# 975
;_pop_region(tc_rgn);};};};};};};};};};}static char _tmp15C[8]="<final>";
# 1075
static struct _dyneither_ptr Cyc_final_str={_tmp15C,_tmp15C,_tmp15C + 8};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1078
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 1082
struct _dyneither_ptr basename;
{struct _handler_cons _tmp15D;_push_handler(& _tmp15D);{int _tmp15F=0;if(setjmp(_tmp15D.handler))_tmp15F=1;if(!_tmp15F){
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp15E=(void*)_exn_thrown;void*_tmp161=_tmp15E;void*_tmp163;_LL30: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp162=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp161;if(_tmp162->tag != Cyc_Core_Invalid_argument)goto _LL32;}_LL31:
# 1086
 basename=*n;goto _LL2F;_LL32: _tmp163=_tmp161;_LL33:(void)_throw(_tmp163);_LL2F:;}};}{
# 1088
const char*_tmp51D;struct _dyneither_ptr _tmp164=Cyc_strconcat((struct _dyneither_ptr)basename,((_tmp51D=".cycio",_tag_dyneither(_tmp51D,sizeof(char),7))));
const char*_tmp51F;const char*_tmp51E;struct Cyc___cycFILE*_tmp165=Cyc_try_file_open((struct _dyneither_ptr)_tmp164,((_tmp51E="rb",_tag_dyneither(_tmp51E,sizeof(char),3))),((_tmp51F="interface object file",_tag_dyneither(_tmp51F,sizeof(char),22))));
if(_tmp165 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 1095
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp164);{
# 1097
struct Cyc_Interface_I*_tmp166=Cyc_Interface_load(_tmp165);
Cyc_file_close(_tmp165);
return _tmp166;};};};}
# 1102
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1106
void GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp525(unsigned int*_tmp524,unsigned int*_tmp523,int**_tmp521){for(*_tmp524=0;*_tmp524 < *_tmp523;(*_tmp524)++){(*_tmp521)[*_tmp524]=0;}}static void _tmp52D(unsigned int*_tmp52C,unsigned int*_tmp52B,struct _dyneither_ptr**_tmp529){for(*_tmp52C=0;*_tmp52C < *_tmp52B;(*_tmp52C)++){(*_tmp529)[*_tmp52C]=(struct _dyneither_ptr)
# 1143
_tag_dyneither(0,0,0);}}static void _tmp533(unsigned int*_tmp532,unsigned int*_tmp531,struct _dyneither_ptr**_tmp52F){for(*_tmp532=0;*_tmp532 < *_tmp531;(*_tmp532)++){(*_tmp52F)[*_tmp532]=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);}}
# 1109
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1114
GC_blacklist_warn_clear();{
# 1116
struct Cyc_List_List*cyclone_arch_path;
struct _dyneither_ptr def_lib_path;
struct _dyneither_ptr comp=(struct _dyneither_ptr)Cstring_to_string(Ccomp);
# 1121
struct _RegionHandle _tmp16A=_new_region("r");struct _RegionHandle*r=& _tmp16A;_push_region(r);{
# 1123
unsigned int _tmp524;unsigned int _tmp523;struct _dyneither_ptr _tmp522;int*_tmp521;unsigned int _tmp520;struct _dyneither_ptr _tmp16B=(_tmp520=(unsigned int)argc,((_tmp521=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp520)),((_tmp522=_tag_dyneither(_tmp521,sizeof(int),_tmp520),((((_tmp523=_tmp520,_tmp525(& _tmp524,& _tmp523,& _tmp521))),_tmp522)))))));
int _tmp16C=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
const char*_tmp526;if(Cyc_strncmp(((_tmp526="-B",_tag_dyneither(_tmp526,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
*((int*)_check_dyneither_subscript(_tmp16B,sizeof(int),i))=1;
++ _tmp16C;}else{
# 1131
const char*_tmp527;if(Cyc_strcmp(((_tmp527="-b",_tag_dyneither(_tmp527,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
*((int*)_check_dyneither_subscript(_tmp16B,sizeof(int),i))=1;
++ _tmp16C;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp16B,sizeof(int),i))=1;
++ _tmp16C;}}}{
# 1143
unsigned int _tmp52C;unsigned int _tmp52B;struct _dyneither_ptr _tmp52A;struct _dyneither_ptr*_tmp529;unsigned int _tmp528;struct _dyneither_ptr _tmp16F=(_tmp528=(unsigned int)(_tmp16C + 1),((_tmp529=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp528)),((_tmp52A=_tag_dyneither(_tmp529,sizeof(struct _dyneither_ptr),_tmp528),((((_tmp52B=_tmp528,_tmp52D(& _tmp52C,& _tmp52B,& _tmp529))),_tmp52A)))))));
unsigned int _tmp532;unsigned int _tmp531;struct _dyneither_ptr _tmp530;struct _dyneither_ptr*_tmp52F;unsigned int _tmp52E;struct _dyneither_ptr _tmp170=(_tmp52E=(unsigned int)(argc - _tmp16C),((_tmp52F=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp52E)),((_tmp530=_tag_dyneither(_tmp52F,sizeof(struct _dyneither_ptr),_tmp52E),((((_tmp531=_tmp52E,_tmp533(& _tmp532,& _tmp531,& _tmp52F))),_tmp530)))))));
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp16F,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp170,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp16B,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp16F,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp170,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}{
# 1152
struct _tuple24*_tmpA8C;const char*_tmpA8B;const char*_tmpA8A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA89;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA88;const char*_tmpA87;struct _tuple24*_tmpA86;const char*_tmpA85;const char*_tmpA84;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA83;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA82;const char*_tmpA81;struct _tuple24*_tmpA80;const char*_tmpA7F;const char*_tmpA7E;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA7D;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA7C;const char*_tmpA7B;struct _tuple24*_tmpA7A;const char*_tmpA79;const char*_tmpA78;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA77;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA76;const char*_tmpA75;struct _tuple24*_tmpA74;const char*_tmpA73;const char*_tmpA72;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA71;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA70;const char*_tmpA6F;struct _tuple24*_tmpA6E;const char*_tmpA6D;const char*_tmpA6C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA6B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA6A;const char*_tmpA69;struct _tuple24*_tmpA68;const char*_tmpA67;const char*_tmpA66;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA65;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA64;const char*_tmpA63;struct _tuple24*_tmpA62;const char*_tmpA61;const char*_tmpA60;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA5F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA5E;const char*_tmpA5D;struct _tuple24*_tmpA5C;const char*_tmpA5B;const char*_tmpA5A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA59;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA58;const char*_tmpA57;struct _tuple24*_tmpA56;const char*_tmpA55;const char*_tmpA54;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA53;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA52;const char*_tmpA51;struct _tuple24*_tmpA50;const char*_tmpA4F;const char*_tmpA4E;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA4D;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA4C;const char*_tmpA4B;struct _tuple24*_tmpA4A;const char*_tmpA49;const char*_tmpA48;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA47;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA46;const char*_tmpA45;struct _tuple24*_tmpA44;const char*_tmpA43;const char*_tmpA42;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA41;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA40;const char*_tmpA3F;struct _tuple24*_tmpA3E;const char*_tmpA3D;const char*_tmpA3C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA3B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA3A;const char*_tmpA39;struct _tuple24*_tmpA38;const char*_tmpA37;const char*_tmpA36;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA35;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA34;const char*_tmpA33;struct _tuple24*_tmpA32;const char*_tmpA31;const char*_tmpA30;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA2F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA2E;const char*_tmpA2D;struct _tuple24*_tmpA2C;const char*_tmpA2B;const char*_tmpA2A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA29;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA28;const char*_tmpA27;struct _tuple24*_tmpA26;const char*_tmpA25;const char*_tmpA24;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA23;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA22;const char*_tmpA21;struct _tuple24*_tmpA20;const char*_tmpA1F;const char*_tmpA1E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA1D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA1C;const char*_tmpA1B;struct _tuple24*_tmpA1A;const char*_tmpA19;const char*_tmpA18;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA17;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA16;const char*_tmpA15;struct _tuple24*_tmpA14;const char*_tmpA13;const char*_tmpA12;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA11;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA10;const char*_tmpA0F;struct _tuple24*_tmpA0E;const char*_tmpA0D;const char*_tmpA0C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA0B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA0A;const char*_tmpA09;struct _tuple24*_tmpA08;const char*_tmpA07;const char*_tmpA06;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA05;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA04;const char*_tmpA03;struct _tuple24*_tmpA02;const char*_tmpA01;const char*_tmpA00;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9FF;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9FE;const char*_tmp9FD;struct _tuple24*_tmp9FC;const char*_tmp9FB;const char*_tmp9FA;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9F9;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9F8;const char*_tmp9F7;struct _tuple24*_tmp9F6;const char*_tmp9F5;const char*_tmp9F4;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9F3;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9F2;const char*_tmp9F1;struct _tuple24*_tmp9F0;const char*_tmp9EF;const char*_tmp9EE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9ED;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9EC;const char*_tmp9EB;struct _tuple24*_tmp9EA;const char*_tmp9E9;const char*_tmp9E8;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9E7;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9E6;const char*_tmp9E5;struct _tuple24*_tmp9E4;const char*_tmp9E3;const char*_tmp9E2;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E1;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E0;const char*_tmp9DF;struct _tuple24*_tmp9DE;const char*_tmp9DD;const char*_tmp9DC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9DB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9DA;const char*_tmp9D9;struct _tuple24*_tmp9D8;const char*_tmp9D7;const char*_tmp9D6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9D5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9D4;const char*_tmp9D3;struct _tuple24*_tmp9D2;const char*_tmp9D1;const char*_tmp9D0;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9CF;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9CE;const char*_tmp9CD;struct _tuple24*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9C9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9C8;const char*_tmp9C7;struct _tuple24*_tmp9C6;const char*_tmp9C5;const char*_tmp9C4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9C3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9C2;const char*_tmp9C1;struct _tuple24*_tmp9C0;const char*_tmp9BF;const char*_tmp9BE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9BD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9BC;const char*_tmp9BB;struct _tuple24*_tmp9BA;const char*_tmp9B9;const char*_tmp9B8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9B7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9B6;const char*_tmp9B5;struct _tuple24*_tmp9B4;const char*_tmp9B3;const char*_tmp9B2;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9B1;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9B0;const char*_tmp9AF;struct _tuple24*_tmp9AE;const char*_tmp9AD;const char*_tmp9AC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9AB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9AA;const char*_tmp9A9;struct _tuple24*_tmp9A8;const char*_tmp9A7;const char*_tmp9A6;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9A5;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9A4;const char*_tmp9A3;struct _tuple24*_tmp9A2;const char*_tmp9A1;const char*_tmp9A0;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp99F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp99E;const char*_tmp99D;struct _tuple24*_tmp99C;const char*_tmp99B;const char*_tmp99A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp999;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp998;const char*_tmp997;struct _tuple24*_tmp996;const char*_tmp995;const char*_tmp994;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp993;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp992;const char*_tmp991;struct _tuple24*_tmp990;const char*_tmp98F;const char*_tmp98E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp98D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp98C;const char*_tmp98B;struct _tuple24*_tmp98A;const char*_tmp989;const char*_tmp988;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp987;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp986;const char*_tmp985;struct _tuple24*_tmp984;const char*_tmp983;const char*_tmp982;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp981;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp980;const char*_tmp97F;struct _tuple24*_tmp97E;const char*_tmp97D;const char*_tmp97C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp97B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp97A;const char*_tmp979;struct _tuple24*_tmp978;const char*_tmp977;const char*_tmp976;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp975;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp974;const char*_tmp973;struct _tuple24*_tmp972;const char*_tmp971;const char*_tmp970;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp96F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp96E;const char*_tmp96D;struct _tuple24*_tmp96C;const char*_tmp96B;const char*_tmp96A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp969;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp968;const char*_tmp967;struct _tuple24*_tmp966;const char*_tmp965;const char*_tmp964;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp963;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp962;const char*_tmp961;struct _tuple24*_tmp960;const char*_tmp95F;const char*_tmp95E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp95D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp95C;const char*_tmp95B;struct _tuple24*_tmp95A;const char*_tmp959;const char*_tmp958;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp957;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp956;const char*_tmp955;struct _tuple24*_tmp954;const char*_tmp953;const char*_tmp952;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp951;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp950;const char*_tmp94F;struct _tuple24*_tmp94E;const char*_tmp94D;const char*_tmp94C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp94B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp94A;const char*_tmp949;struct _tuple24*_tmp948;const char*_tmp947;const char*_tmp946;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp945;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp944;const char*_tmp943;struct _tuple24*_tmp942;const char*_tmp941;const char*_tmp940;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp93F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp93E;const char*_tmp93D;struct _tuple24*_tmp93C;const char*_tmp93B;const char*_tmp93A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp939;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp938;const char*_tmp937;struct _tuple24*_tmp936;const char*_tmp935;const char*_tmp934;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp933;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp932;const char*_tmp931;struct _tuple24*_tmp930;const char*_tmp92F;const char*_tmp92E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp92D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp92C;const char*_tmp92B;struct _tuple24*_tmp92A;const char*_tmp929;const char*_tmp928;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp927;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp926;const char*_tmp925;struct _tuple24*_tmp924;const char*_tmp923;const char*_tmp922;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp921;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp920;const char*_tmp91F;struct _tuple24*_tmp91E;const char*_tmp91D;const char*_tmp91C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp91B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp91A;const char*_tmp919;struct _tuple24*_tmp918;const char*_tmp917;const char*_tmp916;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp915;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp914;const char*_tmp913;struct _tuple24*_tmp912;const char*_tmp911;const char*_tmp910;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp90F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp90E;const char*_tmp90D;struct _tuple24*_tmp90C;const char*_tmp90B;const char*_tmp90A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp909;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp908;const char*_tmp907;struct _tuple24*_tmp906;const char*_tmp905;const char*_tmp904;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp903;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp902;const char*_tmp901;struct _tuple24*_tmp900;const char*_tmp8FF;const char*_tmp8FE;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp8FD;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp8FC;const char*_tmp8FB;struct _tuple24*_tmp8FA;const char*_tmp8F9;const char*_tmp8F8;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8F7;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8F6;const char*_tmp8F5;struct _tuple24*_tmp8F4;const char*_tmp8F3;const char*_tmp8F2;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8F1;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8F0;const char*_tmp8EF;struct _tuple24*_tmp8EE;const char*_tmp8ED;const char*_tmp8EC;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8EB;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8EA;const char*_tmp8E9;struct _tuple24*_tmp8E8;const char*_tmp8E7;const char*_tmp8E6;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8E5;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8E4;const char*_tmp8E3;struct _tuple24*_tmp8E2;const char*_tmp8E1;const char*_tmp8E0;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8DF;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8DE;const char*_tmp8DD;struct _tuple24*_tmp8DC;const char*_tmp8DB;const char*_tmp8DA;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8D9;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8D8;const char*_tmp8D7;struct _tuple24*_tmp8D6;const char*_tmp8D5;const char*_tmp8D4;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8D3;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8D2;const char*_tmp8D1;struct _tuple24*_tmp8D0;const char*_tmp8CF;const char*_tmp8CE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8CD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8CC;const char*_tmp8CB;struct _tuple24*_tmp8CA;const char*_tmp8C9;const char*_tmp8C8;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8C7;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8C6;const char*_tmp8C5;struct _tuple24*_tmp8C4[76];struct Cyc_List_List*options=
(_tmp8C4[75]=(
# 1380
(_tmp8CA=_region_malloc(r,sizeof(*_tmp8CA)),((_tmp8CA->f1=((_tmp8C9="--interface",_tag_dyneither(_tmp8C9,sizeof(char),12))),((_tmp8CA->f2=0,((_tmp8CA->f3=((_tmp8C8=" <file>",_tag_dyneither(_tmp8C8,sizeof(char),8))),((_tmp8CA->f4=(void*)(
(_tmp8C6=_region_malloc(r,sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C7.tag=5,((_tmp8C7.f1=Cyc_set_specified_interface,_tmp8C7)))),_tmp8C6)))),((_tmp8CA->f5=(
(_tmp8C5="Set the interface file to be <file>.",_tag_dyneither(_tmp8C5,sizeof(char),37))),_tmp8CA)))))))))))),((_tmp8C4[74]=(
# 1377
(_tmp8D0=_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0->f1=((_tmp8CF="--geninterface",_tag_dyneither(_tmp8CF,sizeof(char),15))),((_tmp8D0->f2=0,((_tmp8D0->f3=((_tmp8CE="",_tag_dyneither(_tmp8CE,sizeof(char),1))),((_tmp8D0->f4=(void*)(
(_tmp8CC=_region_malloc(r,sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CD.tag=3,((_tmp8CD.f1=& Cyc_generate_interface_r,_tmp8CD)))),_tmp8CC)))),((_tmp8D0->f5=(
(_tmp8CB="Generate interface files",_tag_dyneither(_tmp8CB,sizeof(char),25))),_tmp8D0)))))))))))),((_tmp8C4[73]=(
# 1374
(_tmp8D6=_region_malloc(r,sizeof(*_tmp8D6)),((_tmp8D6->f1=((_tmp8D5="-idirafter",_tag_dyneither(_tmp8D5,sizeof(char),11))),((_tmp8D6->f2=0,((_tmp8D6->f3=((_tmp8D4="<dir>",_tag_dyneither(_tmp8D4,sizeof(char),6))),((_tmp8D6->f4=(void*)(
(_tmp8D2=_region_malloc(r,sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=5,((_tmp8D3.f1=Cyc_add_idirafter,_tmp8D3)))),_tmp8D2)))),((_tmp8D6->f5=(
(_tmp8D1="Add the directory to the second include path.",_tag_dyneither(_tmp8D1,sizeof(char),46))),_tmp8D6)))))))))))),((_tmp8C4[72]=(
# 1371
(_tmp8DC=_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC->f1=((_tmp8DB="-isystem",_tag_dyneither(_tmp8DB,sizeof(char),9))),((_tmp8DC->f2=0,((_tmp8DC->f3=((_tmp8DA="<dir>",_tag_dyneither(_tmp8DA,sizeof(char),6))),((_tmp8DC->f4=(void*)(
(_tmp8D8=_region_malloc(r,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=5,((_tmp8D9.f1=Cyc_add_isystem,_tmp8D9)))),_tmp8D8)))),((_tmp8DC->f5=(
(_tmp8D7="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",_tag_dyneither(_tmp8D7,sizeof(char),90))),_tmp8DC)))))))))))),((_tmp8C4[71]=(
# 1368
(_tmp8E2=_region_malloc(r,sizeof(*_tmp8E2)),((_tmp8E2->f1=((_tmp8E1="-iwithprefixbefore",_tag_dyneither(_tmp8E1,sizeof(char),19))),((_tmp8E2->f2=0,((_tmp8E2->f3=((_tmp8E0="<dir>",_tag_dyneither(_tmp8E0,sizeof(char),6))),((_tmp8E2->f4=(void*)(
(_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8DF.tag=5,((_tmp8DF.f1=Cyc_add_iwithprefixbefore,_tmp8DF)))),_tmp8DE)))),((_tmp8E2->f5=(
(_tmp8DD="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp8DD,sizeof(char),45))),_tmp8E2)))))))))))),((_tmp8C4[70]=(
# 1365
(_tmp8E8=_region_malloc(r,sizeof(*_tmp8E8)),((_tmp8E8->f1=((_tmp8E7="-iwithprefix",_tag_dyneither(_tmp8E7,sizeof(char),13))),((_tmp8E8->f2=0,((_tmp8E8->f3=((_tmp8E6="<dir>",_tag_dyneither(_tmp8E6,sizeof(char),6))),((_tmp8E8->f4=(void*)(
(_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=5,((_tmp8E5.f1=Cyc_add_iwithprefix,_tmp8E5)))),_tmp8E4)))),((_tmp8E8->f5=(
(_tmp8E3="Add <prefix>/<dir> to the second include path.",_tag_dyneither(_tmp8E3,sizeof(char),47))),_tmp8E8)))))))))))),((_tmp8C4[69]=(
# 1362
(_tmp8EE=_region_malloc(r,sizeof(*_tmp8EE)),((_tmp8EE->f1=((_tmp8ED="-iprefix",_tag_dyneither(_tmp8ED,sizeof(char),9))),((_tmp8EE->f2=0,((_tmp8EE->f3=((_tmp8EC="<prefix>",_tag_dyneither(_tmp8EC,sizeof(char),9))),((_tmp8EE->f4=(void*)(
(_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=5,((_tmp8EB.f1=Cyc_add_iprefix,_tmp8EB)))),_tmp8EA)))),((_tmp8EE->f5=(
(_tmp8E9="Specify <prefix> as the prefix for subsequent -iwithprefix options",_tag_dyneither(_tmp8E9,sizeof(char),67))),_tmp8EE)))))))))))),((_tmp8C4[68]=(
# 1359
(_tmp8F4=_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4->f1=((_tmp8F3="-noregions",_tag_dyneither(_tmp8F3,sizeof(char),11))),((_tmp8F4->f2=0,((_tmp8F4->f3=((_tmp8F2="",_tag_dyneither(_tmp8F2,sizeof(char),1))),((_tmp8F4->f4=(void*)(
(_tmp8F0=_region_malloc(r,sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F1.tag=3,((_tmp8F1.f1=& Cyc_Absyn_no_regions,_tmp8F1)))),_tmp8F0)))),((_tmp8F4->f5=(
(_tmp8EF="Generate code that doesn't use regions",_tag_dyneither(_tmp8EF,sizeof(char),39))),_tmp8F4)))))))))))),((_tmp8C4[67]=(
# 1356
(_tmp8FA=_region_malloc(r,sizeof(*_tmp8FA)),((_tmp8FA->f1=((_tmp8F9="-port",_tag_dyneither(_tmp8F9,sizeof(char),6))),((_tmp8FA->f2=0,((_tmp8FA->f3=((_tmp8F8="",_tag_dyneither(_tmp8F8,sizeof(char),1))),((_tmp8FA->f4=(void*)(
(_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F7.tag=0,((_tmp8F7.f1=Cyc_set_port_c_code,_tmp8F7)))),_tmp8F6)))),((_tmp8FA->f5=(
(_tmp8F5="Suggest how to port C code to Cyclone",_tag_dyneither(_tmp8F5,sizeof(char),38))),_tmp8FA)))))))))))),((_tmp8C4[66]=(
# 1353
(_tmp900=_region_malloc(r,sizeof(*_tmp900)),((_tmp900->f1=((_tmp8FF="-detailedlocation",_tag_dyneither(_tmp8FF,sizeof(char),18))),((_tmp900->f2=0,((_tmp900->f3=((_tmp8FE="",_tag_dyneither(_tmp8FE,sizeof(char),1))),((_tmp900->f4=(void*)(
(_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=4,((_tmp8FD.f1=& Cyc_Position_use_gcc_style_location,_tmp8FD)))),_tmp8FC)))),((_tmp900->f5=(
(_tmp8FB="Try to give more detailed location information for errors",_tag_dyneither(_tmp8FB,sizeof(char),58))),_tmp900)))))))))))),((_tmp8C4[65]=(
# 1350
(_tmp906=_region_malloc(r,sizeof(*_tmp906)),((_tmp906->f1=((_tmp905="-noregister",_tag_dyneither(_tmp905,sizeof(char),12))),((_tmp906->f2=0,((_tmp906->f3=((_tmp904="",_tag_dyneither(_tmp904,sizeof(char),1))),((_tmp906->f4=(void*)(
(_tmp902=_region_malloc(r,sizeof(*_tmp902)),((_tmp902[0]=((_tmp903.tag=3,((_tmp903.f1=& Cyc_Parse_no_register,_tmp903)))),_tmp902)))),((_tmp906->f5=(
(_tmp901="Treat register storage class as public",_tag_dyneither(_tmp901,sizeof(char),39))),_tmp906)))))))))))),((_tmp8C4[64]=(
# 1347
(_tmp90C=_region_malloc(r,sizeof(*_tmp90C)),((_tmp90C->f1=((_tmp90B="-warnregioncoerce",_tag_dyneither(_tmp90B,sizeof(char),18))),((_tmp90C->f2=0,((_tmp90C->f3=((_tmp90A="",_tag_dyneither(_tmp90A,sizeof(char),1))),((_tmp90C->f4=(void*)(
(_tmp908=_region_malloc(r,sizeof(*_tmp908)),((_tmp908[0]=((_tmp909.tag=3,((_tmp909.f1=& Cyc_Tcutil_warn_region_coerce,_tmp909)))),_tmp908)))),((_tmp90C->f5=(
(_tmp907="Warn when any region coercion is inserted",_tag_dyneither(_tmp907,sizeof(char),42))),_tmp90C)))))))))))),((_tmp8C4[63]=(
# 1344
(_tmp912=_region_malloc(r,sizeof(*_tmp912)),((_tmp912->f1=((_tmp911="-warnaliascoercion",_tag_dyneither(_tmp911,sizeof(char),19))),((_tmp912->f2=0,((_tmp912->f3=((_tmp910="",_tag_dyneither(_tmp910,sizeof(char),1))),((_tmp912->f4=(void*)(
(_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=3,((_tmp90F.f1=& Cyc_Tcutil_warn_alias_coerce,_tmp90F)))),_tmp90E)))),((_tmp912->f5=(
(_tmp90D="Warn when any alias coercion is inserted",_tag_dyneither(_tmp90D,sizeof(char),41))),_tmp912)))))))))))),((_tmp8C4[62]=(
# 1341
(_tmp918=_region_malloc(r,sizeof(*_tmp918)),((_tmp918->f1=((_tmp917="-warnnullchecks",_tag_dyneither(_tmp917,sizeof(char),16))),((_tmp918->f2=0,((_tmp918->f3=((_tmp916="",_tag_dyneither(_tmp916,sizeof(char),1))),((_tmp918->f4=(void*)(
(_tmp914=_region_malloc(r,sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=3,((_tmp915.f1=& Cyc_Toc_warn_all_null_deref,_tmp915)))),_tmp914)))),((_tmp918->f5=(
(_tmp913="Warn when any null check can't be eliminated",_tag_dyneither(_tmp913,sizeof(char),45))),_tmp918)))))))))))),((_tmp8C4[61]=(
# 1338
(_tmp91E=_region_malloc(r,sizeof(*_tmp91E)),((_tmp91E->f1=((_tmp91D="-warnboundschecks",_tag_dyneither(_tmp91D,sizeof(char),18))),((_tmp91E->f2=0,((_tmp91E->f3=((_tmp91C="",_tag_dyneither(_tmp91C,sizeof(char),1))),((_tmp91E->f4=(void*)(
(_tmp91A=_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91B.tag=3,((_tmp91B.f1=& Cyc_Toc_warn_bounds_checks,_tmp91B)))),_tmp91A)))),((_tmp91E->f5=(
(_tmp919="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp919,sizeof(char),44))),_tmp91E)))))))))))),((_tmp8C4[60]=(
# 1335
(_tmp924=_region_malloc(r,sizeof(*_tmp924)),((_tmp924->f1=((_tmp923="-CI",_tag_dyneither(_tmp923,sizeof(char),4))),((_tmp924->f2=0,((_tmp924->f3=((_tmp922=" <file>",_tag_dyneither(_tmp922,sizeof(char),8))),((_tmp924->f4=(void*)(
(_tmp920=_region_malloc(r,sizeof(*_tmp920)),((_tmp920[0]=((_tmp921.tag=5,((_tmp921.f1=Cyc_set_cyc_include,_tmp921)))),_tmp920)))),((_tmp924->f5=(
(_tmp91F="Set cyc_include.h to be <file>",_tag_dyneither(_tmp91F,sizeof(char),31))),_tmp924)))))))))))),((_tmp8C4[59]=(
# 1332
(_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A->f1=((_tmp929="-compile-for-boot",_tag_dyneither(_tmp929,sizeof(char),18))),((_tmp92A->f2=0,((_tmp92A->f3=((_tmp928="",_tag_dyneither(_tmp928,sizeof(char),1))),((_tmp92A->f4=(void*)(
(_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=3,((_tmp927.f1=& Cyc_Lex_compile_for_boot_r,_tmp927)))),_tmp926)))),((_tmp92A->f5=(
(_tmp925="Compile using the special boot library instead of the standard library",_tag_dyneither(_tmp925,sizeof(char),71))),_tmp92A)))))))))))),((_tmp8C4[58]=(
# 1329
(_tmp930=_region_malloc(r,sizeof(*_tmp930)),((_tmp930->f1=((_tmp92F="-nocyc_setjmp",_tag_dyneither(_tmp92F,sizeof(char),14))),((_tmp930->f2=0,((_tmp930->f3=((_tmp92E="",_tag_dyneither(_tmp92E,sizeof(char),1))),((_tmp930->f4=(void*)(
(_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92D.tag=3,((_tmp92D.f1=& Cyc_nocyc_setjmp_r,_tmp92D)))),_tmp92C)))),((_tmp930->f5=(
(_tmp92B="Do not use compiler special file cyc_setjmp.h",_tag_dyneither(_tmp92B,sizeof(char),46))),_tmp930)))))))))))),((_tmp8C4[57]=(
# 1326
(_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936->f1=((_tmp935="-printalleffects",_tag_dyneither(_tmp935,sizeof(char),17))),((_tmp936->f2=0,((_tmp936->f3=((_tmp934="",_tag_dyneither(_tmp934,sizeof(char),1))),((_tmp936->f4=(void*)(
(_tmp932=_region_malloc(r,sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=3,((_tmp933.f1=& Cyc_print_all_effects_r,_tmp933)))),_tmp932)))),((_tmp936->f5=(
(_tmp931="Print effects for functions (type debugging)",_tag_dyneither(_tmp931,sizeof(char),45))),_tmp936)))))))))))),((_tmp8C4[56]=(
# 1323
(_tmp93C=_region_malloc(r,sizeof(*_tmp93C)),((_tmp93C->f1=((_tmp93B="-printfullevars",_tag_dyneither(_tmp93B,sizeof(char),16))),((_tmp93C->f2=0,((_tmp93C->f3=((_tmp93A="",_tag_dyneither(_tmp93A,sizeof(char),1))),((_tmp93C->f4=(void*)(
(_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=3,((_tmp939.f1=& Cyc_print_full_evars_r,_tmp939)))),_tmp938)))),((_tmp93C->f5=(
(_tmp937="Print full information for evars (type debugging)",_tag_dyneither(_tmp937,sizeof(char),50))),_tmp93C)))))))))))),((_tmp8C4[55]=(
# 1320
(_tmp942=_region_malloc(r,sizeof(*_tmp942)),((_tmp942->f1=((_tmp941="-printallkinds",_tag_dyneither(_tmp941,sizeof(char),15))),((_tmp942->f2=0,((_tmp942->f3=((_tmp940="",_tag_dyneither(_tmp940,sizeof(char),1))),((_tmp942->f4=(void*)(
(_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=3,((_tmp93F.f1=& Cyc_print_all_kinds_r,_tmp93F)))),_tmp93E)))),((_tmp942->f5=(
(_tmp93D="Always print kinds of type variables",_tag_dyneither(_tmp93D,sizeof(char),37))),_tmp942)))))))))))),((_tmp8C4[54]=(
# 1317
(_tmp948=_region_malloc(r,sizeof(*_tmp948)),((_tmp948->f1=((_tmp947="-printalltvars",_tag_dyneither(_tmp947,sizeof(char),15))),((_tmp948->f2=0,((_tmp948->f3=((_tmp946="",_tag_dyneither(_tmp946,sizeof(char),1))),((_tmp948->f4=(void*)(
(_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=3,((_tmp945.f1=& Cyc_print_all_tvars_r,_tmp945)))),_tmp944)))),((_tmp948->f5=(
(_tmp943="Print all type variables (even implicit default effects)",_tag_dyneither(_tmp943,sizeof(char),57))),_tmp948)))))))))))),((_tmp8C4[53]=(
# 1314
(_tmp94E=_region_malloc(r,sizeof(*_tmp94E)),((_tmp94E->f1=((_tmp94D="-noexpandtypedefs",_tag_dyneither(_tmp94D,sizeof(char),18))),((_tmp94E->f2=0,((_tmp94E->f3=((_tmp94C="",_tag_dyneither(_tmp94C,sizeof(char),1))),((_tmp94E->f4=(void*)(
(_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A[0]=((_tmp94B.tag=3,((_tmp94B.f1=& Cyc_noexpand_r,_tmp94B)))),_tmp94A)))),((_tmp94E->f5=(
(_tmp949="Don't expand typedefs in pretty printing",_tag_dyneither(_tmp949,sizeof(char),41))),_tmp94E)))))))))))),((_tmp8C4[52]=(
# 1311
(_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954->f1=((_tmp953="-noremoveunused",_tag_dyneither(_tmp953,sizeof(char),16))),((_tmp954->f2=0,((_tmp954->f3=((_tmp952="",_tag_dyneither(_tmp952,sizeof(char),1))),((_tmp954->f4=(void*)(
(_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950[0]=((_tmp951.tag=3,((_tmp951.f1=& Cyc_noshake_r,_tmp951)))),_tmp950)))),((_tmp954->f5=(
(_tmp94F="Don't remove externed variables that aren't used",_tag_dyneither(_tmp94F,sizeof(char),49))),_tmp954)))))))))))),((_tmp8C4[51]=(
# 1308
(_tmp95A=_region_malloc(r,sizeof(*_tmp95A)),((_tmp95A->f1=((_tmp959="-nogc",_tag_dyneither(_tmp959,sizeof(char),6))),((_tmp95A->f2=0,((_tmp95A->f3=((_tmp958="",_tag_dyneither(_tmp958,sizeof(char),1))),((_tmp95A->f4=(void*)(
(_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956[0]=((_tmp957.tag=3,((_tmp957.f1=& Cyc_nogc_r,_tmp957)))),_tmp956)))),((_tmp95A->f5=(
(_tmp955="Don't link in the garbage collector",_tag_dyneither(_tmp955,sizeof(char),36))),_tmp95A)))))))))))),((_tmp8C4[50]=(
# 1305
(_tmp960=_region_malloc(r,sizeof(*_tmp960)),((_tmp960->f1=((_tmp95F="-nocyc",_tag_dyneither(_tmp95F,sizeof(char),7))),((_tmp960->f2=0,((_tmp960->f3=((_tmp95E="",_tag_dyneither(_tmp95E,sizeof(char),1))),((_tmp960->f4=(void*)(
(_tmp95C=_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C[0]=((_tmp95D.tag=0,((_tmp95D.f1=Cyc_set_nocyc,_tmp95D)))),_tmp95C)))),((_tmp960->f5=(
(_tmp95B="Don't add implicit namespace Cyc",_tag_dyneither(_tmp95B,sizeof(char),33))),_tmp960)))))))))))),((_tmp8C4[49]=(
# 1302
(_tmp966=_region_malloc(r,sizeof(*_tmp966)),((_tmp966->f1=((_tmp965="-no-cpp-precomp",_tag_dyneither(_tmp965,sizeof(char),16))),((_tmp966->f2=0,((_tmp966->f3=((_tmp964="",_tag_dyneither(_tmp964,sizeof(char),1))),((_tmp966->f4=(void*)(
(_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962[0]=((_tmp963.tag=0,((_tmp963.f1=Cyc_set_nocppprecomp,_tmp963)))),_tmp962)))),((_tmp966->f5=(
(_tmp961="Don't do smart preprocessing (mac only)",_tag_dyneither(_tmp961,sizeof(char),40))),_tmp966)))))))))))),((_tmp8C4[48]=(
# 1299
(_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C->f1=((_tmp96B="-use-cpp",_tag_dyneither(_tmp96B,sizeof(char),9))),((_tmp96C->f2=0,((_tmp96C->f3=((_tmp96A="<path>",_tag_dyneither(_tmp96A,sizeof(char),7))),((_tmp96C->f4=(void*)(
(_tmp968=_region_malloc(r,sizeof(*_tmp968)),((_tmp968[0]=((_tmp969.tag=5,((_tmp969.f1=Cyc_set_cpp,_tmp969)))),_tmp968)))),((_tmp96C->f5=(
(_tmp967="Indicate which preprocessor to use",_tag_dyneither(_tmp967,sizeof(char),35))),_tmp96C)))))))))))),((_tmp8C4[47]=(
# 1296
(_tmp972=_region_malloc(r,sizeof(*_tmp972)),((_tmp972->f1=((_tmp971="--inline-checks",_tag_dyneither(_tmp971,sizeof(char),16))),((_tmp972->f2=0,((_tmp972->f3=((_tmp970="",_tag_dyneither(_tmp970,sizeof(char),1))),((_tmp972->f4=(void*)(
(_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp96F.tag=0,((_tmp96F.f1=Cyc_set_inline_functions,_tmp96F)))),_tmp96E)))),((_tmp972->f5=(
(_tmp96D="Inline bounds checks instead of #define",_tag_dyneither(_tmp96D,sizeof(char),40))),_tmp972)))))))))))),((_tmp8C4[46]=(
# 1293
(_tmp978=_region_malloc(r,sizeof(*_tmp978)),((_tmp978->f1=((_tmp977="--noboundschecks",_tag_dyneither(_tmp977,sizeof(char),17))),((_tmp978->f2=0,((_tmp978->f3=((_tmp976="",_tag_dyneither(_tmp976,sizeof(char),1))),((_tmp978->f4=(void*)(
(_tmp974=_region_malloc(r,sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=0,((_tmp975.f1=Cyc_set_noboundschecks,_tmp975)))),_tmp974)))),((_tmp978->f5=(
(_tmp973="Disable bounds checks",_tag_dyneither(_tmp973,sizeof(char),22))),_tmp978)))))))))))),((_tmp8C4[45]=(
# 1290
(_tmp97E=_region_malloc(r,sizeof(*_tmp97E)),((_tmp97E->f1=((_tmp97D="--nonullchecks",_tag_dyneither(_tmp97D,sizeof(char),15))),((_tmp97E->f2=0,((_tmp97E->f3=((_tmp97C="",_tag_dyneither(_tmp97C,sizeof(char),1))),((_tmp97E->f4=(void*)(
(_tmp97A=_region_malloc(r,sizeof(*_tmp97A)),((_tmp97A[0]=((_tmp97B.tag=0,((_tmp97B.f1=Cyc_set_nonullchecks,_tmp97B)))),_tmp97A)))),((_tmp97E->f5=(
(_tmp979="Disable null checks",_tag_dyneither(_tmp979,sizeof(char),20))),_tmp97E)))))))))))),((_tmp8C4[44]=(
# 1287
(_tmp984=_region_malloc(r,sizeof(*_tmp984)),((_tmp984->f1=((_tmp983="--nochecks",_tag_dyneither(_tmp983,sizeof(char),11))),((_tmp984->f2=0,((_tmp984->f3=((_tmp982="",_tag_dyneither(_tmp982,sizeof(char),1))),((_tmp984->f4=(void*)(
(_tmp980=_region_malloc(r,sizeof(*_tmp980)),((_tmp980[0]=((_tmp981.tag=0,((_tmp981.f1=Cyc_set_nochecks,_tmp981)))),_tmp980)))),((_tmp984->f5=(
(_tmp97F="Disable bounds/null checks",_tag_dyneither(_tmp97F,sizeof(char),27))),_tmp984)))))))))))),((_tmp8C4[43]=(
# 1284
(_tmp98A=_region_malloc(r,sizeof(*_tmp98A)),((_tmp98A->f1=((_tmp989="--nolineno",_tag_dyneither(_tmp989,sizeof(char),11))),((_tmp98A->f2=0,((_tmp98A->f3=((_tmp988="",_tag_dyneither(_tmp988,sizeof(char),1))),((_tmp98A->f4=(void*)(
(_tmp986=_region_malloc(r,sizeof(*_tmp986)),((_tmp986[0]=((_tmp987.tag=0,((_tmp987.f1=Cyc_clear_lineno,_tmp987)))),_tmp986)))),((_tmp98A->f5=(
(_tmp985="Don't generate line numbers for debugging (automatic with -pp)",_tag_dyneither(_tmp985,sizeof(char),63))),_tmp98A)))))))))))),((_tmp8C4[42]=(
# 1281
(_tmp990=_region_malloc(r,sizeof(*_tmp990)),((_tmp990->f1=((_tmp98F="-save-c",_tag_dyneither(_tmp98F,sizeof(char),8))),((_tmp990->f2=0,((_tmp990->f3=((_tmp98E="",_tag_dyneither(_tmp98E,sizeof(char),1))),((_tmp990->f4=(void*)(
(_tmp98C=_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=3,((_tmp98D.f1=& Cyc_save_c_r,_tmp98D)))),_tmp98C)))),((_tmp990->f5=(
(_tmp98B="Don't delete temporary C files",_tag_dyneither(_tmp98B,sizeof(char),31))),_tmp990)))))))))))),((_tmp8C4[41]=(
# 1278
(_tmp996=_region_malloc(r,sizeof(*_tmp996)),((_tmp996->f1=((_tmp995="-save-temps",_tag_dyneither(_tmp995,sizeof(char),12))),((_tmp996->f2=0,((_tmp996->f3=((_tmp994="",_tag_dyneither(_tmp994,sizeof(char),1))),((_tmp996->f4=(void*)(
(_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992[0]=((_tmp993.tag=0,((_tmp993.f1=Cyc_set_save_temps,_tmp993)))),_tmp992)))),((_tmp996->f5=(
(_tmp991="Don't delete temporary files",_tag_dyneither(_tmp991,sizeof(char),29))),_tmp996)))))))))))),((_tmp8C4[40]=(
# 1275
(_tmp99C=_region_malloc(r,sizeof(*_tmp99C)),((_tmp99C->f1=((_tmp99B="-c-comp",_tag_dyneither(_tmp99B,sizeof(char),8))),((_tmp99C->f2=0,((_tmp99C->f3=((_tmp99A=" <compiler>",_tag_dyneither(_tmp99A,sizeof(char),12))),((_tmp99C->f4=(void*)(
(_tmp998=_region_malloc(r,sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=5,((_tmp999.f1=Cyc_set_c_compiler,_tmp999)))),_tmp998)))),((_tmp99C->f5=(
(_tmp997="Produce C output for the given compiler",_tag_dyneither(_tmp997,sizeof(char),40))),_tmp99C)))))))))))),((_tmp8C4[39]=(
# 1272
(_tmp9A2=_region_malloc(r,sizeof(*_tmp9A2)),((_tmp9A2->f1=((_tmp9A1="-un-gcc",_tag_dyneither(_tmp9A1,sizeof(char),8))),((_tmp9A2->f2=0,((_tmp9A2->f3=((_tmp9A0="",_tag_dyneither(_tmp9A0,sizeof(char),1))),((_tmp9A2->f4=(void*)(
(_tmp99E=_region_malloc(r,sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=0,((_tmp99F.f1=Cyc_set_tovc,_tmp99F)))),_tmp99E)))),((_tmp9A2->f5=(
(_tmp99D="Avoid gcc extensions in C output (except for attributes)",_tag_dyneither(_tmp99D,sizeof(char),57))),_tmp9A2)))))))))))),((_tmp8C4[38]=(
# 1269
(_tmp9A8=_region_malloc(r,sizeof(*_tmp9A8)),((_tmp9A8->f1=((_tmp9A7="-no-elim-statement-expressions",_tag_dyneither(_tmp9A7,sizeof(char),31))),((_tmp9A8->f2=0,((_tmp9A8->f3=((_tmp9A6="",_tag_dyneither(_tmp9A6,sizeof(char),1))),((_tmp9A8->f4=(void*)(
(_tmp9A4=_region_malloc(r,sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=4,((_tmp9A5.f1=& Cyc_elim_se_r,_tmp9A5)))),_tmp9A4)))),((_tmp9A8->f5=(
(_tmp9A3="Do not avoid statement expressions in C output",_tag_dyneither(_tmp9A3,sizeof(char),47))),_tmp9A8)))))))))))),((_tmp8C4[37]=(
# 1265
(_tmp9AE=_region_malloc(r,sizeof(*_tmp9AE)),((_tmp9AE->f1=((_tmp9AD="-elim-statement-expressions",_tag_dyneither(_tmp9AD,sizeof(char),28))),((_tmp9AE->f2=0,((_tmp9AE->f3=((_tmp9AC="",_tag_dyneither(_tmp9AC,sizeof(char),1))),((_tmp9AE->f4=(void*)(
(_tmp9AA=_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=3,((_tmp9AB.f1=& Cyc_elim_se_r,_tmp9AB)))),_tmp9AA)))),((_tmp9AE->f5=(
# 1268
(_tmp9A9="Avoid statement expressions in C output (implies --inline-checks)",_tag_dyneither(_tmp9A9,sizeof(char),66))),_tmp9AE)))))))))))),((_tmp8C4[36]=(
# 1262
(_tmp9B4=_region_malloc(r,sizeof(*_tmp9B4)),((_tmp9B4->f1=((_tmp9B3="-up",_tag_dyneither(_tmp9B3,sizeof(char),4))),((_tmp9B4->f2=0,((_tmp9B4->f3=((_tmp9B2="",_tag_dyneither(_tmp9B2,sizeof(char),1))),((_tmp9B4->f4=(void*)(
(_tmp9B0=_region_malloc(r,sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=4,((_tmp9B1.f1=& Cyc_pp_r,_tmp9B1)))),_tmp9B0)))),((_tmp9B4->f5=(
(_tmp9AF="Ugly print the C code that Cyclone generates (default)",_tag_dyneither(_tmp9AF,sizeof(char),55))),_tmp9B4)))))))))))),((_tmp8C4[35]=(
# 1259
(_tmp9BA=_region_malloc(r,sizeof(*_tmp9BA)),((_tmp9BA->f1=((_tmp9B9="-pp",_tag_dyneither(_tmp9B9,sizeof(char),4))),((_tmp9BA->f2=0,((_tmp9BA->f3=((_tmp9B8="",_tag_dyneither(_tmp9B8,sizeof(char),1))),((_tmp9BA->f4=(void*)(
(_tmp9B6=_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=3,((_tmp9B7.f1=& Cyc_pp_r,_tmp9B7)))),_tmp9B6)))),((_tmp9BA->f5=(
(_tmp9B5="Pretty print the C code that Cyclone generates",_tag_dyneither(_tmp9B5,sizeof(char),47))),_tmp9BA)))))))))))),((_tmp8C4[34]=(
# 1256
(_tmp9C0=_region_malloc(r,sizeof(*_tmp9C0)),((_tmp9C0->f1=((_tmp9BF="-ic",_tag_dyneither(_tmp9BF,sizeof(char),4))),((_tmp9C0->f2=0,((_tmp9C0->f3=((_tmp9BE="",_tag_dyneither(_tmp9BE,sizeof(char),1))),((_tmp9C0->f4=(void*)(
(_tmp9BC=_region_malloc(r,sizeof(*_tmp9BC)),((_tmp9BC[0]=((_tmp9BD.tag=3,((_tmp9BD.f1=& Cyc_ic_r,_tmp9BD)))),_tmp9BC)))),((_tmp9C0->f5=(
(_tmp9BB="Activate the link-checker",_tag_dyneither(_tmp9BB,sizeof(char),26))),_tmp9C0)))))))))))),((_tmp8C4[33]=(
# 1253
(_tmp9C6=_region_malloc(r,sizeof(*_tmp9C6)),((_tmp9C6->f1=((_tmp9C5="-stopafter-toc",_tag_dyneither(_tmp9C5,sizeof(char),15))),((_tmp9C6->f2=0,((_tmp9C6->f3=((_tmp9C4="",_tag_dyneither(_tmp9C4,sizeof(char),1))),((_tmp9C6->f4=(void*)(
(_tmp9C2=_region_malloc(r,sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=3,((_tmp9C3.f1=& Cyc_toc_r,_tmp9C3)))),_tmp9C2)))),((_tmp9C6->f5=(
(_tmp9C1="Stop after translation to C",_tag_dyneither(_tmp9C1,sizeof(char),28))),_tmp9C6)))))))))))),((_tmp8C4[32]=(
# 1250
(_tmp9CC=_region_malloc(r,sizeof(*_tmp9CC)),((_tmp9CC->f1=((_tmp9CB="-stopafter-cf",_tag_dyneither(_tmp9CB,sizeof(char),14))),((_tmp9CC->f2=0,((_tmp9CC->f3=((_tmp9CA="",_tag_dyneither(_tmp9CA,sizeof(char),1))),((_tmp9CC->f4=(void*)(
(_tmp9C8=_region_malloc(r,sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C9.tag=3,((_tmp9C9.f1=& Cyc_cf_r,_tmp9C9)))),_tmp9C8)))),((_tmp9CC->f5=(
(_tmp9C7="Stop after control-flow checking",_tag_dyneither(_tmp9C7,sizeof(char),33))),_tmp9CC)))))))))))),((_tmp8C4[31]=(
# 1247
(_tmp9D2=_region_malloc(r,sizeof(*_tmp9D2)),((_tmp9D2->f1=((_tmp9D1="-noprint",_tag_dyneither(_tmp9D1,sizeof(char),9))),((_tmp9D2->f2=0,((_tmp9D2->f3=((_tmp9D0="",_tag_dyneither(_tmp9D0,sizeof(char),1))),((_tmp9D2->f4=(void*)(
(_tmp9CE=_region_malloc(r,sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=3,((_tmp9CF.f1=& Cyc_noprint_r,_tmp9CF)))),_tmp9CE)))),((_tmp9D2->f5=(
(_tmp9CD="Do not print output (when stopping early)",_tag_dyneither(_tmp9CD,sizeof(char),42))),_tmp9D2)))))))))))),((_tmp8C4[30]=(
# 1244
(_tmp9D8=_region_malloc(r,sizeof(*_tmp9D8)),((_tmp9D8->f1=((_tmp9D7="-stopafter-tc",_tag_dyneither(_tmp9D7,sizeof(char),14))),((_tmp9D8->f2=0,((_tmp9D8->f3=((_tmp9D6="",_tag_dyneither(_tmp9D6,sizeof(char),1))),((_tmp9D8->f4=(void*)(
(_tmp9D4=_region_malloc(r,sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=3,((_tmp9D5.f1=& Cyc_tc_r,_tmp9D5)))),_tmp9D4)))),((_tmp9D8->f5=(
(_tmp9D3="Stop after type checking",_tag_dyneither(_tmp9D3,sizeof(char),25))),_tmp9D8)))))))))))),((_tmp8C4[29]=(
# 1241
(_tmp9DE=_region_malloc(r,sizeof(*_tmp9DE)),((_tmp9DE->f1=((_tmp9DD="-stopafter-parse",_tag_dyneither(_tmp9DD,sizeof(char),17))),((_tmp9DE->f2=0,((_tmp9DE->f3=((_tmp9DC="",_tag_dyneither(_tmp9DC,sizeof(char),1))),((_tmp9DE->f4=(void*)(
(_tmp9DA=_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=3,((_tmp9DB.f1=& Cyc_parseonly_r,_tmp9DB)))),_tmp9DA)))),((_tmp9DE->f5=(
(_tmp9D9="Stop after parsing",_tag_dyneither(_tmp9D9,sizeof(char),19))),_tmp9DE)))))))))))),((_tmp8C4[28]=(
# 1238
(_tmp9E4=_region_malloc(r,sizeof(*_tmp9E4)),((_tmp9E4->f1=((_tmp9E3="-E",_tag_dyneither(_tmp9E3,sizeof(char),3))),((_tmp9E4->f2=0,((_tmp9E4->f3=((_tmp9E2="",_tag_dyneither(_tmp9E2,sizeof(char),1))),((_tmp9E4->f4=(void*)(
(_tmp9E0=_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=3,((_tmp9E1.f1=& Cyc_stop_after_cpp_r,_tmp9E1)))),_tmp9E0)))),((_tmp9E4->f5=(
(_tmp9DF="Stop after preprocessing",_tag_dyneither(_tmp9DF,sizeof(char),25))),_tmp9E4)))))))))))),((_tmp8C4[27]=(
# 1235
(_tmp9EA=_region_malloc(r,sizeof(*_tmp9EA)),((_tmp9EA->f1=((_tmp9E9="-Wall",_tag_dyneither(_tmp9E9,sizeof(char),6))),((_tmp9EA->f2=0,((_tmp9EA->f3=((_tmp9E8="",_tag_dyneither(_tmp9E8,sizeof(char),1))),((_tmp9EA->f4=(void*)(
(_tmp9E6=_region_malloc(r,sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=0,((_tmp9E7.f1=Cyc_set_all_warnings,_tmp9E7)))),_tmp9E6)))),((_tmp9EA->f5=(
(_tmp9E5="Turn on all warnings",_tag_dyneither(_tmp9E5,sizeof(char),21))),_tmp9EA)))))))))))),((_tmp8C4[26]=(
# 1232
(_tmp9F0=_region_malloc(r,sizeof(*_tmp9F0)),((_tmp9F0->f1=((_tmp9EF="-Wlose-unique",_tag_dyneither(_tmp9EF,sizeof(char),14))),((_tmp9F0->f2=0,((_tmp9F0->f3=((_tmp9EE="",_tag_dyneither(_tmp9EE,sizeof(char),1))),((_tmp9F0->f4=(void*)(
(_tmp9EC=_region_malloc(r,sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9ED.tag=3,((_tmp9ED.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmp9ED)))),_tmp9EC)))),((_tmp9F0->f5=(
(_tmp9EB="Try to warn when a unique pointer might get lost",_tag_dyneither(_tmp9EB,sizeof(char),49))),_tmp9F0)))))))))))),((_tmp8C4[25]=(
# 1229
(_tmp9F6=_region_malloc(r,sizeof(*_tmp9F6)),((_tmp9F6->f1=((_tmp9F5="-b",_tag_dyneither(_tmp9F5,sizeof(char),3))),((_tmp9F6->f2=0,((_tmp9F6->f3=((_tmp9F4="<arch>",_tag_dyneither(_tmp9F4,sizeof(char),7))),((_tmp9F6->f4=(void*)(
(_tmp9F2=_region_malloc(r,sizeof(*_tmp9F2)),((_tmp9F2[0]=((_tmp9F3.tag=5,((_tmp9F3.f1=Cyc_set_target_arch,_tmp9F3)))),_tmp9F2)))),((_tmp9F6->f5=(
(_tmp9F1="Set target architecture; implies -stopafter-toc",_tag_dyneither(_tmp9F1,sizeof(char),48))),_tmp9F6)))))))))))),((_tmp8C4[24]=(
# 1226
(_tmp9FC=_region_malloc(r,sizeof(*_tmp9FC)),((_tmp9FC->f1=((_tmp9FB="-MT",_tag_dyneither(_tmp9FB,sizeof(char),4))),((_tmp9FC->f2=0,((_tmp9FC->f3=((_tmp9FA=" <target>",_tag_dyneither(_tmp9FA,sizeof(char),10))),((_tmp9FC->f4=(void*)(
(_tmp9F8=_region_malloc(r,sizeof(*_tmp9F8)),((_tmp9F8[0]=((_tmp9F9.tag=5,((_tmp9F9.f1=Cyc_set_dependencies_target,_tmp9F9)))),_tmp9F8)))),((_tmp9FC->f5=(
(_tmp9F7="Give target for dependencies",_tag_dyneither(_tmp9F7,sizeof(char),29))),_tmp9FC)))))))))))),((_tmp8C4[23]=(
# 1222
(_tmpA02=_region_malloc(r,sizeof(*_tmpA02)),((_tmpA02->f1=((_tmpA01="-MG",_tag_dyneither(_tmpA01,sizeof(char),4))),((_tmpA02->f2=0,((_tmpA02->f3=((_tmpA00="",_tag_dyneither(_tmpA00,sizeof(char),1))),((_tmpA02->f4=(void*)(
(_tmp9FE=_region_malloc(r,sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmp9FF.tag=1,((_tmp9FF.f1=Cyc_add_cpparg,_tmp9FF)))),_tmp9FE)))),((_tmpA02->f5=(
# 1225
(_tmp9FD="When producing dependencies assume that missing files are generated",_tag_dyneither(_tmp9FD,sizeof(char),68))),_tmpA02)))))))))))),((_tmp8C4[22]=(
# 1219
(_tmpA08=_region_malloc(r,sizeof(*_tmpA08)),((_tmpA08->f1=((_tmpA07="-M",_tag_dyneither(_tmpA07,sizeof(char),3))),((_tmpA08->f2=0,((_tmpA08->f3=((_tmpA06="",_tag_dyneither(_tmpA06,sizeof(char),1))),((_tmpA08->f4=(void*)(
(_tmpA04=_region_malloc(r,sizeof(*_tmpA04)),((_tmpA04[0]=((_tmpA05.tag=0,((_tmpA05.f1=Cyc_set_produce_dependencies,_tmpA05)))),_tmpA04)))),((_tmpA08->f5=(
(_tmpA03="Produce dependencies",_tag_dyneither(_tmpA03,sizeof(char),21))),_tmpA08)))))))))))),((_tmp8C4[21]=(
# 1216
(_tmpA0E=_region_malloc(r,sizeof(*_tmpA0E)),((_tmpA0E->f1=((_tmpA0D="-S",_tag_dyneither(_tmpA0D,sizeof(char),3))),((_tmpA0E->f2=0,((_tmpA0E->f3=((_tmpA0C="",_tag_dyneither(_tmpA0C,sizeof(char),1))),((_tmpA0E->f4=(void*)(
(_tmpA0A=_region_malloc(r,sizeof(*_tmpA0A)),((_tmpA0A[0]=((_tmpA0B.tag=0,((_tmpA0B.f1=Cyc_set_stop_after_asmfile,_tmpA0B)))),_tmpA0A)))),((_tmpA0E->f5=(
(_tmpA09="Stop after producing assembly code",_tag_dyneither(_tmpA09,sizeof(char),35))),_tmpA0E)))))))))))),((_tmp8C4[20]=(
# 1213
(_tmpA14=_region_malloc(r,sizeof(*_tmpA14)),((_tmpA14->f1=((_tmpA13="-pa",_tag_dyneither(_tmpA13,sizeof(char),4))),((_tmpA14->f2=0,((_tmpA14->f3=((_tmpA12="",_tag_dyneither(_tmpA12,sizeof(char),1))),((_tmpA14->f4=(void*)(
(_tmpA10=_region_malloc(r,sizeof(*_tmpA10)),((_tmpA10[0]=((_tmpA11.tag=0,((_tmpA11.f1=Cyc_set_pa,_tmpA11)))),_tmpA10)))),((_tmpA14->f5=(
(_tmpA0F="Compile for profiling with aprof",_tag_dyneither(_tmpA0F,sizeof(char),33))),_tmpA14)))))))))))),((_tmp8C4[19]=(
# 1210
(_tmpA1A=_region_malloc(r,sizeof(*_tmpA1A)),((_tmpA1A->f1=((_tmpA19="-pg",_tag_dyneither(_tmpA19,sizeof(char),4))),((_tmpA1A->f2=0,((_tmpA1A->f3=((_tmpA18="",_tag_dyneither(_tmpA18,sizeof(char),1))),((_tmpA1A->f4=(void*)(
(_tmpA16=_region_malloc(r,sizeof(*_tmpA16)),((_tmpA16[0]=((_tmpA17.tag=0,((_tmpA17.f1=Cyc_set_pg,_tmpA17)))),_tmpA16)))),((_tmpA1A->f5=(
(_tmpA15="Compile for profiling with gprof",_tag_dyneither(_tmpA15,sizeof(char),33))),_tmpA1A)))))))))))),((_tmp8C4[18]=(
# 1207
(_tmpA20=_region_malloc(r,sizeof(*_tmpA20)),((_tmpA20->f1=((_tmpA1F="-p",_tag_dyneither(_tmpA1F,sizeof(char),3))),((_tmpA20->f2=0,((_tmpA20->f3=((_tmpA1E="",_tag_dyneither(_tmpA1E,sizeof(char),1))),((_tmpA20->f4=(void*)(
(_tmpA1C=_region_malloc(r,sizeof(*_tmpA1C)),((_tmpA1C[0]=((_tmpA1D.tag=1,((_tmpA1D.f1=Cyc_add_ccarg,_tmpA1D)))),_tmpA1C)))),((_tmpA20->f5=(
(_tmpA1B="Compile for profiling with prof",_tag_dyneither(_tmpA1B,sizeof(char),32))),_tmpA20)))))))))))),((_tmp8C4[17]=(
# 1204
(_tmpA26=_region_malloc(r,sizeof(*_tmpA26)),((_tmpA26->f1=((_tmpA25="-g",_tag_dyneither(_tmpA25,sizeof(char),3))),((_tmpA26->f2=0,((_tmpA26->f3=((_tmpA24="",_tag_dyneither(_tmpA24,sizeof(char),1))),((_tmpA26->f4=(void*)(
(_tmpA22=_region_malloc(r,sizeof(*_tmpA22)),((_tmpA22[0]=((_tmpA23.tag=1,((_tmpA23.f1=Cyc_add_ccarg,_tmpA23)))),_tmpA22)))),((_tmpA26->f5=(
(_tmpA21="Compile for debugging",_tag_dyneither(_tmpA21,sizeof(char),22))),_tmpA26)))))))))))),((_tmp8C4[16]=(
# 1201
(_tmpA2C=_region_malloc(r,sizeof(*_tmpA2C)),((_tmpA2C->f1=((_tmpA2B="-fomit-frame-pointer",_tag_dyneither(_tmpA2B,sizeof(char),21))),((_tmpA2C->f2=0,((_tmpA2C->f3=((_tmpA2A="",_tag_dyneither(_tmpA2A,sizeof(char),1))),((_tmpA2C->f4=(void*)(
(_tmpA28=_region_malloc(r,sizeof(*_tmpA28)),((_tmpA28[0]=((_tmpA29.tag=1,((_tmpA29.f1=Cyc_add_ccarg,_tmpA29)))),_tmpA28)))),((_tmpA2C->f5=(
(_tmpA27="Omit frame pointer",_tag_dyneither(_tmpA27,sizeof(char),19))),_tmpA2C)))))))))))),((_tmp8C4[15]=(
# 1198
(_tmpA32=_region_malloc(r,sizeof(*_tmpA32)),((_tmpA32->f1=((_tmpA31="-O3",_tag_dyneither(_tmpA31,sizeof(char),4))),((_tmpA32->f2=0,((_tmpA32->f3=((_tmpA30="",_tag_dyneither(_tmpA30,sizeof(char),1))),((_tmpA32->f4=(void*)(
(_tmpA2E=_region_malloc(r,sizeof(*_tmpA2E)),((_tmpA2E[0]=((_tmpA2F.tag=1,((_tmpA2F.f1=Cyc_add_ccarg,_tmpA2F)))),_tmpA2E)))),((_tmpA32->f5=(
(_tmpA2D="Even more optimization",_tag_dyneither(_tmpA2D,sizeof(char),23))),_tmpA32)))))))))))),((_tmp8C4[14]=(
# 1195
(_tmpA38=_region_malloc(r,sizeof(*_tmpA38)),((_tmpA38->f1=((_tmpA37="-O2",_tag_dyneither(_tmpA37,sizeof(char),4))),((_tmpA38->f2=0,((_tmpA38->f3=((_tmpA36="",_tag_dyneither(_tmpA36,sizeof(char),1))),((_tmpA38->f4=(void*)(
(_tmpA34=_region_malloc(r,sizeof(*_tmpA34)),((_tmpA34[0]=((_tmpA35.tag=1,((_tmpA35.f1=Cyc_add_ccarg,_tmpA35)))),_tmpA34)))),((_tmpA38->f5=(
(_tmpA33="A higher level of optimization",_tag_dyneither(_tmpA33,sizeof(char),31))),_tmpA38)))))))))))),((_tmp8C4[13]=(
# 1192
(_tmpA3E=_region_malloc(r,sizeof(*_tmpA3E)),((_tmpA3E->f1=((_tmpA3D="-O",_tag_dyneither(_tmpA3D,sizeof(char),3))),((_tmpA3E->f2=0,((_tmpA3E->f3=((_tmpA3C="",_tag_dyneither(_tmpA3C,sizeof(char),1))),((_tmpA3E->f4=(void*)(
(_tmpA3A=_region_malloc(r,sizeof(*_tmpA3A)),((_tmpA3A[0]=((_tmpA3B.tag=1,((_tmpA3B.f1=Cyc_add_ccarg,_tmpA3B)))),_tmpA3A)))),((_tmpA3E->f5=(
(_tmpA39="Optimize",_tag_dyneither(_tmpA39,sizeof(char),9))),_tmpA3E)))))))))))),((_tmp8C4[12]=(
# 1189
(_tmpA44=_region_malloc(r,sizeof(*_tmpA44)),((_tmpA44->f1=((_tmpA43="-O0",_tag_dyneither(_tmpA43,sizeof(char),4))),((_tmpA44->f2=0,((_tmpA44->f3=((_tmpA42="",_tag_dyneither(_tmpA42,sizeof(char),1))),((_tmpA44->f4=(void*)(
(_tmpA40=_region_malloc(r,sizeof(*_tmpA40)),((_tmpA40[0]=((_tmpA41.tag=1,((_tmpA41.f1=Cyc_add_ccarg,_tmpA41)))),_tmpA40)))),((_tmpA44->f5=(
(_tmpA3F="Don't optimize",_tag_dyneither(_tmpA3F,sizeof(char),15))),_tmpA44)))))))))))),((_tmp8C4[11]=(
# 1186
(_tmpA4A=_region_malloc(r,sizeof(*_tmpA4A)),((_tmpA4A->f1=((_tmpA49="-s",_tag_dyneither(_tmpA49,sizeof(char),3))),((_tmpA4A->f2=0,((_tmpA4A->f3=((_tmpA48="",_tag_dyneither(_tmpA48,sizeof(char),1))),((_tmpA4A->f4=(void*)(
(_tmpA46=_region_malloc(r,sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=1,((_tmpA47.f1=Cyc_add_ccarg,_tmpA47)))),_tmpA46)))),((_tmpA4A->f5=(
(_tmpA45="Remove all symbol table and relocation info from executable",_tag_dyneither(_tmpA45,sizeof(char),60))),_tmpA4A)))))))))))),((_tmp8C4[10]=(
# 1183
(_tmpA50=_region_malloc(r,sizeof(*_tmpA50)),((_tmpA50->f1=((_tmpA4F="-x",_tag_dyneither(_tmpA4F,sizeof(char),3))),((_tmpA50->f2=0,((_tmpA50->f3=((_tmpA4E=" <language>",_tag_dyneither(_tmpA4E,sizeof(char),12))),((_tmpA50->f4=(void*)(
(_tmpA4C=_region_malloc(r,sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=5,((_tmpA4D.f1=Cyc_set_inputtype,_tmpA4D)))),_tmpA4C)))),((_tmpA50->f5=(
(_tmpA4B="Specify <language> for the following input files",_tag_dyneither(_tmpA4B,sizeof(char),49))),_tmpA50)))))))))))),((_tmp8C4[9]=(
# 1180
(_tmpA56=_region_malloc(r,sizeof(*_tmpA56)),((_tmpA56->f1=((_tmpA55="-c",_tag_dyneither(_tmpA55,sizeof(char),3))),((_tmpA56->f2=0,((_tmpA56->f3=((_tmpA54="",_tag_dyneither(_tmpA54,sizeof(char),1))),((_tmpA56->f4=(void*)(
(_tmpA52=_region_malloc(r,sizeof(*_tmpA52)),((_tmpA52[0]=((_tmpA53.tag=0,((_tmpA53.f1=Cyc_set_stop_after_objectfile,_tmpA53)))),_tmpA52)))),((_tmpA56->f5=(
(_tmpA51="Produce an object file instead of an executable; do not link",_tag_dyneither(_tmpA51,sizeof(char),61))),_tmpA56)))))))))))),((_tmp8C4[8]=(
# 1177
(_tmpA5C=_region_malloc(r,sizeof(*_tmpA5C)),((_tmpA5C->f1=((_tmpA5B="-m",_tag_dyneither(_tmpA5B,sizeof(char),3))),((_tmpA5C->f2=1,((_tmpA5C->f3=((_tmpA5A="<option>",_tag_dyneither(_tmpA5A,sizeof(char),9))),((_tmpA5C->f4=(void*)(
(_tmpA58=_region_malloc(r,sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA59.tag=1,((_tmpA59.f1=Cyc_add_marg,_tmpA59)))),_tmpA58)))),((_tmpA5C->f5=(
(_tmpA57="GCC specific: pass machine-dependent flag on to gcc",_tag_dyneither(_tmpA57,sizeof(char),52))),_tmpA5C)))))))))))),((_tmp8C4[7]=(
# 1174
(_tmpA62=_region_malloc(r,sizeof(*_tmpA62)),((_tmpA62->f1=((_tmpA61="-l",_tag_dyneither(_tmpA61,sizeof(char),3))),((_tmpA62->f2=1,((_tmpA62->f3=((_tmpA60="<libname>",_tag_dyneither(_tmpA60,sizeof(char),10))),((_tmpA62->f4=(void*)(
(_tmpA5E=_region_malloc(r,sizeof(*_tmpA5E)),((_tmpA5E[0]=((_tmpA5F.tag=1,((_tmpA5F.f1=Cyc_add_libarg,_tmpA5F)))),_tmpA5E)))),((_tmpA62->f5=(
(_tmpA5D="Library file",_tag_dyneither(_tmpA5D,sizeof(char),13))),_tmpA62)))))))))))),((_tmp8C4[6]=(
# 1171
(_tmpA68=_region_malloc(r,sizeof(*_tmpA68)),((_tmpA68->f1=((_tmpA67="-L",_tag_dyneither(_tmpA67,sizeof(char),3))),((_tmpA68->f2=1,((_tmpA68->f3=((_tmpA66="<dir>",_tag_dyneither(_tmpA66,sizeof(char),6))),((_tmpA68->f4=(void*)(
(_tmpA64=_region_malloc(r,sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA65.tag=1,((_tmpA65.f1=Cyc_add_ccarg,_tmpA65)))),_tmpA64)))),((_tmpA68->f5=(
(_tmpA63="Add to the list of directories for -l",_tag_dyneither(_tmpA63,sizeof(char),38))),_tmpA68)))))))))))),((_tmp8C4[5]=(
# 1168
(_tmpA6E=_region_malloc(r,sizeof(*_tmpA6E)),((_tmpA6E->f1=((_tmpA6D="-I",_tag_dyneither(_tmpA6D,sizeof(char),3))),((_tmpA6E->f2=1,((_tmpA6E->f3=((_tmpA6C="<dir>",_tag_dyneither(_tmpA6C,sizeof(char),6))),((_tmpA6E->f4=(void*)(
(_tmpA6A=_region_malloc(r,sizeof(*_tmpA6A)),((_tmpA6A[0]=((_tmpA6B.tag=1,((_tmpA6B.f1=Cyc_add_cpparg,_tmpA6B)))),_tmpA6A)))),((_tmpA6E->f5=(
(_tmpA69="Add to the list of directories to search for include files",_tag_dyneither(_tmpA69,sizeof(char),59))),_tmpA6E)))))))))))),((_tmp8C4[4]=(
# 1165
(_tmpA74=_region_malloc(r,sizeof(*_tmpA74)),((_tmpA74->f1=((_tmpA73="-B",_tag_dyneither(_tmpA73,sizeof(char),3))),((_tmpA74->f2=1,((_tmpA74->f3=((_tmpA72="<file>",_tag_dyneither(_tmpA72,sizeof(char),7))),((_tmpA74->f4=(void*)(
(_tmpA70=_region_malloc(r,sizeof(*_tmpA70)),((_tmpA70[0]=((_tmpA71.tag=1,((_tmpA71.f1=Cyc_add_cyclone_exec_path,_tmpA71)))),_tmpA70)))),((_tmpA74->f5=(
(_tmpA6F="Add to the list of directories to search for compiler files",_tag_dyneither(_tmpA6F,sizeof(char),60))),_tmpA74)))))))))))),((_tmp8C4[3]=(
# 1162
(_tmpA7A=_region_malloc(r,sizeof(*_tmpA7A)),((_tmpA7A->f1=((_tmpA79="-D",_tag_dyneither(_tmpA79,sizeof(char),3))),((_tmpA7A->f2=1,((_tmpA7A->f3=((_tmpA78="<name>[=<value>]",_tag_dyneither(_tmpA78,sizeof(char),17))),((_tmpA7A->f4=(void*)(
(_tmpA76=_region_malloc(r,sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA77.tag=1,((_tmpA77.f1=Cyc_add_cpparg,_tmpA77)))),_tmpA76)))),((_tmpA7A->f5=(
(_tmpA75="Pass definition to preprocessor",_tag_dyneither(_tmpA75,sizeof(char),32))),_tmpA7A)))))))))))),((_tmp8C4[2]=(
# 1159
(_tmpA80=_region_malloc(r,sizeof(*_tmpA80)),((_tmpA80->f1=((_tmpA7F="-o",_tag_dyneither(_tmpA7F,sizeof(char),3))),((_tmpA80->f2=0,((_tmpA80->f3=((_tmpA7E=" <file>",_tag_dyneither(_tmpA7E,sizeof(char),8))),((_tmpA80->f4=(void*)(
(_tmpA7C=_region_malloc(r,sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7D.tag=5,((_tmpA7D.f1=Cyc_set_output_file,_tmpA7D)))),_tmpA7C)))),((_tmpA80->f5=(
(_tmpA7B="Set the output file name to <file>",_tag_dyneither(_tmpA7B,sizeof(char),35))),_tmpA80)))))))))))),((_tmp8C4[1]=(
# 1156
(_tmpA86=_region_malloc(r,sizeof(*_tmpA86)),((_tmpA86->f1=((_tmpA85="--version",_tag_dyneither(_tmpA85,sizeof(char),10))),((_tmpA86->f2=0,((_tmpA86->f3=((_tmpA84="",_tag_dyneither(_tmpA84,sizeof(char),1))),((_tmpA86->f4=(void*)(
(_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA83.tag=0,((_tmpA83.f1=Cyc_print_version,_tmpA83)))),_tmpA82)))),((_tmpA86->f5=(
(_tmpA81="Print version information and exit",_tag_dyneither(_tmpA81,sizeof(char),35))),_tmpA86)))))))))))),((_tmp8C4[0]=(
# 1153
(_tmpA8C=_region_malloc(r,sizeof(*_tmpA8C)),((_tmpA8C->f1=((_tmpA8B="-v",_tag_dyneither(_tmpA8B,sizeof(char),3))),((_tmpA8C->f2=0,((_tmpA8C->f3=((_tmpA8A="",_tag_dyneither(_tmpA8A,sizeof(char),1))),((_tmpA8C->f4=(void*)(
(_tmpA88=_region_malloc(r,sizeof(*_tmpA88)),((_tmpA88[0]=((_tmpA89.tag=3,((_tmpA89.f1=& Cyc_v_r,_tmpA89)))),_tmpA88)))),((_tmpA8C->f5=(
(_tmpA87="Print compilation stages verbosely",_tag_dyneither(_tmpA87,sizeof(char),35))),_tmpA8C)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp8C4,sizeof(struct _tuple24*),76)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
# 1385
const char*_tmpA8D;struct _dyneither_ptr optstring=(_tmpA8D="Options:",_tag_dyneither(_tmpA8D,sizeof(char),9));
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp16F);
# 1390
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{
# 1398
char*_tmpA8E;struct _dyneither_ptr cyclone_exec_prefix=(_tmpA8E=getenv("CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpA8E,sizeof(char),_get_zero_arr_size_char((void*)_tmpA8E,1)));
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct _dyneither_ptr*_tmpA91;struct Cyc_List_List*_tmpA90;Cyc_cyclone_exec_path=((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->hd=((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91[0]=cyclone_exec_prefix,_tmpA91)))),((_tmpA90->tl=Cyc_cyclone_exec_path,_tmpA90))))));}
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0){
struct _dyneither_ptr*_tmpA97;const char*_tmpA96;struct Cyc_List_List*_tmpA95;Cyc_cyclone_exec_path=(
(_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->hd=((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97[0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpA96="cyc-lib",_tag_dyneither(_tmpA96,sizeof(char),8)))),_tmpA97)))),((_tmpA95->tl=Cyc_cyclone_exec_path,_tmpA95))))));}
# 1406
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1414
const char*_tmpA98;struct _dyneither_ptr _tmp176=Cyc_do_find(cyclone_arch_path,((_tmpA98="cycspecs",_tag_dyneither(_tmpA98,sizeof(char),9))));
if(Cyc_v_r){const char*_tmpA9C;void*_tmpA9B[1];struct Cyc_String_pa_PrintArg_struct _tmpA9A;(_tmpA9A.tag=0,((_tmpA9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp176),((_tmpA9B[0]=& _tmpA9A,Cyc_fprintf(Cyc_stderr,((_tmpA9C="Reading from specs file %s\n",_tag_dyneither(_tmpA9C,sizeof(char),28))),_tag_dyneither(_tmpA9B,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp17A=Cyc_read_specs(_tmp176);
struct _dyneither_ptr _tmp17B=_tag_dyneither(0,0,0);
{struct _handler_cons _tmp17C;_push_handler(& _tmp17C);{int _tmp17E=0;if(setjmp(_tmp17C.handler))_tmp17E=1;if(!_tmp17E){
{struct _dyneither_ptr _tmp17F=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp17A,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
_tmp17B=Cyc_split_specs(_tmp17F);}
# 1419
;_pop_handler();}else{void*_tmp17D=(void*)_exn_thrown;void*_tmp182=_tmp17D;void*_tmp184;_LL35: {struct Cyc_Core_Not_found_exn_struct*_tmp183=(struct Cyc_Core_Not_found_exn_struct*)_tmp182;if(_tmp183->tag != Cyc_Core_Not_found)goto _LL37;}_LL36:
# 1422
 goto _LL34;_LL37: _tmp184=_tmp182;_LL38:(void)_throw(_tmp184);_LL34:;}};}
# 1424
if((struct _dyneither_ptr*)_tmp17B.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp17B);}
# 1429
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp170);{
const char*_tmpA9D;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmpA9D="",_tag_dyneither(_tmpA9D,sizeof(char),1))))== 0){
# 1447 "cyclone.cyc"
const char*_tmp186=Cyc_produce_dependencies?"":" -E";
const char*_tmpAA5;void*_tmpAA4[3];struct Cyc_String_pa_PrintArg_struct _tmpAA3;const char*_tmpAA2;struct Cyc_String_pa_PrintArg_struct _tmpAA1;struct Cyc_String_pa_PrintArg_struct _tmpAA0;Cyc_set_cpp((struct _dyneither_ptr)((_tmpAA0.tag=0,((_tmpAA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp176),((_tmpAA1.tag=0,((_tmpAA1.f1=(struct _dyneither_ptr)((_tmpAA2=_tmp186,_tag_dyneither(_tmpAA2,sizeof(char),_get_zero_arr_size_char((void*)_tmpAA2,1)))),((_tmpAA3.tag=0,((_tmpAA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpAA4[0]=& _tmpAA3,((_tmpAA4[1]=& _tmpAA1,((_tmpAA4[2]=& _tmpAA0,Cyc_aprintf(((_tmpAA5="%s -w -x c%s -specs %s",_tag_dyneither(_tmpAA5,sizeof(char),23))),_tag_dyneither(_tmpAA4,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
# 1451
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
{const char*_tmpAA8;void*_tmpAA7;(_tmpAA7=0,Cyc_fprintf(Cyc_stderr,((_tmpAA8="missing file\n",_tag_dyneither(_tmpAA8,sizeof(char),14))),_tag_dyneither(_tmpAA7,sizeof(void*),0)));}
exit(1);}
# 1460
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){
const char*_tmpAA9;Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,((_tmpAA9="cyc_setjmp.h",_tag_dyneither(_tmpAA9,sizeof(char),13))));}
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0){
const char*_tmpAAA;Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmpAAA="cyc_include.h",_tag_dyneither(_tmpAAA,sizeof(char),14))));}
# 1466
{struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp369 != 0;_tmp369=_tmp369->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp369->hd));
if(Cyc_compile_failure){int _tmp36A=1;_npop_handler(0);return _tmp36A;}}}
# 1471
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp36B=0;_npop_handler(0);return _tmp36B;}
# 1477
if(Cyc_ccargs == 0){int _tmp36C=0;_npop_handler(0);return _tmp36C;}
# 1480
{const char*_tmpAAB;Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(((_tmpAAB="-L",_tag_dyneither(_tmpAAB,sizeof(char),3))),(struct _dyneither_ptr)def_lib_path));}
# 1482
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
const char*_tmpAAC;struct _dyneither_ptr _tmp36E=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpAAC=" ",_tag_dyneither(_tmpAAC,sizeof(char),2))));
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
const char*_tmpAAE;struct Cyc_List_List*_tmpAAD;struct _dyneither_ptr _tmp36F=Cyc_str_sepstr(((_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmpAAD->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs),_tmpAAD)))))),((_tmpAAE=" ",_tag_dyneither(_tmpAAE,sizeof(char),2))));
# 1488
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
const char*_tmpAB0;const char*_tmpAAF;int _tmp370=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1493
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAB0=".a",_tag_dyneither(_tmpAB0,sizeof(char),3))))) || 
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAAF=".lib",_tag_dyneither(_tmpAAF,sizeof(char),5))));
if(_tmp370){
stdlib=0;{
const char*_tmpAB1;stdlib_string=(struct _dyneither_ptr)((_tmpAB1="",_tag_dyneither(_tmpAB1,sizeof(char),1)));};}else{
# 1501
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
{const char*_tmpAB2;libcyc_flag=((_tmpAB2="-lcyc_a",_tag_dyneither(_tmpAB2,sizeof(char),8)));}
{const char*_tmpAB3;nogc_filename=((_tmpAB3="nogc_a.a",_tag_dyneither(_tmpAB3,sizeof(char),9)));}{
const char*_tmpAB4;runtime_filename=((_tmpAB4="runtime_cyc_a.o",_tag_dyneither(_tmpAB4,sizeof(char),16)));};}else{
if(Cyc_nocheck_r){
{const char*_tmpAB5;libcyc_flag=((_tmpAB5="-lcyc_nocheck",_tag_dyneither(_tmpAB5,sizeof(char),14)));}
{const char*_tmpAB6;nogc_filename=((_tmpAB6="nogc.a",_tag_dyneither(_tmpAB6,sizeof(char),7)));}{
const char*_tmpAB7;runtime_filename=((_tmpAB7="runtime_cyc.o",_tag_dyneither(_tmpAB7,sizeof(char),14)));};}else{
if(Cyc_pg_r){
{const char*_tmpAB8;libcyc_flag=((_tmpAB8="-lcyc_pg",_tag_dyneither(_tmpAB8,sizeof(char),9)));}
{const char*_tmpAB9;runtime_filename=((_tmpAB9="runtime_cyc_pg.o",_tag_dyneither(_tmpAB9,sizeof(char),17)));}{
const char*_tmpABA;nogc_filename=((_tmpABA="nogc_pg.a",_tag_dyneither(_tmpABA,sizeof(char),10)));};}else{
if(Cyc_Lex_compile_for_boot_r){
# 1519
{const char*_tmpABB;libcyc_flag=((_tmpABB="-lcycboot",_tag_dyneither(_tmpABB,sizeof(char),10)));}
{const char*_tmpABC;nogc_filename=((_tmpABC="nogc.a",_tag_dyneither(_tmpABC,sizeof(char),7)));}{
const char*_tmpABD;runtime_filename=((_tmpABD="runtime_cyc.o",_tag_dyneither(_tmpABD,sizeof(char),14)));};}else{
# 1523
{const char*_tmpABE;libcyc_flag=((_tmpABE="-lcyc",_tag_dyneither(_tmpABE,sizeof(char),6)));}
{const char*_tmpABF;nogc_filename=((_tmpABF="nogc.a",_tag_dyneither(_tmpABF,sizeof(char),7)));}{
const char*_tmpAC0;runtime_filename=((_tmpAC0="runtime_cyc.o",_tag_dyneither(_tmpAC0,sizeof(char),14)));};}}}}
# 1527
{const char*_tmpAC1;gc_filename=((_tmpAC1="gc.a",_tag_dyneither(_tmpAC1,sizeof(char),5)));}{
# 1529
struct _dyneither_ptr _tmp382=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename):
 Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dyneither_ptr _tmp383=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1536
stdlib=0;{
const char*_tmpAC7;void*_tmpAC6[3];struct Cyc_String_pa_PrintArg_struct _tmpAC5;struct Cyc_String_pa_PrintArg_struct _tmpAC4;struct Cyc_String_pa_PrintArg_struct _tmpAC3;stdlib_string=(struct _dyneither_ptr)((_tmpAC3.tag=0,((_tmpAC3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp382),((_tmpAC4.tag=0,((_tmpAC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpAC5.tag=0,((_tmpAC5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp383),((_tmpAC6[0]=& _tmpAC5,((_tmpAC6[1]=& _tmpAC4,((_tmpAC6[2]=& _tmpAC3,Cyc_aprintf(((_tmpAC7=" %s %s %s",_tag_dyneither(_tmpAC7,sizeof(char),10))),_tag_dyneither(_tmpAC6,sizeof(void*),3))))))))))))))))))));};};}
# 1540
if(Cyc_ic_r){struct _handler_cons _tmp389;_push_handler(& _tmp389);{int _tmp38B=0;if(setjmp(_tmp389.handler))_tmp38B=1;if(!_tmp38B){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp38C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp370){
struct Cyc_List_List*_tmpAC8;_tmp38C=((_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8->hd=Cyc_final_strptr,((_tmpAC8->tl=_tmp38C,_tmpAC8))))));}{
# 1548
struct Cyc_Interface_I*_tmp38E=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp38C,_tmp38C);
if(_tmp38E == 0){
{const char*_tmpACB;void*_tmpACA;(_tmpACA=0,Cyc_fprintf(Cyc_stderr,((_tmpACB="Error: interfaces incompatible\n",_tag_dyneither(_tmpACB,sizeof(char),32))),_tag_dyneither(_tmpACA,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp391=1;_npop_handler(1);return _tmp391;};}
# 1555
if(_tmp370){
if(Cyc_output_file != 0){
const char*_tmpACF;void*_tmpACE[1];struct Cyc_String_pa_PrintArg_struct _tmpACD;struct _dyneither_ptr _tmp392=(_tmpACD.tag=0,((_tmpACD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpACE[0]=& _tmpACD,Cyc_aprintf(((_tmpACF="%s.cycio",_tag_dyneither(_tmpACF,sizeof(char),9))),_tag_dyneither(_tmpACE,sizeof(void*),1)))))));
const char*_tmpAD1;const char*_tmpAD0;struct Cyc___cycFILE*_tmp393=Cyc_try_file_open((struct _dyneither_ptr)_tmp392,((_tmpAD0="wb",_tag_dyneither(_tmpAD0,sizeof(char),3))),((_tmpAD1="interface object file",_tag_dyneither(_tmpAD1,sizeof(char),22))));
if(_tmp393 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp394=1;_npop_handler(1);return _tmp394;};}
# 1564
Cyc_Interface_save(_tmp38E,_tmp393);
Cyc_file_close(_tmp393);}}else{
# 1568
const char*_tmpAD6;const char*_tmpAD5;struct _tuple20*_tmpAD4;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),_tmp38E,(
(_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->f1=((_tmpAD5="empty interface",_tag_dyneither(_tmpAD5,sizeof(char),16))),((_tmpAD4->f2=((_tmpAD6="global interface",_tag_dyneither(_tmpAD6,sizeof(char),17))),_tmpAD4)))))))){
{const char*_tmpAD9;void*_tmpAD8;(_tmpAD8=0,Cyc_fprintf(Cyc_stderr,((_tmpAD9="Error: some objects are still undefined\n",_tag_dyneither(_tmpAD9,sizeof(char),41))),_tag_dyneither(_tmpAD8,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp39F=1;_npop_handler(1);return _tmp39F;};}}};};
# 1541
;_pop_handler();}else{void*_tmp38A=(void*)_exn_thrown;void*_tmp3A1=_tmp38A;void*_tmp3A2;void*_tmp3A3;_LL3A: _tmp3A2=_tmp3A1;_LL3B:
# 1577
{void*_tmp3A4=_tmp3A2;struct _dyneither_ptr _tmp3A6;struct _dyneither_ptr _tmp3A8;struct _dyneither_ptr _tmp3AB;_LL3F: {struct Cyc_Core_Failure_exn_struct*_tmp3A5=(struct Cyc_Core_Failure_exn_struct*)_tmp3A4;if(_tmp3A5->tag != Cyc_Core_Failure)goto _LL41;else{_tmp3A6=_tmp3A5->f1;}}_LL40:
# 1579
{const char*_tmpADD;void*_tmpADC[1];struct Cyc_String_pa_PrintArg_struct _tmpADB;(_tmpADB.tag=0,((_tmpADB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3A6),((_tmpADC[0]=& _tmpADB,Cyc_fprintf(Cyc_stderr,((_tmpADD="Exception Core::Failure %s\n",_tag_dyneither(_tmpADD,sizeof(char),28))),_tag_dyneither(_tmpADC,sizeof(void*),1)))))));}goto _LL3E;_LL41: {struct Cyc_Core_Impossible_exn_struct*_tmp3A7=(struct Cyc_Core_Impossible_exn_struct*)_tmp3A4;if(_tmp3A7->tag != Cyc_Core_Impossible)goto _LL43;else{_tmp3A8=_tmp3A7->f1;}}_LL42:
# 1581
{const char*_tmpAE1;void*_tmpAE0[1];struct Cyc_String_pa_PrintArg_struct _tmpADF;(_tmpADF.tag=0,((_tmpADF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3A8),((_tmpAE0[0]=& _tmpADF,Cyc_fprintf(Cyc_stderr,((_tmpAE1="Exception Core::Impossible %s\n",_tag_dyneither(_tmpAE1,sizeof(char),31))),_tag_dyneither(_tmpAE0,sizeof(void*),1)))))));}goto _LL3E;_LL43: {struct Cyc_Dict_Absent_exn_struct*_tmp3A9=(struct Cyc_Dict_Absent_exn_struct*)_tmp3A4;if(_tmp3A9->tag != Cyc_Dict_Absent)goto _LL45;}_LL44:
# 1583
{const char*_tmpAE4;void*_tmpAE3;(_tmpAE3=0,Cyc_fprintf(Cyc_stderr,((_tmpAE4="Exception Dict::Absent\n",_tag_dyneither(_tmpAE4,sizeof(char),24))),_tag_dyneither(_tmpAE3,sizeof(void*),0)));}goto _LL3E;_LL45: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp3AA=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp3A4;if(_tmp3AA->tag != Cyc_Core_Invalid_argument)goto _LL47;else{_tmp3AB=_tmp3AA->f1;}}_LL46:
# 1585
{const char*_tmpAE8;void*_tmpAE7[1];struct Cyc_String_pa_PrintArg_struct _tmpAE6;(_tmpAE6.tag=0,((_tmpAE6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AB),((_tmpAE7[0]=& _tmpAE6,Cyc_fprintf(Cyc_stderr,((_tmpAE8="Exception Core::Invalid_argument %s\n",_tag_dyneither(_tmpAE8,sizeof(char),37))),_tag_dyneither(_tmpAE7,sizeof(void*),1)))))));}goto _LL3E;_LL47:;_LL48:
# 1587
{const char*_tmpAEB;void*_tmpAEA;(_tmpAEA=0,Cyc_fprintf(Cyc_stderr,((_tmpAEB="Uncaught exception\n",_tag_dyneither(_tmpAEB,sizeof(char),20))),_tag_dyneither(_tmpAEA,sizeof(void*),0)));}goto _LL3E;_LL3E:;}
# 1589
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3B9=1;_npop_handler(0);return _tmp3B9;};_LL3C: _tmp3A3=_tmp3A1;_LL3D:(void)_throw(_tmp3A3);_LL39:;}};}{
# 1594
const char*_tmpAEC;struct _dyneither_ptr outfile_str=(_tmpAEC="",_tag_dyneither(_tmpAEC,sizeof(char),1));
if(Cyc_output_file != 0){
const char*_tmpAF0;void*_tmpAEF[1];struct Cyc_String_pa_PrintArg_struct _tmpAEE;outfile_str=(struct _dyneither_ptr)((_tmpAEE.tag=0,((_tmpAEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpAEF[0]=& _tmpAEE,Cyc_aprintf(((_tmpAF0=" -o %s",_tag_dyneither(_tmpAF0,sizeof(char),7))),_tag_dyneither(_tmpAEF,sizeof(void*),1))))))));}{
# 1598
const char*_tmpAF8;void*_tmpAF7[5];struct Cyc_String_pa_PrintArg_struct _tmpAF6;struct Cyc_String_pa_PrintArg_struct _tmpAF5;struct Cyc_String_pa_PrintArg_struct _tmpAF4;struct Cyc_String_pa_PrintArg_struct _tmpAF3;struct Cyc_String_pa_PrintArg_struct _tmpAF2;struct _dyneither_ptr _tmp3BD=
(_tmpAF2.tag=0,((_tmpAF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36F),((_tmpAF3.tag=0,((_tmpAF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string),((_tmpAF4.tag=0,((_tmpAF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36E),((_tmpAF5.tag=0,((_tmpAF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpAF6.tag=0,((_tmpAF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpAF7[0]=& _tmpAF6,((_tmpAF7[1]=& _tmpAF5,((_tmpAF7[2]=& _tmpAF4,((_tmpAF7[3]=& _tmpAF3,((_tmpAF7[4]=& _tmpAF2,Cyc_aprintf(((_tmpAF8="%s %s %s%s%s",_tag_dyneither(_tmpAF8,sizeof(char),13))),_tag_dyneither(_tmpAF7,sizeof(void*),5)))))))))))))))))))))))))))))));
# 1601
if(Cyc_v_r){{const char*_tmpAFC;void*_tmpAFB[1];struct Cyc_String_pa_PrintArg_struct _tmpAFA;(_tmpAFA.tag=0,((_tmpAFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3BD),((_tmpAFB[0]=& _tmpAFA,Cyc_fprintf(Cyc_stderr,((_tmpAFC="%s\n",_tag_dyneither(_tmpAFC,sizeof(char),4))),_tag_dyneither(_tmpAFB,sizeof(void*),1)))))));}Cyc_fflush(Cyc_stderr);}
if(system((const char*)_untag_dyneither_ptr(_tmp3BD,sizeof(char),1))!= 0){
{const char*_tmpAFF;void*_tmpAFE;(_tmpAFE=0,Cyc_fprintf(Cyc_stderr,((_tmpAFF="Error: C compiler failed\n",_tag_dyneither(_tmpAFF,sizeof(char),26))),_tag_dyneither(_tmpAFE,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3C3=1;_npop_handler(0);return _tmp3C3;};}
# 1608
Cyc_remove_cfiles();{
# 1610
int _tmp3C4=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp3C4;};};};};};
# 1121 "cyclone.cyc"
;_pop_region(r);};}
