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
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 120 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 131
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 121
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 149
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 193
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 249
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 253
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 286
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 305
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 319
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 366
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 62
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 245
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 523
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 905 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 907
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 922
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 927
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 933
void*Cyc_Absyn_compress_kb(void*);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 943
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 945
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 949
extern void*Cyc_Absyn_empty_effect;
# 959
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 968
extern void*Cyc_Absyn_bounds_one;
# 973
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1101
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1146
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1148
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1150
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1152
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1154
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1158
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1160
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
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
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 127
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 130
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 204
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 44
void Cyc_Tcutil_flush_warnings();
# 48
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 53
void*Cyc_Tcutil_copy_type(void*t);
# 55
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 62
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 74
int Cyc_Tcutil_coerceable(void*);
# 76
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 78
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 81
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 83
extern int Cyc_Tcutil_warn_alias_coerce;
# 86
extern int Cyc_Tcutil_warn_region_coerce;
# 89
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 99
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 128
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 133
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 137
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 142
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 144
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 148
void Cyc_Tcutil_explain_failure();
# 150
int Cyc_Tcutil_unify(void*,void*);
# 152
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 158
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 162
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 168
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 174
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 178
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 204 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 206
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 214
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 217
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 224
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 226
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 228
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 230
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 232
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 238
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 240
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 242
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 246
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 252
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 257
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 260
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 265
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 275
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple18{int f1;void*f2;};
# 279
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 287
void*Cyc_Tcutil_normalize_effect(void*e);
# 290
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 292
int Cyc_Tcutil_new_tvar_id();
# 294
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 297
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 299
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 302
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 312
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 316
int Cyc_Tcutil_supports_default(void*);
# 320
int Cyc_Tcutil_admits_zero(void*t);
# 323
int Cyc_Tcutil_is_noreturn(void*);
# 327
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 331
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 335
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 41 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 46 "tcutil.cyc"
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 48
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 52
int Cyc_Tcutil_warn_region_coerce=0;
# 55
void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;
# 60
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 64
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);{
const char*_tmpD5D;void*_tmpD5C[2];const char*_tmpD5B;const char*_tmpD5A;struct Cyc_String_pa_PrintArg_struct _tmpD59;struct Cyc_String_pa_PrintArg_struct _tmpD58;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpD58.tag=0,((_tmpD58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmpD59.tag=0,((_tmpD59.f1=(struct _dyneither_ptr)(
# 67
Cyc_Tcutil_tq1_const?(_tmpD5A="const ",_tag_dyneither(_tmpD5A,sizeof(char),7)):((_tmpD5B="",_tag_dyneither(_tmpD5B,sizeof(char),1)))),((_tmpD5C[0]=& _tmpD59,((_tmpD5C[1]=& _tmpD58,Cyc_aprintf(((_tmpD5D="%s%s",_tag_dyneither(_tmpD5D,sizeof(char),5))),_tag_dyneither(_tmpD5C,sizeof(void*),2))))))))))))));
# 69
const char*_tmpD66;void*_tmpD65[2];const char*_tmpD64;const char*_tmpD63;struct Cyc_String_pa_PrintArg_struct _tmpD62;struct Cyc_String_pa_PrintArg_struct _tmpD61;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpD61.tag=0,((_tmpD61.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmpD62.tag=0,((_tmpD62.f1=(struct _dyneither_ptr)(
# 69
Cyc_Tcutil_tq2_const?(_tmpD63="const ",_tag_dyneither(_tmpD63,sizeof(char),7)):((_tmpD64="",_tag_dyneither(_tmpD64,sizeof(char),1)))),((_tmpD65[0]=& _tmpD62,((_tmpD65[1]=& _tmpD61,Cyc_aprintf(((_tmpD66="%s%s",_tag_dyneither(_tmpD66,sizeof(char),5))),_tag_dyneither(_tmpD65,sizeof(void*),2))))))))))))));
# 71
int pos=2;
{const char*_tmpD6A;void*_tmpD69[1];struct Cyc_String_pa_PrintArg_struct _tmpD68;(_tmpD68.tag=0,((_tmpD68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpD69[0]=& _tmpD68,Cyc_fprintf(Cyc_stderr,((_tmpD6A="  %s",_tag_dyneither(_tmpD6A,sizeof(char),5))),_tag_dyneither(_tmpD69,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpD6D;void*_tmpD6C;(_tmpD6C=0,Cyc_fprintf(Cyc_stderr,((_tmpD6D="\n\t",_tag_dyneither(_tmpD6D,sizeof(char),3))),_tag_dyneither(_tmpD6C,sizeof(void*),0)));}
pos=8;}else{
# 78
{const char*_tmpD70;void*_tmpD6F;(_tmpD6F=0,Cyc_fprintf(Cyc_stderr,((_tmpD70=" ",_tag_dyneither(_tmpD70,sizeof(char),2))),_tag_dyneither(_tmpD6F,sizeof(void*),0)));}
++ pos;}
# 81
{const char*_tmpD73;void*_tmpD72;(_tmpD72=0,Cyc_fprintf(Cyc_stderr,((_tmpD73="and ",_tag_dyneither(_tmpD73,sizeof(char),5))),_tag_dyneither(_tmpD72,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpD76;void*_tmpD75;(_tmpD75=0,Cyc_fprintf(Cyc_stderr,((_tmpD76="\n\t",_tag_dyneither(_tmpD76,sizeof(char),3))),_tag_dyneither(_tmpD75,sizeof(void*),0)));}
pos=8;}
# 87
{const char*_tmpD7A;void*_tmpD79[1];struct Cyc_String_pa_PrintArg_struct _tmpD78;(_tmpD78.tag=0,((_tmpD78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpD79[0]=& _tmpD78,Cyc_fprintf(Cyc_stderr,((_tmpD7A="%s ",_tag_dyneither(_tmpD7A,sizeof(char),4))),_tag_dyneither(_tmpD79,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpD7D;void*_tmpD7C;(_tmpD7C=0,Cyc_fprintf(Cyc_stderr,((_tmpD7D="\n\t",_tag_dyneither(_tmpD7D,sizeof(char),3))),_tag_dyneither(_tmpD7C,sizeof(void*),0)));}
pos=8;}
# 93
{const char*_tmpD80;void*_tmpD7F;(_tmpD7F=0,Cyc_fprintf(Cyc_stderr,((_tmpD80="are not compatible. ",_tag_dyneither(_tmpD80,sizeof(char),21))),_tag_dyneither(_tmpD7F,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpD83;void*_tmpD82;(_tmpD82=0,Cyc_fprintf(Cyc_stderr,((_tmpD83="\n\t",_tag_dyneither(_tmpD83,sizeof(char),3))),_tag_dyneither(_tmpD82,sizeof(void*),0)));}{
# 99
const char*_tmpD87;void*_tmpD86[1];struct Cyc_String_pa_PrintArg_struct _tmpD85;(_tmpD85.tag=0,((_tmpD85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpD86[0]=& _tmpD85,Cyc_fprintf(Cyc_stderr,((_tmpD87="%s",_tag_dyneither(_tmpD87,sizeof(char),3))),_tag_dyneither(_tmpD86,sizeof(void*),1)))))));};}
# 101
{const char*_tmpD8A;void*_tmpD89;(_tmpD89=0,Cyc_fprintf(Cyc_stderr,((_tmpD8A="\n",_tag_dyneither(_tmpD8A,sizeof(char),2))),_tag_dyneither(_tmpD89,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 105
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 108
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 111
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 114
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpD8E;void*_tmpD8D[1];struct Cyc_String_pa_PrintArg_struct _tmpD8C;(_tmpD8C.tag=0,((_tmpD8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpD8D[0]=& _tmpD8C,Cyc_fprintf(Cyc_stderr,((_tmpD8E="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpD8E,sizeof(char),36))),_tag_dyneither(_tmpD8D,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpD91;struct Cyc_Core_Impossible_exn_struct*_tmpD90;(int)_throw((void*)((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90[0]=((_tmpD91.tag=Cyc_Core_Impossible,((_tmpD91.f1=msg,_tmpD91)))),_tmpD90)))));};}
# 120
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 124
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpD96;void*_tmpD95[2];struct Cyc_String_pa_PrintArg_struct _tmpD94;struct Cyc_String_pa_PrintArg_struct _tmpD93;(_tmpD93.tag=0,((_tmpD93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpD94.tag=0,((_tmpD94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpD95[0]=& _tmpD94,((_tmpD95[1]=& _tmpD93,Cyc_fprintf(Cyc_stderr,((_tmpD96="%s::%s ",_tag_dyneither(_tmpD96,sizeof(char),8))),_tag_dyneither(_tmpD95,sizeof(void*),2)))))))))))));}
{const char*_tmpD99;void*_tmpD98;(_tmpD98=0,Cyc_fprintf(Cyc_stderr,((_tmpD99="\n",_tag_dyneither(_tmpD99,sizeof(char),2))),_tag_dyneither(_tmpD98,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 131
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 136
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpD9A;Cyc_Tcutil_warning_segs=((_tmpD9A=_cycalloc(sizeof(*_tmpD9A)),((_tmpD9A->hd=(void*)sg,((_tmpD9A->tl=Cyc_Tcutil_warning_segs,_tmpD9A))))));}{
struct _dyneither_ptr*_tmpD9D;struct Cyc_List_List*_tmpD9C;Cyc_Tcutil_warning_msgs=((_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C->hd=((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D[0]=msg,_tmpD9D)))),((_tmpD9C->tl=Cyc_Tcutil_warning_msgs,_tmpD9C))))));};}
# 143
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpDA0;void*_tmpD9F;(_tmpD9F=0,Cyc_fprintf(Cyc_stderr,((_tmpDA0="***Warnings***\n",_tag_dyneither(_tmpDA0,sizeof(char),16))),_tag_dyneither(_tmpD9F,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpDA5;void*_tmpDA4[2];struct Cyc_String_pa_PrintArg_struct _tmpDA3;struct Cyc_String_pa_PrintArg_struct _tmpDA2;(_tmpDA2.tag=0,((_tmpDA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpDA3.tag=0,((_tmpDA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmpDA4[0]=& _tmpDA3,((_tmpDA4[1]=& _tmpDA2,Cyc_fprintf(Cyc_stderr,((_tmpDA5="%s: %s\n",_tag_dyneither(_tmpDA5,sizeof(char),8))),_tag_dyneither(_tmpDA4,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 155
{const char*_tmpDA8;void*_tmpDA7;(_tmpDA7=0,Cyc_fprintf(Cyc_stderr,((_tmpDA8="**************\n",_tag_dyneither(_tmpDA8,sizeof(char),16))),_tag_dyneither(_tmpDA7,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 160
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 164
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 169
void*Cyc_Tcutil_compress(void*t){
void*_tmp3D=t;void**_tmp41;void**_tmp43;struct Cyc_Absyn_Exp*_tmp45;void*_tmp47;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL3;else{if((void*)_tmp3E->f2 != 0)goto _LL3;}}_LL2:
 goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp3F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3D;if(_tmp3F->tag != 17)goto _LL5;else{if((void*)_tmp3F->f4 != 0)goto _LL5;}}_LL4:
 return t;_LL5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3D;if(_tmp40->tag != 17)goto _LL7;else{_tmp41=(void**)((void**)& _tmp40->f4);}}_LL6: {
# 174
void*ta=(void*)_check_null(*_tmp41);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp41=t2;
return t2;}_LL7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp42=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp42->tag != 1)goto _LL9;else{_tmp43=(void**)((void**)& _tmp42->f2);}}_LL8: {
# 180
void*ta=(void*)_check_null(*_tmp43);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp43=t2;
return t2;}_LL9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp44=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D;if(_tmp44->tag != 18)goto _LLB;else{_tmp45=_tmp44->f1;}}_LLA:
# 186
 Cyc_Evexp_eval_const_uint_exp(_tmp45);{
void*_tmp48=_tmp45->r;void*_tmp4A;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp49=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp48;if(_tmp49->tag != 37)goto _LL12;else{_tmp4A=(void*)_tmp49->f1;}}_LL11:
 return Cyc_Tcutil_compress(_tmp4A);_LL12:;_LL13:
 return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp46=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp3D;if(_tmp46->tag != 26)goto _LLD;else{if(_tmp46->f2 == 0)goto _LLD;_tmp47=*_tmp46->f2;}}_LLC:
# 192
 return Cyc_Tcutil_compress(_tmp47);_LLD:;_LLE:
 return t;_LL0:;}
# 200
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 204
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp4B=cptr;void*_tmp4C;union Cyc_Absyn_Constraint*_tmp4D;_LL15: if((_tmp4B->No_constr).tag != 3)goto _LL17;_LL16:
 return Cyc_Absyn_empty_conref();_LL17: if((_tmp4B->Eq_constr).tag != 1)goto _LL19;_tmp4C=(void*)(_tmp4B->Eq_constr).val;_LL18:
 return Cyc_Absyn_new_conref(_tmp4C);_LL19: if((_tmp4B->Forward_constr).tag != 2)goto _LL14;_tmp4D=(union Cyc_Absyn_Constraint*)(_tmp4B->Forward_constr).val;_LL1A:
 return Cyc_Tcutil_copy_conref(_tmp4D);_LL14:;}
# 211
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp4E=Cyc_Absyn_compress_kb(kb);struct Cyc_Absyn_Kind*_tmp51;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4E;if(_tmp4F->tag != 1)goto _LL1E;}_LL1D: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpDAB;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDAA;return(void*)((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA[0]=((_tmpDAB.tag=1,((_tmpDAB.f1=0,_tmpDAB)))),_tmpDAA))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4E;if(_tmp50->tag != 2)goto _LL20;else{_tmp51=_tmp50->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDAE;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDAD;return(void*)((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD[0]=((_tmpDAE.tag=2,((_tmpDAE.f1=0,((_tmpDAE.f2=_tmp51,_tmpDAE)))))),_tmpDAD))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 218
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 220
struct Cyc_Absyn_Tvar*_tmpDAF;return(_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->name=tv->name,((_tmpDAF->identity=- 1,((_tmpDAF->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpDAF)))))));}
# 222
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 224
struct _dyneither_ptr*_tmp58;struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;struct _tuple10*_tmp57=arg;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;_tmp5A=_tmp57->f3;{
struct _tuple10*_tmpDB0;return(_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->f1=_tmp58,((_tmpDB0->f2=_tmp59,((_tmpDB0->f3=Cyc_Tcutil_copy_type(_tmp5A),_tmpDB0)))))));};}
# 227
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp5D;void*_tmp5E;struct _tuple12*_tmp5C=arg;_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;{
struct _tuple12*_tmpDB1;return(_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1->f1=_tmp5D,((_tmpDB1->f2=Cyc_Tcutil_copy_type(_tmp5E),_tmpDB1)))));};}
# 231
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpDB2;return(_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->name=f->name,((_tmpDB2->tq=f->tq,((_tmpDB2->type=Cyc_Tcutil_copy_type(f->type),((_tmpDB2->width=f->width,((_tmpDB2->attributes=f->attributes,_tmpDB2)))))))))));}
# 234
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp62;void*_tmp63;struct _tuple0*_tmp61=x;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
struct _tuple0*_tmpDB3;return(_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3->f1=Cyc_Tcutil_copy_type(_tmp62),((_tmpDB3->f2=Cyc_Tcutil_copy_type(_tmp63),_tmpDB3)))));};}
# 238
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpDB4;return(_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->name=f->name,((_tmpDB4->tag=f->tag,((_tmpDB4->loc=f->loc,_tmpDB4)))))));}
# 241
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDB7;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDB6;return(void*)((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6[0]=((_tmpDB7.tag=2,((_tmpDB7.f1=Cyc_Tcutil_copy_tvar(t),_tmpDB7)))),_tmpDB6))));}
# 244
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp68=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp6C;union Cyc_Absyn_DatatypeInfoU _tmp6E;struct Cyc_List_List*_tmp6F;union Cyc_Absyn_DatatypeFieldInfoU _tmp71;struct Cyc_List_List*_tmp72;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;void*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp7A;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7F;struct Cyc_Absyn_Exp*_tmp80;union Cyc_Absyn_Constraint*_tmp81;unsigned int _tmp82;struct Cyc_List_List*_tmp84;void*_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct Cyc_List_List*_tmp88;int _tmp89;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_List_List*_tmp8E;enum Cyc_Absyn_AggrKind _tmp90;struct _tuple2*_tmp91;struct Cyc_Core_Opt*_tmp92;struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Aggrdecl**_tmp95;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp98;struct Cyc_List_List*_tmp99;struct _tuple2*_tmp9B;struct Cyc_Absyn_Enumdecl*_tmp9C;struct Cyc_List_List*_tmp9E;void*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;void*_tmpA4;void*_tmpA6;void*_tmpA7;struct _tuple2*_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Typedefdecl*_tmpAB;void*_tmpB0;struct Cyc_List_List*_tmpB2;void*_tmpB4;struct Cyc_Absyn_Aggrdecl*_tmpB7;struct Cyc_Absyn_Enumdecl*_tmpBA;struct Cyc_Absyn_Datatypedecl*_tmpBD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68;if(_tmp69->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68;if(_tmp6A->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68;if(_tmp6B->tag != 2)goto _LL29;else{_tmp6C=_tmp6B->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDBA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDB9;return(void*)((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9[0]=((_tmpDBA.tag=2,((_tmpDBA.f1=Cyc_Tcutil_copy_tvar(_tmp6C),_tmpDBA)))),_tmpDB9))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68;if(_tmp6D->tag != 3)goto _LL2B;else{_tmp6E=(_tmp6D->f1).datatype_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2A: {
# 250
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpDC0;struct Cyc_Absyn_DatatypeInfo _tmpDBF;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpDBE;return(void*)((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE[0]=((_tmpDC0.tag=3,((_tmpDC0.f1=((_tmpDBF.datatype_info=_tmp6E,((_tmpDBF.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpDBF)))),_tmpDC0)))),_tmpDBE))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68;if(_tmp70->tag != 4)goto _LL2D;else{_tmp71=(_tmp70->f1).field_info;_tmp72=(_tmp70->f1).targs;}}_LL2C: {
# 252
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpDC6;struct Cyc_Absyn_DatatypeFieldInfo _tmpDC5;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpDC4;return(void*)((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC6.tag=4,((_tmpDC6.f1=((_tmpDC5.field_info=_tmp71,((_tmpDC5.targs=Cyc_Tcutil_copy_types(_tmp72),_tmpDC5)))),_tmpDC6)))),_tmpDC4))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68;if(_tmp73->tag != 5)goto _LL2F;else{_tmp74=(_tmp73->f1).elt_typ;_tmp75=(_tmp73->f1).elt_tq;_tmp76=((_tmp73->f1).ptr_atts).rgn;_tmp77=((_tmp73->f1).ptr_atts).nullable;_tmp78=((_tmp73->f1).ptr_atts).bounds;_tmp79=((_tmp73->f1).ptr_atts).zero_term;_tmp7A=((_tmp73->f1).ptr_atts).ptrloc;}}_LL2E: {
# 254
void*_tmpC6=Cyc_Tcutil_copy_type(_tmp74);
void*_tmpC7=Cyc_Tcutil_copy_type(_tmp76);
union Cyc_Absyn_Constraint*_tmpC8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp77);
struct Cyc_Absyn_Tqual _tmpC9=_tmp75;
union Cyc_Absyn_Constraint*_tmpCA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp78);
union Cyc_Absyn_Constraint*_tmpCB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpDD0;struct Cyc_Absyn_PtrAtts _tmpDCF;struct Cyc_Absyn_PtrInfo _tmpDCE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDCD;return(void*)((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDD0.tag=5,((_tmpDD0.f1=((_tmpDCE.elt_typ=_tmpC6,((_tmpDCE.elt_tq=_tmpC9,((_tmpDCE.ptr_atts=((_tmpDCF.rgn=_tmpC7,((_tmpDCF.nullable=_tmpC8,((_tmpDCF.bounds=_tmpCA,((_tmpDCF.zero_term=_tmpCB,((_tmpDCF.ptrloc=_tmp7A,_tmpDCF)))))))))),_tmpDCE)))))),_tmpDD0)))),_tmpDCD))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68;if(_tmp7B->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp68;if(_tmp7C->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68;if(_tmp7D->tag != 8)goto _LL35;else{_tmp7E=(_tmp7D->f1).elt_type;_tmp7F=(_tmp7D->f1).tq;_tmp80=(_tmp7D->f1).num_elts;_tmp81=(_tmp7D->f1).zero_term;_tmp82=(_tmp7D->f1).zt_loc;}}_LL34: {
# 265
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDD6;struct Cyc_Absyn_ArrayInfo _tmpDD5;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDD4;return(void*)((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4[0]=((_tmpDD6.tag=8,((_tmpDD6.f1=((_tmpDD5.elt_type=Cyc_Tcutil_copy_type(_tmp7E),((_tmpDD5.tq=_tmp7F,((_tmpDD5.num_elts=_tmp80,((_tmpDD5.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp81),((_tmpDD5.zt_loc=_tmp82,_tmpDD5)))))))))),_tmpDD6)))),_tmpDD4))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68;if(_tmp83->tag != 9)goto _LL37;else{_tmp84=(_tmp83->f1).tvars;_tmp85=(_tmp83->f1).effect;_tmp86=(_tmp83->f1).ret_tqual;_tmp87=(_tmp83->f1).ret_typ;_tmp88=(_tmp83->f1).args;_tmp89=(_tmp83->f1).c_varargs;_tmp8A=(_tmp83->f1).cyc_varargs;_tmp8B=(_tmp83->f1).rgn_po;_tmp8C=(_tmp83->f1).attributes;}}_LL36: {
# 267
struct Cyc_List_List*_tmpD3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp84);
void*effopt2=_tmp85 == 0?0: Cyc_Tcutil_copy_type(_tmp85);
void*_tmpD4=Cyc_Tcutil_copy_type(_tmp87);
struct Cyc_List_List*_tmpD5=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp88);
int _tmpD6=_tmp89;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
struct Cyc_Absyn_VarargInfo*_tmpDD7;cyc_varargs2=((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7->name=cv->name,((_tmpDD7->tq=cv->tq,((_tmpDD7->type=Cyc_Tcutil_copy_type(cv->type),((_tmpDD7->inject=cv->inject,_tmpDD7))))))))));}{
# 278
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp8B);
struct Cyc_List_List*_tmpD9=_tmp8C;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpDDD;struct Cyc_Absyn_FnInfo _tmpDDC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDDB;return(void*)((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB[0]=((_tmpDDD.tag=9,((_tmpDDD.f1=((_tmpDDC.tvars=_tmpD3,((_tmpDDC.effect=effopt2,((_tmpDDC.ret_tqual=_tmp86,((_tmpDDC.ret_typ=_tmpD4,((_tmpDDC.args=_tmpD5,((_tmpDDC.c_varargs=_tmpD6,((_tmpDDC.cyc_varargs=cyc_varargs2,((_tmpDDC.rgn_po=_tmpD8,((_tmpDDC.attributes=_tmpD9,_tmpDDC)))))))))))))))))),_tmpDDD)))),_tmpDDB))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68;if(_tmp8D->tag != 10)goto _LL39;else{_tmp8E=_tmp8D->f1;}}_LL38: {
# 282
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpDE0;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDDF;return(void*)((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF[0]=((_tmpDE0.tag=10,((_tmpDE0.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp8E),_tmpDE0)))),_tmpDDF))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp8F->tag != 11)goto _LL3B;else{if((((_tmp8F->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp90=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f1;_tmp91=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f2;_tmp92=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f3;_tmp93=(_tmp8F->f1).targs;}}_LL3A: {
# 284
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDE6;struct Cyc_Absyn_AggrInfo _tmpDE5;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE4;return(void*)((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4[0]=((_tmpDE6.tag=11,((_tmpDE6.f1=((_tmpDE5.aggr_info=Cyc_Absyn_UnknownAggr(_tmp90,_tmp91,_tmp92),((_tmpDE5.targs=Cyc_Tcutil_copy_types(_tmp93),_tmpDE5)))),_tmpDE6)))),_tmpDE4))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp94->tag != 11)goto _LL3D;else{if((((_tmp94->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp95=(struct Cyc_Absyn_Aggrdecl**)(((_tmp94->f1).aggr_info).KnownAggr).val;_tmp96=(_tmp94->f1).targs;}}_LL3C: {
# 286
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDEC;struct Cyc_Absyn_AggrInfo _tmpDEB;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDEA;return(void*)((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA[0]=((_tmpDEC.tag=11,((_tmpDEC.f1=((_tmpDEB.aggr_info=Cyc_Absyn_KnownAggr(_tmp95),((_tmpDEB.targs=Cyc_Tcutil_copy_types(_tmp96),_tmpDEB)))),_tmpDEC)))),_tmpDEA))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68;if(_tmp97->tag != 12)goto _LL3F;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpDEF;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDEE;return(void*)((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDEF.tag=12,((_tmpDEF.f1=_tmp98,((_tmpDEF.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp99),_tmpDEF)))))),_tmpDEE))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp68;if(_tmp9A->tag != 13)goto _LL41;else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpDF2;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDF1;return(void*)((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF2.tag=13,((_tmpDF2.f1=_tmp9B,((_tmpDF2.f2=_tmp9C,_tmpDF2)))))),_tmpDF1))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp68;if(_tmp9D->tag != 14)goto _LL43;else{_tmp9E=_tmp9D->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpDF5;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpDF4;return(void*)((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4[0]=((_tmpDF5.tag=14,((_tmpDF5.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9E),_tmpDF5)))),_tmpDF4))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp68;if(_tmp9F->tag != 19)goto _LL45;else{_tmpA0=(void*)_tmp9F->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpDF8;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDF7;return(void*)((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF8.tag=19,((_tmpDF8.f1=Cyc_Tcutil_copy_type(_tmpA0),_tmpDF8)))),_tmpDF7))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68;if(_tmpA1->tag != 18)goto _LL47;else{_tmpA2=_tmpA1->f1;}}_LL46: {
# 293
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpDFB;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpDFA;return(void*)((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA[0]=((_tmpDFB.tag=18,((_tmpDFB.f1=_tmpA2,_tmpDFB)))),_tmpDFA))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp68;if(_tmpA3->tag != 15)goto _LL49;else{_tmpA4=(void*)_tmpA3->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpDFE;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpDFD;return(void*)((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD[0]=((_tmpDFE.tag=15,((_tmpDFE.f1=Cyc_Tcutil_copy_type(_tmpA4),_tmpDFE)))),_tmpDFD))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68;if(_tmpA5->tag != 16)goto _LL4B;else{_tmpA6=(void*)_tmpA5->f1;_tmpA7=(void*)_tmpA5->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpE01;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpE00;return(void*)((_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00[0]=((_tmpE01.tag=16,((_tmpE01.f1=Cyc_Tcutil_copy_type(_tmpA6),((_tmpE01.f2=Cyc_Tcutil_copy_type(_tmpA7),_tmpE01)))))),_tmpE00))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68;if(_tmpA8->tag != 17)goto _LL4D;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;_tmpAB=_tmpA8->f3;}}_LL4C: {
# 297
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpE04;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE03;return(void*)((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03[0]=((_tmpE04.tag=17,((_tmpE04.f1=_tmpA9,((_tmpE04.f2=Cyc_Tcutil_copy_types(_tmpAA),((_tmpE04.f3=_tmpAB,((_tmpE04.f4=0,_tmpE04)))))))))),_tmpE03))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp68;if(_tmpAC->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp68;if(_tmpAD->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp68;if(_tmpAE->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp68;if(_tmpAF->tag != 23)goto _LL55;else{_tmpB0=(void*)_tmpAF->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE07;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE06;return(void*)((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE07.tag=23,((_tmpE07.f1=Cyc_Tcutil_copy_type(_tmpB0),_tmpE07)))),_tmpE06))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp68;if(_tmpB1->tag != 24)goto _LL57;else{_tmpB2=_tmpB1->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE0A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE09;return(void*)((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09[0]=((_tmpE0A.tag=24,((_tmpE0A.f1=Cyc_Tcutil_copy_types(_tmpB2),_tmpE0A)))),_tmpE09))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp68;if(_tmpB3->tag != 25)goto _LL59;else{_tmpB4=(void*)_tmpB3->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE0D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpE0C;return(void*)((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C[0]=((_tmpE0D.tag=25,((_tmpE0D.f1=Cyc_Tcutil_copy_type(_tmpB4),_tmpE0D)))),_tmpE0C))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB5->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB5->f1)->r;if(_tmpB6->tag != 0)goto _LL5B;else{_tmpB7=_tmpB6->f1;}}}_LL5A: {
# 306
struct Cyc_List_List*_tmpFB=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB7->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE13;struct Cyc_Absyn_AggrInfo _tmpE12;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE11;return(void*)((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE13.tag=11,((_tmpE13.f1=((_tmpE12.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB7->kind,_tmpB7->name,0),((_tmpE12.targs=_tmpFB,_tmpE12)))),_tmpE13)))),_tmpE11))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB8->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB9=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB8->f1)->r;if(_tmpB9->tag != 1)goto _LL5D;else{_tmpBA=_tmpB9->f1;}}}_LL5C: {
# 309
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE16;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE15;return(void*)((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15[0]=((_tmpE16.tag=13,((_tmpE16.f1=_tmpBA->name,((_tmpE16.f2=0,_tmpE16)))))),_tmpE15))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBB->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpBB->f1)->r;if(_tmpBC->tag != 2)goto _LL22;else{_tmpBD=_tmpBC->f1;}}}_LL5E: {
# 311
struct Cyc_List_List*_tmp101=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBD->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE20;struct Cyc_Absyn_UnknownDatatypeInfo _tmpE1F;struct Cyc_Absyn_DatatypeInfo _tmpE1E;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE1D;return(void*)((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D[0]=((_tmpE20.tag=3,((_tmpE20.f1=((_tmpE1E.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpE1F.name=_tmpBD->name,((_tmpE1F.is_extensible=0,_tmpE1F))))),((_tmpE1E.targs=_tmp101,_tmpE1E)))),_tmpE20)))),_tmpE1D))));}_LL22:;}
# 326 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp106=d;struct Cyc_Absyn_Exp*_tmp108;struct _dyneither_ptr*_tmp10A;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp107=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp106;if(_tmp107->tag != 0)goto _LL62;else{_tmp108=_tmp107->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE23;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE22;return(void*)((_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22[0]=((_tmpE23.tag=0,((_tmpE23.f1=Cyc_Tcutil_deep_copy_exp(_tmp108),_tmpE23)))),_tmpE22))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp109=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp106;if(_tmp109->tag != 1)goto _LL5F;else{_tmp10A=_tmp109->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 332
static struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){
# 334
struct _tuple19*_tmpE24;return(_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmpE24->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmpE24)))));}
# 337
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){
void*_tmp10E=e->r;union Cyc_Absyn_Cnst _tmp110;struct _tuple2*_tmp112;void*_tmp113;enum Cyc_Absyn_Primop _tmp115;struct Cyc_List_List*_tmp116;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Core_Opt*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11C;enum Cyc_Absyn_Incrementor _tmp11D;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_VarargCallInfo*_tmp12E;int _tmp12F;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_List_List*_tmp136;void*_tmp138;struct Cyc_Absyn_Exp*_tmp139;int _tmp13A;enum Cyc_Absyn_Coercion _tmp13B;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;void*_tmp142;struct Cyc_Absyn_Exp*_tmp144;void*_tmp146;void*_tmp147;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;struct _dyneither_ptr*_tmp14C;int _tmp14D;int _tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct _dyneither_ptr*_tmp151;int _tmp152;int _tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_List_List*_tmp158;struct _dyneither_ptr*_tmp15A;struct Cyc_Absyn_Tqual _tmp15B;void*_tmp15C;struct Cyc_List_List*_tmp15D;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp163;int _tmp164;struct _tuple2*_tmp166;struct Cyc_List_List*_tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Absyn_Aggrdecl*_tmp169;void*_tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Datatypedecl*_tmp16F;struct Cyc_Absyn_Datatypefield*_tmp170;struct _tuple2*_tmp172;struct Cyc_Absyn_Enumdecl*_tmp173;struct Cyc_Absyn_Enumfield*_tmp174;struct _tuple2*_tmp176;void*_tmp177;struct Cyc_Absyn_Enumfield*_tmp178;int _tmp17A;struct Cyc_Absyn_Exp*_tmp17B;void**_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;int _tmp17E;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Core_Opt*_tmp183;struct Cyc_List_List*_tmp184;struct Cyc_Absyn_Exp*_tmp187;struct _dyneither_ptr*_tmp188;void*_tmp18A;int _tmp18C;struct _dyneither_ptr _tmp18D;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp10F->tag != 0)goto _LL67;else{_tmp110=_tmp10F->f1;}}_LL66:
 return Cyc_Absyn_const_exp(_tmp110,e->loc);_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp111->tag != 1)goto _LL69;else{_tmp112=_tmp111->f1;_tmp113=(void*)_tmp111->f2;}}_LL68:
 return Cyc_Absyn_varb_exp(_tmp112,_tmp113,e->loc);_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp114=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp114->tag != 2)goto _LL6B;else{_tmp115=_tmp114->f1;_tmp116=_tmp114->f2;}}_LL6A:
 return Cyc_Absyn_primop_exp(_tmp115,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp116),e->loc);_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp117->tag != 3)goto _LL6D;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;_tmp11A=_tmp117->f3;}}_LL6C: {
# 343
struct Cyc_Core_Opt*_tmpE25;return Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp118),(unsigned int)_tmp119?(_tmpE25=_cycalloc_atomic(sizeof(*_tmpE25)),((_tmpE25->v=(void*)((enum Cyc_Absyn_Primop)_tmp119->v),_tmpE25))): 0,Cyc_Tcutil_deep_copy_exp(_tmp11A),e->loc);}_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp11B->tag != 4)goto _LL6F;else{_tmp11C=_tmp11B->f1;_tmp11D=_tmp11B->f2;}}_LL6E:
 return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp11C),_tmp11D,e->loc);_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp11E->tag != 5)goto _LL71;else{_tmp11F=_tmp11E->f1;_tmp120=_tmp11E->f2;_tmp121=_tmp11E->f3;}}_LL70:
# 346
 return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp11F),Cyc_Tcutil_deep_copy_exp(_tmp120),Cyc_Tcutil_deep_copy_exp(_tmp121),e->loc);_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp122=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp122->tag != 6)goto _LL73;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;}}_LL72:
 return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp123),Cyc_Tcutil_deep_copy_exp(_tmp124),e->loc);_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp125=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp125->tag != 7)goto _LL75;else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;}}_LL74:
 return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp126),Cyc_Tcutil_deep_copy_exp(_tmp127),e->loc);_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp128->tag != 8)goto _LL77;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}_LL76:
 return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp129),Cyc_Tcutil_deep_copy_exp(_tmp12A),e->loc);_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp12B->tag != 9)goto _LL79;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;_tmp12E=_tmp12B->f3;_tmp12F=_tmp12B->f4;}}_LL78: {
# 351
struct Cyc_Absyn_VarargCallInfo*_tmp18F=_tmp12E;int _tmp190;struct Cyc_List_List*_tmp191;struct Cyc_Absyn_VarargInfo*_tmp192;_LLB4: if(_tmp18F == 0)goto _LLB6;_tmp190=_tmp18F->num_varargs;_tmp191=_tmp18F->injectors;_tmp192=_tmp18F->vai;_LLB5: {
# 353
struct _dyneither_ptr*_tmp194;struct Cyc_Absyn_Tqual _tmp195;void*_tmp196;int _tmp197;struct Cyc_Absyn_VarargInfo*_tmp193=_tmp192;_tmp194=_tmp193->name;_tmp195=_tmp193->tq;_tmp196=_tmp193->type;_tmp197=_tmp193->inject;{
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE2F;struct Cyc_Absyn_VarargInfo*_tmpE2E;struct Cyc_Absyn_VarargCallInfo*_tmpE2D;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE2C;return Cyc_Absyn_new_exp((void*)((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C[0]=((_tmpE2F.tag=9,((_tmpE2F.f1=
Cyc_Tcutil_deep_copy_exp(_tmp12C),((_tmpE2F.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12D),((_tmpE2F.f3=(
(_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D->num_varargs=_tmp190,((_tmpE2D->injectors=_tmp191,((_tmpE2D->vai=(
(_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->name=_tmp194,((_tmpE2E->tq=_tmp195,((_tmpE2E->type=Cyc_Tcutil_copy_type(_tmp196),((_tmpE2E->inject=_tmp197,_tmpE2E)))))))))),_tmpE2D)))))))),((_tmpE2F.f4=_tmp12F,_tmpE2F)))))))))),_tmpE2C)))),e->loc);};}_LLB6:;_LLB7: {
# 360
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE32;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE31;return Cyc_Absyn_new_exp((void*)((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31[0]=((_tmpE32.tag=9,((_tmpE32.f1=Cyc_Tcutil_deep_copy_exp(_tmp12C),((_tmpE32.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12D),((_tmpE32.f3=_tmp12E,((_tmpE32.f4=_tmp12F,_tmpE32)))))))))),_tmpE31)))),e->loc);}_LLB3:;}_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp130->tag != 10)goto _LL7B;else{_tmp131=_tmp130->f1;}}_LL7A:
# 362
 return Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp131),e->loc);_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp132->tag != 11)goto _LL7D;else{_tmp133=_tmp132->f1;}}_LL7C:
 return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp133),e->loc);_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp134->tag != 12)goto _LL7F;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;}}_LL7E:
# 365
 return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp135),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp136),e->loc);_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp137=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp137->tag != 13)goto _LL81;else{_tmp138=(void*)_tmp137->f1;_tmp139=_tmp137->f2;_tmp13A=_tmp137->f3;_tmp13B=_tmp137->f4;}}_LL80:
# 367
 return Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp138),Cyc_Tcutil_deep_copy_exp(_tmp139),_tmp13A,_tmp13B,e->loc);_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp13C=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp13C->tag != 14)goto _LL83;else{_tmp13D=_tmp13C->f1;}}_LL82:
 return Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp13D),e->loc);_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp13E->tag != 15)goto _LL85;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;}}_LL84: {
# 370
struct Cyc_Absyn_Exp*eo1=_tmp13F;if(_tmp13F != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp13F);
return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp140),e->loc);}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp141=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp141->tag != 16)goto _LL87;else{_tmp142=(void*)_tmp141->f1;}}_LL86:
 return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp142),e->loc);_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp143=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp143->tag != 17)goto _LL89;else{_tmp144=_tmp143->f1;}}_LL88:
 return Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp144),e->loc);_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp145->tag != 18)goto _LL8B;else{_tmp146=(void*)_tmp145->f1;_tmp147=(void*)_tmp145->f2;}}_LL8A:
# 375
 return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp146),_tmp147,e->loc);_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp148=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp148->tag != 19)goto _LL8D;else{_tmp149=_tmp148->f1;}}_LL8C:
 return Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp149),e->loc);_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp14A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp14A->tag != 20)goto _LL8F;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;_tmp14D=_tmp14A->f3;_tmp14E=_tmp14A->f4;}}_LL8E: {
# 378
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpE35;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpE34;return Cyc_Absyn_new_exp((void*)((_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34[0]=((_tmpE35.tag=20,((_tmpE35.f1=Cyc_Tcutil_deep_copy_exp(_tmp14B),((_tmpE35.f2=_tmp14C,((_tmpE35.f3=_tmp14D,((_tmpE35.f4=_tmp14E,_tmpE35)))))))))),_tmpE34)))),e->loc);}_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp14F->tag != 21)goto _LL91;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;_tmp152=_tmp14F->f3;_tmp153=_tmp14F->f4;}}_LL90: {
# 380
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpE38;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpE37;return Cyc_Absyn_new_exp((void*)((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37[0]=((_tmpE38.tag=21,((_tmpE38.f1=Cyc_Tcutil_deep_copy_exp(_tmp150),((_tmpE38.f2=_tmp151,((_tmpE38.f3=_tmp152,((_tmpE38.f4=_tmp153,_tmpE38)))))))))),_tmpE37)))),e->loc);}_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp154->tag != 22)goto _LL93;else{_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;}}_LL92:
 return Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp155),Cyc_Tcutil_deep_copy_exp(_tmp156),e->loc);_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp157->tag != 23)goto _LL95;else{_tmp158=_tmp157->f1;}}_LL94:
 return Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp158),e->loc);_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp159=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp159->tag != 24)goto _LL97;else{_tmp15A=(_tmp159->f1)->f1;_tmp15B=(_tmp159->f1)->f2;_tmp15C=(_tmp159->f1)->f3;_tmp15D=_tmp159->f2;}}_LL96: {
# 384
struct _dyneither_ptr*vopt1=_tmp15A;
if(_tmp15A != 0)vopt1=_tmp15A;{
struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE3E;struct _tuple10*_tmpE3D;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE3C;return Cyc_Absyn_new_exp((void*)((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C[0]=((_tmpE3E.tag=24,((_tmpE3E.f1=((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D->f1=vopt1,((_tmpE3D->f2=_tmp15B,((_tmpE3D->f3=Cyc_Tcutil_copy_type(_tmp15C),_tmpE3D)))))))),((_tmpE3E.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15D),_tmpE3E)))))),_tmpE3C)))),e->loc);};}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp15E->tag != 25)goto _LL99;else{_tmp15F=_tmp15E->f1;}}_LL98: {
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpE41;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE40;return Cyc_Absyn_new_exp((void*)((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40[0]=((_tmpE41.tag=25,((_tmpE41.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15F),_tmpE41)))),_tmpE40)))),e->loc);}_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp160->tag != 26)goto _LL9B;else{_tmp161=_tmp160->f1;_tmp162=_tmp160->f2;_tmp163=_tmp160->f3;_tmp164=_tmp160->f4;}}_LL9A: {
# 390
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpE44;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE43;return Cyc_Absyn_new_exp((void*)((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43[0]=((_tmpE44.tag=26,((_tmpE44.f1=_tmp161,((_tmpE44.f2=Cyc_Tcutil_deep_copy_exp(_tmp162),((_tmpE44.f3=Cyc_Tcutil_deep_copy_exp(_tmp163),((_tmpE44.f4=_tmp164,_tmpE44)))))))))),_tmpE43)))),e->loc);}_LL9B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp165->tag != 27)goto _LL9D;else{_tmp166=_tmp165->f1;_tmp167=_tmp165->f2;_tmp168=_tmp165->f3;_tmp169=_tmp165->f4;}}_LL9C: {
# 392
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE47;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE46;return Cyc_Absyn_new_exp((void*)((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46[0]=((_tmpE47.tag=27,((_tmpE47.f1=_tmp166,((_tmpE47.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp167),((_tmpE47.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp168),((_tmpE47.f4=_tmp169,_tmpE47)))))))))),_tmpE46)))),e->loc);}_LL9D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp16A->tag != 28)goto _LL9F;else{_tmp16B=(void*)_tmp16A->f1;_tmp16C=_tmp16A->f2;}}_LL9E: {
# 395
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE4A;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE49;return Cyc_Absyn_new_exp((void*)((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=((_tmpE4A.tag=28,((_tmpE4A.f1=Cyc_Tcutil_copy_type(_tmp16B),((_tmpE4A.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16C),_tmpE4A)))))),_tmpE49)))),e->loc);}_LL9F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp16D->tag != 29)goto _LLA1;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;_tmp170=_tmp16D->f3;}}_LLA0: {
# 397
struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE4D;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE4C;return Cyc_Absyn_new_exp((void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((_tmpE4D.tag=29,((_tmpE4D.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp16E),((_tmpE4D.f2=_tmp16F,((_tmpE4D.f3=_tmp170,_tmpE4D)))))))),_tmpE4C)))),e->loc);}_LLA1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp171->tag != 30)goto _LLA3;else{_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;_tmp174=_tmp171->f3;}}_LLA2:
 return e;_LLA3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp175->tag != 31)goto _LLA5;else{_tmp176=_tmp175->f1;_tmp177=(void*)_tmp175->f2;_tmp178=_tmp175->f3;}}_LLA4: {
# 400
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpE50;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE4F;return Cyc_Absyn_new_exp((void*)((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE50.tag=31,((_tmpE50.f1=_tmp176,((_tmpE50.f2=Cyc_Tcutil_copy_type(_tmp177),((_tmpE50.f3=_tmp178,_tmpE50)))))))),_tmpE4F)))),e->loc);}_LLA5: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp179->tag != 32)goto _LLA7;else{_tmp17A=(_tmp179->f1).is_calloc;_tmp17B=(_tmp179->f1).rgn;_tmp17C=(_tmp179->f1).elt_type;_tmp17D=(_tmp179->f1).num_elts;_tmp17E=(_tmp179->f1).fat_result;}}_LLA6: {
# 402
struct Cyc_Absyn_Exp*_tmp1B1=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp17B;if(_tmp17B != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp17B);{
void**t1=_tmp17C;if(_tmp17C != 0){void**_tmpE51;t1=((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=Cyc_Tcutil_copy_type(*_tmp17C),_tmpE51))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE57;struct Cyc_Absyn_MallocInfo _tmpE56;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE55;_tmp1B1->r=(void*)((_tmpE55=_cycalloc(sizeof(*_tmpE55)),((_tmpE55[0]=((_tmpE57.tag=32,((_tmpE57.f1=((_tmpE56.is_calloc=_tmp17A,((_tmpE56.rgn=r1,((_tmpE56.elt_type=t1,((_tmpE56.num_elts=_tmp17D,((_tmpE56.fat_result=_tmp17E,_tmpE56)))))))))),_tmpE57)))),_tmpE55))));}
return _tmp1B1;};}_LLA7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp17F->tag != 33)goto _LLA9;else{_tmp180=_tmp17F->f1;_tmp181=_tmp17F->f2;}}_LLA8:
 return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp180),Cyc_Tcutil_deep_copy_exp(_tmp181),e->loc);_LLA9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp182->tag != 34)goto _LLAB;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LLAA: {
# 409
struct Cyc_Core_Opt*nopt1=_tmp183;
if(_tmp183 != 0){struct Cyc_Core_Opt*_tmpE58;nopt1=((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58->v=(struct _tuple2*)_tmp183->v,_tmpE58))));}{
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpE5B;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpE5A;return Cyc_Absyn_new_exp((void*)((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=((_tmpE5B.tag=34,((_tmpE5B.f1=nopt1,((_tmpE5B.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp184),_tmpE5B)))))),_tmpE5A)))),e->loc);};}_LLAB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp185->tag != 35)goto _LLAD;}_LLAC: {
# 413
struct Cyc_Core_Failure_exn_struct _tmpE61;const char*_tmpE60;struct Cyc_Core_Failure_exn_struct*_tmpE5F;(int)_throw((void*)((_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((_tmpE5F[0]=((_tmpE61.tag=Cyc_Core_Failure,((_tmpE61.f1=((_tmpE60="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpE60,sizeof(char),45))),_tmpE61)))),_tmpE5F)))));}_LLAD: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp186->tag != 36)goto _LLAF;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;}}_LLAE: {
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE64;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE63;return Cyc_Absyn_new_exp((void*)((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63[0]=((_tmpE64.tag=36,((_tmpE64.f1=Cyc_Tcutil_deep_copy_exp(_tmp187),((_tmpE64.f2=_tmp188,_tmpE64)))))),_tmpE63)))),e->loc);}_LLAF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp189->tag != 37)goto _LLB1;else{_tmp18A=(void*)_tmp189->f1;}}_LLB0:
 return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp18A),e->loc);_LLB1: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp18B->tag != 38)goto _LL64;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;}}_LLB2:
 return Cyc_Absyn_asm_exp(_tmp18C,_tmp18D,e->loc);_LL64:;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 428 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1BF;enum Cyc_Absyn_AliasQual _tmp1C0;struct Cyc_Absyn_Kind*_tmp1BE=ka1;_tmp1BF=_tmp1BE->kind;_tmp1C0=_tmp1BE->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C2;enum Cyc_Absyn_AliasQual _tmp1C3;struct Cyc_Absyn_Kind*_tmp1C1=ka2;_tmp1C2=_tmp1C1->kind;_tmp1C3=_tmp1C1->aliasqual;
# 432
if(_tmp1BF != _tmp1C2){
struct _tuple20 _tmpE65;struct _tuple20 _tmp1C5=(_tmpE65.f1=_tmp1BF,((_tmpE65.f2=_tmp1C2,_tmpE65)));_LLB9: if(_tmp1C5.f1 != Cyc_Absyn_BoxKind)goto _LLBB;if(_tmp1C5.f2 != Cyc_Absyn_MemKind)goto _LLBB;_LLBA:
 goto _LLBC;_LLBB: if(_tmp1C5.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1C5.f2 != Cyc_Absyn_AnyKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1C5.f1 != Cyc_Absyn_MemKind)goto _LLBF;if(_tmp1C5.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLB8;_LLBF:;_LLC0:
 return 0;_LLB8:;}
# 441
if(_tmp1C0 != _tmp1C3){
struct _tuple21 _tmpE66;struct _tuple21 _tmp1C7=(_tmpE66.f1=_tmp1C0,((_tmpE66.f2=_tmp1C3,_tmpE66)));_LLC2: if(_tmp1C7.f1 != Cyc_Absyn_Aliasable)goto _LLC4;if(_tmp1C7.f2 != Cyc_Absyn_Top)goto _LLC4;_LLC3:
 goto _LLC5;_LLC4: if(_tmp1C7.f1 != Cyc_Absyn_Unique)goto _LLC6;if(_tmp1C7.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 return 1;_LLC6:;_LLC7:
 return 0;_LLC1:;}
# 448
return 1;};}
# 451
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1C8=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp1CA;struct Cyc_Absyn_Kind*_tmp1CC;_LLC9: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1C9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1C8;if(_tmp1C9->tag != 0)goto _LLCB;else{_tmp1CA=_tmp1C9->f1;}}_LLCA:
 return _tmp1CA;_LLCB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1CB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1C8;if(_tmp1CB->tag != 2)goto _LLCD;else{_tmp1CC=_tmp1CB->f2;}}_LLCC:
 return _tmp1CC;_LLCD:;_LLCE:
# 456
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE69;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE68;tv->kind=(void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE69.tag=2,((_tmpE69.f1=0,((_tmpE69.f2=def,_tmpE69)))))),_tmpE68))));}
return def;_LLC8:;}
# 461
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpE6A;struct _tuple0 _tmp1D0=(_tmpE6A.f1=Cyc_Absyn_compress_kb(kb1),((_tmpE6A.f2=Cyc_Absyn_compress_kb(kb2),_tmpE6A)));struct Cyc_Absyn_Kind*_tmp1D2;struct Cyc_Absyn_Kind*_tmp1D4;struct Cyc_Core_Opt**_tmp1D6;struct Cyc_Absyn_Kind*_tmp1D7;struct Cyc_Absyn_Kind*_tmp1D9;struct Cyc_Absyn_Kind*_tmp1DB;struct Cyc_Core_Opt**_tmp1DD;struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Core_Opt**_tmp1E0;struct Cyc_Absyn_Kind*_tmp1E1;struct Cyc_Core_Opt**_tmp1E3;struct Cyc_Absyn_Kind*_tmp1E4;struct Cyc_Core_Opt**_tmp1E6;void*_tmp1E7;void*_tmp1E8;struct Cyc_Core_Opt**_tmp1EA;_LLD0:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1D1->tag != 0)goto _LLD2;else{_tmp1D2=_tmp1D1->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D3=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1D3->tag != 0)goto _LLD2;else{_tmp1D4=_tmp1D3->f1;}};_LLD1:
 return _tmp1D2 == _tmp1D4;_LLD2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1D5->tag != 2)goto _LLD4;else{_tmp1D6=(struct Cyc_Core_Opt**)& _tmp1D5->f1;_tmp1D7=_tmp1D5->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1D8->tag != 0)goto _LLD4;else{_tmp1D9=_tmp1D8->f1;}};_LLD3:
# 465
 if(Cyc_Tcutil_kind_leq(_tmp1D9,_tmp1D7)){
{struct Cyc_Core_Opt*_tmpE6B;*_tmp1D6=((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B->v=kb2,_tmpE6B))));}
return 1;}else{
return 0;}_LLD4:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1DA->tag != 0)goto _LLD6;else{_tmp1DB=_tmp1DA->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1DC->tag != 2)goto _LLD6;else{_tmp1DD=(struct Cyc_Core_Opt**)& _tmp1DC->f1;_tmp1DE=_tmp1DC->f2;}};_LLD5:
# 470
 if(Cyc_Tcutil_kind_leq(_tmp1DB,_tmp1DE)){
{struct Cyc_Core_Opt*_tmpE6C;*_tmp1DD=((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C->v=kb1,_tmpE6C))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1DF->tag != 2)goto _LLD8;else{_tmp1E0=(struct Cyc_Core_Opt**)& _tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1E2->tag != 2)goto _LLD8;else{_tmp1E3=(struct Cyc_Core_Opt**)& _tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}};_LLD7:
# 475
 if(Cyc_Tcutil_kind_leq(_tmp1E1,_tmp1E4)){
{struct Cyc_Core_Opt*_tmpE6D;*_tmp1E3=((_tmpE6D=_cycalloc(sizeof(*_tmpE6D)),((_tmpE6D->v=kb1,_tmpE6D))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1E4,_tmp1E1)){
{struct Cyc_Core_Opt*_tmpE6E;*_tmp1E0=((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E->v=kb2,_tmpE6E))));}
return 1;}else{
return 0;}}_LLD8:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1E5->tag != 1)goto _LLDA;else{_tmp1E6=(struct Cyc_Core_Opt**)& _tmp1E5->f1;}}_tmp1E7=_tmp1D0.f2;_LLD9:
 _tmp1E8=_tmp1E7;_tmp1EA=_tmp1E6;goto _LLDB;_LLDA: _tmp1E8=_tmp1D0.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E9=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1E9->tag != 1)goto _LLCF;else{_tmp1EA=(struct Cyc_Core_Opt**)& _tmp1E9->f1;}};_LLDB:
# 484
{struct Cyc_Core_Opt*_tmpE6F;*_tmp1EA=((_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F->v=_tmp1E8,_tmpE6F))));}
return 1;_LLCF:;}
# 489
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1F0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp1F2;_LLDD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F0;if(_tmp1F1->tag != 2)goto _LLDF;else{_tmp1F2=_tmp1F1->f1;}}_LLDE: {
# 492
void*_tmp1F3=_tmp1F2->kind;
_tmp1F2->kind=kb;{
struct _tuple16 _tmpE70;return(_tmpE70.f1=_tmp1F2,((_tmpE70.f2=_tmp1F3,_tmpE70)));};}_LLDF:;_LLE0: {
# 496
const char*_tmpE74;void*_tmpE73[1];struct Cyc_String_pa_PrintArg_struct _tmpE72;(_tmpE72.tag=0,((_tmpE72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE73[0]=& _tmpE72,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE74="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpE74,sizeof(char),40))),_tag_dyneither(_tmpE73,sizeof(void*),1)))))));}_LLDC:;}
# 504
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1F8=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp1FA;struct Cyc_Absyn_Tvar*_tmp1FC;enum Cyc_Absyn_Size_of _tmp1FF;struct Cyc_Absyn_Datatypedecl*_tmp20B;struct Cyc_Absyn_Datatypefield*_tmp20C;struct Cyc_List_List*_tmp210;struct Cyc_Absyn_AggrdeclImpl*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_Absyn_PtrInfo _tmp215;struct Cyc_Absyn_Exp*_tmp219;struct Cyc_Absyn_Typedefdecl*_tmp21C;_LLE2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1F9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F8;if(_tmp1F9->tag != 1)goto _LLE4;else{_tmp1FA=_tmp1F9->f1;}}_LLE3:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1FA))->v;_LLE4: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1FB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F8;if(_tmp1FB->tag != 2)goto _LLE6;else{_tmp1FC=_tmp1FB->f1;}}_LLE5:
 return Cyc_Tcutil_tvar_kind(_tmp1FC,& Cyc_Tcutil_bk);_LLE6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1FD=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1F8;if(_tmp1FD->tag != 0)goto _LLE8;}_LLE7:
 return& Cyc_Tcutil_mk;_LLE8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1FE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F8;if(_tmp1FE->tag != 6)goto _LLEA;else{_tmp1FF=_tmp1FE->f2;}}_LLE9:
# 510
 return(_tmp1FF == Cyc_Absyn_Int_sz  || _tmp1FF == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLEA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp200=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1F8;if(_tmp200->tag != 7)goto _LLEC;}_LLEB:
# 512
 goto _LLED;_LLEC: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp201=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F8;if(_tmp201->tag != 9)goto _LLEE;}_LLED:
 return& Cyc_Tcutil_mk;_LLEE: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F8;if(_tmp202->tag != 16)goto _LLF0;}_LLEF:
 goto _LLF1;_LLF0: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp203=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1F8;if(_tmp203->tag != 13)goto _LLF2;}_LLF1:
 goto _LLF3;_LLF2: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1F8;if(_tmp204->tag != 14)goto _LLF4;}_LLF3:
 goto _LLF5;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp205=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F8;if(_tmp205->tag != 15)goto _LLF6;}_LLF5:
 return& Cyc_Tcutil_bk;_LLF6: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp206=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1F8;if(_tmp206->tag != 21)goto _LLF8;}_LLF7:
 return& Cyc_Tcutil_urk;_LLF8: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1F8;if(_tmp207->tag != 20)goto _LLFA;}_LLF9:
 return& Cyc_Tcutil_rk;_LLFA: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp208=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1F8;if(_tmp208->tag != 22)goto _LLFC;}_LLFB:
 return& Cyc_Tcutil_trk;_LLFC: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F8;if(_tmp209->tag != 3)goto _LLFE;}_LLFD:
# 523
 return& Cyc_Tcutil_ak;_LLFE: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F8;if(_tmp20A->tag != 4)goto _LL100;else{if((((_tmp20A->f1).field_info).KnownDatatypefield).tag != 2)goto _LL100;_tmp20B=((struct _tuple3)(((_tmp20A->f1).field_info).KnownDatatypefield).val).f1;_tmp20C=((struct _tuple3)(((_tmp20A->f1).field_info).KnownDatatypefield).val).f2;}}_LLFF:
# 525
 return& Cyc_Tcutil_mk;_LL100: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F8;if(_tmp20D->tag != 4)goto _LL102;else{if((((_tmp20D->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL102;}}_LL101: {
# 527
const char*_tmpE77;void*_tmpE76;(_tmpE76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE77="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpE77,sizeof(char),39))),_tag_dyneither(_tmpE76,sizeof(void*),0)));}_LL102: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F8;if(_tmp20E->tag != 11)goto _LL104;else{if((((_tmp20E->f1).aggr_info).UnknownAggr).tag != 1)goto _LL104;}}_LL103:
# 530
 return& Cyc_Tcutil_ak;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F8;if(_tmp20F->tag != 11)goto _LL106;else{if((((_tmp20F->f1).aggr_info).KnownAggr).tag != 2)goto _LL106;_tmp210=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp20F->f1).aggr_info).KnownAggr).val))->tvs;_tmp211=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp20F->f1).aggr_info).KnownAggr).val))->impl;_tmp212=(_tmp20F->f1).targs;}}_LL105:
# 532
 if(_tmp211 == 0)return& Cyc_Tcutil_ak;{
# 534
struct Cyc_List_List*_tmp228=_tmp211->fields;
if(_tmp228 == 0)return& Cyc_Tcutil_mk;
for(0;_tmp228->tl != 0;_tmp228=_tmp228->tl){;}{
void*_tmp229=((struct Cyc_Absyn_Aggrfield*)_tmp228->hd)->type;
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(_tmp229);
if(_tmp212 != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 542
struct _RegionHandle _tmp22A=_new_region("temp");struct _RegionHandle*temp=& _tmp22A;_push_region(temp);
{struct Cyc_List_List*_tmp22B=0;
# 545
for(0;_tmp210 != 0;(_tmp210=_tmp210->tl,_tmp212=_tmp212->tl)){
struct Cyc_Absyn_Tvar*_tmp22C=(struct Cyc_Absyn_Tvar*)_tmp210->hd;
void*_tmp22D=(void*)((struct Cyc_List_List*)_check_null(_tmp212))->hd;
struct Cyc_Absyn_Kind*_tmp22E=Cyc_Tcutil_tvar_kind(_tmp22C,& Cyc_Tcutil_bk);_LL121: if(_tmp22E->kind != Cyc_Absyn_IntKind)goto _LL123;_LL122:
 goto _LL124;_LL123: if(_tmp22E->kind != Cyc_Absyn_AnyKind)goto _LL125;_LL124:
# 551
{struct _tuple16*_tmpE7A;struct Cyc_List_List*_tmpE79;_tmp22B=((_tmpE79=_region_malloc(temp,sizeof(*_tmpE79)),((_tmpE79->hd=((_tmpE7A=_region_malloc(temp,sizeof(*_tmpE7A)),((_tmpE7A->f1=_tmp22C,((_tmpE7A->f2=_tmp22D,_tmpE7A)))))),((_tmpE79->tl=_tmp22B,_tmpE79))))));}goto _LL120;_LL125:;_LL126:
 goto _LL120;_LL120:;}
# 555
if(_tmp22B != 0){
_tmp229=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp22B),_tmp229);
k=Cyc_Tcutil_typ_kind(_tmp229);}
# 559
if(k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak){struct Cyc_Absyn_Kind*_tmp231=k;_npop_handler(0);return _tmp231;}}
# 543
;_pop_region(temp);}else{
# 560
if(k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)
return k;}
return& Cyc_Tcutil_mk;};};_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F8;if(_tmp213->tag != 12)goto _LL108;}_LL107:
 return& Cyc_Tcutil_mk;_LL108: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F8;if(_tmp214->tag != 5)goto _LL10A;else{_tmp215=_tmp214->f1;}}_LL109: {
# 565
void*_tmp232=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp215.ptr_atts).bounds);_LL128: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp233=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp232;if(_tmp233->tag != 0)goto _LL12A;}_LL129:
# 567
 switch((Cyc_Tcutil_typ_kind((_tmp215.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL12C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12D:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL12E:
 return& Cyc_Tcutil_tmk;}_LL12A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp234=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp232;if(_tmp234->tag != 1)goto _LL127;}_LL12B:
# 573
 switch((Cyc_Tcutil_typ_kind((_tmp215.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL130:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL131:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL132:
 return& Cyc_Tcutil_tbk;}_LL127:;}_LL10A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1F8;if(_tmp216->tag != 18)goto _LL10C;}_LL10B:
# 579
 return& Cyc_Tcutil_ik;_LL10C: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1F8;if(_tmp217->tag != 19)goto _LL10E;}_LL10D:
 return& Cyc_Tcutil_bk;_LL10E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp218=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F8;if(_tmp218->tag != 8)goto _LL110;else{_tmp219=(_tmp218->f1).num_elts;}}_LL10F:
# 582
 if(_tmp219 == 0  || Cyc_Tcutil_is_const_exp(_tmp219))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL110: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F8;if(_tmp21A->tag != 10)goto _LL112;}_LL111:
 return& Cyc_Tcutil_mk;_LL112: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1F8;if(_tmp21B->tag != 17)goto _LL114;else{_tmp21C=_tmp21B->f3;}}_LL113:
# 586
 if(_tmp21C == 0  || _tmp21C->kind == 0){
const char*_tmpE7E;void*_tmpE7D[1];struct Cyc_String_pa_PrintArg_struct _tmpE7C;(_tmpE7C.tag=0,((_tmpE7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE7D[0]=& _tmpE7C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE7E="typ_kind: typedef found: %s",_tag_dyneither(_tmpE7E,sizeof(char),28))),_tag_dyneither(_tmpE7D,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp21C->kind))->v;_LL114: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F8;if(_tmp21D->tag != 26)goto _LL116;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp21E=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp21D->f1)->r;if(_tmp21E->tag != 0)goto _LL116;}}_LL115:
 return& Cyc_Tcutil_ak;_LL116: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F8;if(_tmp21F->tag != 26)goto _LL118;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp220=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp21F->f1)->r;if(_tmp220->tag != 1)goto _LL118;}}_LL117:
 return& Cyc_Tcutil_bk;_LL118: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F8;if(_tmp221->tag != 26)goto _LL11A;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp222=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp221->f1)->r;if(_tmp222->tag != 2)goto _LL11A;}}_LL119:
 return& Cyc_Tcutil_ak;_LL11A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1F8;if(_tmp223->tag != 23)goto _LL11C;}_LL11B:
 goto _LL11D;_LL11C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1F8;if(_tmp224->tag != 24)goto _LL11E;}_LL11D:
 goto _LL11F;_LL11E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1F8;if(_tmp225->tag != 25)goto _LLE1;}_LL11F:
 return& Cyc_Tcutil_ek;_LLE1:;}
# 598
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 603
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp238;_push_handler(& _tmp238);{int _tmp23A=0;if(setjmp(_tmp238.handler))_tmp23A=1;if(!_tmp23A){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp23B=1;_npop_handler(0);return _tmp23B;};
# 605
;_pop_handler();}else{void*_tmp239=(void*)_exn_thrown;void*_tmp23D=_tmp239;void*_tmp23F;_LL135: {struct Cyc_Tcutil_Unify_exn_struct*_tmp23E=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp23D;if(_tmp23E->tag != Cyc_Tcutil_Unify)goto _LL137;}_LL136:
# 607
 return 0;_LL137: _tmp23F=_tmp23D;_LL138:(void)_throw(_tmp23F);_LL134:;}};}
# 612
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp240=t;struct Cyc_Absyn_Tvar*_tmp242;void*_tmp244;struct Cyc_Core_Opt**_tmp245;struct Cyc_Absyn_PtrInfo _tmp247;void*_tmp249;struct Cyc_List_List*_tmp24B;void*_tmp24C;struct Cyc_Absyn_Tqual _tmp24D;void*_tmp24E;struct Cyc_List_List*_tmp24F;int _tmp250;struct Cyc_Absyn_VarargInfo*_tmp251;struct Cyc_List_List*_tmp252;struct Cyc_List_List*_tmp253;struct Cyc_List_List*_tmp255;struct Cyc_List_List*_tmp257;struct Cyc_List_List*_tmp259;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25F;void*_tmp261;void*_tmp263;void*_tmp265;void*_tmp267;struct Cyc_List_List*_tmp269;_LL13A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp241=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp240;if(_tmp241->tag != 2)goto _LL13C;else{_tmp242=_tmp241->f1;}}_LL13B:
# 617
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp242)){
{const char*_tmpE7F;Cyc_Tcutil_failure_reason=((_tmpE7F="(type variable would escape scope)",_tag_dyneither(_tmpE7F,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 621
goto _LL139;_LL13C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp243=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp240;if(_tmp243->tag != 1)goto _LL13E;else{_tmp244=(void*)_tmp243->f2;_tmp245=(struct Cyc_Core_Opt**)& _tmp243->f4;}}_LL13D:
# 623
 if(t == evar){
{const char*_tmpE80;Cyc_Tcutil_failure_reason=((_tmpE80="(occurs check)",_tag_dyneither(_tmpE80,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 627
if(_tmp244 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp244);else{
# 630
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp245))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 637
if(problem){
struct Cyc_List_List*_tmp26C=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp245))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpE81;_tmp26C=((_tmpE81=_cycalloc(sizeof(*_tmpE81)),((_tmpE81->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpE81->tl=_tmp26C,_tmpE81))))));}}}{
# 643
struct Cyc_Core_Opt*_tmpE82;*_tmp245=((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82->v=_tmp26C,_tmpE82))));};}}}
# 646
goto _LL139;_LL13E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp246=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp240;if(_tmp246->tag != 5)goto _LL140;else{_tmp247=_tmp246->f1;}}_LL13F:
# 648
 Cyc_Tcutil_occurs(evar,r,env,_tmp247.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp247.ptr_atts).rgn);
goto _LL139;_LL140: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp240;if(_tmp248->tag != 8)goto _LL142;else{_tmp249=(_tmp248->f1).elt_type;}}_LL141:
 Cyc_Tcutil_occurs(evar,r,env,_tmp249);goto _LL139;_LL142: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp240;if(_tmp24A->tag != 9)goto _LL144;else{_tmp24B=(_tmp24A->f1).tvars;_tmp24C=(_tmp24A->f1).effect;_tmp24D=(_tmp24A->f1).ret_tqual;_tmp24E=(_tmp24A->f1).ret_typ;_tmp24F=(_tmp24A->f1).args;_tmp250=(_tmp24A->f1).c_varargs;_tmp251=(_tmp24A->f1).cyc_varargs;_tmp252=(_tmp24A->f1).rgn_po;_tmp253=(_tmp24A->f1).attributes;}}_LL143:
# 653
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp24B,env);
if(_tmp24C != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp24C);
Cyc_Tcutil_occurs(evar,r,env,_tmp24E);
for(0;_tmp24F != 0;_tmp24F=_tmp24F->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp24F->hd)).f3);}
if(_tmp251 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp251->type);
for(0;_tmp252 != 0;_tmp252=_tmp252->tl){
void*_tmp270;void*_tmp271;struct _tuple0*_tmp26F=(struct _tuple0*)_tmp252->hd;_tmp270=_tmp26F->f1;_tmp271=_tmp26F->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp270);
Cyc_Tcutil_occurs(evar,r,env,_tmp271);}
# 665
goto _LL139;_LL144: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp254=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp240;if(_tmp254->tag != 10)goto _LL146;else{_tmp255=_tmp254->f1;}}_LL145:
# 667
 for(0;_tmp255 != 0;_tmp255=_tmp255->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp255->hd)).f2);}
goto _LL139;_LL146: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp256=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp240;if(_tmp256->tag != 3)goto _LL148;else{_tmp257=(_tmp256->f1).targs;}}_LL147:
# 671
 Cyc_Tcutil_occurslist(evar,r,env,_tmp257);goto _LL139;_LL148: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp258=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp240;if(_tmp258->tag != 17)goto _LL14A;else{_tmp259=_tmp258->f2;}}_LL149:
 _tmp25B=_tmp259;goto _LL14B;_LL14A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp25A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp240;if(_tmp25A->tag != 4)goto _LL14C;else{_tmp25B=(_tmp25A->f1).targs;}}_LL14B:
 _tmp25D=_tmp25B;goto _LL14D;_LL14C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp25C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp240;if(_tmp25C->tag != 11)goto _LL14E;else{_tmp25D=(_tmp25C->f1).targs;}}_LL14D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp25D);goto _LL139;_LL14E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp25E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp240;if(_tmp25E->tag != 12)goto _LL150;else{_tmp25F=_tmp25E->f2;}}_LL14F:
# 676
 for(0;_tmp25F != 0;_tmp25F=_tmp25F->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp25F->hd)->type);}
goto _LL139;_LL150: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp260=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp240;if(_tmp260->tag != 19)goto _LL152;else{_tmp261=(void*)_tmp260->f1;}}_LL151:
 _tmp263=_tmp261;goto _LL153;_LL152: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp262=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp240;if(_tmp262->tag != 23)goto _LL154;else{_tmp263=(void*)_tmp262->f1;}}_LL153:
 _tmp265=_tmp263;goto _LL155;_LL154: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp264=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp240;if(_tmp264->tag != 15)goto _LL156;else{_tmp265=(void*)_tmp264->f1;}}_LL155:
 _tmp267=_tmp265;goto _LL157;_LL156: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp266=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp240;if(_tmp266->tag != 25)goto _LL158;else{_tmp267=(void*)_tmp266->f1;}}_LL157:
 Cyc_Tcutil_occurs(evar,r,env,_tmp267);goto _LL139;_LL158: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp268=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp240;if(_tmp268->tag != 24)goto _LL15A;else{_tmp269=_tmp268->f1;}}_LL159:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp269);goto _LL139;_LL15A:;_LL15B:
# 685
 goto _LL139;_LL139:;};}
# 688
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 690
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 695
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 703
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpE85;void*_tmpE84;(_tmpE84=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE85="tq1 real_const not set.",_tag_dyneither(_tmpE85,sizeof(char),24))),_tag_dyneither(_tmpE84,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpE88;void*_tmpE87;(_tmpE87=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE88="tq2 real_const not set.",_tag_dyneither(_tmpE88,sizeof(char),24))),_tag_dyneither(_tmpE87,sizeof(void*),0)));}
# 709
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 712
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpE89;Cyc_Tcutil_failure_reason=((_tmpE89="(qualifiers don't match)",_tag_dyneither(_tmpE89,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 720
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 724
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 731
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 733
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp277=x;void*_tmp278;_LL15D: if((_tmp277->No_constr).tag != 3)goto _LL15F;_LL15E:
# 738
{union Cyc_Absyn_Constraint _tmpE8A;*x=(((_tmpE8A.Forward_constr).val=y,(((_tmpE8A.Forward_constr).tag=2,_tmpE8A))));}return;_LL15F: if((_tmp277->Eq_constr).tag != 1)goto _LL161;_tmp278=(void*)(_tmp277->Eq_constr).val;_LL160: {
# 740
union Cyc_Absyn_Constraint*_tmp27A=y;void*_tmp27B;_LL164: if((_tmp27A->No_constr).tag != 3)goto _LL166;_LL165:
*y=*x;return;_LL166: if((_tmp27A->Eq_constr).tag != 1)goto _LL168;_tmp27B=(void*)(_tmp27A->Eq_constr).val;_LL167:
# 743
 if(cmp(_tmp278,_tmp27B)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 747
return;_LL168: if((_tmp27A->Forward_constr).tag != 2)goto _LL163;_LL169: {
const char*_tmpE8D;void*_tmpE8C;(_tmpE8C=0,Cyc_Tcutil_impos(((_tmpE8D="unify_conref: forward after compress(2)",_tag_dyneither(_tmpE8D,sizeof(char),40))),_tag_dyneither(_tmpE8C,sizeof(void*),0)));}_LL163:;}_LL161: if((_tmp277->Forward_constr).tag != 2)goto _LL15C;_LL162: {
# 750
const char*_tmpE90;void*_tmpE8F;(_tmpE8F=0,Cyc_Tcutil_impos(((_tmpE90="unify_conref: forward after compress",_tag_dyneither(_tmpE90,sizeof(char),37))),_tag_dyneither(_tmpE8F,sizeof(void*),0)));}_LL15C:;};}
# 754
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp280;_push_handler(& _tmp280);{int _tmp282=0;if(setjmp(_tmp280.handler))_tmp282=1;if(!_tmp282){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp283=1;_npop_handler(0);return _tmp283;};
# 756
;_pop_handler();}else{void*_tmp281=(void*)_exn_thrown;void*_tmp285=_tmp281;void*_tmp287;_LL16B: {struct Cyc_Tcutil_Unify_exn_struct*_tmp286=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp285;if(_tmp286->tag != Cyc_Tcutil_Unify)goto _LL16D;}_LL16C:
# 758
 return 0;_LL16D: _tmp287=_tmp285;_LL16E:(void)_throw(_tmp287);_LL16A:;}};}
# 761
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpE91;struct _tuple0 _tmp289=(_tmpE91.f1=b1,((_tmpE91.f2=b2,_tmpE91)));struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp291;_LL170:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28A->tag != 0)goto _LL172;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp28B->tag != 0)goto _LL172;};_LL171:
 return 0;_LL172: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28C->tag != 0)goto _LL174;}_LL173:
 return - 1;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp28D->tag != 0)goto _LL176;}_LL175:
 return 1;_LL176:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp28E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28E->tag != 1)goto _LL16F;else{_tmp28F=_tmp28E->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp290=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp290->tag != 1)goto _LL16F;else{_tmp291=_tmp290->f1;}};_LL177:
# 767
 return Cyc_Evexp_const_exp_cmp(_tmp28F,_tmp291);_LL16F:;}
# 771
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpE92;struct _tuple0 _tmp293=(_tmpE92.f1=b1,((_tmpE92.f2=b2,_tmpE92)));struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp29B;_LL179:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp294=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp294->tag != 0)goto _LL17B;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp295=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp295->tag != 0)goto _LL17B;};_LL17A:
 return 0;_LL17B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp296=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp296->tag != 0)goto _LL17D;}_LL17C:
 return - 1;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp297=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp297->tag != 0)goto _LL17F;}_LL17E:
 return 1;_LL17F:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp298=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp298->tag != 1)goto _LL178;else{_tmp299=_tmp298->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp29A->tag != 1)goto _LL178;else{_tmp29B=_tmp29A->f1;}};_LL180:
 return Cyc_Evexp_const_exp_cmp(_tmp299,_tmp29B);_LL178:;}
# 780
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp29C=att;_LL182: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp29D=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp29D->tag != 0)goto _LL184;}_LL183:
 return 0;_LL184: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp29E=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp29E->tag != 1)goto _LL186;}_LL185:
 return 1;_LL186: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp29F=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp29F->tag != 2)goto _LL188;}_LL187:
 return 2;_LL188: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2A0=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A0->tag != 3)goto _LL18A;}_LL189:
 return 3;_LL18A: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2A1=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A1->tag != 4)goto _LL18C;}_LL18B:
 return 4;_LL18C: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2A2=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A2->tag != 5)goto _LL18E;}_LL18D:
 return 5;_LL18E: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2A3=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A3->tag != 6)goto _LL190;}_LL18F:
 return 6;_LL190: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2A4=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A4->tag != 7)goto _LL192;}_LL191:
 return 7;_LL192: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2A5=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A5->tag != 8)goto _LL194;}_LL193:
 return 8;_LL194: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2A6=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A6->tag != 9)goto _LL196;}_LL195:
 return 9;_LL196: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2A7=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A7->tag != 10)goto _LL198;}_LL197:
 return 10;_LL198: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2A8=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A8->tag != 11)goto _LL19A;}_LL199:
 return 11;_LL19A: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2A9=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A9->tag != 12)goto _LL19C;}_LL19B:
 return 12;_LL19C: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2AA=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AA->tag != 13)goto _LL19E;}_LL19D:
 return 13;_LL19E: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2AB=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AB->tag != 14)goto _LL1A0;}_LL19F:
 return 14;_LL1A0: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2AC=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AC->tag != 15)goto _LL1A2;}_LL1A1:
 return 15;_LL1A2: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2AD=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AD->tag != 16)goto _LL1A4;}_LL1A3:
 return 16;_LL1A4: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2AE=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AE->tag != 17)goto _LL1A6;}_LL1A5:
 return 17;_LL1A6: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2AF=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AF->tag != 18)goto _LL1A8;}_LL1A7:
 return 18;_LL1A8: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2B0=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2B0->tag != 19)goto _LL1AA;}_LL1A9:
 return 19;_LL1AA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2B1->tag != 20)goto _LL1AC;}_LL1AB:
 return 20;_LL1AC:;_LL1AD:
 return 21;_LL181:;}
# 807
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpE93;struct _tuple0 _tmp2B3=(_tmpE93.f1=att1,((_tmpE93.f2=att2,_tmpE93)));int _tmp2B5;int _tmp2B7;int _tmp2B9;int _tmp2BB;int _tmp2BD;int _tmp2BF;struct _dyneither_ptr _tmp2C1;struct _dyneither_ptr _tmp2C3;enum Cyc_Absyn_Format_Type _tmp2C5;int _tmp2C6;int _tmp2C7;enum Cyc_Absyn_Format_Type _tmp2C9;int _tmp2CA;int _tmp2CB;_LL1AF:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B4=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2B4->tag != 0)goto _LL1B1;else{_tmp2B5=_tmp2B4->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B6=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2B6->tag != 0)goto _LL1B1;else{_tmp2B7=_tmp2B6->f1;}};_LL1B0:
 _tmp2B9=_tmp2B5;_tmp2BB=_tmp2B7;goto _LL1B2;_LL1B1:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2B8->tag != 20)goto _LL1B3;else{_tmp2B9=_tmp2B8->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2BA=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2BA->tag != 20)goto _LL1B3;else{_tmp2BB=_tmp2BA->f1;}};_LL1B2:
 _tmp2BD=_tmp2B9;_tmp2BF=_tmp2BB;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2BC->tag != 6)goto _LL1B5;else{_tmp2BD=_tmp2BC->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2BE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2BE->tag != 6)goto _LL1B5;else{_tmp2BF=_tmp2BE->f1;}};_LL1B4:
 return Cyc_Core_intcmp(_tmp2BD,_tmp2BF);_LL1B5:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C0=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2C0->tag != 8)goto _LL1B7;else{_tmp2C1=_tmp2C0->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C2=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2C2->tag != 8)goto _LL1B7;else{_tmp2C3=_tmp2C2->f1;}};_LL1B6:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2C1,(struct _dyneither_ptr)_tmp2C3);_LL1B7:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C4=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2C4->tag != 19)goto _LL1B9;else{_tmp2C5=_tmp2C4->f1;_tmp2C6=_tmp2C4->f2;_tmp2C7=_tmp2C4->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C8=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2C8->tag != 19)goto _LL1B9;else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C8->f2;_tmp2CB=_tmp2C8->f3;}};_LL1B8: {
# 814
int _tmp2CC=Cyc_Core_intcmp((int)((unsigned int)_tmp2C5),(int)((unsigned int)_tmp2C9));
if(_tmp2CC != 0)return _tmp2CC;{
int _tmp2CD=Cyc_Core_intcmp(_tmp2C6,_tmp2CA);
if(_tmp2CD != 0)return _tmp2CD;
return Cyc_Core_intcmp(_tmp2C7,_tmp2CB);};}_LL1B9:;_LL1BA:
# 820
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1AE:;}
# 824
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 828
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 837
static void*Cyc_Tcutil_rgns_of(void*t);
# 839
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 843
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp2CE=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);_LL1BC: if(_tmp2CE->kind != Cyc_Absyn_RgnKind)goto _LL1BE;if(_tmp2CE->aliasqual != Cyc_Absyn_Unique)goto _LL1BE;_LL1BD:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BB;_LL1BE: if(_tmp2CE->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2CE->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BB;_LL1C0: if(_tmp2CE->kind != Cyc_Absyn_EffKind)goto _LL1C2;_LL1C1:
 t=Cyc_Absyn_empty_effect;goto _LL1BB;_LL1C2: if(_tmp2CE->kind != Cyc_Absyn_IntKind)goto _LL1C4;_LL1C3:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpE96;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpE95;t=(void*)((_tmpE95=_cycalloc(sizeof(*_tmpE95)),((_tmpE95[0]=((_tmpE96.tag=18,((_tmpE96.f1=Cyc_Absyn_uint_exp(0,0),_tmpE96)))),_tmpE95))));}goto _LL1BB;_LL1C4:;_LL1C5:
 t=Cyc_Absyn_sint_typ;goto _LL1BB;_LL1BB:;}{
# 852
struct _tuple16*_tmpE97;return(_tmpE97=_cycalloc(sizeof(*_tmpE97)),((_tmpE97->f1=tv,((_tmpE97->f2=t,_tmpE97)))));};}
# 859
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp2D2=Cyc_Tcutil_compress(t);void*_tmp2DC;void*_tmp2DE;void*_tmp2DF;struct Cyc_List_List*_tmp2E1;void*_tmp2E3;void*_tmp2E4;void*_tmp2E6;struct Cyc_List_List*_tmp2E8;struct Cyc_List_List*_tmp2EA;struct Cyc_List_List*_tmp2EC;struct Cyc_List_List*_tmp2EE;struct Cyc_List_List*_tmp2F1;void*_tmp2F2;struct Cyc_Absyn_Tqual _tmp2F3;void*_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_Absyn_VarargInfo*_tmp2F6;struct Cyc_List_List*_tmp2F7;void*_tmp2FE;struct Cyc_List_List*_tmp300;_LL1C7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2D3=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D3->tag != 0)goto _LL1C9;}_LL1C8:
 goto _LL1CA;_LL1C9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2D4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D4->tag != 7)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2D5=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D5->tag != 13)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2D6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D6->tag != 14)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2D7=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D7->tag != 18)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2D8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D8->tag != 6)goto _LL1D3;}_LL1D2:
 return Cyc_Absyn_empty_effect;_LL1D3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2D9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D2;if(_tmp2D9->tag != 1)goto _LL1D5;}_LL1D4:
 goto _LL1D6;_LL1D5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D2;if(_tmp2DA->tag != 2)goto _LL1D7;}_LL1D6: {
# 869
struct Cyc_Absyn_Kind*_tmp302=Cyc_Tcutil_typ_kind(t);_LL1FE: if(_tmp302->kind != Cyc_Absyn_RgnKind)goto _LL200;_LL1FF: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE9A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE99;return(void*)((_tmpE99=_cycalloc(sizeof(*_tmpE99)),((_tmpE99[0]=((_tmpE9A.tag=23,((_tmpE9A.f1=t,_tmpE9A)))),_tmpE99))));}_LL200: if(_tmp302->kind != Cyc_Absyn_EffKind)goto _LL202;_LL201:
 return t;_LL202: if(_tmp302->kind != Cyc_Absyn_IntKind)goto _LL204;_LL203:
 return Cyc_Absyn_empty_effect;_LL204:;_LL205: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE9D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpE9C;return(void*)((_tmpE9C=_cycalloc(sizeof(*_tmpE9C)),((_tmpE9C[0]=((_tmpE9D.tag=25,((_tmpE9D.f1=t,_tmpE9D)))),_tmpE9C))));}_LL1FD:;}_LL1D7: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2DB=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2D2;if(_tmp2DB->tag != 15)goto _LL1D9;else{_tmp2DC=(void*)_tmp2DB->f1;}}_LL1D8: {
# 875
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEA0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE9F;return(void*)((_tmpE9F=_cycalloc(sizeof(*_tmpE9F)),((_tmpE9F[0]=((_tmpEA0.tag=23,((_tmpEA0.f1=_tmp2DC,_tmpEA0)))),_tmpE9F))));}_LL1D9: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2DD=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2D2;if(_tmp2DD->tag != 16)goto _LL1DB;else{_tmp2DE=(void*)_tmp2DD->f1;_tmp2DF=(void*)_tmp2DD->f2;}}_LL1DA: {
# 879
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEA3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEA2;return(void*)((_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2[0]=((_tmpEA3.tag=23,((_tmpEA3.f1=_tmp2DF,_tmpEA3)))),_tmpEA2))));}_LL1DB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2E0=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2D2;if(_tmp2E0->tag != 3)goto _LL1DD;else{_tmp2E1=(_tmp2E0->f1).targs;}}_LL1DC: {
# 881
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2E1);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEA6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEA5;return Cyc_Tcutil_normalize_effect((void*)((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5[0]=((_tmpEA6.tag=24,((_tmpEA6.f1=ts,_tmpEA6)))),_tmpEA5)))));}_LL1DD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D2;if(_tmp2E2->tag != 5)goto _LL1DF;else{_tmp2E3=(_tmp2E2->f1).elt_typ;_tmp2E4=((_tmp2E2->f1).ptr_atts).rgn;}}_LL1DE: {
# 885
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEB5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEB4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEB3;void*_tmpEB2[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEB1;return Cyc_Tcutil_normalize_effect((void*)((_tmpEB1=_cycalloc(sizeof(*_tmpEB1)),((_tmpEB1[0]=((_tmpEB5.tag=24,((_tmpEB5.f1=((_tmpEB2[1]=Cyc_Tcutil_rgns_of(_tmp2E3),((_tmpEB2[0]=(void*)((_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4[0]=((_tmpEB3.tag=23,((_tmpEB3.f1=_tmp2E4,_tmpEB3)))),_tmpEB4)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEB2,sizeof(void*),2)))))),_tmpEB5)))),_tmpEB1)))));}_LL1DF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2E5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2;if(_tmp2E5->tag != 8)goto _LL1E1;else{_tmp2E6=(_tmp2E5->f1).elt_type;}}_LL1E0:
# 887
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2E6));_LL1E1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2E7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D2;if(_tmp2E7->tag != 10)goto _LL1E3;else{_tmp2E8=_tmp2E7->f1;}}_LL1E2: {
# 889
struct Cyc_List_List*_tmp312=0;
for(0;_tmp2E8 != 0;_tmp2E8=_tmp2E8->tl){
struct Cyc_List_List*_tmpEB6;_tmp312=((_tmpEB6=_cycalloc(sizeof(*_tmpEB6)),((_tmpEB6->hd=(*((struct _tuple12*)_tmp2E8->hd)).f2,((_tmpEB6->tl=_tmp312,_tmpEB6))))));}
_tmp2EA=_tmp312;goto _LL1E4;}_LL1E3: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2E9=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2D2;if(_tmp2E9->tag != 4)goto _LL1E5;else{_tmp2EA=(_tmp2E9->f1).targs;}}_LL1E4:
 _tmp2EC=_tmp2EA;goto _LL1E6;_LL1E5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2EB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D2;if(_tmp2EB->tag != 11)goto _LL1E7;else{_tmp2EC=(_tmp2EB->f1).targs;}}_LL1E6: {
# 895
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEB9;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEB8;return Cyc_Tcutil_normalize_effect((void*)((_tmpEB8=_cycalloc(sizeof(*_tmpEB8)),((_tmpEB8[0]=((_tmpEB9.tag=24,((_tmpEB9.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2EC),_tmpEB9)))),_tmpEB8)))));}_LL1E7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2ED=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D2;if(_tmp2ED->tag != 12)goto _LL1E9;else{_tmp2EE=_tmp2ED->f2;}}_LL1E8: {
# 897
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEBC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEBB;return Cyc_Tcutil_normalize_effect((void*)((_tmpEBB=_cycalloc(sizeof(*_tmpEBB)),((_tmpEBB[0]=((_tmpEBC.tag=24,((_tmpEBC.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp2EE),_tmpEBC)))),_tmpEBB)))));}_LL1E9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2EF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2D2;if(_tmp2EF->tag != 19)goto _LL1EB;}_LL1EA:
 return Cyc_Absyn_empty_effect;_LL1EB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D2;if(_tmp2F0->tag != 9)goto _LL1ED;else{_tmp2F1=(_tmp2F0->f1).tvars;_tmp2F2=(_tmp2F0->f1).effect;_tmp2F3=(_tmp2F0->f1).ret_tqual;_tmp2F4=(_tmp2F0->f1).ret_typ;_tmp2F5=(_tmp2F0->f1).args;_tmp2F6=(_tmp2F0->f1).cyc_varargs;_tmp2F7=(_tmp2F0->f1).rgn_po;}}_LL1EC: {
# 907
void*_tmp318=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp2F1),(void*)_check_null(_tmp2F2));
return Cyc_Tcutil_normalize_effect(_tmp318);}_LL1ED: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2F8=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2D2;if(_tmp2F8->tag != 21)goto _LL1EF;}_LL1EE:
 goto _LL1F0;_LL1EF: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2F9=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2D2;if(_tmp2F9->tag != 22)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2D2;if(_tmp2FA->tag != 20)goto _LL1F3;}_LL1F2:
 return Cyc_Absyn_empty_effect;_LL1F3: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2D2;if(_tmp2FB->tag != 23)goto _LL1F5;}_LL1F4:
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D2;if(_tmp2FC->tag != 24)goto _LL1F7;}_LL1F6:
 return t;_LL1F7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2FD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D2;if(_tmp2FD->tag != 25)goto _LL1F9;else{_tmp2FE=(void*)_tmp2FD->f1;}}_LL1F8:
 return Cyc_Tcutil_rgns_of(_tmp2FE);_LL1F9: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp2FF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D2;if(_tmp2FF->tag != 17)goto _LL1FB;else{_tmp300=_tmp2FF->f2;}}_LL1FA: {
# 916
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEBF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEBE;return Cyc_Tcutil_normalize_effect((void*)((_tmpEBE=_cycalloc(sizeof(*_tmpEBE)),((_tmpEBE[0]=((_tmpEBF.tag=24,((_tmpEBF.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp300),_tmpEBF)))),_tmpEBE)))));}_LL1FB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp301=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2D2;if(_tmp301->tag != 26)goto _LL1C6;}_LL1FC: {
const char*_tmpEC2;void*_tmpEC1;(_tmpEC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEC2="typedecl in rgns_of",_tag_dyneither(_tmpEC2,sizeof(char),20))),_tag_dyneither(_tmpEC1,sizeof(void*),0)));}_LL1C6:;}
# 924
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp31D=e;struct Cyc_List_List**_tmp31F;void*_tmp321;_LL207: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp31E=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp31D;if(_tmp31E->tag != 24)goto _LL209;else{_tmp31F=(struct Cyc_List_List**)& _tmp31E->f1;}}_LL208: {
# 928
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp31F;for(0;effs != 0;effs=effs->tl){
void*_tmp322=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp322));{
void*_tmp323=(void*)effs->hd;_LL20E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp324=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp323;if(_tmp324->tag != 24)goto _LL210;}_LL20F:
 goto _LL211;_LL210: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp325=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp323;if(_tmp325->tag != 23)goto _LL212;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp325->f1);if(_tmp326->tag != 20)goto _LL212;}}_LL211:
 redo_join=1;goto _LL20D;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp323;if(_tmp327->tag != 23)goto _LL214;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp327->f1);if(_tmp328->tag != 22)goto _LL214;}}_LL213:
 redo_join=1;goto _LL20D;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp329=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp323;if(_tmp329->tag != 23)goto _LL216;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp329->f1);if(_tmp32A->tag != 21)goto _LL216;}}_LL215:
 redo_join=1;goto _LL20D;_LL216:;_LL217:
 goto _LL20D;_LL20D:;};}}
# 940
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp31F;for(0;effs != 0;effs=effs->tl){
void*_tmp32B=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp32D;void*_tmp334;_LL219: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp32B;if(_tmp32C->tag != 24)goto _LL21B;else{_tmp32D=_tmp32C->f1;}}_LL21A:
# 945
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp32D,effects);
goto _LL218;_LL21B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32B;if(_tmp32E->tag != 23)goto _LL21D;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp32E->f1);if(_tmp32F->tag != 20)goto _LL21D;}}_LL21C:
 goto _LL218;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32B;if(_tmp330->tag != 23)goto _LL21F;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp330->f1);if(_tmp331->tag != 22)goto _LL21F;}}_LL21E:
 goto _LL218;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32B;if(_tmp332->tag != 23)goto _LL221;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp333=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp332->f1);if(_tmp333->tag != 21)goto _LL221;}}_LL220:
 goto _LL218;_LL221: _tmp334=_tmp32B;_LL222:
{struct Cyc_List_List*_tmpEC3;effects=((_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((_tmpEC3->hd=_tmp334,((_tmpEC3->tl=effects,_tmpEC3))))));}goto _LL218;_LL218:;}}
# 953
*_tmp31F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL209: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp320=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp31D;if(_tmp320->tag != 25)goto _LL20B;else{_tmp321=(void*)_tmp320->f1;}}_LL20A: {
# 956
void*_tmp336=Cyc_Tcutil_compress(_tmp321);_LL224: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp337=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp336;if(_tmp337->tag != 1)goto _LL226;}_LL225:
 goto _LL227;_LL226: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp338=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp336;if(_tmp338->tag != 2)goto _LL228;}_LL227:
 return e;_LL228:;_LL229:
 return Cyc_Tcutil_rgns_of(_tmp321);_LL223:;}_LL20B:;_LL20C:
# 961
 return e;_LL206:;};}
# 966
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpEC9;struct Cyc_Absyn_FnInfo _tmpEC8;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpEC7;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp339=(_tmpEC7=_cycalloc(sizeof(*_tmpEC7)),((_tmpEC7[0]=((_tmpEC9.tag=9,((_tmpEC9.f1=((_tmpEC8.tvars=0,((_tmpEC8.effect=eff,((_tmpEC8.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpEC8.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpEC8.args=0,((_tmpEC8.c_varargs=0,((_tmpEC8.cyc_varargs=0,((_tmpEC8.rgn_po=0,((_tmpEC8.attributes=0,_tmpEC8)))))))))))))))))),_tmpEC9)))),_tmpEC7)));
# 972
return Cyc_Absyn_atb_typ((void*)_tmp339,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 979
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp33D=Cyc_Tcutil_compress(e);void*_tmp33F;struct Cyc_List_List*_tmp341;void*_tmp343;struct Cyc_Core_Opt*_tmp345;void**_tmp346;struct Cyc_Core_Opt*_tmp347;_LL22B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33D;if(_tmp33E->tag != 23)goto _LL22D;else{_tmp33F=(void*)_tmp33E->f1;}}_LL22C:
# 986
 if(constrain)return Cyc_Tcutil_unify(r,_tmp33F);
_tmp33F=Cyc_Tcutil_compress(_tmp33F);
if(r == _tmp33F)return 1;{
struct _tuple0 _tmpECA;struct _tuple0 _tmp349=(_tmpECA.f1=r,((_tmpECA.f2=_tmp33F,_tmpECA)));struct Cyc_Absyn_Tvar*_tmp34B;struct Cyc_Absyn_Tvar*_tmp34D;_LL236:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp349.f1;if(_tmp34A->tag != 2)goto _LL238;else{_tmp34B=_tmp34A->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp349.f2;if(_tmp34C->tag != 2)goto _LL238;else{_tmp34D=_tmp34C->f1;}};_LL237:
 return Cyc_Absyn_tvar_cmp(_tmp34B,_tmp34D)== 0;_LL238:;_LL239:
 return 0;_LL235:;};_LL22D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp33D;if(_tmp340->tag != 24)goto _LL22F;else{_tmp341=_tmp340->f1;}}_LL22E:
# 994
 for(0;_tmp341 != 0;_tmp341=_tmp341->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp341->hd))return 1;}
return 0;_LL22F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp33D;if(_tmp342->tag != 25)goto _LL231;else{_tmp343=(void*)_tmp342->f1;}}_LL230: {
# 998
void*_tmp34E=Cyc_Tcutil_rgns_of(_tmp343);void*_tmp350;void*_tmp351;_LL23B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp34F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp34E;if(_tmp34F->tag != 25)goto _LL23D;else{_tmp350=(void*)_tmp34F->f1;}}_LL23C:
# 1000
 if(!constrain)return 0;{
void*_tmp352=Cyc_Tcutil_compress(_tmp350);struct Cyc_Core_Opt*_tmp354;void**_tmp355;struct Cyc_Core_Opt*_tmp356;_LL240: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp353=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp352;if(_tmp353->tag != 1)goto _LL242;else{_tmp354=_tmp353->f1;_tmp355=(void**)((void**)& _tmp353->f2);_tmp356=_tmp353->f4;}}_LL241: {
# 1005
void*_tmp357=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp356);
# 1008
Cyc_Tcutil_occurs(_tmp357,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp356))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpED9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpED8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpED7;void*_tmpED6[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpED5;void*_tmp358=Cyc_Tcutil_dummy_fntype((void*)((_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5[0]=((_tmpED9.tag=24,((_tmpED9.f1=((_tmpED6[1]=(void*)((_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8[0]=((_tmpED7.tag=23,((_tmpED7.f1=r,_tmpED7)))),_tmpED8)))),((_tmpED6[0]=_tmp357,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpED6,sizeof(void*),2)))))),_tmpED9)))),_tmpED5)))));
*_tmp355=_tmp358;
return 1;};}_LL242:;_LL243:
 return 0;_LL23F:;};_LL23D: _tmp351=_tmp34E;_LL23E:
# 1014
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp351);_LL23A:;}_LL231: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33D;if(_tmp344->tag != 1)goto _LL233;else{_tmp345=_tmp344->f1;_tmp346=(void**)((void**)& _tmp344->f2);_tmp347=_tmp344->f4;}}_LL232:
# 1017
 if(_tmp345 == 0  || ((struct Cyc_Absyn_Kind*)_tmp345->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpEDC;void*_tmpEDB;(_tmpEDB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEDC="effect evar has wrong kind",_tag_dyneither(_tmpEDC,sizeof(char),27))),_tag_dyneither(_tmpEDB,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1022
void*_tmp360=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp347);
# 1025
Cyc_Tcutil_occurs(_tmp360,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp347))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEF1;struct Cyc_List_List*_tmpEF0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEEF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEEE;struct Cyc_List_List*_tmpEED;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEEC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp361=(_tmpEEC=_cycalloc(sizeof(*_tmpEEC)),((_tmpEEC[0]=((_tmpEF1.tag=24,((_tmpEF1.f1=((_tmpEED=_cycalloc(sizeof(*_tmpEED)),((_tmpEED->hd=_tmp360,((_tmpEED->tl=((_tmpEF0=_cycalloc(sizeof(*_tmpEF0)),((_tmpEF0->hd=(void*)((_tmpEEE=_cycalloc(sizeof(*_tmpEEE)),((_tmpEEE[0]=((_tmpEEF.tag=23,((_tmpEEF.f1=r,_tmpEEF)))),_tmpEEE)))),((_tmpEF0->tl=0,_tmpEF0)))))),_tmpEED)))))),_tmpEF1)))),_tmpEEC)));
*_tmp346=(void*)_tmp361;
return 1;};};_LL233:;_LL234:
 return 0;_LL22A:;};}
# 1036
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp368=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp36B;void*_tmp36D;struct Cyc_Core_Opt*_tmp36F;void**_tmp370;struct Cyc_Core_Opt*_tmp371;_LL245: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp368;if(_tmp369->tag != 23)goto _LL247;}_LL246:
 return 0;_LL247: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp36A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp368;if(_tmp36A->tag != 24)goto _LL249;else{_tmp36B=_tmp36A->f1;}}_LL248:
# 1041
 for(0;_tmp36B != 0;_tmp36B=_tmp36B->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp36B->hd))
return 1;}
return 0;_LL249: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp368;if(_tmp36C->tag != 25)goto _LL24B;else{_tmp36D=(void*)_tmp36C->f1;}}_LL24A:
# 1046
 _tmp36D=Cyc_Tcutil_compress(_tmp36D);
if(t == _tmp36D)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp36D);{
void*_tmp372=Cyc_Tcutil_rgns_of(t);void*_tmp374;void*_tmp375;_LL250: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp372;if(_tmp373->tag != 25)goto _LL252;else{_tmp374=(void*)_tmp373->f1;}}_LL251: {
# 1051
struct _tuple0 _tmpEF2;struct _tuple0 _tmp377=(_tmpEF2.f1=Cyc_Tcutil_compress(_tmp374),((_tmpEF2.f2=_tmp36D,_tmpEF2)));struct Cyc_Absyn_Tvar*_tmp379;struct Cyc_Absyn_Tvar*_tmp37B;_LL255:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp378=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp377.f1;if(_tmp378->tag != 2)goto _LL257;else{_tmp379=_tmp378->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp377.f2;if(_tmp37A->tag != 2)goto _LL257;else{_tmp37B=_tmp37A->f1;}};_LL256:
 return Cyc_Tcutil_unify(t,_tmp36D);_LL257:;_LL258:
 return _tmp374 == _tmp36D;_LL254:;}_LL252: _tmp375=_tmp372;_LL253:
# 1055
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp375);_LL24F:;};_LL24B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp36E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp368;if(_tmp36E->tag != 1)goto _LL24D;else{_tmp36F=_tmp36E->f1;_tmp370=(void**)((void**)& _tmp36E->f2);_tmp371=_tmp36E->f4;}}_LL24C:
# 1058
 if(_tmp36F == 0  || ((struct Cyc_Absyn_Kind*)_tmp36F->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpEF5;void*_tmpEF4;(_tmpEF4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEF5="effect evar has wrong kind",_tag_dyneither(_tmpEF5,sizeof(char),27))),_tag_dyneither(_tmpEF4,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1063
void*_tmp37E=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp371);
# 1066
Cyc_Tcutil_occurs(_tmp37E,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp371))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF0A;struct Cyc_List_List*_tmpF09;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF08;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF07;struct Cyc_List_List*_tmpF06;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF05;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp37F=(_tmpF05=_cycalloc(sizeof(*_tmpF05)),((_tmpF05[0]=((_tmpF0A.tag=24,((_tmpF0A.f1=((_tmpF06=_cycalloc(sizeof(*_tmpF06)),((_tmpF06->hd=_tmp37E,((_tmpF06->tl=((_tmpF09=_cycalloc(sizeof(*_tmpF09)),((_tmpF09->hd=(void*)((_tmpF07=_cycalloc(sizeof(*_tmpF07)),((_tmpF07[0]=((_tmpF08.tag=25,((_tmpF08.f1=t,_tmpF08)))),_tmpF07)))),((_tmpF09->tl=0,_tmpF09)))))),_tmpF06)))))),_tmpF0A)))),_tmpF05)));
*_tmp370=(void*)_tmp37F;
return 1;};};_LL24D:;_LL24E:
 return 0;_LL244:;};}
# 1077
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp386=e;struct Cyc_Absyn_Tvar*_tmp388;struct Cyc_List_List*_tmp38A;void*_tmp38C;struct Cyc_Core_Opt*_tmp38E;void**_tmp38F;struct Cyc_Core_Opt*_tmp390;_LL25A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp387=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp386;if(_tmp387->tag != 2)goto _LL25C;else{_tmp388=_tmp387->f1;}}_LL25B:
 return Cyc_Absyn_tvar_cmp(v,_tmp388)== 0;_LL25C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp389=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp386;if(_tmp389->tag != 24)goto _LL25E;else{_tmp38A=_tmp389->f1;}}_LL25D:
# 1082
 for(0;_tmp38A != 0;_tmp38A=_tmp38A->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp38A->hd))
return 1;}
return 0;_LL25E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp38B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp386;if(_tmp38B->tag != 25)goto _LL260;else{_tmp38C=(void*)_tmp38B->f1;}}_LL25F: {
# 1087
void*_tmp391=Cyc_Tcutil_rgns_of(_tmp38C);void*_tmp393;void*_tmp394;_LL265: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp392=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp391;if(_tmp392->tag != 25)goto _LL267;else{_tmp393=(void*)_tmp392->f1;}}_LL266:
# 1089
 if(!may_constrain_evars)return 0;{
void*_tmp395=Cyc_Tcutil_compress(_tmp393);struct Cyc_Core_Opt*_tmp397;void**_tmp398;struct Cyc_Core_Opt*_tmp399;_LL26A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp396=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp395;if(_tmp396->tag != 1)goto _LL26C;else{_tmp397=_tmp396->f1;_tmp398=(void**)((void**)& _tmp396->f2);_tmp399=_tmp396->f4;}}_LL26B: {
# 1095
void*_tmp39A=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp399);
# 1097
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp399))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF19;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF18;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF17;void*_tmpF16[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF15;void*_tmp39B=Cyc_Tcutil_dummy_fntype((void*)((_tmpF15=_cycalloc(sizeof(*_tmpF15)),((_tmpF15[0]=((_tmpF19.tag=24,((_tmpF19.f1=((_tmpF16[1]=(void*)((_tmpF18=_cycalloc(sizeof(*_tmpF18)),((_tmpF18[0]=((_tmpF17.tag=2,((_tmpF17.f1=v,_tmpF17)))),_tmpF18)))),((_tmpF16[0]=_tmp39A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF16,sizeof(void*),2)))))),_tmpF19)))),_tmpF15)))));
*_tmp398=_tmp39B;
return 1;};}_LL26C:;_LL26D:
 return 0;_LL269:;};_LL267: _tmp394=_tmp391;_LL268:
# 1103
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp394);_LL264:;}_LL260: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp386;if(_tmp38D->tag != 1)goto _LL262;else{_tmp38E=_tmp38D->f1;_tmp38F=(void**)((void**)& _tmp38D->f2);_tmp390=_tmp38D->f4;}}_LL261:
# 1106
 if(_tmp38E == 0  || ((struct Cyc_Absyn_Kind*)_tmp38E->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF1C;void*_tmpF1B;(_tmpF1B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF1C="effect evar has wrong kind",_tag_dyneither(_tmpF1C,sizeof(char),27))),_tag_dyneither(_tmpF1B,sizeof(void*),0)));}{
# 1110
void*_tmp3A3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp390);
# 1112
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp390))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF31;struct Cyc_List_List*_tmpF30;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF2F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF2E;struct Cyc_List_List*_tmpF2D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF2C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3A4=(_tmpF2C=_cycalloc(sizeof(*_tmpF2C)),((_tmpF2C[0]=((_tmpF31.tag=24,((_tmpF31.f1=((_tmpF2D=_cycalloc(sizeof(*_tmpF2D)),((_tmpF2D->hd=_tmp3A3,((_tmpF2D->tl=((_tmpF30=_cycalloc(sizeof(*_tmpF30)),((_tmpF30->hd=(void*)((_tmpF2E=_cycalloc(sizeof(*_tmpF2E)),((_tmpF2E[0]=((_tmpF2F.tag=2,((_tmpF2F.f1=v,_tmpF2F)))),_tmpF2E)))),((_tmpF30->tl=0,_tmpF30)))))),_tmpF2D)))))),_tmpF31)))),_tmpF2C)));
*_tmp38F=(void*)_tmp3A4;
return 1;};};_LL262:;_LL263:
 return 0;_LL259:;};}
# 1122
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3AB=e;struct Cyc_List_List*_tmp3AD;void*_tmp3AF;_LL26F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AC=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3AB;if(_tmp3AC->tag != 24)goto _LL271;else{_tmp3AD=_tmp3AC->f1;}}_LL270:
# 1126
 for(0;_tmp3AD != 0;_tmp3AD=_tmp3AD->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3AD->hd))
return 1;}
return 0;_LL271: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3AE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3AB;if(_tmp3AE->tag != 25)goto _LL273;else{_tmp3AF=(void*)_tmp3AE->f1;}}_LL272: {
# 1131
void*_tmp3B1=Cyc_Tcutil_rgns_of(_tmp3AF);void*_tmp3B3;void*_tmp3B4;_LL278: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B2=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B1;if(_tmp3B2->tag != 25)goto _LL27A;else{_tmp3B3=(void*)_tmp3B2->f1;}}_LL279:
 return 0;_LL27A: _tmp3B4=_tmp3B1;_LL27B:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3B4);_LL277:;}_LL273: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3AB;if(_tmp3B0->tag != 1)goto _LL275;}_LL274:
# 1135
 return evar == e;_LL275:;_LL276:
 return 0;_LL26E:;};}
# 1149 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1152
void*_tmp3B5=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp3B7;void*_tmp3B9;struct Cyc_Absyn_Tvar*_tmp3BB;void*_tmp3BD;void**_tmp3BF;struct Cyc_Core_Opt*_tmp3C0;_LL27D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B6=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B5;if(_tmp3B6->tag != 24)goto _LL27F;else{_tmp3B7=_tmp3B6->f1;}}_LL27E:
# 1154
 for(0;_tmp3B7 != 0;_tmp3B7=_tmp3B7->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3B7->hd,e2))
return 0;}
return 1;_LL27F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3B8=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B5;if(_tmp3B8->tag != 23)goto _LL281;else{_tmp3B9=(void*)_tmp3B8->f1;}}_LL280:
# 1165
 return Cyc_Tcutil_region_in_effect(0,_tmp3B9,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3B9,(void*)& Cyc_Absyn_HeapRgn_val);_LL281: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3BA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3B5;if(_tmp3BA->tag != 2)goto _LL283;else{_tmp3BB=_tmp3BA->f1;}}_LL282:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3BB,e2);_LL283: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BC=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B5;if(_tmp3BC->tag != 25)goto _LL285;else{_tmp3BD=(void*)_tmp3BC->f1;}}_LL284: {
# 1169
void*_tmp3C1=Cyc_Tcutil_rgns_of(_tmp3BD);void*_tmp3C3;void*_tmp3C4;_LL28A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C1;if(_tmp3C2->tag != 25)goto _LL28C;else{_tmp3C3=(void*)_tmp3C2->f1;}}_LL28B:
# 1174
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3C3,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3C3,Cyc_Absyn_sint_typ);_LL28C: _tmp3C4=_tmp3C1;_LL28D:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3C4,e2);_LL289:;}_LL285: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3BE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B5;if(_tmp3BE->tag != 1)goto _LL287;else{_tmp3BF=(void**)((void**)& _tmp3BE->f2);_tmp3C0=_tmp3BE->f4;}}_LL286:
# 1179
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1183
*_tmp3BF=Cyc_Absyn_empty_effect;
# 1187
return 1;_LL287:;_LL288: {
const char*_tmpF35;void*_tmpF34[1];struct Cyc_String_pa_PrintArg_struct _tmpF33;(_tmpF33.tag=0,((_tmpF33.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpF34[0]=& _tmpF33,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF35="subset_effect: bad effect: %s",_tag_dyneither(_tmpF35,sizeof(char),30))),_tag_dyneither(_tmpF34,sizeof(void*),1)))))));}_LL27C:;}
# 1203 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1219
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1221
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
void*_tmp3C9;void*_tmp3CA;struct _tuple0*_tmp3C8=(struct _tuple0*)r1->hd;_tmp3C9=_tmp3C8->f1;_tmp3CA=_tmp3C8->f2;{
int found=_tmp3C9 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
void*_tmp3CC;void*_tmp3CD;struct _tuple0*_tmp3CB=(struct _tuple0*)r2->hd;_tmp3CC=_tmp3CB->f1;_tmp3CD=_tmp3CB->f2;
if(Cyc_Tcutil_unify(_tmp3C9,_tmp3CC) && Cyc_Tcutil_unify(_tmp3CA,_tmp3CD)){
found=1;
break;}}}
# 1231
if(!found)return 0;};}}
# 1233
return 1;}
# 1237
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1239
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1243
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp3CE=t1;struct Cyc_Core_Opt*_tmp3D0;void**_tmp3D1;struct Cyc_Core_Opt*_tmp3D2;_LL28F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3CF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3CE;if(_tmp3CF->tag != 1)goto _LL291;else{_tmp3D0=_tmp3CF->f1;_tmp3D1=(void**)((void**)& _tmp3CF->f2);_tmp3D2=_tmp3CF->f4;}}_LL290:
# 1254
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D2))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3D3=Cyc_Tcutil_typ_kind(t2);
# 1259
if(Cyc_Tcutil_kind_leq(_tmp3D3,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3D0))->v)){
*_tmp3D1=t2;
return;}else{
# 1263
{void*_tmp3D4=t2;void**_tmp3D6;struct Cyc_Core_Opt*_tmp3D7;struct Cyc_Absyn_PtrInfo _tmp3D9;_LL294: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D4;if(_tmp3D5->tag != 1)goto _LL296;else{_tmp3D6=(void**)((void**)& _tmp3D5->f2);_tmp3D7=_tmp3D5->f4;}}_LL295: {
# 1266
struct Cyc_List_List*_tmp3DA=(struct Cyc_List_List*)_tmp3D2->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D7))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp3DA,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmpF36;Cyc_Tcutil_failure_reason=((_tmpF36="(type variable would escape scope)",_tag_dyneither(_tmpF36,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1273
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3D0->v,_tmp3D3)){
*_tmp3D6=t1;return;}
# 1276
{const char*_tmpF37;Cyc_Tcutil_failure_reason=((_tmpF37="(kinds are incompatible)",_tag_dyneither(_tmpF37,sizeof(char),25)));}
goto _LL293;}_LL296:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D4;if(_tmp3D8->tag != 5)goto _LL298;else{_tmp3D9=_tmp3D8->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3D0->v)->kind == Cyc_Absyn_BoxKind))goto _LL298;_LL297: {
# 1282
union Cyc_Absyn_Constraint*_tmp3DD=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp3D9.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp3DE=_tmp3DD;_LL29B: if((_tmp3DE->No_constr).tag != 3)goto _LL29D;_LL29C:
# 1286
{struct _union_Constraint_Eq_constr*_tmpF38;(_tmpF38=& _tmp3DD->Eq_constr,((_tmpF38->tag=1,_tmpF38->val=Cyc_Absyn_bounds_one)));}
*_tmp3D1=t2;
return;_LL29D:;_LL29E:
 goto _LL29A;_LL29A:;}
# 1291
goto _LL293;}_LL298:;_LL299:
 goto _LL293;_LL293:;}
# 1294
{const char*_tmpF39;Cyc_Tcutil_failure_reason=((_tmpF39="(kinds are incompatible)",_tag_dyneither(_tmpF39,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL291:;_LL292:
# 1297
 goto _LL28E;_LL28E:;}
# 1302
{struct _tuple0 _tmpF3A;struct _tuple0 _tmp3E2=(_tmpF3A.f1=t2,((_tmpF3A.f2=t1,_tmpF3A)));struct Cyc_Absyn_Tvar*_tmp3E7;struct Cyc_Absyn_Tvar*_tmp3E9;union Cyc_Absyn_AggrInfoU _tmp3EB;struct Cyc_List_List*_tmp3EC;union Cyc_Absyn_AggrInfoU _tmp3EE;struct Cyc_List_List*_tmp3EF;struct _tuple2*_tmp3F1;struct _tuple2*_tmp3F3;struct Cyc_List_List*_tmp3F5;struct Cyc_List_List*_tmp3F7;struct Cyc_Absyn_Datatypedecl*_tmp3F9;struct Cyc_List_List*_tmp3FA;struct Cyc_Absyn_Datatypedecl*_tmp3FC;struct Cyc_List_List*_tmp3FD;struct Cyc_Absyn_Datatypedecl*_tmp3FF;struct Cyc_Absyn_Datatypefield*_tmp400;struct Cyc_List_List*_tmp401;struct Cyc_Absyn_Datatypedecl*_tmp403;struct Cyc_Absyn_Datatypefield*_tmp404;struct Cyc_List_List*_tmp405;void*_tmp407;struct Cyc_Absyn_Tqual _tmp408;void*_tmp409;union Cyc_Absyn_Constraint*_tmp40A;union Cyc_Absyn_Constraint*_tmp40B;union Cyc_Absyn_Constraint*_tmp40C;void*_tmp40E;struct Cyc_Absyn_Tqual _tmp40F;void*_tmp410;union Cyc_Absyn_Constraint*_tmp411;union Cyc_Absyn_Constraint*_tmp412;union Cyc_Absyn_Constraint*_tmp413;enum Cyc_Absyn_Sign _tmp415;enum Cyc_Absyn_Size_of _tmp416;enum Cyc_Absyn_Sign _tmp418;enum Cyc_Absyn_Size_of _tmp419;int _tmp41B;int _tmp41D;void*_tmp41F;void*_tmp421;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp425;void*_tmp427;struct Cyc_Absyn_Tqual _tmp428;struct Cyc_Absyn_Exp*_tmp429;union Cyc_Absyn_Constraint*_tmp42A;void*_tmp42C;struct Cyc_Absyn_Tqual _tmp42D;struct Cyc_Absyn_Exp*_tmp42E;union Cyc_Absyn_Constraint*_tmp42F;struct Cyc_List_List*_tmp431;void*_tmp432;struct Cyc_Absyn_Tqual _tmp433;void*_tmp434;struct Cyc_List_List*_tmp435;int _tmp436;struct Cyc_Absyn_VarargInfo*_tmp437;struct Cyc_List_List*_tmp438;struct Cyc_List_List*_tmp439;struct Cyc_List_List*_tmp43B;void*_tmp43C;struct Cyc_Absyn_Tqual _tmp43D;void*_tmp43E;struct Cyc_List_List*_tmp43F;int _tmp440;struct Cyc_Absyn_VarargInfo*_tmp441;struct Cyc_List_List*_tmp442;struct Cyc_List_List*_tmp443;struct Cyc_List_List*_tmp445;struct Cyc_List_List*_tmp447;enum Cyc_Absyn_AggrKind _tmp449;struct Cyc_List_List*_tmp44A;enum Cyc_Absyn_AggrKind _tmp44C;struct Cyc_List_List*_tmp44D;struct Cyc_List_List*_tmp44F;struct Cyc_Absyn_Typedefdecl*_tmp450;struct Cyc_List_List*_tmp452;struct Cyc_Absyn_Typedefdecl*_tmp453;void*_tmp45B;void*_tmp45D;void*_tmp45F;void*_tmp460;void*_tmp462;void*_tmp463;_LL2A0: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E3->tag != 1)goto _LL2A2;}_LL2A1:
# 1305
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A2:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E4=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E4->tag != 0)goto _LL2A4;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E5=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3E5->tag != 0)goto _LL2A4;};_LL2A3:
# 1308
 return;_LL2A4:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E6=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E6->tag != 2)goto _LL2A6;else{_tmp3E7=_tmp3E6->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3E8->tag != 2)goto _LL2A6;else{_tmp3E9=_tmp3E8->f1;}};_LL2A5: {
# 1311
struct _dyneither_ptr*_tmp46A=_tmp3E7->name;
struct _dyneither_ptr*_tmp46B=_tmp3E9->name;
# 1314
int _tmp46C=_tmp3E7->identity;
int _tmp46D=_tmp3E9->identity;
if(_tmp46D == _tmp46C)return;
{const char*_tmpF3B;Cyc_Tcutil_failure_reason=((_tmpF3B="(variable types are not the same)",_tag_dyneither(_tmpF3B,sizeof(char),34)));}
goto _LL29F;}_LL2A6:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3EA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3EA->tag != 11)goto _LL2A8;else{_tmp3EB=(_tmp3EA->f1).aggr_info;_tmp3EC=(_tmp3EA->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3ED->tag != 11)goto _LL2A8;else{_tmp3EE=(_tmp3ED->f1).aggr_info;_tmp3EF=(_tmp3ED->f1).targs;}};_LL2A7: {
# 1321
enum Cyc_Absyn_AggrKind _tmp470;struct _tuple2*_tmp471;struct _tuple13 _tmp46F=Cyc_Absyn_aggr_kinded_name(_tmp3EE);_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;{
enum Cyc_Absyn_AggrKind _tmp473;struct _tuple2*_tmp474;struct _tuple13 _tmp472=Cyc_Absyn_aggr_kinded_name(_tmp3EB);_tmp473=_tmp472.f1;_tmp474=_tmp472.f2;
if(_tmp470 != _tmp473){{const char*_tmpF3C;Cyc_Tcutil_failure_reason=((_tmpF3C="(struct and union type)",_tag_dyneither(_tmpF3C,sizeof(char),24)));}goto _LL29F;}
if(Cyc_Absyn_qvar_cmp(_tmp471,_tmp474)!= 0){{const char*_tmpF3D;Cyc_Tcutil_failure_reason=((_tmpF3D="(different type name)",_tag_dyneither(_tmpF3D,sizeof(char),22)));}goto _LL29F;}
Cyc_Tcutil_unify_list(_tmp3EF,_tmp3EC);
return;};}_LL2A8:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F0->tag != 13)goto _LL2AA;else{_tmp3F1=_tmp3F0->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3F2->tag != 13)goto _LL2AA;else{_tmp3F3=_tmp3F2->f1;}};_LL2A9:
# 1330
 if(Cyc_Absyn_qvar_cmp(_tmp3F1,_tmp3F3)== 0)return;
{const char*_tmpF3E;Cyc_Tcutil_failure_reason=((_tmpF3E="(different enum types)",_tag_dyneither(_tmpF3E,sizeof(char),23)));}
goto _LL29F;_LL2AA:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F4->tag != 14)goto _LL2AC;else{_tmp3F5=_tmp3F4->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3F6->tag != 14)goto _LL2AC;else{_tmp3F7=_tmp3F6->f1;}};_LL2AB: {
# 1335
int bad=0;
for(0;_tmp3F5 != 0  && _tmp3F7 != 0;(_tmp3F5=_tmp3F5->tl,_tmp3F7=_tmp3F7->tl)){
struct Cyc_Absyn_Enumfield*_tmp478=(struct Cyc_Absyn_Enumfield*)_tmp3F5->hd;
struct Cyc_Absyn_Enumfield*_tmp479=(struct Cyc_Absyn_Enumfield*)_tmp3F7->hd;
if(Cyc_Absyn_qvar_cmp(_tmp478->name,_tmp479->name)!= 0){
{const char*_tmpF3F;Cyc_Tcutil_failure_reason=((_tmpF3F="(different names for enum fields)",_tag_dyneither(_tmpF3F,sizeof(char),34)));}
bad=1;
break;}
# 1344
if(_tmp478->tag == _tmp479->tag)continue;
if(_tmp478->tag == 0  || _tmp479->tag == 0){
{const char*_tmpF40;Cyc_Tcutil_failure_reason=((_tmpF40="(different tag values for enum fields)",_tag_dyneither(_tmpF40,sizeof(char),39)));}
bad=1;
break;}
# 1350
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp478->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp479->tag))){
{const char*_tmpF41;Cyc_Tcutil_failure_reason=((_tmpF41="(different tag values for enum fields)",_tag_dyneither(_tmpF41,sizeof(char),39)));}
bad=1;
break;}}
# 1356
if(bad)goto _LL29F;
if(_tmp3F5 == 0  && _tmp3F7 == 0)return;
{const char*_tmpF42;Cyc_Tcutil_failure_reason=((_tmpF42="(different number of fields for enums)",_tag_dyneither(_tmpF42,sizeof(char),39)));}
goto _LL29F;}_LL2AC:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F8->tag != 3)goto _LL2AE;else{if((((_tmp3F8->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2AE;_tmp3F9=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3F8->f1).datatype_info).KnownDatatype).val);_tmp3FA=(_tmp3F8->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3FB->tag != 3)goto _LL2AE;else{if((((_tmp3FB->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2AE;_tmp3FC=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3FB->f1).datatype_info).KnownDatatype).val);_tmp3FD=(_tmp3FB->f1).targs;}};_LL2AD:
# 1363
 if(_tmp3F9 == _tmp3FC  || Cyc_Absyn_qvar_cmp(_tmp3F9->name,_tmp3FC->name)== 0){
Cyc_Tcutil_unify_list(_tmp3FD,_tmp3FA);
return;}
# 1367
goto _LL29F;_LL2AE:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3FE->tag != 4)goto _LL2B0;else{if((((_tmp3FE->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B0;_tmp3FF=((struct _tuple3)(((_tmp3FE->f1).field_info).KnownDatatypefield).val).f1;_tmp400=((struct _tuple3)(((_tmp3FE->f1).field_info).KnownDatatypefield).val).f2;_tmp401=(_tmp3FE->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp402->tag != 4)goto _LL2B0;else{if((((_tmp402->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B0;_tmp403=((struct _tuple3)(((_tmp402->f1).field_info).KnownDatatypefield).val).f1;_tmp404=((struct _tuple3)(((_tmp402->f1).field_info).KnownDatatypefield).val).f2;_tmp405=(_tmp402->f1).targs;}};_LL2AF:
# 1371
 if((_tmp3FF == _tmp403  || Cyc_Absyn_qvar_cmp(_tmp3FF->name,_tmp403->name)== 0) && (
_tmp400 == _tmp404  || Cyc_Absyn_qvar_cmp(_tmp400->name,_tmp404->name)== 0)){
Cyc_Tcutil_unify_list(_tmp405,_tmp401);
return;}
# 1376
{const char*_tmpF43;Cyc_Tcutil_failure_reason=((_tmpF43="(different datatype field types)",_tag_dyneither(_tmpF43,sizeof(char),33)));}
goto _LL29F;_LL2B0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp406->tag != 5)goto _LL2B2;else{_tmp407=(_tmp406->f1).elt_typ;_tmp408=(_tmp406->f1).elt_tq;_tmp409=((_tmp406->f1).ptr_atts).rgn;_tmp40A=((_tmp406->f1).ptr_atts).nullable;_tmp40B=((_tmp406->f1).ptr_atts).bounds;_tmp40C=((_tmp406->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp40D->tag != 5)goto _LL2B2;else{_tmp40E=(_tmp40D->f1).elt_typ;_tmp40F=(_tmp40D->f1).elt_tq;_tmp410=((_tmp40D->f1).ptr_atts).rgn;_tmp411=((_tmp40D->f1).ptr_atts).nullable;_tmp412=((_tmp40D->f1).ptr_atts).bounds;_tmp413=((_tmp40D->f1).ptr_atts).zero_term;}};_LL2B1:
# 1381
 Cyc_Tcutil_unify_it(_tmp40E,_tmp407);
Cyc_Tcutil_unify_it(_tmp409,_tmp410);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF44;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp413,_tmp40C,((_tmpF44="(not both zero terminated)",_tag_dyneither(_tmpF44,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp40F,_tmp40E,_tmp408,_tmp407);
{const char*_tmpF45;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp412,_tmp40B,((_tmpF45="(different pointer bounds)",_tag_dyneither(_tmpF45,sizeof(char),27))));}
# 1389
{void*_tmp481=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp412);_LL2DD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp482=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp481;if(_tmp482->tag != 0)goto _LL2DF;}_LL2DE:
 return;_LL2DF:;_LL2E0:
 goto _LL2DC;_LL2DC:;}
# 1393
{const char*_tmpF46;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp411,_tmp40A,((_tmpF46="(incompatible pointer types)",_tag_dyneither(_tmpF46,sizeof(char),29))));}
return;_LL2B2:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp414=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp414->tag != 6)goto _LL2B4;else{_tmp415=_tmp414->f1;_tmp416=_tmp414->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp417->tag != 6)goto _LL2B4;else{_tmp418=_tmp417->f1;_tmp419=_tmp417->f2;}};_LL2B3:
# 1398
 if(_tmp418 == _tmp415  && ((_tmp419 == _tmp416  || _tmp419 == Cyc_Absyn_Int_sz  && _tmp416 == Cyc_Absyn_Long_sz) || 
_tmp419 == Cyc_Absyn_Long_sz  && _tmp416 == Cyc_Absyn_Int_sz))return;
{const char*_tmpF47;Cyc_Tcutil_failure_reason=((_tmpF47="(different integral types)",_tag_dyneither(_tmpF47,sizeof(char),27)));}
goto _LL29F;_LL2B4:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp41A->tag != 7)goto _LL2B6;else{_tmp41B=_tmp41A->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp41C->tag != 7)goto _LL2B6;else{_tmp41D=_tmp41C->f1;}};_LL2B5:
# 1404
 if(_tmp41D == 0  && _tmp41B == 0)return;else{
if(_tmp41D == 1  && _tmp41B == 1)return;else{
# 1407
if(((_tmp41D != 0  && _tmp41D != 1) && _tmp41B != 0) && _tmp41B != 1)return;}}
goto _LL29F;_LL2B6:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp41E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp41E->tag != 19)goto _LL2B8;else{_tmp41F=(void*)_tmp41E->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp420=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp420->tag != 19)goto _LL2B8;else{_tmp421=(void*)_tmp420->f1;}};_LL2B7:
# 1410
 Cyc_Tcutil_unify_it(_tmp41F,_tmp421);return;_LL2B8:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp422->tag != 18)goto _LL2BA;else{_tmp423=_tmp422->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp424->tag != 18)goto _LL2BA;else{_tmp425=_tmp424->f1;}};_LL2B9:
# 1413
 if(!Cyc_Evexp_same_const_exp(_tmp423,_tmp425)){
{const char*_tmpF48;Cyc_Tcutil_failure_reason=((_tmpF48="(cannot prove expressions are the same)",_tag_dyneither(_tmpF48,sizeof(char),40)));}
goto _LL29F;}
# 1417
return;_LL2BA:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp426=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp426->tag != 8)goto _LL2BC;else{_tmp427=(_tmp426->f1).elt_type;_tmp428=(_tmp426->f1).tq;_tmp429=(_tmp426->f1).num_elts;_tmp42A=(_tmp426->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp42B->tag != 8)goto _LL2BC;else{_tmp42C=(_tmp42B->f1).elt_type;_tmp42D=(_tmp42B->f1).tq;_tmp42E=(_tmp42B->f1).num_elts;_tmp42F=(_tmp42B->f1).zero_term;}};_LL2BB:
# 1421
 Cyc_Tcutil_unify_it(_tmp42C,_tmp427);
Cyc_Tcutil_unify_tqual(_tmp42D,_tmp42C,_tmp428,_tmp427);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF49;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp42A,_tmp42F,((_tmpF49="(not both zero terminated)",_tag_dyneither(_tmpF49,sizeof(char),27))));}
if(_tmp429 == _tmp42E)return;
if(_tmp429 == 0  || _tmp42E == 0)goto _LL29F;
if(Cyc_Evexp_same_const_exp(_tmp429,_tmp42E))
return;
{const char*_tmpF4A;Cyc_Tcutil_failure_reason=((_tmpF4A="(different array sizes)",_tag_dyneither(_tmpF4A,sizeof(char),24)));}
goto _LL29F;_LL2BC:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp430=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp430->tag != 9)goto _LL2BE;else{_tmp431=(_tmp430->f1).tvars;_tmp432=(_tmp430->f1).effect;_tmp433=(_tmp430->f1).ret_tqual;_tmp434=(_tmp430->f1).ret_typ;_tmp435=(_tmp430->f1).args;_tmp436=(_tmp430->f1).c_varargs;_tmp437=(_tmp430->f1).cyc_varargs;_tmp438=(_tmp430->f1).rgn_po;_tmp439=(_tmp430->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp43A->tag != 9)goto _LL2BE;else{_tmp43B=(_tmp43A->f1).tvars;_tmp43C=(_tmp43A->f1).effect;_tmp43D=(_tmp43A->f1).ret_tqual;_tmp43E=(_tmp43A->f1).ret_typ;_tmp43F=(_tmp43A->f1).args;_tmp440=(_tmp43A->f1).c_varargs;_tmp441=(_tmp43A->f1).cyc_varargs;_tmp442=(_tmp43A->f1).rgn_po;_tmp443=(_tmp43A->f1).attributes;}};_LL2BD: {
# 1435
int done=0;
struct _RegionHandle _tmp488=_new_region("rgn");struct _RegionHandle*rgn=& _tmp488;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp43B != 0){
if(_tmp431 == 0){
{const char*_tmpF4B;Cyc_Tcutil_failure_reason=((_tmpF4B="(second function type has too few type variables)",_tag_dyneither(_tmpF4B,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1443
void*_tmp48A=((struct Cyc_Absyn_Tvar*)_tmp43B->hd)->kind;
void*_tmp48B=((struct Cyc_Absyn_Tvar*)_tmp431->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp48A,_tmp48B)){
{const char*_tmpF51;void*_tmpF50[3];struct Cyc_String_pa_PrintArg_struct _tmpF4F;struct Cyc_String_pa_PrintArg_struct _tmpF4E;struct Cyc_String_pa_PrintArg_struct _tmpF4D;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmpF4D.tag=0,((_tmpF4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1449
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp431->hd,& Cyc_Tcutil_bk))),((_tmpF4E.tag=0,((_tmpF4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1448
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp43B->hd,& Cyc_Tcutil_bk))),((_tmpF4F.tag=0,((_tmpF4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1447
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp43B->hd)),((_tmpF50[0]=& _tmpF4F,((_tmpF50[1]=& _tmpF4E,((_tmpF50[2]=& _tmpF4D,Cyc_aprintf(((_tmpF51="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmpF51,sizeof(char),44))),_tag_dyneither(_tmpF50,sizeof(void*),3))))))))))))))))))));}
# 1450
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1452
{struct _tuple16*_tmpF5B;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF5A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF59;struct Cyc_List_List*_tmpF58;inst=((_tmpF58=_region_malloc(rgn,sizeof(*_tmpF58)),((_tmpF58->hd=((_tmpF5B=_region_malloc(rgn,sizeof(*_tmpF5B)),((_tmpF5B->f1=(struct Cyc_Absyn_Tvar*)_tmp431->hd,((_tmpF5B->f2=(void*)((_tmpF59=_cycalloc(sizeof(*_tmpF59)),((_tmpF59[0]=((_tmpF5A.tag=2,((_tmpF5A.f1=(struct Cyc_Absyn_Tvar*)_tmp43B->hd,_tmpF5A)))),_tmpF59)))),_tmpF5B)))))),((_tmpF58->tl=inst,_tmpF58))))));}
_tmp43B=_tmp43B->tl;
_tmp431=_tmp431->tl;};}
# 1456
if(_tmp431 != 0){
{const char*_tmpF5C;Cyc_Tcutil_failure_reason=((_tmpF5C="(second function type has too many type variables)",_tag_dyneither(_tmpF5C,sizeof(char),51)));}
_npop_handler(0);goto _LL29F;}
# 1460
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF68;struct Cyc_Absyn_FnInfo _tmpF67;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF66;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF62;struct Cyc_Absyn_FnInfo _tmpF61;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF60;Cyc_Tcutil_unify_it((void*)((_tmpF60=_cycalloc(sizeof(*_tmpF60)),((_tmpF60[0]=((_tmpF62.tag=9,((_tmpF62.f1=((_tmpF61.tvars=0,((_tmpF61.effect=_tmp43C,((_tmpF61.ret_tqual=_tmp43D,((_tmpF61.ret_typ=_tmp43E,((_tmpF61.args=_tmp43F,((_tmpF61.c_varargs=_tmp440,((_tmpF61.cyc_varargs=_tmp441,((_tmpF61.rgn_po=_tmp442,((_tmpF61.attributes=_tmp443,_tmpF61)))))))))))))))))),_tmpF62)))),_tmpF60)))),
# 1463
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpF66=_cycalloc(sizeof(*_tmpF66)),((_tmpF66[0]=((_tmpF68.tag=9,((_tmpF68.f1=((_tmpF67.tvars=0,((_tmpF67.effect=_tmp432,((_tmpF67.ret_tqual=_tmp433,((_tmpF67.ret_typ=_tmp434,((_tmpF67.args=_tmp435,((_tmpF67.c_varargs=_tmp436,((_tmpF67.cyc_varargs=_tmp437,((_tmpF67.rgn_po=_tmp438,((_tmpF67.attributes=_tmp439,_tmpF67)))))))))))))))))),_tmpF68)))),_tmpF66))))));}
# 1466
done=1;}}
# 1469
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp43E,_tmp434);
Cyc_Tcutil_unify_tqual(_tmp43D,_tmp43E,_tmp433,_tmp434);
for(0;_tmp43F != 0  && _tmp435 != 0;(_tmp43F=_tmp43F->tl,_tmp435=_tmp435->tl)){
struct Cyc_Absyn_Tqual _tmp49D;void*_tmp49E;struct _tuple10 _tmp49C=*((struct _tuple10*)_tmp43F->hd);_tmp49D=_tmp49C.f2;_tmp49E=_tmp49C.f3;{
struct Cyc_Absyn_Tqual _tmp4A0;void*_tmp4A1;struct _tuple10 _tmp49F=*((struct _tuple10*)_tmp435->hd);_tmp4A0=_tmp49F.f2;_tmp4A1=_tmp49F.f3;
Cyc_Tcutil_unify_it(_tmp49E,_tmp4A1);
Cyc_Tcutil_unify_tqual(_tmp49D,_tmp49E,_tmp4A0,_tmp4A1);};}
# 1479
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp43F != 0  || _tmp435 != 0){
{const char*_tmpF69;Cyc_Tcutil_failure_reason=((_tmpF69="(function types have different number of arguments)",_tag_dyneither(_tmpF69,sizeof(char),52)));}
_npop_handler(0);goto _LL29F;}
# 1485
if(_tmp440 != _tmp436){
{const char*_tmpF6A;Cyc_Tcutil_failure_reason=((_tmpF6A="(only one function type takes C varargs)",_tag_dyneither(_tmpF6A,sizeof(char),41)));}
_npop_handler(0);goto _LL29F;}{
# 1490
int bad_cyc_vararg=0;
{struct _tuple22 _tmpF6B;struct _tuple22 _tmp4A5=(_tmpF6B.f1=_tmp441,((_tmpF6B.f2=_tmp437,_tmpF6B)));struct _dyneither_ptr*_tmp4A6;struct Cyc_Absyn_Tqual _tmp4A7;void*_tmp4A8;int _tmp4A9;struct _dyneither_ptr*_tmp4AA;struct Cyc_Absyn_Tqual _tmp4AB;void*_tmp4AC;int _tmp4AD;_LL2E2: if(_tmp4A5.f1 != 0)goto _LL2E4;if(_tmp4A5.f2 != 0)goto _LL2E4;_LL2E3:
 goto _LL2E1;_LL2E4: if(_tmp4A5.f1 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E7;_LL2E6: if(_tmp4A5.f2 != 0)goto _LL2E8;_LL2E7:
# 1495
 bad_cyc_vararg=1;
{const char*_tmpF6C;Cyc_Tcutil_failure_reason=((_tmpF6C="(only one function type takes varargs)",_tag_dyneither(_tmpF6C,sizeof(char),39)));}
goto _LL2E1;_LL2E8: if(_tmp4A5.f1 == 0)goto _LL2E1;_tmp4A6=(_tmp4A5.f1)->name;_tmp4A7=(_tmp4A5.f1)->tq;_tmp4A8=(_tmp4A5.f1)->type;_tmp4A9=(_tmp4A5.f1)->inject;if(_tmp4A5.f2 == 0)goto _LL2E1;_tmp4AA=(_tmp4A5.f2)->name;_tmp4AB=(_tmp4A5.f2)->tq;_tmp4AC=(_tmp4A5.f2)->type;_tmp4AD=(_tmp4A5.f2)->inject;_LL2E9:
# 1499
 Cyc_Tcutil_unify_it(_tmp4A8,_tmp4AC);
Cyc_Tcutil_unify_tqual(_tmp4A7,_tmp4A8,_tmp4AB,_tmp4AC);
if(_tmp4A9 != _tmp4AD){
bad_cyc_vararg=1;{
const char*_tmpF6D;Cyc_Tcutil_failure_reason=((_tmpF6D="(only one function type injects varargs)",_tag_dyneither(_tmpF6D,sizeof(char),41)));};}
# 1505
goto _LL2E1;_LL2E1:;}
# 1507
if(bad_cyc_vararg){_npop_handler(0);goto _LL29F;}{
# 1510
int bad_effect=0;
{struct _tuple0 _tmpF6E;struct _tuple0 _tmp4B1=(_tmpF6E.f1=_tmp43C,((_tmpF6E.f2=_tmp432,_tmpF6E)));_LL2EB: if(_tmp4B1.f1 != 0)goto _LL2ED;if(_tmp4B1.f2 != 0)goto _LL2ED;_LL2EC:
 goto _LL2EA;_LL2ED: if(_tmp4B1.f1 != 0)goto _LL2EF;_LL2EE:
 goto _LL2F0;_LL2EF: if(_tmp4B1.f2 != 0)goto _LL2F1;_LL2F0:
 bad_effect=1;goto _LL2EA;_LL2F1:;_LL2F2:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp43C),(void*)_check_null(_tmp432));goto _LL2EA;_LL2EA:;}
# 1517
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmpF6F;Cyc_Tcutil_failure_reason=((_tmpF6F="(function type effects do not unify)",_tag_dyneither(_tmpF6F,sizeof(char),37)));}
_npop_handler(0);goto _LL29F;}
# 1523
if(!Cyc_Tcutil_same_atts(_tmp439,_tmp443)){
{const char*_tmpF70;Cyc_Tcutil_failure_reason=((_tmpF70="(function types have different attributes)",_tag_dyneither(_tmpF70,sizeof(char),43)));}
_npop_handler(0);goto _LL29F;}
# 1527
if(!Cyc_Tcutil_same_rgn_po(_tmp438,_tmp442)){
{const char*_tmpF71;Cyc_Tcutil_failure_reason=((_tmpF71="(function types have different region lifetime orderings)",_tag_dyneither(_tmpF71,sizeof(char),58)));}
_npop_handler(0);goto _LL29F;}
# 1531
_npop_handler(0);return;};};
# 1436
;_pop_region(rgn);}_LL2BE:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp444=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp444->tag != 10)goto _LL2C0;else{_tmp445=_tmp444->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp446=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp446->tag != 10)goto _LL2C0;else{_tmp447=_tmp446->f1;}};_LL2BF:
# 1534
 for(0;_tmp447 != 0  && _tmp445 != 0;(_tmp447=_tmp447->tl,_tmp445=_tmp445->tl)){
struct Cyc_Absyn_Tqual _tmp4B6;void*_tmp4B7;struct _tuple12 _tmp4B5=*((struct _tuple12*)_tmp447->hd);_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;{
struct Cyc_Absyn_Tqual _tmp4B9;void*_tmp4BA;struct _tuple12 _tmp4B8=*((struct _tuple12*)_tmp445->hd);_tmp4B9=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;
Cyc_Tcutil_unify_it(_tmp4B7,_tmp4BA);
Cyc_Tcutil_unify_tqual(_tmp4B6,_tmp4B7,_tmp4B9,_tmp4BA);};}
# 1540
if(_tmp447 == 0  && _tmp445 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF72;Cyc_Tcutil_failure_reason=((_tmpF72="(tuple types have different numbers of components)",_tag_dyneither(_tmpF72,sizeof(char),51)));}
goto _LL29F;_LL2C0:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp448->tag != 12)goto _LL2C2;else{_tmp449=_tmp448->f1;_tmp44A=_tmp448->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp44B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp44B->tag != 12)goto _LL2C2;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;}};_LL2C1:
# 1547
 if(_tmp44C != _tmp449){{const char*_tmpF73;Cyc_Tcutil_failure_reason=((_tmpF73="(struct and union type)",_tag_dyneither(_tmpF73,sizeof(char),24)));}goto _LL29F;}
for(0;_tmp44D != 0  && _tmp44A != 0;(_tmp44D=_tmp44D->tl,_tmp44A=_tmp44A->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4BD=(struct Cyc_Absyn_Aggrfield*)_tmp44D->hd;
struct Cyc_Absyn_Aggrfield*_tmp4BE=(struct Cyc_Absyn_Aggrfield*)_tmp44A->hd;
if(Cyc_strptrcmp(_tmp4BD->name,_tmp4BE->name)!= 0){
{const char*_tmpF74;Cyc_Tcutil_failure_reason=((_tmpF74="(different member names)",_tag_dyneither(_tmpF74,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1555
Cyc_Tcutil_unify_it(_tmp4BD->type,_tmp4BE->type);
Cyc_Tcutil_unify_tqual(_tmp4BD->tq,_tmp4BD->type,_tmp4BE->tq,_tmp4BE->type);
if(!Cyc_Tcutil_same_atts(_tmp4BD->attributes,_tmp4BE->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF75;Cyc_Tcutil_failure_reason=((_tmpF75="(different attributes on member)",_tag_dyneither(_tmpF75,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1563
if((_tmp4BD->width != 0  && _tmp4BE->width == 0  || 
_tmp4BE->width != 0  && _tmp4BD->width == 0) || 
(_tmp4BD->width != 0  && _tmp4BE->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4BD->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4BE->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF76;Cyc_Tcutil_failure_reason=((_tmpF76="(different bitfield widths on member)",_tag_dyneither(_tmpF76,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1573
if(_tmp44D == 0  && _tmp44A == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF77;Cyc_Tcutil_failure_reason=((_tmpF77="(different number of members)",_tag_dyneither(_tmpF77,sizeof(char),30)));}
goto _LL29F;_LL2C2:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp44E->tag != 17)goto _LL2C4;else{_tmp44F=_tmp44E->f2;_tmp450=_tmp44E->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp451->tag != 17)goto _LL2C4;else{_tmp452=_tmp451->f2;_tmp453=_tmp451->f3;}};_LL2C3:
# 1579
 if(_tmp450 != _tmp453){
{const char*_tmpF78;Cyc_Tcutil_failure_reason=((_tmpF78="(different abstract typedefs)",_tag_dyneither(_tmpF78,sizeof(char),30)));}
goto _LL29F;}
# 1583
{const char*_tmpF79;Cyc_Tcutil_failure_reason=((_tmpF79="(type parameters to typedef differ)",_tag_dyneither(_tmpF79,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp44F,_tmp452);
return;_LL2C4:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp454->tag != 20)goto _LL2C6;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp455->tag != 20)goto _LL2C6;};_LL2C5:
 return;_LL2C6:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp456->tag != 21)goto _LL2C8;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp457->tag != 21)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp458->tag != 22)goto _LL2CA;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp459->tag != 22)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp45A->tag != 15)goto _LL2CC;else{_tmp45B=(void*)_tmp45A->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp45C->tag != 15)goto _LL2CC;else{_tmp45D=(void*)_tmp45C->f1;}};_LL2CB:
# 1590
 Cyc_Tcutil_unify_it(_tmp45B,_tmp45D);
return;_LL2CC:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp45E->tag != 16)goto _LL2CE;else{_tmp45F=(void*)_tmp45E->f1;_tmp460=(void*)_tmp45E->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp461->tag != 16)goto _LL2CE;else{_tmp462=(void*)_tmp461->f1;_tmp463=(void*)_tmp461->f2;}};_LL2CD:
# 1593
 Cyc_Tcutil_unify_it(_tmp45F,_tmp462);
Cyc_Tcutil_unify_it(_tmp460,_tmp463);
return;_LL2CE: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp464->tag != 24)goto _LL2D0;}_LL2CF:
 goto _LL2D1;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp465=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp465->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp466->tag != 23)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp467->tag != 25)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp468->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp469->tag != 23)goto _LL2DA;}_LL2D9:
# 1602
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmpF7A;Cyc_Tcutil_failure_reason=((_tmpF7A="(effects don't unify)",_tag_dyneither(_tmpF7A,sizeof(char),22)));}
goto _LL29F;_LL2DA:;_LL2DB:
 goto _LL29F;_LL29F:;}
# 1607
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1610
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1617
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp4C6=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4C7=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4C6,_tmp4C7);}
# 1624
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4C8=x;void*_tmp4C9;_LL2F4: if((_tmp4C8->No_constr).tag != 3)goto _LL2F6;_LL2F5:
 return - 1;_LL2F6: if((_tmp4C8->Eq_constr).tag != 1)goto _LL2F8;_tmp4C9=(void*)(_tmp4C8->Eq_constr).val;_LL2F7: {
# 1631
union Cyc_Absyn_Constraint*_tmp4CA=y;void*_tmp4CB;_LL2FB: if((_tmp4CA->No_constr).tag != 3)goto _LL2FD;_LL2FC:
 return 1;_LL2FD: if((_tmp4CA->Eq_constr).tag != 1)goto _LL2FF;_tmp4CB=(void*)(_tmp4CA->Eq_constr).val;_LL2FE:
 return cmp(_tmp4C9,_tmp4CB);_LL2FF: if((_tmp4CA->Forward_constr).tag != 2)goto _LL2FA;_LL300: {
const char*_tmpF7D;void*_tmpF7C;(_tmpF7C=0,Cyc_Tcutil_impos(((_tmpF7D="unify_conref: forward after compress(2)",_tag_dyneither(_tmpF7D,sizeof(char),40))),_tag_dyneither(_tmpF7C,sizeof(void*),0)));}_LL2FA:;}_LL2F8: if((_tmp4C8->Forward_constr).tag != 2)goto _LL2F3;_LL2F9: {
# 1636
const char*_tmpF80;void*_tmpF7F;(_tmpF7F=0,Cyc_Tcutil_impos(((_tmpF80="unify_conref: forward after compress",_tag_dyneither(_tmpF80,sizeof(char),37))),_tag_dyneither(_tmpF7F,sizeof(void*),0)));}_LL2F3:;};}
# 1640
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp4D1;void*_tmp4D2;struct _tuple12*_tmp4D0=tqt1;_tmp4D1=_tmp4D0->f1;_tmp4D2=_tmp4D0->f2;{
struct Cyc_Absyn_Tqual _tmp4D4;void*_tmp4D5;struct _tuple12*_tmp4D3=tqt2;_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;{
int _tmp4D6=Cyc_Tcutil_tqual_cmp(_tmp4D1,_tmp4D4);
if(_tmp4D6 != 0)return _tmp4D6;
return Cyc_Tcutil_typecmp(_tmp4D2,_tmp4D5);};};}
# 1648
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp4D7=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp4D7 != 0)return _tmp4D7;{
int _tmp4D8=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp4D8 != 0)return _tmp4D8;{
int _tmp4D9=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp4D9 != 0)return _tmp4D9;{
int _tmp4DA=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp4DA != 0)return _tmp4DA;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}
# 1660
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp4DB=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp4DB != 0)return _tmp4DB;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1666
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp4DC=t;_LL302: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4DD=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4DC;if(_tmp4DD->tag != 0)goto _LL304;}_LL303:
 return 0;_LL304: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4DE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4DC;if(_tmp4DE->tag != 1)goto _LL306;}_LL305:
 return 1;_LL306: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4DF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4DC;if(_tmp4DF->tag != 2)goto _LL308;}_LL307:
 return 2;_LL308: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4E0=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E0->tag != 3)goto _LL30A;}_LL309:
 return 3;_LL30A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4E1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E1->tag != 4)goto _LL30C;}_LL30B:
 return 4;_LL30C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E2->tag != 5)goto _LL30E;}_LL30D:
 return 5;_LL30E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E3->tag != 6)goto _LL310;}_LL30F:
 return 6;_LL310: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E4->tag != 7)goto _LL312;else{if(_tmp4E4->f1 != 0)goto _LL312;}}_LL311:
 return 7;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E5->tag != 7)goto _LL314;}_LL313:
 return 8;_LL314: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E6->tag != 8)goto _LL316;}_LL315:
 return 9;_LL316: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E7=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E7->tag != 9)goto _LL318;}_LL317:
 return 10;_LL318: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4E8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E8->tag != 10)goto _LL31A;}_LL319:
 return 11;_LL31A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E9->tag != 11)goto _LL31C;}_LL31B:
 return 12;_LL31C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4DC;if(_tmp4EA->tag != 12)goto _LL31E;}_LL31D:
 return 14;_LL31E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4DC;if(_tmp4EB->tag != 13)goto _LL320;}_LL31F:
 return 16;_LL320: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4DC;if(_tmp4EC->tag != 14)goto _LL322;}_LL321:
 return 17;_LL322: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4DC;if(_tmp4ED->tag != 15)goto _LL324;}_LL323:
 return 18;_LL324: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DC;if(_tmp4EE->tag != 17)goto _LL326;}_LL325:
 return 19;_LL326: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4DC;if(_tmp4EF->tag != 21)goto _LL328;}_LL327:
 return 20;_LL328: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4DC;if(_tmp4F0->tag != 20)goto _LL32A;}_LL329:
 return 21;_LL32A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4F1=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4DC;if(_tmp4F1->tag != 23)goto _LL32C;}_LL32B:
 return 22;_LL32C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4DC;if(_tmp4F2->tag != 24)goto _LL32E;}_LL32D:
 return 23;_LL32E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4DC;if(_tmp4F3->tag != 25)goto _LL330;}_LL32F:
 return 24;_LL330: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4DC;if(_tmp4F4->tag != 22)goto _LL332;}_LL331:
 return 25;_LL332: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4DC;if(_tmp4F5->tag != 19)goto _LL334;}_LL333:
 return 26;_LL334: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4DC;if(_tmp4F6->tag != 16)goto _LL336;}_LL335:
 return 27;_LL336: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4DC;if(_tmp4F7->tag != 18)goto _LL338;}_LL337:
 return 28;_LL338: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4DC;if(_tmp4F8->tag != 26)goto _LL301;}_LL339:
 return 29;_LL301:;}
# 1701
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp4F9=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp4F9 != 0)
return _tmp4F9;{
# 1710
struct _tuple0 _tmpF81;struct _tuple0 _tmp4FB=(_tmpF81.f1=t2,((_tmpF81.f2=t1,_tmpF81)));struct Cyc_Absyn_Tvar*_tmp4FF;struct Cyc_Absyn_Tvar*_tmp501;union Cyc_Absyn_AggrInfoU _tmp503;struct Cyc_List_List*_tmp504;union Cyc_Absyn_AggrInfoU _tmp506;struct Cyc_List_List*_tmp507;struct _tuple2*_tmp509;struct _tuple2*_tmp50B;struct Cyc_List_List*_tmp50D;struct Cyc_List_List*_tmp50F;struct Cyc_Absyn_Datatypedecl*_tmp511;struct Cyc_List_List*_tmp512;struct Cyc_Absyn_Datatypedecl*_tmp514;struct Cyc_List_List*_tmp515;struct Cyc_Absyn_Datatypedecl*_tmp517;struct Cyc_Absyn_Datatypefield*_tmp518;struct Cyc_List_List*_tmp519;struct Cyc_Absyn_Datatypedecl*_tmp51B;struct Cyc_Absyn_Datatypefield*_tmp51C;struct Cyc_List_List*_tmp51D;void*_tmp51F;struct Cyc_Absyn_Tqual _tmp520;void*_tmp521;union Cyc_Absyn_Constraint*_tmp522;union Cyc_Absyn_Constraint*_tmp523;union Cyc_Absyn_Constraint*_tmp524;void*_tmp526;struct Cyc_Absyn_Tqual _tmp527;void*_tmp528;union Cyc_Absyn_Constraint*_tmp529;union Cyc_Absyn_Constraint*_tmp52A;union Cyc_Absyn_Constraint*_tmp52B;enum Cyc_Absyn_Sign _tmp52D;enum Cyc_Absyn_Size_of _tmp52E;enum Cyc_Absyn_Sign _tmp530;enum Cyc_Absyn_Size_of _tmp531;int _tmp533;int _tmp535;void*_tmp537;struct Cyc_Absyn_Tqual _tmp538;struct Cyc_Absyn_Exp*_tmp539;union Cyc_Absyn_Constraint*_tmp53A;void*_tmp53C;struct Cyc_Absyn_Tqual _tmp53D;struct Cyc_Absyn_Exp*_tmp53E;union Cyc_Absyn_Constraint*_tmp53F;struct Cyc_List_List*_tmp541;void*_tmp542;struct Cyc_Absyn_Tqual _tmp543;void*_tmp544;struct Cyc_List_List*_tmp545;int _tmp546;struct Cyc_Absyn_VarargInfo*_tmp547;struct Cyc_List_List*_tmp548;struct Cyc_List_List*_tmp549;struct Cyc_List_List*_tmp54B;void*_tmp54C;struct Cyc_Absyn_Tqual _tmp54D;void*_tmp54E;struct Cyc_List_List*_tmp54F;int _tmp550;struct Cyc_Absyn_VarargInfo*_tmp551;struct Cyc_List_List*_tmp552;struct Cyc_List_List*_tmp553;struct Cyc_List_List*_tmp555;struct Cyc_List_List*_tmp557;enum Cyc_Absyn_AggrKind _tmp559;struct Cyc_List_List*_tmp55A;enum Cyc_Absyn_AggrKind _tmp55C;struct Cyc_List_List*_tmp55D;void*_tmp55F;void*_tmp561;void*_tmp563;void*_tmp564;void*_tmp566;void*_tmp567;void*_tmp569;void*_tmp56B;struct Cyc_Absyn_Exp*_tmp56D;struct Cyc_Absyn_Exp*_tmp56F;_LL33B:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp4FC->tag != 1)goto _LL33D;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp4FD->tag != 1)goto _LL33D;};_LL33C: {
# 1712
const char*_tmpF84;void*_tmpF83;(_tmpF83=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF84="typecmp: can only compare closed types",_tag_dyneither(_tmpF84,sizeof(char),39))),_tag_dyneither(_tmpF83,sizeof(void*),0)));}_LL33D:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp4FE->tag != 2)goto _LL33F;else{_tmp4FF=_tmp4FE->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp500=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp500->tag != 2)goto _LL33F;else{_tmp501=_tmp500->f1;}};_LL33E:
# 1716
 return Cyc_Core_intcmp(_tmp501->identity,_tmp4FF->identity);_LL33F:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp502=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp502->tag != 11)goto _LL341;else{_tmp503=(_tmp502->f1).aggr_info;_tmp504=(_tmp502->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp505=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp505->tag != 11)goto _LL341;else{_tmp506=(_tmp505->f1).aggr_info;_tmp507=(_tmp505->f1).targs;}};_LL340: {
# 1720
struct _tuple2*_tmp579;struct _tuple13 _tmp578=Cyc_Absyn_aggr_kinded_name(_tmp503);_tmp579=_tmp578.f2;{
struct _tuple2*_tmp57B;struct _tuple13 _tmp57A=Cyc_Absyn_aggr_kinded_name(_tmp506);_tmp57B=_tmp57A.f2;{
int _tmp57C=Cyc_Absyn_qvar_cmp(_tmp579,_tmp57B);
if(_tmp57C != 0)return _tmp57C;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp504,_tmp507);}};};}_LL341:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp508=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp508->tag != 13)goto _LL343;else{_tmp509=_tmp508->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp50A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp50A->tag != 13)goto _LL343;else{_tmp50B=_tmp50A->f1;}};_LL342:
# 1727
 return Cyc_Absyn_qvar_cmp(_tmp509,_tmp50B);_LL343:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp50C->tag != 14)goto _LL345;else{_tmp50D=_tmp50C->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp50E->tag != 14)goto _LL345;else{_tmp50F=_tmp50E->f1;}};_LL344:
# 1730
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp50D,_tmp50F);_LL345:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp510=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp510->tag != 3)goto _LL347;else{if((((_tmp510->f1).datatype_info).KnownDatatype).tag != 2)goto _LL347;_tmp511=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp510->f1).datatype_info).KnownDatatype).val);_tmp512=(_tmp510->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp513=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp513->tag != 3)goto _LL347;else{if((((_tmp513->f1).datatype_info).KnownDatatype).tag != 2)goto _LL347;_tmp514=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp513->f1).datatype_info).KnownDatatype).val);_tmp515=(_tmp513->f1).targs;}};_LL346:
# 1734
 if(_tmp514 == _tmp511)return 0;{
int _tmp57D=Cyc_Absyn_qvar_cmp(_tmp514->name,_tmp511->name);
if(_tmp57D != 0)return _tmp57D;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp515,_tmp512);};_LL347:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp516=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp516->tag != 4)goto _LL349;else{if((((_tmp516->f1).field_info).KnownDatatypefield).tag != 2)goto _LL349;_tmp517=((struct _tuple3)(((_tmp516->f1).field_info).KnownDatatypefield).val).f1;_tmp518=((struct _tuple3)(((_tmp516->f1).field_info).KnownDatatypefield).val).f2;_tmp519=(_tmp516->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp51A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp51A->tag != 4)goto _LL349;else{if((((_tmp51A->f1).field_info).KnownDatatypefield).tag != 2)goto _LL349;_tmp51B=((struct _tuple3)(((_tmp51A->f1).field_info).KnownDatatypefield).val).f1;_tmp51C=((struct _tuple3)(((_tmp51A->f1).field_info).KnownDatatypefield).val).f2;_tmp51D=(_tmp51A->f1).targs;}};_LL348:
# 1741
 if(_tmp51B == _tmp517)return 0;{
int _tmp57E=Cyc_Absyn_qvar_cmp(_tmp517->name,_tmp51B->name);
if(_tmp57E != 0)return _tmp57E;{
int _tmp57F=Cyc_Absyn_qvar_cmp(_tmp518->name,_tmp51C->name);
if(_tmp57F != 0)return _tmp57F;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp51D,_tmp519);};};_LL349:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp51E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp51E->tag != 5)goto _LL34B;else{_tmp51F=(_tmp51E->f1).elt_typ;_tmp520=(_tmp51E->f1).elt_tq;_tmp521=((_tmp51E->f1).ptr_atts).rgn;_tmp522=((_tmp51E->f1).ptr_atts).nullable;_tmp523=((_tmp51E->f1).ptr_atts).bounds;_tmp524=((_tmp51E->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp525=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp525->tag != 5)goto _LL34B;else{_tmp526=(_tmp525->f1).elt_typ;_tmp527=(_tmp525->f1).elt_tq;_tmp528=((_tmp525->f1).ptr_atts).rgn;_tmp529=((_tmp525->f1).ptr_atts).nullable;_tmp52A=((_tmp525->f1).ptr_atts).bounds;_tmp52B=((_tmp525->f1).ptr_atts).zero_term;}};_LL34A: {
# 1750
int _tmp580=Cyc_Tcutil_typecmp(_tmp526,_tmp51F);
if(_tmp580 != 0)return _tmp580;{
int _tmp581=Cyc_Tcutil_typecmp(_tmp528,_tmp521);
if(_tmp581 != 0)return _tmp581;{
int _tmp582=Cyc_Tcutil_tqual_cmp(_tmp527,_tmp520);
if(_tmp582 != 0)return _tmp582;{
int _tmp583=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp52A,_tmp523);
if(_tmp583 != 0)return _tmp583;{
int _tmp584=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52B,_tmp524);
if(_tmp584 != 0)return _tmp584;
{void*_tmp585=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp52A);_LL36E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp586=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp585;if(_tmp586->tag != 0)goto _LL370;}_LL36F:
 return 0;_LL370:;_LL371:
 goto _LL36D;_LL36D:;}
# 1764
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp529,_tmp522);};};};};}_LL34B:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp52C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp52C->tag != 6)goto _LL34D;else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52C->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp52F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp52F->tag != 6)goto _LL34D;else{_tmp530=_tmp52F->f1;_tmp531=_tmp52F->f2;}};_LL34C:
# 1767
 if(_tmp530 != _tmp52D)return Cyc_Core_intcmp((int)((unsigned int)_tmp530),(int)((unsigned int)_tmp52D));
if(_tmp531 != _tmp52E)return Cyc_Core_intcmp((int)((unsigned int)_tmp531),(int)((unsigned int)_tmp52E));
return 0;_LL34D:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp532=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp532->tag != 7)goto _LL34F;else{_tmp533=_tmp532->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp534->tag != 7)goto _LL34F;else{_tmp535=_tmp534->f1;}};_LL34E:
# 1772
 if(_tmp533 == _tmp535)return 0;else{
if(_tmp535 == 0)return - 1;else{
if(_tmp535 == 1  && _tmp533 == 0)return - 1;else{
return 1;}}}_LL34F:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp536=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp536->tag != 8)goto _LL351;else{_tmp537=(_tmp536->f1).elt_type;_tmp538=(_tmp536->f1).tq;_tmp539=(_tmp536->f1).num_elts;_tmp53A=(_tmp536->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp53B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp53B->tag != 8)goto _LL351;else{_tmp53C=(_tmp53B->f1).elt_type;_tmp53D=(_tmp53B->f1).tq;_tmp53E=(_tmp53B->f1).num_elts;_tmp53F=(_tmp53B->f1).zero_term;}};_LL350: {
# 1779
int _tmp587=Cyc_Tcutil_tqual_cmp(_tmp53D,_tmp538);
if(_tmp587 != 0)return _tmp587;{
int _tmp588=Cyc_Tcutil_typecmp(_tmp53C,_tmp537);
if(_tmp588 != 0)return _tmp588;{
int _tmp589=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp53A,_tmp53F);
if(_tmp589 != 0)return _tmp589;
if(_tmp539 == _tmp53E)return 0;
if(_tmp539 == 0  || _tmp53E == 0){
const char*_tmpF87;void*_tmpF86;(_tmpF86=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF87="missing expression in array index",_tag_dyneither(_tmpF87,sizeof(char),34))),_tag_dyneither(_tmpF86,sizeof(void*),0)));}
# 1789
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp539,_tmp53E);};};}_LL351:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp540=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp540->tag != 9)goto _LL353;else{_tmp541=(_tmp540->f1).tvars;_tmp542=(_tmp540->f1).effect;_tmp543=(_tmp540->f1).ret_tqual;_tmp544=(_tmp540->f1).ret_typ;_tmp545=(_tmp540->f1).args;_tmp546=(_tmp540->f1).c_varargs;_tmp547=(_tmp540->f1).cyc_varargs;_tmp548=(_tmp540->f1).rgn_po;_tmp549=(_tmp540->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp54A->tag != 9)goto _LL353;else{_tmp54B=(_tmp54A->f1).tvars;_tmp54C=(_tmp54A->f1).effect;_tmp54D=(_tmp54A->f1).ret_tqual;_tmp54E=(_tmp54A->f1).ret_typ;_tmp54F=(_tmp54A->f1).args;_tmp550=(_tmp54A->f1).c_varargs;_tmp551=(_tmp54A->f1).cyc_varargs;_tmp552=(_tmp54A->f1).rgn_po;_tmp553=(_tmp54A->f1).attributes;}};_LL352: {
# 1793
const char*_tmpF8A;void*_tmpF89;(_tmpF89=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF8A="typecmp: function types not handled",_tag_dyneither(_tmpF8A,sizeof(char),36))),_tag_dyneither(_tmpF89,sizeof(void*),0)));}_LL353:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp554=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp554->tag != 10)goto _LL355;else{_tmp555=_tmp554->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp556->tag != 10)goto _LL355;else{_tmp557=_tmp556->f1;}};_LL354:
# 1796
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp557,_tmp555);_LL355:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp558=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp558->tag != 12)goto _LL357;else{_tmp559=_tmp558->f1;_tmp55A=_tmp558->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp55B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp55B->tag != 12)goto _LL357;else{_tmp55C=_tmp55B->f1;_tmp55D=_tmp55B->f2;}};_LL356:
# 1799
 if(_tmp55C != _tmp559){
if(_tmp55C == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp55D,_tmp55A);_LL357:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp55E=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp55E->tag != 15)goto _LL359;else{_tmp55F=(void*)_tmp55E->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp560->tag != 15)goto _LL359;else{_tmp561=(void*)_tmp560->f1;}};_LL358:
# 1804
 return Cyc_Tcutil_typecmp(_tmp55F,_tmp561);_LL359:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp562->tag != 16)goto _LL35B;else{_tmp563=(void*)_tmp562->f1;_tmp564=(void*)_tmp562->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp565=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp565->tag != 16)goto _LL35B;else{_tmp566=(void*)_tmp565->f1;_tmp567=(void*)_tmp565->f2;}};_LL35A: {
# 1806
int _tmp58E=Cyc_Tcutil_typecmp(_tmp563,_tmp566);
if(_tmp58E != 0)return _tmp58E;else{
return Cyc_Tcutil_typecmp(_tmp564,_tmp567);}}_LL35B:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp568->tag != 19)goto _LL35D;else{_tmp569=(void*)_tmp568->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp56A->tag != 19)goto _LL35D;else{_tmp56B=(void*)_tmp56A->f1;}};_LL35C:
 return Cyc_Tcutil_typecmp(_tmp569,_tmp56B);_LL35D:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp56C->tag != 18)goto _LL35F;else{_tmp56D=_tmp56C->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp56E->tag != 18)goto _LL35F;else{_tmp56F=_tmp56E->f1;}};_LL35E:
# 1811
 return Cyc_Evexp_const_exp_cmp(_tmp56D,_tmp56F);_LL35F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp570=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp570->tag != 24)goto _LL361;}_LL360:
 goto _LL362;_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp571->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp572->tag != 23)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp573->tag != 25)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp574->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp575->tag != 23)goto _LL36B;}_LL36A: {
const char*_tmpF8D;void*_tmpF8C;(_tmpF8C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF8D="typecmp: effects not handled",_tag_dyneither(_tmpF8D,sizeof(char),29))),_tag_dyneither(_tmpF8C,sizeof(void*),0)));}_LL36B:;_LL36C: {
const char*_tmpF90;void*_tmpF8F;(_tmpF8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF90="Unmatched case in typecmp",_tag_dyneither(_tmpF90,sizeof(char),26))),_tag_dyneither(_tmpF8F,sizeof(void*),0)));}_LL33A:;};};}
# 1822
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp593=Cyc_Tcutil_compress(t);_LL373: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp594=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp593;if(_tmp594->tag != 6)goto _LL375;}_LL374:
 goto _LL376;_LL375: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp595=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp593;if(_tmp595->tag != 7)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp593;if(_tmp596->tag != 13)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp597=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp593;if(_tmp597->tag != 14)goto _LL37B;}_LL37A:
 return 1;_LL37B:;_LL37C:
 return 0;_LL372:;}
# 1834
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpF91;struct _tuple0 _tmp599=(_tmpF91.f1=t1,((_tmpF91.f2=t2,_tmpF91)));int _tmp59B;int _tmp59D;_LL37E:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f1;if(_tmp59A->tag != 7)goto _LL380;else{_tmp59B=_tmp59A->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f2;if(_tmp59C->tag != 7)goto _LL380;else{_tmp59D=_tmp59C->f1;}};_LL37F:
# 1839
 return _tmp59D == 0  && _tmp59B != 0  || (_tmp59D == 1  && _tmp59B != 0) && _tmp59B != 1;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f1;if(_tmp59E->tag != 7)goto _LL382;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp59F->tag != 6)goto _LL382;};_LL381:
 goto _LL383;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A0->tag != 7)goto _LL384;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5A1->tag != 19)goto _LL384;};_LL383:
 return 1;_LL384:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A2->tag != 6)goto _LL386;else{if(_tmp5A2->f2 != Cyc_Absyn_LongLong_sz)goto _LL386;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5A3->tag != 6)goto _LL386;else{if(_tmp5A3->f2 != Cyc_Absyn_LongLong_sz)goto _LL386;}};_LL385:
 return 0;_LL386: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A4->tag != 6)goto _LL388;else{if(_tmp5A4->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}_LL387:
 return 1;_LL388:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A5->tag != 6)goto _LL38A;else{if(_tmp5A5->f2 != Cyc_Absyn_Long_sz)goto _LL38A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5A6->tag != 6)goto _LL38A;else{if(_tmp5A6->f2 != Cyc_Absyn_Int_sz)goto _LL38A;}};_LL389:
# 1846
 goto _LL38B;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A7->tag != 6)goto _LL38C;else{if(_tmp5A7->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5A8->tag != 6)goto _LL38C;else{if(_tmp5A8->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}};_LL38B:
 return 0;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A9->tag != 6)goto _LL38E;else{if(_tmp5A9->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5AA->tag != 7)goto _LL38E;else{if(_tmp5AA->f1 != 0)goto _LL38E;}};_LL38D:
# 1849
 goto _LL38F;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5AB->tag != 6)goto _LL390;else{if(_tmp5AB->f2 != Cyc_Absyn_Int_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5AC->tag != 7)goto _LL390;else{if(_tmp5AC->f1 != 0)goto _LL390;}};_LL38F:
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5AD->tag != 6)goto _LL392;else{if(_tmp5AD->f2 != Cyc_Absyn_Long_sz)goto _LL392;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5AE->tag != 6)goto _LL392;else{if(_tmp5AE->f2 != Cyc_Absyn_Short_sz)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5AF->tag != 6)goto _LL394;else{if(_tmp5AF->f2 != Cyc_Absyn_Int_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5B0->tag != 6)goto _LL394;else{if(_tmp5B0->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5B1->tag != 19)goto _LL396;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5B2->tag != 6)goto _LL396;else{if(_tmp5B2->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5B3->tag != 6)goto _LL398;else{if(_tmp5B3->f2 != Cyc_Absyn_Long_sz)goto _LL398;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5B4->tag != 6)goto _LL398;else{if(_tmp5B4->f2 != Cyc_Absyn_Char_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5B5->tag != 6)goto _LL39A;else{if(_tmp5B5->f2 != Cyc_Absyn_Int_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5B6->tag != 6)goto _LL39A;else{if(_tmp5B6->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5B7->tag != 6)goto _LL39C;else{if(_tmp5B7->f2 != Cyc_Absyn_Short_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5B8->tag != 6)goto _LL39C;else{if(_tmp5B8->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5B9->tag != 19)goto _LL39E;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5BA->tag != 6)goto _LL39E;else{if(_tmp5BA->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 return 1;_LL39E:;_LL39F:
# 1859
 return 0;_LL37D:;};}
# 1865
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1868
struct _RegionHandle _tmp5BB=_new_region("r");struct _RegionHandle*r=& _tmp5BB;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmpF92;max_arith_type=((_tmpF92=_region_malloc(r,sizeof(*_tmpF92)),((_tmpF92->v=t1,_tmpF92))));}}}}
# 1878
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5BD=0;_npop_handler(0);return _tmp5BD;}}}
# 1883
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmpF97;void*_tmpF96[2];struct Cyc_String_pa_PrintArg_struct _tmpF95;struct Cyc_String_pa_PrintArg_struct _tmpF94;(_tmpF94.tag=0,((_tmpF94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmpF95.tag=0,((_tmpF95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpF96[0]=& _tmpF95,((_tmpF96[1]=& _tmpF94,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmpF97="type mismatch: expecting %s but found %s",_tag_dyneither(_tmpF97,sizeof(char),41))),_tag_dyneither(_tmpF96,sizeof(void*),2)))))))))))));}{
int _tmp5C2=0;_npop_handler(0);return _tmp5C2;};}}}{
# 1889
int _tmp5C3=1;_npop_handler(0);return _tmp5C3;};
# 1868
;_pop_region(r);}
# 1894
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp5C4=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL3A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C4;if(_tmp5C5->tag != 5)goto _LL3A3;}_LL3A2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A0;_LL3A3:;_LL3A4:
 return 0;_LL3A0:;}
# 1900
return 1;}
# 1903
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp5C6=Cyc_Tcutil_compress(t);_LL3A6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C6;if(_tmp5C7->tag != 6)goto _LL3A8;}_LL3A7:
 goto _LL3A9;_LL3A8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5C8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5C6;if(_tmp5C8->tag != 19)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5C9=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5C6;if(_tmp5C9->tag != 13)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5CA=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5C6;if(_tmp5CA->tag != 14)goto _LL3AE;}_LL3AD:
 return 1;_LL3AE:;_LL3AF:
 return 0;_LL3A5:;}
# 1914
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1918
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmpF9A;void*_tmpF99;(_tmpF99=0,Cyc_Tcutil_warn(e->loc,((_tmpF9A="integral size mismatch; conversion supplied",_tag_dyneither(_tmpF9A,sizeof(char),44))),_tag_dyneither(_tmpF99,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1924
return 0;}
# 1928
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 1932
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmpF9D;void*_tmpF9C;(_tmpF9C=0,Cyc_Tcutil_warn(e->loc,((_tmpF9D="integral size mismatch; conversion supplied",_tag_dyneither(_tmpF9D,sizeof(char),44))),_tag_dyneither(_tmpF9C,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1938
return 0;}
# 1943
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 1951
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 1953
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpF9E;struct _tuple0 _tmp5D0=(_tmpF9E.f1=t1,((_tmpF9E.f2=t2,_tmpF9E)));struct Cyc_Absyn_PtrInfo _tmp5D2;struct Cyc_Absyn_PtrInfo _tmp5D4;void*_tmp5D6;struct Cyc_Absyn_Tqual _tmp5D7;struct Cyc_Absyn_Exp*_tmp5D8;union Cyc_Absyn_Constraint*_tmp5D9;void*_tmp5DB;struct Cyc_Absyn_Tqual _tmp5DC;struct Cyc_Absyn_Exp*_tmp5DD;union Cyc_Absyn_Constraint*_tmp5DE;_LL3B1:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5D1->tag != 5)goto _LL3B3;else{_tmp5D2=_tmp5D1->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5D3->tag != 5)goto _LL3B3;else{_tmp5D4=_tmp5D3->f1;}};_LL3B2: {
# 1957
int okay=1;
# 1959
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D2.ptr_atts).nullable,(_tmp5D4.ptr_atts).nullable))
# 1962
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D2.ptr_atts).nullable);
# 1964
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D2.ptr_atts).bounds,(_tmp5D4.ptr_atts).bounds)){
# 1967
struct _tuple0 _tmpF9F;struct _tuple0 _tmp5E2=(_tmpF9F.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D2.ptr_atts).bounds),((_tmpF9F.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D4.ptr_atts).bounds),_tmpF9F)));struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5ED;_LL3BA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E3->tag != 1)goto _LL3BC;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E4->tag != 0)goto _LL3BC;};_LL3BB:
 goto _LL3BD;_LL3BC:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E5->tag != 0)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E6->tag != 0)goto _LL3BE;};_LL3BD:
 okay=1;goto _LL3B9;_LL3BE:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E7->tag != 1)goto _LL3C0;else{_tmp5E8=_tmp5E7->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E9->tag != 1)goto _LL3C0;else{_tmp5EA=_tmp5E9->f1;}};_LL3BF:
# 1972
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp5EA,_tmp5E8);
# 1976
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D4.ptr_atts).zero_term)){
const char*_tmpFA2;void*_tmpFA1;(_tmpFA1=0,Cyc_Tcutil_warn(loc,((_tmpFA2="implicit cast to shorter array",_tag_dyneither(_tmpFA2,sizeof(char),31))),_tag_dyneither(_tmpFA1,sizeof(void*),0)));}
goto _LL3B9;_LL3C0:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5EB->tag != 0)goto _LL3B9;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5EC->tag != 1)goto _LL3B9;else{_tmp5ED=_tmp5EC->f1;}};_LL3C1:
# 1981
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D2.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp5D4.ptr_atts).bounds))
goto _LL3B9;
okay=0;
goto _LL3B9;_LL3B9:;}
# 1990
okay=okay  && (!(_tmp5D2.elt_tq).real_const  || (_tmp5D4.elt_tq).real_const);
# 1993
if(!Cyc_Tcutil_unify((_tmp5D2.ptr_atts).rgn,(_tmp5D4.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp5D2.ptr_atts).rgn,(_tmp5D4.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmpFA7;void*_tmpFA6[2];struct Cyc_String_pa_PrintArg_struct _tmpFA5;struct Cyc_String_pa_PrintArg_struct _tmpFA4;(_tmpFA4.tag=0,((_tmpFA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1998
Cyc_Absynpp_typ2string((_tmp5D4.ptr_atts).rgn)),((_tmpFA5.tag=0,((_tmpFA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1997
Cyc_Absynpp_typ2string((_tmp5D2.ptr_atts).rgn)),((_tmpFA6[0]=& _tmpFA5,((_tmpFA6[1]=& _tmpFA4,Cyc_Tcutil_warn(loc,((_tmpFA7="implicit cast form region %s to region %s",_tag_dyneither(_tmpFA7,sizeof(char),42))),_tag_dyneither(_tmpFA6,sizeof(void*),2)))))))))))));}}else{
# 1999
okay=0;}}
# 2003
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D2.ptr_atts).zero_term,(_tmp5D4.ptr_atts).zero_term) || 
# 2006
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp5D2.ptr_atts).zero_term) && (_tmp5D4.elt_tq).real_const);{
# 2014
int _tmp5F4=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D4.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2017
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D4.ptr_atts).zero_term);
# 2020
okay=okay  && (Cyc_Tcutil_unify(_tmp5D2.elt_typ,_tmp5D4.elt_typ) || 
(_tmp5F4  && ((_tmp5D4.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp5D4.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp5D2.elt_typ,_tmp5D4.elt_typ));
# 2023
return okay;};}_LL3B3:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5D5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5D5->tag != 8)goto _LL3B5;else{_tmp5D6=(_tmp5D5->f1).elt_type;_tmp5D7=(_tmp5D5->f1).tq;_tmp5D8=(_tmp5D5->f1).num_elts;_tmp5D9=(_tmp5D5->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5DA->tag != 8)goto _LL3B5;else{_tmp5DB=(_tmp5DA->f1).elt_type;_tmp5DC=(_tmp5DA->f1).tq;_tmp5DD=(_tmp5DA->f1).num_elts;_tmp5DE=(_tmp5DA->f1).zero_term;}};_LL3B4: {
# 2027
int okay;
# 2029
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5D9,_tmp5DE) && (
(_tmp5D8 != 0  && _tmp5DD != 0) && Cyc_Evexp_same_const_exp(_tmp5D8,_tmp5DD));
# 2032
return(okay  && Cyc_Tcutil_unify(_tmp5D6,_tmp5DB)) && (!_tmp5D7.real_const  || _tmp5DC.real_const);}_LL3B5:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5DF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5DF->tag != 19)goto _LL3B7;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5E0->tag != 6)goto _LL3B7;};_LL3B6:
# 2034
 return 0;_LL3B7:;_LL3B8:
# 2036
 return Cyc_Tcutil_unify(t1,t2);_LL3B0:;};}
# 2040
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp5F5=Cyc_Tcutil_compress(t);_LL3C3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F5;if(_tmp5F6->tag != 5)goto _LL3C5;}_LL3C4:
 return 1;_LL3C5:;_LL3C6:
 return 0;_LL3C2:;}
# 2046
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp5F7=Cyc_Tcutil_compress(t);void*_tmp5F9;_LL3C8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7;if(_tmp5F8->tag != 5)goto _LL3CA;else{_tmp5F9=(_tmp5F8->f1).elt_typ;}}_LL3C9:
 return _tmp5F9;_LL3CA:;_LL3CB: {
const char*_tmpFAA;void*_tmpFA9;(_tmpFA9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFAA="pointer_elt_type",_tag_dyneither(_tmpFAA,sizeof(char),17))),_tag_dyneither(_tmpFA9,sizeof(void*),0)));}_LL3C7:;}
# 2052
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp5FC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrAtts*_tmp5FE;_LL3CD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FC;if(_tmp5FD->tag != 5)goto _LL3CF;else{_tmp5FE=(struct Cyc_Absyn_PtrAtts*)&(_tmp5FD->f1).ptr_atts;}}_LL3CE:
 return _tmp5FE->rgn;_LL3CF:;_LL3D0: {
const char*_tmpFAD;void*_tmpFAC;(_tmpFAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFAD="pointer_elt_type",_tag_dyneither(_tmpFAD,sizeof(char),17))),_tag_dyneither(_tmpFAC,sizeof(void*),0)));}_LL3CC:;}
# 2059
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp601=Cyc_Tcutil_compress(t);void*_tmp603;_LL3D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp601;if(_tmp602->tag != 5)goto _LL3D4;else{_tmp603=((_tmp602->f1).ptr_atts).rgn;}}_LL3D3:
# 2062
*rgn=_tmp603;
return 1;_LL3D4:;_LL3D5:
 return 0;_LL3D1:;}
# 2070
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
void*_tmp604=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp606;_LL3D7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp604;if(_tmp605->tag != 5)goto _LL3D9;else{_tmp606=((_tmp605->f1).ptr_atts).bounds;}}_LL3D8:
# 2073
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp606)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3D9:;_LL3DA:
# 2076
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D6:;}
# 2081
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp607=e->r;struct _dyneither_ptr _tmp60D;void*_tmp60F;struct Cyc_Absyn_Exp*_tmp610;_LL3DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp608=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp608->tag != 0)goto _LL3DE;else{if(((_tmp608->f1).Int_c).tag != 5)goto _LL3DE;if(((struct _tuple7)((_tmp608->f1).Int_c).val).f2 != 0)goto _LL3DE;}}_LL3DD:
 goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp609=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp609->tag != 0)goto _LL3E0;else{if(((_tmp609->f1).Char_c).tag != 2)goto _LL3E0;if(((struct _tuple5)((_tmp609->f1).Char_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp60A->tag != 0)goto _LL3E2;else{if(((_tmp60A->f1).Short_c).tag != 4)goto _LL3E2;if(((struct _tuple6)((_tmp60A->f1).Short_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp60B->tag != 0)goto _LL3E4;else{if(((_tmp60B->f1).LongLong_c).tag != 6)goto _LL3E4;if(((struct _tuple8)((_tmp60B->f1).LongLong_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
# 2087
 return 1;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp60C->tag != 0)goto _LL3E6;else{if(((_tmp60C->f1).Wchar_c).tag != 3)goto _LL3E6;_tmp60D=(struct _dyneither_ptr)((_tmp60C->f1).Wchar_c).val;}}_LL3E5: {
# 2089
unsigned long _tmp611=Cyc_strlen((struct _dyneither_ptr)_tmp60D);
int i=0;
if(_tmp611 >= 2  && *((const char*)_check_dyneither_subscript(_tmp60D,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp60D,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp60D,sizeof(char),1))== 'x'  && _tmp611 >= 3) && *((const char*)_check_dyneither_subscript(_tmp60D,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp611;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp60D,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2099
return 0;}}_LL3E6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp60E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp60E->tag != 13)goto _LL3E8;else{_tmp60F=(void*)_tmp60E->f1;_tmp610=_tmp60E->f2;}}_LL3E7:
 return Cyc_Tcutil_is_zero(_tmp610) && Cyc_Tcutil_admits_zero(_tmp60F);_LL3E8:;_LL3E9:
 return 0;_LL3DB:;}
# 2105
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2112
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2117
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2122
struct Cyc_Core_Opt Cyc_Tcutil_rko={& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={& Cyc_Tcutil_ek};
# 2129
struct Cyc_Core_Opt Cyc_Tcutil_trko={& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={& Cyc_Tcutil_tmk};
# 2134
struct Cyc_Core_Opt Cyc_Tcutil_urko={& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={& Cyc_Tcutil_umk};
# 2139
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp613;enum Cyc_Absyn_AliasQual _tmp614;struct Cyc_Absyn_Kind*_tmp612=ka;_tmp613=_tmp612->kind;_tmp614=_tmp612->aliasqual;
switch(_tmp614){case Cyc_Absyn_Aliasable: _LL3EA:
# 2143
 switch(_tmp613){case Cyc_Absyn_AnyKind: _LL3EC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3ED:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3EE:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3EF:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F0:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F1:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3EB:
# 2152
 switch(_tmp613){case Cyc_Absyn_AnyKind: _LL3F4:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F5:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F6:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F7:
 return& Cyc_Tcutil_urko;default: _LL3F8:
 break;}
# 2159
break;case Cyc_Absyn_Top: _LL3F3:
# 2161
 switch(_tmp613){case Cyc_Absyn_AnyKind: _LL3FB:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FC:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FD:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3FE:
 return& Cyc_Tcutil_trko;default: _LL3FF:
 break;}
# 2168
break;}{
# 2170
const char*_tmpFB1;void*_tmpFB0[1];struct Cyc_String_pa_PrintArg_struct _tmpFAF;(_tmpFAF.tag=0,((_tmpFAF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFB0[0]=& _tmpFAF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB1="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmpFB1,sizeof(char),26))),_tag_dyneither(_tmpFB0,sizeof(void*),1)))))));};}
# 2173
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2181
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2188
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2193
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2198
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2203
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2208
enum Cyc_Absyn_KindQual _tmp619;enum Cyc_Absyn_AliasQual _tmp61A;struct Cyc_Absyn_Kind*_tmp618=ka;_tmp619=_tmp618->kind;_tmp61A=_tmp618->aliasqual;
switch(_tmp61A){case Cyc_Absyn_Aliasable: _LL401:
# 2211
 switch(_tmp619){case Cyc_Absyn_AnyKind: _LL403:
 return& ab;case Cyc_Absyn_MemKind: _LL404:
 return& mb;case Cyc_Absyn_BoxKind: _LL405:
 return& bb;case Cyc_Absyn_RgnKind: _LL406:
 return& rb;case Cyc_Absyn_EffKind: _LL407:
 return& eb;case Cyc_Absyn_IntKind: _LL408:
 return& ib;}case Cyc_Absyn_Unique: _LL402:
# 2220
 switch(_tmp619){case Cyc_Absyn_AnyKind: _LL40B:
 return& uab;case Cyc_Absyn_MemKind: _LL40C:
 return& umb;case Cyc_Absyn_BoxKind: _LL40D:
 return& ubb;case Cyc_Absyn_RgnKind: _LL40E:
 return& urb;default: _LL40F:
 break;}
# 2227
break;case Cyc_Absyn_Top: _LL40A:
# 2229
 switch(_tmp619){case Cyc_Absyn_AnyKind: _LL412:
 return& tab;case Cyc_Absyn_MemKind: _LL413:
 return& tmb;case Cyc_Absyn_BoxKind: _LL414:
 return& tbb;case Cyc_Absyn_RgnKind: _LL415:
 return& trb;default: _LL416:
 break;}
# 2236
break;}{
# 2238
const char*_tmpFB5;void*_tmpFB4[1];struct Cyc_String_pa_PrintArg_struct _tmpFB3;(_tmpFB3.tag=0,((_tmpFB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFB4[0]=& _tmpFB3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB5="kind_to_b: bad kind %s\n",_tag_dyneither(_tmpFB5,sizeof(char),24))),_tag_dyneither(_tmpFB4,sizeof(void*),1)))))));};}
# 2241
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2244
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2246
struct Cyc_Core_Opt*_tmpFB6;return(_tmpFB6=_cycalloc(sizeof(*_tmpFB6)),((_tmpFB6->v=Cyc_Tcutil_kind_to_bound(k),_tmpFB6)));}
# 2251
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpFB9;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFB8;e1->r=(void*)((_tmpFB8=_cycalloc(sizeof(*_tmpFB8)),((_tmpFB8[0]=((_tmpFB9.tag=0,((_tmpFB9.f1=Cyc_Absyn_Null_c,_tmpFB9)))),_tmpFB8))));}{
struct Cyc_Core_Opt*_tmp62F=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpFC3;struct Cyc_Absyn_PtrAtts _tmpFC2;struct Cyc_Absyn_PtrInfo _tmpFC1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFC0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp630=(_tmpFC0=_cycalloc(sizeof(*_tmpFC0)),((_tmpFC0[0]=((_tmpFC3.tag=5,((_tmpFC3.f1=((_tmpFC1.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp62F),((_tmpFC1.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmpFC1.ptr_atts=(
(_tmpFC2.rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp62F),((_tmpFC2.nullable=Cyc_Absyn_true_conref,((_tmpFC2.bounds=
# 2259
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmpFC2.zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmpFC2.ptrloc=0,_tmpFC2)))))))))),_tmpFC1)))))),_tmpFC3)))),_tmpFC0)));
e1->topt=(void*)_tmp630;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmpFC8;void*_tmpFC7[2];struct Cyc_String_pa_PrintArg_struct _tmpFC6;struct Cyc_String_pa_PrintArg_struct _tmpFC5;(_tmpFC5.tag=0,((_tmpFC5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmpFC6.tag=0,((_tmpFC6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmpFC7[0]=& _tmpFC6,((_tmpFC7[1]=& _tmpFC5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC8="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmpFC8,sizeof(char),56))),_tag_dyneither(_tmpFC7,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2269
return 0;}
# 2272
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL418: {
const char*_tmpFC9;return(_tmpFC9="unknown",_tag_dyneither(_tmpFC9,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL419: {
const char*_tmpFCA;return(_tmpFCA="no coercion",_tag_dyneither(_tmpFCA,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41A: {
const char*_tmpFCB;return(_tmpFCB="null check",_tag_dyneither(_tmpFCB,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41B: {
const char*_tmpFCC;return(_tmpFCC="other coercion",_tag_dyneither(_tmpFCC,sizeof(char),15));}}}
# 2281
int Cyc_Tcutil_warn_alias_coerce=0;
# 2287
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2291
static int counter=0;
struct _dyneither_ptr*_tmpFD9;const char*_tmpFD8;void*_tmpFD7[1];struct Cyc_Int_pa_PrintArg_struct _tmpFD6;struct _tuple2*_tmpFD5;struct _tuple2*v=(_tmpFD5=_cycalloc(sizeof(*_tmpFD5)),((_tmpFD5->f1=Cyc_Absyn_Loc_n,((_tmpFD5->f2=((_tmpFD9=_cycalloc(sizeof(*_tmpFD9)),((_tmpFD9[0]=(struct _dyneither_ptr)((_tmpFD6.tag=1,((_tmpFD6.f1=(unsigned long)counter ++,((_tmpFD7[0]=& _tmpFD6,Cyc_aprintf(((_tmpFD8="__aliasvar%d",_tag_dyneither(_tmpFD8,sizeof(char),13))),_tag_dyneither(_tmpFD7,sizeof(void*),1)))))))),_tmpFD9)))),_tmpFD5)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpFDC;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpFDB;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmpFDB=_cycalloc(sizeof(*_tmpFDB)),((_tmpFDB[0]=((_tmpFDC.tag=4,((_tmpFDC.f1=vd,_tmpFDC)))),_tmpFDB)))),e->loc);
# 2297
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
{void*_tmp63D=Cyc_Tcutil_compress(e_typ);void**_tmp63F;_LL41E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp63E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63D;if(_tmp63E->tag != 5)goto _LL420;else{_tmp63F=(void**)&((_tmp63E->f1).ptr_atts).rgn;}}_LL41F:
# 2300
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFDF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFDE;*_tmp63F=(void*)((_tmpFDE=_cycalloc(sizeof(*_tmpFDE)),((_tmpFDE[0]=((_tmpFDF.tag=2,((_tmpFDF.f1=tv,_tmpFDF)))),_tmpFDE))));}goto _LL41D;_LL420:;_LL421:
 goto _LL41D;_LL41D:;}
# 2303
e->topt=0;
vd->type=e_typ;
vd->initializer=e;{
# 2308
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e->loc);
# 2310
struct _tuple15 _tmpFE0;return(_tmpFE0.f1=d,((_tmpFE0.f2=ve,_tmpFE0)));};}
# 2315
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2318
struct _RegionHandle _tmp64B=_new_region("r");struct _RegionHandle*r=& _tmp64B;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2323
void*_tmp64C=Cyc_Tcutil_compress(wants_typ);void*_tmp64E;_LL423: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp64D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64C;if(_tmp64D->tag != 5)goto _LL425;else{_tmp64E=((_tmp64D->f1).ptr_atts).rgn;}}_LL424: {
# 2325
void*_tmp650=Cyc_Tcutil_compress(_tmp64E);_LL42A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp651=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp650;if(_tmp651->tag != 20)goto _LL42C;}_LL42B: {
int _tmp652=0;_npop_handler(0);return _tmp652;}_LL42C:;_LL42D: {
# 2328
struct Cyc_Absyn_Kind*_tmp653=Cyc_Tcutil_typ_kind(_tmp64E);
int _tmp654=_tmp653->kind == Cyc_Absyn_RgnKind  && _tmp653->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp654;}_LL429:;}_LL425: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp64F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp64C;if(_tmp64F->tag != 17)goto _LL427;}_LL426: {
# 2332
const char*_tmpFE5;void*_tmpFE4[2];struct Cyc_String_pa_PrintArg_struct _tmpFE3;struct Cyc_String_pa_PrintArg_struct _tmpFE2;(_tmpFE2.tag=0,((_tmpFE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmpFE3.tag=0,((_tmpFE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmpFE4[0]=& _tmpFE3,((_tmpFE4[1]=& _tmpFE2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFE5="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmpFE5,sizeof(char),42))),_tag_dyneither(_tmpFE4,sizeof(void*),2)))))))))))));}_LL427:;_LL428: {
int _tmp659=0;_npop_handler(0);return _tmp659;}_LL422:;}{
# 2337
int _tmp65A=0;_npop_handler(0);return _tmp65A;};
# 2319
;_pop_region(r);}
# 2341
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2346
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2348
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2350
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmpFEA;void*_tmpFE9[2];struct Cyc_String_pa_PrintArg_struct _tmpFE8;struct Cyc_String_pa_PrintArg_struct _tmpFE7;(_tmpFE7.tag=0,((_tmpFE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFE8.tag=0,((_tmpFE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFE9[0]=& _tmpFE8,((_tmpFE9[1]=& _tmpFE7,Cyc_Tcutil_warn(e->loc,((_tmpFEA="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmpFEA,sizeof(char),53))),_tag_dyneither(_tmpFE9,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2359
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmpFF0;void*_tmpFEF[3];struct Cyc_String_pa_PrintArg_struct _tmpFEE;struct Cyc_String_pa_PrintArg_struct _tmpFED;struct Cyc_String_pa_PrintArg_struct _tmpFEC;(_tmpFEC.tag=0,((_tmpFEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFED.tag=0,((_tmpFED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFEE.tag=0,((_tmpFEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpFEF[0]=& _tmpFEE,((_tmpFEF[1]=& _tmpFED,((_tmpFEF[2]=& _tmpFEC,Cyc_Tcutil_warn(e->loc,((_tmpFF0="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmpFF0,sizeof(char),40))),_tag_dyneither(_tmpFEF,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2366
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2374
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
# 2376
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmpFF5;void*_tmpFF4[2];struct Cyc_String_pa_PrintArg_struct _tmpFF3;struct Cyc_String_pa_PrintArg_struct _tmpFF2;(_tmpFF2.tag=0,((_tmpFF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFF3.tag=0,((_tmpFF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFF4[0]=& _tmpFF3,((_tmpFF4[1]=& _tmpFF2,Cyc_Tcutil_warn(e->loc,((_tmpFF5="implicit cast from %s to %s",_tag_dyneither(_tmpFF5,sizeof(char),28))),_tag_dyneither(_tmpFF4,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2381
return 0;}}}}}
# 2388
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2391
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2395
int Cyc_Tcutil_coerceable(void*t){
void*_tmp668=Cyc_Tcutil_compress(t);_LL42F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp669=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp668;if(_tmp669->tag != 6)goto _LL431;}_LL430:
 goto _LL432;_LL431: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp66A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp668;if(_tmp66A->tag != 7)goto _LL433;}_LL432:
 return 1;_LL433:;_LL434:
 return 0;_LL42E:;}
# 2413 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2417
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2420
struct Cyc_List_List*_tmp66C;struct _RegionHandle*_tmp66D;struct Cyc_Tcenv_Tenv*_tmp66E;int _tmp66F;struct _tuple23 _tmp66B=*env;_tmp66C=_tmp66B.f1;_tmp66D=_tmp66B.f2;_tmp66E=_tmp66B.f3;_tmp66F=_tmp66B.f4;{
# 2422
void*_tmp670=_tmp66C == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp66D,_tmp66C,x->type);
struct Cyc_List_List*_tmp671=Cyc_Tcutil_flatten_typ(_tmp66D,_tmp66F,_tmp66E,_tmp670);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp671)== 1){
struct _tuple12*_tmpFF8;struct Cyc_List_List*_tmpFF7;return(_tmpFF7=_region_malloc(_tmp66D,sizeof(*_tmpFF7)),((_tmpFF7->hd=((_tmpFF8=_region_malloc(_tmp66D,sizeof(*_tmpFF8)),((_tmpFF8->f1=x->tq,((_tmpFF8->f2=_tmp670,_tmpFF8)))))),((_tmpFF7->tl=0,_tmpFF7)))));}else{
return _tmp671;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2428
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2430
struct _RegionHandle*_tmp675;struct Cyc_Tcenv_Tenv*_tmp676;int _tmp677;struct _tuple24 _tmp674=*env;_tmp675=_tmp674.f1;_tmp676=_tmp674.f2;_tmp677=_tmp674.f3;{
struct Cyc_Absyn_Tqual _tmp679;void*_tmp67A;struct _tuple12 _tmp678=*x;_tmp679=_tmp678.f1;_tmp67A=_tmp678.f2;{
struct Cyc_List_List*_tmp67B=Cyc_Tcutil_flatten_typ(_tmp675,_tmp677,_tmp676,_tmp67A);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp67B)== 1){
struct _tuple12*_tmpFFB;struct Cyc_List_List*_tmpFFA;return(_tmpFFA=_region_malloc(_tmp675,sizeof(*_tmpFFA)),((_tmpFFA->hd=((_tmpFFB=_region_malloc(_tmp675,sizeof(*_tmpFFB)),((_tmpFFB->f1=_tmp679,((_tmpFFB->f2=_tmp67A,_tmpFFB)))))),((_tmpFFA->tl=0,_tmpFFA)))));}else{
return _tmp67B;}};};}
# 2437
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2441
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp67E=t1;struct Cyc_List_List*_tmp681;struct Cyc_Absyn_Aggrdecl*_tmp683;struct Cyc_List_List*_tmp684;struct Cyc_List_List*_tmp686;_LL436: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp67F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp67E;if(_tmp67F->tag != 0)goto _LL438;}_LL437:
 return 0;_LL438: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp680=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp67E;if(_tmp680->tag != 10)goto _LL43A;else{_tmp681=_tmp680->f1;}}_LL439: {
# 2446
struct _tuple24 _tmpFFC;struct _tuple24 _tmp687=(_tmpFFC.f1=r,((_tmpFFC.f2=te,((_tmpFFC.f3=flatten,_tmpFFC)))));
# 2448
struct Cyc_List_List*_tmp688=_tmp681;struct _tuple12*_tmp689;struct Cyc_List_List*_tmp68A;_LL441: if(_tmp688 != 0)goto _LL443;_LL442:
 return 0;_LL443: if(_tmp688 == 0)goto _LL440;_tmp689=(struct _tuple12*)_tmp688->hd;_tmp68A=_tmp688->tl;_LL444: {
# 2451
struct Cyc_List_List*_tmp68B=Cyc_Tcutil_rcopy_tqt(& _tmp687,_tmp689);
_tmp687.f3=0;{
struct Cyc_List_List*_tmp68C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp687,_tmp681);
struct Cyc_List_List*_tmpFFD;struct Cyc_List_List*_tmp68D=(_tmpFFD=_region_malloc(r,sizeof(*_tmpFFD)),((_tmpFFD->hd=_tmp68B,((_tmpFFD->tl=_tmp68C,_tmpFFD)))));
struct Cyc_List_List*_tmpFFE;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmpFFE=_region_malloc(r,sizeof(*_tmpFFE)),((_tmpFFE->hd=_tmp68B,((_tmpFFE->tl=_tmp68C,_tmpFFE)))))));};}_LL440:;}_LL43A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp682=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67E;if(_tmp682->tag != 11)goto _LL43C;else{if((((_tmp682->f1).aggr_info).KnownAggr).tag != 2)goto _LL43C;_tmp683=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp682->f1).aggr_info).KnownAggr).val);_tmp684=(_tmp682->f1).targs;}}_LL43B:
# 2459
 if(((_tmp683->kind == Cyc_Absyn_UnionA  || _tmp683->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp683->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp683->impl))->rgn_po != 0){
# 2461
struct _tuple12*_tmp1001;struct Cyc_List_List*_tmp1000;return(_tmp1000=_region_malloc(r,sizeof(*_tmp1000)),((_tmp1000->hd=((_tmp1001=_region_malloc(r,sizeof(*_tmp1001)),((_tmp1001->f1=Cyc_Absyn_empty_tqual(0),((_tmp1001->f2=t1,_tmp1001)))))),((_tmp1000->tl=0,_tmp1000)))));}{
struct Cyc_List_List*_tmp693=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp683->tvs,_tmp684);
struct _tuple23 _tmp1002;struct _tuple23 env=(_tmp1002.f1=_tmp693,((_tmp1002.f2=r,((_tmp1002.f3=te,((_tmp1002.f4=flatten,_tmp1002)))))));
struct Cyc_List_List*_tmp694=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp683->impl))->fields;struct Cyc_Absyn_Aggrfield*_tmp695;struct Cyc_List_List*_tmp696;_LL446: if(_tmp694 != 0)goto _LL448;_LL447:
 return 0;_LL448: if(_tmp694 == 0)goto _LL445;_tmp695=(struct Cyc_Absyn_Aggrfield*)_tmp694->hd;_tmp696=_tmp694->tl;_LL449: {
# 2467
struct Cyc_List_List*_tmp697=Cyc_Tcutil_flatten_typ_f(& env,_tmp695);
env.f4=0;{
struct Cyc_List_List*_tmp698=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp696);
struct Cyc_List_List*_tmp1003;struct Cyc_List_List*_tmp699=(_tmp1003=_region_malloc(r,sizeof(*_tmp1003)),((_tmp1003->hd=_tmp697,((_tmp1003->tl=_tmp698,_tmp1003)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp699);};}_LL445:;};_LL43C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp685=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp67E;if(_tmp685->tag != 12)goto _LL43E;else{if(_tmp685->f1 != Cyc_Absyn_StructA)goto _LL43E;_tmp686=_tmp685->f2;}}_LL43D: {
# 2474
struct _tuple23 _tmp1004;struct _tuple23 env=(_tmp1004.f1=0,((_tmp1004.f2=r,((_tmp1004.f3=te,((_tmp1004.f4=flatten,_tmp1004)))))));
struct Cyc_List_List*_tmp69C=_tmp686;struct Cyc_Absyn_Aggrfield*_tmp69D;struct Cyc_List_List*_tmp69E;_LL44B: if(_tmp69C != 0)goto _LL44D;_LL44C:
 return 0;_LL44D: if(_tmp69C == 0)goto _LL44A;_tmp69D=(struct Cyc_Absyn_Aggrfield*)_tmp69C->hd;_tmp69E=_tmp69C->tl;_LL44E: {
# 2478
struct Cyc_List_List*_tmp69F=Cyc_Tcutil_flatten_typ_f(& env,_tmp69D);
env.f4=0;{
struct Cyc_List_List*_tmp6A0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp69E);
struct Cyc_List_List*_tmp1005;struct Cyc_List_List*_tmp6A1=(_tmp1005=_region_malloc(r,sizeof(*_tmp1005)),((_tmp1005->hd=_tmp69F,((_tmp1005->tl=_tmp6A0,_tmp1005)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6A1);};}_LL44A:;}_LL43E:;_LL43F:
# 2484
 goto _LL435;_LL435:;};}{
# 2487
struct _tuple12*_tmp1008;struct Cyc_List_List*_tmp1007;return(_tmp1007=_region_malloc(r,sizeof(*_tmp1007)),((_tmp1007->hd=((_tmp1008=_region_malloc(r,sizeof(*_tmp1008)),((_tmp1008->f1=Cyc_Absyn_empty_tqual(0),((_tmp1008->f2=t1,_tmp1008)))))),((_tmp1007->tl=0,_tmp1007)))));};}
# 2491
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp6A6=(void*)t->hd;_LL450: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6A7=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6A6;if(_tmp6A7->tag != 23)goto _LL452;}_LL451:
 goto _LL453;_LL452: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6A8=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6A6;if(_tmp6A8->tag != 4)goto _LL454;}_LL453:
 goto _LL455;_LL454: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6A9=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6A6;if(_tmp6A9->tag != 20)goto _LL456;}_LL455:
# 2497
 continue;_LL456:;_LL457:
# 2499
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL44F:;}}
# 2502
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2505
return 1;}
# 2508
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1009;struct _tuple0 _tmp6AB=(_tmp1009.f1=Cyc_Tcutil_compress(t1),((_tmp1009.f2=Cyc_Tcutil_compress(t2),_tmp1009)));enum Cyc_Absyn_Size_of _tmp6AD;enum Cyc_Absyn_Size_of _tmp6AF;_LL459:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6AC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6AB.f1;if(_tmp6AC->tag != 6)goto _LL45B;else{_tmp6AD=_tmp6AC->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6AB.f2;if(_tmp6AE->tag != 6)goto _LL45B;else{_tmp6AF=_tmp6AE->f2;}};_LL45A:
# 2511
 return(_tmp6AD == _tmp6AF  || _tmp6AD == Cyc_Absyn_Int_sz  && _tmp6AF == Cyc_Absyn_Long_sz) || 
_tmp6AD == Cyc_Absyn_Long_sz  && _tmp6AF == Cyc_Absyn_Int_sz;_LL45B:;_LL45C:
 return 0;_LL458:;}
# 2519
static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2522
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2527
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp100A;struct _tuple0 _tmp6B1=(_tmp100A.f1=t1,((_tmp100A.f2=t2,_tmp100A)));void*_tmp6B3;struct Cyc_Absyn_Tqual _tmp6B4;void*_tmp6B5;union Cyc_Absyn_Constraint*_tmp6B6;union Cyc_Absyn_Constraint*_tmp6B7;union Cyc_Absyn_Constraint*_tmp6B8;void*_tmp6BA;struct Cyc_Absyn_Tqual _tmp6BB;void*_tmp6BC;union Cyc_Absyn_Constraint*_tmp6BD;union Cyc_Absyn_Constraint*_tmp6BE;union Cyc_Absyn_Constraint*_tmp6BF;struct Cyc_Absyn_Datatypedecl*_tmp6C1;struct Cyc_Absyn_Datatypefield*_tmp6C2;struct Cyc_List_List*_tmp6C3;struct Cyc_Absyn_Datatypedecl*_tmp6C5;struct Cyc_List_List*_tmp6C6;struct Cyc_Absyn_FnInfo _tmp6C8;struct Cyc_Absyn_FnInfo _tmp6CA;_LL45E:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B1.f1;if(_tmp6B2->tag != 5)goto _LL460;else{_tmp6B3=(_tmp6B2->f1).elt_typ;_tmp6B4=(_tmp6B2->f1).elt_tq;_tmp6B5=((_tmp6B2->f1).ptr_atts).rgn;_tmp6B6=((_tmp6B2->f1).ptr_atts).nullable;_tmp6B7=((_tmp6B2->f1).ptr_atts).bounds;_tmp6B8=((_tmp6B2->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B1.f2;if(_tmp6B9->tag != 5)goto _LL460;else{_tmp6BA=(_tmp6B9->f1).elt_typ;_tmp6BB=(_tmp6B9->f1).elt_tq;_tmp6BC=((_tmp6B9->f1).ptr_atts).rgn;_tmp6BD=((_tmp6B9->f1).ptr_atts).nullable;_tmp6BE=((_tmp6B9->f1).ptr_atts).bounds;_tmp6BF=((_tmp6B9->f1).ptr_atts).zero_term;}};_LL45F:
# 2535
 if(_tmp6B4.real_const  && !_tmp6BB.real_const)
return 0;
# 2538
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6B6,_tmp6BD) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6B6)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6BD))
return 0;
# 2542
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6B8,_tmp6BF) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6B8)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6BF))
return 0;
# 2546
if(!Cyc_Tcutil_unify(_tmp6B5,_tmp6BC) && !Cyc_Tcenv_region_outlives(te,_tmp6B5,_tmp6BC))
return 0;
# 2549
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6B7,_tmp6BE)){
struct _tuple0 _tmp100B;struct _tuple0 _tmp6CC=(_tmp100B.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6B7),((_tmp100B.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6BE),_tmp100B)));struct Cyc_Absyn_Exp*_tmp6D0;struct Cyc_Absyn_Exp*_tmp6D2;_LL467:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6CD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6CC.f1;if(_tmp6CD->tag != 1)goto _LL469;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6CE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6CC.f2;if(_tmp6CE->tag != 0)goto _LL469;};_LL468:
 goto _LL466;_LL469:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6CF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6CC.f1;if(_tmp6CF->tag != 1)goto _LL46B;else{_tmp6D0=_tmp6CF->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6CC.f2;if(_tmp6D1->tag != 1)goto _LL46B;else{_tmp6D2=_tmp6D1->f1;}};_LL46A:
# 2553
 if(!Cyc_Evexp_lte_const_exp(_tmp6D2,_tmp6D0))
return 0;
goto _LL466;_LL46B:;_LL46C:
 return 0;_LL466:;}
# 2561
if(!_tmp6BB.real_const  && _tmp6B4.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6BA)))
return 0;}{
# 2567
int _tmp6D3=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6BE,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp6BF);
# 2573
struct _tuple0*_tmp100E;struct Cyc_List_List*_tmp100D;return(_tmp6D3  && Cyc_Tcutil_ptrsubtype(te,((_tmp100D=_cycalloc(sizeof(*_tmp100D)),((_tmp100D->hd=((_tmp100E=_cycalloc(sizeof(*_tmp100E)),((_tmp100E->f1=t1,((_tmp100E->f2=t2,_tmp100E)))))),((_tmp100D->tl=assume,_tmp100D)))))),_tmp6B3,_tmp6BA) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(t1,t2);};_LL460:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6C0=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6B1.f1;if(_tmp6C0->tag != 4)goto _LL462;else{if((((_tmp6C0->f1).field_info).KnownDatatypefield).tag != 2)goto _LL462;_tmp6C1=((struct _tuple3)(((_tmp6C0->f1).field_info).KnownDatatypefield).val).f1;_tmp6C2=((struct _tuple3)(((_tmp6C0->f1).field_info).KnownDatatypefield).val).f2;_tmp6C3=(_tmp6C0->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6C4=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6B1.f2;if(_tmp6C4->tag != 3)goto _LL462;else{if((((_tmp6C4->f1).datatype_info).KnownDatatype).tag != 2)goto _LL462;_tmp6C5=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp6C4->f1).datatype_info).KnownDatatype).val);_tmp6C6=(_tmp6C4->f1).targs;}};_LL461:
# 2579
 if(_tmp6C1 != _tmp6C5  && Cyc_Absyn_qvar_cmp(_tmp6C1->name,_tmp6C5->name)!= 0)return 0;
# 2581
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6C3)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6C6))return 0;
for(0;_tmp6C3 != 0;(_tmp6C3=_tmp6C3->tl,_tmp6C6=_tmp6C6->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp6C3->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6C6))->hd))return 0;}
return 1;_LL462:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6C7=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B1.f1;if(_tmp6C7->tag != 9)goto _LL464;else{_tmp6C8=_tmp6C7->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6C9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B1.f2;if(_tmp6C9->tag != 9)goto _LL464;else{_tmp6CA=_tmp6C9->f1;}};_LL463:
# 2588
 if(_tmp6C8.tvars != 0  || _tmp6CA.tvars != 0){
struct Cyc_List_List*_tmp6D6=_tmp6C8.tvars;
struct Cyc_List_List*_tmp6D7=_tmp6CA.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D7))return 0;{
struct _RegionHandle _tmp6D8=_new_region("r");struct _RegionHandle*r=& _tmp6D8;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp6D6 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp6D6->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp6D7))->hd)->kind)){int _tmp6D9=0;_npop_handler(0);return _tmp6D9;}
{struct _tuple16*_tmp1018;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1017;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1016;struct Cyc_List_List*_tmp1015;inst=((_tmp1015=_region_malloc(r,sizeof(*_tmp1015)),((_tmp1015->hd=((_tmp1018=_region_malloc(r,sizeof(*_tmp1018)),((_tmp1018->f1=(struct Cyc_Absyn_Tvar*)_tmp6D7->hd,((_tmp1018->f2=(void*)((_tmp1016=_cycalloc(sizeof(*_tmp1016)),((_tmp1016[0]=((_tmp1017.tag=2,((_tmp1017.f1=(struct Cyc_Absyn_Tvar*)_tmp6D6->hd,_tmp1017)))),_tmp1016)))),_tmp1018)))))),((_tmp1015->tl=inst,_tmp1015))))));}
_tmp6D6=_tmp6D6->tl;
_tmp6D7=_tmp6D7->tl;}
# 2600
if(inst != 0){
_tmp6C8.tvars=0;
_tmp6CA.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp101E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp101D;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp101B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp101A;int _tmp6E2=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp101A=_cycalloc(sizeof(*_tmp101A)),((_tmp101A[0]=((_tmp101B.tag=9,((_tmp101B.f1=_tmp6C8,_tmp101B)))),_tmp101A)))),(void*)((_tmp101D=_cycalloc(sizeof(*_tmp101D)),((_tmp101D[0]=((_tmp101E.tag=9,((_tmp101E.f1=_tmp6CA,_tmp101E)))),_tmp101D)))));_npop_handler(0);return _tmp6E2;};}}
# 2593
;_pop_region(r);};}
# 2608
if(!Cyc_Tcutil_subtype(te,assume,_tmp6C8.ret_typ,_tmp6CA.ret_typ))return 0;{
struct Cyc_List_List*_tmp6E3=_tmp6C8.args;
struct Cyc_List_List*_tmp6E4=_tmp6CA.args;
# 2613
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E3)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E4))return 0;
# 2615
for(0;_tmp6E3 != 0;(_tmp6E3=_tmp6E3->tl,_tmp6E4=_tmp6E4->tl)){
struct Cyc_Absyn_Tqual _tmp6E6;void*_tmp6E7;struct _tuple10 _tmp6E5=*((struct _tuple10*)_tmp6E3->hd);_tmp6E6=_tmp6E5.f2;_tmp6E7=_tmp6E5.f3;{
struct Cyc_Absyn_Tqual _tmp6E9;void*_tmp6EA;struct _tuple10 _tmp6E8=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6E4))->hd);_tmp6E9=_tmp6E8.f2;_tmp6EA=_tmp6E8.f3;
# 2619
if(_tmp6E9.real_const  && !_tmp6E6.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp6EA,_tmp6E7))
return 0;};}
# 2623
if(_tmp6C8.c_varargs != _tmp6CA.c_varargs)return 0;
if(_tmp6C8.cyc_varargs != 0  && _tmp6CA.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp6EB=*_tmp6C8.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp6EC=*_tmp6CA.cyc_varargs;
# 2628
if((_tmp6EC.tq).real_const  && !(_tmp6EB.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp6EC.type,_tmp6EB.type))
return 0;}else{
if(_tmp6C8.cyc_varargs != 0  || _tmp6CA.cyc_varargs != 0)return 0;}
# 2633
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp6C8.effect),(void*)_check_null(_tmp6CA.effect)))return 0;
# 2635
if(!Cyc_Tcutil_sub_rgnpo(_tmp6C8.rgn_po,_tmp6CA.rgn_po))return 0;
# 2637
if(!Cyc_Tcutil_sub_attributes(_tmp6C8.attributes,_tmp6CA.attributes))return 0;
# 2639
return 1;};_LL464:;_LL465:
 return 0;_LL45D:;};}
# 2651 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2653
struct _RegionHandle _tmp6ED=_new_region("temp");struct _RegionHandle*temp=& _tmp6ED;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp6EE=0;_npop_handler(0);return _tmp6EE;}{
struct Cyc_Absyn_Tqual _tmp6F0;void*_tmp6F1;struct _tuple12*_tmp6EF=(struct _tuple12*)tqs1->hd;_tmp6F0=_tmp6EF->f1;_tmp6F1=_tmp6EF->f2;{
struct Cyc_Absyn_Tqual _tmp6F3;void*_tmp6F4;struct _tuple12*_tmp6F2=(struct _tuple12*)tqs2->hd;_tmp6F3=_tmp6F2->f1;_tmp6F4=_tmp6F2->f2;
# 2661
if(_tmp6F0.real_const  && !_tmp6F3.real_const){int _tmp6F5=0;_npop_handler(0);return _tmp6F5;}
# 2663
if((_tmp6F3.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6F4))) && 
Cyc_Tcutil_subtype(te,assume,_tmp6F1,_tmp6F4))
# 2666
continue;
# 2668
if(Cyc_Tcutil_unify(_tmp6F1,_tmp6F4))
# 2670
continue;
# 2672
if(Cyc_Tcutil_isomorphic(_tmp6F1,_tmp6F4))
# 2674
continue;{
# 2677
int _tmp6F6=0;_npop_handler(0);return _tmp6F6;};};};}{
# 2679
int _tmp6F7=1;_npop_handler(0);return _tmp6F7;};}
# 2654
;_pop_region(temp);}
# 2683
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp6F8=Cyc_Tcutil_compress(t);_LL46E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6F9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F8;if(_tmp6F9->tag != 6)goto _LL470;else{if(_tmp6F9->f2 != Cyc_Absyn_Char_sz)goto _LL470;}}_LL46F:
 return 1;_LL470:;_LL471:
 return 0;_LL46D:;}
# 2692
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2698
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp6FA=t2;_LL473: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6FB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6FA;if(_tmp6FB->tag != 6)goto _LL475;else{if(_tmp6FB->f2 != Cyc_Absyn_Int_sz)goto _LL475;}}_LL474:
# 2702
 goto _LL476;_LL475: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6FC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6FA;if(_tmp6FC->tag != 6)goto _LL477;else{if(_tmp6FC->f2 != Cyc_Absyn_Long_sz)goto _LL477;}}_LL476:
# 2704
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL472;_LL477:;_LL478:
 goto _LL472;_LL472:;}{
# 2708
void*_tmp6FD=t1;void*_tmp6FF;struct Cyc_Absyn_Tqual _tmp700;void*_tmp701;union Cyc_Absyn_Constraint*_tmp702;union Cyc_Absyn_Constraint*_tmp703;union Cyc_Absyn_Constraint*_tmp704;void*_tmp706;struct Cyc_Absyn_Tqual _tmp707;struct Cyc_Absyn_Exp*_tmp708;union Cyc_Absyn_Constraint*_tmp709;struct Cyc_Absyn_Enumdecl*_tmp70B;void*_tmp70F;_LL47A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6FE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6FD;if(_tmp6FE->tag != 5)goto _LL47C;else{_tmp6FF=(_tmp6FE->f1).elt_typ;_tmp700=(_tmp6FE->f1).elt_tq;_tmp701=((_tmp6FE->f1).ptr_atts).rgn;_tmp702=((_tmp6FE->f1).ptr_atts).nullable;_tmp703=((_tmp6FE->f1).ptr_atts).bounds;_tmp704=((_tmp6FE->f1).ptr_atts).zero_term;}}_LL47B:
# 2716
{void*_tmp710=t2;void*_tmp712;struct Cyc_Absyn_Tqual _tmp713;void*_tmp714;union Cyc_Absyn_Constraint*_tmp715;union Cyc_Absyn_Constraint*_tmp716;union Cyc_Absyn_Constraint*_tmp717;_LL489: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp711=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp710;if(_tmp711->tag != 5)goto _LL48B;else{_tmp712=(_tmp711->f1).elt_typ;_tmp713=(_tmp711->f1).elt_tq;_tmp714=((_tmp711->f1).ptr_atts).rgn;_tmp715=((_tmp711->f1).ptr_atts).nullable;_tmp716=((_tmp711->f1).ptr_atts).bounds;_tmp717=((_tmp711->f1).ptr_atts).zero_term;}}_LL48A: {
# 2720
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp1021;struct Cyc_List_List*_tmp1020;struct Cyc_List_List*_tmp718=(_tmp1020=_cycalloc(sizeof(*_tmp1020)),((_tmp1020->hd=((_tmp1021=_cycalloc(sizeof(*_tmp1021)),((_tmp1021->f1=t1,((_tmp1021->f2=t2,_tmp1021)))))),((_tmp1020->tl=0,_tmp1020)))));
int _tmp719=_tmp713.real_const  || !_tmp700.real_const;
# 2734 "tcutil.cyc"
int _tmp71A=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp716,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp717);
# 2738
int _tmp71B=_tmp719  && (
((_tmp71A  && Cyc_Tcutil_ptrsubtype(te,_tmp718,_tmp6FF,_tmp712) || 
Cyc_Tcutil_unify(_tmp6FF,_tmp712)) || Cyc_Tcutil_isomorphic(_tmp6FF,_tmp712)) || Cyc_Tcutil_unify(_tmp712,(void*)& Cyc_Absyn_VoidType_val));
# 2742
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp704,_tmp717) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp717);
# 2746
int _tmp71C=_tmp71B?0:((Cyc_Tcutil_bits_only(_tmp6FF) && Cyc_Tcutil_is_char_type(_tmp712)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp717)) && (
_tmp713.real_const  || !_tmp700.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp703,_tmp716);
if(!bounds_ok  && !_tmp71C){
struct _tuple0 _tmp1022;struct _tuple0 _tmp71E=(_tmp1022.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp703),((_tmp1022.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp716),_tmp1022)));struct Cyc_Absyn_Exp*_tmp720;struct Cyc_Absyn_Exp*_tmp722;_LL48E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp71F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp71E.f1;if(_tmp71F->tag != 1)goto _LL490;else{_tmp720=_tmp71F->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp721=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp71E.f2;if(_tmp721->tag != 1)goto _LL490;else{_tmp722=_tmp721->f1;}};_LL48F:
# 2756
 if(Cyc_Evexp_lte_const_exp(_tmp722,_tmp720))
bounds_ok=1;
goto _LL48D;_LL490:;_LL491:
# 2761
 bounds_ok=1;goto _LL48D;_LL48D:;}
# 2763
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp702) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp715))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2768
if(((bounds_ok  && zeroterm_ok) && (_tmp71B  || _tmp71C)) && (
Cyc_Tcutil_unify(_tmp701,_tmp714) || Cyc_Tcenv_region_outlives(te,_tmp701,_tmp714)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL48B:;_LL48C:
 goto _LL488;_LL488:;}
# 2774
return Cyc_Absyn_Unknown_coercion;_LL47C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp705=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6FD;if(_tmp705->tag != 8)goto _LL47E;else{_tmp706=(_tmp705->f1).elt_type;_tmp707=(_tmp705->f1).tq;_tmp708=(_tmp705->f1).num_elts;_tmp709=(_tmp705->f1).zero_term;}}_LL47D:
# 2776
{void*_tmp725=t2;void*_tmp727;struct Cyc_Absyn_Tqual _tmp728;struct Cyc_Absyn_Exp*_tmp729;union Cyc_Absyn_Constraint*_tmp72A;_LL493: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp726=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp725;if(_tmp726->tag != 8)goto _LL495;else{_tmp727=(_tmp726->f1).elt_type;_tmp728=(_tmp726->f1).tq;_tmp729=(_tmp726->f1).num_elts;_tmp72A=(_tmp726->f1).zero_term;}}_LL494: {
# 2778
int okay;
okay=
((_tmp708 != 0  && _tmp729 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp709,_tmp72A)) && 
Cyc_Evexp_lte_const_exp(_tmp729,_tmp708);
return
# 2784
(okay  && Cyc_Tcutil_unify(_tmp706,_tmp727)) && (!_tmp707.real_const  || _tmp728.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL495:;_LL496:
# 2786
 return Cyc_Absyn_Unknown_coercion;_LL492:;}
# 2788
return Cyc_Absyn_Unknown_coercion;_LL47E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp70A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6FD;if(_tmp70A->tag != 13)goto _LL480;else{_tmp70B=_tmp70A->f2;}}_LL47F:
# 2792
{void*_tmp72B=t2;struct Cyc_Absyn_Enumdecl*_tmp72D;_LL498: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp72C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp72B;if(_tmp72C->tag != 13)goto _LL49A;else{_tmp72D=_tmp72C->f2;}}_LL499:
# 2794
 if((_tmp70B->fields != 0  && _tmp72D->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp70B->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp72D->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL497;_LL49A:;_LL49B:
 goto _LL497;_LL497:;}
# 2800
goto _LL481;_LL480: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6FD;if(_tmp70C->tag != 6)goto _LL482;}_LL481:
 goto _LL483;_LL482: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp70D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6FD;if(_tmp70D->tag != 7)goto _LL484;}_LL483:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL484: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp70E=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6FD;if(_tmp70E->tag != 15)goto _LL486;else{_tmp70F=(void*)_tmp70E->f1;}}_LL485:
# 2805
{void*_tmp72E=t2;void*_tmp730;_LL49D: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp72F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp72E;if(_tmp72F->tag != 15)goto _LL49F;else{_tmp730=(void*)_tmp72F->f1;}}_LL49E:
# 2807
 if(Cyc_Tcenv_region_outlives(te,_tmp70F,_tmp730))return Cyc_Absyn_No_coercion;
goto _LL49C;_LL49F:;_LL4A0:
 goto _LL49C;_LL49C:;}
# 2811
return Cyc_Absyn_Unknown_coercion;_LL486:;_LL487:
 return Cyc_Absyn_Unknown_coercion;_LL479:;};}
# 2817
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp731=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1025;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1024;e->r=(void*)((_tmp1024=_cycalloc(sizeof(*_tmp1024)),((_tmp1024[0]=((_tmp1025.tag=13,((_tmp1025.f1=t,((_tmp1025.f2=_tmp731,((_tmp1025.f3=0,((_tmp1025.f4=c,_tmp1025)))))))))),_tmp1024))));}
e->topt=t;}}
# 2825
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp734=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4A2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp735=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp734;if(_tmp735->tag != 6)goto _LL4A4;}_LL4A3:
 goto _LL4A5;_LL4A4: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp736=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp734;if(_tmp736->tag != 13)goto _LL4A6;}_LL4A5:
 goto _LL4A7;_LL4A6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp737=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp734;if(_tmp737->tag != 14)goto _LL4A8;}_LL4A7:
 goto _LL4A9;_LL4A8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp738=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp734;if(_tmp738->tag != 19)goto _LL4AA;}_LL4A9:
 return 1;_LL4AA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp739=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp734;if(_tmp739->tag != 1)goto _LL4AC;}_LL4AB:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4AC:;_LL4AD:
 return 0;_LL4A1:;}
# 2836
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp73A=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4AF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp73B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp73A;if(_tmp73B->tag != 7)goto _LL4B1;}_LL4B0:
 return 1;_LL4B1:;_LL4B2:
 return 0;_LL4AE:;};}
# 2845
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp73C=Cyc_Tcutil_compress(t);_LL4B4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp73D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp73C;if(_tmp73D->tag != 9)goto _LL4B6;}_LL4B5:
 return 1;_LL4B6:;_LL4B7:
 return 0;_LL4B3:;}
# 2852
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1026;struct _tuple0 _tmp73F=(_tmp1026.f1=t1,((_tmp1026.f2=t2,_tmp1026)));int _tmp741;int _tmp743;_LL4B9:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp740=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp73F.f1;if(_tmp740->tag != 7)goto _LL4BB;else{_tmp741=_tmp740->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp742=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp73F.f2;if(_tmp742->tag != 7)goto _LL4BB;else{_tmp743=_tmp742->f1;}};_LL4BA:
# 2855
 if(_tmp741 != 0  && _tmp741 != 1)return t1;else{
if(_tmp743 != 0  && _tmp743 != 1)return t2;else{
if(_tmp741 >= _tmp743)return t1;else{
return t2;}}}_LL4BB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp744=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp73F.f1;if(_tmp744->tag != 7)goto _LL4BD;}_LL4BC:
 return t1;_LL4BD: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp745=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp73F.f2;if(_tmp745->tag != 7)goto _LL4BF;}_LL4BE:
 return t2;_LL4BF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp746=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F.f1;if(_tmp746->tag != 6)goto _LL4C1;else{if(_tmp746->f1 != Cyc_Absyn_Unsigned)goto _LL4C1;if(_tmp746->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C1;}}_LL4C0:
 goto _LL4C2;_LL4C1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp747=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F.f2;if(_tmp747->tag != 6)goto _LL4C3;else{if(_tmp747->f1 != Cyc_Absyn_Unsigned)goto _LL4C3;if(_tmp747->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C3;}}_LL4C2:
 return Cyc_Absyn_ulonglong_typ;_LL4C3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp748=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F.f1;if(_tmp748->tag != 6)goto _LL4C5;else{if(_tmp748->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C5;}}_LL4C4:
 goto _LL4C6;_LL4C5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F.f2;if(_tmp749->tag != 6)goto _LL4C7;else{if(_tmp749->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C7;}}_LL4C6:
 return Cyc_Absyn_slonglong_typ;_LL4C7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp74A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F.f1;if(_tmp74A->tag != 6)goto _LL4C9;else{if(_tmp74A->f1 != Cyc_Absyn_Unsigned)goto _LL4C9;if(_tmp74A->f2 != Cyc_Absyn_Long_sz)goto _LL4C9;}}_LL4C8:
 goto _LL4CA;_LL4C9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp74B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F.f2;if(_tmp74B->tag != 6)goto _LL4CB;else{if(_tmp74B->f1 != Cyc_Absyn_Unsigned)goto _LL4CB;if(_tmp74B->f2 != Cyc_Absyn_Long_sz)goto _LL4CB;}}_LL4CA:
 return Cyc_Absyn_ulong_typ;_LL4CB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp74C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F.f1;if(_tmp74C->tag != 6)goto _LL4CD;else{if(_tmp74C->f1 != Cyc_Absyn_Unsigned)goto _LL4CD;if(_tmp74C->f2 != Cyc_Absyn_Int_sz)goto _LL4CD;}}_LL4CC:
# 2868
 goto _LL4CE;_LL4CD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp74D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F.f2;if(_tmp74D->tag != 6)goto _LL4CF;else{if(_tmp74D->f1 != Cyc_Absyn_Unsigned)goto _LL4CF;if(_tmp74D->f2 != Cyc_Absyn_Int_sz)goto _LL4CF;}}_LL4CE:
 return Cyc_Absyn_uint_typ;_LL4CF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp74E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F.f1;if(_tmp74E->tag != 6)goto _LL4D1;else{if(_tmp74E->f2 != Cyc_Absyn_Long_sz)goto _LL4D1;}}_LL4D0:
 goto _LL4D2;_LL4D1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp74F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F.f2;if(_tmp74F->tag != 6)goto _LL4D3;else{if(_tmp74F->f2 != Cyc_Absyn_Long_sz)goto _LL4D3;}}_LL4D2:
 return Cyc_Absyn_slong_typ;_LL4D3:;_LL4D4:
 return Cyc_Absyn_sint_typ;_LL4B8:;}
# 2877
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp750=e->r;_LL4D6: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp751=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp750;if(_tmp751->tag != 3)goto _LL4D8;else{if(_tmp751->f2 != 0)goto _LL4D8;}}_LL4D7:
{const char*_tmp1029;void*_tmp1028;(_tmp1028=0,Cyc_Tcutil_warn(e->loc,((_tmp1029="assignment in test",_tag_dyneither(_tmp1029,sizeof(char),19))),_tag_dyneither(_tmp1028,sizeof(void*),0)));}goto _LL4D5;_LL4D8:;_LL4D9:
 goto _LL4D5;_LL4D5:;}
# 2891 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp102A;struct _tuple0 _tmp755=(_tmp102A.f1=c1,((_tmp102A.f2=c2,_tmp102A)));struct Cyc_Absyn_Kind*_tmp757;struct Cyc_Absyn_Kind*_tmp759;struct Cyc_Core_Opt**_tmp75B;struct Cyc_Core_Opt**_tmp75D;struct Cyc_Core_Opt**_tmp75F;struct Cyc_Absyn_Kind*_tmp760;struct Cyc_Absyn_Kind*_tmp762;struct Cyc_Absyn_Kind*_tmp764;struct Cyc_Core_Opt**_tmp766;struct Cyc_Absyn_Kind*_tmp767;struct Cyc_Core_Opt**_tmp769;struct Cyc_Absyn_Kind*_tmp76A;struct Cyc_Core_Opt**_tmp76C;struct Cyc_Absyn_Kind*_tmp76D;_LL4DB:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp756=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp755.f1;if(_tmp756->tag != 0)goto _LL4DD;else{_tmp757=_tmp756->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp758=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp755.f2;if(_tmp758->tag != 0)goto _LL4DD;else{_tmp759=_tmp758->f1;}};_LL4DC:
 return _tmp757 == _tmp759;_LL4DD: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp75A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp755.f2;if(_tmp75A->tag != 1)goto _LL4DF;else{_tmp75B=(struct Cyc_Core_Opt**)& _tmp75A->f1;}}_LL4DE:
{struct Cyc_Core_Opt*_tmp102B;*_tmp75B=((_tmp102B=_cycalloc(sizeof(*_tmp102B)),((_tmp102B->v=c1,_tmp102B))));}return 1;_LL4DF: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp75C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp755.f1;if(_tmp75C->tag != 1)goto _LL4E1;else{_tmp75D=(struct Cyc_Core_Opt**)& _tmp75C->f1;}}_LL4E0:
{struct Cyc_Core_Opt*_tmp102C;*_tmp75D=((_tmp102C=_cycalloc(sizeof(*_tmp102C)),((_tmp102C->v=c2,_tmp102C))));}return 1;_LL4E1:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp75E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp755.f1;if(_tmp75E->tag != 2)goto _LL4E3;else{_tmp75F=(struct Cyc_Core_Opt**)& _tmp75E->f1;_tmp760=_tmp75E->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp761=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp755.f2;if(_tmp761->tag != 0)goto _LL4E3;else{_tmp762=_tmp761->f1;}};_LL4E2:
# 2899
 if(Cyc_Tcutil_kind_leq(_tmp762,_tmp760)){
{struct Cyc_Core_Opt*_tmp102D;*_tmp75F=((_tmp102D=_cycalloc(sizeof(*_tmp102D)),((_tmp102D->v=c2,_tmp102D))));}return 1;}else{
return 0;}_LL4E3:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp763=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp755.f1;if(_tmp763->tag != 0)goto _LL4E5;else{_tmp764=_tmp763->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp765=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp755.f2;if(_tmp765->tag != 2)goto _LL4E5;else{_tmp766=(struct Cyc_Core_Opt**)& _tmp765->f1;_tmp767=_tmp765->f2;}};_LL4E4:
# 2903
 if(Cyc_Tcutil_kind_leq(_tmp764,_tmp767)){
{struct Cyc_Core_Opt*_tmp102E;*_tmp766=((_tmp102E=_cycalloc(sizeof(*_tmp102E)),((_tmp102E->v=c1,_tmp102E))));}return 1;}else{
return 0;}_LL4E5:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp768=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp755.f1;if(_tmp768->tag != 2)goto _LL4DA;else{_tmp769=(struct Cyc_Core_Opt**)& _tmp768->f1;_tmp76A=_tmp768->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp76B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp755.f2;if(_tmp76B->tag != 2)goto _LL4DA;else{_tmp76C=(struct Cyc_Core_Opt**)& _tmp76B->f1;_tmp76D=_tmp76B->f2;}};_LL4E6:
# 2907
 if(Cyc_Tcutil_kind_leq(_tmp76A,_tmp76D)){
{struct Cyc_Core_Opt*_tmp102F;*_tmp76C=((_tmp102F=_cycalloc(sizeof(*_tmp102F)),((_tmp102F->v=c1,_tmp102F))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp76D,_tmp76A)){
{struct Cyc_Core_Opt*_tmp1030;*_tmp769=((_tmp1030=_cycalloc(sizeof(*_tmp1030)),((_tmp1030->v=c2,_tmp1030))));}return 1;}else{
return 0;}}_LL4DA:;};}
# 2916
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2921
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1034;void*_tmp1033[1];struct Cyc_Int_pa_PrintArg_struct _tmp1032;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1032.tag=1,((_tmp1032.f1=(unsigned long)i,((_tmp1033[0]=& _tmp1032,Cyc_aprintf(((_tmp1034="#%d",_tag_dyneither(_tmp1034,sizeof(char),4))),_tag_dyneither(_tmp1033,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1037;struct Cyc_Absyn_Tvar*_tmp1036;return(_tmp1036=_cycalloc(sizeof(*_tmp1036)),((_tmp1036->name=((_tmp1037=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1037[0]=s,_tmp1037)))),((_tmp1036->identity=- 1,((_tmp1036->kind=k,_tmp1036)))))));}
# 2928
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp779=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp779,sizeof(char),0))== '#';}
# 2933
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp103B;void*_tmp103A[1];struct Cyc_String_pa_PrintArg_struct _tmp1039;(_tmp1039.tag=0,((_tmp1039.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp103A[0]=& _tmp1039,Cyc_printf(((_tmp103B="%s",_tag_dyneither(_tmp103B,sizeof(char),3))),_tag_dyneither(_tmp103A,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp103C;struct _dyneither_ptr _tmp77D=Cyc_strconcat(((_tmp103C="`",_tag_dyneither(_tmp103C,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp103F;char _tmp103E;struct _dyneither_ptr _tmp103D;(_tmp103D=_dyneither_ptr_plus(_tmp77D,sizeof(char),1),((_tmp103E=*((char*)_check_dyneither_subscript(_tmp103D,sizeof(char),0)),((_tmp103F='t',((_get_dyneither_size(_tmp103D,sizeof(char))== 1  && (_tmp103E == '\000'  && _tmp103F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp103D.curr)=_tmp103F)))))));}{
struct _dyneither_ptr*_tmp1040;t->name=((_tmp1040=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1040[0]=(struct _dyneither_ptr)_tmp77D,_tmp1040))));};};}
# 2942
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 2944
struct _tuple10*_tmp1041;return(_tmp1041=_cycalloc(sizeof(*_tmp1041)),((_tmp1041->f1=(*x).f1,((_tmp1041->f2=(*x).f2,((_tmp1041->f3=(*x).f3,_tmp1041)))))));}
# 2947
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 2954
struct Cyc_List_List*_tmp784=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1042;_tmp784=((_tmp1042=_cycalloc(sizeof(*_tmp1042)),((_tmp1042->hd=(void*)atts->hd,((_tmp1042->tl=_tmp784,_tmp1042))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1048;struct Cyc_Absyn_FnInfo _tmp1047;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1046;return(void*)((_tmp1046=_cycalloc(sizeof(*_tmp1046)),((_tmp1046[0]=((_tmp1048.tag=9,((_tmp1048.f1=((_tmp1047.tvars=fd->tvs,((_tmp1047.effect=fd->effect,((_tmp1047.ret_tqual=fd->ret_tqual,((_tmp1047.ret_typ=fd->ret_type,((_tmp1047.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1047.c_varargs=fd->c_varargs,((_tmp1047.cyc_varargs=fd->cyc_varargs,((_tmp1047.rgn_po=fd->rgn_po,((_tmp1047.attributes=_tmp784,_tmp1047)))))))))))))))))),_tmp1048)))),_tmp1046))));};}
# 2963
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 2967
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp78A;void*_tmp78B;struct _tuple12*_tmp789=pr;_tmp78A=_tmp789->f1;_tmp78B=_tmp789->f2;
if(_tmp78B == t)return pr;else{
struct _tuple12*_tmp1049;return(_tmp1049=_cycalloc(sizeof(*_tmp1049)),((_tmp1049->f1=_tmp78A,((_tmp1049->f2=t,_tmp1049)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2974
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 2976
struct _tuple26*_tmp104C;struct _tuple27*_tmp104B;return(_tmp104B=_region_malloc(rgn,sizeof(*_tmp104B)),((_tmp104B->f1=((_tmp104C=_region_malloc(rgn,sizeof(*_tmp104C)),((_tmp104C->f1=(*y).f1,((_tmp104C->f2=(*y).f2,_tmp104C)))))),((_tmp104B->f2=(*y).f3,_tmp104B)))));}
# 2978
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 2980
struct _dyneither_ptr*_tmp790;struct Cyc_Absyn_Tqual _tmp791;void*_tmp792;struct _tuple10 _tmp78F=*orig_arg;_tmp790=_tmp78F.f1;_tmp791=_tmp78F.f2;_tmp792=_tmp78F.f3;
if(t == _tmp792)return orig_arg;{
struct _tuple10*_tmp104D;return(_tmp104D=_cycalloc(sizeof(*_tmp104D)),((_tmp104D->f1=_tmp790,((_tmp104D->f2=_tmp791,((_tmp104D->f3=t,_tmp104D)))))));};}
# 2984
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 2987
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){
if(f->type == t)return f;{
struct Cyc_Absyn_Aggrfield*_tmp104E;return(_tmp104E=_cycalloc(sizeof(*_tmp104E)),((_tmp104E->name=f->name,((_tmp104E->tq=f->tq,((_tmp104E->type=t,((_tmp104E->width=f->width,((_tmp104E->attributes=f->attributes,_tmp104E)))))))))));};}
# 2993
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2998
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3000
struct Cyc_Absyn_Exp*_tmp104F;return(_tmp104F=_cycalloc(sizeof(*_tmp104F)),((_tmp104F->topt=old->topt,((_tmp104F->r=r,((_tmp104F->loc=old->loc,((_tmp104F->annot=old->annot,_tmp104F)))))))));}
# 3005
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3007
void*_tmp796=e->r;enum Cyc_Absyn_Primop _tmp79C;struct Cyc_List_List*_tmp79D;struct Cyc_Absyn_Exp*_tmp79F;struct Cyc_Absyn_Exp*_tmp7A0;struct Cyc_Absyn_Exp*_tmp7A1;struct Cyc_Absyn_Exp*_tmp7A3;struct Cyc_Absyn_Exp*_tmp7A4;struct Cyc_Absyn_Exp*_tmp7A6;struct Cyc_Absyn_Exp*_tmp7A7;struct Cyc_Absyn_Exp*_tmp7A9;struct Cyc_Absyn_Exp*_tmp7AA;void*_tmp7AC;struct Cyc_Absyn_Exp*_tmp7AD;int _tmp7AE;enum Cyc_Absyn_Coercion _tmp7AF;void*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B3;void*_tmp7B5;void*_tmp7B6;void*_tmp7B8;_LL4E8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp797=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp797->tag != 0)goto _LL4EA;}_LL4E9:
 goto _LL4EB;_LL4EA: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp798=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp798->tag != 30)goto _LL4EC;}_LL4EB:
 goto _LL4ED;_LL4EC: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp799=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp799->tag != 31)goto _LL4EE;}_LL4ED:
 goto _LL4EF;_LL4EE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp79A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp79A->tag != 1)goto _LL4F0;}_LL4EF:
 return e;_LL4F0: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp79B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp79B->tag != 2)goto _LL4F2;else{_tmp79C=_tmp79B->f1;_tmp79D=_tmp79B->f2;}}_LL4F1:
# 3014
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp79D)== 1){
struct Cyc_Absyn_Exp*_tmp7B9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp79D))->hd;
struct Cyc_Absyn_Exp*_tmp7BA=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B9);
if(_tmp7BA == _tmp7B9)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1055;struct Cyc_Absyn_Exp*_tmp1054[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1053;return Cyc_Tcutil_copye(e,(void*)((_tmp1053=_cycalloc(sizeof(*_tmp1053)),((_tmp1053[0]=((_tmp1055.tag=2,((_tmp1055.f1=_tmp79C,((_tmp1055.f2=((_tmp1054[0]=_tmp7BA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1054,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1055)))))),_tmp1053)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp79D)== 2){
struct Cyc_Absyn_Exp*_tmp7BE=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp79D))->hd;
struct Cyc_Absyn_Exp*_tmp7BF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp79D->tl))->hd;
struct Cyc_Absyn_Exp*_tmp7C0=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BE);
struct Cyc_Absyn_Exp*_tmp7C1=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BF);
if(_tmp7C0 == _tmp7BE  && _tmp7C1 == _tmp7BF)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp105B;struct Cyc_Absyn_Exp*_tmp105A[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1059;return Cyc_Tcutil_copye(e,(void*)((_tmp1059=_cycalloc(sizeof(*_tmp1059)),((_tmp1059[0]=((_tmp105B.tag=2,((_tmp105B.f1=_tmp79C,((_tmp105B.f2=((_tmp105A[1]=_tmp7C1,((_tmp105A[0]=_tmp7C0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp105A,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp105B)))))),_tmp1059)))));};}else{
const char*_tmp105E;void*_tmp105D;return(_tmp105D=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp105E="primop does not have 1 or 2 args!",_tag_dyneither(_tmp105E,sizeof(char),34))),_tag_dyneither(_tmp105D,sizeof(void*),0)));}}_LL4F2: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp79E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp79E->tag != 5)goto _LL4F4;else{_tmp79F=_tmp79E->f1;_tmp7A0=_tmp79E->f2;_tmp7A1=_tmp79E->f3;}}_LL4F3: {
# 3028
struct Cyc_Absyn_Exp*_tmp7C7=Cyc_Tcutil_rsubsexp(r,inst,_tmp79F);
struct Cyc_Absyn_Exp*_tmp7C8=Cyc_Tcutil_rsubsexp(r,inst,_tmp7A0);
struct Cyc_Absyn_Exp*_tmp7C9=Cyc_Tcutil_rsubsexp(r,inst,_tmp7A1);
if((_tmp7C7 == _tmp79F  && _tmp7C8 == _tmp7A0) && _tmp7C9 == _tmp7A1)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp1061;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1060;return Cyc_Tcutil_copye(e,(void*)((_tmp1060=_cycalloc(sizeof(*_tmp1060)),((_tmp1060[0]=((_tmp1061.tag=5,((_tmp1061.f1=_tmp7C7,((_tmp1061.f2=_tmp7C8,((_tmp1061.f3=_tmp7C9,_tmp1061)))))))),_tmp1060)))));};}_LL4F4: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7A2=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp7A2->tag != 6)goto _LL4F6;else{_tmp7A3=_tmp7A2->f1;_tmp7A4=_tmp7A2->f2;}}_LL4F5: {
# 3034
struct Cyc_Absyn_Exp*_tmp7CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7A3);
struct Cyc_Absyn_Exp*_tmp7CD=Cyc_Tcutil_rsubsexp(r,inst,_tmp7A4);
if(_tmp7CC == _tmp7A3  && _tmp7CD == _tmp7A4)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1064;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1063;return Cyc_Tcutil_copye(e,(void*)((_tmp1063=_cycalloc(sizeof(*_tmp1063)),((_tmp1063[0]=((_tmp1064.tag=6,((_tmp1064.f1=_tmp7CC,((_tmp1064.f2=_tmp7CD,_tmp1064)))))),_tmp1063)))));};}_LL4F6: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7A5=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp7A5->tag != 7)goto _LL4F8;else{_tmp7A6=_tmp7A5->f1;_tmp7A7=_tmp7A5->f2;}}_LL4F7: {
# 3039
struct Cyc_Absyn_Exp*_tmp7D0=Cyc_Tcutil_rsubsexp(r,inst,_tmp7A6);
struct Cyc_Absyn_Exp*_tmp7D1=Cyc_Tcutil_rsubsexp(r,inst,_tmp7A7);
if(_tmp7D0 == _tmp7A6  && _tmp7D1 == _tmp7A7)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1067;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1066;return Cyc_Tcutil_copye(e,(void*)((_tmp1066=_cycalloc(sizeof(*_tmp1066)),((_tmp1066[0]=((_tmp1067.tag=7,((_tmp1067.f1=_tmp7D0,((_tmp1067.f2=_tmp7D1,_tmp1067)))))),_tmp1066)))));};}_LL4F8: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7A8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp7A8->tag != 8)goto _LL4FA;else{_tmp7A9=_tmp7A8->f1;_tmp7AA=_tmp7A8->f2;}}_LL4F9: {
# 3044
struct Cyc_Absyn_Exp*_tmp7D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp7A9);
struct Cyc_Absyn_Exp*_tmp7D5=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AA);
if(_tmp7D4 == _tmp7A9  && _tmp7D5 == _tmp7AA)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp106A;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1069;return Cyc_Tcutil_copye(e,(void*)((_tmp1069=_cycalloc(sizeof(*_tmp1069)),((_tmp1069[0]=((_tmp106A.tag=8,((_tmp106A.f1=_tmp7D4,((_tmp106A.f2=_tmp7D5,_tmp106A)))))),_tmp1069)))));};}_LL4FA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7AB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp7AB->tag != 13)goto _LL4FC;else{_tmp7AC=(void*)_tmp7AB->f1;_tmp7AD=_tmp7AB->f2;_tmp7AE=_tmp7AB->f3;_tmp7AF=_tmp7AB->f4;}}_LL4FB: {
# 3049
struct Cyc_Absyn_Exp*_tmp7D8=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AD);
void*_tmp7D9=Cyc_Tcutil_rsubstitute(r,inst,_tmp7AC);
if(_tmp7D8 == _tmp7AD  && _tmp7D9 == _tmp7AC)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp106D;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp106C;return Cyc_Tcutil_copye(e,(void*)((_tmp106C=_cycalloc(sizeof(*_tmp106C)),((_tmp106C[0]=((_tmp106D.tag=13,((_tmp106D.f1=_tmp7D9,((_tmp106D.f2=_tmp7D8,((_tmp106D.f3=_tmp7AE,((_tmp106D.f4=_tmp7AF,_tmp106D)))))))))),_tmp106C)))));};}_LL4FC: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp7B0=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp7B0->tag != 16)goto _LL4FE;else{_tmp7B1=(void*)_tmp7B0->f1;}}_LL4FD: {
# 3054
void*_tmp7DC=Cyc_Tcutil_rsubstitute(r,inst,_tmp7B1);
if(_tmp7DC == _tmp7B1)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp1070;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp106F;return Cyc_Tcutil_copye(e,(void*)((_tmp106F=_cycalloc(sizeof(*_tmp106F)),((_tmp106F[0]=((_tmp1070.tag=16,((_tmp1070.f1=_tmp7DC,_tmp1070)))),_tmp106F)))));};}_LL4FE: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7B2=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp7B2->tag != 17)goto _LL500;else{_tmp7B3=_tmp7B2->f1;}}_LL4FF: {
# 3058
struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B3);
if(_tmp7DF == _tmp7B3)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp1073;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1072;return Cyc_Tcutil_copye(e,(void*)((_tmp1072=_cycalloc(sizeof(*_tmp1072)),((_tmp1072[0]=((_tmp1073.tag=17,((_tmp1073.f1=_tmp7DF,_tmp1073)))),_tmp1072)))));};}_LL500: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7B4=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp7B4->tag != 18)goto _LL502;else{_tmp7B5=(void*)_tmp7B4->f1;_tmp7B6=(void*)_tmp7B4->f2;}}_LL501: {
# 3062
void*_tmp7E2=Cyc_Tcutil_rsubstitute(r,inst,_tmp7B5);
if(_tmp7E2 == _tmp7B5)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1076;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1075;return Cyc_Tcutil_copye(e,(void*)((_tmp1075=_cycalloc(sizeof(*_tmp1075)),((_tmp1075[0]=((_tmp1076.tag=18,((_tmp1076.f1=_tmp7E2,((_tmp1076.f2=_tmp7B6,_tmp1076)))))),_tmp1075)))));};}_LL502: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp7B7=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp796;if(_tmp7B7->tag != 37)goto _LL504;else{_tmp7B8=(void*)_tmp7B7->f1;}}_LL503: {
# 3066
void*_tmp7E5=Cyc_Tcutil_rsubstitute(r,inst,_tmp7B8);
if(_tmp7E5 == _tmp7B8)return e;{
# 3069
void*_tmp7E6=Cyc_Tcutil_compress(_tmp7E5);struct Cyc_Absyn_Exp*_tmp7E8;_LL507: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7E7=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7E6;if(_tmp7E7->tag != 18)goto _LL509;else{_tmp7E8=_tmp7E7->f1;}}_LL508:
 return _tmp7E8;_LL509:;_LL50A: {
# 3072
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1079;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1078;return Cyc_Tcutil_copye(e,(void*)((_tmp1078=_cycalloc(sizeof(*_tmp1078)),((_tmp1078[0]=((_tmp1079.tag=37,((_tmp1079.f1=_tmp7E5,_tmp1079)))),_tmp1078)))));}_LL506:;};}_LL504:;_LL505: {
# 3075
const char*_tmp107C;void*_tmp107B;return(_tmp107B=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp107C="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp107C,sizeof(char),46))),_tag_dyneither(_tmp107B,sizeof(void*),0)));}_LL4E7:;}
# 3079
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3082
void*_tmp7ED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp7EF;union Cyc_Absyn_AggrInfoU _tmp7F1;struct Cyc_List_List*_tmp7F2;union Cyc_Absyn_DatatypeInfoU _tmp7F4;struct Cyc_List_List*_tmp7F5;union Cyc_Absyn_DatatypeFieldInfoU _tmp7F7;struct Cyc_List_List*_tmp7F8;struct _tuple2*_tmp7FA;struct Cyc_List_List*_tmp7FB;struct Cyc_Absyn_Typedefdecl*_tmp7FC;void*_tmp7FD;void*_tmp7FF;struct Cyc_Absyn_Tqual _tmp800;struct Cyc_Absyn_Exp*_tmp801;union Cyc_Absyn_Constraint*_tmp802;unsigned int _tmp803;void*_tmp805;struct Cyc_Absyn_Tqual _tmp806;void*_tmp807;union Cyc_Absyn_Constraint*_tmp808;union Cyc_Absyn_Constraint*_tmp809;union Cyc_Absyn_Constraint*_tmp80A;struct Cyc_List_List*_tmp80C;void*_tmp80D;struct Cyc_Absyn_Tqual _tmp80E;void*_tmp80F;struct Cyc_List_List*_tmp810;int _tmp811;struct Cyc_Absyn_VarargInfo*_tmp812;struct Cyc_List_List*_tmp813;struct Cyc_List_List*_tmp814;struct Cyc_List_List*_tmp816;enum Cyc_Absyn_AggrKind _tmp818;struct Cyc_List_List*_tmp819;void*_tmp81B;void*_tmp81D;void*_tmp81F;void*_tmp820;void*_tmp822;struct Cyc_Absyn_Exp*_tmp824;void*_tmp82E;void*_tmp830;struct Cyc_List_List*_tmp832;_LL50C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7EE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7ED;if(_tmp7EE->tag != 2)goto _LL50E;else{_tmp7EF=_tmp7EE->f1;}}_LL50D: {
# 3085
struct _handler_cons _tmp834;_push_handler(& _tmp834);{int _tmp836=0;if(setjmp(_tmp834.handler))_tmp836=1;if(!_tmp836){{void*_tmp837=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp7EF);_npop_handler(0);return _tmp837;};_pop_handler();}else{void*_tmp835=(void*)_exn_thrown;void*_tmp839=_tmp835;void*_tmp83B;_LL543: {struct Cyc_Core_Not_found_exn_struct*_tmp83A=(struct Cyc_Core_Not_found_exn_struct*)_tmp839;if(_tmp83A->tag != Cyc_Core_Not_found)goto _LL545;}_LL544:
 return t;_LL545: _tmp83B=_tmp839;_LL546:(void)_throw(_tmp83B);_LL542:;}};}_LL50E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp7F0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp7ED;if(_tmp7F0->tag != 11)goto _LL510;else{_tmp7F1=(_tmp7F0->f1).aggr_info;_tmp7F2=(_tmp7F0->f1).targs;}}_LL50F: {
# 3088
struct Cyc_List_List*_tmp83C=Cyc_Tcutil_substs(rgn,inst,_tmp7F2);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1082;struct Cyc_Absyn_AggrInfo _tmp1081;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1080;return _tmp83C == _tmp7F2?t:(void*)((_tmp1080=_cycalloc(sizeof(*_tmp1080)),((_tmp1080[0]=((_tmp1082.tag=11,((_tmp1082.f1=((_tmp1081.aggr_info=_tmp7F1,((_tmp1081.targs=_tmp83C,_tmp1081)))),_tmp1082)))),_tmp1080))));}_LL510: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp7F3=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp7ED;if(_tmp7F3->tag != 3)goto _LL512;else{_tmp7F4=(_tmp7F3->f1).datatype_info;_tmp7F5=(_tmp7F3->f1).targs;}}_LL511: {
# 3091
struct Cyc_List_List*_tmp840=Cyc_Tcutil_substs(rgn,inst,_tmp7F5);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp1088;struct Cyc_Absyn_DatatypeInfo _tmp1087;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1086;return _tmp840 == _tmp7F5?t:(void*)(
(_tmp1086=_cycalloc(sizeof(*_tmp1086)),((_tmp1086[0]=((_tmp1088.tag=3,((_tmp1088.f1=((_tmp1087.datatype_info=_tmp7F4,((_tmp1087.targs=_tmp840,_tmp1087)))),_tmp1088)))),_tmp1086))));}_LL512: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp7F6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7ED;if(_tmp7F6->tag != 4)goto _LL514;else{_tmp7F7=(_tmp7F6->f1).field_info;_tmp7F8=(_tmp7F6->f1).targs;}}_LL513: {
# 3095
struct Cyc_List_List*_tmp844=Cyc_Tcutil_substs(rgn,inst,_tmp7F8);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp108E;struct Cyc_Absyn_DatatypeFieldInfo _tmp108D;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp108C;return _tmp844 == _tmp7F8?t:(void*)(
(_tmp108C=_cycalloc(sizeof(*_tmp108C)),((_tmp108C[0]=((_tmp108E.tag=4,((_tmp108E.f1=((_tmp108D.field_info=_tmp7F7,((_tmp108D.targs=_tmp844,_tmp108D)))),_tmp108E)))),_tmp108C))));}_LL514: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp7F9=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp7ED;if(_tmp7F9->tag != 17)goto _LL516;else{_tmp7FA=_tmp7F9->f1;_tmp7FB=_tmp7F9->f2;_tmp7FC=_tmp7F9->f3;_tmp7FD=(void*)_tmp7F9->f4;}}_LL515: {
# 3099
struct Cyc_List_List*_tmp848=Cyc_Tcutil_substs(rgn,inst,_tmp7FB);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp1091;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1090;return _tmp848 == _tmp7FB?t:(void*)((_tmp1090=_cycalloc(sizeof(*_tmp1090)),((_tmp1090[0]=((_tmp1091.tag=17,((_tmp1091.f1=_tmp7FA,((_tmp1091.f2=_tmp848,((_tmp1091.f3=_tmp7FC,((_tmp1091.f4=_tmp7FD,_tmp1091)))))))))),_tmp1090))));}_LL516: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7FE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7ED;if(_tmp7FE->tag != 8)goto _LL518;else{_tmp7FF=(_tmp7FE->f1).elt_type;_tmp800=(_tmp7FE->f1).tq;_tmp801=(_tmp7FE->f1).num_elts;_tmp802=(_tmp7FE->f1).zero_term;_tmp803=(_tmp7FE->f1).zt_loc;}}_LL517: {
# 3102
void*_tmp84B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp7FF);
struct Cyc_Absyn_Exp*_tmp84C=_tmp801 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp801);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp1097;struct Cyc_Absyn_ArrayInfo _tmp1096;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1095;return _tmp84B == _tmp7FF  && _tmp84C == _tmp801?t:(void*)(
(_tmp1095=_cycalloc(sizeof(*_tmp1095)),((_tmp1095[0]=((_tmp1097.tag=8,((_tmp1097.f1=((_tmp1096.elt_type=_tmp84B,((_tmp1096.tq=_tmp800,((_tmp1096.num_elts=_tmp84C,((_tmp1096.zero_term=_tmp802,((_tmp1096.zt_loc=_tmp803,_tmp1096)))))))))),_tmp1097)))),_tmp1095))));}_LL518: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp804=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7ED;if(_tmp804->tag != 5)goto _LL51A;else{_tmp805=(_tmp804->f1).elt_typ;_tmp806=(_tmp804->f1).elt_tq;_tmp807=((_tmp804->f1).ptr_atts).rgn;_tmp808=((_tmp804->f1).ptr_atts).nullable;_tmp809=((_tmp804->f1).ptr_atts).bounds;_tmp80A=((_tmp804->f1).ptr_atts).zero_term;}}_LL519: {
# 3107
void*_tmp850=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp805);
void*_tmp851=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp807);
union Cyc_Absyn_Constraint*_tmp852=_tmp809;
{void*_tmp853=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp809);struct Cyc_Absyn_Exp*_tmp855;_LL548: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp854=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp853;if(_tmp854->tag != 1)goto _LL54A;else{_tmp855=_tmp854->f1;}}_LL549: {
# 3112
struct Cyc_Absyn_Exp*_tmp856=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp855);
if(_tmp856 != _tmp855){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp109A;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1099;_tmp852=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp1099=_cycalloc(sizeof(*_tmp1099)),((_tmp1099[0]=((_tmp109A.tag=1,((_tmp109A.f1=_tmp856,_tmp109A)))),_tmp1099)))));}
goto _LL547;}_LL54A:;_LL54B:
 goto _LL547;_LL547:;}
# 3118
if((_tmp850 == _tmp805  && _tmp851 == _tmp807) && _tmp852 == _tmp809)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10A4;struct Cyc_Absyn_PtrAtts _tmp10A3;struct Cyc_Absyn_PtrInfo _tmp10A2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10A1;return(void*)((_tmp10A1=_cycalloc(sizeof(*_tmp10A1)),((_tmp10A1[0]=((_tmp10A4.tag=5,((_tmp10A4.f1=((_tmp10A2.elt_typ=_tmp850,((_tmp10A2.elt_tq=_tmp806,((_tmp10A2.ptr_atts=((_tmp10A3.rgn=_tmp851,((_tmp10A3.nullable=_tmp808,((_tmp10A3.bounds=_tmp852,((_tmp10A3.zero_term=_tmp80A,((_tmp10A3.ptrloc=0,_tmp10A3)))))))))),_tmp10A2)))))),_tmp10A4)))),_tmp10A1))));};}_LL51A: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp80B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp7ED;if(_tmp80B->tag != 9)goto _LL51C;else{_tmp80C=(_tmp80B->f1).tvars;_tmp80D=(_tmp80B->f1).effect;_tmp80E=(_tmp80B->f1).ret_tqual;_tmp80F=(_tmp80B->f1).ret_typ;_tmp810=(_tmp80B->f1).args;_tmp811=(_tmp80B->f1).c_varargs;_tmp812=(_tmp80B->f1).cyc_varargs;_tmp813=(_tmp80B->f1).rgn_po;_tmp814=(_tmp80B->f1).attributes;}}_LL51B:
# 3123
{struct Cyc_List_List*_tmp85D=_tmp80C;for(0;_tmp85D != 0;_tmp85D=_tmp85D->tl){
struct _tuple16*_tmp10AE;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10AD;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10AC;struct Cyc_List_List*_tmp10AB;inst=((_tmp10AB=_region_malloc(rgn,sizeof(*_tmp10AB)),((_tmp10AB->hd=((_tmp10AE=_region_malloc(rgn,sizeof(*_tmp10AE)),((_tmp10AE->f1=(struct Cyc_Absyn_Tvar*)_tmp85D->hd,((_tmp10AE->f2=(void*)((_tmp10AC=_cycalloc(sizeof(*_tmp10AC)),((_tmp10AC[0]=((_tmp10AD.tag=2,((_tmp10AD.f1=(struct Cyc_Absyn_Tvar*)_tmp85D->hd,_tmp10AD)))),_tmp10AC)))),_tmp10AE)))))),((_tmp10AB->tl=inst,_tmp10AB))))));}}{
struct Cyc_List_List*_tmp863;struct Cyc_List_List*_tmp864;struct _tuple1 _tmp862=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp810));_tmp863=_tmp862.f1;_tmp864=_tmp862.f2;{
struct Cyc_List_List*_tmp865=_tmp810;
struct Cyc_List_List*_tmp866=Cyc_Tcutil_substs(rgn,inst,_tmp864);
if(_tmp866 != _tmp864)
_tmp865=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp810,_tmp866);{
void*eff2;
if(_tmp80D == 0)
eff2=0;else{
# 3135
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp80D);
if(new_eff == _tmp80D)
eff2=_tmp80D;else{
# 3139
eff2=new_eff;}}{
# 3141
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp812 == 0)
cyc_varargs2=0;else{
# 3145
struct _dyneither_ptr*_tmp868;struct Cyc_Absyn_Tqual _tmp869;void*_tmp86A;int _tmp86B;struct Cyc_Absyn_VarargInfo _tmp867=*_tmp812;_tmp868=_tmp867.name;_tmp869=_tmp867.tq;_tmp86A=_tmp867.type;_tmp86B=_tmp867.inject;{
void*_tmp86C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp86A);
if(_tmp86C == _tmp86A)cyc_varargs2=_tmp812;else{
# 3149
struct Cyc_Absyn_VarargInfo*_tmp10AF;cyc_varargs2=((_tmp10AF=_cycalloc(sizeof(*_tmp10AF)),((_tmp10AF->name=_tmp868,((_tmp10AF->tq=_tmp869,((_tmp10AF->type=_tmp86C,((_tmp10AF->inject=_tmp86B,_tmp10AF))))))))));}};}{
# 3151
struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp86F;struct Cyc_List_List*_tmp870;struct _tuple1 _tmp86E=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,_tmp813);_tmp86F=_tmp86E.f1;_tmp870=_tmp86E.f2;{
struct Cyc_List_List*_tmp871=Cyc_Tcutil_substs(rgn,inst,_tmp86F);
struct Cyc_List_List*_tmp872=Cyc_Tcutil_substs(rgn,inst,_tmp870);
if(_tmp871 == _tmp86F  && _tmp872 == _tmp870)
rgn_po2=_tmp813;else{
# 3158
rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp871,_tmp872);}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10B5;struct Cyc_Absyn_FnInfo _tmp10B4;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10B3;return(void*)(
(_tmp10B3=_cycalloc(sizeof(*_tmp10B3)),((_tmp10B3[0]=((_tmp10B5.tag=9,((_tmp10B5.f1=((_tmp10B4.tvars=_tmp80C,((_tmp10B4.effect=eff2,((_tmp10B4.ret_tqual=_tmp80E,((_tmp10B4.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp80F),((_tmp10B4.args=_tmp865,((_tmp10B4.c_varargs=_tmp811,((_tmp10B4.cyc_varargs=cyc_varargs2,((_tmp10B4.rgn_po=rgn_po2,((_tmp10B4.attributes=_tmp814,_tmp10B4)))))))))))))))))),_tmp10B5)))),_tmp10B3))));};};};};};};};_LL51C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp815=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp7ED;if(_tmp815->tag != 10)goto _LL51E;else{_tmp816=_tmp815->f1;}}_LL51D: {
# 3163
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp876=_tmp816;for(0;_tmp876 != 0;_tmp876=_tmp876->tl){
void*_tmp877=(*((struct _tuple12*)_tmp876->hd)).f2;
void*_tmp878=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp877);
if(_tmp877 != _tmp878)
change=1;{
# 3171
struct Cyc_List_List*_tmp10B6;ts2=((_tmp10B6=_region_malloc(rgn,sizeof(*_tmp10B6)),((_tmp10B6->hd=_tmp878,((_tmp10B6->tl=ts2,_tmp10B6))))));};}}
# 3173
if(!change)
return t;{
struct Cyc_List_List*_tmp87A=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp816,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10B9;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10B8;return(void*)((_tmp10B8=_cycalloc(sizeof(*_tmp10B8)),((_tmp10B8[0]=((_tmp10B9.tag=10,((_tmp10B9.f1=_tmp87A,_tmp10B9)))),_tmp10B8))));};}_LL51E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp817=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp7ED;if(_tmp817->tag != 12)goto _LL520;else{_tmp818=_tmp817->f1;_tmp819=_tmp817->f2;}}_LL51F: {
# 3178
struct Cyc_List_List*_tmp87D=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp819);
struct Cyc_List_List*_tmp87E=Cyc_Tcutil_substs(rgn,inst,_tmp87D);
if(_tmp87E == _tmp87D)
return t;{
struct Cyc_List_List*_tmp87F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp819,_tmp87E);
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp10BC;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp10BB;return(void*)((_tmp10BB=_cycalloc(sizeof(*_tmp10BB)),((_tmp10BB[0]=((_tmp10BC.tag=12,((_tmp10BC.f1=_tmp818,((_tmp10BC.f2=_tmp87F,_tmp10BC)))))),_tmp10BB))));};}_LL520: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp81A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7ED;if(_tmp81A->tag != 1)goto _LL522;else{_tmp81B=(void*)_tmp81A->f2;}}_LL521:
# 3185
 if(_tmp81B != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81B);else{
return t;}_LL522: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp81C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp7ED;if(_tmp81C->tag != 15)goto _LL524;else{_tmp81D=(void*)_tmp81C->f1;}}_LL523: {
# 3188
void*_tmp882=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81D);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp10BF;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp10BE;return _tmp882 == _tmp81D?t:(void*)((_tmp10BE=_cycalloc(sizeof(*_tmp10BE)),((_tmp10BE[0]=((_tmp10BF.tag=15,((_tmp10BF.f1=_tmp882,_tmp10BF)))),_tmp10BE))));}_LL524: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp81E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp7ED;if(_tmp81E->tag != 16)goto _LL526;else{_tmp81F=(void*)_tmp81E->f1;_tmp820=(void*)_tmp81E->f2;}}_LL525: {
# 3191
void*_tmp885=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81F);
void*_tmp886=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp820);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp10C2;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp10C1;return _tmp885 == _tmp81F  && _tmp886 == _tmp820?t:(void*)((_tmp10C1=_cycalloc(sizeof(*_tmp10C1)),((_tmp10C1[0]=((_tmp10C2.tag=16,((_tmp10C2.f1=_tmp885,((_tmp10C2.f2=_tmp886,_tmp10C2)))))),_tmp10C1))));}_LL526: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp821=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp7ED;if(_tmp821->tag != 19)goto _LL528;else{_tmp822=(void*)_tmp821->f1;}}_LL527: {
# 3195
void*_tmp889=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp822);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp10C5;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp10C4;return _tmp889 == _tmp822?t:(void*)((_tmp10C4=_cycalloc(sizeof(*_tmp10C4)),((_tmp10C4[0]=((_tmp10C5.tag=19,((_tmp10C5.f1=_tmp889,_tmp10C5)))),_tmp10C4))));}_LL528: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp823=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7ED;if(_tmp823->tag != 18)goto _LL52A;else{_tmp824=_tmp823->f1;}}_LL529: {
# 3198
struct Cyc_Absyn_Exp*_tmp88C=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp824);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp10C8;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp10C7;return _tmp88C == _tmp824?t:(void*)((_tmp10C7=_cycalloc(sizeof(*_tmp10C7)),((_tmp10C7[0]=((_tmp10C8.tag=18,((_tmp10C8.f1=_tmp88C,_tmp10C8)))),_tmp10C7))));}_LL52A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp825=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7ED;if(_tmp825->tag != 13)goto _LL52C;}_LL52B:
 goto _LL52D;_LL52C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp826=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp7ED;if(_tmp826->tag != 14)goto _LL52E;}_LL52D:
 goto _LL52F;_LL52E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp827=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp7ED;if(_tmp827->tag != 0)goto _LL530;}_LL52F:
 goto _LL531;_LL530: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp828=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7ED;if(_tmp828->tag != 6)goto _LL532;}_LL531:
 goto _LL533;_LL532: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp829=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7ED;if(_tmp829->tag != 7)goto _LL534;}_LL533:
 goto _LL535;_LL534: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp82A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp7ED;if(_tmp82A->tag != 22)goto _LL536;}_LL535:
 goto _LL537;_LL536: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp82B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp7ED;if(_tmp82B->tag != 21)goto _LL538;}_LL537:
 goto _LL539;_LL538: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp82C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp7ED;if(_tmp82C->tag != 20)goto _LL53A;}_LL539:
 return t;_LL53A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp82D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp7ED;if(_tmp82D->tag != 25)goto _LL53C;else{_tmp82E=(void*)_tmp82D->f1;}}_LL53B: {
# 3209
void*_tmp88F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82E);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp10CB;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp10CA;return _tmp88F == _tmp82E?t:(void*)((_tmp10CA=_cycalloc(sizeof(*_tmp10CA)),((_tmp10CA[0]=((_tmp10CB.tag=25,((_tmp10CB.f1=_tmp88F,_tmp10CB)))),_tmp10CA))));}_LL53C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp82F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp7ED;if(_tmp82F->tag != 23)goto _LL53E;else{_tmp830=(void*)_tmp82F->f1;}}_LL53D: {
# 3212
void*_tmp892=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp830);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp10CE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp10CD;return _tmp892 == _tmp830?t:(void*)((_tmp10CD=_cycalloc(sizeof(*_tmp10CD)),((_tmp10CD[0]=((_tmp10CE.tag=23,((_tmp10CE.f1=_tmp892,_tmp10CE)))),_tmp10CD))));}_LL53E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp831=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp7ED;if(_tmp831->tag != 24)goto _LL540;else{_tmp832=_tmp831->f1;}}_LL53F: {
# 3215
struct Cyc_List_List*_tmp895=Cyc_Tcutil_substs(rgn,inst,_tmp832);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp10D1;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp10D0;return _tmp895 == _tmp832?t:(void*)((_tmp10D0=_cycalloc(sizeof(*_tmp10D0)),((_tmp10D0[0]=((_tmp10D1.tag=24,((_tmp10D1.f1=_tmp895,_tmp10D1)))),_tmp10D0))));}_LL540: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp833=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp7ED;if(_tmp833->tag != 26)goto _LL50B;}_LL541: {
const char*_tmp10D4;void*_tmp10D3;(_tmp10D3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D4="found typedecltype in rsubs",_tag_dyneither(_tmp10D4,sizeof(char),28))),_tag_dyneither(_tmp10D3,sizeof(void*),0)));}_LL50B:;}
# 3221
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3224
if(ts == 0)
return 0;{
void*_tmp89A=(void*)ts->hd;
struct Cyc_List_List*_tmp89B=ts->tl;
void*_tmp89C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp89A);
struct Cyc_List_List*_tmp89D=Cyc_Tcutil_substs(rgn,inst,_tmp89B);
if(_tmp89A == _tmp89C  && _tmp89B == _tmp89D)
return ts;{
struct Cyc_List_List*_tmp10D5;return(_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5->hd=_tmp89C,((_tmp10D5->tl=_tmp89D,_tmp10D5)))));};};}
# 3235
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3242
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp89F=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10D8;struct _tuple16*_tmp10D7;return(_tmp10D7=_cycalloc(sizeof(*_tmp10D7)),((_tmp10D7->f1=tv,((_tmp10D7->f2=Cyc_Absyn_new_evar(_tmp89F,((_tmp10D8=_cycalloc(sizeof(*_tmp10D8)),((_tmp10D8->v=s,_tmp10D8))))),_tmp10D7)))));}
# 3247
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3249
struct Cyc_List_List*_tmp8A3;struct _RegionHandle*_tmp8A4;struct _tuple17*_tmp8A2=env;_tmp8A3=_tmp8A2->f1;_tmp8A4=_tmp8A2->f2;{
struct Cyc_Core_Opt*_tmp8A5=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10DB;struct _tuple16*_tmp10DA;return(_tmp10DA=_region_malloc(_tmp8A4,sizeof(*_tmp10DA)),((_tmp10DA->f1=tv,((_tmp10DA->f2=Cyc_Absyn_new_evar(_tmp8A5,((_tmp10DB=_cycalloc(sizeof(*_tmp10DB)),((_tmp10DB->v=_tmp8A3,_tmp10DB))))),_tmp10DA)))));};}
# 3259
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3263
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp10E1;void*_tmp10E0[3];struct Cyc_String_pa_PrintArg_struct _tmp10DF;struct Cyc_String_pa_PrintArg_struct _tmp10DE;struct Cyc_String_pa_PrintArg_struct _tmp10DD;(_tmp10DD.tag=0,((_tmp10DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp10DE.tag=0,((_tmp10DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp10DF.tag=0,((_tmp10DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp10E0[0]=& _tmp10DF,((_tmp10E0[1]=& _tmp10DE,((_tmp10E0[2]=& _tmp10DD,Cyc_Tcutil_terr(loc,((_tmp10E1="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp10E1,sizeof(char),59))),_tag_dyneither(_tmp10E0,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp10E4;void*_tmp10E3;(_tmp10E3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E4="same type variable has different identity!",_tag_dyneither(_tmp10E4,sizeof(char),43))),_tag_dyneither(_tmp10E3,sizeof(void*),0)));}}
return tvs;}}}
# 3277
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp10E5;return(_tmp10E5=_cycalloc(sizeof(*_tmp10E5)),((_tmp10E5->hd=tv,((_tmp10E5->tl=tvs,_tmp10E5)))));};}
# 3283
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3285
if(tv->identity == - 1){
const char*_tmp10E8;void*_tmp10E7;(_tmp10E7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E8="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp10E8,sizeof(char),39))),_tag_dyneither(_tmp10E7,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3289
struct Cyc_Absyn_Tvar*_tmp8B2=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp8B2->identity == - 1){
const char*_tmp10EB;void*_tmp10EA;(_tmp10EA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10EB="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp10EB,sizeof(char),41))),_tag_dyneither(_tmp10EA,sizeof(void*),0)));}
if(_tmp8B2->identity == tv->identity)
return tvs;}}{
# 3296
struct Cyc_List_List*_tmp10EC;return(_tmp10EC=_cycalloc(sizeof(*_tmp10EC)),((_tmp10EC->hd=tv,((_tmp10EC->tl=tvs,_tmp10EC)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3302
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3307
if(tv->identity == - 1){
const char*_tmp10EF;void*_tmp10EE;(_tmp10EE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10EF="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp10EF,sizeof(char),44))),_tag_dyneither(_tmp10EE,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3311
struct Cyc_Absyn_Tvar*_tmp8B9;int*_tmp8BA;struct _tuple28*_tmp8B8=(struct _tuple28*)tvs2->hd;_tmp8B9=_tmp8B8->f1;_tmp8BA=(int*)& _tmp8B8->f2;
if(_tmp8B9->identity == - 1){
const char*_tmp10F2;void*_tmp10F1;(_tmp10F1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F2="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp10F2,sizeof(char),46))),_tag_dyneither(_tmp10F1,sizeof(void*),0)));}
if(_tmp8B9->identity == tv->identity){
*_tmp8BA=*_tmp8BA  || b;
return tvs;}}}{
# 3319
struct _tuple28*_tmp10F5;struct Cyc_List_List*_tmp10F4;return(_tmp10F4=_region_malloc(r,sizeof(*_tmp10F4)),((_tmp10F4->hd=((_tmp10F5=_region_malloc(r,sizeof(*_tmp10F5)),((_tmp10F5->f1=tv,((_tmp10F5->f2=b,_tmp10F5)))))),((_tmp10F4->tl=tvs,_tmp10F4)))));};}
# 3323
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3325
if(tv->identity == - 1){
const char*_tmp10F9;void*_tmp10F8[1];struct Cyc_String_pa_PrintArg_struct _tmp10F7;(_tmp10F7.tag=0,((_tmp10F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp10F8[0]=& _tmp10F7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F9="bound tvar id for %s is NULL",_tag_dyneither(_tmp10F9,sizeof(char),29))),_tag_dyneither(_tmp10F8,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp10FA;return(_tmp10FA=_cycalloc(sizeof(*_tmp10FA)),((_tmp10FA->hd=tv,((_tmp10FA->tl=tvs,_tmp10FA)))));};}struct _tuple29{void*f1;int f2;};
# 3334
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3337
void*_tmp8C3=Cyc_Tcutil_compress(e);int _tmp8C5;_LL54D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8C4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8C3;if(_tmp8C4->tag != 1)goto _LL54F;else{_tmp8C5=_tmp8C4->f3;}}_LL54E:
# 3339
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
void*_tmp8C7;int*_tmp8C8;struct _tuple29*_tmp8C6=(struct _tuple29*)es2->hd;_tmp8C7=_tmp8C6->f1;_tmp8C8=(int*)& _tmp8C6->f2;{
void*_tmp8C9=Cyc_Tcutil_compress(_tmp8C7);int _tmp8CB;_LL552: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8CA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8C9;if(_tmp8CA->tag != 1)goto _LL554;else{_tmp8CB=_tmp8CA->f3;}}_LL553:
# 3343
 if(_tmp8C5 == _tmp8CB){
if(b != *_tmp8C8)*_tmp8C8=1;
return es;}
# 3347
goto _LL551;_LL554:;_LL555:
 goto _LL551;_LL551:;};}}{
# 3351
struct _tuple29*_tmp10FD;struct Cyc_List_List*_tmp10FC;return(_tmp10FC=_region_malloc(r,sizeof(*_tmp10FC)),((_tmp10FC->hd=((_tmp10FD=_region_malloc(r,sizeof(*_tmp10FD)),((_tmp10FD->f1=e,((_tmp10FD->f2=b,_tmp10FD)))))),((_tmp10FC->tl=es,_tmp10FC)))));};_LL54F:;_LL550:
 return es;_LL54C:;}
# 3356
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3359
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3368
if(!present){struct Cyc_List_List*_tmp10FE;r=((_tmp10FE=_region_malloc(rgn,sizeof(*_tmp10FE)),((_tmp10FE->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp10FE->tl=r,_tmp10FE))))));}}
# 3370
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3375
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3379
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp8D0;int _tmp8D1;struct _tuple28 _tmp8CF=*((struct _tuple28*)tvs->hd);_tmp8D0=_tmp8CF.f1;_tmp8D1=_tmp8CF.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp8D0->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3389
if(!present){struct Cyc_List_List*_tmp10FF;res=((_tmp10FF=_region_malloc(r,sizeof(*_tmp10FF)),((_tmp10FF->hd=(struct _tuple28*)tvs->hd,((_tmp10FF->tl=res,_tmp10FF))))));}};}
# 3391
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3395
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3397
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp1103;void*_tmp1102[1];struct Cyc_String_pa_PrintArg_struct _tmp1101;(_tmp1101.tag=0,((_tmp1101.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1102[0]=& _tmp1101,Cyc_Tcutil_terr(loc,((_tmp1103="bitfield %s does not have constant width",_tag_dyneither(_tmp1103,sizeof(char),41))),_tag_dyneither(_tmp1102,sizeof(void*),1)))))));}else{
# 3402
unsigned int _tmp8D7;int _tmp8D8;struct _tuple14 _tmp8D6=Cyc_Evexp_eval_const_uint_exp(width);_tmp8D7=_tmp8D6.f1;_tmp8D8=_tmp8D6.f2;
if(!_tmp8D8){
const char*_tmp1106;void*_tmp1105;(_tmp1105=0,Cyc_Tcutil_terr(loc,((_tmp1106="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1106,sizeof(char),45))),_tag_dyneither(_tmp1105,sizeof(void*),0)));}
w=_tmp8D7;}{
# 3407
void*_tmp8DB=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of _tmp8DD;_LL557: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8DC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8DB;if(_tmp8DC->tag != 6)goto _LL559;else{_tmp8DD=_tmp8DC->f2;}}_LL558:
# 3410
 switch(_tmp8DD){case Cyc_Absyn_Char_sz: _LL55B:
 if(w > 8){const char*_tmp1109;void*_tmp1108;(_tmp1108=0,Cyc_Tcutil_terr(loc,((_tmp1109="bitfield larger than type",_tag_dyneither(_tmp1109,sizeof(char),26))),_tag_dyneither(_tmp1108,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL55C:
 if(w > 16){const char*_tmp110C;void*_tmp110B;(_tmp110B=0,Cyc_Tcutil_terr(loc,((_tmp110C="bitfield larger than type",_tag_dyneither(_tmp110C,sizeof(char),26))),_tag_dyneither(_tmp110B,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL55D:
 goto _LL55E;case Cyc_Absyn_Int_sz: _LL55E:
# 3415
 if(w > 32){const char*_tmp110F;void*_tmp110E;(_tmp110E=0,Cyc_Tcutil_terr(loc,((_tmp110F="bitfield larger than type",_tag_dyneither(_tmp110F,sizeof(char),26))),_tag_dyneither(_tmp110E,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL55F:
# 3417
 if(w > 64){const char*_tmp1112;void*_tmp1111;(_tmp1111=0,Cyc_Tcutil_terr(loc,((_tmp1112="bitfield larger than type",_tag_dyneither(_tmp1112,sizeof(char),26))),_tag_dyneither(_tmp1111,sizeof(void*),0)));}break;}
# 3419
goto _LL556;_LL559:;_LL55A:
# 3421
{const char*_tmp1117;void*_tmp1116[2];struct Cyc_String_pa_PrintArg_struct _tmp1115;struct Cyc_String_pa_PrintArg_struct _tmp1114;(_tmp1114.tag=0,((_tmp1114.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp1115.tag=0,((_tmp1115.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1116[0]=& _tmp1115,((_tmp1116[1]=& _tmp1114,Cyc_Tcutil_terr(loc,((_tmp1117="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1117,sizeof(char),52))),_tag_dyneither(_tmp1116,sizeof(void*),2)))))))))))));}
goto _LL556;_LL556:;};}}
# 3428
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp8EA=(void*)atts->hd;_LL562: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp8EB=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp8EA;if(_tmp8EB->tag != 7)goto _LL564;}_LL563:
 continue;_LL564: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp8EC=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp8EA;if(_tmp8EC->tag != 6)goto _LL566;}_LL565:
 continue;_LL566:;_LL567: {
const char*_tmp111C;void*_tmp111B[2];struct Cyc_String_pa_PrintArg_struct _tmp111A;struct Cyc_String_pa_PrintArg_struct _tmp1119;(_tmp1119.tag=0,((_tmp1119.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp111A.tag=0,((_tmp111A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp111B[0]=& _tmp111A,((_tmp111B[1]=& _tmp1119,Cyc_Tcutil_terr(loc,((_tmp111C="bad attribute %s on member %s",_tag_dyneither(_tmp111C,sizeof(char),30))),_tag_dyneither(_tmp111B,sizeof(void*),2)))))))))))));}_LL561:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3452
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3456
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp8F1=t;struct Cyc_Absyn_Typedefdecl*_tmp8F3;void*_tmp8F4;_LL569: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp8F2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp8F1;if(_tmp8F2->tag != 17)goto _LL56B;else{_tmp8F3=_tmp8F2->f3;_tmp8F4=(void*)_tmp8F2->f4;}}_LL56A:
# 3459
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp8F3))->tq).real_const  || (_tmp8F3->tq).print_const){
if(declared_const){const char*_tmp111F;void*_tmp111E;(_tmp111E=0,Cyc_Tcutil_warn(loc,((_tmp111F="extra const",_tag_dyneither(_tmp111F,sizeof(char),12))),_tag_dyneither(_tmp111E,sizeof(void*),0)));}
return 1;}
# 3464
if((unsigned int)_tmp8F4)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp8F4);else{
return declared_const;}_LL56B:;_LL56C:
 return declared_const;_LL568:;}
# 3471
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp8F7=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp8F9;_LL56E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F7;if(_tmp8F8->tag != 5)goto _LL570;else{_tmp8F9=((_tmp8F8->f1).ptr_atts).rgn;}}_LL56F:
# 3476
{void*_tmp8FA=Cyc_Tcutil_compress(_tmp8F9);struct Cyc_Absyn_Tvar*_tmp8FC;_LL573: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8FB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8FA;if(_tmp8FB->tag != 2)goto _LL575;else{_tmp8FC=_tmp8FB->f1;}}_LL574:
# 3478
 return Cyc_Absyn_tvar_cmp(tvar,_tmp8FC)== 0;_LL575:;_LL576:
 goto _LL572;_LL572:;}
# 3481
goto _LL56D;_LL570:;_LL571:
 goto _LL56D;_LL56D:;}}else{
# 3487
return 1;}
return 0;}
# 3491
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3494
void*_tmp8FD=Cyc_Absyn_compress_kb(tvar->kind);_LL578: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8FE=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8FD;if(_tmp8FE->tag != 2)goto _LL57A;else{if((_tmp8FE->f2)->kind != Cyc_Absyn_RgnKind)goto _LL57A;if((_tmp8FE->f2)->aliasqual != Cyc_Absyn_Top)goto _LL57A;}}_LL579:
 goto _LL57B;_LL57A: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8FF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8FD;if(_tmp8FF->tag != 0)goto _LL57C;else{if((_tmp8FF->f1)->kind != Cyc_Absyn_RgnKind)goto _LL57C;if((_tmp8FF->f1)->aliasqual != Cyc_Absyn_Top)goto _LL57C;}}_LL57B:
# 3503
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3506
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3512
return& Cyc_Tcutil_trk;_LL57C:;_LL57D:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL577:;}
# 3547 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3550
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3557
{void*_tmp900=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt**_tmp903;void**_tmp904;struct Cyc_Absyn_Tvar*_tmp906;void*_tmp908;void***_tmp909;struct Cyc_List_List*_tmp90B;struct _tuple2*_tmp90D;struct Cyc_Absyn_Enumdecl**_tmp90E;union Cyc_Absyn_DatatypeInfoU*_tmp910;struct Cyc_List_List**_tmp911;union Cyc_Absyn_DatatypeFieldInfoU*_tmp913;struct Cyc_List_List*_tmp914;void*_tmp916;struct Cyc_Absyn_Tqual*_tmp917;void*_tmp918;union Cyc_Absyn_Constraint*_tmp919;union Cyc_Absyn_Constraint*_tmp91A;union Cyc_Absyn_Constraint*_tmp91B;void*_tmp91D;struct Cyc_Absyn_Exp*_tmp91F;void*_tmp923;struct Cyc_Absyn_Tqual*_tmp924;struct Cyc_Absyn_Exp**_tmp925;union Cyc_Absyn_Constraint*_tmp926;unsigned int _tmp927;struct Cyc_List_List**_tmp929;void**_tmp92A;struct Cyc_Absyn_Tqual*_tmp92B;void*_tmp92C;struct Cyc_List_List*_tmp92D;int _tmp92E;struct Cyc_Absyn_VarargInfo*_tmp92F;struct Cyc_List_List*_tmp930;struct Cyc_List_List*_tmp931;struct Cyc_List_List*_tmp933;enum Cyc_Absyn_AggrKind _tmp935;struct Cyc_List_List*_tmp936;union Cyc_Absyn_AggrInfoU*_tmp938;struct Cyc_List_List**_tmp939;struct _tuple2*_tmp93B;struct Cyc_List_List**_tmp93C;struct Cyc_Absyn_Typedefdecl**_tmp93D;void**_tmp93E;void*_tmp943;void*_tmp945;void*_tmp946;void*_tmp948;void*_tmp94A;struct Cyc_List_List*_tmp94C;_LL57F: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp901=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp900;if(_tmp901->tag != 0)goto _LL581;}_LL580:
 goto _LL57E;_LL581: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp902=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp900;if(_tmp902->tag != 1)goto _LL583;else{_tmp903=(struct Cyc_Core_Opt**)& _tmp902->f1;_tmp904=(void**)((void**)& _tmp902->f2);}}_LL582:
# 3561
 if(*_tmp903 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp903))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp903))->v,expected_kind))
*_tmp903=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3566
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp904=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3569
*_tmp904=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1122;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1121;struct Cyc_Absyn_Tvar*_tmp94D=Cyc_Tcutil_new_tvar((void*)((_tmp1121=_cycalloc(sizeof(*_tmp1121)),((_tmp1121[0]=((_tmp1122.tag=2,((_tmp1122.f1=0,((_tmp1122.f2=expected_kind,_tmp1122)))))),_tmp1121)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1125;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1124;*_tmp904=(void*)((_tmp1124=_cycalloc(sizeof(*_tmp1124)),((_tmp1124[0]=((_tmp1125.tag=2,((_tmp1125.f1=_tmp94D,_tmp1125)))),_tmp1124))));}
_tmp906=_tmp94D;goto _LL584;}else{
# 3575
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3577
goto _LL57E;_LL583: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp905=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp900;if(_tmp905->tag != 2)goto _LL585;else{_tmp906=_tmp905->f1;}}_LL584:
# 3579
{void*_tmp952=Cyc_Absyn_compress_kb(_tmp906->kind);struct Cyc_Core_Opt**_tmp954;_LL5B6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp953=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp952;if(_tmp953->tag != 1)goto _LL5B8;else{_tmp954=(struct Cyc_Core_Opt**)& _tmp953->f1;}}_LL5B7:
# 3581
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp112B;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp112A;struct Cyc_Core_Opt*_tmp1129;*_tmp954=((_tmp1129=_cycalloc(sizeof(*_tmp1129)),((_tmp1129->v=(void*)((_tmp112B=_cycalloc(sizeof(*_tmp112B)),((_tmp112B[0]=((_tmp112A.tag=2,((_tmp112A.f1=0,((_tmp112A.f2=expected_kind,_tmp112A)))))),_tmp112B)))),_tmp1129))));}goto _LL5B5;_LL5B8:;_LL5B9:
 goto _LL5B5;_LL5B5:;}
# 3586
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp906);
# 3589
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp906,put_in_effect);
# 3591
{void*_tmp958=Cyc_Absyn_compress_kb(_tmp906->kind);struct Cyc_Core_Opt**_tmp95A;struct Cyc_Absyn_Kind*_tmp95B;_LL5BB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp959=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp958;if(_tmp959->tag != 2)goto _LL5BD;else{_tmp95A=(struct Cyc_Core_Opt**)& _tmp959->f1;_tmp95B=_tmp959->f2;}}_LL5BC:
# 3593
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp95B)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1131;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1130;struct Cyc_Core_Opt*_tmp112F;*_tmp95A=((_tmp112F=_cycalloc(sizeof(*_tmp112F)),((_tmp112F->v=(void*)((_tmp1131=_cycalloc(sizeof(*_tmp1131)),((_tmp1131[0]=((_tmp1130.tag=2,((_tmp1130.f1=0,((_tmp1130.f2=expected_kind,_tmp1130)))))),_tmp1131)))),_tmp112F))));}
goto _LL5BA;_LL5BD:;_LL5BE:
 goto _LL5BA;_LL5BA:;}
# 3598
goto _LL57E;_LL585: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp907=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp900;if(_tmp907->tag != 26)goto _LL587;else{_tmp908=(_tmp907->f1)->r;_tmp909=(void***)& _tmp907->f2;}}_LL586: {
# 3604
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp95F=_tmp908;struct Cyc_Absyn_Aggrdecl*_tmp961;struct Cyc_Absyn_Enumdecl*_tmp963;struct Cyc_Absyn_Datatypedecl*_tmp965;_LL5C0: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp960=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp95F;if(_tmp960->tag != 0)goto _LL5C2;else{_tmp961=_tmp960->f1;}}_LL5C1:
# 3607
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp961);goto _LL5BF;_LL5C2: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp962=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp95F;if(_tmp962->tag != 1)goto _LL5C4;else{_tmp963=_tmp962->f1;}}_LL5C3:
# 3609
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp963);goto _LL5BF;_LL5C4: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp964=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp95F;if(_tmp964->tag != 2)goto _LL5BF;else{_tmp965=_tmp964->f1;}}_LL5C5:
# 3611
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp965);goto _LL5BF;_LL5BF:;}
# 3613
{void**_tmp1132;*_tmp909=((_tmp1132=_cycalloc(sizeof(*_tmp1132)),((_tmp1132[0]=new_t,_tmp1132))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL587: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp90A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp900;if(_tmp90A->tag != 14)goto _LL589;else{_tmp90B=_tmp90A->f1;}}_LL588: {
# 3618
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp967=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp967;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp90B != 0;_tmp90B=_tmp90B->tl){
struct Cyc_Absyn_Enumfield*_tmp968=(struct Cyc_Absyn_Enumfield*)_tmp90B->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp968->name).f2)){
const char*_tmp1136;void*_tmp1135[1];struct Cyc_String_pa_PrintArg_struct _tmp1134;(_tmp1134.tag=0,((_tmp1134.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp968->name).f2),((_tmp1135[0]=& _tmp1134,Cyc_Tcutil_terr(_tmp968->loc,((_tmp1136="duplicate enum field name %s",_tag_dyneither(_tmp1136,sizeof(char),29))),_tag_dyneither(_tmp1135,sizeof(void*),1)))))));}else{
# 3627
struct Cyc_List_List*_tmp1137;prev_fields=((_tmp1137=_region_malloc(uprev_rgn,sizeof(*_tmp1137)),((_tmp1137->hd=(*_tmp968->name).f2,((_tmp1137->tl=prev_fields,_tmp1137))))));}
# 3629
if(_tmp968->tag == 0)
_tmp968->tag=Cyc_Absyn_uint_exp(tag_count,_tmp968->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp968->tag))){
const char*_tmp113B;void*_tmp113A[1];struct Cyc_String_pa_PrintArg_struct _tmp1139;(_tmp1139.tag=0,((_tmp1139.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp968->name).f2),((_tmp113A[0]=& _tmp1139,Cyc_Tcutil_terr(loc,((_tmp113B="enum field %s: expression is not constant",_tag_dyneither(_tmp113B,sizeof(char),42))),_tag_dyneither(_tmp113A,sizeof(void*),1)))))));}}{
# 3634
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp968->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp970=(*_tmp968->name).f1;_LL5C7: if((_tmp970.Rel_n).tag != 1)goto _LL5C9;_LL5C8:
# 3638
(*_tmp968->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5C6;_LL5C9:;_LL5CA:
 goto _LL5C6;_LL5C6:;}{
# 3641
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp1141;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp1140;struct _tuple29*_tmp113F;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp968->name).f2,(
(_tmp113F=_cycalloc(sizeof(*_tmp113F)),((_tmp113F->f1=(void*)((_tmp1141=_cycalloc(sizeof(*_tmp1141)),((_tmp1141[0]=((_tmp1140.tag=4,((_tmp1140.f1=t,((_tmp1140.f2=_tmp968,_tmp1140)))))),_tmp1141)))),((_tmp113F->f2=1,_tmp113F)))))));};};}}
# 3645
_npop_handler(0);goto _LL57E;
# 3619
;_pop_region(uprev_rgn);}_LL589: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp90C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp900;if(_tmp90C->tag != 13)goto _LL58B;else{_tmp90D=_tmp90C->f1;_tmp90E=(struct Cyc_Absyn_Enumdecl**)& _tmp90C->f2;}}_LL58A:
# 3647
 if(*_tmp90E == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp90E))->fields == 0){
struct _handler_cons _tmp974;_push_handler(& _tmp974);{int _tmp976=0;if(setjmp(_tmp974.handler))_tmp976=1;if(!_tmp976){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp90D);
*_tmp90E=*ed;}
# 3649
;_pop_handler();}else{void*_tmp975=(void*)_exn_thrown;void*_tmp978=_tmp975;void*_tmp97A;_LL5CC: {struct Cyc_Dict_Absent_exn_struct*_tmp979=(struct Cyc_Dict_Absent_exn_struct*)_tmp978;if(_tmp979->tag != Cyc_Dict_Absent)goto _LL5CE;}_LL5CD: {
# 3653
struct Cyc_Tcenv_Genv*_tmp97B=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1142;struct Cyc_Absyn_Enumdecl*_tmp97C=(_tmp1142=_cycalloc(sizeof(*_tmp1142)),((_tmp1142->sc=Cyc_Absyn_Extern,((_tmp1142->name=_tmp90D,((_tmp1142->fields=0,_tmp1142)))))));
Cyc_Tc_tcEnumdecl(te,_tmp97B,loc,_tmp97C);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp90D);
*_tmp90E=*ed;
goto _LL5CB;};}_LL5CE: _tmp97A=_tmp978;_LL5CF:(void)_throw(_tmp97A);_LL5CB:;}};}{
# 3662
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp90E);
# 3664
*_tmp90D=(ed->name)[0];
goto _LL57E;};_LL58B: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp90F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp900;if(_tmp90F->tag != 3)goto _LL58D;else{_tmp910=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp90F->f1).datatype_info;_tmp911=(struct Cyc_List_List**)&(_tmp90F->f1).targs;}}_LL58C: {
# 3667
struct Cyc_List_List*_tmp97E=*_tmp911;
{union Cyc_Absyn_DatatypeInfoU _tmp97F=*_tmp910;struct _tuple2*_tmp980;int _tmp981;struct Cyc_Absyn_Datatypedecl*_tmp982;_LL5D1: if((_tmp97F.UnknownDatatype).tag != 1)goto _LL5D3;_tmp980=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp97F.UnknownDatatype).val).name;_tmp981=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp97F.UnknownDatatype).val).is_extensible;_LL5D2: {
# 3670
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp983;_push_handler(& _tmp983);{int _tmp985=0;if(setjmp(_tmp983.handler))_tmp985=1;if(!_tmp985){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp980);;_pop_handler();}else{void*_tmp984=(void*)_exn_thrown;void*_tmp987=_tmp984;void*_tmp989;_LL5D6: {struct Cyc_Dict_Absent_exn_struct*_tmp988=(struct Cyc_Dict_Absent_exn_struct*)_tmp987;if(_tmp988->tag != Cyc_Dict_Absent)goto _LL5D8;}_LL5D7: {
# 3674
struct Cyc_Tcenv_Genv*_tmp98A=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp1143;struct Cyc_Absyn_Datatypedecl*_tmp98B=(_tmp1143=_cycalloc(sizeof(*_tmp1143)),((_tmp1143->sc=Cyc_Absyn_Extern,((_tmp1143->name=_tmp980,((_tmp1143->tvs=0,((_tmp1143->fields=0,((_tmp1143->is_extensible=_tmp981,_tmp1143)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmp98A,loc,_tmp98B);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp980);
# 3679
if(_tmp97E != 0){
{const char*_tmp1147;void*_tmp1146[1];struct Cyc_String_pa_PrintArg_struct _tmp1145;(_tmp1145.tag=0,((_tmp1145.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp980)),((_tmp1146[0]=& _tmp1145,Cyc_Tcutil_terr(loc,((_tmp1147="declare parameterized datatype %s before using",_tag_dyneither(_tmp1147,sizeof(char),47))),_tag_dyneither(_tmp1146,sizeof(void*),1)))))));}
return cvtenv;}
# 3684
goto _LL5D5;}_LL5D8: _tmp989=_tmp987;_LL5D9:(void)_throw(_tmp989);_LL5D5:;}};}
# 3690
if(_tmp981  && !(*tudp)->is_extensible){
const char*_tmp114B;void*_tmp114A[1];struct Cyc_String_pa_PrintArg_struct _tmp1149;(_tmp1149.tag=0,((_tmp1149.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp980)),((_tmp114A[0]=& _tmp1149,Cyc_Tcutil_terr(loc,((_tmp114B="datatype %s was not declared @extensible",_tag_dyneither(_tmp114B,sizeof(char),41))),_tag_dyneither(_tmp114A,sizeof(void*),1)))))));}
*_tmp910=Cyc_Absyn_KnownDatatype(tudp);
_tmp982=*tudp;goto _LL5D4;}_LL5D3: if((_tmp97F.KnownDatatype).tag != 2)goto _LL5D0;_tmp982=*((struct Cyc_Absyn_Datatypedecl**)(_tmp97F.KnownDatatype).val);_LL5D4: {
# 3697
struct Cyc_List_List*tvs=_tmp982->tvs;
for(0;_tmp97E != 0  && tvs != 0;(_tmp97E=_tmp97E->tl,tvs=tvs->tl)){
void*t=(void*)_tmp97E->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3703
{struct _tuple0 _tmp114C;struct _tuple0 _tmp994=(_tmp114C.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp114C.f2=t,_tmp114C)));struct Cyc_Absyn_Tvar*_tmp997;_LL5DB:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp995=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp994.f1;if(_tmp995->tag != 1)goto _LL5DD;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp996=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp994.f2;if(_tmp996->tag != 2)goto _LL5DD;else{_tmp997=_tmp996->f1;}};_LL5DC:
# 3705
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp997);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp997,1);
continue;_LL5DD:;_LL5DE:
 goto _LL5DA;_LL5DA:;}{
# 3710
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3714
if(_tmp97E != 0){
const char*_tmp1150;void*_tmp114F[1];struct Cyc_String_pa_PrintArg_struct _tmp114E;(_tmp114E.tag=0,((_tmp114E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp982->name)),((_tmp114F[0]=& _tmp114E,Cyc_Tcutil_terr(loc,((_tmp1150="too many type arguments for datatype %s",_tag_dyneither(_tmp1150,sizeof(char),40))),_tag_dyneither(_tmp114F,sizeof(void*),1)))))));}
if(tvs != 0){
# 3719
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1151;hidden_ts=((_tmp1151=_cycalloc(sizeof(*_tmp1151)),((_tmp1151->hd=e,((_tmp1151->tl=hidden_ts,_tmp1151))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3726
*_tmp911=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp911,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3728
goto _LL5D0;}_LL5D0:;}
# 3730
goto _LL57E;}_LL58D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp912=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp900;if(_tmp912->tag != 4)goto _LL58F;else{_tmp913=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp912->f1).field_info;_tmp914=(_tmp912->f1).targs;}}_LL58E:
# 3733
{union Cyc_Absyn_DatatypeFieldInfoU _tmp99C=*_tmp913;struct _tuple2*_tmp99D;struct _tuple2*_tmp99E;int _tmp99F;struct Cyc_Absyn_Datatypedecl*_tmp9A0;struct Cyc_Absyn_Datatypefield*_tmp9A1;_LL5E0: if((_tmp99C.UnknownDatatypefield).tag != 1)goto _LL5E2;_tmp99D=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp99C.UnknownDatatypefield).val).datatype_name;_tmp99E=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp99C.UnknownDatatypefield).val).field_name;_tmp99F=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp99C.UnknownDatatypefield).val).is_extensible;_LL5E1: {
# 3735
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp9A2;_push_handler(& _tmp9A2);{int _tmp9A4=0;if(setjmp(_tmp9A2.handler))_tmp9A4=1;if(!_tmp9A4){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp99D);;_pop_handler();}else{void*_tmp9A3=(void*)_exn_thrown;void*_tmp9A6=_tmp9A3;void*_tmp9A8;_LL5E5: {struct Cyc_Dict_Absent_exn_struct*_tmp9A7=(struct Cyc_Dict_Absent_exn_struct*)_tmp9A6;if(_tmp9A7->tag != Cyc_Dict_Absent)goto _LL5E7;}_LL5E6:
# 3739
{const char*_tmp1155;void*_tmp1154[1];struct Cyc_String_pa_PrintArg_struct _tmp1153;(_tmp1153.tag=0,((_tmp1153.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp99D)),((_tmp1154[0]=& _tmp1153,Cyc_Tcutil_terr(loc,((_tmp1155="unbound datatype %s",_tag_dyneither(_tmp1155,sizeof(char),20))),_tag_dyneither(_tmp1154,sizeof(void*),1)))))));}
return cvtenv;_LL5E7: _tmp9A8=_tmp9A6;_LL5E8:(void)_throw(_tmp9A8);_LL5E4:;}};}
# 3742
{struct _handler_cons _tmp9AC;_push_handler(& _tmp9AC);{int _tmp9AE=0;if(setjmp(_tmp9AC.handler))_tmp9AE=1;if(!_tmp9AE){
{struct _RegionHandle _tmp9AF=_new_region("r");struct _RegionHandle*r=& _tmp9AF;_push_region(r);
{void*_tmp9B0=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp99E,0);struct Cyc_Absyn_Datatypedecl*_tmp9B2;struct Cyc_Absyn_Datatypefield*_tmp9B3;_LL5EA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp9B1=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp9B0;if(_tmp9B1->tag != 2)goto _LL5EC;else{_tmp9B2=_tmp9B1->f1;_tmp9B3=_tmp9B1->f2;}}_LL5EB:
# 3746
 tuf=_tmp9B3;
tud=_tmp9B2;
if(_tmp99F  && !tud->is_extensible){
const char*_tmp1159;void*_tmp1158[1];struct Cyc_String_pa_PrintArg_struct _tmp1157;(_tmp1157.tag=0,((_tmp1157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp99D)),((_tmp1158[0]=& _tmp1157,Cyc_Tcutil_terr(loc,((_tmp1159="datatype %s was not declared @extensible",_tag_dyneither(_tmp1159,sizeof(char),41))),_tag_dyneither(_tmp1158,sizeof(void*),1)))))));}
goto _LL5E9;_LL5EC:;_LL5ED:
{const char*_tmp115E;void*_tmp115D[2];struct Cyc_String_pa_PrintArg_struct _tmp115C;struct Cyc_String_pa_PrintArg_struct _tmp115B;(_tmp115B.tag=0,((_tmp115B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp99D)),((_tmp115C.tag=0,((_tmp115C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp99E)),((_tmp115D[0]=& _tmp115C,((_tmp115D[1]=& _tmp115B,Cyc_Tcutil_terr(loc,((_tmp115E="unbound field %s in type datatype %s",_tag_dyneither(_tmp115E,sizeof(char),37))),_tag_dyneither(_tmp115D,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmp9BB=cvtenv;_npop_handler(1);return _tmp9BB;};_LL5E9:;}
# 3744
;_pop_region(r);}
# 3743
;_pop_handler();}else{void*_tmp9AD=(void*)_exn_thrown;void*_tmp9BD=_tmp9AD;void*_tmp9BF;_LL5EF: {struct Cyc_Dict_Absent_exn_struct*_tmp9BE=(struct Cyc_Dict_Absent_exn_struct*)_tmp9BD;if(_tmp9BE->tag != Cyc_Dict_Absent)goto _LL5F1;}_LL5F0:
# 3759
{const char*_tmp1163;void*_tmp1162[2];struct Cyc_String_pa_PrintArg_struct _tmp1161;struct Cyc_String_pa_PrintArg_struct _tmp1160;(_tmp1160.tag=0,((_tmp1160.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp99D)),((_tmp1161.tag=0,((_tmp1161.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp99E)),((_tmp1162[0]=& _tmp1161,((_tmp1162[1]=& _tmp1160,Cyc_Tcutil_terr(loc,((_tmp1163="unbound field %s in type datatype %s",_tag_dyneither(_tmp1163,sizeof(char),37))),_tag_dyneither(_tmp1162,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F1: _tmp9BF=_tmp9BD;_LL5F2:(void)_throw(_tmp9BF);_LL5EE:;}};}
# 3763
*_tmp913=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp9A0=tud;_tmp9A1=tuf;goto _LL5E3;}_LL5E2: if((_tmp99C.KnownDatatypefield).tag != 2)goto _LL5DF;_tmp9A0=((struct _tuple3)(_tmp99C.KnownDatatypefield).val).f1;_tmp9A1=((struct _tuple3)(_tmp99C.KnownDatatypefield).val).f2;_LL5E3: {
# 3767
struct Cyc_List_List*tvs=_tmp9A0->tvs;
for(0;_tmp914 != 0  && tvs != 0;(_tmp914=_tmp914->tl,tvs=tvs->tl)){
void*t=(void*)_tmp914->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3773
{struct _tuple0 _tmp1164;struct _tuple0 _tmp9C5=(_tmp1164.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1164.f2=t,_tmp1164)));struct Cyc_Absyn_Tvar*_tmp9C8;_LL5F4:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9C6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9C5.f1;if(_tmp9C6->tag != 1)goto _LL5F6;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9C7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9C5.f2;if(_tmp9C7->tag != 2)goto _LL5F6;else{_tmp9C8=_tmp9C7->f1;}};_LL5F5:
# 3775
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9C8);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9C8,1);
continue;_LL5F6:;_LL5F7:
 goto _LL5F3;_LL5F3:;}{
# 3780
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3784
if(_tmp914 != 0){
const char*_tmp1169;void*_tmp1168[2];struct Cyc_String_pa_PrintArg_struct _tmp1167;struct Cyc_String_pa_PrintArg_struct _tmp1166;(_tmp1166.tag=0,((_tmp1166.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9A1->name)),((_tmp1167.tag=0,((_tmp1167.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A0->name)),((_tmp1168[0]=& _tmp1167,((_tmp1168[1]=& _tmp1166,Cyc_Tcutil_terr(loc,((_tmp1169="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1169,sizeof(char),43))),_tag_dyneither(_tmp1168,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp116E;void*_tmp116D[2];struct Cyc_String_pa_PrintArg_struct _tmp116C;struct Cyc_String_pa_PrintArg_struct _tmp116B;(_tmp116B.tag=0,((_tmp116B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9A1->name)),((_tmp116C.tag=0,((_tmp116C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A0->name)),((_tmp116D[0]=& _tmp116C,((_tmp116D[1]=& _tmp116B,Cyc_Tcutil_terr(loc,((_tmp116E="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp116E,sizeof(char),42))),_tag_dyneither(_tmp116D,sizeof(void*),2)))))))))))));}
goto _LL5DF;}_LL5DF:;}
# 3792
goto _LL57E;_LL58F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp915=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp900;if(_tmp915->tag != 5)goto _LL591;else{_tmp916=(_tmp915->f1).elt_typ;_tmp917=(struct Cyc_Absyn_Tqual*)&(_tmp915->f1).elt_tq;_tmp918=((_tmp915->f1).ptr_atts).rgn;_tmp919=((_tmp915->f1).ptr_atts).nullable;_tmp91A=((_tmp915->f1).ptr_atts).bounds;_tmp91B=((_tmp915->f1).ptr_atts).zero_term;}}_LL590: {
# 3795
int is_zero_terminated;
# 3797
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp916,1);
_tmp917->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp917->print_const,_tmp916);{
struct Cyc_Absyn_Kind*k;
switch(expected_kind->aliasqual){case Cyc_Absyn_Aliasable: _LL5F8:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL5F9:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5FA:
 k=& Cyc_Tcutil_trk;break;}
# 3805
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp918,1);
{union Cyc_Absyn_Constraint*_tmp9D1=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp91B);_LL5FD: if((_tmp9D1->No_constr).tag != 3)goto _LL5FF;_LL5FE:
# 3810
{void*_tmp9D2=Cyc_Tcutil_compress(_tmp916);_LL604: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9D3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9D2;if(_tmp9D3->tag != 6)goto _LL606;else{if(_tmp9D3->f2 != Cyc_Absyn_Char_sz)goto _LL606;}}_LL605:
# 3812
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp91B,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL603;_LL606:;_LL607:
# 3816
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp91B,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL603;_LL603:;}
# 3820
goto _LL5FC;_LL5FF: if((_tmp9D1->Eq_constr).tag != 1)goto _LL601;if((int)(_tmp9D1->Eq_constr).val != 1)goto _LL601;_LL600:
# 3823
 if(!Cyc_Tcutil_admits_zero(_tmp916)){
const char*_tmp1172;void*_tmp1171[1];struct Cyc_String_pa_PrintArg_struct _tmp1170;(_tmp1170.tag=0,((_tmp1170.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp916)),((_tmp1171[0]=& _tmp1170,Cyc_Tcutil_terr(loc,((_tmp1172="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp1172,sizeof(char),53))),_tag_dyneither(_tmp1171,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL5FC;_LL601:;_LL602:
# 3829
 is_zero_terminated=0;
goto _LL5FC;_LL5FC:;}
# 3833
{void*_tmp9D7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp91A);struct Cyc_Absyn_Exp*_tmp9DA;_LL609: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp9D8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9D7;if(_tmp9D8->tag != 0)goto _LL60B;}_LL60A:
 goto _LL608;_LL60B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9D9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9D7;if(_tmp9D9->tag != 1)goto _LL608;else{_tmp9DA=_tmp9D9->f1;}}_LL60C: {
# 3836
struct _RegionHandle _tmp9DB=_new_region("temp");struct _RegionHandle*temp=& _tmp9DB;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9DC=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9DC,0,_tmp9DA);}
# 3840
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9DA,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9DA)){
const char*_tmp1175;void*_tmp1174;(_tmp1174=0,Cyc_Tcutil_terr(loc,((_tmp1175="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp1175,sizeof(char),49))),_tag_dyneither(_tmp1174,sizeof(void*),0)));}{
unsigned int _tmp9E0;int _tmp9E1;struct _tuple14 _tmp9DF=Cyc_Evexp_eval_const_uint_exp(_tmp9DA);_tmp9E0=_tmp9DF.f1;_tmp9E1=_tmp9DF.f2;
if(is_zero_terminated  && (!_tmp9E1  || _tmp9E0 < 1)){
const char*_tmp1178;void*_tmp1177;(_tmp1177=0,Cyc_Tcutil_terr(loc,((_tmp1178="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp1178,sizeof(char),55))),_tag_dyneither(_tmp1177,sizeof(void*),0)));}
_npop_handler(0);goto _LL608;};
# 3836
;_pop_region(temp);}_LL608:;}
# 3848
goto _LL57E;};}_LL591: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp91C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp900;if(_tmp91C->tag != 19)goto _LL593;else{_tmp91D=(void*)_tmp91C->f1;}}_LL592:
# 3850
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp91D,1);goto _LL57E;_LL593: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp91E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp900;if(_tmp91E->tag != 18)goto _LL595;else{_tmp91F=_tmp91E->f1;}}_LL594: {
# 3855
struct _RegionHandle _tmp9E4=_new_region("temp");struct _RegionHandle*temp=& _tmp9E4;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9E5=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9E5,0,_tmp91F);}
# 3859
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp91F)){
const char*_tmp117B;void*_tmp117A;(_tmp117A=0,Cyc_Tcutil_terr(loc,((_tmp117B="valueof_t requires an int expression",_tag_dyneither(_tmp117B,sizeof(char),37))),_tag_dyneither(_tmp117A,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp91F,te,cvtenv);
_npop_handler(0);goto _LL57E;
# 3855
;_pop_region(temp);}_LL595: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp920=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp900;if(_tmp920->tag != 6)goto _LL597;}_LL596:
# 3863
 goto _LL598;_LL597: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp921=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp900;if(_tmp921->tag != 7)goto _LL599;}_LL598:
 goto _LL57E;_LL599: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp922=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp900;if(_tmp922->tag != 8)goto _LL59B;else{_tmp923=(_tmp922->f1).elt_type;_tmp924=(struct Cyc_Absyn_Tqual*)&(_tmp922->f1).tq;_tmp925=(struct Cyc_Absyn_Exp**)&(_tmp922->f1).num_elts;_tmp926=(_tmp922->f1).zero_term;_tmp927=(_tmp922->f1).zt_loc;}}_LL59A: {
# 3868
struct Cyc_Absyn_Exp*_tmp9E8=*_tmp925;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp923,1);
_tmp924->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp924->print_const,_tmp923);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp9E9=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp926);_LL60E: if((_tmp9E9->No_constr).tag != 3)goto _LL610;_LL60F:
# 3875
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp926,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 3890 "tcutil.cyc"
goto _LL60D;_LL610: if((_tmp9E9->Eq_constr).tag != 1)goto _LL612;if((int)(_tmp9E9->Eq_constr).val != 1)goto _LL612;_LL611:
# 3893
 if(!Cyc_Tcutil_admits_zero(_tmp923)){
const char*_tmp117F;void*_tmp117E[1];struct Cyc_String_pa_PrintArg_struct _tmp117D;(_tmp117D.tag=0,((_tmp117D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp923)),((_tmp117E[0]=& _tmp117D,Cyc_Tcutil_terr(loc,((_tmp117F="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp117F,sizeof(char),51))),_tag_dyneither(_tmp117E,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL60D;_LL612:;_LL613:
# 3899
 is_zero_terminated=0;
goto _LL60D;_LL60D:;}
# 3904
if(_tmp9E8 == 0){
# 3906
if(is_zero_terminated)
# 3908
*_tmp925=(_tmp9E8=Cyc_Absyn_uint_exp(1,0));else{
# 3911
{const char*_tmp1182;void*_tmp1181;(_tmp1181=0,Cyc_Tcutil_warn(loc,((_tmp1182="array bound defaults to 1 here",_tag_dyneither(_tmp1182,sizeof(char),31))),_tag_dyneither(_tmp1181,sizeof(void*),0)));}
*_tmp925=(_tmp9E8=Cyc_Absyn_uint_exp(1,0));}}{
# 3915
struct _RegionHandle _tmp9EF=_new_region("temp");struct _RegionHandle*temp=& _tmp9EF;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9F0=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9F0,0,_tmp9E8);}
# 3919
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9E8)){
const char*_tmp1185;void*_tmp1184;(_tmp1184=0,Cyc_Tcutil_terr(loc,((_tmp1185="array bounds expression is not an unsigned int",_tag_dyneither(_tmp1185,sizeof(char),47))),_tag_dyneither(_tmp1184,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9E8,te,cvtenv);{
# 3926
unsigned int _tmp9F4;int _tmp9F5;struct _tuple14 _tmp9F3=Cyc_Evexp_eval_const_uint_exp(_tmp9E8);_tmp9F4=_tmp9F3.f1;_tmp9F5=_tmp9F3.f2;
# 3928
if((is_zero_terminated  && _tmp9F5) && _tmp9F4 < 1){
const char*_tmp1188;void*_tmp1187;(_tmp1187=0,Cyc_Tcutil_warn(loc,((_tmp1188="zero terminated array cannot have zero elements",_tag_dyneither(_tmp1188,sizeof(char),48))),_tag_dyneither(_tmp1187,sizeof(void*),0)));}
# 3931
if((_tmp9F5  && _tmp9F4 < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp118B;void*_tmp118A;(_tmp118A=0,Cyc_Tcutil_warn(loc,((_tmp118B="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp118B,sizeof(char),75))),_tag_dyneither(_tmp118A,sizeof(void*),0)));}
*_tmp925=Cyc_Absyn_uint_exp(1,0);}
# 3935
_npop_handler(0);goto _LL57E;};
# 3915
;_pop_region(temp);};};}_LL59B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp928=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp900;if(_tmp928->tag != 9)goto _LL59D;else{_tmp929=(struct Cyc_List_List**)&(_tmp928->f1).tvars;_tmp92A=(void**)&(_tmp928->f1).effect;_tmp92B=(struct Cyc_Absyn_Tqual*)&(_tmp928->f1).ret_tqual;_tmp92C=(_tmp928->f1).ret_typ;_tmp92D=(_tmp928->f1).args;_tmp92E=(_tmp928->f1).c_varargs;_tmp92F=(_tmp928->f1).cyc_varargs;_tmp930=(_tmp928->f1).rgn_po;_tmp931=(_tmp928->f1).attributes;}}_LL59C: {
# 3942
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=- 1;
int fmt_arg_start=- 1;
for(0;_tmp931 != 0;_tmp931=_tmp931->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp931->hd)){
const char*_tmp118F;void*_tmp118E[1];struct Cyc_String_pa_PrintArg_struct _tmp118D;(_tmp118D.tag=0,((_tmp118D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp931->hd)),((_tmp118E[0]=& _tmp118D,Cyc_Tcutil_terr(loc,((_tmp118F="bad function type attribute %s",_tag_dyneither(_tmp118F,sizeof(char),31))),_tag_dyneither(_tmp118E,sizeof(void*),1)))))));}{
void*_tmp9FD=(void*)_tmp931->hd;enum Cyc_Absyn_Format_Type _tmpA02;int _tmpA03;int _tmpA04;_LL615: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp9FE=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp9FD;if(_tmp9FE->tag != 1)goto _LL617;}_LL616:
# 3955
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL614;_LL617: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp9FF=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp9FD;if(_tmp9FF->tag != 2)goto _LL619;}_LL618:
# 3957
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL614;_LL619: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA00=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp9FD;if(_tmpA00->tag != 3)goto _LL61B;}_LL61A:
# 3959
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL614;_LL61B: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA01=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp9FD;if(_tmpA01->tag != 19)goto _LL61D;else{_tmpA02=_tmpA01->f1;_tmpA03=_tmpA01->f2;_tmpA04=_tmpA01->f3;}}_LL61C:
# 3961
 if(!seen_format){
seen_format=1;
ft=_tmpA02;
fmt_desc_arg=_tmpA03;
fmt_arg_start=_tmpA04;}else{
# 3967
const char*_tmp1192;void*_tmp1191;(_tmp1191=0,Cyc_Tcutil_terr(loc,((_tmp1192="function can't have multiple format attributes",_tag_dyneither(_tmp1192,sizeof(char),47))),_tag_dyneither(_tmp1191,sizeof(void*),0)));}
goto _LL614;_LL61D:;_LL61E:
 goto _LL614;_LL614:;};}
# 3972
if(num_convs > 1){
const char*_tmp1195;void*_tmp1194;(_tmp1194=0,Cyc_Tcutil_terr(loc,((_tmp1195="function can't have multiple calling conventions",_tag_dyneither(_tmp1195,sizeof(char),49))),_tag_dyneither(_tmp1194,sizeof(void*),0)));}
# 3977
Cyc_Tcutil_check_unique_tvars(loc,*_tmp929);
{struct Cyc_List_List*b=*_tmp929;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpA09=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);_LL620: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA0A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA09;if(_tmpA0A->tag != 0)goto _LL622;else{if((_tmpA0A->f1)->kind != Cyc_Absyn_MemKind)goto _LL622;}}_LL621:
# 3983
{const char*_tmp1199;void*_tmp1198[1];struct Cyc_String_pa_PrintArg_struct _tmp1197;(_tmp1197.tag=0,((_tmp1197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp1198[0]=& _tmp1197,Cyc_Tcutil_terr(loc,((_tmp1199="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp1199,sizeof(char),51))),_tag_dyneither(_tmp1198,sizeof(void*),1)))))));}
goto _LL61F;_LL622:;_LL623:
 goto _LL61F;_LL61F:;};}}{
# 3991
struct _RegionHandle _tmpA0E=_new_region("newr");struct _RegionHandle*newr=& _tmpA0E;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp119A;struct Cyc_Tcutil_CVTEnv _tmpA0F=
(_tmp119A.r=newr,((_tmp119A.kind_env=cvtenv.kind_env,((_tmp119A.free_vars=0,((_tmp119A.free_evars=0,((_tmp119A.generalize_evars=cvtenv.generalize_evars,((_tmp119A.fn_result=1,_tmp119A)))))))))));
# 3997
_tmpA0F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA0F,& Cyc_Tcutil_tmk,_tmp92C,1);
_tmp92B->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp92B->print_const,_tmp92C);
_tmpA0F.fn_result=0;
{struct Cyc_List_List*a=_tmp92D;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpA10=(struct _tuple10*)a->hd;
void*_tmpA11=(*_tmpA10).f3;
_tmpA0F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA0F,& Cyc_Tcutil_tmk,_tmpA11,1);{
int _tmpA12=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpA10).f2).print_const,_tmpA11);
((*_tmpA10).f2).real_const=_tmpA12;{
# 4008
void*_tmpA13=Cyc_Tcutil_compress(_tmpA11);void*_tmpA15;struct Cyc_Absyn_Tqual _tmpA16;struct Cyc_Absyn_Exp*_tmpA17;union Cyc_Absyn_Constraint*_tmpA18;unsigned int _tmpA19;_LL625: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA14=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA13;if(_tmpA14->tag != 8)goto _LL627;else{_tmpA15=(_tmpA14->f1).elt_type;_tmpA16=(_tmpA14->f1).tq;_tmpA17=(_tmpA14->f1).num_elts;_tmpA18=(_tmpA14->f1).zero_term;_tmpA19=(_tmpA14->f1).zt_loc;}}_LL626: {
# 4011
void*_tmpA1A=Cyc_Absyn_new_evar(0,0);
_tmpA0F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA0F,& Cyc_Tcutil_rk,_tmpA1A,1);{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp119D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp119C;void*_tmpA1B=Cyc_Absyn_atb_typ(_tmpA15,_tmpA1A,_tmpA16,(void*)(
(_tmp119C=_cycalloc(sizeof(*_tmp119C)),((_tmp119C[0]=((_tmp119D.tag=1,((_tmp119D.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA17),_tmp119D)))),_tmp119C)))),_tmpA18);
(*_tmpA10).f3=_tmpA1B;
goto _LL624;};}_LL627:;_LL628:
 goto _LL624;_LL624:;};};}}
# 4022
if(_tmp92F != 0){
if(_tmp92E){const char*_tmp11A0;void*_tmp119F;(_tmp119F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A0="both c_vararg and cyc_vararg",_tag_dyneither(_tmp11A0,sizeof(char),29))),_tag_dyneither(_tmp119F,sizeof(void*),0)));}{
void*_tmpA21;int _tmpA22;struct Cyc_Absyn_VarargInfo _tmpA20=*_tmp92F;_tmpA21=_tmpA20.type;_tmpA22=_tmpA20.inject;
_tmpA0F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA0F,& Cyc_Tcutil_tmk,_tmpA21,1);
(_tmp92F->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp92F->tq).print_const,_tmpA21);
# 4028
if(_tmpA22){
void*_tmpA23=Cyc_Tcutil_compress(_tmpA21);void*_tmpA25;union Cyc_Absyn_Constraint*_tmpA26;union Cyc_Absyn_Constraint*_tmpA27;_LL62A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA24=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA23;if(_tmpA24->tag != 5)goto _LL62C;else{_tmpA25=(_tmpA24->f1).elt_typ;_tmpA26=((_tmpA24->f1).ptr_atts).bounds;_tmpA27=((_tmpA24->f1).ptr_atts).zero_term;}}_LL62B:
# 4031
{void*_tmpA28=Cyc_Tcutil_compress(_tmpA25);_LL62F: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA29=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA28;if(_tmpA29->tag != 3)goto _LL631;}_LL630:
# 4033
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpA27)){
const char*_tmp11A3;void*_tmp11A2;(_tmp11A2=0,Cyc_Tcutil_terr(loc,((_tmp11A3="can't inject into a zeroterm pointer",_tag_dyneither(_tmp11A3,sizeof(char),37))),_tag_dyneither(_tmp11A2,sizeof(void*),0)));}
{void*_tmpA2C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpA26);_LL634: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA2D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA2C;if(_tmpA2D->tag != 0)goto _LL636;}_LL635:
# 4037
{const char*_tmp11A6;void*_tmp11A5;(_tmp11A5=0,Cyc_Tcutil_terr(loc,((_tmp11A6="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp11A6,sizeof(char),44))),_tag_dyneither(_tmp11A5,sizeof(void*),0)));}
goto _LL633;_LL636:;_LL637:
 goto _LL633;_LL633:;}
# 4041
goto _LL62E;_LL631:;_LL632:
{const char*_tmp11A9;void*_tmp11A8;(_tmp11A8=0,Cyc_Tcutil_terr(loc,((_tmp11A9="can't inject a non-datatype type",_tag_dyneither(_tmp11A9,sizeof(char),33))),_tag_dyneither(_tmp11A8,sizeof(void*),0)));}goto _LL62E;_LL62E:;}
# 4044
goto _LL629;_LL62C:;_LL62D:
{const char*_tmp11AC;void*_tmp11AB;(_tmp11AB=0,Cyc_Tcutil_terr(loc,((_tmp11AC="expecting a datatype pointer type",_tag_dyneither(_tmp11AC,sizeof(char),34))),_tag_dyneither(_tmp11AB,sizeof(void*),0)));}goto _LL629;_LL629:;}};}
# 4050
if(seen_format){
int _tmpA34=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp92D);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpA34) || fmt_arg_start < 0) || 
# 4054
_tmp92F == 0  && fmt_arg_start != 0) || 
_tmp92F != 0  && fmt_arg_start != _tmpA34 + 1){
const char*_tmp11AF;void*_tmp11AE;(_tmp11AE=0,Cyc_Tcutil_terr(loc,((_tmp11AF="bad format descriptor",_tag_dyneither(_tmp11AF,sizeof(char),22))),_tag_dyneither(_tmp11AE,sizeof(void*),0)));}else{
# 4059
void*_tmpA38;struct _tuple10 _tmpA37=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp92D,fmt_desc_arg - 1);_tmpA38=_tmpA37.f3;
# 4061
{void*_tmpA39=Cyc_Tcutil_compress(_tmpA38);void*_tmpA3B;union Cyc_Absyn_Constraint*_tmpA3C;union Cyc_Absyn_Constraint*_tmpA3D;_LL639: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA3A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA39;if(_tmpA3A->tag != 5)goto _LL63B;else{_tmpA3B=(_tmpA3A->f1).elt_typ;_tmpA3C=((_tmpA3A->f1).ptr_atts).bounds;_tmpA3D=((_tmpA3A->f1).ptr_atts).zero_term;}}_LL63A:
# 4064
{struct _tuple0 _tmp11B0;struct _tuple0 _tmpA3F=(_tmp11B0.f1=Cyc_Tcutil_compress(_tmpA3B),((_tmp11B0.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA3C),_tmp11B0)));_LL63E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA40=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA3F.f1;if(_tmpA40->tag != 6)goto _LL640;else{if(_tmpA40->f1 != Cyc_Absyn_None)goto _LL640;if(_tmpA40->f2 != Cyc_Absyn_Char_sz)goto _LL640;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA41=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA3F.f2;if(_tmpA41->tag != 0)goto _LL640;};_LL63F:
 goto _LL63D;_LL640:;_LL641:
{const char*_tmp11B3;void*_tmp11B2;(_tmp11B2=0,Cyc_Tcutil_terr(loc,((_tmp11B3="format descriptor is not a char ? type",_tag_dyneither(_tmp11B3,sizeof(char),39))),_tag_dyneither(_tmp11B2,sizeof(void*),0)));}goto _LL63D;_LL63D:;}
# 4068
goto _LL638;_LL63B:;_LL63C:
{const char*_tmp11B6;void*_tmp11B5;(_tmp11B5=0,Cyc_Tcutil_terr(loc,((_tmp11B6="format descriptor is not a char ? type",_tag_dyneither(_tmp11B6,sizeof(char),39))),_tag_dyneither(_tmp11B5,sizeof(void*),0)));}goto _LL638;_LL638:;}
# 4071
if(fmt_arg_start != 0){
# 4075
int problem;
{struct _tuple30 _tmp11B7;struct _tuple30 _tmpA47=(_tmp11B7.f1=ft,((_tmp11B7.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp92F))->type)),_tmp11B7)));struct Cyc_Absyn_Datatypedecl*_tmpA49;struct Cyc_Absyn_Datatypedecl*_tmpA4B;_LL643: if(_tmpA47.f1 != Cyc_Absyn_Printf_ft)goto _LL645;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA48=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA47.f2;if(_tmpA48->tag != 3)goto _LL645;else{if((((_tmpA48->f1).datatype_info).KnownDatatype).tag != 2)goto _LL645;_tmpA49=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA48->f1).datatype_info).KnownDatatype).val);}};_LL644:
# 4078
 problem=Cyc_Absyn_qvar_cmp(_tmpA49->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL642;_LL645: if(_tmpA47.f1 != Cyc_Absyn_Scanf_ft)goto _LL647;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA4A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA47.f2;if(_tmpA4A->tag != 3)goto _LL647;else{if((((_tmpA4A->f1).datatype_info).KnownDatatype).tag != 2)goto _LL647;_tmpA4B=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA4A->f1).datatype_info).KnownDatatype).val);}};_LL646:
# 4080
 problem=Cyc_Absyn_qvar_cmp(_tmpA4B->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL642;_LL647:;_LL648:
# 4082
 problem=1;goto _LL642;_LL642:;}
# 4084
if(problem){
const char*_tmp11BA;void*_tmp11B9;(_tmp11B9=0,Cyc_Tcutil_terr(loc,((_tmp11BA="format attribute and vararg types don't match",_tag_dyneither(_tmp11BA,sizeof(char),46))),_tag_dyneither(_tmp11B9,sizeof(void*),0)));}}}}
# 4092
{struct Cyc_List_List*rpo=_tmp930;for(0;rpo != 0;rpo=rpo->tl){
void*_tmpA4F;void*_tmpA50;struct _tuple0*_tmpA4E=(struct _tuple0*)rpo->hd;_tmpA4F=_tmpA4E->f1;_tmpA50=_tmpA4E->f2;
_tmpA0F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA0F,& Cyc_Tcutil_ek,_tmpA4F,1);
_tmpA0F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA0F,& Cyc_Tcutil_trk,_tmpA50,1);}}
# 4098
if(*_tmp92A != 0)
_tmpA0F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA0F,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp92A),1);else{
# 4101
struct Cyc_List_List*effect=0;
# 4106
{struct Cyc_List_List*tvs=_tmpA0F.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpA52;int _tmpA53;struct _tuple28 _tmpA51=*((struct _tuple28*)tvs->hd);_tmpA52=_tmpA51.f1;_tmpA53=_tmpA51.f2;
if(!_tmpA53)continue;{
void*_tmpA54=Cyc_Absyn_compress_kb(_tmpA52->kind);struct Cyc_Core_Opt**_tmpA56;struct Cyc_Absyn_Kind*_tmpA57;struct Cyc_Absyn_Kind*_tmpA59;struct Cyc_Core_Opt**_tmpA5D;struct Cyc_Core_Opt**_tmpA60;_LL64A:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA55=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA54;if(_tmpA55->tag != 2)goto _LL64C;else{_tmpA56=(struct Cyc_Core_Opt**)& _tmpA55->f1;_tmpA57=_tmpA55->f2;}}if(!(_tmpA57->kind == Cyc_Absyn_RgnKind))goto _LL64C;_LL64B:
# 4112
 if(_tmpA57->aliasqual == Cyc_Absyn_Top){
*_tmpA56=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpA59=_tmpA57;goto _LL64D;}
# 4115
*_tmpA56=Cyc_Tcutil_kind_to_bound_opt(_tmpA57);_tmpA59=_tmpA57;goto _LL64D;_LL64C:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA58=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA54;if(_tmpA58->tag != 0)goto _LL64E;else{_tmpA59=_tmpA58->f1;}}if(!(_tmpA59->kind == Cyc_Absyn_RgnKind))goto _LL64E;_LL64D:
# 4117
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11C9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11C8;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11C7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11C6;struct Cyc_List_List*_tmp11C5;effect=((_tmp11C5=_cycalloc(sizeof(*_tmp11C5)),((_tmp11C5->hd=(void*)((_tmp11C9=_cycalloc(sizeof(*_tmp11C9)),((_tmp11C9[0]=((_tmp11C6.tag=23,((_tmp11C6.f1=(void*)((_tmp11C8=_cycalloc(sizeof(*_tmp11C8)),((_tmp11C8[0]=((_tmp11C7.tag=2,((_tmp11C7.f1=_tmpA52,_tmp11C7)))),_tmp11C8)))),_tmp11C6)))),_tmp11C9)))),((_tmp11C5->tl=effect,_tmp11C5))))));}goto _LL649;_LL64E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA5A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA54;if(_tmpA5A->tag != 2)goto _LL650;else{if((_tmpA5A->f2)->kind != Cyc_Absyn_IntKind)goto _LL650;}}_LL64F:
 goto _LL651;_LL650: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA5B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA54;if(_tmpA5B->tag != 0)goto _LL652;else{if((_tmpA5B->f1)->kind != Cyc_Absyn_IntKind)goto _LL652;}}_LL651:
 goto _LL649;_LL652: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA5C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA54;if(_tmpA5C->tag != 2)goto _LL654;else{_tmpA5D=(struct Cyc_Core_Opt**)& _tmpA5C->f1;if((_tmpA5C->f2)->kind != Cyc_Absyn_EffKind)goto _LL654;}}_LL653:
# 4121
*_tmpA5D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL655;_LL654: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA5E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA54;if(_tmpA5E->tag != 0)goto _LL656;else{if((_tmpA5E->f1)->kind != Cyc_Absyn_EffKind)goto _LL656;}}_LL655:
# 4123
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11CF;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11CE;struct Cyc_List_List*_tmp11CD;effect=((_tmp11CD=_cycalloc(sizeof(*_tmp11CD)),((_tmp11CD->hd=(void*)((_tmp11CF=_cycalloc(sizeof(*_tmp11CF)),((_tmp11CF[0]=((_tmp11CE.tag=2,((_tmp11CE.f1=_tmpA52,_tmp11CE)))),_tmp11CF)))),((_tmp11CD->tl=effect,_tmp11CD))))));}goto _LL649;_LL656: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA5F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA54;if(_tmpA5F->tag != 1)goto _LL658;else{_tmpA60=(struct Cyc_Core_Opt**)& _tmpA5F->f1;}}_LL657:
# 4125
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11D5;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp11D4;struct Cyc_Core_Opt*_tmp11D3;*_tmpA60=((_tmp11D3=_cycalloc(sizeof(*_tmp11D3)),((_tmp11D3->v=(void*)((_tmp11D5=_cycalloc(sizeof(*_tmp11D5)),((_tmp11D5[0]=((_tmp11D4.tag=2,((_tmp11D4.f1=0,((_tmp11D4.f2=& Cyc_Tcutil_ak,_tmp11D4)))))),_tmp11D5)))),_tmp11D3))));}goto _LL659;_LL658:;_LL659:
# 4128
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11E4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11E3;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11E2;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11E1;struct Cyc_List_List*_tmp11E0;effect=((_tmp11E0=_cycalloc(sizeof(*_tmp11E0)),((_tmp11E0->hd=(void*)((_tmp11E4=_cycalloc(sizeof(*_tmp11E4)),((_tmp11E4[0]=((_tmp11E1.tag=25,((_tmp11E1.f1=(void*)((_tmp11E3=_cycalloc(sizeof(*_tmp11E3)),((_tmp11E3[0]=((_tmp11E2.tag=2,((_tmp11E2.f1=_tmpA52,_tmp11E2)))),_tmp11E3)))),_tmp11E1)))),_tmp11E4)))),((_tmp11E0->tl=effect,_tmp11E0))))));}goto _LL649;_LL649:;};}}
# 4132
{struct Cyc_List_List*ts=_tmpA0F.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmpA72;int _tmpA73;struct _tuple29 _tmpA71=*((struct _tuple29*)ts->hd);_tmpA72=_tmpA71.f1;_tmpA73=_tmpA71.f2;
if(!_tmpA73)continue;{
struct Cyc_Absyn_Kind*_tmpA74=Cyc_Tcutil_typ_kind(_tmpA72);_LL65B: if(_tmpA74->kind != Cyc_Absyn_RgnKind)goto _LL65D;_LL65C:
# 4137
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11EA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11E9;struct Cyc_List_List*_tmp11E8;effect=((_tmp11E8=_cycalloc(sizeof(*_tmp11E8)),((_tmp11E8->hd=(void*)((_tmp11EA=_cycalloc(sizeof(*_tmp11EA)),((_tmp11EA[0]=((_tmp11E9.tag=23,((_tmp11E9.f1=_tmpA72,_tmp11E9)))),_tmp11EA)))),((_tmp11E8->tl=effect,_tmp11E8))))));}goto _LL65A;_LL65D: if(_tmpA74->kind != Cyc_Absyn_EffKind)goto _LL65F;_LL65E:
# 4139
{struct Cyc_List_List*_tmp11EB;effect=((_tmp11EB=_cycalloc(sizeof(*_tmp11EB)),((_tmp11EB->hd=_tmpA72,((_tmp11EB->tl=effect,_tmp11EB))))));}goto _LL65A;_LL65F: if(_tmpA74->kind != Cyc_Absyn_IntKind)goto _LL661;_LL660:
 goto _LL65A;_LL661:;_LL662:
# 4142
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11F1;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11F0;struct Cyc_List_List*_tmp11EF;effect=((_tmp11EF=_cycalloc(sizeof(*_tmp11EF)),((_tmp11EF->hd=(void*)((_tmp11F1=_cycalloc(sizeof(*_tmp11F1)),((_tmp11F1[0]=((_tmp11F0.tag=25,((_tmp11F0.f1=_tmpA72,_tmp11F0)))),_tmp11F1)))),((_tmp11EF->tl=effect,_tmp11EF))))));}goto _LL65A;_LL65A:;};}}{
# 4145
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11F4;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11F3;*_tmp92A=(void*)((_tmp11F3=_cycalloc(sizeof(*_tmp11F3)),((_tmp11F3[0]=((_tmp11F4.tag=24,((_tmp11F4.f1=effect,_tmp11F4)))),_tmp11F3))));};}
# 4151
if(*_tmp929 != 0){
struct Cyc_List_List*bs=*_tmp929;for(0;bs != 0;bs=bs->tl){
void*_tmpA7E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt**_tmpA80;struct Cyc_Core_Opt**_tmpA82;struct Cyc_Core_Opt**_tmpA84;struct Cyc_Core_Opt**_tmpA86;struct Cyc_Core_Opt**_tmpA88;struct Cyc_Core_Opt**_tmpA8A;struct Cyc_Core_Opt**_tmpA8C;struct Cyc_Core_Opt**_tmpA8E;struct Cyc_Core_Opt**_tmpA90;struct Cyc_Core_Opt**_tmpA92;struct Cyc_Absyn_Kind*_tmpA93;_LL664: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA7F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA7F->tag != 1)goto _LL666;else{_tmpA80=(struct Cyc_Core_Opt**)& _tmpA7F->f1;}}_LL665:
# 4155
{const char*_tmp11F8;void*_tmp11F7[1];struct Cyc_String_pa_PrintArg_struct _tmp11F6;(_tmp11F6.tag=0,((_tmp11F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp11F7[0]=& _tmp11F6,Cyc_Tcutil_warn(loc,((_tmp11F8="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp11F8,sizeof(char),47))),_tag_dyneither(_tmp11F7,sizeof(void*),1)))))));}
# 4157
_tmpA82=_tmpA80;goto _LL667;_LL666: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA81=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA81->tag != 2)goto _LL668;else{_tmpA82=(struct Cyc_Core_Opt**)& _tmpA81->f1;if((_tmpA81->f2)->kind != Cyc_Absyn_BoxKind)goto _LL668;if((_tmpA81->f2)->aliasqual != Cyc_Absyn_Top)goto _LL668;}}_LL667:
 _tmpA84=_tmpA82;goto _LL669;_LL668: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA83=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA83->tag != 2)goto _LL66A;else{_tmpA84=(struct Cyc_Core_Opt**)& _tmpA83->f1;if((_tmpA83->f2)->kind != Cyc_Absyn_MemKind)goto _LL66A;if((_tmpA83->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66A;}}_LL669:
 _tmpA86=_tmpA84;goto _LL66B;_LL66A: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA85=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA85->tag != 2)goto _LL66C;else{_tmpA86=(struct Cyc_Core_Opt**)& _tmpA85->f1;if((_tmpA85->f2)->kind != Cyc_Absyn_MemKind)goto _LL66C;if((_tmpA85->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL66C;}}_LL66B:
 _tmpA88=_tmpA86;goto _LL66D;_LL66C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA87=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA87->tag != 2)goto _LL66E;else{_tmpA88=(struct Cyc_Core_Opt**)& _tmpA87->f1;if((_tmpA87->f2)->kind != Cyc_Absyn_AnyKind)goto _LL66E;if((_tmpA87->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66E;}}_LL66D:
 _tmpA8A=_tmpA88;goto _LL66F;_LL66E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA89=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA89->tag != 2)goto _LL670;else{_tmpA8A=(struct Cyc_Core_Opt**)& _tmpA89->f1;if((_tmpA89->f2)->kind != Cyc_Absyn_AnyKind)goto _LL670;if((_tmpA89->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL670;}}_LL66F:
# 4163
*_tmpA8A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL663;_LL670: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA8B->tag != 2)goto _LL672;else{_tmpA8C=(struct Cyc_Core_Opt**)& _tmpA8B->f1;if((_tmpA8B->f2)->kind != Cyc_Absyn_MemKind)goto _LL672;if((_tmpA8B->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL672;}}_LL671:
 _tmpA8E=_tmpA8C;goto _LL673;_LL672: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA8D->tag != 2)goto _LL674;else{_tmpA8E=(struct Cyc_Core_Opt**)& _tmpA8D->f1;if((_tmpA8D->f2)->kind != Cyc_Absyn_AnyKind)goto _LL674;if((_tmpA8D->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL674;}}_LL673:
# 4166
*_tmpA8E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL663;_LL674: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA8F->tag != 2)goto _LL676;else{_tmpA90=(struct Cyc_Core_Opt**)& _tmpA8F->f1;if((_tmpA8F->f2)->kind != Cyc_Absyn_RgnKind)goto _LL676;if((_tmpA8F->f2)->aliasqual != Cyc_Absyn_Top)goto _LL676;}}_LL675:
# 4168
*_tmpA90=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL663;_LL676: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA91=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA91->tag != 2)goto _LL678;else{_tmpA92=(struct Cyc_Core_Opt**)& _tmpA91->f1;_tmpA93=_tmpA91->f2;}}_LL677:
# 4170
*_tmpA92=Cyc_Tcutil_kind_to_bound_opt(_tmpA93);goto _LL663;_LL678: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA94=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7E;if(_tmpA94->tag != 0)goto _LL67A;else{if((_tmpA94->f1)->kind != Cyc_Absyn_MemKind)goto _LL67A;}}_LL679:
# 4172
{const char*_tmp11FB;void*_tmp11FA;(_tmp11FA=0,Cyc_Tcutil_terr(loc,((_tmp11FB="functions can't abstract M types",_tag_dyneither(_tmp11FB,sizeof(char),33))),_tag_dyneither(_tmp11FA,sizeof(void*),0)));}goto _LL663;_LL67A:;_LL67B:
 goto _LL663;_LL663:;}}
# 4177
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpA0F.kind_env,*_tmp929);
_tmpA0F.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpA0F.r,_tmpA0F.free_vars,*_tmp929);
# 4180
{struct Cyc_List_List*tvs=_tmpA0F.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpA9B;int _tmpA9C;struct _tuple28 _tmpA9A=*((struct _tuple28*)tvs->hd);_tmpA9B=_tmpA9A.f1;_tmpA9C=_tmpA9A.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA9B,_tmpA9C);}}{
# 4185
struct Cyc_List_List*evs=_tmpA0F.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpA9E;int _tmpA9F;struct _tuple29 _tmpA9D=*((struct _tuple29*)evs->hd);_tmpA9E=_tmpA9D.f1;_tmpA9F=_tmpA9D.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpA9E,_tmpA9F);}};}
# 4190
_npop_handler(0);goto _LL57E;
# 3991
;_pop_region(newr);};}_LL59D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp932=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp900;if(_tmp932->tag != 10)goto _LL59F;else{_tmp933=_tmp932->f1;}}_LL59E:
# 4193
 for(0;_tmp933 != 0;_tmp933=_tmp933->tl){
struct _tuple12*_tmpAA1=(struct _tuple12*)_tmp933->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpAA1).f2,1);
((*_tmpAA1).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAA1).f1).print_const,(*_tmpAA1).f2);}
# 4199
goto _LL57E;_LL59F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp934=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp900;if(_tmp934->tag != 12)goto _LL5A1;else{_tmp935=_tmp934->f1;_tmp936=_tmp934->f2;}}_LL5A0: {
# 4203
struct _RegionHandle _tmpAA2=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpAA2;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp936 != 0;_tmp936=_tmp936->tl){
struct _dyneither_ptr*_tmpAA4;struct Cyc_Absyn_Tqual*_tmpAA5;void*_tmpAA6;struct Cyc_Absyn_Exp*_tmpAA7;struct Cyc_List_List*_tmpAA8;struct Cyc_Absyn_Aggrfield*_tmpAA3=(struct Cyc_Absyn_Aggrfield*)_tmp936->hd;_tmpAA4=_tmpAA3->name;_tmpAA5=(struct Cyc_Absyn_Tqual*)& _tmpAA3->tq;_tmpAA6=_tmpAA3->type;_tmpAA7=_tmpAA3->width;_tmpAA8=_tmpAA3->attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpAA4)){
const char*_tmp11FF;void*_tmp11FE[1];struct Cyc_String_pa_PrintArg_struct _tmp11FD;(_tmp11FD.tag=0,((_tmp11FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAA4),((_tmp11FE[0]=& _tmp11FD,Cyc_Tcutil_terr(loc,((_tmp11FF="duplicate field %s",_tag_dyneither(_tmp11FF,sizeof(char),19))),_tag_dyneither(_tmp11FE,sizeof(void*),1)))))));}
{const char*_tmp1200;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpAA4,((_tmp1200="",_tag_dyneither(_tmp1200,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp1201;prev_fields=((_tmp1201=_region_malloc(aprev_rgn,sizeof(*_tmp1201)),((_tmp1201->hd=_tmpAA4,((_tmp1201->tl=prev_fields,_tmp1201))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpAA6,1);
_tmpAA5->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpAA5->print_const,_tmpAA6);
if(_tmp935 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpAA6)){
# 4215
const char*_tmp1205;void*_tmp1204[1];struct Cyc_String_pa_PrintArg_struct _tmp1203;(_tmp1203.tag=0,((_tmp1203.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAA4),((_tmp1204[0]=& _tmp1203,Cyc_Tcutil_warn(loc,((_tmp1205="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp1205,sizeof(char),74))),_tag_dyneither(_tmp1204,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpAA6,_tmpAA7,_tmpAA4);
Cyc_Tcutil_check_field_atts(loc,_tmpAA4,_tmpAA8);}}
# 4220
_npop_handler(0);goto _LL57E;
# 4203
;_pop_region(aprev_rgn);}_LL5A1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp937=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp900;if(_tmp937->tag != 11)goto _LL5A3;else{_tmp938=(union Cyc_Absyn_AggrInfoU*)&(_tmp937->f1).aggr_info;_tmp939=(struct Cyc_List_List**)&(_tmp937->f1).targs;}}_LL5A2:
# 4223
{union Cyc_Absyn_AggrInfoU _tmpAB1=*_tmp938;enum Cyc_Absyn_AggrKind _tmpAB2;struct _tuple2*_tmpAB3;struct Cyc_Core_Opt*_tmpAB4;struct Cyc_Absyn_Aggrdecl*_tmpAB5;_LL67D: if((_tmpAB1.UnknownAggr).tag != 1)goto _LL67F;_tmpAB2=((struct _tuple4)(_tmpAB1.UnknownAggr).val).f1;_tmpAB3=((struct _tuple4)(_tmpAB1.UnknownAggr).val).f2;_tmpAB4=((struct _tuple4)(_tmpAB1.UnknownAggr).val).f3;_LL67E: {
# 4225
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpAB6;_push_handler(& _tmpAB6);{int _tmpAB8=0;if(setjmp(_tmpAB6.handler))_tmpAB8=1;if(!_tmpAB8){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAB3);{
struct Cyc_Absyn_Aggrdecl*_tmpAB9=*adp;
if(_tmpAB9->kind != _tmpAB2){
if(_tmpAB9->kind == Cyc_Absyn_StructA){
const char*_tmp120A;void*_tmp1209[2];struct Cyc_String_pa_PrintArg_struct _tmp1208;struct Cyc_String_pa_PrintArg_struct _tmp1207;(_tmp1207.tag=0,((_tmp1207.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAB3)),((_tmp1208.tag=0,((_tmp1208.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4231
Cyc_Absynpp_qvar2string(_tmpAB3)),((_tmp1209[0]=& _tmp1208,((_tmp1209[1]=& _tmp1207,Cyc_Tcutil_terr(loc,((_tmp120A="expecting struct %s instead of union %s",_tag_dyneither(_tmp120A,sizeof(char),40))),_tag_dyneither(_tmp1209,sizeof(void*),2)))))))))))));}else{
# 4234
const char*_tmp120F;void*_tmp120E[2];struct Cyc_String_pa_PrintArg_struct _tmp120D;struct Cyc_String_pa_PrintArg_struct _tmp120C;(_tmp120C.tag=0,((_tmp120C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAB3)),((_tmp120D.tag=0,((_tmp120D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4234
Cyc_Absynpp_qvar2string(_tmpAB3)),((_tmp120E[0]=& _tmp120D,((_tmp120E[1]=& _tmp120C,Cyc_Tcutil_terr(loc,((_tmp120F="expecting union %s instead of struct %s",_tag_dyneither(_tmp120F,sizeof(char),40))),_tag_dyneither(_tmp120E,sizeof(void*),2)))))))))))));}}
# 4237
if((unsigned int)_tmpAB4  && (int)_tmpAB4->v){
if(!((unsigned int)_tmpAB9->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAB9->impl))->tagged){
const char*_tmp1213;void*_tmp1212[1];struct Cyc_String_pa_PrintArg_struct _tmp1211;(_tmp1211.tag=0,((_tmp1211.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAB3)),((_tmp1212[0]=& _tmp1211,Cyc_Tcutil_terr(loc,((_tmp1213="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp1213,sizeof(char),42))),_tag_dyneither(_tmp1212,sizeof(void*),1)))))));}}
# 4243
*_tmp938=Cyc_Absyn_KnownAggr(adp);};
# 4227
;_pop_handler();}else{void*_tmpAB7=(void*)_exn_thrown;void*_tmpAC6=_tmpAB7;void*_tmpAC8;_LL682: {struct Cyc_Dict_Absent_exn_struct*_tmpAC7=(struct Cyc_Dict_Absent_exn_struct*)_tmpAC6;if(_tmpAC7->tag != Cyc_Dict_Absent)goto _LL684;}_LL683: {
# 4247
struct Cyc_Tcenv_Genv*_tmpAC9=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1214;struct Cyc_Absyn_Aggrdecl*_tmpACA=(_tmp1214=_cycalloc(sizeof(*_tmp1214)),((_tmp1214->kind=_tmpAB2,((_tmp1214->sc=Cyc_Absyn_Extern,((_tmp1214->name=_tmpAB3,((_tmp1214->tvs=0,((_tmp1214->impl=0,((_tmp1214->attributes=0,_tmp1214)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpAC9,loc,_tmpACA);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAB3);
*_tmp938=Cyc_Absyn_KnownAggr(adp);
# 4253
if(*_tmp939 != 0){
{const char*_tmp1218;void*_tmp1217[1];struct Cyc_String_pa_PrintArg_struct _tmp1216;(_tmp1216.tag=0,((_tmp1216.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAB3)),((_tmp1217[0]=& _tmp1216,Cyc_Tcutil_terr(loc,((_tmp1218="declare parameterized type %s before using",_tag_dyneither(_tmp1218,sizeof(char),43))),_tag_dyneither(_tmp1217,sizeof(void*),1)))))));}
return cvtenv;}
# 4258
goto _LL681;}_LL684: _tmpAC8=_tmpAC6;_LL685:(void)_throw(_tmpAC8);_LL681:;}};}
# 4260
_tmpAB5=*adp;goto _LL680;}_LL67F: if((_tmpAB1.KnownAggr).tag != 2)goto _LL67C;_tmpAB5=*((struct Cyc_Absyn_Aggrdecl**)(_tmpAB1.KnownAggr).val);_LL680: {
# 4262
struct Cyc_List_List*tvs=_tmpAB5->tvs;
struct Cyc_List_List*ts=*_tmp939;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpACF=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpAD0=(void*)ts->hd;
# 4270
{struct _tuple0 _tmp1219;struct _tuple0 _tmpAD2=(_tmp1219.f1=Cyc_Absyn_compress_kb(_tmpACF->kind),((_tmp1219.f2=_tmpAD0,_tmp1219)));struct Cyc_Absyn_Tvar*_tmpAD5;_LL687:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAD3=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAD2.f1;if(_tmpAD3->tag != 1)goto _LL689;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAD4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAD2.f2;if(_tmpAD4->tag != 2)goto _LL689;else{_tmpAD5=_tmpAD4->f1;}};_LL688:
# 4272
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAD5);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAD5,1);
continue;_LL689:;_LL68A:
 goto _LL686;_LL686:;}{
# 4277
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4281
if(ts != 0){
const char*_tmp121D;void*_tmp121C[1];struct Cyc_String_pa_PrintArg_struct _tmp121B;(_tmp121B.tag=0,((_tmp121B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAB5->name)),((_tmp121C[0]=& _tmp121B,Cyc_Tcutil_terr(loc,((_tmp121D="too many parameters for type %s",_tag_dyneither(_tmp121D,sizeof(char),32))),_tag_dyneither(_tmp121C,sizeof(void*),1)))))));}
if(tvs != 0){
# 4285
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp121E;hidden_ts=((_tmp121E=_cycalloc(sizeof(*_tmp121E)),((_tmp121E->hd=e,((_tmp121E->tl=hidden_ts,_tmp121E))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4292
*_tmp939=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp939,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL67C:;}
# 4295
goto _LL57E;_LL5A3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp93A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp900;if(_tmp93A->tag != 17)goto _LL5A5;else{_tmp93B=_tmp93A->f1;_tmp93C=(struct Cyc_List_List**)& _tmp93A->f2;_tmp93D=(struct Cyc_Absyn_Typedefdecl**)& _tmp93A->f3;_tmp93E=(void**)((void**)& _tmp93A->f4);}}_LL5A4: {
# 4298
struct Cyc_List_List*targs=*_tmp93C;
# 4300
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpADA;_push_handler(& _tmpADA);{int _tmpADC=0;if(setjmp(_tmpADA.handler))_tmpADC=1;if(!_tmpADC){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp93B);;_pop_handler();}else{void*_tmpADB=(void*)_exn_thrown;void*_tmpADE=_tmpADB;void*_tmpAE0;_LL68C: {struct Cyc_Dict_Absent_exn_struct*_tmpADF=(struct Cyc_Dict_Absent_exn_struct*)_tmpADE;if(_tmpADF->tag != Cyc_Dict_Absent)goto _LL68E;}_LL68D:
# 4303
{const char*_tmp1222;void*_tmp1221[1];struct Cyc_String_pa_PrintArg_struct _tmp1220;(_tmp1220.tag=0,((_tmp1220.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp93B)),((_tmp1221[0]=& _tmp1220,Cyc_Tcutil_terr(loc,((_tmp1222="unbound typedef name %s",_tag_dyneither(_tmp1222,sizeof(char),24))),_tag_dyneither(_tmp1221,sizeof(void*),1)))))));}
return cvtenv;_LL68E: _tmpAE0=_tmpADE;_LL68F:(void)_throw(_tmpAE0);_LL68B:;}};}
# 4306
*_tmp93D=td;
# 4308
_tmp93B[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpAE4=_new_region("temp");struct _RegionHandle*temp=& _tmpAE4;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4314
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4319
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1225;struct Cyc_List_List*_tmp1224;inst=((_tmp1224=_region_malloc(temp,sizeof(*_tmp1224)),((_tmp1224->hd=((_tmp1225=_region_malloc(temp,sizeof(*_tmp1225)),((_tmp1225->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1225->f2=(void*)ts->hd,_tmp1225)))))),((_tmp1224->tl=inst,_tmp1224))))));};}
# 4323
if(ts != 0){
const char*_tmp1229;void*_tmp1228[1];struct Cyc_String_pa_PrintArg_struct _tmp1227;(_tmp1227.tag=0,((_tmp1227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp93B)),((_tmp1228[0]=& _tmp1227,Cyc_Tcutil_terr(loc,((_tmp1229="too many parameters for typedef %s",_tag_dyneither(_tmp1229,sizeof(char),35))),_tag_dyneither(_tmp1228,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4328
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp122A;hidden_ts=((_tmp122A=_cycalloc(sizeof(*_tmp122A)),((_tmp122A->hd=e,((_tmp122A->tl=hidden_ts,_tmp122A))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp122D;struct Cyc_List_List*_tmp122C;inst=((_tmp122C=_cycalloc(sizeof(*_tmp122C)),((_tmp122C->hd=((_tmp122D=_cycalloc(sizeof(*_tmp122D)),((_tmp122D->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp122D->f2=e,_tmp122D)))))),((_tmp122C->tl=inst,_tmp122C))))));};}
# 4336
*_tmp93C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4338
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp93E=new_typ;}}
# 4345
_npop_handler(0);goto _LL57E;
# 4311
;_pop_region(temp);};}_LL5A5: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp93F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp900;if(_tmp93F->tag != 22)goto _LL5A7;}_LL5A6:
# 4346
 goto _LL5A8;_LL5A7: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp940=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp900;if(_tmp940->tag != 21)goto _LL5A9;}_LL5A8:
 goto _LL5AA;_LL5A9: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp941=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp900;if(_tmp941->tag != 20)goto _LL5AB;}_LL5AA:
 goto _LL57E;_LL5AB: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp942=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp900;if(_tmp942->tag != 15)goto _LL5AD;else{_tmp943=(void*)_tmp942->f1;}}_LL5AC:
# 4350
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp943,1);goto _LL57E;_LL5AD: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp944=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp900;if(_tmp944->tag != 16)goto _LL5AF;else{_tmp945=(void*)_tmp944->f1;_tmp946=(void*)_tmp944->f2;}}_LL5AE:
# 4353
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp945,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp946,1);
goto _LL57E;_LL5AF: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp947=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp900;if(_tmp947->tag != 23)goto _LL5B1;else{_tmp948=(void*)_tmp947->f1;}}_LL5B0:
# 4357
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp948,1);goto _LL57E;_LL5B1: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp949=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp900;if(_tmp949->tag != 25)goto _LL5B3;else{_tmp94A=(void*)_tmp949->f1;}}_LL5B2:
# 4359
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp94A,1);goto _LL57E;_LL5B3: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp94B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp900;if(_tmp94B->tag != 24)goto _LL57E;else{_tmp94C=_tmp94B->f1;}}_LL5B4:
# 4361
 for(0;_tmp94C != 0;_tmp94C=_tmp94C->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp94C->hd,1);}
goto _LL57E;_LL57E:;}
# 4365
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpAED=t;struct Cyc_Core_Opt*_tmpAEF;void*_tmpAF0;_LL691: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAEE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAED;if(_tmpAEE->tag != 1)goto _LL693;else{_tmpAEF=_tmpAEE->f1;_tmpAF0=(void*)_tmpAEE->f2;}}_LL692: {
# 4368
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpAF1=_tmpAEF;struct Cyc_Absyn_Kind*_tmpAF2;_LL696: if(_tmpAF1 != 0)goto _LL698;_LL697:
{const char*_tmp122E;s=((_tmp122E="kind=NULL ",_tag_dyneither(_tmp122E,sizeof(char),11)));}goto _LL695;_LL698: if(_tmpAF1 == 0)goto _LL695;_tmpAF2=(struct Cyc_Absyn_Kind*)_tmpAF1->v;_LL699:
{const char*_tmp1232;void*_tmp1231[1];struct Cyc_String_pa_PrintArg_struct _tmp1230;s=(struct _dyneither_ptr)((_tmp1230.tag=0,((_tmp1230.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpAF2)),((_tmp1231[0]=& _tmp1230,Cyc_aprintf(((_tmp1232="kind=%s ",_tag_dyneither(_tmp1232,sizeof(char),9))),_tag_dyneither(_tmp1231,sizeof(void*),1))))))));}goto _LL695;_LL695:;}
# 4373
if(_tmpAF0 == 0){
const char*_tmp1236;void*_tmp1235[1];struct Cyc_String_pa_PrintArg_struct _tmp1234;s=(struct _dyneither_ptr)((_tmp1234.tag=0,((_tmp1234.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1235[0]=& _tmp1234,Cyc_aprintf(((_tmp1236="%s ref=NULL",_tag_dyneither(_tmp1236,sizeof(char),12))),_tag_dyneither(_tmp1235,sizeof(void*),1))))))));}else{
# 4376
const char*_tmp123B;void*_tmp123A[2];struct Cyc_String_pa_PrintArg_struct _tmp1239;struct Cyc_String_pa_PrintArg_struct _tmp1238;s=(struct _dyneither_ptr)((_tmp1238.tag=0,((_tmp1238.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAF0)),((_tmp1239.tag=0,((_tmp1239.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp123A[0]=& _tmp1239,((_tmp123A[1]=& _tmp1238,Cyc_aprintf(((_tmp123B="%s ref=%s",_tag_dyneither(_tmp123B,sizeof(char),10))),_tag_dyneither(_tmp123A,sizeof(void*),2))))))))))))));}
# 4378
goto _LL690;}_LL693:;_LL694:
 goto _LL690;_LL690:;}{
# 4381
const char*_tmp1241;void*_tmp1240[3];struct Cyc_String_pa_PrintArg_struct _tmp123F;struct Cyc_String_pa_PrintArg_struct _tmp123E;struct Cyc_String_pa_PrintArg_struct _tmp123D;(_tmp123D.tag=0,((_tmp123D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp123E.tag=0,((_tmp123E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp123F.tag=0,((_tmp123F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1240[0]=& _tmp123F,((_tmp1240[1]=& _tmp123E,((_tmp1240[2]=& _tmp123D,Cyc_Tcutil_terr(loc,((_tmp1241="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1241,sizeof(char),51))),_tag_dyneither(_tmp1240,sizeof(void*),3)))))))))))))))))));};}
# 4384
return cvtenv;}
# 4392
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4394
{void*_tmpB03=e->r;struct Cyc_List_List*_tmpB09;struct Cyc_Absyn_Exp*_tmpB0B;struct Cyc_Absyn_Exp*_tmpB0C;struct Cyc_Absyn_Exp*_tmpB0D;struct Cyc_Absyn_Exp*_tmpB0F;struct Cyc_Absyn_Exp*_tmpB10;struct Cyc_Absyn_Exp*_tmpB12;struct Cyc_Absyn_Exp*_tmpB13;struct Cyc_Absyn_Exp*_tmpB15;struct Cyc_Absyn_Exp*_tmpB16;void*_tmpB18;struct Cyc_Absyn_Exp*_tmpB19;void*_tmpB1B;void*_tmpB1D;void*_tmpB1F;struct Cyc_Absyn_Exp*_tmpB21;_LL69B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpB04=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB04->tag != 0)goto _LL69D;}_LL69C:
 goto _LL69E;_LL69D: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB05=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB05->tag != 30)goto _LL69F;}_LL69E:
 goto _LL6A0;_LL69F: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB06=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB06->tag != 31)goto _LL6A1;}_LL6A0:
 goto _LL6A2;_LL6A1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpB07=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB07->tag != 1)goto _LL6A3;}_LL6A2:
 goto _LL69A;_LL6A3: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpB08=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB08->tag != 2)goto _LL6A5;else{_tmpB09=_tmpB08->f2;}}_LL6A4:
# 4400
 for(0;_tmpB09 != 0;_tmpB09=_tmpB09->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpB09->hd,te,cvtenv);}
goto _LL69A;_LL6A5: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB0A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB0A->tag != 5)goto _LL6A7;else{_tmpB0B=_tmpB0A->f1;_tmpB0C=_tmpB0A->f2;_tmpB0D=_tmpB0A->f3;}}_LL6A6:
# 4404
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB0B,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB0C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB0D,te,cvtenv);
goto _LL69A;_LL6A7: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpB0E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB0E->tag != 6)goto _LL6A9;else{_tmpB0F=_tmpB0E->f1;_tmpB10=_tmpB0E->f2;}}_LL6A8:
 _tmpB12=_tmpB0F;_tmpB13=_tmpB10;goto _LL6AA;_LL6A9: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpB11=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB11->tag != 7)goto _LL6AB;else{_tmpB12=_tmpB11->f1;_tmpB13=_tmpB11->f2;}}_LL6AA:
 _tmpB15=_tmpB12;_tmpB16=_tmpB13;goto _LL6AC;_LL6AB: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpB14=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB14->tag != 8)goto _LL6AD;else{_tmpB15=_tmpB14->f1;_tmpB16=_tmpB14->f2;}}_LL6AC:
# 4411
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB15,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB16,te,cvtenv);
goto _LL69A;_LL6AD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpB17=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB17->tag != 13)goto _LL6AF;else{_tmpB18=(void*)_tmpB17->f1;_tmpB19=_tmpB17->f2;}}_LL6AE:
# 4415
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB19,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB18,1);
goto _LL69A;_LL6AF: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB1A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB1A->tag != 18)goto _LL6B1;else{_tmpB1B=(void*)_tmpB1A->f1;}}_LL6B0:
 _tmpB1D=_tmpB1B;goto _LL6B2;_LL6B1: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB1C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB1C->tag != 16)goto _LL6B3;else{_tmpB1D=(void*)_tmpB1C->f1;}}_LL6B2:
# 4420
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB1D,1);
goto _LL69A;_LL6B3: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpB1E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB1E->tag != 37)goto _LL6B5;else{_tmpB1F=(void*)_tmpB1E->f1;}}_LL6B4:
# 4423
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB1F,1);
goto _LL69A;_LL6B5: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpB20=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB03;if(_tmpB20->tag != 17)goto _LL6B7;else{_tmpB21=_tmpB20->f1;}}_LL6B6:
# 4426
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB21,te,cvtenv);
goto _LL69A;_LL6B7:;_LL6B8: {
# 4429
const char*_tmp1244;void*_tmp1243;(_tmp1243=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1244="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1244,sizeof(char),66))),_tag_dyneither(_tmp1243,sizeof(void*),0)));}_LL69A:;}
# 4431
return cvtenv;}
# 4434
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4439
struct Cyc_List_List*_tmpB24=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4442
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct Cyc_Absyn_Tvar*_tmpB26;struct _tuple28 _tmpB25=*((struct _tuple28*)vs->hd);_tmpB26=_tmpB25.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpB24,_tmpB26);}}
# 4450
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpB28;struct _tuple29 _tmpB27=*((struct _tuple29*)evs->hd);_tmpB28=_tmpB27.f1;{
void*_tmpB29=Cyc_Tcutil_compress(_tmpB28);struct Cyc_Core_Opt**_tmpB2B;_LL6BA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB2A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB29;if(_tmpB2A->tag != 1)goto _LL6BC;else{_tmpB2B=(struct Cyc_Core_Opt**)& _tmpB2A->f4;}}_LL6BB:
# 4454
 if(*_tmpB2B == 0){
struct Cyc_Core_Opt*_tmp1245;*_tmpB2B=((_tmp1245=_cycalloc(sizeof(*_tmp1245)),((_tmp1245->v=_tmpB24,_tmp1245))));}else{
# 4458
struct Cyc_List_List*_tmpB2D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpB2B))->v;
struct Cyc_List_List*_tmpB2E=0;
for(0;_tmpB2D != 0;_tmpB2D=_tmpB2D->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpB24,(struct Cyc_Absyn_Tvar*)_tmpB2D->hd)){
struct Cyc_List_List*_tmp1246;_tmpB2E=((_tmp1246=_cycalloc(sizeof(*_tmp1246)),((_tmp1246->hd=(struct Cyc_Absyn_Tvar*)_tmpB2D->hd,((_tmp1246->tl=_tmpB2E,_tmp1246))))));}}{
struct Cyc_Core_Opt*_tmp1247;*_tmpB2B=((_tmp1247=_cycalloc(sizeof(*_tmp1247)),((_tmp1247->v=_tmpB2E,_tmp1247))));};}
# 4465
goto _LL6B9;_LL6BC:;_LL6BD:
 goto _LL6B9;_LL6B9:;};}}
# 4469
return cvt;}
# 4476
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpB31=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpB32=_new_region("temp");struct _RegionHandle*temp=& _tmpB32;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp1248;struct Cyc_Tcutil_CVTEnv _tmpB33=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1248.r=temp,((_tmp1248.kind_env=_tmpB31,((_tmp1248.free_vars=0,((_tmp1248.free_evars=0,((_tmp1248.generalize_evars=generalize_evars,((_tmp1248.fn_result=0,_tmp1248)))))))))))),& Cyc_Tcutil_tmk,t);
# 4484
struct Cyc_List_List*_tmpB34=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB33.free_vars);
struct Cyc_List_List*_tmpB35=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB33.free_evars);
# 4488
if(_tmpB31 != 0){
struct Cyc_List_List*_tmpB36=0;
{struct Cyc_List_List*_tmpB37=_tmpB34;for(0;(unsigned int)_tmpB37;_tmpB37=_tmpB37->tl){
struct Cyc_Absyn_Tvar*_tmpB38=(struct Cyc_Absyn_Tvar*)_tmpB37->hd;
int found=0;
{struct Cyc_List_List*_tmpB39=_tmpB31;for(0;(unsigned int)_tmpB39;_tmpB39=_tmpB39->tl){
if(Cyc_Absyn_tvar_cmp(_tmpB38,(struct Cyc_Absyn_Tvar*)_tmpB39->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp1249;_tmpB36=((_tmp1249=_region_malloc(temp,sizeof(*_tmp1249)),((_tmp1249->hd=(struct Cyc_Absyn_Tvar*)_tmpB37->hd,((_tmp1249->tl=_tmpB36,_tmp1249))))));}}}
# 4498
_tmpB34=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB36);}
# 4504
{struct Cyc_List_List*x=_tmpB34;for(0;x != 0;x=x->tl){
void*_tmpB3B=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt**_tmpB3D;struct Cyc_Core_Opt**_tmpB3F;struct Cyc_Core_Opt**_tmpB41;struct Cyc_Core_Opt**_tmpB43;struct Cyc_Core_Opt**_tmpB45;struct Cyc_Core_Opt**_tmpB47;struct Cyc_Core_Opt**_tmpB49;struct Cyc_Absyn_Kind*_tmpB4A;enum Cyc_Absyn_AliasQual _tmpB4C;_LL6BF: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB3C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB3B;if(_tmpB3C->tag != 1)goto _LL6C1;else{_tmpB3D=(struct Cyc_Core_Opt**)& _tmpB3C->f1;}}_LL6C0:
 _tmpB3F=_tmpB3D;goto _LL6C2;_LL6C1: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB3E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3B;if(_tmpB3E->tag != 2)goto _LL6C3;else{_tmpB3F=(struct Cyc_Core_Opt**)& _tmpB3E->f1;if((_tmpB3E->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6C3;if((_tmpB3E->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6C3;}}_LL6C2:
 _tmpB41=_tmpB3F;goto _LL6C4;_LL6C3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB40=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3B;if(_tmpB40->tag != 2)goto _LL6C5;else{_tmpB41=(struct Cyc_Core_Opt**)& _tmpB40->f1;if((_tmpB40->f2)->kind != Cyc_Absyn_MemKind)goto _LL6C5;if((_tmpB40->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6C5;}}_LL6C4:
 _tmpB43=_tmpB41;goto _LL6C6;_LL6C5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB42=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3B;if(_tmpB42->tag != 2)goto _LL6C7;else{_tmpB43=(struct Cyc_Core_Opt**)& _tmpB42->f1;if((_tmpB42->f2)->kind != Cyc_Absyn_MemKind)goto _LL6C7;if((_tmpB42->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6C7;}}_LL6C6:
# 4510
*_tmpB43=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6BE;_LL6C7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB44=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3B;if(_tmpB44->tag != 2)goto _LL6C9;else{_tmpB45=(struct Cyc_Core_Opt**)& _tmpB44->f1;if((_tmpB44->f2)->kind != Cyc_Absyn_MemKind)goto _LL6C9;if((_tmpB44->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6C9;}}_LL6C8:
# 4512
*_tmpB45=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6BE;_LL6C9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB46=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3B;if(_tmpB46->tag != 2)goto _LL6CB;else{_tmpB47=(struct Cyc_Core_Opt**)& _tmpB46->f1;if((_tmpB46->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6CB;if((_tmpB46->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CB;}}_LL6CA:
# 4514
*_tmpB47=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6BE;_LL6CB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB48=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3B;if(_tmpB48->tag != 2)goto _LL6CD;else{_tmpB49=(struct Cyc_Core_Opt**)& _tmpB48->f1;_tmpB4A=_tmpB48->f2;}}_LL6CC:
# 4516
*_tmpB49=Cyc_Tcutil_kind_to_bound_opt(_tmpB4A);goto _LL6BE;_LL6CD: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB4B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB3B;if(_tmpB4B->tag != 0)goto _LL6CF;else{if((_tmpB4B->f1)->kind != Cyc_Absyn_MemKind)goto _LL6CF;_tmpB4C=(_tmpB4B->f1)->aliasqual;}}_LL6CE:
# 4518
{const char*_tmp1250;void*_tmp124F[2];struct Cyc_String_pa_PrintArg_struct _tmp124E;struct Cyc_Absyn_Kind*_tmp124D;struct Cyc_String_pa_PrintArg_struct _tmp124C;(_tmp124C.tag=0,((_tmp124C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp124D=_cycalloc_atomic(sizeof(*_tmp124D)),((_tmp124D->kind=Cyc_Absyn_MemKind,((_tmp124D->aliasqual=_tmpB4C,_tmp124D)))))))),((_tmp124E.tag=0,((_tmp124E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp124F[0]=& _tmp124E,((_tmp124F[1]=& _tmp124C,Cyc_Tcutil_terr(loc,((_tmp1250="type variable %s cannot have kind %s",_tag_dyneither(_tmp1250,sizeof(char),37))),_tag_dyneither(_tmp124F,sizeof(void*),2)))))))))))));}
goto _LL6BE;_LL6CF:;_LL6D0:
 goto _LL6BE;_LL6BE:;}}
# 4525
if(_tmpB34 != 0  || _tmpB35 != 0){
{void*_tmpB52=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpB54;void*_tmpB55;struct Cyc_Absyn_Tqual _tmpB56;void*_tmpB57;struct Cyc_List_List*_tmpB58;int _tmpB59;struct Cyc_Absyn_VarargInfo*_tmpB5A;struct Cyc_List_List*_tmpB5B;struct Cyc_List_List*_tmpB5C;_LL6D2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB53=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB52;if(_tmpB53->tag != 9)goto _LL6D4;else{_tmpB54=(struct Cyc_List_List**)&(_tmpB53->f1).tvars;_tmpB55=(_tmpB53->f1).effect;_tmpB56=(_tmpB53->f1).ret_tqual;_tmpB57=(_tmpB53->f1).ret_typ;_tmpB58=(_tmpB53->f1).args;_tmpB59=(_tmpB53->f1).c_varargs;_tmpB5A=(_tmpB53->f1).cyc_varargs;_tmpB5B=(_tmpB53->f1).rgn_po;_tmpB5C=(_tmpB53->f1).attributes;}}_LL6D3:
# 4528
 if(*_tmpB54 == 0){
# 4530
*_tmpB54=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpB34);
_tmpB34=0;}
# 4533
goto _LL6D1;_LL6D4:;_LL6D5:
 goto _LL6D1;_LL6D1:;}
# 4536
if(_tmpB34 != 0){
const char*_tmp1254;void*_tmp1253[1];struct Cyc_String_pa_PrintArg_struct _tmp1252;(_tmp1252.tag=0,((_tmp1252.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpB34->hd)->name),((_tmp1253[0]=& _tmp1252,Cyc_Tcutil_terr(loc,((_tmp1254="unbound type variable %s",_tag_dyneither(_tmp1254,sizeof(char),25))),_tag_dyneither(_tmp1253,sizeof(void*),1)))))));}
# 4541
if(_tmpB35 != 0)
for(0;_tmpB35 != 0;_tmpB35=_tmpB35->tl){
void*e=(void*)_tmpB35->hd;
struct Cyc_Absyn_Kind*_tmpB60=Cyc_Tcutil_typ_kind(e);_LL6D7: if(_tmpB60->kind != Cyc_Absyn_RgnKind)goto _LL6D9;if(_tmpB60->aliasqual != Cyc_Absyn_Unique)goto _LL6D9;_LL6D8:
# 4546
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1257;void*_tmp1256;(_tmp1256=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1257="can't unify evar with unique region!",_tag_dyneither(_tmp1257,sizeof(char),37))),_tag_dyneither(_tmp1256,sizeof(void*),0)));}
goto _LL6D6;_LL6D9: if(_tmpB60->kind != Cyc_Absyn_RgnKind)goto _LL6DB;if(_tmpB60->aliasqual != Cyc_Absyn_Aliasable)goto _LL6DB;_LL6DA:
 goto _LL6DC;_LL6DB: if(_tmpB60->kind != Cyc_Absyn_RgnKind)goto _LL6DD;if(_tmpB60->aliasqual != Cyc_Absyn_Top)goto _LL6DD;_LL6DC:
# 4551
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp125A;void*_tmp1259;(_tmp1259=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp125A="can't unify evar with heap!",_tag_dyneither(_tmp125A,sizeof(char),28))),_tag_dyneither(_tmp1259,sizeof(void*),0)));}
goto _LL6D6;_LL6DD: if(_tmpB60->kind != Cyc_Absyn_EffKind)goto _LL6DF;_LL6DE:
# 4554
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp125D;void*_tmp125C;(_tmp125C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp125D="can't unify evar with {}!",_tag_dyneither(_tmp125D,sizeof(char),26))),_tag_dyneither(_tmp125C,sizeof(void*),0)));}
goto _LL6D6;_LL6DF:;_LL6E0:
# 4557
{const char*_tmp1262;void*_tmp1261[2];struct Cyc_String_pa_PrintArg_struct _tmp1260;struct Cyc_String_pa_PrintArg_struct _tmp125F;(_tmp125F.tag=0,((_tmp125F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1260.tag=0,((_tmp1260.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1261[0]=& _tmp1260,((_tmp1261[1]=& _tmp125F,Cyc_Tcutil_terr(loc,((_tmp1262="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1262,sizeof(char),52))),_tag_dyneither(_tmp1261,sizeof(void*),2)))))))))))));}
goto _LL6D6;_LL6D6:;}}}
# 4480
;_pop_region(temp);}
# 4570
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4573
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpB6C=Cyc_Tcutil_compress(t);struct Cyc_List_List*_tmpB6E;void*_tmpB6F;struct Cyc_Absyn_Tqual _tmpB70;void*_tmpB71;struct Cyc_List_List*_tmpB72;_LL6E2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB6D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB6C;if(_tmpB6D->tag != 9)goto _LL6E4;else{_tmpB6E=(_tmpB6D->f1).tvars;_tmpB6F=(_tmpB6D->f1).effect;_tmpB70=(_tmpB6D->f1).ret_tqual;_tmpB71=(_tmpB6D->f1).ret_typ;_tmpB72=(_tmpB6D->f1).args;}}_LL6E3:
# 4576
 fd->tvs=_tmpB6E;
fd->effect=_tmpB6F;
{struct Cyc_List_List*_tmpB73=fd->args;for(0;_tmpB73 != 0;(_tmpB73=_tmpB73->tl,_tmpB72=_tmpB72->tl)){
# 4580
(*((struct _tuple10*)_tmpB73->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpB72))->hd)).f2;
(*((struct _tuple10*)_tmpB73->hd)).f3=(*((struct _tuple10*)_tmpB72->hd)).f3;}}
# 4583
fd->ret_tqual=_tmpB70;
fd->ret_type=_tmpB71;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpB71);
goto _LL6E1;_LL6E4:;_LL6E5: {
const char*_tmp1265;void*_tmp1264;(_tmp1264=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1265="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1265,sizeof(char),38))),_tag_dyneither(_tmp1264,sizeof(void*),0)));}_LL6E1:;}{
# 4590
struct _RegionHandle _tmpB76=_new_region("r");struct _RegionHandle*r=& _tmpB76;_push_region(r);{
const char*_tmp1266;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp1266="function declaration has repeated parameter",_tag_dyneither(_tmp1266,sizeof(char),44))));}
# 4595
fd->cached_typ=t;
# 4590
;_pop_region(r);};}
# 4600
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4603
struct _RegionHandle _tmpB78=_new_region("r");struct _RegionHandle*r=& _tmpB78;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp1267;struct Cyc_Tcutil_CVTEnv _tmpB79=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1267.r=r,((_tmp1267.kind_env=bound_tvars,((_tmp1267.free_vars=0,((_tmp1267.free_evars=0,((_tmp1267.generalize_evars=0,((_tmp1267.fn_result=0,_tmp1267)))))))))))),expected_kind,t);
# 4607
struct Cyc_List_List*_tmpB7A=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB79.free_vars),bound_tvars);
# 4609
struct Cyc_List_List*_tmpB7B=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB79.free_evars);
{struct Cyc_List_List*fs=_tmpB7A;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpB7C=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp126C;void*_tmp126B[2];struct Cyc_String_pa_PrintArg_struct _tmp126A;struct Cyc_String_pa_PrintArg_struct _tmp1269;(_tmp1269.tag=0,((_tmp1269.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp126A.tag=0,((_tmp126A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB7C),((_tmp126B[0]=& _tmp126A,((_tmp126B[1]=& _tmp1269,Cyc_Tcutil_terr(loc,((_tmp126C="unbound type variable %s in type %s",_tag_dyneither(_tmp126C,sizeof(char),36))),_tag_dyneither(_tmp126B,sizeof(void*),2)))))))))))));}}
# 4614
if(!allow_evars  && _tmpB7B != 0)
for(0;_tmpB7B != 0;_tmpB7B=_tmpB7B->tl){
void*e=(void*)_tmpB7B->hd;
struct Cyc_Absyn_Kind*_tmpB81=Cyc_Tcutil_typ_kind(e);_LL6E7: if(_tmpB81->kind != Cyc_Absyn_RgnKind)goto _LL6E9;if(_tmpB81->aliasqual != Cyc_Absyn_Unique)goto _LL6E9;_LL6E8:
# 4619
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp126F;void*_tmp126E;(_tmp126E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp126F="can't unify evar with unique region!",_tag_dyneither(_tmp126F,sizeof(char),37))),_tag_dyneither(_tmp126E,sizeof(void*),0)));}
goto _LL6E6;_LL6E9: if(_tmpB81->kind != Cyc_Absyn_RgnKind)goto _LL6EB;if(_tmpB81->aliasqual != Cyc_Absyn_Aliasable)goto _LL6EB;_LL6EA:
 goto _LL6EC;_LL6EB: if(_tmpB81->kind != Cyc_Absyn_RgnKind)goto _LL6ED;if(_tmpB81->aliasqual != Cyc_Absyn_Top)goto _LL6ED;_LL6EC:
# 4624
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1272;void*_tmp1271;(_tmp1271=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1272="can't unify evar with heap!",_tag_dyneither(_tmp1272,sizeof(char),28))),_tag_dyneither(_tmp1271,sizeof(void*),0)));}
goto _LL6E6;_LL6ED: if(_tmpB81->kind != Cyc_Absyn_EffKind)goto _LL6EF;_LL6EE:
# 4627
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1275;void*_tmp1274;(_tmp1274=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1275="can't unify evar with {}!",_tag_dyneither(_tmp1275,sizeof(char),26))),_tag_dyneither(_tmp1274,sizeof(void*),0)));}
goto _LL6E6;_LL6EF:;_LL6F0:
# 4630
{const char*_tmp127A;void*_tmp1279[2];struct Cyc_String_pa_PrintArg_struct _tmp1278;struct Cyc_String_pa_PrintArg_struct _tmp1277;(_tmp1277.tag=0,((_tmp1277.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1278.tag=0,((_tmp1278.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1279[0]=& _tmp1278,((_tmp1279[1]=& _tmp1277,Cyc_Tcutil_terr(loc,((_tmp127A="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp127A,sizeof(char),52))),_tag_dyneither(_tmp1279,sizeof(void*),2)))))))))))));}
goto _LL6E6;_LL6E6:;}}
# 4604
;_pop_region(r);}
# 4639
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4644
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4650
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4653
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp127F;void*_tmp127E[2];struct Cyc_String_pa_PrintArg_struct _tmp127D;struct Cyc_String_pa_PrintArg_struct _tmp127C;(_tmp127C.tag=0,((_tmp127C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp127D.tag=0,((_tmp127D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp127E[0]=& _tmp127D,((_tmp127E[1]=& _tmp127C,Cyc_Tcutil_terr(loc,((_tmp127F="%s: %s",_tag_dyneither(_tmp127F,sizeof(char),7))),_tag_dyneither(_tmp127E,sizeof(void*),2)))))))))))));}}}}
# 4659
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4663
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4667
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp1280;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1280="duplicate type variable",_tag_dyneither(_tmp1280,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4681 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4686
struct _RegionHandle _tmpB92=_new_region("temp");struct _RegionHandle*temp=& _tmpB92;_push_region(temp);
# 4690
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp1281;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp1281="",_tag_dyneither(_tmp1281,sizeof(char),1))))!= 0){
struct _tuple31*_tmp1284;struct Cyc_List_List*_tmp1283;fields=((_tmp1283=_region_malloc(temp,sizeof(*_tmp1283)),((_tmp1283->hd=((_tmp1284=_region_malloc(temp,sizeof(*_tmp1284)),((_tmp1284->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1284->f2=0,_tmp1284)))))),((_tmp1283->tl=fields,_tmp1283))))));}}}
# 4695
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4697
const char*_tmp1286;const char*_tmp1285;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp1286="struct",_tag_dyneither(_tmp1286,sizeof(char),7)):((_tmp1285="union",_tag_dyneither(_tmp1285,sizeof(char),6)));
# 4700
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct Cyc_List_List*_tmpB97;void*_tmpB98;struct _tuple32*_tmpB96=(struct _tuple32*)des->hd;_tmpB97=_tmpB96->f1;_tmpB98=_tmpB96->f2;
if(_tmpB97 == 0){
# 4705
struct Cyc_List_List*_tmpB99=fields;
for(0;_tmpB99 != 0;_tmpB99=_tmpB99->tl){
if(!(*((struct _tuple31*)_tmpB99->hd)).f2){
(*((struct _tuple31*)_tmpB99->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp128C;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp128B;struct Cyc_List_List*_tmp128A;(*((struct _tuple32*)des->hd)).f1=((_tmp128A=_cycalloc(sizeof(*_tmp128A)),((_tmp128A->hd=(void*)((_tmp128C=_cycalloc(sizeof(*_tmp128C)),((_tmp128C[0]=((_tmp128B.tag=1,((_tmp128B.f1=((*((struct _tuple31*)_tmpB99->hd)).f1)->name,_tmp128B)))),_tmp128C)))),((_tmp128A->tl=0,_tmp128A))))));}
{struct _tuple33*_tmp128F;struct Cyc_List_List*_tmp128E;ans=((_tmp128E=_region_malloc(rgn,sizeof(*_tmp128E)),((_tmp128E->hd=((_tmp128F=_region_malloc(rgn,sizeof(*_tmp128F)),((_tmp128F->f1=(*((struct _tuple31*)_tmpB99->hd)).f1,((_tmp128F->f2=_tmpB98,_tmp128F)))))),((_tmp128E->tl=ans,_tmp128E))))));}
break;}}
# 4713
if(_tmpB99 == 0){
const char*_tmp1293;void*_tmp1292[1];struct Cyc_String_pa_PrintArg_struct _tmp1291;(_tmp1291.tag=0,((_tmp1291.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1292[0]=& _tmp1291,Cyc_Tcutil_terr(loc,((_tmp1293="too many arguments to %s",_tag_dyneither(_tmp1293,sizeof(char),25))),_tag_dyneither(_tmp1292,sizeof(void*),1)))))));}}else{
if(_tmpB97->tl != 0){
# 4717
const char*_tmp1296;void*_tmp1295;(_tmp1295=0,Cyc_Tcutil_terr(loc,((_tmp1296="multiple designators are not yet supported",_tag_dyneither(_tmp1296,sizeof(char),43))),_tag_dyneither(_tmp1295,sizeof(void*),0)));}else{
# 4720
void*_tmpBA4=(void*)_tmpB97->hd;struct _dyneither_ptr*_tmpBA7;_LL6F2: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpBA5=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpBA4;if(_tmpBA5->tag != 0)goto _LL6F4;}_LL6F3:
# 4722
{const char*_tmp129A;void*_tmp1299[1];struct Cyc_String_pa_PrintArg_struct _tmp1298;(_tmp1298.tag=0,((_tmp1298.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1299[0]=& _tmp1298,Cyc_Tcutil_terr(loc,((_tmp129A="array designator used in argument to %s",_tag_dyneither(_tmp129A,sizeof(char),40))),_tag_dyneither(_tmp1299,sizeof(void*),1)))))));}
goto _LL6F1;_LL6F4: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpBA6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpBA4;if(_tmpBA6->tag != 1)goto _LL6F1;else{_tmpBA7=_tmpBA6->f1;}}_LL6F5: {
# 4725
struct Cyc_List_List*_tmpBAB=fields;
for(0;_tmpBAB != 0;_tmpBAB=_tmpBAB->tl){
if(Cyc_strptrcmp(_tmpBA7,((*((struct _tuple31*)_tmpBAB->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpBAB->hd)).f2){
const char*_tmp129E;void*_tmp129D[1];struct Cyc_String_pa_PrintArg_struct _tmp129C;(_tmp129C.tag=0,((_tmp129C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBA7),((_tmp129D[0]=& _tmp129C,Cyc_Tcutil_terr(loc,((_tmp129E="member %s has already been used as an argument",_tag_dyneither(_tmp129E,sizeof(char),47))),_tag_dyneither(_tmp129D,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpBAB->hd)).f2=1;
{struct _tuple33*_tmp12A1;struct Cyc_List_List*_tmp12A0;ans=((_tmp12A0=_region_malloc(rgn,sizeof(*_tmp12A0)),((_tmp12A0->hd=((_tmp12A1=_region_malloc(rgn,sizeof(*_tmp12A1)),((_tmp12A1->f1=(*((struct _tuple31*)_tmpBAB->hd)).f1,((_tmp12A1->f2=_tmpB98,_tmp12A1)))))),((_tmp12A0->tl=ans,_tmp12A0))))));}
break;}}
# 4734
if(_tmpBAB == 0){
const char*_tmp12A5;void*_tmp12A4[1];struct Cyc_String_pa_PrintArg_struct _tmp12A3;(_tmp12A3.tag=0,((_tmp12A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBA7),((_tmp12A4[0]=& _tmp12A3,Cyc_Tcutil_terr(loc,((_tmp12A5="bad field designator %s",_tag_dyneither(_tmp12A5,sizeof(char),24))),_tag_dyneither(_tmp12A4,sizeof(void*),1)))))));}
goto _LL6F1;}_LL6F1:;}}}
# 4739
if(aggr_kind == Cyc_Absyn_StructA)
# 4741
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp12A8;void*_tmp12A7;(_tmp12A7=0,Cyc_Tcutil_terr(loc,((_tmp12A8="too few arguments to struct",_tag_dyneither(_tmp12A8,sizeof(char),28))),_tag_dyneither(_tmp12A7,sizeof(void*),0)));}
break;}}else{
# 4748
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp12AB;void*_tmp12AA;(_tmp12AA=0,Cyc_Tcutil_terr(loc,((_tmp12AB="only one member of a union is allowed",_tag_dyneither(_tmp12AB,sizeof(char),38))),_tag_dyneither(_tmp12AA,sizeof(void*),0)));}
found=1;}}
# 4755
if(!found){const char*_tmp12AE;void*_tmp12AD;(_tmp12AD=0,Cyc_Tcutil_terr(loc,((_tmp12AE="missing member for union",_tag_dyneither(_tmp12AE,sizeof(char),25))),_tag_dyneither(_tmp12AD,sizeof(void*),0)));}}{
# 4758
struct Cyc_List_List*_tmpBBA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpBBA;};};}
# 4690
;_pop_region(temp);}
# 4761
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBBD=Cyc_Tcutil_compress(t);void*_tmpBBF;union Cyc_Absyn_Constraint*_tmpBC0;_LL6F7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBBE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBBD;if(_tmpBBE->tag != 5)goto _LL6F9;else{_tmpBBF=(_tmpBBE->f1).elt_typ;_tmpBC0=((_tmpBBE->f1).ptr_atts).bounds;}}_LL6F8: {
# 4764
void*_tmpBC1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpBC0);_LL6FC: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBC2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBC1;if(_tmpBC2->tag != 0)goto _LL6FE;}_LL6FD:
# 4766
*elt_typ_dest=_tmpBBF;
return 1;_LL6FE:;_LL6FF:
 return 0;_LL6FB:;}_LL6F9:;_LL6FA:
# 4770
 return 0;_LL6F6:;}
# 4774
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBC3=Cyc_Tcutil_compress(t);void*_tmpBC5;union Cyc_Absyn_Constraint*_tmpBC6;_LL701: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBC4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBC3;if(_tmpBC4->tag != 5)goto _LL703;else{_tmpBC5=(_tmpBC4->f1).elt_typ;_tmpBC6=((_tmpBC4->f1).ptr_atts).zero_term;}}_LL702:
# 4777
*elt_typ_dest=_tmpBC5;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBC6);_LL703:;_LL704:
 return 0;_LL700:;}
# 4785
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4787
void*_tmpBC7=Cyc_Tcutil_compress(t);void*_tmpBC9;union Cyc_Absyn_Constraint*_tmpBCA;union Cyc_Absyn_Constraint*_tmpBCB;void*_tmpBCD;struct Cyc_Absyn_Tqual _tmpBCE;struct Cyc_Absyn_Exp*_tmpBCF;union Cyc_Absyn_Constraint*_tmpBD0;_LL706: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBC8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBC7;if(_tmpBC8->tag != 5)goto _LL708;else{_tmpBC9=(_tmpBC8->f1).elt_typ;_tmpBCA=((_tmpBC8->f1).ptr_atts).bounds;_tmpBCB=((_tmpBC8->f1).ptr_atts).zero_term;}}_LL707:
# 4789
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBCB)){
*ptr_type=t;
*elt_type=_tmpBC9;
{void*_tmpBD1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpBCA);_LL70D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBD2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBD1;if(_tmpBD2->tag != 0)goto _LL70F;}_LL70E:
*is_dyneither=1;goto _LL70C;_LL70F:;_LL710:
*is_dyneither=0;goto _LL70C;_LL70C:;}
# 4796
return 1;}else{
return 0;}_LL708: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBCC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBC7;if(_tmpBCC->tag != 8)goto _LL70A;else{_tmpBCD=(_tmpBCC->f1).elt_type;_tmpBCE=(_tmpBCC->f1).tq;_tmpBCF=(_tmpBCC->f1).num_elts;_tmpBD0=(_tmpBCC->f1).zero_term;}}_LL709:
# 4799
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD0)){
*elt_type=_tmpBCD;
*is_dyneither=0;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12C3;struct Cyc_Absyn_PtrAtts _tmp12C2;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp12C1;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12C0;struct Cyc_Absyn_PtrInfo _tmp12BF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12BE;*ptr_type=(void*)(
(_tmp12BE=_cycalloc(sizeof(*_tmp12BE)),((_tmp12BE[0]=((_tmp12C3.tag=5,((_tmp12C3.f1=((_tmp12BF.elt_typ=_tmpBCD,((_tmp12BF.elt_tq=_tmpBCE,((_tmp12BF.ptr_atts=(
(_tmp12C2.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((_tmp12C2.nullable=Cyc_Absyn_false_conref,((_tmp12C2.bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp12C0=_cycalloc(sizeof(*_tmp12C0)),((_tmp12C0[0]=((_tmp12C1.tag=1,((_tmp12C1.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpBCF),_tmp12C1)))),_tmp12C0))))),((_tmp12C2.zero_term=_tmpBD0,((_tmp12C2.ptrloc=0,_tmp12C2)))))))))),_tmp12BF)))))),_tmp12C3)))),_tmp12BE))));}
# 4807
return 1;}else{
return 0;}_LL70A:;_LL70B:
 return 0;_LL705:;}
# 4816
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4818
void*_tmpBD9=e1->r;struct Cyc_Absyn_Exp*_tmpBDC;struct Cyc_Absyn_Exp*_tmpBDE;struct Cyc_Absyn_Exp*_tmpBE0;struct Cyc_Absyn_Exp*_tmpBE2;struct Cyc_Absyn_Exp*_tmpBE4;struct Cyc_Absyn_Exp*_tmpBE6;_LL712: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBDA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBD9;if(_tmpBDA->tag != 13)goto _LL714;}_LL713: {
# 4820
const char*_tmp12C7;void*_tmp12C6[1];struct Cyc_String_pa_PrintArg_struct _tmp12C5;(_tmp12C5.tag=0,((_tmp12C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12C6[0]=& _tmp12C5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C7="we have a cast in a lhs:  %s",_tag_dyneither(_tmp12C7,sizeof(char),29))),_tag_dyneither(_tmp12C6,sizeof(void*),1)))))));}_LL714: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpBDB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBD9;if(_tmpBDB->tag != 19)goto _LL716;else{_tmpBDC=_tmpBDB->f1;}}_LL715:
 _tmpBDE=_tmpBDC;goto _LL717;_LL716: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBDD=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD9;if(_tmpBDD->tag != 22)goto _LL718;else{_tmpBDE=_tmpBDD->f1;}}_LL717:
# 4823
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBDE->topt),ptr_type,is_dyneither,elt_type);_LL718: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpBDF=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBD9;if(_tmpBDF->tag != 21)goto _LL71A;else{_tmpBE0=_tmpBDF->f1;}}_LL719:
 _tmpBE2=_tmpBE0;goto _LL71B;_LL71A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpBE1=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBD9;if(_tmpBE1->tag != 20)goto _LL71C;else{_tmpBE2=_tmpBE1->f1;}}_LL71B:
# 4827
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBE2->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12CB;void*_tmp12CA[1];struct Cyc_String_pa_PrintArg_struct _tmp12C9;(_tmp12C9.tag=0,((_tmp12C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12CA[0]=& _tmp12C9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CB="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp12CB,sizeof(char),51))),_tag_dyneither(_tmp12CA,sizeof(void*),1)))))));}
return 0;_LL71C: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpBE3=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBD9;if(_tmpBE3->tag != 12)goto _LL71E;else{_tmpBE4=_tmpBE3->f1;}}_LL71D:
 _tmpBE6=_tmpBE4;goto _LL71F;_LL71E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpBE5=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBD9;if(_tmpBE5->tag != 11)goto _LL720;else{_tmpBE6=_tmpBE5->f1;}}_LL71F:
# 4833
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBE6->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12CF;void*_tmp12CE[1];struct Cyc_String_pa_PrintArg_struct _tmp12CD;(_tmp12CD.tag=0,((_tmp12CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12CE[0]=& _tmp12CD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CF="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp12CF,sizeof(char),49))),_tag_dyneither(_tmp12CE,sizeof(void*),1)))))));}
return 0;_LL720: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBE7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBD9;if(_tmpBE7->tag != 1)goto _LL722;}_LL721:
 return 0;_LL722:;_LL723: {
# 4839
const char*_tmp12D3;void*_tmp12D2[1];struct Cyc_String_pa_PrintArg_struct _tmp12D1;(_tmp12D1.tag=0,((_tmp12D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12D2[0]=& _tmp12D1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D3="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp12D3,sizeof(char),39))),_tag_dyneither(_tmp12D2,sizeof(void*),1)))))));}_LL711:;}
# 4843
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4854
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpBF4=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpBF8;_LL725: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpBF5=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpBF4;if(_tmpBF5->tag != 22)goto _LL727;}_LL726:
 return !must_be_unique;_LL727: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpBF6=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpBF4;if(_tmpBF6->tag != 21)goto _LL729;}_LL728:
 return 1;_LL729: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBF7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBF4;if(_tmpBF7->tag != 2)goto _LL72B;else{_tmpBF8=_tmpBF7->f1;}}_LL72A: {
# 4859
enum Cyc_Absyn_KindQual _tmpBFA;enum Cyc_Absyn_AliasQual _tmpBFB;struct Cyc_Absyn_Kind*_tmpBF9=Cyc_Tcutil_tvar_kind(_tmpBF8,& Cyc_Tcutil_rk);_tmpBFA=_tmpBF9->kind;_tmpBFB=_tmpBF9->aliasqual;
if(_tmpBFA == Cyc_Absyn_RgnKind  && (_tmpBFB == Cyc_Absyn_Unique  || _tmpBFB == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpBFC=Cyc_Absyn_compress_kb(_tmpBF8->kind);struct Cyc_Core_Opt**_tmpBFE;_LL72E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBFD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBFC;if(_tmpBFD->tag != 2)goto _LL730;else{_tmpBFE=(struct Cyc_Core_Opt**)& _tmpBFD->f1;if((_tmpBFD->f2)->kind != Cyc_Absyn_RgnKind)goto _LL730;if((_tmpBFD->f2)->aliasqual != Cyc_Absyn_Top)goto _LL730;}}_LL72F:
# 4863
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12D9;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12D8;struct Cyc_Core_Opt*_tmp12D7;*_tmpBFE=((_tmp12D7=_cycalloc(sizeof(*_tmp12D7)),((_tmp12D7->v=(void*)((_tmp12D9=_cycalloc(sizeof(*_tmp12D9)),((_tmp12D9[0]=((_tmp12D8.tag=2,((_tmp12D8.f1=0,((_tmp12D8.f2=& Cyc_Tcutil_rk,_tmp12D8)))))),_tmp12D9)))),_tmp12D7))));}
return 0;_LL730:;_LL731:
 return 1;_LL72D:;}
# 4868
return 0;}_LL72B:;_LL72C:
 return 0;_LL724:;}
# 4875
static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int must_be_unique){
void*_tmpC02=Cyc_Tcutil_compress(t);void*_tmpC04;struct Cyc_Absyn_Tvar*_tmpC06;_LL733: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC03=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC02;if(_tmpC03->tag != 5)goto _LL735;else{_tmpC04=((_tmpC03->f1).ptr_atts).rgn;}}_LL734:
# 4878
 return Cyc_Tcutil_is_noalias_region(_tmpC04,must_be_unique);_LL735: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC05=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC02;if(_tmpC05->tag != 2)goto _LL737;else{_tmpC06=_tmpC05->f1;}}_LL736: {
# 4880
enum Cyc_Absyn_KindQual _tmpC08;enum Cyc_Absyn_AliasQual _tmpC09;struct Cyc_Absyn_Kind*_tmpC07=Cyc_Tcutil_tvar_kind(_tmpC06,& Cyc_Tcutil_bk);_tmpC08=_tmpC07->kind;_tmpC09=_tmpC07->aliasqual;
switch(_tmpC08){case Cyc_Absyn_BoxKind: _LL739:
 goto _LL73A;case Cyc_Absyn_AnyKind: _LL73A: goto _LL73B;case Cyc_Absyn_MemKind: _LL73B:
 if(_tmpC09 == Cyc_Absyn_Unique  || _tmpC09 == Cyc_Absyn_Top){
void*_tmpC0A=Cyc_Absyn_compress_kb(_tmpC06->kind);struct Cyc_Core_Opt**_tmpC0C;enum Cyc_Absyn_KindQual _tmpC0D;_LL73E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC0B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC0A;if(_tmpC0B->tag != 2)goto _LL740;else{_tmpC0C=(struct Cyc_Core_Opt**)& _tmpC0B->f1;_tmpC0D=(_tmpC0B->f2)->kind;if((_tmpC0B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL740;}}_LL73F:
# 4886
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12E3;struct Cyc_Absyn_Kind*_tmp12E2;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12E1;struct Cyc_Core_Opt*_tmp12E0;*_tmpC0C=((_tmp12E0=_cycalloc(sizeof(*_tmp12E0)),((_tmp12E0->v=(void*)((_tmp12E3=_cycalloc(sizeof(*_tmp12E3)),((_tmp12E3[0]=((_tmp12E1.tag=2,((_tmp12E1.f1=0,((_tmp12E1.f2=((_tmp12E2=_cycalloc_atomic(sizeof(*_tmp12E2)),((_tmp12E2->kind=_tmpC0D,((_tmp12E2->aliasqual=Cyc_Absyn_Aliasable,_tmp12E2)))))),_tmp12E1)))))),_tmp12E3)))),_tmp12E0))));}
return 0;_LL740:;_LL741:
# 4890
 return 1;_LL73D:;}
# 4893
return 0;default: _LL73C:
 return 0;}}_LL737:;_LL738:
# 4896
 return 0;_LL732:;}
# 4899
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}
# 4901
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpC12=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpC12))return 1;{
void*_tmpC13=_tmpC12;struct Cyc_List_List*_tmpC15;struct Cyc_Absyn_Aggrdecl**_tmpC17;struct Cyc_List_List*_tmpC18;struct Cyc_List_List*_tmpC1A;union Cyc_Absyn_DatatypeInfoU _tmpC1D;struct Cyc_List_List*_tmpC1E;union Cyc_Absyn_DatatypeFieldInfoU _tmpC20;struct Cyc_List_List*_tmpC21;_LL744: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC14=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC13;if(_tmpC14->tag != 10)goto _LL746;else{_tmpC15=_tmpC14->f1;}}_LL745:
# 4906
 while(_tmpC15 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpC15->hd)).f2))return 1;
_tmpC15=_tmpC15->tl;}
# 4910
return 0;_LL746: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC16=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC13;if(_tmpC16->tag != 11)goto _LL748;else{if((((_tmpC16->f1).aggr_info).KnownAggr).tag != 2)goto _LL748;_tmpC17=(struct Cyc_Absyn_Aggrdecl**)(((_tmpC16->f1).aggr_info).KnownAggr).val;_tmpC18=(_tmpC16->f1).targs;}}_LL747:
# 4912
 if((*_tmpC17)->impl == 0)return 0;else{
# 4914
struct Cyc_List_List*_tmpC22=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpC17)->tvs,_tmpC18);
struct Cyc_List_List*_tmpC23=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpC17)->impl))->fields;
void*t;
while(_tmpC23 != 0){
t=_tmpC22 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpC23->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpC22,((struct Cyc_Absyn_Aggrfield*)_tmpC23->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpC23=_tmpC23->tl;}
# 4922
return 0;}_LL748: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC19=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC13;if(_tmpC19->tag != 12)goto _LL74A;else{_tmpC1A=_tmpC19->f2;}}_LL749:
# 4925
 while(_tmpC1A != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpC1A->hd)->type))return 1;
_tmpC1A=_tmpC1A->tl;}
# 4929
return 0;_LL74A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC1B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC13;if(_tmpC1B->tag != 11)goto _LL74C;else{if((((_tmpC1B->f1).aggr_info).UnknownAggr).tag != 1)goto _LL74C;}}_LL74B: {
# 4931
const char*_tmp12E6;void*_tmp12E5;(_tmp12E5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E6="got unknown aggr in is_noalias_aggr",_tag_dyneither(_tmp12E6,sizeof(char),36))),_tag_dyneither(_tmp12E5,sizeof(void*),0)));}_LL74C: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC1C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC13;if(_tmpC1C->tag != 3)goto _LL74E;else{_tmpC1D=(_tmpC1C->f1).datatype_info;_tmpC1E=(_tmpC1C->f1).targs;}}_LL74D: {
# 4933
union Cyc_Absyn_DatatypeInfoU _tmpC26=_tmpC1D;struct _tuple2*_tmpC27;int _tmpC28;struct Cyc_List_List*_tmpC29;struct Cyc_Core_Opt*_tmpC2A;_LL753: if((_tmpC26.UnknownDatatype).tag != 1)goto _LL755;_tmpC27=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC26.UnknownDatatype).val).name;_tmpC28=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC26.UnknownDatatype).val).is_extensible;_LL754: {
# 4935
const char*_tmp12E9;void*_tmp12E8;(_tmp12E8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E9="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp12E9,sizeof(char),40))),_tag_dyneither(_tmp12E8,sizeof(void*),0)));}_LL755: if((_tmpC26.KnownDatatype).tag != 2)goto _LL752;_tmpC29=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC26.KnownDatatype).val))->tvs;_tmpC2A=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC26.KnownDatatype).val))->fields;_LL756:
# 4938
 return 0;_LL752:;}_LL74E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpC1F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpC13;if(_tmpC1F->tag != 4)goto _LL750;else{_tmpC20=(_tmpC1F->f1).field_info;_tmpC21=(_tmpC1F->f1).targs;}}_LL74F: {
# 4941
union Cyc_Absyn_DatatypeFieldInfoU _tmpC2D=_tmpC20;struct Cyc_Absyn_Datatypedecl*_tmpC2E;struct Cyc_Absyn_Datatypefield*_tmpC2F;_LL758: if((_tmpC2D.UnknownDatatypefield).tag != 1)goto _LL75A;_LL759: {
# 4943
const char*_tmp12EC;void*_tmp12EB;(_tmp12EB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EC="got unknown datatype field in is_noalias_aggr",_tag_dyneither(_tmp12EC,sizeof(char),46))),_tag_dyneither(_tmp12EB,sizeof(void*),0)));}_LL75A: if((_tmpC2D.KnownDatatypefield).tag != 2)goto _LL757;_tmpC2E=((struct _tuple3)(_tmpC2D.KnownDatatypefield).val).f1;_tmpC2F=((struct _tuple3)(_tmpC2D.KnownDatatypefield).val).f2;_LL75B: {
# 4945
struct Cyc_List_List*_tmpC32=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpC2E->tvs,_tmpC21);
struct Cyc_List_List*_tmpC33=_tmpC2F->typs;
while(_tmpC33 != 0){
_tmpC12=_tmpC32 == 0?(*((struct _tuple12*)_tmpC33->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpC32,(*((struct _tuple12*)_tmpC33->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpC12))return 1;
_tmpC33=_tmpC33->tl;}
# 4952
return 0;}_LL757:;}_LL750:;_LL751:
# 4954
 return 0;_LL743:;};}
# 4958
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpC34=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Aggrdecl*_tmpC36;struct Cyc_List_List*_tmpC37;struct Cyc_List_List*_tmpC39;_LL75D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC35=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC34;if(_tmpC35->tag != 11)goto _LL75F;else{if((((_tmpC35->f1).aggr_info).KnownAggr).tag != 2)goto _LL75F;_tmpC36=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC35->f1).aggr_info).KnownAggr).val);_tmpC37=(_tmpC35->f1).targs;}}_LL75E:
# 4961
 _tmpC39=_tmpC36->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC36->impl))->fields;goto _LL760;_LL75F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC38=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC34;if(_tmpC38->tag != 12)goto _LL761;else{_tmpC39=_tmpC38->f2;}}_LL760: {
# 4963
struct Cyc_Absyn_Aggrfield*_tmpC3A=Cyc_Absyn_lookup_field(_tmpC39,f);
{struct Cyc_Absyn_Aggrfield*_tmpC3B=_tmpC3A;void*_tmpC3C;_LL764: if(_tmpC3B != 0)goto _LL766;_LL765: {
const char*_tmp12EF;void*_tmp12EE;(_tmp12EE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EF="is_noalias_field: missing field",_tag_dyneither(_tmp12EF,sizeof(char),32))),_tag_dyneither(_tmp12EE,sizeof(void*),0)));}_LL766: if(_tmpC3B == 0)goto _LL763;_tmpC3C=_tmpC3B->type;_LL767:
# 4967
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpC3C);_LL763:;}
# 4969
return 0;}_LL761:;_LL762: {
# 4971
const char*_tmp12F3;void*_tmp12F2[1];struct Cyc_String_pa_PrintArg_struct _tmp12F1;(_tmp12F1.tag=0,((_tmp12F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp12F2[0]=& _tmp12F1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F3="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp12F3,sizeof(char),36))),_tag_dyneither(_tmp12F2,sizeof(void*),1)))))));}_LL75C:;}
# 4979
static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpC42=e->r;struct Cyc_Absyn_Exp*_tmpC46;struct Cyc_Absyn_Exp*_tmpC48;struct Cyc_Absyn_Exp*_tmpC4A;struct _dyneither_ptr*_tmpC4B;struct Cyc_Absyn_Exp*_tmpC4D;struct Cyc_Absyn_Exp*_tmpC4E;struct Cyc_Absyn_Exp*_tmpC50;struct Cyc_Absyn_Exp*_tmpC51;struct Cyc_Absyn_Exp*_tmpC53;struct Cyc_Absyn_Exp*_tmpC55;struct Cyc_Absyn_Stmt*_tmpC57;_LL769: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC43=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC42;if(_tmpC43->tag != 1)goto _LL76B;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC44=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpC43->f2);if(_tmpC44->tag != 1)goto _LL76B;}}_LL76A:
 return 0;_LL76B: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC45=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC42;if(_tmpC45->tag != 21)goto _LL76D;else{_tmpC46=_tmpC45->f1;}}_LL76C:
 _tmpC48=_tmpC46;goto _LL76E;_LL76D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC47=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC42;if(_tmpC47->tag != 19)goto _LL76F;else{_tmpC48=_tmpC47->f1;}}_LL76E:
# 4984
 return Cyc_Tcutil_is_noalias_pointer_aux((void*)_check_null(_tmpC48->topt),1) && 
Cyc_Tcutil_is_noalias_path_aux(r,_tmpC48);_LL76F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC49=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC42;if(_tmpC49->tag != 20)goto _LL771;else{_tmpC4A=_tmpC49->f1;_tmpC4B=_tmpC49->f2;}}_LL770:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC4A);_LL771: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC4C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC42;if(_tmpC4C->tag != 22)goto _LL773;else{_tmpC4D=_tmpC4C->f1;_tmpC4E=_tmpC4C->f2;}}_LL772: {
# 4988
void*_tmpC58=Cyc_Tcutil_compress((void*)_check_null(_tmpC4D->topt));_LL77E: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC59=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC58;if(_tmpC59->tag != 10)goto _LL780;}_LL77F:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC4D);_LL780:;_LL781:
 return 0;_LL77D:;}_LL773: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC4F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC42;if(_tmpC4F->tag != 5)goto _LL775;else{_tmpC50=_tmpC4F->f2;_tmpC51=_tmpC4F->f3;}}_LL774:
# 4993
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC50) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpC51);_LL775: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC52=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC42;if(_tmpC52->tag != 8)goto _LL777;else{_tmpC53=_tmpC52->f2;}}_LL776:
 _tmpC55=_tmpC53;goto _LL778;_LL777: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC54=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC42;if(_tmpC54->tag != 13)goto _LL779;else{_tmpC55=_tmpC54->f2;}}_LL778:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC55);_LL779: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpC56=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC42;if(_tmpC56->tag != 35)goto _LL77B;else{_tmpC57=_tmpC56->f1;}}_LL77A:
# 4997
 while(1){
void*_tmpC5A=_tmpC57->r;struct Cyc_Absyn_Stmt*_tmpC5C;struct Cyc_Absyn_Stmt*_tmpC5D;struct Cyc_Absyn_Decl*_tmpC5F;struct Cyc_Absyn_Stmt*_tmpC60;struct Cyc_Absyn_Exp*_tmpC62;_LL783: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC5B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpC5A;if(_tmpC5B->tag != 2)goto _LL785;else{_tmpC5C=_tmpC5B->f1;_tmpC5D=_tmpC5B->f2;}}_LL784:
 _tmpC57=_tmpC5D;goto _LL782;_LL785: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpC5E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpC5A;if(_tmpC5E->tag != 12)goto _LL787;else{_tmpC5F=_tmpC5E->f1;_tmpC60=_tmpC5E->f2;}}_LL786:
 _tmpC57=_tmpC60;goto _LL782;_LL787: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC61=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpC5A;if(_tmpC61->tag != 1)goto _LL789;else{_tmpC62=_tmpC61->f1;}}_LL788:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC62);_LL789:;_LL78A: {
const char*_tmp12F6;void*_tmp12F5;(_tmp12F5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F6="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp12F6,sizeof(char),40))),_tag_dyneither(_tmp12F5,sizeof(void*),0)));}_LL782:;}_LL77B:;_LL77C:
# 5005
 return 1;_LL768:;}
# 5008
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_is_noalias_path_aux(r,e);}
# 5025 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5027
struct _tuple18 _tmp12F7;struct _tuple18 bogus_ans=(_tmp12F7.f1=0,((_tmp12F7.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12F7)));
void*_tmpC65=e->r;struct _tuple2*_tmpC67;void*_tmpC68;struct Cyc_Absyn_Exp*_tmpC6A;struct _dyneither_ptr*_tmpC6B;int _tmpC6C;struct Cyc_Absyn_Exp*_tmpC6E;struct _dyneither_ptr*_tmpC6F;int _tmpC70;struct Cyc_Absyn_Exp*_tmpC72;struct Cyc_Absyn_Exp*_tmpC74;struct Cyc_Absyn_Exp*_tmpC75;_LL78C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC66=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC65;if(_tmpC66->tag != 1)goto _LL78E;else{_tmpC67=_tmpC66->f1;_tmpC68=(void*)_tmpC66->f2;}}_LL78D: {
# 5031
void*_tmpC76=_tmpC68;struct Cyc_Absyn_Vardecl*_tmpC7A;struct Cyc_Absyn_Vardecl*_tmpC7C;struct Cyc_Absyn_Vardecl*_tmpC7E;struct Cyc_Absyn_Vardecl*_tmpC80;_LL799: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC77=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC76;if(_tmpC77->tag != 0)goto _LL79B;}_LL79A:
 goto _LL79C;_LL79B: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC78=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC76;if(_tmpC78->tag != 2)goto _LL79D;}_LL79C:
 return bogus_ans;_LL79D: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC79=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC76;if(_tmpC79->tag != 1)goto _LL79F;else{_tmpC7A=_tmpC79->f1;}}_LL79E: {
# 5035
void*_tmpC81=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7A6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC82=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC81;if(_tmpC82->tag != 8)goto _LL7A8;}_LL7A7: {
# 5037
struct _tuple18 _tmp12F8;return(_tmp12F8.f1=1,((_tmp12F8.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12F8)));}_LL7A8:;_LL7A9: {
struct _tuple18 _tmp12F9;return(_tmp12F9.f1=(_tmpC7A->tq).real_const,((_tmp12F9.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12F9)));}_LL7A5:;}_LL79F: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC7B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpC76;if(_tmpC7B->tag != 4)goto _LL7A1;else{_tmpC7C=_tmpC7B->f1;}}_LL7A0: {
# 5041
void*_tmpC85=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7AB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC86=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC85;if(_tmpC86->tag != 8)goto _LL7AD;}_LL7AC: {
struct _tuple18 _tmp12FA;return(_tmp12FA.f1=1,((_tmp12FA.f2=(void*)_check_null(_tmpC7C->rgn),_tmp12FA)));}_LL7AD:;_LL7AE:
# 5044
 _tmpC7C->escapes=1;{
struct _tuple18 _tmp12FB;return(_tmp12FB.f1=(_tmpC7C->tq).real_const,((_tmp12FB.f2=(void*)_check_null(_tmpC7C->rgn),_tmp12FB)));};_LL7AA:;}_LL7A1: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpC7D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpC76;if(_tmpC7D->tag != 5)goto _LL7A3;else{_tmpC7E=_tmpC7D->f1;}}_LL7A2:
# 5047
 _tmpC80=_tmpC7E;goto _LL7A4;_LL7A3: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpC7F=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpC76;if(_tmpC7F->tag != 3)goto _LL798;else{_tmpC80=_tmpC7F->f1;}}_LL7A4:
# 5049
 _tmpC80->escapes=1;{
struct _tuple18 _tmp12FC;return(_tmp12FC.f1=(_tmpC80->tq).real_const,((_tmp12FC.f2=(void*)_check_null(_tmpC80->rgn),_tmp12FC)));};_LL798:;}_LL78E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC69=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC65;if(_tmpC69->tag != 20)goto _LL790;else{_tmpC6A=_tmpC69->f1;_tmpC6B=_tmpC69->f2;_tmpC6C=_tmpC69->f3;}}_LL78F:
# 5054
 if(_tmpC6C)return bogus_ans;{
# 5057
void*_tmpC8A=Cyc_Tcutil_compress((void*)_check_null(_tmpC6A->topt));struct Cyc_List_List*_tmpC8C;struct Cyc_Absyn_Aggrdecl*_tmpC8E;_LL7B0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC8B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC8A;if(_tmpC8B->tag != 12)goto _LL7B2;else{_tmpC8C=_tmpC8B->f2;}}_LL7B1: {
# 5059
struct Cyc_Absyn_Aggrfield*_tmpC8F=Cyc_Absyn_lookup_field(_tmpC8C,_tmpC6B);
if(_tmpC8F != 0  && _tmpC8F->width == 0){
struct _tuple18 _tmp12FD;return(_tmp12FD.f1=(_tmpC8F->tq).real_const,((_tmp12FD.f2=(Cyc_Tcutil_addressof_props(te,_tmpC6A)).f2,_tmp12FD)));}
return bogus_ans;}_LL7B2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC8D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC8A;if(_tmpC8D->tag != 11)goto _LL7B4;else{if((((_tmpC8D->f1).aggr_info).KnownAggr).tag != 2)goto _LL7B4;_tmpC8E=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC8D->f1).aggr_info).KnownAggr).val);}}_LL7B3: {
# 5064
struct Cyc_Absyn_Aggrfield*_tmpC91=Cyc_Absyn_lookup_decl_field(_tmpC8E,_tmpC6B);
if(_tmpC91 != 0  && _tmpC91->width == 0){
struct _tuple18 _tmp12FE;return(_tmp12FE.f1=(_tmpC91->tq).real_const,((_tmp12FE.f2=(Cyc_Tcutil_addressof_props(te,_tmpC6A)).f2,_tmp12FE)));}
return bogus_ans;}_LL7B4:;_LL7B5:
 return bogus_ans;_LL7AF:;};_LL790: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC6D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC65;if(_tmpC6D->tag != 21)goto _LL792;else{_tmpC6E=_tmpC6D->f1;_tmpC6F=_tmpC6D->f2;_tmpC70=_tmpC6D->f3;}}_LL791:
# 5072
 if(_tmpC70)return bogus_ans;{
# 5076
void*_tmpC93=Cyc_Tcutil_compress((void*)_check_null(_tmpC6E->topt));void*_tmpC95;void*_tmpC96;_LL7B7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC94=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC93;if(_tmpC94->tag != 5)goto _LL7B9;else{_tmpC95=(_tmpC94->f1).elt_typ;_tmpC96=((_tmpC94->f1).ptr_atts).rgn;}}_LL7B8: {
# 5078
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpC97=Cyc_Tcutil_compress(_tmpC95);struct Cyc_List_List*_tmpC99;struct Cyc_Absyn_Aggrdecl*_tmpC9B;_LL7BC: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC98=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC97;if(_tmpC98->tag != 12)goto _LL7BE;else{_tmpC99=_tmpC98->f2;}}_LL7BD:
# 5081
 finfo=Cyc_Absyn_lookup_field(_tmpC99,_tmpC6F);goto _LL7BB;_LL7BE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC9A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC97;if(_tmpC9A->tag != 11)goto _LL7C0;else{if((((_tmpC9A->f1).aggr_info).KnownAggr).tag != 2)goto _LL7C0;_tmpC9B=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC9A->f1).aggr_info).KnownAggr).val);}}_LL7BF:
# 5083
 finfo=Cyc_Absyn_lookup_decl_field(_tmpC9B,_tmpC6F);goto _LL7BB;_LL7C0:;_LL7C1:
 return bogus_ans;_LL7BB:;}
# 5086
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp12FF;return(_tmp12FF.f1=(finfo->tq).real_const,((_tmp12FF.f2=_tmpC96,_tmp12FF)));}
return bogus_ans;}_LL7B9:;_LL7BA:
 return bogus_ans;_LL7B6:;};_LL792: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC71=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC65;if(_tmpC71->tag != 19)goto _LL794;else{_tmpC72=_tmpC71->f1;}}_LL793: {
# 5093
void*_tmpC9D=Cyc_Tcutil_compress((void*)_check_null(_tmpC72->topt));struct Cyc_Absyn_Tqual _tmpC9F;void*_tmpCA0;_LL7C3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC9E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC9D;if(_tmpC9E->tag != 5)goto _LL7C5;else{_tmpC9F=(_tmpC9E->f1).elt_tq;_tmpCA0=((_tmpC9E->f1).ptr_atts).rgn;}}_LL7C4: {
# 5095
struct _tuple18 _tmp1300;return(_tmp1300.f1=_tmpC9F.real_const,((_tmp1300.f2=_tmpCA0,_tmp1300)));}_LL7C5:;_LL7C6:
 return bogus_ans;_LL7C2:;}_LL794: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC73=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC65;if(_tmpC73->tag != 22)goto _LL796;else{_tmpC74=_tmpC73->f1;_tmpC75=_tmpC73->f2;}}_LL795: {
# 5101
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpC74->topt));
void*_tmpCA2=t;struct Cyc_List_List*_tmpCA4;struct Cyc_Absyn_Tqual _tmpCA6;void*_tmpCA7;struct Cyc_Absyn_Tqual _tmpCA9;_LL7C8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCA3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCA2;if(_tmpCA3->tag != 10)goto _LL7CA;else{_tmpCA4=_tmpCA3->f1;}}_LL7C9: {
# 5105
unsigned int _tmpCAB;int _tmpCAC;struct _tuple14 _tmpCAA=Cyc_Evexp_eval_const_uint_exp(_tmpC75);_tmpCAB=_tmpCAA.f1;_tmpCAC=_tmpCAA.f2;
if(!_tmpCAC)
return bogus_ans;{
struct _tuple12*_tmpCAD=Cyc_Absyn_lookup_tuple_field(_tmpCA4,(int)_tmpCAB);
if(_tmpCAD != 0){
struct _tuple18 _tmp1301;return(_tmp1301.f1=((*_tmpCAD).f1).real_const,((_tmp1301.f2=(Cyc_Tcutil_addressof_props(te,_tmpC74)).f2,_tmp1301)));}
return bogus_ans;};}_LL7CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCA5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCA2;if(_tmpCA5->tag != 5)goto _LL7CC;else{_tmpCA6=(_tmpCA5->f1).elt_tq;_tmpCA7=((_tmpCA5->f1).ptr_atts).rgn;}}_LL7CB: {
# 5113
struct _tuple18 _tmp1302;return(_tmp1302.f1=_tmpCA6.real_const,((_tmp1302.f2=_tmpCA7,_tmp1302)));}_LL7CC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCA8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCA2;if(_tmpCA8->tag != 8)goto _LL7CE;else{_tmpCA9=(_tmpCA8->f1).tq;}}_LL7CD: {
# 5119
struct _tuple18 _tmp1303;return(_tmp1303.f1=_tmpCA9.real_const,((_tmp1303.f2=(Cyc_Tcutil_addressof_props(te,_tmpC74)).f2,_tmp1303)));}_LL7CE:;_LL7CF:
 return bogus_ans;_LL7C7:;}_LL796:;_LL797:
# 5123
{const char*_tmp1306;void*_tmp1305;(_tmp1305=0,Cyc_Tcutil_terr(e->loc,((_tmp1306="unary & applied to non-lvalue",_tag_dyneither(_tmp1306,sizeof(char),30))),_tag_dyneither(_tmp1305,sizeof(void*),0)));}
return bogus_ans;_LL78B:;}
# 5135 "tcutil.cyc"
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){
void*_tmpCB4=Cyc_Tcutil_compress(e_typ);void*_tmpCB6;struct Cyc_Absyn_Tqual _tmpCB7;union Cyc_Absyn_Constraint*_tmpCB8;_LL7D1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCB5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCB4;if(_tmpCB5->tag != 8)goto _LL7D3;else{_tmpCB6=(_tmpCB5->f1).elt_type;_tmpCB7=(_tmpCB5->f1).tq;_tmpCB8=(_tmpCB5->f1).zero_term;}}_LL7D2: {
# 5138
void*_tmpCBA;struct _tuple18 _tmpCB9=Cyc_Tcutil_addressof_props(te,e);_tmpCBA=_tmpCB9.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1309;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1308;return Cyc_Absyn_atb_typ(_tmpCB6,_tmpCBA,_tmpCB7,(void*)((_tmp1308=_cycalloc(sizeof(*_tmp1308)),((_tmp1308[0]=((_tmp1309.tag=1,((_tmp1309.f1=e,_tmp1309)))),_tmp1308)))),_tmpCB8);};}_LL7D3:;_LL7D4:
# 5141
 return e_typ;_LL7D0:;}
# 5147
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpCBD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpCC0;_LL7D6: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCBE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCBD;if(_tmpCBE->tag != 0)goto _LL7D8;}_LL7D7:
 return;_LL7D8: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCBF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCBD;if(_tmpCBF->tag != 1)goto _LL7D5;else{_tmpCC0=_tmpCBF->f1;}}_LL7D9: {
# 5152
unsigned int _tmpCC2;int _tmpCC3;struct _tuple14 _tmpCC1=Cyc_Evexp_eval_const_uint_exp(_tmpCC0);_tmpCC2=_tmpCC1.f1;_tmpCC3=_tmpCC1.f2;
if(_tmpCC3  && _tmpCC2 <= i){
const char*_tmp130E;void*_tmp130D[2];struct Cyc_Int_pa_PrintArg_struct _tmp130C;struct Cyc_Int_pa_PrintArg_struct _tmp130B;(_tmp130B.tag=1,((_tmp130B.f1=(unsigned long)((int)i),((_tmp130C.tag=1,((_tmp130C.f1=(unsigned long)((int)_tmpCC2),((_tmp130D[0]=& _tmp130C,((_tmp130D[1]=& _tmp130B,Cyc_Tcutil_terr(loc,((_tmp130E="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp130E,sizeof(char),39))),_tag_dyneither(_tmp130D,sizeof(void*),2)))))))))))));}
return;}_LL7D5:;};}
# 5159
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5163
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpCC8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpCCA;_LL7DB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCC9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCC8;if(_tmpCC9->tag != 1)goto _LL7DD;else{_tmpCCA=_tmpCC9->f1;}}_LL7DC: {
# 5166
unsigned int _tmpCCC;int _tmpCCD;struct _tuple14 _tmpCCB=Cyc_Evexp_eval_const_uint_exp(_tmpCCA);_tmpCCC=_tmpCCB.f1;_tmpCCD=_tmpCCB.f2;
return _tmpCCD  && _tmpCCC == 1;}_LL7DD:;_LL7DE:
 return 0;_LL7DA:;}
# 5172
int Cyc_Tcutil_bits_only(void*t){
void*_tmpCCE=Cyc_Tcutil_compress(t);void*_tmpCD5;union Cyc_Absyn_Constraint*_tmpCD6;struct Cyc_List_List*_tmpCD8;struct Cyc_Absyn_Aggrdecl*_tmpCDB;struct Cyc_List_List*_tmpCDC;struct Cyc_List_List*_tmpCDE;_LL7E0: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCCF=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCCE;if(_tmpCCF->tag != 0)goto _LL7E2;}_LL7E1:
 goto _LL7E3;_LL7E2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCD0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCCE;if(_tmpCD0->tag != 6)goto _LL7E4;}_LL7E3:
 goto _LL7E5;_LL7E4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCD1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCCE;if(_tmpCD1->tag != 7)goto _LL7E6;}_LL7E5:
 return 1;_LL7E6: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpCD2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpCCE;if(_tmpCD2->tag != 13)goto _LL7E8;}_LL7E7:
 goto _LL7E9;_LL7E8: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpCD3=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpCCE;if(_tmpCD3->tag != 14)goto _LL7EA;}_LL7E9:
 return 0;_LL7EA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCD4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCCE;if(_tmpCD4->tag != 8)goto _LL7EC;else{_tmpCD5=(_tmpCD4->f1).elt_type;_tmpCD6=(_tmpCD4->f1).zero_term;}}_LL7EB:
# 5182
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCD6) && Cyc_Tcutil_bits_only(_tmpCD5);_LL7EC: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCD7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCCE;if(_tmpCD7->tag != 10)goto _LL7EE;else{_tmpCD8=_tmpCD7->f1;}}_LL7ED:
# 5184
 for(0;_tmpCD8 != 0;_tmpCD8=_tmpCD8->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpCD8->hd)).f2))return 0;}
return 1;_LL7EE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCD9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCCE;if(_tmpCD9->tag != 11)goto _LL7F0;else{if((((_tmpCD9->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7F0;}}_LL7EF:
 return 0;_LL7F0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCDA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCCE;if(_tmpCDA->tag != 11)goto _LL7F2;else{if((((_tmpCDA->f1).aggr_info).KnownAggr).tag != 2)goto _LL7F2;_tmpCDB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCDA->f1).aggr_info).KnownAggr).val);_tmpCDC=(_tmpCDA->f1).targs;}}_LL7F1:
# 5189
 if(_tmpCDB->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCDB->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpCDF=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCDF;_push_region(rgn);
{struct Cyc_List_List*_tmpCE0=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpCDB->tvs,_tmpCDC);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCDB->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpCE0,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpCE1=0;_npop_handler(0);return _tmpCE1;}}}{
int _tmpCE2=1;_npop_handler(0);return _tmpCE2;};}
# 5196
;_pop_region(rgn);};};_LL7F2: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCDD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCCE;if(_tmpCDD->tag != 12)goto _LL7F4;else{_tmpCDE=_tmpCDD->f2;}}_LL7F3:
# 5202
 for(0;_tmpCDE != 0;_tmpCDE=_tmpCDE->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpCDE->hd)->type))return 0;}
return 1;_LL7F4:;_LL7F5:
 return 0;_LL7DF:;}
# 5214
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpCE3=e->r;struct _tuple2*_tmpCEB;void*_tmpCEC;struct Cyc_Absyn_Exp*_tmpCEE;struct Cyc_Absyn_Exp*_tmpCEF;struct Cyc_Absyn_Exp*_tmpCF0;struct Cyc_Absyn_Exp*_tmpCF2;struct Cyc_Absyn_Exp*_tmpCF3;struct Cyc_Absyn_Exp*_tmpCF5;struct Cyc_Absyn_Exp*_tmpCF7;void*_tmpCF9;struct Cyc_Absyn_Exp*_tmpCFA;void*_tmpCFC;struct Cyc_Absyn_Exp*_tmpCFD;struct Cyc_Absyn_Exp*_tmpCFF;struct Cyc_Absyn_Exp*_tmpD01;struct Cyc_Absyn_Exp*_tmpD02;struct Cyc_List_List*_tmpD04;struct Cyc_List_List*_tmpD06;struct Cyc_List_List*_tmpD08;enum Cyc_Absyn_Primop _tmpD0A;struct Cyc_List_List*_tmpD0B;struct Cyc_List_List*_tmpD0D;struct Cyc_List_List*_tmpD0F;_LL7F7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpCE4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCE4->tag != 0)goto _LL7F9;}_LL7F8:
 goto _LL7FA;_LL7F9: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCE5=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCE5->tag != 16)goto _LL7FB;}_LL7FA:
 goto _LL7FC;_LL7FB: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCE6=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCE6->tag != 17)goto _LL7FD;}_LL7FC:
 goto _LL7FE;_LL7FD: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCE7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCE7->tag != 18)goto _LL7FF;}_LL7FE:
 goto _LL800;_LL7FF: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCE8=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCE8->tag != 30)goto _LL801;}_LL800:
 goto _LL802;_LL801: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCE9=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCE9->tag != 31)goto _LL803;}_LL802:
 return 1;_LL803: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCEA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCEA->tag != 1)goto _LL805;else{_tmpCEB=_tmpCEA->f1;_tmpCEC=(void*)_tmpCEA->f2;}}_LL804: {
# 5225
void*_tmpD10=_tmpCEC;struct Cyc_Absyn_Vardecl*_tmpD13;struct Cyc_Absyn_Vardecl*_tmpD15;_LL824: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD11=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD10;if(_tmpD11->tag != 2)goto _LL826;}_LL825:
 return 1;_LL826: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD12=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD10;if(_tmpD12->tag != 1)goto _LL828;else{_tmpD13=_tmpD12->f1;}}_LL827: {
# 5228
void*_tmpD17=Cyc_Tcutil_compress(_tmpD13->type);_LL82F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD18=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD17;if(_tmpD18->tag != 8)goto _LL831;}_LL830:
 goto _LL832;_LL831: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD19=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD17;if(_tmpD19->tag != 9)goto _LL833;}_LL832:
 return 1;_LL833:;_LL834:
 return var_okay;_LL82E:;}_LL828: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD14=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD10;if(_tmpD14->tag != 4)goto _LL82A;else{_tmpD15=_tmpD14->f1;}}_LL829:
# 5235
 if(_tmpD15->sc == Cyc_Absyn_Static){
void*_tmpD1A=Cyc_Tcutil_compress(_tmpD15->type);_LL836: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD1B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD1A;if(_tmpD1B->tag != 8)goto _LL838;}_LL837:
 goto _LL839;_LL838: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD1C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD1A;if(_tmpD1C->tag != 9)goto _LL83A;}_LL839:
 return 1;_LL83A:;_LL83B:
 return var_okay;_LL835:;}else{
# 5242
return var_okay;}_LL82A: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD16=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD10;if(_tmpD16->tag != 0)goto _LL82C;}_LL82B:
 return 0;_LL82C:;_LL82D:
 return var_okay;_LL823:;}_LL805: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCED=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCED->tag != 5)goto _LL807;else{_tmpCEE=_tmpCED->f1;_tmpCEF=_tmpCED->f2;_tmpCF0=_tmpCED->f3;}}_LL806:
# 5248
 return(Cyc_Tcutil_cnst_exp(0,_tmpCEE) && 
Cyc_Tcutil_cnst_exp(0,_tmpCEF)) && 
Cyc_Tcutil_cnst_exp(0,_tmpCF0);_LL807: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpCF1=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCF1->tag != 8)goto _LL809;else{_tmpCF2=_tmpCF1->f1;_tmpCF3=_tmpCF1->f2;}}_LL808:
# 5252
 return Cyc_Tcutil_cnst_exp(0,_tmpCF2) && Cyc_Tcutil_cnst_exp(0,_tmpCF3);_LL809: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCF4=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCF4->tag != 11)goto _LL80B;else{_tmpCF5=_tmpCF4->f1;}}_LL80A:
 _tmpCF7=_tmpCF5;goto _LL80C;_LL80B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpCF6=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCF6->tag != 12)goto _LL80D;else{_tmpCF7=_tmpCF6->f1;}}_LL80C:
# 5255
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpCF7);_LL80D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCF8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCF8->tag != 13)goto _LL80F;else{_tmpCF9=(void*)_tmpCF8->f1;_tmpCFA=_tmpCF8->f2;if(_tmpCF8->f4 != Cyc_Absyn_No_coercion)goto _LL80F;}}_LL80E:
# 5257
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpCFA);_LL80F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCFB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCFB->tag != 13)goto _LL811;else{_tmpCFC=(void*)_tmpCFB->f1;_tmpCFD=_tmpCFB->f2;}}_LL810:
# 5260
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpCFD);_LL811: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpCFE=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpCFE->tag != 14)goto _LL813;else{_tmpCFF=_tmpCFE->f1;}}_LL812:
# 5262
 return Cyc_Tcutil_cnst_exp(1,_tmpCFF);_LL813: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD00=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpD00->tag != 26)goto _LL815;else{_tmpD01=_tmpD00->f2;_tmpD02=_tmpD00->f3;}}_LL814:
# 5264
 return Cyc_Tcutil_cnst_exp(0,_tmpD01) && Cyc_Tcutil_cnst_exp(0,_tmpD02);_LL815: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpD03=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpD03->tag != 25)goto _LL817;else{_tmpD04=_tmpD03->f1;}}_LL816:
 _tmpD06=_tmpD04;goto _LL818;_LL817: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD05=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpD05->tag != 28)goto _LL819;else{_tmpD06=_tmpD05->f2;}}_LL818:
 _tmpD08=_tmpD06;goto _LL81A;_LL819: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD07=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpD07->tag != 27)goto _LL81B;else{_tmpD08=_tmpD07->f3;}}_LL81A:
# 5268
 for(0;_tmpD08 != 0;_tmpD08=_tmpD08->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpD08->hd)).f2))
return 0;}
return 1;_LL81B: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD09=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpD09->tag != 2)goto _LL81D;else{_tmpD0A=_tmpD09->f1;_tmpD0B=_tmpD09->f2;}}_LL81C:
# 5273
 _tmpD0D=_tmpD0B;goto _LL81E;_LL81D: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpD0C=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpD0C->tag != 23)goto _LL81F;else{_tmpD0D=_tmpD0C->f1;}}_LL81E:
 _tmpD0F=_tmpD0D;goto _LL820;_LL81F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD0E=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpCE3;if(_tmpD0E->tag != 29)goto _LL821;else{_tmpD0F=_tmpD0E->f1;}}_LL820:
# 5276
 for(0;_tmpD0F != 0;_tmpD0F=_tmpD0F->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpD0F->hd))
return 0;}
return 1;_LL821:;_LL822:
 return 0;_LL7F6:;}
# 5284
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5288
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5290
int Cyc_Tcutil_supports_default(void*t){
void*_tmpD1D=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD22;union Cyc_Absyn_Constraint*_tmpD23;void*_tmpD25;struct Cyc_List_List*_tmpD27;union Cyc_Absyn_AggrInfoU _tmpD29;struct Cyc_List_List*_tmpD2A;struct Cyc_List_List*_tmpD2C;_LL83D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD1E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD1D;if(_tmpD1E->tag != 0)goto _LL83F;}_LL83E:
 goto _LL840;_LL83F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD1F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD1D;if(_tmpD1F->tag != 6)goto _LL841;}_LL840:
 goto _LL842;_LL841: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD20=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD1D;if(_tmpD20->tag != 7)goto _LL843;}_LL842:
 return 1;_LL843: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD21=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1D;if(_tmpD21->tag != 5)goto _LL845;else{_tmpD22=((_tmpD21->f1).ptr_atts).nullable;_tmpD23=((_tmpD21->f1).ptr_atts).bounds;}}_LL844: {
# 5297
void*_tmpD2F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD23);_LL854: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD30=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD2F;if(_tmpD30->tag != 0)goto _LL856;}_LL855:
 return 1;_LL856:;_LL857:
# 5300
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpD22);_LL853:;}_LL845: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD24=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD1D;if(_tmpD24->tag != 8)goto _LL847;else{_tmpD25=(_tmpD24->f1).elt_type;}}_LL846:
# 5303
 return Cyc_Tcutil_supports_default(_tmpD25);_LL847: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD26=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD1D;if(_tmpD26->tag != 10)goto _LL849;else{_tmpD27=_tmpD26->f1;}}_LL848:
# 5305
 for(0;_tmpD27 != 0;_tmpD27=_tmpD27->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpD27->hd)).f2))return 0;}
return 1;_LL849: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD28=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD1D;if(_tmpD28->tag != 11)goto _LL84B;else{_tmpD29=(_tmpD28->f1).aggr_info;_tmpD2A=(_tmpD28->f1).targs;}}_LL84A: {
# 5309
struct Cyc_Absyn_Aggrdecl*_tmpD31=Cyc_Absyn_get_known_aggrdecl(_tmpD29);
if(_tmpD31->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD31->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpD31->tvs,_tmpD2A,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD31->impl))->fields);}_LL84B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD2B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD1D;if(_tmpD2B->tag != 12)goto _LL84D;else{_tmpD2C=_tmpD2B->f2;}}_LL84C:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpD2C);_LL84D: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD2D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD1D;if(_tmpD2D->tag != 14)goto _LL84F;}_LL84E:
# 5315
 goto _LL850;_LL84F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD2E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD1D;if(_tmpD2E->tag != 13)goto _LL851;}_LL850:
 return 1;_LL851:;_LL852:
# 5318
 return 0;_LL83C:;}
# 5323
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpD32=t;struct Cyc_Absyn_Typedefdecl*_tmpD34;_LL859: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpD33=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpD32;if(_tmpD33->tag != 17)goto _LL85B;else{_tmpD34=_tmpD33->f3;}}_LL85A:
# 5326
 if(_tmpD34 != 0){
struct Cyc_Absyn_Tqual _tmpD35=_tmpD34->tq;
if(((_tmpD35.print_const  || _tmpD35.q_volatile) || _tmpD35.q_restrict) || _tmpD35.real_const){
# 5331
const char*_tmp1312;void*_tmp1311[1];struct Cyc_String_pa_PrintArg_struct _tmp1310;(_tmp1310.tag=0,((_tmp1310.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1311[0]=& _tmp1310,Cyc_Tcutil_warn(loc,((_tmp1312="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp1312,sizeof(char),44))),_tag_dyneither(_tmp1311,sizeof(void*),1)))))));}}
# 5334
goto _LL858;_LL85B:;_LL85C:
 goto _LL858;_LL858:;}
# 5339
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5341
struct _RegionHandle _tmpD39=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD39;_push_region(rgn);{
struct Cyc_List_List*_tmpD3A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpD3B=1;_npop_handler(0);return _tmpD3B;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpD3A,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpD3C=0;_npop_handler(0);return _tmpD3C;}}}{
# 5350
int _tmpD3D=1;_npop_handler(0);return _tmpD3D;};
# 5341
;_pop_region(rgn);}
# 5356
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpD3E=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD42;union Cyc_Absyn_Constraint*_tmpD43;_LL85E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD3F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD3E;if(_tmpD3F->tag != 6)goto _LL860;}_LL85F:
 goto _LL861;_LL860: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD40=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD3E;if(_tmpD40->tag != 7)goto _LL862;}_LL861:
 return 1;_LL862: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD41=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD3E;if(_tmpD41->tag != 5)goto _LL864;else{_tmpD42=((_tmpD41->f1).ptr_atts).nullable;_tmpD43=((_tmpD41->f1).ptr_atts).bounds;}}_LL863: {
# 5361
void*_tmpD44=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD43);_LL867: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD45=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD44;if(_tmpD45->tag != 0)goto _LL869;}_LL868:
# 5365
 return 0;_LL869:;_LL86A:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD42);_LL866:;}_LL864:;_LL865:
# 5368
 return 0;_LL85D:;}
# 5372
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpD46=Cyc_Tcutil_compress(t);void*_tmpD48;struct Cyc_List_List*_tmpD4A;_LL86C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD47=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD46;if(_tmpD47->tag != 5)goto _LL86E;else{_tmpD48=(_tmpD47->f1).elt_typ;}}_LL86D:
 return Cyc_Tcutil_is_noreturn(_tmpD48);_LL86E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD49=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD46;if(_tmpD49->tag != 9)goto _LL870;else{_tmpD4A=(_tmpD49->f1).attributes;}}_LL86F:
# 5376
 for(0;_tmpD4A != 0;_tmpD4A=_tmpD4A->tl){
void*_tmpD4B=(void*)_tmpD4A->hd;_LL873: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpD4C=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpD4B;if(_tmpD4C->tag != 4)goto _LL875;}_LL874:
 return 1;_LL875:;_LL876:
 continue;_LL872:;}
# 5381
goto _LL86B;_LL870:;_LL871:
 goto _LL86B;_LL86B:;}
# 5384
return 0;}
# 5389
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpD4D=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpD4F;_LL878: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD4E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD4D;if(_tmpD4E->tag != 9)goto _LL87A;else{_tmpD4F=(struct Cyc_List_List**)&(_tmpD4E->f1).attributes;}}_LL879: {
# 5392
struct Cyc_List_List*_tmpD50=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1313;*_tmpD4F=((_tmp1313=_cycalloc(sizeof(*_tmp1313)),((_tmp1313->hd=(void*)atts->hd,((_tmp1313->tl=*_tmpD4F,_tmp1313))))));}else{
# 5397
struct Cyc_List_List*_tmp1314;_tmpD50=((_tmp1314=_cycalloc(sizeof(*_tmp1314)),((_tmp1314->hd=(void*)atts->hd,((_tmp1314->tl=_tmpD50,_tmp1314))))));}}
return _tmpD50;}_LL87A:;_LL87B: {
const char*_tmp1317;void*_tmp1316;(_tmp1316=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1317="transfer_fn_type_atts",_tag_dyneither(_tmp1317,sizeof(char),22))),_tag_dyneither(_tmp1316,sizeof(void*),0)));}_LL877:;}
