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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 290 "cycboot.h"
int isdigit(int);
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 134 "string.h"
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*c);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
# 913 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 943
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;
# 945
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;
# 947
void*Cyc_Absyn_float_typ(int);
# 979
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 984
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 137 "tcenv.h"
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 40 "tcutil.h"
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 341 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 34 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 37
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);struct _tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;char f5;int f6;};
# 44 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _RegionHandle*r,struct _dyneither_ptr s,int i){
# 48
unsigned long _tmp0=Cyc_strlen((struct _dyneither_ptr)s);
if(i < 0  || i >= _tmp0)return 0;{
# 52
struct Cyc_List_List*_tmp1=0;
char c=' ';
for(0;i < _tmp0;++ i){
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
switch(c){case '+': _LL0:
 goto _LL1;case '-': _LL1:
 goto _LL2;case ' ': _LL2:
 goto _LL3;case '#': _LL3:
 goto _LL4;case '0': _LL4:
{struct Cyc_List_List*_tmpF8;_tmp1=((_tmpF8=_region_malloc(r,sizeof(*_tmpF8)),((_tmpF8->hd=(void*)((int)c),((_tmpF8->tl=_tmp1,_tmpF8))))));}continue;default: _LL5:
 break;}
# 64
break;}
# 66
if(i >= _tmp0)return 0;
_tmp1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1);{
# 70
struct Cyc_List_List*_tmp3=0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == '*'){
{struct Cyc_List_List*_tmpF9;_tmp3=((_tmpF9=_region_malloc(r,sizeof(*_tmpF9)),((_tmpF9->hd=(void*)((int)c),((_tmpF9->tl=_tmp3,_tmpF9))))));}
++ i;}else{
# 76
for(0;i < _tmp0;++ i){
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(isdigit((int)c)){struct Cyc_List_List*_tmpFA;_tmp3=((_tmpFA=_region_malloc(r,sizeof(*_tmpFA)),((_tmpFA->hd=(void*)((int)c),((_tmpFA->tl=_tmp3,_tmpFA))))));}else{
break;}}}
# 82
if(i >= _tmp0)return 0;
_tmp3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3);{
# 86
struct Cyc_List_List*_tmp6=0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == '.'){
{struct Cyc_List_List*_tmpFB;_tmp6=((_tmpFB=_region_malloc(r,sizeof(*_tmpFB)),((_tmpFB->hd=(void*)((int)c),((_tmpFB->tl=_tmp6,_tmpFB))))));}
++ i;
if(i >= _tmp0)return 0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == '*'){
{struct Cyc_List_List*_tmpFC;_tmp6=((_tmpFC=_region_malloc(r,sizeof(*_tmpFC)),((_tmpFC->hd=(void*)((int)c),((_tmpFC->tl=_tmp6,_tmpFC))))));}
++ i;}else{
# 97
for(0;i < _tmp0;++ i){
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(isdigit((int)c)){struct Cyc_List_List*_tmpFD;_tmp6=((_tmpFD=_region_malloc(r,sizeof(*_tmpFD)),((_tmpFD->hd=(void*)((int)c),((_tmpFD->tl=_tmp6,_tmpFD))))));}else{
break;}}}}
# 103
if(i >= _tmp0)return 0;
_tmp6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6);{
# 108
struct Cyc_List_List*_tmpA=0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
switch(c){case 'h': _LL7:
# 112
{struct Cyc_List_List*_tmpFE;_tmpA=((_tmpFE=_region_malloc(r,sizeof(*_tmpFE)),((_tmpFE->hd=(void*)((int)c),((_tmpFE->tl=_tmpA,_tmpFE))))));}
++ i;
if(i >= _tmp0)return 0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == 'h'){{struct Cyc_List_List*_tmpFF;_tmpA=((_tmpFF=_region_malloc(r,sizeof(*_tmpFF)),((_tmpFF->hd=(void*)((int)c),((_tmpFF->tl=_tmpA,_tmpFF))))));}++ i;}
break;case 'l': _LL8:
# 119
{struct Cyc_List_List*_tmp100;_tmpA=((_tmp100=_region_malloc(r,sizeof(*_tmp100)),((_tmp100->hd=(void*)((int)c),((_tmp100->tl=_tmpA,_tmp100))))));}
++ i;
if(i >= _tmp0)return 0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == 'l'){{struct Cyc_List_List*_tmp101;_tmpA=((_tmp101=_region_malloc(r,sizeof(*_tmp101)),((_tmp101->hd=(void*)((int)c),((_tmp101->tl=_tmpA,_tmp101))))));}++ i;}
break;case 'j': _LL9:
 goto _LLA;case 'z': _LLA:
 goto _LLB;case 't': _LLB:
 goto _LLC;case 'L': _LLC:
# 129
{struct Cyc_List_List*_tmp102;_tmpA=((_tmp102=_region_malloc(r,sizeof(*_tmp102)),((_tmp102->hd=(void*)((int)c),((_tmp102->tl=_tmpA,_tmp102))))));}
++ i;
break;default: _LLD:
 break;}
# 134
if(i >= _tmp0)return 0;
_tmpA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA);
# 138
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
switch(c){case 'd': _LLF:
 goto _LL10;case 'i': _LL10:
 goto _LL11;case 'o': _LL11:
 goto _LL12;case 'u': _LL12:
 goto _LL13;case 'x': _LL13:
 goto _LL14;case 'X': _LL14:
 goto _LL15;case 'f': _LL15:
 goto _LL16;case 'F': _LL16:
 goto _LL17;case 'e': _LL17:
 goto _LL18;case 'E': _LL18:
 goto _LL19;case 'g': _LL19:
 goto _LL1A;case 'G': _LL1A:
 goto _LL1B;case 'a': _LL1B:
 goto _LL1C;case 'A': _LL1C:
 goto _LL1D;case 'c': _LL1D:
 goto _LL1E;case 's': _LL1E:
 goto _LL1F;case 'p': _LL1F:
 goto _LL20;case 'n': _LL20:
 goto _LL21;case '%': _LL21:
 break;default: _LL22:
 return 0;}{
# 170 "formatstr.cyc"
struct _tuple10*_tmp105;struct Cyc_Core_Opt*_tmp104;return(_tmp104=_region_malloc(r,sizeof(*_tmp104)),((_tmp104->v=((_tmp105=_region_malloc(r,sizeof(*_tmp105)),((_tmp105->f1=_tmp1,((_tmp105->f2=_tmp3,((_tmp105->f3=_tmp6,((_tmp105->f4=_tmpA,((_tmp105->f5=c,((_tmp105->f6=i + 1,_tmp105)))))))))))))),_tmp104)));};};};};};}
# 173
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,unsigned int loc){
# 176
unsigned long _tmp12=Cyc_strlen((struct _dyneither_ptr)s);
struct Cyc_List_List*_tmp13=0;
int i;
struct _RegionHandle _tmp14=_new_region("temp");struct _RegionHandle*temp=& _tmp14;_push_region(temp);
for(i=0;i < _tmp12;++ i){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),i))!= '%')continue;{
struct Cyc_Core_Opt*_tmp15=Cyc_Formatstr_parse_conversionspecification(temp,s,i + 1);
if(_tmp15 == 0){
{const char*_tmp108;void*_tmp107;(_tmp107=0,Cyc_Tcutil_terr(loc,((_tmp108="bad format string",_tag_dyneither(_tmp108,sizeof(char),18))),_tag_dyneither(_tmp107,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp18=0;_npop_handler(0);return _tmp18;};}{
# 187
struct Cyc_List_List*_tmp1A;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1C;struct Cyc_List_List*_tmp1D;char _tmp1E;int _tmp1F;struct _tuple10*_tmp19=(struct _tuple10*)_tmp15->v;_tmp1A=_tmp19->f1;_tmp1B=_tmp19->f2;_tmp1C=_tmp19->f3;_tmp1D=_tmp19->f4;_tmp1E=_tmp19->f5;_tmp1F=_tmp19->f6;
i=_tmp1F - 1;
{struct Cyc_List_List*_tmp20=_tmp1D;int _tmp21;_LL25: if(_tmp20 == 0)goto _LL27;_tmp21=(int)_tmp20->hd;if(_tmp20->tl != 0)goto _LL27;if(!(
(_tmp21 == 'j'  || _tmp21 == 'z') || _tmp21 == 't'))goto _LL27;_LL26:
# 193
{const char*_tmp10C;void*_tmp10B[1];struct Cyc_Int_pa_PrintArg_struct _tmp10A;(_tmp10A.tag=1,((_tmp10A.f1=(unsigned long)_tmp21,((_tmp10B[0]=& _tmp10A,Cyc_Tcutil_terr(loc,((_tmp10C="length modifier '%c' is not supported",_tag_dyneither(_tmp10C,sizeof(char),38))),_tag_dyneither(_tmp10B,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp25=0;_npop_handler(0);return _tmp25;};_LL27:;_LL28:
 goto _LL24;_LL24:;}
# 197
{struct Cyc_List_List*_tmp26=_tmp1B;int _tmp27;_LL2A: if(_tmp26 == 0)goto _LL2C;_tmp27=(int)_tmp26->hd;if(_tmp26->tl != 0)goto _LL2C;if(!(_tmp27 == '*'))goto _LL2C;_LL2B:
{struct Cyc_List_List*_tmp10D;_tmp13=((_tmp10D=_region_malloc(r,sizeof(*_tmp10D)),((_tmp10D->hd=Cyc_Absyn_sint_typ,((_tmp10D->tl=_tmp13,_tmp10D))))));}goto _LL29;_LL2C:;_LL2D:
 goto _LL29;_LL29:;}
# 201
{struct Cyc_List_List*_tmp29=_tmp1C;int _tmp2A;int _tmp2B;_LL2F: if(_tmp29 == 0)goto _LL31;_tmp2A=(int)_tmp29->hd;if(_tmp29->tl == 0)goto _LL31;_tmp2B=(int)(_tmp29->tl)->hd;if((_tmp29->tl)->tl != 0)goto _LL31;if(!(
_tmp2A == '.'  && _tmp2B == '*'))goto _LL31;_LL30:
{struct Cyc_List_List*_tmp10E;_tmp13=((_tmp10E=_region_malloc(r,sizeof(*_tmp10E)),((_tmp10E->hd=Cyc_Absyn_sint_typ,((_tmp10E->tl=_tmp13,_tmp10E))))));}goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}{
# 206
void*t;
switch(_tmp1E){case 'd': _LL33:
 goto _LL34;case 'i': _LL34:
# 210
{struct Cyc_List_List*f=_tmp1A;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'){
{const char*_tmp112;void*_tmp111[1];struct Cyc_Int_pa_PrintArg_struct _tmp110;(_tmp110.tag=1,((_tmp110.f1=(unsigned long)((int)_tmp1E),((_tmp111[0]=& _tmp110,Cyc_Tcutil_terr(loc,((_tmp112="flag '#' is not valid with %%%c",_tag_dyneither(_tmp112,sizeof(char),32))),_tag_dyneither(_tmp111,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp30=0;_npop_handler(0);return _tmp30;};}}}
# 215
{struct Cyc_List_List*_tmp31=_tmp1D;int _tmp32;int _tmp33;int _tmp34;int _tmp35;_LL37: if(_tmp31 != 0)goto _LL39;_LL38:
 t=Cyc_Absyn_sint_typ;goto _LL36;_LL39: if(_tmp31 == 0)goto _LL3B;_tmp32=(int)_tmp31->hd;if(_tmp31->tl != 0)goto _LL3B;if(!(_tmp32 == 'l'))goto _LL3B;_LL3A:
 t=Cyc_Absyn_slong_typ;goto _LL36;_LL3B: if(_tmp31 == 0)goto _LL3D;_tmp33=(int)_tmp31->hd;if(_tmp31->tl != 0)goto _LL3D;if(!(_tmp33 == 'h'))goto _LL3D;_LL3C:
 t=Cyc_Absyn_sshort_typ;goto _LL36;_LL3D: if(_tmp31 == 0)goto _LL3F;_tmp34=(int)_tmp31->hd;if(_tmp31->tl == 0)goto _LL3F;_tmp35=(int)(_tmp31->tl)->hd;if((_tmp31->tl)->tl != 0)goto _LL3F;if(!(
_tmp34 == 'h'  && _tmp35 == 'h'))goto _LL3F;_LL3E:
 t=Cyc_Absyn_schar_typ;goto _LL36;_LL3F:;_LL40:
# 222
{const char*_tmp117;void*_tmp116[2];struct Cyc_String_pa_PrintArg_struct _tmp115;struct Cyc_Int_pa_PrintArg_struct _tmp114;(_tmp114.tag=1,((_tmp114.f1=(unsigned long)((int)_tmp1E),((_tmp115.tag=0,((_tmp115.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp1D)),((_tmp116[0]=& _tmp115,((_tmp116[1]=& _tmp114,Cyc_Tcutil_terr(loc,((_tmp117="length modifier '%s' is not allowed with %%%c",_tag_dyneither(_tmp117,sizeof(char),46))),_tag_dyneither(_tmp116,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmp3A=0;_npop_handler(0);return _tmp3A;};_LL36:;}
# 226
{struct Cyc_List_List*_tmp118;_tmp13=((_tmp118=_region_malloc(r,sizeof(*_tmp118)),((_tmp118->hd=t,((_tmp118->tl=_tmp13,_tmp118))))));}
break;case 'u': _LL35:
# 229
{struct Cyc_List_List*f=_tmp1A;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'){
{const char*_tmp11B;void*_tmp11A;(_tmp11A=0,Cyc_Tcutil_terr(loc,((_tmp11B="Flag '#' not valid with %%u",_tag_dyneither(_tmp11B,sizeof(char),28))),_tag_dyneither(_tmp11A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp3E=0;_npop_handler(0);return _tmp3E;};}}}
# 234
goto _LL41;case 'o': _LL41:
 goto _LL42;case 'x': _LL42:
 goto _LL43;case 'X': _LL43:
# 238
{struct Cyc_List_List*_tmp3F=_tmp1D;int _tmp40;int _tmp41;int _tmp42;int _tmp43;_LL46: if(_tmp3F != 0)goto _LL48;_LL47:
 t=Cyc_Absyn_uint_typ;goto _LL45;_LL48: if(_tmp3F == 0)goto _LL4A;_tmp40=(int)_tmp3F->hd;if(_tmp3F->tl != 0)goto _LL4A;if(!(_tmp40 == 'l'))goto _LL4A;_LL49:
 t=Cyc_Absyn_ulong_typ;goto _LL45;_LL4A: if(_tmp3F == 0)goto _LL4C;_tmp41=(int)_tmp3F->hd;if(_tmp3F->tl != 0)goto _LL4C;if(!(_tmp41 == 'h'))goto _LL4C;_LL4B:
 t=Cyc_Absyn_ushort_typ;goto _LL45;_LL4C: if(_tmp3F == 0)goto _LL4E;_tmp42=(int)_tmp3F->hd;if(_tmp3F->tl == 0)goto _LL4E;_tmp43=(int)(_tmp3F->tl)->hd;if((_tmp3F->tl)->tl != 0)goto _LL4E;if(!(
_tmp42 == 'h'  && _tmp43 == 'h'))goto _LL4E;_LL4D:
 t=Cyc_Absyn_uchar_typ;goto _LL45;_LL4E:;_LL4F:
# 246
{const char*_tmp120;void*_tmp11F[2];struct Cyc_String_pa_PrintArg_struct _tmp11E;struct Cyc_Int_pa_PrintArg_struct _tmp11D;(_tmp11D.tag=1,((_tmp11D.f1=(unsigned long)((int)_tmp1E),((_tmp11E.tag=0,((_tmp11E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp1D)),((_tmp11F[0]=& _tmp11E,((_tmp11F[1]=& _tmp11D,Cyc_Tcutil_terr(loc,((_tmp120="length modifier '%s' is not allowed with %%%c",_tag_dyneither(_tmp120,sizeof(char),46))),_tag_dyneither(_tmp11F,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmp48=0;_npop_handler(0);return _tmp48;};_LL45:;}
# 250
{struct Cyc_List_List*_tmp121;_tmp13=((_tmp121=_region_malloc(r,sizeof(*_tmp121)),((_tmp121->hd=t,((_tmp121->tl=_tmp13,_tmp121))))));}
break;case 'f': _LL44:
 goto _LL50;case 'F': _LL50:
 goto _LL51;case 'e': _LL51:
 goto _LL52;case 'E': _LL52:
 goto _LL53;case 'g': _LL53:
 goto _LL54;case 'G': _LL54:
 goto _LL55;case 'a': _LL55:
 goto _LL56;case 'A': _LL56:
# 266
{struct Cyc_List_List*_tmp4A=_tmp1D;int _tmp4B;_LL59: if(_tmp4A != 0)goto _LL5B;_LL5A:
{struct Cyc_List_List*_tmp122;_tmp13=((_tmp122=_region_malloc(r,sizeof(*_tmp122)),((_tmp122->hd=Cyc_Absyn_float_typ(1),((_tmp122->tl=_tmp13,_tmp122))))));}goto _LL58;_LL5B: if(_tmp4A == 0)goto _LL5D;_tmp4B=(int)_tmp4A->hd;if(_tmp4A->tl != 0)goto _LL5D;if(!(_tmp4B == 'l'))goto _LL5D;_LL5C:
# 269
{struct Cyc_List_List*_tmp123;_tmp13=((_tmp123=_region_malloc(r,sizeof(*_tmp123)),((_tmp123->hd=Cyc_Absyn_float_typ(2),((_tmp123->tl=_tmp13,_tmp123))))));}goto _LL58;_LL5D:;_LL5E:
# 271
{const char*_tmp128;void*_tmp127[2];struct Cyc_String_pa_PrintArg_struct _tmp126;struct Cyc_Int_pa_PrintArg_struct _tmp125;(_tmp125.tag=1,((_tmp125.f1=(unsigned long)((int)_tmp1E),((_tmp126.tag=0,((_tmp126.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp1D)),((_tmp127[0]=& _tmp126,((_tmp127[1]=& _tmp125,Cyc_Tcutil_terr(loc,((_tmp128="length modifier '%s' is not allowed with %%%c",_tag_dyneither(_tmp128,sizeof(char),46))),_tag_dyneither(_tmp127,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmp52=0;_npop_handler(0);return _tmp52;};_LL58:;}
# 275
break;case 'c': _LL57:
# 277
{struct Cyc_List_List*f=_tmp1A;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'  || (int)f->hd == '0'){
{const char*_tmp12C;void*_tmp12B[1];struct Cyc_Int_pa_PrintArg_struct _tmp12A;(_tmp12A.tag=1,((_tmp12A.f1=(unsigned long)((int)f->hd),((_tmp12B[0]=& _tmp12A,Cyc_Tcutil_terr(loc,((_tmp12C="flag '%c' not allowed with %%c",_tag_dyneither(_tmp12C,sizeof(char),31))),_tag_dyneither(_tmp12B,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp56=0;_npop_handler(0);return _tmp56;};}}}
# 284
if(_tmp1D != 0){
{const char*_tmp130;void*_tmp12F[1];struct Cyc_String_pa_PrintArg_struct _tmp12E;(_tmp12E.tag=0,((_tmp12E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp1D)),((_tmp12F[0]=& _tmp12E,Cyc_Tcutil_terr(loc,((_tmp130="length modifier '%s' not allowed with %%c",_tag_dyneither(_tmp130,sizeof(char),42))),_tag_dyneither(_tmp12F,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp5A=0;_npop_handler(0);return _tmp5A;};}
# 289
if(_tmp1C != 0){
{const char*_tmp134;void*_tmp133[1];struct Cyc_String_pa_PrintArg_struct _tmp132;(_tmp132.tag=0,((_tmp132.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp1C)),((_tmp133[0]=& _tmp132,Cyc_Tcutil_terr(loc,((_tmp134="precision '%s' not allowed with %%c",_tag_dyneither(_tmp134,sizeof(char),36))),_tag_dyneither(_tmp133,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp5E=0;_npop_handler(0);return _tmp5E;};}
# 294
{struct Cyc_List_List*_tmp135;_tmp13=((_tmp135=_region_malloc(r,sizeof(*_tmp135)),((_tmp135->hd=Cyc_Absyn_sint_typ,((_tmp135->tl=_tmp13,_tmp135))))));}
break;case 's': _LL5F:
# 298
{struct Cyc_List_List*f=_tmp1A;for(0;f != 0;f=f->tl){
if((int)f->hd != '-'){
{const char*_tmp138;void*_tmp137;(_tmp137=0,Cyc_Tcutil_terr(loc,((_tmp138="a flag not allowed with %%s",_tag_dyneither(_tmp138,sizeof(char),28))),_tag_dyneither(_tmp137,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp62=0;_npop_handler(0);return _tmp62;};}}}
# 305
if(_tmp1D != 0){
{const char*_tmp13B;void*_tmp13A;(_tmp13A=0,Cyc_Tcutil_terr(loc,((_tmp13B="length modifiers not allowed with %%s",_tag_dyneither(_tmp13B,sizeof(char),38))),_tag_dyneither(_tmp13A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp65=0;_npop_handler(0);return _tmp65;};}{
# 311
struct Cyc_Core_Opt*_tmp13C;void*ptr=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,
Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,(
(_tmp13C=_cycalloc(sizeof(*_tmp13C)),((_tmp13C->v=Cyc_Tcenv_lookup_type_vars(te),_tmp13C))))),
Cyc_Absyn_const_tqual(0),Cyc_Absyn_false_conref);
{struct Cyc_List_List*_tmp13D;_tmp13=((_tmp13D=_region_malloc(r,sizeof(*_tmp13D)),((_tmp13D->hd=ptr,((_tmp13D->tl=_tmp13,_tmp13D))))));}
break;};case 'p': _LL60:
# 319
{struct Cyc_List_List*_tmp13E;_tmp13=((_tmp13E=_region_malloc(r,sizeof(*_tmp13E)),((_tmp13E->hd=Cyc_Absyn_uint_typ,((_tmp13E->tl=_tmp13,_tmp13E))))));}
break;case 'n': _LL61:
# 322
{struct Cyc_List_List*f=_tmp1A;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'  || (int)f->hd == '0'){
{const char*_tmp142;void*_tmp141[1];struct Cyc_Int_pa_PrintArg_struct _tmp140;(_tmp140.tag=1,((_tmp140.f1=(unsigned long)((int)f->hd),((_tmp141[0]=& _tmp140,Cyc_Tcutil_terr(loc,((_tmp142="flag '%c' not allowed with %%n",_tag_dyneither(_tmp142,sizeof(char),31))),_tag_dyneither(_tmp141,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp6C=0;_npop_handler(0);return _tmp6C;};}}}
# 327
if(_tmp1C != 0){
{const char*_tmp146;void*_tmp145[1];struct Cyc_String_pa_PrintArg_struct _tmp144;(_tmp144.tag=0,((_tmp144.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp1C)),((_tmp145[0]=& _tmp144,Cyc_Tcutil_terr(loc,((_tmp146="precision '%s' not allowed with %%n",_tag_dyneither(_tmp146,sizeof(char),36))),_tag_dyneither(_tmp145,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp70=0;_npop_handler(0);return _tmp70;};}{
# 332
struct Cyc_Core_Opt*_tmp147;struct Cyc_Core_Opt*_tmp71=(_tmp147=_cycalloc(sizeof(*_tmp147)),((_tmp147->v=Cyc_Tcenv_lookup_type_vars(te),_tmp147)));
{struct Cyc_List_List*_tmp72=_tmp1D;int _tmp73;int _tmp74;int _tmp75;int _tmp76;_LL64: if(_tmp72 != 0)goto _LL66;_LL65:
 t=Cyc_Absyn_sint_typ;goto _LL63;_LL66: if(_tmp72 == 0)goto _LL68;_tmp73=(int)_tmp72->hd;if(_tmp72->tl != 0)goto _LL68;if(!(_tmp73 == 'l'))goto _LL68;_LL67:
# 336
 t=Cyc_Absyn_ulong_typ;goto _LL63;_LL68: if(_tmp72 == 0)goto _LL6A;_tmp74=(int)_tmp72->hd;if(_tmp72->tl != 0)goto _LL6A;if(!(_tmp74 == 'h'))goto _LL6A;_LL69:
 t=Cyc_Absyn_sshort_typ;goto _LL63;_LL6A: if(_tmp72 == 0)goto _LL6C;_tmp75=(int)_tmp72->hd;if(_tmp72->tl == 0)goto _LL6C;_tmp76=(int)(_tmp72->tl)->hd;if((_tmp72->tl)->tl != 0)goto _LL6C;if(!(
_tmp75 == 'h'  && _tmp76 == 'h'))goto _LL6C;_LL6B:
 t=Cyc_Absyn_schar_typ;goto _LL63;_LL6C:;_LL6D:
# 341
{const char*_tmp14C;void*_tmp14B[2];struct Cyc_String_pa_PrintArg_struct _tmp14A;struct Cyc_Int_pa_PrintArg_struct _tmp149;(_tmp149.tag=1,((_tmp149.f1=(unsigned long)((int)_tmp1E),((_tmp14A.tag=0,((_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp1D)),((_tmp14B[0]=& _tmp14A,((_tmp14B[1]=& _tmp149,Cyc_Tcutil_terr(loc,((_tmp14C="length modifier '%s' is not allowed with %%%c",_tag_dyneither(_tmp14C,sizeof(char),46))),_tag_dyneither(_tmp14B,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmp7B=0;_npop_handler(0);return _tmp7B;};_LL63:;}
# 345
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmp71),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
{struct Cyc_List_List*_tmp14D;_tmp13=((_tmp14D=_region_malloc(r,sizeof(*_tmp14D)),((_tmp14D->hd=t,((_tmp14D->tl=_tmp13,_tmp14D))))));}
break;};case '%': _LL62:
# 349
 if(_tmp1A != 0){
{const char*_tmp151;void*_tmp150[1];struct Cyc_String_pa_PrintArg_struct _tmp14F;(_tmp14F.tag=0,((_tmp14F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1A)),((_tmp150[0]=& _tmp14F,Cyc_Tcutil_terr(loc,((_tmp151="flags '%s' not allowed with %%%%",_tag_dyneither(_tmp151,sizeof(char),33))),_tag_dyneither(_tmp150,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp81=0;_npop_handler(0);return _tmp81;};}
# 353
if(_tmp1B != 0){
{const char*_tmp155;void*_tmp154[1];struct Cyc_String_pa_PrintArg_struct _tmp153;(_tmp153.tag=0,((_tmp153.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1B)),((_tmp154[0]=& _tmp153,Cyc_Tcutil_terr(loc,((_tmp155="width '%s' not allowed with %%%%",_tag_dyneither(_tmp155,sizeof(char),33))),_tag_dyneither(_tmp154,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp85=0;_npop_handler(0);return _tmp85;};}
# 357
if(_tmp1C != 0){
{const char*_tmp159;void*_tmp158[1];struct Cyc_String_pa_PrintArg_struct _tmp157;(_tmp157.tag=0,((_tmp157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1C)),((_tmp158[0]=& _tmp157,Cyc_Tcutil_terr(loc,((_tmp159="precision '%s' not allowed with %%%%",_tag_dyneither(_tmp159,sizeof(char),37))),_tag_dyneither(_tmp158,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp89=0;_npop_handler(0);return _tmp89;};}
# 361
if(_tmp1D != 0){
{const char*_tmp15D;void*_tmp15C[1];struct Cyc_String_pa_PrintArg_struct _tmp15B;(_tmp15B.tag=0,((_tmp15B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1D)),((_tmp15C[0]=& _tmp15B,Cyc_Tcutil_terr(loc,((_tmp15D="length modifier '%s' not allowed with %%%%",_tag_dyneither(_tmp15D,sizeof(char),43))),_tag_dyneither(_tmp15C,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp8D=0;_npop_handler(0);return _tmp8D;};}
# 365
break;default: _LL6E: {
# 367
struct Cyc_List_List*_tmp8E=0;_npop_handler(0);return _tmp8E;}}};};};}{
# 370
struct Cyc_List_List*_tmp8F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13);_npop_handler(0);return _tmp8F;};
# 180
;_pop_region(temp);}struct _tuple11{int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;char f4;int f5;};
# 381 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(struct _RegionHandle*r,struct _dyneither_ptr s,int i){
# 383
unsigned long _tmp90=Cyc_strlen((struct _dyneither_ptr)s);
if(i < 0  || i >= _tmp90)return 0;{
# 386
int _tmp91=0;
char _tmp92=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp92 == '*'){
_tmp91=1;
++ i;
if(i >= _tmp90)return 0;}{
# 394
struct Cyc_List_List*_tmp93=0;
for(0;i < _tmp90;++ i){
_tmp92=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(isdigit((int)_tmp92)){struct Cyc_List_List*_tmp15E;_tmp93=((_tmp15E=_region_malloc(r,sizeof(*_tmp15E)),((_tmp15E->hd=(void*)((int)_tmp92),((_tmp15E->tl=_tmp93,_tmp15E))))));}else{
break;}}
# 400
if(i >= _tmp90)return 0;
_tmp93=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp93);{
# 405
struct Cyc_List_List*_tmp95=0;
_tmp92=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
switch(_tmp92){case 'h': _LL70:
# 409
{struct Cyc_List_List*_tmp15F;_tmp95=((_tmp15F=_region_malloc(r,sizeof(*_tmp15F)),((_tmp15F->hd=(void*)((int)_tmp92),((_tmp15F->tl=_tmp95,_tmp15F))))));}
++ i;
if(i >= _tmp90)return 0;
_tmp92=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp92 == 'h'){{struct Cyc_List_List*_tmp160;_tmp95=((_tmp160=_region_malloc(r,sizeof(*_tmp160)),((_tmp160->hd=(void*)((int)_tmp92),((_tmp160->tl=_tmp95,_tmp160))))));}++ i;}
break;case 'l': _LL71:
# 416
{struct Cyc_List_List*_tmp161;_tmp95=((_tmp161=_region_malloc(r,sizeof(*_tmp161)),((_tmp161->hd=(void*)((int)_tmp92),((_tmp161->tl=_tmp95,_tmp161))))));}
++ i;
if(i >= _tmp90)return 0;
_tmp92=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp92 == 'l'){{struct Cyc_List_List*_tmp162;_tmp95=((_tmp162=_region_malloc(r,sizeof(*_tmp162)),((_tmp162->hd=(void*)((int)_tmp92),((_tmp162->tl=_tmp95,_tmp162))))));}++ i;}
break;case 'j': _LL72:
 goto _LL73;case 'z': _LL73:
 goto _LL74;case 't': _LL74:
 goto _LL75;case 'L': _LL75:
# 426
{struct Cyc_List_List*_tmp163;_tmp95=((_tmp163=_region_malloc(r,sizeof(*_tmp163)),((_tmp163->hd=(void*)((int)_tmp92),((_tmp163->tl=_tmp95,_tmp163))))));}
++ i;
break;default: _LL76:
 break;}
# 431
if(i >= _tmp90)return 0;
_tmp95=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp95);
# 435
_tmp92=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
switch(_tmp92){case 'd': _LL78:
 goto _LL79;case 'i': _LL79:
 goto _LL7A;case 'o': _LL7A:
 goto _LL7B;case 'u': _LL7B:
 goto _LL7C;case 'x': _LL7C:
 goto _LL7D;case 'X': _LL7D:
 goto _LL7E;case 'f': _LL7E:
 goto _LL7F;case 'F': _LL7F:
 goto _LL80;case 'e': _LL80:
 goto _LL81;case 'E': _LL81:
 goto _LL82;case 'g': _LL82:
 goto _LL83;case 'G': _LL83:
 goto _LL84;case 'a': _LL84:
 goto _LL85;case 'A': _LL85:
 goto _LL86;case 'c': _LL86:
 goto _LL87;case 's': _LL87:
 goto _LL88;case 'p': _LL88:
 goto _LL89;case 'n': _LL89:
 goto _LL8A;case '%': _LL8A:
 break;default: _LL8B:
 return 0;}{
# 458
struct _tuple11*_tmp166;struct Cyc_Core_Opt*_tmp165;return(_tmp165=_region_malloc(r,sizeof(*_tmp165)),((_tmp165->v=((_tmp166=_region_malloc(r,sizeof(*_tmp166)),((_tmp166->f1=_tmp91,((_tmp166->f2=_tmp93,((_tmp166->f3=_tmp95,((_tmp166->f4=_tmp92,((_tmp166->f5=i + 1,_tmp166)))))))))))),_tmp165)));};};};};}
# 460
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,unsigned int loc){
# 463
unsigned long _tmp9D=Cyc_strlen((struct _dyneither_ptr)s);
struct Cyc_List_List*_tmp9E=0;
int i;
struct _RegionHandle _tmp9F=_new_region("temp");struct _RegionHandle*temp=& _tmp9F;_push_region(temp);
for(i=0;i < _tmp9D;++ i){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),i))!= '%')continue;{
struct Cyc_Core_Opt*_tmpA0=Cyc_Formatstr_parse_inputformat(temp,s,i + 1);
if(_tmpA0 == 0){
{const char*_tmp169;void*_tmp168;(_tmp168=0,Cyc_Tcutil_terr(loc,((_tmp169="bad format string",_tag_dyneither(_tmp169,sizeof(char),18))),_tag_dyneither(_tmp168,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpA3=0;_npop_handler(0);return _tmpA3;};}{
# 474
int _tmpA5;struct Cyc_List_List*_tmpA6;struct Cyc_List_List*_tmpA7;char _tmpA8;int _tmpA9;struct _tuple11*_tmpA4=(struct _tuple11*)_tmpA0->v;_tmpA5=_tmpA4->f1;_tmpA6=_tmpA4->f2;_tmpA7=_tmpA4->f3;_tmpA8=_tmpA4->f4;_tmpA9=_tmpA4->f5;
i=_tmpA9 - 1;
{struct Cyc_List_List*_tmpAA=_tmpA7;int _tmpAB;_LL8E: if(_tmpAA == 0)goto _LL90;_tmpAB=(int)_tmpAA->hd;if(_tmpAA->tl != 0)goto _LL90;if(!(
(_tmpAB == 'j'  || _tmpAB == 'z') || _tmpAB == 't'))goto _LL90;_LL8F:
# 479
{const char*_tmp16D;void*_tmp16C[1];struct Cyc_Int_pa_PrintArg_struct _tmp16B;(_tmp16B.tag=1,((_tmp16B.f1=(unsigned long)_tmpAB,((_tmp16C[0]=& _tmp16B,Cyc_Tcutil_terr(loc,((_tmp16D="length modifier '%c' is not supported",_tag_dyneither(_tmp16D,sizeof(char),38))),_tag_dyneither(_tmp16C,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmpAF=0;_npop_handler(0);return _tmpAF;};_LL90:;_LL91:
 goto _LL8D;_LL8D:;}
# 483
if(_tmpA5)continue;{
void*t;
switch(_tmpA8){case 'd': _LL92:
 goto _LL93;case 'i': _LL93: {
# 488
struct Cyc_Core_Opt*_tmp16E;struct Cyc_Core_Opt*_tmpB0=(_tmp16E=_cycalloc(sizeof(*_tmp16E)),((_tmp16E->v=Cyc_Tcenv_lookup_type_vars(te),_tmp16E)));
{struct Cyc_List_List*_tmpB1=_tmpA7;int _tmpB2;int _tmpB3;int _tmpB4;int _tmpB5;_LL96: if(_tmpB1 != 0)goto _LL98;_LL97:
 t=Cyc_Absyn_sint_typ;goto _LL95;_LL98: if(_tmpB1 == 0)goto _LL9A;_tmpB2=(int)_tmpB1->hd;if(_tmpB1->tl != 0)goto _LL9A;if(!(_tmpB2 == 'l'))goto _LL9A;_LL99:
 t=Cyc_Absyn_slong_typ;goto _LL95;_LL9A: if(_tmpB1 == 0)goto _LL9C;_tmpB3=(int)_tmpB1->hd;if(_tmpB1->tl != 0)goto _LL9C;if(!(_tmpB3 == 'h'))goto _LL9C;_LL9B:
 t=Cyc_Absyn_sshort_typ;goto _LL95;_LL9C: if(_tmpB1 == 0)goto _LL9E;_tmpB4=(int)_tmpB1->hd;if(_tmpB1->tl == 0)goto _LL9E;_tmpB5=(int)(_tmpB1->tl)->hd;if((_tmpB1->tl)->tl != 0)goto _LL9E;if(!(
_tmpB4 == 'h'  && _tmpB5 == 'h'))goto _LL9E;_LL9D: t=Cyc_Absyn_schar_typ;goto _LL95;_LL9E:;_LL9F:
# 495
{const char*_tmp173;void*_tmp172[2];struct Cyc_String_pa_PrintArg_struct _tmp171;struct Cyc_Int_pa_PrintArg_struct _tmp170;(_tmp170.tag=1,((_tmp170.f1=(unsigned long)((int)_tmpA8),((_tmp171.tag=0,((_tmp171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpA7)),((_tmp172[0]=& _tmp171,((_tmp172[1]=& _tmp170,Cyc_Tcutil_terr(loc,((_tmp173="length modifier '%s' is not allowed with %%%c",_tag_dyneither(_tmp173,sizeof(char),46))),_tag_dyneither(_tmp172,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmpBA=0;_npop_handler(0);return _tmpBA;};_LL95:;}
# 499
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpB0),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
{struct Cyc_List_List*_tmp174;_tmp9E=((_tmp174=_region_malloc(r,sizeof(*_tmp174)),((_tmp174->hd=t,((_tmp174->tl=_tmp9E,_tmp174))))));}
break;}case 'u': _LL94:
 goto _LLA0;case 'o': _LLA0:
 goto _LLA1;case 'x': _LLA1:
 goto _LLA2;case 'X': _LLA2: {
# 506
struct Cyc_Core_Opt*_tmp175;struct Cyc_Core_Opt*_tmpBD=(_tmp175=_cycalloc(sizeof(*_tmp175)),((_tmp175->v=Cyc_Tcenv_lookup_type_vars(te),_tmp175)));
{struct Cyc_List_List*_tmpBE=_tmpA7;int _tmpBF;int _tmpC0;int _tmpC1;int _tmpC2;_LLA5: if(_tmpBE != 0)goto _LLA7;_LLA6:
 t=Cyc_Absyn_uint_typ;goto _LLA4;_LLA7: if(_tmpBE == 0)goto _LLA9;_tmpBF=(int)_tmpBE->hd;if(_tmpBE->tl != 0)goto _LLA9;if(!(_tmpBF == 'l'))goto _LLA9;_LLA8:
 t=Cyc_Absyn_ulong_typ;goto _LLA4;_LLA9: if(_tmpBE == 0)goto _LLAB;_tmpC0=(int)_tmpBE->hd;if(_tmpBE->tl != 0)goto _LLAB;if(!(_tmpC0 == 'h'))goto _LLAB;_LLAA:
 t=Cyc_Absyn_ushort_typ;goto _LLA4;_LLAB: if(_tmpBE == 0)goto _LLAD;_tmpC1=(int)_tmpBE->hd;if(_tmpBE->tl == 0)goto _LLAD;_tmpC2=(int)(_tmpBE->tl)->hd;if((_tmpBE->tl)->tl != 0)goto _LLAD;if(!(
_tmpC1 == 'h'  && _tmpC2 == 'h'))goto _LLAD;_LLAC: t=Cyc_Absyn_uchar_typ;goto _LLA4;_LLAD:;_LLAE:
# 513
{const char*_tmp17A;void*_tmp179[2];struct Cyc_String_pa_PrintArg_struct _tmp178;struct Cyc_Int_pa_PrintArg_struct _tmp177;(_tmp177.tag=1,((_tmp177.f1=(unsigned long)((int)_tmpA8),((_tmp178.tag=0,((_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpA7)),((_tmp179[0]=& _tmp178,((_tmp179[1]=& _tmp177,Cyc_Tcutil_terr(loc,((_tmp17A="length modifier '%s' is not allowed with %%%c",_tag_dyneither(_tmp17A,sizeof(char),46))),_tag_dyneither(_tmp179,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmpC7=0;_npop_handler(0);return _tmpC7;};_LLA4:;}
# 517
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpBD),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
{struct Cyc_List_List*_tmp17B;_tmp9E=((_tmp17B=_region_malloc(r,sizeof(*_tmp17B)),((_tmp17B->hd=t,((_tmp17B->tl=_tmp9E,_tmp17B))))));}
break;}case 'f': _LLA3:
 goto _LLAF;case 'F': _LLAF:
 goto _LLB0;case 'e': _LLB0:
 goto _LLB1;case 'E': _LLB1:
 goto _LLB2;case 'g': _LLB2:
 goto _LLB3;case 'G': _LLB3:
 goto _LLB4;case 'a': _LLB4:
 goto _LLB5;case 'A': _LLB5: {
# 528
struct Cyc_Core_Opt*_tmp17C;struct Cyc_Core_Opt*_tmpCA=(_tmp17C=_cycalloc(sizeof(*_tmp17C)),((_tmp17C->v=Cyc_Tcenv_lookup_type_vars(te),_tmp17C)));
{struct Cyc_List_List*_tmpCB=_tmpA7;int _tmpCC;_LLB8: if(_tmpCB != 0)goto _LLBA;_LLB9:
 t=Cyc_Absyn_float_typ(0);goto _LLB7;_LLBA: if(_tmpCB == 0)goto _LLBC;_tmpCC=(int)_tmpCB->hd;if(_tmpCB->tl != 0)goto _LLBC;if(!(_tmpCC == 'l'))goto _LLBC;_LLBB:
# 532
 t=Cyc_Absyn_float_typ(1);goto _LLB7;_LLBC:;_LLBD:
# 534
{const char*_tmp181;void*_tmp180[2];struct Cyc_String_pa_PrintArg_struct _tmp17F;struct Cyc_Int_pa_PrintArg_struct _tmp17E;(_tmp17E.tag=1,((_tmp17E.f1=(unsigned long)((int)_tmpA8),((_tmp17F.tag=0,((_tmp17F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpA7)),((_tmp180[0]=& _tmp17F,((_tmp180[1]=& _tmp17E,Cyc_Tcutil_terr(loc,((_tmp181="length modifier '%s' is not allowed with %%%c",_tag_dyneither(_tmp181,sizeof(char),46))),_tag_dyneither(_tmp180,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmpD1=0;_npop_handler(0);return _tmpD1;};_LLB7:;}
# 538
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpCA),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
{struct Cyc_List_List*_tmp182;_tmp9E=((_tmp182=_region_malloc(r,sizeof(*_tmp182)),((_tmp182->hd=t,((_tmp182->tl=_tmp9E,_tmp182))))));}
break;}case 'c': _LLB6: {
# 543
struct Cyc_Core_Opt*_tmp183;struct Cyc_Core_Opt*_tmpD4=(_tmp183=_cycalloc(sizeof(*_tmp183)),((_tmp183->v=Cyc_Tcenv_lookup_type_vars(te),_tmp183)));
{struct Cyc_List_List*_tmp184;_tmp9E=((_tmp184=_region_malloc(r,sizeof(*_tmp184)),((_tmp184->hd=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpD4),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((_tmp184->tl=_tmp9E,_tmp184))))));}
# 547
break;}case 's': _LLBE: {
# 549
struct Cyc_Core_Opt*_tmp185;struct Cyc_Core_Opt*_tmpD7=(_tmp185=_cycalloc(sizeof(*_tmp185)),((_tmp185->v=Cyc_Tcenv_lookup_type_vars(te),_tmp185)));
# 551
void*ptr=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpD7),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
{struct Cyc_List_List*_tmp186;_tmp9E=((_tmp186=_region_malloc(r,sizeof(*_tmp186)),((_tmp186->hd=ptr,((_tmp186->tl=_tmp9E,_tmp186))))));}
break;}case '[': _LLBF:
 goto _LLC0;case 'p': _LLC0:
# 557
{const char*_tmp18A;void*_tmp189[1];struct Cyc_Int_pa_PrintArg_struct _tmp188;(_tmp188.tag=1,((_tmp188.f1=(unsigned long)((int)_tmpA8),((_tmp189[0]=& _tmp188,Cyc_Tcutil_terr(loc,((_tmp18A="%%%c is not supported",_tag_dyneither(_tmp18A,sizeof(char),22))),_tag_dyneither(_tmp189,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmpDD=0;_npop_handler(0);return _tmpDD;};case 'n': _LLC1: {
# 560
struct Cyc_Core_Opt*_tmp18B;struct Cyc_Core_Opt*_tmpDE=(_tmp18B=_cycalloc(sizeof(*_tmp18B)),((_tmp18B->v=Cyc_Tcenv_lookup_type_vars(te),_tmp18B)));
{struct Cyc_List_List*_tmpDF=_tmpA7;int _tmpE0;int _tmpE1;int _tmpE2;int _tmpE3;_LLC4: if(_tmpDF != 0)goto _LLC6;_LLC5:
 t=Cyc_Absyn_sint_typ;goto _LLC3;_LLC6: if(_tmpDF == 0)goto _LLC8;_tmpE0=(int)_tmpDF->hd;if(_tmpDF->tl != 0)goto _LLC8;if(!(_tmpE0 == 'l'))goto _LLC8;_LLC7:
 t=Cyc_Absyn_ulong_typ;goto _LLC3;_LLC8: if(_tmpDF == 0)goto _LLCA;_tmpE1=(int)_tmpDF->hd;if(_tmpDF->tl != 0)goto _LLCA;if(!(_tmpE1 == 'h'))goto _LLCA;_LLC9:
 t=Cyc_Absyn_sshort_typ;goto _LLC3;_LLCA: if(_tmpDF == 0)goto _LLCC;_tmpE2=(int)_tmpDF->hd;if(_tmpDF->tl == 0)goto _LLCC;_tmpE3=(int)(_tmpDF->tl)->hd;if((_tmpDF->tl)->tl != 0)goto _LLCC;if(!(
_tmpE2 == 'h'  && _tmpE3 == 'h'))goto _LLCC;_LLCB: t=Cyc_Absyn_schar_typ;goto _LLC3;_LLCC:;_LLCD:
# 567
{const char*_tmp190;void*_tmp18F[2];struct Cyc_String_pa_PrintArg_struct _tmp18E;struct Cyc_Int_pa_PrintArg_struct _tmp18D;(_tmp18D.tag=1,((_tmp18D.f1=(unsigned long)((int)_tmpA8),((_tmp18E.tag=0,((_tmp18E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpA7)),((_tmp18F[0]=& _tmp18E,((_tmp18F[1]=& _tmp18D,Cyc_Tcutil_terr(loc,((_tmp190="length modifier '%s' is not allowed with %%%c",_tag_dyneither(_tmp190,sizeof(char),46))),_tag_dyneither(_tmp18F,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmpE8=0;_npop_handler(0);return _tmpE8;};_LLC3:;}
# 571
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpDE),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
{struct Cyc_List_List*_tmp191;_tmp9E=((_tmp191=_region_malloc(r,sizeof(*_tmp191)),((_tmp191->hd=t,((_tmp191->tl=_tmp9E,_tmp191))))));}
break;}case '%': _LLC2:
# 575
 if(_tmpA5){
{const char*_tmp194;void*_tmp193;(_tmp193=0,Cyc_Tcutil_terr(loc,((_tmp194="Assignment suppression (*) is not allowed with %%%%",_tag_dyneither(_tmp194,sizeof(char),52))),_tag_dyneither(_tmp193,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpED=0;_npop_handler(0);return _tmpED;};}
# 579
if(_tmpA6 != 0){
{const char*_tmp198;void*_tmp197[1];struct Cyc_String_pa_PrintArg_struct _tmp196;(_tmp196.tag=0,((_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmpA6)),((_tmp197[0]=& _tmp196,Cyc_Tcutil_terr(loc,((_tmp198="width '%s' not allowed with %%%%",_tag_dyneither(_tmp198,sizeof(char),33))),_tag_dyneither(_tmp197,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmpF1=0;_npop_handler(0);return _tmpF1;};}
# 583
if(_tmpA7 != 0){
{const char*_tmp19C;void*_tmp19B[1];struct Cyc_String_pa_PrintArg_struct _tmp19A;(_tmp19A.tag=0,((_tmp19A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmpA7)),((_tmp19B[0]=& _tmp19A,Cyc_Tcutil_terr(loc,((_tmp19C="length modifier '%s' not allowed with %%%%",_tag_dyneither(_tmp19C,sizeof(char),43))),_tag_dyneither(_tmp19B,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmpF5=0;_npop_handler(0);return _tmpF5;};}
# 587
break;default: _LLCE: {
# 589
struct Cyc_List_List*_tmpF6=0;_npop_handler(0);return _tmpF6;}}};};};}{
# 593
struct Cyc_List_List*_tmpF7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9E);_npop_handler(0);return _tmpF7;};
# 466
;_pop_region(temp);}
