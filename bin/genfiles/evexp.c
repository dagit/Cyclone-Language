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
typedef int Cyc_bool;struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
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
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 244 "core.h"
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
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
# 527
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1002 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 62
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 65
void*Cyc_Tcutil_compress(void*t);
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 150
int Cyc_Tcutil_unify(void*,void*);
# 152
int Cyc_Tcutil_typecmp(void*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 338 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple10{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple11{union Cyc_Absyn_Cnst f1;int f2;};
# 41 "evexp.cyc"
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 43
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
union Cyc_Absyn_Cnst _tmp0=cn;enum Cyc_Absyn_Sign _tmp1;char _tmp2;enum Cyc_Absyn_Sign _tmp3;short _tmp4;_LL1: if((_tmp0.Char_c).tag != 2)goto _LL3;_tmp1=((struct _tuple3)(_tmp0.Char_c).val).f1;_tmp2=((struct _tuple3)(_tmp0.Char_c).val).f2;_LL2:
 return Cyc_Absyn_Int_c(_tmp1,(int)_tmp2);_LL3: if((_tmp0.Short_c).tag != 4)goto _LL5;_tmp3=((struct _tuple4)(_tmp0.Short_c).val).f1;_tmp4=((struct _tuple4)(_tmp0.Short_c).val).f2;_LL4:
# 47
 return Cyc_Absyn_Int_c(_tmp3,(int)_tmp4);_LL5:;_LL6:
 return cn;_LL0:;}
# 52
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
union Cyc_Absyn_Cnst _tmp6;int _tmp7;struct _tuple11 _tmp5=Cyc_Evexp_eval_const_exp(e);_tmp6=_tmp5.f1;_tmp7=_tmp5.f2;
if(!_tmp7){
struct _tuple10 _tmp176;return(_tmp176.f1=0,((_tmp176.f2=0,_tmp176)));}{
union Cyc_Absyn_Cnst _tmp9=Cyc_Evexp_promote_const(_tmp6);int _tmpA;_LL8: if((_tmp9.Int_c).tag != 5)goto _LLA;_tmpA=((struct _tuple5)(_tmp9.Int_c).val).f2;_LL9: {
struct _tuple10 _tmp177;return(_tmp177.f1=(unsigned int)_tmpA,((_tmp177.f2=1,_tmp177)));}_LLA:;_LLB:
{const char*_tmp17A;void*_tmp179;(_tmp179=0,Cyc_Tcutil_terr(e->loc,((_tmp17A="expecting unsigned int",_tag_dyneither(_tmp17A,sizeof(char),23))),_tag_dyneither(_tmp179,sizeof(void*),0)));}{struct _tuple10 _tmp17B;return(_tmp17B.f1=0,((_tmp17B.f2=1,_tmp17B)));};_LL7:;};}struct _tuple12{int f1;int f2;};
# 62
static struct _tuple12 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
union Cyc_Absyn_Cnst _tmp10;int _tmp11;struct _tuple11 _tmpF=Cyc_Evexp_eval_const_exp(e);_tmp10=_tmpF.f1;_tmp11=_tmpF.f2;
if(!_tmp11){
struct _tuple12 _tmp17C;return(_tmp17C.f1=0,((_tmp17C.f2=0,_tmp17C)));}{
union Cyc_Absyn_Cnst _tmp13=Cyc_Evexp_promote_const(_tmp10);int _tmp14;long long _tmp15;_LLD: if((_tmp13.Int_c).tag != 5)goto _LLF;_tmp14=((struct _tuple5)(_tmp13.Int_c).val).f2;_LLE: {
struct _tuple12 _tmp17D;return(_tmp17D.f1=_tmp14 != 0,((_tmp17D.f2=1,_tmp17D)));}_LLF: if((_tmp13.LongLong_c).tag != 6)goto _LL11;_tmp15=((struct _tuple6)(_tmp13.LongLong_c).val).f2;_LL10: {
struct _tuple12 _tmp17E;return(_tmp17E.f1=_tmp15 != 0,((_tmp17E.f2=1,_tmp17E)));}_LL11: if((_tmp13.Null_c).tag != 1)goto _LL13;_LL12: {
struct _tuple12 _tmp17F;return(_tmp17F.f1=0,((_tmp17F.f2=0,_tmp17F)));}_LL13:;_LL14:
{const char*_tmp182;void*_tmp181;(_tmp181=0,Cyc_Tcutil_terr(e->loc,((_tmp182="expecting bool",_tag_dyneither(_tmp182,sizeof(char),15))),_tag_dyneither(_tmp181,sizeof(void*),0)));}{struct _tuple12 _tmp183;return(_tmp183.f1=0,((_tmp183.f2=0,_tmp183)));};_LLC:;};}struct _tuple13{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 74
static struct _tuple11 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
union Cyc_Absyn_Cnst _tmp1D;int _tmp1E;struct _tuple11 _tmp1C=Cyc_Evexp_eval_const_exp(e);_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;
if(!_tmp1E){
struct _tuple11 _tmp184;return(_tmp184.f1=_tmp1D,((_tmp184.f2=0,_tmp184)));}
{struct _tuple13 _tmp185;struct _tuple13 _tmp21=(_tmp185.f1=p,((_tmp185.f2=_tmp1D,_tmp185)));enum Cyc_Absyn_Sign _tmp22;int _tmp23;enum Cyc_Absyn_Sign _tmp24;int _tmp25;int _tmp26;_LL16: if(_tmp21.f1 != Cyc_Absyn_Plus)goto _LL18;_LL17:
 goto _LL15;_LL18: if(_tmp21.f1 != Cyc_Absyn_Minus)goto _LL1A;if(((_tmp21.f2).Int_c).tag != 5)goto _LL1A;_tmp22=((struct _tuple5)((_tmp21.f2).Int_c).val).f1;_tmp23=((struct _tuple5)((_tmp21.f2).Int_c).val).f2;_LL19:
 _tmp1D=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- _tmp23);goto _LL15;_LL1A: if(_tmp21.f1 != Cyc_Absyn_Bitnot)goto _LL1C;if(((_tmp21.f2).Int_c).tag != 5)goto _LL1C;_tmp24=((struct _tuple5)((_tmp21.f2).Int_c).val).f1;_tmp25=((struct _tuple5)((_tmp21.f2).Int_c).val).f2;_LL1B:
 _tmp1D=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ _tmp25);goto _LL15;_LL1C: if(_tmp21.f1 != Cyc_Absyn_Not)goto _LL1E;if(((_tmp21.f2).Int_c).tag != 5)goto _LL1E;_tmp26=((struct _tuple5)((_tmp21.f2).Int_c).val).f2;_LL1D:
 _tmp1D=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,_tmp26 == 0?1: 0);goto _LL15;_LL1E: if(_tmp21.f1 != Cyc_Absyn_Not)goto _LL20;if(((_tmp21.f2).Null_c).tag != 1)goto _LL20;_LL1F:
 _tmp1D=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL15;_LL20:;_LL21:
{const char*_tmp188;void*_tmp187;(_tmp187=0,Cyc_Tcutil_terr(e->loc,((_tmp188="bad constant expression",_tag_dyneither(_tmp188,sizeof(char),24))),_tag_dyneither(_tmp187,sizeof(void*),0)));}goto _LL15;_LL15:;}{
# 86
struct _tuple11 _tmp189;return(_tmp189.f1=_tmp1D,((_tmp189.f2=1,_tmp189)));};}struct _tuple14{enum Cyc_Absyn_Primop f1;int f2;};
# 90
static struct _tuple11 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
union Cyc_Absyn_Cnst _tmp2B;int _tmp2C;struct _tuple11 _tmp2A=Cyc_Evexp_eval_const_exp(e1);_tmp2B=_tmp2A.f1;_tmp2C=_tmp2A.f2;{
union Cyc_Absyn_Cnst _tmp2E;int _tmp2F;struct _tuple11 _tmp2D=Cyc_Evexp_eval_const_exp(e2);_tmp2E=_tmp2D.f1;_tmp2F=_tmp2D.f2;
if(!_tmp2C  || !_tmp2F){
struct _tuple11 _tmp18A;return(_tmp18A.f1=_tmp2B,((_tmp18A.f2=0,_tmp18A)));}
_tmp2B=Cyc_Evexp_promote_const(_tmp2B);
_tmp2E=Cyc_Evexp_promote_const(_tmp2E);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{union Cyc_Absyn_Cnst _tmp31=_tmp2B;enum Cyc_Absyn_Sign _tmp32;int _tmp33;_LL23: if((_tmp31.Int_c).tag != 5)goto _LL25;_tmp32=((struct _tuple5)(_tmp31.Int_c).val).f1;_tmp33=((struct _tuple5)(_tmp31.Int_c).val).f2;_LL24:
 s1=_tmp32;i1=_tmp33;goto _LL22;_LL25:;_LL26:
{const char*_tmp18D;void*_tmp18C;(_tmp18C=0,Cyc_Tcutil_terr(e1->loc,((_tmp18D="bad constant expression",_tag_dyneither(_tmp18D,sizeof(char),24))),_tag_dyneither(_tmp18C,sizeof(void*),0)));}{struct _tuple11 _tmp18E;return(_tmp18E.f1=_tmp2B,((_tmp18E.f2=1,_tmp18E)));};_LL22:;}
# 103
{union Cyc_Absyn_Cnst _tmp37=_tmp2E;enum Cyc_Absyn_Sign _tmp38;int _tmp39;_LL28: if((_tmp37.Int_c).tag != 5)goto _LL2A;_tmp38=((struct _tuple5)(_tmp37.Int_c).val).f1;_tmp39=((struct _tuple5)(_tmp37.Int_c).val).f2;_LL29:
 s2=_tmp38;i2=_tmp39;goto _LL27;_LL2A:;_LL2B:
{const char*_tmp191;void*_tmp190;(_tmp190=0,Cyc_Tcutil_terr(e2->loc,((_tmp191="bad constant expression",_tag_dyneither(_tmp191,sizeof(char),24))),_tag_dyneither(_tmp190,sizeof(void*),0)));}{struct _tuple11 _tmp192;return(_tmp192.f1=_tmp2B,((_tmp192.f2=1,_tmp192)));};_LL27:;}
# 107
switch(p){case Cyc_Absyn_Div: _LL2C:
 goto _LL2D;case Cyc_Absyn_Mod: _LL2D:
# 110
 if(i2 == 0){
{const char*_tmp195;void*_tmp194;(_tmp194=0,Cyc_Tcutil_terr(e2->loc,((_tmp195="division by zero in constant expression",_tag_dyneither(_tmp195,sizeof(char),40))),_tag_dyneither(_tmp194,sizeof(void*),0)));}{
struct _tuple11 _tmp196;return(_tmp196.f1=_tmp2B,((_tmp196.f2=1,_tmp196)));};}
# 114
break;default: _LL2E:
 break;}{
# 117
int has_u_arg=s1 == Cyc_Absyn_Unsigned  || s2 == Cyc_Absyn_Unsigned;
unsigned int u1=(unsigned int)i1;
unsigned int u2=(unsigned int)i2;
int i3=0;
unsigned int u3=0;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;
{struct _tuple14 _tmp197;struct _tuple14 _tmp41=(_tmp197.f1=p,((_tmp197.f2=has_u_arg,_tmp197)));_LL31: if(_tmp41.f1 != Cyc_Absyn_Plus)goto _LL33;if(_tmp41.f2 != 0)goto _LL33;_LL32:
 i3=i1 + i2;use_i3=1;goto _LL30;_LL33: if(_tmp41.f1 != Cyc_Absyn_Times)goto _LL35;if(_tmp41.f2 != 0)goto _LL35;_LL34:
 i3=i1 * i2;use_i3=1;goto _LL30;_LL35: if(_tmp41.f1 != Cyc_Absyn_Minus)goto _LL37;if(_tmp41.f2 != 0)goto _LL37;_LL36:
 i3=i1 - i2;use_i3=1;goto _LL30;_LL37: if(_tmp41.f1 != Cyc_Absyn_Div)goto _LL39;if(_tmp41.f2 != 0)goto _LL39;_LL38:
 i3=i1 / i2;use_i3=1;goto _LL30;_LL39: if(_tmp41.f1 != Cyc_Absyn_Mod)goto _LL3B;if(_tmp41.f2 != 0)goto _LL3B;_LL3A:
 i3=i1 % i2;use_i3=1;goto _LL30;_LL3B: if(_tmp41.f1 != Cyc_Absyn_Plus)goto _LL3D;if(_tmp41.f2 != 1)goto _LL3D;_LL3C:
 u3=u1 + u2;use_u3=1;goto _LL30;_LL3D: if(_tmp41.f1 != Cyc_Absyn_Times)goto _LL3F;if(_tmp41.f2 != 1)goto _LL3F;_LL3E:
 u3=u1 * u2;use_u3=1;goto _LL30;_LL3F: if(_tmp41.f1 != Cyc_Absyn_Minus)goto _LL41;if(_tmp41.f2 != 1)goto _LL41;_LL40:
 u3=u1 - u2;use_u3=1;goto _LL30;_LL41: if(_tmp41.f1 != Cyc_Absyn_Div)goto _LL43;if(_tmp41.f2 != 1)goto _LL43;_LL42:
 u3=u1 / u2;use_u3=1;goto _LL30;_LL43: if(_tmp41.f1 != Cyc_Absyn_Mod)goto _LL45;if(_tmp41.f2 != 1)goto _LL45;_LL44:
 u3=u1 % u2;use_u3=1;goto _LL30;_LL45: if(_tmp41.f1 != Cyc_Absyn_Eq)goto _LL47;_LL46:
 b3=i1 == i2;use_b3=1;goto _LL30;_LL47: if(_tmp41.f1 != Cyc_Absyn_Neq)goto _LL49;_LL48:
 b3=i1 != i2;use_b3=1;goto _LL30;_LL49: if(_tmp41.f1 != Cyc_Absyn_Gt)goto _LL4B;if(_tmp41.f2 != 0)goto _LL4B;_LL4A:
 b3=i1 > i2;use_b3=1;goto _LL30;_LL4B: if(_tmp41.f1 != Cyc_Absyn_Lt)goto _LL4D;if(_tmp41.f2 != 0)goto _LL4D;_LL4C:
 b3=i1 < i2;use_b3=1;goto _LL30;_LL4D: if(_tmp41.f1 != Cyc_Absyn_Gte)goto _LL4F;if(_tmp41.f2 != 0)goto _LL4F;_LL4E:
 b3=i1 >= i2;use_b3=1;goto _LL30;_LL4F: if(_tmp41.f1 != Cyc_Absyn_Lte)goto _LL51;if(_tmp41.f2 != 0)goto _LL51;_LL50:
 b3=i1 <= i2;use_b3=1;goto _LL30;_LL51: if(_tmp41.f1 != Cyc_Absyn_Gt)goto _LL53;if(_tmp41.f2 != 1)goto _LL53;_LL52:
 b3=u1 > u2;use_b3=1;goto _LL30;_LL53: if(_tmp41.f1 != Cyc_Absyn_Lt)goto _LL55;if(_tmp41.f2 != 1)goto _LL55;_LL54:
 b3=u1 < u2;use_b3=1;goto _LL30;_LL55: if(_tmp41.f1 != Cyc_Absyn_Gte)goto _LL57;if(_tmp41.f2 != 1)goto _LL57;_LL56:
 b3=u1 >= u2;use_b3=1;goto _LL30;_LL57: if(_tmp41.f1 != Cyc_Absyn_Lte)goto _LL59;if(_tmp41.f2 != 1)goto _LL59;_LL58:
 b3=u1 <= u2;use_b3=1;goto _LL30;_LL59: if(_tmp41.f1 != Cyc_Absyn_Bitand)goto _LL5B;_LL5A:
 u3=u1 & u2;use_u3=1;goto _LL30;_LL5B: if(_tmp41.f1 != Cyc_Absyn_Bitor)goto _LL5D;_LL5C:
 u3=u1 | u2;use_u3=1;goto _LL30;_LL5D: if(_tmp41.f1 != Cyc_Absyn_Bitxor)goto _LL5F;_LL5E:
 u3=u1 ^ u2;use_u3=1;goto _LL30;_LL5F: if(_tmp41.f1 != Cyc_Absyn_Bitlshift)goto _LL61;_LL60:
 u3=u1 << u2;use_u3=1;goto _LL30;_LL61: if(_tmp41.f1 != Cyc_Absyn_Bitlrshift)goto _LL63;_LL62:
 u3=u1 >> u2;use_u3=1;goto _LL30;_LL63: if(_tmp41.f1 != Cyc_Absyn_Bitarshift)goto _LL65;_LL64:
{const char*_tmp19A;void*_tmp199;(_tmp199=0,Cyc_Tcutil_terr(e1->loc,((_tmp19A=">>> NOT IMPLEMENTED",_tag_dyneither(_tmp19A,sizeof(char),20))),_tag_dyneither(_tmp199,sizeof(void*),0)));}goto _LL66;_LL65:;_LL66:
{const char*_tmp19D;void*_tmp19C;(_tmp19C=0,Cyc_Tcutil_terr(e1->loc,((_tmp19D="bad constant expression",_tag_dyneither(_tmp19D,sizeof(char),24))),_tag_dyneither(_tmp19C,sizeof(void*),0)));}{struct _tuple11 _tmp19E;return(_tmp19E.f1=_tmp2B,((_tmp19E.f2=1,_tmp19E)));};_LL30:;}
# 155
if(use_i3){struct _tuple11 _tmp19F;return(_tmp19F.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3),((_tmp19F.f2=1,_tmp19F)));}
if(use_u3){struct _tuple11 _tmp1A0;return(_tmp1A0.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3),((_tmp1A0.f2=1,_tmp1A0)));}
if(use_b3){struct _tuple11 _tmp1A1;return(_tmp1A1.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0),((_tmp1A1.f2=1,_tmp1A1)));}{
const char*_tmp1A4;void*_tmp1A3;(_tmp1A3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1A4="Evexp::eval_const_binop",_tag_dyneither(_tmp1A4,sizeof(char),24))),_tag_dyneither(_tmp1A3,sizeof(void*),0)));};};};};}struct _tuple15{void*f1;union Cyc_Absyn_Cnst f2;};
# 165
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 ans;
{void*_tmp4C=e->r;union Cyc_Absyn_Cnst _tmp4E;struct Cyc_Absyn_Exp*_tmp50;struct Cyc_Absyn_Exp*_tmp51;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_Exp*_tmp58;enum Cyc_Absyn_Primop _tmp5A;struct Cyc_List_List*_tmp5B;void*_tmp5D;void*_tmp62;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_Absyn_Enumfield*_tmp65;struct Cyc_Absyn_Enumfield*_tmp67;_LL68: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp4D->tag != 0)goto _LL6A;else{_tmp4E=_tmp4D->f1;}}_LL69: {
struct _tuple11 _tmp1A5;return(_tmp1A5.f1=_tmp4E,((_tmp1A5.f2=1,_tmp1A5)));}_LL6A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp4F->tag != 5)goto _LL6C;else{_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;_tmp52=_tmp4F->f3;}}_LL6B: {
# 170
int _tmp6A;int _tmp6B;struct _tuple12 _tmp69=Cyc_Evexp_eval_const_bool_exp(_tmp50);_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;
if(!_tmp6B){
Cyc_Evexp_eval_const_exp(_tmp51);
Cyc_Evexp_eval_const_exp(_tmp52);{
struct _tuple11 _tmp1A6;return(_tmp1A6.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1A6.f2=0,_tmp1A6)));};}
# 176
ans=_tmp6A?Cyc_Evexp_eval_const_exp(_tmp51): Cyc_Evexp_eval_const_exp(_tmp52);
goto _LL67;}_LL6C: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp53=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp53->tag != 6)goto _LL6E;else{_tmp54=_tmp53->f1;_tmp55=_tmp53->f2;}}_LL6D: {
# 179
int _tmp6E;int _tmp6F;struct _tuple12 _tmp6D=Cyc_Evexp_eval_const_bool_exp(_tmp54);_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
if(!_tmp6F){
Cyc_Evexp_eval_const_exp(_tmp55);{
struct _tuple11 _tmp1A7;return(_tmp1A7.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1A7.f2=0,_tmp1A7)));};}
# 184
if(_tmp6E)ans=Cyc_Evexp_eval_const_exp(_tmp55);else{
struct _tuple11 _tmp1A8;ans=((_tmp1A8.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1A8.f2=1,_tmp1A8))));}
goto _LL67;}_LL6E: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp56=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp56->tag != 7)goto _LL70;else{_tmp57=_tmp56->f1;_tmp58=_tmp56->f2;}}_LL6F: {
# 188
int _tmp73;int _tmp74;struct _tuple12 _tmp72=Cyc_Evexp_eval_const_bool_exp(_tmp57);_tmp73=_tmp72.f1;_tmp74=_tmp72.f2;
if(!_tmp74){
Cyc_Evexp_eval_const_exp(_tmp58);{
struct _tuple11 _tmp1A9;return(_tmp1A9.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1A9.f2=0,_tmp1A9)));};}
# 193
if(!_tmp73)ans=Cyc_Evexp_eval_const_exp(_tmp58);else{
struct _tuple11 _tmp1AA;ans=((_tmp1AA.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1),((_tmp1AA.f2=1,_tmp1AA))));}
goto _LL67;}_LL70: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp59=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp59->tag != 2)goto _LL72;else{_tmp5A=_tmp59->f1;_tmp5B=_tmp59->f2;}}_LL71:
# 197
 if(_tmp5B == 0){
{const char*_tmp1AD;void*_tmp1AC;(_tmp1AC=0,Cyc_Tcutil_terr(e->loc,((_tmp1AD="bad static expression (no args to primop)",_tag_dyneither(_tmp1AD,sizeof(char),42))),_tag_dyneither(_tmp1AC,sizeof(void*),0)));}{
struct _tuple11 _tmp1AE;return(_tmp1AE.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1AE.f2=1,_tmp1AE)));};}
# 201
if(_tmp5B->tl == 0){
ans=Cyc_Evexp_eval_const_unprimop(_tmp5A,(struct Cyc_Absyn_Exp*)_tmp5B->hd);
goto _LL67;}
# 205
if(((struct Cyc_List_List*)_check_null(_tmp5B->tl))->tl == 0){
ans=Cyc_Evexp_eval_const_binprimop(_tmp5A,(struct Cyc_Absyn_Exp*)_tmp5B->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5B->tl))->hd);
goto _LL67;}
# 209
{const char*_tmp1B1;void*_tmp1B0;(_tmp1B0=0,Cyc_Tcutil_terr(e->loc,((_tmp1B1="bad static expression (too many args to primop)",_tag_dyneither(_tmp1B1,sizeof(char),48))),_tag_dyneither(_tmp1B0,sizeof(void*),0)));}{
struct _tuple11 _tmp1B2;return(_tmp1B2.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1B2.f2=1,_tmp1B2)));};_LL72: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp5C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp5C->tag != 37)goto _LL74;else{_tmp5D=(void*)_tmp5C->f1;}}_LL73:
# 213
{void*_tmp7D=Cyc_Tcutil_compress(_tmp5D);struct Cyc_Absyn_Exp*_tmp7F;_LL83: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7D;if(_tmp7E->tag != 18)goto _LL85;else{_tmp7F=_tmp7E->f1;}}_LL84:
 return Cyc_Evexp_eval_const_exp(_tmp7F);_LL85:;_LL86:
 goto _LL82;_LL82:;}
# 217
goto _LL75;_LL74: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp5E=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp5E->tag != 16)goto _LL76;}_LL75:
 goto _LL77;_LL76: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp5F=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp5F->tag != 17)goto _LL78;}_LL77:
 goto _LL79;_LL78: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp60=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp60->tag != 18)goto _LL7A;}_LL79:
{struct _tuple11 _tmp1B3;ans=((_tmp1B3.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1B3.f2=0,_tmp1B3))));}goto _LL67;_LL7A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp61=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp61->tag != 13)goto _LL7C;else{_tmp62=(void*)_tmp61->f1;_tmp63=_tmp61->f2;}}_LL7B:
# 223
 ans=Cyc_Evexp_eval_const_exp(_tmp63);
if(ans.f2){
struct _tuple15 _tmp1B4;struct _tuple15 _tmp82=(_tmp1B4.f1=Cyc_Tcutil_compress(_tmp62),((_tmp1B4.f2=ans.f1,_tmp1B4)));enum Cyc_Absyn_Sign _tmp84;enum Cyc_Absyn_Size_of _tmp85;enum Cyc_Absyn_Sign _tmp86;char _tmp87;enum Cyc_Absyn_Sign _tmp89;enum Cyc_Absyn_Size_of _tmp8A;enum Cyc_Absyn_Sign _tmp8B;short _tmp8C;enum Cyc_Absyn_Sign _tmp8E;enum Cyc_Absyn_Size_of _tmp8F;enum Cyc_Absyn_Sign _tmp90;int _tmp91;void*_tmp93;enum Cyc_Absyn_Sign _tmp94;char _tmp95;void*_tmp97;enum Cyc_Absyn_Sign _tmp98;short _tmp99;void*_tmp9B;enum Cyc_Absyn_Sign _tmp9C;int _tmp9D;_LL88:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp82.f1;if(_tmp83->tag != 6)goto _LL8A;else{_tmp84=_tmp83->f1;_tmp85=_tmp83->f2;}}if(((_tmp82.f2).Char_c).tag != 2)goto _LL8A;_tmp86=((struct _tuple3)((_tmp82.f2).Char_c).val).f1;_tmp87=((struct _tuple3)((_tmp82.f2).Char_c).val).f2;_LL89:
# 227
 _tmp89=_tmp84;_tmp8A=_tmp85;_tmp8B=_tmp86;_tmp8C=(short)_tmp87;goto _LL8B;_LL8A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp88=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp82.f1;if(_tmp88->tag != 6)goto _LL8C;else{_tmp89=_tmp88->f1;_tmp8A=_tmp88->f2;}}if(((_tmp82.f2).Short_c).tag != 4)goto _LL8C;_tmp8B=((struct _tuple4)((_tmp82.f2).Short_c).val).f1;_tmp8C=((struct _tuple4)((_tmp82.f2).Short_c).val).f2;_LL8B:
 _tmp8E=_tmp89;_tmp8F=_tmp8A;_tmp90=_tmp8B;_tmp91=(int)_tmp8C;goto _LL8D;_LL8C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp82.f1;if(_tmp8D->tag != 6)goto _LL8E;else{_tmp8E=_tmp8D->f1;_tmp8F=_tmp8D->f2;}}if(((_tmp82.f2).Int_c).tag != 5)goto _LL8E;_tmp90=((struct _tuple5)((_tmp82.f2).Int_c).val).f1;_tmp91=((struct _tuple5)((_tmp82.f2).Int_c).val).f2;_LL8D:
# 230
 if(_tmp8E != _tmp90)
ans.f1=Cyc_Absyn_Int_c(_tmp8E,_tmp91);
goto _LL87;_LL8E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp92=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp82.f1;if(_tmp92->tag != 19)goto _LL90;else{_tmp93=(void*)_tmp92->f1;}}if(((_tmp82.f2).Char_c).tag != 2)goto _LL90;_tmp94=((struct _tuple3)((_tmp82.f2).Char_c).val).f1;_tmp95=((struct _tuple3)((_tmp82.f2).Char_c).val).f2;_LL8F:
# 234
 _tmp97=_tmp93;_tmp98=_tmp94;_tmp99=(short)_tmp95;goto _LL91;_LL90:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp96=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp82.f1;if(_tmp96->tag != 19)goto _LL92;else{_tmp97=(void*)_tmp96->f1;}}if(((_tmp82.f2).Short_c).tag != 4)goto _LL92;_tmp98=((struct _tuple4)((_tmp82.f2).Short_c).val).f1;_tmp99=((struct _tuple4)((_tmp82.f2).Short_c).val).f2;_LL91:
 _tmp9B=_tmp97;_tmp9C=_tmp98;_tmp9D=(int)_tmp99;goto _LL93;_LL92:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp82.f1;if(_tmp9A->tag != 19)goto _LL94;else{_tmp9B=(void*)_tmp9A->f1;}}if(((_tmp82.f2).Int_c).tag != 5)goto _LL94;_tmp9C=((struct _tuple5)((_tmp82.f2).Int_c).val).f1;_tmp9D=((struct _tuple5)((_tmp82.f2).Int_c).val).f2;_LL93:
# 237
 if(_tmp9D < 0){
const char*_tmp1B7;void*_tmp1B6;(_tmp1B6=0,Cyc_Tcutil_terr(e->loc,((_tmp1B7="cannot cast negative number to a tag type",_tag_dyneither(_tmp1B7,sizeof(char),42))),_tag_dyneither(_tmp1B6,sizeof(void*),0)));}
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1BA;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1B9;Cyc_Tcutil_unify(_tmp9B,(void*)((_tmp1B9=_cycalloc(sizeof(*_tmp1B9)),((_tmp1B9[0]=((_tmp1BA.tag=18,((_tmp1BA.f1=Cyc_Absyn_const_exp(ans.f1,0),_tmp1BA)))),_tmp1B9)))));}
ans.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp9D);
goto _LL87;_LL94:;_LL95:
{const char*_tmp1BE;void*_tmp1BD[1];struct Cyc_String_pa_PrintArg_struct _tmp1BC;(_tmp1BC.tag=0,((_tmp1BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp62)),((_tmp1BD[0]=& _tmp1BC,Cyc_Tcutil_terr(e->loc,((_tmp1BE="cannot cast to %s",_tag_dyneither(_tmp1BE,sizeof(char),18))),_tag_dyneither(_tmp1BD,sizeof(void*),1)))))));}goto _LL87;_LL87:;}
# 244
goto _LL67;_LL7C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp64=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp64->tag != 31)goto _LL7E;else{_tmp65=_tmp64->f3;}}_LL7D:
 _tmp67=_tmp65;goto _LL7F;_LL7E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp66=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4C;if(_tmp66->tag != 30)goto _LL80;else{_tmp67=_tmp66->f3;}}_LL7F:
 ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_check_null(_tmp67))->tag));goto _LL67;_LL80:;_LL81:
# 248
{const char*_tmp1C2;void*_tmp1C1[1];struct Cyc_String_pa_PrintArg_struct _tmp1C0;(_tmp1C0.tag=0,((_tmp1C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1C1[0]=& _tmp1C0,Cyc_Tcutil_terr(e->loc,((_tmp1C2="bad static expression %s",_tag_dyneither(_tmp1C2,sizeof(char),25))),_tag_dyneither(_tmp1C1,sizeof(void*),1)))))));}{
struct _tuple11 _tmp1C3;return(_tmp1C3.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1C3.f2=1,_tmp1C3)));};_LL67:;}
# 251
if(ans.f2){
void*c;
union Cyc_Absyn_Cnst _tmpA9=ans.f1;
{union Cyc_Absyn_Cnst _tmpAA=_tmpA9;_LL97: if((_tmpAA.Int_c).tag != 5)goto _LL99;if(((struct _tuple5)(_tmpAA.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL99;if(((struct _tuple5)(_tmpAA.Int_c).val).f2 != 0)goto _LL99;_LL98: {
# 256
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
# 258
c=(void*)& uzero;
goto _LL96;}_LL99: if((_tmpAA.Int_c).tag != 5)goto _LL9B;if(((struct _tuple5)(_tmpAA.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL9B;if(((struct _tuple5)(_tmpAA.Int_c).val).f2 != 1)goto _LL9B;_LL9A: {
# 261
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 263
c=(void*)& uone;
goto _LL96;}_LL9B: if((_tmpAA.Int_c).tag != 5)goto _LL9D;if(((struct _tuple5)(_tmpAA.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL9D;if(((struct _tuple5)(_tmpAA.Int_c).val).f2 != 0)goto _LL9D;_LL9C: {
# 266
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 268
c=(void*)& szero;
goto _LL96;}_LL9D: if((_tmpAA.Int_c).tag != 5)goto _LL9F;if(((struct _tuple5)(_tmpAA.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL9F;if(((struct _tuple5)(_tmpAA.Int_c).val).f2 != 1)goto _LL9F;_LL9E: {
# 271
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
# 273
c=(void*)& sone;
goto _LL96;}_LL9F:;_LLA0:
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp1C6;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1C5;c=(void*)((_tmp1C5=_cycalloc(sizeof(*_tmp1C5)),((_tmp1C5[0]=((_tmp1C6.tag=0,((_tmp1C6.f1=_tmpA9,_tmp1C6)))),_tmp1C5))));}goto _LL96;_LL96:;}
# 277
e->r=c;}
# 279
return ans;}
# 282
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_tmpB1=e->r;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Absyn_Exp*_tmpC1;enum Cyc_Absyn_Primop _tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_Absyn_Exp*_tmpC7;_LLA2: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB2=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpB2->tag != 31)goto _LLA4;}_LLA3:
 goto _LLA5;_LLA4: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB3=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpB3->tag != 30)goto _LLA6;}_LLA5:
 goto _LLA7;_LLA6: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB4=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpB4->tag != 16)goto _LLA8;}_LLA7:
 goto _LLA9;_LLA8: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpB5=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpB5->tag != 17)goto _LLAA;}_LLA9:
 goto _LLAB;_LLAA: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB6=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpB6->tag != 18)goto _LLAC;}_LLAB:
 goto _LLAD;_LLAC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpB7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpB7->tag != 0)goto _LLAE;}_LLAD:
 return 1;_LLAE: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB8=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpB8->tag != 5)goto _LLB0;else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB8->f2;_tmpBB=_tmpB8->f3;}}_LLAF:
# 291
 return(Cyc_Evexp_c_can_eval(_tmpB9) && Cyc_Evexp_c_can_eval(_tmpBA)) && Cyc_Evexp_c_can_eval(_tmpBB);_LLB0: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpBC=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpBC->tag != 6)goto _LLB2;else{_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;}}_LLB1:
 _tmpC0=_tmpBD;_tmpC1=_tmpBE;goto _LLB3;_LLB2: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpBF=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpBF->tag != 7)goto _LLB4;else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;}}_LLB3:
# 294
 return Cyc_Evexp_c_can_eval(_tmpC0) && Cyc_Evexp_c_can_eval(_tmpC1);_LLB4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpC2=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpC2->tag != 2)goto _LLB6;else{_tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;}}_LLB5:
# 296
 for(0;_tmpC4 != 0;_tmpC4=_tmpC4->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_tmpC4->hd))return 0;}
return 1;_LLB6: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpC5=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpC5->tag != 37)goto _LLB8;}_LLB7:
 return 0;_LLB8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB1;if(_tmpC6->tag != 13)goto _LLBA;else{_tmpC7=_tmpC6->f2;}}_LLB9:
 return Cyc_Evexp_c_can_eval(_tmpC7);_LLBA:;_LLBB:
 return 0;_LLA1:;}
# 305
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmpC8=e->r;_LLBD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpC9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC8;if(_tmpC9->tag != 0)goto _LLBF;}_LLBE:
 return 1;_LLBF: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCA=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC8;if(_tmpCA->tag != 5)goto _LLC1;}_LLC0:
 return 2;_LLC1: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpCB=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC8;if(_tmpCB->tag != 2)goto _LLC3;}_LLC2:
 return 3;_LLC3: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCC=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpC8;if(_tmpCC->tag != 16)goto _LLC5;}_LLC4:
 goto _LLC6;_LLC5: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCD=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpC8;if(_tmpCD->tag != 17)goto _LLC7;}_LLC6:
 return 4;_LLC7: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCE=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpC8;if(_tmpCE->tag != 18)goto _LLC9;}_LLC8:
 return 5;_LLC9: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC8;if(_tmpCF->tag != 13)goto _LLCB;}_LLCA:
 return 6;_LLCB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpD0=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC8;if(_tmpD0->tag != 6)goto _LLCD;}_LLCC:
 return 7;_LLCD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpD1=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC8;if(_tmpD1->tag != 7)goto _LLCF;}_LLCE:
 return 8;_LLCF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpD2=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpC8;if(_tmpD2->tag != 37)goto _LLD1;}_LLD0:
 return 9;_LLD1:;_LLD2:
# 318
{const char*_tmp1CA;void*_tmp1C9[1];struct Cyc_String_pa_PrintArg_struct _tmp1C8;(_tmp1C8.tag=0,((_tmp1C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1C9[0]=& _tmp1C8,Cyc_Tcutil_terr(e->loc,((_tmp1CA="bad static expression %s",_tag_dyneither(_tmp1CA,sizeof(char),25))),_tag_dyneither(_tmp1C9,sizeof(void*),1)))))));}return 0;_LLBC:;}struct _tuple16{void*f1;void*f2;};
# 322
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
unsigned int _tmpD7;int _tmpD8;struct _tuple10 _tmpD6=Cyc_Evexp_eval_const_uint_exp(e1);_tmpD7=_tmpD6.f1;_tmpD8=_tmpD6.f2;{
unsigned int _tmpDA;int _tmpDB;struct _tuple10 _tmpD9=Cyc_Evexp_eval_const_uint_exp(e2);_tmpDA=_tmpD9.f1;_tmpDB=_tmpD9.f2;
if(_tmpD8  && _tmpDB)
return(int)(_tmpD7 - _tmpDA);{
int _tmpDC=Cyc_Evexp_const_exp_case_number(e1);
int _tmpDD=Cyc_Evexp_const_exp_case_number(e2);
if(_tmpDC != _tmpDD)
return _tmpDC - _tmpDD;{
struct _tuple16 _tmp1CB;struct _tuple16 _tmpDF=(_tmp1CB.f1=e1->r,((_tmp1CB.f2=e2->r,_tmp1CB)));struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF3;enum Cyc_Absyn_Primop _tmpF5;struct Cyc_List_List*_tmpF6;enum Cyc_Absyn_Primop _tmpF8;struct Cyc_List_List*_tmpF9;void*_tmpFB;void*_tmpFD;void*_tmpFF;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp103;void*_tmp105;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp109;void*_tmp10B;void*_tmp10C;void*_tmp10E;void*_tmp10F;void*_tmp111;struct Cyc_Absyn_Exp*_tmp112;void*_tmp114;struct Cyc_Absyn_Exp*_tmp115;void*_tmp117;void*_tmp119;_LLD4:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpE0=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmpE0->tag != 5)goto _LLD6;else{_tmpE1=_tmpE0->f1;_tmpE2=_tmpE0->f2;_tmpE3=_tmpE0->f3;}}{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpE4=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmpE4->tag != 5)goto _LLD6;else{_tmpE5=_tmpE4->f1;_tmpE6=_tmpE4->f2;_tmpE7=_tmpE4->f3;}};_LLD5: {
# 336
int _tmp11A=Cyc_Evexp_const_exp_cmp(_tmpE3,_tmpE7);
if(_tmp11A != 0)return _tmp11A;
_tmpE9=_tmpE1;_tmpEA=_tmpE2;_tmpEC=_tmpE5;_tmpED=_tmpE6;goto _LLD7;}_LLD6:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmpE8->tag != 6)goto _LLD8;else{_tmpE9=_tmpE8->f1;_tmpEA=_tmpE8->f2;}}{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmpEB->tag != 6)goto _LLD8;else{_tmpEC=_tmpEB->f1;_tmpED=_tmpEB->f2;}};_LLD7:
 _tmpEF=_tmpE9;_tmpF0=_tmpEA;_tmpF2=_tmpEC;_tmpF3=_tmpED;goto _LLD9;_LLD8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpEE=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmpEE->tag != 7)goto _LLDA;else{_tmpEF=_tmpEE->f1;_tmpF0=_tmpEE->f2;}}{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpF1=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmpF1->tag != 7)goto _LLDA;else{_tmpF2=_tmpF1->f1;_tmpF3=_tmpF1->f2;}};_LLD9: {
# 341
int _tmp11B=Cyc_Evexp_const_exp_cmp(_tmpEF,_tmpF2);
if(_tmp11B != 0)return _tmp11B;
return Cyc_Evexp_const_exp_cmp(_tmpF0,_tmpF3);}_LLDA:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpF4=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmpF4->tag != 2)goto _LLDC;else{_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;}}{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpF7=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmpF7->tag != 2)goto _LLDC;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}};_LLDB: {
# 345
int _tmp11C=(int)_tmpF5 - (int)_tmpF8;
if(_tmp11C != 0)return _tmp11C;
for(0;_tmpF6 != 0  && _tmpF9 != 0;(_tmpF6=_tmpF6->tl,_tmpF9=_tmpF9->tl)){
int _tmp11D=Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)_tmpF6->hd,(struct Cyc_Absyn_Exp*)_tmpF9->hd);
if(_tmp11D != 0)
return _tmp11D;}
# 352
return 0;}_LLDC:{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpFA=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmpFA->tag != 16)goto _LLDE;else{_tmpFB=(void*)_tmpFA->f1;}}{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpFC=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmpFC->tag != 16)goto _LLDE;else{_tmpFD=(void*)_tmpFC->f1;}};_LLDD:
 return Cyc_Tcutil_typecmp(_tmpFB,_tmpFD);_LLDE:{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpFE=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmpFE->tag != 16)goto _LLE0;else{_tmpFF=(void*)_tmpFE->f1;}}{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp100=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmp100->tag != 17)goto _LLE0;else{_tmp101=_tmp100->f1;}};_LLDF: {
# 355
void*_tmp11E=_tmp101->topt;
if(_tmp11E == 0){
const char*_tmp1CE;void*_tmp1CD;(_tmp1CD=0,Cyc_Tcutil_terr(e2->loc,((_tmp1CE="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1CE,sizeof(char),51))),_tag_dyneither(_tmp1CD,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp(_tmpFF,(void*)_check_null(_tmp11E));}_LLE0:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp102=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmp102->tag != 17)goto _LLE2;else{_tmp103=_tmp102->f1;}}{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp104=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmp104->tag != 16)goto _LLE2;else{_tmp105=(void*)_tmp104->f1;}};_LLE1: {
# 360
void*_tmp121=_tmp103->topt;
if(_tmp121 == 0){
const char*_tmp1D1;void*_tmp1D0;(_tmp1D0=0,Cyc_Tcutil_terr(e1->loc,((_tmp1D1="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1D1,sizeof(char),51))),_tag_dyneither(_tmp1D0,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp((void*)_check_null(_tmp121),_tmp105);}_LLE2:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmp106->tag != 17)goto _LLE4;else{_tmp107=_tmp106->f1;}}{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp108=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmp108->tag != 17)goto _LLE4;else{_tmp109=_tmp108->f1;}};_LLE3: {
# 365
void*_tmp124=_tmp107->topt;
void*_tmp125=_tmp109->topt;
if(_tmp124 == 0){
const char*_tmp1D4;void*_tmp1D3;(_tmp1D3=0,Cyc_Tcutil_terr(e1->loc,((_tmp1D4="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1D4,sizeof(char),51))),_tag_dyneither(_tmp1D3,sizeof(void*),0)));}
if(_tmp125 == 0){
const char*_tmp1D7;void*_tmp1D6;(_tmp1D6=0,Cyc_Tcutil_terr(e2->loc,((_tmp1D7="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1D7,sizeof(char),51))),_tag_dyneither(_tmp1D6,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp((void*)_check_null(_tmp124),(void*)_check_null(_tmp125));}_LLE4:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp10A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmp10A->tag != 18)goto _LLE6;else{_tmp10B=(void*)_tmp10A->f1;_tmp10C=(void*)_tmp10A->f2;}}{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp10D=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmp10D->tag != 18)goto _LLE6;else{_tmp10E=(void*)_tmp10D->f1;_tmp10F=(void*)_tmp10D->f2;}};_LLE5: {
# 373
int _tmp12A=Cyc_Tcutil_typecmp(_tmp10B,_tmp10E);
if(_tmp12A != 0)return _tmp12A;{
struct _tuple16 _tmp1D8;struct _tuple16 _tmp12C=(_tmp1D8.f1=_tmp10C,((_tmp1D8.f2=_tmp10F,_tmp1D8)));struct _dyneither_ptr*_tmp132;struct _dyneither_ptr*_tmp134;unsigned int _tmp136;unsigned int _tmp138;_LLED:{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp12D=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp12C.f1;if(_tmp12D->tag != 0)goto _LLEF;}{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp12E=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp12C.f2;if(_tmp12E->tag != 1)goto _LLEF;};_LLEE:
 return - 1;_LLEF:{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp12F=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp12C.f1;if(_tmp12F->tag != 1)goto _LLF1;}{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp130=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp12C.f2;if(_tmp130->tag != 0)goto _LLF1;};_LLF0:
 return 1;_LLF1:{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp131=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp12C.f1;if(_tmp131->tag != 0)goto _LLF3;else{_tmp132=_tmp131->f1;}}{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp133=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp12C.f2;if(_tmp133->tag != 0)goto _LLF3;else{_tmp134=_tmp133->f1;}};_LLF2:
 return Cyc_strptrcmp(_tmp132,_tmp134);_LLF3:{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp135=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp12C.f1;if(_tmp135->tag != 1)goto _LLEC;else{_tmp136=_tmp135->f1;}}{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp137=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp12C.f2;if(_tmp137->tag != 1)goto _LLEC;else{_tmp138=_tmp137->f1;}};_LLF4:
 return(int)(_tmp136 - _tmp138);_LLEC:;};}_LLE6:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp110=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmp110->tag != 13)goto _LLE8;else{_tmp111=(void*)_tmp110->f1;_tmp112=_tmp110->f2;}}{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmp113->tag != 13)goto _LLE8;else{_tmp114=(void*)_tmp113->f1;_tmp115=_tmp113->f2;}};_LLE7: {
# 382
int _tmp139=Cyc_Tcutil_typecmp(_tmp111,_tmp114);
if(_tmp139 != 0)return _tmp139;
return Cyc_Evexp_const_exp_cmp(_tmp112,_tmp115);}_LLE8:{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp116=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDF.f1;if(_tmp116->tag != 37)goto _LLEA;else{_tmp117=(void*)_tmp116->f1;}}{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp118=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDF.f2;if(_tmp118->tag != 37)goto _LLEA;else{_tmp119=(void*)_tmp118->f1;}};_LLE9:
# 386
 if(Cyc_Tcutil_unify(_tmp117,_tmp119))return 0;
return Cyc_Tcutil_typecmp(_tmp117,_tmp119);_LLEA:;_LLEB: {
const char*_tmp1DB;void*_tmp1DA;(_tmp1DA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1DB="Evexp::const_exp_cmp, unexpected case",_tag_dyneither(_tmp1DB,sizeof(char),38))),_tag_dyneither(_tmp1DA,sizeof(void*),0)));}_LLD3:;};};};}
# 391
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}
# 394
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
unsigned int _tmp13D;int _tmp13E;struct _tuple10 _tmp13C=Cyc_Evexp_eval_const_uint_exp(e1);_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;{
unsigned int _tmp140;int _tmp141;struct _tuple10 _tmp13F=Cyc_Evexp_eval_const_uint_exp(e2);_tmp140=_tmp13F.f1;_tmp141=_tmp13F.f2;
if(_tmp13E  && _tmp141)
return _tmp13D <= _tmp140;
return Cyc_Evexp_same_const_exp(e1,e2);};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 404
int Cyc_Evexp_okay_szofarg(void*t){
void*_tmp142=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp145;struct Cyc_Absyn_Datatypefield*_tmp14A;struct Cyc_List_List*_tmp14C;void*_tmp153;struct Cyc_Absyn_Exp*_tmp154;union Cyc_Absyn_AggrInfoU _tmp159;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp15C;struct Cyc_Absyn_Typedefdecl*_tmp168;_LLF6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp142;if(_tmp143->tag != 0)goto _LLF8;}_LLF7:
 return 0;_LLF8: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp144=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142;if(_tmp144->tag != 2)goto _LLFA;else{_tmp145=_tmp144->f1;}}_LLF9:
# 408
 switch((Cyc_Tcutil_tvar_kind(_tmp145,& Cyc_Tcutil_bk))->kind){case Cyc_Absyn_BoxKind: _LL130:
 return 1;default: _LL131:
 return 0;}_LLFA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp146=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp142;if(_tmp146->tag != 1)goto _LLFC;else{if(_tmp146->f1 == 0)goto _LLFC;if(((struct Cyc_Absyn_Kind*)(_tmp146->f1)->v)->kind != Cyc_Absyn_BoxKind)goto _LLFC;}}_LLFB:
# 412
 return 1;_LLFC: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp147=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp142;if(_tmp147->tag != 1)goto _LLFE;}_LLFD:
 return 0;_LLFE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp148=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp142;if(_tmp148->tag != 3)goto _LL100;}_LLFF:
 return 1;_LL100: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp149=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp142;if(_tmp149->tag != 4)goto _LL102;else{if((((_tmp149->f1).field_info).KnownDatatypefield).tag != 2)goto _LL102;_tmp14A=((struct _tuple1)(((_tmp149->f1).field_info).KnownDatatypefield).val).f2;}}_LL101:
# 416
 _tmp14C=_tmp14A->typs;goto _LL103;_LL102: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp14B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp142;if(_tmp14B->tag != 10)goto _LL104;else{_tmp14C=_tmp14B->f1;}}_LL103:
# 418
 for(0;_tmp14C != 0;_tmp14C=_tmp14C->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple17*)_tmp14C->hd)).f2))
return 0;}
return 1;_LL104: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp14D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp142;if(_tmp14D->tag != 4)goto _LL106;}_LL105:
# 423
 return 0;_LL106: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp14E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp142;if(_tmp14E->tag != 5)goto _LL108;}_LL107:
# 426
 return 1;_LL108: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp14F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp142;if(_tmp14F->tag != 19)goto _LL10A;}_LL109:
# 428
 goto _LL10B;_LL10A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp142;if(_tmp150->tag != 6)goto _LL10C;}_LL10B:
 goto _LL10D;_LL10C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp151=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp142;if(_tmp151->tag != 7)goto _LL10E;}_LL10D:
 return 1;_LL10E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp152=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp142;if(_tmp152->tag != 8)goto _LL110;else{_tmp153=(_tmp152->f1).elt_type;_tmp154=(_tmp152->f1).num_elts;}}_LL10F:
# 432
 return _tmp154 != 0;_LL110: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp155=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp142;if(_tmp155->tag != 9)goto _LL112;}_LL111:
 return 0;_LL112: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp156=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp142;if(_tmp156->tag != 15)goto _LL114;}_LL113:
 return 1;_LL114: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp157=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp142;if(_tmp157->tag != 16)goto _LL116;}_LL115:
 return 1;_LL116: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp142;if(_tmp158->tag != 11)goto _LL118;else{_tmp159=(_tmp158->f1).aggr_info;_tmp15A=(_tmp158->f1).targs;}}_LL117: {
# 437
struct Cyc_Absyn_Aggrdecl*ad;
{union Cyc_Absyn_AggrInfoU _tmp169=_tmp159;struct Cyc_Absyn_Aggrdecl*_tmp16A;_LL134: if((_tmp169.UnknownAggr).tag != 1)goto _LL136;_LL135: {
const char*_tmp1DE;void*_tmp1DD;(_tmp1DD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1DE="szof on unchecked StructType",_tag_dyneither(_tmp1DE,sizeof(char),29))),_tag_dyneither(_tmp1DD,sizeof(void*),0)));}_LL136: if((_tmp169.KnownAggr).tag != 2)goto _LL133;_tmp16A=*((struct Cyc_Absyn_Aggrdecl**)(_tmp169.KnownAggr).val);_LL137:
 ad=_tmp16A;_LL133:;}
# 442
if(ad->impl == 0)
return 0;
{
struct _RegionHandle _tmp16D=_new_region("temp");struct _RegionHandle*temp=& _tmp16D;_push_region(temp);
{struct Cyc_List_List*_tmp16E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,_tmp15A);
{struct Cyc_List_List*_tmp16F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp16F != 0;_tmp16F=_tmp16F->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,_tmp16E,((struct Cyc_Absyn_Aggrfield*)_tmp16F->hd)->type))){int _tmp170=0;_npop_handler(0);return _tmp170;}}}{
int _tmp171=1;_npop_handler(0);return _tmp171;};}
# 446
;_pop_region(temp);}
# 451
_tmp15C=(ad->impl)->fields;goto _LL119;}_LL118: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp142;if(_tmp15B->tag != 12)goto _LL11A;else{_tmp15C=_tmp15B->f2;}}_LL119:
# 453
 for(0;_tmp15C != 0;_tmp15C=_tmp15C->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)_tmp15C->hd)->type))
return 0;}
return 1;_LL11A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp142;if(_tmp15D->tag != 13)goto _LL11C;}_LL11B:
 goto _LL11D;_LL11C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp15E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp142;if(_tmp15E->tag != 14)goto _LL11E;}_LL11D:
 return 1;_LL11E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp142;if(_tmp15F->tag != 26)goto _LL120;}_LL11F:
 goto _LL121;_LL120: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp142;if(_tmp160->tag != 20)goto _LL122;}_LL121:
 goto _LL123;_LL122: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp142;if(_tmp161->tag != 22)goto _LL124;}_LL123:
 goto _LL125;_LL124: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp142;if(_tmp162->tag != 21)goto _LL126;}_LL125:
 goto _LL127;_LL126: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp142;if(_tmp163->tag != 24)goto _LL128;}_LL127:
 goto _LL129;_LL128: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp164=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp142;if(_tmp164->tag != 23)goto _LL12A;}_LL129:
 goto _LL12B;_LL12A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp142;if(_tmp165->tag != 18)goto _LL12C;}_LL12B:
 goto _LL12D;_LL12C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp142;if(_tmp166->tag != 25)goto _LL12E;}_LL12D:
 return 0;_LL12E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp167=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp142;if(_tmp167->tag != 17)goto _LLF5;else{_tmp168=_tmp167->f3;}}_LL12F:
# 469
 if(_tmp168 == 0  || _tmp168->kind == 0){
struct Cyc_String_pa_PrintArg_struct _tmp1E6;void*_tmp1E5[1];const char*_tmp1E4;void*_tmp1E3;(_tmp1E3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp1E6.tag=0,((_tmp1E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1E5[0]=& _tmp1E6,Cyc_aprintf(((_tmp1E4="szof typedeftype %s",_tag_dyneither(_tmp1E4,sizeof(char),20))),_tag_dyneither(_tmp1E5,sizeof(void*),1)))))))),_tag_dyneither(_tmp1E3,sizeof(void*),0)));}
return((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp168->kind))->v)->kind == Cyc_Absyn_BoxKind;_LLF5:;}
