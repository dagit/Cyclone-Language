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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);int
Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_rlist(
struct _RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*
tag;};typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}
Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);
struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**
ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(
struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(
struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct
Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};struct
Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};int Cyc_Position_error_p();struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
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
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
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
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
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
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
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
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);int Cyc_Absyn_is_nontagged_union_type(void*);
int Cyc_Absyn_is_aggr_type(void*t);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);int Cyc_Tcutil_is_noreturn(void*);struct _tuple10{
unsigned int f1;int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};enum Cyc_CfFlowInfo_InitLevel{
Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;};union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;
union Cyc_CfFlowInfo_RelnOp rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop
cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[11];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};extern struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;
struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
_union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct
_union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
struct _dyneither_ptr f2;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,void*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*,struct Cyc_CfFlowInfo_ConsumeInfo);
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(
void*t,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*
fs,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*
r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,
union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*
p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));void
Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,
int);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,
struct _tuple13 pr2,int);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);struct _tuple14{
struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct
_tuple14 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo
f,int clear);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*
place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct
Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*
encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct
Cyc_NewControlFlow_CFAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser){enclosee->annot=(void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=
_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct
_tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot
_tmp2;_tmp2.encloser=encloser;_tmp2.visited=0;_tmp2;});_tmp1;});_tmp0;});}struct
Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*
fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo
tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*
param_roots;struct Cyc_Hashtable_Table*flow_table;};static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,
struct Cyc_Absyn_Stmt*);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);
struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);static
struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*
s){void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot _tmp5;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=_tmp4->f1;
_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2: return _tmp6;_LL3:;
_LL4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp7=_cycalloc(
sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Core_Impossible_struct _tmp8;_tmp8.tag=Cyc_Core_Impossible;
_tmp8.f1=({const char*_tmp9="ControlFlow -- wrong stmt annotation";_tag_dyneither(
_tmp9,sizeof(char),37);});_tmp8;});_tmp7;}));_LL0:;}static union Cyc_CfFlowInfo_FlowInfo*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*
_tmpA=_region_malloc(env->r,sizeof(*_tmpA));_tmpA[0]=Cyc_CfFlowInfo_BottomFL();
_tmpA;});((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*
val))Cyc_Hashtable_insert)(env->flow_table,s,res);return res;}return*sflow;}
struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*
f2;};static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=
Cyc_NewControlFlow_get_stmt_flow(env,s);*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,
env->all_changed,inflow,*_tmpC,1);_tmpB->visited=env->iteration_num;return({
struct _tuple17 _tmpD;_tmpD.f1=_tmpB;_tmpD.f2=_tmpC;_tmpD;});}static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){if(
env->in_try){env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
new_flow,env->tryflow,1);{struct Cyc_CfFlowInfo_ConsumeInfo _tmpF;struct Cyc_Dict_Dict
_tmp10;struct Cyc_List_List*_tmp11;struct _tuple14 _tmpE=Cyc_CfFlowInfo_save_consume_info(
env->fenv,env->tryflow,0);_tmpF=_tmpE.f1;_tmp10=_tmpF.consumed;_tmp11=_tmpF.may_consume;
env->tryflow=Cyc_CfFlowInfo_restore_consume_info(env->tryflow,({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp12;_tmp12.consumed=_tmp10;_tmp12.may_consume=0;_tmp12;}),0);};}}static void
Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s,union Cyc_CfFlowInfo_FlowInfo flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp13=
Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo*_tmp14=Cyc_NewControlFlow_get_stmt_flow(
env,s);union Cyc_CfFlowInfo_FlowInfo _tmp15=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
flow,*_tmp14,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp15,*_tmp14)){*_tmp14=
_tmp15;if(_tmp13->visited == env->iteration_num)env->iterate_again=1;}}static
union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,
struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo _tmp16=inflow;int
_tmp17;struct _tuple12 _tmp18;struct Cyc_Dict_Dict _tmp19;struct Cyc_List_List*_tmp1A;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B;_LL6: if((_tmp16.BottomFL).tag != 1)goto
_LL8;_tmp17=(int)(_tmp16.BottomFL).val;_LL7: return Cyc_CfFlowInfo_BottomFL();_LL8:
if((_tmp16.ReachableFL).tag != 2)goto _LL5;_tmp18=(struct _tuple12)(_tmp16.ReachableFL).val;
_tmp19=_tmp18.f1;_tmp1A=_tmp18.f2;_tmp1B=_tmp18.f3;_LL9: for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp1C=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp1D=_region_malloc(fenv->r,sizeof(*_tmp1D));_tmp1D[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp1E;});_tmp1D;});
_tmp19=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp19,(void*)_tmp1C,Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,leafval));}return Cyc_CfFlowInfo_ReachableFL(_tmp19,_tmp1A,_tmp1B);
_LL5:;}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static void Cyc_NewControlFlow_remove_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,
union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Dict_Dict*out_unique_places,struct
Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp20;struct Cyc_Dict_Dict
_tmp21;struct _tuple14 _tmp1F=Cyc_CfFlowInfo_save_consume_info(fenv,outflow,0);
_tmp20=_tmp1F.f1;_tmp21=_tmp20.consumed;{struct _RegionHandle*_tmp22=fenv->r;{
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct
Cyc_Dict_Dict d))Cyc_Dict_make_iter)(_tmp22,*((struct Cyc_Dict_Dict*)_check_null(
out_unique_places)));struct _tuple18 _tmp23=*((struct _tuple18*(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(_tmp22,*out_unique_places);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp23)){
struct Cyc_CfFlowInfo_Place*_tmp24=_tmp23.f1;if(!((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp21,_tmp24))({struct Cyc_String_pa_struct
_tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_CfFlowInfo_place_err_string(
_tmp24));({void*_tmp25[1]={& _tmp27};Cyc_Tcutil_terr(_tmp23.f2,({const char*_tmp26="Failed to consume unique variable %s";
_tag_dyneither(_tmp26,sizeof(char),37);}),_tag_dyneither(_tmp25,sizeof(void*),1));});});}}*
out_unique_places=old_unique_places;};}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;int _tmp29;struct
_tuple12 _tmp2A;struct Cyc_Dict_Dict _tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2D;_LLB: if((_tmp28.BottomFL).tag != 1)goto _LLD;_tmp29=(int)(_tmp28.BottomFL).val;
_LLC: return Cyc_CfFlowInfo_BottomFL();_LLD: if((_tmp28.ReachableFL).tag != 2)goto
_LLA;_tmp2A=(struct _tuple12)(_tmp28.ReachableFL).val;_tmp2B=_tmp2A.f1;_tmp2C=
_tmp2A.f2;_tmp2D=_tmp2A.f3;_LLE: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2B,r)
!= Cyc_CfFlowInfo_AllIL)({void*_tmp2E=0;Cyc_Tcutil_terr(loc,({const char*_tmp2F="expression may not be fully initialized";
_tag_dyneither(_tmp2F,sizeof(char),40);}),_tag_dyneither(_tmp2E,sizeof(void*),0));});{
struct Cyc_Dict_Dict _tmp30=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp2B,env->all_changed,
r);if(_tmp2B.t == _tmp30.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp31=Cyc_CfFlowInfo_ReachableFL(
_tmp30,_tmp2C,_tmp2D);Cyc_NewControlFlow_update_tryflow(env,_tmp31);return _tmp31;};};
_LLA:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*
rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,
struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({struct _tuple19
_tmp32;_tmp32.f1=inflow;_tmp32.f2=0;_tmp32;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo
_tmp34;void*_tmp35;struct _tuple13 _tmp33=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp34=_tmp33.f1;_tmp35=_tmp33.f2;return({struct
_tuple19 _tmp36;_tmp36.f1=_tmp34;_tmp36.f2=({struct Cyc_List_List*_tmp37=
_region_malloc(rgn,sizeof(*_tmp37));_tmp37->hd=_tmp35;_tmp37->tl=0;_tmp37;});
_tmp36;});}{struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;struct Cyc_Dict_Dict
this_all_changed;union Cyc_CfFlowInfo_FlowInfo old_inflow;union Cyc_CfFlowInfo_FlowInfo
outflow;struct Cyc_List_List*rvals;struct Cyc_CfFlowInfo_ConsumeInfo _tmp39;union
Cyc_CfFlowInfo_FlowInfo _tmp3A;struct _tuple14 _tmp38=Cyc_CfFlowInfo_save_consume_info(
env->fenv,inflow,1);_tmp39=_tmp38.f1;_tmp3A=_tmp38.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp5C;_tmp5C.consumed=(env->fenv)->mt_place_set;
_tmp5C.may_consume=0;_tmp5C;});int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp3A=Cyc_CfFlowInfo_restore_consume_info(_tmp3A,_tmp39,0);env->all_changed=({
struct Cyc_Dict_Dict*_tmp3B=_region_malloc(env->r,sizeof(*_tmp3B));_tmp3B[0]=(env->fenv)->mt_place_set;
_tmp3B;});{union Cyc_CfFlowInfo_FlowInfo _tmp3D;void*_tmp3E;struct _tuple13 _tmp3C=
Cyc_NewControlFlow_anal_Rexp(env,_tmp3A,(struct Cyc_Absyn_Exp*)es->hd);_tmp3D=
_tmp3C.f1;_tmp3E=_tmp3C.f2;outflow=_tmp3D;rvals=({struct Cyc_List_List*_tmp3F=
_region_malloc(rgn,sizeof(*_tmp3F));_tmp3F->hd=_tmp3E;_tmp3F->tl=0;_tmp3F;});
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)),0);if(arg1_unconsumed){union Cyc_CfFlowInfo_FlowInfo
_tmp40=outflow;int _tmp41;struct _tuple12 _tmp42;struct Cyc_Dict_Dict _tmp43;struct
Cyc_List_List*_tmp44;struct Cyc_CfFlowInfo_ConsumeInfo _tmp45;_LL10: if((_tmp40.BottomFL).tag
!= 1)goto _LL12;_tmp41=(int)(_tmp40.BottomFL).val;_LL11: goto _LLF;_LL12: if((_tmp40.ReachableFL).tag
!= 2)goto _LLF;_tmp42=(struct _tuple12)(_tmp40.ReachableFL).val;_tmp43=_tmp42.f1;
_tmp44=_tmp42.f2;_tmp45=_tmp42.f3;_LL13: outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp43,_tmp44,({struct Cyc_CfFlowInfo_ConsumeInfo _tmp46;_tmp46.consumed=_tmp45.consumed;
_tmp46.may_consume=_tmp39.may_consume;_tmp46;}));_LLF:;}{struct Cyc_List_List*es2=
es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({struct Cyc_Dict_Dict*_tmp47=
_region_malloc(env->r,sizeof(*_tmp47));_tmp47[0]=(env->fenv)->mt_place_set;
_tmp47;});{union Cyc_CfFlowInfo_FlowInfo _tmp49;void*_tmp4A;struct _tuple13 _tmp48=
Cyc_NewControlFlow_anal_Rexp(env,_tmp3A,(struct Cyc_Absyn_Exp*)es2->hd);_tmp49=
_tmp48.f1;_tmp4A=_tmp48.f2;rvals=({struct Cyc_List_List*_tmp4B=_region_malloc(rgn,
sizeof(*_tmp4B));_tmp4B->hd=_tmp4A;_tmp4B->tl=rvals;_tmp4B;});outflow=Cyc_CfFlowInfo_after_flow(
env->fenv,(struct Cyc_Dict_Dict*)& this_all_changed,outflow,_tmp49,
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)),0);};}}{struct Cyc_CfFlowInfo_ConsumeInfo _tmp4D;
struct _tuple14 _tmp4C=Cyc_CfFlowInfo_save_consume_info(env->fenv,outflow,0);
_tmp4D=_tmp4C.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
outflow_consume,_tmp4D)){({void*_tmp4E=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp4F="sanity consumed: ";_tag_dyneither(_tmp4F,sizeof(char),18);}),
_tag_dyneither(_tmp4E,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp50=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp51="\ncurrent consumed: ";_tag_dyneither(_tmp51,sizeof(char),20);}),
_tag_dyneither(_tmp50,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp4D.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp52=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp53="\nsanity may_consume: ";_tag_dyneither(_tmp53,sizeof(char),22);}),
_tag_dyneither(_tmp52,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(outflow_consume.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp54=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp55="\ncurrent may_consume: ";_tag_dyneither(_tmp55,sizeof(char),23);}),
_tag_dyneither(_tmp54,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(_tmp4D.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp56=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp57="\n";_tag_dyneither(_tmp57,sizeof(char),2);}),_tag_dyneither(_tmp56,
sizeof(void*),0));});({void*_tmp58=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp59="anal_unordered_exps failed to preserve consumed info";
_tag_dyneither(_tmp59,sizeof(char),53);}),_tag_dyneither(_tmp58,sizeof(void*),0));});}
old_inflow=Cyc_CfFlowInfo_restore_consume_info(_tmp3A,outflow_consume,0);}else{
old_inflow=_tmp3A;}init_consume=1;outflow_consume=_tmp4D;_tmp3A=Cyc_CfFlowInfo_join_flow(
env->fenv,outer_all_changed,_tmp3A,outflow,1);};};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3A,old_inflow));if(outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({
struct Cyc_Dict_Dict*_tmp5A=_region_malloc(env->r,sizeof(*_tmp5A));_tmp5A[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp5A;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple19 _tmp5B;_tmp5B.f1=outflow;_tmp5B.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp5B;});};};}
static struct _tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int arg1_unconsumed){
struct _RegionHandle*_tmp5D=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp5F;struct Cyc_List_List*
_tmp60;struct _tuple19 _tmp5E=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp5D,env,
inflow,es,arg1_unconsumed);_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp61=_tmp5F;struct _tuple12 _tmp62;struct Cyc_Dict_Dict _tmp63;int _tmp64;_LL15: if((
_tmp61.ReachableFL).tag != 2)goto _LL17;_tmp62=(struct _tuple12)(_tmp61.ReachableFL).val;
_tmp63=_tmp62.f1;_LL16: for(0;_tmp60 != 0;(_tmp60=_tmp60->tl,es=((struct Cyc_List_List*)
_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp63,(void*)
_tmp60->hd)== Cyc_CfFlowInfo_NoneIL)({void*_tmp65=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp66="expression may not be initialized";
_tag_dyneither(_tmp66,sizeof(char),34);}),_tag_dyneither(_tmp65,sizeof(void*),0));});}
goto _LL14;_LL17: if((_tmp61.BottomFL).tag != 1)goto _LL14;_tmp64=(int)(_tmp61.BottomFL).val;
_LL18: goto _LL14;_LL14:;}return({struct _tuple13 _tmp67;_tmp67.f1=_tmp5F;_tmp67.f2=(
env->fenv)->unknown_all;_tmp67;});}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp68=outflow;int _tmp69;struct _tuple12 _tmp6A;struct Cyc_Dict_Dict _tmp6B;struct
Cyc_List_List*_tmp6C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp6D;_LL1A: if((_tmp68.BottomFL).tag
!= 1)goto _LL1C;_tmp69=(int)(_tmp68.BottomFL).val;_LL1B: return outflow;_LL1C: if((
_tmp68.ReachableFL).tag != 2)goto _LL19;_tmp6A=(struct _tuple12)(_tmp68.ReachableFL).val;
_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;_tmp6D=_tmp6A.f3;_LL1D: {union Cyc_CfFlowInfo_AbsLVal
_tmp6E=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;int _tmp6F;struct Cyc_CfFlowInfo_Place*
_tmp70;_LL1F: if((_tmp6E.UnknownL).tag != 2)goto _LL21;_tmp6F=(int)(_tmp6E.UnknownL).val;
_LL20: return outflow;_LL21: if((_tmp6E.PlaceL).tag != 1)goto _LL1E;_tmp70=(struct Cyc_CfFlowInfo_Place*)(
_tmp6E.PlaceL).val;_LL22: {void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(
env->fenv)->notzerothis;union Cyc_CfFlowInfo_FlowInfo _tmp71=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6B,env->all_changed,_tmp70,
nzval),_tmp6C,_tmp6D);return _tmp71;}_LL1E:;}_LL19:;}static struct _tuple16 Cyc_NewControlFlow_splitzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp72=outflow;int _tmp73;struct _tuple12 _tmp74;struct Cyc_Dict_Dict _tmp75;struct
Cyc_List_List*_tmp76;struct Cyc_CfFlowInfo_ConsumeInfo _tmp77;_LL24: if((_tmp72.BottomFL).tag
!= 1)goto _LL26;_tmp73=(int)(_tmp72.BottomFL).val;_LL25: return({struct _tuple16
_tmp78;_tmp78.f1=outflow;_tmp78.f2=outflow;_tmp78;});_LL26: if((_tmp72.ReachableFL).tag
!= 2)goto _LL23;_tmp74=(struct _tuple12)(_tmp72.ReachableFL).val;_tmp75=_tmp74.f1;
_tmp76=_tmp74.f2;_tmp77=_tmp74.f3;_LL27: {union Cyc_CfFlowInfo_AbsLVal _tmp79=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;int _tmp7A;struct Cyc_CfFlowInfo_Place*_tmp7B;_LL29: if((_tmp79.UnknownL).tag
!= 2)goto _LL2B;_tmp7A=(int)(_tmp79.UnknownL).val;_LL2A: return({struct _tuple16
_tmp7C;_tmp7C.f1=outflow;_tmp7C.f2=outflow;_tmp7C;});_LL2B: if((_tmp79.PlaceL).tag
!= 1)goto _LL28;_tmp7B=(struct Cyc_CfFlowInfo_Place*)(_tmp79.PlaceL).val;_LL2C: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
return({struct _tuple16 _tmp7D;_tmp7D.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp75,env->all_changed,_tmp7B,nzval),_tmp76,_tmp77);_tmp7D.f2=
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp75,
env->all_changed,_tmp7B,(env->fenv)->zero),_tmp76,_tmp77);_tmp7D;});}_LL28:;}
_LL23:;}struct _tuple20{enum Cyc_Absyn_Primop f1;enum Cyc_Absyn_Primop f2;};static
union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow,struct Cyc_Absyn_Exp*e1,void*r1,enum Cyc_Absyn_Primop
p,void*r2){union Cyc_CfFlowInfo_FlowInfo _tmp7E=flow;int _tmp7F;struct _tuple12
_tmp80;struct Cyc_Dict_Dict _tmp81;struct Cyc_List_List*_tmp82;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp83;_LL2E: if((_tmp7E.BottomFL).tag != 1)goto _LL30;_tmp7F=(int)(_tmp7E.BottomFL).val;
_LL2F: return flow;_LL30: if((_tmp7E.ReachableFL).tag != 2)goto _LL2D;_tmp80=(struct
_tuple12)(_tmp7E.ReachableFL).val;_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;_tmp83=
_tmp80.f3;_LL31: {void*_tmp84=r2;struct Cyc_List_List*_tmp86;_LL33: {struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp85=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp84;if(_tmp85->tag != 6)goto _LL35;
else{_tmp86=_tmp85->f1;}}_LL34: {union Cyc_CfFlowInfo_AbsLVal _tmp87=(Cyc_NewControlFlow_anal_Lexp(
env,flow,e1)).f2;int _tmp88;struct Cyc_CfFlowInfo_Place*_tmp89;_LL38: if((_tmp87.UnknownL).tag
!= 2)goto _LL3A;_tmp88=(int)(_tmp87.UnknownL).val;_LL39: return flow;_LL3A: if((
_tmp87.PlaceL).tag != 1)goto _LL37;_tmp89=(struct Cyc_CfFlowInfo_Place*)(_tmp87.PlaceL).val;
_LL3B: {struct Cyc_List_List*new_cl;{void*_tmp8A=r1;struct Cyc_List_List*_tmp8C;
enum Cyc_CfFlowInfo_InitLevel _tmp8E;_LL3D: {struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp8B=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp8A;if(_tmp8B->tag != 6)goto _LL3F;
else{_tmp8C=_tmp8B->f1;}}_LL3E: new_cl=_tmp8C;goto _LL3C;_LL3F: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp8D=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp8A;if(_tmp8D->tag != 3)goto
_LL41;else{_tmp8E=_tmp8D->f1;if(_tmp8E != Cyc_CfFlowInfo_AllIL)goto _LL41;}}_LL40:
goto _LL42;_LL41: {struct Cyc_CfFlowInfo_Zero_struct*_tmp8F=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp8A;if(_tmp8F->tag != 0)goto _LL43;}_LL42: goto _LL44;_LL43: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp90=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp8A;if(_tmp90->tag != 1)goto
_LL45;}_LL44: new_cl=0;goto _LL3C;_LL45:;_LL46: return flow;_LL3C:;}for(0;_tmp86 != 0;
_tmp86=_tmp86->tl){enum Cyc_Absyn_Primop new_cmp;{struct _tuple20 _tmp92=({struct
_tuple20 _tmp91;_tmp91.f1=p;_tmp91.f2=((struct Cyc_CfFlowInfo_TagCmp*)_tmp86->hd)->cmp;
_tmp91;});enum Cyc_Absyn_Primop _tmp93;enum Cyc_Absyn_Primop _tmp94;enum Cyc_Absyn_Primop
_tmp95;enum Cyc_Absyn_Primop _tmp96;_LL48: _tmp93=_tmp92.f1;if(_tmp93 != Cyc_Absyn_Lt)
goto _LL4A;_LL49: goto _LL4B;_LL4A: _tmp94=_tmp92.f2;if(_tmp94 != Cyc_Absyn_Lt)goto
_LL4C;_LL4B: new_cmp=Cyc_Absyn_Lt;goto _LL47;_LL4C: _tmp95=_tmp92.f1;if(_tmp95 != 
Cyc_Absyn_Eq)goto _LL4E;_tmp96=_tmp92.f2;if(_tmp96 != Cyc_Absyn_Eq)goto _LL4E;_LL4D:
new_cmp=Cyc_Absyn_Eq;goto _LL47;_LL4E:;_LL4F: new_cmp=Cyc_Absyn_Lte;goto _LL47;
_LL47:;}new_cl=({struct Cyc_List_List*_tmp97=_region_malloc(env->r,sizeof(*_tmp97));
_tmp97->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp98=_region_malloc(env->r,sizeof(*
_tmp98));_tmp98->cmp=new_cmp;_tmp98->bd=((struct Cyc_CfFlowInfo_TagCmp*)_tmp86->hd)->bd;
_tmp98;});_tmp97->tl=new_cl;_tmp97;});}return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e1->loc,_tmp81,env->all_changed,_tmp89,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp99=_region_malloc(env->r,sizeof(*_tmp99));_tmp99[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp9A;_tmp9A.tag=6;_tmp9A.f1=new_cl;_tmp9A;});_tmp99;})),_tmp82,_tmp83);}_LL37:;}
_LL35:;_LL36: return flow;_LL32:;}_LL2D:;}static struct Cyc_CfFlowInfo_NotZero_struct
Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct
Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};static struct
_tuple13 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*
e,void*r){void*_tmp9D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp9F;void*_tmpA0;struct Cyc_Absyn_PtrAtts
_tmpA1;union Cyc_Absyn_Constraint*_tmpA2;union Cyc_Absyn_Constraint*_tmpA3;_LL51: {
struct Cyc_Absyn_PointerType_struct*_tmp9E=(struct Cyc_Absyn_PointerType_struct*)
_tmp9D;if(_tmp9E->tag != 5)goto _LL53;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9F.elt_typ;
_tmpA1=_tmp9F.ptr_atts;_tmpA2=_tmpA1.bounds;_tmpA3=_tmpA1.zero_term;}}_LL52: {
union Cyc_CfFlowInfo_FlowInfo _tmpA4=f;int _tmpA5;struct _tuple12 _tmpA6;struct Cyc_Dict_Dict
_tmpA7;struct Cyc_List_List*_tmpA8;_LL56: if((_tmpA4.BottomFL).tag != 1)goto _LL58;
_tmpA5=(int)(_tmpA4.BottomFL).val;_LL57: return({struct _tuple13 _tmpA9;_tmpA9.f1=f;
_tmpA9.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA0,(env->fenv)->unknown_all);
_tmpA9;});_LL58: if((_tmpA4.ReachableFL).tag != 2)goto _LL55;_tmpA6=(struct _tuple12)(
_tmpA4.ReachableFL).val;_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_LL59: if(Cyc_Tcutil_is_bound_one(
_tmpA2)){void*_tmpAA=r;struct Cyc_CfFlowInfo_Place*_tmpAE;enum Cyc_CfFlowInfo_InitLevel
_tmpB1;_LL5B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpAB=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpAA;if(_tmpAB->tag != 1)goto _LL5D;}_LL5C: goto _LL5E;_LL5D: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmpAC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmpAA;if(_tmpAC->tag != 2)goto
_LL5F;}_LL5E:{void*_tmpB2=e->annot;struct Cyc_List_List*_tmpB4;_LL68: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmpB3=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmpB2;if(_tmpB3->tag != Cyc_CfFlowInfo_NotZero)
goto _LL6A;else{_tmpB4=_tmpB3->f1;}}_LL69: if(!Cyc_CfFlowInfo_same_relns(_tmpA8,
_tmpB4))goto _LL6B;goto _LL67;_LL6A:;_LL6B:{void*_tmpB5=e->r;_LL6D: {struct Cyc_Absyn_Subscript_e_struct*
_tmpB6=(struct Cyc_Absyn_Subscript_e_struct*)_tmpB5;if(_tmpB6->tag != 24)goto _LL6F;}
_LL6E: e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmpB7=_cycalloc(
sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmpB8;_tmpB8.tag=
Cyc_CfFlowInfo_NotZero;_tmpB8.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA8);_tmpB8;});_tmpB7;});goto _LL6C;_LL6F:;_LL70: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL6C;_LL6C:;}goto _LL67;_LL67:;}goto _LL5A;_LL5F: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpAD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpAA;if(_tmpAD->tag != 5)goto
_LL61;else{_tmpAE=_tmpAD->f1;}}_LL60:{void*_tmpB9=e->annot;struct Cyc_List_List*
_tmpBB;_LL72: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpBA=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpB9;if(_tmpBA->tag != Cyc_CfFlowInfo_NotZero)goto _LL74;else{_tmpBB=_tmpBA->f1;}}
_LL73: if(!Cyc_CfFlowInfo_same_relns(_tmpA8,_tmpBB))goto _LL75;goto _LL71;_LL74:;
_LL75:{void*_tmpBC=e->r;_LL77: {struct Cyc_Absyn_Subscript_e_struct*_tmpBD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpBC;if(_tmpBD->tag != 24)goto _LL79;}_LL78:
e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmpBE=_cycalloc(sizeof(*
_tmpBE));_tmpBE[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmpBF;_tmpBF.tag=Cyc_CfFlowInfo_NotZero;
_tmpBF.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpA8);_tmpBF;});
_tmpBE;});goto _LL76;_LL79:;_LL7A: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL76;_LL76:;}goto _LL71;_LL71:;}return({struct _tuple13 _tmpC0;_tmpC0.f1=f;
_tmpC0.f2=Cyc_CfFlowInfo_lookup_place(_tmpA7,_tmpAE);_tmpC0;});_LL61: {struct Cyc_CfFlowInfo_Zero_struct*
_tmpAF=(struct Cyc_CfFlowInfo_Zero_struct*)_tmpAA;if(_tmpAF->tag != 0)goto _LL63;}
_LL62: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;return({struct _tuple13 _tmpC1;
_tmpC1.f1=Cyc_CfFlowInfo_BottomFL();_tmpC1.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,
_tmpA0,(env->fenv)->unknown_all);_tmpC1;});_LL63: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpB0=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpAA;if(_tmpB0->tag != 3)goto
_LL65;else{_tmpB1=_tmpB0->f1;}}_LL64: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,
_tmpB1);goto _LL66;_LL65:;_LL66:{void*_tmpC2=e->r;_LL7C: {struct Cyc_Absyn_Subscript_e_struct*
_tmpC3=(struct Cyc_Absyn_Subscript_e_struct*)_tmpC2;if(_tmpC3->tag != 24)goto _LL7E;}
_LL7D: e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpC4=_cycalloc(
sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmpC5;_tmpC5.tag=
Cyc_CfFlowInfo_UnknownZ;_tmpC5.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA8);_tmpC5;});_tmpC4;});goto _LL7B;_LL7E:;_LL7F: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL7B;_LL7B:;}goto _LL5A;_LL5A:;}else{void*_tmpC6=e->annot;struct Cyc_List_List*
_tmpC8;_LL81: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpC7=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpC6;if(_tmpC7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL83;else{_tmpC8=_tmpC7->f1;}}
_LL82: if(!Cyc_CfFlowInfo_same_relns(_tmpA8,_tmpC8))goto _LL84;goto _LL80;_LL83:;
_LL84: e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpC9=_cycalloc(
sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmpCA;_tmpCA.tag=
Cyc_CfFlowInfo_UnknownZ;_tmpCA.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA8);_tmpCA;});_tmpC9;});goto _LL80;_LL80:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpA7,r)){case Cyc_CfFlowInfo_NoneIL: _LL85:({void*_tmpCB=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmpCC="dereference of possibly uninitialized pointer";
_tag_dyneither(_tmpCC,sizeof(char),46);}),_tag_dyneither(_tmpCB,sizeof(void*),0));});
goto _LL86;case Cyc_CfFlowInfo_AllIL: _LL86: return({struct _tuple13 _tmpCD;_tmpCD.f1=
f;_tmpCD.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA0,(env->fenv)->unknown_all);
_tmpCD;});case Cyc_CfFlowInfo_ThisIL: _LL87: return({struct _tuple13 _tmpCE;_tmpCE.f1=
f;_tmpCE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA0,(env->fenv)->unknown_none);
_tmpCE;});}_LL55:;}_LL53:;_LL54:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Core_Impossible_struct
_tmpD0;_tmpD0.tag=Cyc_Core_Impossible;_tmpD0.f1=({const char*_tmpD1="right deref of non-pointer-type";
_tag_dyneither(_tmpD1,sizeof(char),32);});_tmpD0;});_tmpCF;}));_LL50:;}static
struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*
rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmpD2=e1->r;void*_tmpD4;struct Cyc_Absyn_Vardecl*_tmpD6;void*_tmpD8;struct
Cyc_Absyn_Vardecl*_tmpDA;void*_tmpDC;struct Cyc_Absyn_Vardecl*_tmpDE;void*_tmpE0;
struct Cyc_Absyn_Vardecl*_tmpE2;_LL8A: {struct Cyc_Absyn_Var_e_struct*_tmpD3=(
struct Cyc_Absyn_Var_e_struct*)_tmpD2;if(_tmpD3->tag != 1)goto _LL8C;else{_tmpD4=(
void*)_tmpD3->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpD5=(struct Cyc_Absyn_Pat_b_struct*)
_tmpD4;if(_tmpD5->tag != 5)goto _LL8C;else{_tmpD6=_tmpD5->f1;}};}}_LL8B: _tmpDA=
_tmpD6;goto _LL8D;_LL8C: {struct Cyc_Absyn_Var_e_struct*_tmpD7=(struct Cyc_Absyn_Var_e_struct*)
_tmpD2;if(_tmpD7->tag != 1)goto _LL8E;else{_tmpD8=(void*)_tmpD7->f2;{struct Cyc_Absyn_Local_b_struct*
_tmpD9=(struct Cyc_Absyn_Local_b_struct*)_tmpD8;if(_tmpD9->tag != 4)goto _LL8E;
else{_tmpDA=_tmpD9->f1;}};}}_LL8D: _tmpDE=_tmpDA;goto _LL8F;_LL8E: {struct Cyc_Absyn_Var_e_struct*
_tmpDB=(struct Cyc_Absyn_Var_e_struct*)_tmpD2;if(_tmpDB->tag != 1)goto _LL90;else{
_tmpDC=(void*)_tmpDB->f2;{struct Cyc_Absyn_Param_b_struct*_tmpDD=(struct Cyc_Absyn_Param_b_struct*)
_tmpDC;if(_tmpDD->tag != 3)goto _LL90;else{_tmpDE=_tmpDD->f1;}};}}_LL8F: _tmpE2=
_tmpDE;goto _LL91;_LL90: {struct Cyc_Absyn_Var_e_struct*_tmpDF=(struct Cyc_Absyn_Var_e_struct*)
_tmpD2;if(_tmpDF->tag != 1)goto _LL92;else{_tmpE0=(void*)_tmpDF->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpE1=(struct Cyc_Absyn_Global_b_struct*)_tmpE0;if(_tmpE1->tag != 1)goto _LL92;
else{_tmpE2=_tmpE1->f1;}};}}_LL91: if(!_tmpE2->escapes){void*_tmpE3=e2->r;void*
_tmpE5;struct Cyc_Absyn_Vardecl*_tmpE7;void*_tmpE9;struct Cyc_Absyn_Vardecl*_tmpEB;
void*_tmpED;struct Cyc_Absyn_Vardecl*_tmpEF;void*_tmpF1;struct Cyc_Absyn_Vardecl*
_tmpF3;_LL95: {struct Cyc_Absyn_Var_e_struct*_tmpE4=(struct Cyc_Absyn_Var_e_struct*)
_tmpE3;if(_tmpE4->tag != 1)goto _LL97;else{_tmpE5=(void*)_tmpE4->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmpE6=(struct Cyc_Absyn_Pat_b_struct*)_tmpE5;if(_tmpE6->tag != 5)goto _LL97;else{
_tmpE7=_tmpE6->f1;}};}}_LL96: _tmpEB=_tmpE7;goto _LL98;_LL97: {struct Cyc_Absyn_Var_e_struct*
_tmpE8=(struct Cyc_Absyn_Var_e_struct*)_tmpE3;if(_tmpE8->tag != 1)goto _LL99;else{
_tmpE9=(void*)_tmpE8->f2;{struct Cyc_Absyn_Local_b_struct*_tmpEA=(struct Cyc_Absyn_Local_b_struct*)
_tmpE9;if(_tmpEA->tag != 4)goto _LL99;else{_tmpEB=_tmpEA->f1;}};}}_LL98: _tmpEF=
_tmpEB;goto _LL9A;_LL99: {struct Cyc_Absyn_Var_e_struct*_tmpEC=(struct Cyc_Absyn_Var_e_struct*)
_tmpE3;if(_tmpEC->tag != 1)goto _LL9B;else{_tmpED=(void*)_tmpEC->f2;{struct Cyc_Absyn_Param_b_struct*
_tmpEE=(struct Cyc_Absyn_Param_b_struct*)_tmpED;if(_tmpEE->tag != 3)goto _LL9B;
else{_tmpEF=_tmpEE->f1;}};}}_LL9A: _tmpF3=_tmpEF;goto _LL9C;_LL9B: {struct Cyc_Absyn_Var_e_struct*
_tmpF0=(struct Cyc_Absyn_Var_e_struct*)_tmpE3;if(_tmpF0->tag != 1)goto _LL9D;else{
_tmpF1=(void*)_tmpF0->f2;{struct Cyc_Absyn_Global_b_struct*_tmpF2=(struct Cyc_Absyn_Global_b_struct*)
_tmpF1;if(_tmpF2->tag != 1)goto _LL9D;else{_tmpF3=_tmpF2->f1;}};}}_LL9C: if(!_tmpF3->escapes){
int found=0;{struct Cyc_List_List*_tmpF4=relns;for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
struct Cyc_CfFlowInfo_Reln*_tmpF5=(struct Cyc_CfFlowInfo_Reln*)_tmpF4->hd;if(
_tmpF5->vd == _tmpF3){union Cyc_CfFlowInfo_RelnOp _tmpF6=_tmpF5->rop;struct Cyc_Absyn_Vardecl*
_tmpF7;_LLA0: if((_tmpF6.LessNumelts).tag != 3)goto _LLA2;_tmpF7=(struct Cyc_Absyn_Vardecl*)(
_tmpF6.LessNumelts).val;if(!(_tmpF7 == _tmpE2))goto _LLA2;_LLA1: return relns;_LLA2:;
_LLA3: continue;_LL9F:;}}}if(!found)return({struct Cyc_List_List*_tmpF8=
_region_malloc(rgn,sizeof(*_tmpF8));_tmpF8->hd=({struct Cyc_CfFlowInfo_Reln*
_tmpF9=_region_malloc(rgn,sizeof(*_tmpF9));_tmpF9->vd=_tmpF3;_tmpF9->rop=Cyc_CfFlowInfo_LessNumelts(
_tmpE2);_tmpF9;});_tmpF8->tl=relns;_tmpF8;});}return relns;_LL9D:;_LL9E: return
relns;_LL94:;}else{return relns;}_LL92:;_LL93: return relns;_LL89:;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_may_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_CfFlowInfo_Place*place,void*t,struct
Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*_tmpFA=env->fenv;struct Cyc_List_List*
_tmpFB=Cyc_NewControlFlow_noalias_ptrs_rec(env,place,t);if(ps != 0)*ps=_tmpFB;if(
_tmpFB == 0)return cinfo;cinfo.may_consume=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(_tmpFA->r,Cyc_CfFlowInfo_place_cmp,
_tmpFB,cinfo.may_consume);return cinfo;}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e){struct _RegionHandle*_tmpFC=env->r;if(Cyc_Tcutil_is_noalias_path(
_tmpFC,e)){struct Cyc_CfFlowInfo_FlowEnv*_tmpFD=env->fenv;union Cyc_CfFlowInfo_AbsLVal
_tmpFF;struct _tuple15 _tmpFE=Cyc_NewControlFlow_anal_Lexp(env,inflow,e);_tmpFF=
_tmpFE.f2;{struct _tuple21 _tmp101=({struct _tuple21 _tmp100;_tmp100.f1=_tmpFF;
_tmp100.f2=inflow;_tmp100;});union Cyc_CfFlowInfo_AbsLVal _tmp102;struct Cyc_CfFlowInfo_Place*
_tmp103;union Cyc_CfFlowInfo_FlowInfo _tmp104;struct _tuple12 _tmp105;struct Cyc_Dict_Dict
_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_CfFlowInfo_ConsumeInfo _tmp108;
_LLA5: _tmp102=_tmp101.f1;if((_tmp102.PlaceL).tag != 1)goto _LLA7;_tmp103=(struct
Cyc_CfFlowInfo_Place*)(_tmp102.PlaceL).val;_tmp104=_tmp101.f2;if((_tmp104.ReachableFL).tag
!= 2)goto _LLA7;_tmp105=(struct _tuple12)(_tmp104.ReachableFL).val;_tmp106=_tmp105.f1;
_tmp107=_tmp105.f2;_tmp108=_tmp105.f3;_LLA6: {struct Cyc_CfFlowInfo_ConsumeInfo
_tmp109=Cyc_NewControlFlow_may_consume_place(env,_tmp108,_tmp103,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,0);return Cyc_CfFlowInfo_ReachableFL(
_tmp106,_tmp107,_tmp109);}_LLA7:;_LLA8:({struct Cyc_String_pa_struct _tmp10C;
_tmp10C.tag=0;_tmp10C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e));({void*_tmp10A[1]={& _tmp10C};Cyc_fprintf(Cyc_stderr,({const char*_tmp10B="Oops---no location for noalias_path |%s|\n";
_tag_dyneither(_tmp10B,sizeof(char),42);}),_tag_dyneither(_tmp10A,sizeof(void*),
1));});});return inflow;_LLA4:;};}return inflow;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*
p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*
loc){if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
cinfo.consumed,p)){struct Cyc_CfFlowInfo_Place _tmp10E;void*_tmp10F;struct Cyc_List_List*
_tmp110;struct Cyc_CfFlowInfo_Place*_tmp10D=p;_tmp10E=*_tmp10D;_tmp10F=_tmp10E.root;
_tmp110=_tmp10E.fields;{void*rval=(env->fenv)->zero;if((((int(*)(struct Cyc_Dict_Dict
d,void*k,void**ans))Cyc_Dict_lookup_bool)(outdict,_tmp10F,& rval) && Cyc_CfFlowInfo_initlevel(
env->fenv,outdict,rval)!= Cyc_CfFlowInfo_NoneIL) && rval != (env->fenv)->zero){
void*_tmp111=_tmp10F;struct Cyc_Absyn_Vardecl*_tmp113;_LLAA: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp112=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp111;if(_tmp112->tag != 0)goto
_LLAC;else{_tmp113=_tmp112->f1;}}_LLAB: {struct _dyneither_ptr _tmp114=Cyc_Absynpp_qvar2string(
_tmp113->name);if(_tmp110 == 0)({struct Cyc_String_pa_struct _tmp117;_tmp117.tag=0;
_tmp117.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp114);({void*_tmp115[
1]={& _tmp117};Cyc_Tcutil_warn(loc,({const char*_tmp116="may clobber unique pointer %s";
_tag_dyneither(_tmp116,sizeof(char),30);}),_tag_dyneither(_tmp115,sizeof(void*),
1));});});else{({struct Cyc_String_pa_struct _tmp11A;_tmp11A.tag=0;_tmp11A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp114);({void*_tmp118[1]={& _tmp11A};
Cyc_Tcutil_warn(loc,({const char*_tmp119="may clobber unique pointer contained in %s";
_tag_dyneither(_tmp119,sizeof(char),43);}),_tag_dyneither(_tmp118,sizeof(void*),
1));});});}goto _LLA9;}_LLAC:;_LLAD:({void*_tmp11B=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp11C="warning locations not for VarRoots";
_tag_dyneither(_tmp11C,sizeof(char),35);}),_tag_dyneither(_tmp11B,sizeof(void*),
0));});_LLA9:;}};}if(env->all_changed == 0)cinfo.consumed=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)((
env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e){struct _RegionHandle*_tmp11D=env->r;if(Cyc_Tcutil_is_noalias_path(
_tmp11D,e)){struct Cyc_CfFlowInfo_FlowEnv*_tmp11E=env->fenv;struct Cyc_List_List*
_tmp11F=0;cinfo=Cyc_NewControlFlow_may_consume_place(env,cinfo,p,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)& _tmp11F);while(
_tmp11F != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp11F))->hd,cinfo,outdict,e->loc);_tmp11F=((
struct Cyc_List_List*)_check_null(_tmp11F))->tl;}}return cinfo;}static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;struct Cyc_CfFlowInfo_Place
_tmp121;void*_tmp122;struct Cyc_List_List*_tmp123;struct Cyc_CfFlowInfo_Place*
_tmp120=p;_tmp121=*_tmp120;_tmp122=_tmp121.root;_tmp123=_tmp121.fields;{int fld=0;
for(0;ts != 0;(fld ++,ts=ts->tl)){void*_tmp124=(void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(
_tmp124)){struct Cyc_List_List*_tmp125=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmp123,({int _tmp128[1];_tmp128[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmp128,sizeof(int),1));}));l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp126=_region_malloc(env->r,sizeof(*_tmp126));_tmp126->hd=({
struct Cyc_CfFlowInfo_Place*_tmp127=_region_malloc(env->r,sizeof(*_tmp127));
_tmp127->root=_tmp122;_tmp127->fields=_tmp125;_tmp127;});_tmp126->tl=0;_tmp126;}),
l);}else{if(Cyc_Absyn_is_aggr_type(_tmp124)){struct Cyc_List_List*_tmp129=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(env->r,_tmp123,({int _tmp12C[1];_tmp12C[0]=fld;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmp12C,sizeof(int),1));}));struct Cyc_CfFlowInfo_Place*_tmp12A=({struct Cyc_CfFlowInfo_Place*
_tmp12B=_region_malloc(env->r,sizeof(*_tmp12B));_tmp12B->root=_tmp122;_tmp12B->fields=
_tmp129;_tmp12B;});l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,
struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,l,Cyc_NewControlFlow_noalias_ptrs_rec(
env,_tmp12A,_tmp124));}}}}return l;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*
f2;};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(t))
return({struct Cyc_List_List*_tmp12D=_region_malloc(env->r,sizeof(*_tmp12D));
_tmp12D->hd=p;_tmp12D->tl=0;_tmp12D;});{void*_tmp12E=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_AggrInfo
_tmp134;union Cyc_Absyn_AggrInfoU _tmp135;struct Cyc_Absyn_Aggrdecl**_tmp136;struct
Cyc_List_List*_tmp137;struct Cyc_Absyn_AggrInfo _tmp139;union Cyc_Absyn_AggrInfoU
_tmp13A;struct _tuple3 _tmp13B;struct Cyc_Absyn_DatatypeFieldInfo _tmp13E;union Cyc_Absyn_DatatypeFieldInfoU
_tmp13F;struct Cyc_List_List*_tmp140;_LLAF: {struct Cyc_Absyn_TupleType_struct*
_tmp12F=(struct Cyc_Absyn_TupleType_struct*)_tmp12E;if(_tmp12F->tag != 11)goto
_LLB1;else{_tmp130=_tmp12F->f1;}}_LLB0: {struct Cyc_List_List*_tmp141=0;while(
_tmp130 != 0){_tmp141=({struct Cyc_List_List*_tmp142=_region_malloc(env->r,sizeof(*
_tmp142));_tmp142->hd=(*((struct _tuple22*)_tmp130->hd)).f2;_tmp142->tl=_tmp141;
_tmp142;});_tmp130=_tmp130->tl;}_tmp141=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp141);return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,
_tmp141);}_LLB1: {struct Cyc_Absyn_AnonAggrType_struct*_tmp131=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp12E;if(_tmp131->tag != 13)goto _LLB3;else{_tmp132=_tmp131->f2;}}_LLB2: {struct
Cyc_List_List*_tmp143=0;while(_tmp132 != 0){_tmp143=({struct Cyc_List_List*_tmp144=
_region_malloc(env->r,sizeof(*_tmp144));_tmp144->hd=((struct Cyc_Absyn_Aggrfield*)
_tmp132->hd)->type;_tmp144->tl=_tmp143;_tmp144;});_tmp132=_tmp132->tl;}_tmp143=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp143);return
Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp143);}_LLB3: {struct Cyc_Absyn_AggrType_struct*
_tmp133=(struct Cyc_Absyn_AggrType_struct*)_tmp12E;if(_tmp133->tag != 12)goto _LLB5;
else{_tmp134=_tmp133->f1;_tmp135=_tmp134.aggr_info;if((_tmp135.KnownAggr).tag != 
2)goto _LLB5;_tmp136=(struct Cyc_Absyn_Aggrdecl**)(_tmp135.KnownAggr).val;_tmp137=
_tmp134.targs;}}_LLB4: if((*_tmp136)->impl == 0)return 0;else{struct Cyc_List_List*
_tmp145=0;struct _RegionHandle*_tmp146=env->r;{struct Cyc_List_List*_tmp147=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp146,_tmp146,(*_tmp136)->tvs,_tmp137);
struct Cyc_List_List*_tmp148=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*
_tmp136)->impl))->fields;while(_tmp148 != 0){_tmp145=({struct Cyc_List_List*
_tmp149=_region_malloc(env->r,sizeof(*_tmp149));_tmp149->hd=Cyc_Tcutil_rsubstitute(
_tmp146,_tmp147,((struct Cyc_Absyn_Aggrfield*)_tmp148->hd)->type);_tmp149->tl=
_tmp145;_tmp149;});_tmp148=_tmp148->tl;}}_tmp145=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp145);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp145);}_LLB5: {struct Cyc_Absyn_AggrType_struct*_tmp138=(struct Cyc_Absyn_AggrType_struct*)
_tmp12E;if(_tmp138->tag != 12)goto _LLB7;else{_tmp139=_tmp138->f1;_tmp13A=_tmp139.aggr_info;
if((_tmp13A.UnknownAggr).tag != 1)goto _LLB7;_tmp13B=(struct _tuple3)(_tmp13A.UnknownAggr).val;}}
_LLB6:({void*_tmp14A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp14B="got unknown aggr in noalias_ptrs_rec";
_tag_dyneither(_tmp14B,sizeof(char),37);}),_tag_dyneither(_tmp14A,sizeof(void*),
0));});_LLB7: {struct Cyc_Absyn_DatatypeType_struct*_tmp13C=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp12E;if(_tmp13C->tag != 3)goto _LLB9;}_LLB8: {struct _RegionHandle*_tmp14C=env->r;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp14C,t))return({struct Cyc_List_List*
_tmp14D=_region_malloc(env->r,sizeof(*_tmp14D));_tmp14D->hd=p;_tmp14D->tl=0;
_tmp14D;});else{return 0;}}_LLB9: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp13D=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp12E;if(_tmp13D->tag != 4)
goto _LLBB;else{_tmp13E=_tmp13D->f1;_tmp13F=_tmp13E.field_info;_tmp140=_tmp13E.targs;}}
_LLBA: {union Cyc_Absyn_DatatypeFieldInfoU _tmp14E=_tmp13F;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmp14F;struct _tuple2 _tmp150;struct Cyc_Absyn_Datatypedecl*_tmp151;struct Cyc_Absyn_Datatypefield*
_tmp152;_LLBE: if((_tmp14E.UnknownDatatypefield).tag != 1)goto _LLC0;_tmp14F=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp14E.UnknownDatatypefield).val;
_LLBF:({void*_tmp153=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp154="got unknown datatype field in noalias_ptrs_rec";
_tag_dyneither(_tmp154,sizeof(char),47);}),_tag_dyneither(_tmp153,sizeof(void*),
0));});_LLC0: if((_tmp14E.KnownDatatypefield).tag != 2)goto _LLBD;_tmp150=(struct
_tuple2)(_tmp14E.KnownDatatypefield).val;_tmp151=_tmp150.f1;_tmp152=_tmp150.f2;
_LLC1: {struct Cyc_List_List*_tmp155=0;struct _RegionHandle*_tmp156=env->r;{struct
Cyc_List_List*_tmp157=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp156,_tmp156,_tmp151->tvs,_tmp140);struct Cyc_List_List*_tmp158=_tmp152->typs;
while(_tmp158 != 0){_tmp155=({struct Cyc_List_List*_tmp159=_region_malloc(env->r,
sizeof(*_tmp159));_tmp159->hd=Cyc_Tcutil_rsubstitute(_tmp156,_tmp157,(*((struct
_tuple22*)_tmp158->hd)).f2);_tmp159->tl=_tmp155;_tmp159;});_tmp158=_tmp158->tl;}}
_tmp155=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp155);return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp155);}_LLBD:;}_LLBB:;
_LLBC: return 0;_LLAE:;};}static struct _tuple13 Cyc_NewControlFlow_do_assign(struct
Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_CfFlowInfo_ConsumeInfo outer_cinfo,struct Cyc_Position_Segment*
loc){outflow=Cyc_CfFlowInfo_consume_unique_rvals(fenv,loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp15A=outflow;int _tmp15B;struct _tuple12 _tmp15C;struct Cyc_Dict_Dict _tmp15D;
struct Cyc_List_List*_tmp15E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp15F;_LLC3: if((
_tmp15A.BottomFL).tag != 1)goto _LLC5;_tmp15B=(int)(_tmp15A.BottomFL).val;_LLC4:
return({struct _tuple13 _tmp160;_tmp160.f1=Cyc_CfFlowInfo_BottomFL();_tmp160.f2=
rval;_tmp160;});_LLC5: if((_tmp15A.ReachableFL).tag != 2)goto _LLC2;_tmp15C=(struct
_tuple12)(_tmp15A.ReachableFL).val;_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;_tmp15F=
_tmp15C.f3;_LLC6: _tmp15F=Cyc_CfFlowInfo_and_consume(fenv->r,outer_cinfo,_tmp15F);{
union Cyc_CfFlowInfo_AbsLVal _tmp161=lval;struct Cyc_CfFlowInfo_Place*_tmp162;int
_tmp163;_LLC8: if((_tmp161.PlaceL).tag != 1)goto _LLCA;_tmp162=(struct Cyc_CfFlowInfo_Place*)(
_tmp161.PlaceL).val;_LLC9: _tmp15F=Cyc_NewControlFlow_consume_assignment(env,
_tmp162,_tmp15F,_tmp15D,lexp);_tmp15D=Cyc_CfFlowInfo_assign_place(fenv,loc,
_tmp15D,env->all_changed,_tmp162,rval);_tmp15E=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp15E,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp15D,_tmp15E,
_tmp15F);Cyc_NewControlFlow_update_tryflow(env,outflow);return({struct _tuple13
_tmp164;_tmp164.f1=outflow;_tmp164.f2=rval;_tmp164;});_LLCA: if((_tmp161.UnknownL).tag
!= 2)goto _LLC7;_tmp163=(int)(_tmp161.UnknownL).val;_LLCB: return({struct _tuple13
_tmp165;_tmp165.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);
_tmp165.f2=rval;_tmp165;});_LLC7:;};_LLC2:;};}struct _tuple23{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp166=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo
_tmp167=inflow;int _tmp168;struct _tuple12 _tmp169;struct Cyc_Dict_Dict _tmp16A;
struct Cyc_List_List*_tmp16B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp16C;_LLCD: if((
_tmp167.BottomFL).tag != 1)goto _LLCF;_tmp168=(int)(_tmp167.BottomFL).val;_LLCE:
return({struct _tuple13 _tmp16D;_tmp16D.f1=Cyc_CfFlowInfo_BottomFL();_tmp16D.f2=
_tmp166->unknown_all;_tmp16D;});_LLCF: if((_tmp167.ReachableFL).tag != 2)goto _LLCC;
_tmp169=(struct _tuple12)(_tmp167.ReachableFL).val;_tmp16A=_tmp169.f1;_tmp16B=
_tmp169.f2;_tmp16C=_tmp169.f3;_LLD0: d=_tmp16A;relns=_tmp16B;cinfo=_tmp16C;_LLCC:;}{
void*_tmp16E=e->r;struct Cyc_Absyn_Exp*_tmp170;enum Cyc_Absyn_Coercion _tmp171;
struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*
_tmp177;union Cyc_Absyn_Cnst _tmp179;int _tmp17A;union Cyc_Absyn_Cnst _tmp17C;struct
_tuple6 _tmp17D;int _tmp17E;union Cyc_Absyn_Cnst _tmp180;struct _tuple6 _tmp181;void*
_tmp183;struct Cyc_List_List*_tmp186;void*_tmp18E;void*_tmp191;struct Cyc_Absyn_Vardecl*
_tmp193;void*_tmp195;struct Cyc_Absyn_Vardecl*_tmp197;void*_tmp199;struct Cyc_Absyn_Vardecl*
_tmp19B;enum Cyc_Absyn_Primop _tmp19D;struct Cyc_List_List*_tmp19E;struct Cyc_Absyn_Exp*
_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Core_Opt*_tmp1A3;struct Cyc_Core_Opt
_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Core_Opt*
_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*
_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_List_List*
_tmp1B1;struct Cyc_Absyn_MallocInfo _tmp1B3;int _tmp1B4;struct Cyc_Absyn_Exp*_tmp1B5;
void**_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B7;int _tmp1B8;struct Cyc_Absyn_Exp*_tmp1BA;
struct Cyc_Absyn_Exp*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*
_tmp1BE;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Exp*
_tmp1C4;struct _dyneither_ptr*_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C7;struct
_dyneither_ptr*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1CA;struct _dyneither_ptr*_tmp1CB;
struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CE;struct Cyc_Absyn_Exp*
_tmp1CF;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*
_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_Absyn_Exp*
_tmp1D8;struct Cyc_List_List*_tmp1DA;struct Cyc_Absyn_Datatypedecl*_tmp1DB;struct
Cyc_List_List*_tmp1DD;struct Cyc_List_List*_tmp1DF;struct Cyc_List_List*_tmp1E1;
struct Cyc_Absyn_Aggrdecl*_tmp1E2;struct Cyc_Absyn_Aggrdecl _tmp1E3;enum Cyc_Absyn_AggrKind
_tmp1E4;struct Cyc_Absyn_AggrdeclImpl*_tmp1E5;struct Cyc_Absyn_AggrdeclImpl _tmp1E6;
struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_Vardecl*
_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;int _tmp1EF;
struct Cyc_Absyn_Stmt*_tmp1F1;void*_tmp1F3;_LLD2: {struct Cyc_Absyn_Cast_e_struct*
_tmp16F=(struct Cyc_Absyn_Cast_e_struct*)_tmp16E;if(_tmp16F->tag != 15)goto _LLD4;
else{_tmp170=_tmp16F->f2;_tmp171=_tmp16F->f4;if(_tmp171 != Cyc_Absyn_NonNull_to_Null)
goto _LLD4;}}_LLD3: {union Cyc_CfFlowInfo_FlowInfo _tmp1FB;void*_tmp1FC;struct
_tuple13 _tmp1FA=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp170);_tmp1FB=_tmp1FA.f1;
_tmp1FC=_tmp1FA.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp1FE;void*_tmp1FF;struct
_tuple13 _tmp1FD=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1FB,_tmp170,
_tmp1FC);_tmp1FE=_tmp1FD.f1;_tmp1FF=_tmp1FD.f2;return({struct _tuple13 _tmp200;
_tmp200.f1=_tmp1FE;_tmp200.f2=_tmp1FC;_tmp200;});};}_LLD4: {struct Cyc_Absyn_Cast_e_struct*
_tmp172=(struct Cyc_Absyn_Cast_e_struct*)_tmp16E;if(_tmp172->tag != 15)goto _LLD6;
else{_tmp173=_tmp172->f2;}}_LLD5: _tmp175=_tmp173;goto _LLD7;_LLD6: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp174=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp16E;if(_tmp174->tag != 13)
goto _LLD8;else{_tmp175=_tmp174->f1;}}_LLD7: _tmp177=_tmp175;goto _LLD9;_LLD8: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp176=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp16E;if(_tmp176->tag != 14)goto _LLDA;else{_tmp177=_tmp176->f1;}}_LLD9: return
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp177);_LLDA: {struct Cyc_Absyn_Const_e_struct*
_tmp178=(struct Cyc_Absyn_Const_e_struct*)_tmp16E;if(_tmp178->tag != 0)goto _LLDC;
else{_tmp179=_tmp178->f1;if((_tmp179.Null_c).tag != 1)goto _LLDC;_tmp17A=(int)(
_tmp179.Null_c).val;}}_LLDB: goto _LLDD;_LLDC: {struct Cyc_Absyn_Const_e_struct*
_tmp17B=(struct Cyc_Absyn_Const_e_struct*)_tmp16E;if(_tmp17B->tag != 0)goto _LLDE;
else{_tmp17C=_tmp17B->f1;if((_tmp17C.Int_c).tag != 4)goto _LLDE;_tmp17D=(struct
_tuple6)(_tmp17C.Int_c).val;_tmp17E=_tmp17D.f2;if(_tmp17E != 0)goto _LLDE;}}_LLDD:
return({struct _tuple13 _tmp201;_tmp201.f1=inflow;_tmp201.f2=_tmp166->zero;_tmp201;});
_LLDE: {struct Cyc_Absyn_Const_e_struct*_tmp17F=(struct Cyc_Absyn_Const_e_struct*)
_tmp16E;if(_tmp17F->tag != 0)goto _LLE0;else{_tmp180=_tmp17F->f1;if((_tmp180.Int_c).tag
!= 4)goto _LLE0;_tmp181=(struct _tuple6)(_tmp180.Int_c).val;}}_LLDF: goto _LLE1;
_LLE0: {struct Cyc_Absyn_Var_e_struct*_tmp182=(struct Cyc_Absyn_Var_e_struct*)
_tmp16E;if(_tmp182->tag != 1)goto _LLE2;else{_tmp183=(void*)_tmp182->f2;{struct Cyc_Absyn_Funname_b_struct*
_tmp184=(struct Cyc_Absyn_Funname_b_struct*)_tmp183;if(_tmp184->tag != 2)goto _LLE2;};}}
_LLE1: return({struct _tuple13 _tmp202;_tmp202.f1=inflow;_tmp202.f2=_tmp166->notzeroall;
_tmp202;});_LLE2: {struct Cyc_Absyn_Datatype_e_struct*_tmp185=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp16E;if(_tmp185->tag != 31)goto _LLE4;else{_tmp186=_tmp185->f1;if(_tmp186 != 0)
goto _LLE4;}}_LLE3: goto _LLE5;_LLE4: {struct Cyc_Absyn_Const_e_struct*_tmp187=(
struct Cyc_Absyn_Const_e_struct*)_tmp16E;if(_tmp187->tag != 0)goto _LLE6;}_LLE5:
goto _LLE7;_LLE6: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp188=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp16E;if(_tmp188->tag != 19)goto _LLE8;}_LLE7: goto _LLE9;_LLE8: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp189=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp16E;if(_tmp189->tag != 18)goto
_LLEA;}_LLE9: goto _LLEB;_LLEA: {struct Cyc_Absyn_Offsetof_e_struct*_tmp18A=(struct
Cyc_Absyn_Offsetof_e_struct*)_tmp16E;if(_tmp18A->tag != 20)goto _LLEC;}_LLEB: goto
_LLED;_LLEC: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp18B=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp16E;if(_tmp18B->tag != 33)goto _LLEE;}_LLED: goto _LLEF;_LLEE: {struct Cyc_Absyn_Enum_e_struct*
_tmp18C=(struct Cyc_Absyn_Enum_e_struct*)_tmp16E;if(_tmp18C->tag != 32)goto _LLF0;}
_LLEF: return({struct _tuple13 _tmp203;_tmp203.f1=inflow;_tmp203.f2=_tmp166->unknown_all;
_tmp203;});_LLF0: {struct Cyc_Absyn_Var_e_struct*_tmp18D=(struct Cyc_Absyn_Var_e_struct*)
_tmp16E;if(_tmp18D->tag != 1)goto _LLF2;else{_tmp18E=(void*)_tmp18D->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp18F=(struct Cyc_Absyn_Global_b_struct*)_tmp18E;if(_tmp18F->tag != 1)goto _LLF2;};}}
_LLF1: return({struct _tuple13 _tmp204;_tmp204.f1=inflow;_tmp204.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp166->unknown_all);
_tmp204;});_LLF2: {struct Cyc_Absyn_Var_e_struct*_tmp190=(struct Cyc_Absyn_Var_e_struct*)
_tmp16E;if(_tmp190->tag != 1)goto _LLF4;else{_tmp191=(void*)_tmp190->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp192=(struct Cyc_Absyn_Param_b_struct*)_tmp191;if(_tmp192->tag != 3)goto _LLF4;
else{_tmp193=_tmp192->f1;}};}}_LLF3: _tmp197=_tmp193;goto _LLF5;_LLF4: {struct Cyc_Absyn_Var_e_struct*
_tmp194=(struct Cyc_Absyn_Var_e_struct*)_tmp16E;if(_tmp194->tag != 1)goto _LLF6;
else{_tmp195=(void*)_tmp194->f2;{struct Cyc_Absyn_Local_b_struct*_tmp196=(struct
Cyc_Absyn_Local_b_struct*)_tmp195;if(_tmp196->tag != 4)goto _LLF6;else{_tmp197=
_tmp196->f1;}};}}_LLF5: _tmp19B=_tmp197;goto _LLF7;_LLF6: {struct Cyc_Absyn_Var_e_struct*
_tmp198=(struct Cyc_Absyn_Var_e_struct*)_tmp16E;if(_tmp198->tag != 1)goto _LLF8;
else{_tmp199=(void*)_tmp198->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp19A=(struct Cyc_Absyn_Pat_b_struct*)
_tmp199;if(_tmp19A->tag != 5)goto _LLF8;else{_tmp19B=_tmp19A->f1;}};}}_LLF7: inflow=
Cyc_NewControlFlow_may_consume_exp(env,inflow,e);return({struct _tuple13 _tmp205;
_tmp205.f1=inflow;_tmp205.f2=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp206=_region_malloc(env->r,
sizeof(*_tmp206));_tmp206[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp207;
_tmp207.tag=0;_tmp207.f1=_tmp19B;_tmp207;});_tmp206;}));_tmp205;});_LLF8: {
struct Cyc_Absyn_Primop_e_struct*_tmp19C=(struct Cyc_Absyn_Primop_e_struct*)
_tmp16E;if(_tmp19C->tag != 3)goto _LLFA;else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;}}
_LLF9: {union Cyc_CfFlowInfo_FlowInfo _tmp209;void*_tmp20A;struct _tuple13 _tmp208=
Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp19E,0);_tmp209=_tmp208.f1;
_tmp20A=_tmp208.f2;switch(_tmp19D){case Cyc_Absyn_Plus: _LL13A: goto _LL13B;case Cyc_Absyn_Minus:
_LL13B: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp19E))->hd)->loc,_tmp209);break;default: _LL13C: _tmp209=Cyc_CfFlowInfo_readthrough_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp19E))->hd)->loc,
_tmp209);break;}return({struct _tuple13 _tmp20B;_tmp20B.f1=_tmp209;_tmp20B.f2=
_tmp20A;_tmp20B;});}_LLFA: {struct Cyc_Absyn_Increment_e_struct*_tmp19F=(struct
Cyc_Absyn_Increment_e_struct*)_tmp16E;if(_tmp19F->tag != 5)goto _LLFC;else{_tmp1A0=
_tmp19F->f1;}}_LLFB: {struct Cyc_List_List _tmp20C=({struct Cyc_List_List _tmp21B;
_tmp21B.hd=_tmp1A0;_tmp21B.tl=0;_tmp21B;});union Cyc_CfFlowInfo_FlowInfo _tmp20E;
struct _tuple13 _tmp20D=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)&
_tmp20C,0);_tmp20E=_tmp20D.f1;Cyc_CfFlowInfo_check_unique_rvals(_tmp1A0->loc,
_tmp20E);{union Cyc_CfFlowInfo_AbsLVal _tmp210;struct _tuple15 _tmp20F=Cyc_NewControlFlow_anal_Lexp(
env,_tmp20E,_tmp1A0);_tmp210=_tmp20F.f2;{struct _tuple21 _tmp212=({struct _tuple21
_tmp211;_tmp211.f1=_tmp210;_tmp211.f2=_tmp20E;_tmp211;});union Cyc_CfFlowInfo_AbsLVal
_tmp213;struct Cyc_CfFlowInfo_Place*_tmp214;union Cyc_CfFlowInfo_FlowInfo _tmp215;
struct _tuple12 _tmp216;struct Cyc_Dict_Dict _tmp217;struct Cyc_List_List*_tmp218;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp219;_LL13F: _tmp213=_tmp212.f1;if((_tmp213.PlaceL).tag
!= 1)goto _LL141;_tmp214=(struct Cyc_CfFlowInfo_Place*)(_tmp213.PlaceL).val;
_tmp215=_tmp212.f2;if((_tmp215.ReachableFL).tag != 2)goto _LL141;_tmp216=(struct
_tuple12)(_tmp215.ReachableFL).val;_tmp217=_tmp216.f1;_tmp218=_tmp216.f2;_tmp219=
_tmp216.f3;_LL140: _tmp218=Cyc_CfFlowInfo_reln_kill_exp(_tmp166->r,_tmp218,
_tmp1A0);_tmp20E=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp166,
_tmp1A0->loc,_tmp217,env->all_changed,_tmp214,_tmp166->unknown_all),_tmp218,
_tmp219);goto _LL13E;_LL141:;_LL142: goto _LL13E;_LL13E:;}return({struct _tuple13
_tmp21A;_tmp21A.f1=_tmp20E;_tmp21A.f2=_tmp166->unknown_all;_tmp21A;});};}_LLFC: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp1A1=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp16E;if(_tmp1A1->tag != 4)goto _LLFE;else{_tmp1A2=_tmp1A1->f1;_tmp1A3=_tmp1A1->f2;
if(_tmp1A3 == 0)goto _LLFE;_tmp1A4=*_tmp1A3;_tmp1A5=_tmp1A1->f3;}}_LLFD: {struct
Cyc_List_List _tmp21C=({struct Cyc_List_List _tmp22C;_tmp22C.hd=_tmp1A5;_tmp22C.tl=
0;_tmp22C;});struct Cyc_List_List _tmp21D=({struct Cyc_List_List _tmp22B;_tmp22B.hd=
_tmp1A2;_tmp22B.tl=(struct Cyc_List_List*)& _tmp21C;_tmp22B;});union Cyc_CfFlowInfo_FlowInfo
_tmp21F;struct _tuple13 _tmp21E=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp21D,1);_tmp21F=_tmp21E.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp221;struct _tuple15 _tmp220=Cyc_NewControlFlow_anal_Lexp(env,_tmp21F,_tmp1A2);
_tmp221=_tmp220.f2;_tmp21F=Cyc_CfFlowInfo_consume_unique_rvals(_tmp166,e->loc,
_tmp21F);{union Cyc_CfFlowInfo_FlowInfo _tmp222=_tmp21F;struct _tuple12 _tmp223;
struct Cyc_Dict_Dict _tmp224;struct Cyc_List_List*_tmp225;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp226;_LL144: if((_tmp222.ReachableFL).tag != 2)goto _LL146;_tmp223=(struct
_tuple12)(_tmp222.ReachableFL).val;_tmp224=_tmp223.f1;_tmp225=_tmp223.f2;_tmp226=
_tmp223.f3;_LL145:{union Cyc_CfFlowInfo_AbsLVal _tmp227=_tmp221;struct Cyc_CfFlowInfo_Place*
_tmp228;int _tmp229;_LL149: if((_tmp227.PlaceL).tag != 1)goto _LL14B;_tmp228=(struct
Cyc_CfFlowInfo_Place*)(_tmp227.PlaceL).val;_LL14A: _tmp226=Cyc_NewControlFlow_consume_assignment(
env,_tmp228,_tmp226,_tmp224,_tmp1A2);_tmp225=Cyc_CfFlowInfo_reln_kill_exp(
_tmp166->r,_tmp225,_tmp1A2);_tmp224=Cyc_CfFlowInfo_assign_place(_tmp166,_tmp1A2->loc,
_tmp224,env->all_changed,_tmp228,_tmp166->unknown_all);_tmp21F=Cyc_CfFlowInfo_ReachableFL(
_tmp224,_tmp225,_tmp226);goto _LL148;_LL14B: if((_tmp227.UnknownL).tag != 2)goto
_LL148;_tmp229=(int)(_tmp227.UnknownL).val;_LL14C: goto _LL148;_LL148:;}goto _LL143;
_LL146:;_LL147: goto _LL143;_LL143:;}return({struct _tuple13 _tmp22A;_tmp22A.f1=
_tmp21F;_tmp22A.f2=_tmp166->unknown_all;_tmp22A;});};}_LLFE: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp1A6=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp16E;if(_tmp1A6->tag != 4)goto
_LL100;else{_tmp1A7=_tmp1A6->f1;_tmp1A8=_tmp1A6->f2;if(_tmp1A8 != 0)goto _LL100;
_tmp1A9=_tmp1A6->f3;}}_LLFF: {struct Cyc_Dict_Dict*_tmp22D=env->all_changed;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp22F;union Cyc_CfFlowInfo_FlowInfo _tmp230;
struct _tuple14 _tmp22E=Cyc_CfFlowInfo_save_consume_info(_tmp166,inflow,1);_tmp22F=
_tmp22E.f1;_tmp230=_tmp22E.f2;{struct Cyc_CfFlowInfo_ConsumeInfo empty_consume=({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp24B;_tmp24B.consumed=_tmp166->mt_place_set;
_tmp24B.may_consume=0;_tmp24B;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp231=_region_malloc(env->r,sizeof(*_tmp231));_tmp231[0]=_tmp166->mt_place_set;
_tmp231;});{union Cyc_CfFlowInfo_FlowInfo _tmp233;union Cyc_CfFlowInfo_AbsLVal
_tmp234;struct _tuple15 _tmp232=Cyc_NewControlFlow_anal_Lexp(env,_tmp230,_tmp1A7);
_tmp233=_tmp232.f1;_tmp234=_tmp232.f2;{struct Cyc_Dict_Dict _tmp235=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp236=
_region_malloc(env->r,sizeof(*_tmp236));_tmp236[0]=_tmp166->mt_place_set;_tmp236;});{
union Cyc_CfFlowInfo_FlowInfo _tmp238;void*_tmp239;struct _tuple13 _tmp237=Cyc_NewControlFlow_anal_Rexp(
env,_tmp230,_tmp1A9);_tmp238=_tmp237.f1;_tmp239=_tmp237.f2;{struct Cyc_Dict_Dict
_tmp23A=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo
_tmp23B=Cyc_CfFlowInfo_after_flow(_tmp166,(struct Cyc_Dict_Dict*)& _tmp235,_tmp233,
_tmp238,_tmp235,_tmp23A);union Cyc_CfFlowInfo_FlowInfo _tmp23C=Cyc_CfFlowInfo_join_flow(
_tmp166,_tmp22D,_tmp230,_tmp23B,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp23E;
struct _tuple14 _tmp23D=Cyc_CfFlowInfo_save_consume_info(_tmp166,_tmp23C,0);
_tmp23E=_tmp23D.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
outflow_consume,_tmp23E)){({void*_tmp23F=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp240="sanity consumed: ";_tag_dyneither(_tmp240,sizeof(char),18);}),
_tag_dyneither(_tmp23F,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp241=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp242="\ncurrent consumed: ";_tag_dyneither(_tmp242,sizeof(char),20);}),
_tag_dyneither(_tmp241,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp23E.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp243=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp244="\nsanity may_consume: ";_tag_dyneither(_tmp244,sizeof(char),22);}),
_tag_dyneither(_tmp243,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(outflow_consume.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp245=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp246="\ncurrent may_consume: ";_tag_dyneither(_tmp246,sizeof(char),23);}),
_tag_dyneither(_tmp245,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(_tmp23E.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp247=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp248="\n";_tag_dyneither(_tmp248,sizeof(char),2);}),_tag_dyneither(_tmp247,
sizeof(void*),0));});({void*_tmp249=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp24A="assignOp_e failed to preserve consume info";
_tag_dyneither(_tmp24A,sizeof(char),43);}),_tag_dyneither(_tmp249,sizeof(void*),
0));});}_tmp230=Cyc_CfFlowInfo_restore_consume_info(_tmp230,outflow_consume,0);}
outflow_consume=_tmp23E;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp23C,_tmp230)){if(_tmp22D == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp22D,Cyc_CfFlowInfo_union_place_set(
_tmp235,_tmp23A,0),0);}return Cyc_NewControlFlow_do_assign(_tmp166,env,_tmp23B,
_tmp1A7,_tmp234,_tmp1A9,_tmp239,_tmp22F,e->loc);}_tmp230=Cyc_CfFlowInfo_restore_consume_info(
_tmp23C,empty_consume,0);};};};};}};}_LL100: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp1AA=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp16E;if(_tmp1AA->tag != 9)goto _LL102;
else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}}_LL101: {union Cyc_CfFlowInfo_FlowInfo
_tmp24D;void*_tmp24E;struct _tuple13 _tmp24C=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1AB);_tmp24D=_tmp24C.f1;_tmp24E=_tmp24C.f2;_tmp24D=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp1AB->loc,_tmp24D);return Cyc_NewControlFlow_anal_Rexp(env,_tmp24D,_tmp1AC);}
_LL102: {struct Cyc_Absyn_Throw_e_struct*_tmp1AD=(struct Cyc_Absyn_Throw_e_struct*)
_tmp16E;if(_tmp1AD->tag != 12)goto _LL104;else{_tmp1AE=_tmp1AD->f1;}}_LL103: {
union Cyc_CfFlowInfo_FlowInfo _tmp250;void*_tmp251;struct _tuple13 _tmp24F=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1AE);_tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;_tmp250=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,_tmp1AE->loc,_tmp250);Cyc_NewControlFlow_use_Rval(env,_tmp1AE->loc,
_tmp250,_tmp251);return({struct _tuple13 _tmp252;_tmp252.f1=Cyc_CfFlowInfo_BottomFL();
_tmp252.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp166,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp166->unknown_all);_tmp252;});}_LL104: {struct Cyc_Absyn_FnCall_e_struct*
_tmp1AF=(struct Cyc_Absyn_FnCall_e_struct*)_tmp16E;if(_tmp1AF->tag != 11)goto
_LL106;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1AF->f2;}}_LL105: {struct
_RegionHandle*_tmp253=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp256;struct Cyc_List_List*
_tmp257;struct _tuple19 _tmp255=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp253,
env,inflow,({struct Cyc_List_List*_tmp254=_region_malloc(_tmp253,sizeof(*_tmp254));
_tmp254->hd=_tmp1B0;_tmp254->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(_tmp253,_tmp1B1);_tmp254;}),1);_tmp256=
_tmp255.f1;_tmp257=_tmp255.f2;_tmp256=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,e->loc,_tmp256);{union Cyc_CfFlowInfo_FlowInfo _tmp258=Cyc_NewControlFlow_use_Rval(
env,_tmp1B0->loc,_tmp256,(void*)((struct Cyc_List_List*)_check_null(_tmp257))->hd);
_tmp257=_tmp257->tl;{struct Cyc_List_List*init_params=0;{void*_tmp259=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B0->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp25B;void*_tmp25C;_LL14E: {struct Cyc_Absyn_PointerType_struct*_tmp25A=(struct
Cyc_Absyn_PointerType_struct*)_tmp259;if(_tmp25A->tag != 5)goto _LL150;else{
_tmp25B=_tmp25A->f1;_tmp25C=_tmp25B.elt_typ;}}_LL14F:{void*_tmp25D=Cyc_Tcutil_compress(
_tmp25C);struct Cyc_Absyn_FnInfo _tmp25F;struct Cyc_List_List*_tmp260;_LL153: {
struct Cyc_Absyn_FnType_struct*_tmp25E=(struct Cyc_Absyn_FnType_struct*)_tmp25D;
if(_tmp25E->tag != 10)goto _LL155;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25F.attributes;}}
_LL154: for(0;_tmp260 != 0;_tmp260=_tmp260->tl){void*_tmp261=(void*)_tmp260->hd;
int _tmp263;_LL158: {struct Cyc_Absyn_Initializes_att_struct*_tmp262=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp261;if(_tmp262->tag != 20)goto _LL15A;else{_tmp263=_tmp262->f1;}}_LL159:
init_params=({struct Cyc_List_List*_tmp264=_region_malloc(_tmp253,sizeof(*_tmp264));
_tmp264->hd=(void*)_tmp263;_tmp264->tl=init_params;_tmp264;});goto _LL157;_LL15A:;
_LL15B: goto _LL157;_LL157:;}goto _LL152;_LL155:;_LL156:({void*_tmp265=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp266="anal_Rexp: bad function type";_tag_dyneither(_tmp266,sizeof(char),29);}),
_tag_dyneither(_tmp265,sizeof(void*),0));});_LL152:;}goto _LL14D;_LL150:;_LL151:({
void*_tmp267=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp268="anal_Rexp: bad function type";_tag_dyneither(_tmp268,sizeof(
char),29);}),_tag_dyneither(_tmp267,sizeof(void*),0));});_LL14D:;}{int i=1;for(0;
_tmp257 != 0;(((_tmp257=_tmp257->tl,_tmp1B1=((struct Cyc_List_List*)_check_null(
_tmp1B1))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
init_params,i)){_tmp258=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1B1))->hd)->loc,_tmp258,(void*)_tmp257->hd);
continue;}{union Cyc_CfFlowInfo_FlowInfo _tmp269=_tmp256;int _tmp26A;struct _tuple12
_tmp26B;struct Cyc_Dict_Dict _tmp26C;_LL15D: if((_tmp269.BottomFL).tag != 1)goto
_LL15F;_tmp26A=(int)(_tmp269.BottomFL).val;_LL15E: goto _LL15C;_LL15F: if((_tmp269.ReachableFL).tag
!= 2)goto _LL15C;_tmp26B=(struct _tuple12)(_tmp269.ReachableFL).val;_tmp26C=
_tmp26B.f1;_LL160: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp26C,(void*)_tmp257->hd)
== Cyc_CfFlowInfo_NoneIL)({void*_tmp26D=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1B1))->hd)->loc,({const char*_tmp26E="expression may not be initialized";
_tag_dyneither(_tmp26E,sizeof(char),34);}),_tag_dyneither(_tmp26D,sizeof(void*),
0));});{union Cyc_CfFlowInfo_FlowInfo _tmp26F=_tmp258;int _tmp270;struct _tuple12
_tmp271;struct Cyc_Dict_Dict _tmp272;struct Cyc_List_List*_tmp273;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp274;_LL162: if((_tmp26F.BottomFL).tag != 1)goto _LL164;_tmp270=(int)(_tmp26F.BottomFL).val;
_LL163: goto _LL161;_LL164: if((_tmp26F.ReachableFL).tag != 2)goto _LL161;_tmp271=(
struct _tuple12)(_tmp26F.ReachableFL).val;_tmp272=_tmp271.f1;_tmp273=_tmp271.f2;
_tmp274=_tmp271.f3;_LL165: {struct Cyc_Dict_Dict _tmp275=Cyc_CfFlowInfo_escape_deref(
_tmp166,_tmp272,env->all_changed,(void*)_tmp257->hd);{void*_tmp276=(void*)
_tmp257->hd;struct Cyc_CfFlowInfo_Place*_tmp278;_LL167: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp277=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp276;if(_tmp277->tag != 5)goto
_LL169;else{_tmp278=_tmp277->f1;}}_LL168:{void*_tmp279=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1B1))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp27B;void*
_tmp27C;_LL16C: {struct Cyc_Absyn_PointerType_struct*_tmp27A=(struct Cyc_Absyn_PointerType_struct*)
_tmp279;if(_tmp27A->tag != 5)goto _LL16E;else{_tmp27B=_tmp27A->f1;_tmp27C=_tmp27B.elt_typ;}}
_LL16D: _tmp275=Cyc_CfFlowInfo_assign_place(_tmp166,((struct Cyc_Absyn_Exp*)
_tmp1B1->hd)->loc,_tmp275,env->all_changed,_tmp278,Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,_tmp27C,_tmp166->esc_all));goto _LL16B;_LL16E:;_LL16F:({void*_tmp27D=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp27E="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp27E,
sizeof(char),39);}),_tag_dyneither(_tmp27D,sizeof(void*),0));});_LL16B:;}goto
_LL166;_LL169:;_LL16A: goto _LL166;_LL166:;}_tmp258=Cyc_CfFlowInfo_ReachableFL(
_tmp275,_tmp273,_tmp274);goto _LL161;}_LL161:;}goto _LL15C;_LL15C:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B0->topt))->v))return({struct
_tuple13 _tmp27F;_tmp27F.f1=Cyc_CfFlowInfo_BottomFL();_tmp27F.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp166->unknown_all);
_tmp27F;});else{return({struct _tuple13 _tmp280;_tmp280.f1=_tmp258;_tmp280.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp166->unknown_all);
_tmp280;});}};};}_LL106: {struct Cyc_Absyn_Malloc_e_struct*_tmp1B2=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp16E;if(_tmp1B2->tag != 34)goto _LL108;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B3.is_calloc;
_tmp1B5=_tmp1B3.rgn;_tmp1B6=_tmp1B3.elt_type;_tmp1B7=_tmp1B3.num_elts;_tmp1B8=
_tmp1B3.fat_result;}}_LL107: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp293=_region_malloc(_tmp166->r,sizeof(*_tmp293));_tmp293[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp294;_tmp294.tag=1;_tmp294.f1=_tmp1B7;_tmp294.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp294;});_tmp293;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp292=_region_malloc(_tmp166->r,sizeof(*_tmp292));
_tmp292->root=root;_tmp292->fields=0;_tmp292;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp290=_region_malloc(_tmp166->r,sizeof(*_tmp290));_tmp290[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp291;_tmp291.tag=5;_tmp291.f1=place;_tmp291;});_tmp290;});void*place_val=
_tmp1B8?_tmp166->notzeroall: Cyc_CfFlowInfo_typ_to_absrval(_tmp166,*((void**)
_check_null(_tmp1B6)),_tmp166->unknown_none);union Cyc_CfFlowInfo_FlowInfo outflow;((
int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);if(_tmp1B5 != 0){
struct _RegionHandle*_tmp281=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp284;struct Cyc_List_List*
_tmp285;struct _tuple19 _tmp283=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp281,
env,inflow,({struct Cyc_Absyn_Exp*_tmp282[2];_tmp282[1]=_tmp1B7;_tmp282[0]=(
struct Cyc_Absyn_Exp*)_tmp1B5;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(_tmp281,_tag_dyneither(_tmp282,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp284=_tmp283.f1;_tmp285=_tmp283.f2;outflow=
_tmp284;}else{union Cyc_CfFlowInfo_FlowInfo _tmp287;struct _tuple13 _tmp286=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1B7);_tmp287=_tmp286.f1;outflow=_tmp287;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1B7->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo _tmp288=outflow;int _tmp289;
struct _tuple12 _tmp28A;struct Cyc_Dict_Dict _tmp28B;struct Cyc_List_List*_tmp28C;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp28D;_LL171: if((_tmp288.BottomFL).tag != 1)
goto _LL173;_tmp289=(int)(_tmp288.BottomFL).val;_LL172: return({struct _tuple13
_tmp28E;_tmp28E.f1=outflow;_tmp28E.f2=rval;_tmp28E;});_LL173: if((_tmp288.ReachableFL).tag
!= 2)goto _LL170;_tmp28A=(struct _tuple12)(_tmp288.ReachableFL).val;_tmp28B=
_tmp28A.f1;_tmp28C=_tmp28A.f2;_tmp28D=_tmp28A.f3;_LL174: return({struct _tuple13
_tmp28F;_tmp28F.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,void*v))Cyc_Dict_insert)(_tmp28B,root,place_val),_tmp28C,_tmp28D);
_tmp28F.f2=rval;_tmp28F;});_LL170:;};}_LL108: {struct Cyc_Absyn_Swap_e_struct*
_tmp1B9=(struct Cyc_Absyn_Swap_e_struct*)_tmp16E;if(_tmp1B9->tag != 35)goto _LL10A;
else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;}}_LL109: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp295=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp298;struct Cyc_List_List*_tmp299;struct _tuple19
_tmp297=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp295,env,inflow,({struct Cyc_Absyn_Exp*
_tmp296[2];_tmp296[1]=_tmp1BB;_tmp296[0]=_tmp1BA;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp295,
_tag_dyneither(_tmp296,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp298=_tmp297.f1;
_tmp299=_tmp297.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp299))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp299->tl))->hd;outflow=
_tmp298;}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,outflow);{union Cyc_CfFlowInfo_AbsLVal
_tmp29B;struct _tuple15 _tmp29A=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp1BA);
_tmp29B=_tmp29A.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp29D;struct _tuple15 _tmp29C=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp1BB);_tmp29D=_tmp29C.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp29E=
outflow;struct _tuple12 _tmp29F;struct Cyc_Dict_Dict _tmp2A0;struct Cyc_List_List*
_tmp2A1;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2A2;_LL176: if((_tmp29E.ReachableFL).tag
!= 2)goto _LL178;_tmp29F=(struct _tuple12)(_tmp29E.ReachableFL).val;_tmp2A0=
_tmp29F.f1;_tmp2A1=_tmp29F.f2;_tmp2A2=_tmp29F.f3;_LL177:{union Cyc_CfFlowInfo_AbsLVal
_tmp2A3=_tmp29B;struct Cyc_CfFlowInfo_Place*_tmp2A4;int _tmp2A5;_LL17B: if((_tmp2A3.PlaceL).tag
!= 1)goto _LL17D;_tmp2A4=(struct Cyc_CfFlowInfo_Place*)(_tmp2A3.PlaceL).val;_LL17C:
_tmp2A0=Cyc_CfFlowInfo_assign_place(_tmp166,_tmp1BA->loc,_tmp2A0,env->all_changed,
_tmp2A4,right_rval);goto _LL17A;_LL17D: if((_tmp2A3.UnknownL).tag != 2)goto _LL17A;
_tmp2A5=(int)(_tmp2A3.UnknownL).val;_LL17E: goto _LL17A;_LL17A:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp2A6=_tmp29D;struct Cyc_CfFlowInfo_Place*_tmp2A7;int _tmp2A8;_LL180: if((_tmp2A6.PlaceL).tag
!= 1)goto _LL182;_tmp2A7=(struct Cyc_CfFlowInfo_Place*)(_tmp2A6.PlaceL).val;_LL181:
_tmp2A0=Cyc_CfFlowInfo_assign_place(_tmp166,_tmp1BB->loc,_tmp2A0,env->all_changed,
_tmp2A7,left_rval);goto _LL17F;_LL182: if((_tmp2A6.UnknownL).tag != 2)goto _LL17F;
_tmp2A8=(int)(_tmp2A6.UnknownL).val;_LL183: goto _LL17F;_LL17F:;}_tmp2A1=Cyc_CfFlowInfo_reln_kill_exp(
_tmp166->r,_tmp2A1,_tmp1BA);_tmp2A1=Cyc_CfFlowInfo_reln_kill_exp(_tmp166->r,
_tmp2A1,_tmp1BB);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2A0,_tmp2A1,_tmp2A2);
goto _LL175;_LL178:;_LL179: goto _LL175;_LL175:;}return({struct _tuple13 _tmp2A9;
_tmp2A9.f1=outflow;_tmp2A9.f2=_tmp166->unknown_all;_tmp2A9;});};};}_LL10A: {
struct Cyc_Absyn_New_e_struct*_tmp1BC=(struct Cyc_Absyn_New_e_struct*)_tmp16E;if(
_tmp1BC->tag != 17)goto _LL10C;else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;}}
_LL10B: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*_tmp2BD=
_region_malloc(_tmp166->r,sizeof(*_tmp2BD));_tmp2BD[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp2BE;_tmp2BE.tag=1;_tmp2BE.f1=_tmp1BE;_tmp2BE.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp2BE;});_tmp2BD;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp2BC=_region_malloc(_tmp166->r,sizeof(*_tmp2BC));
_tmp2BC->root=root;_tmp2BC->fields=0;_tmp2BC;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2BA=_region_malloc(_tmp166->r,sizeof(*_tmp2BA));_tmp2BA[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp2BB;_tmp2BB.tag=5;_tmp2BB.f1=place;_tmp2BB;});_tmp2BA;});((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp1BD != 0){struct _RegionHandle*_tmp2AA=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp2AD;struct Cyc_List_List*_tmp2AE;struct _tuple19 _tmp2AC=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2AA,env,inflow,({struct Cyc_Absyn_Exp*_tmp2AB[2];_tmp2AB[1]=_tmp1BE;_tmp2AB[0]=(
struct Cyc_Absyn_Exp*)_tmp1BD;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(_tmp2AA,_tag_dyneither(_tmp2AB,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp2AD=_tmp2AC.f1;_tmp2AE=_tmp2AC.f2;outflow=
_tmp2AD;place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp2AE))->tl))->hd;}else{union Cyc_CfFlowInfo_FlowInfo _tmp2B0;void*
_tmp2B1;struct _tuple13 _tmp2AF=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1BE);
_tmp2B0=_tmp2AF.f1;_tmp2B1=_tmp2AF.f2;outflow=_tmp2B0;place_val=_tmp2B1;}outflow=
Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1BE->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp2B2=outflow;int _tmp2B3;struct _tuple12 _tmp2B4;struct Cyc_Dict_Dict _tmp2B5;
struct Cyc_List_List*_tmp2B6;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2B7;_LL185: if((
_tmp2B2.BottomFL).tag != 1)goto _LL187;_tmp2B3=(int)(_tmp2B2.BottomFL).val;_LL186:
return({struct _tuple13 _tmp2B8;_tmp2B8.f1=outflow;_tmp2B8.f2=rval;_tmp2B8;});
_LL187: if((_tmp2B2.ReachableFL).tag != 2)goto _LL184;_tmp2B4=(struct _tuple12)(
_tmp2B2.ReachableFL).val;_tmp2B5=_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;_tmp2B7=_tmp2B4.f3;
_LL188: return({struct _tuple13 _tmp2B9;_tmp2B9.f1=Cyc_CfFlowInfo_ReachableFL(((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp2B5,root,place_val),_tmp2B6,_tmp2B7);_tmp2B9.f2=rval;_tmp2B9;});_LL184:;};};}
_LL10C: {struct Cyc_Absyn_Address_e_struct*_tmp1BF=(struct Cyc_Absyn_Address_e_struct*)
_tmp16E;if(_tmp1BF->tag != 16)goto _LL10E;else{_tmp1C0=_tmp1BF->f1;}}_LL10D: {
union Cyc_CfFlowInfo_FlowInfo _tmp2C0;struct _tuple13 _tmp2BF=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1C0);_tmp2C0=_tmp2BF.f1;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp2C2;
struct _tuple14 _tmp2C1=Cyc_CfFlowInfo_save_consume_info(env->fenv,_tmp2C0,0);
_tmp2C2=_tmp2C1.f1;{union Cyc_CfFlowInfo_FlowInfo _tmp2C4;union Cyc_CfFlowInfo_AbsLVal
_tmp2C5;struct _tuple15 _tmp2C3=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp1C0);
_tmp2C4=_tmp2C3.f1;_tmp2C5=_tmp2C3.f2;_tmp2C4=Cyc_CfFlowInfo_restore_consume_info(
_tmp2C4,_tmp2C2,0);{union Cyc_CfFlowInfo_AbsLVal _tmp2C6=_tmp2C5;int _tmp2C7;struct
Cyc_CfFlowInfo_Place*_tmp2C8;_LL18A: if((_tmp2C6.UnknownL).tag != 2)goto _LL18C;
_tmp2C7=(int)(_tmp2C6.UnknownL).val;_LL18B: return({struct _tuple13 _tmp2C9;_tmp2C9.f1=
_tmp2C4;_tmp2C9.f2=_tmp166->notzeroall;_tmp2C9;});_LL18C: if((_tmp2C6.PlaceL).tag
!= 1)goto _LL189;_tmp2C8=(struct Cyc_CfFlowInfo_Place*)(_tmp2C6.PlaceL).val;_LL18D:
return({struct _tuple13 _tmp2CA;_tmp2CA.f1=_tmp2C4;_tmp2CA.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2CB=_region_malloc(env->r,sizeof(*_tmp2CB));_tmp2CB[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp2CC;_tmp2CC.tag=5;_tmp2CC.f1=_tmp2C8;_tmp2CC;});_tmp2CB;});_tmp2CA;});_LL189:;};};};}
_LL10E: {struct Cyc_Absyn_Deref_e_struct*_tmp1C1=(struct Cyc_Absyn_Deref_e_struct*)
_tmp16E;if(_tmp1C1->tag != 21)goto _LL110;else{_tmp1C2=_tmp1C1->f1;}}_LL10F: {
union Cyc_CfFlowInfo_FlowInfo _tmp2CE;void*_tmp2CF;struct _tuple13 _tmp2CD=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1C2);_tmp2CE=_tmp2CD.f1;_tmp2CF=_tmp2CD.f2;_tmp2CE=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp2CE);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2CE,_tmp1C2,
_tmp2CF);}_LL110: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1C3=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp16E;if(_tmp1C3->tag != 22)goto _LL112;else{_tmp1C4=_tmp1C3->f1;_tmp1C5=_tmp1C3->f2;}}
_LL111: {union Cyc_CfFlowInfo_FlowInfo _tmp2D1;void*_tmp2D2;struct _tuple13 _tmp2D0=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1C4);_tmp2D1=_tmp2D0.f1;_tmp2D2=
_tmp2D0.f2;_tmp2D1=Cyc_CfFlowInfo_drop_unique_rvals(e->loc,_tmp2D1);_tmp2D1=Cyc_NewControlFlow_may_consume_exp(
env,_tmp2D1,e);if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C4->topt))->v))return({struct _tuple13 _tmp2D3;_tmp2D3.f1=_tmp2D1;
_tmp2D3.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp166,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp166->unknown_all);_tmp2D3;});{void*_tmp2D4=_tmp2D2;
int _tmp2D6;struct _dyneither_ptr _tmp2D7;_LL18F: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2D5=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2D4;if(_tmp2D5->tag != 7)goto
_LL191;else{_tmp2D6=_tmp2D5->f1;_tmp2D7=_tmp2D5->f2;}}_LL190: {int _tmp2D8=Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C4->topt))->v,_tmp1C5);return({
struct _tuple13 _tmp2D9;_tmp2D9.f1=_tmp2D1;_tmp2D9.f2=*((void**)
_check_dyneither_subscript(_tmp2D7,sizeof(void*),_tmp2D8));_tmp2D9;});}_LL191:;
_LL192:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2DA=_cycalloc(
sizeof(*_tmp2DA));_tmp2DA[0]=({struct Cyc_Core_Impossible_struct _tmp2DB;_tmp2DB.tag=
Cyc_Core_Impossible;_tmp2DB.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2DE;_tmp2DE.tag=0;_tmp2DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));({void*_tmp2DC[1]={& _tmp2DE};Cyc_aprintf(({const char*
_tmp2DD="anal_Rexp: AggrMember: %s";_tag_dyneither(_tmp2DD,sizeof(char),26);}),
_tag_dyneither(_tmp2DC,sizeof(void*),1));});});_tmp2DB;});_tmp2DA;}));_LL18E:;};}
_LL112: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp1C6=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp16E;if(_tmp1C6->tag != 38)goto _LL114;else{_tmp1C7=_tmp1C6->f1;_tmp1C8=_tmp1C6->f2;}}
_LL113: {union Cyc_CfFlowInfo_FlowInfo _tmp2E0;void*_tmp2E1;struct _tuple13 _tmp2DF=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1C7);_tmp2E0=_tmp2DF.f1;_tmp2E1=
_tmp2DF.f2;_tmp2E0=Cyc_CfFlowInfo_drop_unique_rvals(e->loc,_tmp2E0);_tmp2E0=Cyc_NewControlFlow_may_consume_exp(
env,_tmp2E0,e);return({struct _tuple13 _tmp2E2;_tmp2E2.f1=_tmp2E0;_tmp2E2.f2=
_tmp166->unknown_all;_tmp2E2;});}_LL114: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1C9=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp16E;if(_tmp1C9->tag != 23)goto
_LL116;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;}}_LL115: {union Cyc_CfFlowInfo_FlowInfo
_tmp2E4;void*_tmp2E5;struct _tuple13 _tmp2E3=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1CA);_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;_tmp2E4=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp2E4);{union Cyc_CfFlowInfo_FlowInfo _tmp2E7;void*_tmp2E8;struct _tuple13
_tmp2E6=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2E4,_tmp1CA,_tmp2E5);
_tmp2E7=_tmp2E6.f1;_tmp2E8=_tmp2E6.f2;{void*_tmp2E9=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CA->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2EB;void*_tmp2EC;_LL194: {struct Cyc_Absyn_PointerType_struct*_tmp2EA=(struct
Cyc_Absyn_PointerType_struct*)_tmp2E9;if(_tmp2EA->tag != 5)goto _LL196;else{
_tmp2EB=_tmp2EA->f1;_tmp2EC=_tmp2EB.elt_typ;}}_LL195: if(Cyc_Absyn_is_nontagged_union_type(
_tmp2EC))return({struct _tuple13 _tmp2ED;_tmp2ED.f1=_tmp2E7;_tmp2ED.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp166->unknown_all);
_tmp2ED;});{void*_tmp2EE=_tmp2E8;int _tmp2F0;struct _dyneither_ptr _tmp2F1;_LL199: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2EF=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2EE;if(_tmp2EF->tag != 7)goto _LL19B;else{_tmp2F0=_tmp2EF->f1;_tmp2F1=_tmp2EF->f2;}}
_LL19A: {int _tmp2F2=Cyc_CfFlowInfo_get_field_index(_tmp2EC,_tmp1CB);return({
struct _tuple13 _tmp2F3;_tmp2F3.f1=_tmp2E7;_tmp2F3.f2=*((void**)
_check_dyneither_subscript(_tmp2F1,sizeof(void*),_tmp2F2));_tmp2F3;});}_LL19B:;
_LL19C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2F4=_cycalloc(
sizeof(*_tmp2F4));_tmp2F4[0]=({struct Cyc_Core_Impossible_struct _tmp2F5;_tmp2F5.tag=
Cyc_Core_Impossible;_tmp2F5.f1=({const char*_tmp2F6="anal_Rexp: AggrArrow";
_tag_dyneither(_tmp2F6,sizeof(char),21);});_tmp2F5;});_tmp2F4;}));_LL198:;};
_LL196:;_LL197:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2F7=
_cycalloc(sizeof(*_tmp2F7));_tmp2F7[0]=({struct Cyc_Core_Impossible_struct _tmp2F8;
_tmp2F8.tag=Cyc_Core_Impossible;_tmp2F8.f1=({const char*_tmp2F9="anal_Rexp: AggrArrow ptr";
_tag_dyneither(_tmp2F9,sizeof(char),25);});_tmp2F8;});_tmp2F7;}));_LL193:;};};}
_LL116: {struct Cyc_Absyn_Conditional_e_struct*_tmp1CC=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp16E;if(_tmp1CC->tag != 6)goto _LL118;else{_tmp1CD=_tmp1CC->f1;_tmp1CE=_tmp1CC->f2;
_tmp1CF=_tmp1CC->f3;}}_LL117: {union Cyc_CfFlowInfo_FlowInfo _tmp2FB;union Cyc_CfFlowInfo_FlowInfo
_tmp2FC;struct _tuple16 _tmp2FA=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1CD);
_tmp2FB=_tmp2FA.f1;_tmp2FC=_tmp2FA.f2;_tmp2FB=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1CD->loc,_tmp2FB);_tmp2FC=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1CD->loc,
_tmp2FC);{struct _tuple13 _tmp2FD=Cyc_NewControlFlow_anal_Rexp(env,_tmp2FB,_tmp1CE);
struct _tuple13 _tmp2FE=Cyc_NewControlFlow_anal_Rexp(env,_tmp2FC,_tmp1CF);return
Cyc_CfFlowInfo_join_flow_and_rval(_tmp166,env->all_changed,_tmp2FD,_tmp2FE,1);};}
_LL118: {struct Cyc_Absyn_And_e_struct*_tmp1D0=(struct Cyc_Absyn_And_e_struct*)
_tmp16E;if(_tmp1D0->tag != 7)goto _LL11A;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;}}
_LL119: {union Cyc_CfFlowInfo_FlowInfo _tmp300;union Cyc_CfFlowInfo_FlowInfo _tmp301;
struct _tuple16 _tmp2FF=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1D1);_tmp300=
_tmp2FF.f1;_tmp301=_tmp2FF.f2;_tmp300=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1D1->loc,_tmp300);_tmp301=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D1->loc,
_tmp301);{union Cyc_CfFlowInfo_FlowInfo _tmp303;void*_tmp304;struct _tuple13 _tmp302=
Cyc_NewControlFlow_anal_Rexp(env,_tmp300,_tmp1D2);_tmp303=_tmp302.f1;_tmp304=
_tmp302.f2;_tmp303=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D2->loc,_tmp303);{
struct _tuple13 _tmp305=({struct _tuple13 _tmp308;_tmp308.f1=_tmp303;_tmp308.f2=
_tmp304;_tmp308;});struct _tuple13 _tmp306=({struct _tuple13 _tmp307;_tmp307.f1=
_tmp301;_tmp307.f2=_tmp166->zero;_tmp307;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp166,env->all_changed,_tmp305,_tmp306,0);};};}_LL11A: {struct Cyc_Absyn_Or_e_struct*
_tmp1D3=(struct Cyc_Absyn_Or_e_struct*)_tmp16E;if(_tmp1D3->tag != 8)goto _LL11C;
else{_tmp1D4=_tmp1D3->f1;_tmp1D5=_tmp1D3->f2;}}_LL11B: {union Cyc_CfFlowInfo_FlowInfo
_tmp30A;union Cyc_CfFlowInfo_FlowInfo _tmp30B;struct _tuple16 _tmp309=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1D4);_tmp30A=_tmp309.f1;_tmp30B=_tmp309.f2;_tmp30A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1D4->loc,_tmp30A);_tmp30B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D4->loc,
_tmp30B);{union Cyc_CfFlowInfo_FlowInfo _tmp30D;void*_tmp30E;struct _tuple13 _tmp30C=
Cyc_NewControlFlow_anal_Rexp(env,_tmp30B,_tmp1D5);_tmp30D=_tmp30C.f1;_tmp30E=
_tmp30C.f2;_tmp30D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D5->loc,_tmp30D);{
struct _tuple13 _tmp30F=({struct _tuple13 _tmp312;_tmp312.f1=_tmp30D;_tmp312.f2=
_tmp30E;_tmp312;});struct _tuple13 _tmp310=({struct _tuple13 _tmp311;_tmp311.f1=
_tmp30A;_tmp311.f2=_tmp166->notzeroall;_tmp311;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp166,env->all_changed,_tmp30F,_tmp310,0);};};}_LL11C: {struct Cyc_Absyn_Subscript_e_struct*
_tmp1D6=(struct Cyc_Absyn_Subscript_e_struct*)_tmp16E;if(_tmp1D6->tag != 24)goto
_LL11E;else{_tmp1D7=_tmp1D6->f1;_tmp1D8=_tmp1D6->f2;}}_LL11D: {struct
_RegionHandle*_tmp313=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp316;struct Cyc_List_List*
_tmp317;struct _tuple19 _tmp315=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp313,
env,inflow,({struct Cyc_Absyn_Exp*_tmp314[2];_tmp314[1]=_tmp1D8;_tmp314[0]=
_tmp1D7;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
_tmp313,_tag_dyneither(_tmp314,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp316=
_tmp315.f1;_tmp317=_tmp315.f2;_tmp316=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1D8->loc,_tmp316);{union Cyc_CfFlowInfo_FlowInfo _tmp318=_tmp316;{union Cyc_CfFlowInfo_FlowInfo
_tmp319=_tmp316;struct _tuple12 _tmp31A;struct Cyc_Dict_Dict _tmp31B;struct Cyc_List_List*
_tmp31C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp31D;_LL19E: if((_tmp319.ReachableFL).tag
!= 2)goto _LL1A0;_tmp31A=(struct _tuple12)(_tmp319.ReachableFL).val;_tmp31B=
_tmp31A.f1;_tmp31C=_tmp31A.f2;_tmp31D=_tmp31A.f3;_LL19F: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp31B,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp317))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)({void*_tmp31E=0;Cyc_Tcutil_terr(
_tmp1D8->loc,({const char*_tmp31F="expression may not be initialized";
_tag_dyneither(_tmp31F,sizeof(char),34);}),_tag_dyneither(_tmp31E,sizeof(void*),
0));});{struct Cyc_List_List*_tmp320=Cyc_NewControlFlow_add_subscript_reln(
_tmp166->r,_tmp31C,_tmp1D7,_tmp1D8);if(_tmp31C != _tmp320)_tmp318=Cyc_CfFlowInfo_ReachableFL(
_tmp31B,_tmp320,_tmp31D);goto _LL19D;};_LL1A0:;_LL1A1: goto _LL19D;_LL19D:;}{void*
_tmp321=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D7->topt))->v);
struct Cyc_List_List*_tmp323;struct Cyc_Absyn_PtrInfo _tmp325;struct Cyc_Absyn_PtrAtts
_tmp326;union Cyc_Absyn_Constraint*_tmp327;_LL1A3: {struct Cyc_Absyn_TupleType_struct*
_tmp322=(struct Cyc_Absyn_TupleType_struct*)_tmp321;if(_tmp322->tag != 11)goto
_LL1A5;else{_tmp323=_tmp322->f1;}}_LL1A4: {void*_tmp328=(void*)((struct Cyc_List_List*)
_check_null(_tmp317))->hd;int _tmp32A;struct _dyneither_ptr _tmp32B;_LL1AA: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp329=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp328;if(_tmp329->tag != 7)goto _LL1AC;else{_tmp32A=_tmp329->f1;_tmp32B=_tmp329->f2;}}
_LL1AB: _tmp318=Cyc_NewControlFlow_may_consume_exp(env,_tmp318,e);{unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp1D8)).f1;return({struct _tuple13 _tmp32C;_tmp32C.f1=
_tmp318;_tmp32C.f2=*((void**)_check_dyneither_subscript(_tmp32B,sizeof(void*),(
int)i));_tmp32C;});};_LL1AC:;_LL1AD:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D[0]=({struct Cyc_Core_Impossible_struct
_tmp32E;_tmp32E.tag=Cyc_Core_Impossible;_tmp32E.f1=({const char*_tmp32F="anal_Rexp: Subscript";
_tag_dyneither(_tmp32F,sizeof(char),21);});_tmp32E;});_tmp32D;}));_LL1A9:;}
_LL1A5: {struct Cyc_Absyn_PointerType_struct*_tmp324=(struct Cyc_Absyn_PointerType_struct*)
_tmp321;if(_tmp324->tag != 5)goto _LL1A7;else{_tmp325=_tmp324->f1;_tmp326=_tmp325.ptr_atts;
_tmp327=_tmp326.bounds;}}_LL1A6: {union Cyc_CfFlowInfo_FlowInfo _tmp331;void*
_tmp332;struct _tuple13 _tmp330=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp316,
_tmp1D7,(void*)((struct Cyc_List_List*)_check_null(_tmp317))->hd);_tmp331=_tmp330.f1;
_tmp332=_tmp330.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp333=_tmp331;int _tmp334;
_LL1AF: if((_tmp333.BottomFL).tag != 1)goto _LL1B1;_tmp334=(int)(_tmp333.BottomFL).val;
_LL1B0: return({struct _tuple13 _tmp335;_tmp335.f1=_tmp331;_tmp335.f2=_tmp332;
_tmp335;});_LL1B1:;_LL1B2: return({struct _tuple13 _tmp336;_tmp336.f1=_tmp318;
_tmp336.f2=_tmp332;_tmp336;});_LL1AE:;};}_LL1A7:;_LL1A8:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337[0]=({
struct Cyc_Core_Impossible_struct _tmp338;_tmp338.tag=Cyc_Core_Impossible;_tmp338.f1=({
const char*_tmp339="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp339,
sizeof(char),33);});_tmp338;});_tmp337;}));_LL1A2:;};};}_LL11E: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1D9=(struct Cyc_Absyn_Datatype_e_struct*)_tmp16E;if(_tmp1D9->tag != 31)goto
_LL120;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1D9->f2;}}_LL11F: _tmp1DD=_tmp1DA;
goto _LL121;_LL120: {struct Cyc_Absyn_Tuple_e_struct*_tmp1DC=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp16E;if(_tmp1DC->tag != 25)goto _LL122;else{_tmp1DD=_tmp1DC->f1;}}_LL121: {
struct _RegionHandle*_tmp33A=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp33C;struct Cyc_List_List*
_tmp33D;struct _tuple19 _tmp33B=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp33A,
env,inflow,_tmp1DD,0);_tmp33C=_tmp33B.f1;_tmp33D=_tmp33B.f2;_tmp33C=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,e->loc,_tmp33C);{struct _dyneither_ptr aggrdict=({unsigned int _tmp341=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1DD);void**
_tmp342=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp341));
struct _dyneither_ptr _tmp345=_tag_dyneither(_tmp342,sizeof(void*),_tmp341);{
unsigned int _tmp343=_tmp341;unsigned int i;for(i=0;i < _tmp343;i ++){_tmp342[i]=({
void*_tmp344=(void*)((struct Cyc_List_List*)_check_null(_tmp33D))->hd;_tmp33D=
_tmp33D->tl;_tmp344;});}}_tmp345;});return({struct _tuple13 _tmp33E;_tmp33E.f1=
_tmp33C;_tmp33E.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp33F=
_region_malloc(env->r,sizeof(*_tmp33F));_tmp33F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp340;_tmp340.tag=7;_tmp340.f1=0;_tmp340.f2=aggrdict;_tmp340;});_tmp33F;});
_tmp33E;});};}_LL122: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp1DE=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp16E;if(_tmp1DE->tag != 30)goto _LL124;else{_tmp1DF=_tmp1DE->f2;}}_LL123: {
struct Cyc_List_List*fs;{void*_tmp346=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_List_List*_tmp348;_LL1B4: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp347=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp346;if(_tmp347->tag != 13)goto
_LL1B6;else{_tmp348=_tmp347->f2;}}_LL1B5: fs=_tmp348;goto _LL1B3;_LL1B6:;_LL1B7:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp349=_cycalloc(sizeof(*
_tmp349));_tmp349[0]=({struct Cyc_Core_Impossible_struct _tmp34A;_tmp34A.tag=Cyc_Core_Impossible;
_tmp34A.f1=({const char*_tmp34B="anal_Rexp:anon struct has bad type";
_tag_dyneither(_tmp34B,sizeof(char),35);});_tmp34A;});_tmp349;}));_LL1B3:;}
_tmp1E1=_tmp1DF;_tmp1E4=Cyc_Absyn_StructA;_tmp1E7=fs;goto _LL125;}_LL124: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp1E0=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp16E;if(_tmp1E0->tag != 29)goto _LL126;else{_tmp1E1=_tmp1E0->f3;_tmp1E2=_tmp1E0->f4;
if(_tmp1E2 == 0)goto _LL126;_tmp1E3=*_tmp1E2;_tmp1E4=_tmp1E3.kind;_tmp1E5=_tmp1E3.impl;
if(_tmp1E5 == 0)goto _LL126;_tmp1E6=*_tmp1E5;_tmp1E7=_tmp1E6.fields;}}_LL125: {
struct _RegionHandle*_tmp34C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp34E;struct Cyc_List_List*
_tmp34F;struct _tuple19 _tmp34D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp34C,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple23*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp34C,(struct Cyc_Absyn_Exp*(*)(
struct _tuple23*))Cyc_Core_snd,_tmp1E1),0);_tmp34E=_tmp34D.f1;_tmp34F=_tmp34D.f2;
_tmp34E=Cyc_CfFlowInfo_consume_unique_rvals(_tmp166,e->loc,_tmp34E);{struct
_dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp166,_tmp1E7,
_tmp166->unknown_all);{int i=0;for(0;_tmp34F != 0;(((_tmp34F=_tmp34F->tl,_tmp1E1=
_tmp1E1->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple23*)((struct Cyc_List_List*)
_check_null(_tmp1E1))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*_tmp350=(void*)ds->hd;
struct _dyneither_ptr*_tmp353;_LL1B9: {struct Cyc_Absyn_ArrayElement_struct*
_tmp351=(struct Cyc_Absyn_ArrayElement_struct*)_tmp350;if(_tmp351->tag != 0)goto
_LL1BB;}_LL1BA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp354=
_cycalloc(sizeof(*_tmp354));_tmp354[0]=({struct Cyc_Core_Impossible_struct _tmp355;
_tmp355.tag=Cyc_Core_Impossible;_tmp355.f1=({const char*_tmp356="anal_Rexp:Aggregate_e";
_tag_dyneither(_tmp356,sizeof(char),22);});_tmp355;});_tmp354;}));_LL1BB: {
struct Cyc_Absyn_FieldName_struct*_tmp352=(struct Cyc_Absyn_FieldName_struct*)
_tmp350;if(_tmp352->tag != 1)goto _LL1B8;else{_tmp353=_tmp352->f1;}}_LL1BC: {int
_tmp357=Cyc_CfFlowInfo_get_field_index_fs(_tmp1E7,_tmp353);*((void**)
_check_dyneither_subscript(aggrdict,sizeof(void*),_tmp357))=(void*)_tmp34F->hd;
if(_tmp1E4 == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*_tmp358=(*((struct _tuple23*)
_tmp1E1->hd)).f2;_tmp34E=Cyc_NewControlFlow_use_Rval(env,_tmp358->loc,_tmp34E,(
void*)_tmp34F->hd);}}_LL1B8:;}}}return({struct _tuple13 _tmp359;_tmp359.f1=_tmp34E;
_tmp359.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp35A=_region_malloc(
env->r,sizeof(*_tmp35A));_tmp35A[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp35B;_tmp35B.tag=7;_tmp35B.f1=_tmp1E4 == Cyc_Absyn_UnionA;_tmp35B.f2=aggrdict;
_tmp35B;});_tmp35A;});_tmp359;});};}_LL126: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1E8=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp16E;if(_tmp1E8->tag != 29)goto
_LL128;}_LL127:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp35C=
_cycalloc(sizeof(*_tmp35C));_tmp35C[0]=({struct Cyc_Core_Impossible_struct _tmp35D;
_tmp35D.tag=Cyc_Core_Impossible;_tmp35D.f1=({const char*_tmp35E="anal_Rexp:missing aggrdeclimpl";
_tag_dyneither(_tmp35E,sizeof(char),31);});_tmp35D;});_tmp35C;}));_LL128: {
struct Cyc_Absyn_Array_e_struct*_tmp1E9=(struct Cyc_Absyn_Array_e_struct*)_tmp16E;
if(_tmp1E9->tag != 27)goto _LL12A;else{_tmp1EA=_tmp1E9->f1;}}_LL129: {struct
_RegionHandle*_tmp35F=env->r;struct Cyc_List_List*_tmp360=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple23*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp35F,(struct Cyc_Absyn_Exp*(*)(struct _tuple23*))Cyc_Core_snd,
_tmp1EA);union Cyc_CfFlowInfo_FlowInfo _tmp362;struct Cyc_List_List*_tmp363;struct
_tuple19 _tmp361=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp35F,env,inflow,
_tmp360,0);_tmp362=_tmp361.f1;_tmp363=_tmp361.f2;_tmp362=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,e->loc,_tmp362);for(0;_tmp363 != 0;(_tmp363=_tmp363->tl,_tmp360=_tmp360->tl)){
_tmp362=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp360))->hd)->loc,_tmp362,(void*)_tmp363->hd);}return({struct
_tuple13 _tmp364;_tmp364.f1=_tmp362;_tmp364.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp166->unknown_all);
_tmp364;});}_LL12A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp1EB=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp16E;if(_tmp1EB->tag != 28)goto _LL12C;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EB->f2;
_tmp1EE=_tmp1EB->f3;_tmp1EF=_tmp1EB->f4;}}_LL12B: {union Cyc_CfFlowInfo_FlowInfo
_tmp366;void*_tmp367;struct _tuple13 _tmp365=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1ED);_tmp366=_tmp365.f1;_tmp367=_tmp365.f2;_tmp366=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1ED->loc,_tmp366);{union Cyc_CfFlowInfo_FlowInfo _tmp368=_tmp366;int _tmp369;
struct _tuple12 _tmp36A;struct Cyc_Dict_Dict _tmp36B;struct Cyc_List_List*_tmp36C;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp36D;_LL1BE: if((_tmp368.BottomFL).tag != 1)
goto _LL1C0;_tmp369=(int)(_tmp368.BottomFL).val;_LL1BF: return({struct _tuple13
_tmp36E;_tmp36E.f1=_tmp366;_tmp36E.f2=_tmp166->unknown_all;_tmp36E;});_LL1C0: if((
_tmp368.ReachableFL).tag != 2)goto _LL1BD;_tmp36A=(struct _tuple12)(_tmp368.ReachableFL).val;
_tmp36B=_tmp36A.f1;_tmp36C=_tmp36A.f2;_tmp36D=_tmp36A.f3;_LL1C1: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp36B,_tmp367)== Cyc_CfFlowInfo_NoneIL)({void*_tmp36F=0;Cyc_Tcutil_terr(
_tmp1ED->loc,({const char*_tmp370="expression may not be initialized";
_tag_dyneither(_tmp370,sizeof(char),34);}),_tag_dyneither(_tmp36F,sizeof(void*),
0));});{struct Cyc_List_List*new_relns=_tmp36C;comp_loop: {void*_tmp371=_tmp1ED->r;
struct Cyc_Absyn_Exp*_tmp373;void*_tmp375;struct Cyc_Absyn_Vardecl*_tmp377;void*
_tmp379;struct Cyc_Absyn_Vardecl*_tmp37B;void*_tmp37D;struct Cyc_Absyn_Vardecl*
_tmp37F;void*_tmp381;struct Cyc_Absyn_Vardecl*_tmp383;union Cyc_Absyn_Cnst _tmp385;
struct _tuple6 _tmp386;int _tmp387;enum Cyc_Absyn_Primop _tmp389;struct Cyc_List_List*
_tmp38A;struct Cyc_List_List _tmp38B;struct Cyc_Absyn_Exp*_tmp38C;_LL1C3: {struct
Cyc_Absyn_Cast_e_struct*_tmp372=(struct Cyc_Absyn_Cast_e_struct*)_tmp371;if(
_tmp372->tag != 15)goto _LL1C5;else{_tmp373=_tmp372->f2;}}_LL1C4: _tmp1ED=_tmp373;
goto comp_loop;_LL1C5:{struct Cyc_Absyn_Var_e_struct*_tmp374=(struct Cyc_Absyn_Var_e_struct*)
_tmp371;if(_tmp374->tag != 1)goto _LL1C7;else{_tmp375=(void*)_tmp374->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp376=(struct Cyc_Absyn_Global_b_struct*)_tmp375;if(
_tmp376->tag != 1)goto _LL1C7;else{_tmp377=_tmp376->f1;}};}}if(!(!_tmp377->escapes))
goto _LL1C7;_LL1C6: _tmp37B=_tmp377;goto _LL1C8;_LL1C7:{struct Cyc_Absyn_Var_e_struct*
_tmp378=(struct Cyc_Absyn_Var_e_struct*)_tmp371;if(_tmp378->tag != 1)goto _LL1C9;
else{_tmp379=(void*)_tmp378->f2;{struct Cyc_Absyn_Local_b_struct*_tmp37A=(struct
Cyc_Absyn_Local_b_struct*)_tmp379;if(_tmp37A->tag != 4)goto _LL1C9;else{_tmp37B=
_tmp37A->f1;}};}}if(!(!_tmp37B->escapes))goto _LL1C9;_LL1C8: _tmp37F=_tmp37B;goto
_LL1CA;_LL1C9:{struct Cyc_Absyn_Var_e_struct*_tmp37C=(struct Cyc_Absyn_Var_e_struct*)
_tmp371;if(_tmp37C->tag != 1)goto _LL1CB;else{_tmp37D=(void*)_tmp37C->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp37E=(struct Cyc_Absyn_Pat_b_struct*)_tmp37D;if(_tmp37E->tag
!= 5)goto _LL1CB;else{_tmp37F=_tmp37E->f1;}};}}if(!(!_tmp37F->escapes))goto _LL1CB;
_LL1CA: _tmp383=_tmp37F;goto _LL1CC;_LL1CB:{struct Cyc_Absyn_Var_e_struct*_tmp380=(
struct Cyc_Absyn_Var_e_struct*)_tmp371;if(_tmp380->tag != 1)goto _LL1CD;else{
_tmp381=(void*)_tmp380->f2;{struct Cyc_Absyn_Param_b_struct*_tmp382=(struct Cyc_Absyn_Param_b_struct*)
_tmp381;if(_tmp382->tag != 3)goto _LL1CD;else{_tmp383=_tmp382->f1;}};}}if(!(!
_tmp383->escapes))goto _LL1CD;_LL1CC: new_relns=({struct Cyc_List_List*_tmp38D=
_region_malloc(env->r,sizeof(*_tmp38D));_tmp38D->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp38E=_region_malloc(env->r,sizeof(*_tmp38E));_tmp38E->vd=_tmp1EC;_tmp38E->rop=
Cyc_CfFlowInfo_LessVar(_tmp383,_tmp383->type);_tmp38E;});_tmp38D->tl=_tmp36C;
_tmp38D;});goto _LL1C2;_LL1CD: {struct Cyc_Absyn_Const_e_struct*_tmp384=(struct Cyc_Absyn_Const_e_struct*)
_tmp371;if(_tmp384->tag != 0)goto _LL1CF;else{_tmp385=_tmp384->f1;if((_tmp385.Int_c).tag
!= 4)goto _LL1CF;_tmp386=(struct _tuple6)(_tmp385.Int_c).val;_tmp387=_tmp386.f2;}}
_LL1CE: new_relns=({struct Cyc_List_List*_tmp38F=_region_malloc(env->r,sizeof(*
_tmp38F));_tmp38F->hd=({struct Cyc_CfFlowInfo_Reln*_tmp390=_region_malloc(env->r,
sizeof(*_tmp390));_tmp390->vd=_tmp1EC;_tmp390->rop=Cyc_CfFlowInfo_LessConst((
unsigned int)_tmp387);_tmp390;});_tmp38F->tl=_tmp36C;_tmp38F;});goto _LL1C2;
_LL1CF: {struct Cyc_Absyn_Primop_e_struct*_tmp388=(struct Cyc_Absyn_Primop_e_struct*)
_tmp371;if(_tmp388->tag != 3)goto _LL1D1;else{_tmp389=_tmp388->f1;_tmp38A=_tmp388->f2;
if(_tmp38A == 0)goto _LL1D1;_tmp38B=*_tmp38A;_tmp38C=(struct Cyc_Absyn_Exp*)_tmp38B.hd;}}
_LL1D0:{void*_tmp391=_tmp38C->r;void*_tmp393;struct Cyc_Absyn_Vardecl*_tmp395;
void*_tmp397;struct Cyc_Absyn_Vardecl*_tmp399;void*_tmp39B;struct Cyc_Absyn_Vardecl*
_tmp39D;void*_tmp39F;struct Cyc_Absyn_Vardecl*_tmp3A1;_LL1D4:{struct Cyc_Absyn_Var_e_struct*
_tmp392=(struct Cyc_Absyn_Var_e_struct*)_tmp391;if(_tmp392->tag != 1)goto _LL1D6;
else{_tmp393=(void*)_tmp392->f2;{struct Cyc_Absyn_Global_b_struct*_tmp394=(struct
Cyc_Absyn_Global_b_struct*)_tmp393;if(_tmp394->tag != 1)goto _LL1D6;else{_tmp395=
_tmp394->f1;}};}}if(!(!_tmp395->escapes))goto _LL1D6;_LL1D5: _tmp399=_tmp395;goto
_LL1D7;_LL1D6:{struct Cyc_Absyn_Var_e_struct*_tmp396=(struct Cyc_Absyn_Var_e_struct*)
_tmp391;if(_tmp396->tag != 1)goto _LL1D8;else{_tmp397=(void*)_tmp396->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp398=(struct Cyc_Absyn_Local_b_struct*)_tmp397;if(
_tmp398->tag != 4)goto _LL1D8;else{_tmp399=_tmp398->f1;}};}}if(!(!_tmp399->escapes))
goto _LL1D8;_LL1D7: _tmp39D=_tmp399;goto _LL1D9;_LL1D8:{struct Cyc_Absyn_Var_e_struct*
_tmp39A=(struct Cyc_Absyn_Var_e_struct*)_tmp391;if(_tmp39A->tag != 1)goto _LL1DA;
else{_tmp39B=(void*)_tmp39A->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp39C=(struct Cyc_Absyn_Pat_b_struct*)
_tmp39B;if(_tmp39C->tag != 5)goto _LL1DA;else{_tmp39D=_tmp39C->f1;}};}}if(!(!
_tmp39D->escapes))goto _LL1DA;_LL1D9: _tmp3A1=_tmp39D;goto _LL1DB;_LL1DA:{struct Cyc_Absyn_Var_e_struct*
_tmp39E=(struct Cyc_Absyn_Var_e_struct*)_tmp391;if(_tmp39E->tag != 1)goto _LL1DC;
else{_tmp39F=(void*)_tmp39E->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3A0=(struct
Cyc_Absyn_Param_b_struct*)_tmp39F;if(_tmp3A0->tag != 3)goto _LL1DC;else{_tmp3A1=
_tmp3A0->f1;}};}}if(!(!_tmp3A1->escapes))goto _LL1DC;_LL1DB: new_relns=({struct Cyc_List_List*
_tmp3A2=_region_malloc(env->r,sizeof(*_tmp3A2));_tmp3A2->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp3A3=_region_malloc(env->r,sizeof(*_tmp3A3));_tmp3A3->vd=_tmp1EC;_tmp3A3->rop=
Cyc_CfFlowInfo_LessNumelts(_tmp3A1);_tmp3A3;});_tmp3A2->tl=_tmp36C;_tmp3A2;});
goto _LL1D3;_LL1DC:;_LL1DD: goto _LL1D3;_LL1D3:;}goto _LL1C2;_LL1D1:;_LL1D2: goto
_LL1C2;_LL1C2:;}if(_tmp36C != new_relns)_tmp366=Cyc_CfFlowInfo_ReachableFL(
_tmp36B,new_relns,_tmp36D);{void*_tmp3A4=_tmp367;_LL1DF: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3A5=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3A4;if(_tmp3A5->tag != 0)goto
_LL1E1;}_LL1E0: return({struct _tuple13 _tmp3A9;_tmp3A9.f1=_tmp366;_tmp3A9.f2=
_tmp166->unknown_all;_tmp3A9;});_LL1E1: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3A6=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3A4;if(_tmp3A6->tag != 2)
goto _LL1E3;}_LL1E2: goto _LL1E4;_LL1E3: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3A7=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3A4;if(_tmp3A7->tag != 1)
goto _LL1E5;}_LL1E4: goto _LL1E6;_LL1E5: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3A8=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3A4;if(_tmp3A8->tag != 5)goto
_LL1E7;}_LL1E6: {struct Cyc_List_List _tmp3AA=({struct Cyc_List_List _tmp3B7;_tmp3B7.hd=
_tmp1EC;_tmp3B7.tl=0;_tmp3B7;});_tmp366=Cyc_NewControlFlow_add_vars(_tmp166,
_tmp366,(struct Cyc_List_List*)& _tmp3AA,_tmp166->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3AC;void*_tmp3AD;struct _tuple13 _tmp3AB=Cyc_NewControlFlow_anal_Rexp(env,
_tmp366,_tmp1EE);_tmp3AC=_tmp3AB.f1;_tmp3AD=_tmp3AB.f2;_tmp3AC=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,_tmp1EE->loc,_tmp3AC);{union Cyc_CfFlowInfo_FlowInfo _tmp3AE=_tmp3AC;int
_tmp3AF;struct _tuple12 _tmp3B0;struct Cyc_Dict_Dict _tmp3B1;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3B2;_LL1EA: if((_tmp3AE.BottomFL).tag != 1)goto _LL1EC;_tmp3AF=(int)(_tmp3AE.BottomFL).val;
_LL1EB: return({struct _tuple13 _tmp3B3;_tmp3B3.f1=_tmp3AC;_tmp3B3.f2=_tmp166->unknown_all;
_tmp3B3;});_LL1EC: if((_tmp3AE.ReachableFL).tag != 2)goto _LL1E9;_tmp3B0=(struct
_tuple12)(_tmp3AE.ReachableFL).val;_tmp3B1=_tmp3B0.f1;_tmp3B2=_tmp3B0.f3;_LL1ED:
if(Cyc_CfFlowInfo_initlevel(_tmp166,_tmp3B1,_tmp3AD)!= Cyc_CfFlowInfo_AllIL){({
void*_tmp3B4=0;Cyc_Tcutil_terr(_tmp1ED->loc,({const char*_tmp3B5="expression may not be initialized";
_tag_dyneither(_tmp3B5,sizeof(char),34);}),_tag_dyneither(_tmp3B4,sizeof(void*),
0));});return({struct _tuple13 _tmp3B6;_tmp3B6.f1=Cyc_CfFlowInfo_BottomFL();
_tmp3B6.f2=_tmp166->unknown_all;_tmp3B6;});}_LL1E9:;}_tmp366=_tmp3AC;goto _LL1E8;};}
_LL1E7:;_LL1E8: while(1){struct Cyc_List_List _tmp3B8=({struct Cyc_List_List _tmp3C5;
_tmp3C5.hd=_tmp1EC;_tmp3C5.tl=0;_tmp3C5;});_tmp366=Cyc_NewControlFlow_add_vars(
_tmp166,_tmp366,(struct Cyc_List_List*)& _tmp3B8,_tmp166->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo _tmp3BA;void*_tmp3BB;struct _tuple13 _tmp3B9=Cyc_NewControlFlow_anal_Rexp(
env,_tmp366,_tmp1EE);_tmp3BA=_tmp3B9.f1;_tmp3BB=_tmp3B9.f2;_tmp3BA=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,_tmp1EE->loc,_tmp3BA);{union Cyc_CfFlowInfo_FlowInfo _tmp3BC=_tmp3BA;int
_tmp3BD;struct _tuple12 _tmp3BE;struct Cyc_Dict_Dict _tmp3BF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3C0;_LL1EF: if((_tmp3BC.BottomFL).tag != 1)goto _LL1F1;_tmp3BD=(int)(_tmp3BC.BottomFL).val;
_LL1F0: goto _LL1EE;_LL1F1: if((_tmp3BC.ReachableFL).tag != 2)goto _LL1EE;_tmp3BE=(
struct _tuple12)(_tmp3BC.ReachableFL).val;_tmp3BF=_tmp3BE.f1;_tmp3C0=_tmp3BE.f3;
_LL1F2: if(Cyc_CfFlowInfo_initlevel(_tmp166,_tmp3BF,_tmp3BB)!= Cyc_CfFlowInfo_AllIL){({
void*_tmp3C1=0;Cyc_Tcutil_terr(_tmp1ED->loc,({const char*_tmp3C2="expression may not be initialized";
_tag_dyneither(_tmp3C2,sizeof(char),34);}),_tag_dyneither(_tmp3C1,sizeof(void*),
0));});return({struct _tuple13 _tmp3C3;_tmp3C3.f1=Cyc_CfFlowInfo_BottomFL();
_tmp3C3.f2=_tmp166->unknown_all;_tmp3C3;});}_LL1EE:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp3C4=Cyc_CfFlowInfo_join_flow(_tmp166,env->all_changed,_tmp366,_tmp3BA,1);if(
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp3C4,_tmp366))break;_tmp366=_tmp3C4;};};}
return({struct _tuple13 _tmp3C6;_tmp3C6.f1=_tmp366;_tmp3C6.f2=_tmp166->unknown_all;
_tmp3C6;});_LL1DE:;};};_LL1BD:;};}_LL12C: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp1F0=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp16E;if(_tmp1F0->tag != 37)goto
_LL12E;else{_tmp1F1=_tmp1F0->f1;}}_LL12D: while(1){union Cyc_CfFlowInfo_FlowInfo*
_tmp3C8;struct _tuple17 _tmp3C7=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp1F1);_tmp3C8=_tmp3C7.f2;inflow=*_tmp3C8;{void*_tmp3C9=_tmp1F1->r;struct Cyc_Absyn_Stmt*
_tmp3CB;struct Cyc_Absyn_Stmt*_tmp3CC;struct Cyc_Absyn_Decl*_tmp3CE;struct Cyc_Absyn_Stmt*
_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D1;_LL1F4: {struct Cyc_Absyn_Seq_s_struct*
_tmp3CA=(struct Cyc_Absyn_Seq_s_struct*)_tmp3C9;if(_tmp3CA->tag != 2)goto _LL1F6;
else{_tmp3CB=_tmp3CA->f1;_tmp3CC=_tmp3CA->f2;}}_LL1F5: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp3CB);_tmp1F1=_tmp3CC;goto _LL1F3;_LL1F6: {struct Cyc_Absyn_Decl_s_struct*
_tmp3CD=(struct Cyc_Absyn_Decl_s_struct*)_tmp3C9;if(_tmp3CD->tag != 12)goto _LL1F8;
else{_tmp3CE=_tmp3CD->f1;_tmp3CF=_tmp3CD->f2;}}_LL1F7: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp3CE);_tmp1F1=_tmp3CF;goto _LL1F3;_LL1F8: {struct Cyc_Absyn_Exp_s_struct*
_tmp3D0=(struct Cyc_Absyn_Exp_s_struct*)_tmp3C9;if(_tmp3D0->tag != 1)goto _LL1FA;
else{_tmp3D1=_tmp3D0->f1;}}_LL1F9: return Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp3D1);_LL1FA:;_LL1FB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2[0]=({struct Cyc_Core_Impossible_struct
_tmp3D3;_tmp3D3.tag=Cyc_Core_Impossible;_tmp3D3.f1=({const char*_tmp3D4="analyze_Rexp: ill-formed StmtExp";
_tag_dyneither(_tmp3D4,sizeof(char),33);});_tmp3D3;});_tmp3D2;}));_LL1F3:;};}
_LL12E: {struct Cyc_Absyn_Var_e_struct*_tmp1F2=(struct Cyc_Absyn_Var_e_struct*)
_tmp16E;if(_tmp1F2->tag != 1)goto _LL130;else{_tmp1F3=(void*)_tmp1F2->f2;{struct
Cyc_Absyn_Unresolved_b_struct*_tmp1F4=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmp1F3;if(_tmp1F4->tag != 0)goto _LL130;};}}_LL12F: goto _LL131;_LL130: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp1F5=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp16E;if(_tmp1F5->tag != 2)goto
_LL132;}_LL131: goto _LL133;_LL132: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp1F6=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp16E;if(_tmp1F6->tag != 10)goto _LL134;}
_LL133: goto _LL135;_LL134: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1F7=(
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp16E;if(_tmp1F7->tag != 36)goto _LL136;}
_LL135: goto _LL137;_LL136: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp1F8=(struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp16E;if(_tmp1F8->tag != 26)goto _LL138;}_LL137:
goto _LL139;_LL138: {struct Cyc_Absyn_Valueof_e_struct*_tmp1F9=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp16E;if(_tmp1F9->tag != 39)goto _LLD1;}_LL139:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5[0]=({struct Cyc_Core_Impossible_struct
_tmp3D6;_tmp3D6.tag=Cyc_Core_Impossible;_tmp3D6.f1=({const char*_tmp3D7="anal_Rexp, unexpected exp form";
_tag_dyneither(_tmp3D7,sizeof(char),31);});_tmp3D6;});_tmp3D5;}));_LLD1:;};}
static struct _tuple15 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo
f,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp3D8=env->fenv;
void*_tmp3D9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp3DB;void*_tmp3DC;struct Cyc_Absyn_PtrAtts _tmp3DD;union
Cyc_Absyn_Constraint*_tmp3DE;union Cyc_Absyn_Constraint*_tmp3DF;_LL1FD: {struct
Cyc_Absyn_PointerType_struct*_tmp3DA=(struct Cyc_Absyn_PointerType_struct*)
_tmp3D9;if(_tmp3DA->tag != 5)goto _LL1FF;else{_tmp3DB=_tmp3DA->f1;_tmp3DC=_tmp3DB.elt_typ;
_tmp3DD=_tmp3DB.ptr_atts;_tmp3DE=_tmp3DD.bounds;_tmp3DF=_tmp3DD.zero_term;}}
_LL1FE: {union Cyc_CfFlowInfo_FlowInfo _tmp3E0=f;int _tmp3E1;struct _tuple12 _tmp3E2;
struct Cyc_Dict_Dict _tmp3E3;struct Cyc_List_List*_tmp3E4;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3E5;_LL202: if((_tmp3E0.BottomFL).tag != 1)goto _LL204;_tmp3E1=(int)(_tmp3E0.BottomFL).val;
_LL203: return({struct _tuple15 _tmp3E6;_tmp3E6.f1=f;_tmp3E6.f2=Cyc_CfFlowInfo_UnknownL();
_tmp3E6;});_LL204: if((_tmp3E0.ReachableFL).tag != 2)goto _LL201;_tmp3E2=(struct
_tuple12)(_tmp3E0.ReachableFL).val;_tmp3E3=_tmp3E2.f1;_tmp3E4=_tmp3E2.f2;_tmp3E5=
_tmp3E2.f3;_LL205: if(Cyc_Tcutil_is_bound_one(_tmp3DE)){void*_tmp3E7=r;struct Cyc_CfFlowInfo_Place*
_tmp3EB;struct Cyc_CfFlowInfo_Place _tmp3EC;void*_tmp3ED;struct Cyc_List_List*
_tmp3EE;enum Cyc_CfFlowInfo_InitLevel _tmp3F1;_LL207: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3E8=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3E7;if(_tmp3E8->tag != 1)
goto _LL209;}_LL208: goto _LL20A;_LL209: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3E9=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3E7;if(_tmp3E9->tag != 2)
goto _LL20B;}_LL20A: e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp3F2=
_cycalloc(sizeof(*_tmp3F2));_tmp3F2[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp3F3;_tmp3F3.tag=Cyc_CfFlowInfo_NotZero;_tmp3F3.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp3E4);_tmp3F3;});_tmp3F2;});goto _LL206;_LL20B: {struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp3EA=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp3E7;if(_tmp3EA->tag != 5)goto _LL20D;else{_tmp3EB=_tmp3EA->f1;_tmp3EC=*_tmp3EB;
_tmp3ED=_tmp3EC.root;_tmp3EE=_tmp3EC.fields;}}_LL20C: e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp3F5;_tmp3F5.tag=Cyc_CfFlowInfo_NotZero;_tmp3F5.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp3E4);_tmp3F5;});_tmp3F4;});return({struct _tuple15
_tmp3F6;_tmp3F6.f1=f;_tmp3F6.f2=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*
_tmp3F7=_region_malloc(_tmp3D8->r,sizeof(*_tmp3F7));_tmp3F7->root=_tmp3ED;
_tmp3F7->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp3D8->r,_tmp3EE,flds);_tmp3F7;}));
_tmp3F6;});_LL20D: {struct Cyc_CfFlowInfo_Zero_struct*_tmp3EF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3E7;if(_tmp3EF->tag != 0)goto _LL20F;}_LL20E: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple15 _tmp3F8;_tmp3F8.f1=Cyc_CfFlowInfo_BottomFL();_tmp3F8.f2=
Cyc_CfFlowInfo_UnknownL();_tmp3F8;});_LL20F: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3F0=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3E7;if(_tmp3F0->tag != 3)goto
_LL211;else{_tmp3F1=_tmp3F0->f1;}}_LL210: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp3F1);goto _LL212;_LL211:;_LL212: e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp3F9=_cycalloc(sizeof(*_tmp3F9));_tmp3F9[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp3FA;_tmp3FA.tag=Cyc_CfFlowInfo_UnknownZ;_tmp3FA.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp3E4);_tmp3FA;});_tmp3F9;});_LL206:;}else{e->annot=(void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp3FC;_tmp3FC.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp3FC.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3E4);_tmp3FC;});
_tmp3FB;});}if(Cyc_CfFlowInfo_initlevel(_tmp3D8,_tmp3E3,r)== Cyc_CfFlowInfo_NoneIL)({
void*_tmp3FD=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp3FE="dereference of possibly uninitialized pointer";
_tag_dyneither(_tmp3FE,sizeof(char),46);}),_tag_dyneither(_tmp3FD,sizeof(void*),
0));});return({struct _tuple15 _tmp3FF;_tmp3FF.f1=f;_tmp3FF.f2=Cyc_CfFlowInfo_UnknownL();
_tmp3FF;});_LL201:;}_LL1FF:;_LL200:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400[0]=({struct Cyc_Core_Impossible_struct
_tmp401;_tmp401.tag=Cyc_Core_Impossible;_tmp401.f1=({const char*_tmp402="left deref of non-pointer-type";
_tag_dyneither(_tmp402,sizeof(char),31);});_tmp401;});_tmp400;}));_LL1FC:;}
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp403=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo _tmp404=inflow;int _tmp405;struct _tuple12 _tmp406;struct Cyc_Dict_Dict
_tmp407;struct Cyc_List_List*_tmp408;_LL214: if((_tmp404.BottomFL).tag != 1)goto
_LL216;_tmp405=(int)(_tmp404.BottomFL).val;_LL215: return({struct _tuple15 _tmp409;
_tmp409.f1=Cyc_CfFlowInfo_BottomFL();_tmp409.f2=Cyc_CfFlowInfo_UnknownL();
_tmp409;});_LL216: if((_tmp404.ReachableFL).tag != 2)goto _LL213;_tmp406=(struct
_tuple12)(_tmp404.ReachableFL).val;_tmp407=_tmp406.f1;_tmp408=_tmp406.f2;_LL217:
d=_tmp407;_LL213:;}{void*_tmp40A=e->r;struct Cyc_Absyn_Exp*_tmp40C;struct Cyc_Absyn_Exp*
_tmp40E;struct Cyc_Absyn_Exp*_tmp410;void*_tmp412;struct Cyc_Absyn_Vardecl*_tmp414;
void*_tmp416;struct Cyc_Absyn_Vardecl*_tmp418;void*_tmp41A;struct Cyc_Absyn_Vardecl*
_tmp41C;void*_tmp41E;struct Cyc_Absyn_Vardecl*_tmp420;struct Cyc_Absyn_Exp*_tmp422;
struct _dyneither_ptr*_tmp423;struct Cyc_Absyn_Exp*_tmp425;struct Cyc_Absyn_Exp*
_tmp427;struct Cyc_Absyn_Exp*_tmp428;struct Cyc_Absyn_Exp*_tmp42A;struct
_dyneither_ptr*_tmp42B;_LL219: {struct Cyc_Absyn_Cast_e_struct*_tmp40B=(struct Cyc_Absyn_Cast_e_struct*)
_tmp40A;if(_tmp40B->tag != 15)goto _LL21B;else{_tmp40C=_tmp40B->f2;}}_LL21A:
_tmp40E=_tmp40C;goto _LL21C;_LL21B: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp40D=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp40A;if(_tmp40D->tag != 13)
goto _LL21D;else{_tmp40E=_tmp40D->f1;}}_LL21C: _tmp410=_tmp40E;goto _LL21E;_LL21D: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp40F=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp40A;if(_tmp40F->tag != 14)goto _LL21F;else{_tmp410=_tmp40F->f1;}}_LL21E: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp410,flds);_LL21F: {struct Cyc_Absyn_Var_e_struct*
_tmp411=(struct Cyc_Absyn_Var_e_struct*)_tmp40A;if(_tmp411->tag != 1)goto _LL221;
else{_tmp412=(void*)_tmp411->f2;{struct Cyc_Absyn_Param_b_struct*_tmp413=(struct
Cyc_Absyn_Param_b_struct*)_tmp412;if(_tmp413->tag != 3)goto _LL221;else{_tmp414=
_tmp413->f1;}};}}_LL220: _tmp418=_tmp414;goto _LL222;_LL221: {struct Cyc_Absyn_Var_e_struct*
_tmp415=(struct Cyc_Absyn_Var_e_struct*)_tmp40A;if(_tmp415->tag != 1)goto _LL223;
else{_tmp416=(void*)_tmp415->f2;{struct Cyc_Absyn_Local_b_struct*_tmp417=(struct
Cyc_Absyn_Local_b_struct*)_tmp416;if(_tmp417->tag != 4)goto _LL223;else{_tmp418=
_tmp417->f1;}};}}_LL222: _tmp41C=_tmp418;goto _LL224;_LL223: {struct Cyc_Absyn_Var_e_struct*
_tmp419=(struct Cyc_Absyn_Var_e_struct*)_tmp40A;if(_tmp419->tag != 1)goto _LL225;
else{_tmp41A=(void*)_tmp419->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp41B=(struct Cyc_Absyn_Pat_b_struct*)
_tmp41A;if(_tmp41B->tag != 5)goto _LL225;else{_tmp41C=_tmp41B->f1;}};}}_LL224:
return({struct _tuple15 _tmp42C;_tmp42C.f1=inflow;_tmp42C.f2=Cyc_CfFlowInfo_PlaceL(({
struct Cyc_CfFlowInfo_Place*_tmp42D=_region_malloc(env->r,sizeof(*_tmp42D));
_tmp42D->root=(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp42E=
_region_malloc(env->r,sizeof(*_tmp42E));_tmp42E[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp42F;_tmp42F.tag=0;_tmp42F.f1=_tmp41C;_tmp42F;});_tmp42E;});_tmp42D->fields=
flds;_tmp42D;}));_tmp42C;});_LL225: {struct Cyc_Absyn_Var_e_struct*_tmp41D=(
struct Cyc_Absyn_Var_e_struct*)_tmp40A;if(_tmp41D->tag != 1)goto _LL227;else{
_tmp41E=(void*)_tmp41D->f2;{struct Cyc_Absyn_Global_b_struct*_tmp41F=(struct Cyc_Absyn_Global_b_struct*)
_tmp41E;if(_tmp41F->tag != 1)goto _LL227;else{_tmp420=_tmp41F->f1;}};}}_LL226:
return({struct _tuple15 _tmp430;_tmp430.f1=inflow;_tmp430.f2=Cyc_CfFlowInfo_UnknownL();
_tmp430;});_LL227: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp421=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp40A;if(_tmp421->tag != 23)goto _LL229;else{_tmp422=_tmp421->f1;_tmp423=_tmp421->f2;}}
_LL228:{void*_tmp431=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp422->topt))->v);struct Cyc_Absyn_PtrInfo _tmp433;void*_tmp434;
_LL232: {struct Cyc_Absyn_PointerType_struct*_tmp432=(struct Cyc_Absyn_PointerType_struct*)
_tmp431;if(_tmp432->tag != 5)goto _LL234;else{_tmp433=_tmp432->f1;_tmp434=_tmp433.elt_typ;}}
_LL233: if(!Cyc_Absyn_is_nontagged_union_type(_tmp434))flds=({struct Cyc_List_List*
_tmp435=_region_malloc(env->r,sizeof(*_tmp435));_tmp435->hd=(void*)Cyc_CfFlowInfo_get_field_index(
_tmp434,_tmp423);_tmp435->tl=flds;_tmp435;});goto _LL231;_LL234:;_LL235:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp436=_cycalloc(sizeof(*
_tmp436));_tmp436[0]=({struct Cyc_Core_Impossible_struct _tmp437;_tmp437.tag=Cyc_Core_Impossible;
_tmp437.f1=({const char*_tmp438="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp438,
sizeof(char),25);});_tmp437;});_tmp436;}));_LL231:;}_tmp425=_tmp422;goto _LL22A;
_LL229: {struct Cyc_Absyn_Deref_e_struct*_tmp424=(struct Cyc_Absyn_Deref_e_struct*)
_tmp40A;if(_tmp424->tag != 21)goto _LL22B;else{_tmp425=_tmp424->f1;}}_LL22A: {
union Cyc_CfFlowInfo_FlowInfo _tmp43A;void*_tmp43B;struct _tuple13 _tmp439=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp425);_tmp43A=_tmp439.f1;_tmp43B=_tmp439.f2;_tmp43A=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp43A);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp425,_tmp43A,
_tmp43B,flds);}_LL22B: {struct Cyc_Absyn_Subscript_e_struct*_tmp426=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp40A;if(_tmp426->tag != 24)goto _LL22D;else{_tmp427=_tmp426->f1;_tmp428=_tmp426->f2;}}
_LL22C: {void*_tmp43C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp427->topt))->v);struct Cyc_Absyn_PtrInfo _tmp43F;struct Cyc_Absyn_PtrAtts
_tmp440;union Cyc_Absyn_Constraint*_tmp441;_LL237: {struct Cyc_Absyn_TupleType_struct*
_tmp43D=(struct Cyc_Absyn_TupleType_struct*)_tmp43C;if(_tmp43D->tag != 11)goto
_LL239;}_LL238: {unsigned int _tmp442=(Cyc_Evexp_eval_const_uint_exp(_tmp428)).f1;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp427,({struct Cyc_List_List*
_tmp443=_region_malloc(env->r,sizeof(*_tmp443));_tmp443->hd=(void*)((int)_tmp442);
_tmp443->tl=flds;_tmp443;}));}_LL239: {struct Cyc_Absyn_PointerType_struct*
_tmp43E=(struct Cyc_Absyn_PointerType_struct*)_tmp43C;if(_tmp43E->tag != 5)goto
_LL23B;else{_tmp43F=_tmp43E->f1;_tmp440=_tmp43F.ptr_atts;_tmp441=_tmp440.bounds;}}
_LL23A: {struct _RegionHandle*_tmp444=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp447;
struct Cyc_List_List*_tmp448;struct _tuple19 _tmp446=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp444,env,inflow,({struct Cyc_Absyn_Exp*_tmp445[2];_tmp445[1]=_tmp428;_tmp445[0]=
_tmp427;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
_tmp444,_tag_dyneither(_tmp445,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp447=
_tmp446.f1;_tmp448=_tmp446.f2;_tmp447=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp428->loc,_tmp447);{union Cyc_CfFlowInfo_FlowInfo _tmp449=_tmp447;{union Cyc_CfFlowInfo_FlowInfo
_tmp44A=_tmp447;struct _tuple12 _tmp44B;struct Cyc_Dict_Dict _tmp44C;struct Cyc_List_List*
_tmp44D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp44E;_LL23E: if((_tmp44A.ReachableFL).tag
!= 2)goto _LL240;_tmp44B=(struct _tuple12)(_tmp44A.ReachableFL).val;_tmp44C=
_tmp44B.f1;_tmp44D=_tmp44B.f2;_tmp44E=_tmp44B.f3;_LL23F: if(Cyc_CfFlowInfo_initlevel(
_tmp403,_tmp44C,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp448))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)({void*_tmp44F=0;Cyc_Tcutil_terr(
_tmp428->loc,({const char*_tmp450="expression may not be initialized";
_tag_dyneither(_tmp450,sizeof(char),34);}),_tag_dyneither(_tmp44F,sizeof(void*),
0));});{struct Cyc_List_List*_tmp451=Cyc_NewControlFlow_add_subscript_reln(
_tmp403->r,_tmp44D,_tmp427,_tmp428);if(_tmp44D != _tmp451)_tmp449=Cyc_CfFlowInfo_ReachableFL(
_tmp44C,_tmp451,_tmp44E);goto _LL23D;};_LL240:;_LL241: goto _LL23D;_LL23D:;}{union
Cyc_CfFlowInfo_FlowInfo _tmp453;union Cyc_CfFlowInfo_AbsLVal _tmp454;struct _tuple15
_tmp452=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp427,_tmp447,(void*)((
struct Cyc_List_List*)_check_null(_tmp448))->hd,flds);_tmp453=_tmp452.f1;_tmp454=
_tmp452.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp455=_tmp453;int _tmp456;_LL243: if((
_tmp455.BottomFL).tag != 1)goto _LL245;_tmp456=(int)(_tmp455.BottomFL).val;_LL244:
return({struct _tuple15 _tmp457;_tmp457.f1=_tmp453;_tmp457.f2=_tmp454;_tmp457;});
_LL245:;_LL246: return({struct _tuple15 _tmp458;_tmp458.f1=_tmp449;_tmp458.f2=
_tmp454;_tmp458;});_LL242:;};};};}_LL23B:;_LL23C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459[0]=({struct Cyc_Core_Impossible_struct
_tmp45A;_tmp45A.tag=Cyc_Core_Impossible;_tmp45A.f1=({const char*_tmp45B="anal_Lexp: Subscript -- bad type";
_tag_dyneither(_tmp45B,sizeof(char),33);});_tmp45A;});_tmp459;}));_LL236:;}
_LL22D: {struct Cyc_Absyn_AggrMember_e_struct*_tmp429=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp40A;if(_tmp429->tag != 22)goto _LL22F;else{_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;}}
_LL22E: if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp42A->topt))->v))return({struct _tuple15 _tmp45C;_tmp45C.f1=inflow;
_tmp45C.f2=Cyc_CfFlowInfo_UnknownL();_tmp45C;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp42A,({struct Cyc_List_List*_tmp45D=_region_malloc(env->r,sizeof(*
_tmp45D));_tmp45D->hd=(void*)Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp42A->topt))->v,_tmp42B);_tmp45D->tl=flds;_tmp45D;}));_LL22F:;
_LL230: return({struct _tuple15 _tmp45E;_tmp45E.f1=Cyc_CfFlowInfo_BottomFL();
_tmp45E.f2=Cyc_CfFlowInfo_UnknownL();_tmp45E;});_LL218:;};}static struct _tuple15
Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_ConsumeInfo _tmp460;struct
_tuple14 _tmp45F=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);_tmp460=
_tmp45F.f1;{union Cyc_CfFlowInfo_FlowInfo _tmp462;union Cyc_CfFlowInfo_AbsLVal
_tmp463;struct _tuple15 _tmp461=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,e,0);
_tmp462=_tmp461.f1;_tmp463=_tmp461.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp465;
struct _tuple14 _tmp464=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);
_tmp465=_tmp464.f1;if(_tmp460.may_consume != _tmp465.may_consume  || (_tmp460.consumed).t
!= (_tmp465.consumed).t)({void*_tmp466=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp467="oops: anal_Lexp modified consume lists";
_tag_dyneither(_tmp467,sizeof(char),39);}),_tag_dyneither(_tmp466,sizeof(void*),
0));});return({struct _tuple15 _tmp468;_tmp468.f1=_tmp462;_tmp468.f2=_tmp463;
_tmp468;});};};}static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp469=env->fenv;{void*_tmp46A=e->r;struct Cyc_Absyn_Exp*_tmp46C;struct Cyc_Absyn_Exp*
_tmp46D;struct Cyc_Absyn_Exp*_tmp46E;struct Cyc_Absyn_Exp*_tmp470;struct Cyc_Absyn_Exp*
_tmp471;struct Cyc_Absyn_Exp*_tmp473;struct Cyc_Absyn_Exp*_tmp474;struct Cyc_Absyn_Exp*
_tmp476;struct Cyc_Absyn_Exp*_tmp477;enum Cyc_Absyn_Primop _tmp479;struct Cyc_List_List*
_tmp47A;struct Cyc_List_List _tmp47B;struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_List_List*
_tmp47D;enum Cyc_Absyn_Primop _tmp47F;struct Cyc_List_List*_tmp480;_LL248: {struct
Cyc_Absyn_Conditional_e_struct*_tmp46B=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp46A;if(_tmp46B->tag != 6)goto _LL24A;else{_tmp46C=_tmp46B->f1;_tmp46D=_tmp46B->f2;
_tmp46E=_tmp46B->f3;}}_LL249: {union Cyc_CfFlowInfo_FlowInfo _tmp482;union Cyc_CfFlowInfo_FlowInfo
_tmp483;struct _tuple16 _tmp481=Cyc_NewControlFlow_anal_test(env,inflow,_tmp46C);
_tmp482=_tmp481.f1;_tmp483=_tmp481.f2;_tmp482=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp46C->loc,_tmp482);_tmp483=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp46C->loc,
_tmp483);{union Cyc_CfFlowInfo_FlowInfo _tmp485;union Cyc_CfFlowInfo_FlowInfo
_tmp486;struct _tuple16 _tmp484=Cyc_NewControlFlow_anal_test(env,_tmp482,_tmp46D);
_tmp485=_tmp484.f1;_tmp486=_tmp484.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp488;union
Cyc_CfFlowInfo_FlowInfo _tmp489;struct _tuple16 _tmp487=Cyc_NewControlFlow_anal_test(
env,_tmp483,_tmp46E);_tmp488=_tmp487.f1;_tmp489=_tmp487.f2;return({struct
_tuple16 _tmp48A;_tmp48A.f1=Cyc_CfFlowInfo_join_flow(_tmp469,env->all_changed,
_tmp485,_tmp488,1);_tmp48A.f2=Cyc_CfFlowInfo_join_flow(_tmp469,env->all_changed,
_tmp486,_tmp489,1);_tmp48A;});};};}_LL24A: {struct Cyc_Absyn_And_e_struct*_tmp46F=(
struct Cyc_Absyn_And_e_struct*)_tmp46A;if(_tmp46F->tag != 7)goto _LL24C;else{
_tmp470=_tmp46F->f1;_tmp471=_tmp46F->f2;}}_LL24B: {union Cyc_CfFlowInfo_FlowInfo
_tmp48C;union Cyc_CfFlowInfo_FlowInfo _tmp48D;struct _tuple16 _tmp48B=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp470);_tmp48C=_tmp48B.f1;_tmp48D=_tmp48B.f2;_tmp48C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp470->loc,_tmp48C);_tmp48D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp470->loc,
_tmp48D);{union Cyc_CfFlowInfo_FlowInfo _tmp48F;union Cyc_CfFlowInfo_FlowInfo
_tmp490;struct _tuple16 _tmp48E=Cyc_NewControlFlow_anal_test(env,_tmp48C,_tmp471);
_tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;_tmp48F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp471->loc,_tmp48F);_tmp490=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp471->loc,
_tmp490);return({struct _tuple16 _tmp491;_tmp491.f1=_tmp48F;_tmp491.f2=Cyc_CfFlowInfo_join_flow(
_tmp469,env->all_changed,_tmp48D,_tmp490,0);_tmp491;});};}_LL24C: {struct Cyc_Absyn_Or_e_struct*
_tmp472=(struct Cyc_Absyn_Or_e_struct*)_tmp46A;if(_tmp472->tag != 8)goto _LL24E;
else{_tmp473=_tmp472->f1;_tmp474=_tmp472->f2;}}_LL24D: {union Cyc_CfFlowInfo_FlowInfo
_tmp493;union Cyc_CfFlowInfo_FlowInfo _tmp494;struct _tuple16 _tmp492=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp473);_tmp493=_tmp492.f1;_tmp494=_tmp492.f2;_tmp493=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp473->loc,_tmp493);_tmp494=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp473->loc,
_tmp494);{union Cyc_CfFlowInfo_FlowInfo _tmp496;union Cyc_CfFlowInfo_FlowInfo
_tmp497;struct _tuple16 _tmp495=Cyc_NewControlFlow_anal_test(env,_tmp494,_tmp474);
_tmp496=_tmp495.f1;_tmp497=_tmp495.f2;_tmp496=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp474->loc,_tmp496);_tmp497=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp474->loc,
_tmp497);return({struct _tuple16 _tmp498;_tmp498.f1=Cyc_CfFlowInfo_join_flow(
_tmp469,env->all_changed,_tmp493,_tmp496,0);_tmp498.f2=_tmp497;_tmp498;});};}
_LL24E: {struct Cyc_Absyn_SeqExp_e_struct*_tmp475=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp46A;if(_tmp475->tag != 9)goto _LL250;else{_tmp476=_tmp475->f1;_tmp477=_tmp475->f2;}}
_LL24F: {union Cyc_CfFlowInfo_FlowInfo _tmp49A;void*_tmp49B;struct _tuple13 _tmp499=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp476);_tmp49A=_tmp499.f1;_tmp49B=
_tmp499.f2;_tmp49A=Cyc_CfFlowInfo_drop_unique_rvals(_tmp476->loc,_tmp49A);return
Cyc_NewControlFlow_anal_test(env,_tmp49A,_tmp477);}_LL250: {struct Cyc_Absyn_Primop_e_struct*
_tmp478=(struct Cyc_Absyn_Primop_e_struct*)_tmp46A;if(_tmp478->tag != 3)goto _LL252;
else{_tmp479=_tmp478->f1;if(_tmp479 != Cyc_Absyn_Not)goto _LL252;_tmp47A=_tmp478->f2;
if(_tmp47A == 0)goto _LL252;_tmp47B=*_tmp47A;_tmp47C=(struct Cyc_Absyn_Exp*)_tmp47B.hd;
_tmp47D=_tmp47B.tl;if(_tmp47D != 0)goto _LL252;}}_LL251: {union Cyc_CfFlowInfo_FlowInfo
_tmp49D;union Cyc_CfFlowInfo_FlowInfo _tmp49E;struct _tuple16 _tmp49C=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp47C);_tmp49D=_tmp49C.f1;_tmp49E=_tmp49C.f2;return({struct _tuple16
_tmp49F;_tmp49F.f1=_tmp49E;_tmp49F.f2=_tmp49D;_tmp49F;});}_LL252: {struct Cyc_Absyn_Primop_e_struct*
_tmp47E=(struct Cyc_Absyn_Primop_e_struct*)_tmp46A;if(_tmp47E->tag != 3)goto _LL254;
else{_tmp47F=_tmp47E->f1;_tmp480=_tmp47E->f2;}}_LL253: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp4A0=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp4A2;struct
Cyc_List_List*_tmp4A3;struct _tuple19 _tmp4A1=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp4A0,env,inflow,_tmp480,0);_tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp4A3))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp4A3->tl))->hd;f=_tmp4A2;}{union Cyc_CfFlowInfo_FlowInfo _tmp4A4=f;
int _tmp4A5;struct _tuple12 _tmp4A6;struct Cyc_Dict_Dict _tmp4A7;struct Cyc_List_List*
_tmp4A8;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4A9;_LL257: if((_tmp4A4.BottomFL).tag
!= 1)goto _LL259;_tmp4A5=(int)(_tmp4A4.BottomFL).val;_LL258: return({struct
_tuple16 _tmp4AA;_tmp4AA.f1=f;_tmp4AA.f2=f;_tmp4AA;});_LL259: if((_tmp4A4.ReachableFL).tag
!= 2)goto _LL256;_tmp4A6=(struct _tuple12)(_tmp4A4.ReachableFL).val;_tmp4A7=
_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;_tmp4A9=_tmp4A6.f3;_LL25A: {struct Cyc_Absyn_Exp*
_tmp4AB=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp480))->hd;
struct Cyc_Absyn_Exp*_tmp4AC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp480->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4A7,r1)
== Cyc_CfFlowInfo_NoneIL)({void*_tmp4AD=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmp480->hd)->loc,({const char*_tmp4AE="expression may not be initialized";
_tag_dyneither(_tmp4AE,sizeof(char),34);}),_tag_dyneither(_tmp4AD,sizeof(void*),
0));});if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4A7,r2)== Cyc_CfFlowInfo_NoneIL)({
void*_tmp4AF=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp480->tl))->hd)->loc,({const char*_tmp4B0="expression may not be initialized";
_tag_dyneither(_tmp4B0,sizeof(char),34);}),_tag_dyneither(_tmp4AF,sizeof(void*),
0));});if(_tmp47F == Cyc_Absyn_Eq  || _tmp47F == Cyc_Absyn_Neq){struct _tuple0
_tmp4B2=({struct _tuple0 _tmp4B1;_tmp4B1.f1=r1;_tmp4B1.f2=r2;_tmp4B1;});void*
_tmp4B3;enum Cyc_CfFlowInfo_InitLevel _tmp4B5;void*_tmp4B6;void*_tmp4B8;void*
_tmp4BA;enum Cyc_CfFlowInfo_InitLevel _tmp4BC;void*_tmp4BD;void*_tmp4BF;void*
_tmp4C1;void*_tmp4C3;void*_tmp4C5;void*_tmp4C7;void*_tmp4C9;void*_tmp4CB;void*
_tmp4CD;void*_tmp4CF;void*_tmp4D1;void*_tmp4D3;void*_tmp4D5;void*_tmp4D7;_LL25C:
_tmp4B3=_tmp4B2.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4B4=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4B3;if(_tmp4B4->tag != 3)goto _LL25E;else{_tmp4B5=_tmp4B4->f1;}};_tmp4B6=
_tmp4B2.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4B7=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4B6;if(_tmp4B7->tag != 0)goto _LL25E;};_LL25D: {union Cyc_CfFlowInfo_FlowInfo
_tmp4DA;union Cyc_CfFlowInfo_FlowInfo _tmp4DB;struct _tuple16 _tmp4D9=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4AB,_tmp4B5);_tmp4DA=_tmp4D9.f1;_tmp4DB=_tmp4D9.f2;switch(_tmp47F){
case Cyc_Absyn_Eq: _LL270: return({struct _tuple16 _tmp4DC;_tmp4DC.f1=_tmp4DB;_tmp4DC.f2=
_tmp4DA;_tmp4DC;});case Cyc_Absyn_Neq: _LL271: return({struct _tuple16 _tmp4DD;
_tmp4DD.f1=_tmp4DA;_tmp4DD.f2=_tmp4DB;_tmp4DD;});default: _LL272:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));
_tmp4DE[0]=({struct Cyc_Core_Impossible_struct _tmp4DF;_tmp4DF.tag=Cyc_Core_Impossible;
_tmp4DF.f1=({const char*_tmp4E0="anal_test, zero-split";_tag_dyneither(_tmp4E0,
sizeof(char),22);});_tmp4DF;});_tmp4DE;}));}}_LL25E: _tmp4B8=_tmp4B2.f1;{struct
Cyc_CfFlowInfo_Zero_struct*_tmp4B9=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4B8;
if(_tmp4B9->tag != 0)goto _LL260;};_tmp4BA=_tmp4B2.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4BB=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4BA;if(_tmp4BB->tag != 3)goto
_LL260;else{_tmp4BC=_tmp4BB->f1;}};_LL25F: {union Cyc_CfFlowInfo_FlowInfo _tmp4E2;
union Cyc_CfFlowInfo_FlowInfo _tmp4E3;struct _tuple16 _tmp4E1=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4AC,_tmp4BC);_tmp4E2=_tmp4E1.f1;_tmp4E3=_tmp4E1.f2;switch(_tmp47F){
case Cyc_Absyn_Eq: _LL274: return({struct _tuple16 _tmp4E4;_tmp4E4.f1=_tmp4E3;_tmp4E4.f2=
_tmp4E2;_tmp4E4;});case Cyc_Absyn_Neq: _LL275: return({struct _tuple16 _tmp4E5;
_tmp4E5.f1=_tmp4E2;_tmp4E5.f2=_tmp4E3;_tmp4E5;});default: _LL276:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));
_tmp4E6[0]=({struct Cyc_Core_Impossible_struct _tmp4E7;_tmp4E7.tag=Cyc_Core_Impossible;
_tmp4E7.f1=({const char*_tmp4E8="anal_test, zero-split";_tag_dyneither(_tmp4E8,
sizeof(char),22);});_tmp4E7;});_tmp4E6;}));}}_LL260: _tmp4BD=_tmp4B2.f1;{struct
Cyc_CfFlowInfo_Zero_struct*_tmp4BE=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4BD;
if(_tmp4BE->tag != 0)goto _LL262;};_tmp4BF=_tmp4B2.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4C0=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4BF;if(_tmp4C0->tag != 0)goto
_LL262;};_LL261: if(_tmp47F == Cyc_Absyn_Eq)return({struct _tuple16 _tmp4E9;_tmp4E9.f1=
f;_tmp4E9.f2=Cyc_CfFlowInfo_BottomFL();_tmp4E9;});else{return({struct _tuple16
_tmp4EA;_tmp4EA.f1=Cyc_CfFlowInfo_BottomFL();_tmp4EA.f2=f;_tmp4EA;});}_LL262:
_tmp4C1=_tmp4B2.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4C2=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4C1;if(_tmp4C2->tag != 0)goto _LL264;};_tmp4C3=_tmp4B2.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4C4=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4C3;if(_tmp4C4->tag != 1)
goto _LL264;};_LL263: goto _LL265;_LL264: _tmp4C5=_tmp4B2.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4C6=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4C5;if(_tmp4C6->tag != 0)goto
_LL266;};_tmp4C7=_tmp4B2.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4C8=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4C7;if(_tmp4C8->tag != 2)goto _LL266;};
_LL265: goto _LL267;_LL266: _tmp4C9=_tmp4B2.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4CA=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4C9;if(_tmp4CA->tag != 0)goto
_LL268;};_tmp4CB=_tmp4B2.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4CC=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4CB;if(_tmp4CC->tag != 5)goto _LL268;};
_LL267: goto _LL269;_LL268: _tmp4CD=_tmp4B2.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4CE=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4CD;if(_tmp4CE->tag != 1)
goto _LL26A;};_tmp4CF=_tmp4B2.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D0=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4CF;if(_tmp4D0->tag != 0)goto _LL26A;};
_LL269: goto _LL26B;_LL26A: _tmp4D1=_tmp4B2.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4D2=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4D1;if(_tmp4D2->tag != 2)
goto _LL26C;};_tmp4D3=_tmp4B2.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D4=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4D3;if(_tmp4D4->tag != 0)goto _LL26C;};
_LL26B: goto _LL26D;_LL26C: _tmp4D5=_tmp4B2.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4D6=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4D5;if(_tmp4D6->tag != 5)goto
_LL26E;};_tmp4D7=_tmp4B2.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D8=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4D7;if(_tmp4D8->tag != 0)goto _LL26E;};_LL26D: if(_tmp47F == Cyc_Absyn_Neq)
return({struct _tuple16 _tmp4EB;_tmp4EB.f1=f;_tmp4EB.f2=Cyc_CfFlowInfo_BottomFL();
_tmp4EB;});else{return({struct _tuple16 _tmp4EC;_tmp4EC.f1=Cyc_CfFlowInfo_BottomFL();
_tmp4EC.f2=f;_tmp4EC;});}_LL26E:;_LL26F: goto _LL25B;_LL25B:;}{struct _tuple0
_tmp4EE=({struct _tuple0 _tmp4ED;_tmp4ED.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4AB->topt))->v);_tmp4ED.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4AC->topt))->v);_tmp4ED;});void*_tmp4EF;enum Cyc_Absyn_Sign
_tmp4F1;void*_tmp4F2;enum Cyc_Absyn_Sign _tmp4F4;void*_tmp4F5;void*_tmp4F7;_LL279:
_tmp4EF=_tmp4EE.f1;{struct Cyc_Absyn_IntType_struct*_tmp4F0=(struct Cyc_Absyn_IntType_struct*)
_tmp4EF;if(_tmp4F0->tag != 6)goto _LL27B;else{_tmp4F1=_tmp4F0->f1;if(_tmp4F1 != Cyc_Absyn_Unsigned)
goto _LL27B;}};_LL27A: goto _LL27C;_LL27B: _tmp4F2=_tmp4EE.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4F3=(struct Cyc_Absyn_IntType_struct*)_tmp4F2;if(_tmp4F3->tag != 6)goto _LL27D;
else{_tmp4F4=_tmp4F3->f1;if(_tmp4F4 != Cyc_Absyn_Unsigned)goto _LL27D;}};_LL27C:
goto _LL27E;_LL27D: _tmp4F5=_tmp4EE.f1;{struct Cyc_Absyn_TagType_struct*_tmp4F6=(
struct Cyc_Absyn_TagType_struct*)_tmp4F5;if(_tmp4F6->tag != 20)goto _LL27F;};_LL27E:
goto _LL280;_LL27F: _tmp4F7=_tmp4EE.f2;{struct Cyc_Absyn_TagType_struct*_tmp4F8=(
struct Cyc_Absyn_TagType_struct*)_tmp4F7;if(_tmp4F8->tag != 20)goto _LL281;};_LL280:
goto _LL278;_LL281:;_LL282: return({struct _tuple16 _tmp4F9;_tmp4F9.f1=f;_tmp4F9.f2=
f;_tmp4F9;});_LL278:;}switch(_tmp47F){case Cyc_Absyn_Eq: _LL283: {union Cyc_CfFlowInfo_FlowInfo
_tmp4FA=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AB,r1,Cyc_Absyn_Eq,r2);_tmp4FA=
Cyc_NewControlFlow_if_tagcmp(env,_tmp4FA,_tmp4AC,r2,Cyc_Absyn_Eq,r1);return({
struct _tuple16 _tmp4FB;_tmp4FB.f1=_tmp4FA;_tmp4FB.f2=f;_tmp4FB;});}case Cyc_Absyn_Neq:
_LL284: {union Cyc_CfFlowInfo_FlowInfo _tmp4FC=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp4AB,r1,Cyc_Absyn_Eq,r2);_tmp4FC=Cyc_NewControlFlow_if_tagcmp(env,_tmp4FC,
_tmp4AC,r2,Cyc_Absyn_Eq,r1);return({struct _tuple16 _tmp4FD;_tmp4FD.f1=f;_tmp4FD.f2=
_tmp4FC;_tmp4FD;});}case Cyc_Absyn_Gt: _LL285: {union Cyc_CfFlowInfo_FlowInfo
_tmp4FE=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AC,r2,Cyc_Absyn_Lt,r1);union Cyc_CfFlowInfo_FlowInfo
_tmp4FF=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AB,r1,Cyc_Absyn_Lte,r2);return({
struct _tuple16 _tmp500;_tmp500.f1=_tmp4FE;_tmp500.f2=_tmp4FF;_tmp500;});}case Cyc_Absyn_Gte:
_LL286: {union Cyc_CfFlowInfo_FlowInfo _tmp501=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp4AC,r2,Cyc_Absyn_Lte,r1);union Cyc_CfFlowInfo_FlowInfo _tmp502=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp4AB,r1,Cyc_Absyn_Lt,r2);return({struct _tuple16 _tmp503;_tmp503.f1=
_tmp501;_tmp503.f2=_tmp502;_tmp503;});}case Cyc_Absyn_Lt: _LL287: {union Cyc_CfFlowInfo_FlowInfo
_tmp504=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AB,r1,Cyc_Absyn_Lt,r2);union Cyc_CfFlowInfo_FlowInfo
_tmp505=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AC,r2,Cyc_Absyn_Lte,r1);{union
Cyc_CfFlowInfo_FlowInfo _tmp506=_tmp504;int _tmp507;struct _tuple12 _tmp508;struct
Cyc_Dict_Dict _tmp509;struct Cyc_CfFlowInfo_ConsumeInfo _tmp50A;_LL28A: if((_tmp506.BottomFL).tag
!= 1)goto _LL28C;_tmp507=(int)(_tmp506.BottomFL).val;_LL28B:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B[0]=({
struct Cyc_Core_Impossible_struct _tmp50C;_tmp50C.tag=Cyc_Core_Impossible;_tmp50C.f1=({
const char*_tmp50D="anal_test, Lt";_tag_dyneither(_tmp50D,sizeof(char),14);});
_tmp50C;});_tmp50B;}));_LL28C: if((_tmp506.ReachableFL).tag != 2)goto _LL289;
_tmp508=(struct _tuple12)(_tmp506.ReachableFL).val;_tmp509=_tmp508.f1;_tmp50A=
_tmp508.f3;_LL28D: _tmp4A7=_tmp509;_tmp4A9=_tmp50A;_LL289:;}{void*_tmp50E=_tmp4AB->r;
void*_tmp510;struct Cyc_Absyn_Vardecl*_tmp512;void*_tmp514;struct Cyc_Absyn_Vardecl*
_tmp516;void*_tmp518;struct Cyc_Absyn_Vardecl*_tmp51A;void*_tmp51C;struct Cyc_Absyn_Vardecl*
_tmp51E;_LL28F:{struct Cyc_Absyn_Var_e_struct*_tmp50F=(struct Cyc_Absyn_Var_e_struct*)
_tmp50E;if(_tmp50F->tag != 1)goto _LL291;else{_tmp510=(void*)_tmp50F->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp511=(struct Cyc_Absyn_Global_b_struct*)_tmp510;if(
_tmp511->tag != 1)goto _LL291;else{_tmp512=_tmp511->f1;}};}}if(!(!_tmp512->escapes))
goto _LL291;_LL290: _tmp516=_tmp512;goto _LL292;_LL291:{struct Cyc_Absyn_Var_e_struct*
_tmp513=(struct Cyc_Absyn_Var_e_struct*)_tmp50E;if(_tmp513->tag != 1)goto _LL293;
else{_tmp514=(void*)_tmp513->f2;{struct Cyc_Absyn_Local_b_struct*_tmp515=(struct
Cyc_Absyn_Local_b_struct*)_tmp514;if(_tmp515->tag != 4)goto _LL293;else{_tmp516=
_tmp515->f1;}};}}if(!(!_tmp516->escapes))goto _LL293;_LL292: _tmp51A=_tmp516;goto
_LL294;_LL293:{struct Cyc_Absyn_Var_e_struct*_tmp517=(struct Cyc_Absyn_Var_e_struct*)
_tmp50E;if(_tmp517->tag != 1)goto _LL295;else{_tmp518=(void*)_tmp517->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp519=(struct Cyc_Absyn_Pat_b_struct*)_tmp518;if(_tmp519->tag
!= 5)goto _LL295;else{_tmp51A=_tmp519->f1;}};}}if(!(!_tmp51A->escapes))goto _LL295;
_LL294: _tmp51E=_tmp51A;goto _LL296;_LL295:{struct Cyc_Absyn_Var_e_struct*_tmp51B=(
struct Cyc_Absyn_Var_e_struct*)_tmp50E;if(_tmp51B->tag != 1)goto _LL297;else{
_tmp51C=(void*)_tmp51B->f2;{struct Cyc_Absyn_Param_b_struct*_tmp51D=(struct Cyc_Absyn_Param_b_struct*)
_tmp51C;if(_tmp51D->tag != 3)goto _LL297;else{_tmp51E=_tmp51D->f1;}};}}if(!(!
_tmp51E->escapes))goto _LL297;_LL296: {void*_tmp51F=_tmp4AC->r;void*_tmp521;
struct Cyc_Absyn_Vardecl*_tmp523;void*_tmp525;struct Cyc_Absyn_Vardecl*_tmp527;
void*_tmp529;struct Cyc_Absyn_Vardecl*_tmp52B;void*_tmp52D;struct Cyc_Absyn_Vardecl*
_tmp52F;union Cyc_Absyn_Cnst _tmp531;struct _tuple6 _tmp532;int _tmp533;enum Cyc_Absyn_Primop
_tmp535;struct Cyc_List_List*_tmp536;struct Cyc_List_List _tmp537;struct Cyc_Absyn_Exp*
_tmp538;_LL29A:{struct Cyc_Absyn_Var_e_struct*_tmp520=(struct Cyc_Absyn_Var_e_struct*)
_tmp51F;if(_tmp520->tag != 1)goto _LL29C;else{_tmp521=(void*)_tmp520->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp522=(struct Cyc_Absyn_Global_b_struct*)_tmp521;if(
_tmp522->tag != 1)goto _LL29C;else{_tmp523=_tmp522->f1;}};}}if(!(!_tmp523->escapes))
goto _LL29C;_LL29B: _tmp527=_tmp523;goto _LL29D;_LL29C:{struct Cyc_Absyn_Var_e_struct*
_tmp524=(struct Cyc_Absyn_Var_e_struct*)_tmp51F;if(_tmp524->tag != 1)goto _LL29E;
else{_tmp525=(void*)_tmp524->f2;{struct Cyc_Absyn_Local_b_struct*_tmp526=(struct
Cyc_Absyn_Local_b_struct*)_tmp525;if(_tmp526->tag != 4)goto _LL29E;else{_tmp527=
_tmp526->f1;}};}}if(!(!_tmp527->escapes))goto _LL29E;_LL29D: _tmp52B=_tmp527;goto
_LL29F;_LL29E:{struct Cyc_Absyn_Var_e_struct*_tmp528=(struct Cyc_Absyn_Var_e_struct*)
_tmp51F;if(_tmp528->tag != 1)goto _LL2A0;else{_tmp529=(void*)_tmp528->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp52A=(struct Cyc_Absyn_Pat_b_struct*)_tmp529;if(_tmp52A->tag
!= 5)goto _LL2A0;else{_tmp52B=_tmp52A->f1;}};}}if(!(!_tmp52B->escapes))goto _LL2A0;
_LL29F: _tmp52F=_tmp52B;goto _LL2A1;_LL2A0:{struct Cyc_Absyn_Var_e_struct*_tmp52C=(
struct Cyc_Absyn_Var_e_struct*)_tmp51F;if(_tmp52C->tag != 1)goto _LL2A2;else{
_tmp52D=(void*)_tmp52C->f2;{struct Cyc_Absyn_Param_b_struct*_tmp52E=(struct Cyc_Absyn_Param_b_struct*)
_tmp52D;if(_tmp52E->tag != 3)goto _LL2A2;else{_tmp52F=_tmp52E->f1;}};}}if(!(!
_tmp52F->escapes))goto _LL2A2;_LL2A1: {struct _RegionHandle*_tmp539=(env->fenv)->r;
struct Cyc_List_List*_tmp53A=({struct Cyc_List_List*_tmp53C=_region_malloc(_tmp539,
sizeof(*_tmp53C));_tmp53C->hd=({struct Cyc_CfFlowInfo_Reln*_tmp53D=_region_malloc(
_tmp539,sizeof(*_tmp53D));_tmp53D->vd=_tmp51E;_tmp53D->rop=Cyc_CfFlowInfo_LessVar(
_tmp52F,_tmp52F->type);_tmp53D;});_tmp53C->tl=_tmp4A8;_tmp53C;});return({struct
_tuple16 _tmp53B;_tmp53B.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4A7,_tmp53A,_tmp4A9);
_tmp53B.f2=_tmp505;_tmp53B;});}_LL2A2: {struct Cyc_Absyn_Const_e_struct*_tmp530=(
struct Cyc_Absyn_Const_e_struct*)_tmp51F;if(_tmp530->tag != 0)goto _LL2A4;else{
_tmp531=_tmp530->f1;if((_tmp531.Int_c).tag != 4)goto _LL2A4;_tmp532=(struct _tuple6)(
_tmp531.Int_c).val;_tmp533=_tmp532.f2;}}_LL2A3: {struct _RegionHandle*_tmp53E=(
env->fenv)->r;struct Cyc_List_List*_tmp53F=({struct Cyc_List_List*_tmp541=
_region_malloc(_tmp53E,sizeof(*_tmp541));_tmp541->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp542=_region_malloc(_tmp53E,sizeof(*_tmp542));_tmp542->vd=_tmp51E;_tmp542->rop=
Cyc_CfFlowInfo_LessConst((unsigned int)_tmp533);_tmp542;});_tmp541->tl=_tmp4A8;
_tmp541;});return({struct _tuple16 _tmp540;_tmp540.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4A7,_tmp53F,_tmp4A9);_tmp540.f2=_tmp505;_tmp540;});}_LL2A4: {struct Cyc_Absyn_Primop_e_struct*
_tmp534=(struct Cyc_Absyn_Primop_e_struct*)_tmp51F;if(_tmp534->tag != 3)goto _LL2A6;
else{_tmp535=_tmp534->f1;_tmp536=_tmp534->f2;if(_tmp536 == 0)goto _LL2A6;_tmp537=*
_tmp536;_tmp538=(struct Cyc_Absyn_Exp*)_tmp537.hd;}}_LL2A5: {void*_tmp543=_tmp538->r;
void*_tmp545;struct Cyc_Absyn_Vardecl*_tmp547;void*_tmp549;struct Cyc_Absyn_Vardecl*
_tmp54B;void*_tmp54D;struct Cyc_Absyn_Vardecl*_tmp54F;void*_tmp551;struct Cyc_Absyn_Vardecl*
_tmp553;_LL2A9:{struct Cyc_Absyn_Var_e_struct*_tmp544=(struct Cyc_Absyn_Var_e_struct*)
_tmp543;if(_tmp544->tag != 1)goto _LL2AB;else{_tmp545=(void*)_tmp544->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp546=(struct Cyc_Absyn_Global_b_struct*)_tmp545;if(
_tmp546->tag != 1)goto _LL2AB;else{_tmp547=_tmp546->f1;}};}}if(!(!_tmp547->escapes))
goto _LL2AB;_LL2AA: _tmp54B=_tmp547;goto _LL2AC;_LL2AB:{struct Cyc_Absyn_Var_e_struct*
_tmp548=(struct Cyc_Absyn_Var_e_struct*)_tmp543;if(_tmp548->tag != 1)goto _LL2AD;
else{_tmp549=(void*)_tmp548->f2;{struct Cyc_Absyn_Local_b_struct*_tmp54A=(struct
Cyc_Absyn_Local_b_struct*)_tmp549;if(_tmp54A->tag != 4)goto _LL2AD;else{_tmp54B=
_tmp54A->f1;}};}}if(!(!_tmp54B->escapes))goto _LL2AD;_LL2AC: _tmp54F=_tmp54B;goto
_LL2AE;_LL2AD:{struct Cyc_Absyn_Var_e_struct*_tmp54C=(struct Cyc_Absyn_Var_e_struct*)
_tmp543;if(_tmp54C->tag != 1)goto _LL2AF;else{_tmp54D=(void*)_tmp54C->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp54E=(struct Cyc_Absyn_Pat_b_struct*)_tmp54D;if(_tmp54E->tag
!= 5)goto _LL2AF;else{_tmp54F=_tmp54E->f1;}};}}if(!(!_tmp54F->escapes))goto _LL2AF;
_LL2AE: _tmp553=_tmp54F;goto _LL2B0;_LL2AF:{struct Cyc_Absyn_Var_e_struct*_tmp550=(
struct Cyc_Absyn_Var_e_struct*)_tmp543;if(_tmp550->tag != 1)goto _LL2B1;else{
_tmp551=(void*)_tmp550->f2;{struct Cyc_Absyn_Param_b_struct*_tmp552=(struct Cyc_Absyn_Param_b_struct*)
_tmp551;if(_tmp552->tag != 3)goto _LL2B1;else{_tmp553=_tmp552->f1;}};}}if(!(!
_tmp553->escapes))goto _LL2B1;_LL2B0: {struct _RegionHandle*_tmp554=(env->fenv)->r;
struct Cyc_List_List*_tmp555=({struct Cyc_List_List*_tmp557=_region_malloc(_tmp554,
sizeof(*_tmp557));_tmp557->hd=({struct Cyc_CfFlowInfo_Reln*_tmp558=_region_malloc(
_tmp554,sizeof(*_tmp558));_tmp558->vd=_tmp51E;_tmp558->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp553);_tmp558;});_tmp557->tl=_tmp4A8;_tmp557;});return({struct _tuple16 _tmp556;
_tmp556.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4A7,_tmp555,_tmp4A9);_tmp556.f2=
_tmp505;_tmp556;});}_LL2B1:;_LL2B2: return({struct _tuple16 _tmp559;_tmp559.f1=
_tmp504;_tmp559.f2=_tmp505;_tmp559;});_LL2A8:;}_LL2A6:;_LL2A7: return({struct
_tuple16 _tmp55A;_tmp55A.f1=_tmp504;_tmp55A.f2=_tmp505;_tmp55A;});_LL299:;}_LL297:;
_LL298: return({struct _tuple16 _tmp55B;_tmp55B.f1=_tmp504;_tmp55B.f2=_tmp505;
_tmp55B;});_LL28E:;};}case Cyc_Absyn_Lte: _LL288: {union Cyc_CfFlowInfo_FlowInfo
_tmp55C=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AB,r1,Cyc_Absyn_Lte,r2);union Cyc_CfFlowInfo_FlowInfo
_tmp55D=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AC,r1,Cyc_Absyn_Lt,r1);{union Cyc_CfFlowInfo_FlowInfo
_tmp55E=_tmp55C;int _tmp55F;struct _tuple12 _tmp560;struct Cyc_Dict_Dict _tmp561;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp562;_LL2B5: if((_tmp55E.BottomFL).tag != 1)
goto _LL2B7;_tmp55F=(int)(_tmp55E.BottomFL).val;_LL2B6:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563[0]=({
struct Cyc_Core_Impossible_struct _tmp564;_tmp564.tag=Cyc_Core_Impossible;_tmp564.f1=({
const char*_tmp565="anal_test, Lte";_tag_dyneither(_tmp565,sizeof(char),15);});
_tmp564;});_tmp563;}));_LL2B7: if((_tmp55E.ReachableFL).tag != 2)goto _LL2B4;
_tmp560=(struct _tuple12)(_tmp55E.ReachableFL).val;_tmp561=_tmp560.f1;_tmp562=
_tmp560.f3;_LL2B8: _tmp4A7=_tmp561;_tmp4A9=_tmp562;_LL2B4:;}{void*_tmp566=_tmp4AB->r;
void*_tmp568;struct Cyc_Absyn_Vardecl*_tmp56A;void*_tmp56C;struct Cyc_Absyn_Vardecl*
_tmp56E;void*_tmp570;struct Cyc_Absyn_Vardecl*_tmp572;void*_tmp574;struct Cyc_Absyn_Vardecl*
_tmp576;_LL2BA:{struct Cyc_Absyn_Var_e_struct*_tmp567=(struct Cyc_Absyn_Var_e_struct*)
_tmp566;if(_tmp567->tag != 1)goto _LL2BC;else{_tmp568=(void*)_tmp567->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp569=(struct Cyc_Absyn_Global_b_struct*)_tmp568;if(
_tmp569->tag != 1)goto _LL2BC;else{_tmp56A=_tmp569->f1;}};}}if(!(!_tmp56A->escapes))
goto _LL2BC;_LL2BB: _tmp56E=_tmp56A;goto _LL2BD;_LL2BC:{struct Cyc_Absyn_Var_e_struct*
_tmp56B=(struct Cyc_Absyn_Var_e_struct*)_tmp566;if(_tmp56B->tag != 1)goto _LL2BE;
else{_tmp56C=(void*)_tmp56B->f2;{struct Cyc_Absyn_Local_b_struct*_tmp56D=(struct
Cyc_Absyn_Local_b_struct*)_tmp56C;if(_tmp56D->tag != 4)goto _LL2BE;else{_tmp56E=
_tmp56D->f1;}};}}if(!(!_tmp56E->escapes))goto _LL2BE;_LL2BD: _tmp572=_tmp56E;goto
_LL2BF;_LL2BE:{struct Cyc_Absyn_Var_e_struct*_tmp56F=(struct Cyc_Absyn_Var_e_struct*)
_tmp566;if(_tmp56F->tag != 1)goto _LL2C0;else{_tmp570=(void*)_tmp56F->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp571=(struct Cyc_Absyn_Pat_b_struct*)_tmp570;if(_tmp571->tag
!= 5)goto _LL2C0;else{_tmp572=_tmp571->f1;}};}}if(!(!_tmp572->escapes))goto _LL2C0;
_LL2BF: _tmp576=_tmp572;goto _LL2C1;_LL2C0:{struct Cyc_Absyn_Var_e_struct*_tmp573=(
struct Cyc_Absyn_Var_e_struct*)_tmp566;if(_tmp573->tag != 1)goto _LL2C2;else{
_tmp574=(void*)_tmp573->f2;{struct Cyc_Absyn_Param_b_struct*_tmp575=(struct Cyc_Absyn_Param_b_struct*)
_tmp574;if(_tmp575->tag != 3)goto _LL2C2;else{_tmp576=_tmp575->f1;}};}}if(!(!
_tmp576->escapes))goto _LL2C2;_LL2C1: {void*_tmp577=_tmp4AC->r;enum Cyc_Absyn_Primop
_tmp579;struct Cyc_List_List*_tmp57A;struct Cyc_List_List _tmp57B;struct Cyc_Absyn_Exp*
_tmp57C;_LL2C5: {struct Cyc_Absyn_Primop_e_struct*_tmp578=(struct Cyc_Absyn_Primop_e_struct*)
_tmp577;if(_tmp578->tag != 3)goto _LL2C7;else{_tmp579=_tmp578->f1;_tmp57A=_tmp578->f2;
if(_tmp57A == 0)goto _LL2C7;_tmp57B=*_tmp57A;_tmp57C=(struct Cyc_Absyn_Exp*)_tmp57B.hd;}}
_LL2C6: {void*_tmp57D=_tmp57C->r;void*_tmp57F;struct Cyc_Absyn_Vardecl*_tmp581;
void*_tmp583;struct Cyc_Absyn_Vardecl*_tmp585;void*_tmp587;struct Cyc_Absyn_Vardecl*
_tmp589;void*_tmp58B;struct Cyc_Absyn_Vardecl*_tmp58D;_LL2CA:{struct Cyc_Absyn_Var_e_struct*
_tmp57E=(struct Cyc_Absyn_Var_e_struct*)_tmp57D;if(_tmp57E->tag != 1)goto _LL2CC;
else{_tmp57F=(void*)_tmp57E->f2;{struct Cyc_Absyn_Global_b_struct*_tmp580=(struct
Cyc_Absyn_Global_b_struct*)_tmp57F;if(_tmp580->tag != 1)goto _LL2CC;else{_tmp581=
_tmp580->f1;}};}}if(!(!_tmp581->escapes))goto _LL2CC;_LL2CB: _tmp585=_tmp581;goto
_LL2CD;_LL2CC:{struct Cyc_Absyn_Var_e_struct*_tmp582=(struct Cyc_Absyn_Var_e_struct*)
_tmp57D;if(_tmp582->tag != 1)goto _LL2CE;else{_tmp583=(void*)_tmp582->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp584=(struct Cyc_Absyn_Local_b_struct*)_tmp583;if(
_tmp584->tag != 4)goto _LL2CE;else{_tmp585=_tmp584->f1;}};}}if(!(!_tmp585->escapes))
goto _LL2CE;_LL2CD: _tmp589=_tmp585;goto _LL2CF;_LL2CE:{struct Cyc_Absyn_Var_e_struct*
_tmp586=(struct Cyc_Absyn_Var_e_struct*)_tmp57D;if(_tmp586->tag != 1)goto _LL2D0;
else{_tmp587=(void*)_tmp586->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp588=(struct Cyc_Absyn_Pat_b_struct*)
_tmp587;if(_tmp588->tag != 5)goto _LL2D0;else{_tmp589=_tmp588->f1;}};}}if(!(!
_tmp589->escapes))goto _LL2D0;_LL2CF: _tmp58D=_tmp589;goto _LL2D1;_LL2D0:{struct Cyc_Absyn_Var_e_struct*
_tmp58A=(struct Cyc_Absyn_Var_e_struct*)_tmp57D;if(_tmp58A->tag != 1)goto _LL2D2;
else{_tmp58B=(void*)_tmp58A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp58C=(struct
Cyc_Absyn_Param_b_struct*)_tmp58B;if(_tmp58C->tag != 3)goto _LL2D2;else{_tmp58D=
_tmp58C->f1;}};}}if(!(!_tmp58D->escapes))goto _LL2D2;_LL2D1: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp58E=env->fenv;struct Cyc_List_List*_tmp58F=({struct Cyc_List_List*_tmp591=
_region_malloc(_tmp58E->r,sizeof(*_tmp591));_tmp591->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp592=_region_malloc(_tmp58E->r,sizeof(*_tmp592));_tmp592->vd=_tmp576;_tmp592->rop=
Cyc_CfFlowInfo_LessEqNumelts(_tmp58D);_tmp592;});_tmp591->tl=_tmp4A8;_tmp591;});
return({struct _tuple16 _tmp590;_tmp590.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4A7,
_tmp58F,_tmp4A9);_tmp590.f2=_tmp55D;_tmp590;});}_LL2D2:;_LL2D3: return({struct
_tuple16 _tmp593;_tmp593.f1=_tmp55C;_tmp593.f2=_tmp55D;_tmp593;});_LL2C9:;}_LL2C7:;
_LL2C8: return({struct _tuple16 _tmp594;_tmp594.f1=_tmp55C;_tmp594.f2=_tmp55D;
_tmp594;});_LL2C4:;}_LL2C2:;_LL2C3: return({struct _tuple16 _tmp595;_tmp595.f1=
_tmp55C;_tmp595.f2=_tmp55D;_tmp595;});_LL2B9:;};}default: _LL2B3: return({struct
_tuple16 _tmp596;_tmp596.f1=f;_tmp596.f2=f;_tmp596;});}}_LL256:;};}_LL254:;_LL255:
goto _LL247;_LL247:;}{union Cyc_CfFlowInfo_FlowInfo _tmp598;void*_tmp599;struct
_tuple13 _tmp597=Cyc_NewControlFlow_anal_Rexp(env,inflow,e);_tmp598=_tmp597.f1;
_tmp599=_tmp597.f2;_tmp598=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp598);{union Cyc_CfFlowInfo_FlowInfo _tmp59A=_tmp598;int _tmp59B;struct _tuple12
_tmp59C;struct Cyc_Dict_Dict _tmp59D;_LL2D6: if((_tmp59A.BottomFL).tag != 1)goto
_LL2D8;_tmp59B=(int)(_tmp59A.BottomFL).val;_LL2D7: return({struct _tuple16 _tmp59E;
_tmp59E.f1=_tmp598;_tmp59E.f2=_tmp598;_tmp59E;});_LL2D8: if((_tmp59A.ReachableFL).tag
!= 2)goto _LL2D5;_tmp59C=(struct _tuple12)(_tmp59A.ReachableFL).val;_tmp59D=
_tmp59C.f1;_LL2D9: {void*_tmp59F=_tmp599;enum Cyc_CfFlowInfo_InitLevel _tmp5A5;
enum Cyc_CfFlowInfo_InitLevel _tmp5A7;enum Cyc_CfFlowInfo_InitLevel _tmp5A9;_LL2DB: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp5A0=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp59F;if(_tmp5A0->tag != 0)goto _LL2DD;}_LL2DC: return({struct _tuple16 _tmp5AD;
_tmp5AD.f1=Cyc_CfFlowInfo_BottomFL();_tmp5AD.f2=_tmp598;_tmp5AD;});_LL2DD: {
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp5A1=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp59F;if(_tmp5A1->tag != 2)goto _LL2DF;}_LL2DE: goto _LL2E0;_LL2DF: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp5A2=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp59F;if(_tmp5A2->tag != 1)
goto _LL2E1;}_LL2E0: goto _LL2E2;_LL2E1: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5A3=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp59F;if(_tmp5A3->tag != 5)goto
_LL2E3;}_LL2E2: return({struct _tuple16 _tmp5AE;_tmp5AE.f1=_tmp598;_tmp5AE.f2=Cyc_CfFlowInfo_BottomFL();
_tmp5AE;});_LL2E3: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5A4=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp59F;if(_tmp5A4->tag != 3)goto _LL2E5;else{_tmp5A5=_tmp5A4->f1;if(_tmp5A5 != Cyc_CfFlowInfo_NoneIL)
goto _LL2E5;}}_LL2E4: goto _LL2E6;_LL2E5: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5A6=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp59F;if(_tmp5A6->tag != 4)goto _LL2E7;else{
_tmp5A7=_tmp5A6->f1;if(_tmp5A7 != Cyc_CfFlowInfo_NoneIL)goto _LL2E7;}}_LL2E6:({
void*_tmp5AF=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5B0="expression may not be initialized";
_tag_dyneither(_tmp5B0,sizeof(char),34);}),_tag_dyneither(_tmp5AF,sizeof(void*),
0));});return({struct _tuple16 _tmp5B1;_tmp5B1.f1=Cyc_CfFlowInfo_BottomFL();
_tmp5B1.f2=Cyc_CfFlowInfo_BottomFL();_tmp5B1;});_LL2E7: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp5A8=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp59F;if(_tmp5A8->tag != 3)goto
_LL2E9;else{_tmp5A9=_tmp5A8->f1;}}_LL2E8: return Cyc_NewControlFlow_splitzero(env,
inflow,_tmp598,e,_tmp5A9);_LL2E9: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5AA=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp59F;if(_tmp5AA->tag != 4)goto _LL2EB;}_LL2EA:
return({struct _tuple16 _tmp5B2;_tmp5B2.f1=_tmp598;_tmp5B2.f2=_tmp598;_tmp5B2;});
_LL2EB: {struct Cyc_CfFlowInfo_TagCmps_struct*_tmp5AB=(struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp59F;if(_tmp5AB->tag != 6)goto _LL2ED;}_LL2EC: return({struct _tuple16 _tmp5B3;
_tmp5B3.f1=_tmp598;_tmp5B3.f2=_tmp598;_tmp5B3;});_LL2ED: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5AC=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp59F;if(_tmp5AC->tag != 7)goto
_LL2DA;}_LL2EE:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp5B4=
_cycalloc(sizeof(*_tmp5B4));_tmp5B4[0]=({struct Cyc_Core_Impossible_struct _tmp5B5;
_tmp5B5.tag=Cyc_Core_Impossible;_tmp5B5.f1=({const char*_tmp5B6="anal_test";
_tag_dyneither(_tmp5B6,sizeof(char),10);});_tmp5B5;});_tmp5B4;}));_LL2DA:;}
_LL2D5:;};};}static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){union
Cyc_CfFlowInfo_FlowInfo _tmp5B7=flow;int _tmp5B8;struct _tuple12 _tmp5B9;struct Cyc_Dict_Dict
_tmp5BA;_LL2F0: if((_tmp5B7.BottomFL).tag != 1)goto _LL2F2;_tmp5B8=(int)(_tmp5B7.BottomFL).val;
_LL2F1: return;_LL2F2: if((_tmp5B7.ReachableFL).tag != 2)goto _LL2EF;_tmp5B9=(struct
_tuple12)(_tmp5B7.ReachableFL).val;_tmp5BA=_tmp5B9.f1;_LL2F3:{struct Cyc_List_List*
_tmp5BB=env->param_roots;for(0;_tmp5BB != 0;_tmp5BB=_tmp5BB->tl){if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp5BA,Cyc_CfFlowInfo_lookup_place(_tmp5BA,(struct Cyc_CfFlowInfo_Place*)
_tmp5BB->hd))!= Cyc_CfFlowInfo_AllIL)({void*_tmp5BC=0;Cyc_Tcutil_terr(loc,({
const char*_tmp5BD="function may not initialize all the parameters with attribute 'initializes'";
_tag_dyneither(_tmp5BD,sizeof(char),76);}),_tag_dyneither(_tmp5BC,sizeof(void*),
0));});}}return;_LL2EF:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp5BE=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp5C0;struct Cyc_Core_Opt*_tmp5C1;
struct Cyc_Absyn_Exp*_tmp5C2;struct Cyc_Absyn_Stmt*_tmp5C3;struct Cyc_Position_Segment*
_tmp5C4;struct Cyc_Absyn_Switch_clause*_tmp5BF=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp5C0=*_tmp5BF;_tmp5C1=_tmp5C0.pat_vars;_tmp5C2=_tmp5C0.where_clause;
_tmp5C3=_tmp5C0.body;_tmp5C4=_tmp5C0.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5BE,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5C1))->v,_tmp5BE->unknown_all,_tmp5C4);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp5C2 != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp5C2;union Cyc_CfFlowInfo_FlowInfo _tmp5C6;union Cyc_CfFlowInfo_FlowInfo
_tmp5C7;struct _tuple16 _tmp5C5=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp5C6=_tmp5C5.f1;_tmp5C7=_tmp5C5.f2;_tmp5C6=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp5C6);_tmp5C7=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp5C7);inflow=_tmp5C7;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5C6,
_tmp5C3);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp5C3);}{union Cyc_CfFlowInfo_FlowInfo _tmp5C8=clause_outflow;int _tmp5C9;_LL2F5:
if((_tmp5C8.BottomFL).tag != 1)goto _LL2F7;_tmp5C9=(int)(_tmp5C8.BottomFL).val;
_LL2F6: goto _LL2F4;_LL2F7:;_LL2F8: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*
_tmp5CA=0;Cyc_Tcutil_terr(_tmp5C3->loc,({const char*_tmp5CB="switch clause may implicitly fallthru";
_tag_dyneither(_tmp5CB,sizeof(char),38);}),_tag_dyneither(_tmp5CA,sizeof(void*),
0));});else{({void*_tmp5CC=0;Cyc_Tcutil_warn(_tmp5C3->loc,({const char*_tmp5CD="switch clause may implicitly fallthru";
_tag_dyneither(_tmp5CD,sizeof(char),38);}),_tag_dyneither(_tmp5CC,sizeof(void*),
0));});}Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}goto
_LL2F4;_LL2F4:;};};}return Cyc_CfFlowInfo_BottomFL();}static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5CF;union Cyc_CfFlowInfo_FlowInfo*_tmp5D0;struct _tuple17 _tmp5CE=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp5CF=_tmp5CE.f1;_tmp5D0=_tmp5CE.f2;inflow=*_tmp5D0;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp5D1=env->fenv;{void*_tmp5D2=s->r;struct Cyc_Absyn_Exp*_tmp5D5;struct Cyc_Absyn_Exp*
_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Stmt*_tmp5DB;struct Cyc_Absyn_Stmt*
_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DE;struct Cyc_Absyn_Stmt*_tmp5DF;struct Cyc_Absyn_Stmt*
_tmp5E0;struct _tuple9 _tmp5E2;struct Cyc_Absyn_Exp*_tmp5E3;struct Cyc_Absyn_Stmt*
_tmp5E4;struct Cyc_Absyn_Stmt*_tmp5E5;struct Cyc_Absyn_Stmt*_tmp5E7;struct _tuple9
_tmp5E8;struct Cyc_Absyn_Exp*_tmp5E9;struct Cyc_Absyn_Stmt*_tmp5EA;struct Cyc_Absyn_Exp*
_tmp5EC;struct _tuple9 _tmp5ED;struct Cyc_Absyn_Exp*_tmp5EE;struct Cyc_Absyn_Stmt*
_tmp5EF;struct _tuple9 _tmp5F0;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Stmt*
_tmp5F2;struct Cyc_Absyn_Stmt*_tmp5F3;struct Cyc_Absyn_Stmt*_tmp5F5;struct Cyc_List_List*
_tmp5F7;struct Cyc_Absyn_Switch_clause**_tmp5F8;struct Cyc_Absyn_Switch_clause*
_tmp5F9;struct Cyc_Absyn_Stmt*_tmp5FB;struct Cyc_Absyn_Stmt*_tmp5FD;struct Cyc_Absyn_Stmt*
_tmp5FF;struct Cyc_Absyn_Exp*_tmp601;struct Cyc_List_List*_tmp602;struct Cyc_Absyn_Stmt*
_tmp604;struct Cyc_List_List*_tmp605;struct Cyc_Absyn_Decl*_tmp607;struct Cyc_Absyn_Decl
_tmp608;void*_tmp609;struct Cyc_Absyn_Exp*_tmp60B;struct Cyc_Absyn_Tvar*_tmp60C;
struct Cyc_Absyn_Vardecl*_tmp60D;struct Cyc_Absyn_Stmt*_tmp60E;struct Cyc_Absyn_Decl*
_tmp610;struct Cyc_Absyn_Stmt*_tmp611;struct Cyc_Absyn_Stmt*_tmp613;struct Cyc_Absyn_Exp*
_tmp615;_LL2FA: {struct Cyc_Absyn_Skip_s_struct*_tmp5D3=(struct Cyc_Absyn_Skip_s_struct*)
_tmp5D2;if(_tmp5D3->tag != 0)goto _LL2FC;}_LL2FB: return inflow;_LL2FC: {struct Cyc_Absyn_Return_s_struct*
_tmp5D4=(struct Cyc_Absyn_Return_s_struct*)_tmp5D2;if(_tmp5D4->tag != 3)goto _LL2FE;
else{_tmp5D5=_tmp5D4->f1;if(_tmp5D5 != 0)goto _LL2FE;}}_LL2FD: if(env->noreturn)({
void*_tmp616=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp617="`noreturn' function might return";
_tag_dyneither(_tmp617,sizeof(char),33);}),_tag_dyneither(_tmp616,sizeof(void*),
0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL2FE: {struct Cyc_Absyn_Return_s_struct*_tmp5D6=(struct Cyc_Absyn_Return_s_struct*)
_tmp5D2;if(_tmp5D6->tag != 3)goto _LL300;else{_tmp5D7=_tmp5D6->f1;}}_LL2FF: if(env->noreturn)({
void*_tmp618=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp619="`noreturn' function might return";
_tag_dyneither(_tmp619,sizeof(char),33);}),_tag_dyneither(_tmp618,sizeof(void*),
0));});{union Cyc_CfFlowInfo_FlowInfo _tmp61B;void*_tmp61C;struct _tuple13 _tmp61A=
Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)_check_null(
_tmp5D7));_tmp61B=_tmp61A.f1;_tmp61C=_tmp61A.f2;_tmp61B=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp5D1,_tmp5D7->loc,_tmp61B);_tmp61B=Cyc_NewControlFlow_use_Rval(env,_tmp5D7->loc,
_tmp61B,_tmp61C);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp61B);return
Cyc_CfFlowInfo_BottomFL();};_LL300: {struct Cyc_Absyn_Exp_s_struct*_tmp5D8=(
struct Cyc_Absyn_Exp_s_struct*)_tmp5D2;if(_tmp5D8->tag != 1)goto _LL302;else{
_tmp5D9=_tmp5D8->f1;}}_LL301: outflow=(Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp5D9)).f1;goto _LL2F9;_LL302: {struct Cyc_Absyn_Seq_s_struct*_tmp5DA=(struct Cyc_Absyn_Seq_s_struct*)
_tmp5D2;if(_tmp5DA->tag != 2)goto _LL304;else{_tmp5DB=_tmp5DA->f1;_tmp5DC=_tmp5DA->f2;}}
_LL303: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,
inflow,_tmp5DB),_tmp5DC);goto _LL2F9;_LL304: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp5DD=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5D2;if(_tmp5DD->tag != 4)goto
_LL306;else{_tmp5DE=_tmp5DD->f1;_tmp5DF=_tmp5DD->f2;_tmp5E0=_tmp5DD->f3;}}_LL305: {
union Cyc_CfFlowInfo_FlowInfo _tmp61E;union Cyc_CfFlowInfo_FlowInfo _tmp61F;struct
_tuple16 _tmp61D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5DE);_tmp61E=_tmp61D.f1;
_tmp61F=_tmp61D.f2;_tmp61E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp5DE->loc,
_tmp61E);_tmp61F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp5DE->loc,_tmp61F);
outflow=Cyc_CfFlowInfo_join_flow(_tmp5D1,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp61E,_tmp5DF),Cyc_NewControlFlow_anal_stmt(env,_tmp61F,_tmp5E0),1);goto
_LL2F9;}_LL306: {struct Cyc_Absyn_While_s_struct*_tmp5E1=(struct Cyc_Absyn_While_s_struct*)
_tmp5D2;if(_tmp5E1->tag != 5)goto _LL308;else{_tmp5E2=_tmp5E1->f1;_tmp5E3=_tmp5E2.f1;
_tmp5E4=_tmp5E2.f2;_tmp5E5=_tmp5E1->f2;}}_LL307: {union Cyc_CfFlowInfo_FlowInfo*
_tmp621;struct _tuple17 _tmp620=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp5E4);_tmp621=_tmp620.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp622=*_tmp621;union
Cyc_CfFlowInfo_FlowInfo _tmp624;union Cyc_CfFlowInfo_FlowInfo _tmp625;struct
_tuple16 _tmp623=Cyc_NewControlFlow_anal_test(env,_tmp622,_tmp5E3);_tmp624=
_tmp623.f1;_tmp625=_tmp623.f2;_tmp624=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp5E3->loc,_tmp624);_tmp625=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp5E3->loc,
_tmp625);{union Cyc_CfFlowInfo_FlowInfo _tmp626=Cyc_NewControlFlow_anal_stmt(env,
_tmp624,_tmp5E5);Cyc_NewControlFlow_update_flow(env,_tmp5E4,_tmp626);outflow=
_tmp625;goto _LL2F9;};};}_LL308: {struct Cyc_Absyn_Do_s_struct*_tmp5E6=(struct Cyc_Absyn_Do_s_struct*)
_tmp5D2;if(_tmp5E6->tag != 14)goto _LL30A;else{_tmp5E7=_tmp5E6->f1;_tmp5E8=_tmp5E6->f2;
_tmp5E9=_tmp5E8.f1;_tmp5EA=_tmp5E8.f2;}}_LL309: {union Cyc_CfFlowInfo_FlowInfo
_tmp627=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E7);union Cyc_CfFlowInfo_FlowInfo*
_tmp629;struct _tuple17 _tmp628=Cyc_NewControlFlow_pre_stmt_check(env,_tmp627,
_tmp5EA);_tmp629=_tmp628.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp62A=*_tmp629;union
Cyc_CfFlowInfo_FlowInfo _tmp62C;union Cyc_CfFlowInfo_FlowInfo _tmp62D;struct
_tuple16 _tmp62B=Cyc_NewControlFlow_anal_test(env,_tmp62A,_tmp5E9);_tmp62C=
_tmp62B.f1;_tmp62D=_tmp62B.f2;Cyc_NewControlFlow_update_flow(env,_tmp5E7,_tmp62C);
outflow=_tmp62D;goto _LL2F9;};}_LL30A: {struct Cyc_Absyn_For_s_struct*_tmp5EB=(
struct Cyc_Absyn_For_s_struct*)_tmp5D2;if(_tmp5EB->tag != 9)goto _LL30C;else{
_tmp5EC=_tmp5EB->f1;_tmp5ED=_tmp5EB->f2;_tmp5EE=_tmp5ED.f1;_tmp5EF=_tmp5ED.f2;
_tmp5F0=_tmp5EB->f3;_tmp5F1=_tmp5F0.f1;_tmp5F2=_tmp5F0.f2;_tmp5F3=_tmp5EB->f4;}}
_LL30B: {union Cyc_CfFlowInfo_FlowInfo _tmp62E=(Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp5EC)).f1;_tmp62E=Cyc_CfFlowInfo_drop_unique_rvals(_tmp5EC->loc,
_tmp62E);{union Cyc_CfFlowInfo_FlowInfo*_tmp630;struct _tuple17 _tmp62F=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp62E,_tmp5EF);_tmp630=_tmp62F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp631=*
_tmp630;union Cyc_CfFlowInfo_FlowInfo _tmp633;union Cyc_CfFlowInfo_FlowInfo _tmp634;
struct _tuple16 _tmp632=Cyc_NewControlFlow_anal_test(env,_tmp631,_tmp5EE);_tmp633=
_tmp632.f1;_tmp634=_tmp632.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp635=Cyc_NewControlFlow_anal_stmt(
env,_tmp633,_tmp5F3);union Cyc_CfFlowInfo_FlowInfo*_tmp637;struct _tuple17 _tmp636=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp635,_tmp5F2);_tmp637=_tmp636.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp638=*_tmp637;union Cyc_CfFlowInfo_FlowInfo _tmp639=(Cyc_NewControlFlow_anal_Rexp(
env,_tmp638,_tmp5F1)).f1;_tmp639=Cyc_CfFlowInfo_drop_unique_rvals(_tmp5F1->loc,
_tmp639);Cyc_NewControlFlow_update_flow(env,_tmp5EF,_tmp639);outflow=_tmp634;
goto _LL2F9;};};};};}_LL30C: {struct Cyc_Absyn_Break_s_struct*_tmp5F4=(struct Cyc_Absyn_Break_s_struct*)
_tmp5D2;if(_tmp5F4->tag != 6)goto _LL30E;else{_tmp5F5=_tmp5F4->f1;if(_tmp5F5 != 0)
goto _LL30E;}}_LL30D: return Cyc_CfFlowInfo_BottomFL();_LL30E: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp5F6=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp5D2;if(_tmp5F6->tag != 11)goto
_LL310;else{_tmp5F7=_tmp5F6->f1;_tmp5F8=_tmp5F6->f2;if(_tmp5F8 == 0)goto _LL310;
_tmp5F9=*_tmp5F8;}}_LL30F: {struct _RegionHandle*_tmp63A=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp63C;struct Cyc_List_List*_tmp63D;struct _tuple19 _tmp63B=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp63A,env,inflow,_tmp5F7,0);_tmp63C=_tmp63B.f1;_tmp63D=_tmp63B.f2;for(0;
_tmp63D != 0;(_tmp63D=_tmp63D->tl,_tmp5F7=_tmp5F7->tl)){_tmp63C=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5F7))->hd)->loc,
_tmp63C,(void*)_tmp63D->hd);}_tmp63C=Cyc_CfFlowInfo_consume_unique_rvals(_tmp5D1,
s->loc,_tmp63C);_tmp63C=Cyc_NewControlFlow_add_vars(_tmp5D1,_tmp63C,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5F9->pat_vars))->v,_tmp5D1->unknown_all,s->loc);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp5F9->body,_tmp63C);
return Cyc_CfFlowInfo_BottomFL();}_LL310: {struct Cyc_Absyn_Break_s_struct*_tmp5FA=(
struct Cyc_Absyn_Break_s_struct*)_tmp5D2;if(_tmp5FA->tag != 6)goto _LL312;else{
_tmp5FB=_tmp5FA->f1;}}_LL311: _tmp5FD=_tmp5FB;goto _LL313;_LL312: {struct Cyc_Absyn_Continue_s_struct*
_tmp5FC=(struct Cyc_Absyn_Continue_s_struct*)_tmp5D2;if(_tmp5FC->tag != 7)goto
_LL314;else{_tmp5FD=_tmp5FC->f1;}}_LL313: _tmp5FF=_tmp5FD;goto _LL315;_LL314: {
struct Cyc_Absyn_Goto_s_struct*_tmp5FE=(struct Cyc_Absyn_Goto_s_struct*)_tmp5D2;
if(_tmp5FE->tag != 8)goto _LL316;else{_tmp5FF=_tmp5FE->f2;}}_LL315: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp63E=_tmp5CF->encloser;struct Cyc_Absyn_Stmt*
_tmp63F=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp5FF)))->encloser;while(_tmp63F != _tmp63E){struct Cyc_Absyn_Stmt*_tmp640=(Cyc_NewControlFlow_get_stmt_annot(
_tmp63E))->encloser;if(_tmp640 == _tmp63E){({void*_tmp641=0;Cyc_Tcutil_terr(s->loc,({
const char*_tmp642="goto enters local scope or exception handler";_tag_dyneither(
_tmp642,sizeof(char),45);}),_tag_dyneither(_tmp641,sizeof(void*),0));});break;}
_tmp63E=_tmp640;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp5FF),inflow);return Cyc_CfFlowInfo_BottomFL();_LL316: {struct Cyc_Absyn_Switch_s_struct*
_tmp600=(struct Cyc_Absyn_Switch_s_struct*)_tmp5D2;if(_tmp600->tag != 10)goto
_LL318;else{_tmp601=_tmp600->f1;_tmp602=_tmp600->f2;}}_LL317: {union Cyc_CfFlowInfo_FlowInfo
_tmp644;void*_tmp645;struct _tuple13 _tmp643=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp601);_tmp644=_tmp643.f1;_tmp645=_tmp643.f2;_tmp644=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp5D1,_tmp601->loc,_tmp644);_tmp644=Cyc_NewControlFlow_use_Rval(env,_tmp601->loc,
_tmp644,_tmp645);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp644,_tmp602);goto
_LL2F9;}_LL318: {struct Cyc_Absyn_TryCatch_s_struct*_tmp603=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp5D2;if(_tmp603->tag != 15)goto _LL31A;else{_tmp604=_tmp603->f1;_tmp605=_tmp603->f2;}}
_LL319: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp604);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp605);{union Cyc_CfFlowInfo_FlowInfo _tmp646=scs_outflow;int
_tmp647;_LL325: if((_tmp646.BottomFL).tag != 1)goto _LL327;_tmp647=(int)(_tmp646.BottomFL).val;
_LL326: goto _LL324;_LL327:;_LL328:({void*_tmp648=0;Cyc_Tcutil_terr(s->loc,({const
char*_tmp649="last catch clause may implicitly fallthru";_tag_dyneither(_tmp649,
sizeof(char),42);}),_tag_dyneither(_tmp648,sizeof(void*),0));});_LL324:;}outflow=
s1_outflow;goto _LL2F9;};};}_LL31A: {struct Cyc_Absyn_Decl_s_struct*_tmp606=(
struct Cyc_Absyn_Decl_s_struct*)_tmp5D2;if(_tmp606->tag != 12)goto _LL31C;else{
_tmp607=_tmp606->f1;_tmp608=*_tmp607;_tmp609=_tmp608.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp60A=(struct Cyc_Absyn_Alias_d_struct*)_tmp609;if(_tmp60A->tag != 5)goto _LL31C;
else{_tmp60B=_tmp60A->f1;_tmp60C=_tmp60A->f2;_tmp60D=_tmp60A->f3;}};_tmp60E=
_tmp606->f2;}}_LL31B: {union Cyc_CfFlowInfo_FlowInfo _tmp64B;void*_tmp64C;struct
_tuple13 _tmp64A=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp60B);_tmp64B=_tmp64A.f1;
_tmp64C=_tmp64A.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp64E;struct Cyc_List_List*
_tmp64F;struct _tuple14 _tmp64D=Cyc_CfFlowInfo_save_consume_info(_tmp5D1,_tmp64B,0);
_tmp64E=_tmp64D.f1;_tmp64F=_tmp64E.may_consume;_tmp64B=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp5D1,_tmp60B->loc,_tmp64B);_tmp64B=Cyc_NewControlFlow_use_Rval(env,_tmp60B->loc,
_tmp64B,_tmp64C);{struct Cyc_List_List _tmp650=({struct Cyc_List_List _tmp658;
_tmp658.hd=_tmp60D;_tmp658.tl=0;_tmp658;});_tmp64B=Cyc_NewControlFlow_add_vars(
_tmp5D1,_tmp64B,(struct Cyc_List_List*)& _tmp650,_tmp5D1->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp64B,_tmp60E);{union Cyc_CfFlowInfo_FlowInfo
_tmp651=outflow;int _tmp652;struct _tuple12 _tmp653;struct Cyc_Dict_Dict _tmp654;
struct Cyc_List_List*_tmp655;struct Cyc_CfFlowInfo_ConsumeInfo _tmp656;_LL32A: if((
_tmp651.BottomFL).tag != 1)goto _LL32C;_tmp652=(int)(_tmp651.BottomFL).val;_LL32B:
goto _LL329;_LL32C: if((_tmp651.ReachableFL).tag != 2)goto _LL329;_tmp653=(struct
_tuple12)(_tmp651.ReachableFL).val;_tmp654=_tmp653.f1;_tmp655=_tmp653.f2;_tmp656=
_tmp653.f3;_LL32D: while(_tmp64F != 0){struct Cyc_Dict_Dict _tmp657=_tmp656.consumed;
_tmp656.consumed=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,
struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)(_tmp5D1->r,_tmp656.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp64F->hd);if((_tmp656.consumed).t != _tmp657.t);
_tmp64F=_tmp64F->tl;}outflow=Cyc_CfFlowInfo_ReachableFL(_tmp654,_tmp655,_tmp656);
goto _LL329;_LL329:;}goto _LL2F9;};};}_LL31C: {struct Cyc_Absyn_Decl_s_struct*
_tmp60F=(struct Cyc_Absyn_Decl_s_struct*)_tmp5D2;if(_tmp60F->tag != 12)goto _LL31E;
else{_tmp610=_tmp60F->f1;_tmp611=_tmp60F->f2;}}_LL31D: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp610),_tmp611);goto _LL2F9;_LL31E: {
struct Cyc_Absyn_Label_s_struct*_tmp612=(struct Cyc_Absyn_Label_s_struct*)_tmp5D2;
if(_tmp612->tag != 13)goto _LL320;else{_tmp613=_tmp612->f2;}}_LL31F: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp613);goto _LL2F9;_LL320: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp614=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp5D2;if(_tmp614->tag != 16)goto
_LL322;else{_tmp615=_tmp614->f1;}}_LL321: {union Cyc_CfFlowInfo_FlowInfo _tmp65A;
void*_tmp65B;struct _tuple13 _tmp659=Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp615);_tmp65A=_tmp659.f1;_tmp65B=_tmp659.f2;_tmp65A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp615->loc,_tmp65A);{union Cyc_CfFlowInfo_FlowInfo _tmp65C=Cyc_NewControlFlow_use_Rval(
env,_tmp615->loc,_tmp65A,_tmp65B);{void*_tmp65D=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp615->topt))->v);void*_tmp65F;_LL32F: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp65E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp65D;if(_tmp65E->tag != 16)goto _LL331;else{_tmp65F=(void*)_tmp65E->f1;}}_LL330:
outflow=Cyc_CfFlowInfo_kill_flow_region(_tmp5D1,_tmp65A,_tmp65F);goto _LL32E;
_LL331:;_LL332:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp660=
_cycalloc(sizeof(*_tmp660));_tmp660[0]=({struct Cyc_Core_Impossible_struct _tmp661;
_tmp661.tag=Cyc_Core_Impossible;_tmp661.f1=({const char*_tmp662="anal_stmt -- reset_region";
_tag_dyneither(_tmp662,sizeof(char),26);});_tmp661;});_tmp660;}));_LL32E:;}goto
_LL2F9;};}_LL322:;_LL323:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663[0]=({struct Cyc_Core_Impossible_struct
_tmp664;_tmp664.tag=Cyc_Core_Impossible;_tmp664.f1=({const char*_tmp665="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dyneither(_tmp665,sizeof(char),56);});_tmp664;});_tmp663;}));_LL2F9:;}
outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp666=outflow;struct _tuple12 _tmp667;struct Cyc_CfFlowInfo_ConsumeInfo _tmp668;
_LL334: if((_tmp666.ReachableFL).tag != 2)goto _LL336;_tmp667=(struct _tuple12)(
_tmp666.ReachableFL).val;_tmp668=_tmp667.f3;_LL335: goto _LL333;_LL336:;_LL337:
goto _LL333;_LL333:;}return outflow;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*
_tmp669=env->fenv;void*_tmp66A=decl->r;struct Cyc_Absyn_Vardecl*_tmp66C;struct Cyc_Core_Opt*
_tmp66E;struct Cyc_Core_Opt _tmp66F;struct Cyc_List_List*_tmp670;struct Cyc_Absyn_Exp*
_tmp671;struct Cyc_List_List*_tmp673;struct Cyc_Absyn_Fndecl*_tmp675;struct Cyc_Absyn_Tvar*
_tmp677;struct Cyc_Absyn_Vardecl*_tmp678;int _tmp679;struct Cyc_Absyn_Exp*_tmp67A;
_LL339: {struct Cyc_Absyn_Var_d_struct*_tmp66B=(struct Cyc_Absyn_Var_d_struct*)
_tmp66A;if(_tmp66B->tag != 0)goto _LL33B;else{_tmp66C=_tmp66B->f1;}}_LL33A: {
struct Cyc_List_List _tmp67B=({struct Cyc_List_List _tmp68A;_tmp68A.hd=_tmp66C;
_tmp68A.tl=0;_tmp68A;});inflow=Cyc_NewControlFlow_add_vars(_tmp669,inflow,(
struct Cyc_List_List*)& _tmp67B,_tmp669->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp67C=_tmp66C->initializer;if(_tmp67C == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp67E;void*_tmp67F;struct _tuple13 _tmp67D=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp67C);_tmp67E=_tmp67D.f1;_tmp67F=_tmp67D.f2;
_tmp67E=Cyc_CfFlowInfo_consume_unique_rvals(_tmp669,_tmp67C->loc,_tmp67E);{union
Cyc_CfFlowInfo_FlowInfo _tmp680=_tmp67E;int _tmp681;struct _tuple12 _tmp682;struct
Cyc_Dict_Dict _tmp683;struct Cyc_List_List*_tmp684;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp685;_LL346: if((_tmp680.BottomFL).tag != 1)goto _LL348;_tmp681=(int)(_tmp680.BottomFL).val;
_LL347: return Cyc_CfFlowInfo_BottomFL();_LL348: if((_tmp680.ReachableFL).tag != 2)
goto _LL345;_tmp682=(struct _tuple12)(_tmp680.ReachableFL).val;_tmp683=_tmp682.f1;
_tmp684=_tmp682.f2;_tmp685=_tmp682.f3;_LL349: _tmp683=Cyc_CfFlowInfo_assign_place(
_tmp669,decl->loc,_tmp683,env->all_changed,({struct Cyc_CfFlowInfo_Place*_tmp686=
_region_malloc(env->r,sizeof(*_tmp686));_tmp686->root=(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp687=_region_malloc(env->r,sizeof(*_tmp687));_tmp687[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp688;_tmp688.tag=0;_tmp688.f1=_tmp66C;_tmp688;});_tmp687;});_tmp686->fields=0;
_tmp686;}),_tmp67F);_tmp684=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp684,
_tmp66C,(struct Cyc_Absyn_Exp*)_check_null(_tmp66C->initializer));{union Cyc_CfFlowInfo_FlowInfo
_tmp689=Cyc_CfFlowInfo_ReachableFL(_tmp683,_tmp684,_tmp685);Cyc_NewControlFlow_update_tryflow(
env,_tmp689);return _tmp689;};_LL345:;};};};}_LL33B: {struct Cyc_Absyn_Let_d_struct*
_tmp66D=(struct Cyc_Absyn_Let_d_struct*)_tmp66A;if(_tmp66D->tag != 2)goto _LL33D;
else{_tmp66E=_tmp66D->f2;if(_tmp66E == 0)goto _LL33D;_tmp66F=*_tmp66E;_tmp670=(
struct Cyc_List_List*)_tmp66F.v;_tmp671=_tmp66D->f3;}}_LL33C: {union Cyc_CfFlowInfo_FlowInfo
_tmp68C;void*_tmp68D;struct _tuple13 _tmp68B=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp671);_tmp68C=_tmp68B.f1;_tmp68D=_tmp68B.f2;_tmp68C=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp669,_tmp671->loc,_tmp68C);_tmp68C=Cyc_NewControlFlow_use_Rval(env,_tmp671->loc,
_tmp68C,_tmp68D);return Cyc_NewControlFlow_add_vars(_tmp669,_tmp68C,_tmp670,
_tmp669->unknown_all,decl->loc);}_LL33D: {struct Cyc_Absyn_Letv_d_struct*_tmp672=(
struct Cyc_Absyn_Letv_d_struct*)_tmp66A;if(_tmp672->tag != 3)goto _LL33F;else{
_tmp673=_tmp672->f1;}}_LL33E: return Cyc_NewControlFlow_add_vars(_tmp669,inflow,
_tmp673,_tmp669->unknown_none,decl->loc);_LL33F: {struct Cyc_Absyn_Fn_d_struct*
_tmp674=(struct Cyc_Absyn_Fn_d_struct*)_tmp66A;if(_tmp674->tag != 1)goto _LL341;
else{_tmp675=_tmp674->f1;}}_LL340: Cyc_NewControlFlow_check_nested_fun(_tmp669,
inflow,_tmp675);{void*_tmp68E=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp675->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp68F=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp675->fn_vardecl);
return Cyc_NewControlFlow_add_vars(_tmp669,inflow,({struct Cyc_List_List*_tmp690=
_region_malloc(env->r,sizeof(*_tmp690));_tmp690->hd=_tmp68F;_tmp690->tl=0;
_tmp690;}),_tmp669->unknown_all,decl->loc);};_LL341: {struct Cyc_Absyn_Region_d_struct*
_tmp676=(struct Cyc_Absyn_Region_d_struct*)_tmp66A;if(_tmp676->tag != 4)goto _LL343;
else{_tmp677=_tmp676->f1;_tmp678=_tmp676->f2;_tmp679=_tmp676->f3;_tmp67A=_tmp676->f4;}}
_LL342: if(_tmp67A != 0){struct Cyc_Absyn_Exp*_tmp691=(struct Cyc_Absyn_Exp*)_tmp67A;
union Cyc_CfFlowInfo_FlowInfo _tmp693;void*_tmp694;struct _tuple13 _tmp692=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp691);_tmp693=_tmp692.f1;_tmp694=_tmp692.f2;_tmp693=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp669,_tmp691->loc,_tmp693);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp691->loc,
_tmp693,_tmp694);}{struct Cyc_List_List _tmp695=({struct Cyc_List_List _tmp696;
_tmp696.hd=_tmp678;_tmp696.tl=0;_tmp696;});return Cyc_NewControlFlow_add_vars(
_tmp669,inflow,(struct Cyc_List_List*)& _tmp695,_tmp669->unknown_all,decl->loc);};
_LL343:;_LL344:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp697=
_cycalloc(sizeof(*_tmp697));_tmp697[0]=({struct Cyc_Core_Impossible_struct _tmp698;
_tmp698.tag=Cyc_Core_Impossible;_tmp698.f1=({const char*_tmp699="anal_decl: unexpected decl variant";
_tag_dyneither(_tmp699,sizeof(char),35);});_tmp698;});_tmp697;}));_LL338:;}
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle _tmp69A=_new_region("frgn");struct _RegionHandle*frgn=& _tmp69A;
_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,
0,({struct Cyc_CfFlowInfo_ConsumeInfo _tmp69B;_tmp69B.consumed=fenv->mt_place_set;
_tmp69B.may_consume=0;_tmp69B;})),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp69C=fenv->r;struct Cyc_Position_Segment*_tmp69D=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp69D);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp69E=inflow;int _tmp69F;struct
_tuple12 _tmp6A0;struct Cyc_Dict_Dict _tmp6A1;struct Cyc_List_List*_tmp6A2;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp6A3;_LL34B: if((_tmp69E.BottomFL).tag != 1)goto _LL34D;_tmp69F=(int)(_tmp69E.BottomFL).val;
_LL34C:({void*_tmp6A4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp6A5="check_fun";_tag_dyneither(_tmp6A5,sizeof(
char),10);}),_tag_dyneither(_tmp6A4,sizeof(void*),0));});_LL34D: if((_tmp69E.ReachableFL).tag
!= 2)goto _LL34A;_tmp6A0=(struct _tuple12)(_tmp69E.ReachableFL).val;_tmp6A1=
_tmp6A0.f1;_tmp6A2=_tmp6A0.f2;_tmp6A3=_tmp6A0.f3;_LL34E: {struct Cyc_List_List*
atts;{void*_tmp6A6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp6A8;struct Cyc_List_List*_tmp6A9;
_LL350: {struct Cyc_Absyn_FnType_struct*_tmp6A7=(struct Cyc_Absyn_FnType_struct*)
_tmp6A6;if(_tmp6A7->tag != 10)goto _LL352;else{_tmp6A8=_tmp6A7->f1;_tmp6A9=_tmp6A8.attributes;}}
_LL351: atts=_tmp6A9;goto _LL34F;_LL352:;_LL353:({void*_tmp6AA=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp6AB="check_fun: non-function type cached with fndecl_t";
_tag_dyneither(_tmp6AB,sizeof(char),50);}),_tag_dyneither(_tmp6AA,sizeof(void*),
0));});_LL34F:;}for(0;atts != 0;atts=atts->tl){void*_tmp6AC=(void*)atts->hd;int
_tmp6AE;_LL355: {struct Cyc_Absyn_Initializes_att_struct*_tmp6AD=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp6AC;if(_tmp6AD->tag != 20)goto _LL357;else{_tmp6AE=_tmp6AD->f1;}}_LL356: {
unsigned int j=(unsigned int)_tmp6AE;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp6AF=0;Cyc_Tcutil_terr(_tmp69D,({const char*_tmp6B0="initializes attribute exceeds number of parameters";
_tag_dyneither(_tmp6B0,sizeof(char),51);}),_tag_dyneither(_tmp6AF,sizeof(void*),
0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp6B1=((struct Cyc_Absyn_Vardecl*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp6B2=Cyc_Tcutil_compress(
_tmp6B1->type);struct Cyc_Absyn_PtrInfo _tmp6B4;void*_tmp6B5;struct Cyc_Absyn_PtrAtts
_tmp6B6;union Cyc_Absyn_Constraint*_tmp6B7;union Cyc_Absyn_Constraint*_tmp6B8;
union Cyc_Absyn_Constraint*_tmp6B9;_LL35A: {struct Cyc_Absyn_PointerType_struct*
_tmp6B3=(struct Cyc_Absyn_PointerType_struct*)_tmp6B2;if(_tmp6B3->tag != 5)goto
_LL35C;else{_tmp6B4=_tmp6B3->f1;_tmp6B5=_tmp6B4.elt_typ;_tmp6B6=_tmp6B4.ptr_atts;
_tmp6B7=_tmp6B6.nullable;_tmp6B8=_tmp6B6.bounds;_tmp6B9=_tmp6B6.zero_term;}}
_LL35B: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6B7))({
void*_tmp6BA=0;Cyc_Tcutil_terr(_tmp69D,({const char*_tmp6BB="initializes attribute not allowed on nullable pointers";
_tag_dyneither(_tmp6BB,sizeof(char),55);}),_tag_dyneither(_tmp6BA,sizeof(void*),
0));});if(!Cyc_Tcutil_is_bound_one(_tmp6B8))({void*_tmp6BC=0;Cyc_Tcutil_terr(
_tmp69D,({const char*_tmp6BD="initializes attribute allowed only on pointers of size 1";
_tag_dyneither(_tmp6BD,sizeof(char),57);}),_tag_dyneither(_tmp6BC,sizeof(void*),
0));});if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6B9))({void*_tmp6BE=0;Cyc_Tcutil_terr(_tmp69D,({const char*_tmp6BF="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dyneither(_tmp6BF,sizeof(char),77);}),_tag_dyneither(_tmp6BE,sizeof(void*),
0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp6C0=({struct Cyc_CfFlowInfo_InitParam_struct*
_tmp6C8=_region_malloc(_tmp69C,sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_CfFlowInfo_InitParam_struct
_tmp6C9;_tmp6C9.tag=2;_tmp6C9.f1=(int)j;_tmp6C9.f2=(void*)_tmp6B5;_tmp6C9;});
_tmp6C8;});struct Cyc_CfFlowInfo_Place*_tmp6C1=({struct Cyc_CfFlowInfo_Place*
_tmp6C7=_region_malloc(_tmp69C,sizeof(*_tmp6C7));_tmp6C7->root=(void*)_tmp6C0;
_tmp6C7->fields=0;_tmp6C7;});_tmp6A1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,void*v))Cyc_Dict_insert)(_tmp6A1,(void*)_tmp6C0,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp6B5,fenv->esc_none));_tmp6A1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,void*v))Cyc_Dict_insert)(_tmp6A1,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp6C2=_region_malloc(_tmp69C,sizeof(*_tmp6C2));_tmp6C2[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp6C3;_tmp6C3.tag=0;_tmp6C3.f1=_tmp6B1;_tmp6C3;});_tmp6C2;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp6C4=_region_malloc(_tmp69C,sizeof(*_tmp6C4));
_tmp6C4[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp6C5;_tmp6C5.tag=5;_tmp6C5.f1=
_tmp6C1;_tmp6C5;});_tmp6C4;}));param_roots=({struct Cyc_List_List*_tmp6C6=
_region_malloc(_tmp69C,sizeof(*_tmp6C6));_tmp6C6->hd=_tmp6C1;_tmp6C6->tl=
param_roots;_tmp6C6;});goto _LL359;};_LL35C:;_LL35D:({void*_tmp6CA=0;Cyc_Tcutil_terr(
_tmp69D,({const char*_tmp6CB="initializes attribute on non-pointer";
_tag_dyneither(_tmp6CB,sizeof(char),37);}),_tag_dyneither(_tmp6CA,sizeof(void*),
0));});_LL359:;}goto _LL354;};}_LL357:;_LL358: goto _LL354;_LL354:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6A1,_tmp6A2,_tmp6A3);}_LL34A:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp69C,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp6D9=_region_malloc(_tmp69C,sizeof(*_tmp6D9));_tmp6D9->r=_tmp69C;_tmp6D9->fenv=
fenv;_tmp6D9->iterate_again=1;_tmp6D9->iteration_num=0;_tmp6D9->in_try=0;_tmp6D9->tryflow=
inflow;_tmp6D9->all_changed=0;_tmp6D9->noreturn=noreturn;_tmp6D9->param_roots=
param_roots;_tmp6D9->flow_table=flow_table;_tmp6D9;});union Cyc_CfFlowInfo_FlowInfo
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo
_tmp6CC=outflow;int _tmp6CD;_LL35F: if((_tmp6CC.BottomFL).tag != 1)goto _LL361;
_tmp6CD=(int)(_tmp6CC.BottomFL).val;_LL360: goto _LL35E;_LL361:;_LL362: Cyc_NewControlFlow_check_init_params(
_tmp69D,env,outflow);if(noreturn)({void*_tmp6CE=0;Cyc_Tcutil_terr(_tmp69D,({
const char*_tmp6CF="`noreturn' function might (implicitly) return";_tag_dyneither(
_tmp6CF,sizeof(char),46);}),_tag_dyneither(_tmp6CE,sizeof(void*),0));});else{
void*_tmp6D0=Cyc_Tcutil_compress(fd->ret_type);_LL364: {struct Cyc_Absyn_VoidType_struct*
_tmp6D1=(struct Cyc_Absyn_VoidType_struct*)_tmp6D0;if(_tmp6D1->tag != 0)goto _LL366;}
_LL365: goto _LL363;_LL366: {struct Cyc_Absyn_DoubleType_struct*_tmp6D2=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6D0;if(_tmp6D2->tag != 8)goto _LL368;}_LL367: goto _LL369;_LL368: {struct Cyc_Absyn_FloatType_struct*
_tmp6D3=(struct Cyc_Absyn_FloatType_struct*)_tmp6D0;if(_tmp6D3->tag != 7)goto
_LL36A;}_LL369: goto _LL36B;_LL36A: {struct Cyc_Absyn_IntType_struct*_tmp6D4=(
struct Cyc_Absyn_IntType_struct*)_tmp6D0;if(_tmp6D4->tag != 6)goto _LL36C;}_LL36B:({
void*_tmp6D5=0;Cyc_Tcutil_warn(_tmp69D,({const char*_tmp6D6="function may not return a value";
_tag_dyneither(_tmp6D6,sizeof(char),32);}),_tag_dyneither(_tmp6D5,sizeof(void*),
0));});goto _LL363;_LL36C:;_LL36D:({void*_tmp6D7=0;Cyc_Tcutil_terr(_tmp69D,({
const char*_tmp6D8="function may not return a value";_tag_dyneither(_tmp6D8,
sizeof(char),32);}),_tag_dyneither(_tmp6D7,sizeof(void*),0));});goto _LL363;
_LL363:;}goto _LL35E;_LL35E:;};};};}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*
ds){for(0;ds != 0;ds=ds->tl){void*_tmp6DA=((struct Cyc_Absyn_Decl*)ds->hd)->r;
struct Cyc_Absyn_Fndecl*_tmp6DC;struct Cyc_List_List*_tmp6DE;struct Cyc_List_List*
_tmp6E0;struct Cyc_List_List*_tmp6E2;_LL36F: {struct Cyc_Absyn_Fn_d_struct*_tmp6DB=(
struct Cyc_Absyn_Fn_d_struct*)_tmp6DA;if(_tmp6DB->tag != 1)goto _LL371;else{_tmp6DC=
_tmp6DB->f1;}}_LL370: Cyc_NewControlFlow_check_fun(_tmp6DC);goto _LL36E;_LL371: {
struct Cyc_Absyn_ExternC_d_struct*_tmp6DD=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp6DA;if(_tmp6DD->tag != 12)goto _LL373;else{_tmp6DE=_tmp6DD->f1;}}_LL372:
_tmp6E0=_tmp6DE;goto _LL374;_LL373: {struct Cyc_Absyn_Using_d_struct*_tmp6DF=(
struct Cyc_Absyn_Using_d_struct*)_tmp6DA;if(_tmp6DF->tag != 11)goto _LL375;else{
_tmp6E0=_tmp6DF->f2;}}_LL374: _tmp6E2=_tmp6E0;goto _LL376;_LL375: {struct Cyc_Absyn_Namespace_d_struct*
_tmp6E1=(struct Cyc_Absyn_Namespace_d_struct*)_tmp6DA;if(_tmp6E1->tag != 10)goto
_LL377;else{_tmp6E2=_tmp6E1->f2;}}_LL376: Cyc_NewControlFlow_cf_check(_tmp6E2);
goto _LL36E;_LL377: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp6E3=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp6DA;if(_tmp6E3->tag != 13)goto _LL379;}_LL378: goto _LL36E;_LL379:;_LL37A: goto
_LL36E;_LL36E:;}}
