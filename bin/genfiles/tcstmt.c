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
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
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
# 957
void*Cyc_Absyn_exn_typ();
# 979
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 995
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1152
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1174
extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 117 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 131
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 133
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 141
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 148
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);
# 170
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 174
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 178
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 181
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 184
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 192
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 194
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 196
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 198
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
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
# 67
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 76
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 99
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 134
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 148
void Cyc_Tcutil_explain_failure();
# 150
int Cyc_Tcutil_unify(void*,void*);
# 153
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 168
struct _tuple11*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 206 "tcutil.h"
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 214
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 265
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 292
int Cyc_Tcutil_new_tvar_id();
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 327
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 331
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct _tuple12{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple12*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct _tuple13{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple13*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 38 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 42
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple14{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple14 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple15{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple15 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 163 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 182
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 261 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 57 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;struct Cyc_Absyn_Exp*_tmp2;struct Cyc_Absyn_Exp*_tmp4;_LL1: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp1=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1->tag != 4)goto _LL3;else{_tmp2=_tmp1->f1;if(_tmp1->f2 != Cyc_Absyn_PostInc)goto _LL3;}}_LL2:
# 60
{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp16A;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp169;e->r=(void*)((_tmp169=_cycalloc(sizeof(*_tmp169)),((_tmp169[0]=((_tmp16A.tag=4,((_tmp16A.f1=_tmp2,((_tmp16A.f2=Cyc_Absyn_PreInc,_tmp16A)))))),_tmp169))));}goto _LL0;_LL3: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp3->tag != 4)goto _LL5;else{_tmp4=_tmp3->f1;if(_tmp3->f2 != Cyc_Absyn_PostDec)goto _LL5;}}_LL4:
# 62
{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp16D;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp16C;e->r=(void*)((_tmp16C=_cycalloc(sizeof(*_tmp16C)),((_tmp16C[0]=((_tmp16D.tag=4,((_tmp16D.f1=_tmp4,((_tmp16D.f2=Cyc_Absyn_PreDec,_tmp16D)))))),_tmp16C))));}goto _LL0;_LL5:;_LL6:
 goto _LL0;_LL0:;}
# 67
static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
s->try_depth=Cyc_Tcenv_get_try_depth(te);}
# 73
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 75
struct _tuple13*_tmpA;struct Cyc_List_List*_tmpB;struct Cyc_Tcpat_TcPatResult _tmp9=pat_res;_tmpA=_tmp9.tvars_and_bounds_opt;_tmpB=_tmp9.patvars;{
struct Cyc_List_List*_tmpC=_tmpA == 0?0:(*_tmpA).f1;
struct Cyc_List_List*_tmpD=_tmpA == 0?0:(*_tmpA).f2;
struct _RegionHandle _tmpE=_new_region("r");struct _RegionHandle*r=& _tmpE;_push_region(r);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,loc,te,_tmpC);
te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmpD,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(r,loc,te2);{
void*_tmpF=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp10=_tmpB;for(0;_tmp10 != 0;_tmp10=_tmp10->tl){
te2=Cyc_Tcenv_add_pat_var(r,loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp10->hd);
((struct Cyc_Absyn_Vardecl*)_tmp10->hd)->rgn=_tmpF;}}
# 88
if(where_opt != 0){
const char*_tmp16E;Cyc_Tcexp_tcTest(te2,where_opt,((_tmp16E="switch clause guard",_tag_dyneither(_tmp16E,sizeof(char),20))));}
# 91
if(_tmpB != 0)
te2=Cyc_Tcenv_set_encloser(r,te2,s);
Cyc_Tcstmt_tcStmt(te2,s,0);};}
# 96
if(_tmpB != 0)
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 78
;_pop_region(r);};}
# 100
static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Tcenv_CList*const x){
# 102
if(x == (const struct Cyc_Tcenv_CList*)0)return 0;else{
struct Cyc_List_List*_tmp16F;return(_tmp16F=_region_malloc(r,sizeof(*_tmp16F)),((_tmp16F->hd=f(env,x->hd),((_tmp16F->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl),_tmp16F)))));}}
# 109
static void Cyc_Tcstmt_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type){
# 111
struct _RegionHandle _tmp13=_new_region("temp");struct _RegionHandle*temp=& _tmp13;_push_region(temp);
{struct _tuple11*_tmp172;struct Cyc_List_List*_tmp171;struct Cyc_List_List*_tmp14=(_tmp171=_region_malloc(temp,sizeof(*_tmp171)),((_tmp171->hd=((_tmp172=_region_malloc(temp,sizeof(*_tmp172)),((_tmp172->f1=tv,((_tmp172->f2=(void*)& Cyc_Absyn_UniqueRgn_val,_tmp172)))))),((_tmp171->tl=0,_tmp171)))));
void*_tmp15=Cyc_Tcutil_rsubstitute(temp,_tmp14,new_type);
if(Cyc_Tcutil_silent_castable(tenv,loc,old_type,_tmp15)){
struct _tuple12 _tmp173;struct _tuple12 _tmp17=(_tmp173.f1=Cyc_Tcutil_compress(new_type),((_tmp173.f2=Cyc_Tcutil_compress(new_type),_tmp173)));struct Cyc_Absyn_PtrInfo _tmp19;struct Cyc_Absyn_PtrInfo _tmp1B;_LL8:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp18=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17.f1;if(_tmp18->tag != 5)goto _LLA;else{_tmp19=_tmp18->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17.f2;if(_tmp1A->tag != 5)goto _LLA;else{_tmp1B=_tmp1A->f1;}};_LL9:
 goto _LL7;_LLA:;_LLB:
# 118
{const char*_tmp176;void*_tmp175;(_tmp175=0,Cyc_Tcutil_terr(loc,((_tmp176="alias requires pointer type",_tag_dyneither(_tmp176,sizeof(char),28))),_tag_dyneither(_tmp175,sizeof(void*),0)));}goto _LL7;_LL7:;}else{
# 121
{const char*_tmp17B;void*_tmp17A[2];struct Cyc_String_pa_PrintArg_struct _tmp179;struct Cyc_String_pa_PrintArg_struct _tmp178;(_tmp178.tag=0,((_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type)),((_tmp179.tag=0,((_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type)),((_tmp17A[0]=& _tmp179,((_tmp17A[1]=& _tmp178,Cyc_Tcutil_terr(loc,((_tmp17B="cannot alias value of type %s to type %s\n",_tag_dyneither(_tmp17B,sizeof(char),42))),_tag_dyneither(_tmp17A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}}
# 112
;_pop_region(temp);}
# 129
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){
# 132
struct _RegionHandle _tmp24=_new_region("r");struct _RegionHandle*r=& _tmp24;_push_region(r);
{struct _tuple11*tk=0;
struct Cyc_Tcenv_Tenv*_tmp25=Cyc_Tcenv_add_region_equality(r,te,r1,r2,& tk,s0->loc);
Cyc_Tcstmt_tcStmt(_tmp25,s0,new_block);
if(tk != 0)
((*((struct _tuple11*)_check_null(tk))).f1)->kind=(*((struct _tuple11*)_check_null(tk))).f2;}
# 133
;_pop_region(r);}
# 149 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 151
Cyc_Tcstmt_decorate_stmt(te,s0);{
# 153
void*_tmp26=s0->r;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Stmt*_tmp2B;struct Cyc_Absyn_Stmt*_tmp2C;struct Cyc_Absyn_Exp*_tmp2E;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Stmt*_tmp31;struct Cyc_Absyn_Stmt*_tmp32;struct Cyc_Absyn_Exp*_tmp34;struct Cyc_Absyn_Stmt*_tmp35;struct Cyc_Absyn_Stmt*_tmp36;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_Absyn_Exp*_tmp39;struct Cyc_Absyn_Stmt*_tmp3A;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Stmt*_tmp3C;struct Cyc_Absyn_Stmt*_tmp3D;struct Cyc_Absyn_Stmt*_tmp3F;struct Cyc_Absyn_Exp*_tmp40;struct Cyc_Absyn_Stmt*_tmp41;struct Cyc_Absyn_Stmt**_tmp43;struct Cyc_Absyn_Stmt**_tmp45;struct _dyneither_ptr*_tmp47;struct Cyc_Absyn_Stmt**_tmp48;struct Cyc_List_List*_tmp4A;struct Cyc_Absyn_Switch_clause***_tmp4B;struct _dyneither_ptr*_tmp4D;struct Cyc_Absyn_Stmt*_tmp4E;struct Cyc_Absyn_Exp*_tmp50;struct Cyc_List_List*_tmp51;struct Cyc_Absyn_Stmt*_tmp53;struct Cyc_List_List*_tmp54;struct Cyc_Absyn_Decl*_tmp56;struct Cyc_Absyn_Stmt*_tmp57;struct Cyc_Absyn_Exp*_tmp59;_LLD: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp27=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp27->tag != 0)goto _LLF;}_LLE:
# 155
 return;_LLF: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp28=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp28->tag != 1)goto _LL11;else{_tmp29=_tmp28->f1;}}_LL10:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp29);
if(!Cyc_Tcexp_in_stmt_exp)
Cyc_Tcstmt_simplify_unused_result_exp(_tmp29);
return;_LL11: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp2A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp2A->tag != 2)goto _LL13;else{_tmp2B=_tmp2A->f1;_tmp2C=_tmp2A->f2;}}_LL12: {
# 164
struct _RegionHandle _tmp5A=_new_region("r");struct _RegionHandle*r=& _tmp5A;_push_region(r);{
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp17E;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp17D;struct Cyc_Tcenv_Tenv*_tmp5B=Cyc_Tcenv_set_next(r,te,(void*)((_tmp17D=_cycalloc(sizeof(*_tmp17D)),((_tmp17D[0]=((_tmp17E.tag=3,((_tmp17E.f1=_tmp2C,_tmp17E)))),_tmp17D)))));
Cyc_Tcstmt_tcStmt(_tmp5B,_tmp2B,1);}
# 168
Cyc_Tcstmt_tcStmt(te,_tmp2C,1);
_npop_handler(0);return;
# 164
;_pop_region(r);}_LL13: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp2D=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp2D->tag != 3)goto _LL15;else{_tmp2E=_tmp2D->f1;}}_LL14: {
# 189 "tcstmt.cyc"
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp2E == 0){
void*_tmp5E=Cyc_Tcutil_compress(t);_LL30: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp5F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp5E;if(_tmp5F->tag != 0)goto _LL32;}_LL31:
 goto _LL2F;_LL32: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp60=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5E;if(_tmp60->tag != 7)goto _LL34;}_LL33:
 goto _LL35;_LL34: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5E;if(_tmp61->tag != 6)goto _LL36;}_LL35:
# 195
{const char*_tmp182;void*_tmp181[1];struct Cyc_String_pa_PrintArg_struct _tmp180;(_tmp180.tag=0,((_tmp180.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp181[0]=& _tmp180,Cyc_Tcutil_warn(s0->loc,((_tmp182="should return a value of type %s",_tag_dyneither(_tmp182,sizeof(char),33))),_tag_dyneither(_tmp181,sizeof(void*),1)))))));}
goto _LL2F;_LL36:;_LL37:
# 198
{const char*_tmp186;void*_tmp185[1];struct Cyc_String_pa_PrintArg_struct _tmp184;(_tmp184.tag=0,((_tmp184.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp185[0]=& _tmp184,Cyc_Tcutil_terr(s0->loc,((_tmp186="must return a value of type %s",_tag_dyneither(_tmp186,sizeof(char),31))),_tag_dyneither(_tmp185,sizeof(void*),1)))))));}
goto _LL2F;_LL2F:;}else{
# 203
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp2E;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp18B;void*_tmp18A[2];struct Cyc_String_pa_PrintArg_struct _tmp189;struct Cyc_String_pa_PrintArg_struct _tmp188;(_tmp188.tag=0,((_tmp188.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp189.tag=0,((_tmp189.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp18A[0]=& _tmp189,((_tmp18A[1]=& _tmp188,Cyc_Tcutil_terr(s0->loc,((_tmp18B="returns value of type %s but requires %s",_tag_dyneither(_tmp18B,sizeof(char),41))),_tag_dyneither(_tmp18A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 211
struct _RegionHandle _tmp6C=_new_region("temp");struct _RegionHandle*temp=& _tmp6C;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp18E;void*_tmp18D;(_tmp18D=0,Cyc_Tcutil_terr(e->loc,((_tmp18E="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp18E,sizeof(char),49))),_tag_dyneither(_tmp18D,sizeof(void*),0)));}
# 212
;_pop_region(temp);};}
# 216
return;}_LL15: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp2F=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp2F->tag != 4)goto _LL17;else{_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;_tmp32=_tmp2F->f3;}}_LL16: {
# 219
struct _tuple12*_tmp71;int _tmp72;const char*_tmp18F;struct Cyc_Tcexp_TestEnv _tmp70=Cyc_Tcexp_tcTest(te,_tmp30,((_tmp18F="if statement",_tag_dyneither(_tmp18F,sizeof(char),13))));_tmp71=_tmp70.eq;_tmp72=_tmp70.isTrue;
{struct _tuple12*_tmp73=_tmp71;void*_tmp74;void*_tmp75;_LL39: if(_tmp73 == 0)goto _LL3B;_tmp74=_tmp73->f1;_tmp75=_tmp73->f2;_LL3A:
# 222
 if(_tmp72){
Cyc_Tcstmt_tcStmtRefine(te,_tmp31,_tmp74,_tmp75,1);
Cyc_Tcstmt_tcStmt(te,_tmp32,1);}else{
# 227
Cyc_Tcstmt_tcStmt(te,_tmp31,1);
Cyc_Tcstmt_tcStmtRefine(te,_tmp32,_tmp74,_tmp75,1);}
# 230
goto _LL38;_LL3B:;_LL3C:
# 232
 Cyc_Tcstmt_tcStmt(te,_tmp31,1);
Cyc_Tcstmt_tcStmt(te,_tmp32,1);
goto _LL38;_LL38:;}
# 236
return;}_LL17: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp33=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp33->tag != 5)goto _LL19;else{_tmp34=(_tmp33->f1).f1;_tmp35=(_tmp33->f1).f2;_tmp36=_tmp33->f2;}}_LL18:
# 239
 Cyc_Tcstmt_decorate_stmt(te,_tmp35);
{const char*_tmp190;Cyc_Tcexp_tcTest(te,_tmp34,((_tmp190="while loop",_tag_dyneither(_tmp190,sizeof(char),11))));}{
struct _RegionHandle _tmp77=_new_region("r");struct _RegionHandle*r=& _tmp77;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp35),_tmp36,1);
# 244
_npop_handler(0);return;
# 241
;_pop_region(r);};_LL19: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp37=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp37->tag != 9)goto _LL1B;else{_tmp38=_tmp37->f1;_tmp39=(_tmp37->f2).f1;_tmp3A=(_tmp37->f2).f2;_tmp3B=(_tmp37->f3).f1;_tmp3C=(_tmp37->f3).f2;_tmp3D=_tmp37->f4;}}_LL1A:
# 247
 Cyc_Tcstmt_decorate_stmt(te,_tmp3A);
Cyc_Tcstmt_decorate_stmt(te,_tmp3C);
Cyc_Tcexp_tcExp(te,0,_tmp38);
{const char*_tmp191;Cyc_Tcexp_tcTest(te,_tmp39,((_tmp191="for loop",_tag_dyneither(_tmp191,sizeof(char),9))));}{
struct _RegionHandle _tmp79=_new_region("r");struct _RegionHandle*r=& _tmp79;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp7A=Cyc_Tcenv_set_in_loop(r,te,_tmp3C);
Cyc_Tcstmt_tcStmt(_tmp7A,_tmp3D,1);
Cyc_Tcexp_tcExp(_tmp7A,0,_tmp3B);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp3B);}
# 257
_npop_handler(0);return;
# 251
;_pop_region(r);};_LL1B: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3E=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp3E->tag != 14)goto _LL1D;else{_tmp3F=_tmp3E->f1;_tmp40=(_tmp3E->f2).f1;_tmp41=(_tmp3E->f2).f2;}}_LL1C:
# 260
 Cyc_Tcstmt_decorate_stmt(te,_tmp41);{
struct _RegionHandle _tmp7B=_new_region("r");struct _RegionHandle*r=& _tmp7B;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp41),_tmp3F,1);
# 264
{const char*_tmp192;Cyc_Tcexp_tcTest(te,_tmp40,((_tmp192="do loop",_tag_dyneither(_tmp192,sizeof(char),8))));}
_npop_handler(0);return;
# 261
;_pop_region(r);};_LL1D: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp42=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp42->tag != 6)goto _LL1F;else{_tmp43=(struct Cyc_Absyn_Stmt**)& _tmp42->f1;}}_LL1E:
# 269
 Cyc_Tcenv_process_break(te,s0,_tmp43);return;_LL1F: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp44=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp44->tag != 7)goto _LL21;else{_tmp45=(struct Cyc_Absyn_Stmt**)& _tmp44->f1;}}_LL20:
 Cyc_Tcenv_process_continue(te,s0,_tmp45);return;_LL21: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp46=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp46->tag != 8)goto _LL23;else{_tmp47=_tmp46->f1;_tmp48=(struct Cyc_Absyn_Stmt**)& _tmp46->f2;}}_LL22:
 Cyc_Tcenv_process_goto(te,s0,_tmp47,_tmp48);return;_LL23: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp49=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp49->tag != 11)goto _LL25;else{_tmp4A=_tmp49->f1;_tmp4B=(struct Cyc_Absyn_Switch_clause***)& _tmp49->f2;}}_LL24: {
# 274
const struct _tuple10*_tmp7D=Cyc_Tcenv_process_fallthru(te,s0,_tmp4B);
if(_tmp7D == (const struct _tuple10*)0){
{const char*_tmp195;void*_tmp194;(_tmp194=0,Cyc_Tcutil_terr(s0->loc,((_tmp195="fallthru not in a non-last case",_tag_dyneither(_tmp195,sizeof(char),32))),_tag_dyneither(_tmp194,sizeof(void*),0)));}
return;}{
# 279
struct Cyc_List_List*_tmp80=(*_tmp7D).f2;
const struct Cyc_Tcenv_CList*_tmp81=(*_tmp7D).f3;
struct Cyc_List_List*instantiation=
((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp80);
struct Cyc_List_List*_tmp82=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,const struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(Cyc_Core_heap_region,Cyc_Tcutil_substitute,instantiation,_tmp81);
for(0;_tmp82 != 0  && _tmp4A != 0;(_tmp82=_tmp82->tl,_tmp4A=_tmp4A->tl)){
# 286
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp4A->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp4A->hd,(void*)_tmp82->hd,& bogus)){
{const char*_tmp19A;void*_tmp199[2];struct Cyc_String_pa_PrintArg_struct _tmp198;struct Cyc_String_pa_PrintArg_struct _tmp197;(_tmp197.tag=0,((_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 291
Cyc_Absynpp_typ2string((void*)_tmp82->hd)),((_tmp198.tag=0,((_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp4A->hd)->topt))),((_tmp199[0]=& _tmp198,((_tmp199[1]=& _tmp197,Cyc_Tcutil_terr(s0->loc,((_tmp19A="fallthru argument has type %s but pattern variable has type %s",_tag_dyneither(_tmp19A,sizeof(char),63))),_tag_dyneither(_tmp199,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 294
struct _RegionHandle _tmp87=_new_region("temp");struct _RegionHandle*temp=& _tmp87;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_tmp82->hd) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmp4A->hd)){
const char*_tmp19D;void*_tmp19C;(_tmp19C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp4A->hd)->loc,((_tmp19D="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp19D,sizeof(char),49))),_tag_dyneither(_tmp19C,sizeof(void*),0)));}
# 295
;_pop_region(temp);};}
# 300
if(_tmp4A != 0){
const char*_tmp1A0;void*_tmp19F;(_tmp19F=0,Cyc_Tcutil_terr(s0->loc,((_tmp1A0="too many arguments to explicit fallthru",_tag_dyneither(_tmp1A0,sizeof(char),40))),_tag_dyneither(_tmp19F,sizeof(void*),0)));}
if(_tmp82 != 0){
const char*_tmp1A3;void*_tmp1A2;(_tmp1A2=0,Cyc_Tcutil_terr(s0->loc,((_tmp1A3="too few arguments to explicit fallthru",_tag_dyneither(_tmp1A3,sizeof(char),39))),_tag_dyneither(_tmp1A2,sizeof(void*),0)));}
return;};}_LL25: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp4C=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp4C->tag != 13)goto _LL27;else{_tmp4D=_tmp4C->f1;_tmp4E=_tmp4C->f2;}}_LL26: {
# 312
struct _RegionHandle _tmp8E=_new_region("r");struct _RegionHandle*r=& _tmp8E;_push_region(r);{
struct _dyneither_ptr*_tmp1B0;const char*_tmp1AF;void*_tmp1AE[1];struct Cyc_String_pa_PrintArg_struct _tmp1AD;struct Cyc_Absyn_Tvar*_tmp1AC;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(r,s0->loc,Cyc_Tcenv_add_label(te,_tmp4D,_tmp4E),(
(_tmp1AC=_cycalloc(sizeof(*_tmp1AC)),((_tmp1AC->name=((_tmp1B0=_cycalloc(sizeof(*_tmp1B0)),((_tmp1B0[0]=(struct _dyneither_ptr)((_tmp1AD.tag=0,((_tmp1AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4D),((_tmp1AE[0]=& _tmp1AD,Cyc_aprintf(((_tmp1AF="`%s",_tag_dyneither(_tmp1AF,sizeof(char),4))),_tag_dyneither(_tmp1AE,sizeof(void*),1)))))))),_tmp1B0)))),((_tmp1AC->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp1AC->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp1AC))))))))),_tmp4E,0);}
# 318
_npop_handler(0);return;
# 312
;_pop_region(r);}_LL27: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp4F=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp4F->tag != 10)goto _LL29;else{_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;}}_LL28:
# 321
 Cyc_Tcexp_tcExp(te,0,_tmp50);{
void*_tmp94=(void*)_check_null(_tmp50->topt);
# 326
struct _RegionHandle _tmp95=_new_region("r");struct _RegionHandle*r=& _tmp95;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmp94) && !Cyc_Tcutil_is_noalias_path(r,_tmp50)){
const char*_tmp1B3;void*_tmp1B2;(_tmp1B2=0,Cyc_Tcutil_terr(_tmp50->loc,((_tmp1B3="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1B3,sizeof(char),49))),_tag_dyneither(_tmp1B2,sizeof(void*),0)));}{
struct Cyc_Tcenv_Tenv*_tmp98=Cyc_Tcenv_set_in_switch(r,te);
_tmp98=Cyc_Tcenv_clear_fallthru(r,_tmp98);{
struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp51);for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Pat*_tmp99=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
struct Cyc_Tcpat_TcPatResult _tmp9A=Cyc_Tcpat_tcPat(_tmp98,_tmp99,& _tmp94);
struct Cyc_List_List*_tmp9B=_tmp9A.tvars_and_bounds_opt == 0?0:(*_tmp9A.tvars_and_bounds_opt).f1;
# 336
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp99->topt),_tmp94)){
{const char*_tmp1B8;void*_tmp1B7[2];struct Cyc_String_pa_PrintArg_struct _tmp1B6;struct Cyc_String_pa_PrintArg_struct _tmp1B5;(_tmp1B5.tag=0,((_tmp1B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp99->topt))),((_tmp1B6.tag=0,((_tmp1B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp94)),((_tmp1B7[0]=& _tmp1B6,((_tmp1B7[1]=& _tmp1B5,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,((_tmp1B8="switch on type %s, but case expects type %s",_tag_dyneither(_tmp1B8,sizeof(char),44))),_tag_dyneither(_tmp1B7,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}else{
# 342
Cyc_Tcpat_check_pat_regions(_tmp98,_tmp99);}
{struct Cyc_Core_Opt*_tmp1B9;((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((_tmp1B9=_cycalloc(sizeof(*_tmp1B9)),((_tmp1B9->v=_tmp9A.patvars,_tmp1B9))));}
# 345
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp98,_tmp9A,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);
# 347
if(_tmp9A.tvars_and_bounds_opt != 0  && (*_tmp9A.tvars_and_bounds_opt).f2 != 0)
# 349
_tmp98=Cyc_Tcenv_clear_fallthru(r,_tmp98);else{
# 351
_tmp98=Cyc_Tcenv_set_fallthru(r,_tmp98,_tmp9B,_tmp9A.patvars,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}};};
# 354
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp51);
_npop_handler(0);return;
# 326
;_pop_region(r);};_LL29: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp52=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp52->tag != 15)goto _LL2B;else{_tmp53=_tmp52->f1;_tmp54=_tmp52->f2;}}_LL2A:
# 362
{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1BC;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp1BB;_tmp53->r=(Cyc_Absyn_new_stmt((void*)((_tmp1BB=_cycalloc(sizeof(*_tmp1BB)),((_tmp1BB[0]=((_tmp1BC.tag=2,((_tmp1BC.f1=Cyc_Absyn_new_stmt(_tmp53->r,_tmp53->loc),((_tmp1BC.f2=Cyc_Absyn_skip_stmt(_tmp53->loc),_tmp1BC)))))),_tmp1BB)))),_tmp53->loc))->r;}{
# 366
struct _RegionHandle _tmpA3=_new_region("r");struct _RegionHandle*r=& _tmpA3;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_enter_try(r,te),s0),_tmp53,1);{
# 370
struct _RegionHandle _tmpA4=_new_region("r2");struct _RegionHandle*r2=& _tmpA4;_push_region(r2);{
# 372
struct Cyc_Tcenv_Tenv*_tmpA5=Cyc_Tcenv_set_in_switch(r2,te);
_tmpA5=Cyc_Tcenv_clear_fallthru(r2,_tmpA5);{
struct Cyc_List_List*_tmpA6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp54);for(0;_tmpA6 != 0;_tmpA6=_tmpA6->tl){
struct Cyc_Absyn_Pat*_tmpA7=((struct Cyc_Absyn_Switch_clause*)_tmpA6->hd)->pattern;
void*_tmpA8=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult _tmpA9=Cyc_Tcpat_tcPat(_tmpA5,_tmpA7,& _tmpA8);
struct Cyc_List_List*_tmpAA=_tmpA9.tvars_and_bounds_opt == 0?0:(*_tmpA9.tvars_and_bounds_opt).f1;
# 380
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpA7->topt),_tmpA8)){
const char*_tmp1C0;void*_tmp1BF[1];struct Cyc_String_pa_PrintArg_struct _tmp1BE;(_tmp1BE.tag=0,((_tmp1BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpA7->topt))),((_tmp1BF[0]=& _tmp1BE,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmpA6->hd)->loc,((_tmp1C0="expected datatype exn but found %s",_tag_dyneither(_tmp1C0,sizeof(char),35))),_tag_dyneither(_tmp1BF,sizeof(void*),1)))))));}else{
# 384
Cyc_Tcpat_check_pat_regions(_tmpA5,_tmpA7);}
{struct Cyc_Core_Opt*_tmp1C1;((struct Cyc_Absyn_Switch_clause*)_tmpA6->hd)->pat_vars=((_tmp1C1=_cycalloc(sizeof(*_tmp1C1)),((_tmp1C1->v=_tmpA9.patvars,_tmp1C1))));}
# 387
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmpA6->hd)->loc,_tmpA5,_tmpA9,((struct Cyc_Absyn_Switch_clause*)_tmpA6->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmpA6->hd)->where_clause,1);
# 389
if(_tmpA9.tvars_and_bounds_opt != 0  && (*_tmpA9.tvars_and_bounds_opt).f2 != 0)
# 391
_tmpA5=Cyc_Tcenv_clear_fallthru(r2,_tmpA5);else{
# 393
_tmpA5=Cyc_Tcenv_set_fallthru(r2,_tmpA5,_tmpAA,_tmpA9.patvars,(struct Cyc_Absyn_Switch_clause*)_tmpA6->hd);}}};}
# 396
Cyc_Tcpat_check_catch_overlap(s0->loc,te,_tmp54);
_npop_handler(1);return;
# 370
;_pop_region(r2);};
# 366
;_pop_region(r);};_LL2B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp55=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp55->tag != 12)goto _LL2D;else{_tmp56=_tmp55->f1;_tmp57=_tmp55->f2;}}_LL2C: {
# 400
struct _dyneither_ptr unimp_msg_part;
struct _RegionHandle _tmpAF=_new_region("decl_rgn");struct _RegionHandle*decl_rgn=& _tmpAF;_push_region(decl_rgn);{
struct Cyc_Tcenv_Tenv*te2;
if(new_block)
te2=Cyc_Tcenv_new_block(decl_rgn,s0->loc,te);else{
# 406
te2=Cyc_Tcenv_copy_tenv(decl_rgn,te);}{
struct Cyc_Tcenv_Tenv*_tmpB0=te2;
void*_tmpB1=_tmp56->r;struct Cyc_Absyn_Vardecl*_tmpB3;struct Cyc_Absyn_Pat*_tmpB5;struct Cyc_Core_Opt**_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_List_List*_tmpB9;struct Cyc_Absyn_Tvar*_tmpBB;struct Cyc_Absyn_Vardecl*_tmpBC;int _tmpBD;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_Absyn_Tvar*_tmpC0;struct Cyc_Absyn_Vardecl*_tmpC1;struct Cyc_Absyn_Fndecl*_tmpC3;struct _dyneither_ptr*_tmpC5;struct Cyc_List_List*_tmpC6;struct _tuple0*_tmpC8;struct Cyc_List_List*_tmpC9;_LL3E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB2=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpB2->tag != 0)goto _LL40;else{_tmpB3=_tmpB2->f1;}}_LL3F: {
# 410
enum Cyc_Absyn_Scope _tmpD3;union Cyc_Absyn_Nmspace _tmpD4;struct _dyneither_ptr*_tmpD5;struct Cyc_Absyn_Tqual _tmpD6;void*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;void**_tmpD9;struct Cyc_List_List*_tmpDA;struct Cyc_Absyn_Vardecl*_tmpD2=_tmpB3;_tmpD3=_tmpD2->sc;_tmpD4=(_tmpD2->name)->f1;_tmpD5=(_tmpD2->name)->f2;_tmpD6=_tmpD2->tq;_tmpD7=_tmpD2->type;_tmpD8=_tmpD2->initializer;_tmpD9=(void**)& _tmpD2->rgn;_tmpDA=_tmpD2->attributes;{
void*_tmpDB=Cyc_Tcenv_curr_rgn(_tmpB0);
int is_local;
switch(_tmpD3){case Cyc_Absyn_Static: _LL5E:
 goto _LL5F;case Cyc_Absyn_Extern: _LL5F:
 goto _LL60;case Cyc_Absyn_ExternC: _LL60:
# 417
 _tmpB3->escapes=1;
is_local=0;break;case Cyc_Absyn_Abstract: _LL61:
# 420
{const char*_tmp1C4;void*_tmp1C3;(_tmp1C3=0,Cyc_Tcutil_terr(_tmp56->loc,((_tmp1C4="bad abstract scope for local variable",_tag_dyneither(_tmp1C4,sizeof(char),38))),_tag_dyneither(_tmp1C3,sizeof(void*),0)));}
goto _LL62;case Cyc_Absyn_Register: _LL62:
 goto _LL63;case Cyc_Absyn_Public: _LL63:
 is_local=1;break;}
# 426
*_tmpD9=is_local?_tmpDB:(void*)& Cyc_Absyn_HeapRgn_val;
{union Cyc_Absyn_Nmspace _tmpDE=_tmpD4;_LL66: if((_tmpDE.Loc_n).tag != 4)goto _LL68;_LL67:
 goto _LL65;_LL68: if((_tmpDE.Rel_n).tag != 1)goto _LL6A;if((struct Cyc_List_List*)(_tmpDE.Rel_n).val != 0)goto _LL6A;_LL69:
# 430
(*_tmpB3->name).f1=Cyc_Absyn_Loc_n;
goto _LL65;_LL6A: if((_tmpDE.C_n).tag != 3)goto _LL6C;_LL6B:
# 434
 goto _LL6D;_LL6C: if((_tmpDE.Abs_n).tag != 2)goto _LL6E;_LL6D:
 goto _LL6F;_LL6E:;_LL6F:
# 437
{const char*_tmp1C7;void*_tmp1C6;(_tmp1C6=0,Cyc_Tcutil_terr(_tmp56->loc,((_tmp1C7="cannot declare a qualified local variable",_tag_dyneither(_tmp1C7,sizeof(char),42))),_tag_dyneither(_tmp1C6,sizeof(void*),0)));}
goto _LL65;_LL65:;}
# 441
{void*_tmpE1=Cyc_Tcutil_compress(_tmpD7);void*_tmpE4;struct Cyc_Absyn_Tqual _tmpE5;union Cyc_Absyn_Constraint*_tmpE6;unsigned int _tmpE7;_LL71:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE2=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE1;if(_tmpE2->tag != 9)goto _LL73;}if(!is_local)goto _LL73;_LL72:
# 444
 _tmpB3->escapes=1;
_tmpD3=Cyc_Absyn_Extern;
is_local=0;
goto _LL70;_LL73:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE1;if(_tmpE3->tag != 8)goto _LL75;else{_tmpE4=(_tmpE3->f1).elt_type;_tmpE5=(_tmpE3->f1).tq;if((_tmpE3->f1).num_elts != 0)goto _LL75;_tmpE6=(_tmpE3->f1).zero_term;_tmpE7=(_tmpE3->f1).zt_loc;}}if(!(_tmpB3->initializer != 0))goto _LL75;_LL74:
# 449
{void*_tmpE8=((struct Cyc_Absyn_Exp*)_check_null(_tmpB3->initializer))->r;struct _dyneither_ptr _tmpEA;struct _dyneither_ptr _tmpEC;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_List_List*_tmpF0;struct Cyc_List_List*_tmpF2;_LL78: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE8;if(_tmpE9->tag != 0)goto _LL7A;else{if(((_tmpE9->f1).String_c).tag != 8)goto _LL7A;_tmpEA=(struct _dyneither_ptr)((_tmpE9->f1).String_c).val;}}_LL79:
# 451
 _tmpD7=(_tmpB3->type=Cyc_Absyn_array_typ(_tmpE4,_tmpE5,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpEA,sizeof(char)),0),_tmpE6,_tmpE7));
goto _LL77;_LL7A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE8;if(_tmpEB->tag != 0)goto _LL7C;else{if(((_tmpEB->f1).Wstring_c).tag != 9)goto _LL7C;_tmpEC=(struct _dyneither_ptr)((_tmpEB->f1).Wstring_c).val;}}_LL7B:
# 454
 _tmpD7=(_tmpB3->type=Cyc_Absyn_array_typ(_tmpE4,_tmpE5,Cyc_Absyn_uint_exp(1,0),_tmpE6,_tmpE7));
goto _LL77;_LL7C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpED=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE8;if(_tmpED->tag != 26)goto _LL7E;else{_tmpEE=_tmpED->f2;}}_LL7D:
# 458
 _tmpD7=(_tmpB3->type=Cyc_Absyn_array_typ(_tmpE4,_tmpE5,_tmpEE,_tmpE6,_tmpE7));
goto _LL77;_LL7E: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpEF=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE8;if(_tmpEF->tag != 34)goto _LL80;else{_tmpF0=_tmpEF->f2;}}_LL7F:
 _tmpF2=_tmpF0;goto _LL81;_LL80: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF1=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE8;if(_tmpF1->tag != 25)goto _LL82;else{_tmpF2=_tmpF1->f1;}}_LL81:
# 462
 _tmpD7=(_tmpB3->type=Cyc_Absyn_array_typ(_tmpE4,_tmpE5,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF2),0),_tmpE6,_tmpE7));
# 464
goto _LL77;_LL82:;_LL83:
 goto _LL77;_LL77:;}
# 467
goto _LL70;_LL75:;_LL76:
 goto _LL70;_LL70:;}{
# 474
struct Cyc_List_List*_tmpF3=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpB0);
int _tmpF4=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmpB0,_tmpF3,& Cyc_Tcutil_tmk,_tmpF4,_tmpD7);
(_tmpB3->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmpB3->tq).print_const,_tmpD7);{
# 479
struct Cyc_Tcenv_Tenv*_tmpF5=Cyc_Tcenv_add_local_var(decl_rgn,_tmp57->loc,_tmpB0,_tmpB3);
if(_tmpD3 == Cyc_Absyn_Extern  || _tmpD3 == Cyc_Absyn_ExternC){
const char*_tmp1CA;void*_tmp1C9;(_tmp1C9=0,Cyc_Tcutil_terr(_tmp56->loc,((_tmp1CA="extern declarations not yet supported within functions",_tag_dyneither(_tmp1CA,sizeof(char),55))),_tag_dyneither(_tmp1C9,sizeof(void*),0)));}
if(_tmpD8 != 0){
Cyc_Tcexp_tcExpInitializer(_tmpF5,& _tmpD7,_tmpD8);
# 485
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpD8)){
const char*_tmp1CD;void*_tmp1CC;(_tmp1CC=0,Cyc_Tcutil_terr(_tmp56->loc,((_tmp1CD="initializer needs to be a constant expression",_tag_dyneither(_tmp1CD,sizeof(char),46))),_tag_dyneither(_tmp1CC,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(_tmpF5,_tmpD8,_tmpD7)){
struct _dyneither_ptr _tmpFA=*_tmpD5;
struct _dyneither_ptr _tmpFB=Cyc_Absynpp_typ2string(_tmpD7);
struct _dyneither_ptr _tmpFC=Cyc_Absynpp_typ2string((void*)_check_null(_tmpD8->topt));
if(((_get_dyneither_size(_tmpFA,sizeof(char))+ _get_dyneither_size(_tmpFB,sizeof(char)))+ _get_dyneither_size(_tmpFC,sizeof(char)))+ 50 < 80){
const char*_tmp1D3;void*_tmp1D2[3];struct Cyc_String_pa_PrintArg_struct _tmp1D1;struct Cyc_String_pa_PrintArg_struct _tmp1D0;struct Cyc_String_pa_PrintArg_struct _tmp1CF;(_tmp1CF.tag=0,((_tmp1CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFC),((_tmp1D0.tag=0,((_tmp1D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFB),((_tmp1D1.tag=0,((_tmp1D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFA),((_tmp1D2[0]=& _tmp1D1,((_tmp1D2[1]=& _tmp1D0,((_tmp1D2[2]=& _tmp1CF,Cyc_Tcutil_terr(_tmp56->loc,((_tmp1D3="%s declared with type %s but initialized with type %s.",_tag_dyneither(_tmp1D3,sizeof(char),55))),_tag_dyneither(_tmp1D2,sizeof(void*),3)))))))))))))))))));}else{
# 494
if((_get_dyneither_size(_tmpFA,sizeof(char))+ _get_dyneither_size(_tmpFB,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmpFC,sizeof(char))+ 25 < 80){
const char*_tmp1D9;void*_tmp1D8[3];struct Cyc_String_pa_PrintArg_struct _tmp1D7;struct Cyc_String_pa_PrintArg_struct _tmp1D6;struct Cyc_String_pa_PrintArg_struct _tmp1D5;(_tmp1D5.tag=0,((_tmp1D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFC),((_tmp1D6.tag=0,((_tmp1D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFB),((_tmp1D7.tag=0,((_tmp1D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFA),((_tmp1D8[0]=& _tmp1D7,((_tmp1D8[1]=& _tmp1D6,((_tmp1D8[2]=& _tmp1D5,Cyc_Tcutil_terr(_tmp56->loc,((_tmp1D9="%s declared with type %s\n but initialized with type %s.",_tag_dyneither(_tmp1D9,sizeof(char),56))),_tag_dyneither(_tmp1D8,sizeof(void*),3)))))))))))))))))));}else{
# 499
const char*_tmp1DF;void*_tmp1DE[3];struct Cyc_String_pa_PrintArg_struct _tmp1DD;struct Cyc_String_pa_PrintArg_struct _tmp1DC;struct Cyc_String_pa_PrintArg_struct _tmp1DB;(_tmp1DB.tag=0,((_tmp1DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFC),((_tmp1DC.tag=0,((_tmp1DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFB),((_tmp1DD.tag=0,((_tmp1DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFA),((_tmp1DE[0]=& _tmp1DD,((_tmp1DE[1]=& _tmp1DC,((_tmp1DE[2]=& _tmp1DB,Cyc_Tcutil_terr(_tmp56->loc,((_tmp1DF="%s declared with type \n%s\n but initialized with type \n%s.",_tag_dyneither(_tmp1DF,sizeof(char),58))),_tag_dyneither(_tmp1DE,sizeof(void*),3)))))))))))))))))));}}
# 501
Cyc_Tcutil_unify(_tmpD7,(void*)_check_null(_tmpD8->topt));
Cyc_Tcutil_explain_failure();}}
# 505
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpF5,s0),_tmp57,0);
_npop_handler(0);return;};};};}_LL40: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpB4=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpB4->tag != 2)goto _LL42;else{_tmpB5=_tmpB4->f1;_tmpB6=(struct Cyc_Core_Opt**)& _tmpB4->f2;_tmpB7=_tmpB4->f3;}}_LL41:
# 509
 Cyc_Tcexp_tcExpInitializer(_tmpB0,0,_tmpB7);{
# 511
void*pat_type=(void*)_check_null(_tmpB7->topt);
struct Cyc_Tcpat_TcPatResult _tmp10C=Cyc_Tcpat_tcPat(_tmpB0,_tmpB5,& pat_type);
{struct Cyc_Core_Opt*_tmp1E0;*_tmpB6=((_tmp1E0=_cycalloc(sizeof(*_tmp1E0)),((_tmp1E0->v=_tmp10C.patvars,_tmp1E0))));}
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpB5->topt),(void*)_check_null(_tmpB7->topt)) && !
Cyc_Tcutil_coerce_assign(_tmpB0,_tmpB7,(void*)_check_null(_tmpB5->topt))){
{const char*_tmp1E5;void*_tmp1E4[2];struct Cyc_String_pa_PrintArg_struct _tmp1E3;struct Cyc_String_pa_PrintArg_struct _tmp1E2;(_tmp1E2.tag=0,((_tmp1E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpB7->topt))),((_tmp1E3.tag=0,((_tmp1E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpB5->topt))),((_tmp1E4[0]=& _tmp1E3,((_tmp1E4[1]=& _tmp1E2,Cyc_Tcutil_terr(_tmp56->loc,((_tmp1E5="pattern type %s does not match definition type %s",_tag_dyneither(_tmp1E5,sizeof(char),50))),_tag_dyneither(_tmp1E4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(_tmpB5->topt),(void*)_check_null(_tmpB7->topt));
Cyc_Tcutil_explain_failure();}else{
# 522
Cyc_Tcpat_check_pat_regions(_tmpB0,_tmpB5);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmpB5->loc,_tmpB0,_tmpB5);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmpB0,_tmp10C,_tmp57,0,0);
_npop_handler(0);return;};_LL42: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpB8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpB8->tag != 3)goto _LL44;else{_tmpB9=_tmpB8->f1;}}_LL43: {
# 528
void*_tmp112=Cyc_Tcenv_curr_rgn(_tmpB0);
struct Cyc_Tcenv_Tenv*_tmp113=_tmpB0;
for(0;_tmpB9 != 0;_tmpB9=_tmpB9->tl){
struct Cyc_Absyn_Vardecl*_tmp114=(struct Cyc_Absyn_Vardecl*)_tmpB9->hd;
union Cyc_Absyn_Nmspace _tmp116;void*_tmp117;void**_tmp118;struct Cyc_Absyn_Vardecl*_tmp115=_tmp114;_tmp116=(_tmp115->name)->f1;_tmp117=_tmp115->type;_tmp118=(void**)& _tmp115->rgn;
*_tmp118=_tmp112;
{union Cyc_Absyn_Nmspace _tmp119=_tmp116;struct Cyc_List_List*_tmp11A;_LL85: if((_tmp119.Loc_n).tag != 4)goto _LL87;_LL86:
 goto _LL84;_LL87: if((_tmp119.Rel_n).tag != 1)goto _LL89;if((struct Cyc_List_List*)(_tmp119.Rel_n).val != 0)goto _LL89;_LL88:
# 537
(*_tmp114->name).f1=Cyc_Absyn_Loc_n;
goto _LL84;_LL89: if((_tmp119.Abs_n).tag != 2)goto _LL8B;_tmp11A=(struct Cyc_List_List*)(_tmp119.Abs_n).val;_LL8A: {
const char*_tmp1E8;void*_tmp1E7;(int)_throw(((_tmp1E7=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1E8="tcstmt: Abs_n declaration",_tag_dyneither(_tmp1E8,sizeof(char),26))),_tag_dyneither(_tmp1E7,sizeof(void*),0)))));}_LL8B:;_LL8C:
# 541
{const char*_tmp1EB;void*_tmp1EA;(_tmp1EA=0,Cyc_Tcutil_terr(_tmp56->loc,((_tmp1EB="cannot declare a qualified local variable",_tag_dyneither(_tmp1EB,sizeof(char),42))),_tag_dyneither(_tmp1EA,sizeof(void*),0)));}
goto _LL84;_LL84:;}
# 544
Cyc_Tcutil_check_type(s0->loc,_tmp113,Cyc_Tcenv_lookup_type_vars(_tmp113),& Cyc_Tcutil_tmk,1,_tmp117);
_tmp113=Cyc_Tcenv_add_local_var(decl_rgn,_tmp57->loc,_tmp113,_tmp114);}
# 547
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp113,s0),_tmp57,0);
_npop_handler(0);return;}_LL44: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpBA=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpBA->tag != 4)goto _LL46;else{_tmpBB=_tmpBA->f1;_tmpBC=_tmpBA->f2;_tmpBD=_tmpBA->f3;_tmpBE=_tmpBA->f4;}}_LL45:
# 553
{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1EE;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp1ED;_tmp57->r=(Cyc_Absyn_new_stmt((void*)((_tmp1ED=_cycalloc(sizeof(*_tmp1ED)),((_tmp1ED[0]=((_tmp1EE.tag=2,((_tmp1EE.f1=Cyc_Absyn_new_stmt(_tmp57->r,_tmp57->loc),((_tmp1EE.f2=Cyc_Absyn_skip_stmt(_tmp57->loc),_tmp1EE)))))),_tmp1ED)))),_tmp57->loc))->r;}{
# 555
struct Cyc_Tcenv_Tenv*_tmp121=_tmpB0;
void**_tmp123;void**_tmp124;struct Cyc_Absyn_Vardecl*_tmp122=_tmpBC;_tmp123=(void**)& _tmp122->type;_tmp124=(void**)& _tmp122->rgn;{
void*_tmp125=Cyc_Tcenv_curr_rgn(_tmpB0);
*_tmp124=_tmp125;{
void*rgn_typ;
if(_tmpBE != 0){
struct Cyc_Absyn_Exp*open_exp=_tmpBE;
struct Cyc_List_List*_tmp126=Cyc_Tcenv_lookup_type_vars(_tmpB0);
{struct Cyc_Core_Opt*_tmp1EF;rgn_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp1EF=_cycalloc(sizeof(*_tmp1EF)),((_tmp1EF->v=_tmp126,_tmp1EF)))));}{
struct Cyc_Core_Opt*_tmp1F0;void*rgn2_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp1F0=_cycalloc(sizeof(*_tmp1F0)),((_tmp1F0->v=_tmp126,_tmp1F0)))));
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp1F3;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1F2;void*expected=(void*)((_tmp1F2=_cycalloc(sizeof(*_tmp1F2)),((_tmp1F2[0]=((_tmp1F3.tag=16,((_tmp1F3.f1=rgn_typ,((_tmp1F3.f2=rgn2_typ,_tmp1F3)))))),_tmp1F2))));
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp1F6;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1F5;*_tmp123=(void*)((_tmp1F5=_cycalloc(sizeof(*_tmp1F5)),((_tmp1F5[0]=((_tmp1F6.tag=15,((_tmp1F6.f1=rgn_typ,_tmp1F6)))),_tmp1F5))));}
if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(_tmpB0,& expected,open_exp),expected)){
const char*_tmp1FB;void*_tmp1FA[2];struct Cyc_String_pa_PrintArg_struct _tmp1F9;struct Cyc_String_pa_PrintArg_struct _tmp1F8;(_tmp1F8.tag=0,((_tmp1F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 570
Cyc_Absynpp_typ2string((void*)_check_null(open_exp->topt))),((_tmp1F9.tag=0,((_tmp1F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 569
Cyc_Absynpp_typ2string(expected)),((_tmp1FA[0]=& _tmp1F9,((_tmp1FA[1]=& _tmp1F8,Cyc_Tcutil_terr(open_exp->loc,((_tmp1FB="expecting %s but found %s",_tag_dyneither(_tmp1FB,sizeof(char),26))),_tag_dyneither(_tmp1FA,sizeof(void*),2)))))))))))));}
# 573
Cyc_Tcenv_check_rgn_accessible(_tmpB0,open_exp->loc,rgn2_typ);
# 575
_tmp121=Cyc_Tcenv_add_region(decl_rgn,_tmp121,rgn_typ,0,1);};}else{
# 577
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1FE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1FD;rgn_typ=(void*)((_tmp1FD=_cycalloc(sizeof(*_tmp1FD)),((_tmp1FD[0]=((_tmp1FE.tag=2,((_tmp1FE.f1=_tmpBB,_tmp1FE)))),_tmp1FD))));}
{struct Cyc_List_List*_tmp1FF;_tmp121=Cyc_Tcenv_add_type_vars(decl_rgn,s0->loc,_tmp121,((_tmp1FF=_cycalloc(sizeof(*_tmp1FF)),((_tmp1FF->hd=_tmpBB,((_tmp1FF->tl=0,_tmp1FF)))))));}
_tmp121=Cyc_Tcenv_add_region(decl_rgn,_tmp121,rgn_typ,_tmpBD,1);}
# 581
Cyc_Tcutil_check_type(s0->loc,_tmpB0,Cyc_Tcenv_lookup_type_vars(_tmp121),& Cyc_Tcutil_bk,1,*_tmp123);
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp202;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp201;if(!Cyc_Tcutil_unify(*_tmp123,(void*)((_tmp201=_cycalloc(sizeof(*_tmp201)),((_tmp201[0]=((_tmp202.tag=15,((_tmp202.f1=rgn_typ,_tmp202)))),_tmp201)))))){
const char*_tmp205;void*_tmp204;(_tmp204=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp205="region stmt: type of region handle is wrong!",_tag_dyneither(_tmp205,sizeof(char),45))),_tag_dyneither(_tmp204,sizeof(void*),0)));}}
# 586
if(!Cyc_Absyn_no_regions)
_tmp121=Cyc_Tcenv_enter_try(decl_rgn,_tmp121);
# 589
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,Cyc_Tcenv_add_local_var(decl_rgn,_tmp57->loc,_tmp121,_tmpBC),s0),_tmp57,0);
_npop_handler(0);return;};};};_LL46: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmpBF=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpBF->tag != 5)goto _LL48;else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;}}_LL47: {
# 594
struct _RegionHandle _tmp138=_new_region("r");struct _RegionHandle*r=& _tmp138;_push_region(r);{
# 597
struct Cyc_Absyn_Tvar*_tmp206[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,s0->loc,_tmpB0,((_tmp206[0]=_tmpC0,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp206,sizeof(struct Cyc_Absyn_Tvar*),1)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp209;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp208;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp208=_cycalloc(sizeof(*_tmp208)),((_tmp208[0]=((_tmp209.tag=2,((_tmp209.f1=_tmpC0,_tmp209)))),_tmp208)))),0,1);}
Cyc_Tcutil_check_type(s0->loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_tmk,1,_tmpC1->type);{
struct Cyc_Absyn_Exp*_tmp13B=_tmpC1->initializer;
if(_tmp13B == 0){
{const char*_tmp20C;void*_tmp20B;(_tmp20B=0,Cyc_Tcutil_terr(s0->loc,((_tmp20C="alias declaration requires initializer",_tag_dyneither(_tmp20C,sizeof(char),39))),_tag_dyneither(_tmp20B,sizeof(void*),0)));}
_npop_handler(1);return;}
# 605
if(_tmpC1->sc != Cyc_Absyn_Public){
const char*_tmp20F;void*_tmp20E;(_tmp20E=0,Cyc_Tcutil_terr(_tmp56->loc,((_tmp20F="bad scope for alias declaration",_tag_dyneither(_tmp20F,sizeof(char),32))),_tag_dyneither(_tmp20E,sizeof(void*),0)));}
_tmpC1->rgn=Cyc_Tcenv_curr_rgn(_tmpB0);
{union Cyc_Absyn_Nmspace _tmp140=(*_tmpC1->name).f1;_LL8E: if((_tmp140.Loc_n).tag != 4)goto _LL90;_LL8F:
 goto _LL8D;_LL90: if((_tmp140.Rel_n).tag != 1)goto _LL92;if((struct Cyc_List_List*)(_tmp140.Rel_n).val != 0)goto _LL92;_LL91:
(*_tmpC1->name).f1=Cyc_Absyn_Loc_n;goto _LL8D;_LL92:;_LL93:
{const char*_tmp212;void*_tmp211;(_tmp211=0,Cyc_Tcutil_terr(_tmp56->loc,((_tmp212="cannot declare a qualified local variable",_tag_dyneither(_tmp212,sizeof(char),42))),_tag_dyneither(_tmp211,sizeof(void*),0)));}
goto _LL8D;_LL8D:;}
# 614
(_tmpC1->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmpC1->tq).print_const,_tmpC1->type);
# 617
te2=Cyc_Tcenv_add_local_var(r,s0->loc,te2,_tmpC1);
te2=Cyc_Tcenv_set_encloser(r,te2,_tmp57);
# 623
Cyc_Tcexp_tcExpInitializer(_tmpB0,0,_tmp13B);
# 627
Cyc_Tcstmt_check_alias_coercion(te2,_tmp56->loc,(void*)_check_null(_tmp13B->topt),_tmpC0,_tmpC1->type);
# 630
Cyc_Tcstmt_tcStmt(te2,_tmp57,0);};}
# 632
Cyc_NewControlFlow_set_encloser(s0,Cyc_Tcenv_get_encloser(_tmpB0));
_npop_handler(1);return;
# 594
;_pop_region(r);}_LL48: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpC2=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpC2->tag != 1)goto _LL4A;else{_tmpC3=_tmpC2->f1;}}_LL49: {
# 637
void*_tmp144=Cyc_Tcenv_curr_rgn(_tmpB0);
if(_tmpC3->sc != Cyc_Absyn_Public){
const char*_tmp215;void*_tmp214;(_tmp214=0,Cyc_Tcutil_terr(_tmp56->loc,((_tmp215="bad storage class for inner function",_tag_dyneither(_tmp215,sizeof(char),37))),_tag_dyneither(_tmp214,sizeof(void*),0)));}
# 641
{union Cyc_Absyn_Nmspace _tmp147=(*_tmpC3->name).f1;struct Cyc_List_List*_tmp148;_LL95: if((_tmp147.Rel_n).tag != 1)goto _LL97;if((struct Cyc_List_List*)(_tmp147.Rel_n).val != 0)goto _LL97;_LL96:
 goto _LL94;_LL97: if((_tmp147.Abs_n).tag != 2)goto _LL99;_tmp148=(struct Cyc_List_List*)(_tmp147.Abs_n).val;_LL98: {
const char*_tmp218;void*_tmp217;(_tmp217=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp218="tc: Abs_n in tcStmt var decl",_tag_dyneither(_tmp218,sizeof(char),29))),_tag_dyneither(_tmp217,sizeof(void*),0)));}_LL99:;_LL9A: {
const char*_tmp21B;void*_tmp21A;(_tmp21A=0,Cyc_Tcutil_terr(_tmp56->loc,((_tmp21B="explicit namespace not allowed on inner function declaration",_tag_dyneither(_tmp21B,sizeof(char),61))),_tag_dyneither(_tmp21A,sizeof(void*),0)));}_LL94:;}
# 646
(*_tmpC3->name).f1=Cyc_Absyn_Loc_n;
Cyc_Tcutil_check_fndecl_valid_type(_tmp56->loc,_tmpB0,_tmpC3);{
void*t=Cyc_Tcutil_fndecl2typ(_tmpC3);
_tmpC3->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,_tmpC3->attributes);
# 651
{struct Cyc_List_List*atts=_tmpC3->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_tmp14D=(void*)atts->hd;_LL9C: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp14E=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp14D;if(_tmp14E->tag != 7)goto _LL9E;}_LL9D:
 goto _LL9F;_LL9E: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp14F=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp14D;if(_tmp14F->tag != 6)goto _LLA0;}_LL9F:
{const char*_tmp21F;void*_tmp21E[1];struct Cyc_String_pa_PrintArg_struct _tmp21D;(_tmp21D.tag=0,((_tmp21D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp21E[0]=& _tmp21D,Cyc_Tcutil_terr(_tmp56->loc,((_tmp21F="bad attribute %s for function",_tag_dyneither(_tmp21F,sizeof(char),30))),_tag_dyneither(_tmp21E,sizeof(void*),1)))))));}
goto _LL9B;_LLA0:;_LLA1:
 goto _LL9B;_LL9B:;}}{
# 659
struct Cyc_Absyn_Vardecl*_tmp220;struct Cyc_Absyn_Vardecl*vd=(_tmp220=_cycalloc(sizeof(*_tmp220)),((_tmp220->sc=_tmpC3->sc,((_tmp220->name=_tmpC3->name,((_tmp220->tq=Cyc_Absyn_const_tqual(0),((_tmp220->type=
Cyc_Absyn_at_typ(t,_tmp144,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((_tmp220->initializer=0,((_tmp220->rgn=_tmp144,((_tmp220->attributes=0,((_tmp220->escapes=0,_tmp220)))))))))))))))));
# 663
_tmpC3->fn_vardecl=vd;
_tmpB0=Cyc_Tcenv_add_local_var(decl_rgn,_tmp56->loc,_tmpB0,vd);{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpB0->le);
_tmpB0->le=Cyc_Tcenv_nested_fenv(_tmp56->loc,old_fenv,_tmpC3);
Cyc_Tcstmt_tcStmt(_tmpB0,_tmpC3->body,0);
Cyc_Tcenv_check_delayed_effects(_tmpB0);
Cyc_Tcenv_check_delayed_constraints(_tmpB0);
if(!Cyc_Tcenv_all_labels_resolved(_tmpB0)){
const char*_tmp223;void*_tmp222;(_tmp222=0,Cyc_Tcutil_terr(_tmp56->loc,((_tmp223="function has goto statements to undefined labels",_tag_dyneither(_tmp223,sizeof(char),49))),_tag_dyneither(_tmp222,sizeof(void*),0)));}
_tmpB0->le=old_fenv;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpB0,s0),_tmp57,0);
_npop_handler(0);return;};};};}_LL4A: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpC4=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpC4->tag != 10)goto _LL4C;else{_tmpC5=_tmpC4->f1;_tmpC6=_tmpC4->f2;}}_LL4B:
{const char*_tmp224;unimp_msg_part=((_tmp224="namespace",_tag_dyneither(_tmp224,sizeof(char),10)));}goto _LL3D;_LL4C: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpC7=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpC7->tag != 11)goto _LL4E;else{_tmpC8=_tmpC7->f1;_tmpC9=_tmpC7->f2;}}_LL4D:
{const char*_tmp225;unimp_msg_part=((_tmp225="using",_tag_dyneither(_tmp225,sizeof(char),6)));}goto _LL3D;_LL4E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCA=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpCA->tag != 6)goto _LL50;}_LL4F:
{const char*_tmp226;unimp_msg_part=((_tmp226="type",_tag_dyneither(_tmp226,sizeof(char),5)));}goto _LL3D;_LL50: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpCB=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpCB->tag != 7)goto _LL52;}_LL51:
{const char*_tmp227;unimp_msg_part=((_tmp227="datatype",_tag_dyneither(_tmp227,sizeof(char),9)));}goto _LL3D;_LL52: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpCC=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpCC->tag != 8)goto _LL54;}_LL53:
{const char*_tmp228;unimp_msg_part=((_tmp228="enum",_tag_dyneither(_tmp228,sizeof(char),5)));}goto _LL3D;_LL54: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpCD=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpCD->tag != 9)goto _LL56;}_LL55:
{const char*_tmp229;unimp_msg_part=((_tmp229="typedef",_tag_dyneither(_tmp229,sizeof(char),8)));}goto _LL3D;_LL56: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpCE=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpCE->tag != 12)goto _LL58;}_LL57:
{const char*_tmp22A;unimp_msg_part=((_tmp22A="extern \"C\"",_tag_dyneither(_tmp22A,sizeof(char),11)));}goto _LL3D;_LL58: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpCF=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpCF->tag != 13)goto _LL5A;}_LL59:
# 683
{const char*_tmp22B;unimp_msg_part=((_tmp22B="extern \"C include\"",_tag_dyneither(_tmp22B,sizeof(char),19)));}goto _LL3D;_LL5A: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpD0=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpD0->tag != 14)goto _LL5C;}_LL5B:
{const char*_tmp22C;unimp_msg_part=((_tmp22C="__cyclone_port_on__",_tag_dyneither(_tmp22C,sizeof(char),20)));}goto _LL3D;_LL5C: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpD1=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpB1;if(_tmpD1->tag != 15)goto _LL3D;}_LL5D:
{const char*_tmp22D;unimp_msg_part=((_tmp22D="__cyclone_port_off__",_tag_dyneither(_tmp22D,sizeof(char),21)));}goto _LL3D;_LL3D:;};}{
# 688
const char*_tmp231;void*_tmp230[1];struct Cyc_String_pa_PrintArg_struct _tmp22F;(int)_throw(((_tmp22F.tag=0,((_tmp22F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part),((_tmp230[0]=& _tmp22F,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp231="tcStmt: nested %s declarations unimplemented",_tag_dyneither(_tmp231,sizeof(char),45))),_tag_dyneither(_tmp230,sizeof(void*),1)))))))));};
# 401
;_pop_region(decl_rgn);}_LL2D: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp58=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp26;if(_tmp58->tag != 16)goto _LLC;else{_tmp59=_tmp58->f1;}}_LL2E: {
# 692
void*rgn_type=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp234;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp233;void*etype=(void*)((_tmp233=_cycalloc(sizeof(*_tmp233)),((_tmp233[0]=((_tmp234.tag=15,((_tmp234.f1=rgn_type,_tmp234)))),_tmp233))));
if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,& etype,_tmp59),etype)){
const char*_tmp238;void*_tmp237[1];struct Cyc_String_pa_PrintArg_struct _tmp236;(_tmp236.tag=0,((_tmp236.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp59->topt))),((_tmp237[0]=& _tmp236,Cyc_Tcutil_terr(_tmp59->loc,((_tmp238="expecting region_t but found %s",_tag_dyneither(_tmp238,sizeof(char),32))),_tag_dyneither(_tmp237,sizeof(void*),1)))))));}
# 697
Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);
return;}_LLC:;};}
