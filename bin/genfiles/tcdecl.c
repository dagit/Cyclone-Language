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
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
int Cyc_List_length(struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 64
void Cyc_Position_post_error(struct Cyc_Position_Error*);
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
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 934
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 1152
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple10{enum Cyc_Absyn_Scope f1;int f2;};
# 61 "tcdecl.h"
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int allow_externC_extern_merge);
# 66
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 73
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 75
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 79
void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 82
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 88
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Set_Set;
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
# 138 "tcutil.h"
int Cyc_Tcutil_unify_kindbound(void*,void*);
# 151
void Cyc_Tcutil_explain_failure();
# 153
int Cyc_Tcutil_unify(void*,void*);
# 156
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 233 "tcutil.h"
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 305
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 341
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 74
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);char Cyc_Tcdecl_Incompatible[13]="Incompatible";
# 43 "tcdecl.cyc"
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);static void _tmp163(struct _dyneither_ptr**msg1,struct _dyneither_ptr*ap,unsigned int*_tmp162,unsigned int*_tmp161,void***_tmp15F){for(*_tmp162=0;*_tmp162 < *_tmp161;(*_tmp162)++){struct Cyc_String_pa_PrintArg_struct _tmp15D;struct Cyc_String_pa_PrintArg_struct*_tmp15C;(*_tmp15F)[*_tmp162]=
# 53
*_tmp162 == 0?(void*)((_tmp15C=_cycalloc(sizeof(*_tmp15C)),((_tmp15C[0]=((_tmp15D.tag=0,((_tmp15D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*msg1)),_tmp15D)))),_tmp15C)))):*((void**)_check_dyneither_subscript(*ap,sizeof(void*),(int)(*_tmp162 - 1)));}}
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 49
if(msg1 == 0)
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
const char*_tmp15A;struct _dyneither_ptr fmt2=(struct _dyneither_ptr)Cyc_strconcat(((_tmp15A="%s ",_tag_dyneither(_tmp15A,sizeof(char),4))),(struct _dyneither_ptr)fmt);
unsigned int _tmp162;unsigned int _tmp161;struct _dyneither_ptr _tmp160;void**_tmp15F;unsigned int _tmp15E;struct _dyneither_ptr ap2=(_tmp15E=_get_dyneither_size(ap,sizeof(void*))+ 1,((_tmp15F=(void**)_cycalloc(_check_times(sizeof(void*),_tmp15E)),((_tmp160=_tag_dyneither(_tmp15F,sizeof(void*),_tmp15E),((((_tmp161=_tmp15E,_tmp163(& msg1,& ap,& _tmp162,& _tmp161,& _tmp15F))),_tmp160)))))));
# 54
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)
Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2)));};}
# 60
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 62
const char*_tmp164;struct _dyneither_ptr s0_str=s0 == Cyc_Absyn_Public?(_tmp164="public",_tag_dyneither(_tmp164,sizeof(char),7)): Cyc_Absynpp_scope2string(s0);
const char*_tmp165;struct _dyneither_ptr s1_str=s1 == Cyc_Absyn_Public?(_tmp165="public",_tag_dyneither(_tmp165,sizeof(char),7)): Cyc_Absynpp_scope2string(s1);
const char*_tmp16C;void*_tmp16B[4];struct Cyc_String_pa_PrintArg_struct _tmp16A;struct Cyc_String_pa_PrintArg_struct _tmp169;struct Cyc_String_pa_PrintArg_struct _tmp168;struct Cyc_String_pa_PrintArg_struct _tmp167;(_tmp167.tag=0,((_tmp167.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_scope2string(s0)),((_tmp168.tag=0,((_tmp168.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_scope2string(s1)),((_tmp169.tag=0,((_tmp169.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp16A.tag=0,((_tmp16A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp16B[0]=& _tmp16A,((_tmp16B[1]=& _tmp169,((_tmp16B[2]=& _tmp168,((_tmp16B[3]=& _tmp167,Cyc_Tcdecl_merr(loc,msg,((_tmp16C="%s %s is %s whereas expected scope is %s",_tag_dyneither(_tmp16C,sizeof(char),41))),_tag_dyneither(_tmp16B,sizeof(void*),4)))))))))))))))))))))))));}struct _tuple11{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 68
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int externCmerge){
# 70
{struct _tuple11 _tmp16D;struct _tuple11 _tmp11=(_tmp16D.f1=s0,((_tmp16D.f2=s1,_tmp16D)));_LL1: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LL3;if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LL3;_LL2:
# 72
 goto _LL0;_LL3: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LL5;if(_tmp11.f2 != Cyc_Absyn_Public)goto _LL5;_LL4:
# 74
 goto _LL6;_LL5: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LL7;if(_tmp11.f2 != Cyc_Absyn_Extern)goto _LL7;_LL6:
 goto _LL8;_LL7: if(_tmp11.f1 != Cyc_Absyn_Public)goto _LL9;if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LL9;_LL8:
 goto _LLA;_LL9: if(_tmp11.f1 != Cyc_Absyn_Extern)goto _LLB;if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LLB;_LLA:
 if(externCmerge)goto _LL0;
goto _LLC;_LLB: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LLD;_LLC:
 goto _LLE;_LLD: if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LLF;_LLE:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple10 _tmp16E;return(_tmp16E.f1=s1,((_tmp16E.f2=0,_tmp16E)));};_LLF: if(_tmp11.f2 != Cyc_Absyn_Extern)goto _LL11;_LL10:
# 82
 s1=s0;goto _LL0;_LL11: if(_tmp11.f1 != Cyc_Absyn_Extern)goto _LL13;_LL12:
 goto _LL0;_LL13: if(_tmp11.f1 != Cyc_Absyn_Static)goto _LL15;if(_tmp11.f2 != Cyc_Absyn_Static)goto _LL15;_LL14:
# 85
 goto _LL16;_LL15: if(_tmp11.f1 != Cyc_Absyn_Public)goto _LL17;if(_tmp11.f2 != Cyc_Absyn_Public)goto _LL17;_LL16:
 goto _LL18;_LL17: if(_tmp11.f1 != Cyc_Absyn_Abstract)goto _LL19;if(_tmp11.f2 != Cyc_Absyn_Abstract)goto _LL19;_LL18:
 goto _LL0;_LL19: if(_tmp11.f1 != Cyc_Absyn_Register)goto _LL1B;if(_tmp11.f2 != Cyc_Absyn_Register)goto _LL1B;_LL1A:
 goto _LL0;_LL1B:;_LL1C:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple10 _tmp16F;return(_tmp16F.f1=s1,((_tmp16F.f2=0,_tmp16F)));};_LL0:;}{
# 91
struct _tuple10 _tmp170;return(_tmp170.f1=s1,((_tmp170.f2=1,_tmp170)));};}
# 94
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 96
return Cyc_Tcutil_unify(t0,t1);}
# 100
static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
void*_tmp15=e->r;int _tmp17;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp15;if(_tmp16->tag != 0)goto _LL20;else{if(((_tmp16->f1).Int_c).tag != 5)goto _LL20;_tmp17=((struct _tuple5)((_tmp16->f1).Int_c).val).f2;}}_LL1F:
 return(unsigned int)_tmp17;_LL20:;_LL21: {
struct Cyc_Core_Invalid_argument_exn_struct _tmp176;const char*_tmp175;struct Cyc_Core_Invalid_argument_exn_struct*_tmp174;(int)_throw((void*)((_tmp174=_cycalloc(sizeof(*_tmp174)),((_tmp174[0]=((_tmp176.tag=Cyc_Core_Invalid_argument,((_tmp176.f1=((_tmp175="Tcdecl::get_uint_const_value",_tag_dyneither(_tmp175,sizeof(char),29))),_tmp176)))),_tmp174)))));}_LL1D:;}
# 107
inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 110
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1)){
{const char*_tmp17B;void*_tmp17A[2];struct Cyc_String_pa_PrintArg_struct _tmp179;struct Cyc_String_pa_PrintArg_struct _tmp178;(_tmp178.tag=0,((_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp179.tag=0,((_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp17A[0]=& _tmp179,((_tmp17A[1]=& _tmp178,Cyc_Tcdecl_merr(loc,msg,((_tmp17B="%s %s has a different number of type parameters",_tag_dyneither(_tmp17B,sizeof(char),48))),_tag_dyneither(_tmp17A,sizeof(void*),2)))))))))))));}
return 0;}{
# 114
struct Cyc_List_List*_tmp1F=tvs0;
struct Cyc_List_List*_tmp20=tvs1;
for(0;_tmp1F != 0;(_tmp1F=_tmp1F->tl,_tmp20=_tmp20->tl)){
Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp1F->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp20))->hd)->kind);{
struct Cyc_Absyn_Kind*_tmp21=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp1F->hd)->kind);
struct Cyc_Absyn_Kind*_tmp22=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp20->hd)->kind);
if(_tmp21 == _tmp22)continue;
{const char*_tmp183;void*_tmp182[5];struct Cyc_String_pa_PrintArg_struct _tmp181;struct Cyc_String_pa_PrintArg_struct _tmp180;struct Cyc_String_pa_PrintArg_struct _tmp17F;struct Cyc_String_pa_PrintArg_struct _tmp17E;struct Cyc_String_pa_PrintArg_struct _tmp17D;(_tmp17D.tag=0,((_tmp17D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp22)),((_tmp17E.tag=0,((_tmp17E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp1F->hd)->name),((_tmp17F.tag=0,((_tmp17F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp21)),((_tmp180.tag=0,((_tmp180.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp181.tag=0,((_tmp181.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp182[0]=& _tmp181,((_tmp182[1]=& _tmp180,((_tmp182[2]=& _tmp17F,((_tmp182[3]=& _tmp17E,((_tmp182[4]=& _tmp17D,Cyc_Tcdecl_merr(loc,msg,((_tmp183="%s %s has a different kind (%s) for type parameter %s (%s)",_tag_dyneither(_tmp183,sizeof(char),59))),_tag_dyneither(_tmp182,sizeof(void*),5)))))))))))))))))))))))))))))));}
return 0;};}
# 125
return 1;};}
# 128
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 130
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
{const char*_tmp188;void*_tmp187[2];struct Cyc_String_pa_PrintArg_struct _tmp186;struct Cyc_String_pa_PrintArg_struct _tmp185;(_tmp185.tag=0,((_tmp185.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp186.tag=0,((_tmp186.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp187[0]=& _tmp186,((_tmp187[1]=& _tmp185,Cyc_Tcdecl_merr(loc,msg,((_tmp188="%s %s has different attributes",_tag_dyneither(_tmp188,sizeof(char),31))),_tag_dyneither(_tmp187,sizeof(void*),2)))))))))))));}
return 0;}
# 134
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 137
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
# 139
struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
struct _tuple12*_tmp192;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp191;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp190;struct Cyc_List_List*_tmp18F;inst=((_tmp18F=_cycalloc(sizeof(*_tmp18F)),((_tmp18F->hd=((_tmp192=_cycalloc(sizeof(*_tmp192)),((_tmp192->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs1))->hd,((_tmp192->f2=(void*)((_tmp190=_cycalloc(sizeof(*_tmp190)),((_tmp190[0]=((_tmp191.tag=2,((_tmp191.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd,_tmp191)))),_tmp190)))),_tmp192)))))),((_tmp18F->tl=inst,_tmp18F))))));}
return inst;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple14{void*f1;void*f2;};
# 145
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 147
struct _dyneither_ptr _tmp32=Cyc_Absynpp_qvar2string(d0->name);
int _tmp33=1;
# 153
if(!(d0->kind == d1->kind))return 0;
# 156
{const char*_tmp193;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp193="type",_tag_dyneither(_tmp193,sizeof(char),5))),_tmp32,loc,msg))return 0;}{
# 159
enum Cyc_Absyn_Scope _tmp37;int _tmp38;const char*_tmp194;struct _tuple10 _tmp36=
Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp194="type",_tag_dyneither(_tmp194,sizeof(char),5))),_tmp32,loc,msg,1);_tmp37=_tmp36.f1;_tmp38=_tmp36.f2;
if(!_tmp38)_tmp33=0;
# 164
{const char*_tmp195;if(!Cyc_Tcdecl_check_atts(d0->attributes,d1->attributes,((_tmp195="type",_tag_dyneither(_tmp195,sizeof(char),5))),_tmp32,loc,msg))_tmp33=0;}{
# 166
struct Cyc_Absyn_Aggrdecl*d2;
# 168
{struct _tuple13 _tmp196;struct _tuple13 _tmp3B=(_tmp196.f1=d0->impl,((_tmp196.f2=d1->impl,_tmp196)));struct Cyc_List_List*_tmp3C;struct Cyc_List_List*_tmp3D;struct Cyc_List_List*_tmp3E;int _tmp3F;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp41;struct Cyc_List_List*_tmp42;int _tmp43;_LL23: if(_tmp3B.f2 != 0)goto _LL25;_LL24:
 d2=d0;goto _LL22;_LL25: if(_tmp3B.f1 != 0)goto _LL27;_LL26:
 d2=d1;goto _LL22;_LL27: if(_tmp3B.f1 == 0)goto _LL22;_tmp3C=(_tmp3B.f1)->exist_vars;_tmp3D=(_tmp3B.f1)->rgn_po;_tmp3E=(_tmp3B.f1)->fields;_tmp3F=(_tmp3B.f1)->tagged;if(_tmp3B.f2 == 0)goto _LL22;_tmp40=(_tmp3B.f2)->exist_vars;_tmp41=(_tmp3B.f2)->rgn_po;_tmp42=(_tmp3B.f2)->fields;_tmp43=(_tmp3B.f2)->tagged;_LL28:
# 175
{const char*_tmp197;if(!Cyc_Tcdecl_check_tvs(_tmp3C,_tmp40,((_tmp197="type",_tag_dyneither(_tmp197,sizeof(char),5))),_tmp32,loc,msg))
return 0;}{
# 179
struct Cyc_List_List*_tmp45=Cyc_Tcdecl_build_tvs_map(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp3C),
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp40));
# 184
for(0;_tmp3D != 0  && _tmp41 != 0;
(_tmp3D=_tmp3D->tl,_tmp41=_tmp41->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp3D->hd)).f1,(*((struct _tuple14*)_tmp41->hd)).f1);
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp3D->hd)).f2,(*((struct _tuple14*)_tmp41->hd)).f2);}
# 191
for(0;_tmp3E != 0  && _tmp42 != 0;(_tmp3E=_tmp3E->tl,_tmp42=_tmp42->tl)){
struct _dyneither_ptr*_tmp47;struct Cyc_Absyn_Tqual _tmp48;void*_tmp49;struct Cyc_Absyn_Exp*_tmp4A;struct Cyc_List_List*_tmp4B;struct Cyc_Absyn_Aggrfield*_tmp46=(struct Cyc_Absyn_Aggrfield*)_tmp3E->hd;_tmp47=_tmp46->name;_tmp48=_tmp46->tq;_tmp49=_tmp46->type;_tmp4A=_tmp46->width;_tmp4B=_tmp46->attributes;{
struct _dyneither_ptr*_tmp4D;struct Cyc_Absyn_Tqual _tmp4E;void*_tmp4F;struct Cyc_Absyn_Exp*_tmp50;struct Cyc_List_List*_tmp51;struct Cyc_Absyn_Aggrfield*_tmp4C=(struct Cyc_Absyn_Aggrfield*)_tmp42->hd;_tmp4D=_tmp4C->name;_tmp4E=_tmp4C->tq;_tmp4F=_tmp4C->type;_tmp50=_tmp4C->width;_tmp51=_tmp4C->attributes;
# 195
if(Cyc_strptrcmp(_tmp47,_tmp4D)!= 0){
{const char*_tmp1A0;void*_tmp19F[4];const char*_tmp19E;struct Cyc_String_pa_PrintArg_struct _tmp19D;struct Cyc_String_pa_PrintArg_struct _tmp19C;struct Cyc_String_pa_PrintArg_struct _tmp19B;struct Cyc_String_pa_PrintArg_struct _tmp19A;(_tmp19A.tag=0,((_tmp19A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4D),((_tmp19B.tag=0,((_tmp19B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp19C.tag=0,((_tmp19C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp19D.tag=0,((_tmp19D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp19E="type",_tag_dyneither(_tmp19E,sizeof(char),5)))),((_tmp19F[0]=& _tmp19D,((_tmp19F[1]=& _tmp19C,((_tmp19F[2]=& _tmp19B,((_tmp19F[3]=& _tmp19A,Cyc_Tcdecl_merr(loc,msg,((_tmp1A0="%s %s : field name mismatch %s != %s",_tag_dyneither(_tmp1A0,sizeof(char),37))),_tag_dyneither(_tmp19F,sizeof(void*),4)))))))))))))))))))))))));}
# 198
return 0;}
# 200
if(!Cyc_Tcutil_same_atts(_tmp4B,_tmp51)){
{const char*_tmp1A8;void*_tmp1A7[3];const char*_tmp1A6;struct Cyc_String_pa_PrintArg_struct _tmp1A5;struct Cyc_String_pa_PrintArg_struct _tmp1A4;struct Cyc_String_pa_PrintArg_struct _tmp1A3;(_tmp1A3.tag=0,((_tmp1A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp1A4.tag=0,((_tmp1A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1A5.tag=0,((_tmp1A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1A6="type",_tag_dyneither(_tmp1A6,sizeof(char),5)))),((_tmp1A7[0]=& _tmp1A5,((_tmp1A7[1]=& _tmp1A4,((_tmp1A7[2]=& _tmp1A3,Cyc_Tcdecl_merr(loc,msg,((_tmp1A8="%s %s : attribute mismatch on field %s",_tag_dyneither(_tmp1A8,sizeof(char),39))),_tag_dyneither(_tmp1A7,sizeof(void*),3)))))))))))))))))));}
_tmp33=0;}
# 204
if(!Cyc_Tcutil_equal_tqual(_tmp48,_tmp4E)){
{const char*_tmp1B0;void*_tmp1AF[3];const char*_tmp1AE;struct Cyc_String_pa_PrintArg_struct _tmp1AD;struct Cyc_String_pa_PrintArg_struct _tmp1AC;struct Cyc_String_pa_PrintArg_struct _tmp1AB;(_tmp1AB.tag=0,((_tmp1AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp1AC.tag=0,((_tmp1AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1AD.tag=0,((_tmp1AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1AE="type",_tag_dyneither(_tmp1AE,sizeof(char),5)))),((_tmp1AF[0]=& _tmp1AD,((_tmp1AF[1]=& _tmp1AC,((_tmp1AF[2]=& _tmp1AB,Cyc_Tcdecl_merr(loc,msg,((_tmp1B0="%s %s : qualifier mismatch on field %s",_tag_dyneither(_tmp1B0,sizeof(char),39))),_tag_dyneither(_tmp1AF,sizeof(void*),3)))))))))))))))))));}
_tmp33=0;}
# 208
if(((_tmp4A != 0  && _tmp50 != 0) && 
Cyc_Tcdecl_get_uint_const_value(_tmp4A)!= 
Cyc_Tcdecl_get_uint_const_value(_tmp50) || 
_tmp4A == 0  && _tmp50 != 0) || 
_tmp4A != 0  && _tmp50 == 0){
{const char*_tmp1B8;void*_tmp1B7[3];const char*_tmp1B6;struct Cyc_String_pa_PrintArg_struct _tmp1B5;struct Cyc_String_pa_PrintArg_struct _tmp1B4;struct Cyc_String_pa_PrintArg_struct _tmp1B3;(_tmp1B3.tag=0,((_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp1B4.tag=0,((_tmp1B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1B5.tag=0,((_tmp1B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1B6="type",_tag_dyneither(_tmp1B6,sizeof(char),5)))),((_tmp1B7[0]=& _tmp1B5,((_tmp1B7[1]=& _tmp1B4,((_tmp1B7[2]=& _tmp1B3,Cyc_Tcdecl_merr(loc,msg,((_tmp1B8="%s %s : bitfield mismatch on field %s",_tag_dyneither(_tmp1B8,sizeof(char),38))),_tag_dyneither(_tmp1B7,sizeof(void*),3)))))))))))))))))));}
_tmp33=0;}{
# 217
void*subst_t1=Cyc_Tcutil_substitute(_tmp45,_tmp4F);
if(!Cyc_Tcdecl_check_type(_tmp49,subst_t1)){
{const char*_tmp1BF;void*_tmp1BE[4];struct Cyc_String_pa_PrintArg_struct _tmp1BD;struct Cyc_String_pa_PrintArg_struct _tmp1BC;struct Cyc_String_pa_PrintArg_struct _tmp1BB;struct Cyc_String_pa_PrintArg_struct _tmp1BA;(_tmp1BA.tag=0,((_tmp1BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1)),((_tmp1BB.tag=0,((_tmp1BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp49)),((_tmp1BC.tag=0,((_tmp1BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp1BD.tag=0,((_tmp1BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1BE[0]=& _tmp1BD,((_tmp1BE[1]=& _tmp1BC,((_tmp1BE[2]=& _tmp1BB,((_tmp1BE[3]=& _tmp1BA,Cyc_Tcdecl_merr(loc,msg,((_tmp1BF="type %s : type mismatch on field %s: %s != %s",_tag_dyneither(_tmp1BF,sizeof(char),46))),_tag_dyneither(_tmp1BE,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();
_tmp33=0;}};};}
# 226
if(_tmp3E != 0){
{const char*_tmp1C4;void*_tmp1C3[2];struct Cyc_String_pa_PrintArg_struct _tmp1C2;struct Cyc_String_pa_PrintArg_struct _tmp1C1;(_tmp1C1.tag=0,((_tmp1C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp3E->hd)->name),((_tmp1C2.tag=0,((_tmp1C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1C3[0]=& _tmp1C2,((_tmp1C3[1]=& _tmp1C1,Cyc_Tcdecl_merr(loc,msg,((_tmp1C4="type %s is missing field %s",_tag_dyneither(_tmp1C4,sizeof(char),28))),_tag_dyneither(_tmp1C3,sizeof(void*),2)))))))))))));}
_tmp33=0;}
# 230
if(_tmp42 != 0){
{const char*_tmp1C9;void*_tmp1C8[2];struct Cyc_String_pa_PrintArg_struct _tmp1C7;struct Cyc_String_pa_PrintArg_struct _tmp1C6;(_tmp1C6.tag=0,((_tmp1C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp42->hd)->name),((_tmp1C7.tag=0,((_tmp1C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1C8[0]=& _tmp1C7,((_tmp1C8[1]=& _tmp1C6,Cyc_Tcdecl_merr(loc,msg,((_tmp1C9="type %s has extra field %s",_tag_dyneither(_tmp1C9,sizeof(char),27))),_tag_dyneither(_tmp1C8,sizeof(void*),2)))))))))))));}
_tmp33=0;}
# 235
if(_tmp3F != _tmp43){
{const char*_tmp1CD;void*_tmp1CC[1];struct Cyc_String_pa_PrintArg_struct _tmp1CB;(_tmp1CB.tag=0,((_tmp1CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1CC[0]=& _tmp1CB,Cyc_Tcdecl_merr(loc,msg,((_tmp1CD="%s : previous declaration disagrees with @tagged qualifier",_tag_dyneither(_tmp1CD,sizeof(char),59))),_tag_dyneither(_tmp1CC,sizeof(void*),1)))))));}
_tmp33=0;}
# 240
d2=d0;
goto _LL22;};_LL22:;}
# 244
if(!_tmp33)return 0;
# 246
if(_tmp37 == d2->sc)
return d2;else{
# 249
{struct Cyc_Absyn_Aggrdecl*_tmp1CE;d2=((_tmp1CE=_cycalloc(sizeof(*_tmp1CE)),((_tmp1CE[0]=*d2,_tmp1CE))));}
d2->sc=_tmp37;
return d2;}};};}
# 255
inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){
const char*_tmp1D0;const char*_tmp1CF;return is_x?(_tmp1D0="@extensible datatype",_tag_dyneither(_tmp1D0,sizeof(char),21)):((_tmp1CF="datatype",_tag_dyneither(_tmp1CF,sizeof(char),9)));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 261
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg){
# 265
unsigned int loc=f1->loc;
# 268
if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){
{const char*_tmp1D7;void*_tmp1D6[4];struct Cyc_String_pa_PrintArg_struct _tmp1D5;struct Cyc_String_pa_PrintArg_struct _tmp1D4;struct Cyc_String_pa_PrintArg_struct _tmp1D3;struct Cyc_String_pa_PrintArg_struct _tmp1D2;(_tmp1D2.tag=0,((_tmp1D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2),((_tmp1D3.tag=0,((_tmp1D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2),((_tmp1D4.tag=0,((_tmp1D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1D5.tag=0,((_tmp1D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1D6[0]=& _tmp1D5,((_tmp1D6[1]=& _tmp1D4,((_tmp1D6[2]=& _tmp1D3,((_tmp1D6[3]=& _tmp1D2,Cyc_Tcdecl_merr(loc,msg,((_tmp1D7="%s %s: field name mismatch %s != %s",_tag_dyneither(_tmp1D7,sizeof(char),36))),_tag_dyneither(_tmp1D6,sizeof(void*),4)))))))))))))))))))))))));}
# 271
return 0;}{
# 274
struct _dyneither_ptr _tmp85=*(*f0->name).f2;
# 277
enum Cyc_Absyn_Scope _tmp8B;int _tmp8C;const char*_tmp1DC;void*_tmp1DB[2];struct Cyc_String_pa_PrintArg_struct _tmp1DA;struct Cyc_String_pa_PrintArg_struct _tmp1D9;struct _tuple10 _tmp8A=
Cyc_Tcdecl_merge_scope(f0->sc,f1->sc,(struct _dyneither_ptr)((_tmp1D9.tag=0,((_tmp1D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1DA.tag=0,((_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1DB[0]=& _tmp1DA,((_tmp1DB[1]=& _tmp1D9,Cyc_aprintf(((_tmp1DC="in %s %s, field",_tag_dyneither(_tmp1DC,sizeof(char),16))),_tag_dyneither(_tmp1DB,sizeof(void*),2)))))))))))))),_tmp85,loc,msg,0);_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;{
# 281
struct Cyc_List_List*_tmp8D=f0->typs;
struct Cyc_List_List*_tmp8E=f1->typs;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8E)){
{const char*_tmp1E2;void*_tmp1E1[3];struct Cyc_String_pa_PrintArg_struct _tmp1E0;struct Cyc_String_pa_PrintArg_struct _tmp1DF;struct Cyc_String_pa_PrintArg_struct _tmp1DE;(_tmp1DE.tag=0,((_tmp1DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp85),((_tmp1DF.tag=0,((_tmp1DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1E0.tag=0,((_tmp1E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1E1[0]=& _tmp1E0,((_tmp1E1[1]=& _tmp1DF,((_tmp1E1[2]=& _tmp1DE,Cyc_Tcdecl_merr(loc,msg,((_tmp1E2="%s %s, field %s: parameter number mismatch",_tag_dyneither(_tmp1E2,sizeof(char),43))),_tag_dyneither(_tmp1E1,sizeof(void*),3)))))))))))))))))));}
_tmp8C=0;}
# 288
for(0;_tmp8D != 0;(_tmp8D=_tmp8D->tl,_tmp8E=_tmp8E->tl)){
# 290
if(!Cyc_Tcutil_equal_tqual((*((struct _tuple15*)_tmp8D->hd)).f1,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp8E))->hd)).f1)){
{const char*_tmp1E8;void*_tmp1E7[3];struct Cyc_String_pa_PrintArg_struct _tmp1E6;struct Cyc_String_pa_PrintArg_struct _tmp1E5;struct Cyc_String_pa_PrintArg_struct _tmp1E4;(_tmp1E4.tag=0,((_tmp1E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp85),((_tmp1E5.tag=0,((_tmp1E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1E6.tag=0,((_tmp1E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1E7[0]=& _tmp1E6,((_tmp1E7[1]=& _tmp1E5,((_tmp1E7[2]=& _tmp1E4,Cyc_Tcdecl_merr(loc,msg,((_tmp1E8="%s %s, field %s: parameter qualifier",_tag_dyneither(_tmp1E8,sizeof(char),37))),_tag_dyneither(_tmp1E7,sizeof(void*),3)))))))))))))))))));}
_tmp8C=0;}{
# 294
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple15*)_tmp8E->hd)).f2);
# 296
if(!Cyc_Tcdecl_check_type((*((struct _tuple15*)_tmp8D->hd)).f2,subst_t1)){
{const char*_tmp1F0;void*_tmp1EF[5];struct Cyc_String_pa_PrintArg_struct _tmp1EE;struct Cyc_String_pa_PrintArg_struct _tmp1ED;struct Cyc_String_pa_PrintArg_struct _tmp1EC;struct Cyc_String_pa_PrintArg_struct _tmp1EB;struct Cyc_String_pa_PrintArg_struct _tmp1EA;(_tmp1EA.tag=0,((_tmp1EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1)),((_tmp1EB.tag=0,((_tmp1EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((struct _tuple15*)_tmp8D->hd)).f2)),((_tmp1EC.tag=0,((_tmp1EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp85),((_tmp1ED.tag=0,((_tmp1ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1EE.tag=0,((_tmp1EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1EF[0]=& _tmp1EE,((_tmp1EF[1]=& _tmp1ED,((_tmp1EF[2]=& _tmp1EC,((_tmp1EF[3]=& _tmp1EB,((_tmp1EF[4]=& _tmp1EA,Cyc_Tcdecl_merr(loc,msg,((_tmp1F0="%s %s, field %s: parameter type mismatch %s != %s",_tag_dyneither(_tmp1F0,sizeof(char),50))),_tag_dyneither(_tmp1EF,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();
_tmp8C=0;}};}
# 304
if(!_tmp8C)return 0;
if(f0->sc != _tmp8B){
struct Cyc_Absyn_Datatypefield*_tmp1F1;struct Cyc_Absyn_Datatypefield*_tmpA0=(_tmp1F1=_cycalloc(sizeof(*_tmp1F1)),((_tmp1F1[0]=*f0,_tmp1F1)));
((struct Cyc_Absyn_Datatypefield*)_check_null(_tmpA0))->sc=_tmp8B;
return _tmpA0;}else{
return f0;}};};}
# 312
static struct _tuple15*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple15*x){
# 315
struct Cyc_Absyn_Tqual _tmpA3;void*_tmpA4;struct _tuple15*_tmpA2=x;_tmpA3=_tmpA2->f1;_tmpA4=_tmpA2->f2;{
struct _tuple15*_tmp1F2;return(_tmp1F2=_cycalloc(sizeof(*_tmp1F2)),((_tmp1F2->f1=_tmpA3,((_tmp1F2->f2=Cyc_Tcutil_substitute(inst,_tmpA4),_tmp1F2)))));};}
# 321
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 323
struct Cyc_Absyn_Datatypefield*_tmp1F3;struct Cyc_Absyn_Datatypefield*_tmpA6=(_tmp1F3=_cycalloc(sizeof(*_tmp1F3)),((_tmp1F3[0]=*f1,_tmp1F3)));
_tmpA6->typs=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);
# 326
return _tmpA6;}
# 333
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 342
struct Cyc_List_List**_tmp1F4;struct Cyc_List_List**f2sp=(_tmp1F4=_cycalloc(sizeof(*_tmp1F4)),((_tmp1F4[0]=0,_tmp1F4)));
struct Cyc_List_List**_tmpA8=f2sp;
int cmp=- 1;
# 346
for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){
while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){
struct Cyc_List_List*_tmp1F5;struct Cyc_List_List*_tmpA9=(_tmp1F5=_cycalloc(sizeof(*_tmp1F5)),((_tmp1F5->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd,((_tmp1F5->tl=0,_tmp1F5)))));
*_tmpA8=_tmpA9;
_tmpA8=&((struct Cyc_List_List*)_check_null(_tmpA9))->tl;
f0s=f0s->tl;}
# 353
if(f0s == 0  || cmp > 0){
# 355
*incl=0;{
struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmpAB=(_tmp1F6=_cycalloc(sizeof(*_tmp1F6)),((_tmp1F6->hd=Cyc_Tcdecl_substitute_datatypefield(inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd),((_tmp1F6->tl=0,_tmp1F6)))));
*_tmpA8=_tmpAB;
_tmpA8=&((struct Cyc_List_List*)_check_null(_tmpAB))->tl;};}else{
# 361
struct Cyc_Absyn_Datatypefield*_tmpAD=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);
if(_tmpAD != 0){
if(_tmpAD != (struct Cyc_Absyn_Datatypefield*)f0s->hd)*incl=0;{
struct Cyc_List_List*_tmp1F7;struct Cyc_List_List*_tmpAE=(_tmp1F7=_cycalloc(sizeof(*_tmp1F7)),((_tmp1F7->hd=_tmpAD,((_tmp1F7->tl=0,_tmp1F7)))));
*_tmpA8=_tmpAE;
_tmpA8=&((struct Cyc_List_List*)_check_null(_tmpAE))->tl;};}else{
# 368
*res=0;}
# 370
f0s=f0s->tl;}}
# 375
if(f1s != 0){
*incl=0;
*_tmpA8=f1s;}else{
# 379
*_tmpA8=f0s;}
# 382
return*f2sp;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 386
static struct _tuple16 Cyc_Tcdecl_split(struct Cyc_List_List*f){
if(f == 0){struct _tuple16 _tmp1F8;return(_tmp1F8.f1=0,((_tmp1F8.f2=0,_tmp1F8)));}
if(f->tl == 0){struct _tuple16 _tmp1F9;return(_tmp1F9.f1=f,((_tmp1F9.f2=0,_tmp1F9)));}{
struct Cyc_List_List*_tmpB4;struct Cyc_List_List*_tmpB5;struct _tuple16 _tmpB3=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);_tmpB4=_tmpB3.f1;_tmpB5=_tmpB3.f2;{
struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp1FD;struct _tuple16 _tmp1FC;return(_tmp1FC.f1=((_tmp1FD=_cycalloc(sizeof(*_tmp1FD)),((_tmp1FD->hd=f->hd,((_tmp1FD->tl=_tmpB4,_tmp1FD)))))),((_tmp1FC.f2=((_tmp1FE=_cycalloc(sizeof(*_tmp1FE)),((_tmp1FE->hd=((struct Cyc_List_List*)_check_null(f->tl))->hd,((_tmp1FE->tl=_tmpB5,_tmp1FE)))))),_tmp1FC)));};};}
# 395
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg){
# 398
struct Cyc_List_List*_tmpBA;struct Cyc_List_List*_tmpBB;struct _tuple16 _tmpB9=((struct _tuple16(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);_tmpBA=_tmpB9.f1;_tmpBB=_tmpB9.f2;
if(_tmpBA != 0  && _tmpBA->tl != 0)
_tmpBA=Cyc_Tcdecl_sort_xdatatype_fields(_tmpBA,res,v,loc,msg);
if(_tmpBB != 0  && _tmpBB->tl != 0)
_tmpBB=Cyc_Tcdecl_sort_xdatatype_fields(_tmpBB,res,v,loc,msg);{
const char*_tmp200;int*_tmp1FF;return Cyc_Tcdecl_merge_xdatatype_fields(_tmpBA,_tmpBB,0,0,0,res,((_tmp1FF=_cycalloc_atomic(sizeof(*_tmp1FF)),((_tmp1FF[0]=1,_tmp1FF)))),(
(_tmp200="@extensible datatype",_tag_dyneither(_tmp200,sizeof(char),21))),*v,loc,msg);};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 407
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 409
struct _dyneither_ptr _tmpBE=Cyc_Absynpp_qvar2string(d0->name);
const char*_tmp201;struct _dyneither_ptr t=(_tmp201="datatype",_tag_dyneither(_tmp201,sizeof(char),9));
int _tmpBF=1;
# 416
if(d0->is_extensible != d1->is_extensible){
{const char*_tmp207;void*_tmp206[3];struct Cyc_String_pa_PrintArg_struct _tmp205;struct Cyc_String_pa_PrintArg_struct _tmp204;struct Cyc_String_pa_PrintArg_struct _tmp203;(_tmp203.tag=0,((_tmp203.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcdecl_is_x2string(d1->is_extensible)),((_tmp204.tag=0,((_tmp204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d0->is_extensible)),((_tmp205.tag=0,((_tmp205.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBE),((_tmp206[0]=& _tmp205,((_tmp206[1]=& _tmp204,((_tmp206[2]=& _tmp203,Cyc_Tcdecl_merr(loc,msg,((_tmp207="expected %s to be a %s instead of a %s",_tag_dyneither(_tmp207,sizeof(char),39))),_tag_dyneither(_tmp206,sizeof(void*),3)))))))))))))))))));}
_tmpBF=0;}else{
# 421
t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}
# 425
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpBE,loc,msg))return 0;{
# 428
enum Cyc_Absyn_Scope _tmpC6;int _tmpC7;struct _tuple10 _tmpC5=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpBE,loc,msg,0);_tmpC6=_tmpC5.f1;_tmpC7=_tmpC5.f2;
if(!_tmpC7)_tmpBF=0;{
# 431
struct Cyc_Absyn_Datatypedecl*d2;
# 434
{struct _tuple17 _tmp208;struct _tuple17 _tmpC9=(_tmp208.f1=d0->fields,((_tmp208.f2=d1->fields,_tmp208)));struct Cyc_List_List*_tmpCA;struct Cyc_List_List*_tmpCB;_LL2A: if(_tmpC9.f2 != 0)goto _LL2C;_LL2B:
 d2=d0;goto _LL29;_LL2C: if(_tmpC9.f1 != 0)goto _LL2E;_LL2D:
 d2=d1;goto _LL29;_LL2E: if(_tmpC9.f1 == 0)goto _LL29;_tmpCA=(struct Cyc_List_List*)(_tmpC9.f1)->v;if(_tmpC9.f2 == 0)goto _LL29;_tmpCB=(struct Cyc_List_List*)(_tmpC9.f2)->v;_LL2F: {
# 448 "tcdecl.cyc"
struct Cyc_List_List*_tmpCC=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 451
if(d0->is_extensible){
# 455
int _tmpCD=1;
struct Cyc_List_List*_tmpCE=Cyc_Tcdecl_merge_xdatatype_fields(_tmpCA,_tmpCB,_tmpCC,d0->tvs,d1->tvs,& _tmpBF,& _tmpCD,t,_tmpBE,loc,msg);
# 459
if(_tmpCD)
d2=d0;else{
# 462
{struct Cyc_Absyn_Datatypedecl*_tmp209;d2=((_tmp209=_cycalloc(sizeof(*_tmp209)),((_tmp209[0]=*d0,_tmp209))));}
d2->sc=_tmpC6;{
struct Cyc_Core_Opt*_tmp20A;d2->fields=((_tmp20A=_cycalloc(sizeof(*_tmp20A)),((_tmp20A->v=_tmpCE,_tmp20A))));};}}else{
# 468
for(0;_tmpCA != 0  && _tmpCB != 0;(_tmpCA=_tmpCA->tl,_tmpCB=_tmpCB->tl)){
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)_tmpCA->hd,(struct Cyc_Absyn_Datatypefield*)_tmpCB->hd,_tmpCC,t,_tmpBE,msg);}
# 471
if(_tmpCA != 0){
{const char*_tmp210;void*_tmp20F[3];struct Cyc_String_pa_PrintArg_struct _tmp20E;struct Cyc_String_pa_PrintArg_struct _tmp20D;struct Cyc_String_pa_PrintArg_struct _tmp20C;(_tmp20C.tag=0,((_tmp20C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmpCB))->hd)->name).f2),((_tmp20D.tag=0,((_tmp20D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBE),((_tmp20E.tag=0,((_tmp20E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp20F[0]=& _tmp20E,((_tmp20F[1]=& _tmp20D,((_tmp20F[2]=& _tmp20C,Cyc_Tcdecl_merr(loc,msg,((_tmp210="%s %s has extra field %s",_tag_dyneither(_tmp210,sizeof(char),25))),_tag_dyneither(_tmp20F,sizeof(void*),3)))))))))))))))))));}
_tmpBF=0;}
# 475
if(_tmpCB != 0){
{const char*_tmp216;void*_tmp215[3];struct Cyc_String_pa_PrintArg_struct _tmp214;struct Cyc_String_pa_PrintArg_struct _tmp213;struct Cyc_String_pa_PrintArg_struct _tmp212;(_tmp212.tag=0,((_tmp212.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpCB->hd)->name).f2),((_tmp213.tag=0,((_tmp213.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBE),((_tmp214.tag=0,((_tmp214.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp215[0]=& _tmp214,((_tmp215[1]=& _tmp213,((_tmp215[2]=& _tmp212,Cyc_Tcdecl_merr(loc,msg,((_tmp216="%s %s is missing field %s",_tag_dyneither(_tmp216,sizeof(char),26))),_tag_dyneither(_tmp215,sizeof(void*),3)))))))))))))))))));}
_tmpBF=0;}
# 479
d2=d0;}
# 481
goto _LL29;}_LL29:;}
# 484
if(!_tmpBF)return 0;
# 486
if(_tmpC6 == d2->sc)
return d2;else{
# 489
{struct Cyc_Absyn_Datatypedecl*_tmp217;d2=((_tmp217=_cycalloc(sizeof(*_tmp217)),((_tmp217[0]=*d2,_tmp217))));}
d2->sc=_tmpC6;
return d2;}};};}
# 495
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 497
struct _dyneither_ptr _tmpDD=Cyc_Absynpp_qvar2string(d0->name);
int _tmpDE=1;
# 501
enum Cyc_Absyn_Scope _tmpE1;int _tmpE2;const char*_tmp218;struct _tuple10 _tmpE0=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp218="enum",_tag_dyneither(_tmp218,sizeof(char),5))),_tmpDD,loc,msg,1);_tmpE1=_tmpE0.f1;_tmpE2=_tmpE0.f2;
if(!_tmpE2)_tmpDE=0;{
# 504
struct Cyc_Absyn_Enumdecl*d2;
# 507
{struct _tuple17 _tmp219;struct _tuple17 _tmpE4=(_tmp219.f1=d0->fields,((_tmp219.f2=d1->fields,_tmp219)));struct Cyc_List_List*_tmpE5;struct Cyc_List_List*_tmpE6;_LL31: if(_tmpE4.f2 != 0)goto _LL33;_LL32:
 d2=d0;goto _LL30;_LL33: if(_tmpE4.f1 != 0)goto _LL35;_LL34:
 d2=d1;goto _LL30;_LL35: if(_tmpE4.f1 == 0)goto _LL30;_tmpE5=(struct Cyc_List_List*)(_tmpE4.f1)->v;if(_tmpE4.f2 == 0)goto _LL30;_tmpE6=(struct Cyc_List_List*)(_tmpE4.f2)->v;_LL36:
# 512
 for(0;_tmpE5 != 0  && _tmpE6 != 0;(_tmpE5=_tmpE5->tl,_tmpE6=_tmpE6->tl)){
struct _dyneither_ptr*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;unsigned int _tmpEA;struct Cyc_Absyn_Enumfield*_tmpE7=(struct Cyc_Absyn_Enumfield*)_tmpE5->hd;_tmpE8=(_tmpE7->name)->f2;_tmpE9=_tmpE7->tag;_tmpEA=_tmpE7->loc;{
struct _dyneither_ptr*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;unsigned int _tmpEE;struct Cyc_Absyn_Enumfield*_tmpEB=(struct Cyc_Absyn_Enumfield*)_tmpE6->hd;_tmpEC=(_tmpEB->name)->f2;_tmpED=_tmpEB->tag;_tmpEE=_tmpEB->loc;
# 517
if(Cyc_strptrcmp(_tmpE8,_tmpEC)!= 0){
{const char*_tmp21F;void*_tmp21E[3];struct Cyc_String_pa_PrintArg_struct _tmp21D;struct Cyc_String_pa_PrintArg_struct _tmp21C;struct Cyc_String_pa_PrintArg_struct _tmp21B;(_tmp21B.tag=0,((_tmp21B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpEC),((_tmp21C.tag=0,((_tmp21C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE8),((_tmp21D.tag=0,((_tmp21D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDD),((_tmp21E[0]=& _tmp21D,((_tmp21E[1]=& _tmp21C,((_tmp21E[2]=& _tmp21B,Cyc_Tcdecl_merr(loc,msg,((_tmp21F="enum %s: field name mismatch %s != %s",_tag_dyneither(_tmp21F,sizeof(char),38))),_tag_dyneither(_tmp21E,sizeof(void*),3)))))))))))))))))));}
_tmpDE=0;}
# 522
if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpE9))!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpED))){
{const char*_tmp224;void*_tmp223[2];struct Cyc_String_pa_PrintArg_struct _tmp222;struct Cyc_String_pa_PrintArg_struct _tmp221;(_tmp221.tag=0,((_tmp221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpEC),((_tmp222.tag=0,((_tmp222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDD),((_tmp223[0]=& _tmp222,((_tmp223[1]=& _tmp221,Cyc_Tcdecl_merr(loc,msg,((_tmp224="enum %s, field %s, value mismatch",_tag_dyneither(_tmp224,sizeof(char),34))),_tag_dyneither(_tmp223,sizeof(void*),2)))))))))))));}
_tmpDE=0;}};}
# 527
d2=d0;
goto _LL30;_LL30:;}
# 531
if(!_tmpDE)return 0;
# 533
if(d2->sc == _tmpE1)
return d2;else{
# 536
{struct Cyc_Absyn_Enumdecl*_tmp225;d2=((_tmp225=_cycalloc(sizeof(*_tmp225)),((_tmp225[0]=*d2,_tmp225))));}
d2->sc=_tmpE1;
return d2;}};}
# 542
static struct _tuple10 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 546
int _tmpF9=1;
# 549
enum Cyc_Absyn_Scope _tmpFB;int _tmpFC;struct _tuple10 _tmpFA=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);_tmpFB=_tmpFA.f1;_tmpFC=_tmpFA.f2;
if(!_tmpFC)_tmpF9=0;
# 553
if(!Cyc_Tcdecl_check_type(t0,t1)){
{const char*_tmp22C;void*_tmp22B[4];struct Cyc_String_pa_PrintArg_struct _tmp22A;struct Cyc_String_pa_PrintArg_struct _tmp229;struct Cyc_String_pa_PrintArg_struct _tmp228;struct Cyc_String_pa_PrintArg_struct _tmp227;(_tmp227.tag=0,((_tmp227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t0)),((_tmp228.tag=0,((_tmp228.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp229.tag=0,((_tmp229.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp22A.tag=0,((_tmp22A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp22B[0]=& _tmp22A,((_tmp22B[1]=& _tmp229,((_tmp22B[2]=& _tmp228,((_tmp22B[3]=& _tmp227,Cyc_Tcdecl_merr(loc,msg,((_tmp22C="%s %s has type \n%s\n instead of \n%s\n",_tag_dyneither(_tmp22C,sizeof(char),36))),_tag_dyneither(_tmp22B,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();
_tmpF9=0;}
# 561
if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){
{const char*_tmp231;void*_tmp230[2];struct Cyc_String_pa_PrintArg_struct _tmp22F;struct Cyc_String_pa_PrintArg_struct _tmp22E;(_tmp22E.tag=0,((_tmp22E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp22F.tag=0,((_tmp22F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp230[0]=& _tmp22F,((_tmp230[1]=& _tmp22E,Cyc_Tcdecl_merr(loc,msg,((_tmp231="%s %s has different type qualifiers",_tag_dyneither(_tmp231,sizeof(char),36))),_tag_dyneither(_tmp230,sizeof(void*),2)))))))))))));}
_tmpF9=0;}
# 567
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
{const char*_tmp236;void*_tmp235[2];struct Cyc_String_pa_PrintArg_struct _tmp234;struct Cyc_String_pa_PrintArg_struct _tmp233;(_tmp233.tag=0,((_tmp233.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp234.tag=0,((_tmp234.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp235[0]=& _tmp234,((_tmp235[1]=& _tmp233,Cyc_Tcdecl_merr(loc,msg,((_tmp236="%s %s has different attributes",_tag_dyneither(_tmp236,sizeof(char),31))),_tag_dyneither(_tmp235,sizeof(void*),2)))))))))))));}
{const char*_tmp239;void*_tmp238;(_tmp238=0,Cyc_fprintf(Cyc_stderr,((_tmp239="previous attributes: ",_tag_dyneither(_tmp239,sizeof(char),22))),_tag_dyneither(_tmp238,sizeof(void*),0)));}
for(0;atts0 != 0;atts0=atts0->tl){
const char*_tmp23D;void*_tmp23C[1];struct Cyc_String_pa_PrintArg_struct _tmp23B;(_tmp23B.tag=0,((_tmp23B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts0->hd)),((_tmp23C[0]=& _tmp23B,Cyc_fprintf(Cyc_stderr,((_tmp23D="%s ",_tag_dyneither(_tmp23D,sizeof(char),4))),_tag_dyneither(_tmp23C,sizeof(void*),1)))))));}
{const char*_tmp240;void*_tmp23F;(_tmp23F=0,Cyc_fprintf(Cyc_stderr,((_tmp240="\ncurrent attributes: ",_tag_dyneither(_tmp240,sizeof(char),22))),_tag_dyneither(_tmp23F,sizeof(void*),0)));}
for(0;atts1 != 0;atts1=atts1->tl){
const char*_tmp244;void*_tmp243[1];struct Cyc_String_pa_PrintArg_struct _tmp242;(_tmp242.tag=0,((_tmp242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd)),((_tmp243[0]=& _tmp242,Cyc_fprintf(Cyc_stderr,((_tmp244="%s ",_tag_dyneither(_tmp244,sizeof(char),4))),_tag_dyneither(_tmp243,sizeof(void*),1)))))));}
{const char*_tmp247;void*_tmp246;(_tmp246=0,Cyc_fprintf(Cyc_stderr,((_tmp247="\n",_tag_dyneither(_tmp247,sizeof(char),2))),_tag_dyneither(_tmp246,sizeof(void*),0)));}
_tmpF9=0;}{
# 579
struct _tuple10 _tmp248;return(_tmp248.f1=_tmpFB,((_tmp248.f2=_tmpF9,_tmp248)));};}
# 582
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 584
struct _dyneither_ptr _tmp118=Cyc_Absynpp_qvar2string(d0->name);
# 587
enum Cyc_Absyn_Scope _tmp11B;int _tmp11C;const char*_tmp249;struct _tuple10 _tmp11A=
Cyc_Tcdecl_check_var_or_fn_decl(d0->sc,d0->type,d0->tq,d0->attributes,d1->sc,d1->type,d1->tq,d1->attributes,(
# 590
(_tmp249="variable",_tag_dyneither(_tmp249,sizeof(char),9))),_tmp118,loc,msg);_tmp11B=_tmp11A.f1;_tmp11C=_tmp11A.f2;
if(!_tmp11C)return 0;
# 593
if(d0->sc == _tmp11B)
return d0;else{
# 596
struct Cyc_Absyn_Vardecl*_tmp24A;struct Cyc_Absyn_Vardecl*_tmp11D=(_tmp24A=_cycalloc(sizeof(*_tmp24A)),((_tmp24A[0]=*d0,_tmp24A)));
((struct Cyc_Absyn_Vardecl*)_check_null(_tmp11D))->sc=_tmp11B;
return _tmp11D;}}
# 602
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 604
struct _dyneither_ptr _tmp11F=Cyc_Absynpp_qvar2string(d0->name);
# 607
{const char*_tmp24B;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp24B="typedef",_tag_dyneither(_tmp24B,sizeof(char),8))),_tmp11F,loc,msg))return 0;}{
# 610
struct Cyc_List_List*_tmp121=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 613
if(d0->defn != 0  && d1->defn != 0){
void*subst_defn1=Cyc_Tcutil_substitute(_tmp121,(void*)_check_null(d1->defn));
if(!Cyc_Tcdecl_check_type((void*)_check_null(d0->defn),subst_defn1)){
{const char*_tmp251;void*_tmp250[3];struct Cyc_String_pa_PrintArg_struct _tmp24F;struct Cyc_String_pa_PrintArg_struct _tmp24E;struct Cyc_String_pa_PrintArg_struct _tmp24D;(_tmp24D.tag=0,((_tmp24D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(d0->defn))),((_tmp24E.tag=0,((_tmp24E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1)),((_tmp24F.tag=0,((_tmp24F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11F),((_tmp250[0]=& _tmp24F,((_tmp250[1]=& _tmp24E,((_tmp250[2]=& _tmp24D,Cyc_Tcdecl_merr(loc,msg,((_tmp251="typedef %s does not refer to the same type: %s != %s",_tag_dyneither(_tmp251,sizeof(char),53))),_tag_dyneither(_tmp250,sizeof(void*),3)))))))))))))))))));}
return 0;}}
# 621
return d0;};}
# 624
void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned int loc,struct _dyneither_ptr*msg){
struct _tuple14 _tmp252;struct _tuple14 _tmp128=(_tmp252.f1=b0,((_tmp252.f2=b1,_tmp252)));struct Cyc_Absyn_Vardecl*_tmp12C;struct Cyc_Absyn_Vardecl*_tmp12E;struct Cyc_Absyn_Vardecl*_tmp130;struct Cyc_Absyn_Fndecl*_tmp132;struct Cyc_Absyn_Fndecl*_tmp135;struct Cyc_Absyn_Fndecl*_tmp137;struct Cyc_Absyn_Vardecl*_tmp139;_LL38:{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp129=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp128.f1;if(_tmp129->tag != 0)goto _LL3A;}{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp12A=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp128.f2;if(_tmp12A->tag != 0)goto _LL3A;};_LL39:
 return(void*)& Cyc_Absyn_Unresolved_b_val;_LL3A:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp12B=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp128.f1;if(_tmp12B->tag != 1)goto _LL3C;else{_tmp12C=_tmp12B->f1;}}{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp12D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp128.f2;if(_tmp12D->tag != 1)goto _LL3C;else{_tmp12E=_tmp12D->f1;}};_LL3B: {
# 628
struct Cyc_Absyn_Vardecl*_tmp13A=Cyc_Tcdecl_merge_vardecl(_tmp12C,_tmp12E,loc,msg);
if(_tmp13A == 0)return(void*)& Cyc_Absyn_Unresolved_b_val;
if(_tmp13A == _tmp12C)return b0;
if(_tmp13A == _tmp12E)return b1;{
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp255;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp254;return(void*)((_tmp254=_cycalloc(sizeof(*_tmp254)),((_tmp254[0]=((_tmp255.tag=1,((_tmp255.f1=_tmp13A,_tmp255)))),_tmp254))));};}_LL3C:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp12F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp128.f1;if(_tmp12F->tag != 1)goto _LL3E;else{_tmp130=_tmp12F->f1;}}{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp131=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp128.f2;if(_tmp131->tag != 2)goto _LL3E;else{_tmp132=_tmp131->f1;}};_LL3D: {
# 634
int _tmp13F;const char*_tmp256;struct _tuple10 _tmp13E=Cyc_Tcdecl_check_var_or_fn_decl(_tmp130->sc,_tmp130->type,_tmp130->tq,_tmp130->attributes,_tmp132->sc,(void*)_check_null(_tmp132->cached_typ),
Cyc_Absyn_empty_tqual(0),_tmp132->attributes,(
(_tmp256="function",_tag_dyneither(_tmp256,sizeof(char),9))),
Cyc_Absynpp_qvar2string(_tmp130->name),loc,msg);_tmp13F=_tmp13E.f2;
return _tmp13F?b1:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL3E:{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp133=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp128.f1;if(_tmp133->tag != 2)goto _LL40;}{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp134=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp128.f2;if(_tmp134->tag != 2)goto _LL40;else{_tmp135=_tmp134->f1;}};_LL3F:
# 640
{const char*_tmp25A;void*_tmp259[1];struct Cyc_String_pa_PrintArg_struct _tmp258;(_tmp258.tag=0,((_tmp258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp135->name)),((_tmp259[0]=& _tmp258,Cyc_Tcdecl_merr(loc,msg,((_tmp25A="redefinition of function %s",_tag_dyneither(_tmp25A,sizeof(char),28))),_tag_dyneither(_tmp259,sizeof(void*),1)))))));}
return(void*)& Cyc_Absyn_Unresolved_b_val;_LL40:{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp136=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp128.f1;if(_tmp136->tag != 2)goto _LL42;else{_tmp137=_tmp136->f1;}}{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp138=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp128.f2;if(_tmp138->tag != 1)goto _LL42;else{_tmp139=_tmp138->f1;}};_LL41: {
# 643
int _tmp145;const char*_tmp25B;struct _tuple10 _tmp144=Cyc_Tcdecl_check_var_or_fn_decl(_tmp137->sc,(void*)_check_null(_tmp137->cached_typ),Cyc_Absyn_empty_tqual(0),_tmp137->attributes,_tmp139->sc,_tmp139->type,_tmp139->tq,_tmp139->attributes,(
# 645
(_tmp25B="variable",_tag_dyneither(_tmp25B,sizeof(char),9))),
Cyc_Absynpp_qvar2string(_tmp137->name),loc,msg);_tmp145=_tmp144.f2;
return _tmp145?b0:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL42:;_LL43: {
struct Cyc_Core_Invalid_argument_exn_struct _tmp261;const char*_tmp260;struct Cyc_Core_Invalid_argument_exn_struct*_tmp25F;(int)_throw((void*)((_tmp25F=_cycalloc(sizeof(*_tmp25F)),((_tmp25F[0]=((_tmp261.tag=Cyc_Core_Invalid_argument,((_tmp261.f1=((_tmp260="Tcdecl::merge_binding",_tag_dyneither(_tmp260,sizeof(char),22))),_tmp261)))),_tmp25F)))));}_LL37:;}
# 652
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 655
struct Cyc_Absyn_Datatypedecl*_tmp14A;struct Cyc_Absyn_Datatypefield*_tmp14B;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp149=d0;_tmp14A=_tmp149->base;_tmp14B=_tmp149->field;{
struct Cyc_Absyn_Datatypedecl*_tmp14D;struct Cyc_Absyn_Datatypefield*_tmp14E;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp14C=d1;_tmp14D=_tmp14C->base;_tmp14E=_tmp14C->field;{
struct _dyneither_ptr _tmp14F=Cyc_Absynpp_qvar2string(_tmp14B->name);
# 660
if(Cyc_Absyn_qvar_cmp(_tmp14A->name,_tmp14D->name)!= 0){
{const char*_tmp267;void*_tmp266[3];struct Cyc_String_pa_PrintArg_struct _tmp265;struct Cyc_String_pa_PrintArg_struct _tmp264;struct Cyc_String_pa_PrintArg_struct _tmp263;(_tmp263.tag=0,((_tmp263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp14F),((_tmp264.tag=0,((_tmp264.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp14D->name)),((_tmp265.tag=0,((_tmp265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp14A->name)),((_tmp266[0]=& _tmp265,((_tmp266[1]=& _tmp264,((_tmp266[2]=& _tmp263,Cyc_Tcdecl_merr(loc,msg,((_tmp267="@extensible datatype %s and %s have both a field named %s",_tag_dyneither(_tmp267,sizeof(char),58))),_tag_dyneither(_tmp266,sizeof(void*),3)))))))))))))))))));}
return 0;}
# 667
{const char*_tmp268;if(!Cyc_Tcdecl_check_tvs(_tmp14A->tvs,_tmp14D->tvs,((_tmp268="@extensible datatype",_tag_dyneither(_tmp268,sizeof(char),21))),Cyc_Absynpp_qvar2string(_tmp14A->name),loc,msg))
return 0;}{
# 670
struct Cyc_List_List*_tmp156=Cyc_Tcdecl_build_tvs_map(_tmp14A->tvs,_tmp14D->tvs);
# 673
const char*_tmp269;struct Cyc_Absyn_Datatypefield*_tmp157=Cyc_Tcdecl_merge_datatypefield(_tmp14B,_tmp14E,_tmp156,((_tmp269="@extensible datatype field",_tag_dyneither(_tmp269,sizeof(char),27))),_tmp14F,msg);
if(_tmp157 == 0)return 0;
if(_tmp157 == _tmp14B)return d0;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp26A;return(_tmp26A=_cycalloc(sizeof(*_tmp26A)),((_tmp26A->base=_tmp14A,((_tmp26A->field=_tmp157,_tmp26A)))));};};};};}
