#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef ___sigset_t_def_
#define ___sigset_t_def_
typedef struct {unsigned long __val[1024 / (8 * sizeof(unsigned long))];} __sigset_t;
#endif
#ifndef ___jmp_buf_def_
#define ___jmp_buf_def_
typedef int __jmp_buf[6];
#endif
#ifndef ___jmp_buf_tag_def_
#define ___jmp_buf_tag_def_
struct __jmp_buf_tag{
  __jmp_buf __jmpbuf;
  int __mask_was_saved;
  __sigset_t __saved_mask;
};
#endif
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef struct __jmp_buf_tag jmp_buf[1];
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
extern int _throw_null();
extern int _throw_arraybounds();
extern int _throw_badalloc();
extern int _throw(void* e);
#endif

extern struct _xtunion_struct *_exn_thrown;

/* Built-in Exceptions */
struct Cyc_Null_Exception_struct { char *tag; };
struct Cyc_Array_bounds_struct { char *tag; };
struct Cyc_Match_Exception_struct { char *tag; };
struct Cyc_Bad_alloc_struct { char *tag; };
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
_check_null(void *ptr) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null();
  return _check_null_temp;
}
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
_check_known_subscript_null(void *ptr, unsigned bound, unsigned elt_sz, unsigned index) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null();
  if (_cks_index >= _cks_bound) _throw_arraybounds();
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
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
_check_known_subscript_notnull(unsigned bound,unsigned index) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); 
  return _cksnn_index;
}
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
#define _zero_arr_plus_char(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_short(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_int(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_float(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_double(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char(char *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short(short *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int(int *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float(float *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double(double *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble(long double *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar(void **orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
#endif


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
   Note that this expands to call _zero_arr_plus. */
/*#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })
  */
static _INLINE void 
_zero_arr_inplace_plus_char(char *x, int orig_i) {
  char **_zap_x = &x;
  *_zap_x = _zero_arr_plus_char(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_short(short *x, int orig_i) {
  short **_zap_x = &x;
  *_zap_x = _zero_arr_plus_short(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_int(int *x, int orig_i) {
  int **_zap_x = &x;
  *_zap_x = _zero_arr_plus_int(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_float(float *x, int orig_i) {
  float **_zap_x = &x;
  *_zap_x = _zero_arr_plus_float(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_double(double *x, int orig_i) {
  double **_zap_x = &x;
  *_zap_x = _zero_arr_plus_double(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_longdouble(long double *x, int orig_i) {
  long double **_zap_x = &x;
  *_zap_x = _zero_arr_plus_longdouble(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_voidstar(void **x, int orig_i) {
  void ***_zap_x = &x;
  *_zap_x = _zero_arr_plus_voidstar(*_zap_x,1,orig_i);
}




/* Does in-place increment of a zero-terminated pointer (e.g., x++).
   Note that this expands to call _zero_arr_plus. */
/*#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })*/
  
static _INLINE char *
_zero_arr_inplace_plus_post_char(char *x, int orig_i){
  char ** _zap_x = &x;
  char * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_char(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE short *
_zero_arr_inplace_plus_post_short(short *x, int orig_i){
  short **_zap_x = &x;
  short * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_short(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE int *
_zero_arr_inplace_plus_post_int(int *x, int orig_i){
  int **_zap_x = &x;
  int * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_int(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE float *
_zero_arr_inplace_plus_post_float(float *x, int orig_i){
  float **_zap_x = &x;
  float * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_float(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE double *
_zero_arr_inplace_plus_post_double(double *x, int orig_i){
  double **_zap_x = &x;
  double * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_double(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble(long double *x, int orig_i){
  long double **_zap_x = &x;
  long double * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_longdouble(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar(void **x, int orig_i){
  void ***_zap_x = &x;
  void ** _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_voidstar(_zap_res,1,orig_i);
  return _zap_res;
}



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
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
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
_untag_dyneither_ptr(struct _dyneither_ptr arr, 
                     unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
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
extern void* _profile_GC_malloc(int,char *file,int lineno);
extern void* _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                     char *file,int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						char *file,int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*
tag;};typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}
Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};struct Cyc_Lineno_Pos*Cyc_Lineno_pos_of_abs(
struct _dyneither_ptr,int);void Cyc_Lineno_poss_of_abss(struct _dyneither_ptr
filename,struct Cyc_List_List*places);extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};void Cyc_Position_reset_position(struct _dyneither_ptr);void Cyc_Position_set_position_file(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Position_get_position_file();
struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);int Cyc_Position_segment_equals(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Position_string_of_loc(int);
struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_lex(struct Cyc_Position_Segment*,
struct _dyneither_ptr);struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct
Cyc_Position_Segment*,struct _dyneither_ptr);struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_struct{char*tag;};extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_print_context;extern int Cyc_Position_num_errors;extern int
Cyc_Position_max_errors;void Cyc_Position_post_error(struct Cyc_Position_Error*);
int Cyc_Position_error_p();struct _dyneither_ptr Cyc_Position_get_line_directive(
struct Cyc_Position_Segment*loc);unsigned int Cyc_strlen(struct _dyneither_ptr s);
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct
_dyneither_ptr Cyc_strncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned int);
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned int n);
struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_Float_struct{
int tag;};struct Cyc_Typerep_Double_struct{int tag;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple0{unsigned int f1;struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct
_tuple1{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dyneither_ptr f2;};struct _tuple2{unsigned int f1;struct
_dyneither_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;
struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple3{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);int Cyc_Position_use_gcc_style_location=1;char Cyc_Position_Exit[5]="Exit";
static char _tmp0[1]="";static struct _dyneither_ptr Cyc_Position_source={_tmp0,_tmp0,
_tmp0 + 1};struct Cyc_Position_Segment{int start;int end;};struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int start,int end){return({struct Cyc_Position_Segment*
_tmp1=_cycalloc_atomic(sizeof(*_tmp1));_tmp1->start=start;_tmp1->end=end;_tmp1;});}
struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*
s1,struct Cyc_Position_Segment*s2){if(s1 == 0)return s2;if(s2 == 0)return s1;return({
struct Cyc_Position_Segment*_tmp2=_cycalloc_atomic(sizeof(*_tmp2));_tmp2->start=
s1->start;_tmp2->end=s2->end;_tmp2;});}int Cyc_Position_segment_equals(struct Cyc_Position_Segment*
s1,struct Cyc_Position_Segment*s2){if(s1 == s2)return 1;if(s1 == 0)return s2 == 0;
return s2 != 0  && (s1->start == s2->start  && s1->end == s2->end);}struct
_dyneither_ptr Cyc_Position_string_of_loc(int loc){struct Cyc_Lineno_Pos*pos=Cyc_Lineno_pos_of_abs(
Cyc_Position_source,loc);if(Cyc_Position_use_gcc_style_location)return({struct
Cyc_Int_pa_struct _tmp6;_tmp6.tag=1;_tmp6.f1=(unsigned long)pos->line_no;{struct
Cyc_String_pa_struct _tmp5;_tmp5.tag=0;_tmp5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)pos->logical_file);{void*_tmp3[2]={& _tmp5,& _tmp6};Cyc_aprintf(({
const char*_tmp4="%s:%d";_tag_dyneither(_tmp4,sizeof(char),6);}),_tag_dyneither(
_tmp3,sizeof(void*),2));}}});else{return({struct Cyc_Int_pa_struct _tmpB;_tmpB.tag=
1;_tmpB.f1=(unsigned long)pos->col;{struct Cyc_Int_pa_struct _tmpA;_tmpA.tag=1;
_tmpA.f1=(unsigned long)pos->line_no;{struct Cyc_String_pa_struct _tmp9;_tmp9.tag=
0;_tmp9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos->logical_file);{void*
_tmp7[3]={& _tmp9,& _tmpA,& _tmpB};Cyc_aprintf(({const char*_tmp8="%s:(%d:%d)";
_tag_dyneither(_tmp8,sizeof(char),11);}),_tag_dyneither(_tmp7,sizeof(void*),3));}}}});}}
static struct _dyneither_ptr Cyc_Position_string_of_pos_pr(struct Cyc_Lineno_Pos*
pos_s,struct Cyc_Lineno_Pos*pos_e){if(Cyc_Position_use_gcc_style_location)return({
struct Cyc_Int_pa_struct _tmpF;_tmpF.tag=1;_tmpF.f1=(unsigned long)pos_s->line_no;{
struct Cyc_String_pa_struct _tmpE;_tmpE.tag=0;_tmpE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)pos_s->logical_file);{void*_tmpC[2]={& _tmpE,& _tmpF};Cyc_aprintf(({
const char*_tmpD="%s:%d";_tag_dyneither(_tmpD,sizeof(char),6);}),_tag_dyneither(
_tmpC,sizeof(void*),2));}}});else{if(Cyc_strcmp((struct _dyneither_ptr)pos_s->logical_file,(
struct _dyneither_ptr)pos_e->logical_file)== 0)return({struct Cyc_Int_pa_struct
_tmp16;_tmp16.tag=1;_tmp16.f1=(unsigned long)pos_e->col;{struct Cyc_Int_pa_struct
_tmp15;_tmp15.tag=1;_tmp15.f1=(unsigned long)pos_e->line_no;{struct Cyc_Int_pa_struct
_tmp14;_tmp14.tag=1;_tmp14.f1=(unsigned long)pos_s->col;{struct Cyc_Int_pa_struct
_tmp13;_tmp13.tag=1;_tmp13.f1=(unsigned long)pos_s->line_no;{struct Cyc_String_pa_struct
_tmp12;_tmp12.tag=0;_tmp12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos_s->logical_file);{
void*_tmp10[5]={& _tmp12,& _tmp13,& _tmp14,& _tmp15,& _tmp16};Cyc_aprintf(({const char*
_tmp11="%s(%d:%d-%d:%d)";_tag_dyneither(_tmp11,sizeof(char),16);}),
_tag_dyneither(_tmp10,sizeof(void*),5));}}}}}});else{return({struct Cyc_Int_pa_struct
_tmp1E;_tmp1E.tag=1;_tmp1E.f1=(unsigned long)pos_e->col;{struct Cyc_Int_pa_struct
_tmp1D;_tmp1D.tag=1;_tmp1D.f1=(unsigned long)pos_e->line_no;{struct Cyc_String_pa_struct
_tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos_e->logical_file);{
struct Cyc_Int_pa_struct _tmp1B;_tmp1B.tag=1;_tmp1B.f1=(unsigned long)pos_s->col;{
struct Cyc_Int_pa_struct _tmp1A;_tmp1A.tag=1;_tmp1A.f1=(unsigned long)pos_s->line_no;{
struct Cyc_String_pa_struct _tmp19;_tmp19.tag=0;_tmp19.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)pos_s->logical_file);{void*_tmp17[6]={& _tmp19,& _tmp1A,&
_tmp1B,& _tmp1C,& _tmp1D,& _tmp1E};Cyc_aprintf(({const char*_tmp18="%s(%d:%d)-%s(%d:%d)";
_tag_dyneither(_tmp18,sizeof(char),20);}),_tag_dyneither(_tmp17,sizeof(void*),6));}}}}}}});}}}
struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*s){
if(s == 0)return({struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_source);{void*_tmp1F[1]={&
_tmp21};Cyc_aprintf(({const char*_tmp20="%s";_tag_dyneither(_tmp20,sizeof(char),3);}),
_tag_dyneither(_tmp1F,sizeof(void*),1));}});{struct Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(
Cyc_Position_source,s->start);struct Cyc_Lineno_Pos*pos_e=Cyc_Lineno_pos_of_abs(
Cyc_Position_source,s->end);return Cyc_Position_string_of_pos_pr(pos_s,pos_e);}}
static struct Cyc_Lineno_Pos*Cyc_Position_new_pos(){return({struct Cyc_Lineno_Pos*
_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->logical_file=({const char*_tmp23="";
_tag_dyneither(_tmp23,sizeof(char),1);});_tmp22->line=Cyc_Core_new_string(0);
_tmp22->line_no=0;_tmp22->col=0;_tmp22;});}struct _tuple4{int f1;struct Cyc_Lineno_Pos*
f2;};struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*
segs){struct Cyc_List_List*places=0;{struct Cyc_List_List*_tmp24=segs;for(0;_tmp24
!= 0;_tmp24=_tmp24->tl){if((struct Cyc_Position_Segment*)_tmp24->hd == 0)continue;
places=({struct Cyc_List_List*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->hd=({
struct _tuple4*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->f1=((struct Cyc_Position_Segment*)
_check_null((struct Cyc_Position_Segment*)_tmp24->hd))->end;_tmp28->f2=Cyc_Position_new_pos();
_tmp28;});_tmp25->tl=({struct Cyc_List_List*_tmp26=_cycalloc(sizeof(*_tmp26));
_tmp26->hd=({struct _tuple4*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->f1=((struct
Cyc_Position_Segment*)_check_null((struct Cyc_Position_Segment*)_tmp24->hd))->start;
_tmp27->f2=Cyc_Position_new_pos();_tmp27;});_tmp26->tl=places;_tmp26;});_tmp25;});}}
Cyc_Lineno_poss_of_abss(Cyc_Position_source,places);{struct Cyc_List_List*ans=0;
places=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(places);
for(0;segs != 0;segs=segs->tl){if((struct Cyc_Position_Segment*)segs->hd == 0)ans=({
struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->hd=({struct
_dyneither_ptr*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_String_pa_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_source);{
void*_tmp2B[1]={& _tmp2D};Cyc_aprintf(({const char*_tmp2C="%s(unknown)";
_tag_dyneither(_tmp2C,sizeof(char),12);}),_tag_dyneither(_tmp2B,sizeof(void*),1));}});
_tmp2A;});_tmp29->tl=ans;_tmp29;});else{ans=({struct Cyc_List_List*_tmp2E=
_cycalloc(sizeof(*_tmp2E));_tmp2E->hd=({struct _dyneither_ptr*_tmp2F=_cycalloc(
sizeof(*_tmp2F));_tmp2F[0]=Cyc_Position_string_of_pos_pr((*((struct _tuple4*)((
struct Cyc_List_List*)_check_null(places))->hd)).f2,(*((struct _tuple4*)((struct
Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(places))->tl))->hd)).f2);
_tmp2F;});_tmp2E->tl=ans;_tmp2E;});places=((struct Cyc_List_List*)_check_null(
places->tl))->tl;}}return ans;}}struct Cyc_Position_Lex_struct Cyc_Position_Lex_val={
0};struct Cyc_Position_Parse_struct Cyc_Position_Parse_val={1};struct Cyc_Position_Elab_struct
Cyc_Position_Elab_val={2};struct Cyc_Position_Error;struct Cyc_Position_Error*Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*l,struct _dyneither_ptr desc){return({struct Cyc_Position_Error*
_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->source=Cyc_Position_source;_tmp33->seg=
l;_tmp33->kind=(void*)& Cyc_Position_Lex_val;_tmp33->desc=desc;_tmp33;});}struct
Cyc_Position_Error*Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*l,struct
_dyneither_ptr desc){return({struct Cyc_Position_Error*_tmp34=_cycalloc(sizeof(*
_tmp34));_tmp34->source=Cyc_Position_source;_tmp34->seg=l;_tmp34->kind=(void*)&
Cyc_Position_Parse_val;_tmp34->desc=desc;_tmp34;});}struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*l,struct _dyneither_ptr desc){return({struct Cyc_Position_Error*
_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->source=Cyc_Position_source;_tmp35->seg=
l;_tmp35->kind=(void*)& Cyc_Position_Elab_val;_tmp35->desc=desc;_tmp35;});}char
Cyc_Position_Nocontext[10]="Nocontext";struct Cyc_Position_Nocontext_struct Cyc_Position_Nocontext_val={
Cyc_Position_Nocontext};static struct _dyneither_ptr Cyc_Position_trunc(int n,struct
_dyneither_ptr s){int len=(int)Cyc_strlen((struct _dyneither_ptr)s);if(len < n)
return s;{int len_one=(n - 3)/ 2;int len_two=(n - 3)- len_one;struct _dyneither_ptr
mans=Cyc_Core_new_string((unsigned int)(n + 1));struct _dyneither_ptr ans=
_dyneither_ptr_decrease_size(mans,sizeof(char),1);Cyc_strncpy(ans,(struct
_dyneither_ptr)s,(unsigned int)len_one);Cyc_strncpy(_dyneither_ptr_plus(ans,
sizeof(char),len_one),({const char*_tmp37="...";_tag_dyneither(_tmp37,sizeof(char),
4);}),3);Cyc_strncpy(_dyneither_ptr_plus(ans,sizeof(char),len_one + 3),(struct
_dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),len - len_two),(unsigned int)
len_two);return mans;}}static int Cyc_Position_line_length=76;struct _tuple5{struct
_dyneither_ptr f1;int f2;int f3;};static struct _tuple5*Cyc_Position_get_context(
struct Cyc_Position_Segment*seg){if(seg == 0)(int)_throw((void*)& Cyc_Position_Nocontext_val);{
struct Cyc_Lineno_Pos*pos_s;struct Cyc_Lineno_Pos*pos_e;{struct _handler_cons _tmp38;
_push_handler(& _tmp38);{int _tmp3A=0;if(setjmp(_tmp38.handler))_tmp3A=1;if(!
_tmp3A){pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,seg->start);pos_e=Cyc_Lineno_pos_of_abs(
Cyc_Position_source,seg->end);;_pop_handler();}else{void*_tmp39=(void*)
_exn_thrown;void*_tmp3C=_tmp39;_LL1:;_LL2:(int)_throw((void*)& Cyc_Position_Nocontext_val);
_LL3:;_LL4:(void)_throw(_tmp3C);_LL0:;}}}{struct Cyc_Lineno_Pos _tmp3E;struct
_dyneither_ptr _tmp3F;int _tmp40;int _tmp41;struct Cyc_Lineno_Pos*_tmp3D=pos_s;
_tmp3E=*_tmp3D;_tmp3F=_tmp3E.line;_tmp40=_tmp3E.line_no;_tmp41=_tmp3E.col;{
struct Cyc_Lineno_Pos _tmp43;struct _dyneither_ptr _tmp44;int _tmp45;int _tmp46;struct
Cyc_Lineno_Pos*_tmp42=pos_e;_tmp43=*_tmp42;_tmp44=_tmp43.line;_tmp45=_tmp43.line_no;
_tmp46=_tmp43.col;if(_tmp40 == _tmp45){int n=Cyc_Position_line_length / 3;struct
_dyneither_ptr sec_one=Cyc_Position_trunc(n,Cyc_substring((struct _dyneither_ptr)
_tmp3F,0,(unsigned int)_tmp41));struct _dyneither_ptr sec_two=Cyc_Position_trunc(n,
Cyc_substring((struct _dyneither_ptr)_tmp3F,_tmp41,(unsigned int)(_tmp46 - _tmp41)));
struct _dyneither_ptr sec_three=Cyc_Position_trunc(n,Cyc_substring((struct
_dyneither_ptr)_tmp3F,_tmp41,Cyc_strlen((struct _dyneither_ptr)_tmp3F)- _tmp46));
return({struct _tuple5*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->f1=({struct Cyc_String_pa_struct
_tmp4C;_tmp4C.tag=0;_tmp4C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
sec_three);{struct Cyc_String_pa_struct _tmp4B;_tmp4B.tag=0;_tmp4B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)sec_two);{struct Cyc_String_pa_struct _tmp4A;
_tmp4A.tag=0;_tmp4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sec_one);{
void*_tmp48[3]={& _tmp4A,& _tmp4B,& _tmp4C};Cyc_aprintf(({const char*_tmp49="%s%s%s";
_tag_dyneither(_tmp49,sizeof(char),7);}),_tag_dyneither(_tmp48,sizeof(void*),3));}}}});
_tmp47->f2=(int)Cyc_strlen((struct _dyneither_ptr)sec_one);_tmp47->f3=(int)(Cyc_strlen((
struct _dyneither_ptr)sec_one)+ Cyc_strlen((struct _dyneither_ptr)sec_two));_tmp47;});}
else{int n=(Cyc_Position_line_length - 3)/ 4;struct _dyneither_ptr sec_one=Cyc_Position_trunc(
n,Cyc_substring((struct _dyneither_ptr)_tmp3F,0,(unsigned int)_tmp41));struct
_dyneither_ptr sec_two=Cyc_Position_trunc(n,Cyc_substring((struct _dyneither_ptr)
_tmp3F,_tmp41,Cyc_strlen((struct _dyneither_ptr)_tmp3F)- _tmp41));struct
_dyneither_ptr sec_three=Cyc_Position_trunc(n,Cyc_substring((struct _dyneither_ptr)
_tmp44,0,(unsigned int)_tmp46));struct _dyneither_ptr sec_four=Cyc_Position_trunc(
n,Cyc_substring((struct _dyneither_ptr)_tmp44,_tmp46,Cyc_strlen((struct
_dyneither_ptr)_tmp44)- _tmp46));return({struct _tuple5*_tmp4D=_cycalloc(sizeof(*
_tmp4D));_tmp4D->f1=({struct Cyc_String_pa_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)sec_four);{struct Cyc_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
sec_three);{struct Cyc_String_pa_struct _tmp51;_tmp51.tag=0;_tmp51.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)sec_two);{struct Cyc_String_pa_struct _tmp50;
_tmp50.tag=0;_tmp50.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sec_one);{
void*_tmp4E[4]={& _tmp50,& _tmp51,& _tmp52,& _tmp53};Cyc_aprintf(({const char*_tmp4F="%s%s.\\.%s%s";
_tag_dyneither(_tmp4F,sizeof(char),12);}),_tag_dyneither(_tmp4E,sizeof(void*),4));}}}}});
_tmp4D->f2=(int)Cyc_strlen((struct _dyneither_ptr)sec_one);_tmp4D->f3=(int)(((Cyc_strlen((
struct _dyneither_ptr)sec_one)+ Cyc_strlen((struct _dyneither_ptr)sec_two))+ 3)+ 
Cyc_strlen((struct _dyneither_ptr)sec_three));_tmp4D;});}}}}}static int Cyc_Position_error_b=
0;int Cyc_Position_error_p(){return Cyc_Position_error_b;}char Cyc_Position_Error[6]="Error";
struct Cyc_Position_Error_struct{char*tag;struct Cyc_Position_Error*f1;};int Cyc_Position_print_context=
0;int Cyc_Position_first_error=1;int Cyc_Position_num_errors=0;int Cyc_Position_max_errors=
10;void Cyc_Position_post_error(struct Cyc_Position_Error*e){Cyc_Position_error_b=
1;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stdout);if(Cyc_Position_first_error){({
void*_tmp54=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp55="\n";_tag_dyneither(
_tmp55,sizeof(char),2);}),_tag_dyneither(_tmp54,sizeof(void*),0));});Cyc_Position_first_error=
0;}if(Cyc_Position_num_errors <= Cyc_Position_max_errors){({struct Cyc_String_pa_struct
_tmp59;_tmp59.tag=0;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)e->desc);{
struct Cyc_String_pa_struct _tmp58;_tmp58.tag=0;_tmp58.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Position_string_of_segment(e->seg));{void*_tmp56[2]={&
_tmp58,& _tmp59};Cyc_fprintf(Cyc_stderr,({const char*_tmp57="%s: %s\n";
_tag_dyneither(_tmp57,sizeof(char),8);}),_tag_dyneither(_tmp56,sizeof(void*),2));}}});
if(Cyc_Position_print_context){struct _handler_cons _tmp5A;_push_handler(& _tmp5A);{
int _tmp5C=0;if(setjmp(_tmp5A.handler))_tmp5C=1;if(!_tmp5C){{struct _tuple5*x=Cyc_Position_get_context(
e->seg);struct _dyneither_ptr marker_str=({unsigned int _tmp67=(unsigned int)((*x).f3
+ 1);char*_tmp68=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp67 + 1));
struct _dyneither_ptr _tmp6A=_tag_dyneither(_tmp68,sizeof(char),_tmp67 + 1);{
unsigned int _tmp69=_tmp67;unsigned int i;for(i=0;i < _tmp69;i ++){_tmp68[i]='\000';}
_tmp68[_tmp69]=(char)0;}_tmp6A;});int i=- 1;while(++ i < (*x).f2){({struct
_dyneither_ptr _tmp5D=_dyneither_ptr_plus(marker_str,sizeof(char),i);char _tmp5E=*((
char*)_check_dyneither_subscript(_tmp5D,sizeof(char),0));char _tmp5F=' ';if(
_get_dyneither_size(_tmp5D,sizeof(char))== 1  && (_tmp5E == '\000'  && _tmp5F != '\000'))
_throw_arraybounds();*((char*)_tmp5D.curr)=_tmp5F;});}while(++ i < (*x).f3){({
struct _dyneither_ptr _tmp60=_dyneither_ptr_plus(marker_str,sizeof(char),i);char
_tmp61=*((char*)_check_dyneither_subscript(_tmp60,sizeof(char),0));char _tmp62='^';
if(_get_dyneither_size(_tmp60,sizeof(char))== 1  && (_tmp61 == '\000'  && _tmp62 != '\000'))
_throw_arraybounds();*((char*)_tmp60.curr)=_tmp62;});}({struct Cyc_String_pa_struct
_tmp66;_tmp66.tag=0;_tmp66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
marker_str);{struct Cyc_String_pa_struct _tmp65;_tmp65.tag=0;_tmp65.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)(*x).f1);{void*_tmp63[2]={& _tmp65,& _tmp66};
Cyc_fprintf(Cyc_stderr,({const char*_tmp64="  %s\n  %s\n";_tag_dyneither(_tmp64,
sizeof(char),11);}),_tag_dyneither(_tmp63,sizeof(void*),2));}}});};_pop_handler();}
else{void*_tmp5B=(void*)_exn_thrown;void*_tmp6C=_tmp5B;_LL6: {struct Cyc_Position_Nocontext_struct*
_tmp6D=(struct Cyc_Position_Nocontext_struct*)_tmp6C;if(_tmp6D->tag != Cyc_Position_Nocontext)
goto _LL8;}_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp6C);_LL5:;}}}}if(Cyc_Position_num_errors
== Cyc_Position_max_errors)({void*_tmp6E=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp6F="Too many error messages!\n";_tag_dyneither(_tmp6F,sizeof(char),26);}),
_tag_dyneither(_tmp6E,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
++ Cyc_Position_num_errors;}void Cyc_Position_reset_position(struct _dyneither_ptr s){
Cyc_Position_source=s;Cyc_Position_error_b=0;}void Cyc_Position_set_position_file(
struct _dyneither_ptr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}struct
_dyneither_ptr Cyc_Position_get_position_file(){return Cyc_Position_source;}struct
_dyneither_ptr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*s){
struct Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,((struct Cyc_Position_Segment*)
_check_null(s))->start);if((struct Cyc_Lineno_Pos*)pos_s != 0)return(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)pos_s->logical_file);{struct Cyc_Int_pa_struct
_tmp72;_tmp72.tag=1;_tmp72.f1=(unsigned long)pos_s->line_no;{void*_tmp70[2]={&
_tmp72,& _tmp73};Cyc_aprintf(({const char*_tmp71="\n#line %d \"%s\"\n";
_tag_dyneither(_tmp71,sizeof(char),16);}),_tag_dyneither(_tmp70,sizeof(void*),2));}}});
else{return(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}
