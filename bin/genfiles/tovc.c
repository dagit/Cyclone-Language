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
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 282 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 50 "string.h"
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 433 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
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
# 905 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 951
extern struct _tuple0*Cyc_Absyn_exn_name;
# 982
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_RgnOrder_RgnPO;
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
# 65 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 338 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 35 "toc.h"
struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);
# 79 "tovc.cyc"
int Cyc_Tovc_elim_array_initializers=1;struct Cyc_Tovc_BoxingEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};
# 92
typedef struct Cyc_Tovc_BoxingEnv Cyc_Tovc_boxingenv_t;struct Cyc_Tovc_ToExpEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};
# 101
typedef struct Cyc_Tovc_ToExpEnv Cyc_Tovc_toexpenv_t;struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 103
static void Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp0=e->r;struct _dyneither_ptr*_tmp2;struct Cyc_Absyn_Exp*_tmp6;struct Cyc_List_List*_tmp7;struct Cyc_List_List*_tmp9;struct Cyc_Absyn_Exp*_tmpB;struct Cyc_Absyn_Exp*_tmpC;struct Cyc_Absyn_Exp*_tmpD;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Exp*_tmp2E;struct Cyc_List_List*_tmp33;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1->tag != 1)goto _LL3;else{if((((_tmp1->f1)->f1).Loc_n).tag != 4)goto _LL3;_tmp2=(_tmp1->f1)->f2;}}_LL2:
# 109
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(env.varmap,_tmp2)){
e->r=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(env.varmap,_tmp2))->r;
continue;}
# 114
{int i=0;for(0;i < _get_dyneither_size(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*));++ i){
if(Cyc_strptrcmp(_tmp2,*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*),i)))== 0)
return;}}
# 118
for(0;env.boundvars != 0;env.boundvars=(env.boundvars)->tl){
if(Cyc_strptrcmp(_tmp2,(*((struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)== 0)
return;}
# 122
for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
struct Cyc_Absyn_Vardecl*_tmp34=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;
if(Cyc_strptrcmp(_tmp2,(*_tmp34->name).f2)== 0){
# 126
{void*_tmp35=_tmp34->type;_LL34: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35;if(_tmp36->tag != 8)goto _LL36;}_LL35:
 goto _LL33;_LL36:;_LL37:
 e->r=(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp34->name,0),0))->r;goto _LL33;_LL33:;}
# 130
*env.freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Absyn_Vardecl*elt))Cyc_Set_rinsert)(env.rgn,*env.freevars,_tmp34);
return;}}
# 137
for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
if(Cyc_strptrcmp(_tmp2,(*((struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd)->name).f2)== 0){
struct Cyc_Core_Impossible_exn_struct _tmp173;const char*_tmp172;void*_tmp171[1];struct Cyc_String_pa_PrintArg_struct _tmp170;struct Cyc_Core_Impossible_exn_struct*_tmp16F;(int)_throw((void*)((_tmp16F=_cycalloc(sizeof(*_tmp16F)),((_tmp16F[0]=((_tmp173.tag=Cyc_Core_Impossible,((_tmp173.f1=(struct _dyneither_ptr)((_tmp170.tag=0,((_tmp170.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2),((_tmp171[0]=& _tmp170,Cyc_aprintf(((_tmp172="unbound variable %s found in box_free_vars_exp",_tag_dyneither(_tmp172,sizeof(char),47))),_tag_dyneither(_tmp171,sizeof(void*),1)))))))),_tmp173)))),_tmp16F)))));}}
# 144
return;_LL3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp3->tag != 1)goto _LL5;}_LL4:
# 147
 return;_LL5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp4->tag != 0)goto _LL7;}_LL6:
# 150
 return;_LL7: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp5->tag != 9)goto _LL9;else{_tmp6=_tmp5->f1;_tmp7=_tmp5->f2;}}_LL8:
# 152
 Cyc_Tovc_box_free_vars_exp(env,_tmp6);_tmp9=_tmp7;goto _LLA;_LL9: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp8=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp8->tag != 2)goto _LLB;else{_tmp9=_tmp8->f2;}}_LLA:
# 154
 for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_tmp9->hd);}
return;_LLB: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmpA->tag != 5)goto _LLD;else{_tmpB=_tmpA->f1;_tmpC=_tmpA->f2;_tmpD=_tmpA->f3;}}_LLC:
# 159
 Cyc_Tovc_box_free_vars_exp(env,_tmpD);_tmpF=_tmpB;_tmp10=_tmpC;goto _LLE;_LLD: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpE=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmpE->tag != 6)goto _LLF;else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LLE:
 _tmp12=_tmpF;_tmp13=_tmp10;goto _LL10;_LLF: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp11=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp11->tag != 7)goto _LL11;else{_tmp12=_tmp11->f1;_tmp13=_tmp11->f2;}}_LL10:
 _tmp15=_tmp12;_tmp16=_tmp13;goto _LL12;_LL11: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp14=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp14->tag != 22)goto _LL13;else{_tmp15=_tmp14->f1;_tmp16=_tmp14->f2;}}_LL12:
 _tmp18=_tmp15;_tmp19=_tmp16;goto _LL14;_LL13: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp17=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp17->tag != 8)goto _LL15;else{_tmp18=_tmp17->f1;_tmp19=_tmp17->f2;}}_LL14:
 _tmp1B=_tmp18;_tmp1C=_tmp19;goto _LL16;_LL15: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1A=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1A->tag != 3)goto _LL17;else{_tmp1B=_tmp1A->f1;_tmp1C=_tmp1A->f3;}}_LL16:
# 165
 Cyc_Tovc_box_free_vars_exp(env,_tmp1C);_tmp1E=_tmp1B;goto _LL18;_LL17: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1D->tag != 11)goto _LL19;else{_tmp1E=_tmp1D->f1;}}_LL18:
# 167
 _tmp20=_tmp1E;goto _LL1A;_LL19: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1F->tag != 12)goto _LL1B;else{_tmp20=_tmp1F->f1;}}_LL1A:
 _tmp22=_tmp20;goto _LL1C;_LL1B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp21=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp21->tag != 13)goto _LL1D;else{_tmp22=_tmp21->f2;}}_LL1C:
 _tmp24=_tmp22;goto _LL1E;_LL1D: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp23=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp23->tag != 14)goto _LL1F;else{_tmp24=_tmp23->f1;}}_LL1E:
 _tmp26=_tmp24;goto _LL20;_LL1F: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp25=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp25->tag != 17)goto _LL21;else{_tmp26=_tmp25->f1;}}_LL20:
 _tmp28=_tmp26;goto _LL22;_LL21: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp27=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp27->tag != 19)goto _LL23;else{_tmp28=_tmp27->f1;}}_LL22:
 _tmp2A=_tmp28;goto _LL24;_LL23: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp29=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp29->tag != 20)goto _LL25;else{_tmp2A=_tmp29->f1;}}_LL24:
 _tmp2C=_tmp2A;goto _LL26;_LL25: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2B->tag != 21)goto _LL27;else{_tmp2C=_tmp2B->f1;}}_LL26:
 _tmp2E=_tmp2C;goto _LL28;_LL27: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2D=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2D->tag != 4)goto _LL29;else{_tmp2E=_tmp2D->f1;}}_LL28:
# 176
 Cyc_Tovc_box_free_vars_exp(env,_tmp2E);
return;_LL29: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp2F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2F->tag != 16)goto _LL2B;}_LL2A:
# 179
 goto _LL2C;_LL2B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp30=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp30->tag != 18)goto _LL2D;}_LL2C:
 goto _LL2E;_LL2D: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp31=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp31->tag != 30)goto _LL2F;}_LL2E:
 return;_LL2F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp32=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp32->tag != 34)goto _LL31;else{_tmp33=_tmp32->f2;}}_LL30:
# 184
 for(0;_tmp33 != 0;_tmp33=_tmp33->tl){
Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple10*)_tmp33->hd)).f2);}
return;_LL31:;_LL32: {
# 188
struct Cyc_Core_Impossible_exn_struct _tmp179;const char*_tmp178;struct Cyc_Core_Impossible_exn_struct*_tmp177;(int)_throw((void*)((_tmp177=_cycalloc(sizeof(*_tmp177)),((_tmp177[0]=((_tmp179.tag=Cyc_Core_Impossible,((_tmp179.f1=((_tmp178="bad exp form in Tocv::box_free_vars_exp",_tag_dyneither(_tmp178,sizeof(char),40))),_tmp179)))),_tmp177)))));}_LL0:;}}
# 192
static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){
# 196
while(1){
void*_tmp3F=s->r;struct Cyc_Absyn_Stmt*_tmp45;struct Cyc_Absyn_Exp*_tmp47;struct Cyc_Absyn_Exp*_tmp49;struct Cyc_Absyn_Stmt*_tmp4B;struct Cyc_Absyn_Stmt*_tmp4C;struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_Absyn_Stmt*_tmp4F;struct Cyc_Absyn_Stmt*_tmp50;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Stmt*_tmp53;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Exp*_tmp56;struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_Stmt*_tmp58;struct Cyc_Absyn_Stmt*_tmp5A;struct Cyc_Absyn_Exp*_tmp5B;struct Cyc_Absyn_Exp*_tmp5D;struct Cyc_List_List*_tmp5E;struct Cyc_Absyn_Decl*_tmp60;struct Cyc_Absyn_Stmt*_tmp61;_LL39: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp40=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp40->tag != 0)goto _LL3B;}_LL3A:
 goto _LL3C;_LL3B: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp41=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp41->tag != 6)goto _LL3D;}_LL3C:
 goto _LL3E;_LL3D: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp42=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp42->tag != 7)goto _LL3F;}_LL3E:
 return;_LL3F: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp43=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp43->tag != 8)goto _LL41;}_LL40:
# 203
 return;_LL41: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp44=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp44->tag != 13)goto _LL43;else{_tmp45=_tmp44->f2;}}_LL42:
# 205
 s=_tmp45;
continue;_LL43: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp46=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp46->tag != 3)goto _LL45;else{_tmp47=_tmp46->f1;}}_LL44:
# 209
 if(_tmp47 == 0)
return;
_tmp49=_tmp47;goto _LL46;_LL45: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp48=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp48->tag != 1)goto _LL47;else{_tmp49=_tmp48->f1;}}_LL46:
# 213
 Cyc_Tovc_box_free_vars_exp(env,_tmp49);
return;_LL47: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp4A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp4A->tag != 2)goto _LL49;else{_tmp4B=_tmp4A->f1;_tmp4C=_tmp4A->f2;}}_LL48:
# 216
 Cyc_Tovc_box_free_vars_stmt(env,_tmp4B);
s=_tmp4C;
continue;_LL49: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp4D=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp4D->tag != 4)goto _LL4B;else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4D->f2;_tmp50=_tmp4D->f3;}}_LL4A:
# 220
 Cyc_Tovc_box_free_vars_exp(env,_tmp4E);
Cyc_Tovc_box_free_vars_stmt(env,_tmp4F);
s=_tmp50;
continue;_LL4B: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp51=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp51->tag != 5)goto _LL4D;else{_tmp52=(_tmp51->f1).f1;_tmp53=_tmp51->f2;}}_LL4C:
# 225
 Cyc_Tovc_box_free_vars_exp(env,_tmp52);
s=_tmp53;
continue;_LL4D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp54=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp54->tag != 9)goto _LL4F;else{_tmp55=_tmp54->f1;_tmp56=(_tmp54->f2).f1;_tmp57=(_tmp54->f3).f1;_tmp58=_tmp54->f4;}}_LL4E:
# 229
 Cyc_Tovc_box_free_vars_exp(env,_tmp55);
Cyc_Tovc_box_free_vars_exp(env,_tmp56);
Cyc_Tovc_box_free_vars_exp(env,_tmp57);
s=_tmp58;
continue;_LL4F: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp59=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp59->tag != 14)goto _LL51;else{_tmp5A=_tmp59->f1;_tmp5B=(_tmp59->f2).f1;}}_LL50:
# 235
 Cyc_Tovc_box_free_vars_exp(env,_tmp5B);
s=_tmp5A;
continue;_LL51: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5C=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp5C->tag != 10)goto _LL53;else{_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;}}_LL52:
# 240
 Cyc_Tovc_box_free_vars_exp(env,_tmp5D);
for(0;_tmp5E != 0;_tmp5E=_tmp5E->tl){
Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp5E->hd)->body);}
return;_LL53: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp5F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F;if(_tmp5F->tag != 12)goto _LL55;else{_tmp60=_tmp5F->f1;_tmp61=_tmp5F->f2;}}_LL54:
# 245
{void*_tmp62=_tmp60->r;struct Cyc_Absyn_Vardecl*_tmp64;_LL58: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp63=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp62;if(_tmp63->tag != 0)goto _LL5A;else{_tmp64=_tmp63->f1;}}_LL59:
# 247
{struct Cyc_List_List*_tmp17A;env.boundvars=((_tmp17A=_region_malloc(env.rgn,sizeof(*_tmp17A)),((_tmp17A->hd=_tmp64,((_tmp17A->tl=env.boundvars,_tmp17A))))));}
env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete_same)(env.varmap,(*_tmp64->name).f2);
if(_tmp64->initializer != 0)
Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp64->initializer));
s=_tmp61;
continue;_LL5A:;_LL5B:
 goto _LL57;_LL57:;}
# 255
goto _LL56;_LL55:;_LL56: {
struct Cyc_Core_Impossible_exn_struct _tmp180;const char*_tmp17F;struct Cyc_Core_Impossible_exn_struct*_tmp17E;(int)_throw((void*)((_tmp17E=_cycalloc(sizeof(*_tmp17E)),((_tmp17E[0]=((_tmp180.tag=Cyc_Core_Impossible,((_tmp180.f1=((_tmp17F="bad stmt after xlation to C",_tag_dyneither(_tmp17F,sizeof(char),28))),_tmp180)))),_tmp17E)))));}_LL38:;}}
# 267
static int Cyc_Tovc_stmt_to_fun_cmp(struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){
return Cyc_Absyn_qvar_cmp(x->name,y->name);}
# 271
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){
# 274
struct Cyc_Set_Set*freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty)(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
struct Cyc_Tovc_BoxingEnv _tmp181;struct Cyc_Tovc_BoxingEnv _tmp69=(_tmp181.rgn=env.rgn,((_tmp181.all_locals=env.all_locals,((_tmp181.varmap=env.varmap,((_tmp181.boundvars=0,((_tmp181.freevars=& freevars,_tmp181)))))))));
Cyc_Tovc_box_free_vars_stmt(_tmp69,s);{
struct Cyc_List_List*params=0;
struct Cyc_List_List*args=0;
# 280
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,freevars);
# 282
struct Cyc_Absyn_Vardecl*_tmp182;struct Cyc_Absyn_Vardecl*_tmp6A=(_tmp182=_cycalloc(sizeof(*_tmp182)),((_tmp182->sc=Cyc_Absyn_Public,((_tmp182->name=Cyc_Absyn_exn_name,((_tmp182->tq=Cyc_Absyn_empty_tqual(0),((_tmp182->type=(void*)& Cyc_Absyn_VoidType_val,((_tmp182->initializer=0,((_tmp182->rgn=0,((_tmp182->attributes=0,((_tmp182->escapes=0,_tmp182)))))))))))))))));
# 285
while(((int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(iter,& _tmp6A)){
void*_tmp6B=_tmp6A->type;void*_tmp6D;_LL5D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B;if(_tmp6C->tag != 8)goto _LL5F;else{_tmp6D=(_tmp6C->f1).elt_type;}}_LL5E:
# 288
{struct _tuple8*_tmp185;struct Cyc_List_List*_tmp184;params=((_tmp184=_cycalloc(sizeof(*_tmp184)),((_tmp184->hd=((_tmp185=_cycalloc(sizeof(*_tmp185)),((_tmp185->f1=(*_tmp6A->name).f2,((_tmp185->f2=_tmp6A->tq,((_tmp185->f3=
Cyc_Absyn_cstar_typ(_tmp6D,Cyc_Absyn_empty_tqual(0)),_tmp185)))))))),((_tmp184->tl=params,_tmp184))))));}
# 291
{struct Cyc_List_List*_tmp186;args=((_tmp186=_cycalloc(sizeof(*_tmp186)),((_tmp186->hd=Cyc_Absyn_var_exp(_tmp6A->name,0),((_tmp186->tl=args,_tmp186))))));}
goto _LL5C;_LL5F:;_LL60:
# 294
{struct _tuple8*_tmp189;struct Cyc_List_List*_tmp188;params=((_tmp188=_cycalloc(sizeof(*_tmp188)),((_tmp188->hd=((_tmp189=_cycalloc(sizeof(*_tmp189)),((_tmp189->f1=(*_tmp6A->name).f2,((_tmp189->f2=_tmp6A->tq,((_tmp189->f3=
Cyc_Absyn_cstar_typ(_tmp6A->type,Cyc_Absyn_empty_tqual(0)),_tmp189)))))))),((_tmp188->tl=params,_tmp188))))));}
# 297
{struct Cyc_List_List*_tmp18A;args=((_tmp18A=_cycalloc(sizeof(*_tmp18A)),((_tmp18A->hd=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(_tmp6A->name,0),0),((_tmp18A->tl=args,_tmp18A))))));}
goto _LL5C;_LL5C:;}{
# 300
struct _tuple0*funname=Cyc_Toc_temp_var();
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp194;struct Cyc_Absyn_Fndecl*_tmp193;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp192;struct Cyc_List_List*_tmp191;*env.gen_ds=((_tmp191=_cycalloc(sizeof(*_tmp191)),((_tmp191->hd=Cyc_Absyn_new_decl((void*)((_tmp194=_cycalloc(sizeof(*_tmp194)),((_tmp194[0]=((_tmp192.tag=1,((_tmp192.f1=((_tmp193=_cycalloc(sizeof(*_tmp193)),((_tmp193->sc=Cyc_Absyn_Static,((_tmp193->is_inline=0,((_tmp193->name=funname,((_tmp193->tvs=0,((_tmp193->effect=0,((_tmp193->ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp193->ret_type=rettype,((_tmp193->args=params,((_tmp193->c_varargs=0,((_tmp193->cyc_varargs=0,((_tmp193->rgn_po=0,((_tmp193->body=s,((_tmp193->cached_typ=0,((_tmp193->param_vardecls=0,((_tmp193->fn_vardecl=0,((_tmp193->attributes=0,_tmp193)))))))))))))))))))))))))))))))))),_tmp192)))),_tmp194)))),0),((_tmp191->tl=*env.gen_ds,_tmp191))))));}
# 307
return Cyc_Absyn_fncall_exp(Cyc_Absyn_var_exp(funname,0),args,0);};};}
# 310
static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){
void*_tmp7A=e->r;struct _dyneither_ptr*_tmp7C;struct Cyc_Absyn_Exp*_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp83;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_List_List*_tmpAD;_LL62: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp7B->tag != 1)goto _LL64;else{if((((_tmp7B->f1)->f1).Loc_n).tag != 4)goto _LL64;_tmp7C=(_tmp7B->f1)->f2;}}_LL63:
# 315
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(varmap,_tmp7C))
e->r=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(varmap,_tmp7C))->r;
return;_LL64: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp7D->tag != 1)goto _LL66;}_LL65:
 return;_LL66: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp7E->tag != 0)goto _LL68;}_LL67:
# 321
 return;_LL68: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7F=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp7F->tag != 9)goto _LL6A;else{_tmp80=_tmp7F->f1;_tmp81=_tmp7F->f2;}}_LL69:
# 323
 Cyc_Tovc_apply_varmap(varmap,_tmp80);_tmp83=_tmp81;goto _LL6B;_LL6A: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp82=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp82->tag != 2)goto _LL6C;else{_tmp83=_tmp82->f2;}}_LL6B:
# 325
 for(0;_tmp83 != 0;_tmp83=_tmp83->tl){
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)_tmp83->hd);}
return;_LL6C: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp84=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp84->tag != 5)goto _LL6E;else{_tmp85=_tmp84->f1;_tmp86=_tmp84->f2;_tmp87=_tmp84->f3;}}_LL6D:
# 330
 Cyc_Tovc_apply_varmap(varmap,_tmp87);_tmp89=_tmp85;_tmp8A=_tmp86;goto _LL6F;_LL6E: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp88=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp88->tag != 6)goto _LL70;else{_tmp89=_tmp88->f1;_tmp8A=_tmp88->f2;}}_LL6F:
 _tmp8C=_tmp89;_tmp8D=_tmp8A;goto _LL71;_LL70: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp8B=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp8B->tag != 7)goto _LL72;else{_tmp8C=_tmp8B->f1;_tmp8D=_tmp8B->f2;}}_LL71:
 _tmp8F=_tmp8C;_tmp90=_tmp8D;goto _LL73;_LL72: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp8E=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp8E->tag != 22)goto _LL74;else{_tmp8F=_tmp8E->f1;_tmp90=_tmp8E->f2;}}_LL73:
 _tmp92=_tmp8F;_tmp93=_tmp90;goto _LL75;_LL74: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp91=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp91->tag != 8)goto _LL76;else{_tmp92=_tmp91->f1;_tmp93=_tmp91->f2;}}_LL75:
 _tmp95=_tmp92;_tmp96=_tmp93;goto _LL77;_LL76: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp94=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp94->tag != 3)goto _LL78;else{_tmp95=_tmp94->f1;_tmp96=_tmp94->f3;}}_LL77:
# 336
 Cyc_Tovc_apply_varmap(varmap,_tmp96);_tmp98=_tmp95;goto _LL79;_LL78: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp97=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp97->tag != 11)goto _LL7A;else{_tmp98=_tmp97->f1;}}_LL79:
# 338
 _tmp9A=_tmp98;goto _LL7B;_LL7A: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp99=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp99->tag != 12)goto _LL7C;else{_tmp9A=_tmp99->f1;}}_LL7B:
 _tmp9C=_tmp9A;goto _LL7D;_LL7C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp9B->tag != 13)goto _LL7E;else{_tmp9C=_tmp9B->f2;}}_LL7D:
 _tmp9E=_tmp9C;goto _LL7F;_LL7E: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp9D=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp9D->tag != 14)goto _LL80;else{_tmp9E=_tmp9D->f1;}}_LL7F:
 _tmpA0=_tmp9E;goto _LL81;_LL80: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp9F=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmp9F->tag != 17)goto _LL82;else{_tmpA0=_tmp9F->f1;}}_LL81:
 _tmpA2=_tmpA0;goto _LL83;_LL82: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA1=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmpA1->tag != 19)goto _LL84;else{_tmpA2=_tmpA1->f1;}}_LL83:
 _tmpA4=_tmpA2;goto _LL85;_LL84: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA3=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmpA3->tag != 20)goto _LL86;else{_tmpA4=_tmpA3->f1;}}_LL85:
 _tmpA6=_tmpA4;goto _LL87;_LL86: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA5=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmpA5->tag != 21)goto _LL88;else{_tmpA6=_tmpA5->f1;}}_LL87:
 _tmpA8=_tmpA6;goto _LL89;_LL88: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpA7=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmpA7->tag != 4)goto _LL8A;else{_tmpA8=_tmpA7->f1;}}_LL89:
# 347
 Cyc_Tovc_apply_varmap(varmap,_tmpA8);
return;_LL8A: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA9=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmpA9->tag != 16)goto _LL8C;}_LL8B:
# 350
 goto _LL8D;_LL8C: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpAA=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmpAA->tag != 18)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpAB=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmpAB->tag != 30)goto _LL90;}_LL8F:
 return;_LL90: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpAC=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp7A;if(_tmpAC->tag != 34)goto _LL92;else{_tmpAD=_tmpAC->f2;}}_LL91:
# 355
 for(0;_tmpAD != 0;_tmpAD=_tmpAD->tl){
Cyc_Tovc_apply_varmap(varmap,(*((struct _tuple10*)_tmpAD->hd)).f2);}
return;_LL92:;_LL93: {
# 359
struct Cyc_Core_Impossible_exn_struct _tmp19A;const char*_tmp199;struct Cyc_Core_Impossible_exn_struct*_tmp198;(int)_throw((void*)((_tmp198=_cycalloc(sizeof(*_tmp198)),((_tmp198[0]=((_tmp19A.tag=Cyc_Core_Impossible,((_tmp19A.f1=((_tmp199="bad exp form in Tocv::apply_varmap",_tag_dyneither(_tmp199,sizeof(char),35))),_tmp19A)))),_tmp198)))));}_LL61:;}
# 363
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
void*_tmpB1=s->r;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Stmt*_tmpB6;struct Cyc_Absyn_Stmt*_tmpB7;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Stmt*_tmpBA;struct Cyc_Absyn_Stmt*_tmpBB;struct Cyc_Absyn_Decl*_tmpBD;struct Cyc_Absyn_Stmt*_tmpBE;_LL95: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpB2=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmpB1;if(_tmpB2->tag != 0)goto _LL97;}_LL96:
 return Cyc_Absyn_true_exp(0);_LL97: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpB3=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpB1;if(_tmpB3->tag != 1)goto _LL99;else{_tmpB4=_tmpB3->f1;}}_LL98:
# 367
 Cyc_Tovc_apply_varmap(env.varmap,_tmpB4);
return _tmpB4;_LL99: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpB5=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpB1;if(_tmpB5->tag != 2)goto _LL9B;else{_tmpB6=_tmpB5->f1;_tmpB7=_tmpB5->f2;}}_LL9A: {
# 371
struct Cyc_Absyn_Exp*_tmpBF=Cyc_Tovc_stmt_to_exp(env,_tmpB6);
struct Cyc_Absyn_Exp*_tmpC0=Cyc_Tovc_stmt_to_exp(env,_tmpB7);
return Cyc_Absyn_seq_exp(_tmpBF,_tmpC0,0);}_LL9B: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmpB8=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpB1;if(_tmpB8->tag != 4)goto _LL9D;else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB8->f2;_tmpBB=_tmpB8->f3;}}_LL9C:
# 375
 Cyc_Tovc_apply_varmap(env.varmap,_tmpB9);{
# 377
struct Cyc_Absyn_Exp*_tmpC1=Cyc_Tovc_stmt_to_exp(env,_tmpBA);
struct Cyc_Absyn_Exp*_tmpC2=Cyc_Tovc_stmt_to_exp(env,_tmpBB);
return Cyc_Absyn_conditional_exp(_tmpB9,_tmpC1,_tmpC2,0);};_LL9D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpBC=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpB1;if(_tmpBC->tag != 12)goto _LL9F;else{_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;}}_LL9E: {
# 381
void*_tmpC3=_tmpBD->r;struct Cyc_Absyn_Vardecl*_tmpC5;_LLA2: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC4=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC3;if(_tmpC4->tag != 0)goto _LLA4;else{_tmpC5=_tmpC4->f1;}}_LLA3: {
# 384
struct _dyneither_ptr*_tmpC6=(*_tmpC5->name).f2;
struct _tuple0*_tmpC7=Cyc_Toc_temp_var();
_tmpC5->name=_tmpC7;
env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(env.varmap,_tmpC6,Cyc_Absyn_var_exp(_tmpC7,0));{
struct Cyc_List_List*_tmpC8=0;
if(_tmpC5->initializer != 0){
Cyc_Tovc_apply_varmap(env.varmap,(struct Cyc_Absyn_Exp*)_check_null(_tmpC5->initializer));{
void*_tmpC9=((struct Cyc_Absyn_Exp*)_check_null(_tmpC5->initializer))->r;struct Cyc_List_List*_tmpCB;_LLA7: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCA=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpC9;if(_tmpCA->tag != 34)goto _LLA9;else{_tmpCB=_tmpCA->f2;}}_LLA8: {
# 403 "tovc.cyc"
struct Cyc_Absyn_Exp*_tmpCC=Cyc_Absyn_var_exp(_tmpC5->name,0);
{unsigned int i=0;for(0;_tmpCB != 0;(_tmpCB=_tmpCB->tl,++ i)){
struct Cyc_List_List*_tmp19B;_tmpC8=((_tmp19B=_cycalloc(sizeof(*_tmp19B)),((_tmp19B->hd=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(_tmpCC,Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple10*)_tmpCB->hd)).f2,0),((_tmp19B->tl=_tmpC8,_tmp19B))))));}}
# 409
_tmpC5->initializer=0;
goto _LLA6;}_LLA9:;_LLAA:
 goto _LLA6;_LLA6:;};}
# 414
{struct Cyc_List_List*_tmp19C;env.all_locals=((_tmp19C=_region_malloc(env.rgn,sizeof(*_tmp19C)),((_tmp19C->hd=_tmpC5,((_tmp19C->tl=env.all_locals,_tmp19C))))));}
# 416
(env.encloser)->r=(Cyc_Absyn_decl_stmt(_tmpBD,Cyc_Absyn_new_stmt((env.encloser)->r,0),0))->r;{
struct Cyc_Absyn_Exp*_tmpCF=Cyc_Tovc_stmt_to_exp(env,_tmpBE);
if(_tmpC5->initializer != 0){
_tmpCF=Cyc_Absyn_seq_exp(Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmpC5->name,0),(struct Cyc_Absyn_Exp*)_check_null(_tmpC5->initializer),0),_tmpCF,0);
# 421
_tmpC5->initializer=0;}
# 423
for(0;_tmpC8 != 0;_tmpC8=_tmpC8->tl){
_tmpCF=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)_tmpC8->hd,_tmpCF,0);}
return _tmpCF;};};}_LLA4:;_LLA5: {
struct Cyc_Core_Impossible_exn_struct _tmp1A2;const char*_tmp1A1;struct Cyc_Core_Impossible_exn_struct*_tmp1A0;(int)_throw((void*)((_tmp1A0=_cycalloc(sizeof(*_tmp1A0)),((_tmp1A0[0]=((_tmp1A2.tag=Cyc_Core_Impossible,((_tmp1A2.f1=((_tmp1A1="bad local decl in Tovc::stmt_to_exp",_tag_dyneither(_tmp1A1,sizeof(char),36))),_tmp1A2)))),_tmp1A0)))));}_LLA1:;}_LL9F:;_LLA0:
# 429
 return Cyc_Tovc_stmt_to_fun(env,s,(void*)& Cyc_Absyn_VoidType_val);_LL94:;}
# 433
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);
# 435
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
void*_tmpD3=e->r;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_List_List*_tmpD8;struct Cyc_List_List*_tmpDA;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_List_List*_tmp104;struct Cyc_Absyn_Stmt*_tmp106;_LLAC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpD4->tag != 0)goto _LLAE;}_LLAD:
 return;_LLAE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpD5->tag != 1)goto _LLB0;}_LLAF:
 return;_LLB0: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpD6=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpD6->tag != 9)goto _LLB2;else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD6->f2;}}_LLB1:
# 441
 Cyc_Tovc_exp_to_vc(env,_tmpD7);_tmpDA=_tmpD8;goto _LLB3;_LLB2: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD9=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpD9->tag != 2)goto _LLB4;else{_tmpDA=_tmpD9->f2;}}_LLB3:
# 443
 for(0;_tmpDA != 0;_tmpDA=_tmpDA->tl){
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_tmpDA->hd);}
return;_LLB4: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpDB=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpDB->tag != 5)goto _LLB6;else{_tmpDC=_tmpDB->f1;_tmpDD=_tmpDB->f2;_tmpDE=_tmpDB->f3;}}_LLB5:
# 448
 Cyc_Tovc_exp_to_vc(env,_tmpDE);_tmpE0=_tmpDC;_tmpE1=_tmpDD;goto _LLB7;_LLB6: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpDF=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpDF->tag != 6)goto _LLB8;else{_tmpE0=_tmpDF->f1;_tmpE1=_tmpDF->f2;}}_LLB7:
 _tmpE3=_tmpE0;_tmpE4=_tmpE1;goto _LLB9;_LLB8: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpE2=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpE2->tag != 7)goto _LLBA;else{_tmpE3=_tmpE2->f1;_tmpE4=_tmpE2->f2;}}_LLB9:
 _tmpE6=_tmpE3;_tmpE7=_tmpE4;goto _LLBB;_LLBA: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpE5=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpE5->tag != 22)goto _LLBC;else{_tmpE6=_tmpE5->f1;_tmpE7=_tmpE5->f2;}}_LLBB:
 _tmpE9=_tmpE6;_tmpEA=_tmpE7;goto _LLBD;_LLBC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpE8->tag != 8)goto _LLBE;else{_tmpE9=_tmpE8->f1;_tmpEA=_tmpE8->f2;}}_LLBD:
 _tmpEC=_tmpE9;_tmpED=_tmpEA;goto _LLBF;_LLBE: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpEB->tag != 3)goto _LLC0;else{_tmpEC=_tmpEB->f1;_tmpED=_tmpEB->f3;}}_LLBF:
# 454
 Cyc_Tovc_exp_to_vc(env,_tmpED);_tmpEF=_tmpEC;goto _LLC1;_LLC0: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpEE=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpEE->tag != 11)goto _LLC2;else{_tmpEF=_tmpEE->f1;}}_LLC1:
# 456
 _tmpF1=_tmpEF;goto _LLC3;_LLC2: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpF0=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpF0->tag != 12)goto _LLC4;else{_tmpF1=_tmpF0->f1;}}_LLC3:
 _tmpF3=_tmpF1;goto _LLC5;_LLC4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpF2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpF2->tag != 13)goto _LLC6;else{_tmpF3=_tmpF2->f2;}}_LLC5:
 _tmpF5=_tmpF3;goto _LLC7;_LLC6: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpF4=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpF4->tag != 14)goto _LLC8;else{_tmpF5=_tmpF4->f1;}}_LLC7:
 _tmpF7=_tmpF5;goto _LLC9;_LLC8: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpF6=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpF6->tag != 17)goto _LLCA;else{_tmpF7=_tmpF6->f1;}}_LLC9:
 _tmpF9=_tmpF7;goto _LLCB;_LLCA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpF8=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpF8->tag != 19)goto _LLCC;else{_tmpF9=_tmpF8->f1;}}_LLCB:
 _tmpFB=_tmpF9;goto _LLCD;_LLCC: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpFA=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpFA->tag != 20)goto _LLCE;else{_tmpFB=_tmpFA->f1;}}_LLCD:
 _tmpFD=_tmpFB;goto _LLCF;_LLCE: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpFC=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpFC->tag != 21)goto _LLD0;else{_tmpFD=_tmpFC->f1;}}_LLCF:
 _tmpFF=_tmpFD;goto _LLD1;_LLD0: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpFE=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpFE->tag != 4)goto _LLD2;else{_tmpFF=_tmpFE->f1;}}_LLD1:
# 465
 Cyc_Tovc_exp_to_vc(env,_tmpFF);
return;_LLD2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp100=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmp100->tag != 16)goto _LLD4;}_LLD3:
# 468
 goto _LLD5;_LLD4: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmp101->tag != 18)goto _LLD6;}_LLD5:
 goto _LLD7;_LLD6: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp102=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmp102->tag != 30)goto _LLD8;}_LLD7:
 return;_LLD8: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp103=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmp103->tag != 34)goto _LLDA;else{_tmp104=_tmp103->f2;}}_LLD9:
# 476
 for(0;_tmp104 != 0;_tmp104=_tmp104->tl){
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple10*)_tmp104->hd)).f2);}
return;_LLDA: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp105=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmp105->tag != 35)goto _LLDC;else{_tmp106=_tmp105->f1;}}_LLDB:
# 484
 Cyc_Tovc_stmt_to_vc(env,_tmp106);
# 486
e->r=(Cyc_Tovc_stmt_to_exp(env,_tmp106))->r;
return;_LLDC: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp107=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmp107->tag != 38)goto _LLDE;}_LLDD:
# 489
 return;_LLDE:;_LLDF: {
# 491
struct Cyc_Core_Impossible_exn_struct _tmp1A8;const char*_tmp1A7;struct Cyc_Core_Impossible_exn_struct*_tmp1A6;(int)_throw((void*)((_tmp1A6=_cycalloc(sizeof(*_tmp1A6)),((_tmp1A6[0]=((_tmp1A8.tag=Cyc_Core_Impossible,((_tmp1A8.f1=((_tmp1A7="bad exp form after xlation to C",_tag_dyneither(_tmp1A7,sizeof(char),32))),_tmp1A8)))),_tmp1A6)))));}_LLAB:;}
# 495
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 497
while(1){
env.encloser=s;{
void*_tmp10B=s->r;struct Cyc_Absyn_Stmt*_tmp111;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Stmt*_tmp117;struct Cyc_Absyn_Stmt*_tmp118;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Stmt*_tmp11B;struct Cyc_Absyn_Stmt*_tmp11C;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Stmt*_tmp11F;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Stmt*_tmp124;struct Cyc_Absyn_Stmt*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_Decl*_tmp12C;struct Cyc_Absyn_Stmt*_tmp12D;_LLE1: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp10C=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp10C->tag != 0)goto _LLE3;}_LLE2:
 goto _LLE4;_LLE3: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp10D=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp10D->tag != 6)goto _LLE5;}_LLE4:
 goto _LLE6;_LLE5: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp10E=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp10E->tag != 7)goto _LLE7;}_LLE6:
 goto _LLE8;_LLE7: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp10F=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp10F->tag != 8)goto _LLE9;}_LLE8:
 return;_LLE9: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp110=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp110->tag != 13)goto _LLEB;else{_tmp111=_tmp110->f2;}}_LLEA:
# 505
 s=_tmp111;
continue;_LLEB: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp112=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp112->tag != 3)goto _LLED;else{_tmp113=_tmp112->f1;}}_LLEC:
# 508
 if(_tmp113 == 0)
return;
_tmp115=_tmp113;goto _LLEE;_LLED: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp114=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp114->tag != 1)goto _LLEF;else{_tmp115=_tmp114->f1;}}_LLEE:
# 512
 Cyc_Tovc_exp_to_vc(env,_tmp115);
return;_LLEF: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp116=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp116->tag != 2)goto _LLF1;else{_tmp117=_tmp116->f1;_tmp118=_tmp116->f2;}}_LLF0:
# 515
 Cyc_Tovc_stmt_to_vc(env,_tmp117);
s=_tmp118;
continue;_LLF1: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp119=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp119->tag != 4)goto _LLF3;else{_tmp11A=_tmp119->f1;_tmp11B=_tmp119->f2;_tmp11C=_tmp119->f3;}}_LLF2:
# 519
 Cyc_Tovc_exp_to_vc(env,_tmp11A);
Cyc_Tovc_stmt_to_vc(env,_tmp11B);
s=_tmp11C;
continue;_LLF3: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp11D=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp11D->tag != 5)goto _LLF5;else{_tmp11E=(_tmp11D->f1).f1;_tmp11F=_tmp11D->f2;}}_LLF4:
# 524
 Cyc_Tovc_exp_to_vc(env,_tmp11E);
s=_tmp11F;
continue;_LLF5: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp120=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp120->tag != 9)goto _LLF7;else{_tmp121=_tmp120->f1;_tmp122=(_tmp120->f2).f1;_tmp123=(_tmp120->f3).f1;_tmp124=_tmp120->f4;}}_LLF6:
# 528
 Cyc_Tovc_exp_to_vc(env,_tmp121);
Cyc_Tovc_exp_to_vc(env,_tmp122);
Cyc_Tovc_exp_to_vc(env,_tmp123);
s=_tmp124;
continue;_LLF7: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp125=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp125->tag != 14)goto _LLF9;else{_tmp126=_tmp125->f1;_tmp127=(_tmp125->f2).f1;}}_LLF8:
# 534
 Cyc_Tovc_exp_to_vc(env,_tmp127);
s=_tmp126;
continue;_LLF9: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp128=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp128->tag != 10)goto _LLFB;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}_LLFA:
# 540
 Cyc_Tovc_exp_to_vc(env,_tmp129);
for(0;_tmp12A != 0;_tmp12A=_tmp12A->tl){
Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)_tmp12A->hd)->body);}
return;_LLFB: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp12B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp10B;if(_tmp12B->tag != 12)goto _LLFD;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;}}_LLFC:
# 545
{void*_tmp12E=_tmp12C->r;struct Cyc_Absyn_Vardecl*_tmp130;_LL100: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp12F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp12E;if(_tmp12F->tag != 0)goto _LL102;else{_tmp130=_tmp12F->f1;}}_LL101:
# 547
 if(Cyc_Tovc_elim_array_initializers){
# 552
void*_tmp131=Cyc_Tcutil_compress(_tmp130->type);_LL105: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp132=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131;if(_tmp132->tag != 8)goto _LL107;}_LL106:
# 554
 if(_tmp130->initializer != 0){
void*_tmp133=((struct Cyc_Absyn_Exp*)_check_null(_tmp130->initializer))->r;struct Cyc_List_List*_tmp135;_LL10A: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp133;if(_tmp134->tag != 34)goto _LL10C;else{_tmp135=_tmp134->f2;}}_LL10B:
# 557
 _tmp130->initializer=0;{
# 559
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(_tmp130->name,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(_tmp12D->r,0);
{unsigned int i=0;for(0;_tmp135 != 0;(_tmp135=_tmp135->tl,++ i)){
s3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(arr,
Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple10*)_tmp135->hd)).f2,0),0),s3,0);}}
# 567
_tmp12D->r=s3->r;
goto _LL109;};_LL10C:;_LL10D: {
# 570
const char*_tmp1AC;void*_tmp1AB[1];struct Cyc_String_pa_PrintArg_struct _tmp1AA;struct _dyneither_ptr _tmp136=(_tmp1AA.tag=0,((_tmp1AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmp1AB[0]=& _tmp1AA,Cyc_aprintf(((_tmp1AC="|%s|: array type bad initializer",_tag_dyneither(_tmp1AC,sizeof(char),33))),_tag_dyneither(_tmp1AB,sizeof(void*),1)))))));
struct Cyc_Core_Impossible_exn_struct _tmp1AF;struct Cyc_Core_Impossible_exn_struct*_tmp1AE;(int)_throw((void*)((_tmp1AE=_cycalloc(sizeof(*_tmp1AE)),((_tmp1AE[0]=((_tmp1AF.tag=Cyc_Core_Impossible,((_tmp1AF.f1=(struct _dyneither_ptr)_tmp136,_tmp1AF)))),_tmp1AE)))));}_LL109:;}
# 574
goto _LL104;_LL107:;_LL108:
 goto _LL104;_LL104:;}
# 578
if(_tmp130->initializer != 0)
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp130->initializer));
{struct Cyc_List_List*_tmp1B0;env.all_locals=((_tmp1B0=_region_malloc(env.rgn,sizeof(*_tmp1B0)),((_tmp1B0->hd=_tmp130,((_tmp1B0->tl=env.all_locals,_tmp1B0))))));}
s=_tmp12D;
continue;_LL102:;_LL103:
 goto _LLFF;_LLFF:;}
# 585
goto _LLFE;_LLFD:;_LLFE: {
struct Cyc_Core_Impossible_exn_struct _tmp1B6;const char*_tmp1B5;struct Cyc_Core_Impossible_exn_struct*_tmp1B4;(int)_throw((void*)((_tmp1B4=_cycalloc(sizeof(*_tmp1B4)),((_tmp1B4[0]=((_tmp1B6.tag=Cyc_Core_Impossible,((_tmp1B6.f1=((_tmp1B5="bad stmt after xlation to C",_tag_dyneither(_tmp1B5,sizeof(char),28))),_tmp1B6)))),_tmp1B4)))));}_LLE0:;};}}
# 591
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){
struct Cyc_List_List*_tmp140=0;
for(0;old_ds != 0;old_ds=old_ds->tl){
struct Cyc_Absyn_Decl*_tmp141=(struct Cyc_Absyn_Decl*)old_ds->hd;
void*_tmp142=_tmp141->r;struct Cyc_Absyn_Fndecl*_tmp144;struct Cyc_List_List*_tmp14F;struct Cyc_List_List*_tmp151;struct Cyc_List_List*_tmp153;struct Cyc_List_List*_tmp155;_LL10F: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp143=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp143->tag != 1)goto _LL111;else{_tmp144=_tmp143->f1;}}_LL110:
# 600
 _tmp144->cached_typ=0;
# 607
if(_tmp144->param_vardecls != 0){
struct _RegionHandle _tmp158=_new_region("rgn");struct _RegionHandle*rgn=& _tmp158;_push_region(rgn);
{struct Cyc_List_List**_tmp1B7;struct Cyc_List_List**_tmp159=(_tmp1B7=_region_malloc(rgn,sizeof(*_tmp1B7)),((_tmp1B7[0]=0,_tmp1B7)));
if(_tmp144->param_vardecls == 0){
{const char*_tmp1BB;void*_tmp1BA[1];struct Cyc_String_pa_PrintArg_struct _tmp1B9;(_tmp1B9.tag=0,((_tmp1B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp144->name).f2),((_tmp1BA[0]=& _tmp1B9,Cyc_fprintf(Cyc_stderr,((_tmp1BB="No param vardecls for function %s\n",_tag_dyneither(_tmp1BB,sizeof(char),35))),_tag_dyneither(_tmp1BA,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);}
# 614
{struct Cyc_Tovc_ToExpEnv _tmp1BC;Cyc_Tovc_stmt_to_vc(((_tmp1BC.rgn=rgn,((_tmp1BC.all_locals=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp144->param_vardecls))->v),((_tmp1BC.varmap=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(rgn,Cyc_strptrcmp),((_tmp1BC.encloser=_tmp144->body,((_tmp1BC.gen_ds=_tmp159,_tmp1BC)))))))))),_tmp144->body);}
# 620
if(*_tmp159 != 0){
struct Cyc_Absyn_Vardecl*_tmp1BD;struct Cyc_Absyn_Vardecl*vd=(_tmp1BD=_cycalloc(sizeof(*_tmp1BD)),((_tmp1BD->sc=_tmp144->sc,((_tmp1BD->name=_tmp144->name,((_tmp1BD->tq=Cyc_Absyn_const_tqual(0),((_tmp1BD->type=
Cyc_Tcutil_fndecl2typ(_tmp144),((_tmp1BD->initializer=0,((_tmp1BD->rgn=0,((_tmp1BD->attributes=_tmp144->attributes,((_tmp1BD->escapes=0,_tmp1BD)))))))))))))))));
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1C3;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp1C2;struct Cyc_List_List*_tmp1C1;_tmp140=((_tmp1C1=_cycalloc(sizeof(*_tmp1C1)),((_tmp1C1->hd=Cyc_Absyn_new_decl((void*)((_tmp1C3=_cycalloc(sizeof(*_tmp1C3)),((_tmp1C3[0]=((_tmp1C2.tag=0,((_tmp1C2.f1=vd,_tmp1C2)))),_tmp1C3)))),0),((_tmp1C1->tl=_tmp140,_tmp1C1))))));}
# 625
_tmp140=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*_tmp159,_tmp140);}}
# 609
;_pop_region(rgn);}
# 629
goto _LL112;_LL111: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp145=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp145->tag != 0)goto _LL113;}_LL112:
 goto _LL114;_LL113: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp146=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp146->tag != 2)goto _LL115;}_LL114:
 goto _LL116;_LL115: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp147=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp147->tag != 3)goto _LL117;}_LL116:
 goto _LL118;_LL117: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp148=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp148->tag != 4)goto _LL119;}_LL118:
 goto _LL11A;_LL119: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp149=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp149->tag != 5)goto _LL11B;}_LL11A:
 goto _LL11C;_LL11B: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp14A=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp14A->tag != 6)goto _LL11D;}_LL11C:
 goto _LL11E;_LL11D: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp14B=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp14B->tag != 7)goto _LL11F;}_LL11E:
 goto _LL120;_LL11F: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp14C=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp14C->tag != 8)goto _LL121;}_LL120:
 goto _LL122;_LL121: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp14D=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp14D->tag != 9)goto _LL123;}_LL122:
# 639
{struct Cyc_List_List*_tmp1C4;_tmp140=((_tmp1C4=_cycalloc(sizeof(*_tmp1C4)),((_tmp1C4->hd=_tmp141,((_tmp1C4->tl=_tmp140,_tmp1C4))))));}
goto _LL10E;_LL123: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp14E=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp14E->tag != 10)goto _LL125;else{_tmp14F=_tmp14E->f2;}}_LL124:
 _tmp151=_tmp14F;goto _LL126;_LL125: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp150=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp150->tag != 11)goto _LL127;else{_tmp151=_tmp150->f2;}}_LL126:
 _tmp153=_tmp151;goto _LL128;_LL127: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp152=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp152->tag != 12)goto _LL129;else{_tmp153=_tmp152->f1;}}_LL128:
 _tmp155=_tmp153;goto _LL12A;_LL129: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp154=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp154->tag != 13)goto _LL12B;else{_tmp155=_tmp154->f1;}}_LL12A:
 goto _LL12C;_LL12B: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp156=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp156->tag != 14)goto _LL12D;}_LL12C:
 goto _LL12E;_LL12D: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp157=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp142;if(_tmp157->tag != 15)goto _LL10E;}_LL12E: {
# 647
struct Cyc_Core_Impossible_exn_struct _tmp1CA;const char*_tmp1C9;struct Cyc_Core_Impossible_exn_struct*_tmp1C8;(int)_throw((void*)((_tmp1C8=_cycalloc(sizeof(*_tmp1C8)),((_tmp1C8[0]=((_tmp1CA.tag=Cyc_Core_Impossible,((_tmp1CA.f1=((_tmp1C9="nested translation unit after translation to C",_tag_dyneither(_tmp1C9,sizeof(char),47))),_tmp1CA)))),_tmp1C8)))));}_LL10E:;}
# 650
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp140);}
