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
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 138
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 302
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 47
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 922 "absyn.h"
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 967
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1160
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1164
int Cyc_Absyn_is_nontagged_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 65
void*Cyc_Tcutil_compress(void*t);
# 98
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 158
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 167
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 231 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 263
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 273
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 326
int Cyc_Tcutil_is_noreturn(void*);
# 341
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple12{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple12 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
# 79
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
# 82
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};
# 94
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 102
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 140
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 142
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 147
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 163 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 182
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 185
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 188
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 191
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 197
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 200
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 222 "cf_flowinfo.h"
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 225
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 232
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 236
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 241
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple14{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple14 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple14 pr1,struct _tuple14 pr2);
# 247
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 252
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 261
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 35
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 68 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 61 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
typedef unsigned int Cyc_NewControlFlow_seg_t;
typedef struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_vardecl_opt_t;
# 66
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
# 74
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};
# 79
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp6F5;struct Cyc_NewControlFlow_CFStmtAnnot _tmp6F4;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp6F3;enclosee->annot=(void*)((_tmp6F3=_cycalloc(sizeof(*_tmp6F3)),((_tmp6F3[0]=((_tmp6F5.tag=Cyc_NewControlFlow_CFAnnot,((_tmp6F5.f1=((_tmp6F4.encloser=encloser,((_tmp6F4.visited=0,_tmp6F4)))),_tmp6F5)))),_tmp6F3))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
# 105 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;
# 113
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,struct Cyc_Absyn_Exp*);
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 122
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2:
 return _tmp5;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp6FB;const char*_tmp6FA;struct Cyc_Core_Impossible_exn_struct*_tmp6F9;(int)_throw((void*)((_tmp6F9=_cycalloc(sizeof(*_tmp6F9)),((_tmp6F9[0]=((_tmp6FB.tag=Cyc_Core_Impossible,((_tmp6FB.f1=((_tmp6FA="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp6FA,sizeof(char),37))),_tmp6FB)))),_tmp6F9)))));}_LL0:;}
# 129
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp6FC;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp6FC=_region_malloc(env->r,sizeof(*_tmp6FC)),((_tmp6FC[0]=Cyc_CfFlowInfo_BottomFL(),_tmp6FC)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 136
return*sflow;}struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 139
static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpB=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 143
*_tmpB=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpB);
# 146
_tmpA->visited=env->iteration_num;{
struct _tuple17 _tmp6FD;return(_tmp6FD.f1=_tmpA,((_tmp6FD.f2=_tmpB,_tmp6FD)));};}
# 158 "new_control_flow.cyc"
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 166
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple18{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 173
static void Cyc_NewControlFlow_check_unique_root(struct _tuple18*ckenv,void*root,void*rval){
# 175
struct Cyc_NewControlFlow_AnalEnv*_tmpE;unsigned int _tmpF;struct Cyc_Dict_Dict _tmp10;struct _tuple18*_tmpD=ckenv;_tmpE=_tmpD->f1;_tmpF=_tmpD->f2;_tmp10=_tmpD->f3;{
void*_tmp11=root;struct Cyc_Absyn_Vardecl*_tmp13;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp12=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp11;if(_tmp12->tag != 0)goto _LL8;else{_tmp13=_tmp12->f1;}}_LL7:
# 178
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp10,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpE->fenv)->r,_tmp13->type)){
retry: {void*_tmp14=rval;void*_tmp16;_LLB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp15=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp15->tag != 8)goto _LLD;else{_tmp16=(void*)_tmp15->f2;}}_LLC:
 rval=_tmp16;goto retry;_LLD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp17=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp17->tag != 7)goto _LLF;}_LLE:
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp18=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp18->tag != 3)goto _LL11;else{if(_tmp18->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL11;}}_LL10:
 goto _LL12;_LL11: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp19=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp19->tag != 0)goto _LL13;}_LL12:
 goto _LLA;_LL13:;_LL14:
# 187
{const char*_tmp701;void*_tmp700[1];struct Cyc_String_pa_PrintArg_struct _tmp6FF;(_tmp6FF.tag=0,((_tmp6FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp700[0]=& _tmp6FF,Cyc_Tcutil_warn(_tmpF,((_tmp701="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp701,sizeof(char),60))),_tag_dyneither(_tmp700,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}}
# 191
goto _LL5;_LL8:;_LL9:
 goto _LL5;_LL5:;};}
# 199
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1D=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp1E=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp1F=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1E);
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple16 _tmp702;struct _tuple16 _tmp21=(_tmp702.f1=flow,((_tmp702.f2=_tmp1F,_tmp702)));struct Cyc_Dict_Dict _tmp22;struct Cyc_Dict_Dict _tmp23;_LL16: if(((_tmp21.f1).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple13)((_tmp21.f1).ReachableFL).val).f1;if(((_tmp21.f2).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple13)((_tmp21.f2).ReachableFL).val).f1;_LL17: {
# 206
struct _tuple18 _tmp703;struct _tuple18 _tmp24=(_tmp703.f1=env,((_tmp703.f2=s->loc,((_tmp703.f3=_tmp23,_tmp703)))));
((void(*)(void(*f)(struct _tuple18*,void*,void*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp24,_tmp22);
goto _LL15;}_LL18:;_LL19:
 goto _LL15;_LL15:;}
# 214
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1F,*_tmp1E)){
# 221
*_tmp1E=_tmp1F;
# 225
if(_tmp1D->visited == env->iteration_num)
# 227
env->iterate_again=1;}}
# 232
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 236
union Cyc_CfFlowInfo_FlowInfo _tmp26=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp28;_LL1B: if((_tmp26.BottomFL).tag != 1)goto _LL1D;_LL1C:
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp26.ReachableFL).tag != 2)goto _LL1A;_tmp27=((struct _tuple13)(_tmp26.ReachableFL).val).f1;_tmp28=((struct _tuple13)(_tmp26.ReachableFL).val).f2;_LL1E:
# 239
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp706;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp705;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp705=_region_malloc(fenv->r,sizeof(*_tmp705)),((_tmp705[0]=((_tmp706.tag=0,((_tmp706.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp706)))),_tmp705)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp709;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp708;_tmp2A=(void*)((_tmp708=_region_malloc(fenv->r,sizeof(*_tmp708)),((_tmp708[0]=((_tmp709.tag=8,((_tmp709.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp709.f2=_tmp2A,_tmp709)))))),_tmp708))));}
# 246
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp29,_tmp2A);}
# 248
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp28);_LL1A:;}
# 252
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp2F=inflow;struct Cyc_Dict_Dict _tmp30;struct Cyc_List_List*_tmp31;_LL20: if((_tmp2F.BottomFL).tag != 1)goto _LL22;_LL21:
 return Cyc_CfFlowInfo_BottomFL();_LL22: if((_tmp2F.ReachableFL).tag != 2)goto _LL1F;_tmp30=((struct _tuple13)(_tmp2F.ReachableFL).val).f1;_tmp31=((struct _tuple13)(_tmp2F.ReachableFL).val).f2;_LL23:
# 256
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp30,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp70C;void*_tmp70B;(_tmp70B=0,Cyc_Tcutil_terr(loc,((_tmp70C="expression may not be fully initialized",_tag_dyneither(_tmp70C,sizeof(char),40))),_tag_dyneither(_tmp70B,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp34=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp30,env->all_changed,r);
if(_tmp30.t == _tmp34.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp35=Cyc_CfFlowInfo_ReachableFL(_tmp34,_tmp31);
Cyc_NewControlFlow_update_tryflow(env,_tmp35);
return _tmp35;};};_LL1F:;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 266
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp70D;struct _tuple0 _tmp37=(_tmp70D.f1=Cyc_Tcutil_compress(t),((_tmp70D.f2=r,_tmp70D)));void*_tmp3C;struct Cyc_Absyn_Datatypefield*_tmp3E;struct _dyneither_ptr _tmp40;struct Cyc_List_List*_tmp42;struct _dyneither_ptr _tmp44;union Cyc_Absyn_AggrInfoU _tmp46;struct Cyc_List_List*_tmp47;struct _dyneither_ptr _tmp49;enum Cyc_Absyn_AggrKind _tmp4B;struct Cyc_List_List*_tmp4C;struct _dyneither_ptr _tmp4E;_LL25: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp38=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp38->tag != 3)goto _LL27;else{if(_tmp38->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL27;}}_LL26:
 return;_LL27: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp39=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp39->tag != 0)goto _LL29;}_LL28:
 return;_LL29: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp3A->tag != 7)goto _LL2B;}_LL2A:
 return;_LL2B: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp3B->tag != 8)goto _LL2D;else{_tmp3C=(void*)_tmp3B->f2;}}_LL2C:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp3C);return;_LL2D:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp37.f1;if(_tmp3D->tag != 4)goto _LL2F;else{if((((_tmp3D->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2F;_tmp3E=((struct _tuple2)(((_tmp3D->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp3F->tag != 6)goto _LL2F;else{_tmp40=_tmp3F->f2;}};_LL2E:
# 273
 if(_tmp3E->typs == 0)
return;
_tmp42=_tmp3E->typs;_tmp44=_tmp40;goto _LL30;_LL2F:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp37.f1;if(_tmp41->tag != 10)goto _LL31;else{_tmp42=_tmp41->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp43=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp43->tag != 6)goto _LL31;else{_tmp44=_tmp43->f2;}};_LL30: {
# 277
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp42);
{int i=0;for(0;i < sz;++ i){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp42))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp44,sizeof(void*),i)));}}
# 281
return;}_LL31:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp45=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37.f1;if(_tmp45->tag != 11)goto _LL33;else{_tmp46=(_tmp45->f1).aggr_info;_tmp47=(_tmp45->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp48=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp48->tag != 6)goto _LL33;else{_tmp49=_tmp48->f2;}};_LL32: {
# 284
struct Cyc_Absyn_Aggrdecl*_tmp50=Cyc_Absyn_get_known_aggrdecl(_tmp46);
if(_tmp50->impl == 0)return;{
struct Cyc_List_List*_tmp51=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50->impl))->fields;
struct _RegionHandle _tmp52=_new_region("temp");struct _RegionHandle*temp=& _tmp52;_push_region(temp);
{struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp50->tvs,_tmp47);
{int i=0;for(0;i < _get_dyneither_size(_tmp49,sizeof(void*));(i ++,_tmp51=_tmp51->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp51))->hd)->type;
if(_tmp53 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp53,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp49.curr)[i]);}}
# 294
_npop_handler(0);return;}
# 288
;_pop_region(temp);};}_LL33:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp37.f1;if(_tmp4A->tag != 12)goto _LL35;else{_tmp4B=_tmp4A->f1;_tmp4C=_tmp4A->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp4D->tag != 6)goto _LL35;else{_tmp4E=_tmp4D->f2;}};_LL34:
# 297
{int i=0;for(0;i < _get_dyneither_size(_tmp4E,sizeof(void*));(i ++,_tmp4C=_tmp4C->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp4C))->hd)->type,((void**)_tmp4E.curr)[i]);}}
return;_LL35:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37.f1;if(_tmp4F->tag != 5)goto _LL37;}if(!
Cyc_Tcutil_is_noalias_pointer(t))goto _LL37;_LL36:
{const char*_tmp710;void*_tmp70F;(_tmp70F=0,Cyc_Tcutil_warn(loc,((_tmp710="argument may still contain unique pointers",_tag_dyneither(_tmp710,sizeof(char),43))),_tag_dyneither(_tmp70F,sizeof(void*),0)));}
return;_LL37:;_LL38:
 return;_LL24:;}
# 349 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp56=inflow;struct Cyc_Dict_Dict _tmp57;struct Cyc_List_List*_tmp58;_LL3A: if((_tmp56.BottomFL).tag != 1)goto _LL3C;_LL3B:
 return Cyc_CfFlowInfo_BottomFL();_LL3C: if((_tmp56.ReachableFL).tag != 2)goto _LL39;_tmp57=((struct _tuple13)(_tmp56.ReachableFL).val).f1;_tmp58=((struct _tuple13)(_tmp56.ReachableFL).val).f2;_LL3D:
# 353
 if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp713;void*_tmp712;(_tmp712=0,Cyc_Tcutil_terr(loc,((_tmp713="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp713,sizeof(char),47))),_tag_dyneither(_tmp712,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{
# 357
void*_tmp5B=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp5C=r;void*_tmp5E;struct Cyc_CfFlowInfo_Place*_tmp60;_LL3F: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C;if(_tmp5D->tag != 8)goto _LL41;else{_tmp5E=(void*)_tmp5D->f2;}}_LL40:
 r=_tmp5E;goto retry;_LL41: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp5F=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp5C;if(_tmp5F->tag != 5)goto _LL43;else{_tmp60=_tmp5F->f1;}}_LL42:
# 361
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp5B,Cyc_CfFlowInfo_lookup_place(_tmp57,_tmp60));
goto _LL3E;_LL43:;_LL44:
# 364
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp5B)){
const char*_tmp716;void*_tmp715;(_tmp715=0,Cyc_Tcutil_warn(loc,((_tmp716="argument may contain live unique pointers",_tag_dyneither(_tmp716,sizeof(char),42))),_tag_dyneither(_tmp715,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL3E:;}{
# 368
struct Cyc_Dict_Dict _tmp63=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp57,env->all_changed,r);
if(_tmp57.t == _tmp63.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp64=Cyc_CfFlowInfo_ReachableFL(_tmp63,_tmp58);
Cyc_NewControlFlow_update_tryflow(env,_tmp64);
return _tmp64;};};};_LL39:;}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 377
static struct _tuple20 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 384
if(es == 0){
struct _tuple20 _tmp717;return(_tmp717.f1=inflow,((_tmp717.f2=0,_tmp717)));}
if(es->tl == 0){
union Cyc_CfFlowInfo_FlowInfo _tmp67;void*_tmp68;struct _tuple14 _tmp66=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{
struct Cyc_List_List*_tmp71A;struct _tuple20 _tmp719;return(_tmp719.f1=_tmp67,((_tmp719.f2=((_tmp71A=_region_malloc(rgn,sizeof(*_tmp71A)),((_tmp71A->hd=_tmp68,((_tmp71A->tl=0,_tmp71A)))))),_tmp719)));};}{
# 390
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
do{
this_all_changed=(env->fenv)->mt_place_set;
# 398
{struct Cyc_Dict_Dict*_tmp71B;env->all_changed=((_tmp71B=_region_malloc(env->r,sizeof(*_tmp71B)),((_tmp71B[0]=(env->fenv)->mt_place_set,_tmp71B))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp6D;void*_tmp6E;struct _tuple14 _tmp6C=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;
outflow=_tmp6D;
{struct Cyc_List_List*_tmp71C;rvals=((_tmp71C=_region_malloc(rgn,sizeof(*_tmp71C)),((_tmp71C->hd=_tmp6E,((_tmp71C->tl=0,_tmp71C))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 404
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp71D;env->all_changed=((_tmp71D=_region_malloc(env->r,sizeof(*_tmp71D)),((_tmp71D[0]=(env->fenv)->mt_place_set,_tmp71D))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp72;void*_tmp73;struct _tuple14 _tmp71=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp72=_tmp71.f1;_tmp73=_tmp71.f2;
{struct Cyc_List_List*_tmp71E;rvals=((_tmp71E=_region_malloc(rgn,sizeof(*_tmp71E)),((_tmp71E->hd=_tmp73,((_tmp71E->tl=rvals,_tmp71E))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp72,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 410
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 413
old_inflow=inflow;
# 416
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 421
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)
env->all_changed=0;else{
# 425
struct Cyc_Dict_Dict*_tmp71F;env->all_changed=((_tmp71F=_region_malloc(env->r,sizeof(*_tmp71F)),((_tmp71F[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp71F))));}
# 427
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple20 _tmp720;return(_tmp720.f1=outflow,((_tmp720.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp720)));};};}
# 433
static struct _tuple14 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 436
struct _RegionHandle _tmp77=_new_region("rgn");struct _RegionHandle*rgn=& _tmp77;_push_region(rgn);
{union Cyc_CfFlowInfo_FlowInfo _tmp79;struct Cyc_List_List*_tmp7A;struct _tuple20 _tmp78=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp7B=_tmp79;struct Cyc_Dict_Dict _tmp7C;_LL46: if((_tmp7B.ReachableFL).tag != 2)goto _LL48;_tmp7C=((struct _tuple13)(_tmp7B.ReachableFL).val).f1;_LL47:
# 441
 for(0;_tmp7A != 0;(_tmp7A=_tmp7A->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp7C,(void*)_tmp7A->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp723;void*_tmp722;(_tmp722=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp723="expression may not be initialized",_tag_dyneither(_tmp723,sizeof(char),34))),_tag_dyneither(_tmp722,sizeof(void*),0)));}}
goto _LL45;_LL48: if((_tmp7B.BottomFL).tag != 1)goto _LL45;_LL49:
 goto _LL45;_LL45:;}{
# 447
struct _tuple14 _tmp724;struct _tuple14 _tmp80=(_tmp724.f1=_tmp79,((_tmp724.f2=(env->fenv)->unknown_all,_tmp724)));_npop_handler(0);return _tmp80;};}
# 437
;_pop_region(rgn);}
# 461 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 467
union Cyc_CfFlowInfo_FlowInfo _tmp81=outflow;struct Cyc_Dict_Dict _tmp82;struct Cyc_List_List*_tmp83;_LL4B: if((_tmp81.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return outflow;_LL4D: if((_tmp81.ReachableFL).tag != 2)goto _LL4A;_tmp82=((struct _tuple13)(_tmp81.ReachableFL).val).f1;_tmp83=((struct _tuple13)(_tmp81.ReachableFL).val).f2;_LL4E: {
# 470
union Cyc_CfFlowInfo_AbsLVal _tmp84=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp85;_LL50: if((_tmp84.UnknownL).tag != 2)goto _LL52;_LL51:
# 474
 return outflow;_LL52: if((_tmp84.PlaceL).tag != 1)goto _LL4F;_tmp85=(struct Cyc_CfFlowInfo_Place*)(_tmp84.PlaceL).val;_LL53: {
# 478
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp727;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp726;nzval=(void*)((_tmp726=_region_malloc((env->fenv)->r,sizeof(*_tmp726)),((_tmp726[0]=((_tmp727.tag=8,((_tmp727.f1=locname,((_tmp727.f2=nzval,_tmp727)))))),_tmp726))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp88=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp82,env->all_changed,_tmp85,nzval),_tmp83);
# 485
return _tmp88;};}_LL4F:;}_LL4A:;}
# 494
static struct _tuple16 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 497
union Cyc_CfFlowInfo_FlowInfo _tmp89=outflow;struct Cyc_Dict_Dict _tmp8A;struct Cyc_List_List*_tmp8B;_LL55: if((_tmp89.BottomFL).tag != 1)goto _LL57;_LL56: {
struct _tuple16 _tmp728;return(_tmp728.f1=outflow,((_tmp728.f2=outflow,_tmp728)));}_LL57: if((_tmp89.ReachableFL).tag != 2)goto _LL54;_tmp8A=((struct _tuple13)(_tmp89.ReachableFL).val).f1;_tmp8B=((struct _tuple13)(_tmp89.ReachableFL).val).f2;_LL58: {
# 500
union Cyc_CfFlowInfo_AbsLVal _tmp8D=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp8E;_LL5A: if((_tmp8D.UnknownL).tag != 2)goto _LL5C;_LL5B: {
struct _tuple16 _tmp729;return(_tmp729.f1=outflow,((_tmp729.f2=outflow,_tmp729)));}_LL5C: if((_tmp8D.PlaceL).tag != 1)goto _LL59;_tmp8E=(struct Cyc_CfFlowInfo_Place*)(_tmp8D.PlaceL).val;_LL5D: {
# 503
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp72C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp72B;nzval=(void*)((_tmp72B=_region_malloc((env->fenv)->r,sizeof(*_tmp72B)),((_tmp72B[0]=((_tmp72C.tag=8,((_tmp72C.f1=locname,((_tmp72C.f2=nzval,_tmp72C)))))),_tmp72B))));}{
struct _tuple16 _tmp72D;return
(_tmp72D.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8A,env->all_changed,_tmp8E,nzval),_tmp8B),((_tmp72D.f2=
# 509
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8A,env->all_changed,_tmp8E,(env->fenv)->zero),_tmp8B),_tmp72D)));};}_LL59:;}_LL54:;}
# 515
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 521
static struct _tuple14 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 525
void*_tmp95=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp97;union Cyc_Absyn_Constraint*_tmp98;union Cyc_Absyn_Constraint*_tmp99;_LL5F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp96=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95;if(_tmp96->tag != 5)goto _LL61;else{_tmp97=(_tmp96->f1).elt_typ;_tmp98=((_tmp96->f1).ptr_atts).bounds;_tmp99=((_tmp96->f1).ptr_atts).zero_term;}}_LL60: {
# 527
union Cyc_CfFlowInfo_FlowInfo _tmp9A=f;struct Cyc_Dict_Dict _tmp9B;struct Cyc_List_List*_tmp9C;_LL64: if((_tmp9A.BottomFL).tag != 1)goto _LL66;_LL65: {
# 529
struct _tuple14 _tmp72E;return(_tmp72E.f1=f,((_tmp72E.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,(env->fenv)->unknown_all),_tmp72E)));}_LL66: if((_tmp9A.ReachableFL).tag != 2)goto _LL63;_tmp9B=((struct _tuple13)(_tmp9A.ReachableFL).val).f1;_tmp9C=((struct _tuple13)(_tmp9A.ReachableFL).val).f2;_LL67: {
# 532
struct Cyc_Absyn_Vardecl*locname=0;
{void*_tmp9E=r;struct Cyc_Absyn_Vardecl*_tmpA0;void*_tmpA1;_LL69: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9E;if(_tmp9F->tag != 8)goto _LL6B;else{_tmpA0=_tmp9F->f1;_tmpA1=(void*)_tmp9F->f2;}}_LL6A:
 locname=_tmpA0;goto _LL68;_LL6B:;_LL6C:
 goto _LL68;_LL68:;}
# 537
if(Cyc_Tcutil_is_bound_one(_tmp98)){
retry: {void*_tmpA2=r;struct Cyc_Absyn_Vardecl*_tmpA4;void*_tmpA5;struct Cyc_CfFlowInfo_Place*_tmpA9;enum Cyc_CfFlowInfo_InitLevel _tmpAC;_LL6E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpA3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpA3->tag != 8)goto _LL70;else{_tmpA4=_tmpA3->f1;_tmpA5=(void*)_tmpA3->f2;}}_LL6F:
 r=_tmpA5;goto retry;_LL70: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpA6=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpA6->tag != 1)goto _LL72;}_LL71:
 goto _LL73;_LL72: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpA7=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpA7->tag != 2)goto _LL74;}_LL73:
# 542
{void*_tmpAD=e->annot;struct Cyc_List_List*_tmpAF;_LL7D: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpAE=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpAD;if(_tmpAE->tag != Cyc_CfFlowInfo_NotZero)goto _LL7F;else{_tmpAF=_tmpAE->f1;}}_LL7E:
# 544
 if(!Cyc_CfFlowInfo_same_relns(_tmp9C,_tmpAF))goto _LL80;
goto _LL7C;_LL7F:;_LL80:
# 548
{void*_tmpB0=e->r;_LL82: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpB1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB0;if(_tmpB1->tag != 22)goto _LL84;}_LL83:
# 550
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp731;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp730;e->annot=(void*)((_tmp730=_cycalloc(sizeof(*_tmp730)),((_tmp730[0]=((_tmp731.tag=Cyc_CfFlowInfo_NotZero,((_tmp731.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp731)))),_tmp730))));}
goto _LL81;_LL84:;_LL85:
# 553
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL81;_LL81:;}
# 556
goto _LL7C;_LL7C:;}
# 558
goto _LL6D;_LL74: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpA8=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpA8->tag != 5)goto _LL76;else{_tmpA9=_tmpA8->f1;}}_LL75:
# 560
{void*_tmpB4=e->annot;struct Cyc_List_List*_tmpB6;_LL87: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpB5=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpB4;if(_tmpB5->tag != Cyc_CfFlowInfo_NotZero)goto _LL89;else{_tmpB6=_tmpB5->f1;}}_LL88:
# 562
 if(!Cyc_CfFlowInfo_same_relns(_tmp9C,_tmpB6))goto _LL8A;
goto _LL86;_LL89:;_LL8A:
# 566
{void*_tmpB7=e->r;_LL8C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpB8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB7;if(_tmpB8->tag != 22)goto _LL8E;}_LL8D:
# 568
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp734;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp733;e->annot=(void*)((_tmp733=_cycalloc(sizeof(*_tmp733)),((_tmp733[0]=((_tmp734.tag=Cyc_CfFlowInfo_NotZero,((_tmp734.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp734)))),_tmp733))));}
goto _LL8B;_LL8E:;_LL8F:
# 571
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8B;_LL8B:;}
# 574
goto _LL86;_LL86:;}{
# 576
struct _tuple14 _tmp735;return(_tmp735.f1=f,((_tmp735.f2=Cyc_CfFlowInfo_lookup_place(_tmp9B,_tmpA9),_tmp735)));};_LL76: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpAA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpAA->tag != 0)goto _LL78;}_LL77:
# 578
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp736;return(_tmp736.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp736.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,(env->fenv)->unknown_all),_tmp736)));};_LL78: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpAB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpAB->tag != 3)goto _LL7A;else{_tmpAC=_tmpAB->f1;}}_LL79:
# 581
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpAC,locname);goto _LL7B;_LL7A:;_LL7B:
# 583
{void*_tmpBD=e->r;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD;if(_tmpBE->tag != 22)goto _LL93;}_LL92:
# 585
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp739;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp738;e->annot=(void*)((_tmp738=_cycalloc(sizeof(*_tmp738)),((_tmp738[0]=((_tmp739.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp739.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp739)))),_tmp738))));}
goto _LL90;_LL93:;_LL94:
# 588
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL90;_LL90:;}
# 591
goto _LL6D;_LL6D:;}}else{
# 594
void*_tmpC1=e->annot;struct Cyc_List_List*_tmpC3;_LL96: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpC2=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC1;if(_tmpC2->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL98;else{_tmpC3=_tmpC2->f1;}}_LL97:
# 596
 if(!Cyc_CfFlowInfo_same_relns(_tmp9C,_tmpC3))goto _LL99;
goto _LL95;_LL98:;_LL99:
# 599
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp73C;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp73B;e->annot=(void*)((_tmp73B=_cycalloc(sizeof(*_tmp73B)),((_tmp73B[0]=((_tmp73C.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp73C.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp73C)))),_tmp73B))));}
goto _LL95;_LL95:;}
# 603
switch(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp9B,r)){case Cyc_CfFlowInfo_NoneIL: _LL9A:
# 605
{void*_tmpC6=r;_LL9D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpC7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpC6;if(_tmpC7->tag != 7)goto _LL9F;}_LL9E:
# 608
{const char*_tmp73F;void*_tmp73E;(_tmp73E=0,Cyc_Tcutil_terr(e->loc,((_tmp73F="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp73F,sizeof(char),61))),_tag_dyneither(_tmp73E,sizeof(void*),0)));}
goto _LL9C;_LL9F:;_LLA0: {
# 611
const char*_tmp742;void*_tmp741;(_tmp741=0,Cyc_Tcutil_terr(e->loc,((_tmp742="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp742,sizeof(char),46))),_tag_dyneither(_tmp741,sizeof(void*),0)));}_LL9C:;}
# 613
goto _LL9B;case Cyc_CfFlowInfo_AllIL: _LL9B: {
# 615
void*_tmpCC=(env->fenv)->unknown_all;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp745;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp744;_tmpCC=(void*)((_tmp744=_region_malloc((env->fenv)->r,sizeof(*_tmp744)),((_tmp744[0]=((_tmp745.tag=8,((_tmp745.f1=locname,((_tmp745.f2=_tmpCC,_tmp745)))))),_tmp744))));}{
struct _tuple14 _tmp746;return(_tmp746.f1=f,((_tmp746.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,_tmpCC),_tmp746)));};}case Cyc_CfFlowInfo_ThisIL: _LLA1: {
# 620
void*_tmpD0=(env->fenv)->unknown_none;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp749;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp748;_tmpD0=(void*)((_tmp748=_region_malloc((env->fenv)->r,sizeof(*_tmp748)),((_tmp748[0]=((_tmp749.tag=8,((_tmp749.f1=locname,((_tmp749.f2=_tmpD0,_tmp749)))))),_tmp748))));}{
struct _tuple14 _tmp74A;return(_tmp74A.f1=f,((_tmp74A.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,_tmpD0),_tmp74A)));};}}}_LL63:;}_LL61:;_LL62: {
# 626
struct Cyc_Core_Impossible_exn_struct _tmp750;const char*_tmp74F;struct Cyc_Core_Impossible_exn_struct*_tmp74E;(int)_throw((void*)((_tmp74E=_cycalloc(sizeof(*_tmp74E)),((_tmp74E[0]=((_tmp750.tag=Cyc_Core_Impossible,((_tmp750.f1=((_tmp74F="right deref of non-pointer-type",_tag_dyneither(_tmp74F,sizeof(char),32))),_tmp750)))),_tmp74E)))));}_LL5E:;}
# 632
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 634
void*_tmpD7=e1->r;struct Cyc_Absyn_Vardecl*_tmpDA;struct Cyc_Absyn_Vardecl*_tmpDD;struct Cyc_Absyn_Vardecl*_tmpE0;struct Cyc_Absyn_Vardecl*_tmpE3;_LLA4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD7;if(_tmpD8->tag != 1)goto _LLA6;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD9=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpD8->f2);if(_tmpD9->tag != 5)goto _LLA6;else{_tmpDA=_tmpD9->f1;}}}_LLA5:
 _tmpDD=_tmpDA;goto _LLA7;_LLA6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD7;if(_tmpDB->tag != 1)goto _LLA8;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDC=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpDB->f2);if(_tmpDC->tag != 4)goto _LLA8;else{_tmpDD=_tmpDC->f1;}}}_LLA7:
 _tmpE0=_tmpDD;goto _LLA9;_LLA8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD7;if(_tmpDE->tag != 1)goto _LLAA;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpDF=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpDE->f2);if(_tmpDF->tag != 3)goto _LLAA;else{_tmpE0=_tmpDF->f1;}}}_LLA9:
 _tmpE3=_tmpE0;goto _LLAB;_LLAA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD7;if(_tmpE1->tag != 1)goto _LLAC;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE2=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpE1->f2);if(_tmpE2->tag != 1)goto _LLAC;else{_tmpE3=_tmpE2->f1;}}}_LLAB:
# 639
 if(!_tmpE3->escapes){
void*_tmpE4=e2->r;struct Cyc_Absyn_Vardecl*_tmpE7;struct Cyc_Absyn_Vardecl*_tmpEA;struct Cyc_Absyn_Vardecl*_tmpED;struct Cyc_Absyn_Vardecl*_tmpF0;_LLAF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE4;if(_tmpE5->tag != 1)goto _LLB1;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpE6=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpE5->f2);if(_tmpE6->tag != 5)goto _LLB1;else{_tmpE7=_tmpE6->f1;}}}_LLB0:
 _tmpEA=_tmpE7;goto _LLB2;_LLB1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE4;if(_tmpE8->tag != 1)goto _LLB3;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpE8->f2);if(_tmpE9->tag != 4)goto _LLB3;else{_tmpEA=_tmpE9->f1;}}}_LLB2:
 _tmpED=_tmpEA;goto _LLB4;_LLB3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE4;if(_tmpEB->tag != 1)goto _LLB5;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpEC=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpEB->f2);if(_tmpEC->tag != 3)goto _LLB5;else{_tmpED=_tmpEC->f1;}}}_LLB4:
 _tmpF0=_tmpED;goto _LLB6;_LLB5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpEE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE4;if(_tmpEE->tag != 1)goto _LLB7;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpEF=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpEE->f2);if(_tmpEF->tag != 1)goto _LLB7;else{_tmpF0=_tmpEF->f1;}}}_LLB6:
# 645
 if(!_tmpF0->escapes){
# 647
int found=0;
{struct Cyc_List_List*_tmpF1=relns;for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
struct Cyc_CfFlowInfo_Reln*_tmpF2=(struct Cyc_CfFlowInfo_Reln*)_tmpF1->hd;
if(_tmpF2->vd == _tmpF0){
union Cyc_CfFlowInfo_RelnOp _tmpF3=_tmpF2->rop;struct Cyc_Absyn_Vardecl*_tmpF4;_LLBA: if((_tmpF3.LessNumelts).tag != 3)goto _LLBC;_tmpF4=(struct Cyc_Absyn_Vardecl*)(_tmpF3.LessNumelts).val;if(!(_tmpF4 == _tmpE3))goto _LLBC;_LLBB:
 return relns;_LLBC:;_LLBD:
 continue;_LLB9:;}}}
# 656
if(!found){
struct Cyc_CfFlowInfo_Reln*_tmp753;struct Cyc_List_List*_tmp752;return(_tmp752=_region_malloc(rgn,sizeof(*_tmp752)),((_tmp752->hd=((_tmp753=_region_malloc(rgn,sizeof(*_tmp753)),((_tmp753->vd=_tmpF0,((_tmp753->rop=Cyc_CfFlowInfo_LessNumelts(_tmpE3),_tmp753)))))),((_tmp752->tl=relns,_tmp752)))));}}
# 659
return relns;_LLB7:;_LLB8:
 return relns;_LLAE:;}else{
# 662
return relns;}_LLAC:;_LLAD:
 return relns;_LLA3:;}
# 669
static struct _tuple14 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 677
union Cyc_CfFlowInfo_FlowInfo _tmpF7=outflow;struct Cyc_Dict_Dict _tmpF8;struct Cyc_List_List*_tmpF9;_LLBF: if((_tmpF7.BottomFL).tag != 1)goto _LLC1;_LLC0: {
# 680
struct _tuple14 _tmp754;return(_tmp754.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp754.f2=rval,_tmp754)));}_LLC1: if((_tmpF7.ReachableFL).tag != 2)goto _LLBE;_tmpF8=((struct _tuple13)(_tmpF7.ReachableFL).val).f1;_tmpF9=((struct _tuple13)(_tmpF7.ReachableFL).val).f2;_LLC2: {
# 682
union Cyc_CfFlowInfo_AbsLVal _tmpFB=lval;struct Cyc_CfFlowInfo_Place*_tmpFC;_LLC4: if((_tmpFB.PlaceL).tag != 1)goto _LLC6;_tmpFC=(struct Cyc_CfFlowInfo_Place*)(_tmpFB.PlaceL).val;_LLC5: {
# 684
struct Cyc_Dict_Dict _tmpFD=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpF8,env->all_changed,_tmpFC,rval);
_tmpF9=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmpF9,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmpFD,_tmpF9);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 690
void*_tmpFE=Cyc_CfFlowInfo_lookup_place(_tmpF8,_tmpFC);_LLC9: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpFF=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmpFF->tag != 3)goto _LLCB;else{if(_tmpFF->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCB;}}_LLCA:
 goto _LLCC;_LLCB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp100=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmp100->tag != 7)goto _LLCD;}_LLCC:
 goto _LLCE;_LLCD: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp101=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmp101->tag != 0)goto _LLCF;}_LLCE:
 goto _LLD0;_LLCF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp102=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmp102->tag != 8)goto _LLD1;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp103=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp102->f2);if(_tmp103->tag != 3)goto _LLD1;else{if(_tmp103->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLD1;}}}_LLD0:
 goto _LLD2;_LLD1: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp104=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmp104->tag != 8)goto _LLD3;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp105=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp104->f2);if(_tmp105->tag != 7)goto _LLD3;}}_LLD2:
 goto _LLD4;_LLD3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp106=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmp106->tag != 8)goto _LLD5;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp107=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp106->f2);if(_tmp107->tag != 0)goto _LLD5;}}_LLD4:
 goto _LLC8;_LLD5:;_LLD6:
# 698
{const char*_tmp757;void*_tmp756;(_tmp756=0,Cyc_Tcutil_warn(lexp->loc,((_tmp757="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp757,sizeof(char),43))),_tag_dyneither(_tmp756,sizeof(void*),0)));}
goto _LLC8;_LLC8:;}
# 703
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 705
struct _tuple14 _tmp758;return(_tmp758.f1=outflow,((_tmp758.f2=rval,_tmp758)));};}_LLC6: if((_tmpFB.UnknownL).tag != 2)goto _LLC3;_LLC7: {
# 708
struct _tuple14 _tmp759;return(_tmp759.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp759.f2=rval,_tmp759)));}_LLC3:;}_LLBE:;}
# 713
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
void*_tmp10C=e->r;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11C;_LLD8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp10D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp10D->tag != 1)goto _LLDA;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp10E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp10D->f2);if(_tmp10E->tag != 4)goto _LLDA;}}_LLD9:
 goto _LLDB;_LLDA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp10F->tag != 1)goto _LLDC;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp110=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp10F->f2);if(_tmp110->tag != 3)goto _LLDC;}}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp111->tag != 1)goto _LLDE;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp112=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp111->f2);if(_tmp112->tag != 5)goto _LLDE;}}_LLDD:
 return 1;_LLDE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp113->tag != 19)goto _LLE0;else{_tmp114=_tmp113->f1;}}_LLDF:
 _tmp116=_tmp114;goto _LLE1;_LLE0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp115=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp115->tag != 20)goto _LLE2;else{_tmp116=_tmp115->f1;}}_LLE1:
 _tmp118=_tmp116;goto _LLE3;_LLE2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp117->tag != 21)goto _LLE4;else{_tmp118=_tmp117->f1;}}_LLE3:
# 721
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp118);_LLE4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp119=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp119->tag != 22)goto _LLE6;else{_tmp11A=_tmp119->f1;}}_LLE5: {
# 723
void*_tmp11D=Cyc_Tcutil_compress((void*)_check_null(_tmp11A->topt));_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp11E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp11D;if(_tmp11E->tag != 10)goto _LLED;}_LLEC:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp11A);_LLED:;_LLEE:
 return 0;_LLEA:;}_LLE6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp11B->tag != 13)goto _LLE8;else{_tmp11C=_tmp11B->f2;}}_LLE7:
# 727
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp11C);_LLE8:;_LLE9:
 return 0;_LLD7:;}
# 732
static struct _tuple14 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
# 734
struct Cyc_CfFlowInfo_FlowEnv*_tmp11F=env->fenv;
# 738
while(1){
union Cyc_CfFlowInfo_FlowInfo*_tmp121;struct _tuple17 _tmp120=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp121=_tmp120.f2;
inflow=*_tmp121;{
void*_tmp122=s->r;struct Cyc_Absyn_Stmt*_tmp124;struct Cyc_Absyn_Stmt*_tmp125;struct Cyc_Absyn_Tvar*_tmp128;struct Cyc_Absyn_Vardecl*_tmp129;struct Cyc_Absyn_Stmt*_tmp12A;struct Cyc_Absyn_Decl*_tmp12C;struct Cyc_Absyn_Stmt*_tmp12D;struct Cyc_Absyn_Exp*_tmp12F;_LLF0: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp123=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp123->tag != 2)goto _LLF2;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;}}_LLF1:
# 743
 inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp124);
s=_tmp125;
goto _LLEF;_LLF2: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp126=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp126->tag != 12)goto _LLF4;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp127=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp126->f1)->r;if(_tmp127->tag != 5)goto _LLF4;else{_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;}}_tmp12A=_tmp126->f2;}}_LLF3: {
# 749
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp129->initializer);
union Cyc_CfFlowInfo_FlowInfo _tmp131;union Cyc_CfFlowInfo_AbsLVal _tmp132;struct _tuple15 _tmp130=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp131=_tmp130.f1;_tmp132=_tmp130.f2;{
struct _tuple15 _tmp75A;struct _tuple15 _tmp134=(_tmp75A.f1=_tmp131,((_tmp75A.f2=_tmp132,_tmp75A)));struct Cyc_Dict_Dict _tmp135;struct Cyc_List_List*_tmp136;struct Cyc_CfFlowInfo_Place*_tmp137;_LLFB: if(((_tmp134.f1).ReachableFL).tag != 2)goto _LLFD;_tmp135=((struct _tuple13)((_tmp134.f1).ReachableFL).val).f1;_tmp136=((struct _tuple13)((_tmp134.f1).ReachableFL).val).f2;if(((_tmp134.f2).PlaceL).tag != 1)goto _LLFD;_tmp137=(struct Cyc_CfFlowInfo_Place*)((_tmp134.f2).PlaceL).val;_LLFC: {
# 753
void*_tmp138=Cyc_CfFlowInfo_lookup_place(_tmp135,_tmp137);
# 755
void*t=(void*)_check_null(e->topt);
# 762
void*_tmp139=Cyc_CfFlowInfo_make_unique_consumed(_tmp11F,t,e,env->iteration_num,_tmp138);
_tmp135=Cyc_CfFlowInfo_assign_place(_tmp11F,e->loc,_tmp135,env->all_changed,_tmp137,_tmp139);
_tmp131=Cyc_CfFlowInfo_ReachableFL(_tmp135,_tmp136);{
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp11F,_tmp135,_tmp138)){case Cyc_CfFlowInfo_AllIL: _LL101:
 leaf=_tmp11F->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL102:
 leaf=_tmp11F->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL103:
 leaf=_tmp11F->unknown_this;break;}{
# 773
void*_tmp13A=Cyc_CfFlowInfo_typ_to_absrval(_tmp11F,t,0,leaf);
_tmp131=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp131,_tmp138);{
struct Cyc_List_List _tmp75B;struct Cyc_List_List _tmp13B=(_tmp75B.hd=_tmp129,((_tmp75B.tl=0,_tmp75B)));
_tmp131=Cyc_NewControlFlow_add_vars(_tmp11F,_tmp131,& _tmp13B,_tmp11F->unknown_all,s->loc,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp13D;void*_tmp13E;struct _tuple14 _tmp13C=Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,_tmp131,_tmp12A);_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp13F=_tmp13D;struct Cyc_Dict_Dict _tmp140;struct Cyc_List_List*_tmp141;_LL106: if((_tmp13F.ReachableFL).tag != 2)goto _LL108;_tmp140=((struct _tuple13)(_tmp13F.ReachableFL).val).f1;_tmp141=((struct _tuple13)(_tmp13F.ReachableFL).val).f2;_LL107:
# 780
 _tmp140=Cyc_CfFlowInfo_assign_place(_tmp11F,s->loc,_tmp140,env->all_changed,_tmp137,_tmp13A);{
union Cyc_CfFlowInfo_FlowInfo _tmp142=Cyc_CfFlowInfo_ReachableFL(_tmp140,_tmp141);
struct _tuple14 _tmp75C;return(_tmp75C.f1=_tmp142,((_tmp75C.f2=_tmp13E,_tmp75C)));};_LL108:;_LL109: {
struct _tuple14 _tmp75D;return(_tmp75D.f1=_tmp13D,((_tmp75D.f2=_tmp13E,_tmp75D)));}_LL105:;};};};};};}_LLFD: if(((_tmp134.f1).BottomFL).tag != 1)goto _LLFF;_LLFE: {
# 785
struct _tuple14 _tmp75E;return(_tmp75E.f1=_tmp131,((_tmp75E.f2=_tmp11F->unknown_all,_tmp75E)));}_LLFF:;_LL100:
{const char*_tmp761;void*_tmp760;(_tmp760=0,Cyc_Tcutil_terr(e->loc,((_tmp761="bad alias expression--no unique path found",_tag_dyneither(_tmp761,sizeof(char),43))),_tag_dyneither(_tmp760,sizeof(void*),0)));}{
struct _tuple14 _tmp762;return(_tmp762.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp762.f2=_tmp11F->unknown_all,_tmp762)));};_LLFA:;};}_LLF4: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp12B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp12B->tag != 12)goto _LLF6;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;}}_LLF5:
# 790
 inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp12C);
s=_tmp12D;
goto _LLEF;_LLF6: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp12E=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp12E->tag != 1)goto _LLF8;else{_tmp12F=_tmp12E->f1;}}_LLF7:
# 794
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp12F);_LLF8:;_LLF9: {
struct Cyc_Core_Impossible_exn_struct _tmp768;const char*_tmp767;struct Cyc_Core_Impossible_exn_struct*_tmp766;(int)_throw((void*)((_tmp766=_cycalloc(sizeof(*_tmp766)),((_tmp766[0]=((_tmp768.tag=Cyc_Core_Impossible,((_tmp768.f1=((_tmp767="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp767,sizeof(char),33))),_tmp768)))),_tmp766)))));}_LLEF:;};}}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp80A(unsigned int*_tmp809,unsigned int*_tmp808,void***_tmp806,struct Cyc_List_List**_tmp321){for(*_tmp809=0;*_tmp809 < *_tmp808;(*_tmp809)++){
# 1353 "new_control_flow.cyc"
void*_tmp804;(*_tmp806)[*_tmp809]=((_tmp804=(void*)((struct Cyc_List_List*)_check_null(*_tmp321))->hd,((*_tmp321=(*_tmp321)->tl,_tmp804))));}}
# 802 "new_control_flow.cyc"
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 806
struct Cyc_CfFlowInfo_FlowEnv*_tmp14D=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 812
{union Cyc_CfFlowInfo_FlowInfo _tmp14E=inflow;struct Cyc_Dict_Dict _tmp14F;struct Cyc_List_List*_tmp150;_LL10B: if((_tmp14E.BottomFL).tag != 1)goto _LL10D;_LL10C: {
struct _tuple14 _tmp769;return(_tmp769.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp769.f2=_tmp14D->unknown_all,_tmp769)));}_LL10D: if((_tmp14E.ReachableFL).tag != 2)goto _LL10A;_tmp14F=((struct _tuple13)(_tmp14E.ReachableFL).val).f1;_tmp150=((struct _tuple13)(_tmp14E.ReachableFL).val).f2;_LL10E:
 d=_tmp14F;relns=_tmp150;_LL10A:;}
# 821
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 842 "new_control_flow.cyc"
union Cyc_CfFlowInfo_FlowInfo _tmp153;union Cyc_CfFlowInfo_AbsLVal _tmp154;struct _tuple15 _tmp152=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;{
struct _tuple15 _tmp76A;struct _tuple15 _tmp156=(_tmp76A.f1=_tmp153,((_tmp76A.f2=_tmp154,_tmp76A)));struct Cyc_Dict_Dict _tmp157;struct Cyc_List_List*_tmp158;struct Cyc_CfFlowInfo_Place*_tmp159;_LL110: if(((_tmp156.f1).ReachableFL).tag != 2)goto _LL112;_tmp157=((struct _tuple13)((_tmp156.f1).ReachableFL).val).f1;_tmp158=((struct _tuple13)((_tmp156.f1).ReachableFL).val).f2;if(((_tmp156.f2).PlaceL).tag != 1)goto _LL112;_tmp159=(struct Cyc_CfFlowInfo_Place*)((_tmp156.f2).PlaceL).val;_LL111: {
# 845
void*_tmp15A=Cyc_CfFlowInfo_lookup_place(_tmp157,_tmp159);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp15A,& needs_unconsume)){
{const char*_tmp76D;void*_tmp76C;(_tmp76C=0,Cyc_Tcutil_terr(e->loc,((_tmp76D="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp76D,sizeof(char),71))),_tag_dyneither(_tmp76C,sizeof(void*),0)));}{
struct _tuple14 _tmp76E;return(_tmp76E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp76E.f2=_tmp14D->unknown_all,_tmp76E)));};}else{
# 851
if(needs_unconsume){
# 853
struct _tuple14 _tmp76F;return(_tmp76F.f1=_tmp153,((_tmp76F.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp14D,_tmp15A),_tmp76F)));}else{
# 856
void*_tmp15F=Cyc_CfFlowInfo_make_unique_consumed(_tmp14D,(void*)_check_null(e->topt),e,env->iteration_num,_tmp15A);
struct Cyc_Dict_Dict _tmp160=Cyc_CfFlowInfo_assign_place(_tmp14D,e->loc,_tmp157,env->all_changed,_tmp159,_tmp15F);
# 867
struct _tuple14 _tmp770;return(_tmp770.f1=Cyc_CfFlowInfo_ReachableFL(_tmp160,_tmp158),((_tmp770.f2=_tmp15A,_tmp770)));}}}_LL112:;_LL113:
# 869
 goto _LL10F;_LL10F:;};}{
# 872
void*_tmp162=e->r;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Vardecl*_tmp17B;struct Cyc_Absyn_Vardecl*_tmp17E;struct Cyc_Absyn_Vardecl*_tmp181;enum Cyc_Absyn_Primop _tmp183;struct Cyc_List_List*_tmp184;struct Cyc_Absyn_Exp*_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Absyn_Exp*_tmp18C;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_List_List*_tmp194;int _tmp196;struct Cyc_Absyn_Exp*_tmp197;void**_tmp198;struct Cyc_Absyn_Exp*_tmp199;int _tmp19A;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A6;struct _dyneither_ptr*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A9;struct _dyneither_ptr*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AC;struct _dyneither_ptr*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_Datatypedecl*_tmp1BD;struct Cyc_List_List*_tmp1BF;struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*_tmp1C3;enum Cyc_Absyn_AggrKind _tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_Vardecl*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CC;int _tmp1CD;struct Cyc_Absyn_Stmt*_tmp1CF;_LL115: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp163->tag != 13)goto _LL117;else{_tmp164=_tmp163->f2;if(_tmp163->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL117;}}_LL116: {
# 876
union Cyc_CfFlowInfo_FlowInfo _tmp1D7;void*_tmp1D8;struct _tuple14 _tmp1D6=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp164);_tmp1D7=_tmp1D6.f1;_tmp1D8=_tmp1D6.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp1DA;void*_tmp1DB;struct _tuple14 _tmp1D9=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1D7,_tmp164,_tmp1D8);_tmp1DA=_tmp1D9.f1;_tmp1DB=_tmp1D9.f2;{
struct _tuple14 _tmp771;return(_tmp771.f1=_tmp1DA,((_tmp771.f2=_tmp1D8,_tmp771)));};};}_LL117: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp165->tag != 13)goto _LL119;else{_tmp166=_tmp165->f2;}}_LL118:
# 882
 _tmp168=_tmp166;goto _LL11A;_LL119: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp167->tag != 11)goto _LL11B;else{_tmp168=_tmp167->f1;}}_LL11A:
 _tmp16A=_tmp168;goto _LL11C;_LL11B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp169->tag != 12)goto _LL11D;else{_tmp16A=_tmp169->f1;}}_LL11C:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp16A);_LL11D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp16B->tag != 0)goto _LL11F;else{if(((_tmp16B->f1).Null_c).tag != 1)goto _LL11F;}}_LL11E:
# 886
 goto _LL120;_LL11F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp16C->tag != 0)goto _LL121;else{if(((_tmp16C->f1).Int_c).tag != 5)goto _LL121;if(((struct _tuple6)((_tmp16C->f1).Int_c).val).f2 != 0)goto _LL121;}}_LL120: {
struct _tuple14 _tmp772;return(_tmp772.f1=inflow,((_tmp772.f2=_tmp14D->zero,_tmp772)));}_LL121: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp16D->tag != 0)goto _LL123;else{if(((_tmp16D->f1).Int_c).tag != 5)goto _LL123;}}_LL122:
 goto _LL124;_LL123: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp16E->tag != 1)goto _LL125;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp16F=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp16E->f2);if(_tmp16F->tag != 2)goto _LL125;}}_LL124: {
struct _tuple14 _tmp773;return(_tmp773.f1=inflow,((_tmp773.f2=_tmp14D->notzeroall,_tmp773)));}_LL125: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp170->tag != 29)goto _LL127;else{if(_tmp170->f1 != 0)goto _LL127;}}_LL126:
# 891
 goto _LL128;_LL127: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp171->tag != 0)goto _LL129;}_LL128:
 goto _LL12A;_LL129: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp172=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp172->tag != 17)goto _LL12B;}_LL12A:
 goto _LL12C;_LL12B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp173->tag != 16)goto _LL12D;}_LL12C:
 goto _LL12E;_LL12D: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp174->tag != 18)goto _LL12F;}_LL12E:
 goto _LL130;_LL12F: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp175->tag != 31)goto _LL131;}_LL130:
 goto _LL132;_LL131: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp176->tag != 30)goto _LL133;}_LL132: {
struct _tuple14 _tmp774;return(_tmp774.f1=inflow,((_tmp774.f2=_tmp14D->unknown_all,_tmp774)));}_LL133: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp177->tag != 1)goto _LL135;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp178=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp177->f2);if(_tmp178->tag != 1)goto _LL135;}}_LL134: {
# 900
struct _tuple14 _tmp775;return(_tmp775.f1=inflow,((_tmp775.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp775)));}_LL135: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp179->tag != 1)goto _LL137;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp17A=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp179->f2);if(_tmp17A->tag != 3)goto _LL137;else{_tmp17B=_tmp17A->f1;}}}_LL136:
# 903
 _tmp17E=_tmp17B;goto _LL138;_LL137: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp17C->tag != 1)goto _LL139;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp17D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp17C->f2);if(_tmp17D->tag != 4)goto _LL139;else{_tmp17E=_tmp17D->f1;}}}_LL138:
 _tmp181=_tmp17E;goto _LL13A;_LL139: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp17F->tag != 1)goto _LL13B;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp180=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp17F->f2);if(_tmp180->tag != 5)goto _LL13B;else{_tmp181=_tmp180->f1;}}}_LL13A: {
# 907
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp776;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp776.tag=0,((_tmp776.f1=_tmp181,_tmp776)));
struct _tuple14 _tmp777;return(_tmp777.f1=inflow,((_tmp777.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp777)));}_LL13B: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp182->tag != 2)goto _LL13D;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LL13C: {
# 914
union Cyc_CfFlowInfo_FlowInfo _tmp1E4;void*_tmp1E5;struct _tuple14 _tmp1E3=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp184);_tmp1E4=_tmp1E3.f1;_tmp1E5=_tmp1E3.f2;{
struct _tuple14 _tmp778;return(_tmp778.f1=_tmp1E4,((_tmp778.f2=_tmp1E5,_tmp778)));};}_LL13D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp185->tag != 4)goto _LL13F;else{_tmp186=_tmp185->f1;}}_LL13E: {
# 918
struct Cyc_List_List _tmp779;struct Cyc_List_List _tmp1E7=(_tmp779.hd=_tmp186,((_tmp779.tl=0,_tmp779)));
union Cyc_CfFlowInfo_FlowInfo _tmp1E9;struct _tuple14 _tmp1E8=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1E7);_tmp1E9=_tmp1E8.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1EB;struct _tuple15 _tmp1EA=Cyc_NewControlFlow_anal_Lexp(env,_tmp1E9,0,_tmp186);_tmp1EB=_tmp1EA.f2;
{struct _tuple21 _tmp77A;struct _tuple21 _tmp1ED=(_tmp77A.f1=_tmp1EB,((_tmp77A.f2=_tmp1E9,_tmp77A)));struct Cyc_CfFlowInfo_Place*_tmp1EE;struct Cyc_Dict_Dict _tmp1EF;struct Cyc_List_List*_tmp1F0;_LL17C: if(((_tmp1ED.f1).PlaceL).tag != 1)goto _LL17E;_tmp1EE=(struct Cyc_CfFlowInfo_Place*)((_tmp1ED.f1).PlaceL).val;if(((_tmp1ED.f2).ReachableFL).tag != 2)goto _LL17E;_tmp1EF=((struct _tuple13)((_tmp1ED.f2).ReachableFL).val).f1;_tmp1F0=((struct _tuple13)((_tmp1ED.f2).ReachableFL).val).f2;_LL17D:
# 923
 _tmp1F0=Cyc_CfFlowInfo_reln_kill_exp(_tmp14D->r,_tmp1F0,_tmp186);
_tmp1E9=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp14D,_tmp186->loc,_tmp1EF,env->all_changed,_tmp1EE,_tmp14D->unknown_all),_tmp1F0);
# 927
goto _LL17B;_LL17E:;_LL17F:
 goto _LL17B;_LL17B:;}{
# 930
struct _tuple14 _tmp77B;return(_tmp77B.f1=_tmp1E9,((_tmp77B.f2=_tmp14D->unknown_all,_tmp77B)));};};}_LL13F: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp187->tag != 3)goto _LL141;else{_tmp188=_tmp187->f1;if(_tmp187->f2 == 0)goto _LL141;_tmp189=_tmp187->f3;}}_LL140:
# 933
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp77E;void*_tmp77D;(_tmp77D=0,Cyc_Tcutil_terr(e->loc,((_tmp77E="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp77E,sizeof(char),58))),_tag_dyneither(_tmp77D,sizeof(void*),0)));}{
struct _tuple14 _tmp77F;return(_tmp77F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp77F.f2=_tmp14D->unknown_all,_tmp77F)));};}{
# 937
struct Cyc_List_List _tmp780;struct Cyc_List_List _tmp1F6=(_tmp780.hd=_tmp189,((_tmp780.tl=0,_tmp780)));
struct Cyc_List_List _tmp781;struct Cyc_List_List _tmp1F7=(_tmp781.hd=_tmp188,((_tmp781.tl=& _tmp1F6,_tmp781)));
union Cyc_CfFlowInfo_FlowInfo _tmp1F9;struct _tuple14 _tmp1F8=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1F7);_tmp1F9=_tmp1F8.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1FB;struct _tuple15 _tmp1FA=Cyc_NewControlFlow_anal_Lexp(env,_tmp1F9,0,_tmp188);_tmp1FB=_tmp1FA.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp1FC=_tmp1F9;struct Cyc_Dict_Dict _tmp1FD;struct Cyc_List_List*_tmp1FE;_LL181: if((_tmp1FC.ReachableFL).tag != 2)goto _LL183;_tmp1FD=((struct _tuple13)(_tmp1FC.ReachableFL).val).f1;_tmp1FE=((struct _tuple13)(_tmp1FC.ReachableFL).val).f2;_LL182:
# 943
{union Cyc_CfFlowInfo_AbsLVal _tmp1FF=_tmp1FB;struct Cyc_CfFlowInfo_Place*_tmp200;_LL186: if((_tmp1FF.PlaceL).tag != 1)goto _LL188;_tmp200=(struct Cyc_CfFlowInfo_Place*)(_tmp1FF.PlaceL).val;_LL187:
# 947
 _tmp1FE=Cyc_CfFlowInfo_reln_kill_exp(_tmp14D->r,_tmp1FE,_tmp188);
_tmp1FD=Cyc_CfFlowInfo_assign_place(_tmp14D,_tmp188->loc,_tmp1FD,env->all_changed,_tmp200,_tmp14D->unknown_all);
# 950
_tmp1F9=Cyc_CfFlowInfo_ReachableFL(_tmp1FD,_tmp1FE);
# 953
goto _LL185;_LL188: if((_tmp1FF.UnknownL).tag != 2)goto _LL185;_LL189:
# 956
 goto _LL185;_LL185:;}
# 958
goto _LL180;_LL183:;_LL184:
 goto _LL180;_LL180:;}{
# 961
struct _tuple14 _tmp782;return(_tmp782.f1=_tmp1F9,((_tmp782.f2=_tmp14D->unknown_all,_tmp782)));};};};_LL141: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp18A->tag != 3)goto _LL143;else{_tmp18B=_tmp18A->f1;if(_tmp18A->f2 != 0)goto _LL143;_tmp18C=_tmp18A->f3;}}_LL142:
# 965
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp785;void*_tmp784;(_tmp784=0,Cyc_Tcutil_terr(e->loc,((_tmp785="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp785,sizeof(char),58))),_tag_dyneither(_tmp784,sizeof(void*),0)));}{
struct _tuple14 _tmp786;return(_tmp786.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp786.f2=_tmp14D->unknown_all,_tmp786)));};}{
# 969
struct Cyc_Dict_Dict*_tmp207=env->all_changed;
while(1){
{struct Cyc_Dict_Dict*_tmp787;env->all_changed=((_tmp787=_region_malloc(env->r,sizeof(*_tmp787)),((_tmp787[0]=_tmp14D->mt_place_set,_tmp787))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp20A;union Cyc_CfFlowInfo_AbsLVal _tmp20B;struct _tuple15 _tmp209=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp18B);_tmp20A=_tmp209.f1;_tmp20B=_tmp209.f2;{
struct Cyc_Dict_Dict _tmp20C=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp788;env->all_changed=((_tmp788=_region_malloc(env->r,sizeof(*_tmp788)),((_tmp788[0]=_tmp14D->mt_place_set,_tmp788))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp20F;void*_tmp210;struct _tuple14 _tmp20E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp18C);_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;{
struct Cyc_Dict_Dict _tmp211=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp212=Cyc_CfFlowInfo_after_flow(_tmp14D,& _tmp20C,_tmp20A,_tmp20F,_tmp20C,_tmp211);
# 980
union Cyc_CfFlowInfo_FlowInfo _tmp213=Cyc_CfFlowInfo_join_flow(_tmp14D,_tmp207,inflow,_tmp212);
# 983
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp213,inflow)){
if(_tmp207 == 0)
env->all_changed=0;else{
# 987
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp207,
Cyc_CfFlowInfo_union_place_set(_tmp20C,_tmp211,0),0);}
# 990
return Cyc_NewControlFlow_do_assign(_tmp14D,env,_tmp212,_tmp18B,_tmp20B,_tmp18C,_tmp210,e->loc);}
# 993
inflow=_tmp213;};};};};}};_LL143: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp18D=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp18D->tag != 8)goto _LL145;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;}}_LL144: {
# 998
union Cyc_CfFlowInfo_FlowInfo _tmp215;void*_tmp216;struct _tuple14 _tmp214=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp18E);_tmp215=_tmp214.f1;_tmp216=_tmp214.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp215,_tmp18F);}_LL145: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp190->tag != 10)goto _LL147;else{_tmp191=_tmp190->f1;}}_LL146: {
# 1002
union Cyc_CfFlowInfo_FlowInfo _tmp218;void*_tmp219;struct _tuple14 _tmp217=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp191);_tmp218=_tmp217.f1;_tmp219=_tmp217.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp191->loc,_tmp218,_tmp219);{
struct _tuple14 _tmp789;return(_tmp789.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp789.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp789)));};}_LL147: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp192=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp192->tag != 9)goto _LL149;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;}}_LL148: {
# 1007
struct _RegionHandle _tmp21B=_new_region("temp");struct _RegionHandle*temp=& _tmp21B;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp21E;struct Cyc_List_List*_tmp21F;struct Cyc_List_List*_tmp78A;struct _tuple20 _tmp21D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp78A=_region_malloc(temp,sizeof(*_tmp78A)),((_tmp78A->hd=_tmp193,((_tmp78A->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp194),_tmp78A)))))),0,1);_tmp21E=_tmp21D.f1;_tmp21F=_tmp21D.f2;{
# 1010
union Cyc_CfFlowInfo_FlowInfo _tmp220=Cyc_NewControlFlow_use_Rval(env,_tmp193->loc,_tmp21E,(void*)((struct Cyc_List_List*)_check_null(_tmp21F))->hd);
_tmp21F=_tmp21F->tl;{
# 1013
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp221=Cyc_Tcutil_compress((void*)_check_null(_tmp193->topt));void*_tmp223;_LL18B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp221;if(_tmp222->tag != 5)goto _LL18D;else{_tmp223=(_tmp222->f1).elt_typ;}}_LL18C:
# 1018
{void*_tmp224=Cyc_Tcutil_compress(_tmp223);struct Cyc_List_List*_tmp226;_LL190: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp224;if(_tmp225->tag != 9)goto _LL192;else{_tmp226=(_tmp225->f1).attributes;}}_LL191:
# 1020
 for(0;_tmp226 != 0;_tmp226=_tmp226->tl){
# 1022
void*_tmp227=(void*)_tmp226->hd;int _tmp229;int _tmp22B;int _tmp22D;_LL195: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp228=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp227;if(_tmp228->tag != 20)goto _LL197;else{_tmp229=_tmp228->f1;}}_LL196:
# 1024
{struct Cyc_List_List*_tmp78B;init_params=((_tmp78B=_region_malloc(temp,sizeof(*_tmp78B)),((_tmp78B->hd=(void*)_tmp229,((_tmp78B->tl=init_params,_tmp78B))))));}goto _LL194;_LL197: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp22A=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp227;if(_tmp22A->tag != 21)goto _LL199;else{_tmp22B=_tmp22A->f1;}}_LL198:
# 1026
{struct Cyc_List_List*_tmp78C;nolive_unique_params=((_tmp78C=_region_malloc(temp,sizeof(*_tmp78C)),((_tmp78C->hd=(void*)_tmp22B,((_tmp78C->tl=nolive_unique_params,_tmp78C))))));}
goto _LL194;_LL199: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp22C=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp227;if(_tmp22C->tag != 22)goto _LL19B;else{_tmp22D=_tmp22C->f1;}}_LL19A:
# 1030
{struct Cyc_List_List*_tmp78D;noconsume_params=((_tmp78D=_region_malloc(temp,sizeof(*_tmp78D)),((_tmp78D->hd=(void*)_tmp22D,((_tmp78D->tl=noconsume_params,_tmp78D))))));}
goto _LL194;_LL19B:;_LL19C:
 goto _LL194;_LL194:;}
# 1034
goto _LL18F;_LL192:;_LL193: {
const char*_tmp790;void*_tmp78F;(_tmp78F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp790="anal_Rexp: bad function type",_tag_dyneither(_tmp790,sizeof(char),29))),_tag_dyneither(_tmp78F,sizeof(void*),0)));}_LL18F:;}
# 1037
goto _LL18A;_LL18D:;_LL18E: {
const char*_tmp793;void*_tmp792;(_tmp792=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp793="anal_Rexp: bad function type",_tag_dyneither(_tmp793,sizeof(char),29))),_tag_dyneither(_tmp792,sizeof(void*),0)));}_LL18A:;}
# 1040
{int i=1;for(0;_tmp21F != 0;(((_tmp21F=_tmp21F->tl,_tmp194=((struct Cyc_List_List*)_check_null(_tmp194))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp235=_tmp21E;struct Cyc_Dict_Dict _tmp236;_LL19E: if((_tmp235.BottomFL).tag != 1)goto _LL1A0;_LL19F:
 goto _LL19D;_LL1A0: if((_tmp235.ReachableFL).tag != 2)goto _LL19D;_tmp236=((struct _tuple13)(_tmp235.ReachableFL).val).f1;_LL1A1:
# 1045
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp236,(void*)_tmp21F->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp796;void*_tmp795;(_tmp795=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp194))->hd)->loc,((_tmp796="expression may not be initialized",_tag_dyneither(_tmp796,sizeof(char),34))),_tag_dyneither(_tmp795,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp239=_tmp220;struct Cyc_Dict_Dict _tmp23A;struct Cyc_List_List*_tmp23B;_LL1A3: if((_tmp239.BottomFL).tag != 1)goto _LL1A5;_LL1A4:
 goto _LL1A2;_LL1A5: if((_tmp239.ReachableFL).tag != 2)goto _LL1A2;_tmp23A=((struct _tuple13)(_tmp239.ReachableFL).val).f1;_tmp23B=((struct _tuple13)(_tmp239.ReachableFL).val).f2;_LL1A6: {
# 1052
struct Cyc_Dict_Dict _tmp23C=Cyc_CfFlowInfo_escape_deref(_tmp14D,_tmp23A,env->all_changed,(void*)_tmp21F->hd);
{void*_tmp23D=(void*)_tmp21F->hd;struct Cyc_CfFlowInfo_Place*_tmp23F;_LL1A8: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp23E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp23D;if(_tmp23E->tag != 5)goto _LL1AA;else{_tmp23F=_tmp23E->f1;}}_LL1A9:
# 1055
{void*_tmp240=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp194))->hd)->topt));void*_tmp242;_LL1AD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp241=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp240;if(_tmp241->tag != 5)goto _LL1AF;else{_tmp242=(_tmp241->f1).elt_typ;}}_LL1AE:
# 1059
 _tmp23C=Cyc_CfFlowInfo_assign_place(_tmp14D,((struct Cyc_Absyn_Exp*)_tmp194->hd)->loc,_tmp23C,env->all_changed,_tmp23F,
# 1061
Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,_tmp242,0,_tmp14D->esc_all));
# 1063
goto _LL1AC;_LL1AF:;_LL1B0: {
const char*_tmp799;void*_tmp798;(_tmp798=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp799="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp799,sizeof(char),39))),_tag_dyneither(_tmp798,sizeof(void*),0)));}_LL1AC:;}
# 1066
goto _LL1A7;_LL1AA:;_LL1AB:
 goto _LL1A7;_LL1A7:;}
# 1069
_tmp220=Cyc_CfFlowInfo_ReachableFL(_tmp23C,_tmp23B);
goto _LL1A2;}_LL1A2:;}
# 1072
goto _LL19D;_LL19D:;}else{
# 1075
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1078
_tmp220=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp194))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp194))->hd)->topt),_tmp220,(void*)_tmp21F->hd);else{
# 1085
_tmp220=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp194))->hd)->loc,_tmp220,(void*)_tmp21F->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i)){
# 1088
union Cyc_CfFlowInfo_AbsLVal _tmp246;struct _tuple15 _tmp245=Cyc_NewControlFlow_anal_Lexp(env,_tmp220,1,(struct Cyc_Absyn_Exp*)_tmp194->hd);_tmp246=_tmp245.f2;{
struct _tuple15 _tmp79A;struct _tuple15 _tmp248=(_tmp79A.f1=_tmp220,((_tmp79A.f2=_tmp246,_tmp79A)));struct Cyc_Dict_Dict _tmp249;struct Cyc_List_List*_tmp24A;struct Cyc_CfFlowInfo_Place*_tmp24B;_LL1B2: if(((_tmp248.f1).ReachableFL).tag != 2)goto _LL1B4;_tmp249=((struct _tuple13)((_tmp248.f1).ReachableFL).val).f1;_tmp24A=((struct _tuple13)((_tmp248.f1).ReachableFL).val).f2;if(((_tmp248.f2).PlaceL).tag != 1)goto _LL1B4;_tmp24B=(struct Cyc_CfFlowInfo_Place*)((_tmp248.f2).PlaceL).val;_LL1B3: {
# 1091
void*_tmp24C=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp14D,(void*)_tmp21F->hd);
# 1094
_tmp249=Cyc_CfFlowInfo_assign_place(_tmp14D,((struct Cyc_Absyn_Exp*)_tmp194->hd)->loc,_tmp249,env->all_changed,_tmp24B,_tmp24C);
_tmp220=Cyc_CfFlowInfo_ReachableFL(_tmp249,_tmp24A);
# 1099
goto _LL1B1;}_LL1B4: if(((_tmp248.f1).BottomFL).tag != 1)goto _LL1B6;_LL1B5:
 continue;_LL1B6:;_LL1B7: {
# 1102
const char*_tmp79D;void*_tmp79C;(_tmp79C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp194->hd)->loc,((_tmp79D="noconsume() parameters must be l-values",_tag_dyneither(_tmp79D,sizeof(char),40))),_tag_dyneither(_tmp79C,sizeof(void*),0)));}_LL1B1:;};}}}}}
# 1109
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp193->topt))){
struct _tuple14 _tmp79E;struct _tuple14 _tmp250=(_tmp79E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp79E.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp79E)));_npop_handler(0);return _tmp250;}else{
# 1112
struct _tuple14 _tmp79F;struct _tuple14 _tmp252=(_tmp79F.f1=_tmp220,((_tmp79F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp79F)));_npop_handler(0);return _tmp252;}};};}
# 1008
;_pop_region(temp);}_LL149: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp195->tag != 32)goto _LL14B;else{_tmp196=(_tmp195->f1).is_calloc;_tmp197=(_tmp195->f1).rgn;_tmp198=(_tmp195->f1).elt_type;_tmp199=(_tmp195->f1).num_elts;_tmp19A=(_tmp195->f1).fat_result;}}_LL14A: {
# 1115
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7A2;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7A1;void*root=(void*)((_tmp7A1=_region_malloc(_tmp14D->r,sizeof(*_tmp7A1)),((_tmp7A1[0]=((_tmp7A2.tag=1,((_tmp7A2.f1=_tmp199,((_tmp7A2.f2=(void*)_check_null(e->topt),_tmp7A2)))))),_tmp7A1))));
struct Cyc_CfFlowInfo_Place*_tmp7A3;struct Cyc_CfFlowInfo_Place*place=(_tmp7A3=_region_malloc(_tmp14D->r,sizeof(*_tmp7A3)),((_tmp7A3->root=root,((_tmp7A3->fields=0,_tmp7A3)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7A6;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7A5;void*rval=(void*)((_tmp7A5=_region_malloc(_tmp14D->r,sizeof(*_tmp7A5)),((_tmp7A5[0]=((_tmp7A6.tag=5,((_tmp7A6.f1=place,_tmp7A6)))),_tmp7A5))));
void*place_val;
if(_tmp19A)place_val=_tmp14D->notzeroall;else{
if(_tmp196)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,*((void**)_check_null(_tmp198)),0,_tmp14D->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,*((void**)_check_null(_tmp198)),0,_tmp14D->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp197 != 0){
struct _RegionHandle _tmp253=_new_region("temp");struct _RegionHandle*temp=& _tmp253;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp256;struct Cyc_List_List*_tmp257;struct Cyc_Absyn_Exp*_tmp7A7[2];struct _tuple20 _tmp255=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7A7[1]=_tmp199,((_tmp7A7[0]=_tmp197,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7A7,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;
outflow=_tmp256;}
# 1126
;_pop_region(temp);}else{
# 1131
union Cyc_CfFlowInfo_FlowInfo _tmp259;struct _tuple14 _tmp258=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp199);_tmp259=_tmp258.f1;
outflow=_tmp259;}{
# 1134
union Cyc_CfFlowInfo_FlowInfo _tmp25A=outflow;struct Cyc_Dict_Dict _tmp25B;struct Cyc_List_List*_tmp25C;_LL1B9: if((_tmp25A.BottomFL).tag != 1)goto _LL1BB;_LL1BA: {
struct _tuple14 _tmp7A8;return(_tmp7A8.f1=outflow,((_tmp7A8.f2=rval,_tmp7A8)));}_LL1BB: if((_tmp25A.ReachableFL).tag != 2)goto _LL1B8;_tmp25B=((struct _tuple13)(_tmp25A.ReachableFL).val).f1;_tmp25C=((struct _tuple13)(_tmp25A.ReachableFL).val).f2;_LL1BC: {
# 1137
struct _tuple14 _tmp7A9;return(_tmp7A9.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp25B,root,place_val),_tmp25C),((_tmp7A9.f2=rval,_tmp7A9)));}_LL1B8:;};};}_LL14B: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp19B->tag != 33)goto _LL14D;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;}}_LL14C: {
# 1141
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1147
struct _RegionHandle _tmp264=_new_region("temp");struct _RegionHandle*temp=& _tmp264;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp267;struct Cyc_List_List*_tmp268;struct Cyc_Absyn_Exp*_tmp7AA[2];struct _tuple20 _tmp266=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7AA[1]=_tmp19D,((_tmp7AA[0]=_tmp19C,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7AA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp268))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp268->tl))->hd;
outflow=_tmp267;}{
# 1155
union Cyc_CfFlowInfo_AbsLVal _tmp26A;struct _tuple15 _tmp269=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp19C);_tmp26A=_tmp269.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp26C;struct _tuple15 _tmp26B=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp19D);_tmp26C=_tmp26B.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp26D=outflow;struct Cyc_Dict_Dict _tmp26E;struct Cyc_List_List*_tmp26F;_LL1BE: if((_tmp26D.ReachableFL).tag != 2)goto _LL1C0;_tmp26E=((struct _tuple13)(_tmp26D.ReachableFL).val).f1;_tmp26F=((struct _tuple13)(_tmp26D.ReachableFL).val).f2;_LL1BF:
# 1159
{union Cyc_CfFlowInfo_AbsLVal _tmp270=_tmp26A;struct Cyc_CfFlowInfo_Place*_tmp271;_LL1C3: if((_tmp270.PlaceL).tag != 1)goto _LL1C5;_tmp271=(struct Cyc_CfFlowInfo_Place*)(_tmp270.PlaceL).val;_LL1C4:
# 1161
 _tmp26E=Cyc_CfFlowInfo_assign_place(_tmp14D,_tmp19C->loc,_tmp26E,env->all_changed,_tmp271,right_rval);
# 1163
goto _LL1C2;_LL1C5: if((_tmp270.UnknownL).tag != 2)goto _LL1C2;_LL1C6:
 goto _LL1C2;_LL1C2:;}
# 1166
{union Cyc_CfFlowInfo_AbsLVal _tmp272=_tmp26C;struct Cyc_CfFlowInfo_Place*_tmp273;_LL1C8: if((_tmp272.PlaceL).tag != 1)goto _LL1CA;_tmp273=(struct Cyc_CfFlowInfo_Place*)(_tmp272.PlaceL).val;_LL1C9:
# 1168
 _tmp26E=Cyc_CfFlowInfo_assign_place(_tmp14D,_tmp19D->loc,_tmp26E,env->all_changed,_tmp273,left_rval);
# 1170
goto _LL1C7;_LL1CA: if((_tmp272.UnknownL).tag != 2)goto _LL1C7;_LL1CB:
 goto _LL1C7;_LL1C7:;}
# 1174
_tmp26F=Cyc_CfFlowInfo_reln_kill_exp(_tmp14D->r,_tmp26F,_tmp19C);
_tmp26F=Cyc_CfFlowInfo_reln_kill_exp(_tmp14D->r,_tmp26F,_tmp19D);
# 1177
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp26E,_tmp26F);
goto _LL1BD;_LL1C0:;_LL1C1:
 goto _LL1BD;_LL1BD:;}{
# 1183
struct _tuple14 _tmp7AB;struct _tuple14 _tmp275=(_tmp7AB.f1=outflow,((_tmp7AB.f2=_tmp14D->unknown_all,_tmp7AB)));_npop_handler(0);return _tmp275;};};};
# 1147
;_pop_region(temp);}_LL14D: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp19E->tag != 15)goto _LL14F;else{_tmp19F=_tmp19E->f1;_tmp1A0=_tmp19E->f2;}}_LL14E: {
# 1186
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7AE;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7AD;void*root=(void*)((_tmp7AD=_region_malloc(_tmp14D->r,sizeof(*_tmp7AD)),((_tmp7AD[0]=((_tmp7AE.tag=1,((_tmp7AE.f1=_tmp1A0,((_tmp7AE.f2=(void*)_check_null(e->topt),_tmp7AE)))))),_tmp7AD))));
struct Cyc_CfFlowInfo_Place*_tmp7AF;struct Cyc_CfFlowInfo_Place*place=(_tmp7AF=_region_malloc(_tmp14D->r,sizeof(*_tmp7AF)),((_tmp7AF->root=root,((_tmp7AF->fields=0,_tmp7AF)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7B2;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7B1;void*rval=(void*)((_tmp7B1=_region_malloc(_tmp14D->r,sizeof(*_tmp7B1)),((_tmp7B1[0]=((_tmp7B2.tag=5,((_tmp7B2.f1=place,_tmp7B2)))),_tmp7B1))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp19F != 0){
struct _RegionHandle _tmp276=_new_region("temp");struct _RegionHandle*temp=& _tmp276;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_Absyn_Exp*_tmp7B3[2];struct _tuple20 _tmp278=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7B3[1]=_tmp1A0,((_tmp7B3[0]=_tmp19F,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7B3,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp279=_tmp278.f1;_tmp27A=_tmp278.f2;
outflow=_tmp279;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp27A))->tl))->hd;}
# 1194
;_pop_region(temp);}else{
# 1200
union Cyc_CfFlowInfo_FlowInfo _tmp27C;void*_tmp27D;struct _tuple14 _tmp27B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A0);_tmp27C=_tmp27B.f1;_tmp27D=_tmp27B.f2;
outflow=_tmp27C;
place_val=_tmp27D;}{
# 1204
union Cyc_CfFlowInfo_FlowInfo _tmp27E=outflow;struct Cyc_Dict_Dict _tmp27F;struct Cyc_List_List*_tmp280;_LL1CD: if((_tmp27E.BottomFL).tag != 1)goto _LL1CF;_LL1CE: {
struct _tuple14 _tmp7B4;return(_tmp7B4.f1=outflow,((_tmp7B4.f2=rval,_tmp7B4)));}_LL1CF: if((_tmp27E.ReachableFL).tag != 2)goto _LL1CC;_tmp27F=((struct _tuple13)(_tmp27E.ReachableFL).val).f1;_tmp280=((struct _tuple13)(_tmp27E.ReachableFL).val).f2;_LL1D0: {
# 1207
struct _tuple14 _tmp7B5;return(_tmp7B5.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27F,root,place_val),_tmp280),((_tmp7B5.f2=rval,_tmp7B5)));}_LL1CC:;};};}_LL14F: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1A1->tag != 14)goto _LL151;else{_tmp1A2=_tmp1A1->f1;}}_LL150: {
# 1211
union Cyc_CfFlowInfo_FlowInfo _tmp289;union Cyc_CfFlowInfo_AbsLVal _tmp28A;struct _tuple15 _tmp288=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1A2);_tmp289=_tmp288.f1;_tmp28A=_tmp288.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp28B=_tmp28A;struct Cyc_CfFlowInfo_Place*_tmp28C;_LL1D2: if((_tmp28B.UnknownL).tag != 2)goto _LL1D4;_LL1D3: {
struct _tuple14 _tmp7B6;return(_tmp7B6.f1=_tmp289,((_tmp7B6.f2=_tmp14D->notzeroall,_tmp7B6)));}_LL1D4: if((_tmp28B.PlaceL).tag != 1)goto _LL1D1;_tmp28C=(struct Cyc_CfFlowInfo_Place*)(_tmp28B.PlaceL).val;_LL1D5: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7BC;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7BB;struct _tuple14 _tmp7BA;return(_tmp7BA.f1=_tmp289,((_tmp7BA.f2=(void*)((_tmp7BC=_region_malloc(env->r,sizeof(*_tmp7BC)),((_tmp7BC[0]=((_tmp7BB.tag=5,((_tmp7BB.f1=_tmp28C,_tmp7BB)))),_tmp7BC)))),_tmp7BA)));}_LL1D1:;};}_LL151: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1A3->tag != 19)goto _LL153;else{_tmp1A4=_tmp1A3->f1;}}_LL152: {
# 1218
union Cyc_CfFlowInfo_FlowInfo _tmp292;void*_tmp293;struct _tuple14 _tmp291=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A4);_tmp292=_tmp291.f1;_tmp293=_tmp291.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp292,_tmp1A4,_tmp293);}_LL153: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1A5->tag != 20)goto _LL155;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}}_LL154: {
# 1223
union Cyc_CfFlowInfo_FlowInfo _tmp295;void*_tmp296;struct _tuple14 _tmp294=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A6);_tmp295=_tmp294.f1;_tmp296=_tmp294.f2;
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp1A6->topt))){
struct _tuple14 _tmp7BD;return(_tmp7BD.f1=_tmp295,((_tmp7BD.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp7BD)));}{
void*_tmp298=_tmp296;int _tmp29B;int _tmp29C;struct _dyneither_ptr _tmp29D;int _tmp29F;int _tmp2A0;struct _dyneither_ptr _tmp2A1;_LL1D7: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp299=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp298;if(_tmp299->tag != 8)goto _LL1D9;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp29A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp299->f2);if(_tmp29A->tag != 6)goto _LL1D9;else{_tmp29B=(_tmp29A->f1).is_union;_tmp29C=(_tmp29A->f1).fieldnum;_tmp29D=_tmp29A->f2;}}}_LL1D8:
# 1228
 _tmp29F=_tmp29B;_tmp2A0=_tmp29C;_tmp2A1=_tmp29D;goto _LL1DA;_LL1D9: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp29E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp298;if(_tmp29E->tag != 6)goto _LL1DB;else{_tmp29F=(_tmp29E->f1).is_union;_tmp2A0=(_tmp29E->f1).fieldnum;_tmp2A1=_tmp29E->f2;}}_LL1DA: {
# 1230
int _tmp2A2=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1A6->topt),_tmp1A7);
if((_tmp29F  && _tmp2A0 != - 1) && _tmp2A0 != _tmp2A2){
struct _tuple14 _tmp7BE;return(_tmp7BE.f1=_tmp295,((_tmp7BE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),1,_tmp14D->unknown_none),_tmp7BE)));}{
struct _tuple14 _tmp7BF;return(_tmp7BF.f1=_tmp295,((_tmp7BF.f2=*((void**)_check_dyneither_subscript(_tmp2A1,sizeof(void*),_tmp2A2)),_tmp7BF)));};}_LL1DB:;_LL1DC: {
# 1235
struct Cyc_Core_Impossible_exn_struct _tmp7CC;const char*_tmp7CB;void*_tmp7CA[1];struct Cyc_String_pa_PrintArg_struct _tmp7C9;struct Cyc_Core_Impossible_exn_struct*_tmp7C8;(int)_throw((void*)((_tmp7C8=_cycalloc(sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7CC.tag=Cyc_Core_Impossible,((_tmp7CC.f1=(struct _dyneither_ptr)((_tmp7C9.tag=0,((_tmp7C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7CA[0]=& _tmp7C9,Cyc_aprintf(((_tmp7CB="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp7CB,sizeof(char),26))),_tag_dyneither(_tmp7CA,sizeof(void*),1)))))))),_tmp7CC)))),_tmp7C8)))));}_LL1D6:;};}_LL155: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1A8->tag != 36)goto _LL157;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL156: {
# 1242
union Cyc_CfFlowInfo_FlowInfo _tmp2AB;void*_tmp2AC;struct _tuple14 _tmp2AA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A9);_tmp2AB=_tmp2AA.f1;_tmp2AC=_tmp2AA.f2;
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp1A9->topt))){
struct _tuple14 _tmp7CD;return(_tmp7CD.f1=_tmp2AB,((_tmp7CD.f2=_tmp14D->unknown_all,_tmp7CD)));}{
void*_tmp2AE=_tmp2AC;int _tmp2B1;int _tmp2B2;struct _dyneither_ptr _tmp2B3;int _tmp2B5;int _tmp2B6;struct _dyneither_ptr _tmp2B7;_LL1DE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2AF=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2AE;if(_tmp2AF->tag != 8)goto _LL1E0;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B0=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2AF->f2);if(_tmp2B0->tag != 6)goto _LL1E0;else{_tmp2B1=(_tmp2B0->f1).is_union;_tmp2B2=(_tmp2B0->f1).fieldnum;_tmp2B3=_tmp2B0->f2;}}}_LL1DF:
# 1247
 _tmp2B5=_tmp2B1;_tmp2B6=_tmp2B2;_tmp2B7=_tmp2B3;goto _LL1E1;_LL1E0: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AE;if(_tmp2B4->tag != 6)goto _LL1E2;else{_tmp2B5=(_tmp2B4->f1).is_union;_tmp2B6=(_tmp2B4->f1).fieldnum;_tmp2B7=_tmp2B4->f2;}}_LL1E1: {
# 1249
int _tmp2B8=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1A9->topt),_tmp1AA);
if(_tmp2B5  && _tmp2B6 != - 1){
if(_tmp2B6 != _tmp2B8){
struct _tuple14 _tmp7CE;return(_tmp7CE.f1=_tmp2AB,((_tmp7CE.f2=_tmp14D->zero,_tmp7CE)));}else{
# 1254
struct _tuple14 _tmp7CF;return(_tmp7CF.f1=_tmp2AB,((_tmp7CF.f2=_tmp14D->notzeroall,_tmp7CF)));}}else{
# 1256
struct _tuple14 _tmp7D0;return(_tmp7D0.f1=_tmp2AB,((_tmp7D0.f2=_tmp14D->unknown_all,_tmp7D0)));}}_LL1E2:;_LL1E3: {
# 1258
struct Cyc_Core_Impossible_exn_struct _tmp7DD;const char*_tmp7DC;void*_tmp7DB[1];struct Cyc_String_pa_PrintArg_struct _tmp7DA;struct Cyc_Core_Impossible_exn_struct*_tmp7D9;(int)_throw((void*)((_tmp7D9=_cycalloc(sizeof(*_tmp7D9)),((_tmp7D9[0]=((_tmp7DD.tag=Cyc_Core_Impossible,((_tmp7DD.f1=(struct _dyneither_ptr)((_tmp7DA.tag=0,((_tmp7DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7DB[0]=& _tmp7DA,Cyc_aprintf(((_tmp7DC="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp7DC,sizeof(char),26))),_tag_dyneither(_tmp7DB,sizeof(void*),1)))))))),_tmp7DD)))),_tmp7D9)))));}_LL1DD:;};}_LL157: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1AB=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1AB->tag != 21)goto _LL159;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}_LL158: {
# 1263
union Cyc_CfFlowInfo_FlowInfo _tmp2C2;void*_tmp2C3;struct _tuple14 _tmp2C1=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AC);_tmp2C2=_tmp2C1.f1;_tmp2C3=_tmp2C1.f2;{
# 1266
union Cyc_CfFlowInfo_FlowInfo _tmp2C5;void*_tmp2C6;struct _tuple14 _tmp2C4=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C2,_tmp1AC,_tmp2C3);_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;{
# 1269
void*_tmp2C7=Cyc_Tcutil_compress((void*)_check_null(_tmp1AC->topt));void*_tmp2C9;_LL1E5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2C8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7;if(_tmp2C8->tag != 5)goto _LL1E7;else{_tmp2C9=(_tmp2C8->f1).elt_typ;}}_LL1E6:
# 1271
 if(Cyc_Absyn_is_nontagged_union_type(_tmp2C9)){
struct _tuple14 _tmp7DE;return(_tmp7DE.f1=_tmp2C5,((_tmp7DE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp7DE)));}{
void*_tmp2CB=_tmp2C6;int _tmp2CE;int _tmp2CF;struct _dyneither_ptr _tmp2D0;int _tmp2D2;int _tmp2D3;struct _dyneither_ptr _tmp2D4;_LL1EA: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2CC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2CB;if(_tmp2CC->tag != 8)goto _LL1EC;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2CD=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2CC->f2);if(_tmp2CD->tag != 6)goto _LL1EC;else{_tmp2CE=(_tmp2CD->f1).is_union;_tmp2CF=(_tmp2CD->f1).fieldnum;_tmp2D0=_tmp2CD->f2;}}}_LL1EB:
# 1275
 _tmp2D2=_tmp2CE;_tmp2D3=_tmp2CF;_tmp2D4=_tmp2D0;goto _LL1ED;_LL1EC: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2D1=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2CB;if(_tmp2D1->tag != 6)goto _LL1EE;else{_tmp2D2=(_tmp2D1->f1).is_union;_tmp2D3=(_tmp2D1->f1).fieldnum;_tmp2D4=_tmp2D1->f2;}}_LL1ED: {
# 1277
int _tmp2D5=Cyc_CfFlowInfo_get_field_index(_tmp2C9,_tmp1AD);
if((_tmp2D2  && _tmp2D3 != - 1) && _tmp2D3 != _tmp2D5){
struct _tuple14 _tmp7DF;return(_tmp7DF.f1=_tmp2C5,((_tmp7DF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),1,_tmp14D->unknown_none),_tmp7DF)));}{
struct _tuple14 _tmp7E0;return(_tmp7E0.f1=_tmp2C5,((_tmp7E0.f2=*((void**)_check_dyneither_subscript(_tmp2D4,sizeof(void*),_tmp2D5)),_tmp7E0)));};}_LL1EE:;_LL1EF: {
struct Cyc_Core_Impossible_exn_struct _tmp7E6;const char*_tmp7E5;struct Cyc_Core_Impossible_exn_struct*_tmp7E4;(int)_throw((void*)((_tmp7E4=_cycalloc(sizeof(*_tmp7E4)),((_tmp7E4[0]=((_tmp7E6.tag=Cyc_Core_Impossible,((_tmp7E6.f1=((_tmp7E5="anal_Rexp: AggrArrow",_tag_dyneither(_tmp7E5,sizeof(char),21))),_tmp7E6)))),_tmp7E4)))));}_LL1E9:;};_LL1E7:;_LL1E8: {
# 1283
struct Cyc_Core_Impossible_exn_struct _tmp7EC;const char*_tmp7EB;struct Cyc_Core_Impossible_exn_struct*_tmp7EA;(int)_throw((void*)((_tmp7EA=_cycalloc(sizeof(*_tmp7EA)),((_tmp7EA[0]=((_tmp7EC.tag=Cyc_Core_Impossible,((_tmp7EC.f1=((_tmp7EB="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7EB,sizeof(char),25))),_tmp7EC)))),_tmp7EA)))));}_LL1E4:;};};}_LL159: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1AE->tag != 5)goto _LL15B;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;_tmp1B1=_tmp1AE->f3;}}_LL15A: {
# 1287
union Cyc_CfFlowInfo_FlowInfo _tmp2DF;union Cyc_CfFlowInfo_FlowInfo _tmp2E0;struct _tuple16 _tmp2DE=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1AF);_tmp2DF=_tmp2DE.f1;_tmp2E0=_tmp2DE.f2;{
struct _tuple14 _tmp2E1=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DF,_tmp1B0);
struct _tuple14 _tmp2E2=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E0,_tmp1B1);
# 1291
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14D,env->all_changed,_tmp2E1,_tmp2E2);};}_LL15B: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1B2=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1B2->tag != 6)goto _LL15D;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}}_LL15C: {
# 1294
union Cyc_CfFlowInfo_FlowInfo _tmp2E4;union Cyc_CfFlowInfo_FlowInfo _tmp2E5;struct _tuple16 _tmp2E3=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B3);_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2E7;void*_tmp2E8;struct _tuple14 _tmp2E6=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E4,_tmp1B4);_tmp2E7=_tmp2E6.f1;_tmp2E8=_tmp2E6.f2;{
struct _tuple14 _tmp7ED;struct _tuple14 _tmp2E9=(_tmp7ED.f1=_tmp2E7,((_tmp7ED.f2=_tmp2E8,_tmp7ED)));
struct _tuple14 _tmp7EE;struct _tuple14 _tmp2EA=(_tmp7EE.f1=_tmp2E5,((_tmp7EE.f2=_tmp14D->zero,_tmp7EE)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14D,env->all_changed,_tmp2E9,_tmp2EA);};};}_LL15D: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1B5=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1B5->tag != 7)goto _LL15F;else{_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B5->f2;}}_LL15E: {
# 1301
union Cyc_CfFlowInfo_FlowInfo _tmp2EE;union Cyc_CfFlowInfo_FlowInfo _tmp2EF;struct _tuple16 _tmp2ED=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B6);_tmp2EE=_tmp2ED.f1;_tmp2EF=_tmp2ED.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2F1;void*_tmp2F2;struct _tuple14 _tmp2F0=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2EF,_tmp1B7);_tmp2F1=_tmp2F0.f1;_tmp2F2=_tmp2F0.f2;{
struct _tuple14 _tmp7EF;struct _tuple14 _tmp2F3=(_tmp7EF.f1=_tmp2F1,((_tmp7EF.f2=_tmp2F2,_tmp7EF)));
struct _tuple14 _tmp7F0;struct _tuple14 _tmp2F4=(_tmp7F0.f1=_tmp2EE,((_tmp7F0.f2=_tmp14D->notzeroall,_tmp7F0)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14D,env->all_changed,_tmp2F3,_tmp2F4);};};}_LL15F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1B8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1B8->tag != 22)goto _LL161;else{_tmp1B9=_tmp1B8->f1;_tmp1BA=_tmp1B8->f2;}}_LL160: {
# 1308
struct _RegionHandle _tmp2F7=_new_region("temp");struct _RegionHandle*temp=& _tmp2F7;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp2FA;struct Cyc_List_List*_tmp2FB;struct Cyc_Absyn_Exp*_tmp7F1[2];struct _tuple20 _tmp2F9=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7F1[1]=_tmp1BA,((_tmp7F1[0]=_tmp1B9,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7F1,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp2FA=_tmp2F9.f1;_tmp2FB=_tmp2F9.f2;{
# 1313
union Cyc_CfFlowInfo_FlowInfo _tmp2FC=_tmp2FA;
{union Cyc_CfFlowInfo_FlowInfo _tmp2FD=_tmp2FA;struct Cyc_Dict_Dict _tmp2FE;struct Cyc_List_List*_tmp2FF;_LL1F1: if((_tmp2FD.ReachableFL).tag != 2)goto _LL1F3;_tmp2FE=((struct _tuple13)(_tmp2FD.ReachableFL).val).f1;_tmp2FF=((struct _tuple13)(_tmp2FD.ReachableFL).val).f2;_LL1F2:
# 1318
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2FE,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2FB))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7F4;void*_tmp7F3;(_tmp7F3=0,Cyc_Tcutil_terr(_tmp1BA->loc,((_tmp7F4="expression may not be initialized",_tag_dyneither(_tmp7F4,sizeof(char),34))),_tag_dyneither(_tmp7F3,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp302=Cyc_NewControlFlow_add_subscript_reln(_tmp14D->r,_tmp2FF,_tmp1B9,_tmp1BA);
if(_tmp2FF != _tmp302)
_tmp2FC=Cyc_CfFlowInfo_ReachableFL(_tmp2FE,_tmp302);
goto _LL1F0;};_LL1F3:;_LL1F4:
 goto _LL1F0;_LL1F0:;}{
# 1326
void*_tmp303=Cyc_Tcutil_compress((void*)_check_null(_tmp1B9->topt));struct Cyc_List_List*_tmp305;union Cyc_Absyn_Constraint*_tmp307;_LL1F6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp304=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp303;if(_tmp304->tag != 10)goto _LL1F8;else{_tmp305=_tmp304->f1;}}_LL1F7: {
# 1328
void*_tmp308=(void*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd;struct _dyneither_ptr _tmp30B;struct _dyneither_ptr _tmp30D;_LL1FD: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp309=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp308;if(_tmp309->tag != 8)goto _LL1FF;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp309->f2);if(_tmp30A->tag != 6)goto _LL1FF;else{_tmp30B=_tmp30A->f2;}}}_LL1FE:
# 1330
 _tmp30D=_tmp30B;goto _LL200;_LL1FF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp308;if(_tmp30C->tag != 6)goto _LL201;else{_tmp30D=_tmp30C->f2;}}_LL200: {
# 1332
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1BA)).f1;
struct _tuple14 _tmp7F5;struct _tuple14 _tmp30F=(_tmp7F5.f1=_tmp2FC,((_tmp7F5.f2=*((void**)_check_dyneither_subscript(_tmp30D,sizeof(void*),(int)i)),_tmp7F5)));_npop_handler(0);return _tmp30F;}_LL201:;_LL202: {
struct Cyc_Core_Impossible_exn_struct _tmp7FB;const char*_tmp7FA;struct Cyc_Core_Impossible_exn_struct*_tmp7F9;(int)_throw((void*)((_tmp7F9=_cycalloc(sizeof(*_tmp7F9)),((_tmp7F9[0]=((_tmp7FB.tag=Cyc_Core_Impossible,((_tmp7FB.f1=((_tmp7FA="anal_Rexp: Subscript",_tag_dyneither(_tmp7FA,sizeof(char),21))),_tmp7FB)))),_tmp7F9)))));}_LL1FC:;}_LL1F8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp306=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp303;if(_tmp306->tag != 5)goto _LL1FA;else{_tmp307=((_tmp306->f1).ptr_atts).bounds;}}_LL1F9: {
# 1337
union Cyc_CfFlowInfo_FlowInfo _tmp314;void*_tmp315;struct _tuple14 _tmp313=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2FA,_tmp1B9,(void*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd);_tmp314=_tmp313.f1;_tmp315=_tmp313.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp316=_tmp314;_LL204: if((_tmp316.BottomFL).tag != 1)goto _LL206;_LL205: {
struct _tuple14 _tmp7FC;struct _tuple14 _tmp318=(_tmp7FC.f1=_tmp314,((_tmp7FC.f2=_tmp315,_tmp7FC)));_npop_handler(0);return _tmp318;}_LL206:;_LL207: {
struct _tuple14 _tmp7FD;struct _tuple14 _tmp31A=(_tmp7FD.f1=_tmp2FC,((_tmp7FD.f2=_tmp315,_tmp7FD)));_npop_handler(0);return _tmp31A;}_LL203:;};}_LL1FA:;_LL1FB: {
# 1342
struct Cyc_Core_Impossible_exn_struct _tmp803;const char*_tmp802;struct Cyc_Core_Impossible_exn_struct*_tmp801;(int)_throw((void*)((_tmp801=_cycalloc(sizeof(*_tmp801)),((_tmp801[0]=((_tmp803.tag=Cyc_Core_Impossible,((_tmp803.f1=((_tmp802="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp802,sizeof(char),33))),_tmp803)))),_tmp801)))));}_LL1F5:;};};}
# 1309
;_pop_region(temp);}_LL161: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1BB=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1BB->tag != 29)goto _LL163;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;}}_LL162:
# 1347
 _tmp1BF=_tmp1BC;goto _LL164;_LL163: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1BE=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1BE->tag != 23)goto _LL165;else{_tmp1BF=_tmp1BE->f1;}}_LL164: {
# 1349
struct _RegionHandle _tmp31E=_new_region("temp");struct _RegionHandle*temp=& _tmp31E;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp320;struct Cyc_List_List*_tmp321;struct _tuple20 _tmp31F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1BF,1,1);_tmp320=_tmp31F.f1;_tmp321=_tmp31F.f2;{
unsigned int _tmp809;unsigned int _tmp808;struct _dyneither_ptr _tmp807;void**_tmp806;unsigned int _tmp805;struct _dyneither_ptr aggrdict=
(_tmp805=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1BF),((_tmp806=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp805)),((_tmp807=_tag_dyneither(_tmp806,sizeof(void*),_tmp805),((((_tmp808=_tmp805,_tmp80A(& _tmp809,& _tmp808,& _tmp806,& _tmp321))),_tmp807)))))));
# 1357
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp814;struct Cyc_CfFlowInfo_UnionRInfo _tmp813;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp812;struct _tuple14 _tmp811;struct _tuple14 _tmp326=(_tmp811.f1=_tmp320,((_tmp811.f2=(void*)((_tmp814=_region_malloc(env->r,sizeof(*_tmp814)),((_tmp814[0]=((_tmp812.tag=6,((_tmp812.f1=((_tmp813.is_union=0,((_tmp813.fieldnum=- 1,_tmp813)))),((_tmp812.f2=aggrdict,_tmp812)))))),_tmp814)))),_tmp811)));_npop_handler(0);return _tmp326;};}
# 1350
;_pop_region(temp);}_LL165: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1C0->tag != 28)goto _LL167;else{_tmp1C1=_tmp1C0->f2;}}_LL166: {
# 1360
struct Cyc_List_List*fs;
{void*_tmp32C=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_List_List*_tmp32E;_LL209: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32C;if(_tmp32D->tag != 12)goto _LL20B;else{_tmp32E=_tmp32D->f2;}}_LL20A:
 fs=_tmp32E;goto _LL208;_LL20B:;_LL20C: {
struct Cyc_Core_Impossible_exn_struct _tmp81A;const char*_tmp819;struct Cyc_Core_Impossible_exn_struct*_tmp818;(int)_throw((void*)((_tmp818=_cycalloc(sizeof(*_tmp818)),((_tmp818[0]=((_tmp81A.tag=Cyc_Core_Impossible,((_tmp81A.f1=((_tmp819="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp819,sizeof(char),35))),_tmp81A)))),_tmp818)))));}_LL208:;}
# 1365
_tmp1C3=_tmp1C1;_tmp1C4=Cyc_Absyn_StructA;_tmp1C5=fs;goto _LL168;}_LL167: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1C2->tag != 27)goto _LL169;else{_tmp1C3=_tmp1C2->f3;if(_tmp1C2->f4 == 0)goto _LL169;_tmp1C4=(_tmp1C2->f4)->kind;if((_tmp1C2->f4)->impl == 0)goto _LL169;_tmp1C5=((_tmp1C2->f4)->impl)->fields;}}_LL168: {
# 1367
struct _RegionHandle _tmp332=_new_region("temp");struct _RegionHandle*temp=& _tmp332;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp334;struct Cyc_List_List*_tmp335;struct _tuple20 _tmp333=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp1C3),1,1);_tmp334=_tmp333.f1;_tmp335=_tmp333.f2;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp14D,_tmp1C5,_tmp1C4 == Cyc_Absyn_UnionA,_tmp1C4 == Cyc_Absyn_UnionA?_tmp14D->unknown_none: _tmp14D->unknown_all);
int field_no=- 1;
{int i=0;for(0;_tmp335 != 0;(((_tmp335=_tmp335->tl,_tmp1C3=_tmp1C3->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp1C3))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp336=(void*)ds->hd;struct _dyneither_ptr*_tmp339;_LL20E: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp337=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp336;if(_tmp337->tag != 0)goto _LL210;}_LL20F: {
struct Cyc_Core_Impossible_exn_struct _tmp820;const char*_tmp81F;struct Cyc_Core_Impossible_exn_struct*_tmp81E;(int)_throw((void*)((_tmp81E=_cycalloc(sizeof(*_tmp81E)),((_tmp81E[0]=((_tmp820.tag=Cyc_Core_Impossible,((_tmp820.f1=((_tmp81F="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp81F,sizeof(char),22))),_tmp820)))),_tmp81E)))));}_LL210: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp338=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp336;if(_tmp338->tag != 1)goto _LL20D;else{_tmp339=_tmp338->f1;}}_LL211:
# 1379
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1C5,_tmp339);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp335->hd;
# 1382
if(_tmp1C4 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp33D=(*((struct _tuple22*)_tmp1C3->hd)).f2;
_tmp334=Cyc_NewControlFlow_use_Rval(env,_tmp33D->loc,_tmp334,(void*)_tmp335->hd);}_LL20D:;}}}{
# 1387
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp82A;struct Cyc_CfFlowInfo_UnionRInfo _tmp829;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp828;struct _tuple14 _tmp827;struct _tuple14 _tmp342=(_tmp827.f1=_tmp334,((_tmp827.f2=(void*)((_tmp82A=_region_malloc(env->r,sizeof(*_tmp82A)),((_tmp82A[0]=((_tmp828.tag=6,((_tmp828.f1=((_tmp829.is_union=_tmp1C4 == Cyc_Absyn_UnionA,((_tmp829.fieldnum=field_no,_tmp829)))),((_tmp828.f2=aggrdict,_tmp828)))))),_tmp82A)))),_tmp827)));_npop_handler(0);return _tmp342;};};}
# 1368
;_pop_region(temp);}_LL169: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1C6=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1C6->tag != 27)goto _LL16B;}_LL16A: {
# 1390
struct Cyc_Core_Impossible_exn_struct _tmp830;const char*_tmp82F;struct Cyc_Core_Impossible_exn_struct*_tmp82E;(int)_throw((void*)((_tmp82E=_cycalloc(sizeof(*_tmp82E)),((_tmp82E[0]=((_tmp830.tag=Cyc_Core_Impossible,((_tmp830.f1=((_tmp82F="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp82F,sizeof(char),31))),_tmp830)))),_tmp82E)))));}_LL16B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1C7=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1C7->tag != 25)goto _LL16D;else{_tmp1C8=_tmp1C7->f1;}}_LL16C: {
# 1392
struct _RegionHandle _tmp346=_new_region("temp");struct _RegionHandle*temp=& _tmp346;_push_region(temp);
{struct Cyc_List_List*_tmp347=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp1C8);
union Cyc_CfFlowInfo_FlowInfo _tmp349;struct Cyc_List_List*_tmp34A;struct _tuple20 _tmp348=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp347,1,1);_tmp349=_tmp348.f1;_tmp34A=_tmp348.f2;
for(0;_tmp34A != 0;(_tmp34A=_tmp34A->tl,_tmp347=_tmp347->tl)){
_tmp349=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp347))->hd)->loc,_tmp349,(void*)_tmp34A->hd);}{
struct _tuple14 _tmp831;struct _tuple14 _tmp34C=(_tmp831.f1=_tmp349,((_tmp831.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp831)));_npop_handler(0);return _tmp34C;};}
# 1393
;_pop_region(temp);}_LL16D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1C9=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1C9->tag != 26)goto _LL16F;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;_tmp1CC=_tmp1C9->f3;_tmp1CD=_tmp1C9->f4;}}_LL16E: {
# 1401
union Cyc_CfFlowInfo_FlowInfo _tmp34E;void*_tmp34F;struct _tuple14 _tmp34D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CB);_tmp34E=_tmp34D.f1;_tmp34F=_tmp34D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp350=_tmp34E;struct Cyc_Dict_Dict _tmp351;struct Cyc_List_List*_tmp352;_LL213: if((_tmp350.BottomFL).tag != 1)goto _LL215;_LL214: {
struct _tuple14 _tmp832;return(_tmp832.f1=_tmp34E,((_tmp832.f2=_tmp14D->unknown_all,_tmp832)));}_LL215: if((_tmp350.ReachableFL).tag != 2)goto _LL212;_tmp351=((struct _tuple13)(_tmp350.ReachableFL).val).f1;_tmp352=((struct _tuple13)(_tmp350.ReachableFL).val).f2;_LL216:
# 1405
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp351,_tmp34F)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp835;void*_tmp834;(_tmp834=0,Cyc_Tcutil_terr(_tmp1CB->loc,((_tmp835="expression may not be initialized",_tag_dyneither(_tmp835,sizeof(char),34))),_tag_dyneither(_tmp834,sizeof(void*),0)));}{
# 1409
struct Cyc_List_List*new_relns=_tmp352;
comp_loop: {
void*_tmp356=_tmp1CB->r;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_Absyn_Vardecl*_tmp35B;struct Cyc_Absyn_Vardecl*_tmp35E;struct Cyc_Absyn_Vardecl*_tmp361;struct Cyc_Absyn_Vardecl*_tmp364;int _tmp366;enum Cyc_Absyn_Primop _tmp368;struct Cyc_Absyn_Exp*_tmp369;_LL218: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp357=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp356;if(_tmp357->tag != 13)goto _LL21A;else{_tmp358=_tmp357->f2;}}_LL219:
 _tmp1CB=_tmp358;goto comp_loop;_LL21A:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp359=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp356;if(_tmp359->tag != 1)goto _LL21C;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp35A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp359->f2);if(_tmp35A->tag != 1)goto _LL21C;else{_tmp35B=_tmp35A->f1;}}}if(!(!_tmp35B->escapes))goto _LL21C;_LL21B:
 _tmp35E=_tmp35B;goto _LL21D;_LL21C:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp35C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp356;if(_tmp35C->tag != 1)goto _LL21E;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp35D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp35C->f2);if(_tmp35D->tag != 4)goto _LL21E;else{_tmp35E=_tmp35D->f1;}}}if(!(!_tmp35E->escapes))goto _LL21E;_LL21D:
 _tmp361=_tmp35E;goto _LL21F;_LL21E:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp35F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp356;if(_tmp35F->tag != 1)goto _LL220;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp360=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp35F->f2);if(_tmp360->tag != 5)goto _LL220;else{_tmp361=_tmp360->f1;}}}if(!(!_tmp361->escapes))goto _LL220;_LL21F:
 _tmp364=_tmp361;goto _LL221;_LL220:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp362=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp356;if(_tmp362->tag != 1)goto _LL222;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp363=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp362->f2);if(_tmp363->tag != 3)goto _LL222;else{_tmp364=_tmp363->f1;}}}if(!(!_tmp364->escapes))goto _LL222;_LL221:
# 1417
{struct Cyc_CfFlowInfo_Reln*_tmp838;struct Cyc_List_List*_tmp837;new_relns=((_tmp837=_region_malloc(env->r,sizeof(*_tmp837)),((_tmp837->hd=((_tmp838=_region_malloc(env->r,sizeof(*_tmp838)),((_tmp838->vd=_tmp1CA,((_tmp838->rop=Cyc_CfFlowInfo_LessVar(_tmp364,_tmp364->type),_tmp838)))))),((_tmp837->tl=_tmp352,_tmp837))))));}
goto _LL217;_LL222: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp365=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp356;if(_tmp365->tag != 0)goto _LL224;else{if(((_tmp365->f1).Int_c).tag != 5)goto _LL224;_tmp366=((struct _tuple6)((_tmp365->f1).Int_c).val).f2;}}_LL223:
# 1420
{struct Cyc_CfFlowInfo_Reln*_tmp83B;struct Cyc_List_List*_tmp83A;new_relns=((_tmp83A=_region_malloc(env->r,sizeof(*_tmp83A)),((_tmp83A->hd=((_tmp83B=_region_malloc(env->r,sizeof(*_tmp83B)),((_tmp83B->vd=_tmp1CA,((_tmp83B->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp366),_tmp83B)))))),((_tmp83A->tl=_tmp352,_tmp83A))))));}
goto _LL217;_LL224: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp367=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp356;if(_tmp367->tag != 2)goto _LL226;else{_tmp368=_tmp367->f1;if(_tmp367->f2 == 0)goto _LL226;_tmp369=(struct Cyc_Absyn_Exp*)(_tmp367->f2)->hd;}}_LL225:
# 1423
{void*_tmp36E=_tmp369->r;struct Cyc_Absyn_Vardecl*_tmp371;struct Cyc_Absyn_Vardecl*_tmp374;struct Cyc_Absyn_Vardecl*_tmp377;struct Cyc_Absyn_Vardecl*_tmp37A;_LL229:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp36F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp36E;if(_tmp36F->tag != 1)goto _LL22B;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp370=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp36F->f2);if(_tmp370->tag != 1)goto _LL22B;else{_tmp371=_tmp370->f1;}}}if(!(!_tmp371->escapes))goto _LL22B;_LL22A:
 _tmp374=_tmp371;goto _LL22C;_LL22B:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp372=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp36E;if(_tmp372->tag != 1)goto _LL22D;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp373=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp372->f2);if(_tmp373->tag != 4)goto _LL22D;else{_tmp374=_tmp373->f1;}}}if(!(!_tmp374->escapes))goto _LL22D;_LL22C:
 _tmp377=_tmp374;goto _LL22E;_LL22D:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp375=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp36E;if(_tmp375->tag != 1)goto _LL22F;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp376=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp375->f2);if(_tmp376->tag != 5)goto _LL22F;else{_tmp377=_tmp376->f1;}}}if(!(!_tmp377->escapes))goto _LL22F;_LL22E:
 _tmp37A=_tmp377;goto _LL230;_LL22F:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp378=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp36E;if(_tmp378->tag != 1)goto _LL231;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp379=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp378->f2);if(_tmp379->tag != 3)goto _LL231;else{_tmp37A=_tmp379->f1;}}}if(!(!_tmp37A->escapes))goto _LL231;_LL230:
# 1429
{struct Cyc_CfFlowInfo_Reln*_tmp83E;struct Cyc_List_List*_tmp83D;new_relns=((_tmp83D=_region_malloc(env->r,sizeof(*_tmp83D)),((_tmp83D->hd=((_tmp83E=_region_malloc(env->r,sizeof(*_tmp83E)),((_tmp83E->vd=_tmp1CA,((_tmp83E->rop=Cyc_CfFlowInfo_LessNumelts(_tmp37A),_tmp83E)))))),((_tmp83D->tl=_tmp352,_tmp83D))))));}
goto _LL228;_LL231:;_LL232:
 goto _LL228;_LL228:;}
# 1433
goto _LL217;_LL226:;_LL227:
 goto _LL217;_LL217:;}
# 1436
if(_tmp352 != new_relns)
_tmp34E=Cyc_CfFlowInfo_ReachableFL(_tmp351,new_relns);{
# 1440
void*_tmp37D=_tmp34F;_LL234: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp37E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp37D;if(_tmp37E->tag != 0)goto _LL236;}_LL235: {
struct _tuple14 _tmp83F;return(_tmp83F.f1=_tmp34E,((_tmp83F.f2=_tmp14D->unknown_all,_tmp83F)));}_LL236: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp37F=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp37D;if(_tmp37F->tag != 2)goto _LL238;}_LL237:
 goto _LL239;_LL238: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp380=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp37D;if(_tmp380->tag != 1)goto _LL23A;}_LL239:
 goto _LL23B;_LL23A: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp381=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp37D;if(_tmp381->tag != 5)goto _LL23C;}_LL23B: {
# 1445
struct Cyc_List_List _tmp840;struct Cyc_List_List _tmp383=(_tmp840.hd=_tmp1CA,((_tmp840.tl=0,_tmp840)));
_tmp34E=Cyc_NewControlFlow_add_vars(_tmp14D,_tmp34E,& _tmp383,_tmp14D->unknown_all,e->loc,0);{
# 1448
union Cyc_CfFlowInfo_FlowInfo _tmp385;void*_tmp386;struct _tuple14 _tmp384=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34E,_tmp1CC);_tmp385=_tmp384.f1;_tmp386=_tmp384.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp387=_tmp385;struct Cyc_Dict_Dict _tmp388;_LL23F: if((_tmp387.BottomFL).tag != 1)goto _LL241;_LL240: {
struct _tuple14 _tmp841;return(_tmp841.f1=_tmp385,((_tmp841.f2=_tmp14D->unknown_all,_tmp841)));}_LL241: if((_tmp387.ReachableFL).tag != 2)goto _LL23E;_tmp388=((struct _tuple13)(_tmp387.ReachableFL).val).f1;_LL242:
# 1452
 if(Cyc_CfFlowInfo_initlevel(_tmp14D,_tmp388,_tmp386)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp844;void*_tmp843;(_tmp843=0,Cyc_Tcutil_terr(_tmp1CB->loc,((_tmp844="expression may not be initialized",_tag_dyneither(_tmp844,sizeof(char),34))),_tag_dyneither(_tmp843,sizeof(void*),0)));}{
struct _tuple14 _tmp845;return(_tmp845.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp845.f2=_tmp14D->unknown_all,_tmp845)));};}_LL23E:;}
# 1457
_tmp34E=_tmp385;
goto _LL23D;};}_LL23C:;_LL23D:
# 1460
 while(1){
struct Cyc_List_List _tmp846;struct Cyc_List_List _tmp38E=(_tmp846.hd=_tmp1CA,((_tmp846.tl=0,_tmp846)));
_tmp34E=Cyc_NewControlFlow_add_vars(_tmp14D,_tmp34E,& _tmp38E,_tmp14D->unknown_all,e->loc,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp390;void*_tmp391;struct _tuple14 _tmp38F=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34E,_tmp1CC);_tmp390=_tmp38F.f1;_tmp391=_tmp38F.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp392=_tmp390;struct Cyc_Dict_Dict _tmp393;_LL244: if((_tmp392.BottomFL).tag != 1)goto _LL246;_LL245:
 goto _LL243;_LL246: if((_tmp392.ReachableFL).tag != 2)goto _LL243;_tmp393=((struct _tuple13)(_tmp392.ReachableFL).val).f1;_LL247:
# 1467
 if(Cyc_CfFlowInfo_initlevel(_tmp14D,_tmp393,_tmp391)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp849;void*_tmp848;(_tmp848=0,Cyc_Tcutil_terr(_tmp1CB->loc,((_tmp849="expression may not be initialized",_tag_dyneither(_tmp849,sizeof(char),34))),_tag_dyneither(_tmp848,sizeof(void*),0)));}{
struct _tuple14 _tmp84A;return(_tmp84A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp84A.f2=_tmp14D->unknown_all,_tmp84A)));};}_LL243:;}{
# 1472
union Cyc_CfFlowInfo_FlowInfo _tmp397=Cyc_CfFlowInfo_join_flow(_tmp14D,env->all_changed,_tmp34E,_tmp390);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp397,_tmp34E))
break;
_tmp34E=_tmp397;};};}{
# 1477
struct _tuple14 _tmp84B;return(_tmp84B.f1=_tmp34E,((_tmp84B.f2=_tmp14D->unknown_all,_tmp84B)));};_LL233:;};};_LL212:;};}_LL16F: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1CE=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1CE->tag != 35)goto _LL171;else{_tmp1CF=_tmp1CE->f1;}}_LL170:
# 1480
 return Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp1CF);_LL171: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1D0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D0->tag != 1)goto _LL173;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1D1=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1D0->f2);if(_tmp1D1->tag != 0)goto _LL173;}}_LL172:
 goto _LL174;_LL173: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1D2=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D2->tag != 34)goto _LL175;}_LL174:
 goto _LL176;_LL175: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1D3=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D3->tag != 24)goto _LL177;}_LL176:
 goto _LL178;_LL177: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1D4=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D4->tag != 37)goto _LL179;}_LL178:
 goto _LL17A;_LL179: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1D5=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D5->tag != 38)goto _LL114;}_LL17A: {
# 1486
struct Cyc_Core_Impossible_exn_struct _tmp851;const char*_tmp850;struct Cyc_Core_Impossible_exn_struct*_tmp84F;(int)_throw((void*)((_tmp84F=_cycalloc(sizeof(*_tmp84F)),((_tmp84F[0]=((_tmp851.tag=Cyc_Core_Impossible,((_tmp851.f1=((_tmp850="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp850,sizeof(char),31))),_tmp851)))),_tmp84F)))));}_LL114:;};}
# 1495
static struct _tuple15 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
# 1502
struct Cyc_CfFlowInfo_FlowEnv*_tmp39D=env->fenv;
void*_tmp39E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3A0;union Cyc_Absyn_Constraint*_tmp3A1;union Cyc_Absyn_Constraint*_tmp3A2;_LL249: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp39F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39E;if(_tmp39F->tag != 5)goto _LL24B;else{_tmp3A0=(_tmp39F->f1).elt_typ;_tmp3A1=((_tmp39F->f1).ptr_atts).bounds;_tmp3A2=((_tmp39F->f1).ptr_atts).zero_term;}}_LL24A: {
# 1505
union Cyc_CfFlowInfo_FlowInfo _tmp3A3=f;struct Cyc_Dict_Dict _tmp3A4;struct Cyc_List_List*_tmp3A5;_LL24E: if((_tmp3A3.BottomFL).tag != 1)goto _LL250;_LL24F: {
struct _tuple15 _tmp852;return(_tmp852.f1=f,((_tmp852.f2=Cyc_CfFlowInfo_UnknownL(),_tmp852)));}_LL250: if((_tmp3A3.ReachableFL).tag != 2)goto _LL24D;_tmp3A4=((struct _tuple13)(_tmp3A3.ReachableFL).val).f1;_tmp3A5=((struct _tuple13)(_tmp3A3.ReachableFL).val).f2;_LL251: {
# 1509
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp3A1)){
retry: {void*_tmp3A7=r;struct Cyc_Absyn_Vardecl*_tmp3A9;void*_tmp3AA;void*_tmp3AE;struct Cyc_List_List*_tmp3AF;enum Cyc_CfFlowInfo_InitLevel _tmp3B2;_LL253: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3A8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3A7;if(_tmp3A8->tag != 8)goto _LL255;else{_tmp3A9=_tmp3A8->f1;_tmp3AA=(void*)_tmp3A8->f2;}}_LL254:
 r=_tmp3AA;locname=_tmp3A9;goto retry;_LL255: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3AB=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3A7;if(_tmp3AB->tag != 1)goto _LL257;}_LL256:
 goto _LL258;_LL257: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3AC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3A7;if(_tmp3AC->tag != 2)goto _LL259;}_LL258:
# 1515
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp855;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp854;e->annot=(void*)((_tmp854=_cycalloc(sizeof(*_tmp854)),((_tmp854[0]=((_tmp855.tag=Cyc_CfFlowInfo_NotZero,((_tmp855.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3A5),_tmp855)))),_tmp854))));}goto _LL252;_LL259: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3AD=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3A7;if(_tmp3AD->tag != 5)goto _LL25B;else{_tmp3AE=(_tmp3AD->f1)->root;_tmp3AF=(_tmp3AD->f1)->fields;}}_LL25A:
# 1517
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp858;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp857;e->annot=(void*)((_tmp857=_cycalloc(sizeof(*_tmp857)),((_tmp857[0]=((_tmp858.tag=Cyc_CfFlowInfo_NotZero,((_tmp858.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3A5),_tmp858)))),_tmp857))));}{
struct Cyc_CfFlowInfo_Place*_tmp85B;struct _tuple15 _tmp85A;return(_tmp85A.f1=f,((_tmp85A.f2=Cyc_CfFlowInfo_PlaceL(((_tmp85B=_region_malloc(_tmp39D->r,sizeof(*_tmp85B)),((_tmp85B->root=_tmp3AE,((_tmp85B->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp39D->r,_tmp3AF,flds),_tmp85B))))))),_tmp85A)));};_LL25B: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3B0=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3A7;if(_tmp3B0->tag != 0)goto _LL25D;}_LL25C:
# 1520
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp85C;return(_tmp85C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp85C.f2=Cyc_CfFlowInfo_UnknownL(),_tmp85C)));};_LL25D: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3B1=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3A7;if(_tmp3B1->tag != 3)goto _LL25F;else{_tmp3B2=_tmp3B1->f1;}}_LL25E:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp3B2,locname);goto _LL260;_LL25F:;_LL260: {
# 1524
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp85F;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp85E;e->annot=(void*)((_tmp85E=_cycalloc(sizeof(*_tmp85E)),((_tmp85E[0]=((_tmp85F.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp85F.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3A5),_tmp85F)))),_tmp85E))));}_LL252:;}}else{
# 1527
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp862;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp861;e->annot=(void*)((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861[0]=((_tmp862.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp862.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3A5),_tmp862)))),_tmp861))));}
if(Cyc_CfFlowInfo_initlevel(_tmp39D,_tmp3A4,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp865;void*_tmp864;(_tmp864=0,Cyc_Tcutil_terr(e->loc,((_tmp865="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp865,sizeof(char),46))),_tag_dyneither(_tmp864,sizeof(void*),0)));}{
struct _tuple15 _tmp866;return(_tmp866.f1=f,((_tmp866.f2=Cyc_CfFlowInfo_UnknownL(),_tmp866)));};}_LL24D:;}_LL24B:;_LL24C: {
# 1532
struct Cyc_Core_Impossible_exn_struct _tmp86C;const char*_tmp86B;struct Cyc_Core_Impossible_exn_struct*_tmp86A;(int)_throw((void*)((_tmp86A=_cycalloc(sizeof(*_tmp86A)),((_tmp86A[0]=((_tmp86C.tag=Cyc_Core_Impossible,((_tmp86C.f1=((_tmp86B="left deref of non-pointer-type",_tag_dyneither(_tmp86B,sizeof(char),31))),_tmp86C)))),_tmp86A)))));}_LL248:;}
# 1540
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1543
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3C4=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3C5=inflow;struct Cyc_Dict_Dict _tmp3C6;struct Cyc_List_List*_tmp3C7;_LL262: if((_tmp3C5.BottomFL).tag != 1)goto _LL264;_LL263: {
struct _tuple15 _tmp86D;return(_tmp86D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp86D.f2=Cyc_CfFlowInfo_UnknownL(),_tmp86D)));}_LL264: if((_tmp3C5.ReachableFL).tag != 2)goto _LL261;_tmp3C6=((struct _tuple13)(_tmp3C5.ReachableFL).val).f1;_tmp3C7=((struct _tuple13)(_tmp3C5.ReachableFL).val).f2;_LL265:
# 1548
 d=_tmp3C6;_LL261:;}{
# 1552
void*_tmp3C9=e->r;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Vardecl*_tmp3D2;struct Cyc_Absyn_Vardecl*_tmp3D5;struct Cyc_Absyn_Vardecl*_tmp3D8;struct Cyc_Absyn_Vardecl*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DD;struct _dyneither_ptr*_tmp3DE;struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_Absyn_Exp*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E5;struct _dyneither_ptr*_tmp3E6;_LL267: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3CA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3CA->tag != 13)goto _LL269;else{_tmp3CB=_tmp3CA->f2;}}_LL268:
 _tmp3CD=_tmp3CB;goto _LL26A;_LL269: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3CC=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3CC->tag != 11)goto _LL26B;else{_tmp3CD=_tmp3CC->f1;}}_LL26A:
 _tmp3CF=_tmp3CD;goto _LL26C;_LL26B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3CE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3CE->tag != 12)goto _LL26D;else{_tmp3CF=_tmp3CE->f1;}}_LL26C:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3CF,flds);_LL26D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3D0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3D0->tag != 1)goto _LL26F;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3D1=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3D0->f2);if(_tmp3D1->tag != 3)goto _LL26F;else{_tmp3D2=_tmp3D1->f1;}}}_LL26E:
# 1557
 _tmp3D5=_tmp3D2;goto _LL270;_LL26F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3D3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3D3->tag != 1)goto _LL271;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3D4=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3D3->f2);if(_tmp3D4->tag != 4)goto _LL271;else{_tmp3D5=_tmp3D4->f1;}}}_LL270:
 _tmp3D8=_tmp3D5;goto _LL272;_LL271: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3D6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3D6->tag != 1)goto _LL273;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3D7=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3D6->f2);if(_tmp3D7->tag != 5)goto _LL273;else{_tmp3D8=_tmp3D7->f1;}}}_LL272: {
# 1560
struct Cyc_CfFlowInfo_Place*_tmp877;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp876;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp875;struct _tuple15 _tmp874;return(_tmp874.f1=inflow,((_tmp874.f2=Cyc_CfFlowInfo_PlaceL(((_tmp877=_region_malloc(env->r,sizeof(*_tmp877)),((_tmp877->root=(void*)((_tmp875=_region_malloc(env->r,sizeof(*_tmp875)),((_tmp875[0]=((_tmp876.tag=0,((_tmp876.f1=_tmp3D8,_tmp876)))),_tmp875)))),((_tmp877->fields=flds,_tmp877))))))),_tmp874)));}_LL273: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3D9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3D9->tag != 1)goto _LL275;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3DA=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3D9->f2);if(_tmp3DA->tag != 1)goto _LL275;else{_tmp3DB=_tmp3DA->f1;}}}_LL274: {
# 1562
struct _tuple15 _tmp878;return(_tmp878.f1=inflow,((_tmp878.f2=Cyc_CfFlowInfo_UnknownL(),_tmp878)));}_LL275: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3DC=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3DC->tag != 21)goto _LL277;else{_tmp3DD=_tmp3DC->f1;_tmp3DE=_tmp3DC->f2;}}_LL276:
# 1565
{void*_tmp3EC=Cyc_Tcutil_compress((void*)_check_null(_tmp3DD->topt));void*_tmp3EE;_LL280: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC;if(_tmp3ED->tag != 5)goto _LL282;else{_tmp3EE=(_tmp3ED->f1).elt_typ;}}_LL281:
# 1567
 if(!Cyc_Absyn_is_nontagged_union_type(_tmp3EE)){
struct Cyc_List_List*_tmp879;flds=((_tmp879=_region_malloc(env->r,sizeof(*_tmp879)),((_tmp879->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3EE,_tmp3DE),((_tmp879->tl=flds,_tmp879))))));}
goto _LL27F;_LL282:;_LL283: {
struct Cyc_Core_Impossible_exn_struct _tmp87F;const char*_tmp87E;struct Cyc_Core_Impossible_exn_struct*_tmp87D;(int)_throw((void*)((_tmp87D=_cycalloc(sizeof(*_tmp87D)),((_tmp87D[0]=((_tmp87F.tag=Cyc_Core_Impossible,((_tmp87F.f1=((_tmp87E="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp87E,sizeof(char),25))),_tmp87F)))),_tmp87D)))));}_LL27F:;}
# 1572
_tmp3E0=_tmp3DD;goto _LL278;_LL277: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3DF=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3DF->tag != 19)goto _LL279;else{_tmp3E0=_tmp3DF->f1;}}_LL278:
# 1574
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3E0->topt))){
union Cyc_CfFlowInfo_FlowInfo _tmp3F4;union Cyc_CfFlowInfo_AbsLVal _tmp3F5;struct _tuple15 _tmp3F3=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp3E0);_tmp3F4=_tmp3F3.f1;_tmp3F5=_tmp3F3.f2;{
struct _tuple15 _tmp880;struct _tuple15 _tmp3F7=(_tmp880.f1=_tmp3F4,((_tmp880.f2=_tmp3F5,_tmp880)));struct Cyc_Dict_Dict _tmp3F8;struct Cyc_List_List*_tmp3F9;struct Cyc_CfFlowInfo_Place*_tmp3FA;_LL285: if(((_tmp3F7.f1).ReachableFL).tag != 2)goto _LL287;_tmp3F8=((struct _tuple13)((_tmp3F7.f1).ReachableFL).val).f1;_tmp3F9=((struct _tuple13)((_tmp3F7.f1).ReachableFL).val).f2;if(((_tmp3F7.f2).PlaceL).tag != 1)goto _LL287;_tmp3FA=(struct Cyc_CfFlowInfo_Place*)((_tmp3F7.f2).PlaceL).val;_LL286: {
# 1578
void*_tmp3FB=Cyc_CfFlowInfo_lookup_place(_tmp3F8,_tmp3FA);
struct Cyc_Absyn_Vardecl*locname=0;
void*_tmp3FC=_tmp3FB;struct Cyc_Absyn_Vardecl*_tmp3FF;void*_tmp400;_LL28A: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3FD=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(_tmp3FD->tag != 5)goto _LL28C;}_LL28B:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3F4,_tmp3E0,_tmp3F4,_tmp3FB,flds);_LL28C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3FE=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(_tmp3FE->tag != 8)goto _LL28E;else{_tmp3FF=_tmp3FE->f1;_tmp400=(void*)_tmp3FE->f2;}}_LL28D:
 _tmp3FB=_tmp400;locname=_tmp3FF;goto _LL28F;_LL28E:;_LL28F: {
# 1589
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3C4,_tmp3F8,_tmp3FB);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3C4->unknown_all: _tmp3C4->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3C4,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3E0->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp883;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp882;void*new_root=(void*)((_tmp882=_region_malloc(_tmp3C4->r,sizeof(*_tmp882)),((_tmp882[0]=((_tmp883.tag=1,((_tmp883.f1=e,((_tmp883.f2=(void*)_check_null(e->topt),_tmp883)))))),_tmp882))));
struct Cyc_CfFlowInfo_Place*_tmp884;struct Cyc_CfFlowInfo_Place*place=(_tmp884=_region_malloc(_tmp3C4->r,sizeof(*_tmp884)),((_tmp884->root=new_root,((_tmp884->fields=0,_tmp884)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp887;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp886;void*res=(void*)((_tmp886=_region_malloc(_tmp3C4->r,sizeof(*_tmp886)),((_tmp886[0]=((_tmp887.tag=5,((_tmp887.f1=place,_tmp887)))),_tmp886))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp88A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp889;res=(void*)((_tmp889=_region_malloc(_tmp3C4->r,sizeof(*_tmp889)),((_tmp889[0]=((_tmp88A.tag=8,((_tmp88A.f1=locname,((_tmp88A.f2=res,_tmp88A)))))),_tmp889))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3F8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3F8,new_root,new_rval);
_tmp3F8=Cyc_CfFlowInfo_assign_place(_tmp3C4,e->loc,_tmp3F8,env->all_changed,_tmp3FA,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp403=Cyc_CfFlowInfo_ReachableFL(_tmp3F8,_tmp3F9);
# 1604
return Cyc_NewControlFlow_anal_derefL(env,_tmp403,_tmp3E0,_tmp403,res,flds);};}_LL289:;}_LL287:;_LL288:
# 1606
 goto _LL284;_LL284:;};}{
# 1609
union Cyc_CfFlowInfo_FlowInfo _tmp40A;void*_tmp40B;struct _tuple14 _tmp409=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3E0);_tmp40A=_tmp409.f1;_tmp40B=_tmp409.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3E0,_tmp40A,_tmp40B,flds);};_LL279: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3E1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3E1->tag != 22)goto _LL27B;else{_tmp3E2=_tmp3E1->f1;_tmp3E3=_tmp3E1->f2;}}_LL27A: {
# 1613
void*_tmp40C=Cyc_Tcutil_compress((void*)_check_null(_tmp3E2->topt));union Cyc_Absyn_Constraint*_tmp40F;_LL291: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40C;if(_tmp40D->tag != 10)goto _LL293;}_LL292: {
# 1615
unsigned int _tmp410=(Cyc_Evexp_eval_const_uint_exp(_tmp3E3)).f1;
struct Cyc_List_List*_tmp88B;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3E2,((_tmp88B=_region_malloc(env->r,sizeof(*_tmp88B)),((_tmp88B->hd=(void*)((int)_tmp410),((_tmp88B->tl=flds,_tmp88B)))))));}_LL293: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C;if(_tmp40E->tag != 5)goto _LL295;else{_tmp40F=((_tmp40E->f1).ptr_atts).bounds;}}_LL294: {
# 1618
struct _RegionHandle _tmp412=_new_region("temp");struct _RegionHandle*temp=& _tmp412;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp415;struct Cyc_List_List*_tmp416;struct Cyc_Absyn_Exp*_tmp88C[2];struct _tuple20 _tmp414=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp88C[1]=_tmp3E3,((_tmp88C[0]=_tmp3E2,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp88C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp415=_tmp414.f1;_tmp416=_tmp414.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp417=_tmp415;
{union Cyc_CfFlowInfo_FlowInfo _tmp418=_tmp415;struct Cyc_Dict_Dict _tmp419;struct Cyc_List_List*_tmp41A;_LL298: if((_tmp418.ReachableFL).tag != 2)goto _LL29A;_tmp419=((struct _tuple13)(_tmp418.ReachableFL).val).f1;_tmp41A=((struct _tuple13)(_tmp418.ReachableFL).val).f2;_LL299:
# 1624
 if(Cyc_CfFlowInfo_initlevel(_tmp3C4,_tmp419,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp416))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp88F;void*_tmp88E;(_tmp88E=0,Cyc_Tcutil_terr(_tmp3E3->loc,((_tmp88F="expression may not be initialized",_tag_dyneither(_tmp88F,sizeof(char),34))),_tag_dyneither(_tmp88E,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp41D=Cyc_NewControlFlow_add_subscript_reln(_tmp3C4->r,_tmp41A,_tmp3E2,_tmp3E3);
if(_tmp41A != _tmp41D)
_tmp417=Cyc_CfFlowInfo_ReachableFL(_tmp419,_tmp41D);
goto _LL297;};_LL29A:;_LL29B:
 goto _LL297;_LL297:;}{
# 1632
union Cyc_CfFlowInfo_FlowInfo _tmp41F;union Cyc_CfFlowInfo_AbsLVal _tmp420;struct _tuple15 _tmp41E=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3E2,_tmp415,(void*)((struct Cyc_List_List*)_check_null(_tmp416))->hd,flds);_tmp41F=_tmp41E.f1;_tmp420=_tmp41E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp421=_tmp41F;_LL29D: if((_tmp421.BottomFL).tag != 1)goto _LL29F;_LL29E: {
struct _tuple15 _tmp890;struct _tuple15 _tmp423=(_tmp890.f1=_tmp41F,((_tmp890.f2=_tmp420,_tmp890)));_npop_handler(0);return _tmp423;}_LL29F:;_LL2A0: {
struct _tuple15 _tmp891;struct _tuple15 _tmp425=(_tmp891.f1=_tmp417,((_tmp891.f2=_tmp420,_tmp891)));_npop_handler(0);return _tmp425;}_LL29C:;};};};}
# 1619
;_pop_region(temp);}_LL295:;_LL296: {
# 1638
struct Cyc_Core_Impossible_exn_struct _tmp897;const char*_tmp896;struct Cyc_Core_Impossible_exn_struct*_tmp895;(int)_throw((void*)((_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895[0]=((_tmp897.tag=Cyc_Core_Impossible,((_tmp897.f1=((_tmp896="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp896,sizeof(char),33))),_tmp897)))),_tmp895)))));}_LL290:;}_LL27B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3E4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3E4->tag != 20)goto _LL27D;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=_tmp3E4->f2;}}_LL27C:
# 1642
 if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp3E5->topt))){
struct _tuple15 _tmp898;return(_tmp898.f1=inflow,((_tmp898.f2=Cyc_CfFlowInfo_UnknownL(),_tmp898)));}{
struct Cyc_List_List*_tmp899;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3E5,(
(_tmp899=_region_malloc(env->r,sizeof(*_tmp899)),((_tmp899->hd=(void*)Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp3E5->topt),_tmp3E6),((_tmp899->tl=flds,_tmp899)))))));};_LL27D:;_LL27E: {
# 1647
struct _tuple15 _tmp89A;return(_tmp89A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp89A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp89A)));}_LL266:;};}
# 1651
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
# 1653
union Cyc_CfFlowInfo_FlowInfo _tmp42D;union Cyc_CfFlowInfo_AbsLVal _tmp42E;struct _tuple15 _tmp42C=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);_tmp42D=_tmp42C.f1;_tmp42E=_tmp42C.f2;{
struct _tuple15 _tmp89B;return(_tmp89B.f1=_tmp42D,((_tmp89B.f2=_tmp42E,_tmp89B)));};}
# 1659
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1661
struct Cyc_CfFlowInfo_FlowEnv*_tmp430=env->fenv;
{void*_tmp431=e->r;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_Exp*_tmp434;struct Cyc_Absyn_Exp*_tmp435;struct Cyc_Absyn_Exp*_tmp437;struct Cyc_Absyn_Exp*_tmp438;struct Cyc_Absyn_Exp*_tmp43A;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Exp*_tmp440;enum Cyc_Absyn_Primop _tmp442;struct Cyc_List_List*_tmp443;_LL2A2: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp432=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp431;if(_tmp432->tag != 5)goto _LL2A4;else{_tmp433=_tmp432->f1;_tmp434=_tmp432->f2;_tmp435=_tmp432->f3;}}_LL2A3: {
# 1664
union Cyc_CfFlowInfo_FlowInfo _tmp445;union Cyc_CfFlowInfo_FlowInfo _tmp446;struct _tuple16 _tmp444=Cyc_NewControlFlow_anal_test(env,inflow,_tmp433);_tmp445=_tmp444.f1;_tmp446=_tmp444.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp448;union Cyc_CfFlowInfo_FlowInfo _tmp449;struct _tuple16 _tmp447=Cyc_NewControlFlow_anal_test(env,_tmp445,_tmp434);_tmp448=_tmp447.f1;_tmp449=_tmp447.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp44B;union Cyc_CfFlowInfo_FlowInfo _tmp44C;struct _tuple16 _tmp44A=Cyc_NewControlFlow_anal_test(env,_tmp446,_tmp435);_tmp44B=_tmp44A.f1;_tmp44C=_tmp44A.f2;{
struct _tuple16 _tmp89C;return(_tmp89C.f1=Cyc_CfFlowInfo_join_flow(_tmp430,env->all_changed,_tmp448,_tmp44B),((_tmp89C.f2=
Cyc_CfFlowInfo_join_flow(_tmp430,env->all_changed,_tmp449,_tmp44C),_tmp89C)));};};};}_LL2A4: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp436=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp431;if(_tmp436->tag != 6)goto _LL2A6;else{_tmp437=_tmp436->f1;_tmp438=_tmp436->f2;}}_LL2A5: {
# 1670
union Cyc_CfFlowInfo_FlowInfo _tmp44F;union Cyc_CfFlowInfo_FlowInfo _tmp450;struct _tuple16 _tmp44E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp437);_tmp44F=_tmp44E.f1;_tmp450=_tmp44E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp452;union Cyc_CfFlowInfo_FlowInfo _tmp453;struct _tuple16 _tmp451=Cyc_NewControlFlow_anal_test(env,_tmp44F,_tmp438);_tmp452=_tmp451.f1;_tmp453=_tmp451.f2;{
struct _tuple16 _tmp89D;return(_tmp89D.f1=_tmp452,((_tmp89D.f2=Cyc_CfFlowInfo_join_flow(_tmp430,env->all_changed,_tmp450,_tmp453),_tmp89D)));};};}_LL2A6: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp439=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp431;if(_tmp439->tag != 7)goto _LL2A8;else{_tmp43A=_tmp439->f1;_tmp43B=_tmp439->f2;}}_LL2A7: {
# 1674
union Cyc_CfFlowInfo_FlowInfo _tmp456;union Cyc_CfFlowInfo_FlowInfo _tmp457;struct _tuple16 _tmp455=Cyc_NewControlFlow_anal_test(env,inflow,_tmp43A);_tmp456=_tmp455.f1;_tmp457=_tmp455.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp459;union Cyc_CfFlowInfo_FlowInfo _tmp45A;struct _tuple16 _tmp458=Cyc_NewControlFlow_anal_test(env,_tmp457,_tmp43B);_tmp459=_tmp458.f1;_tmp45A=_tmp458.f2;{
struct _tuple16 _tmp89E;return(_tmp89E.f1=Cyc_CfFlowInfo_join_flow(_tmp430,env->all_changed,_tmp456,_tmp459),((_tmp89E.f2=_tmp45A,_tmp89E)));};};}_LL2A8: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp43C=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp431;if(_tmp43C->tag != 8)goto _LL2AA;else{_tmp43D=_tmp43C->f1;_tmp43E=_tmp43C->f2;}}_LL2A9: {
# 1678
union Cyc_CfFlowInfo_FlowInfo _tmp45D;void*_tmp45E;struct _tuple14 _tmp45C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp43D);_tmp45D=_tmp45C.f1;_tmp45E=_tmp45C.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp45D,_tmp43E);}_LL2AA: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp43F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp431;if(_tmp43F->tag != 2)goto _LL2AC;else{if(_tmp43F->f1 != Cyc_Absyn_Not)goto _LL2AC;if(_tmp43F->f2 == 0)goto _LL2AC;_tmp440=(struct Cyc_Absyn_Exp*)(_tmp43F->f2)->hd;if((_tmp43F->f2)->tl != 0)goto _LL2AC;}}_LL2AB: {
# 1681
union Cyc_CfFlowInfo_FlowInfo _tmp460;union Cyc_CfFlowInfo_FlowInfo _tmp461;struct _tuple16 _tmp45F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp440);_tmp460=_tmp45F.f1;_tmp461=_tmp45F.f2;{
struct _tuple16 _tmp89F;return(_tmp89F.f1=_tmp461,((_tmp89F.f2=_tmp460,_tmp89F)));};}_LL2AC: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp441=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp431;if(_tmp441->tag != 2)goto _LL2AE;else{_tmp442=_tmp441->f1;_tmp443=_tmp441->f2;}}_LL2AD: {
# 1686
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp463=_new_region("temp");struct _RegionHandle*temp=& _tmp463;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp465;struct Cyc_List_List*_tmp466;struct _tuple20 _tmp464=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp443,0,0);_tmp465=_tmp464.f1;_tmp466=_tmp464.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp466))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp466->tl))->hd;
f=_tmp465;}{
# 1696
union Cyc_CfFlowInfo_FlowInfo _tmp467=f;struct Cyc_Dict_Dict _tmp468;struct Cyc_List_List*_tmp469;_LL2B1: if((_tmp467.BottomFL).tag != 1)goto _LL2B3;_LL2B2: {
struct _tuple16 _tmp8A0;struct _tuple16 _tmp46B=(_tmp8A0.f1=f,((_tmp8A0.f2=f,_tmp8A0)));_npop_handler(0);return _tmp46B;}_LL2B3: if((_tmp467.ReachableFL).tag != 2)goto _LL2B0;_tmp468=((struct _tuple13)(_tmp467.ReachableFL).val).f1;_tmp469=((struct _tuple13)(_tmp467.ReachableFL).val).f2;_LL2B4: {
# 1699
struct Cyc_Absyn_Exp*_tmp46C=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp443))->hd;
struct Cyc_Absyn_Exp*_tmp46D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp443->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp468,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8A3;void*_tmp8A2;(_tmp8A2=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp443->hd)->loc,((_tmp8A3="expression may not be initialized",_tag_dyneither(_tmp8A3,sizeof(char),34))),_tag_dyneither(_tmp8A2,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp468,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8A6;void*_tmp8A5;(_tmp8A5=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp443->tl))->hd)->loc,((_tmp8A6="expression may not be initialized",_tag_dyneither(_tmp8A6,sizeof(char),34))),_tag_dyneither(_tmp8A5,sizeof(void*),0)));}
# 1708
if(_tmp442 == Cyc_Absyn_Eq  || _tmp442 == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp472=r1;struct Cyc_Absyn_Vardecl*_tmp474;void*_tmp475;_LL2B6: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp473=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp472;if(_tmp473->tag != 8)goto _LL2B8;else{_tmp474=_tmp473->f1;_tmp475=(void*)_tmp473->f2;}}_LL2B7:
 r1=_tmp475;r1n=_tmp474;goto _LL2B5;_LL2B8:;_LL2B9:
 goto _LL2B5;_LL2B5:;}
# 1714
{void*_tmp476=r2;struct Cyc_Absyn_Vardecl*_tmp478;void*_tmp479;_LL2BB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp477=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp476;if(_tmp477->tag != 8)goto _LL2BD;else{_tmp478=_tmp477->f1;_tmp479=(void*)_tmp477->f2;}}_LL2BC:
 r2=_tmp479;r2n=_tmp478;goto _LL2BA;_LL2BD:;_LL2BE:
 goto _LL2BA;_LL2BA:;}{
# 1719
struct _tuple0 _tmp8A7;struct _tuple0 _tmp47B=(_tmp8A7.f1=r1,((_tmp8A7.f2=r2,_tmp8A7)));enum Cyc_CfFlowInfo_InitLevel _tmp47D;enum Cyc_CfFlowInfo_InitLevel _tmp481;_LL2C0:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp47C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp47B.f1;if(_tmp47C->tag != 3)goto _LL2C2;else{_tmp47D=_tmp47C->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp47E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47B.f2;if(_tmp47E->tag != 0)goto _LL2C2;};_LL2C1: {
# 1723
union Cyc_CfFlowInfo_FlowInfo _tmp491;union Cyc_CfFlowInfo_FlowInfo _tmp492;struct _tuple16 _tmp490=Cyc_NewControlFlow_splitzero(env,f,f,_tmp46C,_tmp47D,r1n);_tmp491=_tmp490.f1;_tmp492=_tmp490.f2;
switch(_tmp442){case Cyc_Absyn_Eq: _LL2D4: {
struct _tuple16 _tmp8A8;struct _tuple16 _tmp494=(_tmp8A8.f1=_tmp492,((_tmp8A8.f2=_tmp491,_tmp8A8)));_npop_handler(0);return _tmp494;}case Cyc_Absyn_Neq: _LL2D5: {
struct _tuple16 _tmp8A9;struct _tuple16 _tmp496=(_tmp8A9.f1=_tmp491,((_tmp8A9.f2=_tmp492,_tmp8A9)));_npop_handler(0);return _tmp496;}default: _LL2D6: {
struct Cyc_Core_Impossible_exn_struct _tmp8AF;const char*_tmp8AE;struct Cyc_Core_Impossible_exn_struct*_tmp8AD;(int)_throw((void*)((_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD[0]=((_tmp8AF.tag=Cyc_Core_Impossible,((_tmp8AF.f1=((_tmp8AE="anal_test, zero-split",_tag_dyneither(_tmp8AE,sizeof(char),22))),_tmp8AF)))),_tmp8AD)))));}}}_LL2C2:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp47F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47B.f1;if(_tmp47F->tag != 0)goto _LL2C4;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp480=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp47B.f2;if(_tmp480->tag != 3)goto _LL2C4;else{_tmp481=_tmp480->f1;}};_LL2C3: {
# 1730
union Cyc_CfFlowInfo_FlowInfo _tmp49B;union Cyc_CfFlowInfo_FlowInfo _tmp49C;struct _tuple16 _tmp49A=Cyc_NewControlFlow_splitzero(env,f,f,_tmp46D,_tmp481,r2n);_tmp49B=_tmp49A.f1;_tmp49C=_tmp49A.f2;
switch(_tmp442){case Cyc_Absyn_Eq: _LL2D8: {
struct _tuple16 _tmp8B0;struct _tuple16 _tmp49E=(_tmp8B0.f1=_tmp49C,((_tmp8B0.f2=_tmp49B,_tmp8B0)));_npop_handler(0);return _tmp49E;}case Cyc_Absyn_Neq: _LL2D9: {
struct _tuple16 _tmp8B1;struct _tuple16 _tmp4A0=(_tmp8B1.f1=_tmp49B,((_tmp8B1.f2=_tmp49C,_tmp8B1)));_npop_handler(0);return _tmp4A0;}default: _LL2DA: {
struct Cyc_Core_Impossible_exn_struct _tmp8B7;const char*_tmp8B6;struct Cyc_Core_Impossible_exn_struct*_tmp8B5;(int)_throw((void*)((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5[0]=((_tmp8B7.tag=Cyc_Core_Impossible,((_tmp8B7.f1=((_tmp8B6="anal_test, zero-split",_tag_dyneither(_tmp8B6,sizeof(char),22))),_tmp8B7)))),_tmp8B5)))));}}}_LL2C4:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp482=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47B.f1;if(_tmp482->tag != 0)goto _LL2C6;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp483=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47B.f2;if(_tmp483->tag != 0)goto _LL2C6;};_LL2C5:
# 1737
 if(_tmp442 == Cyc_Absyn_Eq){struct _tuple16 _tmp8B8;struct _tuple16 _tmp4A5=(_tmp8B8.f1=f,((_tmp8B8.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8B8)));_npop_handler(0);return _tmp4A5;}else{
struct _tuple16 _tmp8B9;struct _tuple16 _tmp4A7=(_tmp8B9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8B9.f2=f,_tmp8B9)));_npop_handler(0);return _tmp4A7;}_LL2C6:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp484=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47B.f1;if(_tmp484->tag != 0)goto _LL2C8;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp485=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp47B.f2;if(_tmp485->tag != 1)goto _LL2C8;};_LL2C7:
 goto _LL2C9;_LL2C8:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp486=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47B.f1;if(_tmp486->tag != 0)goto _LL2CA;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp487=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp47B.f2;if(_tmp487->tag != 2)goto _LL2CA;};_LL2C9:
 goto _LL2CB;_LL2CA:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp488=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47B.f1;if(_tmp488->tag != 0)goto _LL2CC;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp489=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp47B.f2;if(_tmp489->tag != 5)goto _LL2CC;};_LL2CB:
 goto _LL2CD;_LL2CC:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp48A=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp47B.f1;if(_tmp48A->tag != 1)goto _LL2CE;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp48B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47B.f2;if(_tmp48B->tag != 0)goto _LL2CE;};_LL2CD:
 goto _LL2CF;_LL2CE:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp48C=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp47B.f1;if(_tmp48C->tag != 2)goto _LL2D0;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp48D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47B.f2;if(_tmp48D->tag != 0)goto _LL2D0;};_LL2CF:
 goto _LL2D1;_LL2D0:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp48E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp47B.f1;if(_tmp48E->tag != 5)goto _LL2D2;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp48F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47B.f2;if(_tmp48F->tag != 0)goto _LL2D2;};_LL2D1:
# 1745
 if(_tmp442 == Cyc_Absyn_Neq){struct _tuple16 _tmp8BA;struct _tuple16 _tmp4A9=(_tmp8BA.f1=f,((_tmp8BA.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8BA)));_npop_handler(0);return _tmp4A9;}else{
struct _tuple16 _tmp8BB;struct _tuple16 _tmp4AB=(_tmp8BB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8BB.f2=f,_tmp8BB)));_npop_handler(0);return _tmp4AB;}_LL2D2:;_LL2D3:
 goto _LL2BF;_LL2BF:;};}
# 1755
{struct _tuple0 _tmp8BC;struct _tuple0 _tmp4AD=(_tmp8BC.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp46C->topt)),((_tmp8BC.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp46D->topt)),_tmp8BC)));_LL2DD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4AD.f1;if(_tmp4AE->tag != 6)goto _LL2DF;else{if(_tmp4AE->f1 != Cyc_Absyn_Unsigned)goto _LL2DF;}}_LL2DE:
 goto _LL2E0;_LL2DF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4AD.f2;if(_tmp4AF->tag != 6)goto _LL2E1;else{if(_tmp4AF->f1 != Cyc_Absyn_Unsigned)goto _LL2E1;}}_LL2E0:
 goto _LL2E2;_LL2E1: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4B0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4AD.f1;if(_tmp4B0->tag != 19)goto _LL2E3;}_LL2E2:
 goto _LL2E4;_LL2E3: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4B1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4AD.f2;if(_tmp4B1->tag != 19)goto _LL2E5;}_LL2E4:
 goto _LL2DC;_LL2E5:;_LL2E6: {
struct _tuple16 _tmp8BD;struct _tuple16 _tmp4B3=(_tmp8BD.f1=f,((_tmp8BD.f2=f,_tmp8BD)));_npop_handler(0);return _tmp4B3;}_LL2DC:;}
# 1763
switch(_tmp442){case Cyc_Absyn_Eq: _LL2E7:
 goto _LL2E8;case Cyc_Absyn_Neq: _LL2E8:
 goto _LL2E9;case Cyc_Absyn_Gt: _LL2E9:
 goto _LL2EA;case Cyc_Absyn_Gte: _LL2EA: {
struct _tuple16 _tmp8BE;struct _tuple16 _tmp4B5=(_tmp8BE.f1=f,((_tmp8BE.f2=f,_tmp8BE)));_npop_handler(0);return _tmp4B5;}case Cyc_Absyn_Lt: _LL2EB: {
# 1769
union Cyc_CfFlowInfo_FlowInfo _tmp4B6=f;
union Cyc_CfFlowInfo_FlowInfo _tmp4B7=f;
# 1772
{union Cyc_CfFlowInfo_FlowInfo _tmp4B8=_tmp4B6;struct Cyc_Dict_Dict _tmp4B9;_LL2EE: if((_tmp4B8.BottomFL).tag != 1)goto _LL2F0;_LL2EF: {
struct Cyc_Core_Impossible_exn_struct _tmp8C4;const char*_tmp8C3;struct Cyc_Core_Impossible_exn_struct*_tmp8C2;(int)_throw((void*)((_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2[0]=((_tmp8C4.tag=Cyc_Core_Impossible,((_tmp8C4.f1=((_tmp8C3="anal_test, Lt",_tag_dyneither(_tmp8C3,sizeof(char),14))),_tmp8C4)))),_tmp8C2)))));}_LL2F0: if((_tmp4B8.ReachableFL).tag != 2)goto _LL2ED;_tmp4B9=((struct _tuple13)(_tmp4B8.ReachableFL).val).f1;_LL2F1:
 _tmp468=_tmp4B9;_LL2ED:;}{
# 1778
void*_tmp4BD=_tmp46C->r;struct Cyc_Absyn_Vardecl*_tmp4C0;struct Cyc_Absyn_Vardecl*_tmp4C3;struct Cyc_Absyn_Vardecl*_tmp4C6;struct Cyc_Absyn_Vardecl*_tmp4C9;_LL2F3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4BE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4BD;if(_tmp4BE->tag != 1)goto _LL2F5;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4BF=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4BE->f2);if(_tmp4BF->tag != 1)goto _LL2F5;else{_tmp4C0=_tmp4BF->f1;}}}if(!(!_tmp4C0->escapes))goto _LL2F5;_LL2F4:
# 1780
 _tmp4C3=_tmp4C0;goto _LL2F6;_LL2F5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4C1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4BD;if(_tmp4C1->tag != 1)goto _LL2F7;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4C2=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4C1->f2);if(_tmp4C2->tag != 4)goto _LL2F7;else{_tmp4C3=_tmp4C2->f1;}}}if(!(!_tmp4C3->escapes))goto _LL2F7;_LL2F6:
 _tmp4C6=_tmp4C3;goto _LL2F8;_LL2F7:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4C4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4BD;if(_tmp4C4->tag != 1)goto _LL2F9;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4C5=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4C4->f2);if(_tmp4C5->tag != 5)goto _LL2F9;else{_tmp4C6=_tmp4C5->f1;}}}if(!(!_tmp4C6->escapes))goto _LL2F9;_LL2F8:
 _tmp4C9=_tmp4C6;goto _LL2FA;_LL2F9:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4C7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4BD;if(_tmp4C7->tag != 1)goto _LL2FB;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4C8=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4C7->f2);if(_tmp4C8->tag != 3)goto _LL2FB;else{_tmp4C9=_tmp4C8->f1;}}}if(!(!_tmp4C9->escapes))goto _LL2FB;_LL2FA: {
# 1784
void*_tmp4CA=_tmp46D->r;struct Cyc_Absyn_Vardecl*_tmp4CD;struct Cyc_Absyn_Vardecl*_tmp4D0;struct Cyc_Absyn_Vardecl*_tmp4D3;struct Cyc_Absyn_Vardecl*_tmp4D6;int _tmp4D8;int _tmp4DB;enum Cyc_Absyn_Primop _tmp4DD;struct Cyc_Absyn_Exp*_tmp4DE;_LL2FE:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4CB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CA;if(_tmp4CB->tag != 1)goto _LL300;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4CC=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4CB->f2);if(_tmp4CC->tag != 1)goto _LL300;else{_tmp4CD=_tmp4CC->f1;}}}if(!(!_tmp4CD->escapes))goto _LL300;_LL2FF:
 _tmp4D0=_tmp4CD;goto _LL301;_LL300:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4CE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CA;if(_tmp4CE->tag != 1)goto _LL302;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4CF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4CE->f2);if(_tmp4CF->tag != 4)goto _LL302;else{_tmp4D0=_tmp4CF->f1;}}}if(!(!_tmp4D0->escapes))goto _LL302;_LL301:
 _tmp4D3=_tmp4D0;goto _LL303;_LL302:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CA;if(_tmp4D1->tag != 1)goto _LL304;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4D2=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4D1->f2);if(_tmp4D2->tag != 5)goto _LL304;else{_tmp4D3=_tmp4D2->f1;}}}if(!(!_tmp4D3->escapes))goto _LL304;_LL303:
 _tmp4D6=_tmp4D3;goto _LL305;_LL304:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CA;if(_tmp4D4->tag != 1)goto _LL306;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4D5=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4D4->f2);if(_tmp4D5->tag != 3)goto _LL306;else{_tmp4D6=_tmp4D5->f1;}}}if(!(!_tmp4D6->escapes))goto _LL306;_LL305: {
# 1790
struct _RegionHandle*_tmp4DF=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8C7;struct Cyc_List_List*_tmp8C6;struct Cyc_List_List*_tmp4E0=(_tmp8C6=_region_malloc(_tmp4DF,sizeof(*_tmp8C6)),((_tmp8C6->hd=((_tmp8C7=_region_malloc(_tmp4DF,sizeof(*_tmp8C7)),((_tmp8C7->vd=_tmp4C9,((_tmp8C7->rop=Cyc_CfFlowInfo_LessVar(_tmp4D6,_tmp4D6->type),_tmp8C7)))))),((_tmp8C6->tl=_tmp469,_tmp8C6)))));
struct _tuple16 _tmp8C8;struct _tuple16 _tmp4E2=(_tmp8C8.f1=Cyc_CfFlowInfo_ReachableFL(_tmp468,_tmp4E0),((_tmp8C8.f2=_tmp4B7,_tmp8C8)));_npop_handler(0);return _tmp4E2;}_LL306: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4D7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4CA;if(_tmp4D7->tag != 0)goto _LL308;else{if(((_tmp4D7->f1).Int_c).tag != 5)goto _LL308;_tmp4D8=((struct _tuple6)((_tmp4D7->f1).Int_c).val).f2;}}_LL307:
 _tmp4DB=_tmp4D8;goto _LL309;_LL308: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4D9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4CA;if(_tmp4D9->tag != 13)goto _LL30A;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4DA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(_tmp4D9->f2)->r;if(_tmp4DA->tag != 0)goto _LL30A;else{if(((_tmp4DA->f1).Int_c).tag != 5)goto _LL30A;_tmp4DB=((struct _tuple6)((_tmp4DA->f1).Int_c).val).f2;}}}_LL309: {
# 1795
struct _RegionHandle*_tmp4E5=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8CB;struct Cyc_List_List*_tmp8CA;struct Cyc_List_List*_tmp4E6=(_tmp8CA=_region_malloc(_tmp4E5,sizeof(*_tmp8CA)),((_tmp8CA->hd=((_tmp8CB=_region_malloc(_tmp4E5,sizeof(*_tmp8CB)),((_tmp8CB->vd=_tmp4C9,((_tmp8CB->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp4DB),_tmp8CB)))))),((_tmp8CA->tl=_tmp469,_tmp8CA)))));
struct _tuple16 _tmp8CC;struct _tuple16 _tmp4E8=(_tmp8CC.f1=Cyc_CfFlowInfo_ReachableFL(_tmp468,_tmp4E6),((_tmp8CC.f2=_tmp4B7,_tmp8CC)));_npop_handler(0);return _tmp4E8;}_LL30A: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4DC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4CA;if(_tmp4DC->tag != 2)goto _LL30C;else{_tmp4DD=_tmp4DC->f1;if(_tmp4DC->f2 == 0)goto _LL30C;_tmp4DE=(struct Cyc_Absyn_Exp*)(_tmp4DC->f2)->hd;}}_LL30B: {
# 1799
void*_tmp4EB=_tmp4DE->r;struct Cyc_Absyn_Vardecl*_tmp4EE;struct Cyc_Absyn_Vardecl*_tmp4F1;struct Cyc_Absyn_Vardecl*_tmp4F4;struct Cyc_Absyn_Vardecl*_tmp4F7;_LL30F:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4EC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EB;if(_tmp4EC->tag != 1)goto _LL311;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4ED=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4EC->f2);if(_tmp4ED->tag != 1)goto _LL311;else{_tmp4EE=_tmp4ED->f1;}}}if(!(!_tmp4EE->escapes))goto _LL311;_LL310:
 _tmp4F1=_tmp4EE;goto _LL312;_LL311:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4EF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EB;if(_tmp4EF->tag != 1)goto _LL313;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4F0=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4EF->f2);if(_tmp4F0->tag != 4)goto _LL313;else{_tmp4F1=_tmp4F0->f1;}}}if(!(!_tmp4F1->escapes))goto _LL313;_LL312:
 _tmp4F4=_tmp4F1;goto _LL314;_LL313:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4F2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EB;if(_tmp4F2->tag != 1)goto _LL315;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4F3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4F2->f2);if(_tmp4F3->tag != 5)goto _LL315;else{_tmp4F4=_tmp4F3->f1;}}}if(!(!_tmp4F4->escapes))goto _LL315;_LL314:
 _tmp4F7=_tmp4F4;goto _LL316;_LL315:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4F5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EB;if(_tmp4F5->tag != 1)goto _LL317;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4F6=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4F5->f2);if(_tmp4F6->tag != 3)goto _LL317;else{_tmp4F7=_tmp4F6->f1;}}}if(!(!_tmp4F7->escapes))goto _LL317;_LL316: {
# 1805
struct _RegionHandle*_tmp4F8=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8CF;struct Cyc_List_List*_tmp8CE;struct Cyc_List_List*_tmp4F9=(_tmp8CE=_region_malloc(_tmp4F8,sizeof(*_tmp8CE)),((_tmp8CE->hd=((_tmp8CF=_region_malloc(_tmp4F8,sizeof(*_tmp8CF)),((_tmp8CF->vd=_tmp4C9,((_tmp8CF->rop=Cyc_CfFlowInfo_LessNumelts(_tmp4F7),_tmp8CF)))))),((_tmp8CE->tl=_tmp469,_tmp8CE)))));
struct _tuple16 _tmp8D0;struct _tuple16 _tmp4FB=(_tmp8D0.f1=Cyc_CfFlowInfo_ReachableFL(_tmp468,_tmp4F9),((_tmp8D0.f2=_tmp4B7,_tmp8D0)));_npop_handler(0);return _tmp4FB;}_LL317:;_LL318: {
struct _tuple16 _tmp8D1;struct _tuple16 _tmp4FF=(_tmp8D1.f1=_tmp4B6,((_tmp8D1.f2=_tmp4B7,_tmp8D1)));_npop_handler(0);return _tmp4FF;}_LL30E:;}_LL30C:;_LL30D: {
# 1810
struct _tuple16 _tmp8D2;struct _tuple16 _tmp501=(_tmp8D2.f1=_tmp4B6,((_tmp8D2.f2=_tmp4B7,_tmp8D2)));_npop_handler(0);return _tmp501;}_LL2FD:;}_LL2FB:;_LL2FC: {
# 1812
struct _tuple16 _tmp8D3;struct _tuple16 _tmp503=(_tmp8D3.f1=_tmp4B6,((_tmp8D3.f2=_tmp4B7,_tmp8D3)));_npop_handler(0);return _tmp503;}_LL2F2:;};}case Cyc_Absyn_Lte: _LL2EC: {
# 1816
union Cyc_CfFlowInfo_FlowInfo _tmp504=f;
union Cyc_CfFlowInfo_FlowInfo _tmp505=f;
# 1819
{union Cyc_CfFlowInfo_FlowInfo _tmp506=_tmp504;struct Cyc_Dict_Dict _tmp507;_LL31B: if((_tmp506.BottomFL).tag != 1)goto _LL31D;_LL31C: {
struct Cyc_Core_Impossible_exn_struct _tmp8D9;const char*_tmp8D8;struct Cyc_Core_Impossible_exn_struct*_tmp8D7;(int)_throw((void*)((_tmp8D7=_cycalloc(sizeof(*_tmp8D7)),((_tmp8D7[0]=((_tmp8D9.tag=Cyc_Core_Impossible,((_tmp8D9.f1=((_tmp8D8="anal_test, Lte",_tag_dyneither(_tmp8D8,sizeof(char),15))),_tmp8D9)))),_tmp8D7)))));}_LL31D: if((_tmp506.ReachableFL).tag != 2)goto _LL31A;_tmp507=((struct _tuple13)(_tmp506.ReachableFL).val).f1;_LL31E:
 _tmp468=_tmp507;_LL31A:;}{
# 1825
void*_tmp50B=_tmp46C->r;struct Cyc_Absyn_Vardecl*_tmp50E;struct Cyc_Absyn_Vardecl*_tmp511;struct Cyc_Absyn_Vardecl*_tmp514;struct Cyc_Absyn_Vardecl*_tmp517;_LL320:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50B;if(_tmp50C->tag != 1)goto _LL322;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp50D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp50C->f2);if(_tmp50D->tag != 1)goto _LL322;else{_tmp50E=_tmp50D->f1;}}}if(!(!_tmp50E->escapes))goto _LL322;_LL321:
# 1827
 _tmp511=_tmp50E;goto _LL323;_LL322:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp50F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50B;if(_tmp50F->tag != 1)goto _LL324;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp510=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp50F->f2);if(_tmp510->tag != 4)goto _LL324;else{_tmp511=_tmp510->f1;}}}if(!(!_tmp511->escapes))goto _LL324;_LL323:
 _tmp514=_tmp511;goto _LL325;_LL324:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp512=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50B;if(_tmp512->tag != 1)goto _LL326;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp513=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp512->f2);if(_tmp513->tag != 5)goto _LL326;else{_tmp514=_tmp513->f1;}}}if(!(!_tmp514->escapes))goto _LL326;_LL325:
 _tmp517=_tmp514;goto _LL327;_LL326:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp515=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50B;if(_tmp515->tag != 1)goto _LL328;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp516=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp515->f2);if(_tmp516->tag != 3)goto _LL328;else{_tmp517=_tmp516->f1;}}}if(!(!_tmp517->escapes))goto _LL328;_LL327: {
# 1831
void*_tmp518=_tmp46D->r;int _tmp51A;int _tmp51D;enum Cyc_Absyn_Primop _tmp51F;struct Cyc_Absyn_Exp*_tmp520;_LL32B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp519=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp519->tag != 0)goto _LL32D;else{if(((_tmp519->f1).Int_c).tag != 5)goto _LL32D;_tmp51A=((struct _tuple6)((_tmp519->f1).Int_c).val).f2;}}_LL32C:
 _tmp51D=_tmp51A;goto _LL32E;_LL32D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp51B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp51B->tag != 13)goto _LL32F;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp51C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(_tmp51B->f2)->r;if(_tmp51C->tag != 0)goto _LL32F;else{if(((_tmp51C->f1).Int_c).tag != 5)goto _LL32F;_tmp51D=((struct _tuple6)((_tmp51C->f1).Int_c).val).f2;}}}_LL32E: {
# 1834
struct _RegionHandle*_tmp521=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8DC;struct Cyc_List_List*_tmp8DB;struct Cyc_List_List*_tmp522=(_tmp8DB=_region_malloc(_tmp521,sizeof(*_tmp8DB)),((_tmp8DB->hd=(
(_tmp8DC=_region_malloc(_tmp521,sizeof(*_tmp8DC)),((_tmp8DC->vd=_tmp517,((_tmp8DC->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)_tmp51D),_tmp8DC)))))),((_tmp8DB->tl=_tmp469,_tmp8DB)))));
struct _tuple16 _tmp8DD;struct _tuple16 _tmp524=(_tmp8DD.f1=Cyc_CfFlowInfo_ReachableFL(_tmp468,_tmp522),((_tmp8DD.f2=_tmp505,_tmp8DD)));_npop_handler(0);return _tmp524;}_LL32F: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp51E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp51E->tag != 2)goto _LL331;else{_tmp51F=_tmp51E->f1;if(_tmp51E->f2 == 0)goto _LL331;_tmp520=(struct Cyc_Absyn_Exp*)(_tmp51E->f2)->hd;}}_LL330: {
# 1839
void*_tmp527=_tmp520->r;struct Cyc_Absyn_Vardecl*_tmp52A;struct Cyc_Absyn_Vardecl*_tmp52D;struct Cyc_Absyn_Vardecl*_tmp530;struct Cyc_Absyn_Vardecl*_tmp533;_LL334:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp527;if(_tmp528->tag != 1)goto _LL336;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp529=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp528->f2);if(_tmp529->tag != 1)goto _LL336;else{_tmp52A=_tmp529->f1;}}}if(!(!_tmp52A->escapes))goto _LL336;_LL335:
 _tmp52D=_tmp52A;goto _LL337;_LL336:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp527;if(_tmp52B->tag != 1)goto _LL338;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp52C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp52B->f2);if(_tmp52C->tag != 4)goto _LL338;else{_tmp52D=_tmp52C->f1;}}}if(!(!_tmp52D->escapes))goto _LL338;_LL337:
 _tmp530=_tmp52D;goto _LL339;_LL338:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp52E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp527;if(_tmp52E->tag != 1)goto _LL33A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp52F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp52E->f2);if(_tmp52F->tag != 5)goto _LL33A;else{_tmp530=_tmp52F->f1;}}}if(!(!_tmp530->escapes))goto _LL33A;_LL339:
 _tmp533=_tmp530;goto _LL33B;_LL33A:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp531=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp527;if(_tmp531->tag != 1)goto _LL33C;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp532=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp531->f2);if(_tmp532->tag != 3)goto _LL33C;else{_tmp533=_tmp532->f1;}}}if(!(!_tmp533->escapes))goto _LL33C;_LL33B: {
# 1845
struct Cyc_CfFlowInfo_FlowEnv*_tmp534=env->fenv;
struct Cyc_CfFlowInfo_Reln*_tmp8E0;struct Cyc_List_List*_tmp8DF;struct Cyc_List_List*_tmp535=(_tmp8DF=_region_malloc(_tmp534->r,sizeof(*_tmp8DF)),((_tmp8DF->hd=((_tmp8E0=_region_malloc(_tmp534->r,sizeof(*_tmp8E0)),((_tmp8E0->vd=_tmp517,((_tmp8E0->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp533),_tmp8E0)))))),((_tmp8DF->tl=_tmp469,_tmp8DF)))));
struct _tuple16 _tmp8E1;struct _tuple16 _tmp537=(_tmp8E1.f1=Cyc_CfFlowInfo_ReachableFL(_tmp468,_tmp535),((_tmp8E1.f2=_tmp505,_tmp8E1)));_npop_handler(0);return _tmp537;}_LL33C:;_LL33D: {
struct _tuple16 _tmp8E2;struct _tuple16 _tmp53B=(_tmp8E2.f1=_tmp504,((_tmp8E2.f2=_tmp505,_tmp8E2)));_npop_handler(0);return _tmp53B;}_LL333:;}_LL331:;_LL332: {
# 1850
struct _tuple16 _tmp8E3;struct _tuple16 _tmp53D=(_tmp8E3.f1=_tmp504,((_tmp8E3.f2=_tmp505,_tmp8E3)));_npop_handler(0);return _tmp53D;}_LL32A:;}_LL328:;_LL329: {
# 1852
struct _tuple16 _tmp8E4;struct _tuple16 _tmp53F=(_tmp8E4.f1=_tmp504,((_tmp8E4.f2=_tmp505,_tmp8E4)));_npop_handler(0);return _tmp53F;}_LL31F:;};}default: _LL319: {
# 1854
struct _tuple16 _tmp8E5;struct _tuple16 _tmp541=(_tmp8E5.f1=f,((_tmp8E5.f2=f,_tmp8E5)));_npop_handler(0);return _tmp541;}}}_LL2B0:;};
# 1688
;_pop_region(temp);}_LL2AE:;_LL2AF:
# 1857
 goto _LL2A1;_LL2A1:;}{
# 1861
union Cyc_CfFlowInfo_FlowInfo _tmp543;void*_tmp544;struct _tuple14 _tmp542=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);_tmp543=_tmp542.f1;_tmp544=_tmp542.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp545=_tmp543;struct Cyc_Dict_Dict _tmp546;_LL340: if((_tmp545.BottomFL).tag != 1)goto _LL342;_LL341: {
struct _tuple16 _tmp8E6;return(_tmp8E6.f1=_tmp543,((_tmp8E6.f2=_tmp543,_tmp8E6)));}_LL342: if((_tmp545.ReachableFL).tag != 2)goto _LL33F;_tmp546=((struct _tuple13)(_tmp545.ReachableFL).val).f1;_LL343: {
# 1865
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp548=_tmp544;struct Cyc_Absyn_Vardecl*_tmp54A;void*_tmp54B;enum Cyc_CfFlowInfo_InitLevel _tmp554;_LL345: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp549=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp549->tag != 8)goto _LL347;else{_tmp54A=_tmp549->f1;_tmp54B=(void*)_tmp549->f2;}}_LL346:
 _tmp544=_tmp54B;locname=_tmp54A;goto retry;_LL347: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp54C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp54C->tag != 0)goto _LL349;}_LL348: {
struct _tuple16 _tmp8E7;return(_tmp8E7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8E7.f2=_tmp543,_tmp8E7)));}_LL349: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp54D=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp54D->tag != 2)goto _LL34B;}_LL34A:
 goto _LL34C;_LL34B: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp54E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp54E->tag != 1)goto _LL34D;}_LL34C:
 goto _LL34E;_LL34D: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp54F=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp54F->tag != 5)goto _LL34F;}_LL34E: {
struct _tuple16 _tmp8E8;return(_tmp8E8.f1=_tmp543,((_tmp8E8.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8E8)));}_LL34F: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp550=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp550->tag != 3)goto _LL351;else{if(_tmp550->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL351;}}_LL350:
 goto _LL352;_LL351: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp551=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp551->tag != 4)goto _LL353;else{if(_tmp551->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL353;}}_LL352:
 goto _LL354;_LL353: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp552=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp552->tag != 7)goto _LL355;}_LL354:
# 1875
{const char*_tmp8EB;void*_tmp8EA;(_tmp8EA=0,Cyc_Tcutil_terr(e->loc,((_tmp8EB="expression may not be initialized",_tag_dyneither(_tmp8EB,sizeof(char),34))),_tag_dyneither(_tmp8EA,sizeof(void*),0)));}{
struct _tuple16 _tmp8EC;return(_tmp8EC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8EC.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8EC)));};_LL355: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp553=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp553->tag != 3)goto _LL357;else{_tmp554=_tmp553->f1;}}_LL356:
# 1878
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp543,e,_tmp554,locname);_LL357: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp555=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp555->tag != 4)goto _LL359;}_LL358: {
struct _tuple16 _tmp8ED;return(_tmp8ED.f1=_tmp543,((_tmp8ED.f2=_tmp543,_tmp8ED)));}_LL359: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp556=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp548;if(_tmp556->tag != 6)goto _LL344;}_LL35A: {
struct Cyc_Core_Impossible_exn_struct _tmp8F3;const char*_tmp8F2;struct Cyc_Core_Impossible_exn_struct*_tmp8F1;(int)_throw((void*)((_tmp8F1=_cycalloc(sizeof(*_tmp8F1)),((_tmp8F1[0]=((_tmp8F3.tag=Cyc_Core_Impossible,((_tmp8F3.f1=((_tmp8F2="anal_test",_tag_dyneither(_tmp8F2,sizeof(char),10))),_tmp8F3)))),_tmp8F1)))));}_LL344:;}}_LL33F:;};};}struct _tuple23{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1885
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple23*ckenv,void*root,void*rval){
# 1887
unsigned int _tmp561;struct Cyc_NewControlFlow_AnalEnv*_tmp562;struct Cyc_Dict_Dict _tmp563;struct _tuple23*_tmp560=ckenv;_tmp561=_tmp560->f1;_tmp562=_tmp560->f2;_tmp563=_tmp560->f3;{
void*_tmp564=root;struct Cyc_Absyn_Vardecl*_tmp566;_LL35C: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp565=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp564;if(_tmp565->tag != 0)goto _LL35E;else{_tmp566=_tmp565->f1;}}_LL35D:
# 1890
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp562->fenv)->r,_tmp566->type)){
void*_tmp567=rval;_LL361: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp568=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp567;if(_tmp568->tag != 8)goto _LL363;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp569=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp568->f2);if(_tmp569->tag != 7)goto _LL363;}}_LL362:
 goto _LL364;_LL363: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp56A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp567;if(_tmp56A->tag != 8)goto _LL365;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp56B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp56A->f2);if(_tmp56B->tag != 0)goto _LL365;}}_LL364:
 goto _LL366;_LL365: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp56C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp567;if(_tmp56C->tag != 8)goto _LL367;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp56D=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp56C->f2);if(_tmp56D->tag != 3)goto _LL367;else{if(_tmp56D->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL367;}}}_LL366:
 goto _LL368;_LL367: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp56E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp567;if(_tmp56E->tag != 7)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp56F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp567;if(_tmp56F->tag != 0)goto _LL36B;}_LL36A:
 goto _LL36C;_LL36B: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp570=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp567;if(_tmp570->tag != 3)goto _LL36D;else{if(_tmp570->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL36D;}}_LL36C:
 goto _LL360;_LL36D:;_LL36E:
# 1899
{const char*_tmp8F7;void*_tmp8F6[1];struct Cyc_String_pa_PrintArg_struct _tmp8F5;(_tmp8F5.tag=0,((_tmp8F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp566->name)),((_tmp8F6[0]=& _tmp8F5,Cyc_Tcutil_warn(_tmp561,((_tmp8F7="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp8F7,sizeof(char),57))),_tag_dyneither(_tmp8F6,sizeof(void*),1)))))));}
goto _LL360;_LL360:;}
# 1902
goto _LL35B;_LL35E:;_LL35F:
 goto _LL35B;_LL35B:;};}
# 1907
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp574=flow;struct Cyc_Dict_Dict _tmp575;_LL370: if((_tmp574.BottomFL).tag != 1)goto _LL372;_LL371:
 return;_LL372: if((_tmp574.ReachableFL).tag != 2)goto _LL36F;_tmp575=((struct _tuple13)(_tmp574.ReachableFL).val).f1;_LL373:
# 1911
{struct Cyc_List_List*_tmp576=env->param_roots;for(0;_tmp576 != 0;_tmp576=_tmp576->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp575,Cyc_CfFlowInfo_lookup_place(_tmp575,(struct Cyc_CfFlowInfo_Place*)_tmp576->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp8FA;void*_tmp8F9;(_tmp8F9=0,Cyc_Tcutil_terr(loc,((_tmp8FA="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp8FA,sizeof(char),76))),_tag_dyneither(_tmp8F9,sizeof(void*),0)));}}}
# 1915
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple23 _tmp8FB;struct _tuple23 _tmp579=(_tmp8FB.f1=loc,((_tmp8FB.f2=env,((_tmp8FB.f3=_tmp575,_tmp8FB)))));
((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp579,_tmp575);}
# 1919
return;_LL36F:;}struct _tuple24{struct Cyc_CfFlowInfo_Place*f1;void*f2;};struct _tuple25{void*f1;void*f2;void*f3;};
# 1923
static void Cyc_NewControlFlow_check_noconsume_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
# 1925
union Cyc_CfFlowInfo_FlowInfo _tmp57B=flow;struct Cyc_Dict_Dict _tmp57C;_LL375: if((_tmp57B.BottomFL).tag != 1)goto _LL377;_LL376:
 return;_LL377: if((_tmp57B.ReachableFL).tag != 2)goto _LL374;_tmp57C=((struct _tuple13)(_tmp57B.ReachableFL).val).f1;_LL378: {
# 1928
struct Cyc_Absyn_Exp*_tmp57D=Cyc_Absyn_exp_unsigned_one;
int _tmp57E=0;
int _tmp57F=1;
{struct Cyc_List_List*_tmp580=env->noconsume_params;for(0;_tmp580 != 0;_tmp580=_tmp580->tl){
# 1934
struct Cyc_CfFlowInfo_Place*_tmp582;void*_tmp583;struct _tuple24*_tmp581=(struct _tuple24*)_tmp580->hd;_tmp582=_tmp581->f1;_tmp583=_tmp581->f2;{
void*_tmp584=Cyc_CfFlowInfo_lookup_place(_tmp57C,_tmp582);
void*_tmp586;struct Cyc_CfFlowInfo_Place*_tmp585=_tmp582;_tmp586=_tmp585->root;{
struct _tuple25 _tmp8FC;struct _tuple25 _tmp588=(_tmp8FC.f1=_tmp586,((_tmp8FC.f2=_tmp583,((_tmp8FC.f3=_tmp584,_tmp8FC)))));struct Cyc_Absyn_Vardecl*_tmp58A;struct Cyc_Absyn_Vardecl*_tmp58C;void*_tmp58D;struct Cyc_Absyn_Vardecl*_tmp58F;void*_tmp590;struct Cyc_Absyn_Vardecl*_tmp592;_LL37A:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp589=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp588.f1;if(_tmp589->tag != 0)goto _LL37C;else{_tmp58A=_tmp589->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp58B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp588.f2;if(_tmp58B->tag != 8)goto _LL37C;else{_tmp58C=_tmp58B->f1;_tmp58D=(void*)_tmp58B->f2;}};{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp58E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp588.f3;if(_tmp58E->tag != 8)goto _LL37C;else{_tmp58F=_tmp58E->f1;_tmp590=(void*)_tmp58E->f2;}};_LL37B:
# 1939
 if(_tmp58C == _tmp58F){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp57D,_tmp57F,_tmp590,& _tmp57E)){
const char*_tmp900;void*_tmp8FF[1];struct Cyc_String_pa_PrintArg_struct _tmp8FE;(_tmp8FE.tag=0,((_tmp8FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1943
Cyc_Absynpp_qvar2string(_tmp58A->name)),((_tmp8FF[0]=& _tmp8FE,Cyc_Tcutil_terr(loc,((_tmp900="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp900,sizeof(char),58))),_tag_dyneither(_tmp8FF,sizeof(void*),1)))))));}else{
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp57C,_tmp590)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp904;void*_tmp903[1];struct Cyc_String_pa_PrintArg_struct _tmp902;(_tmp902.tag=0,((_tmp902.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1947
Cyc_Absynpp_qvar2string(_tmp58A->name)),((_tmp903[0]=& _tmp902,Cyc_Tcutil_terr(loc,((_tmp904="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp904,sizeof(char),84))),_tag_dyneither(_tmp903,sizeof(void*),1)))))));}}}else{
# 1951
_tmp592=_tmp58A;goto _LL37D;}
goto _LL379;_LL37C: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp591=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp588.f1;if(_tmp591->tag != 0)goto _LL37E;else{_tmp592=_tmp591->f1;}}_LL37D:
# 1954
{const char*_tmp908;void*_tmp907[1];struct Cyc_String_pa_PrintArg_struct _tmp906;(_tmp906.tag=0,((_tmp906.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1956
Cyc_Absynpp_qvar2string(_tmp592->name)),((_tmp907[0]=& _tmp906,Cyc_Tcutil_terr(loc,((_tmp908="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp908,sizeof(char),85))),_tag_dyneither(_tmp907,sizeof(void*),1)))))));}
goto _LL379;_LL37E:;_LL37F: {
# 1959
const char*_tmp90B;void*_tmp90A;(_tmp90A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp90B="check_noconsume_params: root is not a varroot",_tag_dyneither(_tmp90B,sizeof(char),46))),_tag_dyneither(_tmp90A,sizeof(void*),0)));}_LL379:;};};}}
# 1962
return;}_LL374:;}
# 1969
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs){
# 1971
struct Cyc_CfFlowInfo_FlowEnv*_tmp59E=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Core_Opt*_tmp5A0;struct Cyc_Absyn_Exp*_tmp5A1;struct Cyc_Absyn_Stmt*_tmp5A2;unsigned int _tmp5A3;struct Cyc_Absyn_Switch_clause*_tmp59F=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp5A0=_tmp59F->pat_vars;_tmp5A1=_tmp59F->where_clause;_tmp5A2=_tmp59F->body;_tmp5A3=_tmp59F->loc;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_add_vars(_tmp59E,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A0))->v,_tmp59E->unknown_all,_tmp5A3,0);
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
if(_tmp5A1 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp5A1;
union Cyc_CfFlowInfo_FlowInfo _tmp5A5;union Cyc_CfFlowInfo_FlowInfo _tmp5A6;struct _tuple16 _tmp5A4=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);_tmp5A5=_tmp5A4.f1;_tmp5A6=_tmp5A4.f2;
inflow=_tmp5A6;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5A5,_tmp5A2);}else{
# 1982
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp5A2);}{
# 1984
union Cyc_CfFlowInfo_FlowInfo _tmp5A7=clause_outflow;_LL381: if((_tmp5A7.BottomFL).tag != 1)goto _LL383;_LL382:
 goto _LL380;_LL383:;_LL384:
# 1988
 if(scs->tl == 0)
return clause_outflow;else{
# 1993
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp90E;void*_tmp90D;(_tmp90D=0,Cyc_Tcutil_terr(_tmp5A2->loc,((_tmp90E="switch clause may implicitly fallthru",_tag_dyneither(_tmp90E,sizeof(char),38))),_tag_dyneither(_tmp90D,sizeof(void*),0)));}else{
# 1996
const char*_tmp911;void*_tmp910;(_tmp910=0,Cyc_Tcutil_warn(_tmp5A2->loc,((_tmp911="switch clause may implicitly fallthru",_tag_dyneither(_tmp911,sizeof(char),38))),_tag_dyneither(_tmp910,sizeof(void*),0)));}
# 1998
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2000
goto _LL380;_LL380:;};};}
# 2003
return Cyc_CfFlowInfo_BottomFL();}struct _tuple26{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2008
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple26*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2010
struct Cyc_NewControlFlow_AnalEnv*_tmp5AD;struct Cyc_Dict_Dict _tmp5AE;unsigned int _tmp5AF;struct _tuple26*_tmp5AC=vdenv;_tmp5AD=_tmp5AC->f1;_tmp5AE=_tmp5AC->f2;_tmp5AF=_tmp5AC->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5AD->fenv)->r,vd->type)){
# 2013
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp912;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp912.tag=0,((_tmp912.f1=vd,_tmp912)));
# 2015
void*_tmp5B0=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp5AE,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp5B1=_tmp5B0;_LL386: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5B2=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5B1;if(_tmp5B2->tag != 8)goto _LL388;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5B3=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5B2->f2);if(_tmp5B3->tag != 7)goto _LL388;}}_LL387:
 goto _LL389;_LL388: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5B4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5B1;if(_tmp5B4->tag != 8)goto _LL38A;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5B5=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5B4->f2);if(_tmp5B5->tag != 0)goto _LL38A;}}_LL389:
 goto _LL38B;_LL38A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5B6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5B1;if(_tmp5B6->tag != 8)goto _LL38C;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5B7=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5B6->f2);if(_tmp5B7->tag != 3)goto _LL38C;else{if(_tmp5B7->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38C;}}}_LL38B:
 goto _LL38D;_LL38C: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5B8=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5B1;if(_tmp5B8->tag != 7)goto _LL38E;}_LL38D:
 goto _LL38F;_LL38E: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5B9=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5B1;if(_tmp5B9->tag != 0)goto _LL390;}_LL38F:
 goto _LL391;_LL390: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5BA=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5B1;if(_tmp5BA->tag != 3)goto _LL392;else{if(_tmp5BA->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL392;}}_LL391:
 goto _LL385;_LL392:;_LL393:
# 2024
{const char*_tmp916;void*_tmp915[1];struct Cyc_String_pa_PrintArg_struct _tmp914;(_tmp914.tag=0,((_tmp914.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp915[0]=& _tmp914,Cyc_Tcutil_warn(_tmp5AF,((_tmp916="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp916,sizeof(char),68))),_tag_dyneither(_tmp915,sizeof(void*),1)))))));}
# 2026
goto _LL385;_LL385:;}}
# 2031
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2033
{union Cyc_CfFlowInfo_FlowInfo _tmp5BF=inflow;struct Cyc_Dict_Dict _tmp5C0;_LL395: if((_tmp5BF.ReachableFL).tag != 2)goto _LL397;_tmp5C0=((struct _tuple13)(_tmp5BF.ReachableFL).val).f1;_LL396: {
# 2035
struct _tuple26 _tmp917;struct _tuple26 _tmp5C1=(_tmp917.f1=env,((_tmp917.f2=_tmp5C0,((_tmp917.f3=decl->loc,_tmp917)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp5C2=env->fenv;
{void*_tmp5C3=decl->r;struct Cyc_Absyn_Vardecl*_tmp5C5;struct Cyc_List_List*_tmp5C7;struct Cyc_List_List*_tmp5C9;_LL39A: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5C4=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5C3;if(_tmp5C4->tag != 0)goto _LL39C;else{_tmp5C5=_tmp5C4->f1;}}_LL39B:
# 2039
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp5C1,_tmp5C5);
goto _LL399;_LL39C: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp5C6=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5C3;if(_tmp5C6->tag != 2)goto _LL39E;else{if(_tmp5C6->f2 == 0)goto _LL39E;_tmp5C7=(struct Cyc_List_List*)(_tmp5C6->f2)->v;}}_LL39D:
 _tmp5C9=_tmp5C7;goto _LL39F;_LL39E: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp5C8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5C3;if(_tmp5C8->tag != 3)goto _LL3A0;else{_tmp5C9=_tmp5C8->f1;}}_LL39F:
# 2043
((void(*)(void(*f)(struct _tuple26*,struct Cyc_Absyn_Vardecl*),struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp5C1,_tmp5C9);
goto _LL399;_LL3A0:;_LL3A1:
 goto _LL399;_LL399:;}
# 2047
goto _LL394;}_LL397:;_LL398:
 goto _LL394;_LL394:;}
# 2050
return;}
# 2053
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo outflow;
# 2058
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5CC;union Cyc_CfFlowInfo_FlowInfo*_tmp5CD;struct _tuple17 _tmp5CB=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp5CC=_tmp5CB.f1;_tmp5CD=_tmp5CB.f2;
inflow=*_tmp5CD;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp5CE=env->fenv;
void*_tmp5CF=s->r;struct Cyc_Absyn_Exp*_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D5;struct Cyc_Absyn_Stmt*_tmp5D7;struct Cyc_Absyn_Stmt*_tmp5D8;struct Cyc_Absyn_Exp*_tmp5DA;struct Cyc_Absyn_Stmt*_tmp5DB;struct Cyc_Absyn_Stmt*_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DE;struct Cyc_Absyn_Stmt*_tmp5DF;struct Cyc_Absyn_Stmt*_tmp5E0;struct Cyc_Absyn_Stmt*_tmp5E2;struct Cyc_Absyn_Exp*_tmp5E3;struct Cyc_Absyn_Stmt*_tmp5E4;struct Cyc_Absyn_Exp*_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E7;struct Cyc_Absyn_Stmt*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5E9;struct Cyc_Absyn_Stmt*_tmp5EA;struct Cyc_Absyn_Stmt*_tmp5EB;struct Cyc_List_List*_tmp5EE;struct Cyc_Absyn_Switch_clause*_tmp5EF;struct Cyc_Absyn_Stmt*_tmp5F1;struct Cyc_Absyn_Stmt*_tmp5F3;struct Cyc_Absyn_Stmt*_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F7;struct Cyc_List_List*_tmp5F8;struct Cyc_Absyn_Stmt*_tmp5FA;struct Cyc_List_List*_tmp5FB;struct Cyc_Absyn_Tvar*_tmp5FE;struct Cyc_Absyn_Vardecl*_tmp5FF;struct Cyc_Absyn_Stmt*_tmp600;struct Cyc_Absyn_Decl*_tmp602;struct Cyc_Absyn_Stmt*_tmp603;struct Cyc_Absyn_Stmt*_tmp605;struct Cyc_Absyn_Exp*_tmp607;_LL3A3: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp5D0=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5D0->tag != 0)goto _LL3A5;}_LL3A4:
 return inflow;_LL3A5: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp5D1=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5D1->tag != 3)goto _LL3A7;else{if(_tmp5D1->f1 != 0)goto _LL3A7;}}_LL3A6:
# 2064
 if(env->noreturn){
const char*_tmp91A;void*_tmp919;(_tmp919=0,Cyc_Tcutil_terr(s->loc,((_tmp91A="`noreturn' function might return",_tag_dyneither(_tmp91A,sizeof(char),33))),_tag_dyneither(_tmp919,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL3A7: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp5D2=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5D2->tag != 3)goto _LL3A9;else{_tmp5D3=_tmp5D2->f1;}}_LL3A8:
# 2070
 if(env->noreturn){
const char*_tmp91D;void*_tmp91C;(_tmp91C=0,Cyc_Tcutil_terr(s->loc,((_tmp91D="`noreturn' function might return",_tag_dyneither(_tmp91D,sizeof(char),33))),_tag_dyneither(_tmp91C,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp60D;void*_tmp60E;struct _tuple14 _tmp60C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5D3));_tmp60D=_tmp60C.f1;_tmp60E=_tmp60C.f2;
_tmp60D=Cyc_NewControlFlow_use_Rval(env,_tmp5D3->loc,_tmp60D,_tmp60E);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp60D);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,_tmp60D);
return Cyc_CfFlowInfo_BottomFL();};_LL3A9: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp5D4=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5D4->tag != 1)goto _LL3AB;else{_tmp5D5=_tmp5D4->f1;}}_LL3AA:
# 2078
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5D5)).f1;_LL3AB: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp5D6=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5D6->tag != 2)goto _LL3AD;else{_tmp5D7=_tmp5D6->f1;_tmp5D8=_tmp5D6->f2;}}_LL3AC:
# 2081
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5D7),_tmp5D8);_LL3AD: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp5D9=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5D9->tag != 4)goto _LL3AF;else{_tmp5DA=_tmp5D9->f1;_tmp5DB=_tmp5D9->f2;_tmp5DC=_tmp5D9->f3;}}_LL3AE: {
# 2084
union Cyc_CfFlowInfo_FlowInfo _tmp610;union Cyc_CfFlowInfo_FlowInfo _tmp611;struct _tuple16 _tmp60F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5DA);_tmp610=_tmp60F.f1;_tmp611=_tmp60F.f2;{
# 2091
union Cyc_CfFlowInfo_FlowInfo _tmp612=Cyc_NewControlFlow_anal_stmt(env,_tmp611,_tmp5DC);
union Cyc_CfFlowInfo_FlowInfo _tmp613=Cyc_NewControlFlow_anal_stmt(env,_tmp610,_tmp5DB);
return Cyc_CfFlowInfo_join_flow(_tmp5CE,env->all_changed,_tmp613,_tmp612);};}_LL3AF: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp5DD=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5DD->tag != 5)goto _LL3B1;else{_tmp5DE=(_tmp5DD->f1).f1;_tmp5DF=(_tmp5DD->f1).f2;_tmp5E0=_tmp5DD->f2;}}_LL3B0: {
# 2099
union Cyc_CfFlowInfo_FlowInfo*_tmp615;struct _tuple17 _tmp614=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp5DF);_tmp615=_tmp614.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp616=*_tmp615;
union Cyc_CfFlowInfo_FlowInfo _tmp618;union Cyc_CfFlowInfo_FlowInfo _tmp619;struct _tuple16 _tmp617=Cyc_NewControlFlow_anal_test(env,_tmp616,_tmp5DE);_tmp618=_tmp617.f1;_tmp619=_tmp617.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp61A=Cyc_NewControlFlow_anal_stmt(env,_tmp618,_tmp5E0);
Cyc_NewControlFlow_update_flow(env,_tmp5DF,_tmp61A);
return _tmp619;};};}_LL3B1: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp5E1=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5E1->tag != 14)goto _LL3B3;else{_tmp5E2=_tmp5E1->f1;_tmp5E3=(_tmp5E1->f2).f1;_tmp5E4=(_tmp5E1->f2).f2;}}_LL3B2: {
# 2109
union Cyc_CfFlowInfo_FlowInfo _tmp61B=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E2);
union Cyc_CfFlowInfo_FlowInfo*_tmp61D;struct _tuple17 _tmp61C=Cyc_NewControlFlow_pre_stmt_check(env,_tmp61B,_tmp5E4);_tmp61D=_tmp61C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp61E=*_tmp61D;
union Cyc_CfFlowInfo_FlowInfo _tmp620;union Cyc_CfFlowInfo_FlowInfo _tmp621;struct _tuple16 _tmp61F=Cyc_NewControlFlow_anal_test(env,_tmp61E,_tmp5E3);_tmp620=_tmp61F.f1;_tmp621=_tmp61F.f2;
Cyc_NewControlFlow_update_flow(env,_tmp5E2,_tmp620);
return _tmp621;};}_LL3B3: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp5E5=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5E5->tag != 9)goto _LL3B5;else{_tmp5E6=_tmp5E5->f1;_tmp5E7=(_tmp5E5->f2).f1;_tmp5E8=(_tmp5E5->f2).f2;_tmp5E9=(_tmp5E5->f3).f1;_tmp5EA=(_tmp5E5->f3).f2;_tmp5EB=_tmp5E5->f4;}}_LL3B4: {
# 2118
union Cyc_CfFlowInfo_FlowInfo _tmp622=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5E6)).f1;
union Cyc_CfFlowInfo_FlowInfo*_tmp624;struct _tuple17 _tmp623=Cyc_NewControlFlow_pre_stmt_check(env,_tmp622,_tmp5E8);_tmp624=_tmp623.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp625=*_tmp624;
union Cyc_CfFlowInfo_FlowInfo _tmp627;union Cyc_CfFlowInfo_FlowInfo _tmp628;struct _tuple16 _tmp626=Cyc_NewControlFlow_anal_test(env,_tmp625,_tmp5E7);_tmp627=_tmp626.f1;_tmp628=_tmp626.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp629=Cyc_NewControlFlow_anal_stmt(env,_tmp627,_tmp5EB);
union Cyc_CfFlowInfo_FlowInfo*_tmp62B;struct _tuple17 _tmp62A=Cyc_NewControlFlow_pre_stmt_check(env,_tmp629,_tmp5EA);_tmp62B=_tmp62A.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp62C=*_tmp62B;
union Cyc_CfFlowInfo_FlowInfo _tmp62D=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp62C,_tmp5E9)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp5E8,_tmp62D);
return _tmp628;};};};}_LL3B5: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp5EC=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5EC->tag != 6)goto _LL3B7;else{if(_tmp5EC->f1 != 0)goto _LL3B7;}}_LL3B6:
# 2130
 return Cyc_CfFlowInfo_BottomFL();_LL3B7: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp5ED=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5ED->tag != 11)goto _LL3B9;else{_tmp5EE=_tmp5ED->f1;if(_tmp5ED->f2 == 0)goto _LL3B9;_tmp5EF=*_tmp5ED->f2;}}_LL3B8: {
# 2132
struct _RegionHandle _tmp62E=_new_region("temp");struct _RegionHandle*temp=& _tmp62E;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp630;struct Cyc_List_List*_tmp631;struct _tuple20 _tmp62F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp5EE,1,1);_tmp630=_tmp62F.f1;_tmp631=_tmp62F.f2;
# 2135
for(0;_tmp631 != 0;(_tmp631=_tmp631->tl,_tmp5EE=_tmp5EE->tl)){
_tmp630=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5EE))->hd)->loc,_tmp630,(void*)_tmp631->hd);}
# 2138
_tmp630=Cyc_NewControlFlow_add_vars(_tmp5CE,_tmp630,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5EF->pat_vars))->v,_tmp5CE->unknown_all,s->loc,0);
Cyc_NewControlFlow_update_flow(env,_tmp5EF->body,_tmp630);{
union Cyc_CfFlowInfo_FlowInfo _tmp632=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp632;};}
# 2133
;_pop_region(temp);}_LL3B9: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp5F0=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5F0->tag != 6)goto _LL3BB;else{_tmp5F1=_tmp5F0->f1;}}_LL3BA:
# 2142
 _tmp5F3=_tmp5F1;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp5F2=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5F2->tag != 7)goto _LL3BD;else{_tmp5F3=_tmp5F2->f1;}}_LL3BC:
 _tmp5F5=_tmp5F3;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp5F4=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5F4->tag != 8)goto _LL3BF;else{_tmp5F5=_tmp5F4->f2;}}_LL3BE:
# 2147
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp633=_tmp5CC->encloser;
struct Cyc_Absyn_Stmt*_tmp634=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp5F5)))->encloser;
while(_tmp634 != _tmp633){
struct Cyc_Absyn_Stmt*_tmp635=(Cyc_NewControlFlow_get_stmt_annot(_tmp633))->encloser;
if(_tmp635 == _tmp633){
{const char*_tmp920;void*_tmp91F;(_tmp91F=0,Cyc_Tcutil_terr(s->loc,((_tmp920="goto enters local scope or exception handler",_tag_dyneither(_tmp920,sizeof(char),45))),_tag_dyneither(_tmp91F,sizeof(void*),0)));}
break;}
# 2156
_tmp633=_tmp635;}}
# 2161
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp5F5),inflow);
return Cyc_CfFlowInfo_BottomFL();_LL3BF: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5F6=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5F6->tag != 10)goto _LL3C1;else{_tmp5F7=_tmp5F6->f1;_tmp5F8=_tmp5F6->f2;}}_LL3C0: {
# 2165
union Cyc_CfFlowInfo_FlowInfo _tmp639;void*_tmp63A;struct _tuple14 _tmp638=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5F7);_tmp639=_tmp638.f1;_tmp63A=_tmp638.f2;
_tmp639=Cyc_NewControlFlow_use_Rval(env,_tmp5F7->loc,_tmp639,_tmp63A);
return Cyc_NewControlFlow_anal_scs(env,_tmp639,_tmp5F8);}_LL3C1: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp5F9=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5F9->tag != 15)goto _LL3C3;else{_tmp5FA=_tmp5F9->f1;_tmp5FB=_tmp5F9->f2;}}_LL3C2: {
# 2172
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5FA);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2180
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2183
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp5FB);
{union Cyc_CfFlowInfo_FlowInfo _tmp63B=scs_outflow;_LL3CE: if((_tmp63B.BottomFL).tag != 1)goto _LL3D0;_LL3CF:
 goto _LL3CD;_LL3D0:;_LL3D1: {
const char*_tmp923;void*_tmp922;(_tmp922=0,Cyc_Tcutil_terr(s->loc,((_tmp923="last catch clause may implicitly fallthru",_tag_dyneither(_tmp923,sizeof(char),42))),_tag_dyneither(_tmp922,sizeof(void*),0)));}_LL3CD:;}
# 2189
outflow=s1_outflow;
# 2191
return outflow;};};}_LL3C3: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp5FC=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp5FC->tag != 12)goto _LL3C5;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp5FD=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp5FC->f1)->r;if(_tmp5FD->tag != 5)goto _LL3C5;else{_tmp5FE=_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;}}_tmp600=_tmp5FC->f2;}}_LL3C4: {
# 2197
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp5FF->initializer);
union Cyc_CfFlowInfo_FlowInfo _tmp63F;union Cyc_CfFlowInfo_AbsLVal _tmp640;struct _tuple15 _tmp63E=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp63F=_tmp63E.f1;_tmp640=_tmp63E.f2;{
# 2202
struct _tuple15 _tmp924;struct _tuple15 _tmp642=(_tmp924.f1=_tmp63F,((_tmp924.f2=_tmp640,_tmp924)));struct Cyc_Dict_Dict _tmp643;struct Cyc_List_List*_tmp644;struct Cyc_CfFlowInfo_Place*_tmp645;_LL3D3: if(((_tmp642.f1).ReachableFL).tag != 2)goto _LL3D5;_tmp643=((struct _tuple13)((_tmp642.f1).ReachableFL).val).f1;_tmp644=((struct _tuple13)((_tmp642.f1).ReachableFL).val).f2;if(((_tmp642.f2).PlaceL).tag != 1)goto _LL3D5;_tmp645=(struct Cyc_CfFlowInfo_Place*)((_tmp642.f2).PlaceL).val;_LL3D4: {
# 2204
void*_tmp646=Cyc_CfFlowInfo_lookup_place(_tmp643,_tmp645);
# 2206
void*t=(void*)_check_null(e->topt);
# 2213
void*_tmp647=Cyc_CfFlowInfo_make_unique_consumed(_tmp5CE,t,e,env->iteration_num,_tmp646);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp927;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp926;_tmp647=(void*)((_tmp926=_region_malloc(_tmp5CE->r,sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=8,((_tmp927.f1=_tmp5FF,((_tmp927.f2=_tmp647,_tmp927)))))),_tmp926))));}
_tmp643=Cyc_CfFlowInfo_assign_place(_tmp5CE,e->loc,_tmp643,env->all_changed,_tmp645,_tmp647);
_tmp63F=Cyc_CfFlowInfo_ReachableFL(_tmp643,_tmp644);{
# 2220
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp5CE,_tmp643,_tmp646)){case Cyc_CfFlowInfo_AllIL: _LL3D9:
 leaf=_tmp5CE->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL3DA:
 leaf=_tmp5CE->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL3DB:
 leaf=_tmp5CE->unknown_this;break;}{
# 2228
void*_tmp64A=Cyc_CfFlowInfo_typ_to_absrval(_tmp5CE,t,0,leaf);
_tmp63F=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp63F,_tmp646);{
struct Cyc_List_List _tmp928;struct Cyc_List_List _tmp64B=(_tmp928.hd=_tmp5FF,((_tmp928.tl=0,_tmp928)));
_tmp63F=Cyc_NewControlFlow_add_vars(_tmp5CE,_tmp63F,& _tmp64B,_tmp5CE->unknown_all,s->loc,0);
# 2235
_tmp63F=Cyc_NewControlFlow_anal_stmt(env,_tmp63F,_tmp600);{
# 2240
union Cyc_CfFlowInfo_FlowInfo _tmp64C=_tmp63F;struct Cyc_Dict_Dict _tmp64D;struct Cyc_List_List*_tmp64E;_LL3DE: if((_tmp64C.ReachableFL).tag != 2)goto _LL3E0;_tmp64D=((struct _tuple13)(_tmp64C.ReachableFL).val).f1;_tmp64E=((struct _tuple13)(_tmp64C.ReachableFL).val).f2;_LL3DF:
# 2243
{void*_tmp64F=Cyc_CfFlowInfo_lookup_place(_tmp64D,_tmp645);struct Cyc_Absyn_Vardecl*_tmp651;_LL3E3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp650=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp64F;if(_tmp650->tag != 8)goto _LL3E5;else{_tmp651=_tmp650->f1;}}_LL3E4:
# 2245
 if(_tmp651 != _tmp5FF)goto _LL3E6;
goto _LL3E2;_LL3E5:;_LL3E6:
# 2248
{const char*_tmp92B;void*_tmp92A;(_tmp92A=0,Cyc_Tcutil_terr(s->loc,((_tmp92B="aliased location was overwritten",_tag_dyneither(_tmp92B,sizeof(char),33))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}
goto _LL3E2;_LL3E2:;}
# 2251
_tmp64D=Cyc_CfFlowInfo_assign_place(_tmp5CE,s->loc,_tmp64D,env->all_changed,_tmp645,_tmp64A);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp64D,_tmp64E);
# 2256
return outflow;_LL3E0:;_LL3E1:
 return _tmp63F;_LL3DD:;};};};};}_LL3D5: if(((_tmp642.f1).BottomFL).tag != 1)goto _LL3D7;_LL3D6:
# 2259
 return _tmp63F;_LL3D7:;_LL3D8:
{const char*_tmp92E;void*_tmp92D;(_tmp92D=0,Cyc_Tcutil_terr(e->loc,((_tmp92E="bad alias expression--no unique path found",_tag_dyneither(_tmp92E,sizeof(char),43))),_tag_dyneither(_tmp92D,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL3D2:;};}_LL3C5: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp601=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp601->tag != 12)goto _LL3C7;else{_tmp602=_tmp601->f1;_tmp603=_tmp601->f2;}}_LL3C6:
# 2265
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp602),_tmp603);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp602);
return outflow;_LL3C7: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp604=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp604->tag != 13)goto _LL3C9;else{_tmp605=_tmp604->f2;}}_LL3C8:
# 2270
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp605);_LL3C9: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp606=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp5CF;if(_tmp606->tag != 16)goto _LL3CB;else{_tmp607=_tmp606->f1;}}_LL3CA: {
# 2273
union Cyc_CfFlowInfo_FlowInfo _tmp658;void*_tmp659;struct _tuple14 _tmp657=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp607);_tmp658=_tmp657.f1;_tmp659=_tmp657.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp65A=Cyc_NewControlFlow_use_Rval(env,_tmp607->loc,_tmp658,_tmp659);
void*_tmp65B=Cyc_Tcutil_compress((void*)_check_null(_tmp607->topt));void*_tmp65D;_LL3E8: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp65C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp65B;if(_tmp65C->tag != 15)goto _LL3EA;else{_tmp65D=(void*)_tmp65C->f1;}}_LL3E9:
# 2277
 return Cyc_CfFlowInfo_kill_flow_region(_tmp5CE,_tmp658,_tmp65D);_LL3EA:;_LL3EB: {
struct Cyc_Core_Impossible_exn_struct _tmp934;const char*_tmp933;struct Cyc_Core_Impossible_exn_struct*_tmp932;(int)_throw((void*)((_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932[0]=((_tmp934.tag=Cyc_Core_Impossible,((_tmp934.f1=((_tmp933="anal_stmt -- reset_region",_tag_dyneither(_tmp933,sizeof(char),26))),_tmp934)))),_tmp932)))));}_LL3E7:;};}_LL3CB:;_LL3CC: {
# 2281
struct Cyc_Core_Impossible_exn_struct _tmp93A;const char*_tmp939;struct Cyc_Core_Impossible_exn_struct*_tmp938;(int)_throw((void*)((_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938[0]=((_tmp93A.tag=Cyc_Core_Impossible,((_tmp93A.f1=((_tmp939="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp939,sizeof(char),56))),_tmp93A)))),_tmp938)))));}_LL3A2:;};}
# 2286
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2288
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp664=env->fenv;
void*_tmp665=decl->r;struct Cyc_Absyn_Vardecl*_tmp667;struct Cyc_List_List*_tmp669;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_List_List*_tmp66C;struct Cyc_Absyn_Fndecl*_tmp66E;struct Cyc_Absyn_Tvar*_tmp670;struct Cyc_Absyn_Vardecl*_tmp671;int _tmp672;struct Cyc_Absyn_Exp*_tmp673;_LL3ED: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp666=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp665;if(_tmp666->tag != 0)goto _LL3EF;else{_tmp667=_tmp666->f1;}}_LL3EE: {
# 2298
struct Cyc_List_List _tmp93B;struct Cyc_List_List _tmp674=(_tmp93B.hd=_tmp667,((_tmp93B.tl=0,_tmp93B)));
inflow=Cyc_NewControlFlow_add_vars(_tmp664,inflow,& _tmp674,_tmp664->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp675=_tmp667->initializer;
if(_tmp675 == 0)
return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp677;void*_tmp678;struct _tuple14 _tmp676=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp675);_tmp677=_tmp676.f1;_tmp678=_tmp676.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp679=_tmp677;struct Cyc_Dict_Dict _tmp67A;struct Cyc_List_List*_tmp67B;_LL3FA: if((_tmp679.BottomFL).tag != 1)goto _LL3FC;_LL3FB:
 return Cyc_CfFlowInfo_BottomFL();_LL3FC: if((_tmp679.ReachableFL).tag != 2)goto _LL3F9;_tmp67A=((struct _tuple13)(_tmp679.ReachableFL).val).f1;_tmp67B=((struct _tuple13)(_tmp679.ReachableFL).val).f2;_LL3FD:
# 2307
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp941;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp940;struct Cyc_CfFlowInfo_Place*_tmp93F;_tmp67A=Cyc_CfFlowInfo_assign_place(_tmp664,decl->loc,_tmp67A,env->all_changed,(
(_tmp93F=_region_malloc(env->r,sizeof(*_tmp93F)),((_tmp93F->root=(void*)((_tmp941=_region_malloc(env->r,sizeof(*_tmp941)),((_tmp941[0]=((_tmp940.tag=0,((_tmp940.f1=_tmp667,_tmp940)))),_tmp941)))),((_tmp93F->fields=0,_tmp93F)))))),_tmp678);}
# 2310
_tmp67B=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp67B,_tmp667,(struct Cyc_Absyn_Exp*)_check_null(_tmp667->initializer));{
union Cyc_CfFlowInfo_FlowInfo _tmp67F=Cyc_CfFlowInfo_ReachableFL(_tmp67A,_tmp67B);
Cyc_NewControlFlow_update_tryflow(env,_tmp67F);
# 2315
return _tmp67F;};_LL3F9:;};};};}_LL3EF: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp668=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp665;if(_tmp668->tag != 2)goto _LL3F1;else{if(_tmp668->f2 == 0)goto _LL3F1;_tmp669=(struct Cyc_List_List*)(_tmp668->f2)->v;_tmp66A=_tmp668->f3;}}_LL3F0: {
# 2320
union Cyc_CfFlowInfo_FlowInfo _tmp682;void*_tmp683;struct _tuple14 _tmp681=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp66A);_tmp682=_tmp681.f1;_tmp683=_tmp681.f2;
_tmp682=Cyc_NewControlFlow_use_Rval(env,_tmp66A->loc,_tmp682,_tmp683);
return Cyc_NewControlFlow_add_vars(_tmp664,_tmp682,_tmp669,_tmp664->unknown_all,decl->loc,0);}_LL3F1: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp66B=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp665;if(_tmp66B->tag != 3)goto _LL3F3;else{_tmp66C=_tmp66B->f1;}}_LL3F2:
# 2325
 return Cyc_NewControlFlow_add_vars(_tmp664,inflow,_tmp66C,_tmp664->unknown_none,decl->loc,0);_LL3F3: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp66D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp665;if(_tmp66D->tag != 1)goto _LL3F5;else{_tmp66E=_tmp66D->f1;}}_LL3F4:
# 2328
 Cyc_NewControlFlow_check_nested_fun(_tmp664,inflow,_tmp66E);{
void*t=(void*)_check_null(_tmp66E->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp684=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp66E->fn_vardecl);
# 2334
struct Cyc_List_List*_tmp942;return Cyc_NewControlFlow_add_vars(_tmp664,inflow,((_tmp942=_region_malloc(env->r,sizeof(*_tmp942)),((_tmp942->hd=_tmp684,((_tmp942->tl=0,_tmp942)))))),_tmp664->unknown_all,decl->loc,0);};_LL3F5: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp66F=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp665;if(_tmp66F->tag != 4)goto _LL3F7;else{_tmp670=_tmp66F->f1;_tmp671=_tmp66F->f2;_tmp672=_tmp66F->f3;_tmp673=_tmp66F->f4;}}_LL3F6:
# 2337
 if(_tmp673 != 0){
struct Cyc_Absyn_Exp*_tmp686=_tmp673;
union Cyc_CfFlowInfo_FlowInfo _tmp688;void*_tmp689;struct _tuple14 _tmp687=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp686);_tmp688=_tmp687.f1;_tmp689=_tmp687.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp686->loc,_tmp688,_tmp689);}{
# 2342
struct Cyc_List_List _tmp943;struct Cyc_List_List _tmp68A=(_tmp943.hd=_tmp671,((_tmp943.tl=0,_tmp943)));
return Cyc_NewControlFlow_add_vars(_tmp664,inflow,& _tmp68A,_tmp664->unknown_all,decl->loc,0);};_LL3F7:;_LL3F8: {
# 2345
struct Cyc_Core_Impossible_exn_struct _tmp949;const char*_tmp948;struct Cyc_Core_Impossible_exn_struct*_tmp947;(int)_throw((void*)((_tmp947=_cycalloc(sizeof(*_tmp947)),((_tmp947[0]=((_tmp949.tag=Cyc_Core_Impossible,((_tmp949.f1=((_tmp948="anal_decl: unexpected decl variant",_tag_dyneither(_tmp948,sizeof(char),35))),_tmp949)))),_tmp947)))));}_LL3EC:;}
# 2385 "new_control_flow.cyc"
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp68F=_new_region("frgn");struct _RegionHandle*frgn=& _tmp68F;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2387
;_pop_region(frgn);}
# 2392
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2396
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2400
struct _RegionHandle*_tmp690=fenv->r;
unsigned int _tmp691=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp691,1);{
# 2406
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_rvals=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp692=inflow;struct Cyc_Dict_Dict _tmp693;struct Cyc_List_List*_tmp694;_LL3FF: if((_tmp692.BottomFL).tag != 1)goto _LL401;_LL400: {
const char*_tmp94C;void*_tmp94B;(_tmp94B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp94C="check_fun",_tag_dyneither(_tmp94C,sizeof(char),10))),_tag_dyneither(_tmp94B,sizeof(void*),0)));}_LL401: if((_tmp692.ReachableFL).tag != 2)goto _LL3FE;_tmp693=((struct _tuple13)(_tmp692.ReachableFL).val).f1;_tmp694=((struct _tuple13)(_tmp692.ReachableFL).val).f2;_LL402: {
# 2413
struct Cyc_List_List*atts;
{void*_tmp697=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));struct Cyc_List_List*_tmp699;_LL404: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp698=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp697;if(_tmp698->tag != 9)goto _LL406;else{_tmp699=(_tmp698->f1).attributes;}}_LL405:
 atts=_tmp699;goto _LL403;_LL406:;_LL407: {
const char*_tmp94F;void*_tmp94E;(_tmp94E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp94F="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp94F,sizeof(char),50))),_tag_dyneither(_tmp94E,sizeof(void*),0)));}_LL403:;}
# 2418
for(0;atts != 0;atts=atts->tl){
void*_tmp69C=(void*)atts->hd;int _tmp69E;int _tmp6A0;int _tmp6A2;_LL409: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp69D=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp69C;if(_tmp69D->tag != 21)goto _LL40B;else{_tmp69E=_tmp69D->f1;}}_LL40A: {
# 2421
unsigned int j=(unsigned int)_tmp69E;
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp952;void*_tmp951;(_tmp951=0,Cyc_Tcutil_terr(_tmp691,((_tmp952="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp952,sizeof(char),52))),_tag_dyneither(_tmp951,sizeof(void*),0)));}
continue;}{
# 2427
struct Cyc_Absyn_Vardecl*_tmp6A5=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp6A5->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp955;void*_tmp954;(_tmp954=0,Cyc_Tcutil_terr(_tmp691,((_tmp955="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp955,sizeof(char),52))),_tag_dyneither(_tmp954,sizeof(void*),0)));}
continue;}{
# 2433
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp6A8=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp958;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp957;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6A9=(_tmp957=_region_malloc(_tmp690,sizeof(*_tmp957)),((_tmp957[0]=((_tmp958.tag=2,((_tmp958.f1=(int)j,((_tmp958.f2=t,_tmp958)))))),_tmp957)));
struct Cyc_CfFlowInfo_Place*_tmp959;struct Cyc_CfFlowInfo_Place*_tmp6AA=(_tmp959=_region_malloc(_tmp690,sizeof(*_tmp959)),((_tmp959->root=(void*)_tmp6A9,((_tmp959->fields=0,_tmp959)))));
_tmp693=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp693,(void*)_tmp6A9,_tmp6A8);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp95F;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp95E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp95C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp95B;_tmp693=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp693,(void*)((_tmp95B=_region_malloc(_tmp690,sizeof(*_tmp95B)),((_tmp95B[0]=((_tmp95C.tag=0,((_tmp95C.f1=_tmp6A5,_tmp95C)))),_tmp95B)))),(void*)((_tmp95E=_region_malloc(_tmp690,sizeof(*_tmp95E)),((_tmp95E[0]=((_tmp95F.tag=5,((_tmp95F.f1=_tmp6AA,_tmp95F)))),_tmp95E)))));}
goto _LL408;};};}_LL40B: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp69F=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp69C;if(_tmp69F->tag != 20)goto _LL40D;else{_tmp6A0=_tmp69F->f1;}}_LL40C: {
# 2443
unsigned int j=(unsigned int)_tmp6A0;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp962;void*_tmp961;(_tmp961=0,Cyc_Tcutil_terr(_tmp691,((_tmp962="initializes attribute exceeds number of parameters",_tag_dyneither(_tmp962,sizeof(char),51))),_tag_dyneither(_tmp961,sizeof(void*),0)));}
# 2447
continue;}{
# 2449
struct Cyc_Absyn_Vardecl*_tmp6B4=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
{void*_tmp6B5=Cyc_Tcutil_compress(_tmp6B4->type);void*_tmp6B7;union Cyc_Absyn_Constraint*_tmp6B8;union Cyc_Absyn_Constraint*_tmp6B9;union Cyc_Absyn_Constraint*_tmp6BA;_LL412: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B5;if(_tmp6B6->tag != 5)goto _LL414;else{_tmp6B7=(_tmp6B6->f1).elt_typ;_tmp6B8=((_tmp6B6->f1).ptr_atts).nullable;_tmp6B9=((_tmp6B6->f1).ptr_atts).bounds;_tmp6BA=((_tmp6B6->f1).ptr_atts).zero_term;}}_LL413:
# 2452
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6B8)){
const char*_tmp965;void*_tmp964;(_tmp964=0,Cyc_Tcutil_terr(_tmp691,((_tmp965="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmp965,sizeof(char),55))),_tag_dyneither(_tmp964,sizeof(void*),0)));}
# 2455
if(!Cyc_Tcutil_is_bound_one(_tmp6B9)){
const char*_tmp968;void*_tmp967;(_tmp967=0,Cyc_Tcutil_terr(_tmp691,((_tmp968="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmp968,sizeof(char),57))),_tag_dyneither(_tmp967,sizeof(void*),0)));}
# 2458
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6BA)){
const char*_tmp96B;void*_tmp96A;(_tmp96A=0,Cyc_Tcutil_terr(_tmp691,((_tmp96B="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmp96B,sizeof(char),77))),_tag_dyneither(_tmp96A,sizeof(void*),0)));}{
# 2461
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp96E;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp96D;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6C1=(_tmp96D=_region_malloc(_tmp690,sizeof(*_tmp96D)),((_tmp96D[0]=((_tmp96E.tag=2,((_tmp96E.f1=(int)j,((_tmp96E.f2=_tmp6B7,_tmp96E)))))),_tmp96D)));
struct Cyc_CfFlowInfo_Place*_tmp96F;struct Cyc_CfFlowInfo_Place*_tmp6C2=(_tmp96F=_region_malloc(_tmp690,sizeof(*_tmp96F)),((_tmp96F->root=(void*)_tmp6C1,((_tmp96F->fields=0,_tmp96F)))));
_tmp693=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp693,(void*)_tmp6C1,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp6B7,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp975;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp974;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp972;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp971;_tmp693=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp693,(void*)((_tmp971=_region_malloc(_tmp690,sizeof(*_tmp971)),((_tmp971[0]=((_tmp972.tag=0,((_tmp972.f1=_tmp6B4,_tmp972)))),_tmp971)))),(void*)((_tmp974=_region_malloc(_tmp690,sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=5,((_tmp975.f1=_tmp6C2,_tmp975)))),_tmp974)))));}
{struct Cyc_List_List*_tmp976;param_roots=((_tmp976=_region_malloc(_tmp690,sizeof(*_tmp976)),((_tmp976->hd=_tmp6C2,((_tmp976->tl=param_roots,_tmp976))))));}
goto _LL411;};_LL414:;_LL415: {
const char*_tmp979;void*_tmp978;(_tmp978=0,Cyc_Tcutil_terr(_tmp691,((_tmp979="initializes attribute on non-pointer",_tag_dyneither(_tmp979,sizeof(char),37))),_tag_dyneither(_tmp978,sizeof(void*),0)));}_LL411:;}
# 2469
goto _LL408;};}_LL40D: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp6A1=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp69C;if(_tmp6A1->tag != 22)goto _LL40F;else{_tmp6A2=_tmp6A1->f1;}}_LL40E: {
# 2471
unsigned int j=(unsigned int)_tmp6A2;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp97C;void*_tmp97B;(_tmp97B=0,Cyc_Tcutil_terr(_tmp691,((_tmp97C="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmp97C,sizeof(char),49))),_tag_dyneither(_tmp97B,sizeof(void*),0)));}
continue;}{
# 2476
struct Cyc_Absyn_Vardecl*_tmp6CF=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp6CF->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp97F;void*_tmp97E;(_tmp97E=0,Cyc_Tcutil_terr(_tmp691,((_tmp97F="noconsume attribute requires unique pointer type",_tag_dyneither(_tmp97F,sizeof(char),49))),_tag_dyneither(_tmp97E,sizeof(void*),0)));}
continue;}{
# 2483
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp982;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp981;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6D2=(_tmp981=_region_malloc(_tmp690,sizeof(*_tmp981)),((_tmp981[0]=((_tmp982.tag=0,((_tmp982.f1=_tmp6CF,_tmp982)))),_tmp981)));
struct Cyc_CfFlowInfo_Place*_tmp983;struct Cyc_CfFlowInfo_Place*_tmp6D3=(_tmp983=_region_malloc(_tmp690,sizeof(*_tmp983)),((_tmp983->root=(void*)_tmp6D2,((_tmp983->fields=0,_tmp983)))));
void*_tmp6D4=Cyc_CfFlowInfo_lookup_place(_tmp693,_tmp6D3);
{struct _tuple24*_tmp986;struct Cyc_List_List*_tmp985;noconsume_rvals=((_tmp985=_region_malloc(_tmp690,sizeof(*_tmp985)),((_tmp985->hd=((_tmp986=_region_malloc(_tmp690,sizeof(*_tmp986)),((_tmp986->f1=_tmp6D3,((_tmp986->f2=_tmp6D4,_tmp986)))))),((_tmp985->tl=noconsume_rvals,_tmp985))))));}
# 2488
goto _LL408;};};}_LL40F:;_LL410:
 goto _LL408;_LL408:;}
# 2491
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp693,_tmp694);}_LL3FE:;}{
# 2494
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp690,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp987;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp987=_region_malloc(_tmp690,sizeof(*_tmp987)),((_tmp987->r=_tmp690,((_tmp987->fenv=fenv,((_tmp987->iterate_again=1,((_tmp987->iteration_num=0,((_tmp987->in_try=0,((_tmp987->tryflow=inflow,((_tmp987->all_changed=0,((_tmp987->noreturn=noreturn,((_tmp987->param_roots=param_roots,((_tmp987->noconsume_params=noconsume_rvals,((_tmp987->flow_table=flow_table,_tmp987)))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_Position_error_p()){
++ env->iteration_num;
# 2504
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
# 2507
union Cyc_CfFlowInfo_FlowInfo _tmp6DA=outflow;_LL417: if((_tmp6DA.BottomFL).tag != 1)goto _LL419;_LL418:
 goto _LL416;_LL419:;_LL41A:
# 2510
 Cyc_NewControlFlow_check_init_params(_tmp691,env,outflow);
Cyc_NewControlFlow_check_noconsume_params(_tmp691,env,outflow);
# 2514
if(noreturn){
const char*_tmp98A;void*_tmp989;(_tmp989=0,Cyc_Tcutil_terr(_tmp691,((_tmp98A="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp98A,sizeof(char),46))),_tag_dyneither(_tmp989,sizeof(void*),0)));}else{
# 2517
void*_tmp6DD=Cyc_Tcutil_compress(fd->ret_type);_LL41C: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp6DE=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp6DD;if(_tmp6DE->tag != 0)goto _LL41E;}_LL41D:
 goto _LL41B;_LL41E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6DF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6DD;if(_tmp6DF->tag != 7)goto _LL420;}_LL41F:
 goto _LL421;_LL420: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6DD;if(_tmp6E0->tag != 6)goto _LL422;}_LL421:
# 2521
{const char*_tmp98D;void*_tmp98C;(_tmp98C=0,Cyc_Tcutil_warn(_tmp691,((_tmp98D="function may not return a value",_tag_dyneither(_tmp98D,sizeof(char),32))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}goto _LL41B;_LL422:;_LL423:
# 2523
{const char*_tmp990;void*_tmp98F;(_tmp98F=0,Cyc_Tcutil_terr(_tmp691,((_tmp990="function may not return a value",_tag_dyneither(_tmp990,sizeof(char),32))),_tag_dyneither(_tmp98F,sizeof(void*),0)));}goto _LL41B;_LL41B:;}
# 2525
goto _LL416;_LL416:;};};};}
# 2530
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_tmp6E6=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp6E8;struct Cyc_List_List*_tmp6EA;struct Cyc_List_List*_tmp6EC;struct Cyc_List_List*_tmp6EE;_LL425: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp6E7=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6E6;if(_tmp6E7->tag != 1)goto _LL427;else{_tmp6E8=_tmp6E7->f1;}}_LL426:
 Cyc_NewControlFlow_check_fun(_tmp6E8);goto _LL424;_LL427: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp6E9=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp6E6;if(_tmp6E9->tag != 12)goto _LL429;else{_tmp6EA=_tmp6E9->f1;}}_LL428:
 _tmp6EC=_tmp6EA;goto _LL42A;_LL429: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp6EB=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp6E6;if(_tmp6EB->tag != 11)goto _LL42B;else{_tmp6EC=_tmp6EB->f2;}}_LL42A:
 _tmp6EE=_tmp6EC;goto _LL42C;_LL42B: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp6ED=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp6E6;if(_tmp6ED->tag != 10)goto _LL42D;else{_tmp6EE=_tmp6ED->f2;}}_LL42C:
 Cyc_NewControlFlow_cf_check(_tmp6EE);goto _LL424;_LL42D: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp6EF=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp6E6;if(_tmp6EF->tag != 13)goto _LL42F;}_LL42E:
 goto _LL424;_LL42F:;_LL430:
 goto _LL424;_LL424:;}}
