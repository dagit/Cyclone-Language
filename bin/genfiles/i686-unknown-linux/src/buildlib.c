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
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct _dyneither_ptr Cstring_to_string(char*);typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long
__pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct
Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;
long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_fputc(int,
struct Cyc___cycFILE*);int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int remove(const char*);int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*
x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);struct
Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char
Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Iter_Iter Cyc_Set_make_iter(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s);unsigned int Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*
s2);struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned int n);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);int Cyc_Hashtable_hash_stringptr(struct
_dyneither_ptr*p);struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_chop_extension(struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};void Cyc_Position_reset_position(
struct _dyneither_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};extern char Cyc_Arg_Bad[4];struct
Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];
struct Cyc_Arg_Error_struct{char*tag;};struct Cyc_Arg_Unit_spec_struct{int tag;void(*
f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct _dyneither_ptr,
struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{
int tag;int*f1;};struct Cyc_Arg_String_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*f1)(int);};struct
Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};void Cyc_Arg_usage(
struct Cyc_List_List*,struct _dyneither_ptr);void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr args);struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(
unsigned int n);struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);void
Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);void Cyc_Buffer_add_string(struct
Cyc_Buffer_t*,struct _dyneither_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract
 = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register
 = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int
real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz
 = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz
 = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind  = 5,
Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
 = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;enum Cyc_Absyn_Kind f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;enum Cyc_Absyn_Kind f2;};struct Cyc_Absyn_Tvar{struct
_dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;
void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{
int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum 
Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int
tag;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{
Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{
int tag;};struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
int tag;};struct Cyc_Absyn_Const_att_struct{int tag;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{int tag;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_struct{int tag;};
struct Cyc_Absyn_Shared_att_struct{int tag;};struct Cyc_Absyn_Unused_att_struct{int
tag;};struct Cyc_Absyn_Weak_att_struct{int tag;};struct Cyc_Absyn_Dllimport_att_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{
int tag;};struct Cyc_Absyn_Constructor_att_struct{int tag;};struct Cyc_Absyn_Destructor_att_struct{
int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{int tag;};struct Cyc_Absyn_Format_att_struct{
int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
 = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,
Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte
 = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor
 = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,
Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc
 = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct
Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;
};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct
Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{
int tag;struct Cyc_List_List*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};
struct Cyc_Absyn_Swap_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{int tag;struct Cyc_Core_Opt*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Tagcheck_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;}
;struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_Absyn_Reference_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct
Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;
struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*
f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct _tuple9{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};struct
_tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc
 = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,
Cyc_Abstract_sc  = 6};struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple10{
struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple10*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple11{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple11*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple12{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple12*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct
_tuple13{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope
val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};
struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple14{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple7*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple15{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple15*val;};
struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{
int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;enum Cyc_Absyn_Kind val;};
struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct
_union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct
_union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{
int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};
struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_YY1
YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct
_union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;
struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9
YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct
_union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14
YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct
_union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19
YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct
_union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24
YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct
_union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29
YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct
_union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34
YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct
_union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39
YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct
_union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44
YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct
_union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49
YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct
_union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tc_tc(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);typedef
struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;void*
abort();void exit(int);int system(const char*);struct Cyc_stat{unsigned long long
st_dev;unsigned short __pad1;unsigned long st_ino;unsigned int st_mode;unsigned int
st_nlink;unsigned int st_uid;unsigned int st_gid;unsigned long long st_rdev;
unsigned short __pad2;long st_size;long st_blksize;long st_blocks;long st_atime;
unsigned long __unused1;long st_mtime;unsigned long __unused2;long st_ctime;
unsigned long __unused3;unsigned long __unused4;unsigned long __unused5;};int mkdir(
const char*pathname,unsigned int mode);struct Cyc_flock{short l_type;short l_whence;
long l_start;long l_len;int l_pid;};struct Cyc_Flock_struct{int tag;struct Cyc_flock*
f1;};struct Cyc_Long_struct{int tag;long f1;};int Cyc_open(const char*,int,struct
_dyneither_ptr);enum Cyc___anonymous_enum_368__{Cyc__PC_LINK_MAX  = 0,Cyc__PC_MAX_CANON
 = 1,Cyc__PC_MAX_INPUT  = 2,Cyc__PC_NAME_MAX  = 3,Cyc__PC_PATH_MAX  = 4,Cyc__PC_PIPE_BUF
 = 5,Cyc__PC_CHOWN_RESTRICTED  = 6,Cyc__PC_NO_TRUNC  = 7,Cyc__PC_VDISABLE  = 8,Cyc__PC_SYNC_IO
 = 9,Cyc__PC_ASYNC_IO  = 10,Cyc__PC_PRIO_IO  = 11,Cyc__PC_SOCK_MAXBUF  = 12,Cyc__PC_FILESIZEBITS
 = 13,Cyc__PC_REC_INCR_XFER_SIZE  = 14,Cyc__PC_REC_MAX_XFER_SIZE  = 15,Cyc__PC_REC_MIN_XFER_SIZE
 = 16,Cyc__PC_REC_XFER_ALIGN  = 17,Cyc__PC_ALLOC_SIZE_MIN  = 18,Cyc__PC_SYMLINK_MAX
 = 19};enum Cyc___anonymous_enum_370__{Cyc__CS_PATH  = 0,Cyc__CS_V6_WIDTH_RESTRICTED_ENVS
 = 1};struct Cyc_option{struct _dyneither_ptr name;int has_arg;int*flag;int val;};int
chdir(const char*);int close(int);struct _dyneither_ptr Cyc_getcwd(struct
_dyneither_ptr buf,unsigned int size);int isspace(int);int toupper(int);void Cyc_Lex_lex_init(
int use_cyclone_keywords);extern char*Ccomp;static int Cyc_do_setjmp=0;struct Cyc___cycFILE*
Cyc_log_file=0;struct Cyc___cycFILE*Cyc_cstubs_file=0;struct Cyc___cycFILE*Cyc_cycstubs_file=
0;int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_log_file == 0){({
void*_tmp0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1="Internal error: log file is NULL\n";
_tag_dyneither(_tmp1,sizeof(char),34);}),_tag_dyneither(_tmp0,sizeof(void*),0));});
exit(1);}{int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),
fmt,ap);Cyc_fflush((struct Cyc___cycFILE*)((struct Cyc___cycFILE*)_check_null(Cyc_log_file)));
return _tmp2;};}static struct _dyneither_ptr*Cyc_current_source=0;static struct Cyc_List_List*
Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(
struct _dyneither_ptr*sptr){Cyc_current_targets=({struct Cyc_Set_Set**_tmp3=
_cycalloc(sizeof(*_tmp3));_tmp3[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr);_tmp3;});}struct _tuple16{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
struct _tuple16*Cyc_line(struct Cyc_Lexing_lexbuf*);int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);int Cyc_token(struct Cyc_Lexing_lexbuf*);int
Cyc_string(struct Cyc_Lexing_lexbuf*);struct Cyc___cycFILE*Cyc_slurp_out=0;int Cyc_slurp(
struct Cyc_Lexing_lexbuf*);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm(
struct Cyc_Lexing_lexbuf*);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf*);struct _tuple17{struct _dyneither_ptr f1;struct
_dyneither_ptr*f2;};struct _tuple17*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);int
Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line=(struct _dyneither_ptr){(void*)0,(void*)0,(
void*)(0 + 0)};struct _tuple18{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
struct _tuple19{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct
_tuple19*Cyc_spec(struct Cyc_Lexing_lexbuf*);int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile=(struct _dyneither_ptr){(void*)0,(void*)
0,(void*)(0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=0;struct Cyc_List_List*
Cyc_current_symbols=0;struct Cyc_List_List*Cyc_current_cstubs=0;struct Cyc_List_List*
Cyc_current_cycstubs=0;struct Cyc_List_List*Cyc_current_hstubs=0;struct Cyc_List_List*
Cyc_current_omit_symbols=0;struct Cyc_List_List*Cyc_current_cpp=0;struct Cyc_Buffer_t*
Cyc_specbuf=0;int Cyc_braces_to_match=0;int Cyc_parens_to_match=0;int Cyc_numdef=0;
const int Cyc_lex_base[425]=(const int[425]){0,0,75,192,305,310,311,166,312,91,27,
384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,
- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,33,23,48,70,54,64,
74,100,91,107,94,370,386,111,103,95,122,122,375,414,111,111,153,377,1024,1139,
525,176,210,228,214,216,217,242,498,1253,1368,- 9,654,- 10,224,245,508,1482,1597,
684,- 8,720,- 11,432,510,515,1674,1751,1828,1909,434,465,550,1984,249,249,249,248,
244,254,0,13,4,2065,5,628,2073,2138,660,49,467,6,2099,7,705,2161,2199,820,- 22,
1051,1056,261,314,242,251,258,250,271,281,274,275,278,288,295,282,- 20,292,296,
300,313,321,326,- 15,309,325,319,315,326,365,400,417,418,404,400,400,427,431,- 17,
422,421,415,436,433,449,427,449,453,441,445,436,436,- 19,444,438,442,453,464,447,
449,482,489,490,4,6,21,491,492,505,504,516,516,524,554,24,556,557,24,20,570,571,
53,627,644,- 13,647,594,596,583,584,598,598,605,606,1,677,623,624,654,647,654,658,
659,623,624,639,645,648,703,704,705,654,655,710,720,721,679,680,735,737,765,713,
714,770,797,798,746,747,803,814,815,- 12,762,763,1030,- 21,1165,762,763,760,773,
772,767,769,772,773,771,827,1144,819,820,818,832,1258,1170,871,872,862,864,862,
875,1370,893,894,892,905,1375,- 7,- 8,8,1263,2231,9,996,2255,2293,1341,1279,- 49,
1150,- 2,945,- 4,974,998,1028,980,1010,1023,1448,981,2320,2363,994,1055,993,996,
2433,994,1057,- 36,- 42,- 37,2508,- 28,1002,- 40,- 25,1021,- 27,- 45,- 39,- 48,2583,2612,
1467,1081,1091,1563,2622,2652,1582,2281,2685,2716,2754,1083,1093,2824,2862,1118,
1157,1195,1206,1198,1241,- 6,- 34,1040,2794,- 47,- 30,- 32,- 46,- 29,- 31,- 33,1048,2902,
1112,1117,2128,1119,1124,1125,1133,1134,1138,1146,1147,1191,2975,3059,- 23,- 16,-
18,2239,1192,- 24,- 41,- 38,- 35,- 26,1394,3141,2,3224,1191,15,1170,1174,1175,1173,
1171,1185,1255};const int Cyc_lex_backtrk[425]=(const int[425]){- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,
- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,
2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,- 1,21,21,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,
- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,-
1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,
9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
425]=(const int[425]){- 1,- 1,- 1,317,306,138,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,
0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,103,- 1,- 1,
- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,
- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,
- 1,386,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,
0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,0,0,0,0,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[3481]=(const int[
3481]){0,0,0,0,0,0,0,0,0,0,22,19,28,417,19,28,19,28,100,19,45,45,45,45,45,22,45,
0,0,0,0,0,21,234,21,418,203,22,- 1,- 1,22,- 1,- 1,45,204,45,224,22,415,415,415,415,
415,415,415,415,415,415,31,103,22,205,114,40,214,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,31,217,
218,221,415,130,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,413,413,413,413,413,413,413,413,413,413,
121,20,74,67,54,55,56,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,57,58,59,60,413,61,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,62,63,37,318,319,318,318,319,39,22,64,65,68,69,70,129,34,34,34,34,34,
34,34,34,71,72,318,320,321,75,76,322,323,324,104,104,325,326,104,327,328,329,330,
331,331,331,331,331,331,331,331,331,332,77,333,334,335,104,19,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,19,- 1,- 1,337,336,101,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,307,338,37,308,139,139,24,24,
139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,309,88,95,139,96,22,26,26,21,
21,115,116,117,140,118,119,120,142,192,193,26,35,35,35,35,35,35,35,35,178,172,
163,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,150,78,151,152,153,
154,66,66,310,155,66,157,158,66,132,123,141,39,73,22,78,159,143,144,145,160,146,
161,27,66,147,31,162,21,73,73,164,165,73,148,166,167,168,113,113,113,113,113,113,
113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,169,- 1,- 1,113,- 1,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,89,89,28,170,89,171,100,173,174,175,97,97,104,104,97,176,
104,112,112,177,179,112,180,89,181,105,105,80,80,105,19,80,21,97,182,104,183,184,
185,186,112,187,188,189,190,191,271,194,105,195,80,196,112,112,91,197,112,198,
199,21,21,21,106,107,106,106,106,106,106,106,106,106,106,106,21,112,200,201,202,
206,207,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,208,209,210,211,106,212,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
47,47,35,213,47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,219,220,92,222,47,33,
33,33,33,33,33,128,128,128,128,128,128,128,128,48,223,92,225,- 1,226,- 1,227,228,
99,99,229,49,99,230,231,232,233,33,33,33,33,33,33,35,35,35,35,35,35,35,35,99,235,
236,237,265,- 1,238,- 1,41,41,239,260,41,101,101,255,250,101,243,240,50,241,244,
245,246,51,52,247,248,249,46,41,53,251,252,253,101,137,137,137,137,137,137,137,
137,254,93,42,42,42,42,42,42,42,42,42,42,242,256,257,258,93,259,28,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,102,100,261,
262,42,263,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,264,91,43,266,267,102,268,33,33,33,33,33,33,33,33,33,33,269,270,272,273,
288,283,279,33,33,33,33,33,33,35,35,35,35,35,35,35,35,280,281,282,36,284,285,286,
44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,287,289,290,291,44,292,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,301,296,297,28,298,299,300,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,302,303,304,305,44,411,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,410,408,78,
- 1,273,273,385,376,273,315,315,315,315,315,315,315,315,351,383,346,341,78,343,
344,409,293,293,273,345,293,275,275,- 1,380,275,274,384,79,79,79,79,79,79,79,79,
79,79,350,293,381,382,379,407,275,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,347,348,349,339,79,387,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,80,294,155,80,287,
287,276,191,287,274,412,295,412,412,404,102,277,100,100,35,35,278,80,270,223,275,
275,287,46,275,293,293,340,412,293,19,162,403,79,79,79,79,79,79,79,79,79,79,275,
100,100,35,35,293,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,177,138,21,31,79,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,89,89,276,31,89,292,292,294,419,292,316,316,
277,420,316,421,422,295,36,423,36,375,- 1,89,424,21,316,316,292,0,316,0,0,316,0,
21,31,0,0,90,90,90,90,90,90,90,90,90,90,316,36,21,36,375,0,375,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,375,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,91,92,92,
300,300,92,0,300,305,305,0,0,305,31,31,31,31,31,31,31,31,0,0,0,92,0,300,412,0,
412,412,305,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,412,0,0,0,0,0,0,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,
0,90,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,97,97,0,377,97,378,378,378,378,378,378,378,378,378,378,0,0,0,0,357,0,357,0,
97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,0,98,98,98,98,98,98,98,98,98,
98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,36,99,99,0,0,99,358,358,358,358,358,358,358,358,358,
358,0,0,0,0,361,0,361,0,99,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,98,
98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,108,108,108,108,108,108,108,
108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,109,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,
0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,0,
0,- 1,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,0,0,0,0,109,109,109,109,109,109,109,109,109,
109,109,109,111,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,28,0,0,124,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,0,0,28,0,0,133,125,125,125,
125,125,125,125,125,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0,127,
127,127,127,127,127,0,0,0,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,31,0,0,-
1,0,138,0,0,127,127,127,127,127,127,405,405,405,405,405,405,405,405,0,126,127,
127,127,127,127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,127,127,127,127,136,
136,136,136,136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,136,136,136,136,0,0,
0,127,127,127,127,127,127,19,0,0,311,0,0,136,136,136,136,136,136,136,136,136,136,
0,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,0,138,312,312,
312,312,312,312,312,312,406,406,406,406,406,406,406,406,0,136,136,136,136,136,
136,0,314,314,314,314,314,314,314,314,314,314,0,0,0,0,0,0,0,314,314,314,314,314,
314,0,0,28,362,362,362,362,362,362,362,362,362,362,0,0,314,314,314,314,314,314,
314,314,314,314,313,314,314,314,314,314,314,314,314,314,314,314,314,0,0,352,0,
363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,314,314,314,
314,314,314,365,0,0,0,0,0,0,0,0,366,0,0,367,352,0,353,353,353,353,353,353,353,
353,353,353,354,0,0,0,0,0,0,365,0,0,0,354,0,0,0,0,366,0,355,367,0,0,0,0,0,0,0,
356,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,354,0,0,0,0,0,0,355,0,0,0,0,0,0,0,0,356,342,
342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,
0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,
0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,359,
359,359,359,359,359,359,359,359,359,0,0,0,0,0,0,0,0,0,0,0,360,93,0,0,0,0,352,93,
353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,
0,354,0,0,360,93,0,0,355,0,0,93,91,0,0,0,0,356,91,0,359,359,359,359,359,359,359,
359,359,359,0,0,0,354,0,0,0,0,0,0,355,360,93,0,91,0,0,0,93,356,91,0,0,362,362,
362,362,362,362,362,362,362,362,0,0,0,0,0,0,0,0,0,0,360,93,93,0,0,0,0,93,93,352,
0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,0,93,0,0,0,0,
373,93,0,0,0,0,0,0,352,374,364,364,364,364,364,364,364,364,364,364,0,0,0,0,0,354,
0,0,0,0,0,354,373,0,0,0,0,0,371,0,0,374,0,0,0,0,0,372,0,0,378,378,378,378,378,
378,378,378,378,378,0,0,0,354,0,0,0,0,0,0,371,360,93,0,0,0,0,0,93,372,368,368,
368,368,368,368,368,368,368,368,0,0,0,0,0,0,0,368,368,368,368,368,368,360,93,0,0,
0,0,0,93,0,0,0,0,0,0,0,368,368,368,368,368,368,368,368,368,368,0,368,368,368,368,
368,368,368,368,368,368,368,368,0,0,0,388,0,369,0,0,389,0,0,0,0,0,370,0,0,390,
390,390,390,390,390,390,390,0,368,368,368,368,368,368,391,0,0,0,0,369,0,0,0,0,0,
0,0,0,370,0,0,0,0,0,0,0,0,0,0,0,0,0,0,392,0,0,0,0,393,394,0,0,0,395,0,0,0,0,0,0,
0,396,0,0,0,397,0,398,0,399,0,400,401,401,401,401,401,401,401,401,401,401,0,0,0,
0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,402,0,0,0,0,
0,0,0,0,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,0,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,19,0,0,414,0,0,0,413,413,413,413,413,
413,413,413,413,413,0,0,0,0,0,0,0,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,0,0,0,0,413,0,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,416,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,0,
0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,0,0,0,0,415,0,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_lex_check[3481]=(const int[3481]){- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,122,124,131,133,308,311,41,41,45,45,41,417,45,- 1,-
1,- 1,- 1,- 1,120,233,414,0,202,10,12,40,10,12,40,41,202,45,203,20,1,1,1,1,1,1,1,1,
1,1,38,48,121,204,10,38,213,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
129,216,217,220,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,
2,2,2,2,2,2,2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,61,62,
7,3,3,3,3,3,7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,30,70,71,3,3,3,74,75,3,3,3,
47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,50,85,27,86,17,27,50,17,4,87,
94,5,95,5,6,8,9,18,114,115,116,5,117,118,119,141,143,143,17,34,34,34,34,34,34,34,
34,144,145,146,27,27,27,27,27,27,27,27,65,65,147,148,65,72,72,77,77,72,149,77,
150,151,152,153,66,66,4,154,66,156,157,65,6,8,5,16,72,16,77,158,142,142,142,159,
142,160,17,66,142,27,161,7,73,73,163,164,73,142,165,166,167,11,11,11,11,11,11,11,
11,11,11,103,27,110,103,73,110,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,111,168,130,111,11,130,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,88,88,66,169,88,170,171,172,
173,174,96,96,104,104,96,175,104,105,105,176,178,105,179,88,180,13,13,80,80,13,
73,80,16,96,181,104,182,183,184,185,105,186,187,188,189,190,192,193,13,194,80,
195,112,112,4,196,112,197,198,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,112,
199,200,201,205,206,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,207,208,209,210,13,211,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,80,212,14,214,215,32,32,32,32,32,32,
32,32,32,32,92,92,218,219,92,221,14,32,32,32,32,32,32,125,125,125,125,125,125,
125,125,14,222,92,224,103,225,110,226,227,99,99,228,14,99,229,230,231,232,32,32,
32,32,32,32,128,128,128,128,128,128,128,128,99,234,235,236,238,111,237,130,15,15,
237,239,15,101,101,240,241,101,242,237,14,237,243,244,245,14,14,246,247,248,249,
15,14,250,251,252,101,134,134,134,134,134,134,134,134,253,254,15,15,15,15,15,15,
15,15,15,15,237,255,256,257,92,258,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,259,99,260,261,15,262,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,263,264,15,265,266,101,
267,33,33,33,33,33,33,33,33,33,33,268,269,271,272,276,277,278,33,33,33,33,33,33,
137,137,137,137,137,137,137,137,279,280,281,282,283,284,285,42,42,42,42,42,42,42,
42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,286,288,289,290,42,291,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,
294,295,296,15,297,298,299,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,301,302,303,304,44,320,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,322,323,78,324,273,273,325,
329,273,312,312,312,312,312,312,312,312,332,326,334,337,78,335,335,323,139,139,
273,344,139,140,140,324,327,140,273,326,78,78,78,78,78,78,78,78,78,78,347,139,
327,327,377,386,140,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,78,78,78,78,78,333,333,333,338,78,324,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,78,78,78,78,78,78,78,78,78,78,78,78,78,79,79,139,388,79,287,287,140,389,287,
391,318,139,318,318,392,393,140,355,356,365,366,140,79,394,395,275,275,287,396,
275,293,293,338,318,293,287,397,398,79,79,79,79,79,79,79,79,79,79,275,355,356,
365,366,293,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,399,406,416,370,79,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,89,89,275,370,89,292,292,293,418,292,309,309,
275,419,309,420,421,293,371,422,372,373,324,89,423,424,316,316,292,- 1,316,- 1,- 1,
309,- 1,309,292,- 1,- 1,89,89,89,89,89,89,89,89,89,89,316,371,316,372,373,- 1,374,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,
- 1,- 1,89,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,89,89,90,90,300,300,90,- 1,300,305,305,- 1,- 1,305,315,315,315,315,315,315,
315,315,- 1,- 1,- 1,90,- 1,300,412,- 1,412,412,305,- 1,- 1,300,- 1,- 1,- 1,- 1,305,90,90,90,
90,90,90,90,90,90,90,412,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,- 1,328,97,328,328,
328,328,328,328,328,328,328,328,- 1,- 1,- 1,- 1,354,- 1,354,- 1,97,354,354,354,354,354,
354,354,354,354,354,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,97,98,98,- 1,- 1,98,357,357,357,357,357,357,357,357,357,357,- 1,-
1,- 1,- 1,360,- 1,360,- 1,98,360,360,360,360,360,360,360,360,360,360,- 1,- 1,- 1,- 1,- 1,
98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,106,106,106,106,
106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,107,107,107,107,107,107,107,107,107,
107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,-
1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,109,- 1,- 1,109,108,- 1,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,
- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,
113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
123,- 1,- 1,123,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,132,- 1,- 1,132,123,123,123,123,
123,123,123,123,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,
126,126,126,126,126,- 1,- 1,- 1,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,-
1,123,- 1,- 1,109,- 1,390,- 1,- 1,126,126,126,126,126,126,390,390,390,390,390,390,390,
390,- 1,123,127,127,127,127,127,127,127,127,127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,
127,127,127,127,135,135,135,135,135,135,135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,
135,135,135,135,135,135,- 1,- 1,- 1,127,127,127,127,127,127,310,- 1,- 1,310,- 1,- 1,136,
136,136,136,136,136,136,136,136,136,- 1,135,135,135,135,135,135,136,136,136,136,
136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,310,310,310,310,310,310,310,310,405,405,405,
405,405,405,405,405,- 1,136,136,136,136,136,136,- 1,313,313,313,313,313,313,313,
313,313,313,- 1,- 1,- 1,- 1,- 1,- 1,- 1,313,313,313,313,313,313,- 1,- 1,310,361,361,361,
361,361,361,361,361,361,361,- 1,- 1,314,314,314,314,314,314,314,314,314,314,310,
313,313,313,313,313,313,314,314,314,314,314,314,- 1,- 1,330,- 1,330,330,330,330,330,
330,330,330,330,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,314,314,314,314,314,314,
330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,330,331,- 1,331,331,331,331,331,331,331,331,
331,331,330,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,330,- 1,331,330,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,
- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,336,336,336,336,336,336,336,336,336,336,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,336,- 1,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,- 1,- 1,- 1,- 1,342,- 1,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,352,352,352,352,352,352,
352,352,352,352,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,352,352,- 1,- 1,- 1,- 1,353,352,353,
353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,- 1,
353,- 1,- 1,352,352,- 1,- 1,353,- 1,- 1,352,358,- 1,- 1,- 1,- 1,353,358,- 1,359,359,359,359,
359,359,359,359,359,359,- 1,- 1,- 1,353,- 1,- 1,- 1,- 1,- 1,- 1,353,359,359,- 1,358,- 1,- 1,
- 1,359,353,358,- 1,- 1,362,362,362,362,362,362,362,362,362,362,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,359,359,362,- 1,- 1,- 1,- 1,359,362,363,- 1,363,363,363,363,363,363,363,
363,363,363,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,363,- 1,362,- 1,- 1,- 1,- 1,363,362,- 1,-
1,- 1,- 1,- 1,- 1,364,363,364,364,364,364,364,364,364,364,364,364,- 1,- 1,- 1,- 1,- 1,363,
- 1,- 1,- 1,- 1,- 1,364,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,378,
378,378,378,378,378,378,378,378,378,- 1,- 1,- 1,364,- 1,- 1,- 1,- 1,- 1,- 1,364,378,378,-
1,- 1,- 1,- 1,- 1,378,364,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,367,367,367,367,367,367,378,378,- 1,- 1,- 1,- 1,- 1,378,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,
368,368,368,368,368,368,368,368,368,- 1,367,367,367,367,367,367,368,368,368,368,
368,368,- 1,- 1,- 1,387,- 1,368,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,387,387,387,387,
387,387,387,387,- 1,368,368,368,368,368,368,387,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,387,387,- 1,-
1,- 1,387,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,387,- 1,387,- 1,387,- 1,387,400,400,400,
400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,
400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,
- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
400,400,400,400,400,400,400,400,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,
401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,-
1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,413,- 1,- 1,413,- 1,- 1,- 1,413,413,413,413,413,413,413,
413,413,413,- 1,- 1,- 1,- 1,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,413,- 1,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,
415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,415,- 1,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1};int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){int
state;int base;int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(
lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(-
state)- 1;}while(1){base=Cyc_lex_base[_check_known_subscript_notnull(425,state)];
if(base < 0)return(- base)- 1;backtrk=Cyc_lex_backtrk[
_check_known_subscript_notnull(425,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]
== state)state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
state=Cyc_lex_default[_check_known_subscript_notnull(425,state)];}if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1)(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4[0]=({
struct Cyc_Lexing_Error_struct _tmp5;_tmp5.tag=Cyc_Lexing_Error;_tmp5.f1=({const
char*_tmp6="empty token";_tag_dyneither(_tmp6,sizeof(char),12);});_tmp5;});_tmp4;}));
else{return lbuf->lex_last_action;}}else{if(c == 256)lbuf->lex_eof_reached=0;}}}
struct _tuple16*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL0: Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(
Cyc_current_args))->tl){Cyc_current_targets=({struct Cyc_Set_Set**_tmp7=_cycalloc(
sizeof(*_tmp7));_tmp7[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);
_tmp7;});}return({struct _tuple16*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->f1=(
struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp8->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets));_tmp8;});case 1: _LL1: return Cyc_line(lexbuf);
case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp9=
_cycalloc(sizeof(*_tmp9));_tmp9[0]=({struct Cyc_Lexing_Error_struct _tmpA;_tmpA.tag=
Cyc_Lexing_Error;_tmpA.f1=({const char*_tmpB="some action didn't return!";
_tag_dyneither(_tmpB,sizeof(char),27);});_tmpA;});_tmp9;}));}struct _tuple16*Cyc_line(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}int Cyc_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL5: Cyc_current_source=({struct _dyneither_ptr*
_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmpC;});Cyc_current_args=0;Cyc_current_targets=({
struct Cyc_Set_Set**_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
_tmpD;});Cyc_token(lexbuf);return 0;case 1: _LL6: Cyc_current_source=({struct
_dyneither_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE[0]=(struct _dyneither_ptr)
Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmpE;});
Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**_tmpF=_cycalloc(
sizeof(*_tmpF));_tmpF[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpF;});Cyc_args(lexbuf);
return 0;case 2: _LL7: Cyc_current_source=({struct _dyneither_ptr*_tmp10=_cycalloc(
sizeof(*_tmp10));_tmp10[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
_tmp10;});Cyc_current_args=0;Cyc_current_targets=({struct Cyc_Set_Set**_tmp11=
_cycalloc(sizeof(*_tmp11));_tmp11[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp11;});
Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);return Cyc_macroname_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp12=
_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_Lexing_Error_struct _tmp13;
_tmp13.tag=Cyc_Lexing_Error;_tmp13.f1=({const char*_tmp14="some action didn't return!";
_tag_dyneither(_tmp14,sizeof(char),27);});_tmp13;});_tmp12;}));}int Cyc_macroname(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}int Cyc_args_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLA: {struct _dyneither_ptr*_tmp15=({struct
_dyneither_ptr*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=(struct _dyneither_ptr)
Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmp17;});
Cyc_current_args=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->hd=
_tmp15;_tmp16->tl=Cyc_current_args;_tmp16;});return Cyc_args(lexbuf);}case 1: _LLB: {
struct _dyneither_ptr*_tmp18=({struct _dyneither_ptr*_tmp1A=_cycalloc(sizeof(*
_tmp1A));_tmp1A[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));_tmp1A;});Cyc_current_args=({struct Cyc_List_List*_tmp19=_cycalloc(
sizeof(*_tmp19));_tmp19->hd=_tmp18;_tmp19->tl=Cyc_current_args;_tmp19;});return
Cyc_args(lexbuf);}case 2: _LLC: {struct _dyneither_ptr*_tmp1B=({struct
_dyneither_ptr*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D[0]=(struct _dyneither_ptr)
Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmp1D;});
Cyc_current_args=({struct Cyc_List_List*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->hd=
_tmp1B;_tmp1C->tl=Cyc_current_args;_tmp1C;});return Cyc_token(lexbuf);}default:
_LLD:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(lexbuf,lexstate);}(int)
_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp1E=_cycalloc(sizeof(*_tmp1E));
_tmp1E[0]=({struct Cyc_Lexing_Error_struct _tmp1F;_tmp1F.tag=Cyc_Lexing_Error;
_tmp1F.f1=({const char*_tmp20="some action didn't return!";_tag_dyneither(_tmp20,
sizeof(char),27);});_tmp1F;});_tmp1E;}));}int Cyc_args(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLF: Cyc_add_target(({struct _dyneither_ptr*_tmp21=_cycalloc(sizeof(*_tmp21));
_tmp21[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp21;}));return Cyc_token(
lexbuf);case 1: _LL10: return 0;case 2: _LL11: return Cyc_token(lexbuf);case 3: _LL12: Cyc_string(
lexbuf);return Cyc_token(lexbuf);case 4: _LL13: return Cyc_token(lexbuf);case 5: _LL14:
return Cyc_token(lexbuf);case 6: _LL15: return Cyc_token(lexbuf);case 7: _LL16: return
Cyc_token(lexbuf);case 8: _LL17: return Cyc_token(lexbuf);case 9: _LL18: return Cyc_token(
lexbuf);case 10: _LL19: return Cyc_token(lexbuf);case 11: _LL1A: return Cyc_token(lexbuf);
case 12: _LL1B: return Cyc_token(lexbuf);case 13: _LL1C: return Cyc_token(lexbuf);case 14:
_LL1D: return Cyc_token(lexbuf);case 15: _LL1E: return Cyc_token(lexbuf);case 16: _LL1F:
return Cyc_token(lexbuf);case 17: _LL20: return Cyc_token(lexbuf);case 18: _LL21: return
Cyc_token(lexbuf);case 19: _LL22: return Cyc_token(lexbuf);case 20: _LL23: return Cyc_token(
lexbuf);case 21: _LL24: return Cyc_token(lexbuf);case 22: _LL25: return Cyc_token(lexbuf);
case 23: _LL26: return Cyc_token(lexbuf);case 24: _LL27: return Cyc_token(lexbuf);case 25:
_LL28: return Cyc_token(lexbuf);case 26: _LL29: return Cyc_token(lexbuf);case 27: _LL2A:
return Cyc_token(lexbuf);case 28: _LL2B: return Cyc_token(lexbuf);case 29: _LL2C: return
Cyc_token(lexbuf);case 30: _LL2D: return Cyc_token(lexbuf);case 31: _LL2E: return Cyc_token(
lexbuf);case 32: _LL2F: return Cyc_token(lexbuf);case 33: _LL30: return Cyc_token(lexbuf);
case 34: _LL31: return Cyc_token(lexbuf);case 35: _LL32: return Cyc_token(lexbuf);case 36:
_LL33: return Cyc_token(lexbuf);case 37: _LL34: return Cyc_token(lexbuf);case 38: _LL35:
return Cyc_token(lexbuf);case 39: _LL36: return Cyc_token(lexbuf);case 40: _LL37: return
Cyc_token(lexbuf);case 41: _LL38: return Cyc_token(lexbuf);case 42: _LL39: return Cyc_token(
lexbuf);case 43: _LL3A: return Cyc_token(lexbuf);case 44: _LL3B: return Cyc_token(lexbuf);
case 45: _LL3C: return Cyc_token(lexbuf);case 46: _LL3D: return Cyc_token(lexbuf);case 47:
_LL3E: return Cyc_token(lexbuf);case 48: _LL3F: return Cyc_token(lexbuf);default: _LL40:(
lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=({
struct Cyc_Lexing_Error_struct _tmp23;_tmp23.tag=Cyc_Lexing_Error;_tmp23.f1=({
const char*_tmp24="some action didn't return!";_tag_dyneither(_tmp24,sizeof(char),
27);});_tmp23;});_tmp22;}));}int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return
Cyc_token_rec(lexbuf,3);}int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL42:
return Cyc_string(lexbuf);case 1: _LL43: return 0;case 2: _LL44: return Cyc_string(lexbuf);
case 3: _LL45: return Cyc_string(lexbuf);case 4: _LL46: return Cyc_string(lexbuf);case 5:
_LL47: return Cyc_string(lexbuf);case 6: _LL48: return Cyc_string(lexbuf);case 7: _LL49:
return 0;case 8: _LL4A: return 0;case 9: _LL4B: return Cyc_string(lexbuf);default: _LL4C:(
lexbuf->refill_buff)(lexbuf);return Cyc_string_rec(lexbuf,lexstate);}(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25[0]=({
struct Cyc_Lexing_Error_struct _tmp26;_tmp26.tag=Cyc_Lexing_Error;_tmp26.f1=({
const char*_tmp27="some action didn't return!";_tag_dyneither(_tmp27,sizeof(char),
27);});_tmp26;});_tmp25;}));}int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_string_rec(lexbuf,4);}int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0:
_LL4E: return 0;case 1: _LL4F: Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(
Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}return 1;case 2: _LL50: Cyc_fputs((
const char*)"*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));({
void*_tmp28=0;Cyc_log(({const char*_tmp29="Warning: declaration of malloc sidestepped\n";
_tag_dyneither(_tmp29,sizeof(char),44);}),_tag_dyneither(_tmp28,sizeof(void*),0));});
return 1;case 3: _LL51: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));({void*_tmp2A=0;Cyc_log(({const
char*_tmp2B="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmp2B,
sizeof(char),44);}),_tag_dyneither(_tmp2A,sizeof(void*),0));});return 1;case 4:
_LL52: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp2C=0;Cyc_log(({const char*_tmp2D="Warning: declaration of calloc sidestepped\n";
_tag_dyneither(_tmp2D,sizeof(char),44);}),_tag_dyneither(_tmp2C,sizeof(void*),0));});
return 1;case 5: _LL53: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));({void*_tmp2E=0;Cyc_log(({const
char*_tmp2F="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmp2F,
sizeof(char),44);}),_tag_dyneither(_tmp2E,sizeof(void*),0));});return 1;case 6:
_LL54: Cyc_fputs((const char*)"__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));({
void*_tmp30=0;Cyc_log(({const char*_tmp31="Warning: use of region sidestepped\n";
_tag_dyneither(_tmp31,sizeof(char),36);}),_tag_dyneither(_tmp30,sizeof(void*),0));});
return 1;case 7: _LL55:({void*_tmp32=0;Cyc_log(({const char*_tmp33="Warning: use of __extension__ deleted\n";
_tag_dyneither(_tmp33,sizeof(char),39);}),_tag_dyneither(_tmp32,sizeof(void*),0));});
return 1;case 8: _LL56:({void*_tmp34=0;Cyc_log(({const char*_tmp35="Warning: use of mode HI deleted\n";
_tag_dyneither(_tmp35,sizeof(char),33);}),_tag_dyneither(_tmp34,sizeof(void*),0));});
return 1;case 9: _LL57:({void*_tmp36=0;Cyc_log(({const char*_tmp37="Warning: use of mode SI deleted\n";
_tag_dyneither(_tmp37,sizeof(char),33);}),_tag_dyneither(_tmp36,sizeof(void*),0));});
return 1;case 10: _LL58:({void*_tmp38=0;Cyc_log(({const char*_tmp39="Warning: use of mode QI deleted\n";
_tag_dyneither(_tmp39,sizeof(char),33);}),_tag_dyneither(_tmp38,sizeof(void*),0));});
return 1;case 11: _LL59:({void*_tmp3A=0;Cyc_log(({const char*_tmp3B="Warning: use of mode DI deleted\n";
_tag_dyneither(_tmp3B,sizeof(char),33);}),_tag_dyneither(_tmp3A,sizeof(void*),0));});
return 1;case 12: _LL5A:({void*_tmp3C=0;Cyc_log(({const char*_tmp3D="Warning: use of mode DI deleted\n";
_tag_dyneither(_tmp3D,sizeof(char),33);}),_tag_dyneither(_tmp3C,sizeof(void*),0));});
return 1;case 13: _LL5B:({void*_tmp3E=0;Cyc_log(({const char*_tmp3F="Warning: use of mode word deleted\n";
_tag_dyneither(_tmp3F,sizeof(char),35);}),_tag_dyneither(_tmp3E,sizeof(void*),0));});
return 1;case 14: _LL5C: Cyc_fputs((const char*)"inline",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 15: _LL5D: Cyc_fputs((const char*)"inline",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LL5E: Cyc_fputs((
const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 17: _LL5F: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 18: _LL60: Cyc_fputs((const char*)"int",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 19: _LL61: return 1;case 20: _LL62: Cyc_parens_to_match=
1;while(Cyc_asm(lexbuf)){;}Cyc_fputs((const char*)"0",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));({void*_tmp40=0;Cyc_log(({const char*_tmp41="Warning: replacing use of __asm__ with 0\n";
_tag_dyneither(_tmp41,sizeof(char),42);}),_tag_dyneither(_tmp40,sizeof(void*),0));});
return 1;case 21: _LL63: Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;default: _LL64:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42[0]=({struct Cyc_Lexing_Error_struct
_tmp43;_tmp43.tag=Cyc_Lexing_Error;_tmp43.f1=({const char*_tmp44="some action didn't return!";
_tag_dyneither(_tmp44,sizeof(char),27);});_tmp43;});_tmp42;}));}int Cyc_slurp(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}int Cyc_slurp_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL66: return 0;case 1: _LL67: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL68:({void*
_tmp45=0;Cyc_log(({const char*_tmp46="Warning: unclosed string\n";_tag_dyneither(
_tmp46,sizeof(char),26);}),_tag_dyneither(_tmp45,sizeof(void*),0));});({struct
Cyc_String_pa_struct _tmp49;_tmp49.tag=0;_tmp49.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp47[1]={& _tmp49};Cyc_fprintf((
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp48="%s";
_tag_dyneither(_tmp48,sizeof(char),3);}),_tag_dyneither(_tmp47,sizeof(void*),1));});});
return 1;case 3: _LL69:({struct Cyc_String_pa_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*
_tmp4A[1]={& _tmp4C};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp4B="%s";_tag_dyneither(_tmp4B,sizeof(char),3);}),_tag_dyneither(
_tmp4A,sizeof(void*),1));});});return 1;case 4: _LL6A:({struct Cyc_String_pa_struct
_tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));({void*_tmp4D[1]={& _tmp4F};Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),({const char*_tmp4E="%s";_tag_dyneither(_tmp4E,sizeof(
char),3);}),_tag_dyneither(_tmp4D,sizeof(void*),1));});});return 1;case 5: _LL6B:({
struct Cyc_String_pa_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp50[1]={& _tmp52};Cyc_fprintf((
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp51="%s";
_tag_dyneither(_tmp51,sizeof(char),3);}),_tag_dyneither(_tmp50,sizeof(void*),1));});});
return 1;case 6: _LL6C:({struct Cyc_String_pa_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*
_tmp53[1]={& _tmp55};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({
const char*_tmp54="%s";_tag_dyneither(_tmp54,sizeof(char),3);}),_tag_dyneither(
_tmp53,sizeof(void*),1));});});return 1;case 7: _LL6D:({struct Cyc_String_pa_struct
_tmp58;_tmp58.tag=0;_tmp58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));({void*_tmp56[1]={& _tmp58};Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),({const char*_tmp57="%s";_tag_dyneither(_tmp57,sizeof(
char),3);}),_tag_dyneither(_tmp56,sizeof(void*),1));});});return 1;case 8: _LL6E:({
struct Cyc_String_pa_struct _tmp5B;_tmp5B.tag=0;_tmp5B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp59[1]={& _tmp5B};Cyc_fprintf((
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp5A="%s";
_tag_dyneither(_tmp5A,sizeof(char),3);}),_tag_dyneither(_tmp59,sizeof(void*),1));});});
return 1;default: _LL6F:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_string_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp5C=
_cycalloc(sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_Lexing_Error_struct _tmp5D;
_tmp5D.tag=Cyc_Lexing_Error;_tmp5D.f1=({const char*_tmp5E="some action didn't return!";
_tag_dyneither(_tmp5E,sizeof(char),27);});_tmp5D;});_tmp5C;}));}int Cyc_slurp_string(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}int Cyc_asm_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL71: return 0;case 1: _LL72: if(Cyc_parens_to_match
== 1)return 0;-- Cyc_parens_to_match;return 1;case 2: _LL73: ++ Cyc_parens_to_match;
return 1;case 3: _LL74: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL75: while(
Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL76: return 1;case 6: _LL77: return 1;
default: _LL78:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp5F=_cycalloc(sizeof(*
_tmp5F));_tmp5F[0]=({struct Cyc_Lexing_Error_struct _tmp60;_tmp60.tag=Cyc_Lexing_Error;
_tmp60.f1=({const char*_tmp61="some action didn't return!";_tag_dyneither(_tmp61,
sizeof(char),27);});_tmp60;});_tmp5F;}));}int Cyc_asm(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL7A:({void*_tmp62=0;Cyc_log(({const char*_tmp63="Warning: unclosed string\n";
_tag_dyneither(_tmp63,sizeof(char),26);}),_tag_dyneither(_tmp62,sizeof(void*),0));});
return 0;case 1: _LL7B: return 0;case 2: _LL7C:({void*_tmp64=0;Cyc_log(({const char*
_tmp65="Warning: unclosed string\n";_tag_dyneither(_tmp65,sizeof(char),26);}),
_tag_dyneither(_tmp64,sizeof(void*),0));});return 1;case 3: _LL7D: return 1;case 4:
_LL7E: return 1;case 5: _LL7F: return 1;case 6: _LL80: return 1;case 7: _LL81: return 1;case 8:
_LL82: return 1;default: _LL83:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_string_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp66=
_cycalloc(sizeof(*_tmp66));_tmp66[0]=({struct Cyc_Lexing_Error_struct _tmp67;
_tmp67.tag=Cyc_Lexing_Error;_tmp67.f1=({const char*_tmp68="some action didn't return!";
_tag_dyneither(_tmp68,sizeof(char),27);});_tmp67;});_tmp66;}));}int Cyc_asm_string(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}int Cyc_asm_comment_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL85:({void*_tmp69=0;Cyc_log(({const char*_tmp6A="Warning: unclosed comment\n";
_tag_dyneither(_tmp6A,sizeof(char),27);}),_tag_dyneither(_tmp69,sizeof(void*),0));});
return 0;case 1: _LL86: return 0;case 2: _LL87: return 1;default: _LL88:(lexbuf->refill_buff)(
lexbuf);return Cyc_asm_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=({struct Cyc_Lexing_Error_struct
_tmp6C;_tmp6C.tag=Cyc_Lexing_Error;_tmp6C.f1=({const char*_tmp6D="some action didn't return!";
_tag_dyneither(_tmp6D,sizeof(char),27);});_tmp6C;});_tmp6B;}));}int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}struct
_tuple17*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8A: Cyc_current_line=({
const char*_tmp6E="#define ";_tag_dyneither(_tmp6E,sizeof(char),9);});Cyc_suck_macroname(
lexbuf);return({struct _tuple17*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->f1=Cyc_current_line;
_tmp6F->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp6F;});case
1: _LL8B: return Cyc_suck_line(lexbuf);case 2: _LL8C: return 0;default: _LL8D:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_line_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Lexing_Error_struct
_tmp71;_tmp71.tag=Cyc_Lexing_Error;_tmp71.f1=({const char*_tmp72="some action didn't return!";
_tag_dyneither(_tmp72,sizeof(char),27);});_tmp71;});_tmp70;}));}struct _tuple17*
Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8F: Cyc_current_source=({
struct _dyneither_ptr*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73[0]=(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp73;});Cyc_current_line=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct
_dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));return
Cyc_suck_restofline(lexbuf);default: _LL90:(lexbuf->refill_buff)(lexbuf);return
Cyc_suck_macroname_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Lexing_Error_struct
_tmp75;_tmp75.tag=Cyc_Lexing_Error;_tmp75.f1=({const char*_tmp76="some action didn't return!";
_tag_dyneither(_tmp76,sizeof(char),27);});_tmp75;});_tmp74;}));}int Cyc_suck_macroname(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}int Cyc_suck_restofline_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL92: Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((
struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 0;default: _LL93:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_restofline_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp77=
_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Lexing_Error_struct _tmp78;
_tmp78.tag=Cyc_Lexing_Error;_tmp78.f1=({const char*_tmp79="some action didn't return!";
_tag_dyneither(_tmp79,sizeof(char),27);});_tmp78;});_tmp77;}));}int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}struct
_tuple19*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL95: return Cyc_spec(lexbuf);case 1: _LL96:
Cyc_current_headerfile=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf),0,(unsigned int)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=0;Cyc_current_cstubs=
0;Cyc_current_cycstubs=0;Cyc_current_hstubs=0;while(Cyc_commands(lexbuf)){;}Cyc_current_hstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cstubs);Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_current_cycstubs);return({struct _tuple19*_tmp7A=
_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=Cyc_current_headerfile;_tmp7A->f2=Cyc_current_symbols;
_tmp7A->f3=Cyc_current_omit_symbols;_tmp7A->f4=Cyc_current_hstubs;_tmp7A->f5=Cyc_current_cstubs;
_tmp7A->f6=Cyc_current_cycstubs;_tmp7A;});case 2: _LL97: return Cyc_spec(lexbuf);
case 3: _LL98: return 0;case 4: _LL99:({struct Cyc_Int_pa_struct _tmp7D;_tmp7D.tag=1;
_tmp7D.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmp7B[1]={&
_tmp7D};Cyc_fprintf(Cyc_stderr,({const char*_tmp7C="Error in .cys file: expected header file name, found '%c' instead\n";
_tag_dyneither(_tmp7C,sizeof(char),67);}),_tag_dyneither(_tmp7B,sizeof(void*),1));});});
return 0;default: _LL9A:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp7E=_cycalloc(
sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Lexing_Error_struct _tmp7F;_tmp7F.tag=Cyc_Lexing_Error;
_tmp7F.f1=({const char*_tmp80="some action didn't return!";_tag_dyneither(_tmp80,
sizeof(char),27);});_tmp7F;});_tmp7E;}));}struct _tuple19*Cyc_spec(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_spec_rec(lexbuf,13);}int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL9C: return 0;case 1: _LL9D: return 0;case 2: _LL9E: Cyc_snarfed_symbols=0;while(
Cyc_snarfsymbols(lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL9F: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LLA0: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple18*x=({struct _tuple18*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=(
struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp82->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp82;});Cyc_current_hstubs=({
struct Cyc_List_List*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->hd=x;_tmp81->tl=
Cyc_current_hstubs;_tmp81;});return 1;};case 5: _LLA1: {struct _dyneither_ptr _tmp83=
Cyc_Lexing_lexeme(lexbuf);_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),(int)
Cyc_strlen(({const char*_tmp84="hstub";_tag_dyneither(_tmp84,sizeof(char),6);})));
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp83,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),1);}{struct _dyneither_ptr t=
_tmp83;while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp85=Cyc_substring((
struct _dyneither_ptr)_tmp83,0,(unsigned int)((t.curr - _tmp83.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple18*x=({struct _tuple18*_tmp87=_cycalloc(
sizeof(*_tmp87));_tmp87->f1=(struct _dyneither_ptr)_tmp85;_tmp87->f2=(struct
_dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp87;});Cyc_current_hstubs=({struct Cyc_List_List*_tmp86=_cycalloc(sizeof(*
_tmp86));_tmp86->hd=x;_tmp86->tl=Cyc_current_hstubs;_tmp86;});return 1;};};};}
case 6: _LLA2: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);while(Cyc_block(lexbuf)){;}{struct _tuple18*x=({struct _tuple18*_tmp89=
_cycalloc(sizeof(*_tmp89));_tmp89->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);
_tmp89->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp89;});Cyc_current_cstubs=({struct Cyc_List_List*
_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->hd=x;_tmp88->tl=Cyc_current_cstubs;
_tmp88;});return 1;};case 7: _LLA3: {struct _dyneither_ptr _tmp8A=Cyc_Lexing_lexeme(
lexbuf);_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),(int)Cyc_strlen(({const
char*_tmp8B="cstub";_tag_dyneither(_tmp8B,sizeof(char),6);})));while(isspace((
int)*((char*)_check_dyneither_subscript(_tmp8A,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),1);}{struct _dyneither_ptr t=
_tmp8A;while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp8C=Cyc_substring((
struct _dyneither_ptr)_tmp8A,0,(unsigned int)((t.curr - _tmp8A.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple18*x=({struct _tuple18*_tmp8E=_cycalloc(
sizeof(*_tmp8E));_tmp8E->f1=(struct _dyneither_ptr)_tmp8C;_tmp8E->f2=(struct
_dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp8E;});Cyc_current_cstubs=({struct Cyc_List_List*_tmp8D=_cycalloc(sizeof(*
_tmp8D));_tmp8D->hd=x;_tmp8D->tl=Cyc_current_cstubs;_tmp8D;});return 1;};};};}
case 8: _LLA4: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);while(Cyc_block(lexbuf)){;}{struct _tuple18*x=({struct _tuple18*_tmp90=
_cycalloc(sizeof(*_tmp90));_tmp90->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);
_tmp90->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf));_tmp90;});Cyc_current_cycstubs=({struct Cyc_List_List*
_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->hd=x;_tmp8F->tl=Cyc_current_cycstubs;
_tmp8F;});return 1;};case 9: _LLA5: {struct _dyneither_ptr _tmp91=Cyc_Lexing_lexeme(
lexbuf);_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),(int)Cyc_strlen(({const
char*_tmp92="cycstub";_tag_dyneither(_tmp92,sizeof(char),8);})));while(isspace((
int)*((char*)_check_dyneither_subscript(_tmp91,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),1);}{struct _dyneither_ptr t=
_tmp91;while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp93=Cyc_substring((
struct _dyneither_ptr)_tmp91,0,(unsigned int)((t.curr - _tmp91.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple18*x=({struct _tuple18*_tmp95=_cycalloc(
sizeof(*_tmp95));_tmp95->f1=(struct _dyneither_ptr)_tmp93;_tmp95->f2=(struct
_dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));
_tmp95;});Cyc_current_cycstubs=({struct Cyc_List_List*_tmp94=_cycalloc(sizeof(*
_tmp94));_tmp94->hd=x;_tmp94->tl=Cyc_current_cycstubs;_tmp94;});return 1;};};};}
case 10: _LLA6: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);while(Cyc_block(lexbuf)){;}{struct _dyneither_ptr*x=({struct _dyneither_ptr*
_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp97;});Cyc_current_cpp=({struct
Cyc_List_List*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->hd=x;_tmp96->tl=Cyc_current_cpp;
_tmp96;});return 1;};case 11: _LLA7: return 1;case 12: _LLA8: return 1;case 13: _LLA9:({
struct Cyc_Int_pa_struct _tmp9A;_tmp9A.tag=1;_tmp9A.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0));({void*_tmp98[1]={& _tmp9A};Cyc_fprintf(Cyc_stderr,({const char*_tmp99="Error in .cys file: expected command, found '%c' instead\n";
_tag_dyneither(_tmp99,sizeof(char),58);}),_tag_dyneither(_tmp98,sizeof(void*),1));});});
return 0;default: _LLAA:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp9B=
_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Lexing_Error_struct _tmp9C;
_tmp9C.tag=Cyc_Lexing_Error;_tmp9C.f1=({const char*_tmp9D="some action didn't return!";
_tag_dyneither(_tmp9D,sizeof(char),27);});_tmp9C;});_tmp9B;}));}int Cyc_commands(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}int Cyc_snarfsymbols_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLAC: Cyc_snarfed_symbols=({struct Cyc_List_List*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->hd=({struct _dyneither_ptr*_tmp9F=
_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf);_tmp9F;});_tmp9E->tl=Cyc_snarfed_symbols;_tmp9E;});return 1;case 1: _LLAD:
return 1;case 2: _LLAE: return 0;case 3: _LLAF:({void*_tmpA0=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmpA1="Error in .cys file: unexpected end-of-file\n";_tag_dyneither(
_tmpA1,sizeof(char),44);}),_tag_dyneither(_tmpA0,sizeof(void*),0));});return 0;
case 4: _LLB0:({struct Cyc_Int_pa_struct _tmpA4;_tmpA4.tag=1;_tmpA4.f1=(
unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmpA2[1]={& _tmpA4};
Cyc_fprintf(Cyc_stderr,({const char*_tmpA3="Error in .cys file: expected symbol, found '%c' instead\n";
_tag_dyneither(_tmpA3,sizeof(char),57);}),_tag_dyneither(_tmpA2,sizeof(void*),1));});});
return 0;default: _LLB1:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(
lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpA5=
_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_Lexing_Error_struct _tmpA6;
_tmpA6.tag=Cyc_Lexing_Error;_tmpA6.f1=({const char*_tmpA7="some action didn't return!";
_tag_dyneither(_tmpA7,sizeof(char),27);});_tmpA6;});_tmpA5;}));}int Cyc_snarfsymbols(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}int Cyc_block_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLB3:({void*_tmpA8=0;Cyc_log(({const char*_tmpA9="Warning: unclosed brace\n";
_tag_dyneither(_tmpA9,sizeof(char),25);}),_tag_dyneither(_tmpA8,sizeof(void*),0));});
return 0;case 1: _LLB4: if(Cyc_braces_to_match == 1)return 0;-- Cyc_braces_to_match;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');return 1;case 2: _LLB5: ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;
case 3: _LLB6: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}return 1;case 4: _LLB7: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),({const char*_tmpAA="/*";
_tag_dyneither(_tmpAA,sizeof(char),3);}));while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LLB8: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLB9: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLBA:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpAB=_cycalloc(
sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Lexing_Error_struct _tmpAC;_tmpAC.tag=Cyc_Lexing_Error;
_tmpAC.f1=({const char*_tmpAD="some action didn't return!";_tag_dyneither(_tmpAD,
sizeof(char),27);});_tmpAC;});_tmpAB;}));}int Cyc_block(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLBC:({void*_tmpAE=0;Cyc_log(({const char*_tmpAF="Warning: unclosed string\n";
_tag_dyneither(_tmpAF,sizeof(char),26);}),_tag_dyneither(_tmpAE,sizeof(void*),0));});
return 0;case 1: _LLBD: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
return 0;case 2: _LLBE:({void*_tmpB0=0;Cyc_log(({const char*_tmpB1="Warning: unclosed string\n";
_tag_dyneither(_tmpB1,sizeof(char),26);}),_tag_dyneither(_tmpB0,sizeof(void*),0));});
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 3: _LLBF: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 4: _LLC0: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 5: _LLC1: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLC2: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 7: _LLC3: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 8: _LLC4: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLC5:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_string_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Lexing_Error_struct
_tmpB3;_tmpB3.tag=Cyc_Lexing_Error;_tmpB3.f1=({const char*_tmpB4="some action didn't return!";
_tag_dyneither(_tmpB4,sizeof(char),27);});_tmpB3;});_tmpB2;}));}int Cyc_block_string(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}int Cyc_block_comment_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLC7:({void*_tmpB5=0;Cyc_log(({const char*_tmpB6="Warning: unclosed comment\n";
_tag_dyneither(_tmpB6,sizeof(char),27);}),_tag_dyneither(_tmpB5,sizeof(void*),0));});
return 0;case 1: _LLC8: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),({
const char*_tmpB7="*/";_tag_dyneither(_tmpB7,sizeof(char),3);}));return 0;case 2:
_LLC9: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLCA:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({
struct Cyc_Lexing_Error_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({
struct Cyc_Lexing_Error_struct _tmpB9;_tmpB9.tag=Cyc_Lexing_Error;_tmpB9.f1=({
const char*_tmpBA="some action didn't return!";_tag_dyneither(_tmpBA,sizeof(char),
27);});_tmpB9;});_tmpB8;}));}int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_block_comment_rec(lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple20{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*
e){void*_tmpBB=((struct Cyc_Absyn_Exp*)_check_null(e))->r;struct _tuple0*_tmpBD;
struct _tuple0 _tmpBE;struct _dyneither_ptr*_tmpBF;struct _tuple0*_tmpC1;struct
_tuple0 _tmpC2;struct _dyneither_ptr*_tmpC3;struct Cyc_List_List*_tmpC5;struct Cyc_Absyn_Exp*
_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*
_tmpCB;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*
_tmpD0;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*
_tmpD6;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*
_tmpDA;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*
_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_List_List*
_tmpE3;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_List_List*_tmpE6;void*_tmpE8;struct
Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_MallocInfo _tmpEB;int _tmpEC;struct Cyc_Absyn_Exp*
_tmpED;void**_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF1;void*
_tmpF3;void*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;struct _dyneither_ptr*_tmpF8;struct
Cyc_Absyn_Exp*_tmpFA;struct _dyneither_ptr*_tmpFB;void*_tmpFD;void*_tmpFE;struct
Cyc_List_List*_tmp101;_LLCD: {struct Cyc_Absyn_Var_e_struct*_tmpBC=(struct Cyc_Absyn_Var_e_struct*)
_tmpBB;if(_tmpBC->tag != 1)goto _LLCF;else{_tmpBD=_tmpBC->f1;_tmpBE=*_tmpBD;_tmpBF=
_tmpBE.f2;}}_LLCE: _tmpC3=_tmpBF;goto _LLD0;_LLCF: {struct Cyc_Absyn_UnknownId_e_struct*
_tmpC0=(struct Cyc_Absyn_UnknownId_e_struct*)_tmpBB;if(_tmpC0->tag != 2)goto _LLD1;
else{_tmpC1=_tmpC0->f1;_tmpC2=*_tmpC1;_tmpC3=_tmpC2.f2;}}_LLD0: Cyc_add_target(
_tmpC3);return;_LLD1: {struct Cyc_Absyn_Primop_e_struct*_tmpC4=(struct Cyc_Absyn_Primop_e_struct*)
_tmpBB;if(_tmpC4->tag != 3)goto _LLD3;else{_tmpC5=_tmpC4->f2;}}_LLD2: for(0;_tmpC5
!= 0;_tmpC5=_tmpC5->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)
_tmpC5->hd));}return;_LLD3: {struct Cyc_Absyn_Subscript_e_struct*_tmpC6=(struct
Cyc_Absyn_Subscript_e_struct*)_tmpBB;if(_tmpC6->tag != 25)goto _LLD5;else{_tmpC7=
_tmpC6->f1;_tmpC8=_tmpC6->f2;}}_LLD4: _tmpCA=_tmpC7;_tmpCB=_tmpC8;goto _LLD6;_LLD5: {
struct Cyc_Absyn_SeqExp_e_struct*_tmpC9=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpBB;
if(_tmpC9->tag != 9)goto _LLD7;else{_tmpCA=_tmpC9->f1;_tmpCB=_tmpC9->f2;}}_LLD6:
_tmpCD=_tmpCA;_tmpCE=_tmpCB;goto _LLD8;_LLD7: {struct Cyc_Absyn_AssignOp_e_struct*
_tmpCC=(struct Cyc_Absyn_AssignOp_e_struct*)_tmpBB;if(_tmpCC->tag != 4)goto _LLD9;
else{_tmpCD=_tmpCC->f1;_tmpCE=_tmpCC->f3;}}_LLD8: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpCD);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCE);return;_LLD9: {struct Cyc_Absyn_Deref_e_struct*
_tmpCF=(struct Cyc_Absyn_Deref_e_struct*)_tmpBB;if(_tmpCF->tag != 22)goto _LLDB;
else{_tmpD0=_tmpCF->f1;}}_LLDA: _tmpD2=_tmpD0;goto _LLDC;_LLDB: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpD1=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpBB;if(_tmpD1->tag != 19)goto _LLDD;
else{_tmpD2=_tmpD1->f1;}}_LLDC: _tmpD4=_tmpD2;goto _LLDE;_LLDD: {struct Cyc_Absyn_Address_e_struct*
_tmpD3=(struct Cyc_Absyn_Address_e_struct*)_tmpBB;if(_tmpD3->tag != 16)goto _LLDF;
else{_tmpD4=_tmpD3->f1;}}_LLDE: _tmpD6=_tmpD4;goto _LLE0;_LLDF: {struct Cyc_Absyn_Increment_e_struct*
_tmpD5=(struct Cyc_Absyn_Increment_e_struct*)_tmpBB;if(_tmpD5->tag != 5)goto _LLE1;
else{_tmpD6=_tmpD5->f1;}}_LLE0: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD6);
return;_LLE1: {struct Cyc_Absyn_Conditional_e_struct*_tmpD7=(struct Cyc_Absyn_Conditional_e_struct*)
_tmpBB;if(_tmpD7->tag != 6)goto _LLE3;else{_tmpD8=_tmpD7->f1;_tmpD9=_tmpD7->f2;
_tmpDA=_tmpD7->f3;}}_LLE2: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD8);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpD9);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDA);return;
_LLE3: {struct Cyc_Absyn_And_e_struct*_tmpDB=(struct Cyc_Absyn_And_e_struct*)
_tmpBB;if(_tmpDB->tag != 7)goto _LLE5;else{_tmpDC=_tmpDB->f1;_tmpDD=_tmpDB->f2;}}
_LLE4: _tmpDF=_tmpDC;_tmpE0=_tmpDD;goto _LLE6;_LLE5: {struct Cyc_Absyn_Or_e_struct*
_tmpDE=(struct Cyc_Absyn_Or_e_struct*)_tmpBB;if(_tmpDE->tag != 8)goto _LLE7;else{
_tmpDF=_tmpDE->f1;_tmpE0=_tmpDE->f2;}}_LLE6: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpDF);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE0);return;_LLE7: {struct Cyc_Absyn_FnCall_e_struct*
_tmpE1=(struct Cyc_Absyn_FnCall_e_struct*)_tmpBB;if(_tmpE1->tag != 11)goto _LLE9;
else{_tmpE2=_tmpE1->f1;_tmpE3=_tmpE1->f2;}}_LLE8: _tmpE5=_tmpE2;_tmpE6=_tmpE3;
goto _LLEA;_LLE9: {struct Cyc_Absyn_UnknownCall_e_struct*_tmpE4=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpBB;if(_tmpE4->tag != 10)goto _LLEB;else{_tmpE5=_tmpE4->f1;_tmpE6=_tmpE4->f2;}}
_LLEA: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE5);for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpE6->hd));}return;
_LLEB: {struct Cyc_Absyn_Cast_e_struct*_tmpE7=(struct Cyc_Absyn_Cast_e_struct*)
_tmpBB;if(_tmpE7->tag != 15)goto _LLED;else{_tmpE8=(void*)_tmpE7->f1;_tmpE9=_tmpE7->f2;}}
_LLEC: Cyc_scan_type(_tmpE8);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE9);return;
_LLED: {struct Cyc_Absyn_Malloc_e_struct*_tmpEA=(struct Cyc_Absyn_Malloc_e_struct*)
_tmpBB;if(_tmpEA->tag != 35)goto _LLEF;else{_tmpEB=_tmpEA->f1;_tmpEC=_tmpEB.is_calloc;
_tmpED=_tmpEB.rgn;_tmpEE=_tmpEB.elt_type;_tmpEF=_tmpEB.num_elts;}}_LLEE: if(
_tmpED != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpED));
if(_tmpEE != 0)Cyc_scan_type(*_tmpEE);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpEF);
return;_LLEF: {struct Cyc_Absyn_Tagcheck_e_struct*_tmpF0=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmpBB;if(_tmpF0->tag != 39)goto _LLF1;else{_tmpF1=_tmpF0->f1;}}_LLF0: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpF1);return;_LLF1: {struct Cyc_Absyn_Valueof_e_struct*
_tmpF2=(struct Cyc_Absyn_Valueof_e_struct*)_tmpBB;if(_tmpF2->tag != 40)goto _LLF3;
else{_tmpF3=(void*)_tmpF2->f1;}}_LLF2: _tmpF5=_tmpF3;goto _LLF4;_LLF3: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpF4=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpBB;if(_tmpF4->tag != 18)goto _LLF5;
else{_tmpF5=(void*)_tmpF4->f1;}}_LLF4: Cyc_scan_type(_tmpF5);return;_LLF5: {
struct Cyc_Absyn_AggrMember_e_struct*_tmpF6=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpBB;if(_tmpF6->tag != 23)goto _LLF7;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;}}
_LLF6: _tmpFA=_tmpF7;_tmpFB=_tmpF8;goto _LLF8;_LLF7: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmpF9=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmpBB;if(_tmpF9->tag != 24)goto _LLF9;
else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpF9->f2;}}_LLF8: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpFA);Cyc_add_target(_tmpFB);return;_LLF9: {struct Cyc_Absyn_Offsetof_e_struct*
_tmpFC=(struct Cyc_Absyn_Offsetof_e_struct*)_tmpBB;if(_tmpFC->tag != 20)goto _LLFB;
else{_tmpFD=(void*)_tmpFC->f1;_tmpFE=(void*)_tmpFC->f2;}}_LLFA: Cyc_scan_type(
_tmpFD);{void*_tmp112=_tmpFE;struct _dyneither_ptr*_tmp114;_LL120: {struct Cyc_Absyn_StructField_struct*
_tmp113=(struct Cyc_Absyn_StructField_struct*)_tmp112;if(_tmp113->tag != 0)goto
_LL122;else{_tmp114=_tmp113->f1;}}_LL121: Cyc_add_target(_tmp114);goto _LL11F;
_LL122: {struct Cyc_Absyn_TupleIndex_struct*_tmp115=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp112;if(_tmp115->tag != 1)goto _LL11F;}_LL123: goto _LL11F;_LL11F:;}return;_LLFB: {
struct Cyc_Absyn_Const_e_struct*_tmpFF=(struct Cyc_Absyn_Const_e_struct*)_tmpBB;
if(_tmpFF->tag != 0)goto _LLFD;}_LLFC: return;_LLFD: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp100=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpBB;if(_tmp100->tag != 37)
goto _LLFF;else{_tmp101=_tmp100->f2;}}_LLFE: for(0;_tmp101 != 0;_tmp101=_tmp101->tl){
struct _tuple20 _tmp117;struct Cyc_Absyn_Exp*_tmp118;struct _tuple20*_tmp116=(struct
_tuple20*)_tmp101->hd;_tmp117=*_tmp116;_tmp118=_tmp117.f2;Cyc_scan_exp((struct
Cyc_Absyn_Exp*)_tmp118);}return;_LLFF: {struct Cyc_Absyn_Swap_e_struct*_tmp102=(
struct Cyc_Absyn_Swap_e_struct*)_tmpBB;if(_tmp102->tag != 36)goto _LL101;}_LL100:({
void*_tmp119=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp11A="Error: unexpected Swap_e\n";
_tag_dyneither(_tmp11A,sizeof(char),26);}),_tag_dyneither(_tmp119,sizeof(void*),
0));});exit(1);return;_LL101: {struct Cyc_Absyn_StmtExp_e_struct*_tmp103=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmpBB;if(_tmp103->tag != 38)goto _LL103;}_LL102:({
void*_tmp11B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp11C="Error: unexpected Stmt_e\n";
_tag_dyneither(_tmp11C,sizeof(char),26);}),_tag_dyneither(_tmp11B,sizeof(void*),
0));});exit(1);return;_LL103: {struct Cyc_Absyn_Throw_e_struct*_tmp104=(struct Cyc_Absyn_Throw_e_struct*)
_tmpBB;if(_tmp104->tag != 12)goto _LL105;}_LL104:({void*_tmp11D=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp11E="Error: unexpected Throw_e\n";_tag_dyneither(_tmp11E,sizeof(
char),27);}),_tag_dyneither(_tmp11D,sizeof(void*),0));});exit(1);return;_LL105: {
struct Cyc_Absyn_NoInstantiate_e_struct*_tmp105=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpBB;if(_tmp105->tag != 13)goto _LL107;}_LL106:({void*_tmp11F=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp120="Error: unexpected NoInstantiate_e\n";_tag_dyneither(_tmp120,
sizeof(char),35);}),_tag_dyneither(_tmp11F,sizeof(void*),0));});exit(1);return;
_LL107: {struct Cyc_Absyn_Instantiate_e_struct*_tmp106=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpBB;if(_tmp106->tag != 14)goto _LL109;}_LL108:({void*_tmp121=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp122="Error: unexpected Instantiate_e\n";_tag_dyneither(_tmp122,
sizeof(char),33);}),_tag_dyneither(_tmp121,sizeof(void*),0));});exit(1);return;
_LL109: {struct Cyc_Absyn_New_e_struct*_tmp107=(struct Cyc_Absyn_New_e_struct*)
_tmpBB;if(_tmp107->tag != 17)goto _LL10B;}_LL10A:({void*_tmp123=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp124="Error: unexpected New_e\n";_tag_dyneither(_tmp124,sizeof(char),
25);}),_tag_dyneither(_tmp123,sizeof(void*),0));});exit(1);return;_LL10B: {
struct Cyc_Absyn_Gentyp_e_struct*_tmp108=(struct Cyc_Absyn_Gentyp_e_struct*)_tmpBB;
if(_tmp108->tag != 21)goto _LL10D;}_LL10C:({void*_tmp125=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp126="Error: unexpected Gentyp_e\n";_tag_dyneither(_tmp126,sizeof(
char),28);}),_tag_dyneither(_tmp125,sizeof(void*),0));});exit(1);return;_LL10D: {
struct Cyc_Absyn_Tuple_e_struct*_tmp109=(struct Cyc_Absyn_Tuple_e_struct*)_tmpBB;
if(_tmp109->tag != 26)goto _LL10F;}_LL10E:({void*_tmp127=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp128="Error: unexpected Tuple_e\n";_tag_dyneither(_tmp128,sizeof(
char),27);}),_tag_dyneither(_tmp127,sizeof(void*),0));});exit(1);return;_LL10F: {
struct Cyc_Absyn_CompoundLit_e_struct*_tmp10A=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpBB;if(_tmp10A->tag != 27)goto _LL111;}_LL110:({void*_tmp129=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp12A="Error: unexpected CompoundLit_e\n";_tag_dyneither(_tmp12A,
sizeof(char),33);}),_tag_dyneither(_tmp129,sizeof(void*),0));});exit(1);return;
_LL111: {struct Cyc_Absyn_Array_e_struct*_tmp10B=(struct Cyc_Absyn_Array_e_struct*)
_tmpBB;if(_tmp10B->tag != 28)goto _LL113;}_LL112:({void*_tmp12B=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp12C="Error: unexpected Array_e\n";_tag_dyneither(_tmp12C,sizeof(
char),27);}),_tag_dyneither(_tmp12B,sizeof(void*),0));});exit(1);return;_LL113: {
struct Cyc_Absyn_Comprehension_e_struct*_tmp10C=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBB;if(_tmp10C->tag != 29)goto _LL115;}_LL114:({void*_tmp12D=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp12E="Error: unexpected Comprehension_e\n";_tag_dyneither(_tmp12E,
sizeof(char),35);}),_tag_dyneither(_tmp12D,sizeof(void*),0));});exit(1);return;
_LL115: {struct Cyc_Absyn_Aggregate_e_struct*_tmp10D=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpBB;if(_tmp10D->tag != 30)goto _LL117;}_LL116:({void*_tmp12F=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp130="Error: unexpected Aggregate_e\n";_tag_dyneither(_tmp130,
sizeof(char),31);}),_tag_dyneither(_tmp12F,sizeof(void*),0));});exit(1);return;
_LL117: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp10E=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpBB;if(_tmp10E->tag != 31)goto _LL119;}_LL118:({void*_tmp131=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp132="Error: unexpected AnonStruct_e\n";_tag_dyneither(_tmp132,
sizeof(char),32);}),_tag_dyneither(_tmp131,sizeof(void*),0));});exit(1);return;
_LL119: {struct Cyc_Absyn_Datatype_e_struct*_tmp10F=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpBB;if(_tmp10F->tag != 32)goto _LL11B;}_LL11A:({void*_tmp133=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp134="Error: unexpected Datatype_e\n";_tag_dyneither(_tmp134,
sizeof(char),30);}),_tag_dyneither(_tmp133,sizeof(void*),0));});exit(1);return;
_LL11B: {struct Cyc_Absyn_Enum_e_struct*_tmp110=(struct Cyc_Absyn_Enum_e_struct*)
_tmpBB;if(_tmp110->tag != 33)goto _LL11D;}_LL11C:({void*_tmp135=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp136="Error: unexpected Enum_e\n";_tag_dyneither(_tmp136,sizeof(
char),26);}),_tag_dyneither(_tmp135,sizeof(void*),0));});exit(1);return;_LL11D: {
struct Cyc_Absyn_AnonEnum_e_struct*_tmp111=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpBB;if(_tmp111->tag != 34)goto _LLCC;}_LL11E:({void*_tmp137=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp138="Error: unexpected AnonEnum_e\n";_tag_dyneither(_tmp138,
sizeof(char),30);}),_tag_dyneither(_tmp137,sizeof(void*),0));});exit(1);return;
_LLCC:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){if((unsigned int)eo)Cyc_scan_exp(
eo);return;}void Cyc_scan_type(void*t){void*_tmp139=t;struct Cyc_Absyn_PtrInfo
_tmp13F;struct Cyc_Absyn_ArrayInfo _tmp141;void*_tmp142;struct Cyc_Absyn_Exp*
_tmp143;union Cyc_Absyn_Constraint*_tmp144;struct Cyc_Absyn_FnInfo _tmp146;struct
Cyc_List_List*_tmp148;struct Cyc_Absyn_AggrInfo _tmp14B;union Cyc_Absyn_AggrInfoU
_tmp14C;struct _tuple0*_tmp14E;struct _tuple0 _tmp14F;struct _dyneither_ptr*_tmp150;
struct _tuple0*_tmp152;struct _tuple0 _tmp153;struct _dyneither_ptr*_tmp154;_LL125: {
struct Cyc_Absyn_VoidType_struct*_tmp13A=(struct Cyc_Absyn_VoidType_struct*)
_tmp139;if(_tmp13A->tag != 0)goto _LL127;}_LL126: goto _LL128;_LL127: {struct Cyc_Absyn_IntType_struct*
_tmp13B=(struct Cyc_Absyn_IntType_struct*)_tmp139;if(_tmp13B->tag != 6)goto _LL129;}
_LL128: goto _LL12A;_LL129: {struct Cyc_Absyn_FloatType_struct*_tmp13C=(struct Cyc_Absyn_FloatType_struct*)
_tmp139;if(_tmp13C->tag != 7)goto _LL12B;}_LL12A: goto _LL12C;_LL12B: {struct Cyc_Absyn_DoubleType_struct*
_tmp13D=(struct Cyc_Absyn_DoubleType_struct*)_tmp139;if(_tmp13D->tag != 8)goto
_LL12D;}_LL12C: return;_LL12D: {struct Cyc_Absyn_PointerType_struct*_tmp13E=(
struct Cyc_Absyn_PointerType_struct*)_tmp139;if(_tmp13E->tag != 5)goto _LL12F;else{
_tmp13F=_tmp13E->f1;}}_LL12E: Cyc_scan_type(_tmp13F.elt_typ);return;_LL12F: {
struct Cyc_Absyn_ArrayType_struct*_tmp140=(struct Cyc_Absyn_ArrayType_struct*)
_tmp139;if(_tmp140->tag != 9)goto _LL131;else{_tmp141=_tmp140->f1;_tmp142=_tmp141.elt_type;
_tmp143=_tmp141.num_elts;_tmp144=_tmp141.zero_term;}}_LL130: Cyc_scan_type(
_tmp142);Cyc_scan_exp_opt(_tmp143);return;_LL131: {struct Cyc_Absyn_FnType_struct*
_tmp145=(struct Cyc_Absyn_FnType_struct*)_tmp139;if(_tmp145->tag != 10)goto _LL133;
else{_tmp146=_tmp145->f1;}}_LL132: Cyc_scan_type(_tmp146.ret_typ);{struct Cyc_List_List*
_tmp163=_tmp146.args;for(0;_tmp163 != 0;_tmp163=_tmp163->tl){struct _tuple7 _tmp165;
void*_tmp166;struct _tuple7*_tmp164=(struct _tuple7*)_tmp163->hd;_tmp165=*_tmp164;
_tmp166=_tmp165.f3;Cyc_scan_type(_tmp166);}}if(_tmp146.cyc_varargs != 0)Cyc_scan_type((
_tmp146.cyc_varargs)->type);return;_LL133: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp147=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp139;if(_tmp147->tag != 13)goto
_LL135;else{_tmp148=_tmp147->f2;}}_LL134: for(0;_tmp148 != 0;_tmp148=_tmp148->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp148->hd)->type);Cyc_scan_exp_opt(((
struct Cyc_Absyn_Aggrfield*)_tmp148->hd)->width);}return;_LL135: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp149=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp139;if(_tmp149->tag != 15)goto
_LL137;}_LL136: return;_LL137: {struct Cyc_Absyn_AggrType_struct*_tmp14A=(struct
Cyc_Absyn_AggrType_struct*)_tmp139;if(_tmp14A->tag != 12)goto _LL139;else{_tmp14B=
_tmp14A->f1;_tmp14C=_tmp14B.aggr_info;}}_LL138: {struct _tuple0*_tmp168;struct
_tuple0 _tmp169;struct _dyneither_ptr*_tmp16A;struct _tuple9 _tmp167=Cyc_Absyn_aggr_kinded_name(
_tmp14C);_tmp168=_tmp167.f2;_tmp169=*_tmp168;_tmp16A=_tmp169.f2;_tmp150=_tmp16A;
goto _LL13A;}_LL139: {struct Cyc_Absyn_EnumType_struct*_tmp14D=(struct Cyc_Absyn_EnumType_struct*)
_tmp139;if(_tmp14D->tag != 14)goto _LL13B;else{_tmp14E=_tmp14D->f1;_tmp14F=*
_tmp14E;_tmp150=_tmp14F.f2;}}_LL13A: _tmp154=_tmp150;goto _LL13C;_LL13B: {struct
Cyc_Absyn_TypedefType_struct*_tmp151=(struct Cyc_Absyn_TypedefType_struct*)
_tmp139;if(_tmp151->tag != 18)goto _LL13D;else{_tmp152=_tmp151->f1;_tmp153=*
_tmp152;_tmp154=_tmp153.f2;}}_LL13C: Cyc_add_target(_tmp154);return;_LL13D: {
struct Cyc_Absyn_Evar_struct*_tmp155=(struct Cyc_Absyn_Evar_struct*)_tmp139;if(
_tmp155->tag != 1)goto _LL13F;}_LL13E:({void*_tmp16B=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp16C="Error: unexpected Evar\n";_tag_dyneither(_tmp16C,sizeof(char),
24);}),_tag_dyneither(_tmp16B,sizeof(void*),0));});exit(1);return;_LL13F: {
struct Cyc_Absyn_VarType_struct*_tmp156=(struct Cyc_Absyn_VarType_struct*)_tmp139;
if(_tmp156->tag != 2)goto _LL141;}_LL140:({void*_tmp16D=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp16E="Error: unexpected VarType\n";_tag_dyneither(_tmp16E,sizeof(
char),27);}),_tag_dyneither(_tmp16D,sizeof(void*),0));});exit(1);return;_LL141: {
struct Cyc_Absyn_DatatypeType_struct*_tmp157=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp139;if(_tmp157->tag != 3)goto _LL143;}_LL142:({void*_tmp16F=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp170="Error: unexpected DatatypeType\n";_tag_dyneither(_tmp170,
sizeof(char),32);}),_tag_dyneither(_tmp16F,sizeof(void*),0));});exit(1);return;
_LL143: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp158=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp139;if(_tmp158->tag != 4)goto _LL145;}_LL144:({void*_tmp171=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp172="Error: unexpected DatatypeFieldType\n";_tag_dyneither(_tmp172,
sizeof(char),37);}),_tag_dyneither(_tmp171,sizeof(void*),0));});exit(1);return;
_LL145: {struct Cyc_Absyn_TupleType_struct*_tmp159=(struct Cyc_Absyn_TupleType_struct*)
_tmp139;if(_tmp159->tag != 11)goto _LL147;}_LL146:({void*_tmp173=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp174="Error: unexpected TupleType\n";_tag_dyneither(_tmp174,sizeof(
char),29);}),_tag_dyneither(_tmp173,sizeof(void*),0));});exit(1);return;_LL147: {
struct Cyc_Absyn_RgnHandleType_struct*_tmp15A=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp139;if(_tmp15A->tag != 16)goto _LL149;}_LL148:({void*_tmp175=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp176="Error: unexpected RgnHandleType\n";_tag_dyneither(_tmp176,
sizeof(char),33);}),_tag_dyneither(_tmp175,sizeof(void*),0));});exit(1);return;
_LL149: {struct Cyc_Absyn_DynRgnType_struct*_tmp15B=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp139;if(_tmp15B->tag != 17)goto _LL14B;}_LL14A:({void*_tmp177=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp178="Error: unexpected DynRgnType\n";_tag_dyneither(_tmp178,
sizeof(char),30);}),_tag_dyneither(_tmp177,sizeof(void*),0));});exit(1);return;
_LL14B: {struct Cyc_Absyn_HeapRgn_struct*_tmp15C=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp139;if(_tmp15C->tag != 21)goto _LL14D;}_LL14C:({void*_tmp179=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp17A="Error: unexpected HeapRgn\n";_tag_dyneither(_tmp17A,sizeof(
char),27);}),_tag_dyneither(_tmp179,sizeof(void*),0));});exit(1);return;_LL14D: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp15D=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp139;if(_tmp15D->tag != 22)goto _LL14F;}_LL14E:({void*_tmp17B=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp17C="Error: unexpected UniqueRgn\n";_tag_dyneither(_tmp17C,sizeof(
char),29);}),_tag_dyneither(_tmp17B,sizeof(void*),0));});exit(1);return;_LL14F: {
struct Cyc_Absyn_AccessEff_struct*_tmp15E=(struct Cyc_Absyn_AccessEff_struct*)
_tmp139;if(_tmp15E->tag != 23)goto _LL151;}_LL150:({void*_tmp17D=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp17E="Error: unexpected AccessEff\n";_tag_dyneither(_tmp17E,sizeof(
char),29);}),_tag_dyneither(_tmp17D,sizeof(void*),0));});exit(1);return;_LL151: {
struct Cyc_Absyn_JoinEff_struct*_tmp15F=(struct Cyc_Absyn_JoinEff_struct*)_tmp139;
if(_tmp15F->tag != 24)goto _LL153;}_LL152:({void*_tmp17F=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp180="Error: unexpected JoinEff\n";_tag_dyneither(_tmp180,sizeof(
char),27);}),_tag_dyneither(_tmp17F,sizeof(void*),0));});exit(1);return;_LL153: {
struct Cyc_Absyn_RgnsEff_struct*_tmp160=(struct Cyc_Absyn_RgnsEff_struct*)_tmp139;
if(_tmp160->tag != 25)goto _LL155;}_LL154:({void*_tmp181=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp182="Error: unexpected RgnsEff\n";_tag_dyneither(_tmp182,sizeof(
char),27);}),_tag_dyneither(_tmp181,sizeof(void*),0));});exit(1);return;_LL155: {
struct Cyc_Absyn_TagType_struct*_tmp161=(struct Cyc_Absyn_TagType_struct*)_tmp139;
if(_tmp161->tag != 20)goto _LL157;}_LL156:({void*_tmp183=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp184="Error: unexpected tag_t\n";_tag_dyneither(_tmp184,sizeof(char),
25);}),_tag_dyneither(_tmp183,sizeof(void*),0));});exit(1);return;_LL157: {
struct Cyc_Absyn_ValueofType_struct*_tmp162=(struct Cyc_Absyn_ValueofType_struct*)
_tmp139;if(_tmp162->tag != 19)goto _LL124;}_LL158:({void*_tmp185=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp186="Error: unexpected valueof_t\n";_tag_dyneither(_tmp186,sizeof(
char),29);}),_tag_dyneither(_tmp185,sizeof(void*),0));});exit(1);return;_LL124:;}
struct _tuple21{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
_tuple16*Cyc_scan_decl(struct Cyc_Absyn_Decl*d){Cyc_current_targets=({struct Cyc_Set_Set**
_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
_tmp187;});{void*_tmp188=d->r;struct Cyc_Absyn_Vardecl*_tmp18A;struct Cyc_Absyn_Fndecl*
_tmp18C;struct Cyc_Absyn_Aggrdecl*_tmp18E;struct Cyc_Absyn_Enumdecl*_tmp190;struct
Cyc_Absyn_Typedefdecl*_tmp192;_LL15A: {struct Cyc_Absyn_Var_d_struct*_tmp189=(
struct Cyc_Absyn_Var_d_struct*)_tmp188;if(_tmp189->tag != 0)goto _LL15C;else{
_tmp18A=_tmp189->f1;}}_LL15B: {struct _tuple0 _tmp19F;struct _dyneither_ptr*_tmp1A0;
struct _tuple0*_tmp19E=_tmp18A->name;_tmp19F=*_tmp19E;_tmp1A0=_tmp19F.f2;Cyc_current_source=(
struct _dyneither_ptr*)_tmp1A0;Cyc_scan_type(_tmp18A->type);Cyc_scan_exp_opt(
_tmp18A->initializer);goto _LL159;}_LL15C: {struct Cyc_Absyn_Fn_d_struct*_tmp18B=(
struct Cyc_Absyn_Fn_d_struct*)_tmp188;if(_tmp18B->tag != 1)goto _LL15E;else{_tmp18C=
_tmp18B->f1;}}_LL15D: {struct _tuple0 _tmp1A2;struct _dyneither_ptr*_tmp1A3;struct
_tuple0*_tmp1A1=_tmp18C->name;_tmp1A2=*_tmp1A1;_tmp1A3=_tmp1A2.f2;Cyc_current_source=(
struct _dyneither_ptr*)_tmp1A3;Cyc_scan_type(_tmp18C->ret_type);{struct Cyc_List_List*
_tmp1A4=_tmp18C->args;for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){struct _tuple21
_tmp1A6;void*_tmp1A7;struct _tuple21*_tmp1A5=(struct _tuple21*)_tmp1A4->hd;_tmp1A6=*
_tmp1A5;_tmp1A7=_tmp1A6.f3;Cyc_scan_type(_tmp1A7);}}if(_tmp18C->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp18C->cyc_varargs))->type);
if(_tmp18C->is_inline)({void*_tmp1A8=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp1A9="Warning: ignoring inline function\n";_tag_dyneither(_tmp1A9,sizeof(char),
35);}),_tag_dyneither(_tmp1A8,sizeof(void*),0));});goto _LL159;}_LL15E: {struct
Cyc_Absyn_Aggr_d_struct*_tmp18D=(struct Cyc_Absyn_Aggr_d_struct*)_tmp188;if(
_tmp18D->tag != 6)goto _LL160;else{_tmp18E=_tmp18D->f1;}}_LL15F: {struct _tuple0
_tmp1AB;struct _dyneither_ptr*_tmp1AC;struct _tuple0*_tmp1AA=_tmp18E->name;_tmp1AB=*
_tmp1AA;_tmp1AC=_tmp1AB.f2;Cyc_current_source=(struct _dyneither_ptr*)_tmp1AC;if((
unsigned int)_tmp18E->impl){{struct Cyc_List_List*_tmp1AD=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp18E->impl))->fields;for(0;_tmp1AD != 0;_tmp1AD=_tmp1AD->tl){
struct Cyc_Absyn_Aggrfield*_tmp1AE=(struct Cyc_Absyn_Aggrfield*)_tmp1AD->hd;Cyc_scan_type(
_tmp1AE->type);Cyc_scan_exp_opt(_tmp1AE->width);}}{struct Cyc_List_List*_tmp1AF=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp18E->impl))->fields;for(0;_tmp1AF
!= 0;_tmp1AF=_tmp1AF->tl){;}};}goto _LL159;}_LL160: {struct Cyc_Absyn_Enum_d_struct*
_tmp18F=(struct Cyc_Absyn_Enum_d_struct*)_tmp188;if(_tmp18F->tag != 8)goto _LL162;
else{_tmp190=_tmp18F->f1;}}_LL161: {struct _tuple0 _tmp1B1;struct _dyneither_ptr*
_tmp1B2;struct _tuple0*_tmp1B0=_tmp190->name;_tmp1B1=*_tmp1B0;_tmp1B2=_tmp1B1.f2;
Cyc_current_source=(struct _dyneither_ptr*)_tmp1B2;if((unsigned int)_tmp190->fields){{
struct Cyc_List_List*_tmp1B3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp190->fields))->v;for(0;_tmp1B3 != 0;_tmp1B3=_tmp1B3->tl){struct
Cyc_Absyn_Enumfield*_tmp1B4=(struct Cyc_Absyn_Enumfield*)_tmp1B3->hd;Cyc_scan_exp_opt(
_tmp1B4->tag);}}{struct Cyc_List_List*_tmp1B5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp190->fields))->v;for(0;_tmp1B5 != 0;_tmp1B5=_tmp1B5->tl){;}};}
goto _LL159;}_LL162: {struct Cyc_Absyn_Typedef_d_struct*_tmp191=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp188;if(_tmp191->tag != 9)goto _LL164;else{_tmp192=_tmp191->f1;}}_LL163: {
struct _tuple0 _tmp1B7;struct _dyneither_ptr*_tmp1B8;struct _tuple0*_tmp1B6=_tmp192->name;
_tmp1B7=*_tmp1B6;_tmp1B8=_tmp1B7.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp1B8;if((unsigned int)_tmp192->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp192->defn))->v);goto _LL159;}_LL164: {struct Cyc_Absyn_Region_d_struct*
_tmp193=(struct Cyc_Absyn_Region_d_struct*)_tmp188;if(_tmp193->tag != 4)goto _LL166;}
_LL165:({void*_tmp1B9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1BA="Error: unexpected region declaration";
_tag_dyneither(_tmp1BA,sizeof(char),37);}),_tag_dyneither(_tmp1B9,sizeof(void*),
0));});exit(1);_LL166: {struct Cyc_Absyn_Alias_d_struct*_tmp194=(struct Cyc_Absyn_Alias_d_struct*)
_tmp188;if(_tmp194->tag != 5)goto _LL168;}_LL167:({void*_tmp1BB=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp1BC="Error: unexpected alias declaration";_tag_dyneither(_tmp1BC,
sizeof(char),36);}),_tag_dyneither(_tmp1BB,sizeof(void*),0));});exit(1);_LL168: {
struct Cyc_Absyn_Porton_d_struct*_tmp195=(struct Cyc_Absyn_Porton_d_struct*)
_tmp188;if(_tmp195->tag != 14)goto _LL16A;}_LL169:({void*_tmp1BD=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp1BE="Error: unexpected __cyclone_port_on__";_tag_dyneither(_tmp1BE,
sizeof(char),38);}),_tag_dyneither(_tmp1BD,sizeof(void*),0));});exit(1);return 0;
_LL16A: {struct Cyc_Absyn_Portoff_d_struct*_tmp196=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp188;if(_tmp196->tag != 15)goto _LL16C;}_LL16B:({void*_tmp1BF=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp1C0="Error: unexpected __cyclone_port_off__";_tag_dyneither(
_tmp1C0,sizeof(char),39);}),_tag_dyneither(_tmp1BF,sizeof(void*),0));});exit(1);
return 0;_LL16C: {struct Cyc_Absyn_Let_d_struct*_tmp197=(struct Cyc_Absyn_Let_d_struct*)
_tmp188;if(_tmp197->tag != 2)goto _LL16E;}_LL16D:({void*_tmp1C1=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp1C2="Error: unexpected let declaration\n";_tag_dyneither(_tmp1C2,
sizeof(char),35);}),_tag_dyneither(_tmp1C1,sizeof(void*),0));});exit(1);return 0;
_LL16E: {struct Cyc_Absyn_Datatype_d_struct*_tmp198=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp188;if(_tmp198->tag != 7)goto _LL170;}_LL16F:({void*_tmp1C3=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp1C4="Error: unexpected datatype declaration\n";_tag_dyneither(
_tmp1C4,sizeof(char),40);}),_tag_dyneither(_tmp1C3,sizeof(void*),0));});exit(1);
return 0;_LL170: {struct Cyc_Absyn_Letv_d_struct*_tmp199=(struct Cyc_Absyn_Letv_d_struct*)
_tmp188;if(_tmp199->tag != 3)goto _LL172;}_LL171:({void*_tmp1C5=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp1C6="Error: unexpected let declaration\n";_tag_dyneither(_tmp1C6,
sizeof(char),35);}),_tag_dyneither(_tmp1C5,sizeof(void*),0));});exit(1);return 0;
_LL172: {struct Cyc_Absyn_Namespace_d_struct*_tmp19A=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp188;if(_tmp19A->tag != 10)goto _LL174;}_LL173:({void*_tmp1C7=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp1C8="Error: unexpected namespace declaration\n";_tag_dyneither(
_tmp1C8,sizeof(char),41);}),_tag_dyneither(_tmp1C7,sizeof(void*),0));});exit(1);
return 0;_LL174: {struct Cyc_Absyn_Using_d_struct*_tmp19B=(struct Cyc_Absyn_Using_d_struct*)
_tmp188;if(_tmp19B->tag != 11)goto _LL176;}_LL175:({void*_tmp1C9=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp1CA="Error: unexpected using declaration\n";_tag_dyneither(_tmp1CA,
sizeof(char),37);}),_tag_dyneither(_tmp1C9,sizeof(void*),0));});exit(1);return 0;
_LL176: {struct Cyc_Absyn_ExternC_d_struct*_tmp19C=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp188;if(_tmp19C->tag != 12)goto _LL178;}_LL177:({void*_tmp1CB=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp1CC="Error: unexpected extern \"C\" declaration\n";_tag_dyneither(
_tmp1CC,sizeof(char),42);}),_tag_dyneither(_tmp1CB,sizeof(void*),0));});exit(1);
return 0;_LL178: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp19D=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp188;if(_tmp19D->tag != 13)goto _LL159;}_LL179:({void*_tmp1CD=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp1CE="Error: unexpected extern \"C include\" declaration\n";
_tag_dyneither(_tmp1CE,sizeof(char),50);}),_tag_dyneither(_tmp1CD,sizeof(void*),
0));});exit(1);return 0;_LL159:;}return({struct _tuple16*_tmp1CF=_cycalloc(sizeof(*
_tmp1CF));_tmp1CF->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
_tmp1CF->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp1CF;});}
struct Cyc_Hashtable_Table*Cyc_new_deps(){return((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct
_dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp1D0;_push_handler(& _tmp1D0);{int _tmp1D2=0;if(setjmp(
_tmp1D0.handler))_tmp1D2=1;if(!_tmp1D2){{struct Cyc_Set_Set*_tmp1D3=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);
_npop_handler(0);return _tmp1D3;};_pop_handler();}else{void*_tmp1D1=(void*)
_exn_thrown;void*_tmp1D5=_tmp1D1;_LL17B: {struct Cyc_Core_Not_found_struct*
_tmp1D6=(struct Cyc_Core_Not_found_struct*)_tmp1D5;if(_tmp1D6->tag != Cyc_Core_Not_found)
goto _LL17D;}_LL17C: return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_LL17D:;_LL17E:(void)
_throw(_tmp1D5);_LL17A:;}};}struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*
init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){curr=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(
curr,(struct _dyneither_ptr*)init->hd);}{struct Cyc_Set_Set*_tmp1D7=curr;struct
_dyneither_ptr*_tmp1D8=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1);while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1D7)
> 0){struct Cyc_Set_Set*_tmp1D9=emptyset;struct Cyc_Iter_Iter _tmp1DA=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,
_tmp1D7);while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(
_tmp1DA,& _tmp1D8)){_tmp1D9=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp1D9,Cyc_find(t,_tmp1D8));}_tmp1D7=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(
_tmp1D9,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(curr,_tmp1D7);}return curr;};}enum Cyc_buildlib_mode{Cyc_NORMAL
 = 0,Cyc_GATHER  = 1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};static enum Cyc_buildlib_mode
Cyc_mode=Cyc_NORMAL;static int Cyc_gathering(){return Cyc_mode == Cyc_GATHER  || Cyc_mode
== Cyc_GATHERSCRIPT;}static struct Cyc___cycFILE*Cyc_script_file=0;int Cyc_prscript(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_script_file == 0){({void*
_tmp1DC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1DD="Internal error: script file is NULL\n";
_tag_dyneither(_tmp1DD,sizeof(char),37);}),_tag_dyneither(_tmp1DC,sizeof(void*),
0));});exit(1);}return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),
fmt,ap);}int Cyc_force_directory(struct _dyneither_ptr d){if(Cyc_mode == Cyc_GATHERSCRIPT)({
struct Cyc_String_pa_struct _tmp1E1;_tmp1E1.tag=0;_tmp1E1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)d);({struct Cyc_String_pa_struct _tmp1E0;_tmp1E0.tag=0;
_tmp1E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp1DE[2]={&
_tmp1E0,& _tmp1E1};Cyc_prscript(({const char*_tmp1DF="if ! test -e %s; then mkdir %s; fi\n";
_tag_dyneither(_tmp1DF,sizeof(char),36);}),_tag_dyneither(_tmp1DE,sizeof(void*),
2));});});});else{int _tmp1E2=({unsigned int _tmp1E6[0];Cyc_open((const char*)
_check_null(_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp1E6,
sizeof(unsigned int),0));});if(_tmp1E2 == - 1){if(mkdir((const char*)
_untag_dyneither_ptr(d,sizeof(char),1),448)== - 1){({struct Cyc_String_pa_struct
_tmp1E5;_tmp1E5.tag=0;_tmp1E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({
void*_tmp1E3[1]={& _tmp1E5};Cyc_fprintf(Cyc_stderr,({const char*_tmp1E4="Error: could not create directory %s\n";
_tag_dyneither(_tmp1E4,sizeof(char),38);}),_tag_dyneither(_tmp1E3,sizeof(void*),
1));});});return 1;}}else{close(_tmp1E2);}}return 0;}int Cyc_force_directory_prefixes(
struct _dyneither_ptr file){struct _dyneither_ptr _tmp1E7=Cyc_strdup((struct
_dyneither_ptr)file);struct Cyc_List_List*_tmp1E8=0;while(1){_tmp1E7=Cyc_Filename_dirname((
struct _dyneither_ptr)_tmp1E7);if(Cyc_strlen((struct _dyneither_ptr)_tmp1E7)== 0)
break;_tmp1E8=({struct Cyc_List_List*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->hd=({
struct _dyneither_ptr*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=(struct
_dyneither_ptr)_tmp1E7;_tmp1EA;});_tmp1E9->tl=_tmp1E8;_tmp1E9;});}for(0;_tmp1E8
!= 0;_tmp1E8=_tmp1E8->tl){if(Cyc_force_directory(*((struct _dyneither_ptr*)
_tmp1E8->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_struct{
char*tag;struct _dyneither_ptr f1;};struct _tuple22{struct Cyc_List_List*f1;struct
Cyc_List_List*f2;};int Cyc_process_file(const char*filename,struct Cyc_List_List*
start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct
Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;struct Cyc___cycFILE*out_file;int errorcode=0;({struct
Cyc_String_pa_struct _tmp1ED;_tmp1ED.tag=0;_tmp1ED.f1=(struct _dyneither_ptr)({
const char*_tmp1EE=filename;_tag_dyneither(_tmp1EE,sizeof(char),
_get_zero_arr_size_char(_tmp1EE,1));});({void*_tmp1EB[1]={& _tmp1ED};Cyc_fprintf(
Cyc_stderr,({const char*_tmp1EC="********************************* %s...\n";
_tag_dyneither(_tmp1EC,sizeof(char),41);}),_tag_dyneither(_tmp1EB,sizeof(void*),
1));});});if(!Cyc_gathering())({struct Cyc_String_pa_struct _tmp1F1;_tmp1F1.tag=0;
_tmp1F1.f1=(struct _dyneither_ptr)({const char*_tmp1F2=filename;_tag_dyneither(
_tmp1F2,sizeof(char),_get_zero_arr_size_char(_tmp1F2,1));});({void*_tmp1EF[1]={&
_tmp1F1};Cyc_log(({const char*_tmp1F0="\n%s:\n";_tag_dyneither(_tmp1F0,sizeof(
char),6);}),_tag_dyneither(_tmp1EF,sizeof(void*),1));});});{struct _dyneither_ptr
_tmp1F3=Cyc_Filename_basename(({const char*_tmp335=filename;_tag_dyneither(
_tmp335,sizeof(char),_get_zero_arr_size_char(_tmp335,1));}));struct
_dyneither_ptr _tmp1F4=Cyc_Filename_dirname(({const char*_tmp334=filename;
_tag_dyneither(_tmp334,sizeof(char),_get_zero_arr_size_char(_tmp334,1));}));
struct _dyneither_ptr _tmp1F5=Cyc_Filename_chop_extension((struct _dyneither_ptr)
_tmp1F3);const char*_tmp1F6=(const char*)_untag_dyneither_ptr(Cyc_strconcat((
struct _dyneither_ptr)_tmp1F5,({const char*_tmp333=".iA";_tag_dyneither(_tmp333,
sizeof(char),4);})),sizeof(char),1);const char*_tmp1F7=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp1F4,sizeof(char))== 0?({struct Cyc_String_pa_struct
_tmp32F;_tmp32F.tag=0;_tmp32F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp1F5);({void*_tmp32D[1]={& _tmp32F};Cyc_aprintf(({const char*_tmp32E="%s.iB";
_tag_dyneither(_tmp32E,sizeof(char),6);}),_tag_dyneither(_tmp32D,sizeof(void*),1));});}):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp1F4,(struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp332;_tmp332.tag=0;_tmp332.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1F5);({void*_tmp330[1]={& _tmp332};Cyc_aprintf(({const
char*_tmp331="%s.iB";_tag_dyneither(_tmp331,sizeof(char),6);}),_tag_dyneither(
_tmp330,sizeof(void*),1));});})),sizeof(char),1);const char*_tmp1F8=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp1F4,sizeof(char))== 0?({struct Cyc_String_pa_struct
_tmp329;_tmp329.tag=0;_tmp329.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp1F5);({void*_tmp327[1]={& _tmp329};Cyc_aprintf(({const char*_tmp328="%s.iC";
_tag_dyneither(_tmp328,sizeof(char),6);}),_tag_dyneither(_tmp327,sizeof(void*),1));});}):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp1F4,(struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp32C;_tmp32C.tag=0;_tmp32C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1F5);({void*_tmp32A[1]={& _tmp32C};Cyc_aprintf(({const
char*_tmp32B="%s.iC";_tag_dyneither(_tmp32B,sizeof(char),6);}),_tag_dyneither(
_tmp32A,sizeof(void*),1));});})),sizeof(char),1);const char*_tmp1F9=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp1F4,sizeof(char))== 0?({struct Cyc_String_pa_struct
_tmp323;_tmp323.tag=0;_tmp323.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp1F5);({void*_tmp321[1]={& _tmp323};Cyc_aprintf(({const char*_tmp322="%s.iD";
_tag_dyneither(_tmp322,sizeof(char),6);}),_tag_dyneither(_tmp321,sizeof(void*),1));});}):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp1F4,(struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp326;_tmp326.tag=0;_tmp326.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1F5);({void*_tmp324[1]={& _tmp326};Cyc_aprintf(({const
char*_tmp325="%s.iD";_tag_dyneither(_tmp325,sizeof(char),6);}),_tag_dyneither(
_tmp324,sizeof(void*),1));});})),sizeof(char),1);struct _handler_cons _tmp1FA;
_push_handler(& _tmp1FA);{int _tmp1FC=0;if(setjmp(_tmp1FA.handler))_tmp1FC=1;if(!
_tmp1FC){if(Cyc_force_directory_prefixes(({const char*_tmp1FD=filename;
_tag_dyneither(_tmp1FD,sizeof(char),_get_zero_arr_size_char(_tmp1FD,1));}))){int
_tmp1FE=1;_npop_handler(0);return _tmp1FE;}if(Cyc_mode != Cyc_FINISH){Cyc_current_cpp=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
if(Cyc_mode == Cyc_GATHERSCRIPT){({struct Cyc_String_pa_struct _tmp201;_tmp201.tag=
0;_tmp201.f1=(struct _dyneither_ptr)({const char*_tmp202=_tmp1F6;_tag_dyneither(
_tmp202,sizeof(char),_get_zero_arr_size_char(_tmp202,1));});({void*_tmp1FF[1]={&
_tmp201};Cyc_prscript(({const char*_tmp200="cat >%s <<XXX\n";_tag_dyneither(
_tmp200,sizeof(char),15);}),_tag_dyneither(_tmp1FF,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp203=Cyc_current_cpp;for(0;_tmp203 != 0;_tmp203=_tmp203->tl){({
struct Cyc_String_pa_struct _tmp206;_tmp206.tag=0;_tmp206.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp203->hd));({void*_tmp204[1]={&
_tmp206};Cyc_prscript(({const char*_tmp205="%s";_tag_dyneither(_tmp205,sizeof(
char),3);}),_tag_dyneither(_tmp204,sizeof(void*),1));});});}}({struct Cyc_String_pa_struct
_tmp209;_tmp209.tag=0;_tmp209.f1=(struct _dyneither_ptr)({const char*_tmp20A=
filename;_tag_dyneither(_tmp20A,sizeof(char),_get_zero_arr_size_char(_tmp20A,1));});({
void*_tmp207[1]={& _tmp209};Cyc_prscript(({const char*_tmp208="#include <%s>\n";
_tag_dyneither(_tmp208,sizeof(char),15);}),_tag_dyneither(_tmp207,sizeof(void*),
1));});});({void*_tmp20B=0;Cyc_prscript(({const char*_tmp20C="XXX\n";
_tag_dyneither(_tmp20C,sizeof(char),5);}),_tag_dyneither(_tmp20B,sizeof(void*),0));});({
struct Cyc_String_pa_struct _tmp210;_tmp210.tag=0;_tmp210.f1=(struct _dyneither_ptr)({
const char*_tmp212=_tmp1F6;_tag_dyneither(_tmp212,sizeof(char),
_get_zero_arr_size_char(_tmp212,1));});({struct Cyc_String_pa_struct _tmp20F;
_tmp20F.tag=0;_tmp20F.f1=(struct _dyneither_ptr)({const char*_tmp211=_tmp1F7;
_tag_dyneither(_tmp211,sizeof(char),_get_zero_arr_size_char(_tmp211,1));});({
void*_tmp20D[2]={& _tmp20F,& _tmp210};Cyc_prscript(({const char*_tmp20E="$GCC -E -dM -o %s -x c %s && \\\n";
_tag_dyneither(_tmp20E,sizeof(char),32);}),_tag_dyneither(_tmp20D,sizeof(void*),
2));});});});({struct Cyc_String_pa_struct _tmp216;_tmp216.tag=0;_tmp216.f1=(
struct _dyneither_ptr)({const char*_tmp218=_tmp1F6;_tag_dyneither(_tmp218,sizeof(
char),_get_zero_arr_size_char(_tmp218,1));});({struct Cyc_String_pa_struct _tmp215;
_tmp215.tag=0;_tmp215.f1=(struct _dyneither_ptr)({const char*_tmp217=_tmp1F8;
_tag_dyneither(_tmp217,sizeof(char),_get_zero_arr_size_char(_tmp217,1));});({
void*_tmp213[2]={& _tmp215,& _tmp216};Cyc_prscript(({const char*_tmp214="$GCC -E     -o %s -x c %s;\n";
_tag_dyneither(_tmp214,sizeof(char),28);}),_tag_dyneither(_tmp213,sizeof(void*),
2));});});});({struct Cyc_String_pa_struct _tmp21B;_tmp21B.tag=0;_tmp21B.f1=(
struct _dyneither_ptr)({const char*_tmp21C=_tmp1F6;_tag_dyneither(_tmp21C,sizeof(
char),_get_zero_arr_size_char(_tmp21C,1));});({void*_tmp219[1]={& _tmp21B};Cyc_prscript(({
const char*_tmp21A="rm %s\n";_tag_dyneither(_tmp21A,sizeof(char),7);}),
_tag_dyneither(_tmp219,sizeof(void*),1));});});}else{maybe=Cyc_fopen(_tmp1F6,(
const char*)"w");if(!((unsigned int)maybe)){({struct Cyc_String_pa_struct _tmp21F;
_tmp21F.tag=0;_tmp21F.f1=(struct _dyneither_ptr)({const char*_tmp220=_tmp1F6;
_tag_dyneither(_tmp220,sizeof(char),_get_zero_arr_size_char(_tmp220,1));});({
void*_tmp21D[1]={& _tmp21F};Cyc_fprintf(Cyc_stderr,({const char*_tmp21E="Error: could not create file %s\n";
_tag_dyneither(_tmp21E,sizeof(char),33);}),_tag_dyneither(_tmp21D,sizeof(void*),
1));});});{int _tmp221=1;_npop_handler(0);return _tmp221;};}out_file=(struct Cyc___cycFILE*)
maybe;{struct Cyc_List_List*_tmp222=Cyc_current_cpp;for(0;_tmp222 != 0;_tmp222=
_tmp222->tl){Cyc_fputs((const char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)
_tmp222->hd),sizeof(char),1),out_file);}}({struct Cyc_String_pa_struct _tmp225;
_tmp225.tag=0;_tmp225.f1=(struct _dyneither_ptr)({const char*_tmp226=filename;
_tag_dyneither(_tmp226,sizeof(char),_get_zero_arr_size_char(_tmp226,1));});({
void*_tmp223[1]={& _tmp225};Cyc_fprintf(out_file,({const char*_tmp224="#include <%s>\n";
_tag_dyneither(_tmp224,sizeof(char),15);}),_tag_dyneither(_tmp223,sizeof(void*),
1));});});Cyc_fclose(out_file);{struct _dyneither_ptr _tmp227=Cstring_to_string(
Ccomp);char*cmd=(char*)_untag_dyneither_ptr(({struct Cyc_String_pa_struct _tmp233;
_tmp233.tag=0;_tmp233.f1=(struct _dyneither_ptr)({const char*_tmp235=_tmp1F6;
_tag_dyneither(_tmp235,sizeof(char),_get_zero_arr_size_char(_tmp235,1));});({
struct Cyc_String_pa_struct _tmp232;_tmp232.tag=0;_tmp232.f1=(struct _dyneither_ptr)({
const char*_tmp234=_tmp1F7;_tag_dyneither(_tmp234,sizeof(char),
_get_zero_arr_size_char(_tmp234,1));});({struct Cyc_String_pa_struct _tmp231;
_tmp231.tag=0;_tmp231.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp227);({
void*_tmp22F[3]={& _tmp231,& _tmp232,& _tmp233};Cyc_aprintf(({const char*_tmp230="%s -E -dM -o %s -x c %s";
_tag_dyneither(_tmp230,sizeof(char),24);}),_tag_dyneither(_tmp22F,sizeof(void*),
3));});});});}),sizeof(char),1);if(!system((const char*)cmd)){cmd=(char*)
_untag_dyneither_ptr(({struct Cyc_String_pa_struct _tmp22C;_tmp22C.tag=0;_tmp22C.f1=(
struct _dyneither_ptr)({const char*_tmp22E=_tmp1F6;_tag_dyneither(_tmp22E,sizeof(
char),_get_zero_arr_size_char(_tmp22E,1));});({struct Cyc_String_pa_struct _tmp22B;
_tmp22B.tag=0;_tmp22B.f1=(struct _dyneither_ptr)({const char*_tmp22D=_tmp1F8;
_tag_dyneither(_tmp22D,sizeof(char),_get_zero_arr_size_char(_tmp22D,1));});({
struct Cyc_String_pa_struct _tmp22A;_tmp22A.tag=0;_tmp22A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp227);({void*_tmp228[3]={& _tmp22A,& _tmp22B,& _tmp22C};Cyc_aprintf(({
const char*_tmp229="%s -E -o %s -x c %s";_tag_dyneither(_tmp229,sizeof(char),20);}),
_tag_dyneither(_tmp228,sizeof(void*),3));});});});}),sizeof(char),1);system((
const char*)cmd);}remove(_tmp1F6);};}}if(Cyc_gathering()){int _tmp236=0;
_npop_handler(0);return _tmp236;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp1F7,(const char*)"r");if(!((unsigned int)maybe))(int)_throw((
void*)({struct Cyc_NO_SUPPORT_struct*_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237[
0]=({struct Cyc_NO_SUPPORT_struct _tmp238;_tmp238.tag=Cyc_NO_SUPPORT;_tmp238.f1=(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp23B;_tmp23B.tag=0;_tmp23B.f1=(
struct _dyneither_ptr)({const char*_tmp23C=_tmp1F7;_tag_dyneither(_tmp23C,sizeof(
char),_get_zero_arr_size_char(_tmp23C,1));});({void*_tmp239[1]={& _tmp23B};Cyc_aprintf(({
const char*_tmp23A="can't open macrosfile %s";_tag_dyneither(_tmp23A,sizeof(char),
25);}),_tag_dyneither(_tmp239,sizeof(void*),1));});});_tmp238;});_tmp237;}));
in_file=(struct Cyc___cycFILE*)maybe;{struct Cyc_Lexing_lexbuf*_tmp23D=Cyc_Lexing_from_file(
in_file);struct _tuple16*entry;while((entry=((struct _tuple16*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_line)(_tmp23D))!= 0){struct _tuple16 _tmp23F;struct _dyneither_ptr*
_tmp240;struct Cyc_Set_Set*_tmp241;struct _tuple16*_tmp23E=(struct _tuple16*)
_check_null(entry);_tmp23F=*_tmp23E;_tmp240=_tmp23F.f1;_tmp241=_tmp23F.f2;((void(*)(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(
t,_tmp240,_tmp241);}Cyc_fclose(in_file);maybe=Cyc_fopen(_tmp1F8,(const char*)"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_struct*
_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242[0]=({struct Cyc_NO_SUPPORT_struct
_tmp243;_tmp243.tag=Cyc_NO_SUPPORT;_tmp243.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp246;_tmp246.tag=0;_tmp246.f1=(struct _dyneither_ptr)({const char*_tmp247=
_tmp1F8;_tag_dyneither(_tmp247,sizeof(char),_get_zero_arr_size_char(_tmp247,1));});({
void*_tmp244[1]={& _tmp246};Cyc_aprintf(({const char*_tmp245="can't open declsfile %s";
_tag_dyneither(_tmp245,sizeof(char),24);}),_tag_dyneither(_tmp244,sizeof(void*),
1));});});_tmp243;});_tmp242;}));in_file=(struct Cyc___cycFILE*)maybe;_tmp23D=Cyc_Lexing_from_file(
in_file);Cyc_slurp_out=Cyc_fopen(_tmp1F9,(const char*)"w");if(!((unsigned int)Cyc_slurp_out)){
int _tmp248=1;_npop_handler(0);return _tmp248;}while(((int(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_slurp)(_tmp23D)){;}Cyc_fclose(in_file);Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != Cyc_FINISH)remove(_tmp1F8);maybe=Cyc_fopen(
_tmp1F9,(const char*)"r");if(!((unsigned int)maybe)){int _tmp249=1;_npop_handler(0);
return _tmp249;}in_file=(struct Cyc___cycFILE*)maybe;Cyc_Position_reset_position(({
const char*_tmp24A=_tmp1F9;_tag_dyneither(_tmp24A,sizeof(char),
_get_zero_arr_size_char(_tmp24A,1));}));Cyc_Lex_lex_init(0);{struct Cyc_List_List*
_tmp24B=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init(0);Cyc_fclose(in_file);
remove(_tmp1F9);{struct Cyc_List_List*_tmp24C=_tmp24B;for(0;_tmp24C != 0;_tmp24C=
_tmp24C->tl){struct _tuple16*_tmp24D=Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp24C->hd);
if(_tmp24D == 0)continue;{struct _tuple16 _tmp24F;struct _dyneither_ptr*_tmp250;
struct Cyc_Set_Set*_tmp251;struct _tuple16*_tmp24E=(struct _tuple16*)_tmp24D;
_tmp24F=*_tmp24E;_tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;{struct Cyc_Set_Set*old;{
struct _handler_cons _tmp252;_push_handler(& _tmp252);{int _tmp254=0;if(setjmp(
_tmp252.handler))_tmp254=1;if(!_tmp254){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*
t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,_tmp250);;_pop_handler();}
else{void*_tmp253=(void*)_exn_thrown;void*_tmp256=_tmp253;_LL180: {struct Cyc_Core_Not_found_struct*
_tmp257=(struct Cyc_Core_Not_found_struct*)_tmp256;if(_tmp257->tag != Cyc_Core_Not_found)
goto _LL182;}_LL181: old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL17F;_LL182:;_LL183:(
void)_throw(_tmp256);_LL17F:;}};}((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp250,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp251,old));};};}}{struct Cyc_Set_Set*_tmp258=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp259=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);{
struct Cyc_List_List*_tmp25A=_tmp24B;for(0;_tmp25A != 0;_tmp25A=_tmp25A->tl){
struct Cyc_Absyn_Decl*_tmp25B=(struct Cyc_Absyn_Decl*)_tmp25A->hd;struct
_dyneither_ptr*name;{void*_tmp25C=_tmp25B->r;struct Cyc_Absyn_Vardecl*_tmp25E;
struct Cyc_Absyn_Fndecl*_tmp260;struct Cyc_Absyn_Aggrdecl*_tmp262;struct Cyc_Absyn_Enumdecl*
_tmp264;struct Cyc_Absyn_Typedefdecl*_tmp266;_LL185: {struct Cyc_Absyn_Var_d_struct*
_tmp25D=(struct Cyc_Absyn_Var_d_struct*)_tmp25C;if(_tmp25D->tag != 0)goto _LL187;
else{_tmp25E=_tmp25D->f1;}}_LL186: {struct _tuple0 _tmp273;struct _dyneither_ptr*
_tmp274;struct _tuple0*_tmp272=_tmp25E->name;_tmp273=*_tmp272;_tmp274=_tmp273.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp274);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp274))name=0;else{
name=(struct _dyneither_ptr*)_tmp274;}goto _LL184;}_LL187: {struct Cyc_Absyn_Fn_d_struct*
_tmp25F=(struct Cyc_Absyn_Fn_d_struct*)_tmp25C;if(_tmp25F->tag != 1)goto _LL189;
else{_tmp260=_tmp25F->f1;}}_LL188: {struct _tuple0 _tmp276;struct _dyneither_ptr*
_tmp277;struct _tuple0*_tmp275=_tmp260->name;_tmp276=*_tmp275;_tmp277=_tmp276.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp277);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp277))name=0;else{
name=(struct _dyneither_ptr*)_tmp277;}goto _LL184;}_LL189: {struct Cyc_Absyn_Aggr_d_struct*
_tmp261=(struct Cyc_Absyn_Aggr_d_struct*)_tmp25C;if(_tmp261->tag != 6)goto _LL18B;
else{_tmp262=_tmp261->f1;}}_LL18A: {struct _tuple0 _tmp279;struct _dyneither_ptr*
_tmp27A;struct _tuple0*_tmp278=_tmp262->name;_tmp279=*_tmp278;_tmp27A=_tmp279.f2;
name=(struct _dyneither_ptr*)_tmp27A;goto _LL184;}_LL18B: {struct Cyc_Absyn_Enum_d_struct*
_tmp263=(struct Cyc_Absyn_Enum_d_struct*)_tmp25C;if(_tmp263->tag != 8)goto _LL18D;
else{_tmp264=_tmp263->f1;}}_LL18C: {struct _tuple0 _tmp27C;struct _dyneither_ptr*
_tmp27D;struct _tuple0*_tmp27B=_tmp264->name;_tmp27C=*_tmp27B;_tmp27D=_tmp27C.f2;
name=(struct _dyneither_ptr*)_tmp27D;if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp258,(struct _dyneither_ptr*)name))
_tmp259=({struct Cyc_List_List*_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->hd=
_tmp25B;_tmp27E->tl=_tmp259;_tmp27E;});else{if((unsigned int)_tmp264->fields){
struct Cyc_List_List*_tmp27F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp264->fields))->v;for(0;_tmp27F != 0;_tmp27F=_tmp27F->tl){struct
Cyc_Absyn_Enumfield*_tmp280=(struct Cyc_Absyn_Enumfield*)_tmp27F->hd;struct
_tuple0 _tmp282;struct _dyneither_ptr*_tmp283;struct _tuple0*_tmp281=_tmp280->name;
_tmp282=*_tmp281;_tmp283=_tmp282.f2;if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(_tmp258,_tmp283)){_tmp259=({struct Cyc_List_List*
_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284->hd=_tmp25B;_tmp284->tl=_tmp259;
_tmp284;});break;}}}}name=0;goto _LL184;}_LL18D: {struct Cyc_Absyn_Typedef_d_struct*
_tmp265=(struct Cyc_Absyn_Typedef_d_struct*)_tmp25C;if(_tmp265->tag != 9)goto
_LL18F;else{_tmp266=_tmp265->f1;}}_LL18E: {struct _tuple0 _tmp286;struct
_dyneither_ptr*_tmp287;struct _tuple0*_tmp285=_tmp266->name;_tmp286=*_tmp285;
_tmp287=_tmp286.f2;name=(struct _dyneither_ptr*)_tmp287;goto _LL184;}_LL18F: {
struct Cyc_Absyn_Porton_d_struct*_tmp267=(struct Cyc_Absyn_Porton_d_struct*)
_tmp25C;if(_tmp267->tag != 14)goto _LL191;}_LL190: goto _LL192;_LL191: {struct Cyc_Absyn_Portoff_d_struct*
_tmp268=(struct Cyc_Absyn_Portoff_d_struct*)_tmp25C;if(_tmp268->tag != 15)goto
_LL193;}_LL192: goto _LL194;_LL193: {struct Cyc_Absyn_Let_d_struct*_tmp269=(struct
Cyc_Absyn_Let_d_struct*)_tmp25C;if(_tmp269->tag != 2)goto _LL195;}_LL194: goto
_LL196;_LL195: {struct Cyc_Absyn_Datatype_d_struct*_tmp26A=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp25C;if(_tmp26A->tag != 7)goto _LL197;}_LL196: goto _LL198;_LL197: {struct Cyc_Absyn_Letv_d_struct*
_tmp26B=(struct Cyc_Absyn_Letv_d_struct*)_tmp25C;if(_tmp26B->tag != 3)goto _LL199;}
_LL198: goto _LL19A;_LL199: {struct Cyc_Absyn_Namespace_d_struct*_tmp26C=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp25C;if(_tmp26C->tag != 10)goto _LL19B;}_LL19A: goto _LL19C;_LL19B: {struct Cyc_Absyn_Using_d_struct*
_tmp26D=(struct Cyc_Absyn_Using_d_struct*)_tmp25C;if(_tmp26D->tag != 11)goto _LL19D;}
_LL19C: goto _LL19E;_LL19D: {struct Cyc_Absyn_ExternC_d_struct*_tmp26E=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp25C;if(_tmp26E->tag != 12)goto _LL19F;}_LL19E: goto _LL1A0;_LL19F: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp26F=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp25C;if(_tmp26F->tag != 13)
goto _LL1A1;}_LL1A0: goto _LL1A2;_LL1A1: {struct Cyc_Absyn_Region_d_struct*_tmp270=(
struct Cyc_Absyn_Region_d_struct*)_tmp25C;if(_tmp270->tag != 4)goto _LL1A3;}_LL1A2:
goto _LL1A4;_LL1A3: {struct Cyc_Absyn_Alias_d_struct*_tmp271=(struct Cyc_Absyn_Alias_d_struct*)
_tmp25C;if(_tmp271->tag != 5)goto _LL184;}_LL1A4: name=0;goto _LL184;_LL184:;}if(
name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp258,(struct _dyneither_ptr*)name))_tmp259=({struct Cyc_List_List*_tmp288=
_cycalloc(sizeof(*_tmp288));_tmp288->hd=_tmp25B;_tmp288->tl=_tmp259;_tmp288;});}}
if(!Cyc_do_setjmp){maybe=Cyc_fopen(filename,(const char*)"w");if(!((unsigned int)
maybe)){int _tmp289=1;_npop_handler(0);return _tmp289;}out_file=(struct Cyc___cycFILE*)
maybe;}else{out_file=Cyc_stdout;}{struct _dyneither_ptr ifdefmacro=({struct Cyc_String_pa_struct
_tmp30A;_tmp30A.tag=0;_tmp30A.f1=(struct _dyneither_ptr)({const char*_tmp30B=
filename;_tag_dyneither(_tmp30B,sizeof(char),_get_zero_arr_size_char(_tmp30B,1));});({
void*_tmp308[1]={& _tmp30A};Cyc_aprintf(({const char*_tmp309="_%s_";_tag_dyneither(
_tmp309,sizeof(char),5);}),_tag_dyneither(_tmp308,sizeof(void*),1));});});{int
_tmp28A=0;for(0;_tmp28A < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp28A){
if(((char*)ifdefmacro.curr)[_tmp28A]== '.'  || ((char*)ifdefmacro.curr)[_tmp28A]
== '/')({struct _dyneither_ptr _tmp28B=_dyneither_ptr_plus(ifdefmacro,sizeof(char),
_tmp28A);char _tmp28C=*((char*)_check_dyneither_subscript(_tmp28B,sizeof(char),0));
char _tmp28D='_';if(_get_dyneither_size(_tmp28B,sizeof(char))== 1  && (_tmp28C == '\000'
 && _tmp28D != '\000'))_throw_arraybounds();*((char*)_tmp28B.curr)=_tmp28D;});
else{if(((char*)ifdefmacro.curr)[_tmp28A]!= '_'  && ((char*)ifdefmacro.curr)[
_tmp28A]!= '/')({struct _dyneither_ptr _tmp28E=_dyneither_ptr_plus(ifdefmacro,
sizeof(char),_tmp28A);char _tmp28F=*((char*)_check_dyneither_subscript(_tmp28E,
sizeof(char),0));char _tmp290=(char)toupper((int)((char*)ifdefmacro.curr)[_tmp28A]);
if(_get_dyneither_size(_tmp28E,sizeof(char))== 1  && (_tmp28F == '\000'  && _tmp290
!= '\000'))_throw_arraybounds();*((char*)_tmp28E.curr)=_tmp290;});}}}({struct Cyc_String_pa_struct
_tmp294;_tmp294.tag=0;_tmp294.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
ifdefmacro);({struct Cyc_String_pa_struct _tmp293;_tmp293.tag=0;_tmp293.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp291[2]={& _tmp293,&
_tmp294};Cyc_fprintf(out_file,({const char*_tmp292="#ifndef %s\n#define %s\n";
_tag_dyneither(_tmp292,sizeof(char),23);}),_tag_dyneither(_tmp291,sizeof(void*),
2));});});});{struct Cyc_List_List*_tmp295=0;struct Cyc_List_List*_tmp296=0;{
struct Cyc_List_List*_tmp297=_tmp259;for(0;_tmp297 != 0;_tmp297=_tmp297->tl){
struct Cyc_Absyn_Decl*_tmp298=(struct Cyc_Absyn_Decl*)_tmp297->hd;int _tmp299=0;
struct _dyneither_ptr*name;{void*_tmp29A=_tmp298->r;struct Cyc_Absyn_Vardecl*
_tmp29C;struct Cyc_Absyn_Fndecl*_tmp29E;struct Cyc_Absyn_Aggrdecl*_tmp2A0;struct
Cyc_Absyn_Enumdecl*_tmp2A2;struct Cyc_Absyn_Typedefdecl*_tmp2A4;_LL1A6: {struct
Cyc_Absyn_Var_d_struct*_tmp29B=(struct Cyc_Absyn_Var_d_struct*)_tmp29A;if(_tmp29B->tag
!= 0)goto _LL1A8;else{_tmp29C=_tmp29B->f1;}}_LL1A7: {struct _tuple0 _tmp2B1;struct
_dyneither_ptr*_tmp2B2;struct _tuple0*_tmp2B0=_tmp29C->name;_tmp2B1=*_tmp2B0;
_tmp2B2=_tmp2B1.f2;name=(struct _dyneither_ptr*)_tmp2B2;goto _LL1A5;}_LL1A8: {
struct Cyc_Absyn_Fn_d_struct*_tmp29D=(struct Cyc_Absyn_Fn_d_struct*)_tmp29A;if(
_tmp29D->tag != 1)goto _LL1AA;else{_tmp29E=_tmp29D->f1;}}_LL1A9: {struct _tuple0
_tmp2B4;struct _dyneither_ptr*_tmp2B5;struct _tuple0*_tmp2B3=_tmp29E->name;_tmp2B4=*
_tmp2B3;_tmp2B5=_tmp2B4.f2;name=(struct _dyneither_ptr*)_tmp2B5;goto _LL1A5;}
_LL1AA: {struct Cyc_Absyn_Aggr_d_struct*_tmp29F=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp29A;if(_tmp29F->tag != 6)goto _LL1AC;else{_tmp2A0=_tmp29F->f1;}}_LL1AB: {
struct _tuple0 _tmp2B7;struct _dyneither_ptr*_tmp2B8;struct _tuple0*_tmp2B6=_tmp2A0->name;
_tmp2B7=*_tmp2B6;_tmp2B8=_tmp2B7.f2;name=(struct _dyneither_ptr*)_tmp2B8;goto
_LL1A5;}_LL1AC: {struct Cyc_Absyn_Enum_d_struct*_tmp2A1=(struct Cyc_Absyn_Enum_d_struct*)
_tmp29A;if(_tmp2A1->tag != 8)goto _LL1AE;else{_tmp2A2=_tmp2A1->f1;}}_LL1AD: {
struct _tuple0 _tmp2BA;struct _dyneither_ptr*_tmp2BB;struct _tuple0*_tmp2B9=_tmp2A2->name;
_tmp2BA=*_tmp2B9;_tmp2BB=_tmp2BA.f2;name=(struct _dyneither_ptr*)_tmp2BB;goto
_LL1A5;}_LL1AE: {struct Cyc_Absyn_Typedef_d_struct*_tmp2A3=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp29A;if(_tmp2A3->tag != 9)goto _LL1B0;else{_tmp2A4=_tmp2A3->f1;}}_LL1AF: {
struct _tuple0 _tmp2BD;struct _dyneither_ptr*_tmp2BE;struct _tuple0*_tmp2BC=_tmp2A4->name;
_tmp2BD=*_tmp2BC;_tmp2BE=_tmp2BD.f2;name=(struct _dyneither_ptr*)_tmp2BE;goto
_LL1A5;}_LL1B0: {struct Cyc_Absyn_Region_d_struct*_tmp2A5=(struct Cyc_Absyn_Region_d_struct*)
_tmp29A;if(_tmp2A5->tag != 4)goto _LL1B2;}_LL1B1: goto _LL1B3;_LL1B2: {struct Cyc_Absyn_Alias_d_struct*
_tmp2A6=(struct Cyc_Absyn_Alias_d_struct*)_tmp29A;if(_tmp2A6->tag != 5)goto _LL1B4;}
_LL1B3: goto _LL1B5;_LL1B4: {struct Cyc_Absyn_Porton_d_struct*_tmp2A7=(struct Cyc_Absyn_Porton_d_struct*)
_tmp29A;if(_tmp2A7->tag != 14)goto _LL1B6;}_LL1B5: goto _LL1B7;_LL1B6: {struct Cyc_Absyn_Portoff_d_struct*
_tmp2A8=(struct Cyc_Absyn_Portoff_d_struct*)_tmp29A;if(_tmp2A8->tag != 15)goto
_LL1B8;}_LL1B7: goto _LL1B9;_LL1B8: {struct Cyc_Absyn_Let_d_struct*_tmp2A9=(struct
Cyc_Absyn_Let_d_struct*)_tmp29A;if(_tmp2A9->tag != 2)goto _LL1BA;}_LL1B9: goto
_LL1BB;_LL1BA: {struct Cyc_Absyn_Datatype_d_struct*_tmp2AA=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp29A;if(_tmp2AA->tag != 7)goto _LL1BC;}_LL1BB: goto _LL1BD;_LL1BC: {struct Cyc_Absyn_Letv_d_struct*
_tmp2AB=(struct Cyc_Absyn_Letv_d_struct*)_tmp29A;if(_tmp2AB->tag != 3)goto _LL1BE;}
_LL1BD: goto _LL1BF;_LL1BE: {struct Cyc_Absyn_Namespace_d_struct*_tmp2AC=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp29A;if(_tmp2AC->tag != 10)goto _LL1C0;}_LL1BF: goto _LL1C1;_LL1C0: {struct Cyc_Absyn_Using_d_struct*
_tmp2AD=(struct Cyc_Absyn_Using_d_struct*)_tmp29A;if(_tmp2AD->tag != 11)goto _LL1C2;}
_LL1C1: goto _LL1C3;_LL1C2: {struct Cyc_Absyn_ExternC_d_struct*_tmp2AE=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp29A;if(_tmp2AE->tag != 12)goto _LL1C4;}_LL1C3: goto _LL1C5;_LL1C4: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp2AF=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp29A;if(_tmp2AF->tag != 13)
goto _LL1A5;}_LL1C5: name=0;goto _LL1A5;_LL1A5:;}if(!((unsigned int)name) && !
_tmp299)continue;_tmp295=({struct Cyc_List_List*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));
_tmp2BF->hd=_tmp298;_tmp2BF->tl=_tmp295;_tmp2BF;});_tmp296=({struct Cyc_List_List*
_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->hd=name;_tmp2C0->tl=_tmp296;_tmp2C0;});}}{
struct _RegionHandle _tmp2C1=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=&
_tmp2C1;_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp2C2=Cyc_Tcenv_tc_init(
tc_rgn);Cyc_Tc_tc(tc_rgn,_tmp2C2,1,_tmp295);}{struct Cyc_List_List*_tmp2C5;struct
Cyc_List_List*_tmp2C6;struct _tuple22 _tmp2C4=({struct _tuple22 _tmp2C3;_tmp2C3.f1=
_tmp295;_tmp2C3.f2=_tmp296;_tmp2C3;});_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;for(
0;_tmp2C5 != 0  && _tmp2C6 != 0;(_tmp2C5=_tmp2C5->tl,_tmp2C6=_tmp2C6->tl)){struct
Cyc_Absyn_Decl*_tmp2C7=(struct Cyc_Absyn_Decl*)_tmp2C5->hd;struct _dyneither_ptr*
_tmp2C8=(struct _dyneither_ptr*)_tmp2C6->hd;int _tmp2C9=0;if(!((unsigned int)
_tmp2C8))_tmp2C9=1;if((unsigned int)_tmp2C8){ifdefmacro=({struct Cyc_String_pa_struct
_tmp2CC;_tmp2CC.tag=0;_tmp2CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp2C8);({void*_tmp2CA[1]={& _tmp2CC};Cyc_aprintf(({const char*_tmp2CB="_%s_def_";
_tag_dyneither(_tmp2CB,sizeof(char),9);}),_tag_dyneither(_tmp2CA,sizeof(void*),1));});});({
struct Cyc_String_pa_struct _tmp2CF;_tmp2CF.tag=0;_tmp2CF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)ifdefmacro);({void*_tmp2CD[1]={& _tmp2CF};Cyc_fprintf(
out_file,({const char*_tmp2CE="#ifndef %s\n";_tag_dyneither(_tmp2CE,sizeof(char),
12);}),_tag_dyneither(_tmp2CD,sizeof(void*),1));});});({struct Cyc_String_pa_struct
_tmp2D2;_tmp2D2.tag=0;_tmp2D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
ifdefmacro);({void*_tmp2D0[1]={& _tmp2D2};Cyc_fprintf(out_file,({const char*
_tmp2D1="#define %s\n";_tag_dyneither(_tmp2D1,sizeof(char),12);}),_tag_dyneither(
_tmp2D0,sizeof(void*),1));});});Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*
_tmp2D3[1];_tmp2D3[0]=_tmp2C7;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp2D3,sizeof(struct Cyc_Absyn_Decl*),1));}),
out_file);({void*_tmp2D4=0;Cyc_fprintf(out_file,({const char*_tmp2D5="#endif\n";
_tag_dyneither(_tmp2D5,sizeof(char),8);}),_tag_dyneither(_tmp2D4,sizeof(void*),0));});}
else{Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp2D6[1];_tmp2D6[0]=
_tmp2C7;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp2D6,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}maybe=
Cyc_fopen(_tmp1F7,(const char*)"r");if(!((unsigned int)maybe))(int)_throw((void*)({
struct Cyc_NO_SUPPORT_struct*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7[0]=({
struct Cyc_NO_SUPPORT_struct _tmp2D8;_tmp2D8.tag=Cyc_NO_SUPPORT;_tmp2D8.f1=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp2DB;_tmp2DB.tag=0;_tmp2DB.f1=(
struct _dyneither_ptr)({const char*_tmp2DC=_tmp1F7;_tag_dyneither(_tmp2DC,sizeof(
char),_get_zero_arr_size_char(_tmp2DC,1));});({void*_tmp2D9[1]={& _tmp2DB};Cyc_aprintf(({
const char*_tmp2DA="can't open macrosfile %s";_tag_dyneither(_tmp2DA,sizeof(char),
25);}),_tag_dyneither(_tmp2D9,sizeof(void*),1));});});_tmp2D8;});_tmp2D7;}));
in_file=(struct Cyc___cycFILE*)maybe;_tmp23D=Cyc_Lexing_from_file(in_file);{
struct _tuple17*entry2;while((entry2=((struct _tuple17*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp23D))!= 0){struct _tuple17 _tmp2DE;struct _dyneither_ptr
_tmp2DF;struct _dyneither_ptr*_tmp2E0;struct _tuple17*_tmp2DD=(struct _tuple17*)
_check_null(entry2);_tmp2DE=*_tmp2DD;_tmp2DF=_tmp2DE.f1;_tmp2E0=_tmp2DE.f2;if(((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp258,
_tmp2E0)){({struct Cyc_String_pa_struct _tmp2E3;_tmp2E3.tag=0;_tmp2E3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp2E0);({void*_tmp2E1[1]={& _tmp2E3};Cyc_fprintf(
out_file,({const char*_tmp2E2="#ifndef %s\n";_tag_dyneither(_tmp2E2,sizeof(char),
12);}),_tag_dyneither(_tmp2E1,sizeof(void*),1));});});({struct Cyc_String_pa_struct
_tmp2E6;_tmp2E6.tag=0;_tmp2E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp2DF);({void*_tmp2E4[1]={& _tmp2E6};Cyc_fprintf(out_file,({const char*_tmp2E5="%s\n";
_tag_dyneither(_tmp2E5,sizeof(char),4);}),_tag_dyneither(_tmp2E4,sizeof(void*),1));});});({
void*_tmp2E7=0;Cyc_fprintf(out_file,({const char*_tmp2E8="#endif\n";
_tag_dyneither(_tmp2E8,sizeof(char),8);}),_tag_dyneither(_tmp2E7,sizeof(void*),0));});}}
Cyc_fclose(in_file);if(Cyc_mode != Cyc_FINISH)remove(_tmp1F7);if(hstubs != 0){
struct Cyc_List_List*_tmp2E9=hstubs;for(0;_tmp2E9 != 0;_tmp2E9=_tmp2E9->tl){struct
_tuple18 _tmp2EB;struct _dyneither_ptr _tmp2EC;struct _dyneither_ptr _tmp2ED;struct
_tuple18*_tmp2EA=(struct _tuple18*)_tmp2E9->hd;_tmp2EB=*_tmp2EA;_tmp2EC=_tmp2EB.f1;
_tmp2ED=_tmp2EB.f2;if(_tmp2ED.curr != (_tag_dyneither(0,0,0)).curr  && (_tmp2EC.curr
== (_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*
_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE[0]=_tmp2EC;_tmp2EE;}))))Cyc_fputs((
const char*)_untag_dyneither_ptr(_tmp2ED,sizeof(char),1),out_file);else{({struct
Cyc_String_pa_struct _tmp2F1;_tmp2F1.tag=0;_tmp2F1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp2EC);({void*_tmp2EF[1]={& _tmp2F1};Cyc_log(({const char*
_tmp2F0="%s is not supported on this platform\n";_tag_dyneither(_tmp2F0,sizeof(
char),38);}),_tag_dyneither(_tmp2EF,sizeof(void*),1));});});}}}({void*_tmp2F2=0;
Cyc_fprintf(out_file,({const char*_tmp2F3="#endif\n";_tag_dyneither(_tmp2F3,
sizeof(char),8);}),_tag_dyneither(_tmp2F2,sizeof(void*),0));});if(Cyc_do_setjmp){
int _tmp2F4=0;_npop_handler(1);return _tmp2F4;}else{Cyc_fclose(out_file);}if(
cstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{struct
Cyc_List_List*_tmp2F5=cstubs;for(0;_tmp2F5 != 0;_tmp2F5=_tmp2F5->tl){struct
_tuple18 _tmp2F7;struct _dyneither_ptr _tmp2F8;struct _dyneither_ptr _tmp2F9;struct
_tuple18*_tmp2F6=(struct _tuple18*)_tmp2F5->hd;_tmp2F7=*_tmp2F6;_tmp2F8=_tmp2F7.f1;
_tmp2F9=_tmp2F7.f2;if(_tmp2F9.curr != (_tag_dyneither(0,0,0)).curr  && (_tmp2F8.curr
== (_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*
_tmp2FA=_cycalloc(sizeof(*_tmp2FA));_tmp2FA[0]=_tmp2F8;_tmp2FA;}))))Cyc_fputs((
const char*)_untag_dyneither_ptr(_tmp2F9,sizeof(char),1),out_file);}};}out_file=(
struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);({struct Cyc_String_pa_struct
_tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=(struct _dyneither_ptr)({const char*_tmp2FE=
filename;_tag_dyneither(_tmp2FE,sizeof(char),_get_zero_arr_size_char(_tmp2FE,1));});({
void*_tmp2FB[1]={& _tmp2FD};Cyc_fprintf(out_file,({const char*_tmp2FC="#include <%s>\n\n";
_tag_dyneither(_tmp2FC,sizeof(char),16);}),_tag_dyneither(_tmp2FB,sizeof(void*),
1));});});if(cycstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
struct Cyc_List_List*_tmp2FF=cycstubs;for(0;_tmp2FF != 0;_tmp2FF=_tmp2FF->tl){
struct _tuple18 _tmp301;struct _dyneither_ptr _tmp302;struct _dyneither_ptr _tmp303;
struct _tuple18*_tmp300=(struct _tuple18*)_tmp2FF->hd;_tmp301=*_tmp300;_tmp302=
_tmp301.f1;_tmp303=_tmp301.f2;if(_tmp303.curr != (_tag_dyneither(0,0,0)).curr  && (
_tmp302.curr == (_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*
_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304[0]=_tmp302;_tmp304;}))))Cyc_fputs((
const char*)_untag_dyneither_ptr(_tmp303,sizeof(char),1),out_file);}}({void*
_tmp305=0;Cyc_fprintf(out_file,({const char*_tmp306="\n";_tag_dyneither(_tmp306,
sizeof(char),2);}),_tag_dyneither(_tmp305,sizeof(void*),0));});}{int _tmp307=0;
_npop_handler(1);return _tmp307;};};;_pop_region(tc_rgn);};};};};};};};;
_pop_handler();}else{void*_tmp1FB=(void*)_exn_thrown;void*_tmp30D=_tmp1FB;struct
_dyneither_ptr _tmp30F;_LL1C7: {struct Cyc_NO_SUPPORT_struct*_tmp30E=(struct Cyc_NO_SUPPORT_struct*)
_tmp30D;if(_tmp30E->tag != Cyc_NO_SUPPORT)goto _LL1C9;else{_tmp30F=_tmp30E->f1;}}
_LL1C8:({struct Cyc_String_pa_struct _tmp312;_tmp312.tag=0;_tmp312.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp30F);({void*_tmp310[1]={& _tmp312};Cyc_fprintf(
Cyc_stderr,({const char*_tmp311="No support because %s\n";_tag_dyneither(_tmp311,
sizeof(char),23);}),_tag_dyneither(_tmp310,sizeof(void*),1));});});goto _LL1CA;
_LL1C9:;_LL1CA: maybe=Cyc_fopen(filename,(const char*)"w");if(!((unsigned int)
maybe)){({struct Cyc_String_pa_struct _tmp315;_tmp315.tag=0;_tmp315.f1=(struct
_dyneither_ptr)({const char*_tmp316=filename;_tag_dyneither(_tmp316,sizeof(char),
_get_zero_arr_size_char(_tmp316,1));});({void*_tmp313[1]={& _tmp315};Cyc_fprintf(
Cyc_stderr,({const char*_tmp314="Error: could not create file %s\n";
_tag_dyneither(_tmp314,sizeof(char),33);}),_tag_dyneither(_tmp313,sizeof(void*),
1));});});return 1;}out_file=(struct Cyc___cycFILE*)maybe;({struct Cyc_String_pa_struct
_tmp319;_tmp319.tag=0;_tmp319.f1=(struct _dyneither_ptr)({const char*_tmp31A=
filename;_tag_dyneither(_tmp31A,sizeof(char),_get_zero_arr_size_char(_tmp31A,1));});({
void*_tmp317[1]={& _tmp319};Cyc_fprintf(out_file,({const char*_tmp318="#error -- %s is not supported on this platform\n";
_tag_dyneither(_tmp318,sizeof(char),48);}),_tag_dyneither(_tmp317,sizeof(void*),
1));});});Cyc_fclose(out_file);({struct Cyc_String_pa_struct _tmp31D;_tmp31D.tag=0;
_tmp31D.f1=(struct _dyneither_ptr)({const char*_tmp31E=filename;_tag_dyneither(
_tmp31E,sizeof(char),_get_zero_arr_size_char(_tmp31E,1));});({void*_tmp31B[1]={&
_tmp31D};Cyc_fprintf(Cyc_stderr,({const char*_tmp31C="Warning: %s will not be supported on this platform\n";
_tag_dyneither(_tmp31C,sizeof(char),52);}),_tag_dyneither(_tmp31B,sizeof(void*),
1));});});({void*_tmp31F=0;Cyc_log(({const char*_tmp320="Not supported on this platform\n";
_tag_dyneither(_tmp320,sizeof(char),32);}),_tag_dyneither(_tmp31F,sizeof(void*),
0));});remove(_tmp1F7);remove(_tmp1F8);remove(_tmp1F9);return 0;_LL1CB:;_LL1CC:(
void)_throw(_tmp30D);_LL1C6:;}};};}int Cyc_process_specfile(const char*file,const
char*dir){struct Cyc___cycFILE*_tmp336=Cyc_fopen(file,(const char*)"r");if(!((
unsigned int)_tmp336)){({struct Cyc_String_pa_struct _tmp339;_tmp339.tag=0;_tmp339.f1=(
struct _dyneither_ptr)({const char*_tmp33A=file;_tag_dyneither(_tmp33A,sizeof(char),
_get_zero_arr_size_char(_tmp33A,1));});({void*_tmp337[1]={& _tmp339};Cyc_fprintf(
Cyc_stderr,({const char*_tmp338="Error: could not open %s\n";_tag_dyneither(
_tmp338,sizeof(char),26);}),_tag_dyneither(_tmp337,sizeof(void*),1));});});
return 1;}{struct Cyc___cycFILE*_tmp33B=(struct Cyc___cycFILE*)_tmp336;struct
_dyneither_ptr buf=({char*_tmp354=({unsigned int _tmp351=(unsigned int)1024;char*
_tmp352=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp351 + 1));{
unsigned int _tmp353=_tmp351;unsigned int i;for(i=0;i < _tmp353;i ++){_tmp352[i]='\000';}
_tmp352[_tmp353]=(char)0;}_tmp352;});_tag_dyneither(_tmp354,sizeof(char),
_get_zero_arr_size_char(_tmp354,(unsigned int)1024 + 1));});struct _dyneither_ptr
_tmp33C=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));if(Cyc_mode != Cyc_GATHERSCRIPT){
if(chdir(dir)){({struct Cyc_String_pa_struct _tmp33F;_tmp33F.tag=0;_tmp33F.f1=(
struct _dyneither_ptr)({const char*_tmp340=dir;_tag_dyneither(_tmp340,sizeof(char),
_get_zero_arr_size_char(_tmp340,1));});({void*_tmp33D[1]={& _tmp33F};Cyc_fprintf(
Cyc_stderr,({const char*_tmp33E="Error: can't change directory to %s\n";
_tag_dyneither(_tmp33E,sizeof(char),37);}),_tag_dyneither(_tmp33D,sizeof(void*),
1));});});return 1;}}if(Cyc_mode == Cyc_GATHER){struct _dyneither_ptr _tmp341=
Cstring_to_string(Ccomp);system((const char*)_untag_dyneither_ptr(({struct Cyc_String_pa_struct
_tmp344;_tmp344.tag=0;_tmp344.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp341);({void*_tmp342[1]={& _tmp344};Cyc_aprintf(({const char*_tmp343="echo | %s -E -dM - -o INITMACROS.h\n";
_tag_dyneither(_tmp343,sizeof(char),36);}),_tag_dyneither(_tmp342,sizeof(void*),
1));});}),sizeof(char),1));}{struct Cyc_Lexing_lexbuf*_tmp345=Cyc_Lexing_from_file(
_tmp33B);struct _tuple19*entry;while((entry=((struct _tuple19*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_spec)(_tmp345))!= 0){struct _tuple19 _tmp347;struct _dyneither_ptr
_tmp348;struct Cyc_List_List*_tmp349;struct Cyc_List_List*_tmp34A;struct Cyc_List_List*
_tmp34B;struct Cyc_List_List*_tmp34C;struct Cyc_List_List*_tmp34D;struct _tuple19*
_tmp346=(struct _tuple19*)_check_null(entry);_tmp347=*_tmp346;_tmp348=_tmp347.f1;
_tmp349=_tmp347.f2;_tmp34A=_tmp347.f3;_tmp34B=_tmp347.f4;_tmp34C=_tmp347.f5;
_tmp34D=_tmp347.f6;if(Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp348,
sizeof(char),1),_tmp349,_tmp34A,_tmp34B,_tmp34C,_tmp34D))return 1;}Cyc_fclose(
_tmp33B);if(Cyc_mode != Cyc_GATHERSCRIPT){if(chdir((const char*)((char*)
_untag_dyneither_ptr(_tmp33C,sizeof(char),1)))){({struct Cyc_String_pa_struct
_tmp350;_tmp350.tag=0;_tmp350.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp33C);({void*_tmp34E[1]={& _tmp350};Cyc_fprintf(Cyc_stderr,({const char*_tmp34F="Error: could not change directory to %s\n";
_tag_dyneither(_tmp34F,sizeof(char),41);}),_tag_dyneither(_tmp34E,sizeof(void*),
1));});});return 1;}}return 0;};};}int Cyc_process_setjmp(const char*dir){struct
_dyneither_ptr buf=({char*_tmp366=({unsigned int _tmp363=(unsigned int)1024;char*
_tmp364=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp363 + 1));{
unsigned int _tmp365=_tmp363;unsigned int i;for(i=0;i < _tmp365;i ++){_tmp364[i]='\000';}
_tmp364[_tmp365]=(char)0;}_tmp364;});_tag_dyneither(_tmp366,sizeof(char),
_get_zero_arr_size_char(_tmp366,(unsigned int)1024 + 1));});struct _dyneither_ptr
_tmp355=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));if(chdir(dir)){({
struct Cyc_String_pa_struct _tmp358;_tmp358.tag=0;_tmp358.f1=(struct _dyneither_ptr)({
const char*_tmp359=dir;_tag_dyneither(_tmp359,sizeof(char),
_get_zero_arr_size_char(_tmp359,1));});({void*_tmp356[1]={& _tmp358};Cyc_fprintf(
Cyc_stderr,({const char*_tmp357="Error: can't change directory to %s\n";
_tag_dyneither(_tmp357,sizeof(char),37);}),_tag_dyneither(_tmp356,sizeof(void*),
1));});});return 1;}if(Cyc_process_file((const char*)"setjmp.h",({struct
_dyneither_ptr*_tmp35A[1];_tmp35A[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"jmp_buf",sizeof(char),8);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp35A,sizeof(struct
_dyneither_ptr*),1));}),0,({struct _tuple18*_tmp35C[1];_tmp35C[0]=({struct
_tuple18*_tmp35D=_cycalloc(sizeof(*_tmp35D));_tmp35D->f1=({const char*_tmp35F="setjmp";
_tag_dyneither(_tmp35F,sizeof(char),7);});_tmp35D->f2=({const char*_tmp35E="extern int setjmp(jmp_buf);\n";
_tag_dyneither(_tmp35E,sizeof(char),29);});_tmp35D;});((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp35C,sizeof(struct
_tuple18*),1));}),0,0))return 1;if(chdir((const char*)((char*)_untag_dyneither_ptr(
_tmp355,sizeof(char),1)))){({struct Cyc_String_pa_struct _tmp362;_tmp362.tag=0;
_tmp362.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp355);({void*_tmp360[
1]={& _tmp362};Cyc_fprintf(Cyc_stderr,({const char*_tmp361="Error: could not change directory to %s\n";
_tag_dyneither(_tmp361,sizeof(char),41);}),_tag_dyneither(_tmp360,sizeof(void*),
1));});});return 1;}return 0;}static char _tmp367[13]="BUILDLIB.OUT";static struct
_dyneither_ptr Cyc_output_dir={_tmp367,_tmp367,_tmp367 + 13};static void Cyc_set_output_dir(
struct _dyneither_ptr s){Cyc_output_dir=s;}static struct Cyc_List_List*Cyc_spec_files=
0;static void Cyc_add_spec_file(struct _dyneither_ptr s){Cyc_spec_files=({struct Cyc_List_List*
_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368->hd=(const char*)_untag_dyneither_ptr(
s,sizeof(char),1);_tmp368->tl=Cyc_spec_files;_tmp368;});}static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}static void Cyc_set_GATHERSCRIPT(){Cyc_mode=Cyc_GATHERSCRIPT;}
static void Cyc_set_FINISH(){Cyc_mode=Cyc_FINISH;}static int Cyc_badparse=0;static
void Cyc_unsupported_option(struct _dyneither_ptr s){({struct Cyc_String_pa_struct
_tmp36B;_tmp36B.tag=0;_tmp36B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({
void*_tmp369[1]={& _tmp36B};Cyc_fprintf(Cyc_stderr,({const char*_tmp36A="Unsupported option %s\n";
_tag_dyneither(_tmp36A,sizeof(char),23);}),_tag_dyneither(_tmp369,sizeof(void*),
1));});});Cyc_badparse=1;}void GC_blacklist_warn_clear();struct _tuple23{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){GC_blacklist_warn_clear();{struct
Cyc_List_List*options=({struct _tuple23*_tmp38C[6];_tmp38C[5]=({struct _tuple23*
_tmp3AB=_cycalloc(sizeof(*_tmp3AB));_tmp3AB->f1=({const char*_tmp3B0="-";
_tag_dyneither(_tmp3B0,sizeof(char),2);});_tmp3AB->f2=1;_tmp3AB->f3=({const char*
_tmp3AF="";_tag_dyneither(_tmp3AF,sizeof(char),1);});_tmp3AB->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD[0]=({struct
Cyc_Arg_Flag_spec_struct _tmp3AE;_tmp3AE.tag=1;_tmp3AE.f1=Cyc_unsupported_option;
_tmp3AE;});_tmp3AD;});_tmp3AB->f5=({const char*_tmp3AC="";_tag_dyneither(_tmp3AC,
sizeof(char),1);});_tmp3AB;});_tmp38C[4]=({struct _tuple23*_tmp3A5=_cycalloc(
sizeof(*_tmp3A5));_tmp3A5->f1=({const char*_tmp3AA="-setjmp";_tag_dyneither(
_tmp3AA,sizeof(char),8);});_tmp3A5->f2=0;_tmp3A5->f3=({const char*_tmp3A9="";
_tag_dyneither(_tmp3A9,sizeof(char),1);});_tmp3A5->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7[0]=({struct Cyc_Arg_Set_spec_struct
_tmp3A8;_tmp3A8.tag=3;_tmp3A8.f1=& Cyc_do_setjmp;_tmp3A8;});_tmp3A7;});_tmp3A5->f5=({
const char*_tmp3A6="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";
_tag_dyneither(_tmp3A6,sizeof(char),186);});_tmp3A5;});_tmp38C[3]=({struct
_tuple23*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->f1=({const char*_tmp3A4="-finish";
_tag_dyneither(_tmp3A4,sizeof(char),8);});_tmp39F->f2=0;_tmp39F->f3=({const char*
_tmp3A3="";_tag_dyneither(_tmp3A3,sizeof(char),1);});_tmp39F->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1[0]=({struct
Cyc_Arg_Unit_spec_struct _tmp3A2;_tmp3A2.tag=0;_tmp3A2.f1=Cyc_set_FINISH;_tmp3A2;});
_tmp3A1;});_tmp39F->f5=({const char*_tmp3A0="Produce Cyclone headers from pre-gathered C library info";
_tag_dyneither(_tmp3A0,sizeof(char),57);});_tmp39F;});_tmp38C[2]=({struct
_tuple23*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399->f1=({const char*_tmp39E="-gatherscript";
_tag_dyneither(_tmp39E,sizeof(char),14);});_tmp399->f2=0;_tmp399->f3=({const char*
_tmp39D="";_tag_dyneither(_tmp39D,sizeof(char),1);});_tmp399->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B[0]=({struct
Cyc_Arg_Unit_spec_struct _tmp39C;_tmp39C.tag=0;_tmp39C.f1=Cyc_set_GATHERSCRIPT;
_tmp39C;});_tmp39B;});_tmp399->f5=({const char*_tmp39A="Produce a script to gather C library info";
_tag_dyneither(_tmp39A,sizeof(char),42);});_tmp399;});_tmp38C[1]=({struct
_tuple23*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->f1=({const char*_tmp398="-gather";
_tag_dyneither(_tmp398,sizeof(char),8);});_tmp393->f2=0;_tmp393->f3=({const char*
_tmp397="";_tag_dyneither(_tmp397,sizeof(char),1);});_tmp393->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395[0]=({struct
Cyc_Arg_Unit_spec_struct _tmp396;_tmp396.tag=0;_tmp396.f1=Cyc_set_GATHER;_tmp396;});
_tmp395;});_tmp393->f5=({const char*_tmp394="Gather C library info but don't produce Cyclone headers";
_tag_dyneither(_tmp394,sizeof(char),56);});_tmp393;});_tmp38C[0]=({struct
_tuple23*_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D->f1=({const char*_tmp392="-d";
_tag_dyneither(_tmp392,sizeof(char),3);});_tmp38D->f2=0;_tmp38D->f3=({const char*
_tmp391=" <file>";_tag_dyneither(_tmp391,sizeof(char),8);});_tmp38D->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({
struct Cyc_Arg_String_spec_struct _tmp390;_tmp390.tag=5;_tmp390.f1=Cyc_set_output_dir;
_tmp390;});_tmp38F;});_tmp38D->f5=({const char*_tmp38E="Set the output directory to <file>";
_tag_dyneither(_tmp38E,sizeof(char),35);});_tmp38D;});((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp38C,sizeof(struct
_tuple23*),6));});Cyc_Arg_parse(options,Cyc_add_spec_file,({const char*_tmp36C="Options:";
_tag_dyneither(_tmp36C,sizeof(char),9);}),argv);if((((Cyc_badparse  || !Cyc_do_setjmp
 && Cyc_spec_files == 0) || Cyc_do_setjmp  && Cyc_spec_files != 0) || Cyc_do_setjmp
 && Cyc_mode == Cyc_GATHER) || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){Cyc_Arg_usage(
options,({const char*_tmp36D="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";
_tag_dyneither(_tmp36D,sizeof(char),59);}));return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){
Cyc_script_file=Cyc_fopen((const char*)"BUILDLIB.sh",(const char*)"w");if(!((
unsigned int)Cyc_script_file)){({void*_tmp36E=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp36F="Could not create file BUILDLIB.sh\n";_tag_dyneither(_tmp36F,
sizeof(char),35);}),_tag_dyneither(_tmp36E,sizeof(void*),0));});exit(1);}({void*
_tmp370=0;Cyc_prscript(({const char*_tmp371="#!/bin/sh\n";_tag_dyneither(_tmp371,
sizeof(char),11);}),_tag_dyneither(_tmp370,sizeof(void*),0));});({void*_tmp372=0;
Cyc_prscript(({const char*_tmp373="GCC=\"gcc\"\n";_tag_dyneither(_tmp373,sizeof(
char),11);}),_tag_dyneither(_tmp372,sizeof(void*),0));});}if(Cyc_force_directory_prefixes(
Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){({struct Cyc_String_pa_struct
_tmp376;_tmp376.tag=0;_tmp376.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir);({void*_tmp374[1]={& _tmp376};Cyc_fprintf(Cyc_stderr,({const char*
_tmp375="Error: could not create directory %s\n";_tag_dyneither(_tmp375,sizeof(
char),38);}),_tag_dyneither(_tmp374,sizeof(void*),1));});});return 1;}if(Cyc_mode
== Cyc_GATHERSCRIPT){({struct Cyc_String_pa_struct _tmp379;_tmp379.tag=0;_tmp379.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp377[1]={&
_tmp379};Cyc_prscript(({const char*_tmp378="cd %s\n";_tag_dyneither(_tmp378,
sizeof(char),7);}),_tag_dyneither(_tmp377,sizeof(void*),1));});});({void*_tmp37A=
0;Cyc_prscript(({const char*_tmp37B="echo | $GCC -E -dM - -o INITMACROS.h\n";
_tag_dyneither(_tmp37B,sizeof(char),38);}),_tag_dyneither(_tmp37A,sizeof(void*),
0));});}if(!Cyc_gathering()){Cyc_log_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp37C="BUILDLIB.LOG";
_tag_dyneither(_tmp37C,sizeof(char),13);})),sizeof(char),1),(const char*)"w");if(
!((unsigned int)Cyc_log_file)){({struct Cyc_String_pa_struct _tmp37F;_tmp37F.tag=0;
_tmp37F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*
_tmp37D[1]={& _tmp37F};Cyc_fprintf(Cyc_stderr,({const char*_tmp37E="Error: could not create log file in directory %s\n";
_tag_dyneither(_tmp37E,sizeof(char),50);}),_tag_dyneither(_tmp37D,sizeof(void*),
1));});});return 1;}if(!Cyc_do_setjmp){Cyc_cstubs_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp380="cstubs.c";
_tag_dyneither(_tmp380,sizeof(char),9);})),sizeof(char),1),(const char*)"w");if(!((
unsigned int)Cyc_cstubs_file)){({struct Cyc_String_pa_struct _tmp383;_tmp383.tag=0;
_tmp383.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*
_tmp381[1]={& _tmp383};Cyc_fprintf(Cyc_stderr,({const char*_tmp382="Error: could not create cstubs.c in directory %s\n";
_tag_dyneither(_tmp382,sizeof(char),50);}),_tag_dyneither(_tmp381,sizeof(void*),
1));});});return 1;}Cyc_cycstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(
Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp384="cycstubs.cyc";
_tag_dyneither(_tmp384,sizeof(char),13);})),sizeof(char),1),(const char*)"w");if(
!((unsigned int)Cyc_cycstubs_file)){({struct Cyc_String_pa_struct _tmp387;_tmp387.tag=
0;_tmp387.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*
_tmp385[1]={& _tmp387};Cyc_fprintf(Cyc_stderr,({const char*_tmp386="Error: could not create cycstubs.c in directory %s\n";
_tag_dyneither(_tmp386,sizeof(char),52);}),_tag_dyneither(_tmp385,sizeof(void*),
1));});});return 1;}({void*_tmp388=0;Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file),({const char*_tmp389="#include <core.h>\nusing Core;\n\n";
_tag_dyneither(_tmp389,sizeof(char),32);}),_tag_dyneither(_tmp388,sizeof(void*),
0));});}}{const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,
sizeof(char),1);if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))return 1;else{for(
0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,
outdir)){({void*_tmp38A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp38B="FATAL ERROR -- QUIT!\n";
_tag_dyneither(_tmp38B,sizeof(char),22);}),_tag_dyneither(_tmp38A,sizeof(void*),
0));});exit(1);}}}if(Cyc_mode == Cyc_GATHERSCRIPT)Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_script_file));else{if(!Cyc_gathering()){Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_log_file));if(!Cyc_do_setjmp){Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_cstubs_file));Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
return 0;};};}
