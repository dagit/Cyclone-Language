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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 163
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 364
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 97
int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
void*Cyc_Set_choose(struct Cyc_Set_Set*s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 322
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 433 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 720 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 933
void*Cyc_Absyn_compress_kb(void*);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 940
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 943
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 945
extern void*Cyc_Absyn_sint_typ;
# 947
void*Cyc_Absyn_float_typ(int);
# 949
extern void*Cyc_Absyn_empty_effect;
# 973
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1093
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1102
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
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
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
# 123 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 196
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 74
int Cyc_Tcutil_coerceable(void*);
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 122
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 153
int Cyc_Tcutil_unify(void*,void*);
# 158
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple11{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 172
struct _tuple12*Cyc_Tcutil_r_make_inst_var(struct _tuple11*,struct Cyc_Absyn_Tvar*);
# 220 "tcutil.h"
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 235
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 263
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 273
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 293
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 312
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 341
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct _tuple13{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple13*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 38 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 42
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 52 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;struct _tuple1*_tmp2;struct _tuple1*_tmp4;struct Cyc_List_List*_tmp5;int _tmp6;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp9;int _tmpA;enum Cyc_Absyn_AggrKind _tmpC;struct _tuple1*_tmpD;struct Cyc_List_List*_tmpE;struct Cyc_List_List**_tmpF;struct Cyc_List_List*_tmp10;int _tmp11;struct Cyc_Absyn_Exp*_tmp13;_LL1: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp1->tag != 14)goto _LL3;else{_tmp2=_tmp1->f1;}}_LL2:
# 55
{struct _handler_cons _tmp14;_push_handler(& _tmp14);{int _tmp16=0;if(setjmp(_tmp14.handler))_tmp16=1;if(!_tmp16){
{struct _RegionHandle _tmp17=_new_region("r");struct _RegionHandle*r=& _tmp17;_push_region(r);
{void*_tmp18=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp2,0);struct Cyc_Absyn_Datatypedecl*_tmp1B;struct Cyc_Absyn_Datatypefield*_tmp1C;struct Cyc_Absyn_Enumdecl*_tmp1E;struct Cyc_Absyn_Enumfield*_tmp1F;void*_tmp21;struct Cyc_Absyn_Enumfield*_tmp22;_LLE: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp19=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp19->tag != 1)goto _LL10;}_LLF:
# 59
 p->r=(void*)& Cyc_Absyn_Wild_p_val;
{const char*_tmp35C;void*_tmp35B;(_tmp35B=0,Cyc_Tcutil_terr(p->loc,((_tmp35C="struct tag used without arguments in pattern",_tag_dyneither(_tmp35C,sizeof(char),45))),_tag_dyneither(_tmp35B,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1A=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1A->tag != 2)goto _LL12;else{_tmp1B=_tmp1A->f1;_tmp1C=_tmp1A->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp35F;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp35E;p->r=(void*)((_tmp35E=_cycalloc(sizeof(*_tmp35E)),((_tmp35E[0]=((_tmp35F.tag=7,((_tmp35F.f1=_tmp1B,((_tmp35F.f2=_tmp1C,((_tmp35F.f3=0,((_tmp35F.f4=0,_tmp35F)))))))))),_tmp35E))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1D=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1D->tag != 3)goto _LL14;else{_tmp1E=_tmp1D->f1;_tmp1F=_tmp1D->f2;}}_LL13:
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp362;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp361;p->r=(void*)((_tmp361=_cycalloc(sizeof(*_tmp361)),((_tmp361[0]=((_tmp362.tag=12,((_tmp362.f1=_tmp1E,((_tmp362.f2=_tmp1F,_tmp362)))))),_tmp361))));}_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp20=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp20->tag != 4)goto _LL16;else{_tmp21=(void*)_tmp20->f1;_tmp22=_tmp20->f2;}}_LL15:
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp365;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp364;p->r=(void*)((_tmp364=_cycalloc(sizeof(*_tmp364)),((_tmp364[0]=((_tmp365.tag=13,((_tmp365.f1=_tmp21,((_tmp365.f2=_tmp22,_tmp365)))))),_tmp364))));}_npop_handler(1);return;_LL16: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp23=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp23->tag != 0)goto _LLD;}_LL17:
 goto _LLD;_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp15=(void*)_exn_thrown;void*_tmp2D=_tmp15;void*_tmp2F;_LL19: {struct Cyc_Dict_Absent_exn_struct*_tmp2E=(struct Cyc_Dict_Absent_exn_struct*)_tmp2D;if(_tmp2E->tag != Cyc_Dict_Absent)goto _LL1B;}_LL1A:
# 69
 goto _LL18;_LL1B: _tmp2F=_tmp2D;_LL1C:(void)_throw(_tmp2F);_LL18:;}};}{
# 71
union Cyc_Absyn_Nmspace _tmp30=(*_tmp2).f1;_LL1E: if((_tmp30.Loc_n).tag != 4)goto _LL20;_LL1F:
 goto _LL21;_LL20: if((_tmp30.Rel_n).tag != 1)goto _LL22;if((struct Cyc_List_List*)(_tmp30.Rel_n).val != 0)goto _LL22;_LL21:
# 74
(*_tmp2).f1=Cyc_Absyn_Loc_n;
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp368;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp367;p->r=(void*)((_tmp367=_cycalloc(sizeof(*_tmp367)),((_tmp367[0]=((_tmp368.tag=1,((_tmp368.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp368.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp368)))))),_tmp367))));}
return;_LL22:;_LL23:
{const char*_tmp36B;void*_tmp36A;(_tmp36A=0,Cyc_Tcutil_terr(p->loc,((_tmp36B="qualified variable in pattern",_tag_dyneither(_tmp36B,sizeof(char),30))),_tag_dyneither(_tmp36A,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=_tmp3->f3;}}_LL4:
# 81
{struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))_tmp37=1;if(!_tmp37){
{struct _RegionHandle _tmp38=_new_region("r");struct _RegionHandle*r=& _tmp38;_push_region(r);
{void*_tmp39=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp4,0);struct Cyc_Absyn_Aggrdecl*_tmp3B;struct Cyc_Absyn_Datatypedecl*_tmp3D;struct Cyc_Absyn_Datatypefield*_tmp3E;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3A=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3A->tag != 1)goto _LL27;else{_tmp3B=_tmp3A->f1;}}_LL26: {
# 85
struct Cyc_List_List*_tmp42=0;
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
struct _tuple15*_tmp36E;struct Cyc_List_List*_tmp36D;_tmp42=((_tmp36D=_cycalloc(sizeof(*_tmp36D)),((_tmp36D->hd=((_tmp36E=_cycalloc(sizeof(*_tmp36E)),((_tmp36E->f1=0,((_tmp36E->f2=(struct Cyc_Absyn_Pat*)_tmp5->hd,_tmp36E)))))),((_tmp36D->tl=_tmp42,_tmp36D))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp378;struct Cyc_Absyn_Aggrdecl**_tmp377;struct Cyc_Absyn_AggrInfo*_tmp376;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp375;p->r=(void*)((_tmp375=_cycalloc(sizeof(*_tmp375)),((_tmp375[0]=((_tmp378.tag=6,((_tmp378.f1=((_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376->aggr_info=Cyc_Absyn_KnownAggr(((_tmp377=_cycalloc(sizeof(*_tmp377)),((_tmp377[0]=_tmp3B,_tmp377))))),((_tmp376->targs=0,_tmp376)))))),((_tmp378.f2=0,((_tmp378.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp42),((_tmp378.f4=_tmp6,_tmp378)))))))))),_tmp375))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3C->tag != 2)goto _LL29;else{_tmp3D=_tmp3C->f1;_tmp3E=_tmp3C->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp37B;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp37A;p->r=(void*)((_tmp37A=_cycalloc(sizeof(*_tmp37A)),((_tmp37A[0]=((_tmp37B.tag=7,((_tmp37B.f1=_tmp3D,((_tmp37B.f2=_tmp3E,((_tmp37B.f3=_tmp5,((_tmp37B.f4=_tmp6,_tmp37B)))))))))),_tmp37A))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp3F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3F->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp40->tag != 4)goto _LL2D;}_LL2C:
# 94
{const char*_tmp37E;void*_tmp37D;(_tmp37D=0,Cyc_Tcutil_terr(p->loc,((_tmp37E="enum tag used with arguments in pattern",_tag_dyneither(_tmp37E,sizeof(char),40))),_tag_dyneither(_tmp37D,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp41=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp41->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 83
;_pop_region(r);}
# 82
;_pop_handler();}else{void*_tmp36=(void*)_exn_thrown;void*_tmp4E=_tmp36;void*_tmp50;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp4F=(struct Cyc_Dict_Absent_exn_struct*)_tmp4E;if(_tmp4F->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 99
 goto _LL2F;_LL32: _tmp50=_tmp4E;_LL33:(void)_throw(_tmp50);_LL2F:;}};}
{const char*_tmp382;void*_tmp381[1];struct Cyc_String_pa_PrintArg_struct _tmp380;(_tmp380.tag=0,((_tmp380.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp381[0]=& _tmp380,Cyc_Tcutil_terr(p->loc,((_tmp382="%s is not a constructor in pattern",_tag_dyneither(_tmp382,sizeof(char),35))),_tag_dyneither(_tmp381,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp7->tag != 6)goto _LL7;else{if(_tmp7->f1 != 0)goto _LL7;_tmp8=_tmp7->f2;_tmp9=_tmp7->f3;_tmpA=_tmp7->f4;}}_LL6:
# 105
 if(topt == 0){
const char*_tmp385;void*_tmp384;(_tmp384=0,Cyc_Tcutil_terr(p->loc,((_tmp385="cannot determine pattern type",_tag_dyneither(_tmp385,sizeof(char),30))),_tag_dyneither(_tmp384,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp56=t;struct Cyc_Absyn_AggrInfo _tmp58;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp57=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56;if(_tmp57->tag != 11)goto _LL37;else{_tmp58=_tmp57->f1;}}_LL36:
# 110
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp38B;struct Cyc_Absyn_AggrInfo*_tmp38A;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp389;p->r=(void*)((_tmp389=_cycalloc(sizeof(*_tmp389)),((_tmp389[0]=((_tmp38B.tag=6,((_tmp38B.f1=((_tmp38A=_cycalloc(sizeof(*_tmp38A)),((_tmp38A[0]=_tmp58,_tmp38A)))),((_tmp38B.f2=_tmp8,((_tmp38B.f3=_tmp9,((_tmp38B.f4=_tmpA,_tmp38B)))))))))),_tmp389))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 114
{const char*_tmp38F;void*_tmp38E[1];struct Cyc_String_pa_PrintArg_struct _tmp38D;(_tmp38D.tag=0,((_tmp38D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp38E[0]=& _tmp38D,Cyc_Tcutil_terr(p->loc,((_tmp38F="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp38F,sizeof(char),45))),_tag_dyneither(_tmp38E,sizeof(void*),1)))))));}
# 117
goto _LL34;_LL34:;}
# 119
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmpB->tag != 6)goto _LL9;else{if(_tmpB->f1 == 0)goto _LL9;if((((_tmpB->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpC=((struct _tuple3)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f1;_tmpD=((struct _tuple3)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f2;_tmpE=(_tmpB->f1)->targs;_tmpF=(struct Cyc_List_List**)& _tmpB->f2;_tmp10=_tmpB->f3;_tmp11=_tmpB->f4;}}_LL8:
# 122
{struct _handler_cons _tmp5F;_push_handler(& _tmp5F);{int _tmp61=0;if(setjmp(_tmp5F.handler))_tmp61=1;if(!_tmp61){
{struct Cyc_Absyn_Aggrdecl**_tmp62=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpD);
struct Cyc_Absyn_Aggrdecl*_tmp63=*_tmp62;
if(_tmp63->impl == 0){
{const char*_tmp397;void*_tmp396[1];const char*_tmp395;const char*_tmp394;struct Cyc_String_pa_PrintArg_struct _tmp393;(_tmp393.tag=0,((_tmp393.f1=(struct _dyneither_ptr)(
_tmp63->kind == Cyc_Absyn_StructA?(_tmp394="struct",_tag_dyneither(_tmp394,sizeof(char),7)):((_tmp395="union",_tag_dyneither(_tmp395,sizeof(char),6)))),((_tmp396[0]=& _tmp393,Cyc_Tcutil_terr(p->loc,((_tmp397="can't destructure an abstract %s",_tag_dyneither(_tmp397,sizeof(char),33))),_tag_dyneither(_tmp396,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 131
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmpF);
if(more_exists < 0){
{const char*_tmp39A;void*_tmp399;(_tmp399=0,Cyc_Tcutil_terr(p->loc,((_tmp39A="too many existentially bound type variables in pattern",_tag_dyneither(_tmp39A,sizeof(char),55))),_tag_dyneither(_tmp399,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6B=_tmpF;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6B=&((struct Cyc_List_List*)_check_null(*_tmp6B))->tl;}}
*_tmp6B=0;};}else{
if(more_exists > 0){
# 140
struct Cyc_List_List*_tmp6C=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3A0;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp39F;struct Cyc_List_List*_tmp39E;_tmp6C=((_tmp39E=_cycalloc(sizeof(*_tmp39E)),((_tmp39E->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3A0=_cycalloc(sizeof(*_tmp3A0)),((_tmp3A0[0]=((_tmp39F.tag=1,((_tmp39F.f1=0,_tmp39F)))),_tmp3A0))))),((_tmp39E->tl=_tmp6C,_tmp39E))))));}
*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpF,_tmp6C);}}{
# 145
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3A6;struct Cyc_Absyn_AggrInfo*_tmp3A5;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3A4;p->r=(void*)((_tmp3A4=_cycalloc(sizeof(*_tmp3A4)),((_tmp3A4[0]=((_tmp3A6.tag=6,((_tmp3A6.f1=((_tmp3A5=_cycalloc(sizeof(*_tmp3A5)),((_tmp3A5->aggr_info=Cyc_Absyn_KnownAggr(_tmp62),((_tmp3A5->targs=_tmpE,_tmp3A5)))))),((_tmp3A6.f2=*_tmpF,((_tmp3A6.f3=_tmp10,((_tmp3A6.f4=_tmp11,_tmp3A6)))))))))),_tmp3A4))));};};}
# 123
;_pop_handler();}else{void*_tmp60=(void*)_exn_thrown;void*_tmp74=_tmp60;void*_tmp76;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp75=(struct Cyc_Dict_Absent_exn_struct*)_tmp74;if(_tmp75->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 148
{const char*_tmp3A9;void*_tmp3A8;(_tmp3A8=0,Cyc_Tcutil_terr(p->loc,((_tmp3A9="Non-aggregate name has designator patterns",_tag_dyneither(_tmp3A9,sizeof(char),43))),_tag_dyneither(_tmp3A8,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp76=_tmp74;_LL3D:(void)_throw(_tmp76);_LL39:;}};}
# 152
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp12=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp12->tag != 16)goto _LLB;else{_tmp13=_tmp12->f1;}}_LLA:
# 154
 Cyc_Tcexp_tcExp(te,0,_tmp13);
if(!Cyc_Tcutil_is_const_exp(_tmp13)){
{const char*_tmp3AC;void*_tmp3AB;(_tmp3AB=0,Cyc_Tcutil_terr(p->loc,((_tmp3AC="non-constant expression in case pattern",_tag_dyneither(_tmp3AC,sizeof(char),40))),_tag_dyneither(_tmp3AB,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 159
unsigned int _tmp7C;int _tmp7D;struct _tuple14 _tmp7B=Cyc_Evexp_eval_const_uint_exp(_tmp13);_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp3AF;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp3AE;p->r=(void*)((_tmp3AE=_cycalloc_atomic(sizeof(*_tmp3AE)),((_tmp3AE[0]=((_tmp3AF.tag=9,((_tmp3AF.f1=Cyc_Absyn_None,((_tmp3AF.f2=(int)_tmp7C,_tmp3AF)))))),_tmp3AE))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 166
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 169
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp3B0;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp3B0=_cycalloc(sizeof(*_tmp3B0)),((_tmp3B0->v=s,_tmp3B0)))));};}
# 174
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 178
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 181
{void*_tmp81=Cyc_Tcutil_compress(numt);_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp82=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp81;if(_tmp82->tag != 13)goto _LL41;}_LL40:
 if(topt != 0)return*topt;goto _LL3E;_LL41:;_LL42:
 goto _LL3E;_LL3E:;}
# 185
return numt;}
# 188
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**v_result_ptr,void*t){
vd->type=t;
vd->tq=Cyc_Absyn_empty_tqual(0);{
struct Cyc_List_List*_tmp3B1;*v_result_ptr=((_tmp3B1=_cycalloc(sizeof(*_tmp3B1)),((_tmp3B1->hd=vd,((_tmp3B1->tl=*v_result_ptr,_tmp3B1))))));};}
# 193
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 195
struct _tuple13*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_Tcpat_TcPatResult _tmp84=res1;_tmp85=_tmp84.tvars_and_bounds_opt;_tmp86=_tmp84.patvars;{
struct _tuple13*_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_Tcpat_TcPatResult _tmp87=res2;_tmp88=_tmp87.tvars_and_bounds_opt;_tmp89=_tmp87.patvars;
if(_tmp85 != 0  || _tmp88 != 0){
if(_tmp85 == 0){
struct _tuple13*_tmp3B2;_tmp85=((_tmp3B2=_cycalloc(sizeof(*_tmp3B2)),((_tmp3B2->f1=0,((_tmp3B2->f2=0,_tmp3B2))))));}
if(_tmp88 == 0){
struct _tuple13*_tmp3B3;_tmp88=((_tmp3B3=_cycalloc(sizeof(*_tmp3B3)),((_tmp3B3->f1=0,((_tmp3B3->f2=0,_tmp3B3))))));}{
struct _tuple13*_tmp3B6;struct Cyc_Tcpat_TcPatResult _tmp3B5;return(_tmp3B5.tvars_and_bounds_opt=((_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp85).f1,(*_tmp88).f1),((_tmp3B6->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp85).f2,(*_tmp88).f2),_tmp3B6)))))),((_tmp3B5.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp86,_tmp89),_tmp3B5)));};}{
# 206
struct Cyc_Tcpat_TcPatResult _tmp3B7;return(_tmp3B7.tvars_and_bounds_opt=0,((_tmp3B7.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp86,_tmp89),_tmp3B7)));};};}
# 209
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp3B8;return(_tmp3B8=_cycalloc(sizeof(*_tmp3B8)),((_tmp3B8->loc=loc,((_tmp3B8->topt=0,((_tmp3B8->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp3B8)))))));}
# 213
static void*Cyc_Tcpat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
void*_tmp90=Cyc_Tcutil_compress(t);void*_tmp92;struct Cyc_Absyn_Tqual _tmp93;struct Cyc_Absyn_Exp*_tmp94;union Cyc_Absyn_Constraint*_tmp95;unsigned int _tmp96;_LL44: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp91=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp90;if(_tmp91->tag != 8)goto _LL46;else{_tmp92=(_tmp91->f1).elt_type;_tmp93=(_tmp91->f1).tq;_tmp94=(_tmp91->f1).num_elts;_tmp95=(_tmp91->f1).zero_term;_tmp96=(_tmp91->f1).zt_loc;}}_LL45: {
# 216
struct Cyc_Core_Opt*_tmp3B9;void*rgn=rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3B9))))): rgn_opt;
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp3BC;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3BB;return Cyc_Absyn_atb_typ(_tmp92,rgn,_tmp93,(void*)((_tmp3BB=_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB[0]=((_tmp3BC.tag=1,((_tmp3BC.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp94),_tmp3BC)))),_tmp3BB)))),_tmp95);}_LL46:;_LL47:
 return t;_LL43:;}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 222
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat){
# 224
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp3BD;struct Cyc_Tcpat_TcPatResult res=(_tmp3BD.tvars_and_bounds_opt=0,((_tmp3BD.patvars=0,_tmp3BD)));
# 229
{void*_tmp9A=p->r;struct Cyc_Absyn_Vardecl*_tmp9D;struct Cyc_Absyn_Pat*_tmp9E;struct Cyc_Absyn_Vardecl*_tmpA0;struct Cyc_Absyn_Pat*_tmpA1;struct Cyc_Absyn_Tvar*_tmpA3;struct Cyc_Absyn_Vardecl*_tmpA4;int _tmpAA;struct Cyc_Absyn_Enumdecl*_tmpAC;void*_tmpAE;struct Cyc_Absyn_Pat*_tmpB1;struct Cyc_List_List**_tmpB3;int _tmpB4;struct Cyc_Absyn_Aggrdecl*_tmpB6;struct Cyc_List_List**_tmpB7;struct Cyc_List_List*_tmpB8;struct Cyc_List_List**_tmpB9;int _tmpBA;struct Cyc_Absyn_Datatypedecl*_tmpBC;struct Cyc_Absyn_Datatypefield*_tmpBD;struct Cyc_List_List**_tmpBE;int _tmpBF;_LL49: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp9B=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmp9B->tag != 0)goto _LL4B;}_LL4A:
# 232
 if(topt != 0)
t=*topt;else{
# 235
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL48;_LL4B: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9C=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmp9C->tag != 1)goto _LL4D;else{_tmp9D=_tmp9C->f1;_tmp9E=_tmp9C->f2;}}_LL4C:
# 238
 res=Cyc_Tcpat_tcPatRec(te,_tmp9E,topt,rgn_pat,allow_ref_pat);
t=(void*)_check_null(_tmp9E->topt);
# 242
{void*_tmpC5=Cyc_Tcutil_compress(t);_LL74: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC5;if(_tmpC6->tag != 8)goto _LL76;}_LL75:
# 244
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp3C0;void*_tmp3BF;(_tmp3BF=0,Cyc_Tcutil_terr(p->loc,((_tmp3C0="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp3C0,sizeof(char),58))),_tag_dyneither(_tmp3BF,sizeof(void*),0)));}
goto _LL73;_LL76:;_LL77:
# 248
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp3C3;void*_tmp3C2;(_tmp3C2=0,Cyc_Tcutil_terr(p->loc,((_tmp3C3="pattern would point to an abstract member",_tag_dyneither(_tmp3C3,sizeof(char),42))),_tag_dyneither(_tmp3C2,sizeof(void*),0)));}
goto _LL73;_LL73:;}
# 252
Cyc_Tcpat_set_vd(_tmp9D,& res.patvars,Cyc_Tcpat_promote_array(te,t,rgn_pat));
goto _LL48;_LL4D: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp9F=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmp9F->tag != 2)goto _LL4F;else{_tmpA0=_tmp9F->f1;_tmpA1=_tmp9F->f2;}}_LL4E:
# 255
 res=Cyc_Tcpat_tcPatRec(te,_tmpA1,topt,rgn_pat,allow_ref_pat);
t=(void*)_check_null(_tmpA1->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp3C6;void*_tmp3C5;(_tmp3C5=0,Cyc_Tcutil_terr(p->loc,((_tmp3C6="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp3C6,sizeof(char),55))),_tag_dyneither(_tmp3C5,sizeof(void*),0)));}
goto _LL48;}else{
# 261
if(Cyc_Tcutil_is_noalias_pointer(t)){
const char*_tmp3C9;void*_tmp3C8;(_tmp3C8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3C9="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp3C9,sizeof(char),56))),_tag_dyneither(_tmp3C8,sizeof(void*),0)));}}
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3D3;struct Cyc_Absyn_PtrAtts _tmp3D2;struct Cyc_Absyn_PtrInfo _tmp3D1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D0;Cyc_Tcpat_set_vd(_tmpA0,& res.patvars,(void*)(
(_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0[0]=((_tmp3D3.tag=5,((_tmp3D3.f1=((_tmp3D1.elt_typ=t,((_tmp3D1.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3D1.ptr_atts=(
(_tmp3D2.rgn=rgn_pat,((_tmp3D2.nullable=Cyc_Absyn_false_conref,((_tmp3D2.bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp3D2.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp3D2.ptrloc=0,_tmp3D2)))))))))),_tmp3D1)))))),_tmp3D3)))),_tmp3D0)))));}
# 268
goto _LL48;_LL4F: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA2=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA2->tag != 3)goto _LL51;else{_tmpA3=_tmpA2->f1;_tmpA4=_tmpA2->f2;}}_LL50:
# 273
 Cyc_Tcpat_set_vd(_tmpA4,& res.patvars,_tmpA4->type);{
# 277
struct _RegionHandle _tmpD3=_new_region("r2");struct _RegionHandle*r2=& _tmpD3;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp3D4[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp3D4[0]=_tmpA3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3D4,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 280
if(res.tvars_and_bounds_opt == 0){
struct _tuple13*_tmp3D5;res.tvars_and_bounds_opt=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5->f1=0,((_tmp3D5->f2=0,_tmp3D5))))));}
{struct Cyc_List_List*_tmp3D6;(*res.tvars_and_bounds_opt).f1=(
(_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6->hd=_tmpA3,((_tmp3D6->tl=(*res.tvars_and_bounds_opt).f1,_tmp3D6))))));}
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL48;
# 277
;_pop_region(r2);};_LL51: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA5=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA5->tag != 9)goto _LL53;else{if(_tmpA5->f1 != Cyc_Absyn_Unsigned)goto _LL53;}}_LL52:
# 287
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL48;_LL53: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA6=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA6->tag != 9)goto _LL55;else{if(_tmpA6->f1 != Cyc_Absyn_None)goto _LL55;}}_LL54:
 goto _LL56;_LL55: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA7=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA7->tag != 9)goto _LL57;else{if(_tmpA7->f1 != Cyc_Absyn_Signed)goto _LL57;}}_LL56:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL48;_LL57: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpA8=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA8->tag != 10)goto _LL59;}_LL58:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL48;_LL59: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpA9=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA9->tag != 11)goto _LL5B;else{_tmpAA=_tmpA9->f2;}}_LL5A:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmpAA));goto _LL48;_LL5B: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpAB=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpAB->tag != 12)goto _LL5D;else{_tmpAC=_tmpAB->f1;}}_LL5C:
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp3D9;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3D8;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8[0]=((_tmp3D9.tag=13,((_tmp3D9.f1=_tmpAC->name,((_tmp3D9.f2=_tmpAC,_tmp3D9)))))),_tmp3D8)))));}goto _LL48;_LL5D: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpAD=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpAD->tag != 13)goto _LL5F;else{_tmpAE=(void*)_tmpAD->f1;}}_LL5E:
 t=Cyc_Tcpat_num_type(topt,_tmpAE);goto _LL48;_LL5F: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpAF=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpAF->tag != 8)goto _LL61;}_LL60:
# 295
 if(topt != 0){
void*_tmpD9=Cyc_Tcutil_compress(*topt);_LL79: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD9;if(_tmpDA->tag != 5)goto _LL7B;}_LL7A:
 t=*topt;goto tcpat_end;_LL7B:;_LL7C:
 goto _LL78;_LL78:;}{
# 300
struct Cyc_Core_Opt*_tmpDB=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3E3;struct Cyc_Absyn_PtrAtts _tmp3E2;struct Cyc_Absyn_PtrInfo _tmp3E1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E0;t=(void*)((_tmp3E0=_cycalloc(sizeof(*_tmp3E0)),((_tmp3E0[0]=((_tmp3E3.tag=5,((_tmp3E3.f1=((_tmp3E1.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpDB),((_tmp3E1.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp3E1.ptr_atts=(
# 305
(_tmp3E2.rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpDB),((_tmp3E2.nullable=Cyc_Absyn_true_conref,((_tmp3E2.bounds=
# 307
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp3E2.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp3E2.ptrloc=0,_tmp3E2)))))))))),_tmp3E1)))))),_tmp3E3)))),_tmp3E0))));}
# 310
goto _LL48;};_LL61: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpB0=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpB0->tag != 5)goto _LL63;else{_tmpB1=_tmpB0->f1;}}_LL62: {
# 316
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpE0=0;
int elt_const=0;
if(topt != 0){
void*_tmpE1=Cyc_Tcutil_compress(*topt);void*_tmpE3;struct Cyc_Absyn_Tqual _tmpE4;_LL7E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE1;if(_tmpE2->tag != 5)goto _LL80;else{_tmpE3=(_tmpE2->f1).elt_typ;_tmpE4=(_tmpE2->f1).elt_tq;}}_LL7F:
# 322
 inner_typ=_tmpE3;
_tmpE0=& inner_typ;
elt_const=_tmpE4.real_const;
goto _LL7D;_LL80:;_LL81:
 goto _LL7D;_LL7D:;}{
# 331
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpB1,_tmpE0,ptr_rgn,1));
# 337
{void*_tmpE5=Cyc_Tcutil_compress((void*)_check_null(_tmpB1->topt));struct Cyc_Absyn_Datatypedecl*_tmpE7;struct Cyc_Absyn_Datatypefield*_tmpE8;struct Cyc_List_List*_tmpE9;_LL83: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpE6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE5;if(_tmpE6->tag != 4)goto _LL85;else{if((((_tmpE6->f1).field_info).KnownDatatypefield).tag != 2)goto _LL85;_tmpE7=((struct _tuple2)(((_tmpE6->f1).field_info).KnownDatatypefield).val).f1;_tmpE8=((struct _tuple2)(((_tmpE6->f1).field_info).KnownDatatypefield).val).f2;_tmpE9=(_tmpE6->f1).targs;}}_LL84:
# 341
{void*_tmpEA=Cyc_Tcutil_compress(inner_typ);_LL88: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpEA;if(_tmpEB->tag != 4)goto _LL8A;}_LL89:
# 343
 goto DONT_PROMOTE;_LL8A:;_LL8B:
 goto _LL87;_LL87:;}{
# 347
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp3ED;struct Cyc_Absyn_Datatypedecl**_tmp3EC;struct Cyc_Absyn_DatatypeInfo _tmp3EB;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3EA;void*new_type=(void*)((_tmp3EA=_cycalloc(sizeof(*_tmp3EA)),((_tmp3EA[0]=((_tmp3ED.tag=3,((_tmp3ED.f1=((_tmp3EB.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC[0]=_tmpE7,_tmp3EC))))),((_tmp3EB.targs=_tmpE9,_tmp3EB)))),_tmp3ED)))),_tmp3EA))));
# 350
_tmpB1->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3F7;struct Cyc_Absyn_PtrAtts _tmp3F6;struct Cyc_Absyn_PtrInfo _tmp3F5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F4;t=(void*)((_tmp3F4=_cycalloc(sizeof(*_tmp3F4)),((_tmp3F4[0]=((_tmp3F7.tag=5,((_tmp3F7.f1=((_tmp3F5.elt_typ=new_type,((_tmp3F5.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp3F5.ptr_atts=(
(_tmp3F6.rgn=ptr_rgn,((_tmp3F6.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp3F6.bounds=Cyc_Absyn_bounds_one_conref,((_tmp3F6.zero_term=Cyc_Absyn_false_conref,((_tmp3F6.ptrloc=0,_tmp3F6)))))))))),_tmp3F5)))))),_tmp3F7)))),_tmp3F4))));}
# 357
goto _LL82;};_LL85:;_LL86:
# 359
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp401;struct Cyc_Absyn_PtrAtts _tmp400;struct Cyc_Absyn_PtrInfo _tmp3FF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FE;t=(void*)((_tmp3FE=_cycalloc(sizeof(*_tmp3FE)),((_tmp3FE[0]=((_tmp401.tag=5,((_tmp401.f1=((_tmp3FF.elt_typ=(void*)_check_null(_tmpB1->topt),((_tmp3FF.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp3FF.ptr_atts=(
(_tmp400.rgn=ptr_rgn,((_tmp400.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp400.bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp400.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp400.ptrloc=0,_tmp400)))))))))),_tmp3FF)))))),_tmp401)))),_tmp3FE))));}_LL82:;}
# 367
goto _LL48;};}_LL63: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpB2=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpB2->tag != 4)goto _LL65;else{_tmpB3=(struct Cyc_List_List**)& _tmpB2->f1;_tmpB4=_tmpB2->f2;}}_LL64: {
# 370
struct Cyc_List_List*_tmpF8=*_tmpB3;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmpF9=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpFB;_LL8D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpFA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF9;if(_tmpFA->tag != 10)goto _LL8F;else{_tmpFB=_tmpFA->f1;}}_LL8E:
# 376
 topt_ts=_tmpFB;
if(_tmpB4){
# 379
int _tmpFC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF8);
int _tmpFD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFB);
if(_tmpFC < _tmpFD){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpFD - _tmpFC;++ i){
struct Cyc_List_List*_tmp402;wild_ps=((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp402->tl=wild_ps,_tmp402))))));}}
*_tmpB3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpF8,wild_ps);
_tmpF8=*_tmpB3;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF8)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFB)){
const char*_tmp405;void*_tmp404;(_tmp404=0,Cyc_Tcutil_warn(p->loc,((_tmp405="unnecessary ... in tuple pattern",_tag_dyneither(_tmp405,sizeof(char),33))),_tag_dyneither(_tmp404,sizeof(void*),0)));}}}
# 391
goto _LL8C;_LL8F:;_LL90:
# 393
 goto _LL8C;_LL8C:;}else{
# 395
if(_tmpB4){
const char*_tmp408;void*_tmp407;(_tmp407=0,Cyc_Tcutil_terr(p->loc,((_tmp408="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp408,sizeof(char),57))),_tag_dyneither(_tmp407,sizeof(void*),0)));}}
for(0;_tmpF8 != 0;_tmpF8=_tmpF8->tl){
void**_tmp103=0;
if(topt_ts != 0){
_tmp103=&(*((struct _tuple16*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}
# 403
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpF8->hd,_tmp103,rgn_pat,allow_ref_pat));{
struct _tuple16*_tmp40B;struct Cyc_List_List*_tmp40A;pat_ts=((_tmp40A=_cycalloc(sizeof(*_tmp40A)),((_tmp40A->hd=((_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B->f1=Cyc_Absyn_empty_tqual(0),((_tmp40B->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpF8->hd)->topt),_tmp40B)))))),((_tmp40A->tl=pat_ts,_tmp40A))))));};}
# 406
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp40E;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp40D;t=(void*)((_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D[0]=((_tmp40E.tag=10,((_tmp40E.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp40E)))),_tmp40D))));}
goto _LL48;}_LL65: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB5=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpB5->tag != 6)goto _LL67;else{if(_tmpB5->f1 == 0)goto _LL67;if((((_tmpB5->f1)->aggr_info).KnownAggr).tag != 2)goto _LL67;_tmpB6=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpB5->f1)->aggr_info).KnownAggr).val);_tmpB7=(struct Cyc_List_List**)&(_tmpB5->f1)->targs;_tmpB8=_tmpB5->f2;_tmpB9=(struct Cyc_List_List**)& _tmpB5->f3;_tmpBA=_tmpB5->f4;}}_LL66: {
# 410
struct Cyc_List_List*_tmp108=*_tmpB9;
const char*_tmp410;const char*_tmp40F;struct _dyneither_ptr aggr_str=_tmpB6->kind == Cyc_Absyn_StructA?(_tmp410="struct",_tag_dyneither(_tmp410,sizeof(char),7)):((_tmp40F="union",_tag_dyneither(_tmp40F,sizeof(char),6)));
if(_tmpB6->impl == 0){
{const char*_tmp414;void*_tmp413[1];struct Cyc_String_pa_PrintArg_struct _tmp412;(_tmp412.tag=0,((_tmp412.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp413[0]=& _tmp412,Cyc_Tcutil_terr(p->loc,((_tmp414="can't destructure an abstract %s",_tag_dyneither(_tmp414,sizeof(char),33))),_tag_dyneither(_tmp413,sizeof(void*),1)))))));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL48;}
# 418
if(_tmpB6->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->tagged)
allow_ref_pat=0;
if(_tmpB8 != 0){
# 422
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 425
struct _RegionHandle _tmp10C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp10C;_push_region(rgn);{
# 427
struct Cyc_List_List*_tmp10D=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp10E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpB8;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp10E))->hd;
_tmp10E=_tmp10E->tl;{
void*_tmp10F=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp110=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp111=_tmp110;struct Cyc_Absyn_Kind*_tmp113;struct Cyc_Absyn_Kind*_tmp115;_LL92: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp112=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp111;if(_tmp112->tag != 2)goto _LL94;else{_tmp113=_tmp112->f2;}}_LL93:
 _tmp115=_tmp113;goto _LL95;_LL94: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp114=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp111;if(_tmp114->tag != 0)goto _LL96;else{_tmp115=_tmp114->f1;}}_LL95:
 k2=_tmp115;goto _LL91;_LL96:;_LL97: {
const char*_tmp417;void*_tmp416;(_tmp416=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp417="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp417,sizeof(char),53))),_tag_dyneither(_tmp416,sizeof(void*),0)));}_LL91:;}
# 443
{void*_tmp118=_tmp10F;struct Cyc_Absyn_Kind*_tmp11A;struct Cyc_Core_Opt**_tmp11C;struct Cyc_Absyn_Kind*_tmp11D;struct Cyc_Core_Opt**_tmp11F;_LL99: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp119=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp118;if(_tmp119->tag != 0)goto _LL9B;else{_tmp11A=_tmp119->f1;}}_LL9A:
# 446
 if(!Cyc_Tcutil_kind_leq(k2,_tmp11A))
error=1;
goto _LL98;_LL9B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp118;if(_tmp11B->tag != 2)goto _LL9D;else{_tmp11C=(struct Cyc_Core_Opt**)& _tmp11B->f1;_tmp11D=_tmp11B->f2;}}_LL9C:
 _tmp11F=_tmp11C;goto _LL9E;_LL9D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp11E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp118;if(_tmp11E->tag != 1)goto _LL98;else{_tmp11F=(struct Cyc_Core_Opt**)& _tmp11E->f1;}}_LL9E:
{struct Cyc_Core_Opt*_tmp418;*_tmp11F=((_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418->v=_tmp110,_tmp418))));}goto _LL98;_LL98:;}
# 452
if(error){
const char*_tmp41E;void*_tmp41D[3];struct Cyc_String_pa_PrintArg_struct _tmp41C;struct Cyc_String_pa_PrintArg_struct _tmp41B;struct Cyc_String_pa_PrintArg_struct _tmp41A;(_tmp41A.tag=0,((_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 456
Cyc_Absynpp_kind2string(k2)),((_tmp41B.tag=0,((_tmp41B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp10F)),((_tmp41C.tag=0,((_tmp41C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp41D[0]=& _tmp41C,((_tmp41D[1]=& _tmp41B,((_tmp41D[2]=& _tmp41A,Cyc_Tcutil_terr(p->loc,((_tmp41E="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp41E,sizeof(char),60))),_tag_dyneither(_tmp41D,sizeof(void*),3)))))))))))))))))));}{
# 458
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp421;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp420;void*vartype=(void*)((_tmp420=_cycalloc(sizeof(*_tmp420)),((_tmp420[0]=((_tmp421.tag=2,((_tmp421.f1=tv,_tmp421)))),_tmp420))));
{struct Cyc_List_List*_tmp422;_tmp10D=((_tmp422=_region_malloc(rgn,sizeof(*_tmp422)),((_tmp422->hd=vartype,((_tmp422->tl=_tmp10D,_tmp422))))));}
# 462
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp425;struct Cyc_List_List*_tmp424;outlives_constraints=(
(_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424->hd=((_tmp425=_cycalloc(sizeof(*_tmp425)),((_tmp425->f1=Cyc_Absyn_empty_effect,((_tmp425->f2=vartype,_tmp425)))))),((_tmp424->tl=outlives_constraints,_tmp424))))));}else{
# 467
const char*_tmp428;void*_tmp427;(_tmp427=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp428="opened existential had unique or top region kind",_tag_dyneither(_tmp428,sizeof(char),49))),_tag_dyneither(_tmp427,sizeof(void*),0)));}}};};}}
# 471
_tmp10D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp10D);{
# 473
struct _RegionHandle _tmp12D=_new_region("r2");struct _RegionHandle*r2=& _tmp12D;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpB8);
# 476
struct Cyc_List_List*_tmp12E=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple11 _tmp429;struct _tuple11 _tmp12F=(_tmp429.f1=_tmp12E,((_tmp429.f2=rgn,_tmp429)));
struct Cyc_List_List*_tmp130=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp12F,_tmpB6->tvs);
struct Cyc_List_List*_tmp131=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->exist_vars,_tmp10D);
struct Cyc_List_List*_tmp132=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp130);
struct Cyc_List_List*_tmp133=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp131);
struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp130,_tmp131);
# 484
if(_tmpB8 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple13*_tmp42A;res.tvars_and_bounds_opt=((_tmp42A=_cycalloc(sizeof(*_tmp42A)),((_tmp42A->f1=0,((_tmp42A->f2=0,_tmp42A))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmpB8);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp136=0;
{struct Cyc_List_List*_tmp137=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->rgn_po;for(0;_tmp137 != 0;_tmp137=_tmp137->tl){
struct _tuple0*_tmp42D;struct Cyc_List_List*_tmp42C;_tmp136=((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C->hd=((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp134,(*((struct _tuple0*)_tmp137->hd)).f1),((_tmp42D->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp134,(*((struct _tuple0*)_tmp137->hd)).f2),_tmp42D)))))),((_tmp42C->tl=_tmp136,_tmp42C))))));}}
# 496
_tmp136=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp136);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp136);};}
# 501
*_tmpB7=_tmp132;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp437;struct Cyc_Absyn_Aggrdecl**_tmp436;struct Cyc_Absyn_AggrInfo _tmp435;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp434;t=(void*)((_tmp434=_cycalloc(sizeof(*_tmp434)),((_tmp434[0]=((_tmp437.tag=11,((_tmp437.f1=((_tmp435.aggr_info=Cyc_Absyn_KnownAggr(((_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436[0]=_tmpB6,_tmp436))))),((_tmp435.targs=*_tmpB7,_tmp435)))),_tmp437)))),_tmp434))));}
if(_tmpBA  && _tmpB6->kind == Cyc_Absyn_UnionA){
const char*_tmp43A;void*_tmp439;(_tmp439=0,Cyc_Tcutil_warn(p->loc,((_tmp43A="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp43A,sizeof(char),43))),_tag_dyneither(_tmp439,sizeof(void*),0)));}else{
if(_tmpBA){
# 507
int _tmp140=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp108);
int _tmp141=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->fields);
if(_tmp140 < _tmp141){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp141 - _tmp140;++ i){
struct _tuple15*_tmp43D;struct Cyc_List_List*_tmp43C;wild_dps=((_tmp43C=_cycalloc(sizeof(*_tmp43C)),((_tmp43C->hd=((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D->f1=0,((_tmp43D->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp43D)))))),((_tmp43C->tl=wild_dps,_tmp43C))))));}}
*_tmpB9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp108,wild_dps);
_tmp108=*_tmpB9;}else{
if(_tmp140 == _tmp141){
const char*_tmp440;void*_tmp43F;(_tmp43F=0,Cyc_Tcutil_warn(p->loc,((_tmp440="unnecessary ... in struct pattern",_tag_dyneither(_tmp440,sizeof(char),34))),_tag_dyneither(_tmp43F,sizeof(void*),0)));}}}}{
# 518
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp108,_tmpB6->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp147;struct Cyc_Absyn_Pat*_tmp148;struct _tuple17*_tmp146=(struct _tuple17*)fields->hd;_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;{
void*_tmp149=Cyc_Tcutil_rsubstitute(rgn,_tmp134,_tmp147->type);
# 524
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp148,& _tmp149,rgn_pat,allow_ref_pat));
# 528
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp148->topt),_tmp149)){
const char*_tmp447;void*_tmp446[4];struct Cyc_String_pa_PrintArg_struct _tmp445;struct Cyc_String_pa_PrintArg_struct _tmp444;struct Cyc_String_pa_PrintArg_struct _tmp443;struct Cyc_String_pa_PrintArg_struct _tmp442;(_tmp442.tag=0,((_tmp442.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 531
Cyc_Absynpp_typ2string((void*)_check_null(_tmp148->topt))),((_tmp443.tag=0,((_tmp443.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 530
Cyc_Absynpp_typ2string(_tmp149)),((_tmp444.tag=0,((_tmp444.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp445.tag=0,((_tmp445.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp147->name),((_tmp446[0]=& _tmp445,((_tmp446[1]=& _tmp444,((_tmp446[2]=& _tmp443,((_tmp446[3]=& _tmp442,Cyc_Tcutil_terr(p->loc,((_tmp447="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp447,sizeof(char),45))),_tag_dyneither(_tmp446,sizeof(void*),4)))))))))))))))))))))))));}};}};}
# 474
;_pop_region(r2);};}
# 535
_npop_handler(0);goto _LL48;
# 425
;_pop_region(rgn);};}_LL67: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpBB=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpBB->tag != 7)goto _LL69;else{_tmpBC=_tmpBB->f1;_tmpBD=_tmpBB->f2;_tmpBE=(struct Cyc_List_List**)& _tmpBB->f3;_tmpBF=_tmpBB->f4;}}_LL68: {
# 538
struct Cyc_List_List*_tmp153=*_tmpBE;
struct _RegionHandle _tmp154=_new_region("rgn");struct _RegionHandle*rgn=& _tmp154;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmpBD->typs;
# 542
struct Cyc_List_List*_tmp155=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple11 _tmp448;struct _tuple11 _tmp156=(_tmp448.f1=_tmp155,((_tmp448.f2=rgn,_tmp448)));
struct Cyc_List_List*_tmp157=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp156,_tmpBC->tvs);
struct Cyc_List_List*_tmp158=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp157);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp44E;struct Cyc_Absyn_DatatypeFieldInfo _tmp44D;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp44C;t=(void*)((_tmp44C=_cycalloc(sizeof(*_tmp44C)),((_tmp44C[0]=((_tmp44E.tag=4,((_tmp44E.f1=((_tmp44D.field_info=Cyc_Absyn_KnownDatatypefield(_tmpBC,_tmpBD),((_tmp44D.targs=_tmp158,_tmp44D)))),_tmp44E)))),_tmp44C))));}
# 548
if(_tmpBF){
# 550
int _tmp15C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp153);
int _tmp15D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp15C < _tmp15D){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp15D - _tmp15C;++ i){
struct Cyc_List_List*_tmp44F;wild_ps=((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp44F->tl=wild_ps,_tmp44F))))));}}
*_tmpBE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp153,wild_ps);
_tmp153=*_tmpBE;}else{
if(_tmp15C == _tmp15D){
const char*_tmp453;void*_tmp452[1];struct Cyc_String_pa_PrintArg_struct _tmp451;(_tmp451.tag=0,((_tmp451.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBC->name)),((_tmp452[0]=& _tmp451,Cyc_Tcutil_warn(p->loc,((_tmp453="unnecessary ... in datatype field %s",_tag_dyneither(_tmp453,sizeof(char),37))),_tag_dyneither(_tmp452,sizeof(void*),1)))))));}}}
# 562
for(0;_tmp153 != 0  && tqts != 0;(_tmp153=_tmp153->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp162=(struct Cyc_Absyn_Pat*)_tmp153->hd;
# 566
void*_tmp163=Cyc_Tcutil_rsubstitute(rgn,_tmp157,(*((struct _tuple16*)tqts->hd)).f2);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp162,& _tmp163,rgn_pat,allow_ref_pat));
# 571
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp162->topt),_tmp163)){
const char*_tmp459;void*_tmp458[3];struct Cyc_String_pa_PrintArg_struct _tmp457;struct Cyc_String_pa_PrintArg_struct _tmp456;struct Cyc_String_pa_PrintArg_struct _tmp455;(_tmp455.tag=0,((_tmp455.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 574
Cyc_Absynpp_typ2string((void*)_check_null(_tmp162->topt))),((_tmp456.tag=0,((_tmp456.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 573
Cyc_Absynpp_typ2string(_tmp163)),((_tmp457.tag=0,((_tmp457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD->name)),((_tmp458[0]=& _tmp457,((_tmp458[1]=& _tmp456,((_tmp458[2]=& _tmp455,Cyc_Tcutil_terr(_tmp162->loc,((_tmp459="%s expects argument type %s, not %s",_tag_dyneither(_tmp459,sizeof(char),36))),_tag_dyneither(_tmp458,sizeof(void*),3)))))))))))))))))));}}
# 576
if(_tmp153 != 0){
const char*_tmp45D;void*_tmp45C[1];struct Cyc_String_pa_PrintArg_struct _tmp45B;(_tmp45B.tag=0,((_tmp45B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD->name)),((_tmp45C[0]=& _tmp45B,Cyc_Tcutil_terr(p->loc,((_tmp45D="too many arguments for datatype constructor %s",_tag_dyneither(_tmp45D,sizeof(char),47))),_tag_dyneither(_tmp45C,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp461;void*_tmp460[1];struct Cyc_String_pa_PrintArg_struct _tmp45F;(_tmp45F.tag=0,((_tmp45F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD->name)),((_tmp460[0]=& _tmp45F,Cyc_Tcutil_terr(p->loc,((_tmp461="too few arguments for datatype constructor %s",_tag_dyneither(_tmp461,sizeof(char),46))),_tag_dyneither(_tmp460,sizeof(void*),1)))))));}}
# 583
_npop_handler(0);goto _LL48;
# 539
;_pop_region(rgn);}_LL69: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC0=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC0->tag != 6)goto _LL6B;else{if(_tmpC0->f1 != 0)goto _LL6B;}}_LL6A:
# 585
 goto _LL6C;_LL6B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC1=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC1->tag != 6)goto _LL6D;else{if(_tmpC1->f1 == 0)goto _LL6D;if((((_tmpC1->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL6D;}}_LL6C:
 goto _LL6E;_LL6D: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpC2=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC2->tag != 14)goto _LL6F;}_LL6E:
 goto _LL70;_LL6F: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpC3=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC3->tag != 16)goto _LL71;}_LL70:
 goto _LL72;_LL71: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpC4=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC4->tag != 15)goto _LL48;}_LL72:
# 590
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL48;_LL48:;}
# 592
tcpat_end:
 p->topt=t;
return res;};}
# 597
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){
struct Cyc_Tcpat_TcPatResult _tmp171=Cyc_Tcpat_tcPatRec(te,p,topt,0,0);
# 600
struct _RegionHandle _tmp172=_new_region("r");struct _RegionHandle*r=& _tmp172;_push_region(r);{
const char*_tmp462;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp171.patvars),p->loc,(
(_tmp462="pattern contains a repeated variable",_tag_dyneither(_tmp462,sizeof(char),37))));}{
# 604
struct Cyc_Tcpat_TcPatResult _tmp174=_tmp171;_npop_handler(0);return _tmp174;};
# 600
;_pop_region(r);}
# 607
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
void*_tmp175=p->r;struct Cyc_Absyn_Pat*_tmp177;struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17B;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Pat*_tmp17F;struct Cyc_Absyn_Pat*_tmp181;_LLA0: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp176=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp175;if(_tmp176->tag != 5)goto _LLA2;else{_tmp177=_tmp176->f1;}}_LLA1:
# 610
 Cyc_Tcpat_check_pat_regions(te,_tmp177);{
# 612
struct _RegionHandle _tmp182=_new_region("temp");struct _RegionHandle*temp=& _tmp182;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(_tmp177->topt))){
{const char*_tmp465;void*_tmp464;(_tmp464=0,Cyc_Tcutil_terr(p->loc,((_tmp465="Pattern dereferences to a non-aliased pointer; use swap",_tag_dyneither(_tmp465,sizeof(char),56))),_tag_dyneither(_tmp464,sizeof(void*),0)));}
_npop_handler(0);return;}
# 619
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(p->topt))){
{const char*_tmp468;void*_tmp467;(_tmp467=0,Cyc_Tcutil_terr(p->loc,((_tmp468="Pattern dereferences a non-aliased pointer; use swap",_tag_dyneither(_tmp468,sizeof(char),53))),_tag_dyneither(_tmp467,sizeof(void*),0)));}
_npop_handler(0);return;}{
# 624
void*_tmp187=(void*)_check_null(p->topt);void*_tmp189;_LLAF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp187;if(_tmp188->tag != 5)goto _LLB1;else{_tmp189=((_tmp188->f1).ptr_atts).rgn;}}_LLB0:
# 626
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp189);
_npop_handler(0);return;_LLB1:;_LLB2: {
const char*_tmp46B;void*_tmp46A;(_tmp46A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp46B="check_pat_regions: bad pointer type",_tag_dyneither(_tmp46B,sizeof(char),36))),_tag_dyneither(_tmp46A,sizeof(void*),0)));}_LLAE:;};
# 612
;_pop_region(temp);};_LLA2: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp178=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp175;if(_tmp178->tag != 6)goto _LLA4;else{_tmp179=_tmp178->f3;}}_LLA3:
# 631
 for(0;_tmp179 != 0;_tmp179=_tmp179->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct _tuple15*)_tmp179->hd)).f2);}
return;_LLA4: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp17A=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp175;if(_tmp17A->tag != 7)goto _LLA6;else{_tmp17B=_tmp17A->f3;}}_LLA5:
# 634
 _tmp17D=_tmp17B;goto _LLA7;_LLA6: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp17C=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp175;if(_tmp17C->tag != 4)goto _LLA8;else{_tmp17D=_tmp17C->f1;}}_LLA7:
# 636
 for(0;_tmp17D != 0;_tmp17D=_tmp17D->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp17D->hd);}
return;_LLA8: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp17E=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp175;if(_tmp17E->tag != 2)goto _LLAA;else{_tmp17F=_tmp17E->f2;}}_LLA9:
 _tmp181=_tmp17F;goto _LLAB;_LLAA: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp180=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp175;if(_tmp180->tag != 1)goto _LLAC;else{_tmp181=_tmp180->f2;}}_LLAB:
 Cyc_Tcpat_check_pat_regions(te,_tmp181);
return;_LLAC:;_LLAD:
 return;_LL9F:;}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 682 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp46C;return((_tmp46C.Name_v).val=s,(((_tmp46C.Name_v).tag=1,_tmp46C)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp46D;return((_tmp46D.Int_v).val=i,(((_tmp46D.Int_v).tag=2,_tmp46D)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 692
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 697
typedef void*Cyc_Tcpat_simple_pat_t;
# 700
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp18E=c1->name;struct _dyneither_ptr _tmp18F;int _tmp190;_LLB4: if((_tmp18E.Name_v).tag != 1)goto _LLB6;_tmp18F=(struct _dyneither_ptr)(_tmp18E.Name_v).val;_LLB5: {
# 703
union Cyc_Tcpat_Name_value _tmp191=c2->name;struct _dyneither_ptr _tmp192;_LLB9: if((_tmp191.Name_v).tag != 1)goto _LLBB;_tmp192=(struct _dyneither_ptr)(_tmp191.Name_v).val;_LLBA:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp18F,(struct _dyneither_ptr)_tmp192);_LLBB: if((_tmp191.Int_v).tag != 2)goto _LLB8;_LLBC:
 return - 1;_LLB8:;}_LLB6: if((_tmp18E.Int_v).tag != 2)goto _LLB3;_tmp190=(int)(_tmp18E.Int_v).val;_LLB7: {
# 708
union Cyc_Tcpat_Name_value _tmp193=c2->name;int _tmp194;_LLBE: if((_tmp193.Name_v).tag != 1)goto _LLC0;_LLBF:
 return 1;_LLC0: if((_tmp193.Int_v).tag != 2)goto _LLBD;_tmp194=(int)(_tmp193.Int_v).val;_LLC1:
 return _tmp190 - _tmp194;_LLBD:;}_LLB3:;}
# 716
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 720
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 724
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp470;struct Cyc_Tcpat_Con_s*_tmp46F;return(_tmp46F=_region_malloc(r,sizeof(*_tmp46F)),((_tmp46F->name=Cyc_Tcpat_Name_v(((_tmp470="NULL",_tag_dyneither(_tmp470,sizeof(char),5)))),((_tmp46F->arity=0,((_tmp46F->span=& Cyc_Tcpat_two_opt,((_tmp46F->orig_pat=p,_tmp46F)))))))));}
# 727
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp473;struct Cyc_Tcpat_Con_s*_tmp472;return(_tmp472=_region_malloc(r,sizeof(*_tmp472)),((_tmp472->name=Cyc_Tcpat_Name_v(((_tmp473="&",_tag_dyneither(_tmp473,sizeof(char),2)))),((_tmp472->arity=1,((_tmp472->span=& Cyc_Tcpat_two_opt,((_tmp472->orig_pat=p,_tmp472)))))))));}
# 730
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp476;struct Cyc_Tcpat_Con_s*_tmp475;return(_tmp475=_region_malloc(r,sizeof(*_tmp475)),((_tmp475->name=Cyc_Tcpat_Name_v(((_tmp476="&",_tag_dyneither(_tmp476,sizeof(char),2)))),((_tmp475->arity=1,((_tmp475->span=& Cyc_Tcpat_one_opt,((_tmp475->orig_pat=p,_tmp475)))))))));}
# 733
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp477;return(_tmp477=_region_malloc(r,sizeof(*_tmp477)),((_tmp477->name=Cyc_Tcpat_Int_v(i),((_tmp477->arity=0,((_tmp477->span=0,((_tmp477->orig_pat=p,_tmp477)))))))));}
# 736
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp478;return(_tmp478=_region_malloc(r,sizeof(*_tmp478)),((_tmp478->name=Cyc_Tcpat_Name_v(f),((_tmp478->arity=0,((_tmp478->span=0,((_tmp478->orig_pat=p,_tmp478)))))))));}
# 739
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp479;return(_tmp479=_region_malloc(r,sizeof(*_tmp479)),((_tmp479->name=Cyc_Tcpat_Int_v((int)c),((_tmp479->arity=0,((_tmp479->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp479->orig_pat=p,_tmp479)))))))));}
# 742
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp47C;struct Cyc_Tcpat_Con_s*_tmp47B;return(_tmp47B=_region_malloc(r,sizeof(*_tmp47B)),((_tmp47B->name=Cyc_Tcpat_Name_v(((_tmp47C="$",_tag_dyneither(_tmp47C,sizeof(char),2)))),((_tmp47B->arity=i,((_tmp47B->span=& Cyc_Tcpat_one_opt,((_tmp47B->orig_pat=p,_tmp47B)))))))));}
# 747
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp47F;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp47E;return(void*)((_tmp47E=_region_malloc(r,sizeof(*_tmp47E)),((_tmp47E[0]=((_tmp47F.tag=1,((_tmp47F.f1=Cyc_Tcpat_null_con(r,p),((_tmp47F.f2=0,_tmp47F)))))),_tmp47E))));}
# 750
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp482;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp481;return(void*)((_tmp481=_region_malloc(r,sizeof(*_tmp481)),((_tmp481[0]=((_tmp482.tag=1,((_tmp482.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp482.f2=0,_tmp482)))))),_tmp481))));}
# 753
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp485;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp484;return(void*)((_tmp484=_region_malloc(r,sizeof(*_tmp484)),((_tmp484[0]=((_tmp485.tag=1,((_tmp485.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp485.f2=0,_tmp485)))))),_tmp484))));}
# 756
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp488;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp487;return(void*)((_tmp487=_region_malloc(r,sizeof(*_tmp487)),((_tmp487[0]=((_tmp488.tag=1,((_tmp488.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp488.f2=0,_tmp488)))))),_tmp487))));}
# 759
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp48E;struct Cyc_List_List*_tmp48D;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp48C;return(void*)((_tmp48C=_region_malloc(r,sizeof(*_tmp48C)),((_tmp48C[0]=((_tmp48E.tag=1,((_tmp48E.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp48E.f2=((_tmp48D=_region_malloc(r,sizeof(*_tmp48D)),((_tmp48D->hd=p,((_tmp48D->tl=0,_tmp48D)))))),_tmp48E)))))),_tmp48C))));}
# 762
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp494;struct Cyc_List_List*_tmp493;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp492;return(void*)((_tmp492=_region_malloc(r,sizeof(*_tmp492)),((_tmp492[0]=((_tmp494.tag=1,((_tmp494.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp494.f2=((_tmp493=_region_malloc(r,sizeof(*_tmp493)),((_tmp493->hd=p,((_tmp493->tl=0,_tmp493)))))),_tmp494)))))),_tmp492))));}
# 765
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp497;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp496;return(void*)((_tmp496=_region_malloc(r,sizeof(*_tmp496)),((_tmp496[0]=((_tmp497.tag=1,((_tmp497.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp497.f2=ss,_tmp497)))))),_tmp496))));}
# 768
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 770
struct Cyc_Tcpat_Con_s*_tmp498;struct Cyc_Tcpat_Con_s*c=(_tmp498=_region_malloc(r,sizeof(*_tmp498)),((_tmp498->name=Cyc_Tcpat_Name_v(con_name),((_tmp498->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp498->span=span,((_tmp498->orig_pat=p,_tmp498)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp49B;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp49A;return(void*)((_tmp49A=_region_malloc(r,sizeof(*_tmp49A)),((_tmp49A[0]=((_tmp49B.tag=1,((_tmp49B.f1=c,((_tmp49B.f2=ps,_tmp49B)))))),_tmp49A))));}
# 775
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1B3=p->r;enum Cyc_Absyn_Sign _tmp1B8;int _tmp1B9;char _tmp1BB;struct _dyneither_ptr _tmp1BD;struct Cyc_Absyn_Pat*_tmp1BF;struct Cyc_Absyn_Pat*_tmp1C1;struct Cyc_Absyn_Pat*_tmp1C3;struct Cyc_Absyn_Datatypedecl*_tmp1C5;struct Cyc_Absyn_Datatypefield*_tmp1C6;struct Cyc_List_List*_tmp1C7;struct Cyc_List_List*_tmp1C9;struct Cyc_Absyn_Aggrdecl*_tmp1CB;struct Cyc_List_List*_tmp1CC;struct Cyc_Absyn_Enumdecl*_tmp1CE;struct Cyc_Absyn_Enumfield*_tmp1CF;void*_tmp1D1;struct Cyc_Absyn_Enumfield*_tmp1D2;_LLC3: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1B4=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1B4->tag != 0)goto _LLC5;}_LLC4:
 goto _LLC6;_LLC5: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp1B5=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1B5->tag != 3)goto _LLC7;}_LLC6:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp49E;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp49D;s=(void*)((_tmp49D=_region_malloc(r,sizeof(*_tmp49D)),((_tmp49D[0]=((_tmp49E.tag=0,_tmp49E)),_tmp49D))));}goto _LLC2;_LLC7: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1B6=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1B6->tag != 8)goto _LLC9;}_LLC8:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLC2;_LLC9: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1B7=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1B7->tag != 9)goto _LLCB;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B7->f2;}}_LLCA:
 s=Cyc_Tcpat_int_pat(r,_tmp1B9,p);goto _LLC2;_LLCB: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1BA=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1BA->tag != 10)goto _LLCD;else{_tmp1BB=_tmp1BA->f1;}}_LLCC:
 s=Cyc_Tcpat_char_pat(r,_tmp1BB,p);goto _LLC2;_LLCD: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1BC=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1BC->tag != 11)goto _LLCF;else{_tmp1BD=_tmp1BC->f1;}}_LLCE:
 s=Cyc_Tcpat_float_pat(r,_tmp1BD,p);goto _LLC2;_LLCF: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1BE=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1BE->tag != 1)goto _LLD1;else{_tmp1BF=_tmp1BE->f2;}}_LLD0:
 s=Cyc_Tcpat_compile_pat(r,_tmp1BF);goto _LLC2;_LLD1: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1C0=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1C0->tag != 2)goto _LLD3;else{_tmp1C1=_tmp1C0->f2;}}_LLD2:
 s=Cyc_Tcpat_compile_pat(r,_tmp1C1);goto _LLC2;_LLD3: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1C2=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1C2->tag != 5)goto _LLD5;else{_tmp1C3=_tmp1C2->f1;}}_LLD4:
# 787
{void*_tmp1D9=Cyc_Tcutil_compress((void*)_check_null(p->topt));union Cyc_Absyn_Constraint*_tmp1DB;_LLE8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DA->tag != 5)goto _LLEA;else{_tmp1DB=((_tmp1DA->f1).ptr_atts).nullable;}}_LLE9: {
# 789
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1DC=_tmp1DB;union Cyc_Absyn_Constraint*_tmp1DD;int _tmp1DE;_LLED: if((_tmp1DC->Forward_constr).tag != 2)goto _LLEF;_tmp1DD=(union Cyc_Absyn_Constraint*)(_tmp1DC->Forward_constr).val;_LLEE:
# 794
*_tmp1DB=*_tmp1DD;
continue;_LLEF: if((_tmp1DC->No_constr).tag != 3)goto _LLF1;_LLF0:
# 797
{struct _union_Constraint_Eq_constr*_tmp49F;(_tmp49F=& _tmp1DB->Eq_constr,((_tmp49F->tag=1,_tmp49F->val=0)));}
is_nullable=0;
still_working=0;
goto _LLEC;_LLF1: if((_tmp1DC->Eq_constr).tag != 1)goto _LLEC;_tmp1DE=(int)(_tmp1DC->Eq_constr).val;_LLF2:
# 802
 is_nullable=_tmp1DE;
still_working=0;
goto _LLEC;_LLEC:;}{
# 807
void*ss=Cyc_Tcpat_compile_pat(r,_tmp1C3);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 811
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LLE7;};}_LLEA:;_LLEB: {
const char*_tmp4A2;void*_tmp4A1;(_tmp4A1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4A2="expecting pointertype for pattern!",_tag_dyneither(_tmp4A2,sizeof(char),35))),_tag_dyneither(_tmp4A1,sizeof(void*),0)));}_LLE7:;}
# 815
goto _LLC2;_LLD5: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1C4=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1C4->tag != 7)goto _LLD7;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C4->f2;_tmp1C7=_tmp1C4->f3;}}_LLD6: {
# 817
int*span;
{void*_tmp1E2=Cyc_Tcutil_compress((void*)_check_null(p->topt));_LLF4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1E3=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1E2;if(_tmp1E3->tag != 3)goto _LLF6;}_LLF5:
# 820
 if(_tmp1C5->is_extensible)
span=0;else{
# 823
int*_tmp4A3;span=((_tmp4A3=_region_malloc(r,sizeof(*_tmp4A3)),((_tmp4A3[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1C5->fields))->v),_tmp4A3))));}
goto _LLF3;_LLF6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1E4=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1E2;if(_tmp1E4->tag != 4)goto _LLF8;}_LLF7:
 span=& Cyc_Tcpat_one_opt;goto _LLF3;_LLF8:;_LLF9:
{const char*_tmp4A6;void*_tmp4A5;span=((_tmp4A5=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4A6="void datatype pattern has bad type",_tag_dyneither(_tmp4A6,sizeof(char),35))),_tag_dyneither(_tmp4A5,sizeof(void*),0))));}goto _LLF3;_LLF3:;}
# 828
s=Cyc_Tcpat_con_pat(r,*(*_tmp1C6->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1C7),p);
goto _LLC2;}_LLD7: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1C8=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1C8->tag != 4)goto _LLD9;else{_tmp1C9=_tmp1C8->f1;}}_LLD8:
# 832
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1C9),p);goto _LLC2;_LLD9: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1CA=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1CA->tag != 6)goto _LLDB;else{if(_tmp1CA->f1 == 0)goto _LLDB;if((((_tmp1CA->f1)->aggr_info).KnownAggr).tag != 2)goto _LLDB;_tmp1CB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp1CA->f1)->aggr_info).KnownAggr).val);_tmp1CC=_tmp1CA->f3;}}_LLDA:
# 837
 if(_tmp1CB->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1CB->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 841
const char*_tmp4A7;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp4A7="",_tag_dyneither(_tmp4A7,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp1CC;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct Cyc_List_List*_tmp1E9;struct Cyc_Absyn_Pat*_tmp1EA;struct _tuple15*_tmp1E8=(struct _tuple15*)dlps0->hd;_tmp1E9=_tmp1E8->f1;_tmp1EA=_tmp1E8->f2;{
struct Cyc_List_List*_tmp1EB=_tmp1E9;struct _dyneither_ptr*_tmp1ED;_LLFB: if(_tmp1EB == 0)goto _LLFD;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1EC=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp1EB->hd);if(_tmp1EC->tag != 1)goto _LLFD;else{_tmp1ED=_tmp1EC->f1;}}if(_tmp1EB->tl != 0)goto _LLFD;_LLFC:
# 846
 if(Cyc_strptrcmp(_tmp1ED,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp4A8;ps=((_tmp4A8=_region_malloc(r,sizeof(*_tmp4A8)),((_tmp4A8->hd=Cyc_Tcpat_compile_pat(r,_tmp1EA),((_tmp4A8->tl=ps,_tmp4A8))))));}
found=1;}
# 850
goto _LLFA;_LLFD:;_LLFE: {
const char*_tmp4AB;void*_tmp4AA;(_tmp4AA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4AB="bad designator(s)",_tag_dyneither(_tmp4AB,sizeof(char),18))),_tag_dyneither(_tmp4AA,sizeof(void*),0)));}_LLFA:;};}}
# 854
if(!found){
const char*_tmp4AE;void*_tmp4AD;(_tmp4AD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4AE="bad designator",_tag_dyneither(_tmp4AE,sizeof(char),15))),_tag_dyneither(_tmp4AD,sizeof(void*),0)));}}}
# 857
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 860
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1CB->impl))->tagged){
const char*_tmp4B1;void*_tmp4B0;(_tmp4B0=0,Cyc_Tcutil_terr(p->loc,((_tmp4B1="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp4B1,sizeof(char),47))),_tag_dyneither(_tmp4B0,sizeof(void*),0)));}{
int*_tmp4B2;int*span=(_tmp4B2=_region_malloc(r,sizeof(*_tmp4B2)),((_tmp4B2[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1CB->impl))->fields),_tmp4B2)));
struct Cyc_List_List*_tmp1F6=_tmp1CC;struct _dyneither_ptr*_tmp1F8;struct Cyc_Absyn_Pat*_tmp1F9;_LL100: if(_tmp1F6 == 0)goto _LL102;if(((struct _tuple15*)_tmp1F6->hd)->f1 == 0)goto _LL102;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1F7=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp1F6->hd)->f1)->hd);if(_tmp1F7->tag != 1)goto _LL102;else{_tmp1F8=_tmp1F7->f1;}}if((((struct _tuple15*)_tmp1F6->hd)->f1)->tl != 0)goto _LL102;_tmp1F9=((struct _tuple15*)_tmp1F6->hd)->f2;if(_tmp1F6->tl != 0)goto _LL102;_LL101:
# 865
{struct Cyc_List_List*_tmp4B3;s=Cyc_Tcpat_con_pat(r,*_tmp1F8,span,((_tmp4B3=_region_malloc(r,sizeof(*_tmp4B3)),((_tmp4B3->hd=Cyc_Tcpat_compile_pat(r,_tmp1F9),((_tmp4B3->tl=0,_tmp4B3)))))),p);}
goto _LLFF;_LL102:;_LL103: {
const char*_tmp4B6;void*_tmp4B5;(_tmp4B5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B6="bad union pattern",_tag_dyneither(_tmp4B6,sizeof(char),18))),_tag_dyneither(_tmp4B5,sizeof(void*),0)));}_LLFF:;};}
# 870
goto _LLC2;_LLDB: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1CD=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1CD->tag != 12)goto _LLDD;else{_tmp1CE=_tmp1CD->f1;_tmp1CF=_tmp1CD->f2;}}_LLDC: {
# 875
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1CE->fields))->v);
{int*_tmp4B7;s=Cyc_Tcpat_con_pat(r,*(*_tmp1CF->name).f2,((_tmp4B7=_region_malloc(r,sizeof(*_tmp4B7)),((_tmp4B7[0]=span,_tmp4B7)))),0,p);}
goto _LLC2;}_LLDD: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1D0=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1D0->tag != 13)goto _LLDF;else{_tmp1D1=(void*)_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;}}_LLDE: {
# 882
struct Cyc_List_List*fields;
{void*_tmp1FF=Cyc_Tcutil_compress(_tmp1D1);struct Cyc_List_List*_tmp201;_LL105: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp200=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1FF;if(_tmp200->tag != 14)goto _LL107;else{_tmp201=_tmp200->f1;}}_LL106:
 fields=_tmp201;goto _LL104;_LL107:;_LL108: {
const char*_tmp4BA;void*_tmp4B9;(_tmp4B9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4BA="bad type in AnonEnum_p",_tag_dyneither(_tmp4BA,sizeof(char),23))),_tag_dyneither(_tmp4B9,sizeof(void*),0)));}_LL104:;}{
# 889
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp4BB;s=Cyc_Tcpat_con_pat(r,*(*_tmp1D2->name).f2,((_tmp4BB=_region_malloc(r,sizeof(*_tmp4BB)),((_tmp4BB[0]=span,_tmp4BB)))),0,p);}
goto _LLC2;};}_LLDF: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1D3=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1D3->tag != 14)goto _LLE1;}_LLE0:
# 893
 goto _LLE2;_LLE1: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1D4=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1D4->tag != 15)goto _LLE3;}_LLE2:
 goto _LLE4;_LLE3: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1D5=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1D5->tag != 6)goto _LLE5;}_LLE4:
 goto _LLE6;_LLE5: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1D6=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1B3;if(_tmp1D6->tag != 16)goto _LLC2;}_LLE6: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4BE;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4BD;s=(void*)((_tmp4BD=_region_malloc(r,sizeof(*_tmp4BD)),((_tmp4BD[0]=((_tmp4BE.tag=0,_tmp4BE)),_tmp4BD))));}_LLC2:;}
# 898
return s;}
# 906
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 917
typedef void*Cyc_Tcpat_term_desc_t;
# 921
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 928
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 937
typedef void*Cyc_Tcpat_decision_t;struct _tuple18{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 942
typedef struct _tuple18*Cyc_Tcpat_work_stack_frame_t;
# 945
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 947
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 950
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
const char*_tmp4C1;void*_tmp4C0;(_tmp4C0=0,Cyc_fprintf(Cyc_stderr,((_tmp4C1=" ",_tag_dyneither(_tmp4C1,sizeof(char),2))),_tag_dyneither(_tmp4C0,sizeof(void*),0)));}}
# 956
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp209=c->name;
union Cyc_Tcpat_Name_value _tmp20A=_tmp209;struct _dyneither_ptr _tmp20B;int _tmp20C;_LL10A: if((_tmp20A.Name_v).tag != 1)goto _LL10C;_tmp20B=(struct _dyneither_ptr)(_tmp20A.Name_v).val;_LL10B:
{const char*_tmp4C5;void*_tmp4C4[1];struct Cyc_String_pa_PrintArg_struct _tmp4C3;(_tmp4C3.tag=0,((_tmp4C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20B),((_tmp4C4[0]=& _tmp4C3,Cyc_fprintf(Cyc_stderr,((_tmp4C5="%s",_tag_dyneither(_tmp4C5,sizeof(char),3))),_tag_dyneither(_tmp4C4,sizeof(void*),1)))))));}goto _LL109;_LL10C: if((_tmp20A.Int_v).tag != 2)goto _LL109;_tmp20C=(int)(_tmp20A.Int_v).val;_LL10D:
{const char*_tmp4C9;void*_tmp4C8[1];struct Cyc_Int_pa_PrintArg_struct _tmp4C7;(_tmp4C7.tag=1,((_tmp4C7.f1=(unsigned long)_tmp20C,((_tmp4C8[0]=& _tmp4C7,Cyc_fprintf(Cyc_stderr,((_tmp4C9="%d",_tag_dyneither(_tmp4C9,sizeof(char),3))),_tag_dyneither(_tmp4C8,sizeof(void*),1)))))));}goto _LL109;_LL109:;}
# 964
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp213=d;void*_tmp215;struct Cyc_List_List*_tmp218;struct Cyc_Tcpat_Con_s*_tmp219;void*_tmp21A;void*_tmp21B;_LL10F: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp214=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp213;if(_tmp214->tag != 1)goto _LL111;else{_tmp215=(void*)_tmp214->f1;}}_LL110:
# 967
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4CC;void*_tmp4CB;(_tmp4CB=0,Cyc_fprintf(Cyc_stderr,((_tmp4CC="Success(",_tag_dyneither(_tmp4CC,sizeof(char),9))),_tag_dyneither(_tmp4CB,sizeof(void*),0)));}print_rhs(_tmp215);{const char*_tmp4CF;void*_tmp4CE;(_tmp4CE=0,Cyc_fprintf(Cyc_stderr,((_tmp4CF=")\n",_tag_dyneither(_tmp4CF,sizeof(char),3))),_tag_dyneither(_tmp4CE,sizeof(void*),0)));}
goto _LL10E;_LL111: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp216=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp213;if(_tmp216->tag != 0)goto _LL113;}_LL112:
{const char*_tmp4D2;void*_tmp4D1;(_tmp4D1=0,Cyc_fprintf(Cyc_stderr,((_tmp4D2="Failure\n",_tag_dyneither(_tmp4D2,sizeof(char),9))),_tag_dyneither(_tmp4D1,sizeof(void*),0)));}goto _LL10E;_LL113: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp217=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp213;if(_tmp217->tag != 2)goto _LL10E;else{_tmp218=_tmp217->f1;_tmp219=_tmp217->f2;_tmp21A=(void*)_tmp217->f3;_tmp21B=(void*)_tmp217->f4;}}_LL114:
# 972
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4D5;void*_tmp4D4;(_tmp4D4=0,Cyc_fprintf(Cyc_stderr,((_tmp4D5="Access[",_tag_dyneither(_tmp4D5,sizeof(char),8))),_tag_dyneither(_tmp4D4,sizeof(void*),0)));}
for(0;_tmp218 != 0;_tmp218=_tmp218->tl){
{const char*_tmp4D9;void*_tmp4D8[1];struct Cyc_Int_pa_PrintArg_struct _tmp4D7;(_tmp4D7.tag=1,((_tmp4D7.f1=(unsigned long)((int)_tmp218->hd),((_tmp4D8[0]=& _tmp4D7,Cyc_fprintf(Cyc_stderr,((_tmp4D9="%d",_tag_dyneither(_tmp4D9,sizeof(char),3))),_tag_dyneither(_tmp4D8,sizeof(void*),1)))))));}
if(_tmp218->tl != 0){const char*_tmp4DC;void*_tmp4DB;(_tmp4DB=0,Cyc_fprintf(Cyc_stderr,((_tmp4DC=",",_tag_dyneither(_tmp4DC,sizeof(char),2))),_tag_dyneither(_tmp4DB,sizeof(void*),0)));}}
# 978
{const char*_tmp4DF;void*_tmp4DE;(_tmp4DE=0,Cyc_fprintf(Cyc_stderr,((_tmp4DF="],",_tag_dyneither(_tmp4DF,sizeof(char),3))),_tag_dyneither(_tmp4DE,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp219);
{const char*_tmp4E2;void*_tmp4E1;(_tmp4E1=0,Cyc_fprintf(Cyc_stderr,((_tmp4E2=",\n",_tag_dyneither(_tmp4E2,sizeof(char),3))),_tag_dyneither(_tmp4E1,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp21A,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp21B,print_rhs,tab + 7);_LL10E:;}struct _tuple19{int f1;unsigned int f2;};
# 986
static void Cyc_Tcpat_print_swrhs(struct _tuple19*x){
return;}
# 994
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp22D=td;struct Cyc_Set_Set*_tmp22F;_LL116: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp22E=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp22D;if(_tmp22E->tag != 1)goto _LL118;else{_tmp22F=_tmp22E->f1;}}_LL117: {
# 1004
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp4E5;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp4E4;return(void*)((_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4)),((_tmp4E4[0]=((_tmp4E5.tag=1,((_tmp4E5.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp22F,c),_tmp4E5)))),_tmp4E4))));}_LL118: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp230=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp22D;if(_tmp230->tag != 0)goto _LL115;}_LL119: {
const char*_tmp4E8;void*_tmp4E7;(_tmp4E7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E8="add_neg called when td is Positive",_tag_dyneither(_tmp4E8,sizeof(char),35))),_tag_dyneither(_tmp4E7,sizeof(void*),0)));}_LL115:;}
# 1011
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp235=td;struct Cyc_Tcpat_Con_s*_tmp237;struct Cyc_Set_Set*_tmp239;_LL11B: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp236=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp235;if(_tmp236->tag != 0)goto _LL11D;else{_tmp237=_tmp236->f1;}}_LL11C:
# 1015
 if(Cyc_Tcpat_compare_con(c,_tmp237)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL11D: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp238=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp235;if(_tmp238->tag != 1)goto _LL11A;else{_tmp239=_tmp238->f1;}}_LL11E:
# 1019
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp239,c))return Cyc_Tcpat_No;else{
# 1022
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp239)+ 1)
return Cyc_Tcpat_Yes;else{
# 1025
return Cyc_Tcpat_Maybe;}}_LL11A:;}struct _tuple20{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1033
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1035
struct Cyc_List_List*_tmp23A=ctxt;struct Cyc_Tcpat_Con_s*_tmp23B;struct Cyc_List_List*_tmp23C;struct Cyc_List_List*_tmp23D;_LL120: if(_tmp23A != 0)goto _LL122;_LL121:
 return 0;_LL122: if(_tmp23A == 0)goto _LL11F;_tmp23B=((struct _tuple20*)_tmp23A->hd)->f1;_tmp23C=((struct _tuple20*)_tmp23A->hd)->f2;_tmp23D=_tmp23A->tl;_LL123: {
# 1038
struct _tuple20*_tmp4EE;struct Cyc_List_List*_tmp4ED;struct Cyc_List_List*_tmp4EC;return(_tmp4EC=_region_malloc(r,sizeof(*_tmp4EC)),((_tmp4EC->hd=(
(_tmp4EE=_region_malloc(r,sizeof(*_tmp4EE)),((_tmp4EE->f1=_tmp23B,((_tmp4EE->f2=((_tmp4ED=_region_malloc(r,sizeof(*_tmp4ED)),((_tmp4ED->hd=dsc,((_tmp4ED->tl=_tmp23C,_tmp4ED)))))),_tmp4EE)))))),((_tmp4EC->tl=_tmp23D,_tmp4EC)))));}_LL11F:;}
# 1046
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp241=ctxt;struct Cyc_Tcpat_Con_s*_tmp242;struct Cyc_List_List*_tmp243;struct Cyc_List_List*_tmp244;_LL125: if(_tmp241 != 0)goto _LL127;_LL126: {
const char*_tmp4F1;void*_tmp4F0;(_tmp4F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4F1="norm_context: empty context",_tag_dyneither(_tmp4F1,sizeof(char),28))),_tag_dyneither(_tmp4F0,sizeof(void*),0)));}_LL127: if(_tmp241 == 0)goto _LL124;_tmp242=((struct _tuple20*)_tmp241->hd)->f1;_tmp243=((struct _tuple20*)_tmp241->hd)->f2;_tmp244=_tmp241->tl;_LL128: {
# 1050
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp4F4;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp4F3;return Cyc_Tcpat_augment(r,_tmp244,(void*)((_tmp4F3=_region_malloc(r,sizeof(*_tmp4F3)),((_tmp4F3[0]=((_tmp4F4.tag=0,((_tmp4F4.f1=_tmp242,((_tmp4F4.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp243),_tmp4F4)))))),_tmp4F3)))));}_LL124:;}
# 1059
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1061
struct _tuple13 _tmp4F5;struct _tuple13 _tmp24A=(_tmp4F5.f1=ctxt,((_tmp4F5.f2=work,_tmp4F5)));struct Cyc_Tcpat_Con_s*_tmp24B;struct Cyc_List_List*_tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_List_List*_tmp24E;struct Cyc_List_List*_tmp24F;_LL12A: if(_tmp24A.f1 != 0)goto _LL12C;if(_tmp24A.f2 != 0)goto _LL12C;_LL12B:
 return dsc;_LL12C: if(_tmp24A.f1 != 0)goto _LL12E;_LL12D:
 goto _LL12F;_LL12E: if(_tmp24A.f2 != 0)goto _LL130;_LL12F: {
const char*_tmp4F8;void*_tmp4F7;(_tmp4F7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4F8="build_desc: ctxt and work don't match",_tag_dyneither(_tmp4F8,sizeof(char),38))),_tag_dyneither(_tmp4F7,sizeof(void*),0)));}_LL130: if(_tmp24A.f1 == 0)goto _LL129;_tmp24B=((struct _tuple20*)(_tmp24A.f1)->hd)->f1;_tmp24C=((struct _tuple20*)(_tmp24A.f1)->hd)->f2;_tmp24D=(_tmp24A.f1)->tl;if(_tmp24A.f2 == 0)goto _LL129;_tmp24E=((struct _tuple18*)(_tmp24A.f2)->hd)->f3;_tmp24F=(_tmp24A.f2)->tl;_LL131: {
# 1066
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp4FE;struct Cyc_List_List*_tmp4FD;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp4FC;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp252=(_tmp4FC=_region_malloc(r,sizeof(*_tmp4FC)),((_tmp4FC[0]=((_tmp4FE.tag=0,((_tmp4FE.f1=_tmp24B,((_tmp4FE.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp24C),(
(_tmp4FD=_region_malloc(r,sizeof(*_tmp4FD)),((_tmp4FD->hd=dsc,((_tmp4FD->tl=_tmp24E,_tmp4FD))))))),_tmp4FE)))))),_tmp4FC)));
return Cyc_Tcpat_build_desc(r,_tmp24D,(void*)_tmp252,_tmp24F);}_LL129:;}
# 1072
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1079
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1081
struct Cyc_List_List*_tmp256=allmrules;void*_tmp257;void*_tmp258;struct Cyc_List_List*_tmp259;_LL133: if(_tmp256 != 0)goto _LL135;_LL134: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp501;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp500;return(void*)((_tmp500=_cycalloc(sizeof(*_tmp500)),((_tmp500[0]=((_tmp501.tag=0,((_tmp501.f1=dsc,_tmp501)))),_tmp500))));}_LL135: if(_tmp256 == 0)goto _LL132;_tmp257=((struct _tuple0*)_tmp256->hd)->f1;_tmp258=((struct _tuple0*)_tmp256->hd)->f2;_tmp259=_tmp256->tl;_LL136:
# 1084
 return Cyc_Tcpat_match(r,_tmp257,0,dsc,0,0,_tmp258,_tmp259);_LL132:;}
# 1089
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1091
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp504;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp503;return Cyc_Tcpat_or_match(r,(void*)((_tmp503=_region_malloc(r,sizeof(*_tmp503)),((_tmp503[0]=((_tmp504.tag=1,((_tmp504.f1=Cyc_Tcpat_empty_con_set(r),_tmp504)))),_tmp503)))),allmrules);}
# 1097
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1101
struct Cyc_List_List*_tmp25E=work;struct Cyc_List_List*_tmp25F;struct Cyc_List_List*_tmp260;struct Cyc_List_List*_tmp261;struct Cyc_List_List*_tmp262;struct Cyc_List_List*_tmp263;_LL138: if(_tmp25E != 0)goto _LL13A;_LL139: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp507;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp506;return(void*)((_tmp506=_region_malloc(r,sizeof(*_tmp506)),((_tmp506[0]=((_tmp507.tag=1,((_tmp507.f1=right_hand_side,_tmp507)))),_tmp506))));}_LL13A: if(_tmp25E == 0)goto _LL13C;if(((struct _tuple18*)_tmp25E->hd)->f1 != 0)goto _LL13C;if(((struct _tuple18*)_tmp25E->hd)->f2 != 0)goto _LL13C;if(((struct _tuple18*)_tmp25E->hd)->f3 != 0)goto _LL13C;_tmp25F=_tmp25E->tl;_LL13B:
# 1104
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp25F,right_hand_side,rules);_LL13C: if(_tmp25E == 0)goto _LL137;_tmp260=((struct _tuple18*)_tmp25E->hd)->f1;_tmp261=((struct _tuple18*)_tmp25E->hd)->f2;_tmp262=((struct _tuple18*)_tmp25E->hd)->f3;_tmp263=_tmp25E->tl;_LL13D:
# 1106
 if((_tmp260 == 0  || _tmp261 == 0) || _tmp262 == 0){
const char*_tmp50A;void*_tmp509;(_tmp509=0,Cyc_Tcutil_impos(((_tmp50A="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp50A,sizeof(char),38))),_tag_dyneither(_tmp509,sizeof(void*),0)));}{
void*_tmp269;struct Cyc_List_List*_tmp26A;struct Cyc_List_List*_tmp268=_tmp260;_tmp269=(void*)_tmp268->hd;_tmp26A=_tmp268->tl;{
struct Cyc_List_List*_tmp26C;struct Cyc_List_List*_tmp26D;struct Cyc_List_List*_tmp26B=_tmp261;_tmp26C=(struct Cyc_List_List*)_tmp26B->hd;_tmp26D=_tmp26B->tl;{
void*_tmp26F;struct Cyc_List_List*_tmp270;struct Cyc_List_List*_tmp26E=_tmp262;_tmp26F=(void*)_tmp26E->hd;_tmp270=_tmp26E->tl;{
struct _tuple18*_tmp50B;struct _tuple18*_tmp271=(_tmp50B=_region_malloc(r,sizeof(*_tmp50B)),((_tmp50B->f1=_tmp26A,((_tmp50B->f2=_tmp26D,((_tmp50B->f3=_tmp270,_tmp50B)))))));
struct Cyc_List_List*_tmp50C;return Cyc_Tcpat_match(r,_tmp269,_tmp26C,_tmp26F,ctx,((_tmp50C=_region_malloc(r,sizeof(*_tmp50C)),((_tmp50C->hd=_tmp271,((_tmp50C->tl=_tmp263,_tmp50C)))))),right_hand_side,rules);};};};};_LL137:;}
# 1118
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1120
void*_tmp274=dsc;struct Cyc_Set_Set*_tmp276;struct Cyc_List_List*_tmp278;_LL13F: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp275=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp274;if(_tmp275->tag != 1)goto _LL141;else{_tmp276=_tmp275->f1;}}_LL140: {
# 1125
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp50F;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp50E;void*any=(void*)((_tmp50E=_region_malloc(r,sizeof(*_tmp50E)),((_tmp50E[0]=((_tmp50F.tag=1,((_tmp50F.f1=Cyc_Tcpat_empty_con_set(r),_tmp50F)))),_tmp50E))));
struct Cyc_List_List*_tmp279=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp510;_tmp279=((_tmp510=_region_malloc(r,sizeof(*_tmp510)),((_tmp510->hd=any,((_tmp510->tl=_tmp279,_tmp510))))));}}
return _tmp279;}_LL141: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp277=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp274;if(_tmp277->tag != 0)goto _LL13E;else{_tmp278=_tmp277->f2;}}_LL142:
 return _tmp278;_LL13E:;}struct _tuple21{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1135
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple21*env,int i){
struct _RegionHandle*_tmp27E;struct Cyc_List_List*_tmp27F;struct _tuple21*_tmp27D=env;_tmp27E=_tmp27D->f1;_tmp27F=_tmp27D->f2;{
struct Cyc_List_List*_tmp511;return(_tmp511=_region_malloc(_tmp27E,sizeof(*_tmp511)),((_tmp511->hd=(void*)(i + 1),((_tmp511->tl=_tmp27F,_tmp511)))));};}
# 1139
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1141
struct _tuple21 _tmp512;struct _tuple21 _tmp281=(_tmp512.f1=r,((_tmp512.f2=obj,_tmp512)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple21*,int),struct _tuple21*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp281);}
# 1148
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1152
void*_tmp283=p;struct Cyc_Tcpat_Con_s*_tmp286;struct Cyc_List_List*_tmp287;_LL144: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp284=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp283;if(_tmp284->tag != 0)goto _LL146;}_LL145:
# 1154
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL146: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp285=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp283;if(_tmp285->tag != 1)goto _LL143;else{_tmp286=_tmp285->f1;_tmp287=_tmp285->f2;}}_LL147:
# 1156
 switch(Cyc_Tcpat_static_match(_tmp286,dsc)){case Cyc_Tcpat_Yes: _LL148: {
# 1158
struct _tuple20*_tmp515;struct Cyc_List_List*_tmp514;struct Cyc_List_List*ctx2=(_tmp514=_region_malloc(r,sizeof(*_tmp514)),((_tmp514->hd=((_tmp515=_region_malloc(r,sizeof(*_tmp515)),((_tmp515->f1=_tmp286,((_tmp515->f2=0,_tmp515)))))),((_tmp514->tl=ctx,_tmp514)))));
struct _tuple18*_tmp516;struct _tuple18*work_frame=(_tmp516=_region_malloc(r,sizeof(*_tmp516)),((_tmp516->f1=_tmp287,((_tmp516->f2=Cyc_Tcpat_getoargs(r,_tmp286,obj),((_tmp516->f3=
Cyc_Tcpat_getdargs(r,_tmp286,dsc),_tmp516)))))));
struct Cyc_List_List*_tmp517;struct Cyc_List_List*work2=(_tmp517=_region_malloc(r,sizeof(*_tmp517)),((_tmp517->hd=work_frame,((_tmp517->tl=work,_tmp517)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL149:
# 1164
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL14A: {
# 1166
struct _tuple20*_tmp51A;struct Cyc_List_List*_tmp519;struct Cyc_List_List*ctx2=(_tmp519=_region_malloc(r,sizeof(*_tmp519)),((_tmp519->hd=((_tmp51A=_region_malloc(r,sizeof(*_tmp51A)),((_tmp51A->f1=_tmp286,((_tmp51A->f2=0,_tmp51A)))))),((_tmp519->tl=ctx,_tmp519)))));
struct _tuple18*_tmp51B;struct _tuple18*work_frame=(_tmp51B=_region_malloc(r,sizeof(*_tmp51B)),((_tmp51B->f1=_tmp287,((_tmp51B->f2=Cyc_Tcpat_getoargs(r,_tmp286,obj),((_tmp51B->f3=
Cyc_Tcpat_getdargs(r,_tmp286,dsc),_tmp51B)))))));
struct Cyc_List_List*_tmp51C;struct Cyc_List_List*work2=(_tmp51C=_region_malloc(r,sizeof(*_tmp51C)),((_tmp51C->hd=work_frame,((_tmp51C->tl=work,_tmp51C)))));
void*_tmp28C=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp28D=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp286),work),rules);
# 1173
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp51F;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp51E;return(void*)((_tmp51E=_region_malloc(r,sizeof(*_tmp51E)),((_tmp51E[0]=((_tmp51F.tag=2,((_tmp51F.f1=obj,((_tmp51F.f2=_tmp286,((_tmp51F.f3=_tmp28C,((_tmp51F.f4=_tmp28D,_tmp51F)))))))))),_tmp51E))));}}_LL143:;}
# 1183
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1189
void*_tmp294=d;void*_tmp296;void*_tmp298;void*_tmp29A;void*_tmp29B;_LL14D: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp295=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp294;if(_tmp295->tag != 0)goto _LL14F;else{_tmp296=(void*)_tmp295->f1;}}_LL14E:
# 1191
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp296);*exhaust_done=1;}
goto _LL14C;_LL14F: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp297=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp294;if(_tmp297->tag != 1)goto _LL151;else{_tmp298=(void*)_tmp297->f1;}}_LL150:
 rhs_appears(env2,_tmp298);goto _LL14C;_LL151: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp299=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp294;if(_tmp299->tag != 2)goto _LL14C;else{_tmp29A=(void*)_tmp299->f3;_tmp29B=(void*)_tmp299->f4;}}_LL152:
# 1195
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp29A,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1197
Cyc_Tcpat_check_exhaust_overlap(r,_tmp29B,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1199
goto _LL14C;_LL14C:;}struct _tuple22{void*f1;struct _tuple19*f2;};struct _tuple23{struct _RegionHandle*f1;int*f2;};
# 1207
static struct _tuple22*Cyc_Tcpat_get_match(struct _tuple23*env,struct Cyc_Absyn_Switch_clause*swc){
# 1210
struct _RegionHandle*_tmp29D;int*_tmp29E;struct _tuple23*_tmp29C=env;_tmp29D=_tmp29C->f1;_tmp29E=_tmp29C->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp29D,swc->pattern);
struct _tuple19*_tmp520;struct _tuple19*rhs=(_tmp520=_region_malloc(_tmp29D,sizeof(*_tmp520)),((_tmp520->f1=0,((_tmp520->f2=(swc->pattern)->loc,_tmp520)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp523;struct Cyc_List_List*_tmp522;sp=Cyc_Tcpat_tuple_pat(_tmp29D,((_tmp522=_region_malloc(_tmp29D,sizeof(*_tmp522)),((_tmp522->hd=sp0,((_tmp522->tl=(
(_tmp523=_region_malloc(_tmp29D,sizeof(*_tmp523)),((_tmp523->hd=Cyc_Tcpat_int_pat(_tmp29D,*_tmp29E,0),((_tmp523->tl=0,_tmp523)))))),_tmp522)))))),0);}
*_tmp29E=*_tmp29E + 1;}else{
# 1219
struct Cyc_List_List*_tmp52D;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp52C;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp52B;struct Cyc_List_List*_tmp52A;sp=Cyc_Tcpat_tuple_pat(_tmp29D,((_tmp52A=_region_malloc(_tmp29D,sizeof(*_tmp52A)),((_tmp52A->hd=sp0,((_tmp52A->tl=((_tmp52D=_region_malloc(_tmp29D,sizeof(*_tmp52D)),((_tmp52D->hd=(void*)((_tmp52B=_region_malloc(_tmp29D,sizeof(*_tmp52B)),((_tmp52B[0]=((_tmp52C.tag=0,_tmp52C)),_tmp52B)))),((_tmp52D->tl=0,_tmp52D)))))),_tmp52A)))))),0);}{
struct _tuple22*_tmp52E;return(_tmp52E=_region_malloc(_tmp29D,sizeof(*_tmp52E)),((_tmp52E->f1=sp,((_tmp52E->f2=rhs,_tmp52E)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1225
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1227
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp2A8=d;struct Cyc_Tcpat_Con_s*_tmp2AA;struct Cyc_List_List*_tmp2AB;struct Cyc_Set_Set*_tmp2AD;_LL154: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2A9=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2A8;if(_tmp2A9->tag != 0)goto _LL156;else{_tmp2AA=_tmp2A9->f1;_tmp2AB=_tmp2A9->f2;}}_LL155: {
# 1231
union Cyc_Tcpat_Name_value _tmp2AE=_tmp2AA->name;
struct Cyc_Absyn_Pat*_tmp2AF=_tmp2AA->orig_pat;
if(_tmp2AF == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp2AB))->hd);{
# 1236
void*_tmp2B0=_tmp2AF->r;struct Cyc_Absyn_Vardecl*_tmp2B3;struct Cyc_Absyn_Vardecl*_tmp2B5;struct Cyc_Absyn_Tvar*_tmp2B7;struct Cyc_Absyn_Vardecl*_tmp2B8;struct Cyc_Absyn_Aggrdecl*_tmp2BC;struct Cyc_List_List*_tmp2BD;struct Cyc_Absyn_Datatypefield*_tmp2BF;int _tmp2C2;char _tmp2C4;struct _dyneither_ptr _tmp2C6;int _tmp2C7;struct Cyc_Absyn_Enumfield*_tmp2C9;struct Cyc_Absyn_Enumfield*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CD;_LL159: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2B1=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2B1->tag != 0)goto _LL15B;}_LL15A: {
const char*_tmp52F;return(_tmp52F="_",_tag_dyneither(_tmp52F,sizeof(char),2));}_LL15B: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2B2=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2B2->tag != 1)goto _LL15D;else{_tmp2B3=_tmp2B2->f1;}}_LL15C:
 return Cyc_Absynpp_qvar2string(_tmp2B3->name);_LL15D: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2B4=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2B4->tag != 2)goto _LL15F;else{_tmp2B5=_tmp2B4->f1;}}_LL15E: {
const char*_tmp533;void*_tmp532[1];struct Cyc_String_pa_PrintArg_struct _tmp531;return(struct _dyneither_ptr)((_tmp531.tag=0,((_tmp531.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B5->name)),((_tmp532[0]=& _tmp531,Cyc_aprintf(((_tmp533="*%s",_tag_dyneither(_tmp533,sizeof(char),4))),_tag_dyneither(_tmp532,sizeof(void*),1))))))));}_LL15F: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp2B6=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2B6->tag != 3)goto _LL161;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=_tmp2B6->f2;}}_LL160: {
const char*_tmp538;void*_tmp537[2];struct Cyc_String_pa_PrintArg_struct _tmp536;struct Cyc_String_pa_PrintArg_struct _tmp535;return(struct _dyneither_ptr)((_tmp535.tag=0,((_tmp535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B7->name),((_tmp536.tag=0,((_tmp536.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B8->name)),((_tmp537[0]=& _tmp536,((_tmp537[1]=& _tmp535,Cyc_aprintf(((_tmp538="%s<`%s>",_tag_dyneither(_tmp538,sizeof(char),8))),_tag_dyneither(_tmp537,sizeof(void*),2))))))))))))));}_LL161: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp2B9=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2B9->tag != 4)goto _LL163;}_LL162: {
# 1242
const char*_tmp53C;void*_tmp53B[1];struct Cyc_String_pa_PrintArg_struct _tmp53A;return(struct _dyneither_ptr)((_tmp53A.tag=0,((_tmp53A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2AB)),((_tmp53B[0]=& _tmp53A,Cyc_aprintf(((_tmp53C="$(%s)",_tag_dyneither(_tmp53C,sizeof(char),6))),_tag_dyneither(_tmp53B,sizeof(void*),1))))))));}_LL163: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp2BA=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2BA->tag != 5)goto _LL165;}_LL164: {
# 1244
const char*_tmp540;void*_tmp53F[1];struct Cyc_String_pa_PrintArg_struct _tmp53E;return(struct _dyneither_ptr)((_tmp53E.tag=0,((_tmp53E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2AB)),((_tmp53F[0]=& _tmp53E,Cyc_aprintf(((_tmp540="&%s",_tag_dyneither(_tmp540,sizeof(char),4))),_tag_dyneither(_tmp53F,sizeof(void*),1))))))));}_LL165: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp2BB=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2BB->tag != 6)goto _LL167;else{if(_tmp2BB->f1 == 0)goto _LL167;if((((_tmp2BB->f1)->aggr_info).KnownAggr).tag != 2)goto _LL167;_tmp2BC=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp2BB->f1)->aggr_info).KnownAggr).val);_tmp2BD=_tmp2BB->f3;}}_LL166:
# 1246
 if(_tmp2BC->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp2DC=_tmp2BD;struct _dyneither_ptr*_tmp2DE;_LL17A: if(_tmp2DC == 0)goto _LL17C;if(((struct _tuple15*)_tmp2DC->hd)->f1 == 0)goto _LL17C;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp2DD=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp2DC->hd)->f1)->hd);if(_tmp2DD->tag != 1)goto _LL17C;else{_tmp2DE=_tmp2DD->f1;}};_LL17B: {
# 1249
const char*_tmp546;void*_tmp545[3];struct Cyc_String_pa_PrintArg_struct _tmp544;struct Cyc_String_pa_PrintArg_struct _tmp543;struct Cyc_String_pa_PrintArg_struct _tmp542;return(struct _dyneither_ptr)((_tmp542.tag=0,((_tmp542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp2AB)),((_tmp543.tag=0,((_tmp543.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2DE),((_tmp544.tag=0,((_tmp544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1249
Cyc_Absynpp_qvar2string(_tmp2BC->name)),((_tmp545[0]=& _tmp544,((_tmp545[1]=& _tmp543,((_tmp545[2]=& _tmp542,Cyc_aprintf(((_tmp546="%s{.%s = %s}",_tag_dyneither(_tmp546,sizeof(char),13))),_tag_dyneither(_tmp545,sizeof(void*),3))))))))))))))))))));}_LL17C:;_LL17D:
# 1251
 goto _LL179;_LL179:;}{
# 1254
const char*_tmp54B;void*_tmp54A[2];struct Cyc_String_pa_PrintArg_struct _tmp549;struct Cyc_String_pa_PrintArg_struct _tmp548;return(struct _dyneither_ptr)((_tmp548.tag=0,((_tmp548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2AB)),((_tmp549.tag=0,((_tmp549.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2BC->name)),((_tmp54A[0]=& _tmp549,((_tmp54A[1]=& _tmp548,Cyc_aprintf(((_tmp54B="%s{%s}",_tag_dyneither(_tmp54B,sizeof(char),7))),_tag_dyneither(_tmp54A,sizeof(void*),2))))))))))))));};_LL167: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp2BE=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2BE->tag != 7)goto _LL169;else{_tmp2BF=_tmp2BE->f2;}}_LL168:
# 1256
 if(_tmp2AB == 0){
const char*_tmp54F;void*_tmp54E[1];struct Cyc_String_pa_PrintArg_struct _tmp54D;return(struct _dyneither_ptr)((_tmp54D.tag=0,((_tmp54D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2BF->name)),((_tmp54E[0]=& _tmp54D,Cyc_aprintf(((_tmp54F="%s",_tag_dyneither(_tmp54F,sizeof(char),3))),_tag_dyneither(_tmp54E,sizeof(void*),1))))))));}else{
# 1259
const char*_tmp554;void*_tmp553[2];struct Cyc_String_pa_PrintArg_struct _tmp552;struct Cyc_String_pa_PrintArg_struct _tmp551;return(struct _dyneither_ptr)((_tmp551.tag=0,((_tmp551.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2AB)),((_tmp552.tag=0,((_tmp552.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2BF->name)),((_tmp553[0]=& _tmp552,((_tmp553[1]=& _tmp551,Cyc_aprintf(((_tmp554="%s(%s)",_tag_dyneither(_tmp554,sizeof(char),7))),_tag_dyneither(_tmp553,sizeof(void*),2))))))))))))));}_LL169: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp2C0=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2C0->tag != 8)goto _LL16B;}_LL16A: {
const char*_tmp555;return(_tmp555="NULL",_tag_dyneither(_tmp555,sizeof(char),5));}_LL16B: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2C1=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2C1->tag != 9)goto _LL16D;else{_tmp2C2=_tmp2C1->f2;}}_LL16C: {
const char*_tmp559;void*_tmp558[1];struct Cyc_Int_pa_PrintArg_struct _tmp557;return(struct _dyneither_ptr)((_tmp557.tag=1,((_tmp557.f1=(unsigned long)_tmp2C2,((_tmp558[0]=& _tmp557,Cyc_aprintf(((_tmp559="%d",_tag_dyneither(_tmp559,sizeof(char),3))),_tag_dyneither(_tmp558,sizeof(void*),1))))))));}_LL16D: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp2C3=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2C3->tag != 10)goto _LL16F;else{_tmp2C4=_tmp2C3->f1;}}_LL16E: {
const char*_tmp55D;void*_tmp55C[1];struct Cyc_Int_pa_PrintArg_struct _tmp55B;return(struct _dyneither_ptr)((_tmp55B.tag=1,((_tmp55B.f1=(unsigned long)((int)_tmp2C4),((_tmp55C[0]=& _tmp55B,Cyc_aprintf(((_tmp55D="%d",_tag_dyneither(_tmp55D,sizeof(char),3))),_tag_dyneither(_tmp55C,sizeof(void*),1))))))));}_LL16F: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp2C5=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2C5->tag != 11)goto _LL171;else{_tmp2C6=_tmp2C5->f1;_tmp2C7=_tmp2C5->f2;}}_LL170:
 return _tmp2C6;_LL171: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp2C8=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2C8->tag != 12)goto _LL173;else{_tmp2C9=_tmp2C8->f2;}}_LL172:
 _tmp2CB=_tmp2C9;goto _LL174;_LL173: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp2CA=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2CA->tag != 13)goto _LL175;else{_tmp2CB=_tmp2CA->f2;}}_LL174:
 return Cyc_Absynpp_qvar2string(_tmp2CB->name);_LL175: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp2CC=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2B0;if(_tmp2CC->tag != 16)goto _LL177;else{_tmp2CD=_tmp2CC->f1;}}_LL176:
 return Cyc_Absynpp_exp2string(_tmp2CD);_LL177:;_LL178:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL158:;};}_LL156: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2AC=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2A8;if(_tmp2AC->tag != 1)goto _LL153;else{_tmp2AD=_tmp2AC->f1;}}_LL157:
# 1270
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2AD)){const char*_tmp55E;return(_tmp55E="_",_tag_dyneither(_tmp55E,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp2F7=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2AD);
struct Cyc_Absyn_Pat*_tmp2F8=_tmp2F7->orig_pat;
# 1275
if(_tmp2F8 == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_tmp2F9=Cyc_Tcutil_compress((void*)_check_null(_tmp2F8->topt));struct Cyc_Absyn_PtrAtts _tmp2FD;struct Cyc_Absyn_Datatypedecl*_tmp2FF;struct Cyc_Absyn_Aggrdecl*_tmp301;_LL17F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2F9;if(_tmp2FA->tag != 6)goto _LL181;else{if(_tmp2FA->f2 != Cyc_Absyn_Char_sz)goto _LL181;}}_LL180:
# 1279
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp302=Cyc_Tcpat_char_con(r,(char)i,_tmp2F8);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2AD,_tmp302)){
const char*_tmp562;void*_tmp561[1];struct Cyc_Int_pa_PrintArg_struct _tmp560;return(struct _dyneither_ptr)((_tmp560.tag=1,((_tmp560.f1=(unsigned long)i,((_tmp561[0]=& _tmp560,Cyc_aprintf(((_tmp562="%d",_tag_dyneither(_tmp562,sizeof(char),3))),_tag_dyneither(_tmp561,sizeof(void*),1))))))));}}}
# 1284
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL181: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2F9;if(_tmp2FB->tag != 6)goto _LL183;}_LL182:
# 1287
{unsigned int i=0;for(0;i < 0 - 1;++ i){
struct Cyc_Tcpat_Con_s*_tmp306=Cyc_Tcpat_int_con(r,(int)i,_tmp2F8);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2AD,_tmp306)){
const char*_tmp566;void*_tmp565[1];struct Cyc_Int_pa_PrintArg_struct _tmp564;return(struct _dyneither_ptr)((_tmp564.tag=1,((_tmp564.f1=(unsigned long)((int)i),((_tmp565[0]=& _tmp564,Cyc_aprintf(((_tmp566="%d",_tag_dyneither(_tmp566,sizeof(char),3))),_tag_dyneither(_tmp565,sizeof(void*),1))))))));}}}
# 1292
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL183: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F9;if(_tmp2FC->tag != 5)goto _LL185;else{_tmp2FD=(_tmp2FC->f1).ptr_atts;}}_LL184: {
# 1294
union Cyc_Absyn_Constraint*_tmp30A=_tmp2FD.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp30A);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2AD,Cyc_Tcpat_null_con(r,_tmp2F8))){
const char*_tmp567;return(_tmp567="NULL",_tag_dyneither(_tmp567,sizeof(char),5));}}{
# 1300
const char*_tmp568;return(_tmp568="&_",_tag_dyneither(_tmp568,sizeof(char),3));};}_LL185: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2FE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2F9;if(_tmp2FE->tag != 3)goto _LL187;else{if((((_tmp2FE->f1).datatype_info).KnownDatatype).tag != 2)goto _LL187;_tmp2FF=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp2FE->f1).datatype_info).KnownDatatype).val);}}_LL186:
# 1302
 if(_tmp2FF->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp30D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2FF->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30D);
for(0;(unsigned int)_tmp30D;_tmp30D=_tmp30D->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp30D->hd)->name).f2;
struct Cyc_List_List*_tmp30E=((struct Cyc_Absyn_Datatypefield*)_tmp30D->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp569;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2AD,((_tmp569=_cycalloc(sizeof(*_tmp569)),((_tmp569->name=Cyc_Tcpat_Name_v(n),((_tmp569->arity=0,((_tmp569->span=0,((_tmp569->orig_pat=_tmp2F8,_tmp569)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30E)== 0)
return n;else{
# 1312
const char*_tmp56D;void*_tmp56C[1];struct Cyc_String_pa_PrintArg_struct _tmp56B;return(struct _dyneither_ptr)((_tmp56B.tag=0,((_tmp56B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp56C[0]=& _tmp56B,Cyc_aprintf(((_tmp56D="%s(...)",_tag_dyneither(_tmp56D,sizeof(char),8))),_tag_dyneither(_tmp56C,sizeof(void*),1))))))));}}}
# 1315
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL187:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F9;if(_tmp300->tag != 11)goto _LL189;else{if((((_tmp300->f1).aggr_info).KnownAggr).tag != 2)goto _LL189;_tmp301=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp300->f1).aggr_info).KnownAggr).val);}}if(!(_tmp301->kind == Cyc_Absyn_UnionA))goto _LL189;_LL188: {
# 1317
struct Cyc_List_List*_tmp313=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp301->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp313);
struct _dyneither_ptr _tmp315;struct _tuple1*_tmp314=_tmp301->name;_tmp315=*_tmp314->f2;
for(0;(unsigned int)_tmp313;_tmp313=_tmp313->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp313->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp56E;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2AD,((_tmp56E=_cycalloc(sizeof(*_tmp56E)),((_tmp56E->name=Cyc_Tcpat_Name_v(n),((_tmp56E->arity=0,((_tmp56E->span=0,((_tmp56E->orig_pat=_tmp2F8,_tmp56E)))))))))))){
const char*_tmp573;void*_tmp572[2];struct Cyc_String_pa_PrintArg_struct _tmp571;struct Cyc_String_pa_PrintArg_struct _tmp570;return(struct _dyneither_ptr)((_tmp570.tag=0,((_tmp570.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp571.tag=0,((_tmp571.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp315),((_tmp572[0]=& _tmp571,((_tmp572[1]=& _tmp570,Cyc_aprintf(((_tmp573="%s{.%s = _}",_tag_dyneither(_tmp573,sizeof(char),12))),_tag_dyneither(_tmp572,sizeof(void*),2))))))))))))));}}
# 1325
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL189:;_LL18A:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL17E:;};};_LL153:;}
# 1330
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp574;return(_tmp574=_cycalloc(sizeof(*_tmp574)),((_tmp574[0]=Cyc_Tcpat_desc2string(r,d),_tmp574)));}
# 1333
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp31C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp31D=_tmp31C;for(0;_tmp31D != 0;_tmp31D=((struct Cyc_List_List*)_check_null(_tmp31D))->tl){
if(_tmp31D->tl != 0){
{struct Cyc_List_List*_tmp575;_tmp31D->tl=((_tmp575=_cycalloc(sizeof(*_tmp575)),((_tmp575->hd=comma,((_tmp575->tl=_tmp31D->tl,_tmp575))))));}
_tmp31D=_tmp31D->tl;}}}
# 1342
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp31C);}
# 1345
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp320;_push_handler(& _tmp320);{int _tmp322=0;if(setjmp(_tmp320.handler))_tmp322=1;if(!_tmp322){
{struct _dyneither_ptr _tmp323=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp579;void*_tmp578[1];struct Cyc_String_pa_PrintArg_struct _tmp577;(_tmp577.tag=0,((_tmp577.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp323),((_tmp578[0]=& _tmp577,Cyc_Tcutil_terr(loc,((_tmp579="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp579,sizeof(char),53))),_tag_dyneither(_tmp578,sizeof(void*),1)))))));}
# 1347
;_pop_handler();}else{void*_tmp321=(void*)_exn_thrown;void*_tmp328=_tmp321;void*_tmp32A;_LL18C: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp329=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp328;if(_tmp329->tag != Cyc_Tcpat_Desc2string)goto _LL18E;}_LL18D:
# 1351
{const char*_tmp57C;void*_tmp57B;(_tmp57B=0,Cyc_Tcutil_terr(loc,((_tmp57C="patterns may not be exhaustive.",_tag_dyneither(_tmp57C,sizeof(char),32))),_tag_dyneither(_tmp57B,sizeof(void*),0)));}
goto _LL18B;_LL18E: _tmp32A=_tmp328;_LL18F:(void)_throw(_tmp32A);_LL18B:;}};}
# 1355
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple19*rhs){
(*rhs).f1=1;}
# 1359
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1364
struct _RegionHandle _tmp32D=_new_region("r");struct _RegionHandle*r=& _tmp32D;_push_region(r);
{int _tmp32E=0;
struct _tuple23 _tmp57D;struct _tuple23 _tmp32F=(_tmp57D.f1=r,((_tmp57D.f2=& _tmp32E,_tmp57D)));
struct Cyc_List_List*_tmp330=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple22*(*f)(struct _tuple23*,struct Cyc_Absyn_Switch_clause*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp32F,swcs);
void*_tmp331=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp330);
int _tmp332=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple19*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp331,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp332);
# 1372
for(0;_tmp330 != 0;_tmp330=_tmp330->tl){
int _tmp334;unsigned int _tmp335;struct _tuple22*_tmp333=(struct _tuple22*)_tmp330->hd;_tmp334=(_tmp333->f2)->f1;_tmp335=(_tmp333->f2)->f2;
if(!_tmp334){
{const char*_tmp580;void*_tmp57F;(_tmp57F=0,Cyc_Tcutil_terr(_tmp335,((_tmp580="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp580,sizeof(char),74))),_tag_dyneither(_tmp57F,sizeof(void*),0)));}
# 1377
break;}}}
# 1365
;_pop_region(r);}
# 1387
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp339;_push_handler(& _tmp339);{int _tmp33B=0;if(setjmp(_tmp339.handler))_tmp33B=1;if(!_tmp33B){
{struct _dyneither_ptr _tmp33C=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp584;void*_tmp583[1];struct Cyc_String_pa_PrintArg_struct _tmp582;(_tmp582.tag=0,((_tmp582.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33C),((_tmp583[0]=& _tmp582,Cyc_Tcutil_warn((*pr).f1,((_tmp584="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp584,sizeof(char),45))),_tag_dyneither(_tmp583,sizeof(void*),1)))))));}
# 1390
;_pop_handler();}else{void*_tmp33A=(void*)_exn_thrown;void*_tmp341=_tmp33A;void*_tmp343;_LL191: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp342=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp341;if(_tmp342->tag != Cyc_Tcpat_Desc2string)goto _LL193;}_LL192:
# 1394
{const char*_tmp587;void*_tmp586;(_tmp586=0,Cyc_Tcutil_warn((*pr).f1,((_tmp587="pattern not exhaustive.",_tag_dyneither(_tmp587,sizeof(char),24))),_tag_dyneither(_tmp586,sizeof(void*),0)));}
goto _LL190;_LL193: _tmp343=_tmp341;_LL194:(void)_throw(_tmp343);_LL190:;}};};}
# 1398
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple24{void*f1;int f2;};
# 1401
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp346=_new_region("r");struct _RegionHandle*r=& _tmp346;_push_region(r);
{struct _tuple24*_tmp58A;struct Cyc_List_List*_tmp589;struct Cyc_List_List*_tmp347=(_tmp589=_region_malloc(r,sizeof(*_tmp589)),((_tmp589->hd=((_tmp58A=_region_malloc(r,sizeof(*_tmp58A)),((_tmp58A->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp58A->f2=0,_tmp58A)))))),((_tmp589->tl=0,_tmp589)))));
void*_tmp348=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp347);
struct _tuple14 _tmp58B;struct _tuple14 _tmp349=(_tmp58B.f1=loc,((_tmp58B.f2=1,_tmp58B)));
int _tmp34A=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp348,Cyc_Tcpat_not_exhaust_warn,& _tmp349,Cyc_Tcpat_dummy_fn,0,& _tmp34A);{
# 1409
int _tmp34B=_tmp349.f2;_npop_handler(0);return _tmp34B;};}
# 1403
;_pop_region(r);}
# 1417
static struct _tuple22*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1419
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple19*_tmp58C;struct _tuple19*rhs=(_tmp58C=_region_malloc(r,sizeof(*_tmp58C)),((_tmp58C->f1=0,((_tmp58C->f2=(swc->pattern)->loc,_tmp58C)))));
struct _tuple22*_tmp58D;return(_tmp58D=_region_malloc(r,sizeof(*_tmp58D)),((_tmp58D->f1=sp0,((_tmp58D->f2=rhs,_tmp58D)))));}
# 1423
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1426
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1428
struct _RegionHandle _tmp351=_new_region("r");struct _RegionHandle*r=& _tmp351;_push_region(r);
{struct Cyc_List_List*_tmp352=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple22*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp353=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp352);
int _tmp354=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple19*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp353,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp354);
# 1434
for(0;_tmp352 != 0;_tmp352=_tmp352->tl){
int _tmp356;unsigned int _tmp357;struct _tuple22*_tmp355=(struct _tuple22*)_tmp352->hd;_tmp356=(_tmp355->f2)->f1;_tmp357=(_tmp355->f2)->f2;
if(!_tmp356){
{const char*_tmp590;void*_tmp58F;(_tmp58F=0,Cyc_Tcutil_terr(_tmp357,((_tmp590="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp590,sizeof(char),74))),_tag_dyneither(_tmp58F,sizeof(void*),0)));}
break;}}}
# 1429
;_pop_region(r);}
