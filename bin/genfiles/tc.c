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
typedef void*Cyc_sarg_t;
# 174 "cycboot.h"
struct _dyneither_ptr Cyc_rprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160 "core.h"
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 302
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 305
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 377
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 748 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 905
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 933
void*Cyc_Absyn_compress_kb(void*);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 962
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 984
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 995
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1066
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1152
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 64
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 77
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 189
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);struct _tuple10{void*f1;void*f2;};
# 232 "dict.h"
struct _tuple10*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 255
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 288
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
# 115 "tcenv.h"
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 121
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 126
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 182
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 210
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 65
void*Cyc_Tcutil_compress(void*t);
# 68
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 94
int Cyc_Tcutil_is_function_type(void*t);
# 102
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 137
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 151
void Cyc_Tcutil_explain_failure();
# 153
int Cyc_Tcutil_unify(void*,void*);
# 167
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 177
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 207 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 209
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 217
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 221
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 273
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 298
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 309
int Cyc_Tcutil_bits_only(void*t);
# 312
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 319
int Cyc_Tcutil_supports_default(void*);
# 330
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 334
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 341
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 41
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 66 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 79
void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 88
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);static char _tmp0[1]="";
# 46 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 49
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 55
(*_tmp1).f3=1;
return 1;}}
# 59
return 0;}
# 62
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _RegionHandle _tmp2=_new_region("temp");struct _RegionHandle*temp=& _tmp2;_push_region(temp);
{struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
void*_tmp3=Cyc_Tcutil_compress(t);struct Cyc_List_List*_tmp5;_LL1: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3;if(_tmp4->tag != 9)goto _LL3;else{_tmp5=(_tmp4->f1).attributes;}}_LL2:
# 69
 for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
void*_tmp6=(void*)_tmp5->hd;int _tmp8;int _tmpA;int _tmpC;_LL6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp7=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6;if(_tmp7->tag != 20)goto _LL8;else{_tmp8=_tmp7->f1;}}_LL7:
# 72
 if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,_tmp8)){
struct Cyc_Int_pa_PrintArg_struct _tmp243;void*_tmp242[1];const char*_tmp241;void*_tmp240;(_tmp240=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp243.tag=1,((_tmp243.f1=(unsigned long)_tmp8,((_tmp242[0]=& _tmp243,Cyc_rprintf(temp,((_tmp241="incompatible initializes() and noliveunique() attributes on parameter %d",_tag_dyneither(_tmp241,sizeof(char),73))),_tag_dyneither(_tmp242,sizeof(void*),1)))))))),_tag_dyneither(_tmp240,sizeof(void*),0)));}
# 76
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,_tmp8)){
struct Cyc_Int_pa_PrintArg_struct _tmp24B;void*_tmp24A[1];const char*_tmp249;void*_tmp248;(_tmp248=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp24B.tag=1,((_tmp24B.f1=(unsigned long)_tmp8,((_tmp24A[0]=& _tmp24B,Cyc_rprintf(temp,((_tmp249="incompatible initializes() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp249,sizeof(char),70))),_tag_dyneither(_tmp24A,sizeof(void*),1)))))))),_tag_dyneither(_tmp248,sizeof(void*),0)));}
# 79
{struct Cyc_List_List*_tmp24C;init_params=((_tmp24C=_region_malloc(temp,sizeof(*_tmp24C)),((_tmp24C->hd=(void*)_tmp8,((_tmp24C->tl=init_params,_tmp24C))))));}goto _LL5;_LL8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp9=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp6;if(_tmp9->tag != 21)goto _LLA;else{_tmpA=_tmp9->f1;}}_LL9:
# 81
 if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,_tmpA)){
struct Cyc_Int_pa_PrintArg_struct _tmp254;void*_tmp253[1];const char*_tmp252;void*_tmp251;(_tmp251=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp254.tag=1,((_tmp254.f1=(unsigned long)_tmpA,((_tmp253[0]=& _tmp254,Cyc_rprintf(temp,((_tmp252="incompatible initializes() and noliveunique() attributes on parameter %d",_tag_dyneither(_tmp252,sizeof(char),73))),_tag_dyneither(_tmp253,sizeof(void*),1)))))))),_tag_dyneither(_tmp251,sizeof(void*),0)));}
# 85
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,_tmpA)){
struct Cyc_Int_pa_PrintArg_struct _tmp25C;void*_tmp25B[1];const char*_tmp25A;void*_tmp259;(_tmp259=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp25C.tag=1,((_tmp25C.f1=(unsigned long)_tmpA,((_tmp25B[0]=& _tmp25C,Cyc_rprintf(temp,((_tmp25A="incompatible noliveunique() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp25A,sizeof(char),71))),_tag_dyneither(_tmp25B,sizeof(void*),1)))))))),_tag_dyneither(_tmp259,sizeof(void*),0)));}
# 88
{struct Cyc_List_List*_tmp25D;nolive_unique_params=((_tmp25D=_region_malloc(temp,sizeof(*_tmp25D)),((_tmp25D->hd=(void*)_tmpA,((_tmp25D->tl=nolive_unique_params,_tmp25D))))));}goto _LL5;_LLA: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpB=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp6;if(_tmpB->tag != 22)goto _LLC;else{_tmpC=_tmpB->f1;}}_LLB:
# 90
 if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,_tmpC)){
struct Cyc_Int_pa_PrintArg_struct _tmp265;void*_tmp264[1];const char*_tmp263;void*_tmp262;(_tmp262=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp265.tag=1,((_tmp265.f1=(unsigned long)_tmpC,((_tmp264[0]=& _tmp265,Cyc_rprintf(temp,((_tmp263="incompatible initializes() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp263,sizeof(char),70))),_tag_dyneither(_tmp264,sizeof(void*),1)))))))),_tag_dyneither(_tmp262,sizeof(void*),0)));}
# 94
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,_tmpC)){
struct Cyc_Int_pa_PrintArg_struct _tmp26D;void*_tmp26C[1];const char*_tmp26B;void*_tmp26A;(_tmp26A=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp26D.tag=1,((_tmp26D.f1=(unsigned long)_tmpC,((_tmp26C[0]=& _tmp26D,Cyc_rprintf(temp,((_tmp26B="incompatible noliveunique() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp26B,sizeof(char),71))),_tag_dyneither(_tmp26C,sizeof(void*),1)))))))),_tag_dyneither(_tmp26A,sizeof(void*),0)));}
# 97
{struct Cyc_List_List*_tmp26E;noconsume_params=((_tmp26E=_region_malloc(temp,sizeof(*_tmp26E)),((_tmp26E->hd=(void*)_tmpC,((_tmp26E->tl=noconsume_params,_tmp26E))))));}goto _LL5;_LLC:;_LLD:
 goto _LL5;_LL5:;}
# 101
goto _LL0;_LL3:;_LL4: {
# 103
const char*_tmp271;void*_tmp270;(_tmp270=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp271="fnTypeAttsOK: not a function type",_tag_dyneither(_tmp271,sizeof(char),34))),_tag_dyneither(_tmp270,sizeof(void*),0)));}_LL0:;}
# 64
;_pop_region(temp);}struct _tuple13{void*f1;int f2;};
# 108
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 112
enum Cyc_Absyn_Scope _tmp2B;union Cyc_Absyn_Nmspace _tmp2C;struct _dyneither_ptr*_tmp2D;void*_tmp2E;struct Cyc_Absyn_Exp*_tmp2F;struct Cyc_List_List*_tmp30;struct Cyc_Absyn_Vardecl*_tmp2A=vd;_tmp2B=_tmp2A->sc;_tmp2C=(_tmp2A->name)->f1;_tmp2D=(_tmp2A->name)->f2;_tmp2E=_tmp2A->type;_tmp2F=_tmp2A->initializer;_tmp30=_tmp2A->attributes;
# 118
{union Cyc_Absyn_Nmspace _tmp31=_tmp2C;_LLF: if((_tmp31.Rel_n).tag != 1)goto _LL11;_LL10:
# 121
(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LLE;_LL11:;_LL12:
 goto _LLE;_LLE:;}
# 127
{void*_tmp32=Cyc_Tcutil_compress(_tmp2E);void*_tmp34;struct Cyc_Absyn_Tqual _tmp35;union Cyc_Absyn_Constraint*_tmp36;unsigned int _tmp37;_LL14:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp33=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp32;if(_tmp33->tag != 8)goto _LL16;else{_tmp34=(_tmp33->f1).elt_type;_tmp35=(_tmp33->f1).tq;if((_tmp33->f1).num_elts != 0)goto _LL16;_tmp36=(_tmp33->f1).zero_term;_tmp37=(_tmp33->f1).zt_loc;}}if(!(_tmp2F != 0))goto _LL16;_LL15:
# 129
{void*_tmp38=_tmp2F->r;struct _dyneither_ptr _tmp3A;struct _dyneither_ptr _tmp3C;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp42;_LL19: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp39=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp39->tag != 0)goto _LL1B;else{if(((_tmp39->f1).String_c).tag != 8)goto _LL1B;_tmp3A=(struct _dyneither_ptr)((_tmp39->f1).String_c).val;}}_LL1A:
# 131
 _tmp2E=(vd->type=Cyc_Absyn_array_typ(_tmp34,_tmp35,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp3A,sizeof(char)),0),_tmp36,_tmp37));
goto _LL18;_LL1B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp3B->tag != 0)goto _LL1D;else{if(((_tmp3B->f1).Wstring_c).tag != 9)goto _LL1D;_tmp3C=(struct _dyneither_ptr)((_tmp3B->f1).Wstring_c).val;}}_LL1C:
# 134
 _tmp2E=(vd->type=Cyc_Absyn_array_typ(_tmp34,_tmp35,Cyc_Absyn_uint_exp(1,0),_tmp36,_tmp37));
goto _LL18;_LL1D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3D=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp3D->tag != 26)goto _LL1F;else{_tmp3E=_tmp3D->f2;}}_LL1E:
# 138
 _tmp2E=(vd->type=Cyc_Absyn_array_typ(_tmp34,_tmp35,_tmp3E,_tmp36,_tmp37));
goto _LL18;_LL1F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp3F=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp3F->tag != 34)goto _LL21;else{_tmp40=_tmp3F->f2;}}_LL20:
 _tmp42=_tmp40;goto _LL22;_LL21: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp41=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp41->tag != 25)goto _LL23;else{_tmp42=_tmp41->f1;}}_LL22:
# 142
 _tmp2E=(vd->type=Cyc_Absyn_array_typ(_tmp34,_tmp35,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp42),0),_tmp36,_tmp37));
goto _LL18;_LL23:;_LL24:
 goto _LL18;_LL18:;}
# 146
goto _LL13;_LL16:;_LL17:
 goto _LL13;_LL13:;}
# 150
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp2E);
# 152
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp2E);
# 155
{void*_tmp43=Cyc_Tcutil_compress(_tmp2E);_LL26: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp44=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43;if(_tmp44->tag != 8)goto _LL28;}_LL27:
 vd->escapes=0;goto _LL25;_LL28:;_LL29:
 vd->escapes=1;goto _LL25;_LL25:;}
# 160
if(Cyc_Tcutil_is_function_type(_tmp2E)){
_tmp30=Cyc_Tcutil_transfer_fn_type_atts(_tmp2E,_tmp30);
Cyc_Tc_fnTypeAttsOK(te,_tmp2E,loc);}
# 165
if(_tmp2B == Cyc_Absyn_Extern  || _tmp2B == Cyc_Absyn_ExternC){
if(_tmp2F != 0){
const char*_tmp274;void*_tmp273;(_tmp273=0,Cyc_Tcutil_terr(loc,((_tmp274="extern declaration should not have initializer",_tag_dyneither(_tmp274,sizeof(char),47))),_tag_dyneither(_tmp273,sizeof(void*),0)));}}else{
if(!Cyc_Tcutil_is_function_type(_tmp2E)){
# 172
for(0;_tmp30 != 0;_tmp30=_tmp30->tl){
void*_tmp47=(void*)_tmp30->hd;_LL2B: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp48=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp47;if(_tmp48->tag != 6)goto _LL2D;}_LL2C:
 goto _LL2E;_LL2D: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp49=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp47;if(_tmp49->tag != 8)goto _LL2F;}_LL2E:
# 177
 goto _LL30;_LL2F: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp4A=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp47;if(_tmp4A->tag != 9)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp4B=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp47;if(_tmp4B->tag != 10)goto _LL33;}_LL32:
 goto _LL34;_LL33: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp4C=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp47;if(_tmp4C->tag != 11)goto _LL35;}_LL34:
 goto _LL36;_LL35: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp4D=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp47;if(_tmp4D->tag != 12)goto _LL37;}_LL36:
 goto _LL38;_LL37: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp4E=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp47;if(_tmp4E->tag != 13)goto _LL39;}_LL38:
 goto _LL3A;_LL39: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp4F=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp47;if(_tmp4F->tag != 14)goto _LL3B;}_LL3A:
 continue;_LL3B:;_LL3C:
# 185
{const char*_tmp279;void*_tmp278[2];struct Cyc_String_pa_PrintArg_struct _tmp277;struct Cyc_String_pa_PrintArg_struct _tmp276;(_tmp276.tag=0,((_tmp276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp277.tag=0,((_tmp277.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp30->hd)),((_tmp278[0]=& _tmp277,((_tmp278[1]=& _tmp276,Cyc_Tcutil_terr(loc,((_tmp279="bad attribute %s for variable %s",_tag_dyneither(_tmp279,sizeof(char),33))),_tag_dyneither(_tmp278,sizeof(void*),2)))))))))))));}
goto _LL2A;_LL2A:;}
# 190
if(_tmp2F == 0  || in_cinclude){
if(check_var_init  && !Cyc_Tcutil_supports_default(_tmp2E)){
const char*_tmp27E;void*_tmp27D[2];struct Cyc_String_pa_PrintArg_struct _tmp27C;struct Cyc_String_pa_PrintArg_struct _tmp27B;(_tmp27B.tag=0,((_tmp27B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2E)),((_tmp27C.tag=0,((_tmp27C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp27D[0]=& _tmp27C,((_tmp27D[1]=& _tmp27B,Cyc_Tcutil_terr(loc,((_tmp27E="initializer required for variable %s of type %s",_tag_dyneither(_tmp27E,sizeof(char),48))),_tag_dyneither(_tmp27D,sizeof(void*),2)))))))))))));}}else{
# 195
struct Cyc_Absyn_Exp*_tmp58=_tmp2F;
# 197
void*_tmp59=Cyc_Tcexp_tcExpInitializer(te,& _tmp2E,_tmp58);
if(!Cyc_Tcutil_coerce_assign(te,_tmp58,_tmp2E)){
{const char*_tmp284;void*_tmp283[3];struct Cyc_String_pa_PrintArg_struct _tmp282;struct Cyc_String_pa_PrintArg_struct _tmp281;struct Cyc_String_pa_PrintArg_struct _tmp280;(_tmp280.tag=0,((_tmp280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp59)),((_tmp281.tag=0,((_tmp281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2E)),((_tmp282.tag=0,((_tmp282.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp283[0]=& _tmp282,((_tmp283[1]=& _tmp281,((_tmp283[2]=& _tmp280,Cyc_Tcutil_terr(loc,((_tmp284="%s declared with type \n%s\n but initialized with type \n%s",_tag_dyneither(_tmp284,sizeof(char),57))),_tag_dyneither(_tmp283,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}
# 204
if(!Cyc_Tcutil_is_const_exp(_tmp58)){
const char*_tmp287;void*_tmp286;(_tmp286=0,Cyc_Tcutil_terr(loc,((_tmp287="initializer is not a constant expression",_tag_dyneither(_tmp287,sizeof(char),41))),_tag_dyneither(_tmp286,sizeof(void*),0)));}}}else{
# 209
for(0;_tmp30 != 0;_tmp30=_tmp30->tl){
void*_tmp61=(void*)_tmp30->hd;_LL3E: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp62=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp61;if(_tmp62->tag != 0)goto _LL40;}_LL3F:
# 212
 goto _LL41;_LL40: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp63=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp61;if(_tmp63->tag != 1)goto _LL42;}_LL41:
 goto _LL43;_LL42: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp64=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp61;if(_tmp64->tag != 2)goto _LL44;}_LL43:
 goto _LL45;_LL44: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp65=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp61;if(_tmp65->tag != 3)goto _LL46;}_LL45:
 goto _LL47;_LL46: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp66=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp61;if(_tmp66->tag != 4)goto _LL48;}_LL47:
 goto _LL49;_LL48: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp67=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp61;if(_tmp67->tag != 19)goto _LL4A;}_LL49:
 goto _LL4B;_LL4A: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp68=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp61;if(_tmp68->tag != 20)goto _LL4C;}_LL4B:
 goto _LL4D;_LL4C: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp69=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp61;if(_tmp69->tag != 23)goto _LL4E;}_LL4D:
 goto _LL4F;_LL4E: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp6A=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp61;if(_tmp6A->tag != 5)goto _LL50;}_LL4F: {
const char*_tmp28A;void*_tmp289;(_tmp289=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp28A="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp28A,sizeof(char),45))),_tag_dyneither(_tmp289,sizeof(void*),0)));}_LL50: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp6B=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp61;if(_tmp6B->tag != 6)goto _LL52;}_LL51:
# 222
 goto _LL53;_LL52: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp6C=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp61;if(_tmp6C->tag != 7)goto _LL54;}_LL53:
# 224
{const char*_tmp28E;void*_tmp28D[1];struct Cyc_String_pa_PrintArg_struct _tmp28C;(_tmp28C.tag=0,((_tmp28C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp30->hd)),((_tmp28D[0]=& _tmp28C,Cyc_Tcutil_terr(loc,((_tmp28E="bad attribute %s in function declaration",_tag_dyneither(_tmp28E,sizeof(char),41))),_tag_dyneither(_tmp28D,sizeof(void*),1)))))));}
goto _LL3D;_LL54:;_LL55:
 continue;_LL3D:;}}}
# 232
{struct _handler_cons _tmp72;_push_handler(& _tmp72);{int _tmp74=0;if(setjmp(_tmp72.handler))_tmp74=1;if(!_tmp74){
{struct _tuple13*_tmp75=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp2D);
void*_tmp76=(*_tmp75).f1;void*_tmp78;_LL57: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp77=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp76;if(_tmp77->tag != 0)goto _LL59;else{_tmp78=(void*)_tmp77->f1;}}_LL58: {
# 236
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp291;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp290;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp7D=(_tmp290=_cycalloc(sizeof(*_tmp290)),((_tmp290[0]=((_tmp291.tag=1,((_tmp291.f1=vd,_tmp291)))),_tmp290)));
void*_tmp7E=Cyc_Tcdecl_merge_binding(_tmp78,(void*)_tmp7D,loc,Cyc_Tc_tc_msg);
if(_tmp7E == (void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}
# 240
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp7E != _tmp78  || (*_tmp75).f2){
# 243
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp297;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp296;struct _tuple13*_tmp295;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp2D,(
(_tmp295=_cycalloc(sizeof(*_tmp295)),((_tmp295->f1=(void*)((_tmp297=_cycalloc(sizeof(*_tmp297)),((_tmp297[0]=((_tmp296.tag=0,((_tmp296.f1=_tmp7E,_tmp296)))),_tmp297)))),((_tmp295->f2=1,_tmp295)))))));}}
_npop_handler(0);return;}_LL59: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp79=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp76;if(_tmp79->tag != 1)goto _LL5B;}_LL5A:
# 247
{const char*_tmp29A;void*_tmp299;(_tmp299=0,Cyc_Tcutil_warn(loc,((_tmp29A="variable declaration shadows previous struct declaration",_tag_dyneither(_tmp29A,sizeof(char),57))),_tag_dyneither(_tmp299,sizeof(void*),0)));}
goto _LL56;_LL5B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp7A=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp76;if(_tmp7A->tag != 2)goto _LL5D;}_LL5C:
# 250
{const char*_tmp29D;void*_tmp29C;(_tmp29C=0,Cyc_Tcutil_warn(loc,((_tmp29D="variable declaration shadows previous datatype constructor",_tag_dyneither(_tmp29D,sizeof(char),59))),_tag_dyneither(_tmp29C,sizeof(void*),0)));}
goto _LL56;_LL5D: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp7B=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp76;if(_tmp7B->tag != 4)goto _LL5F;}_LL5E:
 goto _LL60;_LL5F: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp7C=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp76;if(_tmp7C->tag != 3)goto _LL56;}_LL60:
# 254
{const char*_tmp2A0;void*_tmp29F;(_tmp29F=0,Cyc_Tcutil_warn(loc,((_tmp2A0="variable declaration shadows previous enum tag",_tag_dyneither(_tmp2A0,sizeof(char),47))),_tag_dyneither(_tmp29F,sizeof(void*),0)));}
goto _LL56;_LL56:;}
# 233
;_pop_handler();}else{void*_tmp73=(void*)_exn_thrown;void*_tmp8B=_tmp73;void*_tmp8D;_LL62: {struct Cyc_Dict_Absent_exn_struct*_tmp8C=(struct Cyc_Dict_Absent_exn_struct*)_tmp8B;if(_tmp8C->tag != Cyc_Dict_Absent)goto _LL64;}_LL63:
# 257
 goto _LL61;_LL64: _tmp8D=_tmp8B;_LL65:(void)_throw(_tmp8D);_LL61:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2AF;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2AE;struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp2AD;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2AC;struct _tuple13*_tmp2AB;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp2D,(
(_tmp2AB=_cycalloc(sizeof(*_tmp2AB)),((_tmp2AB->f1=(void*)((_tmp2AF=_cycalloc(sizeof(*_tmp2AF)),((_tmp2AF[0]=((_tmp2AC.tag=0,((_tmp2AC.f1=(void*)((_tmp2AE=_cycalloc(sizeof(*_tmp2AE)),((_tmp2AE[0]=((_tmp2AD.tag=1,((_tmp2AD.f1=vd,_tmp2AD)))),_tmp2AE)))),_tmp2AC)))),_tmp2AF)))),((_tmp2AB->f2=0,_tmp2AB)))))));}}
# 263
static int Cyc_Tc_is_main(struct _tuple0*n){
union Cyc_Absyn_Nmspace _tmp94;struct _dyneither_ptr*_tmp95;struct _tuple0*_tmp93=n;_tmp94=_tmp93->f1;_tmp95=_tmp93->f2;{
union Cyc_Absyn_Nmspace _tmp96=_tmp94;_LL67: if((_tmp96.Abs_n).tag != 2)goto _LL69;if((struct Cyc_List_List*)(_tmp96.Abs_n).val != 0)goto _LL69;_LL68: {
# 267
const char*_tmp2B0;return Cyc_strcmp((struct _dyneither_ptr)*_tmp95,((_tmp2B0="main",_tag_dyneither(_tmp2B0,sizeof(char),5))))== 0;}_LL69:;_LL6A:
 return 0;_LL66:;};}
# 272
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**exports){
# 275
struct _dyneither_ptr*v=(*fd->name).f2;
# 279
if(fd->sc == Cyc_Absyn_ExternC  && !in_cinclude){
const char*_tmp2B3;void*_tmp2B2;(_tmp2B2=0,Cyc_Tcutil_terr(loc,((_tmp2B3="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(_tmp2B3,sizeof(char),54))),_tag_dyneither(_tmp2B2,sizeof(void*),0)));}
# 282
{union Cyc_Absyn_Nmspace _tmp9A=(*fd->name).f1;_LL6C: if((_tmp9A.Rel_n).tag != 1)goto _LL6E;if((struct Cyc_List_List*)(_tmp9A.Rel_n).val != 0)goto _LL6E;_LL6D:
# 285
(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LL6B;_LL6E:;_LL6F:
# 288
 goto _LL6B;_LL6B:;}
# 291
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 293
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 295
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 298
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp9B=(void*)atts->hd;_LL71: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp9C=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp9B;if(_tmp9C->tag != 7)goto _LL73;}_LL72:
 goto _LL74;_LL73: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp9D=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp9B;if(_tmp9D->tag != 6)goto _LL75;}_LL74:
# 302
{const char*_tmp2B7;void*_tmp2B6[1];struct Cyc_String_pa_PrintArg_struct _tmp2B5;(_tmp2B5.tag=0,((_tmp2B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp2B6[0]=& _tmp2B5,Cyc_Tcutil_terr(loc,((_tmp2B7="bad attribute %s for function",_tag_dyneither(_tmp2B7,sizeof(char),30))),_tag_dyneither(_tmp2B6,sizeof(void*),1)))))));}
goto _LL70;_LL75:;_LL76:
 goto _LL70;_LL70:;}}
# 308
{struct _handler_cons _tmpA1;_push_handler(& _tmpA1);{int _tmpA3=0;if(setjmp(_tmpA1.handler))_tmpA3=1;if(!_tmpA3){
{struct _tuple13*_tmpA4=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
void*_tmpA5=(*_tmpA4).f1;void*_tmpA7;_LL78: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA6=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA5;if(_tmpA6->tag != 0)goto _LL7A;else{_tmpA7=(void*)_tmpA6->f1;}}_LL79: {
# 312
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2BA;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2B9;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAC=(_tmp2B9=_cycalloc(sizeof(*_tmp2B9)),((_tmp2B9[0]=((_tmp2BA.tag=2,((_tmp2BA.f1=fd,_tmp2BA)))),_tmp2B9)));
void*_tmpAD=Cyc_Tcdecl_merge_binding(_tmpA7,(void*)_tmpAC,loc,Cyc_Tc_tc_msg);
if(_tmpAD == (void*)& Cyc_Absyn_Unresolved_b_val)goto _LL77;
# 316
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
if(!(_tmpAD == _tmpA7  && (*_tmpA4).f2)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2C0;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2BF;struct _tuple13*_tmp2BE;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp2BE=_cycalloc(sizeof(*_tmp2BE)),((_tmp2BE->f1=(void*)((_tmp2C0=_cycalloc(sizeof(*_tmp2C0)),((_tmp2C0[0]=((_tmp2BF.tag=0,((_tmp2BF.f1=_tmpAD,_tmp2BF)))),_tmp2C0)))),((_tmp2BE->f2=1,_tmp2BE)))))));}}
goto _LL77;}_LL7A: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpA8=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA5;if(_tmpA8->tag != 1)goto _LL7C;}_LL7B:
# 322
{const char*_tmp2C3;void*_tmp2C2;(_tmp2C2=0,Cyc_Tcutil_warn(loc,((_tmp2C3="function declaration shadows previous type declaration",_tag_dyneither(_tmp2C3,sizeof(char),55))),_tag_dyneither(_tmp2C2,sizeof(void*),0)));}
goto _LL77;_LL7C: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpA9=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA5;if(_tmpA9->tag != 2)goto _LL7E;}_LL7D:
# 325
{const char*_tmp2C6;void*_tmp2C5;(_tmp2C5=0,Cyc_Tcutil_warn(loc,((_tmp2C6="function declaration shadows previous datatype constructor",_tag_dyneither(_tmp2C6,sizeof(char),59))),_tag_dyneither(_tmp2C5,sizeof(void*),0)));}
goto _LL77;_LL7E: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAA=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA5;if(_tmpAA->tag != 4)goto _LL80;}_LL7F:
 goto _LL81;_LL80: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA5;if(_tmpAB->tag != 3)goto _LL77;}_LL81:
# 329
{const char*_tmp2C9;void*_tmp2C8;(_tmp2C8=0,Cyc_Tcutil_warn(loc,((_tmp2C9="function declaration shadows previous enum tag",_tag_dyneither(_tmp2C9,sizeof(char),47))),_tag_dyneither(_tmp2C8,sizeof(void*),0)));}
goto _LL77;_LL77:;}
# 309
;_pop_handler();}else{void*_tmpA2=(void*)_exn_thrown;void*_tmpBA=_tmpA2;void*_tmpBC;_LL83: {struct Cyc_Dict_Absent_exn_struct*_tmpBB=(struct Cyc_Dict_Absent_exn_struct*)_tmpBA;if(_tmpBB->tag != Cyc_Dict_Absent)goto _LL85;}_LL84:
# 333
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2CC;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2CB;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBD=(_tmp2CB=_cycalloc(sizeof(*_tmp2CB)),((_tmp2CB[0]=((_tmp2CC.tag=2,((_tmp2CC.f1=fd,_tmp2CC)))),_tmp2CB)));
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2DB;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2DA;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2D9;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2D8;struct _tuple13*_tmp2D7;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp2D7=_cycalloc(sizeof(*_tmp2D7)),((_tmp2D7->f1=(void*)((_tmp2DB=_cycalloc(sizeof(*_tmp2DB)),((_tmp2DB[0]=((_tmp2D8.tag=0,((_tmp2D8.f1=(void*)((_tmp2DA=_cycalloc(sizeof(*_tmp2DA)),((_tmp2DA[0]=((_tmp2D9.tag=2,((_tmp2D9.f1=fd,_tmp2D9)))),_tmp2DA)))),_tmp2D8)))),_tmp2DB)))),((_tmp2D7->f2=0,_tmp2D7)))))));}
# 338
goto _LL82;_LL85: _tmpBC=_tmpBA;_LL86:(void)_throw(_tmpBC);_LL82:;}};}
# 342
if(in_cinclude)return;{
# 347
struct _RegionHandle _tmpC5=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpC5;_push_region(fnrgn);{
struct Cyc_Tcenv_Fenv*_tmpC6=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmp2DC;struct Cyc_Tcenv_Tenv*_tmpC7=(_tmp2DC=_region_malloc(fnrgn,sizeof(*_tmp2DC)),((_tmp2DC->ns=te->ns,((_tmp2DC->ae=te->ae,((_tmp2DC->le=_tmpC6,((_tmp2DC->allow_valueof=0,_tmp2DC)))))))));
# 351
Cyc_Tcstmt_tcStmt(_tmpC7,fd->body,0);
# 354
Cyc_Tcenv_check_delayed_effects(_tmpC7);
Cyc_Tcenv_check_delayed_constraints(_tmpC7);
# 358
if(!Cyc_Tcenv_all_labels_resolved(_tmpC7)){
const char*_tmp2DF;void*_tmp2DE;(_tmp2DE=0,Cyc_Tcutil_terr(loc,((_tmp2DF="function has goto statements to undefined labels",_tag_dyneither(_tmp2DF,sizeof(char),49))),_tag_dyneither(_tmp2DE,sizeof(void*),0)));}}
# 362
if(Cyc_Tc_is_main(fd->name)){
# 364
{void*_tmpCB=Cyc_Tcutil_compress(fd->ret_type);enum Cyc_Absyn_Size_of _tmpCE;_LL88: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCC=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCB;if(_tmpCC->tag != 0)goto _LL8A;}_LL89:
{const char*_tmp2E2;void*_tmp2E1;(_tmp2E1=0,Cyc_Tcutil_warn(loc,((_tmp2E2="main declared with return type void",_tag_dyneither(_tmp2E2,sizeof(char),36))),_tag_dyneither(_tmp2E1,sizeof(void*),0)));}goto _LL87;_LL8A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCB;if(_tmpCD->tag != 6)goto _LL8C;else{_tmpCE=_tmpCD->f2;}}_LL8B:
 goto _LL87;_LL8C:;_LL8D:
# 368
{const char*_tmp2E6;void*_tmp2E5[1];struct Cyc_String_pa_PrintArg_struct _tmp2E4;(_tmp2E4.tag=0,((_tmp2E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp2E5[0]=& _tmp2E4,Cyc_Tcutil_terr(loc,((_tmp2E6="main declared with return type %s instead of int or void",_tag_dyneither(_tmp2E6,sizeof(char),57))),_tag_dyneither(_tmp2E5,sizeof(void*),1)))))));}
goto _LL87;_LL87:;}
# 372
if(fd->c_varargs  || fd->cyc_varargs != 0){
const char*_tmp2E9;void*_tmp2E8;(_tmp2E8=0,Cyc_Tcutil_terr(loc,((_tmp2E9="main declared with varargs",_tag_dyneither(_tmp2E9,sizeof(char),27))),_tag_dyneither(_tmp2E8,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD6=fd->args;
if(_tmpD6 != 0){
void*_tmpD8;struct _tuple8*_tmpD7=(struct _tuple8*)_tmpD6->hd;_tmpD8=_tmpD7->f3;
{void*_tmpD9=Cyc_Tcutil_compress(_tmpD8);enum Cyc_Absyn_Size_of _tmpDB;_LL8F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD9;if(_tmpDA->tag != 6)goto _LL91;else{_tmpDB=_tmpDA->f2;}}_LL90:
 goto _LL8E;_LL91:;_LL92:
# 380
{const char*_tmp2ED;void*_tmp2EC[1];struct Cyc_String_pa_PrintArg_struct _tmp2EB;(_tmp2EB.tag=0,((_tmp2EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpD8)),((_tmp2EC[0]=& _tmp2EB,Cyc_Tcutil_terr(loc,((_tmp2ED="main declared with first argument of type %s instead of int",_tag_dyneither(_tmp2ED,sizeof(char),60))),_tag_dyneither(_tmp2EC,sizeof(void*),1)))))));}
goto _LL8E;_LL8E:;}
# 384
_tmpD6=_tmpD6->tl;
if(_tmpD6 != 0){
void*_tmpE0;struct _tuple8*_tmpDF=(struct _tuple8*)_tmpD6->hd;_tmpE0=_tmpDF->f3;
_tmpD6=_tmpD6->tl;
if(_tmpD6 != 0){
const char*_tmp2F0;void*_tmp2EF;(_tmp2EF=0,Cyc_Tcutil_terr(loc,((_tmp2F0="main declared with too many arguments",_tag_dyneither(_tmp2F0,sizeof(char),38))),_tag_dyneither(_tmp2EF,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp2F1;struct Cyc_Core_Opt*tvs=(_tmp2F1=_cycalloc(sizeof(*_tmp2F1)),((_tmp2F1->v=fd->tvs,_tmp2F1)));
struct Cyc_Core_Opt*_tmp2F9;struct Cyc_Core_Opt*_tmp2F8;struct Cyc_Core_Opt*_tmp2F7;struct Cyc_Core_Opt*_tmp2F6;struct Cyc_Core_Opt*_tmp2F5;struct Cyc_Core_Opt*_tmp2F4;struct Cyc_Core_Opt*_tmp2F3;struct Cyc_Core_Opt*_tmp2F2;if(((!Cyc_Tcutil_unify(_tmpE0,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp2F8=_cycalloc(sizeof(*_tmp2F8)),((_tmp2F8->v=& Cyc_Tcutil_rk,_tmp2F8)))),tvs)),
Cyc_Absyn_new_evar(((_tmp2F9=_cycalloc(sizeof(*_tmp2F9)),((_tmp2F9->v=& Cyc_Tcutil_rk,_tmp2F9)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmpE0,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp2F6=_cycalloc(sizeof(*_tmp2F6)),((_tmp2F6->v=& Cyc_Tcutil_rk,_tmp2F6)))),tvs)),
# 396
Cyc_Absyn_new_evar(((_tmp2F7=_cycalloc(sizeof(*_tmp2F7)),((_tmp2F7->v=& Cyc_Tcutil_rk,_tmp2F7)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpE0,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp2F4=_cycalloc(sizeof(*_tmp2F4)),((_tmp2F4->v=& Cyc_Tcutil_rk,_tmp2F4)))),tvs)),
Cyc_Absyn_new_evar(((_tmp2F5=_cycalloc(sizeof(*_tmp2F5)),((_tmp2F5->v=& Cyc_Tcutil_rk,_tmp2F5)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpE0,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp2F2=_cycalloc(sizeof(*_tmp2F2)),((_tmp2F2->v=& Cyc_Tcutil_rk,_tmp2F2)))),tvs)),
# 403
Cyc_Absyn_new_evar(((_tmp2F3=_cycalloc(sizeof(*_tmp2F3)),((_tmp2F3->v=& Cyc_Tcutil_rk,_tmp2F3)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))){
const char*_tmp2FD;void*_tmp2FC[1];struct Cyc_String_pa_PrintArg_struct _tmp2FB;(_tmp2FB.tag=0,((_tmp2FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpE0)),((_tmp2FC[0]=& _tmp2FB,Cyc_Tcutil_terr(loc,((_tmp2FD="second argument of main has type %s instead of char??",_tag_dyneither(_tmp2FD,sizeof(char),54))),_tag_dyneither(_tmp2FC,sizeof(void*),1)))))));}};}}};}
# 347
;_pop_region(fnrgn);};};}
# 413
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _dyneither_ptr*v=(*td->name).f2;
# 419
{union Cyc_Absyn_Nmspace _tmpEF=(*td->name).f1;_LL94: if((_tmpEF.Rel_n).tag != 1)goto _LL96;_LL95:
# 422
(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);
goto _LL93;_LL96:;_LL97:
 goto _LL93;_LL93:;}
# 427
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
{const char*_tmp301;void*_tmp300[1];struct Cyc_String_pa_PrintArg_struct _tmp2FF;(_tmp2FF.tag=0,((_tmp2FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp300[0]=& _tmp2FF,Cyc_Tcutil_terr(loc,((_tmp301="redeclaration of typedef %s",_tag_dyneither(_tmp301,sizeof(char),28))),_tag_dyneither(_tmp300,sizeof(void*),1)))))));}
return;}
# 432
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 441
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpF3=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt**_tmpF5;struct Cyc_Core_Opt**_tmpF7;struct Cyc_Absyn_Kind*_tmpF8;_LL99: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpF4=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpF3;if(_tmpF4->tag != 1)goto _LL9B;else{_tmpF5=(struct Cyc_Core_Opt**)& _tmpF4->f1;}}_LL9A:
# 444
 if(td->defn != 0){
const char*_tmp305;void*_tmp304[1];struct Cyc_String_pa_PrintArg_struct _tmp303;(_tmp303.tag=0,((_tmp303.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp304[0]=& _tmp303,Cyc_Tcutil_warn(loc,((_tmp305="type variable %s is not used in typedef definition",_tag_dyneither(_tmp305,sizeof(char),51))),_tag_dyneither(_tmp304,sizeof(void*),1)))))));}
# 447
{struct Cyc_Core_Opt*_tmp306;*_tmpF5=((_tmp306=_cycalloc(sizeof(*_tmp306)),((_tmp306->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp306))));}goto _LL98;_LL9B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpF6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3;if(_tmpF6->tag != 2)goto _LL9D;else{_tmpF7=(struct Cyc_Core_Opt**)& _tmpF6->f1;_tmpF8=_tmpF6->f2;}}_LL9C:
# 454
{struct Cyc_Core_Opt*_tmp307;*_tmpF7=((_tmp307=_cycalloc(sizeof(*_tmp307)),((_tmp307->v=Cyc_Tcutil_kind_to_bound(_tmpF8),_tmp307))));}
goto _LL98;_LL9D:;_LL9E:
 continue;_LL98:;}}
# 460
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 463
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 470
struct _RegionHandle _tmpFE=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpFE;_push_region(uprev_rgn);
# 472
for(0;rpo != 0;rpo=rpo->tl){
void*_tmp100;void*_tmp101;struct _tuple10*_tmpFF=(struct _tuple10*)rpo->hd;_tmp100=_tmpFF->f1;_tmp101=_tmpFF->f2;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,_tmp100);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,_tmp101);}{
# 478
struct Cyc_List_List*prev_fields=0;
# 480
struct Cyc_List_List*_tmp102=fields;for(0;_tmp102 != 0;_tmp102=_tmp102->tl){
struct _dyneither_ptr*_tmp104;struct Cyc_Absyn_Tqual _tmp105;void*_tmp106;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_Absyn_Aggrfield*_tmp103=(struct Cyc_Absyn_Aggrfield*)_tmp102->hd;_tmp104=_tmp103->name;_tmp105=_tmp103->tq;_tmp106=_tmp103->type;_tmp107=_tmp103->width;_tmp108=_tmp103->attributes;
# 483
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp104)){
const char*_tmp30B;void*_tmp30A[1];struct Cyc_String_pa_PrintArg_struct _tmp309;(_tmp309.tag=0,((_tmp309.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp104),((_tmp30A[0]=& _tmp309,Cyc_Tcutil_terr(loc,((_tmp30B="duplicate member %s",_tag_dyneither(_tmp30B,sizeof(char),20))),_tag_dyneither(_tmp30A,sizeof(void*),1)))))));}
# 487
{const char*_tmp30C;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp104,((_tmp30C="",_tag_dyneither(_tmp30C,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp30D;prev_fields=((_tmp30D=_region_malloc(uprev_rgn,sizeof(*_tmp30D)),((_tmp30D->hd=_tmp104,((_tmp30D->tl=prev_fields,_tmp30D))))));}}{
# 490
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 494
if(_tmp102->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,_tmp106);
# 498
(((struct Cyc_Absyn_Aggrfield*)_tmp102->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp102->hd)->tq).print_const,_tmp106);
# 501
Cyc_Tcutil_check_bitfield(loc,te,_tmp106,_tmp107,_tmp104);};}};
# 472
;_pop_region(uprev_rgn);}
# 507
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 509
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp10E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt**_tmp110;struct Cyc_Core_Opt**_tmp112;enum Cyc_Absyn_AliasQual _tmp113;struct Cyc_Core_Opt**_tmp115;struct Cyc_Core_Opt**_tmp117;struct Cyc_Absyn_Kind*_tmp118;enum Cyc_Absyn_AliasQual _tmp11A;_LLA0: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp10F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp10E;if(_tmp10F->tag != 1)goto _LLA2;else{_tmp110=(struct Cyc_Core_Opt**)& _tmp10F->f1;}}_LLA1:
# 512
{struct Cyc_Core_Opt*_tmp30E;*_tmp110=((_tmp30E=_cycalloc(sizeof(*_tmp30E)),((_tmp30E->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp30E))));}continue;_LLA2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp111=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp10E;if(_tmp111->tag != 2)goto _LLA4;else{_tmp112=(struct Cyc_Core_Opt**)& _tmp111->f1;if((_tmp111->f2)->kind != Cyc_Absyn_MemKind)goto _LLA4;_tmp113=(_tmp111->f2)->aliasqual;}}_LLA3:
# 514
 if(constrain_top_kind  && _tmp113 == Cyc_Absyn_Top){
struct Cyc_Absyn_Kind*_tmp311;struct Cyc_Core_Opt*_tmp310;*_tmp112=((_tmp310=_cycalloc(sizeof(*_tmp310)),((_tmp310->v=Cyc_Tcutil_kind_to_bound(((_tmp311=_cycalloc_atomic(sizeof(*_tmp311)),((_tmp311->kind=Cyc_Absyn_BoxKind,((_tmp311->aliasqual=Cyc_Absyn_Aliasable,_tmp311))))))),_tmp310))));}else{
# 517
struct Cyc_Absyn_Kind*_tmp314;struct Cyc_Core_Opt*_tmp313;*_tmp112=((_tmp313=_cycalloc(sizeof(*_tmp313)),((_tmp313->v=Cyc_Tcutil_kind_to_bound(((_tmp314=_cycalloc_atomic(sizeof(*_tmp314)),((_tmp314->kind=Cyc_Absyn_BoxKind,((_tmp314->aliasqual=_tmp113,_tmp314))))))),_tmp313))));}
continue;_LLA4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp114=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp10E;if(_tmp114->tag != 2)goto _LLA6;else{_tmp115=(struct Cyc_Core_Opt**)& _tmp114->f1;if((_tmp114->f2)->kind != Cyc_Absyn_BoxKind)goto _LLA6;if((_tmp114->f2)->aliasqual != Cyc_Absyn_Top)goto _LLA6;}}if(!constrain_top_kind)goto _LLA6;_LLA5:
# 520
{struct Cyc_Absyn_Kind*_tmp317;struct Cyc_Core_Opt*_tmp316;*_tmp115=((_tmp316=_cycalloc(sizeof(*_tmp316)),((_tmp316->v=Cyc_Tcutil_kind_to_bound(((_tmp317=_cycalloc_atomic(sizeof(*_tmp317)),((_tmp317->kind=Cyc_Absyn_BoxKind,((_tmp317->aliasqual=Cyc_Absyn_Aliasable,_tmp317))))))),_tmp316))));}
continue;_LLA6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp116=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp10E;if(_tmp116->tag != 2)goto _LLA8;else{_tmp117=(struct Cyc_Core_Opt**)& _tmp116->f1;_tmp118=_tmp116->f2;}}_LLA7:
# 523
{struct Cyc_Core_Opt*_tmp318;*_tmp117=((_tmp318=_cycalloc(sizeof(*_tmp318)),((_tmp318->v=Cyc_Tcutil_kind_to_bound(_tmp118),_tmp318))));}continue;_LLA8: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp119=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp10E;if(_tmp119->tag != 0)goto _LLAA;else{if((_tmp119->f1)->kind != Cyc_Absyn_MemKind)goto _LLAA;_tmp11A=(_tmp119->f1)->aliasqual;}}_LLA9:
# 525
{const char*_tmp320;void*_tmp31F[3];struct Cyc_String_pa_PrintArg_struct _tmp31E;struct Cyc_String_pa_PrintArg_struct _tmp31D;struct Cyc_Absyn_Kind*_tmp31C;struct Cyc_String_pa_PrintArg_struct _tmp31B;(_tmp31B.tag=0,((_tmp31B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp31C=_cycalloc_atomic(sizeof(*_tmp31C)),((_tmp31C->kind=Cyc_Absyn_MemKind,((_tmp31C->aliasqual=_tmp11A,_tmp31C)))))))),((_tmp31D.tag=0,((_tmp31D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp31E.tag=0,((_tmp31E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp31F[0]=& _tmp31E,((_tmp31F[1]=& _tmp31D,((_tmp31F[2]=& _tmp31B,Cyc_Tcutil_terr(loc,((_tmp320="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp320,sizeof(char),57))),_tag_dyneither(_tmp31F,sizeof(void*),3)))))))))))))))))));}
continue;_LLAA:;_LLAB:
 continue;_LL9F:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 533
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _dyneither_ptr*_tmp129=(*ad->name).f2;
# 540
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp12A=(void*)atts->hd;_LLAD: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp12B=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp12A;if(_tmp12B->tag != 7)goto _LLAF;}_LLAE:
 goto _LLB0;_LLAF: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp12C=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp12A;if(_tmp12C->tag != 6)goto _LLB1;}_LLB0:
 continue;_LLB1:;_LLB2:
# 545
{const char*_tmp325;void*_tmp324[2];struct Cyc_String_pa_PrintArg_struct _tmp323;struct Cyc_String_pa_PrintArg_struct _tmp322;(_tmp322.tag=0,((_tmp322.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp129),((_tmp323.tag=0,((_tmp323.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp324[0]=& _tmp323,((_tmp324[1]=& _tmp322,Cyc_Tcutil_terr(loc,((_tmp325="bad attribute %s in  %s definition",_tag_dyneither(_tmp325,sizeof(char),35))),_tag_dyneither(_tmp324,sizeof(void*),2)))))))))))));}
goto _LLAC;_LLAC:;}}{
# 550
struct Cyc_List_List*_tmp131=ad->tvs;
# 552
{union Cyc_Absyn_Nmspace _tmp132=(*ad->name).f1;_LLB4: if((_tmp132.Rel_n).tag != 1)goto _LLB6;if((struct Cyc_List_List*)(_tmp132.Rel_n).val != 0)goto _LLB6;_LLB5:
# 555
(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
goto _LLB3;_LLB6:;_LLB7:
# 558
 goto _LLB3;_LLB3:;}
# 562
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 564
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 568
{struct _tuple14 _tmp326;struct _tuple14 _tmp134=(_tmp326.f1=ad->impl,((_tmp326.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp129),_tmp326)));struct Cyc_List_List*_tmp135;struct Cyc_List_List*_tmp136;struct Cyc_List_List*_tmp137;int _tmp138;struct Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*_tmp13B;int _tmp13C;struct Cyc_Absyn_Aggrdecl**_tmp13D;struct Cyc_Absyn_Aggrdecl**_tmp13E;_LLB9: if(_tmp134.f1 != 0)goto _LLBB;if(_tmp134.f2 != 0)goto _LLBB;_LLBA:
# 571
 Cyc_Tc_rule_out_memkind(loc,_tmp129,_tmp131,0);
{struct Cyc_Absyn_Aggrdecl**_tmp327;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp129,((_tmp327=_cycalloc(sizeof(*_tmp327)),((_tmp327[0]=ad,_tmp327)))));}
goto _LLB8;_LLBB: if(_tmp134.f1 == 0)goto _LLBD;_tmp135=(_tmp134.f1)->exist_vars;_tmp136=(_tmp134.f1)->rgn_po;_tmp137=(_tmp134.f1)->fields;_tmp138=(_tmp134.f1)->tagged;if(_tmp134.f2 != 0)goto _LLBD;_LLBC: {
# 577
struct Cyc_Absyn_Aggrdecl*_tmp32A;struct Cyc_Absyn_Aggrdecl**_tmp329;struct Cyc_Absyn_Aggrdecl**_tmp140=(_tmp329=_cycalloc(sizeof(*_tmp329)),((_tmp329[0]=((_tmp32A=_cycalloc(sizeof(*_tmp32A)),((_tmp32A->kind=ad->kind,((_tmp32A->sc=Cyc_Absyn_Extern,((_tmp32A->name=ad->name,((_tmp32A->tvs=_tmp131,((_tmp32A->impl=0,((_tmp32A->attributes=ad->attributes,_tmp32A)))))))))))))),_tmp329)));
# 579
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp129,_tmp140);
# 582
Cyc_Tcutil_check_unique_tvars(loc,_tmp135);
# 584
Cyc_Tcutil_add_tvar_identities(_tmp135);
# 587
if(_tmp138  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp32D;void*_tmp32C;(_tmp32C=0,Cyc_Tcutil_terr(loc,((_tmp32D="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp32D,sizeof(char),56))),_tag_dyneither(_tmp32C,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp131,_tmp135),_tmp136,_tmp137);
# 591
Cyc_Tc_rule_out_memkind(loc,_tmp129,_tmp131,0);
# 594
Cyc_Tc_rule_out_memkind(loc,_tmp129,_tmp135,1);
# 596
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp138){
# 599
struct Cyc_List_List*f=_tmp137;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)){
const char*_tmp333;void*_tmp332[3];struct Cyc_String_pa_PrintArg_struct _tmp331;struct Cyc_String_pa_PrintArg_struct _tmp330;struct Cyc_String_pa_PrintArg_struct _tmp32F;(_tmp32F.tag=0,((_tmp32F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)),((_tmp330.tag=0,((_tmp330.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp129),((_tmp331.tag=0,((_tmp331.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp332[0]=& _tmp331,((_tmp332[1]=& _tmp330,((_tmp332[2]=& _tmp32F,Cyc_Tcutil_warn(loc,((_tmp333="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp333,sizeof(char),98))),_tag_dyneither(_tmp332,sizeof(void*),3)))))))))))))))))));}}}
# 604
*_tmp140=ad;
goto _LLB8;}_LLBD: if(_tmp134.f1 == 0)goto _LLBF;_tmp139=(_tmp134.f1)->exist_vars;_tmp13A=(_tmp134.f1)->rgn_po;_tmp13B=(_tmp134.f1)->fields;_tmp13C=(_tmp134.f1)->tagged;if(_tmp134.f2 == 0)goto _LLBF;_tmp13D=*_tmp134.f2;_LLBE:
# 608
 if(ad->kind != (*_tmp13D)->kind){
const char*_tmp336;void*_tmp335;(_tmp335=0,Cyc_Tcutil_terr(loc,((_tmp336="cannot reuse struct names for unions and vice-versa",_tag_dyneither(_tmp336,sizeof(char),52))),_tag_dyneither(_tmp335,sizeof(void*),0)));}{
# 611
struct Cyc_Absyn_Aggrdecl*_tmp14C=*_tmp13D;
# 613
{struct Cyc_Absyn_Aggrdecl*_tmp337;*_tmp13D=((_tmp337=_cycalloc(sizeof(*_tmp337)),((_tmp337->kind=ad->kind,((_tmp337->sc=Cyc_Absyn_Extern,((_tmp337->name=ad->name,((_tmp337->tvs=_tmp131,((_tmp337->impl=0,((_tmp337->attributes=ad->attributes,_tmp337))))))))))))));}
# 617
Cyc_Tcutil_check_unique_tvars(loc,_tmp139);
# 619
Cyc_Tcutil_add_tvar_identities(_tmp139);
# 621
if(_tmp13C  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp33A;void*_tmp339;(_tmp339=0,Cyc_Tcutil_terr(loc,((_tmp33A="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp33A,sizeof(char),56))),_tag_dyneither(_tmp339,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp131,_tmp139),_tmp13A,_tmp13B);
# 625
*_tmp13D=_tmp14C;
_tmp13E=_tmp13D;goto _LLC0;};_LLBF: if(_tmp134.f1 != 0)goto _LLB8;if(_tmp134.f2 == 0)goto _LLB8;_tmp13E=*_tmp134.f2;_LLC0: {
# 629
struct Cyc_Absyn_Aggrdecl*_tmp150=Cyc_Tcdecl_merge_aggrdecl(*_tmp13E,ad,loc,Cyc_Tc_tc_msg);
if(_tmp150 == 0)
return;else{
# 633
Cyc_Tc_rule_out_memkind(loc,_tmp129,_tmp131,0);
# 636
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp129,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 639
*_tmp13E=_tmp150;
ad=_tmp150;
goto _LLB8;}}_LLB8:;}{
# 645
struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp340;struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp33F;struct _tuple13*_tmp33E;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp129,(
(_tmp33E=_cycalloc(sizeof(*_tmp33E)),((_tmp33E->f1=(void*)((_tmp340=_cycalloc(sizeof(*_tmp340)),((_tmp340[0]=((_tmp33F.tag=1,((_tmp33F.f1=ad,_tmp33F)))),_tmp340)))),((_tmp33E->f2=1,_tmp33E)))))));};};}
# 649
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp154=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt**_tmp156;struct Cyc_Core_Opt**_tmp158;struct Cyc_Core_Opt**_tmp15A;struct Cyc_Core_Opt**_tmp15C;struct Cyc_Core_Opt**_tmp15E;struct Cyc_Core_Opt**_tmp160;struct Cyc_Core_Opt**_tmp163;enum Cyc_Absyn_KindQual _tmp164;enum Cyc_Absyn_KindQual _tmp166;enum Cyc_Absyn_AliasQual _tmp168;_LLC2: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp155=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp154;if(_tmp155->tag != 1)goto _LLC4;else{_tmp156=(struct Cyc_Core_Opt**)& _tmp155->f1;}}_LLC3:
 _tmp158=_tmp156;goto _LLC5;_LLC4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp157=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp154;if(_tmp157->tag != 2)goto _LLC6;else{_tmp158=(struct Cyc_Core_Opt**)& _tmp157->f1;if((_tmp157->f2)->kind != Cyc_Absyn_MemKind)goto _LLC6;if((_tmp157->f2)->aliasqual != Cyc_Absyn_Top)goto _LLC6;}}_LLC5:
# 654
 _tmp15A=_tmp158;goto _LLC7;_LLC6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp159=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp154;if(_tmp159->tag != 2)goto _LLC8;else{_tmp15A=(struct Cyc_Core_Opt**)& _tmp159->f1;if((_tmp159->f2)->kind != Cyc_Absyn_MemKind)goto _LLC8;if((_tmp159->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLC8;}}_LLC7:
# 656
{struct Cyc_Core_Opt*_tmp341;*_tmp15A=((_tmp341=_cycalloc(sizeof(*_tmp341)),((_tmp341->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp341))));}goto _LLC1;_LLC8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp154;if(_tmp15B->tag != 2)goto _LLCA;else{_tmp15C=(struct Cyc_Core_Opt**)& _tmp15B->f1;if((_tmp15B->f2)->kind != Cyc_Absyn_AnyKind)goto _LLCA;if((_tmp15B->f2)->aliasqual != Cyc_Absyn_Top)goto _LLCA;}}_LLC9:
 _tmp15E=_tmp15C;goto _LLCB;_LLCA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp154;if(_tmp15D->tag != 2)goto _LLCC;else{_tmp15E=(struct Cyc_Core_Opt**)& _tmp15D->f1;if((_tmp15D->f2)->kind != Cyc_Absyn_AnyKind)goto _LLCC;if((_tmp15D->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLCC;}}_LLCB:
# 659
{struct Cyc_Core_Opt*_tmp342;*_tmp15E=((_tmp342=_cycalloc(sizeof(*_tmp342)),((_tmp342->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp342))));}goto _LLC1;_LLCC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp154;if(_tmp15F->tag != 2)goto _LLCE;else{_tmp160=(struct Cyc_Core_Opt**)& _tmp15F->f1;if((_tmp15F->f2)->kind != Cyc_Absyn_RgnKind)goto _LLCE;if((_tmp15F->f2)->aliasqual != Cyc_Absyn_Top)goto _LLCE;}}_LLCD:
# 661
{struct Cyc_Core_Opt*_tmp343;*_tmp160=((_tmp343=_cycalloc(sizeof(*_tmp343)),((_tmp343->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp343))));}goto _LLC1;_LLCE: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp161=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp154;if(_tmp161->tag != 0)goto _LLD0;else{if((_tmp161->f1)->kind != Cyc_Absyn_RgnKind)goto _LLD0;if((_tmp161->f1)->aliasqual != Cyc_Absyn_Top)goto _LLD0;}}_LLCF:
# 663
{const char*_tmp348;void*_tmp347[2];struct Cyc_String_pa_PrintArg_struct _tmp346;struct Cyc_String_pa_PrintArg_struct _tmp345;(_tmp345.tag=0,((_tmp345.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp346.tag=0,((_tmp346.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp347[0]=& _tmp346,((_tmp347[1]=& _tmp345,Cyc_Tcutil_terr(loc,((_tmp348="type %s attempts to abstract type variable %s of kind TR",_tag_dyneither(_tmp348,sizeof(char),57))),_tag_dyneither(_tmp347,sizeof(void*),2)))))))))))));}
goto _LLC1;_LLD0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp162=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp154;if(_tmp162->tag != 2)goto _LLD2;else{_tmp163=(struct Cyc_Core_Opt**)& _tmp162->f1;_tmp164=(_tmp162->f2)->kind;if((_tmp162->f2)->aliasqual != Cyc_Absyn_Unique)goto _LLD2;}}_LLD1:
 _tmp166=_tmp164;goto _LLD3;_LLD2: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp165=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp154;if(_tmp165->tag != 0)goto _LLD4;else{_tmp166=(_tmp165->f1)->kind;if((_tmp165->f1)->aliasqual != Cyc_Absyn_Unique)goto _LLD4;}}_LLD3:
# 667
{const char*_tmp350;void*_tmp34F[3];struct Cyc_String_pa_PrintArg_struct _tmp34E;struct Cyc_String_pa_PrintArg_struct _tmp34D;struct Cyc_Absyn_Kind*_tmp34C;struct Cyc_String_pa_PrintArg_struct _tmp34B;(_tmp34B.tag=0,((_tmp34B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 669
Cyc_Absynpp_kind2string(((_tmp34C=_cycalloc_atomic(sizeof(*_tmp34C)),((_tmp34C->kind=_tmp166,((_tmp34C->aliasqual=Cyc_Absyn_Unique,_tmp34C)))))))),((_tmp34D.tag=0,((_tmp34D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp34E.tag=0,((_tmp34E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp34F[0]=& _tmp34E,((_tmp34F[1]=& _tmp34D,((_tmp34F[2]=& _tmp34B,Cyc_Tcutil_terr(loc,((_tmp350="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp350,sizeof(char),57))),_tag_dyneither(_tmp34F,sizeof(void*),3)))))))))))))))))));}goto _LLC1;_LLD4: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp167=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp154;if(_tmp167->tag != 0)goto _LLD6;else{if((_tmp167->f1)->kind != Cyc_Absyn_MemKind)goto _LLD6;_tmp168=(_tmp167->f1)->aliasqual;}}_LLD5:
# 671
{const char*_tmp358;void*_tmp357[3];struct Cyc_String_pa_PrintArg_struct _tmp356;struct Cyc_String_pa_PrintArg_struct _tmp355;struct Cyc_Absyn_Kind*_tmp354;struct Cyc_String_pa_PrintArg_struct _tmp353;(_tmp353.tag=0,((_tmp353.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 673
Cyc_Absynpp_kind2string(((_tmp354=_cycalloc_atomic(sizeof(*_tmp354)),((_tmp354->kind=Cyc_Absyn_MemKind,((_tmp354->aliasqual=_tmp168,_tmp354)))))))),((_tmp355.tag=0,((_tmp355.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp356.tag=0,((_tmp356.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp357[0]=& _tmp356,((_tmp357[1]=& _tmp355,((_tmp357[2]=& _tmp353,Cyc_Tcutil_terr(loc,((_tmp358="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp358,sizeof(char),57))),_tag_dyneither(_tmp357,sizeof(void*),3)))))))))))))))))));}goto _LLC1;_LLD6:;_LLD7:
 goto _LLC1;_LLC1:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 680
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 689
{struct Cyc_List_List*_tmp17C=fields;for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
struct Cyc_Absyn_Datatypefield*_tmp17D=(struct Cyc_Absyn_Datatypefield*)_tmp17C->hd;
# 692
{struct Cyc_List_List*typs=_tmp17D->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp17D->loc,te,tvs,& Cyc_Tcutil_tmk,0,(*((struct _tuple15*)typs->hd)).f2);{
# 695
struct _RegionHandle _tmp17E=_new_region("temp");struct _RegionHandle*temp=& _tmp17E;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple15*)typs->hd)).f2)){
const char*_tmp35C;void*_tmp35B[1];struct Cyc_String_pa_PrintArg_struct _tmp35A;(_tmp35A.tag=0,((_tmp35A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 699
Cyc_Absynpp_qvar2string(_tmp17D->name)),((_tmp35B[0]=& _tmp35A,Cyc_Tcutil_terr(_tmp17D->loc,((_tmp35C="noalias pointers in datatypes are not allowed (%s)",_tag_dyneither(_tmp35C,sizeof(char),51))),_tag_dyneither(_tmp35B,sizeof(void*),1)))))));}
# 701
((*((struct _tuple15*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp17D->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)typs->hd)).f2);
# 695
;_pop_region(temp);};}}{
# 706
union Cyc_Absyn_Nmspace _tmp182=(*_tmp17D->name).f1;_LLD9: if((_tmp182.Rel_n).tag != 1)goto _LLDB;if((struct Cyc_List_List*)(_tmp182.Rel_n).val != 0)goto _LLDB;_LLDA:
# 708
 if(is_extensible)
(*_tmp17D->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{
# 711
(*_tmp17D->name).f1=(*name).f1;}
goto _LLD8;_LLDB: if((_tmp182.Rel_n).tag != 1)goto _LLDD;_LLDC:
# 714
{const char*_tmp360;void*_tmp35F[1];struct Cyc_String_pa_PrintArg_struct _tmp35E;(_tmp35E.tag=0,((_tmp35E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp17D->name)),((_tmp35F[0]=& _tmp35E,Cyc_Tcutil_terr(_tmp17D->loc,((_tmp360="qualified datatypefield declarations are not allowed (%s)",_tag_dyneither(_tmp360,sizeof(char),58))),_tag_dyneither(_tmp35F,sizeof(void*),1)))))));}
goto _LLD8;_LLDD: if((_tmp182.C_n).tag != 3)goto _LLDF;_LLDE:
 goto _LLD8;_LLDF: if((_tmp182.Abs_n).tag != 2)goto _LLE1;_LLE0:
 goto _LLD8;_LLE1: if((_tmp182.Loc_n).tag != 4)goto _LLD8;_LLE2: {
const char*_tmp363;void*_tmp362;(_tmp362=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp363="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp363,sizeof(char),24))),_tag_dyneither(_tmp362,sizeof(void*),0)));}_LLD8:;};}}{
# 723
struct Cyc_List_List*fields2;
if(is_extensible){
# 726
int _tmp188=1;
struct Cyc_List_List*_tmp189=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp188,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp188)
fields2=_tmp189;else{
# 731
fields2=0;}}else{
# 733
struct _RegionHandle _tmp18A=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp18A;_push_region(uprev_rgn);
# 735
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp18B=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp18B->name).f2)){
const char*_tmp368;void*_tmp367[2];struct Cyc_String_pa_PrintArg_struct _tmp366;struct Cyc_String_pa_PrintArg_struct _tmp365;(_tmp365.tag=0,((_tmp365.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp366.tag=0,((_tmp366.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp18B->name).f2),((_tmp367[0]=& _tmp366,((_tmp367[1]=& _tmp365,Cyc_Tcutil_terr(_tmp18B->loc,((_tmp368="duplicate field name %s in %s",_tag_dyneither(_tmp368,sizeof(char),30))),_tag_dyneither(_tmp367,sizeof(void*),2)))))))))))));}else{
# 741
struct Cyc_List_List*_tmp369;prev_fields=((_tmp369=_region_malloc(uprev_rgn,sizeof(*_tmp369)),((_tmp369->hd=(*_tmp18B->name).f2,((_tmp369->tl=prev_fields,_tmp369))))));}
# 743
if(_tmp18B->sc != Cyc_Absyn_Public){
{const char*_tmp36D;void*_tmp36C[1];struct Cyc_String_pa_PrintArg_struct _tmp36B;(_tmp36B.tag=0,((_tmp36B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp18B->name).f2),((_tmp36C[0]=& _tmp36B,Cyc_Tcutil_warn(loc,((_tmp36D="ignoring scope of field %s",_tag_dyneither(_tmp36D,sizeof(char),27))),_tag_dyneither(_tmp36C,sizeof(void*),1)))))));}
_tmp18B->sc=Cyc_Absyn_Public;}}}
# 748
fields2=fields;}
# 735
;_pop_region(uprev_rgn);}
# 754
{struct Cyc_List_List*_tmp194=fields;for(0;_tmp194 != 0;_tmp194=_tmp194->tl){
struct Cyc_Absyn_Datatypefield*_tmp195=(struct Cyc_Absyn_Datatypefield*)_tmp194->hd;
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp373;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp372;struct _tuple13*_tmp371;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp195->name).f2,(
(_tmp371=_cycalloc(sizeof(*_tmp371)),((_tmp371->f1=(void*)((_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373[0]=((_tmp372.tag=2,((_tmp372.f1=tudres,((_tmp372.f2=_tmp195,_tmp372)))))),_tmp373)))),((_tmp371->f2=1,_tmp371)))))));}}
# 760
return fields2;};}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 763
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _dyneither_ptr*v=(*tud->name).f2;
const char*_tmp375;const char*_tmp374;struct _dyneither_ptr obj=tud->is_extensible?(_tmp375="@extensible datatype",_tag_dyneither(_tmp375,sizeof(char),21)):((_tmp374="datatype",_tag_dyneither(_tmp374,sizeof(char),9)));
# 770
struct Cyc_List_List*tvs=tud->tvs;
# 773
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 775
Cyc_Tcutil_add_tvar_identities(tvs);{
# 780
struct _RegionHandle _tmp199=_new_region("temp");struct _RegionHandle*temp=& _tmp199;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 790 "tc.cyc"
{struct _handler_cons _tmp19A;_push_handler(& _tmp19A);{int _tmp19C=0;if(setjmp(_tmp19A.handler))_tmp19C=1;if(!_tmp19C){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_exn_struct _tmp378;struct Cyc_Dict_Absent_exn_struct*_tmp377;(int)_throw((void*)((_tmp377=_cycalloc_atomic(sizeof(*_tmp377)),((_tmp377[0]=((_tmp378.tag=Cyc_Dict_Absent,_tmp378)),_tmp377)))));}
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 796
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 791
;_pop_handler();}else{void*_tmp19B=(void*)_exn_thrown;void*_tmp1A0=_tmp19B;void*_tmp1A2;_LLE4: {struct Cyc_Dict_Absent_exn_struct*_tmp1A1=(struct Cyc_Dict_Absent_exn_struct*)_tmp1A0;if(_tmp1A1->tag != Cyc_Dict_Absent)goto _LLE6;}_LLE5:
# 800
{union Cyc_Absyn_Nmspace _tmp1A3=(*tud->name).f1;_LLE9: if((_tmp1A3.Rel_n).tag != 1)goto _LLEB;if((struct Cyc_List_List*)(_tmp1A3.Rel_n).val != 0)goto _LLEB;_LLEA:
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LLE8;_LLEB:;_LLEC:
 goto _LLE8;_LLE8:;}{
# 804
struct Cyc_Absyn_Datatypedecl***_tmp1A4=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
{struct Cyc_Absyn_Datatypedecl***_tmp379;tud_opt=(unsigned int)_tmp1A4?(_tmp379=_region_malloc(temp,sizeof(*_tmp379)),((_tmp379[0]=*_tmp1A4,_tmp379))): 0;}
goto _LLE3;};_LLE6: _tmp1A2=_tmp1A0;_LLE7:(void)_throw(_tmp1A2);_LLE3:;}};}{
# 811
struct _tuple16 _tmp37A;struct _tuple16 _tmp1A7=(_tmp37A.f1=tud->fields,((_tmp37A.f2=tud_opt,_tmp37A)));struct Cyc_List_List**_tmp1A8;struct Cyc_List_List**_tmp1A9;struct Cyc_Absyn_Datatypedecl**_tmp1AA;struct Cyc_Absyn_Datatypedecl**_tmp1AB;_LLEE: if(_tmp1A7.f1 != 0)goto _LLF0;if(_tmp1A7.f2 != 0)goto _LLF0;_LLEF:
# 814
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
{struct Cyc_Absyn_Datatypedecl**_tmp37B;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,((_tmp37B=_cycalloc(sizeof(*_tmp37B)),((_tmp37B[0]=tud,_tmp37B)))));}
goto _LLED;_LLF0: if(_tmp1A7.f1 == 0)goto _LLF2;_tmp1A8=(struct Cyc_List_List**)&(_tmp1A7.f1)->v;if(_tmp1A7.f2 != 0)goto _LLF2;_LLF1: {
# 819
struct Cyc_Absyn_Datatypedecl*_tmp37E;struct Cyc_Absyn_Datatypedecl**_tmp37D;struct Cyc_Absyn_Datatypedecl**_tmp1AD=(_tmp37D=_cycalloc(sizeof(*_tmp37D)),((_tmp37D[0]=((_tmp37E=_cycalloc(sizeof(*_tmp37E)),((_tmp37E->sc=Cyc_Absyn_Extern,((_tmp37E->name=tud->name,((_tmp37E->tvs=tvs,((_tmp37E->fields=0,((_tmp37E->is_extensible=tud->is_extensible,_tmp37E)))))))))))),_tmp37D)));
# 821
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp1AD);
# 824
*_tmp1A8=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1A8,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
*_tmp1AD=tud;
goto _LLED;}_LLF2: if(_tmp1A7.f1 == 0)goto _LLF4;_tmp1A9=(struct Cyc_List_List**)&(_tmp1A7.f1)->v;if(_tmp1A7.f2 == 0)goto _LLF4;_tmp1AA=*_tmp1A7.f2;_LLF3: {
# 829
struct Cyc_Absyn_Datatypedecl*_tmp1B0=*_tmp1AA;
# 832
if((!tud->is_extensible  && (unsigned int)_tmp1B0) && _tmp1B0->is_extensible)
tud->is_extensible=1;
# 835
{struct Cyc_Absyn_Datatypedecl*_tmp37F;*_tmp1AA=((_tmp37F=_cycalloc(sizeof(*_tmp37F)),((_tmp37F->sc=Cyc_Absyn_Extern,((_tmp37F->name=tud->name,((_tmp37F->tvs=tvs,((_tmp37F->fields=0,((_tmp37F->is_extensible=tud->is_extensible,_tmp37F))))))))))));}
# 839
*_tmp1A9=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1A9,tvs,tud);
# 843
*_tmp1AA=_tmp1B0;
_tmp1AB=_tmp1AA;goto _LLF5;}_LLF4: if(_tmp1A7.f1 != 0)goto _LLED;if(_tmp1A7.f2 == 0)goto _LLED;_tmp1AB=*_tmp1A7.f2;_LLF5: {
# 847
struct Cyc_Absyn_Datatypedecl*_tmp1B2=Cyc_Tcdecl_merge_datatypedecl(*_tmp1AB,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
if(_tmp1B2 == 0){
_npop_handler(0);return;}else{
# 852
*_tmp1AB=_tmp1B2;
goto _LLED;}}_LLED:;};}
# 781 "tc.cyc"
;_pop_region(temp);};}
# 859 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _dyneither_ptr*v=(*ed->name).f2;
# 866
{union Cyc_Absyn_Nmspace _tmp1B5=(*ed->name).f1;_LLF7: if((_tmp1B5.Rel_n).tag != 1)goto _LLF9;if((struct Cyc_List_List*)(_tmp1B5.Rel_n).val != 0)goto _LLF9;_LLF8:
# 869
(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
goto _LLF6;_LLF9:;_LLFA:
 goto _LLF6;_LLF6:;}
# 875
if(ed->fields != 0){struct _RegionHandle _tmp1B6=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1B6;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1B7=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 881
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1B7->name).f2)){
const char*_tmp383;void*_tmp382[1];struct Cyc_String_pa_PrintArg_struct _tmp381;(_tmp381.tag=0,((_tmp381.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1B7->name).f2),((_tmp382[0]=& _tmp381,Cyc_Tcutil_terr(_tmp1B7->loc,((_tmp383="duplicate field name %s",_tag_dyneither(_tmp383,sizeof(char),24))),_tag_dyneither(_tmp382,sizeof(void*),1)))))));}else{
# 884
struct Cyc_List_List*_tmp384;prev_fields=((_tmp384=_region_malloc(uprev_rgn,sizeof(*_tmp384)),((_tmp384->hd=(*_tmp1B7->name).f2,((_tmp384->tl=prev_fields,_tmp384))))));}
# 886
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp1B7->name).f2)!= 0){
const char*_tmp388;void*_tmp387[1];struct Cyc_String_pa_PrintArg_struct _tmp386;(_tmp386.tag=0,((_tmp386.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1B7->name).f2),((_tmp387[0]=& _tmp386,Cyc_Tcutil_terr(_tmp1B7->loc,((_tmp388="enum field name %s shadows global name",_tag_dyneither(_tmp388,sizeof(char),39))),_tag_dyneither(_tmp387,sizeof(void*),1)))))));}
# 889
if(_tmp1B7->tag == 0){
_tmp1B7->tag=Cyc_Absyn_uint_exp(tag_count,_tmp1B7->loc);
++ tag_count;}else{
# 894
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1B7->tag))){
unsigned int _tmp1C0;int _tmp1C1;struct _tuple11 _tmp1BF=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1B7->tag));_tmp1C0=_tmp1BF.f1;_tmp1C1=_tmp1BF.f2;
if(_tmp1C1)tag_count=_tmp1C0 + 1;}}
# 898
(*_tmp1B7->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}}
# 876
;_pop_region(uprev_rgn);}
# 903
{struct _handler_cons _tmp1C2;_push_handler(& _tmp1C2);{int _tmp1C4=0;if(setjmp(_tmp1C2.handler))_tmp1C4=1;if(!_tmp1C4){
{struct Cyc_Absyn_Enumdecl**_tmp1C5=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
struct Cyc_Absyn_Enumdecl*_tmp1C6=Cyc_Tcdecl_merge_enumdecl(*_tmp1C5,ed,loc,Cyc_Tc_tc_msg);
if(_tmp1C6 == 0){_npop_handler(0);return;}
*_tmp1C5=_tmp1C6;}
# 904
;_pop_handler();}else{void*_tmp1C3=(void*)_exn_thrown;void*_tmp1C8=_tmp1C3;void*_tmp1CA;_LLFC: {struct Cyc_Dict_Absent_exn_struct*_tmp1C9=(struct Cyc_Dict_Absent_exn_struct*)_tmp1C8;if(_tmp1C9->tag != Cyc_Dict_Absent)goto _LLFE;}_LLFD: {
# 909
struct Cyc_Absyn_Enumdecl**_tmp389;struct Cyc_Absyn_Enumdecl**_tmp1CB=(_tmp389=_cycalloc(sizeof(*_tmp389)),((_tmp389[0]=ed,_tmp389)));
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp1CB);
goto _LLFB;}_LLFE: _tmp1CA=_tmp1C8;_LLFF:(void)_throw(_tmp1CA);_LLFB:;}};}
# 913
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1CD=(struct Cyc_Absyn_Enumfield*)fs->hd;
struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp38F;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp38E;struct _tuple13*_tmp38D;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1CD->name).f2,(
(_tmp38D=_cycalloc(sizeof(*_tmp38D)),((_tmp38D->f1=(void*)((_tmp38F=_cycalloc(sizeof(*_tmp38F)),((_tmp38F[0]=((_tmp38E.tag=3,((_tmp38E.f1=ed,((_tmp38E.f2=_tmp1CD,_tmp38E)))))),_tmp38F)))),((_tmp38D->f2=1,_tmp38D)))))));}}
# 922
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1D1=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1D1->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1D1->tag))){
const char*_tmp394;void*_tmp393[2];struct Cyc_String_pa_PrintArg_struct _tmp392;struct Cyc_String_pa_PrintArg_struct _tmp391;(_tmp391.tag=0,((_tmp391.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D1->name).f2),((_tmp392.tag=0,((_tmp392.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp393[0]=& _tmp392,((_tmp393[1]=& _tmp391,Cyc_Tcutil_terr(loc,((_tmp394="enum %s, field %s: expression is not constant",_tag_dyneither(_tmp394,sizeof(char),46))),_tag_dyneither(_tmp393,sizeof(void*),2)))))))))))));}}}}
# 932
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
switch(sc){case Cyc_Absyn_Static: _LL100:
# 935
 if(!in_include){
const char*_tmp397;void*_tmp396;(_tmp396=0,Cyc_Tcutil_warn(loc,((_tmp397="static declaration nested within extern \"C\"",_tag_dyneither(_tmp397,sizeof(char),44))),_tag_dyneither(_tmp396,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LL101:
# 939
{const char*_tmp39A;void*_tmp399;(_tmp399=0,Cyc_Tcutil_warn(loc,((_tmp39A="abstract declaration nested within extern \"C\"",_tag_dyneither(_tmp39A,sizeof(char),46))),_tag_dyneither(_tmp399,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LL102:
 goto _LL103;case Cyc_Absyn_Register: _LL103:
 goto _LL104;case Cyc_Absyn_Extern: _LL104:
 return 1;case Cyc_Absyn_ExternC: _LL105:
# 945
{const char*_tmp39D;void*_tmp39C;(_tmp39C=0,Cyc_Tcutil_warn(loc,((_tmp39D="nested extern \"C\" declaration",_tag_dyneither(_tmp39D,sizeof(char),30))),_tag_dyneither(_tmp39C,sizeof(void*),0)));}
return 1;}}
# 950
static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,struct _tuple12*exp){
unsigned int _tmp1DD;struct _tuple0*_tmp1DE;struct _tuple12 _tmp1DC=*exp;_tmp1DD=_tmp1DC.f1;_tmp1DE=_tmp1DC.f2;{
union Cyc_Absyn_Nmspace _tmp1E0;struct _dyneither_ptr*_tmp1E1;struct _tuple0*_tmp1DF=_tmp1DE;_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;{
union Cyc_Absyn_Nmspace _tmp1E2=_tmp1E0;_LL108: if((_tmp1E2.Rel_n).tag != 1)goto _LL10A;if((struct Cyc_List_List*)(_tmp1E2.Rel_n).val != 0)goto _LL10A;_LL109:
# 956
(*_tmp1DE).f1=Cyc_Absyn_Abs_n(te->ns,1);
goto _LL107;_LL10A:;_LL10B:
 goto _LL107;_LL107:;};};}
# 967
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 974
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;
# 977
struct Cyc_List_List*_tmp1E3=ds0;for(0;_tmp1E3 != 0;(last=_tmp1E3,_tmp1E3=_tmp1E3->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1E3->hd;
unsigned int loc=d->loc;
# 981
void*_tmp1E4=d->r;struct Cyc_Absyn_Vardecl*_tmp1EA;struct Cyc_Absyn_Fndecl*_tmp1EC;struct Cyc_Absyn_Typedefdecl*_tmp1EE;struct Cyc_Absyn_Aggrdecl*_tmp1F0;struct Cyc_Absyn_Datatypedecl*_tmp1F2;struct Cyc_Absyn_Enumdecl*_tmp1F4;struct _dyneither_ptr*_tmp1F8;struct Cyc_List_List*_tmp1F9;union Cyc_Absyn_Nmspace _tmp1FB;struct _dyneither_ptr*_tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp1FF;struct Cyc_List_List*_tmp201;struct Cyc_List_List*_tmp202;_LL10D: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp1E5=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1E5->tag != 2)goto _LL10F;}_LL10E:
 goto _LL110;_LL10F: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp1E6=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1E6->tag != 3)goto _LL111;}_LL110:
# 984
{const char*_tmp3A0;void*_tmp39F;(_tmp39F=0,Cyc_Tcutil_terr(loc,((_tmp3A0="top level let-declarations are not implemented",_tag_dyneither(_tmp3A0,sizeof(char),47))),_tag_dyneither(_tmp39F,sizeof(void*),0)));}
goto _LL10C;_LL111: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp1E7=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1E7->tag != 4)goto _LL113;}_LL112:
# 987
{const char*_tmp3A3;void*_tmp3A2;(_tmp3A2=0,Cyc_Tcutil_terr(loc,((_tmp3A3="top level region declarations are not implemented",_tag_dyneither(_tmp3A3,sizeof(char),50))),_tag_dyneither(_tmp3A2,sizeof(void*),0)));}
goto _LL10C;_LL113: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp1E8=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1E8->tag != 5)goto _LL115;}_LL114:
# 990
{const char*_tmp3A6;void*_tmp3A5;(_tmp3A5=0,Cyc_Tcutil_terr(loc,((_tmp3A6="top level alias declarations are not implemented",_tag_dyneither(_tmp3A6,sizeof(char),49))),_tag_dyneither(_tmp3A5,sizeof(void*),0)));}
goto _LL10C;_LL115: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1E9=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1E9->tag != 0)goto _LL117;else{_tmp1EA=_tmp1E9->f1;}}_LL116:
# 993
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1EA->sc,in_externCinclude))
_tmp1EA->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp1EA,check_var_init,in_externCinclude,exports);
goto _LL10C;_LL117: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp1EB=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1EB->tag != 1)goto _LL119;else{_tmp1EC=_tmp1EB->f1;}}_LL118:
# 998
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1EC->sc,in_externCinclude))
_tmp1EC->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp1EC,in_externCinclude,exports);
goto _LL10C;_LL119: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp1ED=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1ED->tag != 9)goto _LL11B;else{_tmp1EE=_tmp1ED->f1;}}_LL11A:
# 1003
 Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp1EE);
goto _LL10C;_LL11B: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1EF=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1EF->tag != 6)goto _LL11D;else{_tmp1F0=_tmp1EF->f1;}}_LL11C:
# 1006
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F0->sc,in_externCinclude))
_tmp1F0->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp1F0);
goto _LL10C;_LL11D: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1F1=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1F1->tag != 7)goto _LL11F;else{_tmp1F2=_tmp1F1->f1;}}_LL11E:
# 1011
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F2->sc,in_externCinclude))
_tmp1F2->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp1F2);
goto _LL10C;_LL11F: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1F3=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1F3->tag != 8)goto _LL121;else{_tmp1F4=_tmp1F3->f1;}}_LL120:
# 1016
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F4->sc,in_externCinclude))
_tmp1F4->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp1F4);
goto _LL10C;_LL121: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp1F5=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1F5->tag != 14)goto _LL123;}_LL122:
# 1021
{const char*_tmp3A9;void*_tmp3A8;(_tmp3A8=0,Cyc_Tcutil_warn(d->loc,((_tmp3A9="spurious __cyclone_port_on__",_tag_dyneither(_tmp3A9,sizeof(char),29))),_tag_dyneither(_tmp3A8,sizeof(void*),0)));}
goto _LL10C;_LL123: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp1F6=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1F6->tag != 15)goto _LL125;}_LL124:
# 1024
 goto _LL10C;_LL125: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp1F7=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1F7->tag != 10)goto _LL127;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=_tmp1F7->f2;}}_LL126: {
# 1027
struct Cyc_List_List*_tmp20B=te->ns;
struct Cyc_List_List*_tmp3AA;struct Cyc_List_List*_tmp20C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20B,((_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((_tmp3AA->hd=_tmp1F8,((_tmp3AA->tl=0,_tmp3AA)))))));
# 1031
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp1F8)){
ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp1F8);
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp20C,Cyc_Tcenv_empty_genv(grgn));}
# 1036
te->ns=_tmp20C;
Cyc_Tc_tc_decls(te,_tmp1F9,in_externC,in_externCinclude,check_var_init,grgn,exports);
te->ns=_tmp20B;
goto _LL10C;}_LL127: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp1FA=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1FA->tag != 11)goto _LL129;else{_tmp1FB=(_tmp1FA->f1)->f1;_tmp1FC=(_tmp1FA->f1)->f2;_tmp1FD=_tmp1FA->f2;}}_LL128: {
# 1043
struct _dyneither_ptr*first;
struct Cyc_List_List*rest;
{union Cyc_Absyn_Nmspace _tmp20E=_tmp1FB;struct _dyneither_ptr*_tmp20F;struct Cyc_List_List*_tmp210;struct _dyneither_ptr*_tmp211;struct Cyc_List_List*_tmp212;struct _dyneither_ptr*_tmp213;struct Cyc_List_List*_tmp214;_LL12E: if((_tmp20E.Loc_n).tag != 4)goto _LL130;_LL12F:
 goto _LL131;_LL130: if((_tmp20E.Rel_n).tag != 1)goto _LL132;if((struct Cyc_List_List*)(_tmp20E.Rel_n).val != 0)goto _LL132;_LL131:
 goto _LL133;_LL132: if((_tmp20E.C_n).tag != 3)goto _LL134;if((struct Cyc_List_List*)(_tmp20E.C_n).val != 0)goto _LL134;_LL133:
 goto _LL135;_LL134: if((_tmp20E.Abs_n).tag != 2)goto _LL136;if((struct Cyc_List_List*)(_tmp20E.Abs_n).val != 0)goto _LL136;_LL135:
# 1050
 first=_tmp1FC;rest=0;goto _LL12D;_LL136: if((_tmp20E.Rel_n).tag != 1)goto _LL138;if((struct Cyc_List_List*)(_tmp20E.Rel_n).val == 0)goto _LL138;_tmp20F=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp20E.Rel_n).val)->hd;_tmp210=((struct Cyc_List_List*)(_tmp20E.Rel_n).val)->tl;_LL137:
 _tmp211=_tmp20F;_tmp212=_tmp210;goto _LL139;_LL138: if((_tmp20E.C_n).tag != 3)goto _LL13A;if((struct Cyc_List_List*)(_tmp20E.C_n).val == 0)goto _LL13A;_tmp211=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp20E.C_n).val)->hd;_tmp212=((struct Cyc_List_List*)(_tmp20E.C_n).val)->tl;_LL139:
 _tmp213=_tmp211;_tmp214=_tmp212;goto _LL13B;_LL13A: if((_tmp20E.Abs_n).tag != 2)goto _LL12D;if((struct Cyc_List_List*)(_tmp20E.Abs_n).val == 0)goto _LL12D;_tmp213=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp20E.Abs_n).val)->hd;_tmp214=((struct Cyc_List_List*)(_tmp20E.Abs_n).val)->tl;_LL13B:
# 1054
 first=_tmp213;{struct Cyc_List_List*_tmp3AB;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp214,((_tmp3AB=_cycalloc(sizeof(*_tmp3AB)),((_tmp3AB->hd=_tmp1FC,((_tmp3AB->tl=0,_tmp3AB)))))));}goto _LL12D;_LL12D:;}{
# 1056
struct Cyc_List_List*_tmp216=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);
{struct Cyc_List_List*_tmp3AC;ge->availables=((_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((_tmp3AC->hd=_tmp216,((_tmp3AC->tl=ge->availables,_tmp3AC))))));}
Cyc_Tc_tc_decls(te,_tmp1FD,in_externC,in_externCinclude,check_var_init,grgn,exports);
ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
goto _LL10C;};}_LL129: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp1FE=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp1FE->tag != 12)goto _LL12B;else{_tmp1FF=_tmp1FE->f1;}}_LL12A:
# 1063
 Cyc_Tc_tc_decls(te,_tmp1FF,1,in_externCinclude,check_var_init,grgn,exports);
goto _LL10C;_LL12B: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp200=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1E4;if(_tmp200->tag != 13)goto _LL10C;else{_tmp201=_tmp200->f1;_tmp202=_tmp200->f2;}}_LL12C:
# 1067
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp202);{
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp202,(unsigned int)exports?*exports: 0);
Cyc_Tc_tc_decls(te,_tmp201,1,1,check_var_init,grgn,& newexs);
# 1072
for(0;_tmp202 != 0;_tmp202=_tmp202->tl){
struct _tuple12*_tmp218=(struct _tuple12*)_tmp202->hd;
if(!(*_tmp218).f3){
const char*_tmp3B0;void*_tmp3AF[1];struct Cyc_String_pa_PrintArg_struct _tmp3AE;(_tmp3AE.tag=0,((_tmp3AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp218).f2)),((_tmp3AF[0]=& _tmp3AE,Cyc_Tcutil_warn((*_tmp218).f1,((_tmp3B0="%s is exported but not defined",_tag_dyneither(_tmp3B0,sizeof(char),31))),_tag_dyneither(_tmp3AF,sizeof(void*),1)))))));}}
# 1078
goto _LL10C;};_LL10C:;}}
# 1083
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1085
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1102 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1104
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1106
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1110
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp21C=d->r;struct Cyc_Absyn_Vardecl*_tmp21E;struct Cyc_List_List**_tmp220;struct Cyc_List_List**_tmp222;struct Cyc_List_List**_tmp224;struct Cyc_List_List**_tmp226;_LL13D: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp21D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp21C;if(_tmp21D->tag != 0)goto _LL13F;else{_tmp21E=_tmp21D->f1;}}_LL13E:
# 1115
 if((env->in_cinclude  || _tmp21E->sc != Cyc_Absyn_Extern  && _tmp21E->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp21E->name,Cyc_Absyn_uniquergn_qvar))
# 1118
return 1;{
# 1120
union Cyc_Absyn_Nmspace _tmp228;struct _dyneither_ptr*_tmp229;struct _tuple0*_tmp227=_tmp21E->name;_tmp228=_tmp227->f1;_tmp229=_tmp227->f2;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp22A=_tmp228;struct Cyc_List_List*_tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_List_List*_tmp22D;_LL14A: if((_tmp22A.Loc_n).tag != 4)goto _LL14C;_LL14B:
 ns=0;goto _LL149;_LL14C: if((_tmp22A.Rel_n).tag != 1)goto _LL14E;_tmp22B=(struct Cyc_List_List*)(_tmp22A.Rel_n).val;_LL14D:
 ns=_tmp22B;goto _LL149;_LL14E: if((_tmp22A.C_n).tag != 3)goto _LL150;_tmp22C=(struct Cyc_List_List*)(_tmp22A.C_n).val;_LL14F:
 ns=_tmp22C;goto _LL149;_LL150: if((_tmp22A.Abs_n).tag != 2)goto _LL149;_tmp22D=(struct Cyc_List_List*)(_tmp22A.Abs_n).val;_LL151:
 ns=_tmp22D;goto _LL149;_LL149:;}{
# 1128
struct _tuple17*_tmp22E=((struct _tuple17*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp22F=(*_tmp22E).f1;
int _tmp230=(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp22F->ordinaries,_tmp229)).f2;
if(!_tmp230)
(*_tmp22E).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp22E).f2,_tmp229);
# 1136
return _tmp230;};};};_LL13F: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp21F=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp21C;if(_tmp21F->tag != 12)goto _LL141;else{_tmp220=(struct Cyc_List_List**)& _tmp21F->f1;}}_LL140:
 _tmp222=_tmp220;goto _LL142;_LL141: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp221=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp21C;if(_tmp221->tag != 11)goto _LL143;else{_tmp222=(struct Cyc_List_List**)& _tmp221->f2;}}_LL142:
 _tmp224=_tmp222;goto _LL144;_LL143: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp223=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp21C;if(_tmp223->tag != 10)goto _LL145;else{_tmp224=(struct Cyc_List_List**)& _tmp223->f2;}}_LL144:
# 1140
*_tmp224=Cyc_Tc_treeshake_f(env,*_tmp224);
return 1;_LL145: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp225=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp21C;if(_tmp225->tag != 13)goto _LL147;else{_tmp226=(struct Cyc_List_List**)& _tmp225->f1;}}_LL146: {
# 1143
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp226=Cyc_Tc_treeshake_f(env,*_tmp226);
env->in_cinclude=in_cinclude;
return 1;}_LL147:;_LL148:
 return 1;_LL13C:;}
# 1152
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1156
static struct _tuple17*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1158
struct _tuple17*_tmp3B1;return(_tmp3B1=_region_malloc(rgn,sizeof(*_tmp3B1)),((_tmp3B1->f1=ge,((_tmp3B1->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp3B1)))));}struct _tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};
# 1161
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct _RegionHandle _tmp232=_new_region("rgn");struct _RegionHandle*rgn=& _tmp232;_push_region(rgn);
{struct Cyc_Tc_TreeshakeEnv _tmp3B2;struct Cyc_Tc_TreeshakeEnv _tmp233=(_tmp3B2.rgn=rgn,((_tmp3B2.in_cinclude=0,((_tmp3B2.nsdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp3B2)))));
struct Cyc_List_List*_tmp234=Cyc_Tc_treeshake_f(& _tmp233,ds);
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp233.nsdict)){
struct Cyc_List_List*_tmp235=_tmp234;_npop_handler(0);return _tmp235;}{
# 1169
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp233.nsdict);
struct _tuple18 _tmp236=*((struct _tuple18*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp233.nsdict);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp236)){
struct Cyc_Tcenv_Genv*_tmp238;struct Cyc_Set_Set*_tmp239;struct _tuple18 _tmp237=_tmp236;_tmp238=(_tmp237.f2)->f1;_tmp239=(_tmp237.f2)->f2;
_tmp238->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple13*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp238->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple13*y))Cyc_Tc_treeshake_remove_f,_tmp239,_tmp238->ordinaries);}{
# 1175
struct Cyc_List_List*_tmp23A=_tmp234;_npop_handler(0);return _tmp23A;};};}
# 1163
;_pop_region(rgn);}
