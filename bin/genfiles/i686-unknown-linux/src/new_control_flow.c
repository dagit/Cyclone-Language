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
char*tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),struct Cyc_List_List*
a,struct Cyc_List_List*b);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
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
ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,void*);
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
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
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{
int tag;struct Cyc_List_List*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
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
_tmp12;_tmp12.consumed=_tmp10;_tmp12.may_consume=0;_tmp12;}),0);}}}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp13=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmp14=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp15=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
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
struct Cyc_Iter_Iter _tmp23=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(_tmp22,*((struct Cyc_Dict_Dict*)
_check_null(out_unique_places)));struct _tuple18 _tmp24=*((struct _tuple18*(*)(
struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(_tmp22,*
out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(
_tmp23,& _tmp24)){struct Cyc_CfFlowInfo_Place*_tmp25=_tmp24.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp21,_tmp25))({
struct Cyc_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_CfFlowInfo_place_err_string(_tmp25));{void*_tmp26[1]={&
_tmp28};Cyc_Tcutil_terr(_tmp24.f2,({const char*_tmp27="Failed to consume unique variable %s";
_tag_dyneither(_tmp27,sizeof(char),37);}),_tag_dyneither(_tmp26,sizeof(void*),1));}});}}*
out_unique_places=old_unique_places;}}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp29=inflow;int _tmp2A;struct
_tuple12 _tmp2B;struct Cyc_Dict_Dict _tmp2C;struct Cyc_List_List*_tmp2D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2E;_LLB: if((_tmp29.BottomFL).tag != 1)goto _LLD;_tmp2A=(int)(_tmp29.BottomFL).val;
_LLC: return Cyc_CfFlowInfo_BottomFL();_LLD: if((_tmp29.ReachableFL).tag != 2)goto
_LLA;_tmp2B=(struct _tuple12)(_tmp29.ReachableFL).val;_tmp2C=_tmp2B.f1;_tmp2D=
_tmp2B.f2;_tmp2E=_tmp2B.f3;_LLE: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2C,r)
!= Cyc_CfFlowInfo_AllIL)({void*_tmp2F=0;Cyc_Tcutil_terr(loc,({const char*_tmp30="expression may not be fully initialized";
_tag_dyneither(_tmp30,sizeof(char),40);}),_tag_dyneither(_tmp2F,sizeof(void*),0));});{
struct Cyc_Dict_Dict _tmp31=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp2C,env->all_changed,
r);if(_tmp2C.t == _tmp31.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp32=Cyc_CfFlowInfo_ReachableFL(
_tmp31,_tmp2D,_tmp2E);Cyc_NewControlFlow_update_tryflow(env,_tmp32);return _tmp32;}}
_LLA:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*
rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,
struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({struct _tuple19
_tmp33;_tmp33.f1=inflow;_tmp33.f2=0;_tmp33;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo
_tmp35;void*_tmp36;struct _tuple13 _tmp34=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp35=_tmp34.f1;_tmp36=_tmp34.f2;return({struct
_tuple19 _tmp37;_tmp37.f1=_tmp35;_tmp37.f2=({struct Cyc_List_List*_tmp38=
_region_malloc(rgn,sizeof(*_tmp38));_tmp38->hd=_tmp36;_tmp38->tl=0;_tmp38;});
_tmp37;});}{struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;struct Cyc_Dict_Dict
this_all_changed;union Cyc_CfFlowInfo_FlowInfo old_inflow;union Cyc_CfFlowInfo_FlowInfo
outflow;struct Cyc_List_List*rvals;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3A;union
Cyc_CfFlowInfo_FlowInfo _tmp3B;struct _tuple14 _tmp39=Cyc_CfFlowInfo_save_consume_info(
env->fenv,inflow,1);_tmp3A=_tmp39.f1;_tmp3B=_tmp39.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp5D;_tmp5D.consumed=(env->fenv)->mt_place_set;
_tmp5D.may_consume=0;_tmp5D;});int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp3B=Cyc_CfFlowInfo_restore_consume_info(_tmp3B,_tmp3A,0);env->all_changed=({
struct Cyc_Dict_Dict*_tmp3C=_region_malloc(env->r,sizeof(*_tmp3C));_tmp3C[0]=(env->fenv)->mt_place_set;
_tmp3C;});{union Cyc_CfFlowInfo_FlowInfo _tmp3E;void*_tmp3F;struct _tuple13 _tmp3D=
Cyc_NewControlFlow_anal_Rexp(env,_tmp3B,(struct Cyc_Absyn_Exp*)es->hd);_tmp3E=
_tmp3D.f1;_tmp3F=_tmp3D.f2;outflow=_tmp3E;rvals=({struct Cyc_List_List*_tmp40=
_region_malloc(rgn,sizeof(*_tmp40));_tmp40->hd=_tmp3F;_tmp40->tl=0;_tmp40;});
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)),0);if(arg1_unconsumed){union Cyc_CfFlowInfo_FlowInfo
_tmp41=outflow;int _tmp42;struct _tuple12 _tmp43;struct Cyc_Dict_Dict _tmp44;struct
Cyc_List_List*_tmp45;struct Cyc_CfFlowInfo_ConsumeInfo _tmp46;_LL10: if((_tmp41.BottomFL).tag
!= 1)goto _LL12;_tmp42=(int)(_tmp41.BottomFL).val;_LL11: goto _LLF;_LL12: if((_tmp41.ReachableFL).tag
!= 2)goto _LLF;_tmp43=(struct _tuple12)(_tmp41.ReachableFL).val;_tmp44=_tmp43.f1;
_tmp45=_tmp43.f2;_tmp46=_tmp43.f3;_LL13: outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp44,_tmp45,({struct Cyc_CfFlowInfo_ConsumeInfo _tmp47;_tmp47.consumed=_tmp46.consumed;
_tmp47.may_consume=_tmp3A.may_consume;_tmp47;}));_LLF:;}{struct Cyc_List_List*es2=
es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({struct Cyc_Dict_Dict*_tmp48=
_region_malloc(env->r,sizeof(*_tmp48));_tmp48[0]=(env->fenv)->mt_place_set;
_tmp48;});{union Cyc_CfFlowInfo_FlowInfo _tmp4A;void*_tmp4B;struct _tuple13 _tmp49=
Cyc_NewControlFlow_anal_Rexp(env,_tmp3B,(struct Cyc_Absyn_Exp*)es2->hd);_tmp4A=
_tmp49.f1;_tmp4B=_tmp49.f2;rvals=({struct Cyc_List_List*_tmp4C=_region_malloc(rgn,
sizeof(*_tmp4C));_tmp4C->hd=_tmp4B;_tmp4C->tl=rvals;_tmp4C;});outflow=Cyc_CfFlowInfo_after_flow(
env->fenv,(struct Cyc_Dict_Dict*)& this_all_changed,outflow,_tmp4A,
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)),0);}}}{struct Cyc_CfFlowInfo_ConsumeInfo _tmp4E;
struct _tuple14 _tmp4D=Cyc_CfFlowInfo_save_consume_info(env->fenv,outflow,0);
_tmp4E=_tmp4D.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
outflow_consume,_tmp4E)){({void*_tmp4F=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp50="sanity consumed: ";_tag_dyneither(_tmp50,sizeof(char),18);}),
_tag_dyneither(_tmp4F,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp51=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp52="\ncurrent consumed: ";_tag_dyneither(_tmp52,sizeof(char),20);}),
_tag_dyneither(_tmp51,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp4E.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp53=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp54="\nsanity may_consume: ";_tag_dyneither(_tmp54,sizeof(char),22);}),
_tag_dyneither(_tmp53,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(outflow_consume.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp55=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp56="\ncurrent may_consume: ";_tag_dyneither(_tmp56,sizeof(char),23);}),
_tag_dyneither(_tmp55,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(_tmp4E.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp57=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp58="\n";_tag_dyneither(_tmp58,sizeof(char),2);}),_tag_dyneither(_tmp57,
sizeof(void*),0));});({void*_tmp59=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5A="anal_unordered_exps failed to preserve consumed info";
_tag_dyneither(_tmp5A,sizeof(char),53);}),_tag_dyneither(_tmp59,sizeof(void*),0));});}
old_inflow=Cyc_CfFlowInfo_restore_consume_info(_tmp3B,outflow_consume,0);}else{
old_inflow=_tmp3B;}init_consume=1;outflow_consume=_tmp4E;_tmp3B=Cyc_CfFlowInfo_join_flow(
env->fenv,outer_all_changed,_tmp3B,outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3B,old_inflow));if(outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({
struct Cyc_Dict_Dict*_tmp5B=_region_malloc(env->r,sizeof(*_tmp5B));_tmp5B[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp5B;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple19 _tmp5C;_tmp5C.f1=outflow;_tmp5C.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp5C;});}}}
static struct _tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int arg1_unconsumed){
struct _RegionHandle*_tmp5E=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp60;struct Cyc_List_List*
_tmp61;struct _tuple19 _tmp5F=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp5E,env,
inflow,es,arg1_unconsumed);_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp62=_tmp60;struct _tuple12 _tmp63;struct Cyc_Dict_Dict _tmp64;int _tmp65;_LL15: if((
_tmp62.ReachableFL).tag != 2)goto _LL17;_tmp63=(struct _tuple12)(_tmp62.ReachableFL).val;
_tmp64=_tmp63.f1;_LL16: for(0;_tmp61 != 0;(_tmp61=_tmp61->tl,es=((struct Cyc_List_List*)
_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp64,(void*)
_tmp61->hd)== Cyc_CfFlowInfo_NoneIL)({void*_tmp66=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp67="expression may not be initialized";
_tag_dyneither(_tmp67,sizeof(char),34);}),_tag_dyneither(_tmp66,sizeof(void*),0));});}
goto _LL14;_LL17: if((_tmp62.BottomFL).tag != 1)goto _LL14;_tmp65=(int)(_tmp62.BottomFL).val;
_LL18: goto _LL14;_LL14:;}return({struct _tuple13 _tmp68;_tmp68.f1=_tmp60;_tmp68.f2=(
env->fenv)->unknown_all;_tmp68;});}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp69=outflow;int _tmp6A;struct _tuple12 _tmp6B;struct Cyc_Dict_Dict _tmp6C;struct
Cyc_List_List*_tmp6D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp6E;_LL1A: if((_tmp69.BottomFL).tag
!= 1)goto _LL1C;_tmp6A=(int)(_tmp69.BottomFL).val;_LL1B: return outflow;_LL1C: if((
_tmp69.ReachableFL).tag != 2)goto _LL19;_tmp6B=(struct _tuple12)(_tmp69.ReachableFL).val;
_tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;_tmp6E=_tmp6B.f3;_LL1D: {union Cyc_CfFlowInfo_AbsLVal
_tmp6F=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;int _tmp70;struct Cyc_CfFlowInfo_Place*
_tmp71;_LL1F: if((_tmp6F.UnknownL).tag != 2)goto _LL21;_tmp70=(int)(_tmp6F.UnknownL).val;
_LL20: return outflow;_LL21: if((_tmp6F.PlaceL).tag != 1)goto _LL1E;_tmp71=(struct Cyc_CfFlowInfo_Place*)(
_tmp6F.PlaceL).val;_LL22: {void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(
env->fenv)->notzerothis;union Cyc_CfFlowInfo_FlowInfo _tmp72=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6C,env->all_changed,_tmp71,
nzval),_tmp6D,_tmp6E);return _tmp72;}_LL1E:;}_LL19:;}static struct _tuple16 Cyc_NewControlFlow_splitzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp73=outflow;int _tmp74;struct _tuple12 _tmp75;struct Cyc_Dict_Dict _tmp76;struct
Cyc_List_List*_tmp77;struct Cyc_CfFlowInfo_ConsumeInfo _tmp78;_LL24: if((_tmp73.BottomFL).tag
!= 1)goto _LL26;_tmp74=(int)(_tmp73.BottomFL).val;_LL25: return({struct _tuple16
_tmp79;_tmp79.f1=outflow;_tmp79.f2=outflow;_tmp79;});_LL26: if((_tmp73.ReachableFL).tag
!= 2)goto _LL23;_tmp75=(struct _tuple12)(_tmp73.ReachableFL).val;_tmp76=_tmp75.f1;
_tmp77=_tmp75.f2;_tmp78=_tmp75.f3;_LL27: {union Cyc_CfFlowInfo_AbsLVal _tmp7A=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;int _tmp7B;struct Cyc_CfFlowInfo_Place*_tmp7C;_LL29: if((_tmp7A.UnknownL).tag
!= 2)goto _LL2B;_tmp7B=(int)(_tmp7A.UnknownL).val;_LL2A: return({struct _tuple16
_tmp7D;_tmp7D.f1=outflow;_tmp7D.f2=outflow;_tmp7D;});_LL2B: if((_tmp7A.PlaceL).tag
!= 1)goto _LL28;_tmp7C=(struct Cyc_CfFlowInfo_Place*)(_tmp7A.PlaceL).val;_LL2C: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
return({struct _tuple16 _tmp7E;_tmp7E.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp76,env->all_changed,_tmp7C,nzval),_tmp77,_tmp78);_tmp7E.f2=
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp76,
env->all_changed,_tmp7C,(env->fenv)->zero),_tmp77,_tmp78);_tmp7E;});}_LL28:;}
_LL23:;}struct _tuple20{enum Cyc_Absyn_Primop f1;enum Cyc_Absyn_Primop f2;};static
union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow,struct Cyc_Absyn_Exp*e1,void*r1,enum Cyc_Absyn_Primop
p,void*r2){union Cyc_CfFlowInfo_FlowInfo _tmp7F=flow;int _tmp80;struct _tuple12
_tmp81;struct Cyc_Dict_Dict _tmp82;struct Cyc_List_List*_tmp83;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp84;_LL2E: if((_tmp7F.BottomFL).tag != 1)goto _LL30;_tmp80=(int)(_tmp7F.BottomFL).val;
_LL2F: return flow;_LL30: if((_tmp7F.ReachableFL).tag != 2)goto _LL2D;_tmp81=(struct
_tuple12)(_tmp7F.ReachableFL).val;_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;_tmp84=
_tmp81.f3;_LL31: {void*_tmp85=r2;struct Cyc_List_List*_tmp87;_LL33: {struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp86=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp85;if(_tmp86->tag != 6)goto _LL35;
else{_tmp87=_tmp86->f1;}}_LL34: {union Cyc_CfFlowInfo_AbsLVal _tmp88=(Cyc_NewControlFlow_anal_Lexp(
env,flow,e1)).f2;int _tmp89;struct Cyc_CfFlowInfo_Place*_tmp8A;_LL38: if((_tmp88.UnknownL).tag
!= 2)goto _LL3A;_tmp89=(int)(_tmp88.UnknownL).val;_LL39: return flow;_LL3A: if((
_tmp88.PlaceL).tag != 1)goto _LL37;_tmp8A=(struct Cyc_CfFlowInfo_Place*)(_tmp88.PlaceL).val;
_LL3B: {struct Cyc_List_List*new_cl;{void*_tmp8B=r1;struct Cyc_List_List*_tmp8D;
enum Cyc_CfFlowInfo_InitLevel _tmp8F;_LL3D: {struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp8C=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp8B;if(_tmp8C->tag != 6)goto _LL3F;
else{_tmp8D=_tmp8C->f1;}}_LL3E: new_cl=_tmp8D;goto _LL3C;_LL3F: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp8E=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp8B;if(_tmp8E->tag != 3)goto
_LL41;else{_tmp8F=_tmp8E->f1;if(_tmp8F != Cyc_CfFlowInfo_AllIL)goto _LL41;}}_LL40:
goto _LL42;_LL41: {struct Cyc_CfFlowInfo_Zero_struct*_tmp90=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp8B;if(_tmp90->tag != 0)goto _LL43;}_LL42: goto _LL44;_LL43: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp91=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp8B;if(_tmp91->tag != 1)goto
_LL45;}_LL44: new_cl=0;goto _LL3C;_LL45:;_LL46: return flow;_LL3C:;}for(0;_tmp87 != 0;
_tmp87=_tmp87->tl){enum Cyc_Absyn_Primop new_cmp;{struct _tuple20 _tmp93=({struct
_tuple20 _tmp92;_tmp92.f1=p;_tmp92.f2=((struct Cyc_CfFlowInfo_TagCmp*)_tmp87->hd)->cmp;
_tmp92;});enum Cyc_Absyn_Primop _tmp94;enum Cyc_Absyn_Primop _tmp95;enum Cyc_Absyn_Primop
_tmp96;enum Cyc_Absyn_Primop _tmp97;_LL48: _tmp94=_tmp93.f1;if(_tmp94 != Cyc_Absyn_Lt)
goto _LL4A;_LL49: goto _LL4B;_LL4A: _tmp95=_tmp93.f2;if(_tmp95 != Cyc_Absyn_Lt)goto
_LL4C;_LL4B: new_cmp=Cyc_Absyn_Lt;goto _LL47;_LL4C: _tmp96=_tmp93.f1;if(_tmp96 != 
Cyc_Absyn_Eq)goto _LL4E;_tmp97=_tmp93.f2;if(_tmp97 != Cyc_Absyn_Eq)goto _LL4E;_LL4D:
new_cmp=Cyc_Absyn_Eq;goto _LL47;_LL4E:;_LL4F: new_cmp=Cyc_Absyn_Lte;goto _LL47;
_LL47:;}new_cl=({struct Cyc_List_List*_tmp98=_region_malloc(env->r,sizeof(*_tmp98));
_tmp98->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp99=_region_malloc(env->r,sizeof(*
_tmp99));_tmp99->cmp=new_cmp;_tmp99->bd=((struct Cyc_CfFlowInfo_TagCmp*)_tmp87->hd)->bd;
_tmp99;});_tmp98->tl=new_cl;_tmp98;});}return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e1->loc,_tmp82,env->all_changed,_tmp8A,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp9A=_region_malloc(env->r,sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp9B;_tmp9B.tag=6;_tmp9B.f1=new_cl;_tmp9B;});_tmp9A;})),_tmp83,_tmp84);}_LL37:;}
_LL35:;_LL36: return flow;_LL32:;}_LL2D:;}static struct Cyc_CfFlowInfo_NotZero_struct
Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct
Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};static struct
_tuple13 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*
e,void*r){void*_tmp9E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmpA0;void*_tmpA1;struct Cyc_Absyn_PtrAtts
_tmpA2;union Cyc_Absyn_Constraint*_tmpA3;union Cyc_Absyn_Constraint*_tmpA4;_LL51: {
struct Cyc_Absyn_PointerType_struct*_tmp9F=(struct Cyc_Absyn_PointerType_struct*)
_tmp9E;if(_tmp9F->tag != 5)goto _LL53;else{_tmpA0=_tmp9F->f1;_tmpA1=_tmpA0.elt_typ;
_tmpA2=_tmpA0.ptr_atts;_tmpA3=_tmpA2.bounds;_tmpA4=_tmpA2.zero_term;}}_LL52: {
union Cyc_CfFlowInfo_FlowInfo _tmpA5=f;int _tmpA6;struct _tuple12 _tmpA7;struct Cyc_Dict_Dict
_tmpA8;struct Cyc_List_List*_tmpA9;_LL56: if((_tmpA5.BottomFL).tag != 1)goto _LL58;
_tmpA6=(int)(_tmpA5.BottomFL).val;_LL57: return({struct _tuple13 _tmpAA;_tmpAA.f1=f;
_tmpAA.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA1,(env->fenv)->unknown_all);
_tmpAA;});_LL58: if((_tmpA5.ReachableFL).tag != 2)goto _LL55;_tmpA7=(struct _tuple12)(
_tmpA5.ReachableFL).val;_tmpA8=_tmpA7.f1;_tmpA9=_tmpA7.f2;_LL59: if(Cyc_Tcutil_is_bound_one(
_tmpA3)){void*_tmpAB=r;struct Cyc_CfFlowInfo_Place*_tmpAF;enum Cyc_CfFlowInfo_InitLevel
_tmpB2;_LL5B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpAC=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpAB;if(_tmpAC->tag != 1)goto _LL5D;}_LL5C: goto _LL5E;_LL5D: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmpAD=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmpAB;if(_tmpAD->tag != 2)goto
_LL5F;}_LL5E:{void*_tmpB3=e->annot;struct Cyc_List_List*_tmpB5;_LL68: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmpB4=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmpB3;if(_tmpB4->tag != Cyc_CfFlowInfo_NotZero)
goto _LL6A;else{_tmpB5=_tmpB4->f1;}}_LL69: if(!Cyc_CfFlowInfo_same_relns(_tmpA9,
_tmpB5))goto _LL6B;goto _LL67;_LL6A:;_LL6B:{void*_tmpB6=e->r;_LL6D: {struct Cyc_Absyn_Subscript_e_struct*
_tmpB7=(struct Cyc_Absyn_Subscript_e_struct*)_tmpB6;if(_tmpB7->tag != 25)goto _LL6F;}
_LL6E: e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmpB8=_cycalloc(
sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmpB9;_tmpB9.tag=
Cyc_CfFlowInfo_NotZero;_tmpB9.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA9);_tmpB9;});_tmpB8;});goto _LL6C;_LL6F:;_LL70: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL6C;_LL6C:;}goto _LL67;_LL67:;}goto _LL5A;_LL5F: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpAE=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpAB;if(_tmpAE->tag != 5)goto
_LL61;else{_tmpAF=_tmpAE->f1;}}_LL60:{void*_tmpBA=e->annot;struct Cyc_List_List*
_tmpBC;_LL72: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpBB=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpBA;if(_tmpBB->tag != Cyc_CfFlowInfo_NotZero)goto _LL74;else{_tmpBC=_tmpBB->f1;}}
_LL73: if(!Cyc_CfFlowInfo_same_relns(_tmpA9,_tmpBC))goto _LL75;goto _LL71;_LL74:;
_LL75:{void*_tmpBD=e->r;_LL77: {struct Cyc_Absyn_Subscript_e_struct*_tmpBE=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpBD;if(_tmpBE->tag != 25)goto _LL79;}_LL78:
e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmpBF=_cycalloc(sizeof(*
_tmpBF));_tmpBF[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmpC0;_tmpC0.tag=Cyc_CfFlowInfo_NotZero;
_tmpC0.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpA9);_tmpC0;});
_tmpBF;});goto _LL76;_LL79:;_LL7A: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL76;_LL76:;}goto _LL71;_LL71:;}return({struct _tuple13 _tmpC1;_tmpC1.f1=f;
_tmpC1.f2=Cyc_CfFlowInfo_lookup_place(_tmpA8,_tmpAF);_tmpC1;});_LL61: {struct Cyc_CfFlowInfo_Zero_struct*
_tmpB0=(struct Cyc_CfFlowInfo_Zero_struct*)_tmpAB;if(_tmpB0->tag != 0)goto _LL63;}
_LL62: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;return({struct _tuple13 _tmpC2;
_tmpC2.f1=Cyc_CfFlowInfo_BottomFL();_tmpC2.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,
_tmpA1,(env->fenv)->unknown_all);_tmpC2;});_LL63: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpB1=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpAB;if(_tmpB1->tag != 3)goto
_LL65;else{_tmpB2=_tmpB1->f1;}}_LL64: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,
_tmpB2);goto _LL66;_LL65:;_LL66:{void*_tmpC3=e->r;_LL7C: {struct Cyc_Absyn_Subscript_e_struct*
_tmpC4=(struct Cyc_Absyn_Subscript_e_struct*)_tmpC3;if(_tmpC4->tag != 25)goto _LL7E;}
_LL7D: e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpC5=_cycalloc(
sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmpC6;_tmpC6.tag=
Cyc_CfFlowInfo_UnknownZ;_tmpC6.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA9);_tmpC6;});_tmpC5;});goto _LL7B;_LL7E:;_LL7F: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL7B;_LL7B:;}goto _LL5A;_LL5A:;}else{void*_tmpC7=e->annot;struct Cyc_List_List*
_tmpC9;_LL81: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpC8=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpC7;if(_tmpC8->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL83;else{_tmpC9=_tmpC8->f1;}}
_LL82: if(!Cyc_CfFlowInfo_same_relns(_tmpA9,_tmpC9))goto _LL84;goto _LL80;_LL83:;
_LL84: e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpCA=_cycalloc(
sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmpCB;_tmpCB.tag=
Cyc_CfFlowInfo_UnknownZ;_tmpCB.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA9);_tmpCB;});_tmpCA;});goto _LL80;_LL80:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpA8,r)){case Cyc_CfFlowInfo_NoneIL: _LL85:({void*_tmpCC=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmpCD="dereference of possibly uninitialized pointer";
_tag_dyneither(_tmpCD,sizeof(char),46);}),_tag_dyneither(_tmpCC,sizeof(void*),0));});
goto _LL86;case Cyc_CfFlowInfo_AllIL: _LL86: return({struct _tuple13 _tmpCE;_tmpCE.f1=
f;_tmpCE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA1,(env->fenv)->unknown_all);
_tmpCE;});case Cyc_CfFlowInfo_ThisIL: _LL87: return({struct _tuple13 _tmpCF;_tmpCF.f1=
f;_tmpCF.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA1,(env->fenv)->unknown_none);
_tmpCF;});}_LL55:;}_LL53:;_LL54:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Core_Impossible_struct
_tmpD1;_tmpD1.tag=Cyc_Core_Impossible;_tmpD1.f1=({const char*_tmpD2="right deref of non-pointer-type";
_tag_dyneither(_tmpD2,sizeof(char),32);});_tmpD1;});_tmpD0;}));_LL50:;}static
struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*
rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmpD3=e1->r;void*_tmpD5;struct Cyc_Absyn_Vardecl*_tmpD7;void*_tmpD9;struct
Cyc_Absyn_Vardecl*_tmpDB;void*_tmpDD;struct Cyc_Absyn_Vardecl*_tmpDF;void*_tmpE1;
struct Cyc_Absyn_Vardecl*_tmpE3;_LL8A: {struct Cyc_Absyn_Var_e_struct*_tmpD4=(
struct Cyc_Absyn_Var_e_struct*)_tmpD3;if(_tmpD4->tag != 1)goto _LL8C;else{_tmpD5=(
void*)_tmpD4->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpD6=(struct Cyc_Absyn_Pat_b_struct*)
_tmpD5;if(_tmpD6->tag != 5)goto _LL8C;else{_tmpD7=_tmpD6->f1;}}}}_LL8B: _tmpDB=
_tmpD7;goto _LL8D;_LL8C: {struct Cyc_Absyn_Var_e_struct*_tmpD8=(struct Cyc_Absyn_Var_e_struct*)
_tmpD3;if(_tmpD8->tag != 1)goto _LL8E;else{_tmpD9=(void*)_tmpD8->f2;{struct Cyc_Absyn_Local_b_struct*
_tmpDA=(struct Cyc_Absyn_Local_b_struct*)_tmpD9;if(_tmpDA->tag != 4)goto _LL8E;
else{_tmpDB=_tmpDA->f1;}}}}_LL8D: _tmpDF=_tmpDB;goto _LL8F;_LL8E: {struct Cyc_Absyn_Var_e_struct*
_tmpDC=(struct Cyc_Absyn_Var_e_struct*)_tmpD3;if(_tmpDC->tag != 1)goto _LL90;else{
_tmpDD=(void*)_tmpDC->f2;{struct Cyc_Absyn_Param_b_struct*_tmpDE=(struct Cyc_Absyn_Param_b_struct*)
_tmpDD;if(_tmpDE->tag != 3)goto _LL90;else{_tmpDF=_tmpDE->f1;}}}}_LL8F: _tmpE3=
_tmpDF;goto _LL91;_LL90: {struct Cyc_Absyn_Var_e_struct*_tmpE0=(struct Cyc_Absyn_Var_e_struct*)
_tmpD3;if(_tmpE0->tag != 1)goto _LL92;else{_tmpE1=(void*)_tmpE0->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpE2=(struct Cyc_Absyn_Global_b_struct*)_tmpE1;if(_tmpE2->tag != 1)goto _LL92;
else{_tmpE3=_tmpE2->f1;}}}}_LL91: if(!_tmpE3->escapes){void*_tmpE4=e2->r;void*
_tmpE6;struct Cyc_Absyn_Vardecl*_tmpE8;void*_tmpEA;struct Cyc_Absyn_Vardecl*_tmpEC;
void*_tmpEE;struct Cyc_Absyn_Vardecl*_tmpF0;void*_tmpF2;struct Cyc_Absyn_Vardecl*
_tmpF4;_LL95: {struct Cyc_Absyn_Var_e_struct*_tmpE5=(struct Cyc_Absyn_Var_e_struct*)
_tmpE4;if(_tmpE5->tag != 1)goto _LL97;else{_tmpE6=(void*)_tmpE5->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmpE7=(struct Cyc_Absyn_Pat_b_struct*)_tmpE6;if(_tmpE7->tag != 5)goto _LL97;else{
_tmpE8=_tmpE7->f1;}}}}_LL96: _tmpEC=_tmpE8;goto _LL98;_LL97: {struct Cyc_Absyn_Var_e_struct*
_tmpE9=(struct Cyc_Absyn_Var_e_struct*)_tmpE4;if(_tmpE9->tag != 1)goto _LL99;else{
_tmpEA=(void*)_tmpE9->f2;{struct Cyc_Absyn_Local_b_struct*_tmpEB=(struct Cyc_Absyn_Local_b_struct*)
_tmpEA;if(_tmpEB->tag != 4)goto _LL99;else{_tmpEC=_tmpEB->f1;}}}}_LL98: _tmpF0=
_tmpEC;goto _LL9A;_LL99: {struct Cyc_Absyn_Var_e_struct*_tmpED=(struct Cyc_Absyn_Var_e_struct*)
_tmpE4;if(_tmpED->tag != 1)goto _LL9B;else{_tmpEE=(void*)_tmpED->f2;{struct Cyc_Absyn_Param_b_struct*
_tmpEF=(struct Cyc_Absyn_Param_b_struct*)_tmpEE;if(_tmpEF->tag != 3)goto _LL9B;
else{_tmpF0=_tmpEF->f1;}}}}_LL9A: _tmpF4=_tmpF0;goto _LL9C;_LL9B: {struct Cyc_Absyn_Var_e_struct*
_tmpF1=(struct Cyc_Absyn_Var_e_struct*)_tmpE4;if(_tmpF1->tag != 1)goto _LL9D;else{
_tmpF2=(void*)_tmpF1->f2;{struct Cyc_Absyn_Global_b_struct*_tmpF3=(struct Cyc_Absyn_Global_b_struct*)
_tmpF2;if(_tmpF3->tag != 1)goto _LL9D;else{_tmpF4=_tmpF3->f1;}}}}_LL9C: if(!_tmpF4->escapes){
int found=0;{struct Cyc_List_List*_tmpF5=relns;for(0;_tmpF5 != 0;_tmpF5=_tmpF5->tl){
struct Cyc_CfFlowInfo_Reln*_tmpF6=(struct Cyc_CfFlowInfo_Reln*)_tmpF5->hd;if(
_tmpF6->vd == _tmpF4){union Cyc_CfFlowInfo_RelnOp _tmpF7=_tmpF6->rop;struct Cyc_Absyn_Vardecl*
_tmpF8;_LLA0: if((_tmpF7.LessNumelts).tag != 3)goto _LLA2;_tmpF8=(struct Cyc_Absyn_Vardecl*)(
_tmpF7.LessNumelts).val;if(!(_tmpF8 == _tmpE3))goto _LLA2;_LLA1: return relns;_LLA2:;
_LLA3: continue;_LL9F:;}}}if(!found)return({struct Cyc_List_List*_tmpF9=
_region_malloc(rgn,sizeof(*_tmpF9));_tmpF9->hd=({struct Cyc_CfFlowInfo_Reln*
_tmpFA=_region_malloc(rgn,sizeof(*_tmpFA));_tmpFA->vd=_tmpF4;_tmpFA->rop=Cyc_CfFlowInfo_LessNumelts(
_tmpE3);_tmpFA;});_tmpF9->tl=relns;_tmpF9;});}return relns;_LL9D:;_LL9E: return
relns;_LL94:;}else{return relns;}_LL92:;_LL93: return relns;_LL89:;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_may_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_CfFlowInfo_Place*place,void*t,struct
Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*_tmpFB=env->fenv;struct Cyc_List_List*
_tmpFC=Cyc_NewControlFlow_noalias_ptrs_rec(env,place,t);if(ps != 0)*ps=_tmpFC;if(
_tmpFC == 0)return cinfo;cinfo.may_consume=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(_tmpFB->r,Cyc_CfFlowInfo_place_cmp,
_tmpFC,cinfo.may_consume);return cinfo;}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e){struct _RegionHandle*_tmpFD=env->r;if(Cyc_Tcutil_is_noalias_path(
_tmpFD,e)){struct Cyc_CfFlowInfo_FlowEnv*_tmpFE=env->fenv;union Cyc_CfFlowInfo_AbsLVal
_tmp100;struct _tuple15 _tmpFF=Cyc_NewControlFlow_anal_Lexp(env,inflow,e);_tmp100=
_tmpFF.f2;{struct _tuple21 _tmp102=({struct _tuple21 _tmp101;_tmp101.f1=_tmp100;
_tmp101.f2=inflow;_tmp101;});union Cyc_CfFlowInfo_AbsLVal _tmp103;struct Cyc_CfFlowInfo_Place*
_tmp104;union Cyc_CfFlowInfo_FlowInfo _tmp105;struct _tuple12 _tmp106;struct Cyc_Dict_Dict
_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_CfFlowInfo_ConsumeInfo _tmp109;
_LLA5: _tmp103=_tmp102.f1;if((_tmp103.PlaceL).tag != 1)goto _LLA7;_tmp104=(struct
Cyc_CfFlowInfo_Place*)(_tmp103.PlaceL).val;_tmp105=_tmp102.f2;if((_tmp105.ReachableFL).tag
!= 2)goto _LLA7;_tmp106=(struct _tuple12)(_tmp105.ReachableFL).val;_tmp107=_tmp106.f1;
_tmp108=_tmp106.f2;_tmp109=_tmp106.f3;_LLA6: {struct Cyc_CfFlowInfo_ConsumeInfo
_tmp10A=Cyc_NewControlFlow_may_consume_place(env,_tmp109,_tmp104,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,0);return Cyc_CfFlowInfo_ReachableFL(
_tmp107,_tmp108,_tmp10A);}_LLA7:;_LLA8:({struct Cyc_String_pa_struct _tmp10D;
_tmp10D.tag=0;_tmp10D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp10B[1]={& _tmp10D};Cyc_fprintf(Cyc_stderr,({const char*_tmp10C="Oops---no location for noalias_path |%s|\n";
_tag_dyneither(_tmp10C,sizeof(char),42);}),_tag_dyneither(_tmp10B,sizeof(void*),
1));}});return inflow;_LLA4:;}}return inflow;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*
p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*
loc){if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
cinfo.consumed,p)){struct Cyc_CfFlowInfo_Place _tmp10F;void*_tmp110;struct Cyc_List_List*
_tmp111;struct Cyc_CfFlowInfo_Place*_tmp10E=p;_tmp10F=*_tmp10E;_tmp110=_tmp10F.root;
_tmp111=_tmp10F.fields;{void*rval=(env->fenv)->zero;if((((int(*)(struct Cyc_Dict_Dict
d,void*k,void**ans))Cyc_Dict_lookup_bool)(outdict,_tmp110,& rval) && Cyc_CfFlowInfo_initlevel(
env->fenv,outdict,rval)!= Cyc_CfFlowInfo_NoneIL) && rval != (env->fenv)->zero){
void*_tmp112=_tmp110;struct Cyc_Absyn_Vardecl*_tmp114;_LLAA: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp113=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp112;if(_tmp113->tag != 0)goto
_LLAC;else{_tmp114=_tmp113->f1;}}_LLAB: {struct _dyneither_ptr _tmp115=Cyc_Absynpp_qvar2string(
_tmp114->name);if(_tmp111 == 0)({struct Cyc_String_pa_struct _tmp118;_tmp118.tag=0;
_tmp118.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp115);{void*_tmp116[1]={&
_tmp118};Cyc_Tcutil_warn(loc,({const char*_tmp117="may clobber unique pointer %s";
_tag_dyneither(_tmp117,sizeof(char),30);}),_tag_dyneither(_tmp116,sizeof(void*),
1));}});else{({struct Cyc_String_pa_struct _tmp11B;_tmp11B.tag=0;_tmp11B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp115);{void*_tmp119[1]={& _tmp11B};
Cyc_Tcutil_warn(loc,({const char*_tmp11A="may clobber unique pointer contained in %s";
_tag_dyneither(_tmp11A,sizeof(char),43);}),_tag_dyneither(_tmp119,sizeof(void*),
1));}});}goto _LLA9;}_LLAC:;_LLAD:({void*_tmp11C=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp11D="warning locations not for VarRoots";
_tag_dyneither(_tmp11D,sizeof(char),35);}),_tag_dyneither(_tmp11C,sizeof(void*),
0));});_LLA9:;}}}if(env->all_changed == 0)cinfo.consumed=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)((
env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e){struct _RegionHandle*_tmp11E=env->r;if(Cyc_Tcutil_is_noalias_path(
_tmp11E,e)){struct Cyc_CfFlowInfo_FlowEnv*_tmp11F=env->fenv;struct Cyc_List_List*
_tmp120=0;cinfo=Cyc_NewControlFlow_may_consume_place(env,cinfo,p,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)& _tmp120);while(
_tmp120 != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp120))->hd,cinfo,outdict,e->loc);_tmp120=((
struct Cyc_List_List*)_check_null(_tmp120))->tl;}}return cinfo;}static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;struct Cyc_CfFlowInfo_Place
_tmp122;void*_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_CfFlowInfo_Place*
_tmp121=p;_tmp122=*_tmp121;_tmp123=_tmp122.root;_tmp124=_tmp122.fields;{int fld=0;
for(0;ts != 0;(fld ++,ts=ts->tl)){void*_tmp125=(void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(
_tmp125)){struct Cyc_List_List*_tmp126=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmp124,({int _tmp129[1];_tmp129[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmp129,sizeof(int),1));}));l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp127=_region_malloc(env->r,sizeof(*_tmp127));_tmp127->hd=({
struct Cyc_CfFlowInfo_Place*_tmp128=_region_malloc(env->r,sizeof(*_tmp128));
_tmp128->root=_tmp123;_tmp128->fields=_tmp126;_tmp128;});_tmp127->tl=0;_tmp127;}),
l);}else{if(Cyc_Absyn_is_aggr_type(_tmp125)){struct Cyc_List_List*_tmp12A=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(env->r,_tmp124,({int _tmp12D[1];_tmp12D[0]=fld;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmp12D,sizeof(int),1));}));struct Cyc_CfFlowInfo_Place*_tmp12B=({struct Cyc_CfFlowInfo_Place*
_tmp12C=_region_malloc(env->r,sizeof(*_tmp12C));_tmp12C->root=_tmp123;_tmp12C->fields=
_tmp12A;_tmp12C;});l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,
struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,l,Cyc_NewControlFlow_noalias_ptrs_rec(
env,_tmp12B,_tmp125));}}}}return l;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*
f2;};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(t))
return({struct Cyc_List_List*_tmp12E=_region_malloc(env->r,sizeof(*_tmp12E));
_tmp12E->hd=p;_tmp12E->tl=0;_tmp12E;});{void*_tmp12F=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp131;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_AggrInfo
_tmp135;union Cyc_Absyn_AggrInfoU _tmp136;struct Cyc_Absyn_Aggrdecl**_tmp137;struct
Cyc_List_List*_tmp138;struct Cyc_Absyn_AggrInfo _tmp13A;union Cyc_Absyn_AggrInfoU
_tmp13B;struct _tuple3 _tmp13C;struct Cyc_Absyn_DatatypeFieldInfo _tmp13F;union Cyc_Absyn_DatatypeFieldInfoU
_tmp140;struct Cyc_List_List*_tmp141;_LLAF: {struct Cyc_Absyn_TupleType_struct*
_tmp130=(struct Cyc_Absyn_TupleType_struct*)_tmp12F;if(_tmp130->tag != 11)goto
_LLB1;else{_tmp131=_tmp130->f1;}}_LLB0: {struct Cyc_List_List*_tmp142=0;while(
_tmp131 != 0){_tmp142=({struct Cyc_List_List*_tmp143=_region_malloc(env->r,sizeof(*
_tmp143));_tmp143->hd=(*((struct _tuple22*)_tmp131->hd)).f2;_tmp143->tl=_tmp142;
_tmp143;});_tmp131=_tmp131->tl;}_tmp142=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp142);return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,
_tmp142);}_LLB1: {struct Cyc_Absyn_AnonAggrType_struct*_tmp132=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp12F;if(_tmp132->tag != 13)goto _LLB3;else{_tmp133=_tmp132->f2;}}_LLB2: {struct
Cyc_List_List*_tmp144=0;while(_tmp133 != 0){_tmp144=({struct Cyc_List_List*_tmp145=
_region_malloc(env->r,sizeof(*_tmp145));_tmp145->hd=((struct Cyc_Absyn_Aggrfield*)
_tmp133->hd)->type;_tmp145->tl=_tmp144;_tmp145;});_tmp133=_tmp133->tl;}_tmp144=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp144);return
Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp144);}_LLB3: {struct Cyc_Absyn_AggrType_struct*
_tmp134=(struct Cyc_Absyn_AggrType_struct*)_tmp12F;if(_tmp134->tag != 12)goto _LLB5;
else{_tmp135=_tmp134->f1;_tmp136=_tmp135.aggr_info;if((_tmp136.KnownAggr).tag != 
2)goto _LLB5;_tmp137=(struct Cyc_Absyn_Aggrdecl**)(_tmp136.KnownAggr).val;_tmp138=
_tmp135.targs;}}_LLB4: if((*_tmp137)->impl == 0)return 0;else{struct Cyc_List_List*
_tmp146=0;struct _RegionHandle*_tmp147=env->r;{struct Cyc_List_List*_tmp148=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp147,_tmp147,(*_tmp137)->tvs,_tmp138);
struct Cyc_List_List*_tmp149=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*
_tmp137)->impl))->fields;while(_tmp149 != 0){_tmp146=({struct Cyc_List_List*
_tmp14A=_region_malloc(env->r,sizeof(*_tmp14A));_tmp14A->hd=Cyc_Tcutil_rsubstitute(
_tmp147,_tmp148,((struct Cyc_Absyn_Aggrfield*)_tmp149->hd)->type);_tmp14A->tl=
_tmp146;_tmp14A;});_tmp149=_tmp149->tl;}}_tmp146=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp146);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp146);}_LLB5: {struct Cyc_Absyn_AggrType_struct*_tmp139=(struct Cyc_Absyn_AggrType_struct*)
_tmp12F;if(_tmp139->tag != 12)goto _LLB7;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp13A.aggr_info;
if((_tmp13B.UnknownAggr).tag != 1)goto _LLB7;_tmp13C=(struct _tuple3)(_tmp13B.UnknownAggr).val;}}
_LLB6:({void*_tmp14B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp14C="got unknown aggr in noalias_ptrs_rec";
_tag_dyneither(_tmp14C,sizeof(char),37);}),_tag_dyneither(_tmp14B,sizeof(void*),
0));});_LLB7: {struct Cyc_Absyn_DatatypeType_struct*_tmp13D=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp12F;if(_tmp13D->tag != 3)goto _LLB9;}_LLB8: {struct _RegionHandle*_tmp14D=env->r;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp14D,t))return({struct Cyc_List_List*
_tmp14E=_region_malloc(env->r,sizeof(*_tmp14E));_tmp14E->hd=p;_tmp14E->tl=0;
_tmp14E;});else{return 0;}}_LLB9: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp13E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp12F;if(_tmp13E->tag != 4)
goto _LLBB;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13F.field_info;_tmp141=_tmp13F.targs;}}
_LLBA: {union Cyc_Absyn_DatatypeFieldInfoU _tmp14F=_tmp140;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmp150;struct _tuple2 _tmp151;struct Cyc_Absyn_Datatypedecl*_tmp152;struct Cyc_Absyn_Datatypefield*
_tmp153;_LLBE: if((_tmp14F.UnknownDatatypefield).tag != 1)goto _LLC0;_tmp150=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp14F.UnknownDatatypefield).val;
_LLBF:({void*_tmp154=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp155="got unknown datatype field in noalias_ptrs_rec";
_tag_dyneither(_tmp155,sizeof(char),47);}),_tag_dyneither(_tmp154,sizeof(void*),
0));});_LLC0: if((_tmp14F.KnownDatatypefield).tag != 2)goto _LLBD;_tmp151=(struct
_tuple2)(_tmp14F.KnownDatatypefield).val;_tmp152=_tmp151.f1;_tmp153=_tmp151.f2;
_LLC1: {struct Cyc_List_List*_tmp156=0;struct _RegionHandle*_tmp157=env->r;{struct
Cyc_List_List*_tmp158=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp157,_tmp157,_tmp152->tvs,_tmp141);struct Cyc_List_List*_tmp159=_tmp153->typs;
while(_tmp159 != 0){_tmp156=({struct Cyc_List_List*_tmp15A=_region_malloc(env->r,
sizeof(*_tmp15A));_tmp15A->hd=Cyc_Tcutil_rsubstitute(_tmp157,_tmp158,(*((struct
_tuple22*)_tmp159->hd)).f2);_tmp15A->tl=_tmp156;_tmp15A;});_tmp159=_tmp159->tl;}}
_tmp156=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp156);return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp156);}_LLBD:;}_LLBB:;
_LLBC: return 0;_LLAE:;}}static struct _tuple13 Cyc_NewControlFlow_do_assign(struct
Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_CfFlowInfo_ConsumeInfo outer_cinfo,struct Cyc_Position_Segment*
loc){outflow=Cyc_CfFlowInfo_consume_unique_rvals(fenv,loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp15B=outflow;int _tmp15C;struct _tuple12 _tmp15D;struct Cyc_Dict_Dict _tmp15E;
struct Cyc_List_List*_tmp15F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp160;_LLC3: if((
_tmp15B.BottomFL).tag != 1)goto _LLC5;_tmp15C=(int)(_tmp15B.BottomFL).val;_LLC4:
return({struct _tuple13 _tmp161;_tmp161.f1=Cyc_CfFlowInfo_BottomFL();_tmp161.f2=
rval;_tmp161;});_LLC5: if((_tmp15B.ReachableFL).tag != 2)goto _LLC2;_tmp15D=(struct
_tuple12)(_tmp15B.ReachableFL).val;_tmp15E=_tmp15D.f1;_tmp15F=_tmp15D.f2;_tmp160=
_tmp15D.f3;_LLC6: _tmp160=Cyc_CfFlowInfo_and_consume(fenv->r,outer_cinfo,_tmp160);{
union Cyc_CfFlowInfo_AbsLVal _tmp162=lval;struct Cyc_CfFlowInfo_Place*_tmp163;int
_tmp164;_LLC8: if((_tmp162.PlaceL).tag != 1)goto _LLCA;_tmp163=(struct Cyc_CfFlowInfo_Place*)(
_tmp162.PlaceL).val;_LLC9: _tmp160=Cyc_NewControlFlow_consume_assignment(env,
_tmp163,_tmp160,_tmp15E,lexp);_tmp15E=Cyc_CfFlowInfo_assign_place(fenv,loc,
_tmp15E,env->all_changed,_tmp163,rval);_tmp15F=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp15F,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp15E,_tmp15F,
_tmp160);Cyc_NewControlFlow_update_tryflow(env,outflow);return({struct _tuple13
_tmp165;_tmp165.f1=outflow;_tmp165.f2=rval;_tmp165;});_LLCA: if((_tmp162.UnknownL).tag
!= 2)goto _LLC7;_tmp164=(int)(_tmp162.UnknownL).val;_LLCB: return({struct _tuple13
_tmp166;_tmp166.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);
_tmp166.f2=rval;_tmp166;});_LLC7:;}_LLC2:;}}struct _tuple23{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp167=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo
_tmp168=inflow;int _tmp169;struct _tuple12 _tmp16A;struct Cyc_Dict_Dict _tmp16B;
struct Cyc_List_List*_tmp16C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp16D;_LLCD: if((
_tmp168.BottomFL).tag != 1)goto _LLCF;_tmp169=(int)(_tmp168.BottomFL).val;_LLCE:
return({struct _tuple13 _tmp16E;_tmp16E.f1=Cyc_CfFlowInfo_BottomFL();_tmp16E.f2=
_tmp167->unknown_all;_tmp16E;});_LLCF: if((_tmp168.ReachableFL).tag != 2)goto _LLCC;
_tmp16A=(struct _tuple12)(_tmp168.ReachableFL).val;_tmp16B=_tmp16A.f1;_tmp16C=
_tmp16A.f2;_tmp16D=_tmp16A.f3;_LLD0: d=_tmp16B;relns=_tmp16C;cinfo=_tmp16D;_LLCC:;}{
void*_tmp16F=e->r;struct Cyc_Absyn_Exp*_tmp171;enum Cyc_Absyn_Coercion _tmp172;
struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*
_tmp178;union Cyc_Absyn_Cnst _tmp17A;int _tmp17B;union Cyc_Absyn_Cnst _tmp17D;struct
_tuple6 _tmp17E;int _tmp17F;union Cyc_Absyn_Cnst _tmp181;struct _tuple6 _tmp182;void*
_tmp184;struct Cyc_List_List*_tmp187;void*_tmp190;void*_tmp193;struct Cyc_Absyn_Vardecl*
_tmp195;void*_tmp197;struct Cyc_Absyn_Vardecl*_tmp199;void*_tmp19B;struct Cyc_Absyn_Vardecl*
_tmp19D;enum Cyc_Absyn_Primop _tmp19F;struct Cyc_List_List*_tmp1A0;struct Cyc_Absyn_Exp*
_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Core_Opt*_tmp1A5;struct Cyc_Core_Opt
_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Core_Opt*
_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*
_tmp1AE;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_List_List*
_tmp1B3;struct Cyc_Absyn_MallocInfo _tmp1B5;int _tmp1B6;struct Cyc_Absyn_Exp*_tmp1B7;
void**_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B9;int _tmp1BA;struct Cyc_Absyn_Exp*_tmp1BC;
struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*
_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Absyn_Exp*
_tmp1C6;struct _dyneither_ptr*_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C9;struct
_dyneither_ptr*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1CC;struct _dyneither_ptr*_tmp1CD;
struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_Absyn_Exp*_tmp1D0;struct Cyc_Absyn_Exp*
_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D4;struct Cyc_Absyn_Exp*
_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D9;struct Cyc_Absyn_Exp*
_tmp1DA;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_Datatypedecl*_tmp1DD;struct
Cyc_List_List*_tmp1DF;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*_tmp1E3;
struct Cyc_Absyn_Aggrdecl*_tmp1E4;struct Cyc_Absyn_Aggrdecl _tmp1E5;enum Cyc_Absyn_AggrKind
_tmp1E6;struct Cyc_Absyn_AggrdeclImpl*_tmp1E7;struct Cyc_Absyn_AggrdeclImpl _tmp1E8;
struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1EC;struct Cyc_Absyn_Vardecl*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;int _tmp1F1;
struct Cyc_Absyn_Stmt*_tmp1F3;void*_tmp1F5;_LLD2: {struct Cyc_Absyn_Cast_e_struct*
_tmp170=(struct Cyc_Absyn_Cast_e_struct*)_tmp16F;if(_tmp170->tag != 15)goto _LLD4;
else{_tmp171=_tmp170->f2;_tmp172=_tmp170->f4;if(_tmp172 != Cyc_Absyn_NonNull_to_Null)
goto _LLD4;}}_LLD3: {union Cyc_CfFlowInfo_FlowInfo _tmp1FD;void*_tmp1FE;struct
_tuple13 _tmp1FC=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp171);_tmp1FD=_tmp1FC.f1;
_tmp1FE=_tmp1FC.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp200;void*_tmp201;struct
_tuple13 _tmp1FF=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1FD,_tmp171,
_tmp1FE);_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;return({struct _tuple13 _tmp202;
_tmp202.f1=_tmp200;_tmp202.f2=_tmp1FE;_tmp202;});}}_LLD4: {struct Cyc_Absyn_Cast_e_struct*
_tmp173=(struct Cyc_Absyn_Cast_e_struct*)_tmp16F;if(_tmp173->tag != 15)goto _LLD6;
else{_tmp174=_tmp173->f2;}}_LLD5: _tmp176=_tmp174;goto _LLD7;_LLD6: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp175=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp16F;if(_tmp175->tag != 13)
goto _LLD8;else{_tmp176=_tmp175->f1;}}_LLD7: _tmp178=_tmp176;goto _LLD9;_LLD8: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp177=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp16F;if(_tmp177->tag != 14)goto _LLDA;else{_tmp178=_tmp177->f1;}}_LLD9: return
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp178);_LLDA: {struct Cyc_Absyn_Const_e_struct*
_tmp179=(struct Cyc_Absyn_Const_e_struct*)_tmp16F;if(_tmp179->tag != 0)goto _LLDC;
else{_tmp17A=_tmp179->f1;if((_tmp17A.Null_c).tag != 1)goto _LLDC;_tmp17B=(int)(
_tmp17A.Null_c).val;}}_LLDB: goto _LLDD;_LLDC: {struct Cyc_Absyn_Const_e_struct*
_tmp17C=(struct Cyc_Absyn_Const_e_struct*)_tmp16F;if(_tmp17C->tag != 0)goto _LLDE;
else{_tmp17D=_tmp17C->f1;if((_tmp17D.Int_c).tag != 4)goto _LLDE;_tmp17E=(struct
_tuple6)(_tmp17D.Int_c).val;_tmp17F=_tmp17E.f2;if(_tmp17F != 0)goto _LLDE;}}_LLDD:
return({struct _tuple13 _tmp203;_tmp203.f1=inflow;_tmp203.f2=_tmp167->zero;_tmp203;});
_LLDE: {struct Cyc_Absyn_Const_e_struct*_tmp180=(struct Cyc_Absyn_Const_e_struct*)
_tmp16F;if(_tmp180->tag != 0)goto _LLE0;else{_tmp181=_tmp180->f1;if((_tmp181.Int_c).tag
!= 4)goto _LLE0;_tmp182=(struct _tuple6)(_tmp181.Int_c).val;}}_LLDF: goto _LLE1;
_LLE0: {struct Cyc_Absyn_Var_e_struct*_tmp183=(struct Cyc_Absyn_Var_e_struct*)
_tmp16F;if(_tmp183->tag != 1)goto _LLE2;else{_tmp184=(void*)_tmp183->f2;{struct Cyc_Absyn_Funname_b_struct*
_tmp185=(struct Cyc_Absyn_Funname_b_struct*)_tmp184;if(_tmp185->tag != 2)goto _LLE2;}}}
_LLE1: return({struct _tuple13 _tmp204;_tmp204.f1=inflow;_tmp204.f2=_tmp167->notzeroall;
_tmp204;});_LLE2: {struct Cyc_Absyn_Datatype_e_struct*_tmp186=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp16F;if(_tmp186->tag != 32)goto _LLE4;else{_tmp187=_tmp186->f1;if(_tmp187 != 0)
goto _LLE4;}}_LLE3: goto _LLE5;_LLE4: {struct Cyc_Absyn_Const_e_struct*_tmp188=(
struct Cyc_Absyn_Const_e_struct*)_tmp16F;if(_tmp188->tag != 0)goto _LLE6;}_LLE5:
goto _LLE7;_LLE6: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp189=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp16F;if(_tmp189->tag != 19)goto _LLE8;}_LLE7: goto _LLE9;_LLE8: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp18A=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp16F;if(_tmp18A->tag != 18)goto
_LLEA;}_LLE9: goto _LLEB;_LLEA: {struct Cyc_Absyn_Offsetof_e_struct*_tmp18B=(struct
Cyc_Absyn_Offsetof_e_struct*)_tmp16F;if(_tmp18B->tag != 20)goto _LLEC;}_LLEB: goto
_LLED;_LLEC: {struct Cyc_Absyn_Gentyp_e_struct*_tmp18C=(struct Cyc_Absyn_Gentyp_e_struct*)
_tmp16F;if(_tmp18C->tag != 21)goto _LLEE;}_LLED: goto _LLEF;_LLEE: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp18D=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp16F;if(_tmp18D->tag != 34)goto
_LLF0;}_LLEF: goto _LLF1;_LLF0: {struct Cyc_Absyn_Enum_e_struct*_tmp18E=(struct Cyc_Absyn_Enum_e_struct*)
_tmp16F;if(_tmp18E->tag != 33)goto _LLF2;}_LLF1: return({struct _tuple13 _tmp205;
_tmp205.f1=inflow;_tmp205.f2=_tmp167->unknown_all;_tmp205;});_LLF2: {struct Cyc_Absyn_Var_e_struct*
_tmp18F=(struct Cyc_Absyn_Var_e_struct*)_tmp16F;if(_tmp18F->tag != 1)goto _LLF4;
else{_tmp190=(void*)_tmp18F->f2;{struct Cyc_Absyn_Global_b_struct*_tmp191=(struct
Cyc_Absyn_Global_b_struct*)_tmp190;if(_tmp191->tag != 1)goto _LLF4;}}}_LLF3: return({
struct _tuple13 _tmp206;_tmp206.f1=inflow;_tmp206.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp167,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp167->unknown_all);
_tmp206;});_LLF4: {struct Cyc_Absyn_Var_e_struct*_tmp192=(struct Cyc_Absyn_Var_e_struct*)
_tmp16F;if(_tmp192->tag != 1)goto _LLF6;else{_tmp193=(void*)_tmp192->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp194=(struct Cyc_Absyn_Param_b_struct*)_tmp193;if(_tmp194->tag != 3)goto _LLF6;
else{_tmp195=_tmp194->f1;}}}}_LLF5: _tmp199=_tmp195;goto _LLF7;_LLF6: {struct Cyc_Absyn_Var_e_struct*
_tmp196=(struct Cyc_Absyn_Var_e_struct*)_tmp16F;if(_tmp196->tag != 1)goto _LLF8;
else{_tmp197=(void*)_tmp196->f2;{struct Cyc_Absyn_Local_b_struct*_tmp198=(struct
Cyc_Absyn_Local_b_struct*)_tmp197;if(_tmp198->tag != 4)goto _LLF8;else{_tmp199=
_tmp198->f1;}}}}_LLF7: _tmp19D=_tmp199;goto _LLF9;_LLF8: {struct Cyc_Absyn_Var_e_struct*
_tmp19A=(struct Cyc_Absyn_Var_e_struct*)_tmp16F;if(_tmp19A->tag != 1)goto _LLFA;
else{_tmp19B=(void*)_tmp19A->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp19C=(struct Cyc_Absyn_Pat_b_struct*)
_tmp19B;if(_tmp19C->tag != 5)goto _LLFA;else{_tmp19D=_tmp19C->f1;}}}}_LLF9: inflow=
Cyc_NewControlFlow_may_consume_exp(env,inflow,e);return({struct _tuple13 _tmp207;
_tmp207.f1=inflow;_tmp207.f2=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp208=_region_malloc(env->r,
sizeof(*_tmp208));_tmp208[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp209;
_tmp209.tag=0;_tmp209.f1=_tmp19D;_tmp209;});_tmp208;}));_tmp207;});_LLFA: {
struct Cyc_Absyn_Primop_e_struct*_tmp19E=(struct Cyc_Absyn_Primop_e_struct*)
_tmp16F;if(_tmp19E->tag != 3)goto _LLFC;else{_tmp19F=_tmp19E->f1;_tmp1A0=_tmp19E->f2;}}
_LLFB: {union Cyc_CfFlowInfo_FlowInfo _tmp20B;void*_tmp20C;struct _tuple13 _tmp20A=
Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1A0,0);_tmp20B=_tmp20A.f1;
_tmp20C=_tmp20A.f2;switch(_tmp19F){case Cyc_Absyn_Plus: _LL13C: goto _LL13D;case Cyc_Absyn_Minus:
_LL13D: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1A0))->hd)->loc,_tmp20B);break;default: _LL13E: _tmp20B=Cyc_CfFlowInfo_readthrough_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1A0))->hd)->loc,
_tmp20B);break;}return({struct _tuple13 _tmp20D;_tmp20D.f1=_tmp20B;_tmp20D.f2=
_tmp20C;_tmp20D;});}_LLFC: {struct Cyc_Absyn_Increment_e_struct*_tmp1A1=(struct
Cyc_Absyn_Increment_e_struct*)_tmp16F;if(_tmp1A1->tag != 5)goto _LLFE;else{_tmp1A2=
_tmp1A1->f1;}}_LLFD: {struct Cyc_List_List _tmp20E=({struct Cyc_List_List _tmp21D;
_tmp21D.hd=_tmp1A2;_tmp21D.tl=0;_tmp21D;});union Cyc_CfFlowInfo_FlowInfo _tmp210;
struct _tuple13 _tmp20F=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)&
_tmp20E,0);_tmp210=_tmp20F.f1;Cyc_CfFlowInfo_check_unique_rvals(_tmp1A2->loc,
_tmp210);{union Cyc_CfFlowInfo_AbsLVal _tmp212;struct _tuple15 _tmp211=Cyc_NewControlFlow_anal_Lexp(
env,_tmp210,_tmp1A2);_tmp212=_tmp211.f2;{struct _tuple21 _tmp214=({struct _tuple21
_tmp213;_tmp213.f1=_tmp212;_tmp213.f2=_tmp210;_tmp213;});union Cyc_CfFlowInfo_AbsLVal
_tmp215;struct Cyc_CfFlowInfo_Place*_tmp216;union Cyc_CfFlowInfo_FlowInfo _tmp217;
struct _tuple12 _tmp218;struct Cyc_Dict_Dict _tmp219;struct Cyc_List_List*_tmp21A;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp21B;_LL141: _tmp215=_tmp214.f1;if((_tmp215.PlaceL).tag
!= 1)goto _LL143;_tmp216=(struct Cyc_CfFlowInfo_Place*)(_tmp215.PlaceL).val;
_tmp217=_tmp214.f2;if((_tmp217.ReachableFL).tag != 2)goto _LL143;_tmp218=(struct
_tuple12)(_tmp217.ReachableFL).val;_tmp219=_tmp218.f1;_tmp21A=_tmp218.f2;_tmp21B=
_tmp218.f3;_LL142: _tmp21A=Cyc_CfFlowInfo_reln_kill_exp(_tmp167->r,_tmp21A,
_tmp1A2);_tmp210=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp167,
_tmp1A2->loc,_tmp219,env->all_changed,_tmp216,_tmp167->unknown_all),_tmp21A,
_tmp21B);goto _LL140;_LL143:;_LL144: goto _LL140;_LL140:;}return({struct _tuple13
_tmp21C;_tmp21C.f1=_tmp210;_tmp21C.f2=_tmp167->unknown_all;_tmp21C;});}}_LLFE: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp1A3=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp16F;if(_tmp1A3->tag != 4)goto _LL100;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A3->f2;
if(_tmp1A5 == 0)goto _LL100;_tmp1A6=*_tmp1A5;_tmp1A7=_tmp1A3->f3;}}_LLFF: {struct
Cyc_List_List _tmp21E=({struct Cyc_List_List _tmp22E;_tmp22E.hd=_tmp1A7;_tmp22E.tl=
0;_tmp22E;});struct Cyc_List_List _tmp21F=({struct Cyc_List_List _tmp22D;_tmp22D.hd=
_tmp1A4;_tmp22D.tl=(struct Cyc_List_List*)& _tmp21E;_tmp22D;});union Cyc_CfFlowInfo_FlowInfo
_tmp221;struct _tuple13 _tmp220=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp21F,1);_tmp221=_tmp220.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp223;struct _tuple15 _tmp222=Cyc_NewControlFlow_anal_Lexp(env,_tmp221,_tmp1A4);
_tmp223=_tmp222.f2;_tmp221=Cyc_CfFlowInfo_consume_unique_rvals(_tmp167,e->loc,
_tmp221);{union Cyc_CfFlowInfo_FlowInfo _tmp224=_tmp221;struct _tuple12 _tmp225;
struct Cyc_Dict_Dict _tmp226;struct Cyc_List_List*_tmp227;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp228;_LL146: if((_tmp224.ReachableFL).tag != 2)goto _LL148;_tmp225=(struct
_tuple12)(_tmp224.ReachableFL).val;_tmp226=_tmp225.f1;_tmp227=_tmp225.f2;_tmp228=
_tmp225.f3;_LL147:{union Cyc_CfFlowInfo_AbsLVal _tmp229=_tmp223;struct Cyc_CfFlowInfo_Place*
_tmp22A;int _tmp22B;_LL14B: if((_tmp229.PlaceL).tag != 1)goto _LL14D;_tmp22A=(struct
Cyc_CfFlowInfo_Place*)(_tmp229.PlaceL).val;_LL14C: _tmp228=Cyc_NewControlFlow_consume_assignment(
env,_tmp22A,_tmp228,_tmp226,_tmp1A4);_tmp227=Cyc_CfFlowInfo_reln_kill_exp(
_tmp167->r,_tmp227,_tmp1A4);_tmp226=Cyc_CfFlowInfo_assign_place(_tmp167,_tmp1A4->loc,
_tmp226,env->all_changed,_tmp22A,_tmp167->unknown_all);_tmp221=Cyc_CfFlowInfo_ReachableFL(
_tmp226,_tmp227,_tmp228);goto _LL14A;_LL14D: if((_tmp229.UnknownL).tag != 2)goto
_LL14A;_tmp22B=(int)(_tmp229.UnknownL).val;_LL14E: goto _LL14A;_LL14A:;}goto _LL145;
_LL148:;_LL149: goto _LL145;_LL145:;}return({struct _tuple13 _tmp22C;_tmp22C.f1=
_tmp221;_tmp22C.f2=_tmp167->unknown_all;_tmp22C;});}}_LL100: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp1A8=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp16F;if(_tmp1A8->tag != 4)goto
_LL102;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;if(_tmp1AA != 0)goto _LL102;
_tmp1AB=_tmp1A8->f3;}}_LL101: {struct Cyc_Dict_Dict*_tmp22F=env->all_changed;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp231;union Cyc_CfFlowInfo_FlowInfo _tmp232;
struct _tuple14 _tmp230=Cyc_CfFlowInfo_save_consume_info(_tmp167,inflow,1);_tmp231=
_tmp230.f1;_tmp232=_tmp230.f2;{struct Cyc_CfFlowInfo_ConsumeInfo empty_consume=({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp24D;_tmp24D.consumed=_tmp167->mt_place_set;
_tmp24D.may_consume=0;_tmp24D;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp233=_region_malloc(env->r,sizeof(*_tmp233));_tmp233[0]=_tmp167->mt_place_set;
_tmp233;});{union Cyc_CfFlowInfo_FlowInfo _tmp235;union Cyc_CfFlowInfo_AbsLVal
_tmp236;struct _tuple15 _tmp234=Cyc_NewControlFlow_anal_Lexp(env,_tmp232,_tmp1A9);
_tmp235=_tmp234.f1;_tmp236=_tmp234.f2;{struct Cyc_Dict_Dict _tmp237=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp238=
_region_malloc(env->r,sizeof(*_tmp238));_tmp238[0]=_tmp167->mt_place_set;_tmp238;});{
union Cyc_CfFlowInfo_FlowInfo _tmp23A;void*_tmp23B;struct _tuple13 _tmp239=Cyc_NewControlFlow_anal_Rexp(
env,_tmp232,_tmp1AB);_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;{struct Cyc_Dict_Dict
_tmp23C=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo
_tmp23D=Cyc_CfFlowInfo_after_flow(_tmp167,(struct Cyc_Dict_Dict*)& _tmp237,_tmp235,
_tmp23A,_tmp237,_tmp23C);union Cyc_CfFlowInfo_FlowInfo _tmp23E=Cyc_CfFlowInfo_join_flow(
_tmp167,_tmp22F,_tmp232,_tmp23D,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp240;
struct _tuple14 _tmp23F=Cyc_CfFlowInfo_save_consume_info(_tmp167,_tmp23E,0);
_tmp240=_tmp23F.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
outflow_consume,_tmp240)){({void*_tmp241=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp242="sanity consumed: ";_tag_dyneither(_tmp242,sizeof(char),18);}),
_tag_dyneither(_tmp241,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp243=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp244="\ncurrent consumed: ";_tag_dyneither(_tmp244,sizeof(char),20);}),
_tag_dyneither(_tmp243,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp240.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp245=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp246="\nsanity may_consume: ";_tag_dyneither(_tmp246,sizeof(char),22);}),
_tag_dyneither(_tmp245,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(outflow_consume.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp247=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp248="\ncurrent may_consume: ";_tag_dyneither(_tmp248,sizeof(char),23);}),
_tag_dyneither(_tmp247,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(_tmp240.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp249=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp24A="\n";_tag_dyneither(_tmp24A,sizeof(char),2);}),_tag_dyneither(_tmp249,
sizeof(void*),0));});({void*_tmp24B=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp24C="assignOp_e failed to preserve consume info";
_tag_dyneither(_tmp24C,sizeof(char),43);}),_tag_dyneither(_tmp24B,sizeof(void*),
0));});}_tmp232=Cyc_CfFlowInfo_restore_consume_info(_tmp232,outflow_consume,0);}
outflow_consume=_tmp240;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp23E,_tmp232)){if(_tmp22F == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp22F,Cyc_CfFlowInfo_union_place_set(
_tmp237,_tmp23C,0),0);}return Cyc_NewControlFlow_do_assign(_tmp167,env,_tmp23D,
_tmp1A9,_tmp236,_tmp1AB,_tmp23B,_tmp231,e->loc);}_tmp232=Cyc_CfFlowInfo_restore_consume_info(
_tmp23E,empty_consume,0);}}}}}}}_LL102: {struct Cyc_Absyn_SeqExp_e_struct*_tmp1AC=(
struct Cyc_Absyn_SeqExp_e_struct*)_tmp16F;if(_tmp1AC->tag != 9)goto _LL104;else{
_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;}}_LL103: {union Cyc_CfFlowInfo_FlowInfo
_tmp24F;void*_tmp250;struct _tuple13 _tmp24E=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1AD);_tmp24F=_tmp24E.f1;_tmp250=_tmp24E.f2;_tmp24F=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp1AD->loc,_tmp24F);return Cyc_NewControlFlow_anal_Rexp(env,_tmp24F,_tmp1AE);}
_LL104: {struct Cyc_Absyn_Throw_e_struct*_tmp1AF=(struct Cyc_Absyn_Throw_e_struct*)
_tmp16F;if(_tmp1AF->tag != 12)goto _LL106;else{_tmp1B0=_tmp1AF->f1;}}_LL105: {
union Cyc_CfFlowInfo_FlowInfo _tmp252;void*_tmp253;struct _tuple13 _tmp251=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1B0);_tmp252=_tmp251.f1;_tmp253=_tmp251.f2;_tmp252=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp167,_tmp1B0->loc,_tmp252);Cyc_NewControlFlow_use_Rval(env,_tmp1B0->loc,
_tmp252,_tmp253);return({struct _tuple13 _tmp254;_tmp254.f1=Cyc_CfFlowInfo_BottomFL();
_tmp254.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp167->unknown_all);_tmp254;});}_LL106: {struct Cyc_Absyn_FnCall_e_struct*
_tmp1B1=(struct Cyc_Absyn_FnCall_e_struct*)_tmp16F;if(_tmp1B1->tag != 11)goto
_LL108;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=_tmp1B1->f2;}}_LL107: {struct
_RegionHandle*_tmp255=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp258;struct Cyc_List_List*
_tmp259;struct _tuple19 _tmp257=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp255,
env,inflow,({struct Cyc_List_List*_tmp256=_region_malloc(_tmp255,sizeof(*_tmp256));
_tmp256->hd=_tmp1B2;_tmp256->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(_tmp255,_tmp1B3);_tmp256;}),1);_tmp258=
_tmp257.f1;_tmp259=_tmp257.f2;_tmp258=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp167,e->loc,_tmp258);{union Cyc_CfFlowInfo_FlowInfo _tmp25A=Cyc_NewControlFlow_use_Rval(
env,_tmp1B2->loc,_tmp258,(void*)((struct Cyc_List_List*)_check_null(_tmp259))->hd);
_tmp259=_tmp259->tl;{struct Cyc_List_List*init_params=0;{void*_tmp25B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B2->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp25D;void*_tmp25E;_LL150: {struct Cyc_Absyn_PointerType_struct*_tmp25C=(struct
Cyc_Absyn_PointerType_struct*)_tmp25B;if(_tmp25C->tag != 5)goto _LL152;else{
_tmp25D=_tmp25C->f1;_tmp25E=_tmp25D.elt_typ;}}_LL151:{void*_tmp25F=Cyc_Tcutil_compress(
_tmp25E);struct Cyc_Absyn_FnInfo _tmp261;struct Cyc_List_List*_tmp262;_LL155: {
struct Cyc_Absyn_FnType_struct*_tmp260=(struct Cyc_Absyn_FnType_struct*)_tmp25F;
if(_tmp260->tag != 10)goto _LL157;else{_tmp261=_tmp260->f1;_tmp262=_tmp261.attributes;}}
_LL156: for(0;_tmp262 != 0;_tmp262=_tmp262->tl){void*_tmp263=(void*)_tmp262->hd;
int _tmp265;_LL15A: {struct Cyc_Absyn_Initializes_att_struct*_tmp264=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp263;if(_tmp264->tag != 20)goto _LL15C;else{_tmp265=_tmp264->f1;}}_LL15B:
init_params=({struct Cyc_List_List*_tmp266=_region_malloc(_tmp255,sizeof(*_tmp266));
_tmp266->hd=(void*)_tmp265;_tmp266->tl=init_params;_tmp266;});goto _LL159;_LL15C:;
_LL15D: goto _LL159;_LL159:;}goto _LL154;_LL157:;_LL158:({void*_tmp267=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp268="anal_Rexp: bad function type";_tag_dyneither(_tmp268,sizeof(char),29);}),
_tag_dyneither(_tmp267,sizeof(void*),0));});_LL154:;}goto _LL14F;_LL152:;_LL153:({
void*_tmp269=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp26A="anal_Rexp: bad function type";_tag_dyneither(_tmp26A,sizeof(
char),29);}),_tag_dyneither(_tmp269,sizeof(void*),0));});_LL14F:;}{int i=1;for(0;
_tmp259 != 0;(((_tmp259=_tmp259->tl,_tmp1B3=((struct Cyc_List_List*)_check_null(
_tmp1B3))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
init_params,i)){_tmp25A=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1B3))->hd)->loc,_tmp25A,(void*)_tmp259->hd);
continue;}{union Cyc_CfFlowInfo_FlowInfo _tmp26B=_tmp258;int _tmp26C;struct _tuple12
_tmp26D;struct Cyc_Dict_Dict _tmp26E;_LL15F: if((_tmp26B.BottomFL).tag != 1)goto
_LL161;_tmp26C=(int)(_tmp26B.BottomFL).val;_LL160: goto _LL15E;_LL161: if((_tmp26B.ReachableFL).tag
!= 2)goto _LL15E;_tmp26D=(struct _tuple12)(_tmp26B.ReachableFL).val;_tmp26E=
_tmp26D.f1;_LL162: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp26E,(void*)_tmp259->hd)
== Cyc_CfFlowInfo_NoneIL)({void*_tmp26F=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1B3))->hd)->loc,({const char*_tmp270="expression may not be initialized";
_tag_dyneither(_tmp270,sizeof(char),34);}),_tag_dyneither(_tmp26F,sizeof(void*),
0));});{union Cyc_CfFlowInfo_FlowInfo _tmp271=_tmp25A;int _tmp272;struct _tuple12
_tmp273;struct Cyc_Dict_Dict _tmp274;struct Cyc_List_List*_tmp275;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp276;_LL164: if((_tmp271.BottomFL).tag != 1)goto _LL166;_tmp272=(int)(_tmp271.BottomFL).val;
_LL165: goto _LL163;_LL166: if((_tmp271.ReachableFL).tag != 2)goto _LL163;_tmp273=(
struct _tuple12)(_tmp271.ReachableFL).val;_tmp274=_tmp273.f1;_tmp275=_tmp273.f2;
_tmp276=_tmp273.f3;_LL167: {struct Cyc_Dict_Dict _tmp277=Cyc_CfFlowInfo_escape_deref(
_tmp167,_tmp274,env->all_changed,(void*)_tmp259->hd);{void*_tmp278=(void*)
_tmp259->hd;struct Cyc_CfFlowInfo_Place*_tmp27A;_LL169: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp279=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp278;if(_tmp279->tag != 5)goto
_LL16B;else{_tmp27A=_tmp279->f1;}}_LL16A:{void*_tmp27B=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1B3))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp27D;void*
_tmp27E;_LL16E: {struct Cyc_Absyn_PointerType_struct*_tmp27C=(struct Cyc_Absyn_PointerType_struct*)
_tmp27B;if(_tmp27C->tag != 5)goto _LL170;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27D.elt_typ;}}
_LL16F: _tmp277=Cyc_CfFlowInfo_assign_place(_tmp167,((struct Cyc_Absyn_Exp*)
_tmp1B3->hd)->loc,_tmp277,env->all_changed,_tmp27A,Cyc_CfFlowInfo_typ_to_absrval(
_tmp167,_tmp27E,_tmp167->esc_all));goto _LL16D;_LL170:;_LL171:({void*_tmp27F=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp280="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp280,
sizeof(char),39);}),_tag_dyneither(_tmp27F,sizeof(void*),0));});_LL16D:;}goto
_LL168;_LL16B:;_LL16C: goto _LL168;_LL168:;}_tmp25A=Cyc_CfFlowInfo_ReachableFL(
_tmp277,_tmp275,_tmp276);goto _LL163;}_LL163:;}goto _LL15E;_LL15E:;}}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B2->topt))->v))return({struct
_tuple13 _tmp281;_tmp281.f1=Cyc_CfFlowInfo_BottomFL();_tmp281.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp167,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp167->unknown_all);
_tmp281;});else{return({struct _tuple13 _tmp282;_tmp282.f1=_tmp25A;_tmp282.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp167,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp167->unknown_all);
_tmp282;});}}}}_LL108: {struct Cyc_Absyn_Malloc_e_struct*_tmp1B4=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp16F;if(_tmp1B4->tag != 35)goto _LL10A;else{_tmp1B5=_tmp1B4->f1;_tmp1B6=_tmp1B5.is_calloc;
_tmp1B7=_tmp1B5.rgn;_tmp1B8=_tmp1B5.elt_type;_tmp1B9=_tmp1B5.num_elts;_tmp1BA=
_tmp1B5.fat_result;}}_LL109: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp295=_region_malloc(_tmp167->r,sizeof(*_tmp295));_tmp295[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp296;_tmp296.tag=1;_tmp296.f1=_tmp1B9;_tmp296.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp296;});_tmp295;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp294=_region_malloc(_tmp167->r,sizeof(*_tmp294));
_tmp294->root=root;_tmp294->fields=0;_tmp294;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp292=_region_malloc(_tmp167->r,sizeof(*_tmp292));_tmp292[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp293;_tmp293.tag=5;_tmp293.f1=place;_tmp293;});_tmp292;});void*place_val=
_tmp1BA?_tmp167->notzeroall: Cyc_CfFlowInfo_typ_to_absrval(_tmp167,*((void**)
_check_null(_tmp1B8)),_tmp167->unknown_none);union Cyc_CfFlowInfo_FlowInfo outflow;((
int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);if(_tmp1B7 != 0){
struct _RegionHandle*_tmp283=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp286;struct Cyc_List_List*
_tmp287;struct _tuple19 _tmp285=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp283,
env,inflow,({struct Cyc_Absyn_Exp*_tmp284[2];_tmp284[1]=_tmp1B9;_tmp284[0]=(
struct Cyc_Absyn_Exp*)_tmp1B7;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(_tmp283,_tag_dyneither(_tmp284,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;outflow=
_tmp286;}else{union Cyc_CfFlowInfo_FlowInfo _tmp289;struct _tuple13 _tmp288=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1B9);_tmp289=_tmp288.f1;outflow=_tmp289;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1B9->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo _tmp28A=outflow;int _tmp28B;
struct _tuple12 _tmp28C;struct Cyc_Dict_Dict _tmp28D;struct Cyc_List_List*_tmp28E;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp28F;_LL173: if((_tmp28A.BottomFL).tag != 1)
goto _LL175;_tmp28B=(int)(_tmp28A.BottomFL).val;_LL174: return({struct _tuple13
_tmp290;_tmp290.f1=outflow;_tmp290.f2=rval;_tmp290;});_LL175: if((_tmp28A.ReachableFL).tag
!= 2)goto _LL172;_tmp28C=(struct _tuple12)(_tmp28A.ReachableFL).val;_tmp28D=
_tmp28C.f1;_tmp28E=_tmp28C.f2;_tmp28F=_tmp28C.f3;_LL176: return({struct _tuple13
_tmp291;_tmp291.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,void*v))Cyc_Dict_insert)(_tmp28D,root,place_val),_tmp28E,_tmp28F);
_tmp291.f2=rval;_tmp291;});_LL172:;}}_LL10A: {struct Cyc_Absyn_Swap_e_struct*
_tmp1BB=(struct Cyc_Absyn_Swap_e_struct*)_tmp16F;if(_tmp1BB->tag != 36)goto _LL10C;
else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;}}_LL10B: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp297=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp29A;struct Cyc_List_List*_tmp29B;struct _tuple19
_tmp299=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp297,env,inflow,({struct Cyc_Absyn_Exp*
_tmp298[2];_tmp298[1]=_tmp1BD;_tmp298[0]=_tmp1BC;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp297,
_tag_dyneither(_tmp298,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp29A=_tmp299.f1;
_tmp29B=_tmp299.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp29B))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp29B->tl))->hd;outflow=
_tmp29A;}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,outflow);{union Cyc_CfFlowInfo_AbsLVal
_tmp29D;struct _tuple15 _tmp29C=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp1BC);
_tmp29D=_tmp29C.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp29F;struct _tuple15 _tmp29E=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp1BD);_tmp29F=_tmp29E.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2A0=
outflow;struct _tuple12 _tmp2A1;struct Cyc_Dict_Dict _tmp2A2;struct Cyc_List_List*
_tmp2A3;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2A4;_LL178: if((_tmp2A0.ReachableFL).tag
!= 2)goto _LL17A;_tmp2A1=(struct _tuple12)(_tmp2A0.ReachableFL).val;_tmp2A2=
_tmp2A1.f1;_tmp2A3=_tmp2A1.f2;_tmp2A4=_tmp2A1.f3;_LL179:{union Cyc_CfFlowInfo_AbsLVal
_tmp2A5=_tmp29D;struct Cyc_CfFlowInfo_Place*_tmp2A6;int _tmp2A7;_LL17D: if((_tmp2A5.PlaceL).tag
!= 1)goto _LL17F;_tmp2A6=(struct Cyc_CfFlowInfo_Place*)(_tmp2A5.PlaceL).val;_LL17E:
_tmp2A2=Cyc_CfFlowInfo_assign_place(_tmp167,_tmp1BC->loc,_tmp2A2,env->all_changed,
_tmp2A6,right_rval);goto _LL17C;_LL17F: if((_tmp2A5.UnknownL).tag != 2)goto _LL17C;
_tmp2A7=(int)(_tmp2A5.UnknownL).val;_LL180: goto _LL17C;_LL17C:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp2A8=_tmp29F;struct Cyc_CfFlowInfo_Place*_tmp2A9;int _tmp2AA;_LL182: if((_tmp2A8.PlaceL).tag
!= 1)goto _LL184;_tmp2A9=(struct Cyc_CfFlowInfo_Place*)(_tmp2A8.PlaceL).val;_LL183:
_tmp2A2=Cyc_CfFlowInfo_assign_place(_tmp167,_tmp1BD->loc,_tmp2A2,env->all_changed,
_tmp2A9,left_rval);goto _LL181;_LL184: if((_tmp2A8.UnknownL).tag != 2)goto _LL181;
_tmp2AA=(int)(_tmp2A8.UnknownL).val;_LL185: goto _LL181;_LL181:;}_tmp2A3=Cyc_CfFlowInfo_reln_kill_exp(
_tmp167->r,_tmp2A3,_tmp1BC);_tmp2A3=Cyc_CfFlowInfo_reln_kill_exp(_tmp167->r,
_tmp2A3,_tmp1BD);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2A2,_tmp2A3,_tmp2A4);
goto _LL177;_LL17A:;_LL17B: goto _LL177;_LL177:;}return({struct _tuple13 _tmp2AB;
_tmp2AB.f1=outflow;_tmp2AB.f2=_tmp167->unknown_all;_tmp2AB;});}}}_LL10C: {struct
Cyc_Absyn_New_e_struct*_tmp1BE=(struct Cyc_Absyn_New_e_struct*)_tmp16F;if(_tmp1BE->tag
!= 17)goto _LL10E;else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BE->f2;}}_LL10D: {void*
root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*_tmp2BF=_region_malloc(
_tmp167->r,sizeof(*_tmp2BF));_tmp2BF[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp2C0;_tmp2C0.tag=1;_tmp2C0.f1=_tmp1C0;_tmp2C0.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp2C0;});_tmp2BF;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp2BE=_region_malloc(_tmp167->r,sizeof(*_tmp2BE));
_tmp2BE->root=root;_tmp2BE->fields=0;_tmp2BE;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2BC=_region_malloc(_tmp167->r,sizeof(*_tmp2BC));_tmp2BC[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp2BD;_tmp2BD.tag=5;_tmp2BD.f1=place;_tmp2BD;});_tmp2BC;});((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp1BF != 0){struct _RegionHandle*_tmp2AC=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp2AF;struct Cyc_List_List*_tmp2B0;struct _tuple19 _tmp2AE=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2AC,env,inflow,({struct Cyc_Absyn_Exp*_tmp2AD[2];_tmp2AD[1]=_tmp1C0;_tmp2AD[0]=(
struct Cyc_Absyn_Exp*)_tmp1BF;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(_tmp2AC,_tag_dyneither(_tmp2AD,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp2AF=_tmp2AE.f1;_tmp2B0=_tmp2AE.f2;outflow=
_tmp2AF;place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp2B0))->tl))->hd;}else{union Cyc_CfFlowInfo_FlowInfo _tmp2B2;void*
_tmp2B3;struct _tuple13 _tmp2B1=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1C0);
_tmp2B2=_tmp2B1.f1;_tmp2B3=_tmp2B1.f2;outflow=_tmp2B2;place_val=_tmp2B3;}outflow=
Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1C0->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp2B4=outflow;int _tmp2B5;struct _tuple12 _tmp2B6;struct Cyc_Dict_Dict _tmp2B7;
struct Cyc_List_List*_tmp2B8;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2B9;_LL187: if((
_tmp2B4.BottomFL).tag != 1)goto _LL189;_tmp2B5=(int)(_tmp2B4.BottomFL).val;_LL188:
return({struct _tuple13 _tmp2BA;_tmp2BA.f1=outflow;_tmp2BA.f2=rval;_tmp2BA;});
_LL189: if((_tmp2B4.ReachableFL).tag != 2)goto _LL186;_tmp2B6=(struct _tuple12)(
_tmp2B4.ReachableFL).val;_tmp2B7=_tmp2B6.f1;_tmp2B8=_tmp2B6.f2;_tmp2B9=_tmp2B6.f3;
_LL18A: return({struct _tuple13 _tmp2BB;_tmp2BB.f1=Cyc_CfFlowInfo_ReachableFL(((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp2B7,root,place_val),_tmp2B8,_tmp2B9);_tmp2BB.f2=rval;_tmp2BB;});_LL186:;}}}
_LL10E: {struct Cyc_Absyn_Address_e_struct*_tmp1C1=(struct Cyc_Absyn_Address_e_struct*)
_tmp16F;if(_tmp1C1->tag != 16)goto _LL110;else{_tmp1C2=_tmp1C1->f1;}}_LL10F: {
union Cyc_CfFlowInfo_FlowInfo _tmp2C2;struct _tuple13 _tmp2C1=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1C2);_tmp2C2=_tmp2C1.f1;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp2C4;
struct _tuple14 _tmp2C3=Cyc_CfFlowInfo_save_consume_info(env->fenv,_tmp2C2,0);
_tmp2C4=_tmp2C3.f1;{union Cyc_CfFlowInfo_FlowInfo _tmp2C6;union Cyc_CfFlowInfo_AbsLVal
_tmp2C7;struct _tuple15 _tmp2C5=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp1C2);
_tmp2C6=_tmp2C5.f1;_tmp2C7=_tmp2C5.f2;_tmp2C6=Cyc_CfFlowInfo_restore_consume_info(
_tmp2C6,_tmp2C4,0);{union Cyc_CfFlowInfo_AbsLVal _tmp2C8=_tmp2C7;int _tmp2C9;struct
Cyc_CfFlowInfo_Place*_tmp2CA;_LL18C: if((_tmp2C8.UnknownL).tag != 2)goto _LL18E;
_tmp2C9=(int)(_tmp2C8.UnknownL).val;_LL18D: return({struct _tuple13 _tmp2CB;_tmp2CB.f1=
_tmp2C6;_tmp2CB.f2=_tmp167->notzeroall;_tmp2CB;});_LL18E: if((_tmp2C8.PlaceL).tag
!= 1)goto _LL18B;_tmp2CA=(struct Cyc_CfFlowInfo_Place*)(_tmp2C8.PlaceL).val;_LL18F:
return({struct _tuple13 _tmp2CC;_tmp2CC.f1=_tmp2C6;_tmp2CC.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2CD=_region_malloc(env->r,sizeof(*_tmp2CD));_tmp2CD[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp2CE;_tmp2CE.tag=5;_tmp2CE.f1=_tmp2CA;_tmp2CE;});_tmp2CD;});_tmp2CC;});_LL18B:;}}}}
_LL110: {struct Cyc_Absyn_Deref_e_struct*_tmp1C3=(struct Cyc_Absyn_Deref_e_struct*)
_tmp16F;if(_tmp1C3->tag != 22)goto _LL112;else{_tmp1C4=_tmp1C3->f1;}}_LL111: {
union Cyc_CfFlowInfo_FlowInfo _tmp2D0;void*_tmp2D1;struct _tuple13 _tmp2CF=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1C4);_tmp2D0=_tmp2CF.f1;_tmp2D1=_tmp2CF.f2;_tmp2D0=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp2D0);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2D0,_tmp1C4,
_tmp2D1);}_LL112: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1C5=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp16F;if(_tmp1C5->tag != 23)goto _LL114;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;}}
_LL113: {union Cyc_CfFlowInfo_FlowInfo _tmp2D3;void*_tmp2D4;struct _tuple13 _tmp2D2=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1C6);_tmp2D3=_tmp2D2.f1;_tmp2D4=
_tmp2D2.f2;_tmp2D3=Cyc_CfFlowInfo_drop_unique_rvals(e->loc,_tmp2D3);_tmp2D3=Cyc_NewControlFlow_may_consume_exp(
env,_tmp2D3,e);if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C6->topt))->v))return({struct _tuple13 _tmp2D5;_tmp2D5.f1=_tmp2D3;
_tmp2D5.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp167->unknown_all);_tmp2D5;});{void*_tmp2D6=_tmp2D4;
int _tmp2D8;struct _dyneither_ptr _tmp2D9;_LL191: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2D7=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2D6;if(_tmp2D7->tag != 7)goto
_LL193;else{_tmp2D8=_tmp2D7->f1;_tmp2D9=_tmp2D7->f2;}}_LL192: {int _tmp2DA=Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C6->topt))->v,_tmp1C7);return({
struct _tuple13 _tmp2DB;_tmp2DB.f1=_tmp2D3;_tmp2DB.f2=*((void**)
_check_dyneither_subscript(_tmp2D9,sizeof(void*),_tmp2DA));_tmp2DB;});}_LL193:;
_LL194:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2DC=_cycalloc(
sizeof(*_tmp2DC));_tmp2DC[0]=({struct Cyc_Core_Impossible_struct _tmp2DD;_tmp2DD.tag=
Cyc_Core_Impossible;_tmp2DD.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2E0;_tmp2E0.tag=0;_tmp2E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp2DE[1]={& _tmp2E0};Cyc_aprintf(({const char*
_tmp2DF="anal_Rexp: AggrMember: %s";_tag_dyneither(_tmp2DF,sizeof(char),26);}),
_tag_dyneither(_tmp2DE,sizeof(void*),1));}});_tmp2DD;});_tmp2DC;}));_LL190:;}}
_LL114: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp1C8=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp16F;if(_tmp1C8->tag != 39)goto _LL116;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C8->f2;}}
_LL115: {union Cyc_CfFlowInfo_FlowInfo _tmp2E2;void*_tmp2E3;struct _tuple13 _tmp2E1=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1C9);_tmp2E2=_tmp2E1.f1;_tmp2E3=
_tmp2E1.f2;_tmp2E2=Cyc_CfFlowInfo_drop_unique_rvals(e->loc,_tmp2E2);_tmp2E2=Cyc_NewControlFlow_may_consume_exp(
env,_tmp2E2,e);return({struct _tuple13 _tmp2E4;_tmp2E4.f1=_tmp2E2;_tmp2E4.f2=
_tmp167->unknown_all;_tmp2E4;});}_LL116: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1CB=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp16F;if(_tmp1CB->tag != 24)goto
_LL118;else{_tmp1CC=_tmp1CB->f1;_tmp1CD=_tmp1CB->f2;}}_LL117: {union Cyc_CfFlowInfo_FlowInfo
_tmp2E6;void*_tmp2E7;struct _tuple13 _tmp2E5=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1CC);_tmp2E6=_tmp2E5.f1;_tmp2E7=_tmp2E5.f2;_tmp2E6=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp2E6);{union Cyc_CfFlowInfo_FlowInfo _tmp2E9;void*_tmp2EA;struct _tuple13
_tmp2E8=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2E6,_tmp1CC,_tmp2E7);
_tmp2E9=_tmp2E8.f1;_tmp2EA=_tmp2E8.f2;{void*_tmp2EB=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CC->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2ED;void*_tmp2EE;_LL196: {struct Cyc_Absyn_PointerType_struct*_tmp2EC=(struct
Cyc_Absyn_PointerType_struct*)_tmp2EB;if(_tmp2EC->tag != 5)goto _LL198;else{
_tmp2ED=_tmp2EC->f1;_tmp2EE=_tmp2ED.elt_typ;}}_LL197: if(Cyc_Absyn_is_nontagged_union_type(
_tmp2EE))return({struct _tuple13 _tmp2EF;_tmp2EF.f1=_tmp2E9;_tmp2EF.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp167,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp167->unknown_all);
_tmp2EF;});{void*_tmp2F0=_tmp2EA;int _tmp2F2;struct _dyneither_ptr _tmp2F3;_LL19B: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2F1=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2F0;if(_tmp2F1->tag != 7)goto _LL19D;else{_tmp2F2=_tmp2F1->f1;_tmp2F3=_tmp2F1->f2;}}
_LL19C: {int _tmp2F4=Cyc_CfFlowInfo_get_field_index(_tmp2EE,_tmp1CD);return({
struct _tuple13 _tmp2F5;_tmp2F5.f1=_tmp2E9;_tmp2F5.f2=*((void**)
_check_dyneither_subscript(_tmp2F3,sizeof(void*),_tmp2F4));_tmp2F5;});}_LL19D:;
_LL19E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2F6=_cycalloc(
sizeof(*_tmp2F6));_tmp2F6[0]=({struct Cyc_Core_Impossible_struct _tmp2F7;_tmp2F7.tag=
Cyc_Core_Impossible;_tmp2F7.f1=({const char*_tmp2F8="anal_Rexp: AggrArrow";
_tag_dyneither(_tmp2F8,sizeof(char),21);});_tmp2F7;});_tmp2F6;}));_LL19A:;}
_LL198:;_LL199:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2F9=
_cycalloc(sizeof(*_tmp2F9));_tmp2F9[0]=({struct Cyc_Core_Impossible_struct _tmp2FA;
_tmp2FA.tag=Cyc_Core_Impossible;_tmp2FA.f1=({const char*_tmp2FB="anal_Rexp: AggrArrow ptr";
_tag_dyneither(_tmp2FB,sizeof(char),25);});_tmp2FA;});_tmp2F9;}));_LL195:;}}}
_LL118: {struct Cyc_Absyn_Conditional_e_struct*_tmp1CE=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp16F;if(_tmp1CE->tag != 6)goto _LL11A;else{_tmp1CF=_tmp1CE->f1;_tmp1D0=_tmp1CE->f2;
_tmp1D1=_tmp1CE->f3;}}_LL119: {union Cyc_CfFlowInfo_FlowInfo _tmp2FD;union Cyc_CfFlowInfo_FlowInfo
_tmp2FE;struct _tuple16 _tmp2FC=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1CF);
_tmp2FD=_tmp2FC.f1;_tmp2FE=_tmp2FC.f2;_tmp2FD=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1CF->loc,_tmp2FD);_tmp2FE=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1CF->loc,
_tmp2FE);{struct _tuple13 _tmp2FF=Cyc_NewControlFlow_anal_Rexp(env,_tmp2FD,_tmp1D0);
struct _tuple13 _tmp300=Cyc_NewControlFlow_anal_Rexp(env,_tmp2FE,_tmp1D1);return
Cyc_CfFlowInfo_join_flow_and_rval(_tmp167,env->all_changed,_tmp2FF,_tmp300,1);}}
_LL11A: {struct Cyc_Absyn_And_e_struct*_tmp1D2=(struct Cyc_Absyn_And_e_struct*)
_tmp16F;if(_tmp1D2->tag != 7)goto _LL11C;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;}}
_LL11B: {union Cyc_CfFlowInfo_FlowInfo _tmp302;union Cyc_CfFlowInfo_FlowInfo _tmp303;
struct _tuple16 _tmp301=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1D3);_tmp302=
_tmp301.f1;_tmp303=_tmp301.f2;_tmp302=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1D3->loc,_tmp302);_tmp303=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D3->loc,
_tmp303);{union Cyc_CfFlowInfo_FlowInfo _tmp305;void*_tmp306;struct _tuple13 _tmp304=
Cyc_NewControlFlow_anal_Rexp(env,_tmp302,_tmp1D4);_tmp305=_tmp304.f1;_tmp306=
_tmp304.f2;_tmp305=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D4->loc,_tmp305);{
struct _tuple13 _tmp307=({struct _tuple13 _tmp30A;_tmp30A.f1=_tmp305;_tmp30A.f2=
_tmp306;_tmp30A;});struct _tuple13 _tmp308=({struct _tuple13 _tmp309;_tmp309.f1=
_tmp303;_tmp309.f2=_tmp167->zero;_tmp309;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp167,env->all_changed,_tmp307,_tmp308,0);}}}_LL11C: {struct Cyc_Absyn_Or_e_struct*
_tmp1D5=(struct Cyc_Absyn_Or_e_struct*)_tmp16F;if(_tmp1D5->tag != 8)goto _LL11E;
else{_tmp1D6=_tmp1D5->f1;_tmp1D7=_tmp1D5->f2;}}_LL11D: {union Cyc_CfFlowInfo_FlowInfo
_tmp30C;union Cyc_CfFlowInfo_FlowInfo _tmp30D;struct _tuple16 _tmp30B=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1D6);_tmp30C=_tmp30B.f1;_tmp30D=_tmp30B.f2;_tmp30C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1D6->loc,_tmp30C);_tmp30D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D6->loc,
_tmp30D);{union Cyc_CfFlowInfo_FlowInfo _tmp30F;void*_tmp310;struct _tuple13 _tmp30E=
Cyc_NewControlFlow_anal_Rexp(env,_tmp30D,_tmp1D7);_tmp30F=_tmp30E.f1;_tmp310=
_tmp30E.f2;_tmp30F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D7->loc,_tmp30F);{
struct _tuple13 _tmp311=({struct _tuple13 _tmp314;_tmp314.f1=_tmp30F;_tmp314.f2=
_tmp310;_tmp314;});struct _tuple13 _tmp312=({struct _tuple13 _tmp313;_tmp313.f1=
_tmp30C;_tmp313.f2=_tmp167->notzeroall;_tmp313;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp167,env->all_changed,_tmp311,_tmp312,0);}}}_LL11E: {struct Cyc_Absyn_Subscript_e_struct*
_tmp1D8=(struct Cyc_Absyn_Subscript_e_struct*)_tmp16F;if(_tmp1D8->tag != 25)goto
_LL120;else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D8->f2;}}_LL11F: {struct
_RegionHandle*_tmp315=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp318;struct Cyc_List_List*
_tmp319;struct _tuple19 _tmp317=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp315,
env,inflow,({struct Cyc_Absyn_Exp*_tmp316[2];_tmp316[1]=_tmp1DA;_tmp316[0]=
_tmp1D9;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
_tmp315,_tag_dyneither(_tmp316,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp318=
_tmp317.f1;_tmp319=_tmp317.f2;_tmp318=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1DA->loc,_tmp318);{union Cyc_CfFlowInfo_FlowInfo _tmp31A=_tmp318;{union Cyc_CfFlowInfo_FlowInfo
_tmp31B=_tmp318;struct _tuple12 _tmp31C;struct Cyc_Dict_Dict _tmp31D;struct Cyc_List_List*
_tmp31E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp31F;_LL1A0: if((_tmp31B.ReachableFL).tag
!= 2)goto _LL1A2;_tmp31C=(struct _tuple12)(_tmp31B.ReachableFL).val;_tmp31D=
_tmp31C.f1;_tmp31E=_tmp31C.f2;_tmp31F=_tmp31C.f3;_LL1A1: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp31D,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp319))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)({void*_tmp320=0;Cyc_Tcutil_terr(
_tmp1DA->loc,({const char*_tmp321="expression may not be initialized";
_tag_dyneither(_tmp321,sizeof(char),34);}),_tag_dyneither(_tmp320,sizeof(void*),
0));});{struct Cyc_List_List*_tmp322=Cyc_NewControlFlow_add_subscript_reln(
_tmp167->r,_tmp31E,_tmp1D9,_tmp1DA);if(_tmp31E != _tmp322)_tmp31A=Cyc_CfFlowInfo_ReachableFL(
_tmp31D,_tmp322,_tmp31F);goto _LL19F;}_LL1A2:;_LL1A3: goto _LL19F;_LL19F:;}{void*
_tmp323=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D9->topt))->v);
struct Cyc_List_List*_tmp325;struct Cyc_Absyn_PtrInfo _tmp327;struct Cyc_Absyn_PtrAtts
_tmp328;union Cyc_Absyn_Constraint*_tmp329;_LL1A5: {struct Cyc_Absyn_TupleType_struct*
_tmp324=(struct Cyc_Absyn_TupleType_struct*)_tmp323;if(_tmp324->tag != 11)goto
_LL1A7;else{_tmp325=_tmp324->f1;}}_LL1A6: {void*_tmp32A=(void*)((struct Cyc_List_List*)
_check_null(_tmp319))->hd;int _tmp32C;struct _dyneither_ptr _tmp32D;_LL1AC: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp32B=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp32A;if(_tmp32B->tag != 7)goto _LL1AE;else{_tmp32C=_tmp32B->f1;_tmp32D=_tmp32B->f2;}}
_LL1AD: _tmp31A=Cyc_NewControlFlow_may_consume_exp(env,_tmp31A,e);{unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp1DA)).f1;return({struct _tuple13 _tmp32E;_tmp32E.f1=
_tmp31A;_tmp32E.f2=*((void**)_check_dyneither_subscript(_tmp32D,sizeof(void*),(
int)i));_tmp32E;});}_LL1AE:;_LL1AF:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F[0]=({struct Cyc_Core_Impossible_struct
_tmp330;_tmp330.tag=Cyc_Core_Impossible;_tmp330.f1=({const char*_tmp331="anal_Rexp: Subscript";
_tag_dyneither(_tmp331,sizeof(char),21);});_tmp330;});_tmp32F;}));_LL1AB:;}
_LL1A7: {struct Cyc_Absyn_PointerType_struct*_tmp326=(struct Cyc_Absyn_PointerType_struct*)
_tmp323;if(_tmp326->tag != 5)goto _LL1A9;else{_tmp327=_tmp326->f1;_tmp328=_tmp327.ptr_atts;
_tmp329=_tmp328.bounds;}}_LL1A8: {union Cyc_CfFlowInfo_FlowInfo _tmp333;void*
_tmp334;struct _tuple13 _tmp332=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp318,
_tmp1D9,(void*)((struct Cyc_List_List*)_check_null(_tmp319))->hd);_tmp333=_tmp332.f1;
_tmp334=_tmp332.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp335=_tmp333;int _tmp336;
_LL1B1: if((_tmp335.BottomFL).tag != 1)goto _LL1B3;_tmp336=(int)(_tmp335.BottomFL).val;
_LL1B2: return({struct _tuple13 _tmp337;_tmp337.f1=_tmp333;_tmp337.f2=_tmp334;
_tmp337;});_LL1B3:;_LL1B4: return({struct _tuple13 _tmp338;_tmp338.f1=_tmp31A;
_tmp338.f2=_tmp334;_tmp338;});_LL1B0:;}}_LL1A9:;_LL1AA:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339[0]=({
struct Cyc_Core_Impossible_struct _tmp33A;_tmp33A.tag=Cyc_Core_Impossible;_tmp33A.f1=({
const char*_tmp33B="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp33B,
sizeof(char),33);});_tmp33A;});_tmp339;}));_LL1A4:;}}}_LL120: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1DB=(struct Cyc_Absyn_Datatype_e_struct*)_tmp16F;if(_tmp1DB->tag != 32)goto
_LL122;else{_tmp1DC=_tmp1DB->f1;_tmp1DD=_tmp1DB->f2;}}_LL121: _tmp1DF=_tmp1DC;
goto _LL123;_LL122: {struct Cyc_Absyn_Tuple_e_struct*_tmp1DE=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp16F;if(_tmp1DE->tag != 26)goto _LL124;else{_tmp1DF=_tmp1DE->f1;}}_LL123: {
struct _RegionHandle*_tmp33C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp33E;struct Cyc_List_List*
_tmp33F;struct _tuple19 _tmp33D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp33C,
env,inflow,_tmp1DF,0);_tmp33E=_tmp33D.f1;_tmp33F=_tmp33D.f2;_tmp33E=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp167,e->loc,_tmp33E);{struct _dyneither_ptr aggrdict=({unsigned int _tmp343=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1DF);void**
_tmp344=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp343));
struct _dyneither_ptr _tmp347=_tag_dyneither(_tmp344,sizeof(void*),_tmp343);{
unsigned int _tmp345=_tmp343;unsigned int i;for(i=0;i < _tmp345;i ++){_tmp344[i]=({
void*_tmp346=(void*)((struct Cyc_List_List*)_check_null(_tmp33F))->hd;_tmp33F=
_tmp33F->tl;_tmp346;});}}_tmp347;});return({struct _tuple13 _tmp340;_tmp340.f1=
_tmp33E;_tmp340.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp341=
_region_malloc(env->r,sizeof(*_tmp341));_tmp341[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp342;_tmp342.tag=7;_tmp342.f1=0;_tmp342.f2=aggrdict;_tmp342;});_tmp341;});
_tmp340;});}}_LL124: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp1E0=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp16F;if(_tmp1E0->tag != 31)goto _LL126;else{_tmp1E1=_tmp1E0->f2;}}_LL125: {
struct Cyc_List_List*fs;{void*_tmp348=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_List_List*_tmp34A;_LL1B6: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp349=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp348;if(_tmp349->tag != 13)goto
_LL1B8;else{_tmp34A=_tmp349->f2;}}_LL1B7: fs=_tmp34A;goto _LL1B5;_LL1B8:;_LL1B9:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp34B=_cycalloc(sizeof(*
_tmp34B));_tmp34B[0]=({struct Cyc_Core_Impossible_struct _tmp34C;_tmp34C.tag=Cyc_Core_Impossible;
_tmp34C.f1=({const char*_tmp34D="anal_Rexp:anon struct has bad type";
_tag_dyneither(_tmp34D,sizeof(char),35);});_tmp34C;});_tmp34B;}));_LL1B5:;}
_tmp1E3=_tmp1E1;_tmp1E6=Cyc_Absyn_StructA;_tmp1E9=fs;goto _LL127;}_LL126: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp1E2=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp16F;if(_tmp1E2->tag != 30)goto _LL128;else{_tmp1E3=_tmp1E2->f3;_tmp1E4=_tmp1E2->f4;
if(_tmp1E4 == 0)goto _LL128;_tmp1E5=*_tmp1E4;_tmp1E6=_tmp1E5.kind;_tmp1E7=_tmp1E5.impl;
if(_tmp1E7 == 0)goto _LL128;_tmp1E8=*_tmp1E7;_tmp1E9=_tmp1E8.fields;}}_LL127: {
struct _RegionHandle*_tmp34E=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp350;struct Cyc_List_List*
_tmp351;struct _tuple19 _tmp34F=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp34E,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple23*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp34E,(struct Cyc_Absyn_Exp*(*)(
struct _tuple23*))Cyc_Core_snd,_tmp1E3),0);_tmp350=_tmp34F.f1;_tmp351=_tmp34F.f2;
_tmp350=Cyc_CfFlowInfo_consume_unique_rvals(_tmp167,e->loc,_tmp350);{struct
_dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp167,_tmp1E9,
_tmp167->unknown_all);{int i=0;for(0;_tmp351 != 0;(((_tmp351=_tmp351->tl,_tmp1E3=
_tmp1E3->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple23*)((struct Cyc_List_List*)
_check_null(_tmp1E3))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*_tmp352=(void*)ds->hd;
struct _dyneither_ptr*_tmp355;_LL1BB: {struct Cyc_Absyn_ArrayElement_struct*
_tmp353=(struct Cyc_Absyn_ArrayElement_struct*)_tmp352;if(_tmp353->tag != 0)goto
_LL1BD;}_LL1BC:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp356=
_cycalloc(sizeof(*_tmp356));_tmp356[0]=({struct Cyc_Core_Impossible_struct _tmp357;
_tmp357.tag=Cyc_Core_Impossible;_tmp357.f1=({const char*_tmp358="anal_Rexp:Aggregate_e";
_tag_dyneither(_tmp358,sizeof(char),22);});_tmp357;});_tmp356;}));_LL1BD: {
struct Cyc_Absyn_FieldName_struct*_tmp354=(struct Cyc_Absyn_FieldName_struct*)
_tmp352;if(_tmp354->tag != 1)goto _LL1BA;else{_tmp355=_tmp354->f1;}}_LL1BE: {int
_tmp359=Cyc_CfFlowInfo_get_field_index_fs(_tmp1E9,_tmp355);*((void**)
_check_dyneither_subscript(aggrdict,sizeof(void*),_tmp359))=(void*)_tmp351->hd;
if(_tmp1E6 == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*_tmp35A=(*((struct _tuple23*)
_tmp1E3->hd)).f2;_tmp350=Cyc_NewControlFlow_use_Rval(env,_tmp35A->loc,_tmp350,(
void*)_tmp351->hd);}}_LL1BA:;}}}return({struct _tuple13 _tmp35B;_tmp35B.f1=_tmp350;
_tmp35B.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp35C=_region_malloc(
env->r,sizeof(*_tmp35C));_tmp35C[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp35D;_tmp35D.tag=7;_tmp35D.f1=_tmp1E6 == Cyc_Absyn_UnionA;_tmp35D.f2=aggrdict;
_tmp35D;});_tmp35C;});_tmp35B;});}}_LL128: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1EA=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp16F;if(_tmp1EA->tag != 30)goto
_LL12A;}_LL129:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp35E=
_cycalloc(sizeof(*_tmp35E));_tmp35E[0]=({struct Cyc_Core_Impossible_struct _tmp35F;
_tmp35F.tag=Cyc_Core_Impossible;_tmp35F.f1=({const char*_tmp360="anal_Rexp:missing aggrdeclimpl";
_tag_dyneither(_tmp360,sizeof(char),31);});_tmp35F;});_tmp35E;}));_LL12A: {
struct Cyc_Absyn_Array_e_struct*_tmp1EB=(struct Cyc_Absyn_Array_e_struct*)_tmp16F;
if(_tmp1EB->tag != 28)goto _LL12C;else{_tmp1EC=_tmp1EB->f1;}}_LL12B: {struct
_RegionHandle*_tmp361=env->r;struct Cyc_List_List*_tmp362=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple23*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp361,(struct Cyc_Absyn_Exp*(*)(struct _tuple23*))Cyc_Core_snd,
_tmp1EC);union Cyc_CfFlowInfo_FlowInfo _tmp364;struct Cyc_List_List*_tmp365;struct
_tuple19 _tmp363=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp361,env,inflow,
_tmp362,0);_tmp364=_tmp363.f1;_tmp365=_tmp363.f2;_tmp364=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp167,e->loc,_tmp364);for(0;_tmp365 != 0;(_tmp365=_tmp365->tl,_tmp362=_tmp362->tl)){
_tmp364=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp362))->hd)->loc,_tmp364,(void*)_tmp365->hd);}return({struct
_tuple13 _tmp366;_tmp366.f1=_tmp364;_tmp366.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp167,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp167->unknown_all);
_tmp366;});}_LL12C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp1ED=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp16F;if(_tmp1ED->tag != 29)goto _LL12E;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=_tmp1ED->f2;
_tmp1F0=_tmp1ED->f3;_tmp1F1=_tmp1ED->f4;}}_LL12D: {union Cyc_CfFlowInfo_FlowInfo
_tmp368;void*_tmp369;struct _tuple13 _tmp367=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1EF);_tmp368=_tmp367.f1;_tmp369=_tmp367.f2;_tmp368=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1EF->loc,_tmp368);{union Cyc_CfFlowInfo_FlowInfo _tmp36A=_tmp368;int _tmp36B;
struct _tuple12 _tmp36C;struct Cyc_Dict_Dict _tmp36D;struct Cyc_List_List*_tmp36E;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp36F;_LL1C0: if((_tmp36A.BottomFL).tag != 1)
goto _LL1C2;_tmp36B=(int)(_tmp36A.BottomFL).val;_LL1C1: return({struct _tuple13
_tmp370;_tmp370.f1=_tmp368;_tmp370.f2=_tmp167->unknown_all;_tmp370;});_LL1C2: if((
_tmp36A.ReachableFL).tag != 2)goto _LL1BF;_tmp36C=(struct _tuple12)(_tmp36A.ReachableFL).val;
_tmp36D=_tmp36C.f1;_tmp36E=_tmp36C.f2;_tmp36F=_tmp36C.f3;_LL1C3: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp36D,_tmp369)== Cyc_CfFlowInfo_NoneIL)({void*_tmp371=0;Cyc_Tcutil_terr(
_tmp1EF->loc,({const char*_tmp372="expression may not be initialized";
_tag_dyneither(_tmp372,sizeof(char),34);}),_tag_dyneither(_tmp371,sizeof(void*),
0));});{struct Cyc_List_List*new_relns=_tmp36E;comp_loop: {void*_tmp373=_tmp1EF->r;
struct Cyc_Absyn_Exp*_tmp375;void*_tmp377;struct Cyc_Absyn_Vardecl*_tmp379;void*
_tmp37B;struct Cyc_Absyn_Vardecl*_tmp37D;void*_tmp37F;struct Cyc_Absyn_Vardecl*
_tmp381;void*_tmp383;struct Cyc_Absyn_Vardecl*_tmp385;union Cyc_Absyn_Cnst _tmp387;
struct _tuple6 _tmp388;int _tmp389;enum Cyc_Absyn_Primop _tmp38B;struct Cyc_List_List*
_tmp38C;struct Cyc_List_List _tmp38D;struct Cyc_Absyn_Exp*_tmp38E;_LL1C5: {struct
Cyc_Absyn_Cast_e_struct*_tmp374=(struct Cyc_Absyn_Cast_e_struct*)_tmp373;if(
_tmp374->tag != 15)goto _LL1C7;else{_tmp375=_tmp374->f2;}}_LL1C6: _tmp1EF=_tmp375;
goto comp_loop;_LL1C7:{struct Cyc_Absyn_Var_e_struct*_tmp376=(struct Cyc_Absyn_Var_e_struct*)
_tmp373;if(_tmp376->tag != 1)goto _LL1C9;else{_tmp377=(void*)_tmp376->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp378=(struct Cyc_Absyn_Global_b_struct*)_tmp377;if(
_tmp378->tag != 1)goto _LL1C9;else{_tmp379=_tmp378->f1;}}}}if(!(!_tmp379->escapes))
goto _LL1C9;_LL1C8: _tmp37D=_tmp379;goto _LL1CA;_LL1C9:{struct Cyc_Absyn_Var_e_struct*
_tmp37A=(struct Cyc_Absyn_Var_e_struct*)_tmp373;if(_tmp37A->tag != 1)goto _LL1CB;
else{_tmp37B=(void*)_tmp37A->f2;{struct Cyc_Absyn_Local_b_struct*_tmp37C=(struct
Cyc_Absyn_Local_b_struct*)_tmp37B;if(_tmp37C->tag != 4)goto _LL1CB;else{_tmp37D=
_tmp37C->f1;}}}}if(!(!_tmp37D->escapes))goto _LL1CB;_LL1CA: _tmp381=_tmp37D;goto
_LL1CC;_LL1CB:{struct Cyc_Absyn_Var_e_struct*_tmp37E=(struct Cyc_Absyn_Var_e_struct*)
_tmp373;if(_tmp37E->tag != 1)goto _LL1CD;else{_tmp37F=(void*)_tmp37E->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp380=(struct Cyc_Absyn_Pat_b_struct*)_tmp37F;if(_tmp380->tag
!= 5)goto _LL1CD;else{_tmp381=_tmp380->f1;}}}}if(!(!_tmp381->escapes))goto _LL1CD;
_LL1CC: _tmp385=_tmp381;goto _LL1CE;_LL1CD:{struct Cyc_Absyn_Var_e_struct*_tmp382=(
struct Cyc_Absyn_Var_e_struct*)_tmp373;if(_tmp382->tag != 1)goto _LL1CF;else{
_tmp383=(void*)_tmp382->f2;{struct Cyc_Absyn_Param_b_struct*_tmp384=(struct Cyc_Absyn_Param_b_struct*)
_tmp383;if(_tmp384->tag != 3)goto _LL1CF;else{_tmp385=_tmp384->f1;}}}}if(!(!
_tmp385->escapes))goto _LL1CF;_LL1CE: new_relns=({struct Cyc_List_List*_tmp38F=
_region_malloc(env->r,sizeof(*_tmp38F));_tmp38F->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp390=_region_malloc(env->r,sizeof(*_tmp390));_tmp390->vd=_tmp1EE;_tmp390->rop=
Cyc_CfFlowInfo_LessVar(_tmp385,_tmp385->type);_tmp390;});_tmp38F->tl=_tmp36E;
_tmp38F;});goto _LL1C4;_LL1CF: {struct Cyc_Absyn_Const_e_struct*_tmp386=(struct Cyc_Absyn_Const_e_struct*)
_tmp373;if(_tmp386->tag != 0)goto _LL1D1;else{_tmp387=_tmp386->f1;if((_tmp387.Int_c).tag
!= 4)goto _LL1D1;_tmp388=(struct _tuple6)(_tmp387.Int_c).val;_tmp389=_tmp388.f2;}}
_LL1D0: new_relns=({struct Cyc_List_List*_tmp391=_region_malloc(env->r,sizeof(*
_tmp391));_tmp391->hd=({struct Cyc_CfFlowInfo_Reln*_tmp392=_region_malloc(env->r,
sizeof(*_tmp392));_tmp392->vd=_tmp1EE;_tmp392->rop=Cyc_CfFlowInfo_LessConst((
unsigned int)_tmp389);_tmp392;});_tmp391->tl=_tmp36E;_tmp391;});goto _LL1C4;
_LL1D1: {struct Cyc_Absyn_Primop_e_struct*_tmp38A=(struct Cyc_Absyn_Primop_e_struct*)
_tmp373;if(_tmp38A->tag != 3)goto _LL1D3;else{_tmp38B=_tmp38A->f1;_tmp38C=_tmp38A->f2;
if(_tmp38C == 0)goto _LL1D3;_tmp38D=*_tmp38C;_tmp38E=(struct Cyc_Absyn_Exp*)_tmp38D.hd;}}
_LL1D2:{void*_tmp393=_tmp38E->r;void*_tmp395;struct Cyc_Absyn_Vardecl*_tmp397;
void*_tmp399;struct Cyc_Absyn_Vardecl*_tmp39B;void*_tmp39D;struct Cyc_Absyn_Vardecl*
_tmp39F;void*_tmp3A1;struct Cyc_Absyn_Vardecl*_tmp3A3;_LL1D6:{struct Cyc_Absyn_Var_e_struct*
_tmp394=(struct Cyc_Absyn_Var_e_struct*)_tmp393;if(_tmp394->tag != 1)goto _LL1D8;
else{_tmp395=(void*)_tmp394->f2;{struct Cyc_Absyn_Global_b_struct*_tmp396=(struct
Cyc_Absyn_Global_b_struct*)_tmp395;if(_tmp396->tag != 1)goto _LL1D8;else{_tmp397=
_tmp396->f1;}}}}if(!(!_tmp397->escapes))goto _LL1D8;_LL1D7: _tmp39B=_tmp397;goto
_LL1D9;_LL1D8:{struct Cyc_Absyn_Var_e_struct*_tmp398=(struct Cyc_Absyn_Var_e_struct*)
_tmp393;if(_tmp398->tag != 1)goto _LL1DA;else{_tmp399=(void*)_tmp398->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp39A=(struct Cyc_Absyn_Local_b_struct*)_tmp399;if(
_tmp39A->tag != 4)goto _LL1DA;else{_tmp39B=_tmp39A->f1;}}}}if(!(!_tmp39B->escapes))
goto _LL1DA;_LL1D9: _tmp39F=_tmp39B;goto _LL1DB;_LL1DA:{struct Cyc_Absyn_Var_e_struct*
_tmp39C=(struct Cyc_Absyn_Var_e_struct*)_tmp393;if(_tmp39C->tag != 1)goto _LL1DC;
else{_tmp39D=(void*)_tmp39C->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp39E=(struct Cyc_Absyn_Pat_b_struct*)
_tmp39D;if(_tmp39E->tag != 5)goto _LL1DC;else{_tmp39F=_tmp39E->f1;}}}}if(!(!
_tmp39F->escapes))goto _LL1DC;_LL1DB: _tmp3A3=_tmp39F;goto _LL1DD;_LL1DC:{struct Cyc_Absyn_Var_e_struct*
_tmp3A0=(struct Cyc_Absyn_Var_e_struct*)_tmp393;if(_tmp3A0->tag != 1)goto _LL1DE;
else{_tmp3A1=(void*)_tmp3A0->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3A2=(struct
Cyc_Absyn_Param_b_struct*)_tmp3A1;if(_tmp3A2->tag != 3)goto _LL1DE;else{_tmp3A3=
_tmp3A2->f1;}}}}if(!(!_tmp3A3->escapes))goto _LL1DE;_LL1DD: new_relns=({struct Cyc_List_List*
_tmp3A4=_region_malloc(env->r,sizeof(*_tmp3A4));_tmp3A4->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp3A5=_region_malloc(env->r,sizeof(*_tmp3A5));_tmp3A5->vd=_tmp1EE;_tmp3A5->rop=
Cyc_CfFlowInfo_LessNumelts(_tmp3A3);_tmp3A5;});_tmp3A4->tl=_tmp36E;_tmp3A4;});
goto _LL1D5;_LL1DE:;_LL1DF: goto _LL1D5;_LL1D5:;}goto _LL1C4;_LL1D3:;_LL1D4: goto
_LL1C4;_LL1C4:;}if(_tmp36E != new_relns)_tmp368=Cyc_CfFlowInfo_ReachableFL(
_tmp36D,new_relns,_tmp36F);{void*_tmp3A6=_tmp369;_LL1E1: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3A7=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3A6;if(_tmp3A7->tag != 0)goto
_LL1E3;}_LL1E2: return({struct _tuple13 _tmp3AB;_tmp3AB.f1=_tmp368;_tmp3AB.f2=
_tmp167->unknown_all;_tmp3AB;});_LL1E3: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3A8=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3A6;if(_tmp3A8->tag != 2)
goto _LL1E5;}_LL1E4: goto _LL1E6;_LL1E5: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3A9=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3A6;if(_tmp3A9->tag != 1)
goto _LL1E7;}_LL1E6: goto _LL1E8;_LL1E7: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3AA=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3A6;if(_tmp3AA->tag != 5)goto
_LL1E9;}_LL1E8: {struct Cyc_List_List _tmp3AC=({struct Cyc_List_List _tmp3B9;_tmp3B9.hd=
_tmp1EE;_tmp3B9.tl=0;_tmp3B9;});_tmp368=Cyc_NewControlFlow_add_vars(_tmp167,
_tmp368,(struct Cyc_List_List*)& _tmp3AC,_tmp167->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3AE;void*_tmp3AF;struct _tuple13 _tmp3AD=Cyc_NewControlFlow_anal_Rexp(env,
_tmp368,_tmp1F0);_tmp3AE=_tmp3AD.f1;_tmp3AF=_tmp3AD.f2;_tmp3AE=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp167,_tmp1F0->loc,_tmp3AE);{union Cyc_CfFlowInfo_FlowInfo _tmp3B0=_tmp3AE;int
_tmp3B1;struct _tuple12 _tmp3B2;struct Cyc_Dict_Dict _tmp3B3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3B4;_LL1EC: if((_tmp3B0.BottomFL).tag != 1)goto _LL1EE;_tmp3B1=(int)(_tmp3B0.BottomFL).val;
_LL1ED: return({struct _tuple13 _tmp3B5;_tmp3B5.f1=_tmp3AE;_tmp3B5.f2=_tmp167->unknown_all;
_tmp3B5;});_LL1EE: if((_tmp3B0.ReachableFL).tag != 2)goto _LL1EB;_tmp3B2=(struct
_tuple12)(_tmp3B0.ReachableFL).val;_tmp3B3=_tmp3B2.f1;_tmp3B4=_tmp3B2.f3;_LL1EF:
if(Cyc_CfFlowInfo_initlevel(_tmp167,_tmp3B3,_tmp3AF)!= Cyc_CfFlowInfo_AllIL){({
void*_tmp3B6=0;Cyc_Tcutil_terr(_tmp1EF->loc,({const char*_tmp3B7="expression may not be initialized";
_tag_dyneither(_tmp3B7,sizeof(char),34);}),_tag_dyneither(_tmp3B6,sizeof(void*),
0));});return({struct _tuple13 _tmp3B8;_tmp3B8.f1=Cyc_CfFlowInfo_BottomFL();
_tmp3B8.f2=_tmp167->unknown_all;_tmp3B8;});}_LL1EB:;}_tmp368=_tmp3AE;goto _LL1EA;}}
_LL1E9:;_LL1EA: while(1){struct Cyc_List_List _tmp3BA=({struct Cyc_List_List _tmp3C7;
_tmp3C7.hd=_tmp1EE;_tmp3C7.tl=0;_tmp3C7;});_tmp368=Cyc_NewControlFlow_add_vars(
_tmp167,_tmp368,(struct Cyc_List_List*)& _tmp3BA,_tmp167->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo _tmp3BC;void*_tmp3BD;struct _tuple13 _tmp3BB=Cyc_NewControlFlow_anal_Rexp(
env,_tmp368,_tmp1F0);_tmp3BC=_tmp3BB.f1;_tmp3BD=_tmp3BB.f2;_tmp3BC=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp167,_tmp1F0->loc,_tmp3BC);{union Cyc_CfFlowInfo_FlowInfo _tmp3BE=_tmp3BC;int
_tmp3BF;struct _tuple12 _tmp3C0;struct Cyc_Dict_Dict _tmp3C1;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3C2;_LL1F1: if((_tmp3BE.BottomFL).tag != 1)goto _LL1F3;_tmp3BF=(int)(_tmp3BE.BottomFL).val;
_LL1F2: goto _LL1F0;_LL1F3: if((_tmp3BE.ReachableFL).tag != 2)goto _LL1F0;_tmp3C0=(
struct _tuple12)(_tmp3BE.ReachableFL).val;_tmp3C1=_tmp3C0.f1;_tmp3C2=_tmp3C0.f3;
_LL1F4: if(Cyc_CfFlowInfo_initlevel(_tmp167,_tmp3C1,_tmp3BD)!= Cyc_CfFlowInfo_AllIL){({
void*_tmp3C3=0;Cyc_Tcutil_terr(_tmp1EF->loc,({const char*_tmp3C4="expression may not be initialized";
_tag_dyneither(_tmp3C4,sizeof(char),34);}),_tag_dyneither(_tmp3C3,sizeof(void*),
0));});return({struct _tuple13 _tmp3C5;_tmp3C5.f1=Cyc_CfFlowInfo_BottomFL();
_tmp3C5.f2=_tmp167->unknown_all;_tmp3C5;});}_LL1F0:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp3C6=Cyc_CfFlowInfo_join_flow(_tmp167,env->all_changed,_tmp368,_tmp3BC,1);if(
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp3C6,_tmp368))break;_tmp368=_tmp3C6;}}}
return({struct _tuple13 _tmp3C8;_tmp3C8.f1=_tmp368;_tmp3C8.f2=_tmp167->unknown_all;
_tmp3C8;});_LL1E0:;}}_LL1BF:;}}_LL12E: {struct Cyc_Absyn_StmtExp_e_struct*_tmp1F2=(
struct Cyc_Absyn_StmtExp_e_struct*)_tmp16F;if(_tmp1F2->tag != 38)goto _LL130;else{
_tmp1F3=_tmp1F2->f1;}}_LL12F: while(1){union Cyc_CfFlowInfo_FlowInfo*_tmp3CA;
struct _tuple17 _tmp3C9=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp1F3);
_tmp3CA=_tmp3C9.f2;inflow=*_tmp3CA;{void*_tmp3CB=_tmp1F3->r;struct Cyc_Absyn_Stmt*
_tmp3CD;struct Cyc_Absyn_Stmt*_tmp3CE;struct Cyc_Absyn_Decl*_tmp3D0;struct Cyc_Absyn_Stmt*
_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D3;_LL1F6: {struct Cyc_Absyn_Seq_s_struct*
_tmp3CC=(struct Cyc_Absyn_Seq_s_struct*)_tmp3CB;if(_tmp3CC->tag != 2)goto _LL1F8;
else{_tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;}}_LL1F7: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp3CD);_tmp1F3=_tmp3CE;goto _LL1F5;_LL1F8: {struct Cyc_Absyn_Decl_s_struct*
_tmp3CF=(struct Cyc_Absyn_Decl_s_struct*)_tmp3CB;if(_tmp3CF->tag != 12)goto _LL1FA;
else{_tmp3D0=_tmp3CF->f1;_tmp3D1=_tmp3CF->f2;}}_LL1F9: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp3D0);_tmp1F3=_tmp3D1;goto _LL1F5;_LL1FA: {struct Cyc_Absyn_Exp_s_struct*
_tmp3D2=(struct Cyc_Absyn_Exp_s_struct*)_tmp3CB;if(_tmp3D2->tag != 1)goto _LL1FC;
else{_tmp3D3=_tmp3D2->f1;}}_LL1FB: return Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp3D3);_LL1FC:;_LL1FD:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4[0]=({struct Cyc_Core_Impossible_struct
_tmp3D5;_tmp3D5.tag=Cyc_Core_Impossible;_tmp3D5.f1=({const char*_tmp3D6="analyze_Rexp: ill-formed StmtExp";
_tag_dyneither(_tmp3D6,sizeof(char),33);});_tmp3D5;});_tmp3D4;}));_LL1F5:;}}
_LL130: {struct Cyc_Absyn_Var_e_struct*_tmp1F4=(struct Cyc_Absyn_Var_e_struct*)
_tmp16F;if(_tmp1F4->tag != 1)goto _LL132;else{_tmp1F5=(void*)_tmp1F4->f2;{struct
Cyc_Absyn_Unresolved_b_struct*_tmp1F6=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmp1F5;if(_tmp1F6->tag != 0)goto _LL132;}}}_LL131: goto _LL133;_LL132: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp1F7=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp16F;if(_tmp1F7->tag != 2)goto
_LL134;}_LL133: goto _LL135;_LL134: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp1F8=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp16F;if(_tmp1F8->tag != 10)goto _LL136;}
_LL135: goto _LL137;_LL136: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1F9=(
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp16F;if(_tmp1F9->tag != 37)goto _LL138;}
_LL137: goto _LL139;_LL138: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp1FA=(struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp16F;if(_tmp1FA->tag != 27)goto _LL13A;}_LL139:
goto _LL13B;_LL13A: {struct Cyc_Absyn_Valueof_e_struct*_tmp1FB=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp16F;if(_tmp1FB->tag != 40)goto _LLD1;}_LL13B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7[0]=({struct Cyc_Core_Impossible_struct
_tmp3D8;_tmp3D8.tag=Cyc_Core_Impossible;_tmp3D8.f1=({const char*_tmp3D9="anal_Rexp, unexpected exp form";
_tag_dyneither(_tmp3D9,sizeof(char),31);});_tmp3D8;});_tmp3D7;}));_LLD1:;}}
static struct _tuple15 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo
f,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp3DA=env->fenv;
void*_tmp3DB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp3DD;void*_tmp3DE;struct Cyc_Absyn_PtrAtts _tmp3DF;union
Cyc_Absyn_Constraint*_tmp3E0;union Cyc_Absyn_Constraint*_tmp3E1;_LL1FF: {struct
Cyc_Absyn_PointerType_struct*_tmp3DC=(struct Cyc_Absyn_PointerType_struct*)
_tmp3DB;if(_tmp3DC->tag != 5)goto _LL201;else{_tmp3DD=_tmp3DC->f1;_tmp3DE=_tmp3DD.elt_typ;
_tmp3DF=_tmp3DD.ptr_atts;_tmp3E0=_tmp3DF.bounds;_tmp3E1=_tmp3DF.zero_term;}}
_LL200: {union Cyc_CfFlowInfo_FlowInfo _tmp3E2=f;int _tmp3E3;struct _tuple12 _tmp3E4;
struct Cyc_Dict_Dict _tmp3E5;struct Cyc_List_List*_tmp3E6;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3E7;_LL204: if((_tmp3E2.BottomFL).tag != 1)goto _LL206;_tmp3E3=(int)(_tmp3E2.BottomFL).val;
_LL205: return({struct _tuple15 _tmp3E8;_tmp3E8.f1=f;_tmp3E8.f2=Cyc_CfFlowInfo_UnknownL();
_tmp3E8;});_LL206: if((_tmp3E2.ReachableFL).tag != 2)goto _LL203;_tmp3E4=(struct
_tuple12)(_tmp3E2.ReachableFL).val;_tmp3E5=_tmp3E4.f1;_tmp3E6=_tmp3E4.f2;_tmp3E7=
_tmp3E4.f3;_LL207: if(Cyc_Tcutil_is_bound_one(_tmp3E0)){void*_tmp3E9=r;struct Cyc_CfFlowInfo_Place*
_tmp3ED;struct Cyc_CfFlowInfo_Place _tmp3EE;void*_tmp3EF;struct Cyc_List_List*
_tmp3F0;enum Cyc_CfFlowInfo_InitLevel _tmp3F3;_LL209: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3EA=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3E9;if(_tmp3EA->tag != 1)
goto _LL20B;}_LL20A: goto _LL20C;_LL20B: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3EB=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3E9;if(_tmp3EB->tag != 2)
goto _LL20D;}_LL20C: e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp3F4=
_cycalloc(sizeof(*_tmp3F4));_tmp3F4[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp3F5;_tmp3F5.tag=Cyc_CfFlowInfo_NotZero;_tmp3F5.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp3E6);_tmp3F5;});_tmp3F4;});goto _LL208;_LL20D: {struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp3EC=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp3E9;if(_tmp3EC->tag != 5)goto _LL20F;else{_tmp3ED=_tmp3EC->f1;_tmp3EE=*_tmp3ED;
_tmp3EF=_tmp3EE.root;_tmp3F0=_tmp3EE.fields;}}_LL20E: e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp3F6=_cycalloc(sizeof(*_tmp3F6));_tmp3F6[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp3F7;_tmp3F7.tag=Cyc_CfFlowInfo_NotZero;_tmp3F7.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp3E6);_tmp3F7;});_tmp3F6;});return({struct _tuple15
_tmp3F8;_tmp3F8.f1=f;_tmp3F8.f2=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*
_tmp3F9=_region_malloc(_tmp3DA->r,sizeof(*_tmp3F9));_tmp3F9->root=_tmp3EF;
_tmp3F9->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp3DA->r,_tmp3F0,flds);_tmp3F9;}));
_tmp3F8;});_LL20F: {struct Cyc_CfFlowInfo_Zero_struct*_tmp3F1=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3E9;if(_tmp3F1->tag != 0)goto _LL211;}_LL210: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple15 _tmp3FA;_tmp3FA.f1=Cyc_CfFlowInfo_BottomFL();_tmp3FA.f2=
Cyc_CfFlowInfo_UnknownL();_tmp3FA;});_LL211: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3F2=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3E9;if(_tmp3F2->tag != 3)goto
_LL213;else{_tmp3F3=_tmp3F2->f1;}}_LL212: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp3F3);goto _LL214;_LL213:;_LL214: e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp3FC;_tmp3FC.tag=Cyc_CfFlowInfo_UnknownZ;_tmp3FC.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp3E6);_tmp3FC;});_tmp3FB;});_LL208:;}else{e->annot=(void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp3FE;_tmp3FE.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp3FE.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3E6);_tmp3FE;});
_tmp3FD;});}if(Cyc_CfFlowInfo_initlevel(_tmp3DA,_tmp3E5,r)== Cyc_CfFlowInfo_NoneIL)({
void*_tmp3FF=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp400="dereference of possibly uninitialized pointer";
_tag_dyneither(_tmp400,sizeof(char),46);}),_tag_dyneither(_tmp3FF,sizeof(void*),
0));});return({struct _tuple15 _tmp401;_tmp401.f1=f;_tmp401.f2=Cyc_CfFlowInfo_UnknownL();
_tmp401;});_LL203:;}_LL201:;_LL202:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402[0]=({struct Cyc_Core_Impossible_struct
_tmp403;_tmp403.tag=Cyc_Core_Impossible;_tmp403.f1=({const char*_tmp404="left deref of non-pointer-type";
_tag_dyneither(_tmp404,sizeof(char),31);});_tmp403;});_tmp402;}));_LL1FE:;}
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp405=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo _tmp406=inflow;int _tmp407;struct _tuple12 _tmp408;struct Cyc_Dict_Dict
_tmp409;struct Cyc_List_List*_tmp40A;_LL216: if((_tmp406.BottomFL).tag != 1)goto
_LL218;_tmp407=(int)(_tmp406.BottomFL).val;_LL217: return({struct _tuple15 _tmp40B;
_tmp40B.f1=Cyc_CfFlowInfo_BottomFL();_tmp40B.f2=Cyc_CfFlowInfo_UnknownL();
_tmp40B;});_LL218: if((_tmp406.ReachableFL).tag != 2)goto _LL215;_tmp408=(struct
_tuple12)(_tmp406.ReachableFL).val;_tmp409=_tmp408.f1;_tmp40A=_tmp408.f2;_LL219:
d=_tmp409;_LL215:;}{void*_tmp40C=e->r;struct Cyc_Absyn_Exp*_tmp40E;struct Cyc_Absyn_Exp*
_tmp410;struct Cyc_Absyn_Exp*_tmp412;void*_tmp414;struct Cyc_Absyn_Vardecl*_tmp416;
void*_tmp418;struct Cyc_Absyn_Vardecl*_tmp41A;void*_tmp41C;struct Cyc_Absyn_Vardecl*
_tmp41E;void*_tmp420;struct Cyc_Absyn_Vardecl*_tmp422;struct Cyc_Absyn_Exp*_tmp424;
struct _dyneither_ptr*_tmp425;struct Cyc_Absyn_Exp*_tmp427;struct Cyc_Absyn_Exp*
_tmp429;struct Cyc_Absyn_Exp*_tmp42A;struct Cyc_Absyn_Exp*_tmp42C;struct
_dyneither_ptr*_tmp42D;_LL21B: {struct Cyc_Absyn_Cast_e_struct*_tmp40D=(struct Cyc_Absyn_Cast_e_struct*)
_tmp40C;if(_tmp40D->tag != 15)goto _LL21D;else{_tmp40E=_tmp40D->f2;}}_LL21C:
_tmp410=_tmp40E;goto _LL21E;_LL21D: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp40F=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp40C;if(_tmp40F->tag != 13)
goto _LL21F;else{_tmp410=_tmp40F->f1;}}_LL21E: _tmp412=_tmp410;goto _LL220;_LL21F: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp411=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp40C;if(_tmp411->tag != 14)goto _LL221;else{_tmp412=_tmp411->f1;}}_LL220: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp412,flds);_LL221: {struct Cyc_Absyn_Var_e_struct*
_tmp413=(struct Cyc_Absyn_Var_e_struct*)_tmp40C;if(_tmp413->tag != 1)goto _LL223;
else{_tmp414=(void*)_tmp413->f2;{struct Cyc_Absyn_Param_b_struct*_tmp415=(struct
Cyc_Absyn_Param_b_struct*)_tmp414;if(_tmp415->tag != 3)goto _LL223;else{_tmp416=
_tmp415->f1;}}}}_LL222: _tmp41A=_tmp416;goto _LL224;_LL223: {struct Cyc_Absyn_Var_e_struct*
_tmp417=(struct Cyc_Absyn_Var_e_struct*)_tmp40C;if(_tmp417->tag != 1)goto _LL225;
else{_tmp418=(void*)_tmp417->f2;{struct Cyc_Absyn_Local_b_struct*_tmp419=(struct
Cyc_Absyn_Local_b_struct*)_tmp418;if(_tmp419->tag != 4)goto _LL225;else{_tmp41A=
_tmp419->f1;}}}}_LL224: _tmp41E=_tmp41A;goto _LL226;_LL225: {struct Cyc_Absyn_Var_e_struct*
_tmp41B=(struct Cyc_Absyn_Var_e_struct*)_tmp40C;if(_tmp41B->tag != 1)goto _LL227;
else{_tmp41C=(void*)_tmp41B->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp41D=(struct Cyc_Absyn_Pat_b_struct*)
_tmp41C;if(_tmp41D->tag != 5)goto _LL227;else{_tmp41E=_tmp41D->f1;}}}}_LL226:
return({struct _tuple15 _tmp42E;_tmp42E.f1=inflow;_tmp42E.f2=Cyc_CfFlowInfo_PlaceL(({
struct Cyc_CfFlowInfo_Place*_tmp42F=_region_malloc(env->r,sizeof(*_tmp42F));
_tmp42F->root=(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp430=
_region_malloc(env->r,sizeof(*_tmp430));_tmp430[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp431;_tmp431.tag=0;_tmp431.f1=_tmp41E;_tmp431;});_tmp430;});_tmp42F->fields=
flds;_tmp42F;}));_tmp42E;});_LL227: {struct Cyc_Absyn_Var_e_struct*_tmp41F=(
struct Cyc_Absyn_Var_e_struct*)_tmp40C;if(_tmp41F->tag != 1)goto _LL229;else{
_tmp420=(void*)_tmp41F->f2;{struct Cyc_Absyn_Global_b_struct*_tmp421=(struct Cyc_Absyn_Global_b_struct*)
_tmp420;if(_tmp421->tag != 1)goto _LL229;else{_tmp422=_tmp421->f1;}}}}_LL228:
return({struct _tuple15 _tmp432;_tmp432.f1=inflow;_tmp432.f2=Cyc_CfFlowInfo_UnknownL();
_tmp432;});_LL229: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp423=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp40C;if(_tmp423->tag != 24)goto _LL22B;else{_tmp424=_tmp423->f1;_tmp425=_tmp423->f2;}}
_LL22A:{void*_tmp433=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp424->topt))->v);struct Cyc_Absyn_PtrInfo _tmp435;void*_tmp436;
_LL234: {struct Cyc_Absyn_PointerType_struct*_tmp434=(struct Cyc_Absyn_PointerType_struct*)
_tmp433;if(_tmp434->tag != 5)goto _LL236;else{_tmp435=_tmp434->f1;_tmp436=_tmp435.elt_typ;}}
_LL235: if(!Cyc_Absyn_is_nontagged_union_type(_tmp436))flds=({struct Cyc_List_List*
_tmp437=_region_malloc(env->r,sizeof(*_tmp437));_tmp437->hd=(void*)Cyc_CfFlowInfo_get_field_index(
_tmp436,_tmp425);_tmp437->tl=flds;_tmp437;});goto _LL233;_LL236:;_LL237:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp438=_cycalloc(sizeof(*
_tmp438));_tmp438[0]=({struct Cyc_Core_Impossible_struct _tmp439;_tmp439.tag=Cyc_Core_Impossible;
_tmp439.f1=({const char*_tmp43A="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp43A,
sizeof(char),25);});_tmp439;});_tmp438;}));_LL233:;}_tmp427=_tmp424;goto _LL22C;
_LL22B: {struct Cyc_Absyn_Deref_e_struct*_tmp426=(struct Cyc_Absyn_Deref_e_struct*)
_tmp40C;if(_tmp426->tag != 22)goto _LL22D;else{_tmp427=_tmp426->f1;}}_LL22C: {
union Cyc_CfFlowInfo_FlowInfo _tmp43C;void*_tmp43D;struct _tuple13 _tmp43B=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp427);_tmp43C=_tmp43B.f1;_tmp43D=_tmp43B.f2;_tmp43C=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp43C);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp427,_tmp43C,
_tmp43D,flds);}_LL22D: {struct Cyc_Absyn_Subscript_e_struct*_tmp428=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp40C;if(_tmp428->tag != 25)goto _LL22F;else{_tmp429=_tmp428->f1;_tmp42A=_tmp428->f2;}}
_LL22E: {void*_tmp43E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp429->topt))->v);struct Cyc_Absyn_PtrInfo _tmp441;struct Cyc_Absyn_PtrAtts
_tmp442;union Cyc_Absyn_Constraint*_tmp443;_LL239: {struct Cyc_Absyn_TupleType_struct*
_tmp43F=(struct Cyc_Absyn_TupleType_struct*)_tmp43E;if(_tmp43F->tag != 11)goto
_LL23B;}_LL23A: {unsigned int _tmp444=(Cyc_Evexp_eval_const_uint_exp(_tmp42A)).f1;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp429,({struct Cyc_List_List*
_tmp445=_region_malloc(env->r,sizeof(*_tmp445));_tmp445->hd=(void*)((int)_tmp444);
_tmp445->tl=flds;_tmp445;}));}_LL23B: {struct Cyc_Absyn_PointerType_struct*
_tmp440=(struct Cyc_Absyn_PointerType_struct*)_tmp43E;if(_tmp440->tag != 5)goto
_LL23D;else{_tmp441=_tmp440->f1;_tmp442=_tmp441.ptr_atts;_tmp443=_tmp442.bounds;}}
_LL23C: {struct _RegionHandle*_tmp446=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp449;
struct Cyc_List_List*_tmp44A;struct _tuple19 _tmp448=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp446,env,inflow,({struct Cyc_Absyn_Exp*_tmp447[2];_tmp447[1]=_tmp42A;_tmp447[0]=
_tmp429;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
_tmp446,_tag_dyneither(_tmp447,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp449=
_tmp448.f1;_tmp44A=_tmp448.f2;_tmp449=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp42A->loc,_tmp449);{union Cyc_CfFlowInfo_FlowInfo _tmp44B=_tmp449;{union Cyc_CfFlowInfo_FlowInfo
_tmp44C=_tmp449;struct _tuple12 _tmp44D;struct Cyc_Dict_Dict _tmp44E;struct Cyc_List_List*
_tmp44F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp450;_LL240: if((_tmp44C.ReachableFL).tag
!= 2)goto _LL242;_tmp44D=(struct _tuple12)(_tmp44C.ReachableFL).val;_tmp44E=
_tmp44D.f1;_tmp44F=_tmp44D.f2;_tmp450=_tmp44D.f3;_LL241: if(Cyc_CfFlowInfo_initlevel(
_tmp405,_tmp44E,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp44A))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)({void*_tmp451=0;Cyc_Tcutil_terr(
_tmp42A->loc,({const char*_tmp452="expression may not be initialized";
_tag_dyneither(_tmp452,sizeof(char),34);}),_tag_dyneither(_tmp451,sizeof(void*),
0));});{struct Cyc_List_List*_tmp453=Cyc_NewControlFlow_add_subscript_reln(
_tmp405->r,_tmp44F,_tmp429,_tmp42A);if(_tmp44F != _tmp453)_tmp44B=Cyc_CfFlowInfo_ReachableFL(
_tmp44E,_tmp453,_tmp450);goto _LL23F;}_LL242:;_LL243: goto _LL23F;_LL23F:;}{union
Cyc_CfFlowInfo_FlowInfo _tmp455;union Cyc_CfFlowInfo_AbsLVal _tmp456;struct _tuple15
_tmp454=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp429,_tmp449,(void*)((
struct Cyc_List_List*)_check_null(_tmp44A))->hd,flds);_tmp455=_tmp454.f1;_tmp456=
_tmp454.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp457=_tmp455;int _tmp458;_LL245: if((
_tmp457.BottomFL).tag != 1)goto _LL247;_tmp458=(int)(_tmp457.BottomFL).val;_LL246:
return({struct _tuple15 _tmp459;_tmp459.f1=_tmp455;_tmp459.f2=_tmp456;_tmp459;});
_LL247:;_LL248: return({struct _tuple15 _tmp45A;_tmp45A.f1=_tmp44B;_tmp45A.f2=
_tmp456;_tmp45A;});_LL244:;}}}}_LL23D:;_LL23E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B[0]=({struct Cyc_Core_Impossible_struct
_tmp45C;_tmp45C.tag=Cyc_Core_Impossible;_tmp45C.f1=({const char*_tmp45D="anal_Lexp: Subscript -- bad type";
_tag_dyneither(_tmp45D,sizeof(char),33);});_tmp45C;});_tmp45B;}));_LL238:;}
_LL22F: {struct Cyc_Absyn_AggrMember_e_struct*_tmp42B=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp40C;if(_tmp42B->tag != 23)goto _LL231;else{_tmp42C=_tmp42B->f1;_tmp42D=_tmp42B->f2;}}
_LL230: if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp42C->topt))->v))return({struct _tuple15 _tmp45E;_tmp45E.f1=inflow;
_tmp45E.f2=Cyc_CfFlowInfo_UnknownL();_tmp45E;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp42C,({struct Cyc_List_List*_tmp45F=_region_malloc(env->r,sizeof(*
_tmp45F));_tmp45F->hd=(void*)Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp42C->topt))->v,_tmp42D);_tmp45F->tl=flds;_tmp45F;}));_LL231:;
_LL232: return({struct _tuple15 _tmp460;_tmp460.f1=Cyc_CfFlowInfo_BottomFL();
_tmp460.f2=Cyc_CfFlowInfo_UnknownL();_tmp460;});_LL21A:;}}static struct _tuple15
Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_ConsumeInfo _tmp462;struct
_tuple14 _tmp461=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);_tmp462=
_tmp461.f1;{union Cyc_CfFlowInfo_FlowInfo _tmp464;union Cyc_CfFlowInfo_AbsLVal
_tmp465;struct _tuple15 _tmp463=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,e,0);
_tmp464=_tmp463.f1;_tmp465=_tmp463.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp467;
struct _tuple14 _tmp466=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);
_tmp467=_tmp466.f1;if(_tmp462.may_consume != _tmp467.may_consume  || (_tmp462.consumed).t
!= (_tmp467.consumed).t)({void*_tmp468=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp469="oops: anal_Lexp modified consume lists";
_tag_dyneither(_tmp469,sizeof(char),39);}),_tag_dyneither(_tmp468,sizeof(void*),
0));});return({struct _tuple15 _tmp46A;_tmp46A.f1=_tmp464;_tmp46A.f2=_tmp465;
_tmp46A;});}}}static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp46B=env->fenv;{void*_tmp46C=e->r;struct Cyc_Absyn_Exp*_tmp46E;struct Cyc_Absyn_Exp*
_tmp46F;struct Cyc_Absyn_Exp*_tmp470;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*
_tmp473;struct Cyc_Absyn_Exp*_tmp475;struct Cyc_Absyn_Exp*_tmp476;struct Cyc_Absyn_Exp*
_tmp478;struct Cyc_Absyn_Exp*_tmp479;enum Cyc_Absyn_Primop _tmp47B;struct Cyc_List_List*
_tmp47C;struct Cyc_List_List _tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_List_List*
_tmp47F;enum Cyc_Absyn_Primop _tmp481;struct Cyc_List_List*_tmp482;_LL24A: {struct
Cyc_Absyn_Conditional_e_struct*_tmp46D=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp46C;if(_tmp46D->tag != 6)goto _LL24C;else{_tmp46E=_tmp46D->f1;_tmp46F=_tmp46D->f2;
_tmp470=_tmp46D->f3;}}_LL24B: {union Cyc_CfFlowInfo_FlowInfo _tmp484;union Cyc_CfFlowInfo_FlowInfo
_tmp485;struct _tuple16 _tmp483=Cyc_NewControlFlow_anal_test(env,inflow,_tmp46E);
_tmp484=_tmp483.f1;_tmp485=_tmp483.f2;_tmp484=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp46E->loc,_tmp484);_tmp485=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp46E->loc,
_tmp485);{union Cyc_CfFlowInfo_FlowInfo _tmp487;union Cyc_CfFlowInfo_FlowInfo
_tmp488;struct _tuple16 _tmp486=Cyc_NewControlFlow_anal_test(env,_tmp484,_tmp46F);
_tmp487=_tmp486.f1;_tmp488=_tmp486.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp48A;union
Cyc_CfFlowInfo_FlowInfo _tmp48B;struct _tuple16 _tmp489=Cyc_NewControlFlow_anal_test(
env,_tmp485,_tmp470);_tmp48A=_tmp489.f1;_tmp48B=_tmp489.f2;return({struct
_tuple16 _tmp48C;_tmp48C.f1=Cyc_CfFlowInfo_join_flow(_tmp46B,env->all_changed,
_tmp487,_tmp48A,1);_tmp48C.f2=Cyc_CfFlowInfo_join_flow(_tmp46B,env->all_changed,
_tmp488,_tmp48B,1);_tmp48C;});}}}_LL24C: {struct Cyc_Absyn_And_e_struct*_tmp471=(
struct Cyc_Absyn_And_e_struct*)_tmp46C;if(_tmp471->tag != 7)goto _LL24E;else{
_tmp472=_tmp471->f1;_tmp473=_tmp471->f2;}}_LL24D: {union Cyc_CfFlowInfo_FlowInfo
_tmp48E;union Cyc_CfFlowInfo_FlowInfo _tmp48F;struct _tuple16 _tmp48D=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp472);_tmp48E=_tmp48D.f1;_tmp48F=_tmp48D.f2;_tmp48E=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp472->loc,_tmp48E);_tmp48F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp472->loc,
_tmp48F);{union Cyc_CfFlowInfo_FlowInfo _tmp491;union Cyc_CfFlowInfo_FlowInfo
_tmp492;struct _tuple16 _tmp490=Cyc_NewControlFlow_anal_test(env,_tmp48E,_tmp473);
_tmp491=_tmp490.f1;_tmp492=_tmp490.f2;_tmp491=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp473->loc,_tmp491);_tmp492=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp473->loc,
_tmp492);return({struct _tuple16 _tmp493;_tmp493.f1=_tmp491;_tmp493.f2=Cyc_CfFlowInfo_join_flow(
_tmp46B,env->all_changed,_tmp48F,_tmp492,0);_tmp493;});}}_LL24E: {struct Cyc_Absyn_Or_e_struct*
_tmp474=(struct Cyc_Absyn_Or_e_struct*)_tmp46C;if(_tmp474->tag != 8)goto _LL250;
else{_tmp475=_tmp474->f1;_tmp476=_tmp474->f2;}}_LL24F: {union Cyc_CfFlowInfo_FlowInfo
_tmp495;union Cyc_CfFlowInfo_FlowInfo _tmp496;struct _tuple16 _tmp494=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp475);_tmp495=_tmp494.f1;_tmp496=_tmp494.f2;_tmp495=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp475->loc,_tmp495);_tmp496=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp475->loc,
_tmp496);{union Cyc_CfFlowInfo_FlowInfo _tmp498;union Cyc_CfFlowInfo_FlowInfo
_tmp499;struct _tuple16 _tmp497=Cyc_NewControlFlow_anal_test(env,_tmp496,_tmp476);
_tmp498=_tmp497.f1;_tmp499=_tmp497.f2;_tmp498=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp476->loc,_tmp498);_tmp499=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp476->loc,
_tmp499);return({struct _tuple16 _tmp49A;_tmp49A.f1=Cyc_CfFlowInfo_join_flow(
_tmp46B,env->all_changed,_tmp495,_tmp498,0);_tmp49A.f2=_tmp499;_tmp49A;});}}
_LL250: {struct Cyc_Absyn_SeqExp_e_struct*_tmp477=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp46C;if(_tmp477->tag != 9)goto _LL252;else{_tmp478=_tmp477->f1;_tmp479=_tmp477->f2;}}
_LL251: {union Cyc_CfFlowInfo_FlowInfo _tmp49C;void*_tmp49D;struct _tuple13 _tmp49B=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp478);_tmp49C=_tmp49B.f1;_tmp49D=
_tmp49B.f2;_tmp49C=Cyc_CfFlowInfo_drop_unique_rvals(_tmp478->loc,_tmp49C);return
Cyc_NewControlFlow_anal_test(env,_tmp49C,_tmp479);}_LL252: {struct Cyc_Absyn_Primop_e_struct*
_tmp47A=(struct Cyc_Absyn_Primop_e_struct*)_tmp46C;if(_tmp47A->tag != 3)goto _LL254;
else{_tmp47B=_tmp47A->f1;if(_tmp47B != Cyc_Absyn_Not)goto _LL254;_tmp47C=_tmp47A->f2;
if(_tmp47C == 0)goto _LL254;_tmp47D=*_tmp47C;_tmp47E=(struct Cyc_Absyn_Exp*)_tmp47D.hd;
_tmp47F=_tmp47D.tl;if(_tmp47F != 0)goto _LL254;}}_LL253: {union Cyc_CfFlowInfo_FlowInfo
_tmp49F;union Cyc_CfFlowInfo_FlowInfo _tmp4A0;struct _tuple16 _tmp49E=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp47E);_tmp49F=_tmp49E.f1;_tmp4A0=_tmp49E.f2;return({struct _tuple16
_tmp4A1;_tmp4A1.f1=_tmp4A0;_tmp4A1.f2=_tmp49F;_tmp4A1;});}_LL254: {struct Cyc_Absyn_Primop_e_struct*
_tmp480=(struct Cyc_Absyn_Primop_e_struct*)_tmp46C;if(_tmp480->tag != 3)goto _LL256;
else{_tmp481=_tmp480->f1;_tmp482=_tmp480->f2;}}_LL255: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp4A2=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp4A4;struct
Cyc_List_List*_tmp4A5;struct _tuple19 _tmp4A3=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp4A2,env,inflow,_tmp482,0);_tmp4A4=_tmp4A3.f1;_tmp4A5=_tmp4A3.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp4A5))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp4A5->tl))->hd;f=_tmp4A4;}{union Cyc_CfFlowInfo_FlowInfo _tmp4A6=f;
int _tmp4A7;struct _tuple12 _tmp4A8;struct Cyc_Dict_Dict _tmp4A9;struct Cyc_List_List*
_tmp4AA;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4AB;_LL259: if((_tmp4A6.BottomFL).tag
!= 1)goto _LL25B;_tmp4A7=(int)(_tmp4A6.BottomFL).val;_LL25A: return({struct
_tuple16 _tmp4AC;_tmp4AC.f1=f;_tmp4AC.f2=f;_tmp4AC;});_LL25B: if((_tmp4A6.ReachableFL).tag
!= 2)goto _LL258;_tmp4A8=(struct _tuple12)(_tmp4A6.ReachableFL).val;_tmp4A9=
_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;_tmp4AB=_tmp4A8.f3;_LL25C: {struct Cyc_Absyn_Exp*
_tmp4AD=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp482))->hd;
struct Cyc_Absyn_Exp*_tmp4AE=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp482->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4A9,r1)
== Cyc_CfFlowInfo_NoneIL)({void*_tmp4AF=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmp482->hd)->loc,({const char*_tmp4B0="expression may not be initialized";
_tag_dyneither(_tmp4B0,sizeof(char),34);}),_tag_dyneither(_tmp4AF,sizeof(void*),
0));});if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4A9,r2)== Cyc_CfFlowInfo_NoneIL)({
void*_tmp4B1=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp482->tl))->hd)->loc,({const char*_tmp4B2="expression may not be initialized";
_tag_dyneither(_tmp4B2,sizeof(char),34);}),_tag_dyneither(_tmp4B1,sizeof(void*),
0));});if(_tmp481 == Cyc_Absyn_Eq  || _tmp481 == Cyc_Absyn_Neq){struct _tuple0
_tmp4B4=({struct _tuple0 _tmp4B3;_tmp4B3.f1=r1;_tmp4B3.f2=r2;_tmp4B3;});void*
_tmp4B5;enum Cyc_CfFlowInfo_InitLevel _tmp4B7;void*_tmp4B8;void*_tmp4BA;void*
_tmp4BC;enum Cyc_CfFlowInfo_InitLevel _tmp4BE;void*_tmp4BF;void*_tmp4C1;void*
_tmp4C3;void*_tmp4C5;void*_tmp4C7;void*_tmp4C9;void*_tmp4CB;void*_tmp4CD;void*
_tmp4CF;void*_tmp4D1;void*_tmp4D3;void*_tmp4D5;void*_tmp4D7;void*_tmp4D9;_LL25E:
_tmp4B5=_tmp4B4.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4B6=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4B5;if(_tmp4B6->tag != 3)goto _LL260;else{_tmp4B7=_tmp4B6->f1;}}_tmp4B8=
_tmp4B4.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4B9=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4B8;if(_tmp4B9->tag != 0)goto _LL260;}_LL25F: {union Cyc_CfFlowInfo_FlowInfo
_tmp4DC;union Cyc_CfFlowInfo_FlowInfo _tmp4DD;struct _tuple16 _tmp4DB=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4AD,_tmp4B7);_tmp4DC=_tmp4DB.f1;_tmp4DD=_tmp4DB.f2;switch(_tmp481){
case Cyc_Absyn_Eq: _LL272: return({struct _tuple16 _tmp4DE;_tmp4DE.f1=_tmp4DD;_tmp4DE.f2=
_tmp4DC;_tmp4DE;});case Cyc_Absyn_Neq: _LL273: return({struct _tuple16 _tmp4DF;
_tmp4DF.f1=_tmp4DC;_tmp4DF.f2=_tmp4DD;_tmp4DF;});default: _LL274:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));
_tmp4E0[0]=({struct Cyc_Core_Impossible_struct _tmp4E1;_tmp4E1.tag=Cyc_Core_Impossible;
_tmp4E1.f1=({const char*_tmp4E2="anal_test, zero-split";_tag_dyneither(_tmp4E2,
sizeof(char),22);});_tmp4E1;});_tmp4E0;}));}}_LL260: _tmp4BA=_tmp4B4.f1;{struct
Cyc_CfFlowInfo_Zero_struct*_tmp4BB=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4BA;
if(_tmp4BB->tag != 0)goto _LL262;}_tmp4BC=_tmp4B4.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4BD=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4BC;if(_tmp4BD->tag != 3)goto
_LL262;else{_tmp4BE=_tmp4BD->f1;}}_LL261: {union Cyc_CfFlowInfo_FlowInfo _tmp4E4;
union Cyc_CfFlowInfo_FlowInfo _tmp4E5;struct _tuple16 _tmp4E3=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4AE,_tmp4BE);_tmp4E4=_tmp4E3.f1;_tmp4E5=_tmp4E3.f2;switch(_tmp481){
case Cyc_Absyn_Eq: _LL276: return({struct _tuple16 _tmp4E6;_tmp4E6.f1=_tmp4E5;_tmp4E6.f2=
_tmp4E4;_tmp4E6;});case Cyc_Absyn_Neq: _LL277: return({struct _tuple16 _tmp4E7;
_tmp4E7.f1=_tmp4E4;_tmp4E7.f2=_tmp4E5;_tmp4E7;});default: _LL278:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));
_tmp4E8[0]=({struct Cyc_Core_Impossible_struct _tmp4E9;_tmp4E9.tag=Cyc_Core_Impossible;
_tmp4E9.f1=({const char*_tmp4EA="anal_test, zero-split";_tag_dyneither(_tmp4EA,
sizeof(char),22);});_tmp4E9;});_tmp4E8;}));}}_LL262: _tmp4BF=_tmp4B4.f1;{struct
Cyc_CfFlowInfo_Zero_struct*_tmp4C0=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4BF;
if(_tmp4C0->tag != 0)goto _LL264;}_tmp4C1=_tmp4B4.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4C2=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4C1;if(_tmp4C2->tag != 0)goto
_LL264;}_LL263: if(_tmp481 == Cyc_Absyn_Eq)return({struct _tuple16 _tmp4EB;_tmp4EB.f1=
f;_tmp4EB.f2=Cyc_CfFlowInfo_BottomFL();_tmp4EB;});else{return({struct _tuple16
_tmp4EC;_tmp4EC.f1=Cyc_CfFlowInfo_BottomFL();_tmp4EC.f2=f;_tmp4EC;});}_LL264:
_tmp4C3=_tmp4B4.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4C4=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4C3;if(_tmp4C4->tag != 0)goto _LL266;}_tmp4C5=_tmp4B4.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4C6=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4C5;if(_tmp4C6->tag != 1)
goto _LL266;}_LL265: goto _LL267;_LL266: _tmp4C7=_tmp4B4.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4C8=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4C7;if(_tmp4C8->tag != 0)goto
_LL268;}_tmp4C9=_tmp4B4.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4CA=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4C9;if(_tmp4CA->tag != 2)goto _LL268;}
_LL267: goto _LL269;_LL268: _tmp4CB=_tmp4B4.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4CC=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4CB;if(_tmp4CC->tag != 0)goto
_LL26A;}_tmp4CD=_tmp4B4.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4CE=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4CD;if(_tmp4CE->tag != 5)goto _LL26A;}
_LL269: goto _LL26B;_LL26A: _tmp4CF=_tmp4B4.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4D0=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4CF;if(_tmp4D0->tag != 1)
goto _LL26C;}_tmp4D1=_tmp4B4.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D2=(struct
Cyc_CfFlowInfo_Zero_struct*)_tmp4D1;if(_tmp4D2->tag != 0)goto _LL26C;}_LL26B: goto
_LL26D;_LL26C: _tmp4D3=_tmp4B4.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4D4=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4D3;if(_tmp4D4->tag != 2)
goto _LL26E;}_tmp4D5=_tmp4B4.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D6=(struct
Cyc_CfFlowInfo_Zero_struct*)_tmp4D5;if(_tmp4D6->tag != 0)goto _LL26E;}_LL26D: goto
_LL26F;_LL26E: _tmp4D7=_tmp4B4.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4D8=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4D7;if(_tmp4D8->tag != 5)goto _LL270;}
_tmp4D9=_tmp4B4.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4DA=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4D9;if(_tmp4DA->tag != 0)goto _LL270;}_LL26F: if(_tmp481 == Cyc_Absyn_Neq)return({
struct _tuple16 _tmp4ED;_tmp4ED.f1=f;_tmp4ED.f2=Cyc_CfFlowInfo_BottomFL();_tmp4ED;});
else{return({struct _tuple16 _tmp4EE;_tmp4EE.f1=Cyc_CfFlowInfo_BottomFL();_tmp4EE.f2=
f;_tmp4EE;});}_LL270:;_LL271: goto _LL25D;_LL25D:;}{struct _tuple0 _tmp4F0=({struct
_tuple0 _tmp4EF;_tmp4EF.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4AD->topt))->v);_tmp4EF.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4AE->topt))->v);_tmp4EF;});void*_tmp4F1;enum Cyc_Absyn_Sign
_tmp4F3;void*_tmp4F4;enum Cyc_Absyn_Sign _tmp4F6;void*_tmp4F7;void*_tmp4F9;_LL27B:
_tmp4F1=_tmp4F0.f1;{struct Cyc_Absyn_IntType_struct*_tmp4F2=(struct Cyc_Absyn_IntType_struct*)
_tmp4F1;if(_tmp4F2->tag != 6)goto _LL27D;else{_tmp4F3=_tmp4F2->f1;if(_tmp4F3 != Cyc_Absyn_Unsigned)
goto _LL27D;}}_LL27C: goto _LL27E;_LL27D: _tmp4F4=_tmp4F0.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4F5=(struct Cyc_Absyn_IntType_struct*)_tmp4F4;if(_tmp4F5->tag != 6)goto _LL27F;
else{_tmp4F6=_tmp4F5->f1;if(_tmp4F6 != Cyc_Absyn_Unsigned)goto _LL27F;}}_LL27E:
goto _LL280;_LL27F: _tmp4F7=_tmp4F0.f1;{struct Cyc_Absyn_TagType_struct*_tmp4F8=(
struct Cyc_Absyn_TagType_struct*)_tmp4F7;if(_tmp4F8->tag != 20)goto _LL281;}_LL280:
goto _LL282;_LL281: _tmp4F9=_tmp4F0.f2;{struct Cyc_Absyn_TagType_struct*_tmp4FA=(
struct Cyc_Absyn_TagType_struct*)_tmp4F9;if(_tmp4FA->tag != 20)goto _LL283;}_LL282:
goto _LL27A;_LL283:;_LL284: return({struct _tuple16 _tmp4FB;_tmp4FB.f1=f;_tmp4FB.f2=
f;_tmp4FB;});_LL27A:;}switch(_tmp481){case Cyc_Absyn_Eq: _LL285: {union Cyc_CfFlowInfo_FlowInfo
_tmp4FC=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AD,r1,Cyc_Absyn_Eq,r2);_tmp4FC=
Cyc_NewControlFlow_if_tagcmp(env,_tmp4FC,_tmp4AE,r2,Cyc_Absyn_Eq,r1);return({
struct _tuple16 _tmp4FD;_tmp4FD.f1=_tmp4FC;_tmp4FD.f2=f;_tmp4FD;});}case Cyc_Absyn_Neq:
_LL286: {union Cyc_CfFlowInfo_FlowInfo _tmp4FE=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp4AD,r1,Cyc_Absyn_Eq,r2);_tmp4FE=Cyc_NewControlFlow_if_tagcmp(env,_tmp4FE,
_tmp4AE,r2,Cyc_Absyn_Eq,r1);return({struct _tuple16 _tmp4FF;_tmp4FF.f1=f;_tmp4FF.f2=
_tmp4FE;_tmp4FF;});}case Cyc_Absyn_Gt: _LL287: {union Cyc_CfFlowInfo_FlowInfo
_tmp500=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AE,r2,Cyc_Absyn_Lt,r1);union Cyc_CfFlowInfo_FlowInfo
_tmp501=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AD,r1,Cyc_Absyn_Lte,r2);return({
struct _tuple16 _tmp502;_tmp502.f1=_tmp500;_tmp502.f2=_tmp501;_tmp502;});}case Cyc_Absyn_Gte:
_LL288: {union Cyc_CfFlowInfo_FlowInfo _tmp503=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp4AE,r2,Cyc_Absyn_Lte,r1);union Cyc_CfFlowInfo_FlowInfo _tmp504=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp4AD,r1,Cyc_Absyn_Lt,r2);return({struct _tuple16 _tmp505;_tmp505.f1=
_tmp503;_tmp505.f2=_tmp504;_tmp505;});}case Cyc_Absyn_Lt: _LL289: {union Cyc_CfFlowInfo_FlowInfo
_tmp506=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AD,r1,Cyc_Absyn_Lt,r2);union Cyc_CfFlowInfo_FlowInfo
_tmp507=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AE,r2,Cyc_Absyn_Lte,r1);{union
Cyc_CfFlowInfo_FlowInfo _tmp508=_tmp506;int _tmp509;struct _tuple12 _tmp50A;struct
Cyc_Dict_Dict _tmp50B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp50C;_LL28C: if((_tmp508.BottomFL).tag
!= 1)goto _LL28E;_tmp509=(int)(_tmp508.BottomFL).val;_LL28D:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D[0]=({
struct Cyc_Core_Impossible_struct _tmp50E;_tmp50E.tag=Cyc_Core_Impossible;_tmp50E.f1=({
const char*_tmp50F="anal_test, Lt";_tag_dyneither(_tmp50F,sizeof(char),14);});
_tmp50E;});_tmp50D;}));_LL28E: if((_tmp508.ReachableFL).tag != 2)goto _LL28B;
_tmp50A=(struct _tuple12)(_tmp508.ReachableFL).val;_tmp50B=_tmp50A.f1;_tmp50C=
_tmp50A.f3;_LL28F: _tmp4A9=_tmp50B;_tmp4AB=_tmp50C;_LL28B:;}{void*_tmp510=_tmp4AD->r;
void*_tmp512;struct Cyc_Absyn_Vardecl*_tmp514;void*_tmp516;struct Cyc_Absyn_Vardecl*
_tmp518;void*_tmp51A;struct Cyc_Absyn_Vardecl*_tmp51C;void*_tmp51E;struct Cyc_Absyn_Vardecl*
_tmp520;_LL291:{struct Cyc_Absyn_Var_e_struct*_tmp511=(struct Cyc_Absyn_Var_e_struct*)
_tmp510;if(_tmp511->tag != 1)goto _LL293;else{_tmp512=(void*)_tmp511->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp513=(struct Cyc_Absyn_Global_b_struct*)_tmp512;if(
_tmp513->tag != 1)goto _LL293;else{_tmp514=_tmp513->f1;}}}}if(!(!_tmp514->escapes))
goto _LL293;_LL292: _tmp518=_tmp514;goto _LL294;_LL293:{struct Cyc_Absyn_Var_e_struct*
_tmp515=(struct Cyc_Absyn_Var_e_struct*)_tmp510;if(_tmp515->tag != 1)goto _LL295;
else{_tmp516=(void*)_tmp515->f2;{struct Cyc_Absyn_Local_b_struct*_tmp517=(struct
Cyc_Absyn_Local_b_struct*)_tmp516;if(_tmp517->tag != 4)goto _LL295;else{_tmp518=
_tmp517->f1;}}}}if(!(!_tmp518->escapes))goto _LL295;_LL294: _tmp51C=_tmp518;goto
_LL296;_LL295:{struct Cyc_Absyn_Var_e_struct*_tmp519=(struct Cyc_Absyn_Var_e_struct*)
_tmp510;if(_tmp519->tag != 1)goto _LL297;else{_tmp51A=(void*)_tmp519->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp51B=(struct Cyc_Absyn_Pat_b_struct*)_tmp51A;if(_tmp51B->tag
!= 5)goto _LL297;else{_tmp51C=_tmp51B->f1;}}}}if(!(!_tmp51C->escapes))goto _LL297;
_LL296: _tmp520=_tmp51C;goto _LL298;_LL297:{struct Cyc_Absyn_Var_e_struct*_tmp51D=(
struct Cyc_Absyn_Var_e_struct*)_tmp510;if(_tmp51D->tag != 1)goto _LL299;else{
_tmp51E=(void*)_tmp51D->f2;{struct Cyc_Absyn_Param_b_struct*_tmp51F=(struct Cyc_Absyn_Param_b_struct*)
_tmp51E;if(_tmp51F->tag != 3)goto _LL299;else{_tmp520=_tmp51F->f1;}}}}if(!(!
_tmp520->escapes))goto _LL299;_LL298: {void*_tmp521=_tmp4AE->r;void*_tmp523;
struct Cyc_Absyn_Vardecl*_tmp525;void*_tmp527;struct Cyc_Absyn_Vardecl*_tmp529;
void*_tmp52B;struct Cyc_Absyn_Vardecl*_tmp52D;void*_tmp52F;struct Cyc_Absyn_Vardecl*
_tmp531;union Cyc_Absyn_Cnst _tmp533;struct _tuple6 _tmp534;int _tmp535;enum Cyc_Absyn_Primop
_tmp537;struct Cyc_List_List*_tmp538;struct Cyc_List_List _tmp539;struct Cyc_Absyn_Exp*
_tmp53A;_LL29C:{struct Cyc_Absyn_Var_e_struct*_tmp522=(struct Cyc_Absyn_Var_e_struct*)
_tmp521;if(_tmp522->tag != 1)goto _LL29E;else{_tmp523=(void*)_tmp522->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp524=(struct Cyc_Absyn_Global_b_struct*)_tmp523;if(
_tmp524->tag != 1)goto _LL29E;else{_tmp525=_tmp524->f1;}}}}if(!(!_tmp525->escapes))
goto _LL29E;_LL29D: _tmp529=_tmp525;goto _LL29F;_LL29E:{struct Cyc_Absyn_Var_e_struct*
_tmp526=(struct Cyc_Absyn_Var_e_struct*)_tmp521;if(_tmp526->tag != 1)goto _LL2A0;
else{_tmp527=(void*)_tmp526->f2;{struct Cyc_Absyn_Local_b_struct*_tmp528=(struct
Cyc_Absyn_Local_b_struct*)_tmp527;if(_tmp528->tag != 4)goto _LL2A0;else{_tmp529=
_tmp528->f1;}}}}if(!(!_tmp529->escapes))goto _LL2A0;_LL29F: _tmp52D=_tmp529;goto
_LL2A1;_LL2A0:{struct Cyc_Absyn_Var_e_struct*_tmp52A=(struct Cyc_Absyn_Var_e_struct*)
_tmp521;if(_tmp52A->tag != 1)goto _LL2A2;else{_tmp52B=(void*)_tmp52A->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp52C=(struct Cyc_Absyn_Pat_b_struct*)_tmp52B;if(_tmp52C->tag
!= 5)goto _LL2A2;else{_tmp52D=_tmp52C->f1;}}}}if(!(!_tmp52D->escapes))goto _LL2A2;
_LL2A1: _tmp531=_tmp52D;goto _LL2A3;_LL2A2:{struct Cyc_Absyn_Var_e_struct*_tmp52E=(
struct Cyc_Absyn_Var_e_struct*)_tmp521;if(_tmp52E->tag != 1)goto _LL2A4;else{
_tmp52F=(void*)_tmp52E->f2;{struct Cyc_Absyn_Param_b_struct*_tmp530=(struct Cyc_Absyn_Param_b_struct*)
_tmp52F;if(_tmp530->tag != 3)goto _LL2A4;else{_tmp531=_tmp530->f1;}}}}if(!(!
_tmp531->escapes))goto _LL2A4;_LL2A3: {struct _RegionHandle*_tmp53B=(env->fenv)->r;
struct Cyc_List_List*_tmp53C=({struct Cyc_List_List*_tmp53E=_region_malloc(_tmp53B,
sizeof(*_tmp53E));_tmp53E->hd=({struct Cyc_CfFlowInfo_Reln*_tmp53F=_region_malloc(
_tmp53B,sizeof(*_tmp53F));_tmp53F->vd=_tmp520;_tmp53F->rop=Cyc_CfFlowInfo_LessVar(
_tmp531,_tmp531->type);_tmp53F;});_tmp53E->tl=_tmp4AA;_tmp53E;});return({struct
_tuple16 _tmp53D;_tmp53D.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4A9,_tmp53C,_tmp4AB);
_tmp53D.f2=_tmp507;_tmp53D;});}_LL2A4: {struct Cyc_Absyn_Const_e_struct*_tmp532=(
struct Cyc_Absyn_Const_e_struct*)_tmp521;if(_tmp532->tag != 0)goto _LL2A6;else{
_tmp533=_tmp532->f1;if((_tmp533.Int_c).tag != 4)goto _LL2A6;_tmp534=(struct _tuple6)(
_tmp533.Int_c).val;_tmp535=_tmp534.f2;}}_LL2A5: {struct _RegionHandle*_tmp540=(
env->fenv)->r;struct Cyc_List_List*_tmp541=({struct Cyc_List_List*_tmp543=
_region_malloc(_tmp540,sizeof(*_tmp543));_tmp543->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp544=_region_malloc(_tmp540,sizeof(*_tmp544));_tmp544->vd=_tmp520;_tmp544->rop=
Cyc_CfFlowInfo_LessConst((unsigned int)_tmp535);_tmp544;});_tmp543->tl=_tmp4AA;
_tmp543;});return({struct _tuple16 _tmp542;_tmp542.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4A9,_tmp541,_tmp4AB);_tmp542.f2=_tmp507;_tmp542;});}_LL2A6: {struct Cyc_Absyn_Primop_e_struct*
_tmp536=(struct Cyc_Absyn_Primop_e_struct*)_tmp521;if(_tmp536->tag != 3)goto _LL2A8;
else{_tmp537=_tmp536->f1;_tmp538=_tmp536->f2;if(_tmp538 == 0)goto _LL2A8;_tmp539=*
_tmp538;_tmp53A=(struct Cyc_Absyn_Exp*)_tmp539.hd;}}_LL2A7: {void*_tmp545=_tmp53A->r;
void*_tmp547;struct Cyc_Absyn_Vardecl*_tmp549;void*_tmp54B;struct Cyc_Absyn_Vardecl*
_tmp54D;void*_tmp54F;struct Cyc_Absyn_Vardecl*_tmp551;void*_tmp553;struct Cyc_Absyn_Vardecl*
_tmp555;_LL2AB:{struct Cyc_Absyn_Var_e_struct*_tmp546=(struct Cyc_Absyn_Var_e_struct*)
_tmp545;if(_tmp546->tag != 1)goto _LL2AD;else{_tmp547=(void*)_tmp546->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp548=(struct Cyc_Absyn_Global_b_struct*)_tmp547;if(
_tmp548->tag != 1)goto _LL2AD;else{_tmp549=_tmp548->f1;}}}}if(!(!_tmp549->escapes))
goto _LL2AD;_LL2AC: _tmp54D=_tmp549;goto _LL2AE;_LL2AD:{struct Cyc_Absyn_Var_e_struct*
_tmp54A=(struct Cyc_Absyn_Var_e_struct*)_tmp545;if(_tmp54A->tag != 1)goto _LL2AF;
else{_tmp54B=(void*)_tmp54A->f2;{struct Cyc_Absyn_Local_b_struct*_tmp54C=(struct
Cyc_Absyn_Local_b_struct*)_tmp54B;if(_tmp54C->tag != 4)goto _LL2AF;else{_tmp54D=
_tmp54C->f1;}}}}if(!(!_tmp54D->escapes))goto _LL2AF;_LL2AE: _tmp551=_tmp54D;goto
_LL2B0;_LL2AF:{struct Cyc_Absyn_Var_e_struct*_tmp54E=(struct Cyc_Absyn_Var_e_struct*)
_tmp545;if(_tmp54E->tag != 1)goto _LL2B1;else{_tmp54F=(void*)_tmp54E->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp550=(struct Cyc_Absyn_Pat_b_struct*)_tmp54F;if(_tmp550->tag
!= 5)goto _LL2B1;else{_tmp551=_tmp550->f1;}}}}if(!(!_tmp551->escapes))goto _LL2B1;
_LL2B0: _tmp555=_tmp551;goto _LL2B2;_LL2B1:{struct Cyc_Absyn_Var_e_struct*_tmp552=(
struct Cyc_Absyn_Var_e_struct*)_tmp545;if(_tmp552->tag != 1)goto _LL2B3;else{
_tmp553=(void*)_tmp552->f2;{struct Cyc_Absyn_Param_b_struct*_tmp554=(struct Cyc_Absyn_Param_b_struct*)
_tmp553;if(_tmp554->tag != 3)goto _LL2B3;else{_tmp555=_tmp554->f1;}}}}if(!(!
_tmp555->escapes))goto _LL2B3;_LL2B2: {struct _RegionHandle*_tmp556=(env->fenv)->r;
struct Cyc_List_List*_tmp557=({struct Cyc_List_List*_tmp559=_region_malloc(_tmp556,
sizeof(*_tmp559));_tmp559->hd=({struct Cyc_CfFlowInfo_Reln*_tmp55A=_region_malloc(
_tmp556,sizeof(*_tmp55A));_tmp55A->vd=_tmp520;_tmp55A->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp555);_tmp55A;});_tmp559->tl=_tmp4AA;_tmp559;});return({struct _tuple16 _tmp558;
_tmp558.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4A9,_tmp557,_tmp4AB);_tmp558.f2=
_tmp507;_tmp558;});}_LL2B3:;_LL2B4: return({struct _tuple16 _tmp55B;_tmp55B.f1=
_tmp506;_tmp55B.f2=_tmp507;_tmp55B;});_LL2AA:;}_LL2A8:;_LL2A9: return({struct
_tuple16 _tmp55C;_tmp55C.f1=_tmp506;_tmp55C.f2=_tmp507;_tmp55C;});_LL29B:;}_LL299:;
_LL29A: return({struct _tuple16 _tmp55D;_tmp55D.f1=_tmp506;_tmp55D.f2=_tmp507;
_tmp55D;});_LL290:;}}case Cyc_Absyn_Lte: _LL28A: {union Cyc_CfFlowInfo_FlowInfo
_tmp55E=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AD,r1,Cyc_Absyn_Lte,r2);union Cyc_CfFlowInfo_FlowInfo
_tmp55F=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AE,r1,Cyc_Absyn_Lt,r1);{union Cyc_CfFlowInfo_FlowInfo
_tmp560=_tmp55E;int _tmp561;struct _tuple12 _tmp562;struct Cyc_Dict_Dict _tmp563;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp564;_LL2B7: if((_tmp560.BottomFL).tag != 1)
goto _LL2B9;_tmp561=(int)(_tmp560.BottomFL).val;_LL2B8:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565[0]=({
struct Cyc_Core_Impossible_struct _tmp566;_tmp566.tag=Cyc_Core_Impossible;_tmp566.f1=({
const char*_tmp567="anal_test, Lte";_tag_dyneither(_tmp567,sizeof(char),15);});
_tmp566;});_tmp565;}));_LL2B9: if((_tmp560.ReachableFL).tag != 2)goto _LL2B6;
_tmp562=(struct _tuple12)(_tmp560.ReachableFL).val;_tmp563=_tmp562.f1;_tmp564=
_tmp562.f3;_LL2BA: _tmp4A9=_tmp563;_tmp4AB=_tmp564;_LL2B6:;}{void*_tmp568=_tmp4AD->r;
void*_tmp56A;struct Cyc_Absyn_Vardecl*_tmp56C;void*_tmp56E;struct Cyc_Absyn_Vardecl*
_tmp570;void*_tmp572;struct Cyc_Absyn_Vardecl*_tmp574;void*_tmp576;struct Cyc_Absyn_Vardecl*
_tmp578;_LL2BC:{struct Cyc_Absyn_Var_e_struct*_tmp569=(struct Cyc_Absyn_Var_e_struct*)
_tmp568;if(_tmp569->tag != 1)goto _LL2BE;else{_tmp56A=(void*)_tmp569->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp56B=(struct Cyc_Absyn_Global_b_struct*)_tmp56A;if(
_tmp56B->tag != 1)goto _LL2BE;else{_tmp56C=_tmp56B->f1;}}}}if(!(!_tmp56C->escapes))
goto _LL2BE;_LL2BD: _tmp570=_tmp56C;goto _LL2BF;_LL2BE:{struct Cyc_Absyn_Var_e_struct*
_tmp56D=(struct Cyc_Absyn_Var_e_struct*)_tmp568;if(_tmp56D->tag != 1)goto _LL2C0;
else{_tmp56E=(void*)_tmp56D->f2;{struct Cyc_Absyn_Local_b_struct*_tmp56F=(struct
Cyc_Absyn_Local_b_struct*)_tmp56E;if(_tmp56F->tag != 4)goto _LL2C0;else{_tmp570=
_tmp56F->f1;}}}}if(!(!_tmp570->escapes))goto _LL2C0;_LL2BF: _tmp574=_tmp570;goto
_LL2C1;_LL2C0:{struct Cyc_Absyn_Var_e_struct*_tmp571=(struct Cyc_Absyn_Var_e_struct*)
_tmp568;if(_tmp571->tag != 1)goto _LL2C2;else{_tmp572=(void*)_tmp571->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp573=(struct Cyc_Absyn_Pat_b_struct*)_tmp572;if(_tmp573->tag
!= 5)goto _LL2C2;else{_tmp574=_tmp573->f1;}}}}if(!(!_tmp574->escapes))goto _LL2C2;
_LL2C1: _tmp578=_tmp574;goto _LL2C3;_LL2C2:{struct Cyc_Absyn_Var_e_struct*_tmp575=(
struct Cyc_Absyn_Var_e_struct*)_tmp568;if(_tmp575->tag != 1)goto _LL2C4;else{
_tmp576=(void*)_tmp575->f2;{struct Cyc_Absyn_Param_b_struct*_tmp577=(struct Cyc_Absyn_Param_b_struct*)
_tmp576;if(_tmp577->tag != 3)goto _LL2C4;else{_tmp578=_tmp577->f1;}}}}if(!(!
_tmp578->escapes))goto _LL2C4;_LL2C3: {void*_tmp579=_tmp4AE->r;enum Cyc_Absyn_Primop
_tmp57B;struct Cyc_List_List*_tmp57C;struct Cyc_List_List _tmp57D;struct Cyc_Absyn_Exp*
_tmp57E;_LL2C7: {struct Cyc_Absyn_Primop_e_struct*_tmp57A=(struct Cyc_Absyn_Primop_e_struct*)
_tmp579;if(_tmp57A->tag != 3)goto _LL2C9;else{_tmp57B=_tmp57A->f1;_tmp57C=_tmp57A->f2;
if(_tmp57C == 0)goto _LL2C9;_tmp57D=*_tmp57C;_tmp57E=(struct Cyc_Absyn_Exp*)_tmp57D.hd;}}
_LL2C8: {void*_tmp57F=_tmp57E->r;void*_tmp581;struct Cyc_Absyn_Vardecl*_tmp583;
void*_tmp585;struct Cyc_Absyn_Vardecl*_tmp587;void*_tmp589;struct Cyc_Absyn_Vardecl*
_tmp58B;void*_tmp58D;struct Cyc_Absyn_Vardecl*_tmp58F;_LL2CC:{struct Cyc_Absyn_Var_e_struct*
_tmp580=(struct Cyc_Absyn_Var_e_struct*)_tmp57F;if(_tmp580->tag != 1)goto _LL2CE;
else{_tmp581=(void*)_tmp580->f2;{struct Cyc_Absyn_Global_b_struct*_tmp582=(struct
Cyc_Absyn_Global_b_struct*)_tmp581;if(_tmp582->tag != 1)goto _LL2CE;else{_tmp583=
_tmp582->f1;}}}}if(!(!_tmp583->escapes))goto _LL2CE;_LL2CD: _tmp587=_tmp583;goto
_LL2CF;_LL2CE:{struct Cyc_Absyn_Var_e_struct*_tmp584=(struct Cyc_Absyn_Var_e_struct*)
_tmp57F;if(_tmp584->tag != 1)goto _LL2D0;else{_tmp585=(void*)_tmp584->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp586=(struct Cyc_Absyn_Local_b_struct*)_tmp585;if(
_tmp586->tag != 4)goto _LL2D0;else{_tmp587=_tmp586->f1;}}}}if(!(!_tmp587->escapes))
goto _LL2D0;_LL2CF: _tmp58B=_tmp587;goto _LL2D1;_LL2D0:{struct Cyc_Absyn_Var_e_struct*
_tmp588=(struct Cyc_Absyn_Var_e_struct*)_tmp57F;if(_tmp588->tag != 1)goto _LL2D2;
else{_tmp589=(void*)_tmp588->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp58A=(struct Cyc_Absyn_Pat_b_struct*)
_tmp589;if(_tmp58A->tag != 5)goto _LL2D2;else{_tmp58B=_tmp58A->f1;}}}}if(!(!
_tmp58B->escapes))goto _LL2D2;_LL2D1: _tmp58F=_tmp58B;goto _LL2D3;_LL2D2:{struct Cyc_Absyn_Var_e_struct*
_tmp58C=(struct Cyc_Absyn_Var_e_struct*)_tmp57F;if(_tmp58C->tag != 1)goto _LL2D4;
else{_tmp58D=(void*)_tmp58C->f2;{struct Cyc_Absyn_Param_b_struct*_tmp58E=(struct
Cyc_Absyn_Param_b_struct*)_tmp58D;if(_tmp58E->tag != 3)goto _LL2D4;else{_tmp58F=
_tmp58E->f1;}}}}if(!(!_tmp58F->escapes))goto _LL2D4;_LL2D3: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp590=env->fenv;struct Cyc_List_List*_tmp591=({struct Cyc_List_List*_tmp593=
_region_malloc(_tmp590->r,sizeof(*_tmp593));_tmp593->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp594=_region_malloc(_tmp590->r,sizeof(*_tmp594));_tmp594->vd=_tmp578;_tmp594->rop=
Cyc_CfFlowInfo_LessEqNumelts(_tmp58F);_tmp594;});_tmp593->tl=_tmp4AA;_tmp593;});
return({struct _tuple16 _tmp592;_tmp592.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4A9,
_tmp591,_tmp4AB);_tmp592.f2=_tmp55F;_tmp592;});}_LL2D4:;_LL2D5: return({struct
_tuple16 _tmp595;_tmp595.f1=_tmp55E;_tmp595.f2=_tmp55F;_tmp595;});_LL2CB:;}_LL2C9:;
_LL2CA: return({struct _tuple16 _tmp596;_tmp596.f1=_tmp55E;_tmp596.f2=_tmp55F;
_tmp596;});_LL2C6:;}_LL2C4:;_LL2C5: return({struct _tuple16 _tmp597;_tmp597.f1=
_tmp55E;_tmp597.f2=_tmp55F;_tmp597;});_LL2BB:;}}default: _LL2B5: return({struct
_tuple16 _tmp598;_tmp598.f1=f;_tmp598.f2=f;_tmp598;});}}_LL258:;}}_LL256:;_LL257:
goto _LL249;_LL249:;}{union Cyc_CfFlowInfo_FlowInfo _tmp59A;void*_tmp59B;struct
_tuple13 _tmp599=Cyc_NewControlFlow_anal_Rexp(env,inflow,e);_tmp59A=_tmp599.f1;
_tmp59B=_tmp599.f2;_tmp59A=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp59A);{union Cyc_CfFlowInfo_FlowInfo _tmp59C=_tmp59A;int _tmp59D;struct _tuple12
_tmp59E;struct Cyc_Dict_Dict _tmp59F;_LL2D8: if((_tmp59C.BottomFL).tag != 1)goto
_LL2DA;_tmp59D=(int)(_tmp59C.BottomFL).val;_LL2D9: return({struct _tuple16 _tmp5A0;
_tmp5A0.f1=_tmp59A;_tmp5A0.f2=_tmp59A;_tmp5A0;});_LL2DA: if((_tmp59C.ReachableFL).tag
!= 2)goto _LL2D7;_tmp59E=(struct _tuple12)(_tmp59C.ReachableFL).val;_tmp59F=
_tmp59E.f1;_LL2DB: {void*_tmp5A1=_tmp59B;enum Cyc_CfFlowInfo_InitLevel _tmp5A7;
enum Cyc_CfFlowInfo_InitLevel _tmp5A9;enum Cyc_CfFlowInfo_InitLevel _tmp5AB;_LL2DD: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp5A2=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp5A1;if(_tmp5A2->tag != 0)goto _LL2DF;}_LL2DE: return({struct _tuple16 _tmp5AF;
_tmp5AF.f1=Cyc_CfFlowInfo_BottomFL();_tmp5AF.f2=_tmp59A;_tmp5AF;});_LL2DF: {
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp5A3=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp5A1;if(_tmp5A3->tag != 2)goto _LL2E1;}_LL2E0: goto _LL2E2;_LL2E1: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp5A4=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp5A1;if(_tmp5A4->tag != 1)
goto _LL2E3;}_LL2E2: goto _LL2E4;_LL2E3: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5A5=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5A1;if(_tmp5A5->tag != 5)goto
_LL2E5;}_LL2E4: return({struct _tuple16 _tmp5B0;_tmp5B0.f1=_tmp59A;_tmp5B0.f2=Cyc_CfFlowInfo_BottomFL();
_tmp5B0;});_LL2E5: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5A6=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5A1;if(_tmp5A6->tag != 3)goto _LL2E7;else{_tmp5A7=_tmp5A6->f1;if(_tmp5A7 != Cyc_CfFlowInfo_NoneIL)
goto _LL2E7;}}_LL2E6: goto _LL2E8;_LL2E7: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5A8=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp5A1;if(_tmp5A8->tag != 4)goto _LL2E9;else{
_tmp5A9=_tmp5A8->f1;if(_tmp5A9 != Cyc_CfFlowInfo_NoneIL)goto _LL2E9;}}_LL2E8:({
void*_tmp5B1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5B2="expression may not be initialized";
_tag_dyneither(_tmp5B2,sizeof(char),34);}),_tag_dyneither(_tmp5B1,sizeof(void*),
0));});return({struct _tuple16 _tmp5B3;_tmp5B3.f1=Cyc_CfFlowInfo_BottomFL();
_tmp5B3.f2=Cyc_CfFlowInfo_BottomFL();_tmp5B3;});_LL2E9: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp5AA=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5A1;if(_tmp5AA->tag != 3)goto
_LL2EB;else{_tmp5AB=_tmp5AA->f1;}}_LL2EA: return Cyc_NewControlFlow_splitzero(env,
inflow,_tmp59A,e,_tmp5AB);_LL2EB: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5AC=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp5A1;if(_tmp5AC->tag != 4)goto _LL2ED;}_LL2EC:
return({struct _tuple16 _tmp5B4;_tmp5B4.f1=_tmp59A;_tmp5B4.f2=_tmp59A;_tmp5B4;});
_LL2ED: {struct Cyc_CfFlowInfo_TagCmps_struct*_tmp5AD=(struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp5A1;if(_tmp5AD->tag != 6)goto _LL2EF;}_LL2EE: return({struct _tuple16 _tmp5B5;
_tmp5B5.f1=_tmp59A;_tmp5B5.f2=_tmp59A;_tmp5B5;});_LL2EF: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5AE=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp5A1;if(_tmp5AE->tag != 7)goto
_LL2DC;}_LL2F0:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp5B6=
_cycalloc(sizeof(*_tmp5B6));_tmp5B6[0]=({struct Cyc_Core_Impossible_struct _tmp5B7;
_tmp5B7.tag=Cyc_Core_Impossible;_tmp5B7.f1=({const char*_tmp5B8="anal_test";
_tag_dyneither(_tmp5B8,sizeof(char),10);});_tmp5B7;});_tmp5B6;}));_LL2DC:;}
_LL2D7:;}}}static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){union
Cyc_CfFlowInfo_FlowInfo _tmp5B9=flow;int _tmp5BA;struct _tuple12 _tmp5BB;struct Cyc_Dict_Dict
_tmp5BC;_LL2F2: if((_tmp5B9.BottomFL).tag != 1)goto _LL2F4;_tmp5BA=(int)(_tmp5B9.BottomFL).val;
_LL2F3: return;_LL2F4: if((_tmp5B9.ReachableFL).tag != 2)goto _LL2F1;_tmp5BB=(struct
_tuple12)(_tmp5B9.ReachableFL).val;_tmp5BC=_tmp5BB.f1;_LL2F5:{struct Cyc_List_List*
_tmp5BD=env->param_roots;for(0;_tmp5BD != 0;_tmp5BD=_tmp5BD->tl){if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp5BC,Cyc_CfFlowInfo_lookup_place(_tmp5BC,(struct Cyc_CfFlowInfo_Place*)
_tmp5BD->hd))!= Cyc_CfFlowInfo_AllIL)({void*_tmp5BE=0;Cyc_Tcutil_terr(loc,({
const char*_tmp5BF="function may not initialize all the parameters with attribute 'initializes'";
_tag_dyneither(_tmp5BF,sizeof(char),76);}),_tag_dyneither(_tmp5BE,sizeof(void*),
0));});}}return;_LL2F1:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp5C0=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp5C2;struct Cyc_Core_Opt*_tmp5C3;
struct Cyc_Absyn_Exp*_tmp5C4;struct Cyc_Absyn_Stmt*_tmp5C5;struct Cyc_Position_Segment*
_tmp5C6;struct Cyc_Absyn_Switch_clause*_tmp5C1=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp5C2=*_tmp5C1;_tmp5C3=_tmp5C2.pat_vars;_tmp5C4=_tmp5C2.where_clause;
_tmp5C5=_tmp5C2.body;_tmp5C6=_tmp5C2.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5C0,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5C3))->v,_tmp5C0->unknown_all,_tmp5C6);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp5C4 != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp5C4;union Cyc_CfFlowInfo_FlowInfo _tmp5C8;union Cyc_CfFlowInfo_FlowInfo
_tmp5C9;struct _tuple16 _tmp5C7=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp5C8=_tmp5C7.f1;_tmp5C9=_tmp5C7.f2;_tmp5C8=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp5C8);_tmp5C9=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp5C9);inflow=_tmp5C9;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5C8,
_tmp5C5);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp5C5);}{union Cyc_CfFlowInfo_FlowInfo _tmp5CA=clause_outflow;int _tmp5CB;_LL2F7:
if((_tmp5CA.BottomFL).tag != 1)goto _LL2F9;_tmp5CB=(int)(_tmp5CA.BottomFL).val;
_LL2F8: goto _LL2F6;_LL2F9:;_LL2FA: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*
_tmp5CC=0;Cyc_Tcutil_terr(_tmp5C5->loc,({const char*_tmp5CD="switch clause may implicitly fallthru";
_tag_dyneither(_tmp5CD,sizeof(char),38);}),_tag_dyneither(_tmp5CC,sizeof(void*),
0));});else{({void*_tmp5CE=0;Cyc_Tcutil_warn(_tmp5C5->loc,({const char*_tmp5CF="switch clause may implicitly fallthru";
_tag_dyneither(_tmp5CF,sizeof(char),38);}),_tag_dyneither(_tmp5CE,sizeof(void*),
0));});}Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}goto
_LL2F6;_LL2F6:;}}}return Cyc_CfFlowInfo_BottomFL();}static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5D1;union Cyc_CfFlowInfo_FlowInfo*_tmp5D2;struct _tuple17 _tmp5D0=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp5D1=_tmp5D0.f1;_tmp5D2=_tmp5D0.f2;inflow=*_tmp5D2;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp5D3=env->fenv;{void*_tmp5D4=s->r;struct Cyc_Absyn_Exp*_tmp5D7;struct Cyc_Absyn_Exp*
_tmp5D9;struct Cyc_Absyn_Exp*_tmp5DB;struct Cyc_Absyn_Stmt*_tmp5DD;struct Cyc_Absyn_Stmt*
_tmp5DE;struct Cyc_Absyn_Exp*_tmp5E0;struct Cyc_Absyn_Stmt*_tmp5E1;struct Cyc_Absyn_Stmt*
_tmp5E2;struct _tuple9 _tmp5E4;struct Cyc_Absyn_Exp*_tmp5E5;struct Cyc_Absyn_Stmt*
_tmp5E6;struct Cyc_Absyn_Stmt*_tmp5E7;struct Cyc_Absyn_Stmt*_tmp5E9;struct _tuple9
_tmp5EA;struct Cyc_Absyn_Exp*_tmp5EB;struct Cyc_Absyn_Stmt*_tmp5EC;struct Cyc_Absyn_Exp*
_tmp5EE;struct _tuple9 _tmp5EF;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Stmt*
_tmp5F1;struct _tuple9 _tmp5F2;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_Absyn_Stmt*
_tmp5F4;struct Cyc_Absyn_Stmt*_tmp5F5;struct Cyc_Absyn_Stmt*_tmp5F7;struct Cyc_List_List*
_tmp5F9;struct Cyc_Absyn_Switch_clause**_tmp5FA;struct Cyc_Absyn_Switch_clause*
_tmp5FB;struct Cyc_Absyn_Stmt*_tmp5FD;struct Cyc_Absyn_Stmt*_tmp5FF;struct Cyc_Absyn_Stmt*
_tmp601;struct Cyc_Absyn_Exp*_tmp603;struct Cyc_List_List*_tmp604;struct Cyc_Absyn_Stmt*
_tmp606;struct Cyc_List_List*_tmp607;struct Cyc_Absyn_Decl*_tmp609;struct Cyc_Absyn_Decl
_tmp60A;void*_tmp60B;struct Cyc_Absyn_Exp*_tmp60D;struct Cyc_Absyn_Tvar*_tmp60E;
struct Cyc_Absyn_Vardecl*_tmp60F;struct Cyc_Absyn_Stmt*_tmp610;struct Cyc_Absyn_Decl*
_tmp612;struct Cyc_Absyn_Stmt*_tmp613;struct Cyc_Absyn_Stmt*_tmp615;struct Cyc_Absyn_Exp*
_tmp617;_LL2FC: {struct Cyc_Absyn_Skip_s_struct*_tmp5D5=(struct Cyc_Absyn_Skip_s_struct*)
_tmp5D4;if(_tmp5D5->tag != 0)goto _LL2FE;}_LL2FD: return inflow;_LL2FE: {struct Cyc_Absyn_Return_s_struct*
_tmp5D6=(struct Cyc_Absyn_Return_s_struct*)_tmp5D4;if(_tmp5D6->tag != 3)goto _LL300;
else{_tmp5D7=_tmp5D6->f1;if(_tmp5D7 != 0)goto _LL300;}}_LL2FF: if(env->noreturn)({
void*_tmp618=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp619="`noreturn' function might return";
_tag_dyneither(_tmp619,sizeof(char),33);}),_tag_dyneither(_tmp618,sizeof(void*),
0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL300: {struct Cyc_Absyn_Return_s_struct*_tmp5D8=(struct Cyc_Absyn_Return_s_struct*)
_tmp5D4;if(_tmp5D8->tag != 3)goto _LL302;else{_tmp5D9=_tmp5D8->f1;}}_LL301: if(env->noreturn)({
void*_tmp61A=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp61B="`noreturn' function might return";
_tag_dyneither(_tmp61B,sizeof(char),33);}),_tag_dyneither(_tmp61A,sizeof(void*),
0));});{union Cyc_CfFlowInfo_FlowInfo _tmp61D;void*_tmp61E;struct _tuple13 _tmp61C=
Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)_check_null(
_tmp5D9));_tmp61D=_tmp61C.f1;_tmp61E=_tmp61C.f2;_tmp61D=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp5D3,_tmp5D9->loc,_tmp61D);_tmp61D=Cyc_NewControlFlow_use_Rval(env,_tmp5D9->loc,
_tmp61D,_tmp61E);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp61D);return
Cyc_CfFlowInfo_BottomFL();}_LL302: {struct Cyc_Absyn_Exp_s_struct*_tmp5DA=(struct
Cyc_Absyn_Exp_s_struct*)_tmp5D4;if(_tmp5DA->tag != 1)goto _LL304;else{_tmp5DB=
_tmp5DA->f1;}}_LL303: outflow=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp5DB)).f1;
goto _LL2FB;_LL304: {struct Cyc_Absyn_Seq_s_struct*_tmp5DC=(struct Cyc_Absyn_Seq_s_struct*)
_tmp5D4;if(_tmp5DC->tag != 2)goto _LL306;else{_tmp5DD=_tmp5DC->f1;_tmp5DE=_tmp5DC->f2;}}
_LL305: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,
inflow,_tmp5DD),_tmp5DE);goto _LL2FB;_LL306: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp5DF=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5D4;if(_tmp5DF->tag != 4)goto
_LL308;else{_tmp5E0=_tmp5DF->f1;_tmp5E1=_tmp5DF->f2;_tmp5E2=_tmp5DF->f3;}}_LL307: {
union Cyc_CfFlowInfo_FlowInfo _tmp620;union Cyc_CfFlowInfo_FlowInfo _tmp621;struct
_tuple16 _tmp61F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5E0);_tmp620=_tmp61F.f1;
_tmp621=_tmp61F.f2;_tmp620=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp5E0->loc,
_tmp620);_tmp621=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp5E0->loc,_tmp621);
outflow=Cyc_CfFlowInfo_join_flow(_tmp5D3,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp620,_tmp5E1),Cyc_NewControlFlow_anal_stmt(env,_tmp621,_tmp5E2),1);goto
_LL2FB;}_LL308: {struct Cyc_Absyn_While_s_struct*_tmp5E3=(struct Cyc_Absyn_While_s_struct*)
_tmp5D4;if(_tmp5E3->tag != 5)goto _LL30A;else{_tmp5E4=_tmp5E3->f1;_tmp5E5=_tmp5E4.f1;
_tmp5E6=_tmp5E4.f2;_tmp5E7=_tmp5E3->f2;}}_LL309: {union Cyc_CfFlowInfo_FlowInfo*
_tmp623;struct _tuple17 _tmp622=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp5E6);_tmp623=_tmp622.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp624=*_tmp623;union
Cyc_CfFlowInfo_FlowInfo _tmp626;union Cyc_CfFlowInfo_FlowInfo _tmp627;struct
_tuple16 _tmp625=Cyc_NewControlFlow_anal_test(env,_tmp624,_tmp5E5);_tmp626=
_tmp625.f1;_tmp627=_tmp625.f2;_tmp626=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp5E5->loc,_tmp626);_tmp627=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp5E5->loc,
_tmp627);{union Cyc_CfFlowInfo_FlowInfo _tmp628=Cyc_NewControlFlow_anal_stmt(env,
_tmp626,_tmp5E7);Cyc_NewControlFlow_update_flow(env,_tmp5E6,_tmp628);outflow=
_tmp627;goto _LL2FB;}}}_LL30A: {struct Cyc_Absyn_Do_s_struct*_tmp5E8=(struct Cyc_Absyn_Do_s_struct*)
_tmp5D4;if(_tmp5E8->tag != 14)goto _LL30C;else{_tmp5E9=_tmp5E8->f1;_tmp5EA=_tmp5E8->f2;
_tmp5EB=_tmp5EA.f1;_tmp5EC=_tmp5EA.f2;}}_LL30B: {union Cyc_CfFlowInfo_FlowInfo
_tmp629=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E9);union Cyc_CfFlowInfo_FlowInfo*
_tmp62B;struct _tuple17 _tmp62A=Cyc_NewControlFlow_pre_stmt_check(env,_tmp629,
_tmp5EC);_tmp62B=_tmp62A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp62C=*_tmp62B;union
Cyc_CfFlowInfo_FlowInfo _tmp62E;union Cyc_CfFlowInfo_FlowInfo _tmp62F;struct
_tuple16 _tmp62D=Cyc_NewControlFlow_anal_test(env,_tmp62C,_tmp5EB);_tmp62E=
_tmp62D.f1;_tmp62F=_tmp62D.f2;Cyc_NewControlFlow_update_flow(env,_tmp5E9,_tmp62E);
outflow=_tmp62F;goto _LL2FB;}}_LL30C: {struct Cyc_Absyn_For_s_struct*_tmp5ED=(
struct Cyc_Absyn_For_s_struct*)_tmp5D4;if(_tmp5ED->tag != 9)goto _LL30E;else{
_tmp5EE=_tmp5ED->f1;_tmp5EF=_tmp5ED->f2;_tmp5F0=_tmp5EF.f1;_tmp5F1=_tmp5EF.f2;
_tmp5F2=_tmp5ED->f3;_tmp5F3=_tmp5F2.f1;_tmp5F4=_tmp5F2.f2;_tmp5F5=_tmp5ED->f4;}}
_LL30D: {union Cyc_CfFlowInfo_FlowInfo _tmp630=(Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp5EE)).f1;_tmp630=Cyc_CfFlowInfo_drop_unique_rvals(_tmp5EE->loc,
_tmp630);{union Cyc_CfFlowInfo_FlowInfo*_tmp632;struct _tuple17 _tmp631=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp630,_tmp5F1);_tmp632=_tmp631.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp633=*
_tmp632;union Cyc_CfFlowInfo_FlowInfo _tmp635;union Cyc_CfFlowInfo_FlowInfo _tmp636;
struct _tuple16 _tmp634=Cyc_NewControlFlow_anal_test(env,_tmp633,_tmp5F0);_tmp635=
_tmp634.f1;_tmp636=_tmp634.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp637=Cyc_NewControlFlow_anal_stmt(
env,_tmp635,_tmp5F5);union Cyc_CfFlowInfo_FlowInfo*_tmp639;struct _tuple17 _tmp638=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp637,_tmp5F4);_tmp639=_tmp638.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp63A=*_tmp639;union Cyc_CfFlowInfo_FlowInfo _tmp63B=(Cyc_NewControlFlow_anal_Rexp(
env,_tmp63A,_tmp5F3)).f1;_tmp63B=Cyc_CfFlowInfo_drop_unique_rvals(_tmp5F3->loc,
_tmp63B);Cyc_NewControlFlow_update_flow(env,_tmp5F1,_tmp63B);outflow=_tmp636;
goto _LL2FB;}}}}}_LL30E: {struct Cyc_Absyn_Break_s_struct*_tmp5F6=(struct Cyc_Absyn_Break_s_struct*)
_tmp5D4;if(_tmp5F6->tag != 6)goto _LL310;else{_tmp5F7=_tmp5F6->f1;if(_tmp5F7 != 0)
goto _LL310;}}_LL30F: return Cyc_CfFlowInfo_BottomFL();_LL310: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp5F8=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp5D4;if(_tmp5F8->tag != 11)goto
_LL312;else{_tmp5F9=_tmp5F8->f1;_tmp5FA=_tmp5F8->f2;if(_tmp5FA == 0)goto _LL312;
_tmp5FB=*_tmp5FA;}}_LL311: {struct _RegionHandle*_tmp63C=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp63E;struct Cyc_List_List*_tmp63F;struct _tuple19 _tmp63D=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp63C,env,inflow,_tmp5F9,0);_tmp63E=_tmp63D.f1;_tmp63F=_tmp63D.f2;for(0;
_tmp63F != 0;(_tmp63F=_tmp63F->tl,_tmp5F9=_tmp5F9->tl)){_tmp63E=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5F9))->hd)->loc,
_tmp63E,(void*)_tmp63F->hd);}_tmp63E=Cyc_CfFlowInfo_consume_unique_rvals(_tmp5D3,
s->loc,_tmp63E);_tmp63E=Cyc_NewControlFlow_add_vars(_tmp5D3,_tmp63E,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5FB->pat_vars))->v,_tmp5D3->unknown_all,s->loc);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp5FB->body,_tmp63E);
return Cyc_CfFlowInfo_BottomFL();}_LL312: {struct Cyc_Absyn_Break_s_struct*_tmp5FC=(
struct Cyc_Absyn_Break_s_struct*)_tmp5D4;if(_tmp5FC->tag != 6)goto _LL314;else{
_tmp5FD=_tmp5FC->f1;}}_LL313: _tmp5FF=_tmp5FD;goto _LL315;_LL314: {struct Cyc_Absyn_Continue_s_struct*
_tmp5FE=(struct Cyc_Absyn_Continue_s_struct*)_tmp5D4;if(_tmp5FE->tag != 7)goto
_LL316;else{_tmp5FF=_tmp5FE->f1;}}_LL315: _tmp601=_tmp5FF;goto _LL317;_LL316: {
struct Cyc_Absyn_Goto_s_struct*_tmp600=(struct Cyc_Absyn_Goto_s_struct*)_tmp5D4;
if(_tmp600->tag != 8)goto _LL318;else{_tmp601=_tmp600->f2;}}_LL317: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp640=_tmp5D1->encloser;struct Cyc_Absyn_Stmt*
_tmp641=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp601)))->encloser;while(_tmp641 != _tmp640){struct Cyc_Absyn_Stmt*_tmp642=(Cyc_NewControlFlow_get_stmt_annot(
_tmp640))->encloser;if(_tmp642 == _tmp640){({void*_tmp643=0;Cyc_Tcutil_terr(s->loc,({
const char*_tmp644="goto enters local scope or exception handler";_tag_dyneither(
_tmp644,sizeof(char),45);}),_tag_dyneither(_tmp643,sizeof(void*),0));});break;}
_tmp640=_tmp642;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp601),inflow);return Cyc_CfFlowInfo_BottomFL();_LL318: {struct Cyc_Absyn_Switch_s_struct*
_tmp602=(struct Cyc_Absyn_Switch_s_struct*)_tmp5D4;if(_tmp602->tag != 10)goto
_LL31A;else{_tmp603=_tmp602->f1;_tmp604=_tmp602->f2;}}_LL319: {union Cyc_CfFlowInfo_FlowInfo
_tmp646;void*_tmp647;struct _tuple13 _tmp645=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp603);_tmp646=_tmp645.f1;_tmp647=_tmp645.f2;_tmp646=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp5D3,_tmp603->loc,_tmp646);_tmp646=Cyc_NewControlFlow_use_Rval(env,_tmp603->loc,
_tmp646,_tmp647);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp646,_tmp604);goto
_LL2FB;}_LL31A: {struct Cyc_Absyn_TryCatch_s_struct*_tmp605=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp5D4;if(_tmp605->tag != 15)goto _LL31C;else{_tmp606=_tmp605->f1;_tmp607=_tmp605->f2;}}
_LL31B: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp606);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp607);{union Cyc_CfFlowInfo_FlowInfo _tmp648=scs_outflow;int
_tmp649;_LL327: if((_tmp648.BottomFL).tag != 1)goto _LL329;_tmp649=(int)(_tmp648.BottomFL).val;
_LL328: goto _LL326;_LL329:;_LL32A:({void*_tmp64A=0;Cyc_Tcutil_terr(s->loc,({const
char*_tmp64B="last catch clause may implicitly fallthru";_tag_dyneither(_tmp64B,
sizeof(char),42);}),_tag_dyneither(_tmp64A,sizeof(void*),0));});_LL326:;}outflow=
s1_outflow;goto _LL2FB;}}}_LL31C: {struct Cyc_Absyn_Decl_s_struct*_tmp608=(struct
Cyc_Absyn_Decl_s_struct*)_tmp5D4;if(_tmp608->tag != 12)goto _LL31E;else{_tmp609=
_tmp608->f1;_tmp60A=*_tmp609;_tmp60B=_tmp60A.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp60C=(struct Cyc_Absyn_Alias_d_struct*)_tmp60B;if(_tmp60C->tag != 5)goto _LL31E;
else{_tmp60D=_tmp60C->f1;_tmp60E=_tmp60C->f2;_tmp60F=_tmp60C->f3;}}_tmp610=
_tmp608->f2;}}_LL31D: {union Cyc_CfFlowInfo_FlowInfo _tmp64D;void*_tmp64E;struct
_tuple13 _tmp64C=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp60D);_tmp64D=_tmp64C.f1;
_tmp64E=_tmp64C.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp650;struct Cyc_List_List*
_tmp651;struct _tuple14 _tmp64F=Cyc_CfFlowInfo_save_consume_info(_tmp5D3,_tmp64D,0);
_tmp650=_tmp64F.f1;_tmp651=_tmp650.may_consume;_tmp64D=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp5D3,_tmp60D->loc,_tmp64D);_tmp64D=Cyc_NewControlFlow_use_Rval(env,_tmp60D->loc,
_tmp64D,_tmp64E);{struct Cyc_List_List _tmp652=({struct Cyc_List_List _tmp65A;
_tmp65A.hd=_tmp60F;_tmp65A.tl=0;_tmp65A;});_tmp64D=Cyc_NewControlFlow_add_vars(
_tmp5D3,_tmp64D,(struct Cyc_List_List*)& _tmp652,_tmp5D3->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp64D,_tmp610);{union Cyc_CfFlowInfo_FlowInfo
_tmp653=outflow;int _tmp654;struct _tuple12 _tmp655;struct Cyc_Dict_Dict _tmp656;
struct Cyc_List_List*_tmp657;struct Cyc_CfFlowInfo_ConsumeInfo _tmp658;_LL32C: if((
_tmp653.BottomFL).tag != 1)goto _LL32E;_tmp654=(int)(_tmp653.BottomFL).val;_LL32D:
goto _LL32B;_LL32E: if((_tmp653.ReachableFL).tag != 2)goto _LL32B;_tmp655=(struct
_tuple12)(_tmp653.ReachableFL).val;_tmp656=_tmp655.f1;_tmp657=_tmp655.f2;_tmp658=
_tmp655.f3;_LL32F: while(_tmp651 != 0){struct Cyc_Dict_Dict _tmp659=_tmp658.consumed;
_tmp658.consumed=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,
struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)(_tmp5D3->r,_tmp658.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp651->hd);if((_tmp658.consumed).t != _tmp659.t);
_tmp651=_tmp651->tl;}outflow=Cyc_CfFlowInfo_ReachableFL(_tmp656,_tmp657,_tmp658);
goto _LL32B;_LL32B:;}goto _LL2FB;}}}_LL31E: {struct Cyc_Absyn_Decl_s_struct*_tmp611=(
struct Cyc_Absyn_Decl_s_struct*)_tmp5D4;if(_tmp611->tag != 12)goto _LL320;else{
_tmp612=_tmp611->f1;_tmp613=_tmp611->f2;}}_LL31F: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp612),_tmp613);goto _LL2FB;_LL320: {
struct Cyc_Absyn_Label_s_struct*_tmp614=(struct Cyc_Absyn_Label_s_struct*)_tmp5D4;
if(_tmp614->tag != 13)goto _LL322;else{_tmp615=_tmp614->f2;}}_LL321: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp615);goto _LL2FB;_LL322: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp616=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp5D4;if(_tmp616->tag != 16)goto
_LL324;else{_tmp617=_tmp616->f1;}}_LL323: {union Cyc_CfFlowInfo_FlowInfo _tmp65C;
void*_tmp65D;struct _tuple13 _tmp65B=Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp617);_tmp65C=_tmp65B.f1;_tmp65D=_tmp65B.f2;_tmp65C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp617->loc,_tmp65C);{union Cyc_CfFlowInfo_FlowInfo _tmp65E=Cyc_NewControlFlow_use_Rval(
env,_tmp617->loc,_tmp65C,_tmp65D);{void*_tmp65F=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp617->topt))->v);void*_tmp661;_LL331: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp660=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp65F;if(_tmp660->tag != 16)goto _LL333;else{_tmp661=(void*)_tmp660->f1;}}_LL332:
outflow=Cyc_CfFlowInfo_kill_flow_region(_tmp5D3,_tmp65C,_tmp661);goto _LL330;
_LL333:;_LL334:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp662=
_cycalloc(sizeof(*_tmp662));_tmp662[0]=({struct Cyc_Core_Impossible_struct _tmp663;
_tmp663.tag=Cyc_Core_Impossible;_tmp663.f1=({const char*_tmp664="anal_stmt -- reset_region";
_tag_dyneither(_tmp664,sizeof(char),26);});_tmp663;});_tmp662;}));_LL330:;}goto
_LL2FB;}}_LL324:;_LL325:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665[0]=({struct Cyc_Core_Impossible_struct
_tmp666;_tmp666.tag=Cyc_Core_Impossible;_tmp666.f1=({const char*_tmp667="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dyneither(_tmp667,sizeof(char),56);});_tmp666;});_tmp665;}));_LL2FB:;}
outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp668=outflow;struct _tuple12 _tmp669;struct Cyc_CfFlowInfo_ConsumeInfo _tmp66A;
_LL336: if((_tmp668.ReachableFL).tag != 2)goto _LL338;_tmp669=(struct _tuple12)(
_tmp668.ReachableFL).val;_tmp66A=_tmp669.f3;_LL337: goto _LL335;_LL338:;_LL339:
goto _LL335;_LL335:;}return outflow;}}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*
_tmp66B=env->fenv;void*_tmp66C=decl->r;struct Cyc_Absyn_Vardecl*_tmp66E;struct Cyc_Core_Opt*
_tmp670;struct Cyc_Core_Opt _tmp671;struct Cyc_List_List*_tmp672;struct Cyc_Absyn_Exp*
_tmp673;struct Cyc_List_List*_tmp675;struct Cyc_Absyn_Fndecl*_tmp677;struct Cyc_Absyn_Tvar*
_tmp679;struct Cyc_Absyn_Vardecl*_tmp67A;int _tmp67B;struct Cyc_Absyn_Exp*_tmp67C;
_LL33B: {struct Cyc_Absyn_Var_d_struct*_tmp66D=(struct Cyc_Absyn_Var_d_struct*)
_tmp66C;if(_tmp66D->tag != 0)goto _LL33D;else{_tmp66E=_tmp66D->f1;}}_LL33C: {
struct Cyc_List_List _tmp67D=({struct Cyc_List_List _tmp68C;_tmp68C.hd=_tmp66E;
_tmp68C.tl=0;_tmp68C;});inflow=Cyc_NewControlFlow_add_vars(_tmp66B,inflow,(
struct Cyc_List_List*)& _tmp67D,_tmp66B->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp67E=_tmp66E->initializer;if(_tmp67E == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp680;void*_tmp681;struct _tuple13 _tmp67F=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp67E);_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;
_tmp680=Cyc_CfFlowInfo_consume_unique_rvals(_tmp66B,_tmp67E->loc,_tmp680);{union
Cyc_CfFlowInfo_FlowInfo _tmp682=_tmp680;int _tmp683;struct _tuple12 _tmp684;struct
Cyc_Dict_Dict _tmp685;struct Cyc_List_List*_tmp686;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp687;_LL348: if((_tmp682.BottomFL).tag != 1)goto _LL34A;_tmp683=(int)(_tmp682.BottomFL).val;
_LL349: return Cyc_CfFlowInfo_BottomFL();_LL34A: if((_tmp682.ReachableFL).tag != 2)
goto _LL347;_tmp684=(struct _tuple12)(_tmp682.ReachableFL).val;_tmp685=_tmp684.f1;
_tmp686=_tmp684.f2;_tmp687=_tmp684.f3;_LL34B: _tmp685=Cyc_CfFlowInfo_assign_place(
_tmp66B,decl->loc,_tmp685,env->all_changed,({struct Cyc_CfFlowInfo_Place*_tmp688=
_region_malloc(env->r,sizeof(*_tmp688));_tmp688->root=(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp689=_region_malloc(env->r,sizeof(*_tmp689));_tmp689[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp68A;_tmp68A.tag=0;_tmp68A.f1=_tmp66E;_tmp68A;});_tmp689;});_tmp688->fields=0;
_tmp688;}),_tmp681);_tmp686=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp686,
_tmp66E,(struct Cyc_Absyn_Exp*)_check_null(_tmp66E->initializer));{union Cyc_CfFlowInfo_FlowInfo
_tmp68B=Cyc_CfFlowInfo_ReachableFL(_tmp685,_tmp686,_tmp687);Cyc_NewControlFlow_update_tryflow(
env,_tmp68B);return _tmp68B;}_LL347:;}}}}_LL33D: {struct Cyc_Absyn_Let_d_struct*
_tmp66F=(struct Cyc_Absyn_Let_d_struct*)_tmp66C;if(_tmp66F->tag != 2)goto _LL33F;
else{_tmp670=_tmp66F->f2;if(_tmp670 == 0)goto _LL33F;_tmp671=*_tmp670;_tmp672=(
struct Cyc_List_List*)_tmp671.v;_tmp673=_tmp66F->f3;}}_LL33E: {union Cyc_CfFlowInfo_FlowInfo
_tmp68E;void*_tmp68F;struct _tuple13 _tmp68D=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp673);_tmp68E=_tmp68D.f1;_tmp68F=_tmp68D.f2;_tmp68E=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp66B,_tmp673->loc,_tmp68E);_tmp68E=Cyc_NewControlFlow_use_Rval(env,_tmp673->loc,
_tmp68E,_tmp68F);return Cyc_NewControlFlow_add_vars(_tmp66B,_tmp68E,_tmp672,
_tmp66B->unknown_all,decl->loc);}_LL33F: {struct Cyc_Absyn_Letv_d_struct*_tmp674=(
struct Cyc_Absyn_Letv_d_struct*)_tmp66C;if(_tmp674->tag != 3)goto _LL341;else{
_tmp675=_tmp674->f1;}}_LL340: return Cyc_NewControlFlow_add_vars(_tmp66B,inflow,
_tmp675,_tmp66B->unknown_none,decl->loc);_LL341: {struct Cyc_Absyn_Fn_d_struct*
_tmp676=(struct Cyc_Absyn_Fn_d_struct*)_tmp66C;if(_tmp676->tag != 1)goto _LL343;
else{_tmp677=_tmp676->f1;}}_LL342: Cyc_NewControlFlow_check_nested_fun(_tmp66B,
inflow,_tmp677);{void*_tmp690=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp677->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp691=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp677->fn_vardecl);
return Cyc_NewControlFlow_add_vars(_tmp66B,inflow,({struct Cyc_List_List*_tmp692=
_region_malloc(env->r,sizeof(*_tmp692));_tmp692->hd=_tmp691;_tmp692->tl=0;
_tmp692;}),_tmp66B->unknown_all,decl->loc);}_LL343: {struct Cyc_Absyn_Region_d_struct*
_tmp678=(struct Cyc_Absyn_Region_d_struct*)_tmp66C;if(_tmp678->tag != 4)goto _LL345;
else{_tmp679=_tmp678->f1;_tmp67A=_tmp678->f2;_tmp67B=_tmp678->f3;_tmp67C=_tmp678->f4;}}
_LL344: if(_tmp67C != 0){struct Cyc_Absyn_Exp*_tmp693=(struct Cyc_Absyn_Exp*)_tmp67C;
union Cyc_CfFlowInfo_FlowInfo _tmp695;void*_tmp696;struct _tuple13 _tmp694=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp693);_tmp695=_tmp694.f1;_tmp696=_tmp694.f2;_tmp695=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp66B,_tmp693->loc,_tmp695);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp693->loc,
_tmp695,_tmp696);}{struct Cyc_List_List _tmp697=({struct Cyc_List_List _tmp698;
_tmp698.hd=_tmp67A;_tmp698.tl=0;_tmp698;});return Cyc_NewControlFlow_add_vars(
_tmp66B,inflow,(struct Cyc_List_List*)& _tmp697,_tmp66B->unknown_all,decl->loc);}
_LL345:;_LL346:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp699=
_cycalloc(sizeof(*_tmp699));_tmp699[0]=({struct Cyc_Core_Impossible_struct _tmp69A;
_tmp69A.tag=Cyc_Core_Impossible;_tmp69A.f1=({const char*_tmp69B="anal_decl: unexpected decl variant";
_tag_dyneither(_tmp69B,sizeof(char),35);});_tmp69A;});_tmp699;}));_LL33A:;}
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle _tmp69C=_new_region("frgn");struct _RegionHandle*frgn=& _tmp69C;
_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,
0,({struct Cyc_CfFlowInfo_ConsumeInfo _tmp69D;_tmp69D.consumed=fenv->mt_place_set;
_tmp69D.may_consume=0;_tmp69D;})),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp69E=fenv->r;struct Cyc_Position_Segment*_tmp69F=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp69F);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp6A0=inflow;int _tmp6A1;struct
_tuple12 _tmp6A2;struct Cyc_Dict_Dict _tmp6A3;struct Cyc_List_List*_tmp6A4;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp6A5;_LL34D: if((_tmp6A0.BottomFL).tag != 1)goto _LL34F;_tmp6A1=(int)(_tmp6A0.BottomFL).val;
_LL34E:({void*_tmp6A6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp6A7="check_fun";_tag_dyneither(_tmp6A7,sizeof(
char),10);}),_tag_dyneither(_tmp6A6,sizeof(void*),0));});_LL34F: if((_tmp6A0.ReachableFL).tag
!= 2)goto _LL34C;_tmp6A2=(struct _tuple12)(_tmp6A0.ReachableFL).val;_tmp6A3=
_tmp6A2.f1;_tmp6A4=_tmp6A2.f2;_tmp6A5=_tmp6A2.f3;_LL350: {struct Cyc_List_List*
atts;{void*_tmp6A8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp6AA;struct Cyc_List_List*_tmp6AB;
_LL352: {struct Cyc_Absyn_FnType_struct*_tmp6A9=(struct Cyc_Absyn_FnType_struct*)
_tmp6A8;if(_tmp6A9->tag != 10)goto _LL354;else{_tmp6AA=_tmp6A9->f1;_tmp6AB=_tmp6AA.attributes;}}
_LL353: atts=_tmp6AB;goto _LL351;_LL354:;_LL355:({void*_tmp6AC=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp6AD="check_fun: non-function type cached with fndecl_t";
_tag_dyneither(_tmp6AD,sizeof(char),50);}),_tag_dyneither(_tmp6AC,sizeof(void*),
0));});_LL351:;}for(0;atts != 0;atts=atts->tl){void*_tmp6AE=(void*)atts->hd;int
_tmp6B0;_LL357: {struct Cyc_Absyn_Initializes_att_struct*_tmp6AF=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp6AE;if(_tmp6AF->tag != 20)goto _LL359;else{_tmp6B0=_tmp6AF->f1;}}_LL358: {
unsigned int j=(unsigned int)_tmp6B0;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp6B1=0;Cyc_Tcutil_terr(_tmp69F,({const char*_tmp6B2="initializes attribute exceeds number of parameters";
_tag_dyneither(_tmp6B2,sizeof(char),51);}),_tag_dyneither(_tmp6B1,sizeof(void*),
0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp6B3=((struct Cyc_Absyn_Vardecl*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp6B4=Cyc_Tcutil_compress(
_tmp6B3->type);struct Cyc_Absyn_PtrInfo _tmp6B6;void*_tmp6B7;struct Cyc_Absyn_PtrAtts
_tmp6B8;union Cyc_Absyn_Constraint*_tmp6B9;union Cyc_Absyn_Constraint*_tmp6BA;
union Cyc_Absyn_Constraint*_tmp6BB;_LL35C: {struct Cyc_Absyn_PointerType_struct*
_tmp6B5=(struct Cyc_Absyn_PointerType_struct*)_tmp6B4;if(_tmp6B5->tag != 5)goto
_LL35E;else{_tmp6B6=_tmp6B5->f1;_tmp6B7=_tmp6B6.elt_typ;_tmp6B8=_tmp6B6.ptr_atts;
_tmp6B9=_tmp6B8.nullable;_tmp6BA=_tmp6B8.bounds;_tmp6BB=_tmp6B8.zero_term;}}
_LL35D: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6B9))({
void*_tmp6BC=0;Cyc_Tcutil_terr(_tmp69F,({const char*_tmp6BD="initializes attribute not allowed on nullable pointers";
_tag_dyneither(_tmp6BD,sizeof(char),55);}),_tag_dyneither(_tmp6BC,sizeof(void*),
0));});if(!Cyc_Tcutil_is_bound_one(_tmp6BA))({void*_tmp6BE=0;Cyc_Tcutil_terr(
_tmp69F,({const char*_tmp6BF="initializes attribute allowed only on pointers of size 1";
_tag_dyneither(_tmp6BF,sizeof(char),57);}),_tag_dyneither(_tmp6BE,sizeof(void*),
0));});if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6BB))({void*_tmp6C0=0;Cyc_Tcutil_terr(_tmp69F,({const char*_tmp6C1="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dyneither(_tmp6C1,sizeof(char),77);}),_tag_dyneither(_tmp6C0,sizeof(void*),
0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp6C2=({struct Cyc_CfFlowInfo_InitParam_struct*
_tmp6CA=_region_malloc(_tmp69E,sizeof(*_tmp6CA));_tmp6CA[0]=({struct Cyc_CfFlowInfo_InitParam_struct
_tmp6CB;_tmp6CB.tag=2;_tmp6CB.f1=(int)j;_tmp6CB.f2=(void*)_tmp6B7;_tmp6CB;});
_tmp6CA;});struct Cyc_CfFlowInfo_Place*_tmp6C3=({struct Cyc_CfFlowInfo_Place*
_tmp6C9=_region_malloc(_tmp69E,sizeof(*_tmp6C9));_tmp6C9->root=(void*)_tmp6C2;
_tmp6C9->fields=0;_tmp6C9;});_tmp6A3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,void*v))Cyc_Dict_insert)(_tmp6A3,(void*)_tmp6C2,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp6B7,fenv->esc_none));_tmp6A3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,void*v))Cyc_Dict_insert)(_tmp6A3,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp6C4=_region_malloc(_tmp69E,sizeof(*_tmp6C4));_tmp6C4[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp6C5;_tmp6C5.tag=0;_tmp6C5.f1=_tmp6B3;_tmp6C5;});_tmp6C4;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp6C6=_region_malloc(_tmp69E,sizeof(*_tmp6C6));
_tmp6C6[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp6C7;_tmp6C7.tag=5;_tmp6C7.f1=
_tmp6C3;_tmp6C7;});_tmp6C6;}));param_roots=({struct Cyc_List_List*_tmp6C8=
_region_malloc(_tmp69E,sizeof(*_tmp6C8));_tmp6C8->hd=_tmp6C3;_tmp6C8->tl=
param_roots;_tmp6C8;});goto _LL35B;}_LL35E:;_LL35F:({void*_tmp6CC=0;Cyc_Tcutil_terr(
_tmp69F,({const char*_tmp6CD="initializes attribute on non-pointer";
_tag_dyneither(_tmp6CD,sizeof(char),37);}),_tag_dyneither(_tmp6CC,sizeof(void*),
0));});_LL35B:;}goto _LL356;}}_LL359:;_LL35A: goto _LL356;_LL356:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6A3,_tmp6A4,_tmp6A5);}_LL34C:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp69E,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp6DB=_region_malloc(_tmp69E,sizeof(*_tmp6DB));_tmp6DB->r=_tmp69E;_tmp6DB->fenv=
fenv;_tmp6DB->iterate_again=1;_tmp6DB->iteration_num=0;_tmp6DB->in_try=0;_tmp6DB->tryflow=
inflow;_tmp6DB->all_changed=0;_tmp6DB->noreturn=noreturn;_tmp6DB->param_roots=
param_roots;_tmp6DB->flow_table=flow_table;_tmp6DB;});union Cyc_CfFlowInfo_FlowInfo
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo
_tmp6CE=outflow;int _tmp6CF;_LL361: if((_tmp6CE.BottomFL).tag != 1)goto _LL363;
_tmp6CF=(int)(_tmp6CE.BottomFL).val;_LL362: goto _LL360;_LL363:;_LL364: Cyc_NewControlFlow_check_init_params(
_tmp69F,env,outflow);if(noreturn)({void*_tmp6D0=0;Cyc_Tcutil_terr(_tmp69F,({
const char*_tmp6D1="`noreturn' function might (implicitly) return";_tag_dyneither(
_tmp6D1,sizeof(char),46);}),_tag_dyneither(_tmp6D0,sizeof(void*),0));});else{
void*_tmp6D2=Cyc_Tcutil_compress(fd->ret_type);_LL366: {struct Cyc_Absyn_VoidType_struct*
_tmp6D3=(struct Cyc_Absyn_VoidType_struct*)_tmp6D2;if(_tmp6D3->tag != 0)goto _LL368;}
_LL367: goto _LL365;_LL368: {struct Cyc_Absyn_DoubleType_struct*_tmp6D4=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6D2;if(_tmp6D4->tag != 8)goto _LL36A;}_LL369: goto _LL36B;_LL36A: {struct Cyc_Absyn_FloatType_struct*
_tmp6D5=(struct Cyc_Absyn_FloatType_struct*)_tmp6D2;if(_tmp6D5->tag != 7)goto
_LL36C;}_LL36B: goto _LL36D;_LL36C: {struct Cyc_Absyn_IntType_struct*_tmp6D6=(
struct Cyc_Absyn_IntType_struct*)_tmp6D2;if(_tmp6D6->tag != 6)goto _LL36E;}_LL36D:({
void*_tmp6D7=0;Cyc_Tcutil_warn(_tmp69F,({const char*_tmp6D8="function may not return a value";
_tag_dyneither(_tmp6D8,sizeof(char),32);}),_tag_dyneither(_tmp6D7,sizeof(void*),
0));});goto _LL365;_LL36E:;_LL36F:({void*_tmp6D9=0;Cyc_Tcutil_terr(_tmp69F,({
const char*_tmp6DA="function may not return a value";_tag_dyneither(_tmp6DA,
sizeof(char),32);}),_tag_dyneither(_tmp6D9,sizeof(void*),0));});goto _LL365;
_LL365:;}goto _LL360;_LL360:;}}}}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*
ds){for(0;ds != 0;ds=ds->tl){void*_tmp6DC=((struct Cyc_Absyn_Decl*)ds->hd)->r;
struct Cyc_Absyn_Fndecl*_tmp6DE;struct Cyc_List_List*_tmp6E0;struct Cyc_List_List*
_tmp6E2;struct Cyc_List_List*_tmp6E4;_LL371: {struct Cyc_Absyn_Fn_d_struct*_tmp6DD=(
struct Cyc_Absyn_Fn_d_struct*)_tmp6DC;if(_tmp6DD->tag != 1)goto _LL373;else{_tmp6DE=
_tmp6DD->f1;}}_LL372: Cyc_NewControlFlow_check_fun(_tmp6DE);goto _LL370;_LL373: {
struct Cyc_Absyn_ExternC_d_struct*_tmp6DF=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp6DC;if(_tmp6DF->tag != 12)goto _LL375;else{_tmp6E0=_tmp6DF->f1;}}_LL374:
_tmp6E2=_tmp6E0;goto _LL376;_LL375: {struct Cyc_Absyn_Using_d_struct*_tmp6E1=(
struct Cyc_Absyn_Using_d_struct*)_tmp6DC;if(_tmp6E1->tag != 11)goto _LL377;else{
_tmp6E2=_tmp6E1->f2;}}_LL376: _tmp6E4=_tmp6E2;goto _LL378;_LL377: {struct Cyc_Absyn_Namespace_d_struct*
_tmp6E3=(struct Cyc_Absyn_Namespace_d_struct*)_tmp6DC;if(_tmp6E3->tag != 10)goto
_LL379;else{_tmp6E4=_tmp6E3->f2;}}_LL378: Cyc_NewControlFlow_cf_check(_tmp6E4);
goto _LL370;_LL379: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp6E5=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp6DC;if(_tmp6E5->tag != 13)goto _LL37B;}_LL37A: goto _LL370;_LL37B:;_LL37C: goto
_LL370;_LL370:;}}
